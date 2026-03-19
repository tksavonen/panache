#include <stdint.h>

uint8_t kernel_stack[4096];
uint32_t kernel_stack_top = (uint32_t)kernel_stack + sizeof(kernel_stack);

uint8_t user_stack[4096];
uint32_t user_stack_top = (uint32_t)user_stack + sizeof(user_stack);