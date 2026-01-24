#pragma once
#include "task.h"

# define SCHED_FCFS 0
# define SCHED_RMS 1
# define SCHED_EDF 2

#define KERNEL_SCHED_POLICY SCHED_EDF

task_t *kernel_schedule(void);