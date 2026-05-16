; panache/proc/switch.asm

GLOBAL context_switch
context_switch:
    PUSH ebx
    PUSH esi
    PUSH edi
    PUSH ebp

    MOV eax, [esp+20]       
    MOV [eax], esp  
    MOV ecx, [esp+28]
    MOV esp, [esp+24]
    MOV cr3, ecx

    POP ebp
    POP edi
    POP esi
    POP ebx

    RET