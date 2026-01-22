#pragma once
#include <stdint.h>

void kernel_time_init(void);
void kernel_time_tick_from_isr(void);
uint32_t kernel_time_now(void);