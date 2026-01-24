#include "workload.h"
#include "metrics_config.h"
#include "../kernel/kernel.h"

static volatile uint32_t workload_sink;
static uint32_t workload_exec_counts[WORKLOAD_TASK_COUNT];

void workload_init(void) {
    for (uint32_t i = 0; i < WORKLOAD_TASK_COUNT; i++) {
        workload_exec_counts[i] = 0;
    }
}

static uint32_t workload_base_iters(uint32_t task_id) {
    switch (task_id) {
        case 0:
            return WORKLOAD_TASK0_BASE_ITERS;
        case 1:
            return WORKLOAD_TASK1_BASE_ITERS;
        case 2:
            return WORKLOAD_TASK2_BASE_ITERS;
        default:
            return 0;
    }
}

static uint32_t workload_iterations(uint32_t task_id, uint32_t now) {
    uint32_t base = workload_base_iters(task_id);

#if WORKLOAD_PROFILE == WORKLOAD_PROFILE_STEP
    if (((now / WORKLOAD_STEP_PERIOD_TICKS) & 1u) != 0u) {
        return base * WORKLOAD_STEP_MULTIPLIER;
    }
    return base;
#elif WORKLOAD_PROFILE == WORKLOAD_PROFILE_RAMP
    uint32_t phase = (now / WORKLOAD_RAMP_STEP_TICKS) % WORKLOAD_RAMP_STEPS;
    return base * (phase + 1u);
#elif WORKLOAD_PROFILE == WORKLOAD_PROFILE_SPIKE
    if ((now % WORKLOAD_SPIKE_PERIOD_TICKS) == 0u) {
        return base * WORKLOAD_SPIKE_MULTIPLIER;
    }
    return base;
#else
    (void)now;
    return base;
#endif
}

static void workload_spin(uint32_t iterations, uint32_t task_id) {
    for (uint32_t i = 0; i < iterations; i++) {
        workload_sink += (i ^ task_id);
    }
}

static void workload_spin_ticks(uint32_t ticks) {
    uint32_t start = kernel_time_now();
    while ((kernel_time_now() - start) < ticks) {
        __asm volatile("nop");
    }
}

static void workload_task_run(uint32_t task_id) {
    if (task_id >= WORKLOAD_TASK_COUNT) {
        return;
    }
    uint32_t now = kernel_time_now();
    uint32_t iterations = workload_iterations(task_id, now);
    workload_exec_counts[task_id]++;
    // workload_spin(iterations, task_id);
    workload_spin_ticks(iterations/50000);
}

void workload_task0(void) {
    workload_task_run(0);
}

void workload_task1(void) {
    workload_task_run(1);
}

void workload_task2(void) {
    workload_task_run(2);
}
