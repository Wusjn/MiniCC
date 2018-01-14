mcc: Main.o syntax.tab.o lexical.yy.o syntax_tree.o table.o types.o semantic.o
	cc -g -o mcc Main.o syntax.tab.o lexical.yy.o syntax_tree.o table.o types.o semantic.o

Main.o: Main.c
	cc -g -c Main.c

syntax.tab.o: syntax.tab.c syntax_tree.h
	cc -g -c syntax.tab.c

syntax.tab.c: syntax.y
	bison -o syntax.tab.c -dv syntax.y

syntax.tab.h: syntax.y
	echo "syntax.tab.h was created at the same time as syntax.tab.c"

lexical.yy.o: lexical.yy.c syntax.tab.h
	cc -g -c lexical.yy.c

lexical.yy.c: lexical.l
	flex -o lexical.yy.c lexical.l

syntax_tree.o: syntax_tree.c syntax_tree.h
	cc -g -c syntax_tree.c

table.o: table.c table.h
	cc -g -c table.c

types.o: types.c types.h
	cc -g -c types.c

semantic.o: semantic.c types.h syntax_tree.h
	cc -g -c semantic.c

clean:
	rm -f mcc Main.o syntax.tab.o syntax.tab.c syntax.tab.h syntax.output lexical.yy.o lexical.yy.c syntax_tree.o table.o types.o semantic.o
	rm -f code code.o code.t code.e code.s

