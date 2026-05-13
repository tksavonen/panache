; panache/arch/i386/isr128.asm

EXTERN isr_common_stub
EXTERN syscall_handler
EXTERN debug_marker

GLOBAL isr128
isr128:
    PUSHA

    MOV eax, [esp+28]
    PUSH edx
    PUSH ecx
    PUSH ebx
    PUSH eax

    CALL syscall_handler

    ADD esp, 16
    POPA
    IRET