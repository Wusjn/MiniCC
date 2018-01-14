#include <stdio.h>
#include <stdlib.h>
#include "nodes.h"
#include "symbol_table.h"


static stable top;
static int used=0;
static int notdefn=-4;
static type INT_type;

int stats_check(stats,int);
int vardefn_check(vardefn);
type var2type(binder,int,int);
int var2len(binder);
int comp2types(type,type,int);
type functypematch(binder,int,int);

binder table_find_err(stable tab,identifier key_id,int error_line){
	string key=key_id->id;
	binder bin=table_find(tab,key);
	if(bin==NULL){
		fprintf(stderr,"line %d: %s not declared\n",error_line,key);
	}
	else if(bin->kind==var_binder){
		if(bin->flag==1) key_id->tag=((vardefn)(bin->defin))->id->tag;
		else key_id->tag=((vardecl)(bin->defin))->id->tag;
	}
	return bin;
}

int comp2args(vardecls x, vardecls y){
	for(;x&&y;x=x->head,y=y->head){
		if(x->tail->kind!=y->tail->kind||x->tail->len!=y->tail->len||
			x->tail->thistype->kind!=y->tail->thistype->kind)
			return 0;
	}
	if(x!=NULL||y!=NULL) return 0;
	return 1;  
}
int compidsargs(identifiers x,binder tar,int error_line){
	vardecls y;
	binder tar_binder;
	int flag;
	type tar_type;
	if(tar->flag==1) y=((funcdefn)(tar->defin))->args;
	else y=((funcdecl)(tar->defin))->args;
	for(;x&&y;x=x->head,y=y->head){
		tar_binder=table_find_err(top,x->tail,x->pos.line);
		if(y->tail->kind==uni_vardecl) flag=0;
		else flag=1;
		tar_type=functypematch(tar_binder,flag,x->pos.line);
		if(tar_type==NULL) return 0;
		if(!comp2types(tar_type,y->tail->thistype,x->pos.line)) return 0;

		if(y->tail->kind==fix_vardecl&&y->tail->len!=var2len(tar_binder)){
			fprintf(stderr,"line %d: array length not matched\n",x->pos.line);
			return 0;
		}
	}
	if(x!=NULL){
		fprintf(stderr,"line %d: too many args\n",error_line);
		return 0;
	}
	if(y!=NULL){
		fprintf(stderr,"line %d: too few args\n",error_line);
		return 0;
	}
	return 1; 
}
int comp2types(type x,type y,int error_line){
	if(x==NULL||y==NULL) return 0;
	else if(x->kind!=y->kind){
		fprintf(stderr,"line %d: type not matched\n",error_line);
		return 0;
	}
	else return 1;
}
type functypematch(binder x,int array,int error_line){
	if(x==NULL) return NULL;
	if(x->kind!=var_binder) return NULL;
	if(x->flag==1){
		vardefn temp=((vardefn)(x->defin));
		if(array==1&&temp->kind!=array_vardefn){
			fprintf(stderr,"line %d: function needs an array, %s(defn in %d) is not an arrray\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		if(array==0&&temp->kind==array_vardefn){
			fprintf(stderr,"line %d: function needs a single number, %s(defn in %d) is an arrray\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		return temp->thistype;
	}
	else{
		vardecl temp=((vardecl)(x->defin));
		if(array==1&&temp->kind==uni_vardecl){
			fprintf(stderr,"line %d: function needs an array, %s(defn in %d) is not an arrray\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		if(array==0&&temp->kind!=uni_vardecl){
			fprintf(stderr,"line %d: function needs a single number, %s(defn in %d) is an arrray\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		return temp->thistype;
	}
}

type var2type(binder x,int array,int error_line){
	if(x==NULL) return NULL;
	if(x->kind!=var_binder) return NULL;
	if(x->flag==1){
		vardefn temp=((vardefn)(x->defin));
		if((array==1&&temp->kind!=array_vardefn)||(array==0&&temp->kind!=uni_vardefn)){
			fprintf(stderr,"line %d: type of %s defn-use not matched,defn in line %d\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		return temp->thistype;
	}
	else{
		vardecl temp=((vardecl)(x->defin));
		if((array==1&&temp->kind==uni_vardecl)||(array==0&&temp->kind!=uni_vardecl)){
			fprintf(stderr,"line %d: type of %s defn-use not matched,defn in line %d\n",error_line,temp->id->id,temp->pos.line);
			return NULL;
		}
		return temp->thistype;
	}
}
int var2len(binder x){
	if(x==NULL) return -1;
	if(x->kind!=var_binder) return -1;
	if(x->flag==1){
		vardefn temp=((vardefn)(x->defin));
		return temp->len;
	}
	else{
		vardecl temp=((vardecl)(x->defin));
		return temp->len;
	}
}

type expr_check(expr tar){
	type left_type,right_type;
	binder tar_binder;
	switch(tar->kind){
	case biop_expr:
		left_type=expr_check(tar->u.biop.left);
		right_type=expr_check(tar->u.biop.right);
		if(left_type==NULL||left_type->kind!=int_type) return NULL;
		if(right_type==NULL||right_type->kind!=int_type) return NULL;
		return left_type;
	case uniop_expr:
		right_type=expr_check(tar->u.uniop.right);
		if(right_type==NULL||right_type->kind!=int_type) return NULL;
		return right_type;
	case int_expr:
		return INT_type;
	case id_expr:
		tar_binder=table_find_err(top,tar->u.id,tar->pos.line);
		right_type=var2type(tar_binder,0,tar->pos.line);
		if(right_type==NULL) return NULL;
		return right_type;
	case array_expr:
		tar_binder=table_find_err(top,tar->u.array.id,tar->pos.line);
		left_type=var2type(tar_binder,1,tar->pos.line);
		if(left_type==NULL) return NULL;
		right_type=expr_check(tar->u.array.pos);
		if(right_type==NULL||right_type->kind!=int_type) return NULL;
		return left_type;
	case voidfunc_expr:
		tar_binder=table_find_err(top,tar->u.voidfunc,tar->pos.line);
		if(tar_binder==NULL||tar_binder->kind!=func_binder) return NULL;
		if(!compidsargs(NULL,tar_binder,tar->pos.line)) return NULL;
		return INT_type;
	case func_expr:
		tar_binder=table_find_err(top,tar->u.voidfunc,tar->pos.line);
		if(tar_binder==NULL||tar_binder->kind!=func_binder) return NULL;
		if(!compidsargs(tar->u.func.args,tar_binder,tar->pos.line)) return NULL;
		return INT_type;
	};	
}


int stat_check(stat tar){
	type tar_type,left_type,right_type;
	binder tar_binder;
	switch(tar->kind){
	case void_stat: return 0;
	case stats_stat: return stats_check(tar->u.statss,1);
	case ifthen_stat:
		tar_type=expr_check(tar->u.ifthen.cond);
		if(tar_type==NULL||tar_type->kind!=int_type)
			return -1;
		return stat_check(tar->u.ifthen.act);
	case ifelse_stat:
		tar_type=expr_check(tar->u.ifelse.cond);
		if(tar_type==NULL||tar_type->kind!=int_type)
			return -1;
		return stat_check(tar->u.ifelse.act)||stat_check(tar->u.ifelse.elseact);
	case while_stat:
		tar_type=expr_check(tar->u.whilee.cond);
		if(tar_type==NULL||tar_type->kind!=int_type)
			return -1;
		return stat_check(tar->u.whilee.act);
	case idassign_stat:
		tar_binder=table_find_err(top,tar->u.idassign.id,tar->pos.line);
		left_type=var2type(tar_binder,0,tar->pos.line);
		if(left_type==NULL) return -1;
		right_type=expr_check(tar->u.idassign.value);
		if(!comp2types(right_type,left_type,tar->pos.line))
			return -1;
		return 0;
	case aryassign_stat:
		tar_binder=table_find_err(top,tar->u.aryassign.id,tar->pos.line);
		left_type=var2type(tar_binder,1,tar->pos.line);
		if(left_type==NULL) return -1;
		right_type=expr_check(tar->u.aryassign.value);
		if(!comp2types(right_type,left_type,tar->pos.line))
			return -1;
		tar_type=expr_check(tar->u.aryassign.pos);
		if(tar_type==NULL||tar_type->kind!=int_type)
			return -1;
		return 0;
	case vardefn_stat:
		return vardefn_check(tar->u.vardefnn);
	case return_stat:
		right_type=expr_check(tar->u.returnn);
		if(right_type==NULL||right_type->kind!=int_type)
			return -1;
		return 0;
	};
}

int stats_check(stats tar,int create){
	if(tar==NULL) return 0;
	int ret;
	int prevused;
	stable prevtop;
	if(create){
		//prevused=used;
		prevtop=top;
		top=table_new(prevtop);
	}
	if(tar->kind==uni_stats){
		ret=stat_check(tar->tail);
	}
	else{
		ret=stats_check(tar->head,0)||stat_check(tar->tail);
	}
	if(create){
		top=prevtop;
		//used=prevused;
	}
	return ret;
}

int insert_args(vardecls tar){
	int temp_used=1;
	for(;tar;tar=tar->head){
		tar->tail->id->tag=-(temp_used++);
		binder tar_binder=(binder)checked_malloc(sizeof(*tar_binder));
		tar_binder->kind=var_binder;
		tar_binder->defin=tar->tail;
		tar_binder->flag=0;
		string tar_string=String(tar->tail->id->id);
		if(table_insert(top,tar_string,tar_binder)!=NULL){
			fprintf(stderr,"line %d: %s redeclared\n",tar->tail->pos.line,tar_string);
			return -1;
		}
	}
	return temp_used-1;
}

int mainfunc_check(mainfunc tar){
	if(tar->kind==empty_mainfunc) return 0;
	stable prevtop=top;
	//int prevused=used;
	top=table_new(prevtop);
	int ret=stats_check(tar->content,0);
	top=prevtop;
	//used=prevused;
	return ret;
}

int funcdecl_check(funcdecl tar){
	if(tar->thistype->kind!=int_type) return -1;
	binder tar_binder=(binder)checked_malloc(sizeof(*tar_binder));
	tar_binder->kind=func_binder;
	tar_binder->defin=tar;
	tar_binder->flag=0;
	string tar_string=String(tar->id->id);
	if((tar_binder=table_insert(top,tar_string,tar_binder))!=NULL){
		if(tar_binder->kind==func_binder&&tar_binder->flag==0){
			if(comp2args(tar->args,((funcdecl)(tar_binder->defin))->args)){
				return 0;
			}
		}
		fprintf(stderr,"line %d: %s redeclared\n",tar->pos.line,tar_string);
		return -1;
	}
	notdefn++;
	return 0;
}


int funcdefn_check(funcdefn tar){
	if(tar->thistype->kind!=int_type) return -1;
	binder tar_binder=(binder)checked_malloc(sizeof(*tar_binder));
	tar_binder->kind=func_binder;
	tar_binder->defin=tar;
	tar_binder->flag=1;
	string tar_string=String(tar->id->id);
	if((tar_binder=table_insert(top,tar_string,tar_binder))!=NULL){
		if(tar_binder->kind==func_binder&&tar_binder->flag==0){
			if(comp2args(tar->args,((funcdecl)(tar_binder->defin))->args)){
				tar_binder->flag=-1;
				notdefn--;
				return 0;
			}
		}
		fprintf(stderr,"line %d: %s redeclared\n",tar->pos.line,tar_string);
		return -1;
	}
	stable prevtop=top;
	top=table_new(prevtop);
	//int prevused=used;
	if((tar->argc=insert_args(tar->args))<0) return -1;
	int ret=stats_check(tar->content,0);
	top=prevtop;
	//used=prevused;
	return ret;
}

int vardefn_check(vardefn tar){
	tar->id->tag=used++;
	binder tar_binder=(binder)checked_malloc(sizeof(*tar_binder));
	tar_binder->kind=var_binder;
	tar_binder->defin=tar;
	tar_binder->flag=1;
	string tar_string=String(tar->id->id);
	if(table_insert(top,tar_string,tar_binder)!=NULL){
		fprintf(stderr,"line %d: %s redeclared\n",tar->pos.line,tar_string);
		return -1;
	}
	else return 0;
}

int def_check(def tar){
	if(tar->kind==vardefn_def){
		return vardefn_check(tar->vardefnn);
	}
	else if(tar->kind==funcdefn_def){
		return funcdefn_check(tar->funcdefnn);
	}
	else{
		return funcdecl_check(tar->funcdecll);
	}
}

int defs_check(defs tar){
	if(tar->kind==uni_defs){
		return def_check(tar->tail);
	}
	else{
		return defs_check(tar->head)||def_check(tar->tail);
	}
}

int goal_check(goal tar){
	if(tar->kind==nodef_goal){
		return mainfunc_check(tar->main);
	}
	else{
		return defs_check(tar->util)||mainfunc_check(tar->main);
	}
}

int type_check(goal tar){
	int ret;
	top=table_new(NULL);
	INT_type=(type)checked_malloc(sizeof(*INT_type));
	INT_type->kind=int_type;
	if(goal_check(tar)==0) ret=0;
	else ret=-1;
	if(notdefn!=0){
		ret=-1;
		fprintf(stderr,"a function declared but not defined\n");
	}
	return ret;
}


