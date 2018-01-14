#include "utility.h"

typedef struct goal_ * goal;
typedef struct def_ * def;
typedef struct typedefn_ * typedefn;
typedef struct vardefn_ * vardefn;
typedef struct dimensions_ * dimensions;
typedef struct typevardefns_ * typevardefns;
typedef struct vardecl_ * vardecl;
typedef struct funcdefn_ * funcdefn;
typedef struct funcdecl_ * funcdecl;
typedef struct vardecls_ * vardecls;
typedef struct type_ * type;
typedef struct stat_ * stat;
typedef struct stats_ * stats;
typedef struct expr_ * expr;
typedef struct indexes_ * indexes;
typedef struct parameters_ * parameters;
typedef struct leftvalue_ * leftvalue;

enum bioper {plus,minus,times,divide,mod,andd,orr,lt,eq,gt,le,ge,ne};
enum unioper {nott,uminus};

struct position{
	int first_line;
	int first_column;
	int last_line;
	int last_column;
};



expr Biop_expr(enum bioper x,expr y,expr z);
expr Uniop_expr(enum unioper x,expr y);
expr Int_expr(int x);
expr Float_expr(float x);
expr Left_expr(leftvalue x);
expr Voidfunc_expr(string x);
expr Func_expr(string x,parameters y);
expr Cast_expr(type x,expr y);
expr Size_expr(type x);

stat Stats_stat(stats x);
stat Ifthen_stat(expr x,stat y);
stat Ifelse_stat(expr x,stat y,stat z);
stat While_stat(expr x,stat y);
stat Assign_stat(leftvalue x,expr y);
stat Vardefn_stat(vardefn x);
stat Return_stat(expr x);
stat Expr_stat(expr x);
stat Break_stat();

stats Stats(stat x,stats y);

parameters Parameters(expr x,parameters y);

indexes Indexes(expr x,indexes y);

leftvalue Leftvalue(string x,indexes y,leftvalue z);

type Int_type();
type Float_type();
type User_type(string x);
type Pointer_type(type x);

funcdefn Void_funcdefn(string x,stats y,type z);
funcdefn Args_funcdefn(string x,vardecls y,stats z,type p);

funcdecl Void_funcdecl(string x,type y);
funcdecl Args_funcdecl(string x,vardecls y,type z);

vardefn Vardefn(string x,type y,dimensions z);

dimensions Dimensions(int x,dimensions y);

vardecl Uni_vardecl(string x,type y);
vardecl Multi_vardecl(string x,type y);

vardecls Vardecls(vardecl x,vardecls y);

typedefn Typedefn(string x,typevardefns y);

typevardefns Typedefn_typevar(typedefn x,typevardefns y);
typevardefns Vardefn_typevar(vardefn x,typevardefns y);

def Vardefn_def(vardefn x,def y);
def Typedefn_def(typedefn x,def y);
def Funcdefn_def(funcdefn x,def y);
def Funcdecl_def(funcdecl x,def y);

goal Goal(def x);

struct expr_
	{
	enum{biop_expr,uniop_expr,int_expr,float_expr,
		voidfunc_expr,func_expr,left_expr,size_expr,cast_expr}kind;
	struct position pos;
	void *thistype;
	int tmp;
	union{
		int intt;
		float floatt;
		leftvalue leftval;
		struct{enum bioper oper;expr left;expr right;} biop;
		struct{enum unioper oper;expr right;} uniop;
		struct{string name;parameters args;} func;
		string voidfunc;
		type size;
		struct{type t;expr sub;} cast;
		}u;
	};

struct parameters_
	{
	struct position pos;
	expr head;
	parameters tail;
	};

struct stat_
	{
	enum{stats_stat,ifthen_stat,ifelse_stat,while_stat,
		assign_stat,vardefn_stat,return_stat,expr_stat,break_stat}kind;
	struct position pos;
	union{
		stats statss;
		vardefn vardefnn;
		expr returnn;
		expr exprr;
		struct{expr cond;stat act;} ifthen;
		struct{expr cond;stat act;stat elseact;} ifelse;
		struct{expr cond;stat act;} whilee;
		struct{leftvalue left;expr value;} assign;
		}u;
	};

struct indexes_
	{
	struct position pos;
	expr head;
	indexes tail;
	};

struct leftvalue_
	{
	struct position pos;
	string name;
	indexes index;
	leftvalue tail;
	};

struct stats_
	{
	struct position pos;
	stat head;
	stats tail;
	};

struct type_{
	enum{int_type,float_type,user_type,pointer_type}kind;
	struct position pos;
	string name;
	type subtype;
	};

struct funcdefn_
	{
	enum{void_funcdefn,args_funcdefn}kind;
	struct position pos;
	type thistype;
	string name;
	vardecls args;
	stats content;
	};

struct funcdecl_
	{
	enum{void_funcdecl,args_funcdecl}kind;
	struct position pos;
	type thistype;
	string name;
	vardecls args;
	};

struct vardefn_
	{
	struct position pos;
	type thistype;
	string name;
	dimensions dimens;
	};

struct dimensions_
	{
	struct position pos;
	int dimen;
	dimensions tail;
	};

struct vardecl_
	{
	enum{uni_vardecl,multi_vardecl}kind;
	struct position pos;
	type thistype;
	string name;
	};

struct vardecls_
	{
	struct position pos;
	vardecl head;
	vardecls tail;
	};

struct typedefn_
	{
	string name;
	typevardefns defns;
	};

struct typevardefns_
	{
	enum{vardefn_typevar,typedefn_typevar}kind;
	struct position pos;
	union{
		vardefn vardefnn;
		typedefn typedefnn;
	}u;
	typevardefns tail;
	};

struct def_
	{
	enum{vardefn_def,funcdefn_def,funcdecl_def,typedefn_def}kind;
	struct position pos;
	union{
		vardefn vardefnn;
		funcdefn funcdefnn;
		funcdecl funcdecll;
		typedefn typedefnn;
	}u;
	def tail;
	};


struct goal_
	{
	struct position pos;
	def deff;
	};
