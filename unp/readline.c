#include "unp.h"

//static vars to preserve state info, but they are not reentrant nor thread-safe
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr) {
	if(read_cnt <= 0) {
again:
		if((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if(errno == EINTR) 
				goto again;
			return -1;
		} else if (read_cnt == 0)
			return 0;
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for(n = 1; n < maxlen; n++) {
again:
		if((rc = my_read(fd, &c)) == 1) {
			*ptr ++= c;
			if(c == '\n')
				break; //newline is stored fgets()
		} else if(rc == 0) {
			*ptr = 0;
			return (n - 1); //EOF
		} else {
			if(errno == EINTR)
				goto again;
			return -1; //error
		}
	}
	*ptr = 0;
	return n;
}
/*
ssize_t Readline(int fd, void *vptr, size_t maxlen) {
	ssize_t res = readline(fd, vptr, maxlen);
	if(res == 0) {
		printf("Error. readline yielded 0 (EOF)\n");
	} else if(res < 0)
		printf("Error. readline did not find suitable buffer\n");
	return res;
}
*/
ssize_t readlinebuf(void **vptrptr) {
	if(read_cnt)
		*vptrptr = read_ptr;
	return (read_cnt);
}
