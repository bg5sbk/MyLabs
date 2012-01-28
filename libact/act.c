#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
#include <ucontext.h>

#include "act.h"

//==============================================================================
//    Private
//==============================================================================

#define act_access_procs_begin() do{ \
    pthread_mutex_lock(&act_procs_mutex); \
}while(0)

#define act_access_procs_end() do{ \
    pthread_mutex_unlock(&act_procs_mutex); \
}while(0)

static pthread_mutex_t act_procs_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct act_proc act_proc_t;

struct act_proc {
    act_pid     pid;
    ucontext_t *ctx;
    act_status  status;
    
    act_proc_t *prev;
    act_proc_t *next;
};

static act_pid act_max_pid;

static act_proc_t *act_procs;

typedef struct act_sche act_sche_t;

struct act_sche {
  pthread_t   tid;
  ucontext_t *ctx;
  act_proc_t *running_proc;
};

static int act_sche_num;

static act_sche_t **act_sches;

static void *act_scheduler(void *data)
{
    int sche_id = (int)data;
    
    act_sche_t sche;
    
    ucontext_t main_ctx;

    act_proc_t *proc = NULL;
    act_proc_t *next_proc = act_procs;
    
    sche.ctx = &main_ctx;
    sche.tid = pthread_self();
    sche.running_proc = NULL;
    
    act_sches[sche_id] = &sche;
    
    //printf("scheduler %d begin\n", sche_id);
    
    //printf("thread id %ul\n", sche.tid);
    
    for (;;) {
        for (;;) {
            act_access_procs_begin();
        
            if (proc == NULL) {
                proc = act_procs;
            } else {
                proc = proc->next;
            }
            
            if (ACT_RUNABLE == proc->status) {
                proc->status = ACT_RUNNING;
                act_access_procs_end();
                break;
            }
            
            act_access_procs_end();
            
            //printf("scheduler %d wait\n", sche_id);
            
            //usleep(1000 * 10);
        }
        
        //printf("scheduler %d run proc %d begin\n", sche_id, proc->pid);
    
        sche.running_proc = proc;
        
        swapcontext(&main_ctx, proc->ctx);
        
        sche.running_proc = NULL;
        
        //printf("scheduler %d run proc %d end\n\n", sche_id, proc->pid);
        
        if (ACT_RUNNING == proc->status) {
            proc->status = ACT_RUNABLE;
        }
        else if (ACT_EXIT == proc->status) {
            act_access_procs_begin();
            
            proc->next->prev = proc->prev;
            proc->prev->next = proc->next;
            
            act_access_procs_end();
            
            free(proc->ctx->uc_stack.ss_sp);
            free(proc->ctx);
            free(proc);
            
            proc = NULL;
        }
    }
}

static act_sche_t *act_current_sche()
{
    int i;
    
    pthread_t tid;
    
    tid = pthread_self();
    
    for (i = 0; i < act_sche_num; i ++) {
        act_sche_t *sche = act_sches[i];
        
        if (sche != NULL && sche->tid == tid) {
            return sche;
        }
    }
    
    return NULL;
}

//==============================================================================
//    Public
//==============================================================================

int act_default_stack_size = 64 * 1024;

act_pid act_spawn(act_func func, void *data)
{
    return act_spawn2(func, data, act_default_stack_size);
}

act_pid act_spawn2(act_func func, void *data, int stack_size)
{
    act_proc_t *proc = NULL;
    
    proc = (act_proc_t *)malloc(sizeof(act_proc_t));

    proc->status = ACT_RUNABLE;
    
    proc->ctx = (ucontext_t *)malloc(sizeof(ucontext_t));
    proc->ctx->uc_stack.ss_sp = (char *)malloc(stack_size);
    proc->ctx->uc_stack.ss_size = stack_size;
    
    getcontext(proc->ctx);
    
    makecontext(proc->ctx, (void (*)(void))func, 1, data);
    
    act_access_procs_begin();
    
    proc->pid = ++ act_max_pid;
    
    if (act_procs != NULL) {
        proc->next = act_procs;
        proc->prev = act_procs->prev;
    
        proc->next->prev = proc;
        proc->prev->next = proc;
    }
    else {
        proc->next = proc;
        proc->prev = proc;
        
        act_procs = proc;
    }
    
    act_access_procs_end();
}

void act_exit()
{
    act_sche_t *sche = act_current_sche();
    
    sche->running_proc->status = ACT_EXIT;
}

act_pid act_self()
{
    act_sche_t *sche = act_current_sche();
    
    return sche->running_proc->pid;
}

void act_yield()
{
    act_sche_t *sche = act_current_sche();
    
    //printf("yield by thread %u\n", sche->tid);
    
    swapcontext(sche->running_proc->ctx, sche->ctx);
}

void act_loop(int num_schedulers)
{
    int i;
    int rc;
    void *status;
    
    pthread_mutex_init(&act_procs_mutex, NULL);
    
    act_sche_num = num_schedulers;
    
    act_sches = (act_sche_t **)malloc(num_schedulers * sizeof(act_sche_t *));
    
    pthread_attr_t attr;
    pthread_t schedulers[num_schedulers];
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   
    for (i = 0; i < num_schedulers; i ++) {
        act_sches[i] = NULL;
        
        if (rc = pthread_create(&schedulers[i], &attr, act_scheduler, (void *)i)) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    pthread_attr_destroy(&attr);
    
    for (i = 0; i < num_schedulers; i ++) {
        if (rc = pthread_join(schedulers[i], &status)) {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }
    
    pthread_exit(NULL);
}
