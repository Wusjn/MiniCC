%{
#include "syntax_tree.h"
extern void *syntax_tree;
int noerror=1;
int yylex(void); 
void yyerror(string msg){
}
void errmsg(int line,string msg){
	fprintf(stderr,"syntax error at line %d : %s\n",line,msg);
}
%}

%union  {
	int ival;
	float fval;
	string sval;
	goal goall;
	def deff;
	typedefn typedefnn;
	vardefn vardefnn;
	dimensions dimensionss;
	typevardefns typevardefnss;
	vardecl vardecll;
	funcdefn funcdefnn;
	funcdecl funcdecll;
	vardecls vardeclss;
	type typee;
	stat statt;
	stats statss;
	expr exprr;
	indexes indexess;
	parameters parameterss;
	leftvalue leftvaluee;
	}

%token <sval> ID
%token <ival> INTEGER
%token <fval> FLOATNUM

%type <goall> Goal
%type <deff> Def
%type <typedefnn> TypeDefn
%type <vardefnn> VarDefn
%type <dimensionss> Dimensions
%type <typevardefnss> TypeVarDefns
%type <vardecll> VarDecl
%type <funcdefnn> FuncDefn
%type <funcdecll> FuncDecl
%type <vardeclss> VarDecls
%type <typee> Type
%type <statt> Statement
%type <statss> Statements
%type <exprr> Expression
%type <indexess> Indexes
%type <parameterss> Parameters
%type <leftvaluee> LeftValue



%token
	COMMA	SEMI	LPAREN RPAREN
	RBRACK	LBRACE	RBRACE ASSIGN
	IF	ELSE	WHILE	BREAK
	RETURN	INT	FLOAT	STRUCT
	SIZEOF


%left	OR
%left	AND
%nonassoc	EQ	NE
%nonassoc	LT	GT	LE	GE
%left	PLUS	MINUS
%left	TIMES	DIVIDE	MOD
%left	NOT
%left	UMINUS
%left	LBRACK
%left	DOT
%start	Goal

%%

Goal		:	Def	{$$=Goal($1);syntax_tree=$$;}
		;

Def		:	TypeDefn Def	{$$=Typedefn_def($1,$2);}
		|	VarDefn Def	{$$=Vardefn_def($1,$2);}
		|	FuncDefn Def	{$$=Funcdefn_def($1,$2);}
		|	FuncDecl Def	{$$=Funcdecl_def($1,$2);}
		|	/* empty */	{$$=NULL;}
		;

TypeDefn	:	STRUCT ID LBRACE TypeVarDefns RBRACE	{$$=Typedefn($2,$4);}
		;

VarDefn		:	Type ID Dimensions SEMI	{$$=Vardefn($2,$1,$3);}
		;

TypeVarDefns	:	VarDefn TypeVarDefns	{$$=Vardefn_typevar($1,$2);}
		|	TypeDefn TypeVarDefns	{$$=Typedefn_typevar($1,$2);}
		|	/* empty */	{$$=NULL;}
		;

Dimensions	:	LBRACK INTEGER RBRACK Dimensions	{$$=Dimensions($2,$4);}
		|	/* empty */	{$$=NULL;}
		;

VarDecl		:	Type ID	{$$=Uni_vardecl($2,$1);}
		|	Type ID LBRACK RBRACK	{$$=Multi_vardecl($2,$1);}
		;

VarDecls	:	VarDecl COMMA VarDecls	{$$=Vardecls($1,$3);}
		|	VarDecl	{$$=Vardecls($1,NULL);}
		;

FuncDefn	:	Type ID LPAREN VarDecls RPAREN LBRACE Statements RBRACE	{$$=Args_funcdefn($2,$4,$7,$1);}
		|	Type ID LPAREN RPAREN LBRACE Statements RBRACE	{$$=Void_funcdefn($2,$6,$1);}
		;

FuncDecl	:	Type ID LPAREN VarDecls RPAREN SEMI	{$$=Args_funcdecl($2,$4,$1);}
		|	Type ID LPAREN RPAREN SEMI	{$$=Void_funcdecl($2,$1);}
		;

Statements	:	Statement Statements	{$$=Stats($1,$2);}
		|	/* empty */	{$$=NULL;}
		;

Indexes		:	LBRACK Expression RBRACK Indexes	{$$=Indexes($2,$4);}
		|	/* empty */	{$$=NULL;}
		;

LeftValue	:	ID Indexes DOT LeftValue	{$$=Leftvalue($1,$2,$4);}
		|	ID Indexes	{$$=Leftvalue($1,$2,NULL);}
		;

Statement	:	LBRACE Statements RBRACE	{$$=Stats_stat($2);}
		|	IF LPAREN Expression RPAREN Statement	{$$=Ifthen_stat($3,$5);}
		|	IF LPAREN Expression RPAREN Statement ELSE Statement	{$$=Ifelse_stat($3,$5,$7);}
		|	WHILE LPAREN Expression	RPAREN Statement	{$$=While_stat($3,$5);}
		|	LeftValue ASSIGN Expression SEMI	{$$=Assign_stat($1,$3);}
		|	VarDefn	{$$=Vardefn_stat($1);}
		|	Expression SEMI	{$$=Expr_stat($1);}
		|	RETURN Expression SEMI	{$$=Return_stat($2);}
		|	BREAK SEMI	{$$=Break_stat();}
		|	error	SEMI	{errmsg(@$.first_line,"statement-recovered");noerror=0;}
		;

Parameters	:	Expression COMMA Parameters	{$$=Parameters($1,$3);}
		|	Expression	{$$=Parameters($1,NULL);}
		;

Expression	:	Expression PLUS Expression	{$$=Biop_expr(plus,$1,$3);}
		|	Expression MINUS Expression	{$$=Biop_expr(minus,$1,$3);}
		|	Expression TIMES Expression	{$$=Biop_expr(times,$1,$3);}
		|	Expression DIVIDE Expression	{$$=Biop_expr(divide,$1,$3);}
		|	Expression MOD Expression	{$$=Biop_expr(mod,$1,$3);}
		|	Expression AND Expression	{$$=Biop_expr(andd,$1,$3);}
		|	Expression OR Expression	{$$=Biop_expr(orr,$1,$3);}
		|	Expression LT Expression	{$$=Biop_expr(lt,$1,$3);}
		|	Expression GT Expression	{$$=Biop_expr(gt,$1,$3);}
		|	Expression EQ Expression	{$$=Biop_expr(eq,$1,$3);}
		|	Expression NE Expression	{$$=Biop_expr(ne,$1,$3);}
		|	Expression LE Expression	{$$=Biop_expr(le,$1,$3);}
		|	Expression GE Expression	{$$=Biop_expr(ge,$1,$3);}
		|	NOT Expression	{$$=Uniop_expr(nott,$2);}
		|	MINUS Expression	%prec UMINUS	{$$=Uniop_expr(uminus,$2);}
		|	LeftValue	{$$=Left_expr($1);}
		|	INTEGER	{$$=Int_expr($1);}
		|	FLOATNUM	{$$=Float_expr($1);}
		|	ID LPAREN Parameters RPAREN	{$$=Func_expr($1,$3);}
		|	ID LPAREN RPAREN	{$$=Voidfunc_expr($1);}
		|	LPAREN Expression RPAREN	{$$=$2;}
		|	SIZEOF LPAREN Type RPAREN	{$$=Size_expr($3);}
		|	LPAREN Type RPAREN LPAREN Expression RPAREN	{$$=Cast_expr($2,$5);}
		;

Type		:	INT	{$$=Int_type();}
		|	FLOAT	{$$=Float_type();}
		|	STRUCT ID	{$$=User_type($2);}
		|	Type TIMES	{$$=Pointer_type($1);}
		;


%%
