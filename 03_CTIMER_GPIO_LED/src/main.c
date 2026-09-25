#include "MCXN236.h"
#include "systick.h"
#include <stdint.h>


volatile uint32_t msTicks = 0; 

void SysTick_Handler(void)
{
    msTicks++;              // In while loop as per the delay number the interrupt executes that much number or iterates or For each millisecond the interrupt executes and increments the msTicks variable by 1
}

void delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < ms)
    {
        // After the delay number of milliseconds the while loop will exit and the program will continue to execute the next line of code after the delay function call
    }
}

/*polling delay*/

/* void delay(void)
{
    volatile uint32_t  i;
    for (i = 0; i < 800000; i++)
    {
        __asm("NOP");     inline assembly instruction NOP Mean NO OPERATION 
    }
} */

int main(void) {

    Clock_Enable();

    Peripheral_Reset();

    PIN_MUX();

    GPIO4_PTR->PDDR |= (1UL<<19);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 19 is the P4_19 GREEN LED
    GPIO4_PTR->PDDR |= (1UL<<17);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 17 is the P4_17 BLUE LED
    GPIO4_PTR->PDDR |= (1UL<<18);  // SET THIS ONE DIRECTION AS OUTPUT '1' '0' INPUT 18 is the P4_18 RED LED 

    /*CTIMER0 INITIALIZATION*/

    CTIMER0_INIT_CLOCK();

    /*Systick initialization*/

    // Systick_Enable_Init(); 

#define RGB_MASK ((1U << 17) | (1U << 18) | (1U << 19))

    while (1)
    {
        GPIO4_PTR->PSOR = RGB_MASK;       // LEDs off: drive pins high 0=LED ON, 1=LED OFF  Pin 0 = LED on   Pin 1 = LED off

        GPIO4_PTR->PCOR = (1U << 18);     // Red on
        delay_ms(2000);

        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 19);     // Green on
        delay_ms(2000);

        GPIO4_PTR->PSOR = RGB_MASK;
        GPIO4_PTR->PCOR = (1U << 17);     // Blue on
        delay_ms(2000);
   
    }

    
}