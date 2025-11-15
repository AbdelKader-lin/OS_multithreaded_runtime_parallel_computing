#ifndef TASKS_QUEUE_H
#define TASKS_QUEUE_H

#include <pthread.h>
#include "tasks.h"


typedef struct tasks_queue {
    task_t **task_buffer;   // array of task pointers
    int task_buf_size;      // queue capacity
    int index;              // number of tasks currently stored

    
    pthread_mutex_t mutex;      // protects the queue
    pthread_cond_t not_empty;   // signals when tasks available
    pthread_cond_t not_full;    // signals when space available
    int stop;                   // signals workers to exit
} tasks_queue_t;

// API
tasks_queue_t* create_tasks_queue(void);
void free_tasks_queue(tasks_queue_t *q);
void enqueue_task(tasks_queue_t *q, task_t *t);
task_t* dequeue_task(tasks_queue_t *q);
void stop_tasks_queue(tasks_queue_t *q);

#endif

