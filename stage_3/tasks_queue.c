#include <stdio.h>
#include <stdlib.h>

#include "tasks_queue.h"
#include <pthread.h>


int nbTasks ;

pthread_mutex_t mtx ;

pthread_cond_t notEmpty ;

extern pthread_cond_t finish ;

tasks_queue_t* create_tasks_queue(void){

    tasks_queue_t *q = (tasks_queue_t*) malloc(sizeof(tasks_queue_t));

    q->task_buf_size = QUEUE_CAPACITY;
    q->task_buffer = (task_t**) malloc(sizeof(task_t*) * q->task_buf_size);

    q->index = 0;

    return q;
}


void free_tasks_queue(tasks_queue_t *q)
{
    /* IMPORTANT: We chose not to free the queues to simplify the
     * termination of the program (and make debugging less complex) */
    
    /* free(q->task_buffer); */
    /* free(q); */
}


void enqueue_task( tasks_queue_t *q , task_t *t ) { // Producer


    pthread_mutex_lock( &mtx ) ;

    if ( q->index == q->task_buf_size ){ // Buffer is full
        // We resize the buffer : 16 new free spots.
        int new_size = q->task_buf_size + 16 ;
        task_t** new_buffer = realloc( q->task_buffer , new_size * sizeof(task_t*) ) ;
        
        q->task_buf_size = new_size ;
        q->task_buffer = new_buffer ;

    }
    
    
    
    // We add the task to the buffer
    q->task_buffer[ q->index ] = t;
    q->index++;
    

    pthread_cond_broadcast( &notEmpty ) ; // Tell other threads that the buffer has at least one element now
    pthread_mutex_unlock( &mtx ) ; // We give up the lock

}


task_t* dequeue_task( tasks_queue_t *q ) { // Consumer
    
    pthread_mutex_lock( &mtx ) ; // We acquire the lock

    while( q->index == 0 ){  // nb elts >= 1 ?
        pthread_cond_wait( &notEmpty , &mtx ) ;
    }
    
    // We consume an element : A free spot is now available
    task_t *t = q->task_buffer[ q->index - 1 ];
    q->index--;

    //pthread_cond_broadcast( &notFull ) ; // Tell everyone that the buffer is not full.
    pthread_mutex_unlock( &mtx ) ; // Give up the lock

    return t;
}

