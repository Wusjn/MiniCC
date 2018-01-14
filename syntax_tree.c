#include "syntax_tree.h"
#include "syntax.tab.h"



expr Biop_expr(enum bioper x,expr y,expr z){
	expr r=checked_malloc(sizeof(*r));
	r->kind=biop_expr;
	r->u.biop.oper=x;
	r->u.biop.left=y;
	r->u.biop.right=z;
	return r;
}
expr Uniop_expr(enum unioper x,expr y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=uniop_expr;
	r->u.uniop.oper=x;
	r->u.uniop.right=y;
	return r;
}
expr Int_expr(int x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=int_expr;
	r->u.intt=x;
	return r;
}
expr Float_expr(float x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=float_expr;
	r->u.floatt=x;
	return r;
}
expr Left_expr(leftvalue x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=left_expr;
	r->u.leftval=x;
	return r;
}

expr Voidfunc_expr(string x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=voidfunc_expr;
	r->u.voidfunc=x;
	return r;
}
expr Func_expr(string x,parameters y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=func_expr;
	r->u.func.name=x;
	r->u.func.args=y;
	return r;
}
expr Size_expr(type x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=size_expr;
	r->u.size=x;
	return r;
}
expr Cast_expr(type x,expr y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=cast_expr;
	r->u.cast.t=x;
	r->u.cast.sub=y;
	return r;
}


stat Stats_stat(stats x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=stats_stat;
	r->u.statss=x;
	return r;
}
stat Ifthen_stat(expr x,stat y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=ifthen_stat;
	r->u.ifthen.cond=x;
	r->u.ifthen.act=y;
	return r;
}
stat Ifelse_stat(expr x,stat y,stat z){
	stat r=checked_malloc(sizeof(*r));
	r->kind=ifelse_stat;
	r->u.ifelse.cond=x;
	r->u.ifelse.act=y;
	r->u.ifelse.elseact=z;
	return r;
}
stat While_stat(expr x,stat y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=while_stat;
	r->u.whilee.cond=x;
	r->u.whilee.act=y;
	return r;
}
stat Assign_stat(leftvalue x,expr y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=assign_stat;
	r->u.assign.left=x;
	r->u.assign.value=y;
	return r;
}
stat Vardefn_stat(vardefn x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=vardefn_stat;
	r->u.vardefnn=x;
	return r;
}
stat Return_stat(expr x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=return_stat;
	r->u.returnn=x;
	return r;
}
stat Expr_stat(expr x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=expr_stat;
	r->u.exprr=x;
	return r;
}
stat Break_stat(){
	stat r=checked_malloc(sizeof(*r));
	r->kind=break_stat;
	return r;
}

stats Stats(stat x,stats y){
	stats r=checked_malloc(sizeof(*r));
	r->head=x;
	r->tail=y;
	return r;
}

parameters Parameters(expr x,parameters y){
	parameters r=checked_malloc(sizeof(*r));
	r->head=x;
	r->tail=y;
	return r;
}

indexes Indexes(expr x,indexes y){
	indexes r=checked_malloc(sizeof(*r));
	r->head=x;
	r->tail=y;
	return r;
}

leftvalue Leftvalue(string x,indexes y,leftvalue z){
	leftvalue r=checked_malloc(sizeof(*r));
	r->name=x;
	r->index=y;
	r->tail=z;
	return r;
}

type INT_type=NULL;
type FLOAT_type=NULL;

type Int_type(){
	if(INT_type==NULL){
		type r=checked_malloc(sizeof(*r));
		r->kind=int_type;
		INT_type=r;
	}
	return INT_type;
}
type Float_type(){
	if(FLOAT_type==NULL){
		type r=checked_malloc(sizeof(*r));
		r->kind=float_type;
		FLOAT_type=r;
	}
	return FLOAT_type;
}
type User_type(string x){
	type r=checked_malloc(sizeof(*r));
	r->kind=user_type;
	r->name=String(x);
	return r;
}
type Pointer_type(type x){
	type r=checked_malloc(sizeof(*r));
	r->kind=pointer_type;
	r->subtype=x;
	return r;
}


funcdefn Void_funcdefn(string x,stats y,type z){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=void_funcdefn;
	r->name=x;
	r->args=NULL;
	r->content=y;
	r->thistype=z;
	return r;
}
funcdefn Args_funcdefn(string x,vardecls y,stats z,type p){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=args_funcdefn;
	r->name=x;
	r->args=y;
	r->content=z;
	r->thistype=p;
	return r;
}

funcdecl Void_funcdecl(string x,type y){
	funcdecl r=checked_malloc(sizeof(*r));
	r->kind=void_funcdecl;
	r->name=x;
	r->args=NULL;
	r->thistype=y;
	return r;
}
funcdecl Args_funcdecl(string x,vardecls y,type z){
	funcdecl r=checked_malloc(sizeof(*r));
	r->kind=args_funcdecl;
	r->name=x;
	r->args=y;
	r->thistype=z;
	return r;
}

vardefn Vardefn(string x,type y,dimensions z){
	vardefn r=checked_malloc(sizeof(*r));
	r->name=x;
	r->thistype=y;
	r->dimens=z;
	return r;
}

dimensions Dimensions(int x,dimensions y){
	dimensions r=checked_malloc(sizeof(*r));
	r->dimen=x;
	r->tail=y;
	return r;
}

vardecl Uni_vardecl(string x,type y){
	vardecl r=checked_malloc(sizeof(*r));
	r->kind=uni_vardecl;
	r->name=x;
	r->thistype=y;
	return r;
}
vardecl Multi_vardecl(string x,type y){
	vardecl r=checked_malloc(sizeof(*r));
	r->kind=multi_vardecl;
	r->name=x;
	r->thistype=y;
	return r;
}

vardecls Vardecls(vardecl x,vardecls y){
	vardecls r=checked_malloc(sizeof(*r));
	r->head=x;
	r->tail=y;
	return r;
}

typedefn Typedefn(string x,typevardefns y){
	typedefn r=checked_malloc(sizeof(*r));
	r->name=x;
	r->defns=y;
	return r;
}

typevardefns Typedefn_typevar(typedefn x,typevardefns y){
	typevardefns r=checked_malloc(sizeof(*r));
	r->kind=typedefn_typevar;
	r->u.typedefnn=x;
	r->tail=y;
	return r;
}
typevardefns Vardefn_typevar(vardefn x,typevardefns y){
	typevardefns r=checked_malloc(sizeof(*r));
	r->kind=vardefn_typevar;
	r->u.vardefnn=x;
	r->tail=y;
	return r;
}

def Vardefn_def(vardefn x,def y){
	def r=checked_malloc(sizeof(*r));
	r->kind=vardefn_def;
	r->u.vardefnn=x;
	r->tail=y;
	return r;
}
def Typedefn_def(typedefn x,def y){
	def r=checked_malloc(sizeof(*r));
	r->kind=typedefn_def;
	r->u.typedefnn=x;
	r->tail=y;
	return r;
}
def Funcdefn_def(funcdefn x,def y){
	def r=checked_malloc(sizeof(*r));
	r->kind=funcdefn_def;
	r->u.funcdefnn=x;
	r->tail=y;
	return r;
}
def Funcdecl_def(funcdecl x,def y){
	def r=checked_malloc(sizeof(*r));
	r->kind=funcdecl_def;
	r->u.funcdecll=x;
	r->tail=y;
	return r;
}


goal Goal(def x){
	goal r=checked_malloc(sizeof(*r));
	r->deff=x;
	return r;
}
