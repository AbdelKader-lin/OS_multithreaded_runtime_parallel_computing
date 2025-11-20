#ifndef __TASKS_QUEUE_H__
#define __TASKS_QUEUE_H__

#include "tasks.h"
#include <pthread.h>


typedef struct tasks_queue{
    task_t** task_buffer;
    unsigned int task_buf_size;
    unsigned int index;
    unsigned int id ;
    int steal_idx ; // Indice of the element to steal 
} tasks_queue_t;

typedef struct tasks_queues_array{
    tasks_queue_t** tab_queues ;
    pthread_mutex_t* locks_array ; 
    unsigned int in ; // Next thread pour dispatch_task : Algo RR
    unsigned int out ;
} tasks_queues_array_t ;

tasks_queues_array_t* create_tasks_queue(void);
void free_tasks_queue(tasks_queue_t *q);

void enqueue_task(tasks_queue_t *q, task_t *t );
task_t* dequeue_task(tasks_queue_t *q );
task_t* steal_task( tasks_queue_t *q ) ;

#endif
