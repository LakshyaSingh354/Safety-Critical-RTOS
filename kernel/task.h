#pragma once
#include <stdint.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_DEADLINE_MISSED
} task_state_t;

typedef struct task {
    /* Task identity */
    const char *name;
    void (*run)(void);

    /* Real-time parameters */
    uint32_t period_ticks;
    uint32_t deadline_ticks;
    uint32_t wcet_ticks;

    /* Kernel-managed state */
    uint32_t next_release;
    uint32_t last_start;
    uint32_t last_finish;

    task_state_t state;

    /* Accounting */
    uint32_t executions;
    uint32_t deadline_misses;
} task_t;