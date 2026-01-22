#include "task.h"

void task_control(void);
void task_monitor(void);

task_t tasks[] = {
    {
        .name = "control",
        .run = task_control,
        .period_ticks = 10,
        .deadline_ticks = 10,
        .wcet_ticks = 2,
        .state = TASK_WAITING,
        .next_release = 10,
    },
    {
        .name = "monitor",
        .run = task_monitor,
        .period_ticks = 50,
        .deadline_ticks = 50,
        .wcet_ticks = 5,
        .state = TASK_WAITING,
        .next_release = 50,
    }
};

const uint32_t task_count = sizeof(tasks) / sizeof(tasks[0]);