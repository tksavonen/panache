; panache/arch/i386/isr177.asm

EXTERN isr_common_stub

GLOBAL isr177
isr177:
    PUSH dword 0
    PUSH dword 177
    JMP isr_common_stub