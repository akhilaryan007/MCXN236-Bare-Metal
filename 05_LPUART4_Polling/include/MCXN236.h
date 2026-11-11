// Flash size / RAM size (Memory Map chapter)

#ifndef MCXN236_H
#define MCXN236_H

#include <stdint.h>
#include <stddef.h>


void Reset_Handler(void);
// void NMI_Handler (void);

extern int main(void);
 
/* void NMI_Handler (void)
 {
     while (1){ }
 }*/

void Default_Handler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/*INTERRUPT REQ NUMBER */

#define CTIMER0_IRQn    31U

/*Enable Clock*/

#define Reg(addr)  (*(volatile uint32_t *)(addr))    /* #define SQUARE(x)  ((x) * (x))   int result = SQUARE(5)  result=((5)  (5))*/

/*ARM REGISTERS*/

#define  NVIC      Reg(0xE000E100UL)    // NVIC BASE ADDRESS

#define ISER     Reg(0xE000E100UL)   // Interrupt Set Enable Register

#define IPR        Reg( 0xE000E41CUL)  //  Interrupt Priority Register 400 base but 7x4=28-> 400 + 28 = 1CIPR[7]

/*--------------------------------------SYSTEM CLOCK GENERATOR-----------------------------------------------------*/

#define SCG_BASE_ADDRESS    0x40044000UL

#define SCG_CSR             Reg(SCG_BASE_ADDRESS+0x10UL)

#define SCG_OFFSET_RCCR     Reg(SCG_BASE_ADDRESS+0x14UL)   /*  Run Clock Control Register (RCCR)*/

#define SCG_FIRCCSR         Reg(SCG_BASE_ADDRESS+0x300UL)

#define FIRCCFG             Reg(SCG_BASE_ADDRESS+0x3058UL)  

/*---------------------------------------------SYSCON-----------------------------------------------------*/

#define SYSCON                0x40000000UL

#define AHBCLKCTRL0           Reg(SYSCON + 0x200UL)

#define AHBCLKCTRLSET0        Reg(SYSCON+0X220UL)

#define AHBCLKCTRLCLR0        Reg(SYSCON+0x240UL)


static inline void Clock_Enable (void)
{
    AHBCLKCTRLSET0=(1<<23);          /* AHBCLKCTRL Register is a write only register no need to use '|' GPIO 4 */
    AHBCLKCTRLSET0=(1<<17);          /* AHBCLKCTRL Register is a write only register no need to use '|' PORT 4 */
}


static inline void Clock_Disable(void)
{
        AHBCLKCTRLCLR0 = (1U << 23);
        AHBCLKCTRLCLR0 = (1U << 17);
}
// GPIO4 [23]   PORT4 [17]

/*------------------------------------------------------------Peripheral Reset---------------------------------*/

#define PRESETCTRL0         Reg(SYSCON+0x100UL)

#define PRESETCTRLSET0      Reg(SYSCON+0x120UL)

#define PRESETCTRLCLR0      Reg(SYSCON+0X140UL)

static inline void Peripheral_Reset(void)
{
    PRESETCTRLSET0=(1<<23);  /* it is a write only register don't use '|' */
    PRESETCTRLSET0=(1<<17);  

    PRESETCTRLCLR0 = (1<<23);  /*Release GPIO4 */
    PRESETCTRLCLR0 = (1<<17);  /*Release PORT4 */

}

// void Peripheral_Reset(void);

/*-----------------------------------------------------PORT PIN MUX--------------------------------------*/

#define PORT4_Base      0x4011A000UL 

#define PCR19             Reg(PORT4_Base+0xCCUL)

static inline void PIN_MUX(void)
{
    PCR19 &= ~(0xFU << 8);       /*[11:8] needs to clear the MUX bits 15U << 8  PCR19&=~(((1<<4)-1)<<8);*/ 
}

/*-------------------------------------------------GPIO-------------------------------------------------------*/

#define GPIO4      0x4009E000UL

// #define PDIR        Reg(GPIO0+50UL)

// #define PDDR        Reg(GPIO0+54UL)

typedef struct{

    uint32_t Reserved[16];    /* PDOR=40 decimal of 40 calculation Refer definition.md file */

    volatile uint32_t PDOR;     //0x40
    
    volatile uint32_t PSOR;     //0x44

    volatile uint32_t PCOR;     //0X48

    volatile uint32_t PTOR;     //0X4C

    volatile uint32_t PDIR;     //0X50

    volatile uint32_t  PDDR;    //0X54

}GPIO_Type;

#define GPIO4_PTR       ((GPIO_Type *)GPIO4)

/*-------------------------------------------------CTIMER0--------------------------------------------------------------------------*/
 
/*CLOCK ENABLE*/

#define CTIMERCLKSEL0          Reg(SYSCON+0x270UL)

#define AHBCLKCTRLSET1         Reg(SYSCON+0x224UL)   

#define CTIMER0                 0x4000D000UL

typedef struct {
    volatile uint32_t INTERRUPT;  // 0

    volatile uint32_t TCR;      //4

    volatile uint32_t TIMERCOUNTER;        // 8

    volatile uint32_t PRESCALEREG;          //C

    volatile uint32_t PRESCALECOUNTER;      //10

    volatile uint32_t MCR;                  // 14 

    volatile uint32_t MR0;

    volatile uint32_t MR1;

    volatile uint32_t MR2;

    volatile uint32_t MR3;

    volatile uint32_t CCR;

    volatile uint32_t CR0;

    volatile uint32_t CR1;

    volatile uint32_t CR2;

    volatile uint32_t CR3;

    volatile uint32_t EXTERNALMATCH;

}CTIMER_TYPE;

#define CTIMER0_PTR ((CTIMER_TYPE *)CTIMER0)



_Static_assert(offsetof(CTIMER_TYPE, INTERRUPT)       == 0x00, "INTERRUPT offset wrong");
_Static_assert(offsetof(CTIMER_TYPE, TCR)             == 0x04, "TCR offset wrong");

/*-------------------------------LP_UART4--------------------------------------------*/

#define AHBCLKCTRL1     Reg(SYSCON+0x204UL) 

#define PORT1_Base      0x40117000UL

#define LP_FLEXCOMM4_base     0x400B4000UL

#define FLEXCOMM4_PRESETCTRL1     Reg(SYSCON+0x104UL)

#define PRESETCTRLCLR1          Reg(SYSCON+0x144UL)

#define FROHFDIV            Reg(SYSCON + 0x388UL)

#define FCCLKSEL4       Reg(SYSCON+0x2C0UL)

#define PCR_8           Reg(PORT1_Base+0xA0UL)   /*FC4_RX*/

#define PCR_9           Reg(PORT1_Base+0xA4UL)   /*FC4_TX*/

#define PSELID          Reg(LP_FLEXCOMM4_base + 0xFF8UL) /* off set of LP_FC4*/

#define CTRL            Reg(LP_FLEXCOMM4_base + 0x18UL )    /*offset of CTRL*/

#define BAUD            Reg(LP_FLEXCOMM4_base + 0x10UL )    /*offset of BAUD*/

#define DATA            Reg(LP_FLEXCOMM4_base + 0x1CUL)     /* offset of data register */

#define STAT            Reg(LP_FLEXCOMM4_base + 0x14UL)     /* Provie the module status */

#define FIFO            Reg(LP_FLEXCOMM4_base + 0x28UL)     /*FIFO FLUSH TXEMPT TX COUNT*/

#define FLEXCOMM4_CLKDIV_REG   (*(volatile uint32_t *)0x40000860U)

#define CLKDIV_UNSTAB          (1U << 31)
#define CLKDIV_HALT            (1U << 30)
#define CLKDIV_RESET           (1U << 29)

#endif