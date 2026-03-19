// panache/arch/i386/gdt.c

#include <gdt.h>
#include <util.h>

extern void gdt_flush(addr_t);
extern void tss_flush();

struct gdt_entry_struct gdt_entries[6];
struct gdt_ptr_struct gdt_ptr;
struct tss_entry_struct tss_entry;

extern uint8_t kernel_stack[4096];
extern uint32_t kernel_stack_top;

extern uint8_t user_stack[4096];
extern uint32_t user_stack_top;

void init_gdt() {
	gdt_ptr.gdt_limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
	gdt_ptr.base = (uint32_t)&gdt_entries;

	set_gdt_gate(0, 0, 0, 0, 0);					// null segment
	set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);		// kernel code segment (dpl0)
	set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);		// kernel data segment (dpl0)
	set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);		// user code segment (dpl3)
	set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);		// user data segment (dpl3)

	write_tss(5, 0x10, kernel_stack_top);  // esp0 points to top of kernel stack

	gdt_flush(&gdt_ptr);
	tss_flush();
}

void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
		gdt_entries[num].base_low = (base & 0xFFFF);
		gdt_entries[num].base_middle = (base >> 16) & 0xFF;
		gdt_entries[num].base_high = (base >> 24) & 0xFF;

		gdt_entries[num].limit = (limit & 0xFFFF);
		gdt_entries[num].flags = (limit >> 16) & 0xFF;
		gdt_entries[num].flags |= (gran & 0xF0);

		gdt_entries[num].access = access;
}

void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0) {
	uint32_t base = (uint32_t) &tss_entry;
	uint32_t limit = sizeof(tss_entry) - 1;
	
	set_gdt_gate(num, base, limit, 0x89, 0x00);

	memset(&tss_entry, 0, sizeof(tss_entry));

	tss_entry.iomap_base = sizeof(tss_entry);

	tss_entry.ss0 = 0x10;
	tss_entry.esp0 = kernel_stack_top;

	tss_entry.cs = 0x08 | 0x3;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}
