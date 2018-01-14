#include "eeyore.h"
#include "symbol_table.h"

typedef struct listnode_ * listnode;
typedef struct list_ * list;

static int regs[REGNUM];
stable func2reg;
binder built_in_bin;
int built_in_reg[REGNUM];

struct listnode_{
	int num;
	listnode next;
};

struct list_{
	int sum;
	listnode begin;
};

listnode listnode_create(int num){
	listnode tar=(listnode)checked_malloc(sizeof(*tar));
	tar->num=num;
	tar->next=NULL;
	return tar;
}

list list_create(){
	list tar=(list)checked_malloc(sizeof(*tar));
	tar->sum=0;
	tar->begin=NULL;
	return tar;
}

void reg_release(funcinfo info,int tar){
	regs[info->regs[info->zones[tar]->number]]=0;
	return;
}

void reg_alloc(funcinfo info,int tar,int fix){
	if(fix>=0){
		if(regs[fix]==0){
			regs[fix]=1;
			info->regs[info->zones[tar]->number]=fix;
		}
		else{
			fprintf(stderr,"reg_alloc fix failed\n");
		}
		return;
	}
	for(int i=0;i<REGNUM;i++){
		if(regs[i]==0){
			regs[i]=1;
			info->regs[info->zones[tar]->number]=i;
			break;
		}
	}
	return;
}

void list_insert(funcinfo info,list active,int tar){
	listnode new_node=(listnode)checked_malloc(sizeof(*new_node));
	listnode cur,last=NULL;
	new_node->num=tar;
	active->sum++;
	for(cur=active->begin;cur!=NULL;cur=cur->next){
		if(info->zones[cur->num]->end<info->zones[tar]->end){
			last=cur;
		}
		else{
			if(last==NULL){
				new_node->next=active->begin;
				active->begin=new_node;
			}
			else{
				last->next=new_node;
				new_node->next=cur;
			}
			break;
		}
	}
	if(cur==NULL){
		if(last!=NULL){
			last->next=new_node;
			new_node->next=NULL;
		}
		else{
			active->begin=new_node;
			new_node->next=NULL;
		}
	}
	fprintf(stderr,"insert one %d\n",info->zones[tar]->number);
	return;
}

void expire_old(funcinfo info,list active,int beginpoint){
	listnode cur;
	for(cur=active->begin;cur!=NULL;cur=cur->next){
		if(info->zones[cur->num]->end>=beginpoint)
			break;
		reg_release(info,cur->num);
		active->sum--;
		fprintf(stderr,"expire one %d\n",info->zones[cur->num]->number);
	}
	active->begin=cur;
}

void spill_one(list active,funcinfo info,int tar){
	listnode prev,spill;
	for(spill=active->begin;spill->next!=NULL;spill=spill->next){
		prev=spill;
	}
	if(info->zones[spill->num]->end>info->zones[tar]->end){
		info->regs[info->zones[tar]->number]=info->regs[info->zones[spill->num]->number];
		info->regs[info->zones[spill->num]->number]=-1;
		prev->next=NULL;
		active->sum--;
		fprintf(stderr,"spill last %d\n",info->zones[spill->num]->number);
		list_insert(info,active,tar);
	}
	else{
		info->regs[info->zones[tar]->number]=-1;
		fprintf(stderr,"spill itself %d\n",info->zones[tar]->number);
	}
	return;
}

void linear_scan(funcinfo info,int argc){
	list active=list_create();
	int cur;
	memset(regs,0,sizeof(regs));
	for(cur=0;cur<info->varnum;cur++){
		if(info->zones[cur]->begin<0)
			info->regs[info->zones[cur]->number]=-2;
		else break;
	}
	for(;cur<info->varnum;cur++){
		expire_old(info,active,info->zones[cur]->begin);
		if(active->sum==REGNUM){
			spill_one(active,info,cur);
		}
		else{
			if(info->zones[cur]->number<argc)
				reg_alloc(info,cur,info->zones[cur]->number);
			else reg_alloc(info,cur,-1);
			list_insert(info,active,cur);
		}
	}
	info->regnum=0;
	for(int i=0;i<info->varnum;i++){
		if(info->regnum<info->regs[i])
			info->regnum=info->regs[i];
	}
	info->regnum++;
	//printf("regsum :%d\n",info->regnum);
	return;
}

void reg_realloc(int *reg,int num,int callnum){
	if(callnum==0) return;
	for(int i=0;i<num;i++){
		if(reg[i]>=0){
			reg[i]=(reg[i]+CALLERSAVE)%REGNUM;
			/*if(reg[i]<CALLERSAVE){
				reg[i]=CALLERSAVE-1-reg[i];
			}*/
		}
	}
	return;
}

void stack_alloc(funcinfo info){
	int stk_loc=0;
	memset(info->reg_used,0,sizeof(info->reg_used));
	for(int i=0;i<info->varnum;i++){
		if(info->regs[i]==-2) info->stack[i]=-1;
		else if(info->regs[i]==-1){
			if(info->spec[i]<=0)
				info->stack[i]=stk_loc++;
			else info->stack[i]=-1;
		}
		else{
			info->reg_used[info->regs[i]]=1;
			info->stack[i]=-1;
		}
	}
	info->stacknum=stk_loc;
	return;
}

void reg_stk_print(int *reg,int *stack,int reg_used[],int num){
	printf("\n");
	for(int i=0;i<num;i++){
		fprintf(stderr,"var %d : reg %d stk %d\n",i,reg[i],stack[i]);
	}
	for(int i=0;i<REGNUM;i++)
		if(reg_used[i])
			fprintf(stderr,"reg %d : used\n",i);
	return;
}

void bind_funcname_regs(funcinfo func){
	binder bin=(binder)checked_malloc(sizeof(*bin));
	if(func->callnum==func->selfnum) bin->defin=func->reg_used;
	else bin->defin=NULL;
	bin->flag=func->args;
	table_insert(func2reg,func->func,bin);
}

void register_alloc(interstat eeyore){
	func2reg=table_new(NULL);
	built_in_bin=(binder)checked_malloc(sizeof(*built_in_bin));
	built_in_bin->defin=built_in_reg;
	built_in_bin->flag=1;
	//table_insert(func2reg,"getint",built_in_bin);
	//table_insert(func2reg,"putint",built_in_bin);
	//table_insert(func2reg,"getchar",built_in_bin);
	//table_insert(func2reg,"putchar",built_in_bin);
	for(interstat cur=eeyore;cur!=NULL;cur=cur->nextblk){
		if(cur->kind!=begin_inter)
			continue;
		funcinfo info=cur->u.begin;
		linear_scan(info,info->args);
		reg_realloc(info->regs,info->varnum,info->callnum);
		stack_alloc(info);
		reg_stk_print(info->regs,info->stack,info->reg_used,info->varnum);
		bind_funcname_regs(cur->u.begin);
	}
	return;
}
