// panache/arch/i386/stack.c

#include <stdint.h>

__attribute__((section(".stack")))
uint8_t kernel_stack[65536];

__attribute__((section(".stack")))  
uint8_t user_stack[4096];

uint32_t kernel_stack_top = (uint32_t)kernel_stack + sizeof(kernel_stack);
uint32_t user_stack_top   = (uint32_t)user_stack   + sizeof(user_stack);