#include "uart_tx_buf.h"

typedef struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    uint8_t buffer[UART_TX_BUFFER_SIZE];
} uart_tx_buffer_t;

static uart_tx_buffer_t tx_buf;

void uart_tx_buffer_init(void) {
    tx_buf.head = 0;
    tx_buf.tail = 0;
}

int uart_tx_buffer_put(uint8_t byte) {
    uint16_t next = (tx_buf.head + 1) & (UART_TX_BUFFER_SIZE - 1);

    if (next == tx_buf.tail) {
        return 0; // buffer full
    }

    tx_buf.buffer[tx_buf.head] = byte;
    tx_buf.head = next;
    return 1;
}

int uart_tx_buffer_get(uint8_t *byte) {
    if (tx_buf.head == tx_buf.tail) {
        return 0; // empty
    }

    *byte = tx_buf.buffer[tx_buf.tail];
    tx_buf.tail = (tx_buf.tail + 1) & (UART_TX_BUFFER_SIZE - 1);
    return 1;
}