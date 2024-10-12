#include "apue.h"

int globvar = 5;

int main(void) {
	int var;//automatic var
	pid_t pid;
	var = 88;
	printf("Before vfork\n"); /* not flushing stdio */
	if((pid = vfork()) < 0) {
		err_sys("vfork() error");
	} else if(pid == 0) { //childe, same address space as parent, until exit orexec
		globvar++;
		var++;
		exit(0);
	}
	/*parent cont's here*/
	printf("pid = %ld, glob = %d, var = %d\n", pid, globvar, var);
	exit(0);
}
