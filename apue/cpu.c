#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void read_cpu_times(unsigned long long int *user, unsigned long long int *nice,
		unsigned long long int *system, unsigned long long int *idle) {
	FILE *fp = fopen("/proc/stat", "r");
	if (fp == NULL) {
		perror("Failed to open /proc/stat");
		exit(EXIT_FAILURE);
	}
	
	char buffer[1024];
	fgets(buffer, sizeof(buffer) - 1, fp); //Read first line
	sscanf(buffer, "cpu %llu %llu %llu %llu", user, nice, system, idle);

	fclose(fp);
}

//Calc CPU usage %age
double calculate_cpu_usage(unsigned long long int prev_user, unsigned long long int prev_nice,
		unsigned long long int prev_system, unsigned long long int prev_idle,
		unsigned long long int user, unsigned long long int nice,
		unsigned long long int system, unsigned long long int idle) {
	unsigned long long int prev_idle_time = prev_idle;
	unsigned long long int idle_time = idle;
	
	unsigned long long int prev_total_time = prev_user + prev_nice + prev_system + prev_idle_time;
	unsigned long long int total_time = user + nice + system + idle_time;

	unsigned long long int total_diff = total_time - prev_total_time;
	unsigned long long int idle_diff = idle_time - prev_idle_time;

	return (double) (total_diff - idle_diff) / total_diff * 100;
}

int main() {
	unsigned long long int prev_user, prev_nice, prev_system, prev_idle;
	unsigned long long int user, nice, system, idle;

	//Init first read
	read_cpu_times(&prev_user, &prev_nice, &prev_system, &prev_idle);
	sleep(1);

	while(1) {
		read_cpu_times(&user, &nice, &system, &idle);
		
		double cpu_usage = calculate_cpu_usage(prev_user, prev_nice, prev_system, prev_idle, user, nice, system, idle);
		printf("CPU Usage: %.2f%%\n", cpu_usage);
		
		//Prepare for the next iter
		prev_user = user;
		prev_nice = nice;
		prev_system = system;
		prev_idle = idle;

		sleep(1);
	}

	return 0;
}
	
