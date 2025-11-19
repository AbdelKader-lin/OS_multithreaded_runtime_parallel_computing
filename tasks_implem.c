#include <stdio.h>

#include "tasks_implem.h"
#include "tasks_queue.h"
#include "debug.h"

#include <stdlib.h>
#include <pthread.h>
#include "tasks.h"


extern __thread task_t *active_task;

extern int pending_tasks;
extern pthread_mutex_t pending_mutex;
extern pthread_cond_t finish;

tasks_queue_t *tqueue= NULL;

extern int nbElts ;

void create_queues( void ) {
    tqueue = create_tasks_queue();
}

void delete_queues(void)
{
    free_tasks_queue(tqueue);
}    

void create_thread_pool(void){
    
    pthread_t *tids ;
    int nb_threads = THREAD_COUNT ; // THREAD_COUNT IN MAKEFILE.CONFIG ;

    tids = malloc ( nb_threads * sizeof( pthread_t ) ) ;

    /* Create the threads */
    for ( int i = 1 ; i <= nb_threads ; i++ ){
        pthread_create ( &tids[i] , NULL , work_thread , NULL ) ;
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

    PRINT_DEBUG(10, "Execution of task %u (step %u)\n",
                t->task_id, t->step);

    task_t *previous = active_task;
    active_task = t;

    unsigned int result = t->fct(t, t->step);

    active_task = previous;

    return result;
}


void terminate_task(task_t *t)
{
    t->status = TERMINATED;

#ifdef WITH_DEPENDENCIES
    if (t->parent_task != NULL) {
        task_t *p = t->parent_task;
        p->task_dependency_done++;

        task_check_runnable(p);
    }
#endif

    pthread_mutex_lock(&pending_mutex);
    pending_tasks--;
    if (pending_tasks == 0)
        pthread_cond_broadcast(&finish);
    pthread_mutex_unlock(&pending_mutex);
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