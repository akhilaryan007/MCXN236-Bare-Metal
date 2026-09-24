#include "systick.h"
#include <stdint.h>

void Systick_Demo(void)

{
    Systick_Enable_Init();

    volatile uint32_t  TICKINT = Systick->SYST_CSR & (1UL<<1);    // Tick interrupt. Indicates whether counting to 0 causes the status of the SysTick exception to change to pending.


    /* volatile uint32_t CSR_COUNTFLAG_STATUS = Systick->SYST_CSR & (1UL << 16); COUNTFLAG is a status flag that becomes set when the counter transitions from 1 to 0, and reading SYST_CSR can clear it*/

    volatile uint32_t  SYST_RVR_VALUE = Systick->SYST_RVR;   // Reload value register. This register contains the value to load into the SysTick Current Value Register when the counter reaches 0.

    volatile uint32_t  SYS_CVR_vALUE = Systick->SYST_CVR & 0x00FFFFFF;   // 0000 0000 1111 1111 1111 1111 1111 1111 MASK

    volatile uint32_t  SYST_RVR_24BITS = Systick->SYST_RVR & 0x00FFFFFF;  // 24 BITS MASK 

}

