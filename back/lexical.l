%{
#include "utility.h"
#include "translate.tab.h"
int yywrap(void){
	return 1;
}
%}

char	[A-Z_a-z]
digit	[0-9]
ws	[ \t]

%%

"//"(.)*\n			{}
{ws}+				{}
\n				{}
":"				{return COLON;}
"["				{return LBRACK;}
"]"				{return RBRACK;}
"+"				{yylval.ival=plus;return OPER;}
"-"				{yylval.ival=minus;return OPER;}
"*"				{yylval.ival=times;return OPER;}
"/"				{yylval.ival=divide;return OPER;}
"%"				{yylval.ival=mod;return OPER;}
"||"				{yylval.ival=orr;return OPER;}
"&&"				{yylval.ival=andd;return OPER;}
"<"				{yylval.ival=lt;return OPER;}
">"				{yylval.ival=gt;return OPER;}
"<="				{yylval.ival=le;return OPER;}
">="				{yylval.ival=ge;return OPER;}
"=="				{yylval.ival=eq;return OPER;}
"="				{return ASSIGN;}
"!="				{yylval.ival=ne;return OPER;}
"!"				{yylval.ival=nott;return OPER;}
return				{return RETURN;}
l{digit}+			{yylval.ival=atoi(yytext+1);return LABEL;}
v{digit}+			{yylval.ival=atoi(yytext+1);return VAR;}
call				{return CALL;}
if				{return IF;}
goto				{return GOTO;}
end				{return END;}
load				{return LOAD;}
store				{return STORE;}
loadaddr			{return LOADADDR;}
malloc				{return MALLOC;}
x0				{yylval.sval=String("zero");return REG;}
s([0-9]|10|11)			{yylval.sval=String(yytext);return REG;}
a[0-7]				{yylval.sval=String(yytext);return REG;}
t[0-6]				{yylval.sval=String(yytext);return REG;}
{digit}+			{yylval.ival=atoi(yytext);return INTEGER;}
f_{char}({char}|{digit})*	{yylval.sval=String(yytext+2);return FUNC;}
.				{printf("scan error\n");exit(1);}

%%

