Each CTIMER is designed to count cycles of the CTIMER function clock or an externally supplied clock. A CTIMER can optionally
generate an interrupt or perform other actions at specified timer values based on the settings of Match (MR0 - MR3). Each
CTIMER also includes capture input pins (see Capture mode) to capture the timer value when an input signal transitions, optionally
generating an interrupt.

 Initialization  1156 PAGE 
1. Select a clock source for the CTIMER using the CTIMERCLKSEL register.
2. Enable the clock to the CTIMER via CTIMERCLKSELn[SEL]. This enables the register interface and the peripheral
function clock.
3. Clear the CTIMER peripheral reset using the Peripheral Reset Control 0 (PRESETCTRL0) and Peripheral Reset Control
1 (PRESETCTRL1) registers.
4. Each CTIMER provides interrupts to the NVIC. See MCR and CCR registers in the CTIMER register section for match
and capture events. For interrupt connections, see NVIC interrupt assignments.
5. Select timer pins and pin modes as needed through the relevant PORT registers.
6. The CTIMER DMA request lines are connected to the DMA trigger inputs via the DMAC0_ITRIG_INMUX registers (See
INPUTMUX). Note that timer DMA request outputs are connected to DMA trigger inputs.

0011b - FRO_HF clock is the default clock so select this clock in [3:0] out of 32 bits

FRO_HF → SYSCON mux → CTIMER0 → prescaler → timer counter.

// 1. Enable clock to CTIMER0 peripheral
//    AHBCLKCTRL... 

// 2. Select CTIMER0 clock source
CTIMERCLKSEL0 = (CTIMERCLKSEL0 & ~0xFU) | 0x3U;

// 3. Configure CTIMER0 registers
//    CTCR, PR, MR0, MCR, etc.

// 4. Start CTIMER0


