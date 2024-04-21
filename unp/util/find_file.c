#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/stats.h>
#include <string.h>


int callback_handler() {
}

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return -1;
	}

}
