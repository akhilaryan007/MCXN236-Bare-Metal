# CTIMER0 Hardware Timer Interrupt — Debug Story

## Objective

The objective was to configure the MCXN236 CTIMER0 peripheral as a bare-metal hardware timer and generate periodic interrupts to toggle an RGB LED.

The implementation was intentionally developed without the MCUXpresso SDK to understand the peripheral clock, reset, timer registers, NVIC configuration, and interrupt flow at register level.

## Initial Configuration

The timer was configured using the MCXN236 reference manual:

* CTIMER0 base address: `0x4000C000`
* CTIMER0 interrupt: `CTIMER0_IRQn = 31`
* Timer clock source: FRO_HF
* Timer clock: 48 MHz
* Target timer tick: 1 µs
* Prescaler: `47`

The intended flow was:

```text
FRO_HF / peripheral clock
        ↓
    CTIMER0
        ↓
 Timer reaches match
        ↓
 Interrupt request
        ↓
      NVIC
        ↓
 CTIMER0_IRQHandler()
        ↓
    Toggle LED
```

## Debugging

The first problem occurred when accessing CTIMER0 registers after enabling its peripheral clock.

The following register access was used to verify peripheral accessibility:

```c
volatile uint32_t test =
    *(volatile uint32_t *)0x4000C000;
```

The debugger was initially able to read the CTIMER0 address and returned:

```text
0x4000C000 → 1073790976
```

However, after stepping further into the initialization code, the debugger lost communication with the target and the processor entered a fault/default-handler condition.

The investigation then focused on the two main conditions required for peripheral access:

### 1. Peripheral clock

The CTIMER0 clock gate was investigated through the SYSCON AHB clock-control registers.

The CTIMER0 clock-enable bit was verified using:

```text
AHBCLKCTRLSET1
```

The corresponding register state changed after enabling the CTIMER0 clock.

### 2. Peripheral reset

The CTIMER0 reset control was investigated through:

```text
PRESETCTRL1
SET1
CLR1
```

The CTIMER0 reset bit was observed and verified during the initialization sequence.

### 3. Register access

After clock and reset configuration appeared correct, accesses to CTIMER0 registers such as:

```c
CTIMER0->TCR
CTIMER0->PRESCALEREG
```

still resulted in debugger instability / processor fault behaviour.

This indicated that the problem was not simply an incorrect timer-period calculation. The failure occurred at the peripheral-access level before a reliable CTIMER0 interrupt could be demonstrated.

## Result

CTIMER0 was therefore treated as a debugging exercise rather than marked as a completed timer-interrupt project.

The investigation provided practical experience with:

* Memory-mapped peripheral access
* SYSCON clock gating
* Peripheral reset control
* Timer clock selection
* Register-level debugging
* NVIC interrupt configuration
* Cortex-M fault behaviour
* Debugger peripheral-access failures

The next implementation moved to the MCXN236 MRT peripheral to demonstrate the same hardware-timer → NVIC → ISR → GPIO-toggle concept using a different timer peripheral.

## Key Lesson

A timer interrupt problem should not be debugged only from the timer configuration itself.

The complete dependency chain must be verified:

```text
Clock source
    ↓
Peripheral clock gate
    ↓
Peripheral reset release
    ↓
Peripheral register accessibility
    ↓
Timer configuration
    ↓
Interrupt flag
    ↓
NVIC enable
    ↓
ISR execution
    ↓
GPIO action
```

This debugging exercise helped establish a systematic approach for investigating low-level MCU peripherals before moving to the next hardware timer.






## ABOUT CTIMER

Each CTIMER is designed to count cycles of the CTIMER function clock or an externally supplied clock. A CTIMER can optionally
generate an interrupt or perform other actions at specified timer values based on the settings of Match (MR0 - MR3). Each
CTIMER also includes capture input pins (see Capture mode) to capture the timer value when an input signal transitions, optionally
generating an interrupt.

 Initialization  1156 PAGE 
1. Select a clock source for the CTIMER using the CTIMERCLKSEL register.
2. Enable the clock to the CTIMER via AHBCLKCTRL1. This enables the register interface and the peripheral
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

48,000,000 Hz
       ↓ / 1,000,000
48 timer ticks per microsecond?  ❌

Actually, because you're using the prescaler to make the timer counter tick every 1 µs, the correct match value is simply:

500 timer counts = 500 µs = 0.5 ms

48 MHz
  ↓
PR = 47
  ↓
1 MHz timer counter
  ↓
1 µs/count
  ↓
MR0 = 500
  ↓
500 µs
  ↓
0.5 ms



/*  ------------------------------OVERALL APPROACH------------------------------  */

FRO_HF
  ↓
CTIMERCLKSEL0
  ↓
Enable CTIMER0 clock
  ↓
Reset CTIMER0
  ↓
Release reset
  ↓
Configure prescaler
  ↓
Configure MR0
  ↓
Configure MCR
  ↓
Configure interrupt if required
  ↓
Start CTIMER0

                 CTIMER0
                    │
              48 MHz clock
                    │
              Prescaler = 47
                    │
                    ▼
             TC increments
                every 1 µs
                    │
                    ▼
              MR0 = 500
                    │
                    ▼
              500 µs = 0.5 ms
                    │
             Match occurs
                    │
          ┌─────────┴─────────┐
          ▼                   ▼
      Interrupt          TC reset to 0
          │
          ▼
      LED toggle


TOTAL CTIMER0 operation:

      48 MHz
  │
  ▼
Prescaler = 47
  │
  ▼
TC tick = 1 µs
  │
  ▼
TC: 0 → 1 → 2 → 3 → ... → 500
                              │
                              │ TC == MR0
                              ▼
                         MATCH EVENT
                              │
                    ┌─────────┴─────────┐
                    ▼                   ▼
                TC RESET             IRQ SET
                    │                   │
                    │                   ▼
                    │            CTIMER0_IRQHandler()
                    │                   │
                    │             tick_count++
                    │                   │
                    │            tick_count == 1000?
                    │                   │
                    │                  YES
                    │                   │
                    │                   ▼
                    │              LED TOGGLE
                    │                   │
                    │            tick_count = 0
                    │
                    └─────── repeat


void CTIMER0_Init(void)
{
    /*
     * 48 MHz CTIMER clock
     * Prescaler = 47
     * TC increments every 1 us
     */
    CTIMER0_PTR->PRESCALEREG =
        (CTIMER0_CLOCK_HZ / CTIMER0_TICK_HZ) - 1U;

    /*
     * 500 us = 0.5 ms
     */
    CTIMER0_PTR->MR0 = CTIMER0_MATCH_US;

    /*
     * Configure MCR:
     * - interrupt on MR0 match
     * - reset TC on MR0 match
     */
    
    /*
     * Enable CTIMER0 interrupt in NVIC
     */

    /*
     * Start timer
     */
}

CTIMER hardware
      │
      ▼
match event
      │
      ▼
NVIC
      │
      ▼
CPU interrupts current code
      │
      ▼
CTIMER0_IRQHandler()

------> For MCXN236 CTIMER0, the reference manual gives:

CTIMER0 IRQ number = 31
Vector number = 47
NVIC non-IPR register number = 0
NVIC IPR register number = 7

1. Calculate n for NVIC->ISER[n]

The NVIC ISER registers contain 32 interrupt-enable bits each:

ISER[0] → IRQ 0  ... 31
ISER[1] → IRQ 32 ... 63

So:

n = IRQ / 32

For CTIMER0:

n = 31 / 32
  = 0

And the bit is:

IRQ % 32
= 31 % 32
= 31

Therefore:

NVIC->ISER[0] = (1UL << 31);

or, preferably:

NVIC->ISER[0] |= (1UL << 31);

2. So what exactly happens?

Think of ISER[0] like this:

              ISER[0]
       ┌───────────────────────┐
bit 31 │ 1                     │ ← CTIMER0 IRQ 31
       │                       │
bit 30 │ 0                     │
  ...  │ ...                   │
bit  1 │ 0                     │
bit  0 │ 0                     │
       └───────────────────────┘

Setting bit 31 enables CTIMER0 at the NVIC level.

So your bare-metal code will have:

NVIC->ISER[0] |= (1UL << 31);
3. What about NVIC->IPR[n]?

This is where your table is slightly confusing.

The MCXN236 has 4 priority bytes packed into each 32-bit IPR register.

For CTIMER0:

IRQ = 31

IPR register:
31 / 4 = 7

Therefore:

NVIC->IPR[7]

And CTIMER0 occupies the fourth byte of IPR[7]:

NVIC->IPR[7]

31             24 23             16 15              8 7               0
┌────────────────┬────────────────┬─────────────────┬─────────────────┐
│    IRQ 31      │    IRQ 30      │    IRQ 29       │    IRQ 28       │
└────────────────┴────────────────┴─────────────────┴─────────────────┘
       ↑
 CTIMER0

The MCXN236 RM explicitly lists CTIMER0 as IRQ 31, NVIC IPR register 7.

If you want priority 0:

NVIC->IPR[7] = 0x00000000;

But be careful: that writes the priorities of IRQ 28–31, not just CTIMER0.

If you only want to modify CTIMER0's priority byte:

NVIC->IPR[7] &= ~(0xFFUL << 24);
NVIC->IPR[7] |=  (0x00UL << 24);

However, if priority 0 is already the reset/default value, you don't need to touch it.

The important distinction

For CTIMER0:

Item	Value
Peripheral	CTIMER0
IRQ number	31
Vector number	47
ISER index	0
ISER bit	31
IPR index	7
IPR byte	3 / bits 31:24
ISR function	CTIMER0_IRQHandler()

So the basic NVIC enable is simply:

NVIC->ISER[0] |= (1UL << 31);
And this is a very useful formula to remember

For any peripheral IRQ number:

ISER_index = IRQn / 32;
ISER_bit   = IRQn % 32;

IPR_index  = IRQn / 4;
IPR_byte    = IRQn % 4;

For CTIMER0:

IRQn = 31

ISER:
31 / 32 = 0
31 % 32 = 31

→ ISER[0], bit 31

IPR:
31 / 4 = 7
31 % 4 = 3

→ IPR[7], byte 3 (bits 31:24)

One more important point for your current CTIMER0 project: enabling NVIC alone is not enough. The CTIMER itself must generate an interrupt (for example, Match 0 interrupt enabled in MCR), and your CTIMER0_IRQHandler() must clear the CTIMER interrupt flag. The NXP documentation confirms CTIMER has separate match interrupt-enable and status flags.

ISER[n] address = 0xE000E100 + (n × 4)

ARM's NVIC documentation specifies that the IPR register number is:

IPR register number = IRQ number / 4

and the byte offset within that register is:

byte offset = IRQ number % 4  

                 IRQ number
                     │
          ┌──────────┴──────────┐
          │                     │
       ENABLE                 PRIORITY
          │                     │
       ÷ 32                   ÷ 4
          │                     │
       ISER[n]                IPR[n]
          │                     │
      remainder               remainder
          │                     │
        bit #                 byte #

The IPR area starts at:

0xE000E400

Each IPR is 4 bytes apart:

IPR[0] → 0xE000E400
IPR[1] → 0xE000E404
IPR[2] → 0xE000E408
IPR[3] → 0xE000E40C
IPR[4] → 0xE000E410
IPR[5] → 0xE000E414
IPR[6] → 0xE000E418
IPR[7] → 0xE000E41C

ARM's register map confirms these addresses.

So:

IPR[7] = 0xE000E400 + (7 × 4)

       = 0xE000E41C

For the ISER array:

ISER[0] = 0xE000E100
ISER[1] = 0xE000E104
ISER[2] = 0xE000E108
...

because each ISER is 32 bits = 4 bytes.

Therefore:

ISER[n] address = 0xE000E100 + (n × 4)

2. Now the important part: Why IPR[7]?

This comes directly from how ARM designed the Interrupt Priority Registers.

Each interrupt has one 8-bit priority field.

So:

IRQ 0  → 8-bit priority
IRQ 1  → 8-bit priority
IRQ 2  → 8-bit priority
IRQ 3  → 8-bit priority

Four 8-bit priorities fit into one 32-bit register:

                 IPR[0]
       31       24 23       16 15        8 7         0
       ┌──────────┬──────────┬───────────┬───────────┐
       │ IRQ 3    │ IRQ 2    │ IRQ 1     │ IRQ 0     │
       └──────────┴──────────┴───────────┴───────────┘

ARM's NVIC documentation specifies that the IPR register number is:

IPR register number = IRQ number / 4

and the byte offset within that register is:

byte offset = IRQ number % 4

3. Therefore, IPR[7] contains IRQ 28–31

Let's calculate it rather than memorize it.

IPR[0]

Contains:

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IPR[1]

Contains:

IRQ 4
IRQ 5
IRQ 6
IRQ 7
IPR[2]

Contains:

IRQ 8
IRQ 9
IRQ 10
IRQ 11

And so on.

Therefore:

IPR[7]

contains:

7 × 4 = 28

through:

7 × 4 + 3 = 31

So:

IPR[7] → IRQ 28, 29, 30, 31

That's why I said IPR[7] contains IRQ 28–31.