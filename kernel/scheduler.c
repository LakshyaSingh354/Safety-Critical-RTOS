#include "scheduler.h"
#include "task.h"
extern task_t tasks[];
extern const uint32_t task_count;

task_t *kernel_schedule(void) {
    for (uint32_t i = 0; i < task_count; i++) {
        task_t *t = &tasks[i];

        if (t->state == TASK_READY) {
            t->state = TASK_RUNNING;
            return t;
        }
    }

    return (void*)0; // nothing to run
}