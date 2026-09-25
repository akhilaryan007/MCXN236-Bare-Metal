SysTick 24-bit down-counter
void SysTick_Handler(void)
{
    msTicks++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < ms) {
        __asm__("WFI");   // optional: sleep until next interrupt
    }
}

SysTick base = 0xE000E010

Offset   Register   Address
0x00     CTRL       0xE000E010
0x04     LOAD       0xE000E014
0x08     VAL        0xE000E018
0x0C     CALIB      0xE000E01C

The compiler calculates the member offsets from your struct:

SysTick
  │
  ├── CTRL  +0x00 → 0xE000E010
  ├── LOAD  +0x04 → 0xE000E014
  ├── VAL   +0x08 → 0xE000E018
  └── CALIB +0x0C → 0xE000E01C  

  Your mask:

0x00FFFFFF

31        24 23                         0
+-----------+----------------------------+
| 00000000  | 111111111111111111111111  |
+-----------+----------------------------+

AND:

10101010 110011001100110011001100
AND
00000000 111111111111111111111111
---------------------------------
00000000 110011001100110011001100

So:

uint32_t value = REG & 0x00FFFFFFUL;


-----> Those are not external functions. They are just compiler warnings suppressors.

This pattern:

(void)TICKINT;
(void)CSR_COUNTFLAG_STATUS;
(void)SYST_RVR_VALUE;
(void)SYS_CVR_vALUE;
(void)SYST_RVR_24BITS;

means:

“I intentionally read these variables”
“I am not using them afterward”
“Please do not warn me about unused variables”
This is common when you compile with -Wall, and the compiler says something like:

variable set but not used

Since the file is just a test/driver example, the variables are read to inspect SysTick registers, but no real logic uses them later. The (void) cast forces the compiler to treat the expression as intentionally used.

Example 

volatile uint32_t x = 5;
(void)x;

This does nothing at runtime, but tells the compiler: “I know x is unused on purpose.”

In your case
They are there because this file is not yet part of the real firmware logic; it is more like a register-probe or reference example. In actual application code, you usually remove them once the register values are used for something meaningful.

So: they are not “external functions”; they are just unused-variable guard statements.

FRO_HF = 48 MHz
RVR = Core_clock / 1000 - 1 = 48000 - 1 = 47999  1 ms tick