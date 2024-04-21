#include "apue.h"
#include <setjmp.h>

static void f1(int, int, int, int);
static void f2(void);

static jmp_buf jmpbuffer;
static int globval;

int main(void) {
	int autoval;
	register int regival;
	volatile int volaval;
	static int statval;
	globval = 1; autoval = 2; regival = 3; volaval = 4; statval = 5;

	if(setjmp(jmpbuffer) != 0) {
		printf("after longjmp:\n");
		printf("globval=%d\tautoval=%d\tregival=%d\tvolaval=%d\tstatval=%d\n", globval, autoval, regival, volaval, statval);
		exit(0);
	}
	/* Change vars after setjmp, but before longjmp */
	globval = 81; autoval = 82; regival = 83; volaval = 84; statval = 85;
	
	f1(autoval, regival, volaval, statval);
	exit(0);
}

static void f1(int i, int j, int k, int l) {
	printf("in f1:\n");
	printf("globval=%d\tautoval=%d\tregival=%d\tvolaval=%d\tstatval=%d\n", globval, i, j, k, l);
	/* call f2 in order to check if the stack frame was 'uppered' for f1 */
	f2();
}

static void f2(void) {
	/* static, volatile and global vars will be unaffected */
	longjmp(jmpbuffer, 1); // 1 will be accepted by setjmp
}
