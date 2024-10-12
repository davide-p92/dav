#include "apue.h"
#include <pthread.h>

void *thr_fn1(void *arg) {
	printf("thread 1 returning\n");
	return((void *)1);
}

void *thr_fn2(void *arg) {
	printf("thread 2 exiting\n");
	pthread_exit((void *)2);
}

int main(void) {
	int err;
	pthread_t tid1, tid2;

	void *tret;
	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	if(err != 0)
		err_exit(err, "cannot create thread 1\n");
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	if(err != 0)
		err_exit(err, "cannot create thread 2\n");
	err = pthread_join(tid1, &tret);
	if(err != 0)
		err_exit(err, "can't join with thread 1\n");
	printf("Thread 1 exit coode: %ld\n", (long)tret);
	err = pthread_join(tid2, &tret);
	if(err != 0)
		err_exit(err, "can't join with thread 2\n");
	printf("Thread 2 exit coode: %ld\n", (long)tret);
	exit(0);
}
