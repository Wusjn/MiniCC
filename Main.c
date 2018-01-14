#include "utility.h"

extern FILE *yyin;
extern int yyparse(void);
extern void semantic(void *);
extern int noerror;

void *syntax_tree;
int filenum;
FILE **files;

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

int main(int argc,char **argv){
	string fname;
	int success;

	filenum=argc-1;
	files=(FILE **)checked_malloc(sizeof(FILE *)*filenum);

	//open file
	//freopen("debug.txt","w",stderr);
	for(int i=0;i<filenum;i++){
		files[i]=fopen(argv[i+1],"r");
		if(!files[i]) {fprintf(stderr,"can not open file %s\n",argv[i+1]);exit(1);}
	}
	yyin=fopen("head.h","r");

	//parse
	success=yyparse();
	if(success==0&&noerror) fprintf(stderr,"parse succeed\n");
	else {fprintf(stderr,"parse failed\n");return 0;}

	//semantics
	freopen("code.e","w",stdout);
	semantic(syntax_tree);
	fprintf(stderr,"semantic check succeed\n");

	return 0;
}





