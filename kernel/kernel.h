#pragma once
#include <stdint.h>

static volatile uint32_t kernel_ticks;
static volatile uint32_t idle_ticks;
static volatile uint32_t active_ticks;

void kernel_time_tick_from_isr(void);
uint32_t kernel_time_now(void);

void kernel_init(void);
void kernel_run(void);