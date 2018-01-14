#include <stdio.h>
int getint();
int putint(int i);

int putint(int i){
	return printf("%d",i);
}
int getint(){
	int a;
	scanf("%d",&a);
	return a;
}


