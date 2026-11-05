#ifndef SYSTICK_H
#define SYSTICK_H

#include<stdio.h>
#include<stdint.h>


// SysTick Timer Registers

typedef struct{
    volatile uint32_t  SYST_CSR;                      // 0x00 SysTick Control and Status Register
    volatile uint32_t  SYST_RVR;                      // 0x04 SysTick Reload Value Register
    volatile uint32_t  SYST_CVR;                      // 0x08 SysTick Current Value Register
    volatile uint32_t  SYST_CALIB;                    // 0x0c SysTick Calibration Value Register
}Systick_Type;

#define Systick      ((Systick_Type *)0xE000E010UL)

static inline void Systick_Enable_Init(void)
{
    Systick->SYST_CVR = 0;
    Systick->SYST_RVR = (48000 - 1);             // FRO 48MHz 48000000 / 1000 ms = 48000 counts per millisecond
    Systick->SYST_CSR |= (1UL << 0);          // ENABLE      Counter is disabled=0  Counter is enabled=1
    Systick->SYST_CSR |= (1UL << 1);           // TICK INT will enable the SysTick exception.
    Systick->SYST_CSR |= (1UL << 2);          // CLKSOURCE  
}

#endif