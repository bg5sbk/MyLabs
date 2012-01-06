#ifndef _ACT_H_
#define _ACT_H_

#include <stdint.h>

#define act_pid int

typedef enum {
    ACT_RUNABLE,
    ACT_RUNNING,
    ACT_WAITING,
    ACT_EXIT
} act_status;

typedef void (*act_func)(void *);

act_pid act_spawn(act_func func, void *data);

act_pid act_spawn2(act_func func, void *data, int stack_size);

void act_exit();

act_pid act_self();

void act_yield();

void act_loop(int num_schedulers);

#endif