#pragma once
#include <stdint.h>

#define UART_TX_BUFFER_SIZE 128

#if (UART_TX_BUFFER_SIZE & (UART_TX_BUFFER_SIZE - 1)) != 0
#error "UART_TX_BUFFER_SIZE must be power of two"
#endif

void uart_tx_buffer_init(void);
int uart_tx_buffer_put(uint8_t byte);
int uart_tx_buffer_get(uint8_t *byte);