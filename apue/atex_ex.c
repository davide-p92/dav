#include "apue.h"

static void my_exit1(void);
static void my_exit2(void);

typedef void Exitfunc(void);
static Exitfunc func;

int main(void) {
	if(atexit(my_exit2) != 0)
		err_sys("can't register my_exit2");
	if(atexit(my_exit1) != 0)
		err_sys("can't register my_exit1");
	if(atexit(func) != 0)
		err_sys("can't register func");
	printf("main done \n");
	return 0;
}

static void my_exit1(void) {
	printf("first exit handler\n");
}

static void my_exit2(void) {
	printf("second exit handler\n");
}

static void func(void) {
	printf("third exit handler\n");
}
