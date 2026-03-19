; panache/boot/boot.asm

EXTERN __bss_start
EXTERN __bss_end

section .multiboot
ALIGN 4
DD 0x1BADB002
DD 0
DD -(0x1BADB002)

section .text
GLOBAL _start
EXTERN kernel_main

_start:
	MOV edi, __bss_start
	MOV ecx, __bss_end
	SUB ecx, edi
	XOR eax, eax
	SHR ecx, 2
	REP stosd
	
    CALL kernel_main

.hang:
    CLI
    HLT
    JMP .hang
