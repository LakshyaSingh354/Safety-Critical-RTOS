#include "uart_rx_buf.h"

// Single global RX buffer
static uart_rx_buffer_t rx_buf;

void uart_rx_buffer_init(void) {
    rx_buf.head = 0;
    rx_buf.tail = 0;
}

// Called ONLY from UART ISR
void uart_rx_buffer_put(uint8_t byte) {
    uint16_t next = (rx_buf.head + 1) & (UART_RX_BUFFER_SIZE - 1);

    if (next != rx_buf.tail) {
        rx_buf.buffer[rx_buf.head] = byte;
        rx_buf.head = next;
    }
    // else: buffer full → drop byte (intentional)
}

// Called ONLY from main context
int uart_rx_buffer_get(uint8_t *byte) {
    if (rx_buf.head == rx_buf.tail) {
        return 0; // empty
    }

    *byte = rx_buf.buffer[rx_buf.tail];
    rx_buf.tail = (rx_buf.tail + 1) & (UART_RX_BUFFER_SIZE - 1);
    return 1;
}