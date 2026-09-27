#include "MCXN236.h"
#include "systick.h"
#include <stdint.h>
#include "CTIMER0.h"


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

    
    CTIMER0_INIT_CLOCK();

    CTIMER_INIT();

    NVIC_CTimer0_Enable(); 


    

#define RGB_MASK ((1U << 17) | (1U << 18) | (1U << 19))

    while (1)
    {
        GPIO4_PTR->PSOR = RGB_MASK;       // LEDs off: drive pins high 0=LED ON, 1=LED OFF  Pin 0 = LED on   Pin 1 = LED off

        GPIO4_PTR->PCOR = (1U << 18);     // Red on


        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 19);     // Green on


        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 17);     // Blue on

    }

  

    
}