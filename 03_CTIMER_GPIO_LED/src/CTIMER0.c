#include "CTIMER0.h"
#include "MCXN236.h"
#include <stdint.h>

uint32_t clock_hz = 48000000U;  // 48MHz
uint32_t period_ms = 500U;  // Counter for the number of ticks 48000000/1000 = 48000 ticks for 1ms, 48000*0.5 = 24000 ticks for 0.5ms

void CTIMER_CLOCK(void)
{

    /* PR = (clock_hz /1_000_000) - 1
    AT 48MHZ : PR = 47 -> tick every 1micro sec */

    CTIMER0_PTR->PRESCALEREG = (clock_hz / 1000000) - 1;  // Prescale Register (PR) is used to divide the input clock to the timer. The value in PR determines how many input clock cycles are required for the timer counter to increment by 1. In this case, we want the timer to tick every 1 microsecond, so we set PR to (clock_hz / 1_000_000) - 1. For a clock frequency of 48 MHz, this results in a PR value of 47.
    
    CTIMER0_PTR->MR0 = period_ms;  // Match Register 0 (MR0) is used to set the value at which the timer will generate an interrupt or reset. In this case, we want the timer to generate an interrupt every 0.5 milliseconds, so we set MR0 to period_ms, which is calculated as (48000 * (500/1000)) = 24000 ticks.


    
    CTIMER0_PTR->TCR |= (1<<4) ;   // Allow Trigger Count Enable

    CTIMER0_PTR->TCR |= (1<<1);   // Count Reset Enable

    CTIMER0_PTR->TCR |= (1<<0);   // Counter Enable

}
