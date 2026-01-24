#include "task.h"
#include "../metrics/metrics_config.h"
#include "../metrics/workload.h"

#if WORKLOAD_TASK_COUNT < 1
#error "WORKLOAD_TASK_COUNT must be at least 1"
#endif

#if WORKLOAD_TASK_COUNT > 3
#error "WORKLOAD_TASK_COUNT exceeds configured tasks"
#endif

task_t tasks[] = {
#if WORKLOAD_TASK_COUNT >= 1
    {
        .name = WORKLOAD_TASK0_NAME,
        .run = workload_task0,
        .period_ticks = WORKLOAD_TASK0_PERIOD_TICKS,
        .deadline_ticks = WORKLOAD_TASK0_DEADLINE_TICKS,
        .wcet_ticks = WORKLOAD_TASK0_WCET_TICKS,
        .state = TASK_WAITING,
        .next_release = WORKLOAD_TASK0_START_OFFSET,
    },
#endif
#if WORKLOAD_TASK_COUNT >= 2
    {
        .name = WORKLOAD_TASK1_NAME,
        .run = workload_task1,
        .period_ticks = WORKLOAD_TASK1_PERIOD_TICKS,
        .deadline_ticks = WORKLOAD_TASK1_DEADLINE_TICKS,
        .wcet_ticks = WORKLOAD_TASK1_WCET_TICKS,
        .state = TASK_WAITING,
        .next_release = WORKLOAD_TASK1_START_OFFSET,
    },
#endif
#if WORKLOAD_TASK_COUNT >= 3
    {
        .name = WORKLOAD_TASK2_NAME,
        .run = workload_task2,
        .period_ticks = WORKLOAD_TASK2_PERIOD_TICKS,
        .deadline_ticks = WORKLOAD_TASK2_DEADLINE_TICKS,
        .wcet_ticks = WORKLOAD_TASK2_WCET_TICKS,
        .state = TASK_WAITING,
        .next_release = WORKLOAD_TASK2_START_OFFSET,
    }
#endif
};

const uint32_t task_count = sizeof(tasks) / sizeof(tasks[0]);