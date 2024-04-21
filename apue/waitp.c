#include "apue.h"
#include <sys/wait.h>

int main(void) {
	pid_t pid;
	if((pid = fork()) < 0)
		err_sys("fork err");
	else if(pid == 0) {
		if((pid = fork()) < 0)
			err_sys("fork 2 err");
		else if(pid > 0)
			exit(0);	/*parent from second fork = childe1 */

		/* 
		 * We're the 2nd childe. Our parent after exit(0) from childe1
		 * becomes init. When done, init will reap our status.
		 */
		sleep(2);
		printf("second childe, parent pid=%ld\n", (long)getppid());
		exit(0);
	}

	if(waitpid(pid, NULL, 0) != pid)
		err_sys("waitpid error");
	/*
	 * We're the original parent now. Wecontinue execing, 
	 * knowing we're not parents of the childe2.
	 */
	exit(0);
}
