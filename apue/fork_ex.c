#include "apue.h"

/* When writing to file, all variables will be copied from parent to childe without not even after \n, so it will output the line 'before fork' twice*/ 

int globvar = 6;
char buf[] = "a stdout print\n";

int main(void) {
	int var;
	pid_t pid;
	var = 88;
	/*sizeof calculated at compile time */
	if(write(STDOUT_FILENO, buf, sizeof(buf) - 1) < sizeof(buf) - 1)
	       err_sys("before fork error");
	printf("before fork\n"); /* we don't flush stdout */

	if((pid = fork()) < 0) {
		err_sys("fork error");
	} else if(pid == 0) {
		globvar++;
		var++;
	} else {
		sleep(2);
	}
	
	printf("pid = %ld\tglobvar=%d\tvar=%d\n", (long)getpid(), globvar, var);
	exit(0);
}
