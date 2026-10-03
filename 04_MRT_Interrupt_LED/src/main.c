#include "MCXN236.h"
#include "systick.h"
#include <stdint.h>
#include "CTIMER0.h"
#include "MRT.h"


void MRT_IRQHandler (void)
{
    volatile uint32_t pre_status = (MRT_CFG->STATUS_STAT0 >> 0)&1;
    (void)pre_status;

    MRT_CFG->STATUS_STAT0 = 1<<0; 

    volatile uint32_t post_status = (MRT_CFG->STATUS_STAT0 >> 0)&1;
    (void)post_status;

    LED_TOGGLE();
}


int main(void) {

    /*GPIO4 CLOCK*/

    Clock_Enable();

    /*GPIO4 PERIPHERAL*/

    Peripheral_Reset();

    /*GPIO4 PIN CONTROL REGISTE PIN MULTIPLEXING FROM PORT4*/

    PIN_MUX();

    GPIO4_PTR->PDDR |= (1UL<<18);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 18 is the P4_18 RED LED 
    GPIO4_PTR->PDDR |= (1UL<<19);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 19 is the P4_19 GREEN LED
    GPIO4_PTR->PDDR |= (1UL<<17);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 17 is the P4_17 BLUE LED


    /*MRT INITIALIZATION*/
    
    MRT_CLOCK();   // MRT ENABLE

    NVIC_ENABLE();  // MRT NVIC

    MRT_TIMER_INIT();   // MRT TIMER INITIALIZARION

    while (1)
    {
        /* CPU is free to do other work */
    }

    
}