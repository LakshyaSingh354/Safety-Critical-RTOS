#include "scheduler.h"
#include "task.h"
extern task_t tasks[];
extern const uint32_t task_count;

task_t *kernel_schedule(void) {
#if KERNEL_SCHED_POLICY == SCHED_FCFS
    for (uint32_t i = 0; i < task_count; i++) {
        if (tasks[i].state == TASK_READY) {
            tasks[i].state = TASK_RUNNING;
            return &tasks[i];
        }
    }
    return (void *)0;

#elif KERNEL_SCHED_POLICY == SCHED_RMS
    task_t *best = (void *)0;

    for (uint32_t i = 0; i < task_count; i++) {
        task_t *t = &tasks[i];

        if (t->state != TASK_READY) {
            continue;
        }

        if (best == (void *)0 ||
            t->period_ticks < best->period_ticks) {
            best = t;
        }
    }

    if (best) {
        best->state = TASK_RUNNING;
    }

    return best;

#elif KERNEL_SCHED_POLICY == SCHED_EDF
    task_t *best = (void *)0;

    for (uint32_t i = 0; i < task_count; i++) {
        task_t *t = &tasks[i];

        if (t->state != TASK_READY) {
            continue;
        }

        if (best == (void *)0 ||
            t->abs_deadline < best->abs_deadline) {
            best = t;
        }
    }

    if (best) {
        best->state = TASK_RUNNING;
    }

    return best;
#endif
}