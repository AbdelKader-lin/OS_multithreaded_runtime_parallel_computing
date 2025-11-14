#include <stdio.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "debug.h"

#include <stdlib.h>
#include <pthread.h>

tasks_queue_t *tqueue= NULL;

extern int nbTasks ;

extern pthread_mutex_t mtx ;
pthread_mutex_t mtx_size ;
//extern pthread_mutex_t m ;

extern pthread_cond_t finish ;
extern pthread_cond_t notEmpty ;
extern pthread_cond_t notFull ;

void create_queues( void ) {

    tqueue = create_tasks_queue();

    // We init the mutex locks
    pthread_mutex_init( &mtx , NULL );
    pthread_mutex_init( &mtx_size , NULL );

    // Iniit of the conditional variables
    pthread_cond_init( &notEmpty , NULL ) ;
    pthread_cond_init( &notFull , NULL ) ;
    pthread_cond_init( &finish , NULL ) ;

}

void delete_queues(void)
{
    free_tasks_queue(tqueue);
}    

void create_thread_pool(void){
    
    pthread_t *tids ;
    int nb_threads = THREAD_COUNT ; // THREAD_COUNT IN MAKEFILE.CONFIG ;
    //pthread_t *tids = malloc( nb_threads * sizeof( *tids ) );

    tids = malloc ( nb_threads * sizeof( pthread_t ) ) ;

    /* Create the threads */
    for ( int i = 1 ; i <= nb_threads ; i++ ){
        pthread_create ( &tids[ i - 1 ] , NULL , work_thread , NULL ) ;
        printf( "T%d = Created !\n", i );
    }
    
    return ;
}


void dispatch_task(task_t *t)
{
    enqueue_task(tqueue, t);
}

task_t* get_task_to_execute(void)
{
    return dequeue_task(tqueue);
}

unsigned int exec_task(task_t *t)
{
    t->step++;
    t->status = RUNNING;

    PRINT_DEBUG(10, "Execution of task %u (step %u)\n", t->task_id, t->step);
    
    unsigned int result = t->fct(t, t->step);
    
    return result;
}

void terminate_task(task_t *t)
{
    t->status = TERMINATED;

    pthread_mutex_lock( &mtx_size ) ;
    nbTasks-- ;
    if ( nbTasks == 0 ){ // We send a signal to the main thread : You can keep going.
        pthread_cond_broadcast( &finish );
    }
    pthread_mutex_unlock( &mtx_size ) ;

    
    PRINT_DEBUG(10, "Task terminated: %u\n", t->task_id);

#ifdef WITH_DEPENDENCIES
    if(t->parent_task != NULL){
        task_t *waiting_task = t->parent_task;
        waiting_task->task_dependency_done++;
        
        task_check_runnable(waiting_task);
    }
#endif

}

void task_check_runnable(task_t *t)
{
#ifdef WITH_DEPENDENCIES
    if(t->task_dependency_done == t->task_dependency_count){
        t->status = READY;
        dispatch_task(t);
    }
#endif
}