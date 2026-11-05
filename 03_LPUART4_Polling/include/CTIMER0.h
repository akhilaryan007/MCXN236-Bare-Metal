#ifndef CTIMER0_H
#define CTIMER0_H

#include"MCXN236.h"
#include<stdint.h>
#include "MRT.h"


/*---------------------------CTIMER INITIALIZATION------------------------------------------*/

volatile uint32_t CTIMER0_TICK_COUNT = 0;  // Global variable to keep track of the number of ticks
uint32_t clock_hz = 48000000U;  // 48MHz
uint32_t period_us = 500U;  // Counter for the number of ticks 48000000/1000 = 48000 ticks for 1ms, 48000*0.5 = 24000 ticks for 0.5ms


static inline void CTIMER0_INIT_CLOCK(void)
{

    PRESETCTRLSET1 = (1U << 27);
    (void)PRESETCTRLSET1;
 

    PRESETCTRLCLR1 = (1U << 27);
    (void)PRESETCTRLCLR1;

    AHBCLKCTRLSET1 = (1U << 27);

    CTIMERCLKSEL0 &= ~(0xFU << 0);

    CTIMERCLKSEL0 |= (0x3U << 0);

}

static inline void CTIMER_INIT(void)
{

    /* PR = (clock_hz /1_000_000) - 1
    AT 48MHZ : PR = 47 -> tick every 1micro sec */

    CTIMER0_PTR->PRESCALEREG = (clock_hz / 1000000) - 1;  // Prescale Register (PR) is used to divide the input clock to the timer. The value in PR determines how many input clock cycles are required for the timer counter to increment by 1. In this case, we want the timer to tick every 1 microsecond, so we set PR to (clock_hz / 1_000_000) - 1. For a clock frequency of 48 MHz, this results in a PR value of 47.
    
    CTIMER0_PTR->MR0 = period_us;  // Match Register 0 (MR0) is used to set the value at which the timer will generate an interrupt or reset. In this case, we want the timer to generate an interrupt every 0.5 milliseconds, so we set MR0 to period_ms, which is calculated as (48000 * (500/1000)) = 24000 ticks.

    CTIMER0_PTR->MCR |= (1<<0);   // Interrupt on MR0 Enable

    CTIMER0_PTR->MCR |= (1<<1);   // Reset on MR0 Enable

    /*CTIMER0_PTR->TCR |= (1<<4) ;    Allow Trigger Count Enable*/

    CTIMER0_PTR->TCR |= (1<<1);   // Count Reset Enable

    CTIMER0_PTR->TCR &= ~(1U << 1);  // clear 

    CTIMER0_PTR->TCR |= (1<<0);   // Counter Enable


}

static inline void CTIMER0_PERIPHERAL_RESET(void)
{

    PRESETCTRLCLR1 = (1U << 26);

}

void CTIMER_INIT();

void NVIC_CTimer0_Enable(void);

void  CTIMER0_IRQHandler(void);

#endif