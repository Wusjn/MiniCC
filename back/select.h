#include "utility.h"

void uni_varDecl(int var,int val);
void multi_varDecl(int var,int size);
void begin_func(string func,int argc,int size);
void end_func(string func);
void bireg(string l,string r1,int oper,string r2);
void regint(string l,string r1,int oper,int r2);
void unireg(string l,int oper,string r);
void reg(string l,string r);
void integer(string l,int r);
void larray(string l,int index,string r);
void rarray(string l,string r,int index);
void bif(string l,int oper,string r,int lab);
void gotol(int l);
void label(int l);
void call(string func);
void store(string reg,int stk);
void loadstk(int stk,string reg);
void loadglb(int glb,string reg);
void addrstk(int stk,string reg);
void addrglb(int glb,string reg);
void ret();

