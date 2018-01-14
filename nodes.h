


typedef char * string;


void *checked_malloc(int len);
string String(char *s);

extern int line;
extern int charpos;

typedef struct goal_ * goal;
typedef struct defs_ * defs;
typedef struct def_ * def;
typedef struct vardefn_ * vardefn;
typedef struct vardecl_ * vardecl;
typedef struct funcdefn_ * funcdefn;
typedef struct funcdecl_ * funcdecl;
typedef struct vardecls_ * vardecls;
typedef struct mainfunc_ * mainfunc;
typedef struct type_ * type;
typedef struct stat_ * stat;
typedef struct stats_ * stats;
typedef struct expr_ * expr;
typedef struct identifier_ * identifier;
typedef struct identifiers_ * identifiers;

enum bioper {plus,minus,times,divide,mod,andd,orr,lt,eq,gt,neq};
enum unioper {nott,uminus};

struct position{int line;int charpos;};



expr Biop_expr(enum bioper,expr,expr);
expr Uniop_expr(enum unioper,expr);
expr Int_expr(int);
expr Id_expr(identifier);
expr Array_expr(identifier,expr);
expr Voidfunc_expr(identifier);
expr Func_expr(identifier,identifiers);

stat Void_stat();
stat Stats_stat(stats);
stat Ifthen_stat(expr,stat);
stat Ifelse_stat(expr,stat,stat);
stat While_stat(expr,stat);
stat Idassign_stat(identifier,expr);
stat Aryassign_stat(identifier,expr,expr);
stat Vardefn_stat(vardefn);
stat Return_stat(expr);

stats Uni_stats(stat);
stats Multi_stats(stats,stat);

identifiers Uni_identifiers(identifier);
identifiers Multi_identifiers(identifiers,identifier);

type Int_type();

mainfunc Empty_mainfunc();
mainfunc Content_mainfunc(stats);

funcdefn Void_funcdefn(identifier,stats,type);
funcdefn Empty_funcdefn(identifier,vardecls,type);
funcdefn Norm_funcdefn(identifier,vardecls,stats,type);
funcdefn None_funcdefn(identifier,type);

funcdecl Void_funcdecl(identifier,type);
funcdecl Norm_funcdecl(identifier,vardecls,type);

vardefn Uni_vardefn(identifier,type);
vardefn Array_vardefn(identifier,int,type);

vardecl Uni_vardecl(identifier,type);
vardecl Fix_vardecl(identifier,int,type);
vardecl Unfix_vardecl(identifier,type);

vardecls Uni_vardecls(vardecl);
vardecls Multi_vardecls(vardecls,vardecl);

def Vardefn_def(vardefn);
def Funcdefn_def(funcdefn);
def Funcdecl_def(funcdecl);

defs Uni_defs(def);
defs Multi_defs(defs,def);

goal Nodef_goal(mainfunc);
goal Withdef_goal(defs,mainfunc);

identifier Identifier(string);

struct identifier_{
	string id;
	int tag;
};

struct expr_
	{
	enum{biop_expr,uniop_expr,int_expr,id_expr,
		array_expr,voidfunc_expr,func_expr}kind;
	struct position pos;
	int tag;
	union{
		int intt;
		identifier id;
		struct{identifier id;expr pos;} array;
		struct{enum bioper oper;expr left;expr right;} biop;
		struct{enum unioper oper;expr right;} uniop;
		struct{identifier id;identifiers args;} func;
		identifier voidfunc;
		}u;
	};

struct stat_
	{
	enum{void_stat,stats_stat,ifthen_stat,ifelse_stat,while_stat,
		idassign_stat,aryassign_stat,vardefn_stat,return_stat}kind;
	struct position pos;
	union{
		stats statss;
		vardefn vardefnn;
		expr returnn;
		struct{expr cond;stat act;} ifthen;
		struct{expr cond;stat act;stat elseact;} ifelse;
		struct{expr cond;stat act;} whilee;
		struct{identifier id;expr value;} idassign;
		struct{identifier id;expr pos;expr value;} aryassign;
		}u;
	};

struct stats_
	{
	enum{uni_stats,multi_stats}kind;
	struct position pos;
	stats head;
	stat tail;
	};

struct identifiers_
	{
	enum{uni_identifiers,multi_identifiers}kind;
	struct position pos;
	identifiers head;
	identifier tail;
	};

struct type_{
	enum{int_type}kind;
	struct position pos;
	};

struct mainfunc_
	{
	enum{empty_mainfunc,content_mainfunc}kind;
	struct position pos;
	stats content;
	};

struct funcdefn_
	{
	enum{void_funcdefn,empty_funcdefn,none_funcdefn,norm_funcdefn}kind;
	struct position pos;
	type thistype;
	int argc;
	identifier id;
	vardecls args;
	stats content;
	};

struct funcdecl_
	{
	enum{void_funcdecl,norm_funcdecl}kind;
	struct position pos;
	type thistype;
	identifier id;
	vardecls args;
	};

struct vardefn_
	{
	enum{uni_vardefn,array_vardefn}kind;
	struct position pos;
	type thistype;
	identifier id;
	int len;
	};

struct vardecl_
	{
	enum{uni_vardecl,fix_vardecl,unfix_vardecl}kind;
	struct position pos;
	type thistype;
	identifier id;
	int len;
	};

struct vardecls_
	{
	enum{uni_vardecls,multi_vardecls}kind;
	struct position pos;
	vardecls head;
	vardecl tail;
	};

struct def_
	{
	enum{vardefn_def,funcdefn_def,funcdecl_def}kind;
	struct position pos;
	vardefn vardefnn;
	funcdefn funcdefnn;
	funcdecl funcdecll;
	};

struct defs_
	{
	enum{uni_defs,multi_defs}kind;
	struct position pos;
	defs head;
	def tail;
	};

struct goal_
	{
	enum{nodef_goal,withdef_goal}kind;
	struct position pos;
	defs util;
	mainfunc main;
	};
