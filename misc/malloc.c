#include <stdio.h>
#include <stdlib.h>

int main() {
	char *s = (char *)malloc(sizeof(char) * 32);
	int i = 0;
	while(i < 12) {
		printf(s);
		i = i / 2;
	}
	free(s);
	return 0;
}
