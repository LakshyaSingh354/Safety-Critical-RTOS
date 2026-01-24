#include "kernel.h"
#include "release.h"
#include "task.h"
#include "scheduler.h"
#include "execute.h"
#include "idle.h"
#include "../drivers/gptm/timer.h"
#include "../metrics/metrics.h"
#include "../metrics/workload.h"

void kernel_time_tick_from_isr(void) {
    kernel_ticks++;
}

uint32_t kernel_time_now(void) {
    return kernel_ticks;
}

void kernel_init(void) {
    kernel_ticks = 0;
    // driver initialization
    timer_init();
    workload_init();
    metrics_init();
}

void kernel_run(void) {
    while (1) {
        metrics_poll(kernel_time_now());
        kernel_release_tasks();

        task_t *t = kernel_schedule();
        if (t) {
            kernel_execute_task(t);
        } else {
            kernel_idle();
        }
    }
}