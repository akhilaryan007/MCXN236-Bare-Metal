To initialize the MRT module:
1. Enable the clock to MRT via MRT field in AHB Clock control 1 AHBCLKCTRL1 register in SYSCON.
2. Deassert the MRT reset via MRT_RST field in the Peripheral reset control 1 PRESETCTRL1 register in SYSCON.
3. Ensure that the MRT can issue an interrupt in the chip. See registers for SmartDMA arch B inputs

INPUTMUX0 base address: 4000_6000
(SMARTDMAARCHB_INMUX0 - SMARTDMAARCHB_INMUX7) in INPUTMUX.

For a = 0 to 7:

SMARTDMAARCHB_IN
MUXa
A0h + (a × 4h)

INP = 1 1 1 1 1 1 1
001_0100b - MRT0 MRT_CH0_IRQ input is selected

-----> NVIC
Address  = 0000_00B8
Vector   = 46
IRQ      = 30

NVIC
non-IPR    = 0
register
number

NVIC IPR
register    = 7
number

Alias       = MRT0 Multi-Rate Timer interrupt

The Interrupt Controller Type Register (ICTR) defines the number of external interrupt lines that are supported.

32-bit read/write-one-to-set register located at 0xE000E100 + 4n.  ---> ISERn 

ISERn MRT0 IRQ = 30 every bit is a 1 for the corresponding interrupt line. Writing a 1 to a bit in this register enables the corresponding interrupt line. so it is less than 31 it falls under the first ISER register (ISER0). The bit position for MRT0 IRQ is 30, so to enable it, you would write a 1 to bit 30 of ISER0.

             CPU                         MRT HARDWARE
              │                               │
              │  do other work                │
              │◄──────────────────────────────│
              │                               │
              │                               │ count
              │                               │ 500 ms
              │                               │
              │◄────── interrupt ─────────────│
              │                               │
          MRT_IRQHandler()                    │
              │                               │
          toggle LED                          │
              │                               │
              └──────── return ───────────────►
                                              │
                                           repeat


                                           Time
│
0 ms       main() doing other work
│
│
│
500 ms ───► MRT interrupt
             │
             ▼
         MRT_IRQHandler()
             │
             ├── clear flag
             └── toggle LED
             │
             ▼
         return from ISR
             │
             ▼
          main()
             │
             │ doing other work
             │
1000 ms ──► MRT interrupt
             │
             ▼
         toggle LED
             │
             ▼
          main()
             │
             ...

/*---------------------------------------FINAL MRT FLOW---------------------------------------*/
CLOCK
 │
 ├── GPIO clock
 │
 └── MRT clock
       │
       ▼
    MRT reset
       │
       ▼
    MRT CH0
       │
       ├── Repeat mode
       ├── Interrupt enable
       └── Interval
              │
              ▼
          IRQ 30
              │
              ▼
        NVIC_ISER0[30]
              │
              ▼
        NVIC_IPR7[23:21]
              │
              ▼
      MRT_IRQHandler()
              │
              ├── Clear INTFLAG
              │
              └── LED toggle







