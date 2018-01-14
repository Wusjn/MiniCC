#include "eeyore.h"


extern FILE *yyin;
extern int yyparse(void);
extern int syntax_error;
extern interstat eeyore;
extern vartable labeltab;

extern int type_check(goal);
extern int codegen(goal);
extern int flowmap_create(interstat,vartable);
extern int live_analyse(interstat);
extern void register_alloc(interstat);
extern void tigger_eeyore(interstat);

goal program;
int filenum;
FILE **files;

int main(int argc,char **argv){
	string fname;
	int success;

	filenum=argc-1;
	files=(FILE **)checked_malloc(sizeof(FILE *)*filenum);

	//open file
	freopen("debug.txt","w",stderr);
	for(int i=0;i<filenum;i++){
		files[i]=fopen(argv[i+1],"r");
		if(!files[i]) {fprintf(stderr,"can not open file %s\n",argv[i+1]);exit(1);}
	}
	yyin=fopen("head.h","r");

	//parse
	success=yyparse();
	if(success==0&&syntax_error==0) fprintf(stderr,"parse succeed\n");
	else {fprintf(stderr,"parse failed\n");return 0;}

	//type check
	success=type_check(program);
	if(success==0) fprintf(stderr,"type check succeed\n");
	else {fprintf(stderr,"type check failed\n");return 0;}

	//code generate
	success=codegen(program);
	if(success==0) fprintf(stderr,"code generate succeed\n");
	else {fprintf(stderr,"code generate failed\n");return 0;}

	//flow graph
	success=flowmap_create(eeyore,labeltab);
	if(success==0) fprintf(stderr,"flow graph generate succeed\n");
	else {fprintf(stderr,"flow graph generate failed\n");return 0;}
	
	//liveness analysis
	success=live_analyse(eeyore);
	if(success==0) fprintf(stderr,"liveness analysis succeed\n");
	else {fprintf(stderr,"liveness analysis failed\n");return 0;}
	
	//register_allocation
	register_alloc(eeyore);

	//tigger generate
	tigger_eeyore(eeyore);

	return 0;
}





