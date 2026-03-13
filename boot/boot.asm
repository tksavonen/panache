; panache/boot/boot.asm

extern __bss_start
extern __bss_end

section .multiboot
align 4
dd 0x1BADB002
dd 0
dd -(0x1BADB002)

section .text
global _start
extern kernel_main

_start:
	mov edi, __bss_start
	mov ecx, __bss_end
	sub ecx, edi
	xor eax, eax
	shr ecx, 2
	rep stosd
	
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
