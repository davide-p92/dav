#include "apue.h"
#include <sys/wait.h>

int cnt = 0;

void sigint_handler(int signo) {
	//Compiler runs signal(signo, SIG_DFL); so reset handler
	signal(SIGCHLD, sigint_handler);
	(void)signo; //get rid of unused var
	printf("pid: %d\n", getpid());
	cnt++;
	printf("cnt: %d\n", cnt);
	if(cnt == 2) {
		printf("Exiting!\n");
		exit(0);
	}
}

int main(void) {
	pid_t pid;
	int status;
	signal(SIGCHLD, sigint_handler);
	siginfo_t infop;
	siginfo_t infop2;
	siginfo_t infop3;

	if((pid = fork()) < 0) /*error check */
		err_sys("fork error");
	else if(pid == 0) /*childe*/
		exit(7);
	/*waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)*/
	if(waitid(P_ALL, 0, &infop, WEXITED | WSTOPPED) == pid) { /*wait for children with id = 0
			exited or stopped by sign and save info on struct infop*/
		printf("si_pid: %d\tsi_uid: %d\tsi_status: %d\tsi_code: %d\tsi_addr:%x\n", infop.si_pid, infop.si_uid, infop.si_status, infop.si_code, infop.si_addr);
	} else
		err_sys("waitid error");
	if((pid = fork()) < 0)
		err_sys("fork2 error");
	else if(pid == 0) /*childe 2*/
		abort();
	if(waitid(P_ALL, 0, &infop2, WEXITED | WSTOPPED) == pid) {
		printf("si_pid2: %d\tsi_uid2: %d\tsi_status2: %d\tsi_code2: %d\tsi_addr2: %x\n", infop2.si_pid, infop2.si_uid, infop2.si_status, infop2.si_code, infop2.si_addr);
	} else
		err_sys("waitid2 error");
	if((pid = fork()) < 0)
		err_sys("fork3 error");
	else if(pid == 0) /*childe 3*/
		status /= 0; //divide by 0 geners SIGFPE
	if(waitid(P_ALL, 0, &infop3, WEXITED | WSTOPPED) == pid) { /*wait for childe*/
		printf("si_pid3: %d\tsi_uid3: %d\tsi_status3: %d\tsi_code3: %d\tsi_addr3: %x\n", infop3.si_pid, infop3.si_uid, infop3.si_status, infop3.si_code, infop3.si_addr);
	} else
		err_sys("waitid 3 error");
	//wait try
	int stat;
	if((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {/* to-do: study signals
		struct sigaction sa;
		sa.sa_handler = sigint_handler;
		
		signal(SIGCHLD, sigint_handler);
		exit(9);*/
	if(wait(&stat) != pid)
		err_sys("wait error");
	if(WIFEXITED(stat) != 0)
		printf("WEXITSTATUS: %d\n", WEXITSTATUS(stat));
	if(WIFSIGNALED(stat) != 0)
		printf("WTERMSIG: %d\n", WTERMSIG(stat));
	if(WIFSTOPPED(stat) != 0)
		printf("WSTOPSIG: %d\n", WSTOPSIG(stat));
	exit(0);
}
