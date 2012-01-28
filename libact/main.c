#include <stdio.h>
#include <stdlib.h>

#include "act.h"

void proc(void *data)
{
    for (;;) {
        printf("%s\n", (char *)data);
        act_yield();
    }

    act_exit();
}

void main(void)
{
    act_spawn(proc, "A");
    act_spawn(proc, " B");
    act_spawn(proc, "  C");
    act_spawn(proc, "   D");
    act_spawn(proc, "    E");

    act_loop(3);
}
