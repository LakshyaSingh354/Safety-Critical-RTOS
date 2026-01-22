#include "idle.h"
#include "task.h"
#include "kernel.h"

extern task_t tasks[];
extern const uint32_t task_count;

static int any_task_ready(void) {
    for (uint32_t i = 0; i < task_count; i++) {
        if (tasks[i].state == TASK_READY) {
            return 1;
        }
    }
    return 0;
}

void kernel_idle(void) {
    if (any_task_ready()) {
        return;
    }

    /* sleep until interrupt */
    uint32_t before = kernel_time_now();
    __asm volatile ("wfi");
    uint32_t after = kernel_time_now();
    idle_ticks += (after - before);
}