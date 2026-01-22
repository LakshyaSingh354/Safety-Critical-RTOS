#include "kernel.h"
#include "release.h"
#include "../drivers/gptm/timer.h"

void kernel_init(void) {
    // driver initialization
    timer_init();
}

void kernel_run(void) {
    kernel_release_tasks();
}