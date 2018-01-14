#include "eeyore.h"

static interstat curfunc;
interstat eeyore;
static vartable globaltab;
vartable labeltab;
static int tag=0;
static int label=0;
int label_sum;

static char expr_prefix;
static char expr_tag;

void stats_gen(stats);
void vardefn_gen(vardefn);

static string biopstring[]={"+","-","*","/","%","&&","||","<","==",">","!="};
static string uniopstring[]={"!","-"};

string op2string(int op,int biop){
	if(biop) return biopstring[op];
	else return uniopstring[op];
}

void get_fixtag(int tag,char *prefix,int *abstag){
	if(tag<0){
		*abstag=(-tag)-1;
		*prefix='p';
	}
	else{
		*abstag=tag;
		*prefix='T';
	}
}

void pushargs(identifiers tar){
	char prefix;
	int abstag;
	for(;tar;tar=tar->head){
		get_fixtag(tar->tail->tag,&prefix,&abstag);
		Param_inter(curfunc,prefix,abstag);
		printf("\tparam %c%d\n",prefix,abstag);
	}
}

void expr_gen(expr tar){
	char prefix;
	int abstag;

	tar->tag=tag++;
	Decl_inter(globaltab,curfunc,0,'t',tar->tag);
	printf("\tvar t%d\n",tar->tag);

	switch(tar->kind){
	case biop_expr:
		expr_gen(tar->u.biop.left);
		expr_gen(tar->u.biop.right);
		Biop_inter(curfunc,'t',tar->tag,'t',tar->u.biop.left->tag,
			tar->u.biop.oper,'t',tar->u.biop.right->tag);
		printf("\tt%d = t%d %s t%d\n",tar->tag,tar->u.biop.left->tag,
			op2string(tar->u.biop.oper,1),tar->u.biop.right->tag);
		break;
	case uniop_expr:
		expr_gen(tar->u.uniop.right);
		Uniop_inter(curfunc,'t',tar->tag,
			tar->u.uniop.oper,'t',tar->u.uniop.right->tag);
		printf("\tt%d = %s t%d\n",tar->tag,
			op2string(tar->u.uniop.oper,0),tar->u.uniop.right->tag);
		break;
	case int_expr:
		Trans_inter(curfunc,'t',tar->tag,'i',tar->u.intt);
		printf("\tt%d = %d\n",tar->tag,tar->u.intt);
		break;
	case id_expr:
		get_fixtag(tar->u.id->tag,&prefix,&abstag);
		Trans_inter(curfunc,'t',tar->tag,prefix,abstag);
		printf("\tt%d = %c%d\n",tar->tag,prefix,abstag);
		break;
	case array_expr:
		get_fixtag(tar->u.array.id->tag,&prefix,&abstag);
		expr_gen(tar->u.array.pos);
		Biop_inter(curfunc,'t',tar->u.array.pos->tag,
			't',tar->u.array.pos->tag,times,'i',4);
		Rarray_inter(curfunc,'t',tar->tag,prefix,abstag,
			't',tar->u.array.pos->tag);
		printf("\tt%d = t%d * 4\n",
			tar->u.array.pos->tag,tar->u.array.pos->tag);
		printf("\tt%d = %c%d [t%d]\n",tar->tag,
			prefix,abstag,tar->u.array.pos->tag);
		break;
	case voidfunc_expr:
		Call_inter(curfunc,'t',tar->tag,tar->u.voidfunc->id);
		printf("\tt%d = call f_%s\n",tar->tag,tar->u.voidfunc->id);
		break;
	case func_expr:
		pushargs(tar->u.func.args);
		Call_inter(curfunc,'t',tar->tag,tar->u.func.id->id);
		printf("\tt%d = call f_%s\n",tar->tag,tar->u.func.id->id);
		break;
	};
}



void stat_gen(stat tar){
	int prev;
	int reclabel1,reclabel2;
	char prefix;
	int abstag;
	switch(tar->kind){
	case void_stat: break;
	case stats_stat: stats_gen(tar->u.statss);break;
	case ifthen_stat:
		reclabel1=label++;
		expr_gen(tar->u.ifthen.cond);
		If_inter(curfunc,'t',tar->u.ifthen.cond->tag,eq,'i',0,reclabel1);
		printf("\tif t%d == 0 goto l%d\n",
			tar->u.ifthen.cond->tag,reclabel1);
		stat_gen(tar->u.ifthen.act);
		Label_inter(labeltab,curfunc,reclabel1);
		printf("l%d:\n",reclabel1);
		break;
	case ifelse_stat:
		reclabel1=label++;
		reclabel2=label++;
		expr_gen(tar->u.ifelse.cond);
		If_inter(curfunc,'t',tar->u.ifelse.cond->tag,eq,'i',0,reclabel1);
		printf("\tif t%d == 0 goto l%d\n",
			tar->u.ifelse.cond->tag,reclabel1);
		stat_gen(tar->u.ifelse.act);
		Goto_inter(curfunc,reclabel2);
		Label_inter(labeltab,curfunc,reclabel1);
		printf("\tgoto l%d\n",reclabel2);
		printf("l%d:\n",reclabel1);
		stat_gen(tar->u.ifelse.elseact);
		Label_inter(labeltab,curfunc,reclabel2);
		printf("l%d:\n",reclabel2);
		break;
	case while_stat:
		reclabel1=label++;
		reclabel2=label++;
		Label_inter(labeltab,curfunc,reclabel1);
		printf("l%d:\n",reclabel1);
		expr_gen(tar->u.whilee.cond);
		If_inter(curfunc,'t',tar->u.whilee.cond->tag,eq,'i',0,reclabel2);
		printf("\tif t%d == 0 goto l%d\n",
			tar->u.whilee.cond->tag,reclabel2);
		stat_gen(tar->u.whilee.act);
		Goto_inter(curfunc,reclabel1);
		Label_inter(labeltab,curfunc,reclabel2);
		printf("\tgoto l%d\n",reclabel1);
		printf("l%d:\n",reclabel2);
		break;
	case idassign_stat:
		get_fixtag(tar->u.idassign.id->tag,&prefix,&abstag);
		expr_gen(tar->u.idassign.value);
		Trans_inter(curfunc,prefix,abstag,'t',tar->u.idassign.value->tag);
		printf("\t%c%d = t%d\n",
			prefix,abstag,tar->u.idassign.value->tag);
		break;
	case aryassign_stat:
		get_fixtag(tar->u.aryassign.id->tag,&prefix,&abstag);
		expr_gen(tar->u.aryassign.value);
		expr_gen(tar->u.aryassign.pos);
		Biop_inter(curfunc,'t',tar->u.aryassign.pos->tag,
			't',tar->u.aryassign.pos->tag,times,'i',4);
		Larray_inter(curfunc,prefix,abstag,'t',tar->u.aryassign.pos->tag,
			't',tar->u.aryassign.value->tag);
		printf("\tt%d = t%d * 4\n",
			tar->u.aryassign.pos->tag,tar->u.aryassign.pos->tag);
		printf("\t%c%d [t%d] = t%d\n",prefix,abstag,
			tar->u.aryassign.pos->tag,tar->u.aryassign.value->tag);
		break;
	case vardefn_stat:
		vardefn_gen(tar->u.vardefnn);
		break;
	case return_stat:
		expr_gen(tar->u.returnn);
		Return_inter(curfunc,'t',tar->u.returnn->tag);
		printf("\treturn t%d\n",tar->u.returnn->tag);
		break;
	};
}

void stats_gen(stats tar){
	if(tar==NULL) return;
	stats_gen(tar->head);
	stat_gen(tar->tail);
}

void mainfunc_gen(mainfunc tar){
	//int prevtag=tag;
	curfunc->nextblk=Begin_inter("main",0);
	curfunc=curfunc->nextblk;
	Args_inter(curfunc->u.begin->tab,0);
	printf("\nf_main [0]\n");
	stats_gen(tar->content);
	End_inter(globaltab,curfunc,"main");
	printf("end f_main\n\n");
	//tag=prevtag;
}


void funcdefn_gen(funcdefn tar){
	//int prevtag=tag;
	curfunc->nextblk=Begin_inter(tar->id->id,tar->argc);
	curfunc=curfunc->nextblk;
	Args_inter(curfunc->u.begin->tab,tar->argc);
	printf("\nf_%s [%d]\n",tar->id->id,tar->argc);
	stats_gen(tar->content);
	End_inter(globaltab,curfunc,tar->id->id);
	printf("end f_%s\n\n",tar->id->id);
	//tag=prevtag;
}

void vardefn_gen(vardefn tar){
	interstat recinter;
	if(tar->kind==uni_vardefn){
		recinter=Decl_inter(globaltab,curfunc,0,'T',tar->id->tag);
		printf("\tvar T%d\n",tar->id->tag);
	}
	else{
		recinter=Decl_inter(globaltab,curfunc,tar->len*4,'T',tar->id->tag);
		printf("\tvar %d T%d\n",tar->len*4,tar->id->tag);
	}
	if(recinter!=NULL){
		curfunc->nextblk=recinter;
		curfunc=recinter;
	}
}

void def_gen(def tar){
	if(tar->kind==vardefn_def)
		vardefn_gen(tar->vardefnn);
	else if(tar->kind==funcdefn_def)
		funcdefn_gen(tar->funcdefnn);
}

void defs_gen(defs tar){
	if(tar->kind==multi_defs)
		defs_gen(tar->head);
	def_gen(tar->tail);
}

void goal_gen(goal tar){
	if(tar->kind==withdef_goal)
		defs_gen(tar->util);
	mainfunc_gen(tar->main);
}

int codegen(goal tar){
	freopen("a.e","w",stdout);
	eeyore=(interstat)checked_malloc(sizeof(*eeyore));
	eeyore->nextblk=NULL;
	eeyore->kind=end_inter;
	globaltab=vartab_create();
	labeltab=vartab_create();
	curfunc=eeyore;

	goal_gen(tar);
	label_sum=label;	

	return 0;
}
