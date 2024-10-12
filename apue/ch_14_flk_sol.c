#include "apue.h"
#include <errno.h>
#include <fcntl.h>

void sigint(int signo) {
}

int main() {
	pid_t pid1, pid2, pid3;
	int fd;

	setbuf(stdout, NULL);
	signal_intr(SIGINT, sigint);
	//File create
	if((fd = open("lockfile", O_RDWR | O_CREAT, 0666)) < 0)
		err_sys("can't open/create lockfile");
	
	//read-lock file.
	if((pid1 = fork()) < 0) {
		err_sys("error fork()");
	} else if(pid1 == 0) { /*childe*/
		if(lock_reg(fd, F_SETLK, F_RDLCK, 0, SEEK_SET, 0) < 0)
			err_sys("childe1: can't read-lock file");
		printf("childe1: obtained read lock on file");
		pause();
		printf("childe1: exit after pause");
		printf("\n");
		//exit
		exit(0);
	} else {
		sleep(2); //parent
	}

	//parent continues ... read-lock the file
	if((pid2 = fork()) < 0) {
		err_sys("fork2 failed");
	} else if (pid2 == 0) { /* child */
		if(lock_reg(fd, F_SETLK, F_RDLCK, 0, SEEK_SET, 0) < 0) {
			err_sys("childe2: can't read lock file");
		}
		printf("childe2: obtained read lock on file");
		pause();
		printf("child2: exit after pause\n");
		exit(0);
	} else { /*parent*/
		sleep(2);
	}
	//fork weiter Kind
	if((pid3 = fork()) < 0)
		err_sys("fork failed");
	else if (pid3 == 0) { /*child*/
		if(lock_reg(fd, F_SETLK, F_WRLCK, 0, SEEK_SET, 0) < 0) {
			printf("child 3: canìt set write lock on file, %s", strerror(errno));
		}
		printf("child 3 about to block in write-lock...\n");
		if(lock_reg(fd, F_SETLKW, F_WRLCK, 0, SEEK_SET, 0) < 0) {
			//in lock-wait
			err_sys("child 3: can't write lock file\n");
		}
		printf("child 3 returned and got write lock????\n");
		pause();
		printf("child 3 exit after paus\n");
		exit(0);
	} else { /*parent*/
		sleep(2);
	}
	/*See if pending write lock will block the next read-lock attempt.*/
	if(lock_reg(fd, F_SETLK, F_RDLCK, 0, SEEK_SET, 0) < 0)
		printf("parent: can't set read lock: %s\n", strerror(errno));
	else
		printf("parent: obtained additional read lock while write lock is pending\n");
	printf("killing childe 1...\n");
	kill(pid1, SIGINT);
	printf("killing childe 2...\n");
	kill(pid2, SIGINT);
	printf("killing child 3...\n");
	kill(pid3, SIGINT);
	exit(0);
}	
