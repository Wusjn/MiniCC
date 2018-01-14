#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nodes.h"

#define REGNUM 27
#define CALLERSAVE 15
#define CALLEESAVE 12
#define ARGS 8
#define VARTABSIZE 109

typedef struct interstat_ * interstat;
typedef struct rval_ * rval;
typedef struct funcinfo_ * funcinfo;
typedef struct vartable_ * vartable;
typedef struct varentry_ * varentry;
typedef struct livezone_ * livezone;

typedef struct bitmap_ * bitmap;
typedef struct flownode_ * flownode;

struct bitmap_{
	char *map;
	int size;
	int bitsize;
};

struct flownode_{
	enum {begin_flow,end_flow,inter_flow}kind;
	interstat begin,end;
	bitmap def,use;
	bitmap in,out,new_out;
	flownode succ[2];
	int visited;
};

struct varentry_{
	char type;
	int global,local;
	int array;
	varentry next;
};

struct vartable_{
	varentry table[VARTABSIZE];
	int localsum;
};

struct livezone_{
	int number;
	int begin;
	int end;
};

struct funcinfo_{
	int stacknum;
	int callnum;
	int selfnum;
	int regnum;
	int args;
	string func;
	int statnum;
	int varnum;
	string *vars;
	int *spec;
	int *is_array;
	int *regs;
	int *temp_regs;
	int reg_used[REGNUM];
	int *stack;
	vartable tab;
	interstat end;
	flownode graph;
	livezone *zones;
	livezone *ori_zones;
};

struct rval_{
	enum {var_rval,int_rval}kind;
	int value;
};

struct interstat_{
	enum {biop_inter,uniop_inter,trans_inter,larray_inter,
		rarray_inter,if_inter,goto_inter,label_inter,
		param_inter,call_inter,return_inter,decl_inter,
		begin_inter,end_inter}kind;
	interstat next,prev,nextblk;
	flownode basicblk;
	int def,use[3];
	int statnum;
	bitmap out;
	int num;
	int waste;
	union{
		struct {enum bioper oper;int var;rval r1,r2;} biop;
		struct {enum unioper oper;int var;rval r;} uniop;
		struct {int var;rval r;} trans;
		struct {int var;rval index,r;} larray;
		struct {int l,r;rval index;} rarray;
		struct {enum bioper oper;rval r1,r2;int label;} iff;
		int gotoo;
		int label;
		struct {rval param;string name;} param;
		struct {int var;string func;} call;
		rval returnn;
		struct {int len,var;} decl;
		funcinfo begin;
		string end;
	}u;
};

vartable vartab_create();

int vartab_find(vartable,char,int);

int bitmap_or(bitmap,bitmap);

int bitmap_minus(bitmap,bitmap);

int bitmap_equal(bitmap,bitmap);

int bitmap_set(bitmap,int);

int bitmap_reset(bitmap,int);

int bitmap_get(bitmap,int);

int bitmap_copy(bitmap,bitmap);

void bitmap_clear(bitmap);

void Biop_inter(interstat,char,int,char,int,enum bioper,char,int);

void Uniop_inter(interstat,char,int,enum unioper,char,int);

void Trans_inter(interstat,char,int,char,int);

void Rarray_inter(interstat,char,int,char,int,char,int);

void Call_inter(interstat,char,int,string);

void If_inter(interstat,char,int,enum bioper,char,int,int);

void Label_inter(vartable,interstat,int);

void Goto_inter(interstat,int);

void Larray_inter(interstat,char,int,char,int,char,int);

void Return_inter(interstat,char,int);

interstat Begin_inter(string,int);

void End_inter(vartable,interstat,string);

interstat Decl_inter(vartable,interstat,int,char,int);

void Param_inter(interstat,char,int);

void Args_inter(vartable,int);

void flow_clear_history(flownode);


