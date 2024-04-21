#include "apue.h"

#define IN TRUE /* inside file hole */
#define OUT FALSE /* outside file hole */

#define BUFFSIZE 4096
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

static int write2fd(int fd, char *buf, int nbytes);
static int llseek(int fd, int offset, int whence);

int main(int argc, char **argv) {
	if(argc != 3)
		err_quit("Usage: %s <in_file> <out_file>\n", argv[0]);
	int n, i, j;
	char rbuf[BUFFSIZE], wbuf[BUFFSIZE];
	int rfd, wfd;
	int flag, offset;

	if((rfd = open(argv[1], O_RDONLY)) < 0)
		err_sys("open read error");
	if((wfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, RWRWRW)) < 0)
		err_sys("open write error");

	while((n = read(rfd, rbuf, BUFFSIZE)) > 0) {
		flag = OUT;
		offset = 0;

		for(i = 0, j = 0; i < n; i++) {
			if(rbuf[i] != '\0') {
				if(flag == OUT && offset > 0 && llseek(rfd, offset, SEEK_CUR) == TRUE)
					offset = 0; //reset offset
				flag = IN;
				wbuf[j++] = rbuf[i];
			} else {
				if(flag == IN && write2fd(wfd, wbuf, j))
					j = 0;
				flag = OUT;
				offset++;
			}
		}
		if(flag == IN && j > 0)
			write2fd(wfd, wbuf, j);
	}

	if(close(rfd) < 0)
		err_sys("close read error");
	if(close(wfd) < 0)
		err_sys("close write error");
}

static int write2fd(int fd, char *buf, int nbytes) {
	if(write(fd, *buf, nbytes) != nbytes)
		err_sys("write error");
	return TRUE;
}

static int llseek(int fd, int offset, int whence) {
	if(lseek(fd, offset, whence) < 0)
		err_sys("lseek error");
	return TRUE;
}
