#include "execute.h"
#include "kernel_time.h"

void kernel_execute_task(task_t *t) {
    uint32_t start = kernel_time_now();
    t->last_start = start;

    /* Execute task */
    t->run();

    uint32_t end = kernel_time_now();
    t->last_finish = end;

    t->executions++;

    /* WCET violation detection */
    uint32_t exec_time = end - start;
    if (exec_time > t->wcet_ticks) {
        /* Safety violation */
        t->state = TASK_DEADLINE_MISSED;
        return;
    }

    /* Deadline miss detection */
    if (end > (t->next_release - t->period_ticks + t->deadline_ticks)) {
        t->deadline_misses++;
        t->state = TASK_DEADLINE_MISSED;
        return;
    }

    /* Task completed successfully */
    t->state = TASK_WAITING;
}