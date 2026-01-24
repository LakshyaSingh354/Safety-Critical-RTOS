#include "execute.h"
#include "kernel.h"
#include "../metrics/metrics.h"
#include "../metrics/metrics_config.h"

void kernel_execute_task(task_t *t) {
    uint32_t start = kernel_time_now();
    t->last_start = start;
    metrics_on_task_start(t, start);

    /* Execute task */
    t->run();

    uint32_t end = kernel_time_now();
    t->last_finish = end;

    t->executions++;

    /* WCET violation detection */
    uint32_t exec_time = end - start;
    uint8_t wcet_violation = (exec_time > t->wcet_ticks);
    uint8_t deadline_miss = 0;
    if (!wcet_violation) {
        /* Deadline miss detection */
        if (end > (t->next_release - t->period_ticks + t->deadline_ticks)) {
            deadline_miss = 1;
        }
    }

    metrics_on_task_end(t, start, end, wcet_violation, deadline_miss);

    if (wcet_violation) {
        /* Safety violation */
        #if METRICS_ENABLED
            t->state = TASK_WAITING;
            return;
        #else
            t->state = TASK_DEADLINE_MISSED;
            return;
        #endif
    }

    /* Deadline miss detection */
    if (deadline_miss) {
        t->deadline_misses++;
        t->state = TASK_DEADLINE_MISSED;
        return;
    }
    active_ticks += (end - start);
    /* Task completed successfully */
    t->state = TASK_WAITING;
}