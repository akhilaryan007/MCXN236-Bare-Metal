ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);   /* top of stack */

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))   /* keep is function that tells the linker to retain the .isr_vector section in the final output, even if it appears to be unused. This is important for interrupt vector tables, which are critical for system startup and interrupt handling. */
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* Stack sanity check: ensure we don't overflow RAM */
    ._stack_check :
    {
        . = ALIGN(8);  /* the start address of the stack must be aligned to 8 bytes divisible by the start address of the RAM */
        . = . + 0x400;   /* reserve 1KB minimum stack   0x400 hex bytes (1024 bytes, or 1 KB) */ 
    } > RAM
} 

/* If your application grows too large and your variables consume almost all of the RAM, the linker will attempt to place this ._stack_check section into what remains.If there is less than 1 KB of RAM left, the linker will throw a compilation error: "region RAM overflowed". */

/* This acts as a safety net, ensuring your build fails immediately at your computer rather than flashing a broken binary onto a device that will instantly crash at runtime due to running out of stack space. */