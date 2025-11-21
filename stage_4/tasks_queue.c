#include <stdio.h>
#include <stdlib.h>

#include "tasks_queue.h"
#include <pthread.h>


int nbTasks ;


pthread_mutex_t mtx ;

pthread_cond_t notEmpty ;

extern pthread_cond_t finish ;
extern tasks_queues_array_t* tqueue  ;

tasks_queue_t* create_tasks_queue_stage2(void){

    tasks_queue_t *q = (tasks_queue_t*) malloc(sizeof(tasks_queue_t));

    q->task_buf_size = QUEUE_CAPACITY;
    q->task_buffer = (task_t**) malloc(sizeof(task_t*) * q->task_buf_size);

    q->index = 0;

    return q;
}
tasks_queues_array_t* create_tasks_queue(void){
    tasks_queues_array_t* tab = ( tasks_queues_array_t* ) malloc( sizeof( tasks_queues_array_t ) );

    int nbth = THREAD_COUNT ;
    tasks_queue_t** array_q = (tasks_queue_t** ) malloc( nbth * sizeof( tasks_queue_t* ) );

    pthread_mutex_t* locks_a = malloc( nbth * sizeof( pthread_mutex_t ) );

    // We create the queues
    for ( int i = 0 ; i < nbth ; i++ ){
        *( array_q + i ) = create_tasks_queue_stage2() ;
        array_q[ i ]->id = i ;
        array_q[ i ]->steal_idx = -1 ;
        pthread_mutex_init( ( locks_a + i ) , NULL ); ;
    }
    tab->tab_queues = array_q ;
    tab->locks_array = locks_a ;
    tab->in = 0 ;
    tab->out = 0 ;


    return tab ;
}


void free_tasks_queue(tasks_queue_t *q)
{
    /* IMPORTANT: We chose not to free the queues to simplify the
     * termination of the program (and make debugging less complex) */
    
    /* free(q->task_buffer); */
    /* free(q); */
}


void enqueue_task( tasks_queue_t *q , task_t *t ) { // Producer
    int id = q->id ;

    pthread_mutex_lock( &tqueue->locks_array[ id ]  ) ;

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
    pthread_mutex_unlock( &tqueue->locks_array[ id ] ) ; // We give up the lock
}


task_t* dequeue_task( tasks_queue_t *q ) { // Consumer
    
    int id = q->id ;

    pthread_mutex_lock( &tqueue->locks_array[ id ] ) ; // We acquire the lock
    
    if( q->index == 0 ){  // nb elts >= 1 ?
        pthread_mutex_unlock( &tqueue->locks_array[ id ] ) ; 
        return NULL ;
    }
    
    // We consume an element : A free spot is now available
    task_t *t = q->task_buffer[ q->index - 1 ];
    if ( q->index > 0 ){
        q->index--;
    }

    pthread_mutex_unlock( &tqueue->locks_array[ id ] ) ; // Give up the lock

    return t;
}


task_t* steal_task( tasks_queue_t *q ) { // Consumer
    int id = q->id ;
    pthread_mutex_lock( &tqueue->locks_array[ id ] ) ; // We acquire the lock

    //pthread_t tid = pthread_self(); // For Debug purposes
    //printf("Stealing Thread ID: %lu\n", (unsigned long)tid);

    if ( q->index == 0 || q->steal_idx == -1 ){  // nb elts >= 1 ?
        pthread_mutex_unlock( &tqueue->locks_array[ id ] ) ; // Give up the lock
        return NULL ; 
    }
    
    // We consume an element : A free spot is now available
    task_t *t = q->task_buffer[ q->steal_idx ];
    q->steal_idx = ( q->steal_idx + 1 ) % THREAD_COUNT ;


    pthread_mutex_unlock( &tqueue->locks_array[ id ] ) ; // Give up the lock

    return t;
}

