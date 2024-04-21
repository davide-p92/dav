#include "apue.h"
#include <errno.h>
#include <stdarg.h>

static void err_doit(int, int, const char *, va_list);
/*
 * Nonfatal error related to a system call.
 * Print message and ret
 */
void err_ret(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/*
 * Fatal error related to system call.
 * Print message and terminate
 */
void err_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}
/*
 * Nonfatal error unrelated to syscall.
 * Error code passed as explicit param.
 * Print msg and ret.
 */
void err_cont(int error, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
}
/*
 * Fatal error unrelated to syscall.
 * Error code passed as expl param.
 * Print msg and term.
 */
void err_exit(int error, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}
/*
 * Fatal error related to syscall.
 * Print msg, dump core, term.
 */
void err_dump(const char *fmt, ...) {
	va_list ap;
	
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort(); /*dump core and terminate */
	exit(1); /*shouldnìt get here */
}
/*
 * Nonfatal error unrelated to syscall.
 * Print msg and ret.
 */
void err_msg(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
}
/*
 * Fatal error unrelated to syscall.
 * Print msg and term.
 */
void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}
/*
 * Print a messsage and return to caller.
 * Caller specifies 'errnoflag'.
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap) {
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE - 1, fmt, ap);
	if(errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, " :%s", strerror(error));
	strcat(buf, "\n");
	fflush(stdout); /* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL); /* flushes all stdio output streams */
}
