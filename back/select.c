#include "select.h"

string operator[]={"+","-","*","/","%","&&","||","<","==",">","<=",">=","!=","!"};
static int stksz;

void uni_varDecl(int var,int val){
	printf("\t.global\tv%d\n",var);
	printf("\t.section\t.sdata\n");
	printf("\t.align 2\n");
	printf("\t.type\tv%d, @object\n",var);
	printf("\t.size\tv%d, 4\n",var);
	printf("v%d:\n",var);
	printf("\t.word\t%d\n",val);
	printf("\n");
}

void multi_varDecl(int var,int size){
	printf("\t.comm\tv%d,%d,4\n",var,size*4);
}

void begin_func(string func,int argc,int size){
	stksz=(size/4+1)*16;
	printf("\t.text\n");
	printf("\t.align\t2\n");
	printf("\t.global\t%s\n",func);
	printf("\t.type\t%s, @function\n",func);
	printf("%s:\n",func);
	printf("\taddi\tsp,sp,-%d\n",stksz);
	printf("\tsw\tra,%d(sp)\n",stksz-4);
}

void end_func(string func){
	stksz=0;
	printf("\t.size\t%s, .-%s\n",func,func);
	printf("\n");
}

void bireg(string l,string r1,int oper,string r2){
	switch(oper){
	case plus:
		printf("\tadd\t%s,%s,%s\n",l,r1,r2);
		break;
	case minus:
		printf("\tsub\t%s,%s,%s\n",l,r1,r2);
		break;
	case divide:
		printf("\tdiv\t%s,%s,%s\n",l,r1,r2);
		break;
	case times:
		printf("\tmul\t%s,%s,%s\n",l,r1,r2);
		break;
	case mod:
		printf("\trem\t%s,%s,%s\n",l,r1,r2);
		break;
	case lt:
		printf("\tslt\t%s,%s,%s\n",l,r1,r2);
		break;
	case gt:
		printf("\tsgt\t%s,%s,%s\n",l,r1,r2);
		break;
	case eq:
		printf("\tsub\t%s,%s,%s\n",l,r1,r2);
		printf("\tseqz\t%s,%s\n",l,l);
		break;
	case ne:
		printf("\tsub\t%s,%s,%s\n",l,r1,r2);
		printf("\tsnez\t%s,%s\n",l,l);
		break;
	case andd:
		printf("\tseqz\t%s,%s\n",l,r1);
		printf("\taddi\t%s,%s,-1\n",l,l);
		printf("\tand\t%s,%s,%s\n",l,l,r2);
		break;
	case orr:
		printf("\tor\t%s,%s,%s\n",l,r1,r2);
		break;
	case le:
		printf("\tsub\t%s,%s,%s\n",l,r1,r2);
		printf("\tsgtz\t%s,%s\n",l,l);
		printf("\tnot\t%s,%s\n",l,l);
		break;
	case ge:
		printf("\tsub\t%s,%s,%s\n",l,r2,r1);
		printf("\tsgtz\t%s,%s\n",l,l);
		printf("\tnot\t%s,%s\n",l,l);
		break;
	default:
		fprintf(stderr,"invalid oper in bireg\n");
		exit(1);
	}
}

void regint(string l,string r1,int oper,int r2){
	if(oper==plus){
		printf("\taddi\t%s,%s,%d\n",l,r1,r2);
	}
	else if(oper==lt){
		printf("\tslti\t%s,%s,%d\n",l,r1,r2);
	}
	else{
		fprintf(stderr,"invalid oper in regint\n");
		exit(1);
	}
}

void unireg(string l,int oper,string r){
	if(oper==minus){
		printf("\tneg\t%s,%s\n",l,r);
	}
	else if(oper==nott){
		printf("\tnot\t%s,%s\n",l,r);
	}
	else{
		fprintf(stderr,"invalid oper in unireg\n");
		exit(1);
	}
}

void reg(string l,string r){
	printf("\tmv\t%s,%s\n",l,r);
}

void integer(string l,int r){
	printf("\tli\t%s,%d\n",l,r);
}

void larray(string l,int index,string r){
	printf("\tsw\t%s,%d(%s)\n",r,index,l);
}

void rarray(string l,string r,int index){
	printf("\tlw\t%s,%d(%s)\n",l,index,r);
}

void bif(string l,int oper,string r,int lab){
	switch(oper){
	case lt:
		printf("\tblt\t%s,%s,.l%d\n",l,r,lab);
		break;
	case gt:
		printf("\tblt\t%s,%s,.l%d\n",r,l,lab);
		break;
	case eq:
		printf("\tbeq\t%s,%s,.l%d\n",l,r,lab);
		break;
	case ne:
		printf("\tbne\t%s,%s,.l%d\n",l,r,lab);
		break;
	case le:
		printf("\tbge\t%s,%s,.l%d\n",r,l,lab);
		break;
	case ge:
		printf("\tbge\t%s,%s,.l%d\n",l,r,lab);
		break;
	default:
		fprintf(stderr,"invalid oper in if\n");
		exit(1);
	}
}

void gotol(int l){
	printf("\tj\t.l%d\n",l);
}

void label(int l){
	printf(".l%d:\n",l);
}

void call(string func){
	printf("\tcall\t%s\n",func);
}

void store(string reg,int stk){
	printf("\tsw\t%s,%d(sp)\n",reg,stk*4);
}

void loadstk(int stk,string reg){
	printf("\tlw\t%s,%d(sp)\n",reg,stk*4);
}

void loadglb(int glb,string reg){	
	printf("\tlw\t%s,v%d\n",reg,glb);
}

void addrstk(int stk,string reg){
	printf("\tadd\t%s,sp,%d\n",reg,stk*4);
}

void addrglb(int glb,string reg){
	printf("\tla\t%s,v%d\n",reg,glb);
}

void ret(){
	printf("\tlw\tra,%d(sp)\n",stksz-4);
	printf("\tadd\tsp,sp,%d\n",stksz);
	printf("\tjr ra\n");
}

