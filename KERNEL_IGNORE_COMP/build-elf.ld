ENTRY(_start)
SECTIONS
{
    . = 0x00800000;

    .text : { *(.text*) }

    /DISCARD/ :
    {
        *(.eh_frame*)
        *(.note*)
        *(.gnu*)
        *(.dynamic*)
        *(.got*)
        *(.interp*)
        *(.dynsym*)
        *(.dynstr*)
        *(.rela*)
        *(.rel*)
        *(.comment*)
        *(.data*)
        *(.bss*)
        *(.rodata*)
    }
}