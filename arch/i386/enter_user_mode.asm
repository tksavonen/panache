; panache/arch/i386/enter_user_mode.asm

GLOBAL enter_user_mode
EXTERN user_main
EXTERN user_stack_top
EXTERN k_print_hex

enter_user_mode:
    MOV ax, 0x23               
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    MOV eax, user_stack_top

    PUSH 0x23                
    PUSH eax

    pushf                    
    PUSH 0x1B    
                
    PUSH user_mode_entry 
    IRET

GLOBAL user_mode_entry
user_mode_entry:
    MOV ax, 0x23
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax
    
    MOV eax, 0xDEADBEEF
    CALL user_main
.loop:
    JMP .loop