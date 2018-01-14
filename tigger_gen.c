#include "eeyore.h"
#include "symbol_table.h"

static string regname[]={"a0","a1","a2","a3","a4","a5","a6","a7",
	"t0","t1","t2","t3","t4","t5","t6",
	"s0","s1","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11",
	"x0"};

extern string op2string(int,int);
extern int label_sum;
extern stable func2reg;

static int reg_used[REGNUM];	//regs used by current statmemt
static int stack_pos;
static int stack_top;	//the max stack_pos it has been
static int enter_call;
static int param_num;
static int reg_in_stack[REGNUM];
static int mem_file_pos;
static int cur_file_pos;
static int end_num=0;
static int pushed=0;
static binder call_reg_bin;
static int reg_temp_used[REGNUM];

void adjust_top(){	//adjust stack_top if needed
	if(stack_top<stack_pos)
		stack_top=stack_pos;
}

//

void set_reg(funcinfo info,int var){	//announce that reg of var is used
	if(info->regs[var]>=0) reg_used[info->regs[var]]=1;
}

void set_reg_rval(funcinfo info,rval var){	//announce that reg of rval is used
	if(var->kind==var_rval&&info->regs[var->value]>=0)
		reg_used[info->regs[var->value]]=1;
}

int get_empty_reg(funcinfo info){	//find a empty reg, push one to stack if needed, announce that reg is used
	if(info->regnum<REGNUM){
		for(int i=0;i<REGNUM;i++){
			if(info->reg_used[i]==0&&reg_used[i]==0){
				reg_used[i]=1;
				info->regnum++;
				if(reg_temp_used[i]==0){
					printf("\tstore %s %d\n",regname[i],stack_pos++);
					reg_temp_used[i]=1;
					adjust_top();
				}
				return i;
			}
		}
	}
	for(int i=0;i<REGNUM;i++){
		if(reg_used[i]==0){
			printf("\tstore %s %d\n",regname[i],stack_pos++);
			reg_used[i]=1;
			return i;
		}
	}
	printf("err\n");
	return -1;
}

void resume_empty_reg(funcinfo info,int var){	//cancel a var's temp_regs, pop it's original value to it from stack if needed
	if(info->reg_used[info->temp_regs[var]]==0){
		info->regnum--;
		info->temp_regs[var]=-1;
		return;
	}
	printf("\tload %d %s\n",--stack_pos,regname[info->temp_regs[var]]);
	info->temp_regs[var]=-1;
}

void resume_empty_reg_temp(funcinfo info,int reg){	//cancel a temp reg that contain a immediate, pop it's original value to it from stack if needed
	if(info->reg_used[reg]==0){
		info->regnum--;
		return;
	}
	printf("\tload %d %s\n",--stack_pos,regname[reg]);
}


string get_reg(funcinfo info,int var,int alloc){	//get a reg for var if it has not a reg of temp_regs
	if(info->regs[var]>=0) return regname[info->regs[var]];
	else if(info->regs[var]==-1){
		if(info->temp_regs[var]>=0)	return regname[info->temp_regs[var]];
		else{
			if(alloc==0) return NULL;
			int temp_reg=get_empty_reg(info);
			info->temp_regs[var]=temp_reg;
			if(info->spec[var]==1){
				if(info->is_array[var])
					printf("\tloadaddr %s %s\n",info->vars[var],regname[temp_reg]);
				else printf("\tload %s %s\n",info->vars[var],regname[temp_reg]);
			}
			else{
				if(info->is_array[var])
					printf("\tloadaddr %d %s\n",info->stack[var],regname[temp_reg]);
				else printf("\tload %d %s\n",info->stack[var],regname[temp_reg]);
			}
			return regname[temp_reg];
		}
	}
	else{
		fprintf(stderr,"get_reg: get reg for a dead variabe\n");
		return NULL;
	}
}

string get_reg_rval(funcinfo info,rval var,int alloc){	//get a reg for rval, if it's a immediate, then get a empty reg immediately
	if(var->kind==var_rval) return get_reg(info,var->value,1);
	if(alloc){
		int temp_reg=get_empty_reg(info);
		printf("\t%s = %d\n",regname[temp_reg],var->value);
		return regname[temp_reg];
	}
	else return NULL;
}


void resume_reg(funcinfo info,int var){	//cancel the reg of var, if it has a temp_regs
	if(info->regs[var]>=0) return;
	else if(info->regs[var]==-1){
		if(info->temp_regs[var]>=0){
			resume_empty_reg(info,var);
		}
		else return;
	}
	else return;
}

int reg2int(string reg){	//regname to regindex
	int num=0;
	if(reg[0]=='s') num+=15;
	else if(reg[0]=='t') num+=8;
	else if(reg[0]=='x') num+=27; //should never happen
	num+=atoi(reg+1);
	return num;
}

void resume_reg_rval(funcinfo info,rval var,string reg,int alloc){	//cancel the reg of rvar, if it's a immediate, then cancel that reg immediately
	if(var->kind==var_rval) resume_reg(info,var->value);
	else if(alloc){
		int regindex=reg2int(reg);
		resume_empty_reg_temp(info,regindex);
	}
}

void write_back(funcinfo info,int var){	//write back a var that has no reg
	if(info->regs[var]>=0)  return;
	else if(info->regs[var]==-1){
		if(info->spec[var]==1){
			int temp_addr=get_empty_reg(info);
			printf("\tloadaddr %s %s\n",info->vars[var],regname[temp_addr]);
			printf("\t%s [0] = %s\n",regname[temp_addr],regname[info->temp_regs[var]]);
			adjust_top();
			resume_empty_reg_temp(info,temp_addr);
		}
		else{
			printf("\tstore %s %d\n",regname[info->temp_regs[var]],info->stack[var]);
		}
	}
	else return;
}

void reg_clear(){	//clear the regs used by the statment
	memset(reg_used,0,sizeof(reg_used));
}

int stat_out_zone(funcinfo info,int line,int var){
	int begin=info->ori_zones[var]->begin;
	int end=info->ori_zones[var]->end;
	if(line<begin||line>end) return 1;
	return 0;
}

//

void tigger_biop(funcinfo info,interstat tar){
	if(info->regs[tar->u.biop.var]==-2||stat_out_zone(info,tar->statnum,tar->u.biop.var)) return;
	reg_clear();
	set_reg_rval(info,tar->u.biop.r1);
	set_reg_rval(info,tar->u.biop.r2);
	set_reg(info,tar->u.biop.var);
	string var=get_reg(info,tar->u.biop.var,1);
	string r1=get_reg_rval(info,tar->u.biop.r1,1);
	string r2=get_reg_rval(info,tar->u.biop.r2,1);
	if(var!=NULL){
		printf("\t%s = %s %s %s\n",var,r1,op2string(tar->u.biop.oper,1),r2);
	}
	adjust_top();
	write_back(info,tar->u.biop.var);
	resume_reg_rval(info,tar->u.biop.r2,r2,1);
	resume_reg_rval(info,tar->u.biop.r1,r1,1);
	resume_reg(info,tar->u.biop.var);
}

void tigger_uniop(funcinfo info,interstat tar){
	if(info->regs[tar->u.uniop.var]==-2||stat_out_zone(info,tar->statnum,tar->u.uniop.var)) return;
	reg_clear();
	set_reg_rval(info,tar->u.uniop.r);
	set_reg(info,tar->u.uniop.var);
	string var=get_reg(info,tar->u.uniop.var,0);
	string r=get_reg_rval(info,tar->u.uniop.r,1);
	adjust_top();
	if(var!=NULL){
		printf("\t%s = %s %s\n",var,op2string(tar->u.uniop.oper,0),r);
	}
	else{
		printf("\t%s = %s %s\n",r,op2string(tar->u.uniop.oper,0),r);
		printf("\tstore %s %d\n",r,info->stack[tar->u.uniop.var]);
		printf("\t%s = %s %s\n",r,op2string(tar->u.uniop.oper,0),r);
	}
	resume_reg_rval(info,tar->u.uniop.r,r,1);
	resume_reg(info,tar->u.uniop.var);
}

void tigger_trans(funcinfo info,interstat tar){
	if(info->regs[tar->u.trans.var]==-2||stat_out_zone(info,tar->statnum,tar->u.trans.var)) return;
	if(tar->u.trans.r->kind==var_rval&&tar->u.trans.r->value==tar->u.trans.var) return;	//should never happen
	reg_clear();
	set_reg_rval(info,tar->u.trans.r);
	set_reg(info,tar->u.trans.var);
	string var=get_reg(info,tar->u.trans.var,0);
	string r=get_reg_rval(info,tar->u.trans.r,0);
	adjust_top();
	if(var!=NULL){
		if(r!=NULL){
			if(var!=r) printf("\t%s = %s\n",var,r);
		}
		else printf("\t%s = %d\n",var,tar->u.trans.r->value);
	}
	else{
		if(r!=NULL){
			printf("\tstore %s %d\n",r,info->stack[tar->u.trans.var]);
		}
		else{
			fprintf(stderr,"tigger_trans: front assert failed\n");
		}
	}
	resume_reg_rval(info,tar->u.trans.r,r,0);
	resume_reg(info,tar->u.trans.var);
}

void tigger_larray(funcinfo info,interstat tar){
	reg_clear();
	set_reg_rval(info,tar->u.larray.index);
	set_reg_rval(info,tar->u.larray.r);
	set_reg(info,tar->u.larray.var);
	string index=get_reg_rval(info,tar->u.larray.index,0);
	string r=get_reg_rval(info,tar->u.larray.r,1);
	string var=get_reg(info,tar->u.larray.var,1);
	if(index!=NULL){
		printf("\t%s = %s + %s\n",index,index,var);
		printf("\t%s [0] = %s\n",index,r);
	}
	else printf("\t%s [%d] = %s\n",var,tar->u.larray.index->value,r);
	adjust_top();
	resume_reg(info,tar->u.larray.var);
	resume_reg_rval(info,tar->u.larray.r,r,1);
	resume_reg_rval(info,tar->u.larray.index,index,0);
}

void tigger_rarray(funcinfo info,interstat tar){
	if(info->regs[tar->u.rarray.l]==-2||stat_out_zone(info,tar->statnum,tar->u.rarray.l)) return;
	reg_clear();
	set_reg_rval(info,tar->u.rarray.index);
	set_reg(info,tar->u.rarray.r);
	set_reg(info,tar->u.rarray.l);
	string l=get_reg(info,tar->u.rarray.l,1);
	string index=get_reg_rval(info,tar->u.rarray.index,0);
	string r=get_reg(info,tar->u.rarray.r,1);
	adjust_top();
	if(index!=NULL){
			printf("\t%s = %s + %s\n",index,index,r);
			printf("\t%s = %s [0]\n",l,index);
		}
	else printf("\t%s = %s [%d]\n",l,r,tar->u.rarray.index->value);
	write_back(info,tar->u.rarray.l);
	resume_reg(info,tar->u.rarray.r);
	resume_reg_rval(info,tar->u.rarray.index,index,0);
	resume_reg(info,tar->u.rarray.l);
}

void tigger_if(funcinfo info,interstat tar){
	reg_clear();
	set_reg_rval(info,tar->u.iff.r1);
	set_reg_rval(info,tar->u.iff.r2);
	string r1=get_reg_rval(info,tar->u.iff.r1,1);
	string r2=get_reg_rval(info,tar->u.iff.r2,0);
	if(r2!=NULL||tar->u.iff.r2->value!=0) printf("opt failed\n");
	printf("\tif %s == x0 goto l%d\n",r1,tar->u.iff.label);
	adjust_top();
	resume_reg_rval(info,tar->u.iff.r2,r2,0);
	resume_reg_rval(info,tar->u.iff.r1,r1,1);
}

void tigger_goto(funcinfo info,interstat tar){
	printf("\tgoto l%d\n",tar->u.gotoo);
}

void tigger_label(funcinfo info,interstat tar){
	printf("l%d:\n",tar->u.label);
}

void tigger_decl(funcinfo info,interstat tar){
	if(tar->u.decl.len==0) return;
	string var=get_reg(info,tar->u.decl.var,1);
	printf("\tloadaddr %d %s\n",stack_pos,var);
	stack_pos+=tar->u.decl.len;
	adjust_top();
	write_back(info,tar->u.decl.var);
	resume_reg(info,tar->u.decl.var);
}

void push_all(funcinfo info){
	int *call_reg_used=(int *)call_reg_bin->defin;
	int call_argc=call_reg_bin->flag;
	if(call_argc==0) call_argc=1;	//always push a0
	if(call_reg_used)	
		for(int i=0;i<CALLERSAVE;i++){
			if(info->reg_used[i]&&(call_reg_used[i]||i<call_argc)){
				reg_in_stack[i]=stack_pos;
				printf("\tstore %s %d\n",regname[i],stack_pos++);
			}
		}
	else
		for(int i=0;i<CALLERSAVE;i++){
			if(info->reg_used[i]){
				reg_in_stack[i]=stack_pos;
				printf("\tstore %s %d\n",regname[i],stack_pos++);
			}
		}
	adjust_top();
}

void pop_all(funcinfo info){
	int *call_reg_used=(int *)call_reg_bin->defin;
	int call_argc=call_reg_bin->flag;
	if(call_argc==0) call_argc=1;	//always push a0
	if(call_reg_used)
		for(int i=CALLERSAVE-1;i>=0;i--){
			if(info->reg_used[i]&&(call_reg_used[i]||i<call_argc)){	
				reg_in_stack[i]=-1;
				printf("\tload %d %s\n",--stack_pos,regname[i]);
			}
		}
	else
		for(int i=CALLERSAVE-1;i>=0;i--){
			if(info->reg_used[i]){	
				reg_in_stack[i]=-1;
				printf("\tload %d %s\n",--stack_pos,regname[i]);
			}
		}
}

void load_param(funcinfo info,rval var,int param_index){
	if(var->kind==int_rval){
		printf("\ta%d = %d\n",param_index,var->value);
		return;
	}
	if(info->regs[var->value]==-1){
		if(info->spec[var->value]==1){
			if(info->is_array[var->value]){
				printf("\tloadaddr %s a%d\n",info->vars[var->value,param_index],param_index);
			}
			else{
				printf("\tload %s a%d\n",info->vars[var->value],param_index);
			}
		}
		else{
			printf("\tload %d a%d\n",info->stack[var->value],param_index);
		}
	}
	else if(info->regs[var->value]>=0){
		int *call_reg_used=(int *)call_reg_bin->defin;
		int call_argc=call_reg_bin->flag;
		if(call_argc==0) call_argc=1;	//always push a0
		if(info->regs[var->value]>=CALLERSAVE||call_reg_used&&(call_reg_used[info->regs[var->value]]==0&&info->regs[var->value]>=call_argc))	
			printf("\ta%d = %s\n",param_index,regname[info->regs[var->value]]);
		else{	// in stack
			printf("\tload %d a%d\n",reg_in_stack[info->regs[var->value]],param_index);
		}
	}
	else return;	//should never happen
}

void store_res(funcinfo info,int var){
	if(info->regs[var]==-1){
		if(info->spec[var]==1){
			printf("\tloadaddr %s a1\n",info->vars[var]);
			printf("\ta1 [0] = a0\n");
		}
		else{
			printf("\tstore a0 %d\n",info->stack[var]);
		}
	}
	else if(info->regs[var]>=0){
		int *call_reg_used=(int *)call_reg_bin->defin;
		int call_argc=call_reg_bin->flag;
		if(call_argc==0) call_argc=1;	//always push a0
		if(info->regs[var]>=CALLERSAVE||call_reg_used&&(call_reg_used[info->regs[var]]==0&&info->regs[var]>=call_argc))
			printf("\t%s = a0\n",regname[info->regs[var]]);
		else{	// in stack
			printf("\tstore a0 %d\n",reg_in_stack[info->regs[var]]);
		}
	}
	else return;	//should never happen
}

void tigger_param(funcinfo info,interstat tar){
	if(!enter_call){
		call_reg_bin=table_find(func2reg,tar->u.param.name);
		enter_call=1;
		push_all(info);
	}
	load_param(info,tar->u.param.param,param_num++);
}

void tigger_call(funcinfo info,interstat tar){
	if(!enter_call){
		call_reg_bin=table_find(func2reg,tar->u.call.func);
		enter_call=1;
		push_all(info);
	}
	param_num=0;
	enter_call=0;
	printf("\tcall f_%s\n",tar->u.call.func);
	interstat nxt=tar->next;
	if(nxt->kind==trans_inter){
		int nxt_var=nxt->u.trans.var;
		if(info->regs[nxt_var]==-2||stat_out_zone(info,nxt->statnum,nxt_var)){
			pop_all(info);
			return;
		}
	}
	store_res(info,tar->u.call.var);
	pop_all(info);
}

void load_res(funcinfo info,rval var){
	if(var->kind==var_rval){
		if(info->regs[var->value]>=0){
			printf("\ta0 = %s\n",regname[info->regs[var->value]]);
			return;
		}
		if(info->regs[var->value]==-1){
			if(info->spec[var->value]==1){
				printf("\tload %s a0\n",info->vars[var->value]);
			}
			else{
				printf("\tload %d a0\n",info->stack[var->value]);
			}
		}
		else return;	//should never happen
	}
	else printf("\ta0 = %d\n",var->value);
}

void tigger_return(funcinfo info,interstat tar){
	load_res(info,tar->u.returnn);
	while(tar->next->kind==label_inter)
		tar=tar->next;
	if(tar->next->kind!=end_inter)
		printf("\tgoto l%d\n",end_num);
}

void tigger_begin(funcinfo info,interstat tar){
	printf("\nf_%s [%d] [",info->func,info->args);
	mem_file_pos=ftell(stdout);
	printf("          ]\n");
	for(int i=CALLERSAVE;i<REGNUM;i++){
		if(info->reg_used[i]){
			reg_used[i]=1;
			printf("\tstore %s %d\n",regname[i],stack_pos++);
		}
	}
	adjust_top();
	for(int j=0;j<info->varnum;j++){
		if(info->zones[j]->begin!=0) continue;
		int i=info->zones[j]->number;
		if(info->spec[i]==1){
			if(info->regs[i]<0) continue;
			if(info->is_array[i]){
				printf("\tloadaddr %s %s\n",
					info->vars[i],
					regname[info->regs[i]]);
			}
			else{
				printf("\tload %s %s\n",
					info->vars[i],
					regname[info->regs[i]]);
			}
		}
		else if(info->spec[i]==-1){
			if(info->regs[i]<0)
				printf("\tstore %s %d\n",
					regname[info->regs[i]],
					info->stack[i]);
		}
	}
	adjust_top();
	for(int j=info->varnum-1;j>=0;j--){
		if(info->zones[j]->begin!=0) continue;
		int i=info->zones[j]->number;
		if(info->spec[i]==-1&&info->regs[i]>=0&&info->regs[i]!=i)
			printf("\t%s = a%d\n",regname[info->regs[i]],i);
	}
	return;
}

void tigger_end(funcinfo info,interstat tar){
	printf("l%d:\n",end_num++);
	int avail=info->args;	//max(argc,1(//a0)) is always available
	if(avail==0) avail=1;
	for(int i=REGNUM-1;i>=avail;i--){	//no need to resume max(argc,1(//a0))
		if(reg_temp_used[i])
			printf("\tload %d %s\n",--stack_pos,regname[i]);
	}
	for(int i=REGNUM-1;i>=CALLERSAVE;i--){
		if(info->reg_used[i])
			printf("\tload %d %s\n",--stack_pos,regname[i]);
	}
	printf("\treturn\n");
	printf("end f_%s\n",info->func);
	cur_file_pos=ftell(stdout);
	fseek(stdout,mem_file_pos,0);
	printf("%d",stack_top);
	fseek(stdout,cur_file_pos,0);
}



void tigger_func(interstat func){
	interstat cur=func;
	funcinfo info=func->u.begin;
	stack_pos=info->stacknum;
	stack_top=stack_pos;
	enter_call=0;
	param_num=0;
	memset(reg_temp_used,0,sizeof(reg_temp_used));
	int avail=info->args;	//max(argc,1(//a0)) is always available
	if(avail==0) avail=1;
	for(int i=0;i<avail;i++){
		reg_temp_used[i]=1;
	}
	for(;cur!=NULL;cur=cur->next){
		if(cur->waste) {continue;}
		switch(cur->kind){
			case biop_inter: tigger_biop(info,cur);break;
			case uniop_inter: tigger_uniop(info,cur);break;
			case trans_inter: tigger_trans(info,cur);break;
			case larray_inter: tigger_larray(info,cur);break;
			case rarray_inter: tigger_rarray(info,cur);break;
			case if_inter: tigger_if(info,cur);break;
			case goto_inter: tigger_goto(info,cur);break;
			case label_inter: tigger_label(info,cur);break;
			case param_inter: tigger_param(info,cur);break;
			case call_inter: tigger_call(info,cur);break;
			case return_inter: tigger_return(info,cur);break;
			case decl_inter: tigger_decl(info,cur);break;
			case begin_inter: tigger_begin(info,cur);break;
			case end_inter: tigger_end(info,cur);break;
		}
	}
}


void tigger_eeyore(interstat eeyore){
	freopen("a.t","w",stdout);
	int global_num=0;
	end_num=label_sum;
	memset(reg_in_stack,-1,sizeof(reg_in_stack));
	for(interstat cur=eeyore;cur!=NULL;cur=cur->nextblk){
		if(cur->kind==begin_inter){
			tigger_func(cur);
			printf("\n");
		}
		else if(cur->kind==decl_inter){
			if(cur->u.decl.len==0)
				printf("v%d = 0\n",global_num++);
			else printf("v%d = malloc %d\n",global_num++,cur->u.decl.len);
		}
	}
	fclose(stdout);
	freopen("/dev/tty","w",stdout);
}
