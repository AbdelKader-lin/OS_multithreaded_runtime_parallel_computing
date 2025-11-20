#include "tasks_queue.h"
#include <stdlib.h>

pthread_mutex_t mtx;
pthread_cond_t notEmpty;
pthread_cond_t notFull;


/* Create queue */
tasks_queue_t* create_tasks_queue(void) {
    tasks_queue_t *q = malloc(sizeof(tasks_queue_t));

    q->task_buf_size = QUEUE_CAPACITY;
    q->task_buffer = malloc(sizeof(task_t*) * q->task_buf_size);
    q->index = 0;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return q;
}

void free_tasks_queue(tasks_queue_t *q) {
    
}


void enqueue_task(tasks_queue_t *q, task_t *t)
{
    pthread_mutex_lock(&q->mutex);

    while (q->index == q->task_buf_size) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    q->task_buffer[q->index++] = t;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}


task_t* dequeue_task(tasks_queue_t *q)
{
    pthread_mutex_lock(&q->mutex);

    while (q->index == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    task_t *t = q->task_buffer[--q->index];

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return t;
}
