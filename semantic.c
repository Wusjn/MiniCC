#include "types.h"
#include "syntax_tree.h"

static stable types,top;
static TYPE INT_TYPE;
static TYPE FLOAT_TYPE;
static int tag=0;
static int offset=0;
static int enclosing=-1;
static TYPE funcret=NULL;
static int tmp=0;
static int label=0;
static int instruct=0;

void expr_check(expr tar);

string BIOP[]={"+","-","*","/","%","&&","||","<","==",">","<=",">=","!="};
string UNIOP[]={"!","-"};

char tag2c(int tag){
	if(tag>=0) return 'T';
	else return 'p';
}

int abstag(int tag){
	if(tag>=0) return tag;
	else return -tag-1;
}

int get_subtype_width(TYPE tar){
	if(tar->kind==pointer_TYPE)
		return tar->u.subtype->width;
	else return tar->u.array.elem->width;
}

void biop_check(expr tar){
	TYPE lefttype=(TYPE)(tar->u.biop.left->thistype);
	TYPE righttype=(TYPE)(tar->u.biop.right->thistype);
	int lefttag=tar->u.biop.left->tmp;
	int righttag=tar->u.biop.right->tmp;
	int tartag=tar->tmp;
	enum bioper oper=tar->u.biop.oper;
	if(lefttype==INT_TYPE&&righttype==INT_TYPE){
		tar->thistype=INT_TYPE;
		printf("\tt%d = t%d %s t%d\n",tartag,lefttag,BIOP[oper],righttag);
		return;
	}
	else if(lefttype==INT_TYPE){
		if((oper==plus||oper==minus)&&
			(righttype->kind==pointer_TYPE||righttype->kind==array_TYPE)){
			int width=get_subtype_width(righttype);
			tar->thistype=righttype;
			printf("\tt%d = t%d * %d\n",lefttag,lefttag,width);
			printf("\tt%d = t%d %s t%d\n",tartag,righttag,BIOP[oper],lefttag);
			return;
		}
		else if((oper==eq||oper==ne)&&righttype->kind==pointer_TYPE){
			tar->thistype=INT_TYPE;
			printf("\tt%d = t%d %s t%d\n",tartag,righttag,BIOP[oper],lefttag);
		}
		else panic("1unmatched operand");
	}
	else if(righttype==INT_TYPE){
		if((oper==plus||oper==minus)&&
			(lefttype->kind==pointer_TYPE||lefttype->kind==array_TYPE)){
			int width=get_subtype_width(lefttype);
			tar->thistype=lefttype;
			printf("\tt%d = t%d * %d\n",righttag,righttag,width);
			printf("\tt%d = t%d %s t%d\n",tartag,lefttag,BIOP[oper],righttag);
			return;
		}
		else if((oper==eq||oper==ne)&&lefttype->kind==pointer_TYPE){
			tar->thistype=INT_TYPE;
			printf("\tt%d = t%d %s t%d\n",tartag,lefttag,BIOP[oper],righttag);
		}
		else panic("2unmatched operand");
	}
	else panic("3unmatched operand");
}

TYPE uniop_type(enum unioper oper,TYPE right){
	if(right->kind!=int_TYPE)
		panic("4unmatched operand");
	return INT_TYPE;
}

int assign_valid(TYPE left,TYPE right){
	if(left->kind!=right->kind){
		if(left->kind==pointer_TYPE){
			if(right->kind==int_TYPE)
				return 1;
			if(right->kind==array_TYPE
				&&type_equal(left->u.subtype,right->u.array.elem))
				return 1;
		}
		else return 0;
	}
	if(left->kind==int_TYPE)
		return 1;
	if(left->kind==pointer_TYPE
		&&type_equal(left->u.subtype,right->u.subtype))
		return 1;
	else return 0;
}

int offtag;
int basetag;
int firstptr;

TYPE parse_index(TYPE tar,indexes index){
	if(index==NULL){
		return tar;
	}
	expr_check(index->head);
	if(((TYPE)index->head->thistype)->kind!=int_TYPE)
		panic("invalid leftvalue");
	if(tar->kind==array_TYPE){
		printf("\tt%d = t%d * %d\n",index->head->tmp,index->head->tmp,tar->u.array.elem->width);
		printf("\tt%d = t%d + t%d\n",offtag,offtag,index->head->tmp);
		return parse_index(tar->u.array.elem,index->tail);
	}
	else if(tar->kind==pointer_TYPE){
		//printf("pointer!\n");
		if(!firstptr){
			printf("\tt%d = t%d [t%d]\n",basetag,basetag,offtag);
			printf("\tt%d = t%d * %d\n",index->head->tmp,index->head->tmp,tar->u.subtype->width);
			printf("\tt%d = t%d\n",offtag,index->head->tmp);
		}
		else{
			printf("\tt%d = t%d * %d\n",index->head->tmp,index->head->tmp,tar->u.subtype->width);
			printf("\tt%d = t%d + t%d\n",offtag,offtag,index->head->tmp);
			firstptr=0;
		}
		return parse_index(tar->u.subtype,index->tail);
	}
	else	panic("invalid leftvalue");
}

TYPE parse_field(TYPE tar,leftvalue left){
	if(left==NULL)
		return tar;
	if(tar->kind!=record_TYPE)
		panic("invalid leftvalue");
	VARINFO field_info=get_field_VARINFO(tar,left->name);
	if(field_info==NULL)
		panic("invalid leftvalue");
	printf("\tt%d = t%d + %d\n",offtag,offtag,field_info->offset);
	TYPE type=parse_index(field_info->type,left->index);
	return parse_field(type,left->tail);
}

Access get_access(leftvalue left){
	int saveoff=offtag,savebase=basetag,saveptr=firstptr;
	Access r=(Access)checked_malloc(sizeof(*r));
	VARINFO tar_info=get_info(top,left->name);
	if(tar_info==NULL||tar_info->type->kind==args_TYPE)
		panic("unknown variable");
	basetag=tar_info->tag;
	TYPE tar=tar_info->type;
	if(tar->kind==pointer_TYPE) firstptr=1;
	else firstptr=0;
	if(left->index==NULL&&left->tail==NULL)
		r->offset=-1;
	else{
		offtag=tmp++;
		basetag=tmp++;
		r->offset=offtag;
		printf("\tvar t%d\n",offtag);
		printf("\tt%d = 0\n",offtag);
		printf("\tvar t%d\n",basetag);
		printf("\tt%d = %c%d\n",basetag,tag2c(tar_info->tag),abstag(tar_info->tag));
	}
	tar=parse_index(tar,left->index);
	tar=parse_field(tar,left->tail);

	r->tag=basetag;
	r->type=tar;
	offtag=saveoff;
	basetag=savebase;
	firstptr=saveptr;
	return r;
}

TYPE get_func(string name){
	VARINFO tar=get_info(top,name);
	if(tar==NULL||tar->type->kind!=args_TYPE)
		panic("unknown function");
	return tar->type;
}


int args_valid(TYPE func,parameters args){
	if(func==NULL&&args==NULL)
		return 1;
	else if(func==NULL)
		return 0;
	else if(args==NULL)
		return 0;
	expr_check(args->head);
	printf("\tparam t%d\n",args->head->tmp);
	TYPE oarg=func->u.args.head;
	TYPE carg=(TYPE)(args->head->thistype);
	if(!type_equal(oarg,carg)){
		if(!(oarg->kind==pointer_TYPE&&carg->kind==array_TYPE
				&&type_equal(carg->u.array.elem,oarg->u.subtype)))	
			return 0;
	}
	return args_valid(func->u.args.tail,args->tail);
}

void orr_check(expr tar){
	int l=label++,exprtmp=tar->tmp;

	expr_check(tar->u.biop.left);
	if(((TYPE)tar->u.biop.left->thistype)->kind!=int_TYPE)
		panic("nonbool beside a ||");
	printf("\tt%d = 1\n",exprtmp);
	printf("\tif t%d != 0 goto l%d\n",tar->u.biop.left->tmp,l);

	expr_check(tar->u.biop.right);
	if(((TYPE)tar->u.biop.right->thistype)->kind!=int_TYPE)
		panic("nonbool beside a ||");
	printf("\tt%d = t%d\n",exprtmp,tar->u.biop.right->tmp);

	tar->thistype=INT_TYPE;
	printf("l%d:\n",l);
}

void andd_check(expr tar){
	int l=label++,exprtmp=tar->tmp;

	expr_check(tar->u.biop.left);
	if(((TYPE)tar->u.biop.left->thistype)->kind!=int_TYPE)
		panic("nonbool beside a ||");
	printf("\tt%d = 0\n",exprtmp);
	printf("\tif t%d == 0 goto l%d\n",tar->u.biop.left->tmp,l);

	expr_check(tar->u.biop.right);
	if(((TYPE)tar->u.biop.right->thistype)->kind!=int_TYPE)
		panic("nonbool beside a ||");
	printf("\tt%d = t%d\n",exprtmp,tar->u.biop.right->tmp);

	tar->thistype=INT_TYPE;
	printf("l%d:\n",l);
}

TYPE type2TYPE(type tar);

void expr_check(expr tar){
	Access left;
	TYPE func;
	int exprtmp=tmp++;
	printf("\tvar t%d\n",exprtmp);
	tar->tmp=exprtmp;
	switch(tar->kind){
	case biop_expr:
		if(tar->u.biop.oper==orr){
			orr_check(tar);
			break;
		}
		else if(tar->u.biop.oper==andd){
			andd_check(tar);
			break;
		}
		expr_check(tar->u.biop.left);
		expr_check(tar->u.biop.right);
		biop_check(tar);
		break;
	case uniop_expr:
		expr_check(tar->u.uniop.right);
		tar->thistype=uniop_type(tar->u.uniop.oper,(TYPE)tar->u.uniop.right);
		printf("\tt%d = %s t%d\n",exprtmp,UNIOP[tar->u.uniop.oper],tar->u.uniop.right->tmp);
		break;
	case int_expr:
		tar->thistype=INT_TYPE;
		printf("\tt%d = %d\n",exprtmp,tar->u.intt);
		break;
	case float_expr:
		tar->thistype=FLOAT_TYPE;
		printf("\tt%d = %f\n",exprtmp,tar->u.floatt);
		break;
	case voidfunc_expr:
		func=get_func(tar->u.voidfunc);
		if(func->u.args.tail!=NULL)
			panic("args invalid");
		tar->thistype=func->u.args.head;
		printf("\tt%d = call f_%s\n",exprtmp,tar->u.voidfunc);
		break;
	case func_expr:
		func=get_func(tar->u.func.name);
		if(!args_valid(func->u.args.tail,tar->u.func.args))
			panic("args invalid");
		tar->thistype=func->u.args.head;
		printf("\tt%d = call f_%s\n",exprtmp,tar->u.voidfunc);
		break;
	case left_expr:
		left=get_access(tar->u.leftval);
		tar->thistype=left->type;
		//tar->u.leftval=(leftvalue)left;
		if(left->offset==-1){
				printf("\tt%d = %c%d\n",exprtmp,tag2c(left->tag),abstag(left->tag));
		}
		else{
			if(left->type->kind==int_TYPE||left->type->kind==pointer_TYPE) 
				printf("\tt%d = t%d [t%d]\n",exprtmp,left->tag,left->offset);
			else
				printf("\tt%d = t%d + t%d\n",exprtmp,left->tag,left->offset);
		}
		break;
	case size_expr:
		tar->thistype=INT_TYPE;
		printf("\tt%d = %d\n",exprtmp,type2TYPE(tar->u.size)->width);
		break;
	case cast_expr:
		tar->thistype=type2TYPE(tar->u.cast.t);
		expr_check(tar->u.cast.sub);
		printf("\tt%d = t%d\n",exprtmp,tar->u.cast.sub->tmp);
		break;
	}
}

void stats_check(stats tar);
void vardefn_check(vardefn tar);

void stat_check(stat tar){
	stable savetop;
	Access left;
	int l1,l2;
	switch(tar->kind){
	case stats_stat:
		savetop=top;
		top=table_new(top);
		stats_check(tar->u.statss);
		top=savetop;
		break;
	case ifthen_stat:
		l1=label++;
		expr_check(tar->u.ifthen.cond);
		printf("\tif t%d == 0 goto l%d\n",tar->u.ifthen.cond->tmp,l1);
		if(tar->u.ifthen.cond->thistype!=INT_TYPE)
			panic("nonbool in if");
		stat_check(tar->u.ifthen.act);
		printf("l%d:\n",l1);
		break;
	case ifelse_stat:
		l1=label++;l2=label++;
		expr_check(tar->u.ifelse.cond);
		printf("\tif t%d == 0 goto l%d\n",tar->u.ifelse.cond->tmp,l1);
		if(tar->u.ifelse.cond->thistype!=INT_TYPE)
			panic("nonbool in if");
		stat_check(tar->u.ifelse.act);
		printf("\tgoto l%d\n",l2);
		printf("l%d:\n",l1);
		stat_check(tar->u.ifelse.elseact);
		printf("\tl%d:\n",l2);
		break;
	case while_stat:
		l1=label++;l2=label++;
		enclosing=l2;
		printf("l%d:\n",l1);
		expr_check(tar->u.whilee.cond);
		printf("\tif t%d == 0 goto l%d\n",tar->u.whilee.cond->tmp,l2);
		if(tar->u.whilee.cond->thistype!=INT_TYPE)
			panic("nonbool in while");
		stat_check(tar->u.whilee.act);
		printf("\tgoto l%d\n",l1);
		printf("l%d:\n",l2);
		enclosing=-1;
		break;
	case assign_stat:
		left=get_access(tar->u.assign.left);
		expr_check(tar->u.assign.value);
		if(!assign_valid(left->type,(TYPE)(tar->u.assign.value->thistype)))
			panic("invalid assign");
		//tar->u.assign.left=(leftvalue)left;
		if(left->offset==-1){
				printf("\t%c%d = t%d\n",tag2c(left->tag),abstag(left->tag),tar->u.assign.value->tmp);
		}
		else{
			if(left->type->kind==int_TYPE||left->type->kind==pointer_TYPE) 
				printf("\tt%d [t%d] = t%d\n",left->tag,left->offset,tar->u.assign.value->tmp);
			else
				panic("invalid assign");
		}
		break;
	case vardefn_stat:
		vardefn_check(tar->u.vardefnn);
		break;
	case return_stat:
		expr_check(tar->u.returnn);
		TYPE ret=(TYPE)(tar->u.returnn->thistype);
		if(!type_equal(ret,funcret)){
			if(!(funcret->kind==pointer_TYPE&&ret->kind==array_TYPE
				&&type_equal(ret->u.array.elem,funcret->u.subtype)))
				panic("invalid return type");
		}
		printf("\treturn t%d\n",tar->u.returnn->tmp);
		break;
	case expr_stat:
		expr_check(tar->u.exprr);
		break;
	case break_stat:
		if(enclosing==-1)
			panic("break out any brace");
		printf("\tgoto l%d\n",enclosing);
		break;
	}
}

TYPE type_expand(TYPE tar,dimensions dims){
	if(dims==NULL)
		return tar;
	else
		return array_type(type_expand(tar,dims->tail),dims->dimen);
}

TYPE type2TYPE(type tar){
	TYPE tar_type;
	if(tar->kind==user_type){
		tar_type=get_record_type(types,tar->name);
		if(tar_type==NULL)
			panic("type undeclared");
	}
	else if(tar->kind==int_type)
		tar_type=INT_TYPE;
	else if(tar->kind==float_type)
		tar_type=FLOAT_TYPE;
	else if(tar->kind==pointer_type)
		tar_type=point_type(type2TYPE(tar->subtype));
	else panic("unknown type");
	return tar_type;
}

void vardefn_check(vardefn tar){
	TYPE tar_type;
	tar_type=type2TYPE(tar->thistype);
	tar_type=type_expand(tar_type,tar->dimens);
	add_var(top,tar->name,combinfo(tag++,offset,tar_type));
	offset+=tar_type->width;
	if(instruct) return;
	if(tar_type->kind==array_TYPE||tar_type->kind==record_TYPE)
		printf("\tvar %d T%d\n",tar_type->width,tag-1);
	else if(tar_type->kind==pointer_TYPE)
		printf("\tvar T%d\n",tag-1);
	else printf("\tvar T%d\n",tag-1);
}

void typevardefns_check(typevardefns tar);

void typedefn_check(typedefn tar){
	stable savetop=top,savetypes=types;
	int saveoff=offset;
	int savetag=tag;
	tag=0;
	offset=0;
	top=table_new(top);
	types=table_new(types);
	instruct++;
	TYPE thistype=record_type(tar->name,top,types,offset);
	add_type(savetypes,tar->name,thistype);

	typevardefns_check(tar->defns);

	refill_type(thistype,offset);
	instruct--;
	top=savetop;
	types=savetypes;
	offset=saveoff;
	tag=savetag;
}

void typevardefns_check(typevardefns tar){
	if(tar==NULL)
		return;
	if(tar->kind==vardefn_typevar)
		vardefn_check(tar->u.vardefnn);
	else typedefn_check(tar->u.typedefnn);
	typevardefns_check(tar->tail);
}

int push_args(vardecls args,int para){
	if(args==NULL)
		return 0;
	
	vardecl tar=args->head;
	TYPE tar_type;
	tar_type=type2TYPE(tar->thistype);
	if(tar->kind==multi_vardecl)
		tar_type=array_type(tar_type,0);
	add_var(top,tar->name,combinfo(-para-1,offset,tar_type));	//p0 -> -1, p1 -> -2, ...
	offset+=tar_type->width;

	return push_args(args->tail,para+1)+1;
}

void stats_check(stats tar){
	if(tar==NULL)
		return;
	stat_check(tar->head);
	stats_check(tar->tail);
}


TYPE func_args_type(vardecls tar){
	if(tar==NULL)
		return NULL;
	TYPE tail=func_args_type(tar->tail);
	TYPE head=type2TYPE(tar->head->thistype);
	if(tar->head->kind==multi_vardecl)
		head=array_type(head,0);
	return args_type(head,tail);
}

TYPE func_type(type ret,TYPE args){
	TYPE head=type2TYPE(ret);
	return args_type(head,args);
}

void funcdefn_check(funcdefn tar){
	TYPE tar_type=func_type(tar->thistype,func_args_type(tar->args));
	add_var(top,tar->name,combinfo(1,0,tar_type));
	stable savetop=top;
	top=table_new(top);
	int saveoff=offset;
	offset=0;
	funcret=tar_type->u.args.head;
	
	printf("f_%s [%d]\n",tar->name,push_args(tar->args,0));
	stats_check(tar->content);
	printf("end f_%s\n",tar->name);	

	funcret=NULL;
	top=savetop;
	offset=saveoff;
}

void funcdecl_check(funcdecl tar){
	TYPE tar_type=func_type(tar->thistype,func_args_type(tar->args));
	add_var(top,tar->name,combinfo(0,0,tar_type));
}

void def_check(def tar){
	if(tar==NULL)
		return;
	switch(tar->kind){
	case vardefn_def: vardefn_check(tar->u.vardefnn);break;
	case funcdefn_def: funcdefn_check(tar->u.funcdefnn);break;
	case typedefn_def: typedefn_check(tar->u.typedefnn);break;
	case funcdecl_def: funcdecl_check(tar->u.funcdecll);break;
	}
	def_check(tar->tail);
}

void goal_check(goal tar){
	def_check(tar->deff);
}

void semantic(void *tree){
	goal program=(goal)tree;

	top=table_new(NULL);
	types=table_new(NULL);

	INT_TYPE=(TYPE)checked_malloc(sizeof(*INT_TYPE));
	FLOAT_TYPE=(TYPE)checked_malloc(sizeof(*FLOAT_TYPE));
	INT_TYPE->kind=int_type;
	INT_TYPE->width=4;
	FLOAT_TYPE->kind=float_type;
	FLOAT_TYPE->width=4;

	goal_check(program);
}
