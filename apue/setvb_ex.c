#include <stdio.h>
#include <stdlib.h>

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

FILE *open_data(void){
	FILE *fp;
	char *databuf; /*databuf[BUFSIZ] will return error because the setvbuf function will make databuf stdio buffer, so the next function call will try to access that buffer unsuccessfully because it belongs to open_data stack frame and will give err */
	databuf = (char *)malloc(sizeof(char) * BUFSIZ);

	if((fp = fopen("dav.txt", "r")) == NULL)
		return (NULL);
	if(setvbuf(fp, databuf, _IOLBF, BUFSIZ) != 0)
		return (NULL);
	free(databuf);
	return(fp);	/* error if databuf is not global or dynamic */
}

int main() {
	open_data();
	return 0;
}
