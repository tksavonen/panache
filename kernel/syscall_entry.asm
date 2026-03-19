; panache/kernel/syscall_entry.asm

GLOBAL syscall_entry
EXTERN syscall_handler
syscall_entry:
    pusha

    PUSH edx
    PUSH ecx
    PUSH ebx
    PUSH eax
    
    CALL syscall_handler
    ADD esp, 16

    POPA
    IRET

