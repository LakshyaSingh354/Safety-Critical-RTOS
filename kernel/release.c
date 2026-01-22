#include "release.h"
#include "task.h"
#include "kernel_time.h"

extern task_t tasks[];
extern const uint32_t task_count;

void kernel_release_tasks(void) {
    uint32_t now = kernel_time_now();

    for (uint32_t i = 0; i < task_count; i++) {
        task_t *t = &tasks[i];

        if (t->state == TASK_WAITING && now >= t->next_release) {
            t->state = TASK_READY;

            /*
             * Schedule next release.
             * This preserves periodic behavior even if releases are delayed.
             */
            t->next_release += t->period_ticks;
        }
    }
}