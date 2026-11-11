#include<stdint.h>
#include "MCXN236.h"

static inline void LP_FLEXCOM_INITT(void)
{

   AHBCLKCTRLSET1 = (1 << 15);          /*LP_FLEXCOMM4 */


    /* Reset and halt the divider */
    FLEXCOMM4_CLKDIV_REG = CLKDIV_HALT | CLKDIV_RESET;

    /* Configure divide-by-one and keep divider halted */
    FLEXCOMM4_CLKDIV_REG = CLKDIV_HALT;

    /* Release divider */
    FLEXCOMM4_CLKDIV_REG &= ~CLKDIV_HALT;

    /* Wait until divider reports stable */
    while ((FLEXCOMM4_CLKDIV_REG & CLKDIV_UNSTAB) != 0U)
    {
    }

   /* FRO_HF_DIV = FRO_HF / (0 + 1) */

   // FROHFDIV &= ~(0XFFU);

   FCCLKSEL4 &= ~(0x7U << 0);           /* LP_FLEXCOMM4_CLK_SEL*/

   FCCLKSEL4 |= (0x2U << 0);           /* 011b - fro_hf_div clock */

   PRESETCTRLCLR1 = (1<<15);             /*preset FC_4*/

   PSELID &= ~(0x7U << 0);

   PSELID |= (0x1U <<0);               /* Selection of UART*/

}
static inline void PIN_MUX_UART (void)
{
   AHBCLKCTRLSET0 = (1<<14);            /*PORT1 CLOCK ENABLE*/ 

   PCR_8 &= ~(0xFU << 8);              /*RX_PIN_MUX*/
   PCR_8 |= (0x2U << 8);
   PCR_8 |=  (1U << 12);         // IBE enable input buffer enable

   PCR_9 &= ~(0xFU << 8);              /*TX_PIN_MUX*/
   PCR_9 |= (0x2U << 8);
}

static inline void TX_RX_DISABLE(void)
{
   CTRL &= ~(1<<18U);        /*CONTROL RX */ 
   
   CTRL &= ~(1<<19U);        /*CONTROL TX*/

}

static inline void TX_RX_ENABLE(void)
{
   CTRL |= (1<<18U);        /*CONTROL RX */ 

   CTRL |= (1<<19U);        /*CONTROL TX*/
}

static inline void MADULO_BAUD_DIV(void)        /*modulo baud rate divisor SBR*/
{
   // BAUD &= ~(0x1FFFU << 0);                     
   
   // BAUD |= (0x1AU << 0);                        /*Set 26 SBR for baud rate 115200*/

   // /* OSR = 8 (OSR field = 7) */
   //  BAUD &= ~(0x1FU << 24);
   //  BAUD |=  (7U << 24);

   // /* SBR = 13 */
   //  BAUD &= ~(0x1FFFU);
   //  BAUD |= 13U;
   // BAUD = (BAUD & ~0x1FFFU) | 13U;
   // baud = 12000000/16x13
   //Baud Error(%) = (Actual Baud - Targated Baud/Targated Baud ) x 100
   //BAUD = (15U << 24) | 13U;

   BAUD = 0x0F00000DU; 

   /*OSR by default is 16 so using 16 only for the 115200 baud rate below is for debugging purpose*/

   uint32_t OSR_24 = (BAUD >> 24) & 1;
   uint32_t OSR_25 = (BAUD >> 25) & 1;   
   uint32_t OSR_26 = (BAUD >> 26) & 1;
   uint32_t OSR_27 = (BAUD >> 27) & 1;
   uint32_t OSR_28 = (BAUD >> 28) & 1;
   (void)OSR_24;
   (void)OSR_25;
   (void)OSR_26;
   (void)OSR_27;
   (void)OSR_28;

   /*SBNS SELECTION OF STOP BITS '1' = 2 STOP BITS '0'= 1 STOP BIT */
   BAUD &= ~(0x1U << 13);

   /*selection of message size 8 bit from CTRL[M] = 0*/
   CTRL &= ~(0x1U << 4);

   CTRL &= ~(0x1U << 11);  /*M7 */

   CTRL &= ~(0X1U << 7);

   // BAUD |= (0x1U << 13);       
   FIFO |= (0x1U << 15);
   FIFO |= (0x1U << 14);


}

static inline void RX_INTERRUPT_ENABLE (void)
{

   CTRL |= (1<<21);     // Receive interrupt enable

}









// void LP_FLEXCOMM4_IRQHandler(void)
// {
//     if (UART_STAT & (1U << RDRF_BIT))
//     {
//         uint8_t received_byte;

//         received_byte = (uint8_t)UART_DATA;

//         /* Insert received byte into RX ring buffer */
//     }
// }


