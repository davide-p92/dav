#include <stdio.h>

int cdecl_addme(short a, short b) {
	return a + b;
}

int main() {
	cdecl_addme(3,8);
	return 0;
}
