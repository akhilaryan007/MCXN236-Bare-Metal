#include "MCXN236.h"
#include "systick.h"
#include <stdint.h>
#include "CTIMER0.h"
#include "lpuart4.h"



static uint8_t UART_RECEIVED_BYTE(void)
{
    /* Wait until transmitter is ready */

    while ((STAT & (1U << 21)) == 0U)
    {
    }

    uint8_t received =  (uint8_t)(DATA & 0xFFU);
    return received;
    
}

static void UART_TRANSMIT_BYTE(uint8_t data)
{
    while ((STAT & (1U << 23)) == 0U)
    {
        /* Wait until TX data register is ready */
    }

    DATA = data;
    
        /* Wait until the character has finished transmitting */
    
        while ((STAT & (1U << 22)) == 0U)
    {
    }
}

int main(void) {

    LP_FLEXCOM_INITT();
    PIN_MUX_UART();
    TX_RX_DISABLE();
    MADULO_BAUD_DIV();
    TX_RX_ENABLE(); 

    while (1)
    {
        uint8_t received = UART_RECEIVED_BYTE();
        // Send the received character back to the PC
        UART_TRANSMIT_BYTE(received);
    }

}