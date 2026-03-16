// panache/kernel/anemoia.c
// ----- THE PANACHEOS KERNEL -----

#include <vga.h>
#include <gdt.h>
#include <interrupts.h>
#include <stdint.h>

void kernel_main() {
	k_reset();
	k_set_color(COLOR_WHITE, COLOR_BLACK);
	k_print("You are in the ANEMOIA kernel.");

	k_new_line();
	init_gdt();
	k_print("GDT is initialized!");

	k_new_line();
	init_idt();
	k_print("Enabling interrupts...");

	init_time();
}
