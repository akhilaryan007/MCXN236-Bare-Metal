#ifndef MRT_H
#define MRT_H

#include "MCXN236.h"
#include <stdint.h>
/*-----------------------MUTI-RATE TIMER-----------------------------------------*/\

#define AHBCLKCTRL1           Reg(SYSCON + 0x204UL)

#define PRESETCTRL1             Reg(SYSCON+0x104UL)

#define PRESETCTRLSET1           Reg(SYSCON+0x124UL)

#define PRESETCTRLCLR1          Reg(SYSCON+0x144UL)

#define INPUTMUX0                0x40006000UL

#define SMARTDMAARCHB_INMUX0a    Reg(INPUTMUX0+0xA0)

#define NVIC_base               0x000000B8UL

#define MRT0_base                0x40013000UL

#define NVIC_ISER0              Reg(0xE000E100UL)

#define NVIC_IPR7               Reg(0xE000E41CUL)    // 0xE000E400 + 4n 

typedef struct {
    volatile uint32_t TIMER_INTVAL;         // 0
    volatile uint32_t TIMER0;               // 4
    volatile uint32_t CONTROL_CTRL0;        // 8
    volatile uint32_t STATUS_STAT0;         // C
}MRT_TIMER_TYPE;

#define MRT_CFG  ((MRT_TIMER_TYPE*)MRT0_base)


static inline void MRT_CLOCK(void)
{
    // AHBCLKCTRLSET0 = 1<<12;      // INPUTMUX CLOCK

    AHBCLKCTRLSET1 = 1<<0;        // MRT CLOCK 

    PRESETCTRLSET1 = (1U << 0);    // MRT ASSERT
    (void)PRESETCTRLSET1;
 

    PRESETCTRLCLR1 = (1U << 0);   // MRT RELEASE
    (void)PRESETCTRLCLR1;

    SMARTDMAARCHB_INMUX0a &= ~( 0x7FU << 0 );
    (void)SMARTDMAARCHB_INMUX0a;

    SMARTDMAARCHB_INMUX0a |= (0x14U<<0);       // (0x14U << 0); MRT0 MRT_CH0_IRQ input is selected
    (void)SMARTDMAARCHB_INMUX0a;

}


static inline void MRT_TIMER_INIT(void)
{
    MRT_CFG->TIMER_INTVAL = 12000000U;     // IVAL-1    48 000 000/1000 ticks/ms   48,000,000 × 0.25 = 12,000,000 

    MRT_CFG->TIMER_INTVAL |= (1UL << 31);    // LOAD 

    MRT_CFG->CONTROL_CTRL0 = (0U << 1) | (1U << 0);   // 00b - Repeat Interrupt mode   Enables the TIMERn interrupt    

    uint32_t RUN_STATUS = (MRT_CFG->STATUS_STAT0 >> 1) & 1U;    // Gives the Timer n State 0b - Idle state 1b - Running.
    (void)RUN_STATUS;

    uint32_t INTERRUPT_FLAG =(MRT_CFG->STATUS_STAT0 >> 0) & 1U;   // Gives the interrupt status 0b - No pending interrupt 1b - Pending interrupt
    (void)INTERRUPT_FLAG;
}

static inline void NVIC_ENABLE(void)
{
    NVIC_ISER0 = (1UL<<30);                 // 30 is MRT0 NVIC en 

    NVIC_IPR7 &= ~(0xFFUL<<16);         // mask to the [16:23] 
    NVIC_IPR7 |= ((0X2UL<<5)<<16);      // SET THE PRIORITY 2 (0X20) (2UL<<21)

}

static inline void LED_TOGGLE(void)
{
    #define RGB_MASK ((1U << 17) | (1U << 18) | (1U << 19))

        GPIO4_PTR->PTOR = RGB_MASK;       // change all LEDs
        GPIO4_PTR->PTOR = (1U << 18);


}


#endif