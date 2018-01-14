%{
#include "select.h"
void *checked_malloc(int len){
	void *p = malloc(len);
	if (!p) {
		fprintf(stderr,"\nRan out of memory!\n");
		exit(1);
	}
	return p;
}
string String(char *s){
	string p = checked_malloc(strlen(s)+1);
	strcpy(p,s);
	return p;
}
extern FILE *yyin;
int yylex(void); 
void yyerror(string msg){
}
%}

%union{
	int ival;
	string sval;
}

%token <ival> LABEL;
%token <ival> VAR;
%token <ival> OPER;
%token <ival> INTEGER;
%token <sval> REG;
%token <sval> FUNC;

%token
	COLON	LBRACK	RBRACK	ASSIGN	RETURN
	CALL	IF	GOTO	END	LOAD	
	STORE	MALLOC	LOADADDR

%%

Goal		:	FuncDecl Goal
		|	VarDecl Goal
		|	/* empty */
		;

VarDecl		:	VAR ASSIGN INTEGER	{uni_varDecl($1,$3);}
		|	VAR ASSIGN MALLOC INTEGER	{multi_varDecl($1,$4);}
		;

FuncDecl	:	FUNC LBRACK INTEGER RBRACK LBRACK INTEGER RBRACK {begin_func($1,$3,$6);} Exprs END FUNC	{end_func($1);}
		;

Exprs		:	Expr Exprs
		|	/* empty */
		;

Expr		:	REG ASSIGN REG OPER REG		{bireg($1,$3,$4,$5);}
		|	REG ASSIGN REG OPER INTEGER	{regint($1,$3,$4,$5);}
		|	REG ASSIGN OPER REG		{unireg($1,$3,$4);}
		|	REG ASSIGN REG			{reg($1,$3);}
		|	REG ASSIGN INTEGER		{integer($1,$3);}
		|	REG LBRACK INTEGER RBRACK ASSIGN REG	{larray($1,$3,$6);}
		|	REG ASSIGN REG LBRACK INTEGER RBRACK	{rarray($1,$3,$5);}
		|	IF REG OPER REG GOTO LABEL		{bif($2,$3,$4,$6);}
		|	GOTO LABEL		{gotol($2);}
		|	LABEL COLON		{label($1);}
		|	CALL FUNC		{call($2);}
		|	STORE REG INTEGER	{store($2,$3);}
		|	LOAD INTEGER REG	{loadstk($2,$3);}
		|	LOAD VAR REG		{loadglb($2,$3);}
		|	LOADADDR INTEGER REG	{addrstk($2,$3);}
		|	LOADADDR VAR REG	{addrglb($2,$3);}
		|	RETURN			{ret();}
		;

%%

int main(int argc,char *argv[]){
	if(argc!=3){
		fprintf(stderr,"argc != 3\n");
		return 0;
	}
	yyin=fopen(argv[1],"r");
	freopen(argv[2],"w",stdout);
	yyparse();
	freopen("/dev/tty","w",stdout);
	return 0;
}
