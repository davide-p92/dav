#include "apue.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
	if(argc != 2)
		err_quit("Usage: %s <file>\n", argv[0]);
	int fd, wfd;
	int nread, nwrite, i, offset;
	char rbuf[BUFSIZ], wbuf[BUFSIZ], c[11] = "aaaaaaaaaa\0";
	
	char rfname[50], wfname[50];
	strcpy(rfname, argv[1]);
	//strcpy(wfname, argv[2]);

	if((fd = open(rfname, O_RDWR, O_CREAT | O_APPEND)) < 0)
		err_sys("error open read.");
	printf("Write some string for %s: ", rfname);
	
	//scanf("%c%*1[\n]", &char);
	fgets(wbuf, BUFSIZ, stdin);
	if((nwrite = write(fd, wbuf, BUFSIZ)) < 0)
		err_sys("error write.");
	
	printf("Which offset? ");
	scanf("%d", &offset);
	lseek(fd, SEEK_SET, offset);

	printf("writing to file\n");

	if((i = write(fd, c, 10)) < 0)
		err_sys("error 2nd write.");
		
	if(close(fd) < 0)
		err_sys("error close on %s", rfname);
	//Read created file
	if((fd = open(rfname, O_RDWR)) < 0)
		err_sys("error 2nd open");
	nread = 0;
	if((nread = read(fd, rbuf, BUFSIZ)) < 0)
	       err_sys("error reading file %s", rfname);
	for(int k = 0; k < nread; k++) {
		printf("%c", rbuf[k]);
	}
	printf("\n");
	printf("file size: %d\n", nread);
	
	if(close(fd) < 0)
		err_sys("error 2nd close");


	return 0;
}	
