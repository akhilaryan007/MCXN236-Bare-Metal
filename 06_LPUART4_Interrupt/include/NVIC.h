#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>

#define NVIC_IPR_BASE  0xE000E400U

#define NVIC_BASE_ISER 0xE000E100U

static inline void UART_ENABLE_IRQ(uint32_t IRQ_NUMBER)

{
   uint32_t register_index;
   uint32_t bit_position;
   volatile uint32_t *NVIC_ISER;
   register_index = IRQ_NUMBER/32U;

   bit_position = (IRQ_NUMBER % 32U);

   NVIC_ISER = (volatile uint32_t *)(NVIC_BASE_ISER + register_index*4U);     // ISERn= ISER_BASE_ADDRESS + (regiater_index x n)    n = 4    every register address 4 bytes advance

   *NVIC_ISER = (1U<<bit_position);    // 7 is bit position  W1 TO SET ENABLE REGISTERS 

}

static inline void NVIC_SET_PRIORITY(uint32_t IRQ_Number, uint32_t priority) 
{
    uint32_t register_index;
    uint32_t slot;
    uint32_t shift;

    volatile uint32_t *NVIC_IPR;

    register_index = IRQ_Number / 4U;      // decides the index number IPRn n= register 7
    slot = IRQ_Number % 4U;               // decides the IRQ slot

    shift = (slot * 8U) + 5U;             // (3 x 8) + 5 = 29U

    NVIC_IPR = (volatile uint32_t *)(NVIC_IPR_BASE + (register_index * 4U));

    *NVIC_IPR &= ~(0x7U << shift);
    *NVIC_IPR |=  (priority << shift);    // priority = 3 slot = 3 shift upto 29 is reserved         
}

