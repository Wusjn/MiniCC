mcc: driver.o gramar.tab.o lex.yy.o nodes.o symbol_table.o type_check.o codegen.o inter.o flow.o liveness.o reg_alloc.o tigger_gen.o
	cc -g -o mcc driver.o gramar.tab.o lex.yy.o nodes.o symbol_table.o type_check.o codegen.o inter.o flow.o liveness.o reg_alloc.o tigger_gen.o

driver.o: driver.c gramar.tab.h nodes.h
	cc -g -c driver.c

gramar.tab.o: gramar.tab.c nodes.h
	cc -g -c gramar.tab.c

gramar.tab.c: gramar.y
	bison -dv gramar.y

gramar.tab.h: gramar.tab.c
	echo "gramar.tab.h was created at the same time as gramar.tab.c"

lex.yy.o: lex.yy.c gramar.tab.h nodes.h
	cc -g -c lex.yy.c

lex.yy.c: lex.l
	flex lex.l

nodes.o: nodes.c nodes.h
	cc -g -c nodes.c

symbol_table.o: symbol_table.c symbol_table.h
	cc -g -c symbol_table.c

type_check.o: type_check.c nodes.h symbol_table.h
	cc -g -c type_check.c

codegen.o: codegen.c eeyore.h 
	cc -g -c codegen.c

inter.o: inter.c eeyore.h
	cc -g -c inter.c

flow.o: flow.c eeyore.h
	cc -g -c flow.c

liveness.o: liveness.c eeyore.h
	cc -g -c liveness.c

reg_alloc.o: reg_alloc.c eeyore.h
	cc -g -c reg_alloc.c

tigger_gen.o: tigger_gen.c eeyore.h
	cc -g -c tigger_gen.c

clean: 
	rm -f mcc driver.o gramar.tab.o gramar.tab.c gramar.tab.h lex.yy.o lex.yy.c
	rm -f nodes.o gramar.output symbol_table.o type_check.o codegen.o
	rm -f inter.o flow.o liveness.o reg_alloc.o tigger_gen.o

combine:
	cat codegen.c driver.c eeyore.h flow.c gramar.y head.h inter.c lex.l liveness.c makefile nodes.c nodes.h reg_alloc.c symbol_table.c symbol_table.h test tigger_gen.c type_check.c > z_total

