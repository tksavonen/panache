ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot2 :
    {
        KEEP(*(.multiboot2))
    }

    .text :
    {
        *(.text*)
    }

    .data :
    {
        *(.data*)
    }

    .rodata : 
    {
        *(.rodata*)
    }

    .bss :
    {
    	__bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }
}
