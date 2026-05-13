; panache/arch/i386/idt_flush.asm

GLOBAL idt_flush
idt_flush:
    MOV eax, [esp+4]
    LIDT [eax]
    STI
    RET

%macro ISR_NOERRCODE 1
GLOBAL isr%1
isr%1:
    PUSH LONG 0
    PUSH LONG %1
    JMP isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
GLOBAL isr%1
isr%1:
    PUSH LONG %1
    JMP isr_common_stub
%endmacro

%macro IRQ_STUB 1
GLOBAL irq%1
irq%1:
    PUSH DWORD 0       
    PUSH DWORD (%1+32)      
    JMP irq_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7

ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

GLOBAL isr_common_stub
EXTERN isr_handler
isr_common_stub:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp              
    call isr_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8            
    iret

GLOBAL isr_stub_table
isr_stub_table:
    %assign i 0
    %rep 32
        dd isr%+i
    %assign i i+1
    %endrep

GLOBAL irq_common_stub
EXTERN irq_handler
irq_common_stub:
    pusha

    push gs
    push fs
    push es
    push ds

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler     
    add esp, 4

    pop ds
    pop es
    pop fs
    pop gs

    popa
    add esp, 8
    iret

GLOBAL irq_stub_table
irq_stub_table:
    %assign i 0
    %rep 16
        dd irq%+i
    %assign i i+1
    %endrep

