#include "CTIMER0.h"
#include "MCXN236.h"
#include <stdint.h>

volatile uint32_t CTIMER0_TICK_COUNT = 0;  // Global variable to keep track of the number of ticks
uint32_t clock_hz = 48000000U;  // 48MHz
uint32_t period_us = 500000U;  // Counter for the number of ticks 48000000/1000 = 48000 ticks for 1ms, 48000*0.5 = 24000 ticks for 0.5ms

void CTIMER_INIT(void)
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


void NVIC_CTimer0_Enable(void)
{
    /* Enable CTIMER0 interrupt in NVIC
     *
     * IRQ 31:
     * ISER index = 31 / 32 = 0
     * ISER bit   = 31 % 32 = 31
     */
    ISER |= (1UL << CTIMER0_IRQn);   // ISER[0]

    /* Set CTIMER0 interrupt priority
     *
     * IRQ 31:
     * IPR register = 31 / 4 = 7
     * Priority byte = 31 % 4 = 3
     *
     * Byte 3 occupies bits [31:24].
     */
    IPR &= ~(0xFFUL << 24);   // IPR[7]
    IPR |=  (0x00UL << 24);
}