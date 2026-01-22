#include "kernel/kernel.h"

int main(void) {
    kernel_init();

    while (1) {
        kernel_run();
    }
}