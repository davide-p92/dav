#include "unp.h"

long writen(int fd, void *vptr, unsigned int n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if((nwritten = write(fd, ptr, nleft)) <= 0) {
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1; /*error*/
		}
		nleft -= nwritten;
		ptr += nwritten; //increment ptr
	}
	return (long)(n - nleft);
}

long Writen(int fd, void *vptr, unsigned int n) {
	if (writen(fd, vptr, n) == -1) {
		printf("error writen: %s\n", strerror(errno));
		return -1;
	}
}
