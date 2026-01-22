.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global _stack_top

.extern _data_load
.extern _data_start
.extern _data_end
.extern _bss_start
.extern _bss_end

/* ---------------- VECTOR TABLE ---------------- */
.section .isr_vector, "a", %progbits
.word _stack_top            /* 0 */
.word Reset_Handler         /* 1 */
.word Default_Handler       /* 2  NMI */
.word HardFault_Handler     /* 3 */

.word Default_Handler       /* 4  MemManage */
.word Default_Handler       /* 5  BusFault */
.word Default_Handler       /* 6  UsageFault */
.word Default_Handler       /* 7  Reserved */
.word Default_Handler       /* 8  Reserved */
.word Default_Handler       /* 9  Reserved */
.word Default_Handler       /* 10 Reserved */
.word Default_Handler       /* 11 SVCall */
.word Default_Handler       /* 12 Debug */
.word Default_Handler       /* 13 Reserved */
.word Default_Handler       /* 14 PendSV */
.word Default_Handler       /* 15 SysTick */

.rept 5                     /* IRQ0–IRQ4 */
.word Default_Handler
.endr

.word UART0_Handler         /* IROQ5 */

.rept 13
.word Default_Handler
.endr

.word Timer0A_Handler       /* IRQ19 → vector 35 */


.section .text
.thumb_func
Reset_Handler:
    ldr r0, = _data_load;
    ldr r1, = _data_start;
    ldr r2, = _data_end;

copy_data:
    cmp r1, r2
    ittt lt
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt copy_data

    /* Zero .bss */
    ldr r0, =_bss_start
    ldr r1, =_bss_end
    movs r2, #0

zero_bss:
    cmp r0, r1
    itt lt
    strlt r2, [r0], #4
    addlt r0, r0, #4
    blt zero_bss

    bl main
    b .                    /* NEVER return */

.thumb_func
HardFault_Handler:
    b .                    /* Trap forever */

.thumb_func
Default_Handler:
    b .
