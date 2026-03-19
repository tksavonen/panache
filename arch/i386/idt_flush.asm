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
    PUSH DWORD %1       
    JMP isr_common_stub
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

IRQ_STUB 32
IRQ_STUB 33
IRQ_STUB 34
IRQ_STUB 35
IRQ_STUB 36
IRQ_STUB 37
IRQ_STUB 38
IRQ_STUB 39
IRQ_STUB 40
IRQ_STUB 41
IRQ_STUB 42
IRQ_STUB 43
IRQ_STUB 44
IRQ_STUB 45
IRQ_STUB 46
IRQ_STUB 47

GLOBAL isr_common_stub
EXTERN isr_handler
isr_common_stub:
    PUSHA

    MOV eax, ds
    PUSH eax
    
    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    PUSH esp
    CALL isr_handler

    ADD esp, 4

    POP ebx
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx

    POPA
    ADD esp, 8
    IRET

GLOBAL isr_stub_table
isr_stub_table:
    %assign i 0
    %rep 32
        dd isr%+i
    %assign i i+1
    %endrep

GLOBAL irq_stub_table
irq_stub_table:
    %assign i 32
    %rep 16
        dd irq%+i
    %assign i i+1
    %endrep

