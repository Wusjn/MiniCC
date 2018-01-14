#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum oper {plus,minus,times,divide,mod,andd,orr,lt,eq,gt,le,ge,ne,nott};
typedef char * string;
void *checked_malloc(int len);
string String(char *s);
