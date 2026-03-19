; panache/arch/i386/isr128.asm

EXTERN isr_common_stub

GLOBAL isr128
isr128:
    PUSH dword 0
    PUSH dword 128
    JMP isr_common_stub