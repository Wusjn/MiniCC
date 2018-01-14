#include "eeyore.h"

bitmap bitmap_create(int size){
	bitmap tar=(bitmap)checked_malloc(sizeof(*tar));
	int num=(size+7)/8;
	tar->map=(char *)checked_malloc(sizeof(char)*num);
	tar->bitsize=size;
	tar->size=num;
	memset(tar->map,0,sizeof(char)*num);
	return tar;
}

int bitmap_or(bitmap a,bitmap b){
	if(a->bitsize!=b->bitsize)
		return -1;
	for(int i=0;i<a->size;i++){
		a->map[i]|=b->map[i];
	}
	return 0;
}

int bitmap_minus(bitmap a,bitmap b){
	if(a->bitsize!=b->bitsize)
		return -1;
	for(int i=0;i<a->size;i++){
		a->map[i]&=(~(b->map[i]));
	}
	return 0;
}

int bitmap_equal(bitmap a,bitmap b){
	if(a->bitsize!=b->bitsize)
		return 0;
	for(int i=0;i<a->size;i++){
		if(a->map[i]!=b->map[i])
			return 0;
	}
	return 1;
}

int bitmap_set(bitmap tar,int pos){
	if(pos<0||tar->bitsize<=pos)
		return -1;
	int charpos=pos/8;
	int bitpos=pos%8;
	tar->map[charpos]|=(1<<bitpos);
	return 0;
}

int bitmap_reset(bitmap tar,int pos){
	if(pos<0||tar->bitsize<=pos)
		return -1;
	int charpos=pos/8;
	int bitpos=pos%8;
	tar->map[charpos]&=~(1<<bitpos);
	return 0;
}

int bitmap_get(bitmap tar,int pos){
	if(pos<0||tar->bitsize<=pos)
		return -1;
	int charpos=pos/8;
	int bitpos=pos%8;
	return tar->map[charpos]&(1<<bitpos);
}

void bitmap_clear(bitmap tar){
	memset(tar->map,0,sizeof(char)*(tar->size));
}

int bitmap_copy(bitmap tar,bitmap sour){
	if(tar->bitsize!=sour->bitsize)
		return -1;
	for(int i=0;i<tar->size;i++){
		tar->map[i]=sour->map[i];
	}
	return 0;
}

void bitmap_print(bitmap tar){
	printf("map : ");
	for(int i=0;i<tar->bitsize;i++){
		if(tar->map[i/8]&(1<<(i%8)))
			printf("%d ",i);
	}
	printf("\n");
}

int flownode_aug(flownode tar,interstat end,int first,int varnum){
	if(!first&&end->kind==label_inter) return -1;
	tar->end=end;
	end->basicblk=tar;
	if(!bitmap_get(tar->use,end->def)) bitmap_set(tar->def,end->def);
	if(!bitmap_get(tar->def,end->use[0])) bitmap_set(tar->use,end->use[0]);
	if(!bitmap_get(tar->def,end->use[1])) bitmap_set(tar->use,end->use[1]);
	if(!bitmap_get(tar->def,end->use[2])) bitmap_set(tar->use,end->use[2]);
	end->out=bitmap_create(varnum);
	if(end->kind==if_inter||end->kind==goto_inter||
		end->kind==return_inter||end->kind==end_inter)
		return -1;
	return 0;
}

flownode flownode_scan(interstat begin,int varnum){
	flownode tar=(flownode)checked_malloc(sizeof(*tar));
	tar->kind=inter_flow;
	tar->begin=begin;
	tar->def=bitmap_create(varnum);
	tar->use=bitmap_create(varnum);
	tar->in=bitmap_create(varnum);
	tar->out=bitmap_create(varnum);
	tar->new_out=bitmap_create(varnum);
	tar->succ[0]=NULL;
	tar->succ[1]=NULL;
	interstat cur=begin;
	if(cur->kind==label_inter){
		flownode_aug(tar,cur,1,varnum);
		cur->out=bitmap_create(varnum);
		cur=cur->next;
	}
	while(1){
		if(flownode_aug(tar,cur,0,varnum))
			break;
		cur->out=bitmap_create(varnum);
		cur=cur->next;
	}
	tar->visited=0;
	return tar;
}

flownode beginnode_create(funcinfo info){
	flownode begin=(flownode)checked_malloc(sizeof(*begin));
	begin->kind=begin_flow;
	begin->out=bitmap_create(info->varnum);
	for(int i=0;i<info->varnum;i++){
		if(info->spec[i]!=0)
			bitmap_set(begin->out,i);
	}
	begin->succ[0]=NULL;
	begin->succ[1]=NULL;
	begin->visited=0;
	return begin;
}

flownode endnode_create(funcinfo info){
	flownode end=(flownode)checked_malloc(sizeof(*end));
	end->kind=end_flow;
	end->in=bitmap_create(info->varnum);
	for(int i=0;i<info->varnum;i++){
		if(info->spec[i]>0)
			bitmap_set(end->in,i);
	}
	end->succ[0]=NULL;
	end->succ[1]=NULL;
	end->visited=0;
	return end;
}

flownode flow_scan(vartable labeltab,interstat func){
	if(func->kind!=begin_inter)
		return NULL;
	funcinfo info=func->u.begin;
	flownode begin=beginnode_create(info);
	flownode end=endnode_create(info);
	interstat curstat=func->next;
	flownode lastnode=begin,curnode;
	while(curstat!=NULL){
		curnode=flownode_scan(curstat,info->varnum);
		if(curnode->end->kind==return_inter)
			curnode->succ[0]=end;
		if(lastnode->kind==begin_flow||
			(lastnode->end->kind!=return_inter&&
				lastnode->end->kind!=goto_inter))
			lastnode->succ[0]=curnode;
		curstat=curnode->end->next;
		lastnode=curnode;
	}
	for(curstat=func->next;curstat;curstat=curstat->next){
		if(curstat->kind==if_inter){
			interstat labelstat=
				(interstat)vartab_find(labeltab,'l',curstat->u.iff.label);
			if((int)labelstat==-1) printf("wtf\n");
			curstat->basicblk->succ[1]=labelstat->basicblk;
		}
		else if(curstat->kind==goto_inter){
			interstat labelstat=
				(interstat)vartab_find(labeltab,'l',curstat->u.gotoo);
			if((int)labelstat==-1) printf("wtf\n");
			curstat->basicblk->succ[1]=labelstat->basicblk;
		}
	}
	return begin;
}

void flow_clear_history(flownode begin){
	if(begin==NULL||begin->visited==0) return;
	begin->visited=0;
	flow_clear_history(begin->succ[0]);
	flow_clear_history(begin->succ[1]);
}

int flow_check(flownode begin){
	if(begin==NULL||begin->visited==1) return 0;
	begin->visited=1;
	if(begin->kind==inter_flow&&begin->end->kind==end_inter)
		return -1;
	if(begin->kind==inter_flow){
		for(interstat cur=begin->begin;cur!=begin->end;cur=cur->next){
			cur->waste=0;
		}
		begin->end->waste=0;
	}
	return flow_check(begin->succ[0])||flow_check(begin->succ[1]);
}

void flow_travel(flownode begin){
	if(begin==NULL||begin->visited==1) return;
	begin->visited=1;
	if(begin->kind==inter_flow){
		printf("beginnum: %d end : %d\n",begin->begin->statnum,begin->end->statnum);
		printf("def ");
		bitmap_print(begin->def);
		printf("use ");
		bitmap_print(begin->use);
	}
	else if(begin->kind==begin_flow){
		printf("beginnode\nout ");
		bitmap_print(begin->out);
	}
	else {
		printf("endnode\nin ");
		bitmap_print(begin->in);
	}
	printf("succ 0: %d succ 1: %d\n",(int)(begin->succ[0]),(int)(begin->succ[1]));
	for(int i=0;i<2;i++){
		flow_travel(begin->succ[i]);
	}
}

int flowmap_create(interstat eeyore,vartable labeltab){
	for(interstat cur=eeyore;cur;cur=cur->nextblk){
		if(cur->kind!=begin_inter)
			continue;
		cur->u.begin->graph=flow_scan(labeltab,cur);
		//printf("\n");
		//flow_travel(cur->u.begin->graph);
		//flow_clear_history(cur->u.begin->graph);
		if(flow_check(cur->u.begin->graph)) return -1;
		flow_clear_history(cur->u.begin->graph);
	}
	return 0;
}
