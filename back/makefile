riscv: translate.tab.o lexical.yy.o select.o
	cc -g -o riscv translate.tab.o lexical.yy.o select.o

translate.tab.o: translate.tab.c select.h
	cc -g -c translate.tab.c

translate.tab.c: translate.y
	bison -o translate.tab.c -dv translate.y

translate.tab.h: translate.tab.c
	echo "translate.tab.h was created at the same time as translate.tab.c"

lexical.yy.o: lexical.yy.c translate.tab.h utility.h
	cc -g -c lexical.yy.c

lexical.yy.c: lexical.l
	flex -o lexical.yy.c lexical.l 

select.o: select.c select.h
	cc -g -c select.c

clean: 
	rm -f riscv translate.tab.o translate.tab.c translate.tab.h lexical.yy.o lexical.yy.c translate.output select.o



