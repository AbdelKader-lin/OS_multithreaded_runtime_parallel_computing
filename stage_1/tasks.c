#include <stdlib.h>

#include "tasks.h"
#include "tasks_implem.h"
#include "debug.h"
#include "utils.h"

#include <pthread.h>

system_state_t sys_state;

__thread task_t *active_task;

//int i = 0 ;
//pthread_mutex_t m ;

extern pthread_cond_t notEmpty ;
extern pthread_cond_t notFull ;

extern int nbElts ;


void runtime_init(void)
{
    /* a random number generator might be useful towards the end of
       the lab */
    rand_generator_init();

    create_queues();
    //printf("Avant pool\n");
    create_thread_pool();
    //printf("Apres pool\n");

    sys_state.task_counter = 0;    
}

void runtime_init_with_deps(void)
{
#ifndef WITH_DEPENDENCIES
    fprintf(stderr, "ERROR: dependencies are not supported by the runtime. This application cannot be executed\n");
    exit(EXIT_FAILURE);
#endif

    runtime_init();
}



void runtime_finalize(void) {
    /*
    The idea is that we already wait for the work to finish 
    when we pass ptr_taask_waitall to the created thread
    */

    task_waitall();


    pthread_cond_broadcast( &notEmpty ) ;
    pthread_cond_broadcast( &notFull ) ;


    PRINT_DEBUG(1, "Terminating ... \t Total task count: %lu \n", sys_state.task_counter);
    
    delete_queues();
    

}


task_t* create_task(task_routine_t f)
{
    task_t *t = malloc(sizeof(task_t));

    t->task_id = ++sys_state.task_counter;    
    t->fct = f;
    t->step = 0;

    t->tstate.input_list = NULL;
    t->tstate.output_list = NULL;

#ifdef WITH_DEPENDENCIES
    t->tstate.output_from_dependencies_list = NULL;
    t->task_dependency_count = 0;
    t->parent_task = NULL;
#endif
    
    t->status = INIT;
    
    PRINT_DEBUG(10, "task created with id %u\n", t->task_id);    
    
    return t;
}

void submit_task(task_t *t)
{
    t->status = READY;

#ifdef WITH_DEPENDENCIES    
    if(active_task != NULL){
        t->parent_task = active_task;
        active_task->task_dependency_count++;
        
        PRINT_DEBUG(100, "Dependency %u -> %u\n", active_task->task_id, t->task_id);
    }
#endif
    
    dispatch_task(t);
}

void task_waitall(void) {
    task_t* active_tk = get_task_to_execute();
    printf("OK !\n");

    while(active_tk != NULL){
        //i++ ;
        //printf("\ni = %d\n",i);
        //printf("Avant exec !\n");
        task_return_value_t ret = exec_task(active_tk);
	    //printf("Apres exec !\n");
        if (ret == TASK_COMPLETED){
            terminate_task(active_tk);
	    //printf("Apres Task terminated !\n");
        }
#ifdef WITH_DEPENDENCIES
        else{
            active_tk->status = WAITING;
        }
#endif
	    //printf("Juste avant next elt!\n");
        if ( nbElts != 0 ){
            active_tk = get_task_to_execute();
        } else {
            break;
        }
        
	    //printf("OK !\n");
    }
}
