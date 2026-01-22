#include "kernel.h"
#include "release.h"
#include "task.h"
#include "scheduler.h"
#include "../drivers/gptm/timer.h"

void kernel_init(void) {
    // driver initialization
    timer_init();
}

void kernel_run(void) {
    while (1) {
        kernel_release_tasks();

        task_t *t = kernel_schedule();
        if (t) {
            /* execution comes next */
        }

        /* idle comes later */
    }
}