#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 256

int main(int argc, char **argv) {
	FILE *fp;
	char path[MAX_BUF_SIZE];
	int val = 0;
	size_t sz = sizeof(val);

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <dir_to_scan>.\n");
		exit(EXIT_FAILURE);
	}
	//def path to sysctl param 
	snprintf(path, sizeof(path), argv[1]);//"/proc/sys/kernel/pid_max");
	//open file containing the sysctl param to read
	fp = fopen(path, "r");
	if(fp == NULL) {
		perror("Error opening sysctl file\n");
		exit(EXIT_FAILURE);
	}

	//read val of sysctl param
	if(fscanf(fp, "%d", &val) != 1) {
		perror("Error opening sysctl file");
		exit(EXIT_FAILURE);
	}
	fclose(fp);

	printf("Value of var %s: %d\n", argv[1], val);

	return 0;
}
