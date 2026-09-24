// Flash size / RAM size (Memory Map chapter)

#ifndef MCXN236_H
#define MCXN236_H

#include <stdint.h>

void Reset_Handler(void);

extern int main(void);
 

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

/*Enable Clock*/

#define Reg(addr)  (*(volatile uint32_t *)(addr))    /* #define SQUARE(x)  ((x) * (x))   int result = SQUARE(5)  result=((5)  (5))*/

/*SYSTEM CLOCK GENERATOR*/

#define SCG_BASE_ADDRESS    0x40044000UL

#define SCG_CSR             Reg(SCG_BASE_ADDRESS+0x10UL)

#define SCG_OFFSET_RCCR     Reg(SCG_BASE_ADDRESS+0x14UL)   /*  Run Clock Control Register (RCCR)*/

#define SCG_FIRCCSR         Reg(SCG_BASE_ADDRESS+0x300UL)

#define FIRCCFG             Reg(SCG_BASE_ADDRESS+0x3058UL)  

/*SYSCON*/

#define SYSCON                0x40000000UL

#define AHBCLKCTRL0           Reg(SYSCON + 0x200UL)

#define AHBCLKCTRLSET0        Reg(SYSCON+0X220UL)

#define AHBCLKCTRLCLR0        Reg(SYSCON+240UL)

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

/*Peripheral Reset*/

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



/*PORT PIN MUX*/

#define PORT4_Base      0x4011A000UL 

#define PCR19             Reg(PORT4_Base+0xCCUL)

static inline void PIN_MUX(void)
{
    PCR19 &= ~(0xFU << 8);       /*[11:8] needs to clear the MUX bits 15U << 8  PCR19&=~(((1<<4)-1)<<8);*/ 
}

/*GPIO*/

#define GPIO4      0x4009E000UL


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


#endif
