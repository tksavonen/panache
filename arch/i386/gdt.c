// panache/arch/i386/gdt.c

#include <gdt.h>
#include <util.h>

#include <stdint.h>

typedef uint32_t addr_t;

extern void gdt_flush(void* ptr);
extern void tss_flush();

struct gdt_entry_struct gdt_entries[6];
struct gdt_ptr_struct gdt_ptr;
struct tss_entry_struct tss_entry;

extern uint8_t kernel_stack[65536];
extern uint32_t kernel_stack_top;

extern uint8_t user_stack[4096];
extern uint32_t user_stack_top;

void init_gdt() {
    gdt_ptr.gdt_limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    set_gdt_gate(0, 0, 0, 0, 0);
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    write_tss(5, 0x10, (uint32_t)kernel_stack + sizeof(kernel_stack));
    debug_serial_str("after write_tss:"); debug_serial_hex(tss_entry.esp0); debug_serial_str("\n");

    gdt_flush(&gdt_ptr);
    debug_serial_str("after gdt_flush:"); debug_serial_hex(tss_entry.esp0); debug_serial_str("\n");

    tss_flush();
    debug_serial_str("after tss_flush:"); debug_serial_hex(tss_entry.esp0); debug_serial_str("\n");
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

	tss_entry.ss0 = ss0;
	tss_entry.esp0 = esp0;

	tss_entry.cs = 0x08 | 0x3;
	tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;

	debug_serial_str("write_tss esp0=");
    debug_serial_hex(tss_entry.esp0);
    debug_serial_str("\n");
}
