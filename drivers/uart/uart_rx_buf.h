#pragma once
#include <stdint.h>

#define UART_RX_BUFFER_SIZE 128

#if (UART_RX_BUFFER_SIZE & (UART_RX_BUFFER_SIZE - 1)) != 0
#error "UART_RX_BUFFER_SIZE must be power of two"
#endif

typedef struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    uint8_t buffer[UART_RX_BUFFER_SIZE];
} uart_rx_buffer_t;

void uart_rx_buffer_init(void);

// ISR-side
void uart_rx_buffer_put(uint8_t byte);

// main-side
int uart_rx_buffer_get(uint8_t *byte);