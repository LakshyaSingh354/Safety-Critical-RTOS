#include <stdint.h>

void task_hog(void) {
    volatile uint32_t count = 0;
    for (uint32_t i = 0; i < 1000000; i++){
        count++;
    }
}

void task_victim(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < 10; i++){
        count++;
    }
    return;
}