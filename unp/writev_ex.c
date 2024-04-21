#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <time.h>
#include <pthread.h>

#define NANOSECONDS_PER_SECOND 1000000000

//Buffers
char buffer1[] = "Hello, ";
char buffer2[] = "writev!\n";

//Function for thread using write
void *writeThread(void *arg) {
	ssize_t bytesWritten = write(STDOUT_FILENO, arg, sizeof(buffer1) - 1);
	if(bytesWritten == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}
	return NULL;
}

int main() {
	//char buffer1[] = "hello, ";
	//char buffer2[] = "writev!\n";

	//IO vectors
	struct iovec iov[2];
	iov[0].iov_base = buffer1;
	iov[1].iov_base = buffer2;


	iov[0].iov_len = sizeof(buffer1) - 1; /*exclude null char */
	iov[1].iov_len = sizeof(buffer2) - 1;

	//Time measurements
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	//Write BOTH buffs at the same time
	ssize_t bytesWritten = writev(STDOUT_FILENO, iov, 2);

	clock_gettime(CLOCK_MONOTONIC, &end);
	long long elapsed = (end.tv_sec - start.tv_sec) * NANOSECONDS_PER_SECOND + (end.tv_nsec - start.tv_nsec);
	if(bytesWritten == -1) {
		perror("writev");
		exit(EXIT_FAILURE);
	}

	printf("Writev wrote %zd bytes in %lld ns\n", bytesWritten, elapsed);
	
	//Comparison with write separately
	clock_gettime(CLOCK_MONOTONIC, &start);
	ssize_t bytesWrittenBuffer1 = write(STDOUT_FILENO, buffer1, sizeof(buffer1) - 1);
	ssize_t bytesWrittenBuffer2 = write(STDOUT_FILENO, buffer2, sizeof(buffer2) - 1);
	clock_gettime(CLOCK_MONOTONIC, &end);

	elapsed = (end.tv_sec - start.tv_sec) * NANOSECONDS_PER_SECOND + (end.tv_nsec - start.tv_nsec);

	if(bytesWrittenBuffer1 == -1 || bytesWrittenBuffer2 == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}

	printf("Write wrote %zd + %zd bytes in %lld ns.\n", bytesWrittenBuffer1, bytesWrittenBuffer2, elapsed);
	
	//THREAD COMPARISON with write
	clock_gettime(CLOCK_MONOTONIC, &start);
	pthread_t writeThread1, writeThread2;
	//Create thread
	if(pthread_create(&writeThread1, NULL, writeThread, buffer1) != 0 || pthread_create(&writeThread2, NULL, writeThread, buffer2) != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	//Wait for threads to finish
	pthread_join(writeThread1, NULL);
	pthread_join(writeThread2, NULL);
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed = (end.tv_sec - start.tv_sec) * NANOSECONDS_PER_SECOND + (end.tv_nsec - start.tv_nsec);

	printf("Write wrote in threads in %lld ns.\n", elapsed);

	return 0;
}
