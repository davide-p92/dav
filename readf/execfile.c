#include "hd.h"
#include <sys/wait.h>

#define NLINES 512

void read_file(const char *fname, char *content[]) {
	char **lines;
	int lenl, nlin;
	int lenf = 0;
	size_t nread;
	FILE *f;

	if((f = fopen(fname, "r")) == NULL)
		print_err(errno, "Error fopen");
		
	char line[MAXLINE];
	lines = (char **)malloc(sizeof(char *) * NLINES);
	/* Double check lines memory allocation */
	if(lines == NULL)
		print_err(errno, "error allocating mem for lines");
	for (int i = 0; i < NLINES; i++) {
		lines[i] = (char *)malloc(MAXLINE);
		if(lines[i] == NULL)
			print_err(errno, "error allocating mem for line");
	}
	nlin = 0;
	while(fgets(line, sizeof(line), f) != NULL) {
		size_t len = strlen(line);
		lenf += len;
		/*Let's avoid empty lines*/
		if(line[0] != '\n' && line[0] != '\0') {
			if(len > 0 && line[len - 1] == '\n') {
				/*printf("line: %s\n",line);
				 */
				strcpy(lines[nlin], line);
				content[nlin] = (char *)malloc(len);
				if(content[nlin] == NULL)
					print_err(errno, "error malloc for content[nlin]\n");
				//Better here
				strcpy(content[nlin],line);
				nlin++;
			}
		}
	}
	//content = (char **)malloc(sizeof(char *)*nlin);
	//Free allocated array
	/*counter for content array, which has to be filled
	 * from lines only if lines[i] is not empty*/
	int k = 0;
	for(int i = 0; i < NLINES; i++) {
		/*if(lines[i][0] != '\0' && lines[i][0] != '\n') {
			//content[k] = (char *)malloc(sizeof(char) * sizeof(lines[i]));
			content[k] = lines[i];
			printf("lines[i] (FROM read_file): %s\n", lines[i]);
			k++;
		}*/
		free(lines[i]);
	}
	free(lines);

	fclose(f);
	return;// content;
}

/*Function to chop pat from entry in lines*/
char *chopp(char *s) {
	char *n = malloc(strlen(s ? s : "\n"));
	if(s)
		strcpy(n, s);
	n[strlen(n) - 1] = '\0';
	return n;
}

int main(int argc, char **argv) {
	if(argc != 3)
		print_exit("Usage: %s <file> <name-repo>\n", argv[0]);
	char fname[256];
	char nrepo[256];
	int len;
	strcpy(fname, argv[1]);
	strcpy(nrepo, argv[2]);
	char *contf[NLINES];
	//contf = (char **)malloc(sizeof(char *)*NLINES);
	for(int i = 0; i < NLINES; i++) {
		contf[i] = (char *)malloc(MAXLINE);
	}
	printf("Length of file: %d\n", len);

	read_file(fname, contf);
	for(int i = 0; i < NLINES; i++)
		printf("contf: %s\n", contf[i]);
	//read only first line of contf, which is the PAT
	char pat[MAXLINE];
       	strcpy(pat, contf[0]);
	strcpy(pat, chopp(pat));/*eliminate newline*/
	//Allocate memory for exec comman
	char cmd[1024];
	size_t prevlen = strlen(cmd);
	/*memset(cmd + prevlen, cmd, 1023 - prevlen);
	*(cmd + 1023) = '\0';
	*/
	/*strcpy(cmd, "git clone https://%s@github.com/iitip/c_rep.git");
	memset(cmd + prevlen, cmd, 1023 - prevlen);
	*(cmd + 1023) = '\0';
	*/
	sprintf(cmd, "git clone https://x-token-auth:%s@bitbucket.org/iitip/%s.git", pat, argv[2]);	
	if(fork() == 0) {
		printf("cmd: %s\n", cmd);
		if(system(cmd) < 0)
			print_err(errno, "Error execl.");
	}
	//Parent now waits for child (PID 0) to complete the system call
	if(wait((int *)0) < 0)
		print_err(errno, "error wait");		

	fflush(stdout);
	/*Free the alloced memory*/
	for(int i = 0; i < NLINES; i++) {
		free(contf[i]);
	}
	//free(contf);

	return 0;
}
