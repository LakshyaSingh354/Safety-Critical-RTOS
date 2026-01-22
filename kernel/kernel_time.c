#include "kernel_time.h"

static volatile uint32_t kernel_ticks;

void kernel_time_init(void) {
    kernel_ticks = 0;
}

void kernel_time_tick_from_isr(void) {
    kernel_ticks++;
}

uint32_t kernel_time_now(void) {
    return kernel_ticks;
}