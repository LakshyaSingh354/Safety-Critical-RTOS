#include "task.h"

void task_hog(void);
void task_victim(void);

task_t tasks[] = {
    {
        .name = "hog",
        .run = task_hog,
        .period_ticks = 10,
        .deadline_ticks = 10,
        .wcet_ticks = 2,
        .state = TASK_WAITING,
        .next_release = 10,
    },
    {
        .name = "victim",
        .run = task_victim,
        .period_ticks = 10,
        .deadline_ticks = 2,
        .wcet_ticks = 5,
        .state = TASK_WAITING,
        .next_release = 13,
    }
};

const uint32_t task_count = sizeof(tasks) / sizeof(tasks[0]);