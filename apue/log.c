/*
 * Error routines for programs that can run as daemons
 */
#include "apue.h"
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

static void log_doit(int, int, int, const char *, va_list ap);
/*
 * Caller must define and set this: nonzero if
 * interactive, zero if daemon
 */
extern int log_to_stderr;
/*
 * Initialize syslog(), if running
 */
void log_open(const char *ident, int option, int facility) {
	if(log_to_stderr == 0)
		openlog(ident, option, facility);
}
/* Nonfatal error related to a system call.
 * Print a msg with the system's errno and ret.
 */
void log_ret(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
}
/* Fatal error related to a system call.
 * Print msg with system's errno and term.
 */
void log_sys(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	log_doit(1, errno, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}
/* Nonfatal err unrelated to a syscall.
 * Print msg and ret.
 */
void log_msg(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
}
/* Fatal err unrelated to a syscall.
 * Print msg and term.
 */
void log_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	log_doit(0, 0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}
/* Fatal err related to syscall.
 * Error nr. passed as explicit param.
 * Print msg. and term.
 */
void log_exit(int error, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	log_doit(1, error, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(2);
}
/* 
 * Print a message and return to caller.
 * Caller specifes 'errnoflag' and 'priority'.
 */
static void log_doit(int errnoflag, int error, int priority, const char *fmt, va_list ap) {
	char buf[MAXLINE];
	vnsprintf(buf, MAXLINE-1, fmt, ap);
	if(errnoflag) 
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(error));
	strcat(buf, "\n");
	if(log_to_stderr) {
		fflush(stdout);
		fputss(buf, stderr);
		fflush(stderr);
	} else {
		syslog(priority, "%s", buf);
	}
}
