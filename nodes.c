


#include "nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void *checked_malloc(int len)
{void *p = malloc(len);
 if (!p) {
    fprintf(stderr,"\nRan out of memory!\n");
    exit(1);
 }
 return p;
}

string String(char *s)
{string p = checked_malloc(strlen(s)+1);
 strcpy(p,s);
 return p;
}

identifier Identifier(string x){
	identifier r=checked_malloc(sizeof(*r));
	r->id=String(x);
	return r;
}

expr Biop_expr(enum bioper x,expr y,expr z){
	expr r=checked_malloc(sizeof(*r));
	r->kind=biop_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.biop.oper=x;
	r->u.biop.left=y;
	r->u.biop.right=z;
	return r;
}
expr Uniop_expr(enum unioper x,expr y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=uniop_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.uniop.oper=x;
	r->u.uniop.right=y;
	return r;
}
expr Int_expr(int x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=int_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.intt=x;
	return r;
}
expr Id_expr(identifier x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=id_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.id=x;
	return r;
}
expr Array_expr(identifier x,expr y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=array_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.array.id=x;
	r->u.array.pos=y;
	return r;
}
expr Voidfunc_expr(identifier x){
	expr r=checked_malloc(sizeof(*r));
	r->kind=voidfunc_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.voidfunc=x;
	return r;
}
expr Func_expr(identifier x,identifiers y){
	expr r=checked_malloc(sizeof(*r));
	r->kind=func_expr;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.func.id=x;
	r->u.func.args=y;
	return r;
}

stat Void_stat(){
	stat r=checked_malloc(sizeof(*r));
	r->kind=stats_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.statss=NULL;
	return r;
}

stat Stats_stat(stats x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=stats_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.statss=x;
	return r;
}
stat Ifthen_stat(expr x,stat y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=ifthen_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.ifthen.cond=x;
	r->u.ifthen.act=y;
	return r;
}
stat Ifelse_stat(expr x,stat y,stat z){
	stat r=checked_malloc(sizeof(*r));
	r->kind=ifelse_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.ifelse.cond=x;
	r->u.ifelse.act=y;
	r->u.ifelse.elseact=z;
	return r;
}
stat While_stat(expr x,stat y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=while_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.whilee.cond=x;
	r->u.whilee.act=y;
	return r;
}
stat Idassign_stat(identifier x,expr y){
	stat r=checked_malloc(sizeof(*r));
	r->kind=idassign_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.idassign.id=x;
	r->u.idassign.value=y;
	return r;
}
stat Aryassign_stat(identifier x,expr y,expr z){
	stat r=checked_malloc(sizeof(*r));
	r->kind=aryassign_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.aryassign.id=x;
	r->u.aryassign.pos=y;
	r->u.aryassign.value=z;
	return r;
}
stat Vardefn_stat(vardefn x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=vardefn_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.vardefnn=x;
	return r;
}
stat Return_stat(expr x){
	stat r=checked_malloc(sizeof(*r));
	r->kind=return_stat;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->u.returnn=x;
	return r;
}

stats Uni_stats(stat x){
	stats r=checked_malloc(sizeof(*r));
	r->kind=uni_stats;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->tail=x;
	r->head=NULL;
	return r;
}
stats Multi_stats(stats x,stat y){
	stats r=checked_malloc(sizeof(*r));
	r->kind=multi_stats;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=x;
	r->tail=y;
	return r;
}

identifiers Uni_identifiers(identifier x){
	identifiers r=checked_malloc(sizeof(*r));
	r->kind=uni_identifiers;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->tail=x;
	r->head=NULL;
	return r;
}
identifiers Multi_identifiers(identifiers x,identifier y){
	identifiers r=checked_malloc(sizeof(*r));
	r->kind=multi_identifiers;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=x;
	r->tail=y;
	return r;
}

type Int_type(){
	type r=checked_malloc(sizeof(*r));
	r->kind=int_type;
	r->pos.line=line;
	r->pos.charpos=charpos;
	return r;
}

mainfunc Empty_mainfunc(){
	mainfunc r=checked_malloc(sizeof(*r));
	r->kind=empty_mainfunc;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->content=NULL;
	return r;
}
mainfunc Content_mainfunc(stats x){
	mainfunc r=checked_malloc(sizeof(*r));
	r->kind=content_mainfunc;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->content=x;
	return r;
}

funcdefn Void_funcdefn(identifier x,stats y,type z){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=void_funcdefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=NULL;
	r->content=y;
	r->thistype=z;
	return r;
}
funcdefn Empty_funcdefn(identifier x,vardecls y,type z){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=empty_funcdefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=y;
	r->content=NULL;
	r->thistype=z;
	return r;
}
funcdefn Norm_funcdefn(identifier x,vardecls y,stats z,type p){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=norm_funcdefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=y;
	r->content=z;
	r->thistype=p;
	return r;
}
funcdefn None_funcdefn(identifier x,type y){
	funcdefn r=checked_malloc(sizeof(*r));
	r->kind=none_funcdefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=NULL;
	r->content=NULL;
	r->thistype=y;
	return r;
}

funcdecl Void_funcdecl(identifier x,type y){
	funcdecl r=checked_malloc(sizeof(*r));
	r->kind=void_funcdecl;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=NULL;
	r->thistype=y;
	return r;
}
funcdecl Norm_funcdecl(identifier x,vardecls y,type z){
	funcdecl r=checked_malloc(sizeof(*r));
	r->kind=norm_funcdecl;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->args=y;
	r->thistype=z;
	return r;
}

vardefn Uni_vardefn(identifier x,type y){
	vardefn r=checked_malloc(sizeof(*r));
	r->kind=uni_vardefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->len=0;
	r->thistype=y;
	return r;
}
vardefn Array_vardefn(identifier x,int y,type z){
	vardefn r=checked_malloc(sizeof(*r));
	r->kind=array_vardefn;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->len=y;
	r->thistype=z;
	return r;
}

vardecl Uni_vardecl(identifier x,type y){
	vardecl r=checked_malloc(sizeof(*r));
	r->kind=uni_vardecl;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->len=0;
	r->thistype=y;
	return r;
}
vardecl Fix_vardecl(identifier x,int y,type z){
	vardecl r=checked_malloc(sizeof(*r));
	r->kind=fix_vardecl;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->len=y;
	r->thistype=z;
	return r;
}
vardecl Unfix_vardecl(identifier x,type y){
	vardecl r=checked_malloc(sizeof(*r));
	r->kind=unfix_vardecl;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->id=x;
	r->len=0;
	r->thistype=y;
	return r;
}

vardecls Uni_vardecls(vardecl x){
	vardecls r=checked_malloc(sizeof(*r));
	r->kind=uni_vardecls;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=NULL;
	r->tail=x;
	return r;
}
vardecls Multi_vardecls(vardecls x,vardecl y){
	vardecls r=checked_malloc(sizeof(*r));
	r->kind=multi_vardecls;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=x;
	r->tail=y;
	return r;
}

def Vardefn_def(vardefn x){
	def r=checked_malloc(sizeof(*r));
	r->kind=vardefn_def;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->vardefnn=x;
	r->funcdefnn=NULL;
	r->funcdecll=NULL;
	return r;
}
def Funcdefn_def(funcdefn x){
	def r=checked_malloc(sizeof(*r));
	r->kind=funcdefn_def;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->vardefnn=NULL;
	r->funcdefnn=x;
	r->funcdecll=NULL;
	return r;
}
def Funcdecl_def(funcdecl x){
	def r=checked_malloc(sizeof(*r));
	r->kind=funcdecl_def;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->vardefnn=NULL;
	r->funcdefnn=NULL;
	r->funcdecll=x;
	return r;
}

defs Uni_defs(def x){
	defs r=checked_malloc(sizeof(*r));
	r->kind=uni_defs;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=NULL;
	r->tail=x;
	return r;
}
defs Multi_defs(defs x,def y){
	defs r=checked_malloc(sizeof(*r));
	r->kind=multi_defs;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->head=x;
	r->tail=y;
	return r;
}

goal Nodef_goal(mainfunc x){
	goal r=checked_malloc(sizeof(*r));
	r->kind=nodef_goal;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->util=NULL;
	r->main=x;
	return r;
}
goal Withdef_goal(defs x,mainfunc y){
	goal r=checked_malloc(sizeof(*r));
	r->kind=withdef_goal;
	r->pos.line=line;
	r->pos.charpos=charpos;
	r->util=x;
	r->main=y;
	return r;
}
