#include <stdint.h>
#include<MCXN236.h>

/* Symbols provided by the linker script */
extern uint32_t _sidata;   /* .data load address (flash) */
extern uint32_t _sdata;    /* .data start (RAM)          */
extern uint32_t _edata;    /* .data end (RAM)            */
extern uint32_t _sbss;     /* .bss start (RAM)           */
extern uint32_t _ebss;     /* .bss end (RAM)             */
extern uint32_t _estack;   /* top of stack               */

/* ---- Default handler for all other exceptions ---- */
void Default_Handler(void) {
    while (1) { }
}

/* ---- Weak aliases: real handlers can override these ---- */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));    
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void CTIMER0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));  /* Weak alias for CTIMER0 interrupt handler */

/* ---- Vector table (minimal: 16 system exceptions) ---- */

__attribute__((section(".isr_vector")))
const uint32_t vector_table[] = {
    (uint32_t)&_estack,          /* 0  Initial Stack Pointer */
    (uint32_t)Reset_Handler,     /* 1  Reset                 */
    (uint32_t)NMI_Handler,       /* 2  NMI                   */
    (uint32_t)HardFault_Handler, /* 3  HardFault             */
    (uint32_t)MemManage_Handler, /* 4                        */
    (uint32_t)BusFault_Handler,  /* 5                        */
    (uint32_t)UsageFault_Handler,/* 6                        */
    0, 0, 0, 0,                  /* 7-10 Reserved            */
    (uint32_t)SVC_Handler,       /* 11 SVCall                */
    (uint32_t)DebugMon_Handler,  /* 12                       */
    0,                           /* 13 Reserved              */
    (uint32_t)PendSV_Handler,    /* 14 PendSV                */
    (uint32_t)SysTick_Handler,   /* 15 SysTick               */
    /* Peripheral IRQs would follow here (index 16+) */
    0,                            /* 16 */
    0,                            /* 17 */
    0,                            /* 18 */
    0,                            /* 19 */
    0,                            /* 20 */
    0,                            /* 21 */
    0,                            /* 22 */
    0,                            /* 23 */
    0,                            /* 24 */
    0,                            /* 25 */
    0,                            /* 26 */
    0,                            /* 27 */
    0,                            /* 28 */
    0,                            /* 29 */
    0,                            /* 30 */
    0,                            /* 31 */
    0,                            /* 32 */
    0,                            /* 33 */
    0,                            /* 34 */
    0,                            /* 35 */
    0,                            /* 36 */
    0,                            /* 37 */
    0,                            /* 38 */
    0,                            /* 39 */
    0,                            /* 40 */
    0,                            /* 41 */
    0,                            /* 42 */
    0,                            /* 43 */
    0,                            /* 44 */
    0,                            /* 45 */
    0,                            /* 46 */

    (uint32_t)CTIMER0_IRQHandler  /* 47 */
};

/* ---- Reset Handler ---- */
void Reset_Handler(void) {

    uint32_t *src, *dst;

    /* 1. Copy .data from flash to RAM */
    src = &_sidata;
    dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    /* 2. Zero-initialize .bss */
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }



    /* 3. Call main() */
    main();

    /* 4. Never return */
    while (1) { }
}

