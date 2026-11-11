#include "MCXN236.h"
#include "systick.h"
#include <stdint.h>
#include "CTIMER0.h"
#include "lpuart4.h"
#include "ring_buffer.h"
#include "NVIC.h"

Ringbuffer rx_buffer;
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

void UART_IRQHandler(void)
{
    uint8_t received_byte;
    /* Check Receive Data Register Full */
    if ((STAT & (1U << 21)) != 0U)
    {
        /* Read received byte from UART hardware */
        received_byte = (uint8_t)(DATA & 0xFFU);

        /* Store received byte in RX ring buffer */
        RING_BUFFER_HEAD(&rx_buffer, received_byte);
    }
}


int main(void) {
    uint8_t received_byte;
    LP_FLEXCOM_INITT();
    PIN_MUX_UART();
    TX_RX_DISABLE();
    MADULO_BAUD_DIV();
    TX_RX_ENABLE();
    RING_BUFFER_INIT(&rx_buffer);
    NVIC_SET_PRIORITY(39U,3U);
    RX_INTERRUPT_ENABLE();
    UART_ENABLE_IRQ(39U);
    while (1)
    {

        /* Retrieve byte from RX ring buffer */
        if (RING_BUFFER_TAIL(&rx_buffer, &received_byte))
        {
            /* Echo received byte to PC */
            UART_TRANSMIT_BYTE(received_byte);
        }

    }

}