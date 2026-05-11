; panache/boot/boot.asm

EXTERN __bss_start
EXTERN __bss_end

section .multiboot
ALIGN 8

MB2_MAGIC      	equ 0xE85250D6
MB2_ARCH_I386  	equ 0
MB2_HEADER_LEN 	equ mb2_header_end - mb2_header_start

mb2_header_start:
    DD MB2_MAGIC              ; magic
    DD MB2_ARCH_I386          ; arch
    DD MB2_HEADER_LEN         ; header length
    DD -(MB2_MAGIC + MB2_ARCH_I386 + MB2_HEADER_LEN)                      ; checksum 

mb2_tag_framebuffer:
    DW 5                      
    DW 0                      
    DD mb2_tag_framebuffer_end - mb2_tag_framebuffer
    DD 1024                  
    DD 768                   
    DD 32                    
mb2_tag_framebuffer_end:

mb2_tag_end:
    DW 0                      
    DW 0                      
    DD 8                      

mb2_header_end:

section .multiboot2
ALIGN 4

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
	
	PUSH ebx
	PUSH eax
    CALL kernel_main

.hang:
    CLI
    HLT
    JMP .hang
