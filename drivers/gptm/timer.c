#include "timer.h"
#include "../../event_control/event.h"
#include "../../kernel/kernel.h"

#define TIMER0_BASE     0x40030000

#define GPTM_CFG        (*(volatile uint32_t *)(TIMER0_BASE + 0x000))
#define GPTM_TAMR       (*(volatile uint32_t *)(TIMER0_BASE + 0x004))
#define GPTM_CTL        (*(volatile uint32_t *)(TIMER0_BASE + 0x00C))
#define GPTM_IMR        (*(volatile uint32_t *)(TIMER0_BASE + 0x018))
#define GPTM_ICR        (*(volatile uint32_t *)(TIMER0_BASE + 0x024))
#define GPTM_TAILR      (*(volatile uint32_t *)(TIMER0_BASE + 0x028))

#define SYSCTL_RCGCTIMER (*(volatile uint32_t *)0x400FE104) // bit 16 for Timer 0

#define NVIC_EN0 (*(volatile uint32_t *)0xE000E100)
#define TIMER0A_IRQ 19

void timer_init(void) {
    // Enable clock to Timer0
    SYSCTL_RCGCTIMER |= (1 << 16);
    volatile uint32_t dummy = SYSCTL_RCGCTIMER;

    // Disable timer during setup
    GPTM_CTL = 0x0;

    // 32-bit timer
    GPTM_CFG = 0x0;

    // Periodic mode, count-down
    GPTM_TAMR = 0x2;

    // Load value 
    GPTM_TAILR = 400000;

    // Enable timeout interrupt
    GPTM_IMR |= (1 << 0);

    // Enable Timer0A interrupt in NVIC
    NVIC_EN0 |= (1 << TIMER0A_IRQ);

    // Enable timer
    GPTM_CTL |= (1 << 0);
}

void Timer0A_Handler(void) {
    GPTM_ICR = 1;
    kernel_time_tick_from_isr();;
}

