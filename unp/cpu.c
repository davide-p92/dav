#include <stdio.h>
#include <libcpuid.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int main(void) {
	struct cpu_raw_data_t raw;
	struct cpu_id_t data;
	union {
		short s;
		char c[sizeof(short)];
	} un;
	un.s = 0x0102;
	if(cpuid_get_raw_data(&raw) < 0) {
		printf("Error CPUID raw data: %s\n", strerror(errno));
		printf("Err: %s\n", cpuid_error());
		//exit(EXIT_FAILURE);
		return -1;
	}

	if(cpu_identify(&raw, &data) < 0) {
		printf("CPU ident failed: %s.\n", strerror(errno));
		printf("Err: %s\n", cpuid_error());
		//exit(EXIT_FAILURE);
		return -1;
	}

	printf("Processor has %d physical cores\n", data.num_cores);
	printf("Processor vendor: %s\n", data.vendor_str);
	//Check if big or little endian
	if(sizeof(short) == 2) {
		if(un.c[0] == 1 && un.c[1] == 2)
			printf("big-endian\n");
		else if(un.c[0] == 2 && un.c[1] == 1)
			printf("little-endian\n");
		else
			printf("unknown");
	} else
		printf("sizeof(short) = %d\n", sizeof(short));
	return 0;
}

