// panache/arch/i386/gdt.c

#include "gdt.h"

extern void gdt_flush(addr_t);

struct gdt_entry_struct gdt_entries[5];
struct gdt_ptr_struct gdt_ptr;

void init_gdt() {
	gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
	gdt_ptr.base = &gdt_entries;

	set_gdt_gate(0, 0, 0, 0, 0);					// null segment
	set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);		// kernel segment

	// TODO: define set_gdt_gate here
}
