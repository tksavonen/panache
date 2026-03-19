// panache/kernel/anemoia.c
// ----- THE PANACHEOS KERNEL -----

#include <vga.h>
#include <gdt.h>
#include <interrupts.h>
#include <syscall.h>
#include <stdint.h>

extern uint8_t kernel_stack[4096];
extern uint32_t kernel_stack_top;

extern uint8_t user_stack[4096];
extern uint32_t user_stack_top;

extern void enter_user_mode(void);

struct tss_entry_struct tes;

#define SYS_PRINT 1

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    switch (eax) {
        case SYS_PRINT: {
            const char* str = (const char*)ebx;
            k_print(str);
            break;
        }
        default:
            k_print("Unknown syscall\n");
            break;
    }
}

void kernel_main() {
	k_reset();
	k_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
	k_print("You are in the ANEMOIA kernel.");
    k_new_line();
    k_set_color(COLOR_WHITE, COLOR_BLACK);

	init_gdt();
	init_idt();

    k_print("Leaving kernel space...");
    k_new_line();

    asm volatile("cli");
    enter_user_mode();
    __builtin_unreachable();
}
