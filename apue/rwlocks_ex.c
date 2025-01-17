#include <stdlib.h>
#include <pthread.h>


pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

struct job {
	struct job *j_next;
	struct job *j_prev;
	pthread_t j_id; //to tell which threads handls this job
};

struct queue {
	struct job *q_head;
	struct job *q_tail;
	pthread_rwlock_t q_lock;
};

/*
 * Initialize queue
 */
int queue_init(struct queue *qp) {
	int err;

	qp->q_head = NULL;
	qp->q_tail = NULL;
	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if(err != 0)
		return err;
	return 0;
}
/*
 * Insert a job at head of q
 */
void job_insert(struct queue *qp, struct job *jp) {
	//Wait for lock to be released before starting
	pthread_cond_wait(&qready);
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;
	if(qp->q_head != NULL)
		qp->q_head->j_prev = jp;
	else
		qp->q_tail = jp; /*list empty was*/
	qp->q_head = jp;
	pthread_rwlock_unlock(&qp->q_lock);
	//Tell other threads lock has been released
	pthread_cond_signal(&qready);
}

/*
 * Append job on thail of queue
 */
void job_append(struct queue *qp, struct job *jp) {
	pthread_cond_wait(&ready);
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = NULL;
	jp->j_prev = qp->q_tail;
	if(qp->q_tail != NULL)
		qp->q_tail->j_next = jp;
	else
		qp->q_head = jp;
	qp->q_tail = jp;
	pthread_rwlock_unlock(&qp->q_lock);
	pthread_cond_signal(&qready);
}
/*
 * Remove given job from queue
 */
void job_remove(struct queue *qp, struct job *jp) {
	pthread_cond_wait(&qready);
	pthread_rwlock_wrlock(&qp->q_lock);
	if(jp == qp->q_head) {
		qp->q_head = jp->j_next;
		if(qp->q_tail == jp)
			qp->q_tail = NULL;
		else
			jp->j_next->j_prev = jp->j_prev;
	} else if(jp == qp->q_tail) {
		qp->q_tail = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}
	pthread_cond_signal(&qready);
	pthread_rwlock_unlock(&qp->q_lock);
}
/*
 *Find jb for given tid
 */
struct job *job_find(struct queue *qp, pthread_t id) {
	struct job *jp;
	pthread_cond_wait(&qready);
	if(pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return NULL;
	for(jp = qp->q_head; jp != NULL; jp = jp->j_next) {
		if(pthread_equal(jp->j_id, id))
			break;
	}
	pthread_rwlock_unlock(&qp->q_lock);
	pthread_cond_signal(&qready);
	return(jp);
}
