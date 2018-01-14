%{
#include "syntax_tree.h"
#include "syntax.tab.h"
static int tokcharpos=1;
static int tokline=-4;
static int fileindex=0;
extern int filenum;
extern FILE **files;
int yywrap(void)
{
	if(fileindex<filenum){
		yyin=files[fileindex++];
		return 0;
	}
	else return 1;
}
void adjust(void)
{
	yylloc.first_line=tokline;
	yylloc.first_column=tokcharpos;
	tokcharpos+=yyleng;
	yylloc.last_line=tokline;
	yylloc.last_column=tokcharpos;
	//printf("%s\n",yytext);
}
void newline(void)
{
	tokline+=1;
	tokcharpos=1;
}

%}

char	[A-Z_a-z]
digit	[0-9]
ws	[ \t]

%%

"//"(.)*\n		{newline();}
{ws}+			{adjust();}
\n			{newline();}
";"			{adjust();return SEMI;}
"("			{adjust();return LPAREN;}
")"			{adjust();return RPAREN;}
"["			{adjust();return LBRACK;}
"]"			{adjust();return RBRACK;}
"{"			{adjust();return LBRACE;}
"}"			{adjust();return RBRACE;}
"."			{adjust();return DOT;}
","			{adjust();return COMMA;}
"+"			{adjust();return PLUS;}
"-"			{adjust();return MINUS;}
"*"			{adjust();return TIMES;}
"/"			{adjust();return DIVIDE;}
"%"			{adjust();return MOD;}
"||"			{adjust();return OR;}
"&&"			{adjust();return AND;}
"<"			{adjust();return LT;}
">"			{adjust();return GT;}
"<="			{adjust();return LE;}
">="			{adjust();return GE;}
"=="			{adjust();return EQ;}
"="			{adjust();return ASSIGN;}
"!="			{adjust();return NE;}
"!"			{adjust();return NOT;}
int			{adjust();return INT;}
float			{adjust();return FLOAT;}
if			{adjust();return IF;}
else			{adjust();return ELSE;}
while			{adjust();return WHILE;}
break			{adjust();return BREAK;}
return			{adjust();return RETURN;}
true			{adjust();yylval.ival=1;return INTEGER;}
false			{adjust();yylval.ival=0;return INTEGER;}
NULL			{adjust();yylval.ival=0;return INTEGER;}
struct			{adjust();return STRUCT;}
sizeof			{adjust();return SIZEOF;}
{digit}+		{adjust();yylval.ival=atoi(yytext);return INTEGER;}
{digit}+"."{digit}+	{adjust();yylval.fval=atof(yytext);return FLOATNUM;}
{char}({char}|{digit})*	{adjust();yylval.sval=String(yytext);return ID;}
.			{printf("scan error\n");exit(1);}

%%


