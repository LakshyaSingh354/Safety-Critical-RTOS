#include "kernel.h"
#include "../event_control/event.h"
#include "../drivers/gptm/timer.h"
#include "../drivers/uart/uart.h"
#include "../drivers/uart/uart_rx_buf.h"
#include "../drivers/uart/uart_tx_buf.h"

void kernel_init(void) {
    // temporary: old inits
    // driver initialization
    timer_init();
    uart_rx_buffer_init();
    uart_tx_buffer_init();
    uart_init();
}

void kernel_run(void) {
    // temporary: old event loop lives here for now
    event_t evt;
    if (event_get(&evt)) {
        switch (evt.type) {
                case EVENT_UART_RX: {
                    uint8_t byte;
                    while (uart_rx_buffer_get(&byte)) {
                        uart_send_byte(byte); // echo
                    }
                    break;
                }
        }
    }
}