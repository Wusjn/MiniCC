%{
#include <stdio.h>
#include "nodes.h"
extern goal program;
int line;int charpos;int syntax_error;
int yylex(void); 
void errormsg(int errorline,string msg){
	syntax_error=1;
	printf("syntax error at line %d : %s\n",errorline,msg);
}
void yyerror(string msg){
}

#define ADJUST(x) line=x.first_line;charpos=x.first_column;
%}

%union  {
	int ival;
	string sval;
	goal ngoal;
	defs ndefs;
	def ndef;
	vardefn nvardefn;
	vardecl nvardecl;
	funcdefn nfuncdefn;
	funcdecl nfuncdecl;
	vardecls nvardecls;
	mainfunc nmainfunc;
	type ntype;
	stat nstat;
	stats nstats;
	expr nexpr;
	identifier nidentifier;
	identifiers nidentifiers;
	}

%token <sval> IDENTIFIER
%token <ival> INTERGER

%type <ngoal> Goal
%type <ndefs> Defs
%type <ndef> Def
%type <nvardefn> VarDefn
%type <nvardecl> VarDecl
%type <nfuncdefn> FuncDefn
%type <nfuncdecl> FuncDecl
%type <nvardecls> VarDecls
%type <nmainfunc> MainFunc
%type <ntype> Type
%type <nstat> Statement
%type <nstats> Statements
%type <nexpr> Expression
%type <nidentifier> Identifier
%type <nidentifiers> Identifiers

%token
	COMMA	SEMICOLON	LPAREN
	RPAREN	RBRACK	LBRACE	RBRACE
	ASSIGN	IF	ELSE	WHILE
	MAIN	RETURN	INT


%left	OR
%left	AND
%nonassoc	EQ	NEQ
%nonassoc	LT	GT
%left	PLUS	MINUS
%left	TIMES	DIVIDE	MOD
%left	NOT
%left	UMINUS
%left	LBRACK
%start	Goal

%%
Goal		:	MainFunc	{ADJUST(@$) $$=Nodef_goal($1); program=$$;}
		|	Defs	MainFunc	{ADJUST(@$) $$=Withdef_goal($1,$2); program=$$;}
		;

Defs		:	Defs	Def	{ADJUST(@$) $$=Multi_defs($1,$2);}
		|	Def	{ADJUST(@$) $$=Uni_defs($1);}
		;

Def		:	VarDefn		{ADJUST(@$) $$=Vardefn_def($1);}
		|	FuncDefn	{ADJUST(@$) $$=Funcdefn_def($1);}
		|	FuncDecl	{ADJUST(@$) $$=Funcdecl_def($1);}
		;

VarDefn		:	Type	Identifier	SEMICOLON	{ADJUST(@$) $$=Uni_vardefn($2,$1);}
		|	Type	Identifier	LBRACK	INTERGER	RBRACK	SEMICOLON	{ADJUST(@$) $$=Array_vardefn($2,$4,$1);}
		;

VarDecl		:	Type	Identifier	{ADJUST(@$) $$=Uni_vardecl($2,$1);}
		|	Type	Identifier	LBRACK	INTERGER	RBRACK	{ADJUST(@$) $$=Fix_vardecl($2,$4,$1);}
		|	Type	Identifier	LBRACK	RBRACK	{ADJUST(@$) $$=Unfix_vardecl($2,$1);}
		;

FuncDefn	:	Type	Identifier	LPAREN	VarDecls	RPAREN	LBRACE	Statements	RBRACE	{ADJUST(@$) $$=Norm_funcdefn($2,$4,$7,$1);}
		|	Type	Identifier	LPAREN	RPAREN	LBRACE	Statements	RBRACE	{ADJUST(@$) $$=Void_funcdefn($2,$6,$1);}
		|	Type	Identifier	LPAREN	VarDecls	RPAREN	LBRACE	RBRACE	{ADJUST(@$) $$=Empty_funcdefn($2,$4,$1);}
		|	Type	Identifier	LPAREN	RPAREN	LBRACE	RBRACE	{ADJUST(@$) $$=None_funcdefn($2,$1);}
		;

FuncDecl	:	Type	Identifier	LPAREN	VarDecls	RPAREN   SEMICOLON	{ADJUST(@$) $$=Norm_funcdecl($2,$4,$1);}
		|	Type	Identifier	LPAREN	RPAREN   SEMICOLON	{ADJUST(@$) $$=Void_funcdecl($2,$1);}
		;

VarDecls	:	VarDecls	COMMA	VarDecl	{ADJUST(@$) $$=Multi_vardecls($1,$3);}	
		|	VarDecl	{ADJUST(@$) $$=Uni_vardecls($1);}
		;

MainFunc	:	INT	MAIN	LPAREN	RPAREN	LBRACE	Statements	RBRACE	{ADJUST(@$) $$=Content_mainfunc($6);}
		|	INT	MAIN	LPAREN	RPAREN	LBRACE	RBRACE	{ADJUST(@$) $$=Empty_mainfunc();}
		;

Type		:	INT	{ADJUST(@$) $$=Int_type();}
		;

Statement	:	LBRACE	Statements	RBRACE	{ADJUST(@$) $$=Stats_stat($2);}
		|	LBRACE	RBRACE	{ADJUST(@$) $$=Void_stat();}
		|	IF	LPAREN	Expression	RPAREN	Statement	{ADJUST(@$) $$=Ifthen_stat($3,$5);}
		|	IF	LPAREN	Expression	RPAREN	Statement	ELSE	Statement	{ADJUST(@$) $$=Ifelse_stat($3,$5,$7);}
		|	WHILE		LPAREN	Expression	RPAREN	Statement	{ADJUST(@$) $$=While_stat($3,$5);}
		|	Identifier	ASSIGN	Expression	SEMICOLON	{ADJUST(@$) $$=Idassign_stat($1,$3);}
		|	Identifier	LBRACK	Expression	RBRACK	ASSIGN	Expression	SEMICOLON    {ADJUST(@$) $$=Aryassign_stat($1,$3,$6);}
		|	VarDefn	{ADJUST(@$) $$=Vardefn_stat($1);}
		|	RETURN	Expression	SEMICOLON	{ADJUST(@$) $$=Return_stat($2);}
		|	error	SEMICOLON	{errormsg(@$.first_line,"stat-recovered");}
		;

Statements	:	Statements	Statement	{ADJUST(@$) $$=Multi_stats($1,$2);}
		|	Statement	{ADJUST(@$) $$=Uni_stats($1);}
		;

Expression	:	Expression	PLUS	Expression	{ADJUST(@$) $$=Biop_expr(plus,$1,$3);}
		|	Expression	MINUS	Expression	{ADJUST(@$) $$=Biop_expr(minus,$1,$3);}
		|	Expression	TIMES	Expression	{ADJUST(@$) $$=Biop_expr(times,$1,$3);}
		|	Expression	DIVIDE	Expression	{ADJUST(@$) $$=Biop_expr(divide,$1,$3);}
		|	Expression	MOD	Expression	{ADJUST(@$) $$=Biop_expr(mod,$1,$3);}
		|	Expression	AND	Expression	{ADJUST(@$) $$=Biop_expr(andd,$1,$3);}
		|	Expression	OR	Expression	{ADJUST(@$) $$=Biop_expr(orr,$1,$3);}
		|	Expression	LT	Expression	{ADJUST(@$) $$=Biop_expr(lt,$1,$3);}
		|	Expression	EQ	Expression	{ADJUST(@$) $$=Biop_expr(eq,$1,$3);}
		|	Expression	GT	Expression	{ADJUST(@$) $$=Biop_expr(gt,$1,$3);}
		|	Expression	NEQ	Expression	{ADJUST(@$) $$=Biop_expr(neq,$1,$3);}
		|	MINUS		Expression	%prec UMINUS	{ADJUST(@$) $$=Uniop_expr(uminus,$2);}
		|	NOT		Expression	{ADJUST(@$) $$=Uniop_expr(nott,$2);}
		|	INTERGER	{ADJUST(@$) $$=Int_expr($1);}
		|	Identifier	{ADJUST(@$) $$=Id_expr($1);}
		|	Identifier	LBRACK	Expression	RBRACK	{ADJUST(@$) $$=Array_expr($1,$3);}
		|	LPAREN	Expression	RPAREN	{ADJUST(@$) $$=$2;}
		|	Identifier	LPAREN	RPAREN	{ADJUST(@$) $$=Voidfunc_expr($1);}
		|	Identifier	LPAREN	Identifiers	RPAREN	{ADJUST(@$) $$=Func_expr($1,$3);}
		;

Identifiers	:	Identifiers	COMMA	Identifier	{ADJUST(@$) $$=Multi_identifiers($1,$3);}
		|	Identifier	{ADJUST(@$) $$=Uni_identifiers($1);}
		;

Identifier	:	IDENTIFIER	{ADJUST(@$) $$=Identifier($1);}
		;

%%





