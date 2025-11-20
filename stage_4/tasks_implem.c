#include <stdio.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "debug.h"

#include <stdlib.h>
#include <pthread.h>

tasks_queues_array_t* tqueue = NULL;

extern int nbTasks ;

extern pthread_mutex_t mtx ;
pthread_mutex_t mtx_size ;
extern pthread_mutex_t mtx_dep_count ;
extern pthread_mutex_t m_creation ;
pthread_mutex_t mtx_in ;
pthread_mutex_t mtx_out ;


extern pthread_cond_t finish ;
extern pthread_cond_t notEmpty ;

void create_queues( void ) {

    tqueue = create_tasks_queue();

    // We init the mutex locks
    pthread_mutex_init( &mtx , NULL );
    pthread_mutex_init( &mtx_size , NULL );
    pthread_mutex_init( &mtx_dep_count , NULL );
    pthread_mutex_init( &m_creation , NULL );
    pthread_mutex_init( &mtx_in , NULL );
    pthread_mutex_init( &mtx_out , NULL );


    // Iniit of the conditional variables
    pthread_cond_init( &notEmpty , NULL ) ;
    pthread_cond_init( &finish , NULL ) ;

    nbTasks = 0 ;

}

void delete_queues(void)
{
    /*for ( int i = 0 ; i < THREAD_COUNT ; i++ ){
        free_tasks_queue( tqueue->tab_queues[ i ] );
    }
    for ( int i = 0 ; i < THREAD_COUNT ; i++ ){
        pthread_mutex_destroy( &tqueue->locks_array[ i ] ) ;
    }
    free( tqueue->locks_array );
    free( tqueue );*/
    
}    

void create_thread_pool(void){
    
    pthread_t *tids ;
    int nb_threads = THREAD_COUNT ; // THREAD_COUNT IN MAKEFILE.CONFIG ;

    tids = malloc ( nb_threads * sizeof( pthread_t ) ) ;

    int* my_ids = malloc ( nb_threads * sizeof( int ) ) ;

    /* Create the threads */
    /*
    In this case we will "create" a distinct integer that will serve as an id for each of the threads we will create
    */
    for ( int i = 1 ; i <= nb_threads ; i++ ){
        *( my_ids + i - 1 ) = i - 1 ; 
        pthread_create ( &tids[ i - 1 ] , NULL , work_thread , &my_ids[ i - 1 ] ) ;
        printf( "T%d = Created !\n", i );
    }
    
    return ;
}


void dispatch_task( task_t* t ) {
    enqueue_task( tqueue->tab_queues[ tqueue->in ] , t ) ;
    pthread_mutex_lock( &mtx_in ) ;
    tqueue->in = ( tqueue->in + 1 ) % THREAD_COUNT ;
    pthread_mutex_unlock( &mtx_in ) ;
}

task_t* get_task_to_execute( int id  ) { 
    task_t* t = dequeue_task( tqueue->tab_queues[ id ]  );
    return t ;
}

unsigned int exec_task(task_t *t)
{

    //pthread_t tid = pthread_self() % 1296103165068 ; // For Debug purposes
    //printf("\nThread ID: %lu\n", (unsigned long)tid);

    active_task = t ;
    t->step++;
    t->status = RUNNING;

    PRINT_DEBUG(10, "Execution of task %u (step %u)\n", t->task_id, t->step);
    
    unsigned int result = t->fct(t, t->step);
    
    return result;
}

void terminate_task(task_t *t  )
{
    t->status = TERMINATED;
    PRINT_DEBUG(10, "Task terminated: %u\n", t->task_id);


#ifdef WITH_DEPENDENCIES
    pthread_mutex_lock( &mtx_dep_count ) ;
    if(t->parent_task != NULL){
        task_t *waiting_task = t->parent_task;
        waiting_task->task_dependency_done++;
        if ( waiting_task->status == WAITING ){
            task_check_runnable(waiting_task);
        }
        
    }
    pthread_mutex_unlock( &mtx_dep_count ) ;
#endif
    pthread_mutex_lock( &mtx_size ) ;
    nbTasks-- ;
    if ( nbTasks == 0 ){ // We send a signal to the main thread : You can keep going.
        pthread_cond_broadcast( &finish );
    }
    pthread_mutex_unlock( &mtx_size ) ;



}
/*
    unsigned int task_dependency_count;  number of tasks this task depends on 
    
    unsigned int task_dependency_done;    number of solved dependencies 
    
    struct task *parent_task;      task that depends on this task 
*/

void task_check_runnable(task_t *t )
{
#ifdef WITH_DEPENDENCIES
    if(t->task_dependency_done == t->task_dependency_count){
        t->status = READY;
        dispatch_task(t);
    }
#endif
}