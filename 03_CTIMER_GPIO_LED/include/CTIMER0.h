#ifndef CTIMER0_H
#define CTIMER0_H

#include"MCXN236.h"
#include<stdint.h>
static inline void CTIMER0_INIT_CLOCK(void)
{
        /*Select FRO_HF as CTIMER0 functional clock*/

        CTIMERCLKSEL0 &= ~(0xFU << 0);       // CLEAR [3:0]  0000b
        CTIMERCLKSEL0 |= (0x3U << 0);       // SET [3:0]  0011b

        /*CLOCK ENABLE*/

        AHBCLKCTRLSET1 =(1<<26);                /*It is a write only register so don't use '|*/

        /*PERIPHERAL RESET*/

        PRESETCTRLSET1 = (1<<26);            /*set the peripheral ASSERT*/
        PRESETCTRLCLR1 = (1<<26);            /*Reset the peripheral*/

}

#endif