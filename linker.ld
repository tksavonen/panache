ENTRY(_start)

SECTIONS
{
    . = 1M;

    .multiboot :
    {
        *(.multiboot)
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
