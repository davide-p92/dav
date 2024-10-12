#include "apue.h"
#include <fcntl.h>
#include <errno.h>
#include <ucontext.h>


int write_fd_lk(int fd, int offset, int whence, int len) {
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = offset;
	fl.l_whence = whence;
	fl.l_len = len;
	if(fcntl(fd, F_GETLK, &fl) < 0)
		err_sys ("fcntl error");
	return (fl.l_pid);
}

/*sig handler for SIGCHLD*/
void handlr(int sig, siginfo_t *siginf, void *ptr) {
	printf("handlr: signo=%d, uap=%p\n", siginf->si_signo, ptr);
#ifdef __x86_64__
	ucontext_t *uap = ptr;
	uap->uc_mcontext.__fpregs->__rip += 4;
	printf("uap->uc_mcontext.__fpregs->__rip=%x\n", uap->uc_mcontext.__fpregs->__rip);
#endif
}

int main(int argc, char *argv[]) {
	if(argc > 2)
		err_quit("Usage: %s [<FILE>]\n", argv[0]);
	char *fname;
	if((fname = (char *)malloc(BUFSIZ)) == NULL) {
		err_sys("malloc fname error");
	}
	if(argc == 2) {
		strcpy(fname, argv[1]);
	} else {
		strcpy(fname, "fl_lk_ex.txt");
	}
	printf("%s\n", fname);
	
	//file descriptor, create if needed
	FILE *file1;
	int modes = 0;
	modes |= S_IRUSR | S_IWUSR;
	int flags = 0;
	flags |= O_ASYNC | O_WRONLY | O_CREAT | O_APPEND;
	int fd = open(fname, flags, modes);
       	if(fd < 0)
		err_sys("error open");	
	if((file1 = fdopen(fd, "a")) == NULL)
		err_sys("error fdopen");
	/*check position in stream*/
	off_t offset;
	if((offset =fseek(file1, 0, SEEK_END)) == -1)
		err_sys("error fseek");
	
	/*check if lck present*/
	pid_t pid_tst = lock_test(fd, F_WRLCK, offset, SEEK_CUR, 0); 
	int pid_w = write_fd_lk(fd, offset, SEEK_CUR, 0);

	printf("pid_w: %d\n", pid_w);
	pid_t pid1 = getpid();
	struct sigaction sa;
	siginfo_t si;

	sa.sa_flags |= SA_SIGINFO | SA_NOCLDWAIT;
	sa.sa_sigaction = handlr;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGCHLD, &sa, NULL);

	pid_t pid;
	if((pid = fork()) < 0)
		err_sys("fork() error");
	if(pid == 0) { /*childe*/
		struct flock fl;
		fl.l_type = F_RDLCK;
		fl.l_start = offset;
		fl.l_whence = SEEK_CUR;
		fl.l_len = 0;
		pid_t pid_tst_chd;
		if((pid_tst_chd = fcntl(fd, F_GETLK, &fl)) < 0) {
			//tell parent and handle
			kill(pid, SIGCHLD);
		}			
	}
	/*Don't forget to close/free*/
	free(fname);
	fclose(file1);
	close(fd);
	return 0;
}
