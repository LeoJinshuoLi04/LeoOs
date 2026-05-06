ENTRY(_start)

SECTIONS
{
    /* 1. Define the Virtual Address (VMA) */
    /* We start at 3GB + 1MB to leave room for lower tables */
    . = 0xC0100000;

    /* 2. Start the sections */
    _kernel_start = .;

    /* The AT() command specifies the Physical Load Address (LMA) */
    .text BLOCK(4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata BLOCK(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss BLOCK(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }

    _kernel_end = .;
}