%{
#include "nodes.h"
#include "gramar.tab.h"
int tokcharpos=1;
int tokline=1;
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

char	[A-Za-z]
digit	[0-9]
ws	[ \t]

%%

"//"(.)*\n		{newline();}
{ws}+			{adjust();}
\n			{newline();}
";"			{adjust();return SEMICOLON;}
"("			{adjust();return LPAREN;}
")"			{adjust();return RPAREN;}
"["			{adjust();return LBRACK;}
"]"			{adjust();return RBRACK;}
"{"			{adjust();return LBRACE;}
"}"			{adjust();return RBRACE;}
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
"=="			{adjust();return EQ;}
"="			{adjust();return ASSIGN;}
"!="			{adjust();return NEQ;}
"!"			{adjust();return NOT;}
int			{adjust();return INT;}
main			{adjust();return MAIN;}
if			{adjust();return IF;}
else			{adjust();return ELSE;}
while			{adjust();return WHILE;}
return			{adjust();return RETURN;}
{digit}+		{adjust();yylval.ival=atoi(yytext);return INTERGER;}
{char}({char}|{digit})*	{adjust();yylval.sval=String(yytext);return IDENTIFIER;}
.			{printf("scan error\n");exit(1);}

%%


