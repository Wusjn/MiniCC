#include "eeyore.h"


vartable vartab_create(){
	vartable tar=(vartable)checked_malloc(sizeof(*tar));
	memset(tar->table,0,sizeof(tar->table));
	tar->localsum=0;
	return tar;
}

int vartab_insert(vartable tab,char type,int global){
	varentry tar=tab->table[global%VARTABSIZE];
	for(varentry cur=tar;cur;cur=cur->next){
		if(cur->global==global&&cur->type==type)
			return cur->local;
	}
	varentry new_entry=(varentry)checked_malloc(sizeof(*new_entry));
	new_entry->global=global;
	new_entry->type=type;
	new_entry->local=tab->localsum++;
	new_entry->next=tar;
	new_entry->array=0;
	tab->table[global%VARTABSIZE]=new_entry;
	return new_entry->local;
}

int vartab_array_insert(vartable tab,char type,int global){
	int tar=vartab_insert(tab,type,global);
	varentry tar_entry=tab->table[global%VARTABSIZE];
	tar_entry->array=1;
	return tar;
}

void vartab_labinsert(vartable tab,int label,int addr){
	varentry tar=tab->table[label%VARTABSIZE];
	varentry new_entry=(varentry)checked_malloc(sizeof(*new_entry));
	new_entry->global=label;
	new_entry->type='l';
	new_entry->local=addr;
	tab->localsum++;
	new_entry->next=tar;
	tab->table[label%VARTABSIZE]=new_entry;
	return;
}

int vartab_find(vartable tab,char type,int global){
	varentry tar=tab->table[global%VARTABSIZE];
	for(varentry cur=tar;cur;cur=cur->next){
		if(cur->global==global&&cur->type==type)
			return cur->local;
	}
	return -1;
}

string make_string(char type,int global,int trans){
	if(type=='p') type='a';
	if(trans==1) type='v';
	char temp[30];
	temp[0]=type;
	sprintf(temp+1,"%d",global);
	return String(temp);
}

void refillfunc(vartable tab,funcinfo func){
	func->varnum=func->tab->localsum;
	func->is_array=(int *)checked_malloc(sizeof(int)*(func->varnum));
	memset(func->is_array,0,sizeof(int)*(func->varnum));
	func->temp_regs=(int *)checked_malloc(sizeof(int)*(func->varnum));
	memset(func->temp_regs,-1,sizeof(int)*(func->varnum));
	func->regs=(int *)checked_malloc(sizeof(int)*(func->varnum));
	func->spec=(int *)checked_malloc(sizeof(int)*(func->varnum));
	func->vars=(string *)checked_malloc(sizeof(string)*(func->varnum));
	func->zones=(livezone *)checked_malloc(sizeof(livezone)*(func->varnum));
	func->stack=(int *)checked_malloc(sizeof(int)*(func->varnum));
	for(int i=0;i<func->varnum;i++){
		func->zones[i]=(livezone)checked_malloc(sizeof(struct livezone_));
	}
	for(int i=0;i<VARTABSIZE;i++){
		varentry tar=func->tab->table[i];
		while(tar){
			if(tar->array){
				func->is_array[tar->local]=1;
			}
			int temp=-1;
			if(tar->type=='p') func->spec[tar->local]=-1;
			else if(tar->type=='T'&&((temp=vartab_find(tab,tar->type,tar->global))>=0)) func->spec[tar->local]=1;
			else func->spec[tar->local]=0;
			if(temp<0) temp=tar->global;
			func->vars[tar->local]=make_string(tar->type,temp,func->spec[tar->local]);
			tar=tar->next;
		}
	}
	for(int i=0;i<func->varnum;i++) fprintf(stderr,"var %d : %s : %d\n",i,func->vars[i],func->spec[i]);
}

void def_use(interstat tar,int def,rval usea,rval useb,int usec){
	tar->def=def;
	if(usea!=NULL&&usea->kind==var_rval) tar->use[0]=usea->value;
	else tar->use[0]=-1;
	if(useb!=NULL&&useb->kind==var_rval) tar->use[1]=useb->value;
	else tar->use[1]=-1;
	tar->use[2]=usec;
}

rval Rval(vartable tab,char t,int g){
	rval tar=(rval)checked_malloc(sizeof(*tar));
	if(t=='i'){
		tar->kind=int_rval;
		tar->value=g;
	}
	else{
		tar->kind=var_rval;
		tar->value=vartab_insert(tab,t,g);
	}
	return tar;
}

int Lval(vartable tab,char t,int g){
	return vartab_insert(tab,t,g);
}

void bindchain(interstat curfunc,interstat tar){
	//printf("%d",curfunc->u.begin->statnum);
	tar->prev=curfunc->u.begin->end;
	tar->prev->next=tar;
	curfunc->u.begin->end=tar;
	tar->statnum=curfunc->u.begin->statnum++;
	tar->waste=1;
}

void Biop_inter(interstat curfunc,char t1,int g1,
	char t2,int g2,enum bioper oper,char t3,int g3){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=biop_inter;
	tar->u.biop.var=Lval(curfunc->u.begin->tab,t1,g1);
	tar->u.biop.r1=Rval(curfunc->u.begin->tab,t2,g2);
	tar->u.biop.r2=Rval(curfunc->u.begin->tab,t3,g3);
	tar->u.biop.oper=oper;
	def_use(tar,tar->u.biop.var,tar->u.biop.r1,tar->u.biop.r2,-1);
	bindchain(curfunc,tar);
}

void Uniop_inter(interstat curfunc,char t1,int g1,
	enum unioper oper,char t2,int g2){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=uniop_inter;
	tar->u.uniop.var=Lval(curfunc->u.begin->tab,t1,g1);
	tar->u.uniop.r=Rval(curfunc->u.begin->tab,t2,g2);
	tar->u.uniop.oper=oper;
	def_use(tar,tar->u.uniop.var,tar->u.uniop.r,NULL,-1);
	bindchain(curfunc,tar);
}

void Trans_inter(interstat curfunc,char t1,int g1,char t2,int g2){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=trans_inter;
	tar->u.trans.var=Lval(curfunc->u.begin->tab,t1,g1);
	tar->u.trans.r=Rval(curfunc->u.begin->tab,t2,g2);
	def_use(tar,tar->u.trans.var,tar->u.trans.r,NULL,-1);
	bindchain(curfunc,tar);
}

void Rarray_inter(interstat curfunc,char t1,int g1,
	char t2,int g2,char t3,int g3){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=rarray_inter;
	tar->u.rarray.l=Lval(curfunc->u.begin->tab,t1,g1);
	tar->u.rarray.r=Lval(curfunc->u.begin->tab,t2,g2);
	tar->u.rarray.index=Rval(curfunc->u.begin->tab,t3,g3);
	def_use(tar,tar->u.rarray.l,tar->u.rarray.index,NULL,tar->u.rarray.r);
	bindchain(curfunc,tar);
}

void Call_inter(interstat curfunc,char t,int g,string name){
	if(!strcmp(curfunc->u.begin->func,name))
		curfunc->u.begin->selfnum++;
	curfunc->u.begin->callnum++;
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=call_inter;
	tar->u.call.var=Lval(curfunc->u.begin->tab,t,g);
	tar->u.call.func=String(name);
	def_use(tar,tar->u.call.var,NULL,NULL,-1);
	bindchain(curfunc,tar);
	for(interstat cur=tar->prev;cur!=NULL&&cur->kind==param_inter;cur=cur->prev){
		cur->u.param.name=tar->u.call.func;
	}
}

void If_inter(interstat curfunc,char t1,int g1,
	enum bioper oper,char t2,int g2,int label){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=if_inter;
	tar->u.iff.r1=Rval(curfunc->u.begin->tab,t1,g1);
	tar->u.iff.r2=Rval(curfunc->u.begin->tab,t2,g2);
	tar->u.iff.label=label;
	tar->u.iff.oper=oper;
	def_use(tar,-1,tar->u.iff.r1,tar->u.iff.r2,-1);
	bindchain(curfunc,tar);
}

void Label_inter(vartable tab,interstat curfunc,int label){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=label_inter;
	tar->u.label=label;
	vartab_labinsert(tab,label,(int)tar);
	def_use(tar,-1,NULL,NULL,-1);
	bindchain(curfunc,tar);
}

void Goto_inter(interstat curfunc,int label){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=goto_inter;
	tar->u.gotoo=label;
	def_use(tar,-1,NULL,NULL,-1);
	bindchain(curfunc,tar);
}

void Larray_inter(interstat curfunc,char t1,int g1,
	char t2,int g2,char t3,int g3){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=larray_inter;
	tar->u.larray.var=Lval(curfunc->u.begin->tab,t1,g1);
	tar->u.larray.index=Rval(curfunc->u.begin->tab,t2,g2);
	tar->u.larray.r=Rval(curfunc->u.begin->tab,t3,g3);
	def_use(tar,-1,tar->u.larray.r,tar->u.larray.index,tar->u.larray.var);
	bindchain(curfunc,tar);
}

void Return_inter(interstat curfunc,char t,int g){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=return_inter;
	tar->u.returnn=Rval(curfunc->u.begin->tab,t,g);
	def_use(tar,-1,tar->u.returnn,NULL,-1);
	bindchain(curfunc,tar);
}

interstat Begin_inter(string name,int argc){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=begin_inter;
	tar->waste=0;
	funcinfo tarinfo=(funcinfo)checked_malloc(sizeof(*tarinfo));
	tarinfo->args=argc;
	tarinfo->func=String(name);
	tarinfo->statnum=0;
	tarinfo->end=tar;
	tarinfo->tab=vartab_create();
	tar->u.begin=tarinfo;
	tar->nextblk=NULL;
	def_use(tar,-1,NULL,NULL,-1);
	tarinfo->callnum=0;
	tarinfo->selfnum=0;
	return tar;
}

void End_inter(vartable tab,interstat curfunc,string name){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=end_inter;
	tar->u.end=String(name);
	def_use(tar,-1,NULL,NULL,-1);
	bindchain(curfunc,tar);
	tar->waste=0;
	refillfunc(tab,curfunc->u.begin);
}

interstat Decl_inter(vartable tab,interstat curfunc,int len,char t,int g){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=decl_inter;
	tar->u.decl.len=len;
	if(curfunc->kind!=begin_inter||
		curfunc->u.begin->end->kind==end_inter){
		tar->u.decl.var=Lval(tab,t,g);
		tar->nextblk=NULL;
		return tar;
	}
	else{
		if(len==0){
			tar->u.decl.var=Lval(curfunc->u.begin->tab,t,g);
			def_use(tar,-1,NULL,NULL,-1);
		}
		else{
			tar->u.decl.var=vartab_array_insert(curfunc->u.begin->tab,t,g);
			def_use(tar,tar->u.decl.var,NULL,NULL,-1);
		}
		bindchain(curfunc,tar);
		return NULL;
	}
}

void Param_inter(interstat curfunc,char t,int g){
	interstat tar=(interstat)checked_malloc(sizeof(*tar));
	tar->kind=param_inter;
	tar->u.param.param=Rval(curfunc->u.begin->tab,t,g);
	def_use(tar,-1,tar->u.param.param,NULL,-1);
	bindchain(curfunc,tar);
}

void Args_inter(vartable tab,int argc){
	for(int i=0;i<argc;i++){
		vartab_insert(tab,'p',i);
	}
}

