/* weak tells the linker that if no other definition of NMI_Handler is found, use Default_Handler instead and alias tells the linker to use Default_Handler as the implementation of NMI_Handler This allows the user to provide their own implementation of NMI_Handler in their application code, which will override the weak alias If no user-defined implementation is provided, the Default_Handler will be used instead*/

                 .c FILE
                   │
                   ▼
             PREPROCESSOR
                   │
          #include is expanded
                   │
                   ▼
          TRANSLATION UNIT
                   │
                   ▼
               COMPILER
                   │
                   ▼
                 .o
                   │
                   │
          ┌────────┴────────┐
          │                 │
       main.o          startup.o
          │                 │
          └────────┬────────┘
                   ▼
                LINKER
                   │
                   ▼
             firmware.elf


MCXN236.h
void my_function(void);
startup_MCXN236.c
void my_function(void)
{
    // implementation
}
main.c
#include "MCXN236.h"

int main(void)
{
    my_function();
}

This works.

main.c
   ↓
main.o
   └── "I need a function called my_function"

startup_MCXN236.c
   ↓
startup_MCXN236.o
   └── "Here is the actual my_function implementation"

----->> Step 3 — The LINKER connects them

This is the part you were slightly attributing to the compiler.

The compiler does NOT search throughout the entire project to find the implementation.

Instead:

Compiler
   │
   ├── main.c → main.o
   │
   └── startup_MCXN236.c → startup_MCXN236.o
                            │
                            ▼
                         LINKER
                            │
                matches the symbol
                            │
              my_function() ←→ my_function()
                            │
                            ▼
                     firmware.elf

The linker sees:

main.o:
    needs → my_function

and:

startup_MCXN236.o:
    provides → my_function


Normal function call:
main.o ───────────────► linker ───────────────► startup.o
                         ✅ can resolve across .c files


GCC alias:
alias("Default_Handler")
        │
        └── compiler needs Default_Handler
            in THIS translation unit
            ❌ doesn't wait for linker

---> The preprocessor copies the declaration from the header into the translation unit. The compiler uses that declaration to compile the function call. Each .c file is compiled separately. Later, the linker searches the object files for the actual definition of that function and connects the call to that definition.

So normal function calls are resolved by the linker across .c files, while GCC's alias() attribute is handled by the compiler and has the same-translation-unit requirement.

#define SYSCON_BASE 0x40000000UL

#define REG32(addr) (*(volatile uint32_t *)(addr))

#define AHBCLKCTRL0 REG32(SYSCON_BASE + 0x200UL)

void Clock_Enable(void)
{
    AHBCLKCTRL0 |= (1U << 19);
    AHBCLKCTRL0 |= (1U << 13);
}

static inline void Clock_Enable(void)
{
    AHBCLKCTRL0 |= (1U << 19) | (1U << 13);
}

----> Why is static inline commonly useful in firmware?

Imagine you create small register-access functions:

static inline void GPIO_Set(uint32_t mask)
{
    GPIO->PDOR |= mask;
}

Then:

GPIO_Set(1U << 18);

Instead of having lots of function-call overhead for tiny operations, the compiler can inline it.

------> The SYSCON->AHBCLKCTRL[19] syntax is different

This:

SYSCON->AHBCLKCTRL[19] |= (1U << 19);

does not mean "bit 19 of AHBCLKCTRL".

The [19] means array element 19.

For example, if the header defines:

typedef struct
{
    volatile uint32_t AHBCLKCTRL[2];
} SYSCON_Type;

#define SYSCON ((SYSCON_Type *)0x40000000UL)

then:

SYSCON->AHBCLKCTRL[0]

means the first register,

SYSCON->AHBCLKCTRL[1]

means the second register.

It does not mean bit 0 or bit 1.

So if your register is AHBCLKCTRL0, you would normally have something like:

SYSCON->AHBCLKCTRL[0] |= (1U << 19);

if the SDK structure represents the registers that way.

                 ┌──────────────────┐
                 │   AHBCLKCTRL0    │
                 │                  │
                 │ bit19 = GPIO0    │
                 │ bit13 = PORT0    │
                 └────────┬─────────┘
                          ▲
             ┌────────────┴────────────┐
             │                         │
     AHBCLKCTRLSET0             AHBCLKCTRLCLR0
       0x40000220                 0x40000240
             │                         │
       write 1 → SET              write 1 → CLEAR

                    SYSCON
                      │
          ┌───────────┴────────────┐
          │                        │
      CLOCK                        RESET
          │                        │
    AHBCLKCTRLSET             PRESETCTRLCLR
          │                        │
          ↓                        ↓
      GPIO0 clock              GPIO0 out of reset
      PORT0 clock              PORT0 out of reset
          │                        │
          └───────────┬────────────┘
                      ↓
                 GPIO / PORT
                      │
              configure pin
                      │
                PDDR / PCR
                      │
                 LED output

Configure the pin mux for the desired pin using the PORT PCR registers.

~(0xFU << 8)
-------> The formula behind it

If the field is [HIGH:LOW]:

width = HIGH - LOW + 1

Then:

field_mask = (1U << width) - 1

And clearing the field:

REGISTER &= ~(field_mask << LOW);

For your MUX [11:8]:

width = 11 - 8 + 1
      = 4

field_mask = (1U << 4) - 1
           = 16 - 1
           = 15
           = 0xF

Therefore:

PCR0 &= ~(((1U << 4) - 1) << 8);

which simplifies to:

PCR0 &= ~(0xFU << 8);


Updating the state of multiple pins by using the following ways:
— Writing to Port Data Output (PDOR).
— Writing to Port Set Output (PSOR) to write 1 to Port Data Output (PDOR).
— Writing to Port Clear Output (PCOR) to write 0 to Port Data Output (PDOR).
— Writing to Port Toggle Output (PTOR) to toggle Port Data Output (PDOR).


Physical pin
     │
     ▼
PORTx PCR0
     │
     │ MUX[11:8] = 0000
     ▼
GPIO function selected
     │
     ▼
GPIO PDDR bit 0
     │
     │ 1 = output
     ▼
GPIO output register
     │
     ├── PSOR → set output HIGH
     ├── PCOR → clear output LOW
     └── PTOR → toggle output


----------->  Step 1: Convert Hexadecimal to DecimalThe prefix 0x means the number is in hexadecimal (base-16). In hex, the position on the left represents 16s, and the position on the right represents 1s.
Calculation: 0x40 16^1x0 + 16^2x4 =0+64 =64 byes

ARM processors are 32 bit so 64/4 =16 bytes 
now in struct I can use the uint32_t reserved[16]; for the off sets of GPIO registers.

typedef struct
{
    uint32_t RESERVED0[16];   // 16 × 4 = 0x40

    volatile uint32_t PDOR;   // 0x40
    volatile uint32_t PSOR;   // 0x44

    uint32_t RESERVED1[3];    // 0x48, 0x4C, 0x50

    volatile uint32_t PDDR;   // 0x54

} GPIO_Type;

typedef struct
{
    uint8_t RESERVED0[0x40];

    volatile uint32_t PDOR;   // 0x40
    volatile uint32_t PSOR;   // 0x44

    uint8_t RESERVED1[0x0C];  // 0x48 → 0x53

    volatile uint32_t PDDR;   // 0x54

} GPIO_Type; is also valid


                 PDDR
                  │
                  │ 1 = OUTPUT
                  ▼
              GPIO PIN
                  │
        ┌─────────┴─────────┐
        │                   │
       PSOR                PCOR
        │                   │
     SET = 1             CLEAR = 0
        │                   │
        └─────────┬─────────┘
                  ▼
                 PDOR
                  │
                  ▼
             Physical pin
                  │
                  ▼
                LED

Clock to GPIO/PORT
        ↓
Release peripheral reset
        ↓
PORT PCR → select GPIO function
        ↓
GPIO PDDR → output
        ↓
GPIO PSOR/PCOR/PTOR/PDOR
        ↓
Physical MCU pin
        ↓
PCB LED
        ↓
Blink


                CLOCK
                  │
                  ▼
          AHBCLKCTRL0
          bit 23 = GPIO4
          bit 17 = PORT4
                  │
                  ▼
                RESET
                  │
                  ▼
          PORT4 PCR19
                  │
             MUX[11:8]
                  │
               0000
                  │
                  ▼
             GPIO4[19]
                  │
                  ▼
             PDDR[19] = 1
                  │
                  ▼
             GPIO OUTPUT
                  │
          ┌───────┴───────┐
          │       │       │
        PSOR     PCOR    PTOR
          │       │       │
        SET     CLEAR    TOGGLE
          │       │       │
          └───────┬───────┘
                  ▼
               P4_19
                  │
                  ▼
             GREEN LED


I'd add a compile-time check:

_Static_assert(offsetof(GPIO_Type, PDOR) == 0x40, "PDOR offset wrong");
_Static_assert(offsetof(GPIO_Type, PDIR) == 0x50, "PDIR offset wrong");
_Static_assert(offsetof(GPIO_Type, PDDR) == 0x54, "PDDR offset wrong");

You would need:

#include <stddef.h>

This is excellent practice when you're learning register-level programming. 


2. What does one NOP take?

This is where your clock frequency becomes important.

Suppose your Cortex-M33 is running at:

150 MHz

Then:

150 MHz = 150,000,000 clock cycles/second

Therefore one clock cycle is:

1 / 150,000,000
= 6.6667 ns

A NOP instruction on Cortex-M is generally one cycle when executed normally.

So approximately:

1 NOP ≈ 6.67 ns

at 150 MHz.

 RGB LEDs are active-low:

Pin 0 = LED on
Pin 1 = LED off
