#include <stdint.h>

typedef enum {
    EVENT_NONE = 0,
    EVENT_UART_RX,
} event_type_t;


typedef struct {
    event_type_t type;
    uint32_t data;
} event_t;

#define EVENT_QUEUE_SIZE 16

#if (EVENT_QUEUE_SIZE & (EVENT_QUEUE_SIZE - 1)) != 0
#error "EVENT_QUEUE_SIZE must be a power of two"
#endif

typedef struct {
    volatile uint8_t head;
    volatile uint8_t tail;
    event_t buffer[EVENT_QUEUE_SIZE];
} event_queue_t;

extern event_queue_t event_queue;

void event_post_from_isr(event_type_t type, uint32_t data);
int event_get(event_t *evt);