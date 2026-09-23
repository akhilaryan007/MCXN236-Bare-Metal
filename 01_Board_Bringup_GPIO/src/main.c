#include <stdint.h>
#include<MCXN236.h>
#include<stdio.h>


/* ---- These variables test .data and .bss ---- */

void delay(void)
{
    volatile uint32_t  i;
    for (i = 0; i < 800000; i++)
    {
        __asm("NOP");    /* inline assembly instruction NOP Mean NO OPERATION */
    }
}

int main(void) {

    Clock_Enable();

    Peripheral_Reset();

    PIN_MUX();

    GPIO4_PTR->PDDR |= (1UL<<19);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 19 is the P4_19 GREEN LED
    GPIO4_PTR->PDDR |= (1UL<<17);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 17 is the P4_17 BLUE LED
    GPIO4_PTR->PDDR |= (1UL<<18);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 18 is the P4_18 RED LED 


#define RGB_MASK ((1U << 17) | (1U << 18) | (1U << 19))

    while (1)
    {
        GPIO4_PTR->PSOR = RGB_MASK;       // LEDs off: drive pins high 0=LED ON, 1=LED OFF  Pin 0 = LED on   Pin 1 = LED off

        GPIO4_PTR->PCOR = (1U << 18);     // Red on
        delay();

        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 19);     // Green on
        delay();

        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 17);     // Blue on
        delay();   
            

    }

    
}