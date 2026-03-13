// PanacheOS/kernel/anemoia.c
// THE PANACHEOS KERNEL

#include <vga.h>

void kernel_main() {
	k_reset();
	k_set_color(COLOR_WHITE, COLOR_BLACK);
	k_print("You are in the ANEMOIA kernel.");
}
