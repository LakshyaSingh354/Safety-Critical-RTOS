#include "event.h"

event_queue_t event_queue = {0};

void event_post_from_isr(event_type_t type, uint32_t data) {
    uint8_t next = (event_queue.head + 1) % EVENT_QUEUE_SIZE;

    if (next != event_queue.tail) {
        event_queue.buffer[event_queue.head].type = type;
        event_queue.buffer[event_queue.head].data = data;
        event_queue.head = next;
    }
}

int event_get(event_t *evt) {
    if (event_queue.head == event_queue.tail) {
        return 0;
    }

    *evt = event_queue.buffer[event_queue.tail];
    event_queue.tail = (event_queue.tail + 1) % EVENT_QUEUE_SIZE;
    return 1;
}