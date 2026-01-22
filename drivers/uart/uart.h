#pragma once
#include <stdint.h>

void uart_init(void);
void uart_send_byte(uint8_t byte);