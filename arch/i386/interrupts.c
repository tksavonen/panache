// panache/arch/i386/interrupts.c

#include <anemoia.h>
#include <interrupts.h>
#include <util.h>
#include <vga.h>
#include <stdint.h>

static volatile uint64_t tick = 0;

uint64_t timer_ticks() { return tick; }

uint64_t k_uptime_seconds() { return ktime.seconds; }
uint64_t k_uptime_millis() { return ktime.millis; }
uint64_t k_uptime_ticks() { return ktime.ticks; }

struct idt_entry_struct idt_entries[256];
struct idt_ptr_struct idt_ptr;

extern void idt_flush(uint32_t* idt_ptr);
extern void (*isr_stub_table[32])(void);
extern void (*irq_stub_table[16])(void);

typedef void (*irq_handler_t)(struct interrupt_registers* regs);

void init_idt() {
    idt_ptr.limit = sizeof(struct idt_entry_struct) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry_struct) * 256);

    out_port_b(0x20, 0x11);
    out_port_b(0xA0, 0x11);

    out_port_b(0x21, 0x20);
    out_port_b(0xA1, 0x28);

    out_port_b(0x21, 0x04);
    out_port_b(0xA1, 0x02);

    out_port_b(0x21, 0x01);
    out_port_b(0xA1, 0x01);

    out_port_b(0x21, 0x0);
    out_port_b(0xA1, 0x0);  
    
    for (int i = 0; i < 32; i++) {
        set_idt_gate(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);
    }

    for (int i = 0; i < 16; i++) {
        set_idt_gate(32 + i, (uint32_t)irq_stub_table[i], 0x08, 0x8E);
    }

    set_idt_gate(128, (uint32_t)isr128, 0x08, 0xEE);
    set_idt_gate(177, (uint32_t)isr177, 0x08, 0xEE);

    idt_flush((uint32_t)&idt_ptr);
}

void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

static irq_handler_t irq_routines[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

const char *exception_msgs[] = {
    "Division by Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Triple Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(struct interrupt_registers* regs) {
    if (regs->int_no < 32) {
    k_set_color(COLOR_RED, COLOR_BLACK);

    k_print("\n=== EXCEPTION ===\n");
    k_print("Type: ");
    k_print(exception_msgs[regs->int_no]);
    k_new_line();

    k_print("INT NO: "); k_print_hex(regs->int_no); k_new_line();
    k_print("ERR CODE: "); k_print_hex(regs->err_code); k_new_line();

    k_print("EIP: "); k_print_hex(regs->eip); k_new_line();
    k_print("EFLAGS: "); k_print_hex(regs->eflags); k_new_line();

    k_print("ESP: "); k_print_hex(regs->esp); k_new_line();
    k_print("SS: "); k_print_hex(regs->ss); k_new_line();
    k_print("CS: "); k_print_hex(regs->csm); k_new_line();

    k_print("EAX: "); k_print_hex(regs->eax); k_new_line();
    k_print("EBX: "); k_print_hex(regs->ebx); k_new_line();
    k_print("ECX: "); k_print_hex(regs->ecx); k_new_line();
    k_print("EDX: "); k_print_hex(regs->edx); k_new_line();

    k_print("ESI: "); k_print_hex(regs->esi); k_new_line();
    k_print("EDI: "); k_print_hex(regs->edi); k_new_line();
    k_print("EBP: "); k_print_hex(regs->ebp); k_new_line();

    k_print("=================\n");

    for (;;);
}

    else if (regs->int_no >= 32 && regs->int_no < 47) {
        int irq = regs->int_no - 32;

        if (irq_routines[irq]) {
            irq_routines[irq](regs);
        }

        if (regs->int_no >= 40) {
            out_port_b(0xA0, 0x20);
        }
        out_port_b(0x20, 0x20);
    }

    else if (regs->int_no == 128 || regs->int_no == 177) {
        k_set_color(COLOR_LIGHT_BLUE, COLOR_BLACK); k_print("SYSCALL!"); k_set_color(COLOR_BRIGHT_WHITE, COLOR_BLACK);
        k_new_line(); k_new_line();
        syscall_handler(regs->eax, regs->ebx, regs->ecx, regs->edx);
        return; 
    }
}

void irq_install_handler(int irq, irq_handler_t handler) {
    if (irq < 0 || irq > 15) return;
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    if (irq < 0 || irq > 15) return;
    irq_routines[irq] = 0;
}

void timer_callback(struct interrupt_registers* regs) {
    (void)regs;
    ktime.ticks++;
    if (ktime.ticks % 100 == 0) {
        ktime.seconds++;
    }
    ktime.millis = ktime.ticks * 10;
}

void sleep(uint64_t ms) {
    uint64_t target = k_uptime_millis() + ms;
    while (k_uptime_millis() < target) {
        asm volatile("hlt");
    }
}

void pit_set_freq(uint32_t hz) {
    uint32_t div = PIT_FREQUENCY / hz;
    out_port_b(PIT_COMMAND, 0x36);
    out_port_b(PIT_CHANNEL0, div & 0xFF);
    out_port_b(PIT_CHANNEL0, (div >> 8) & 0xFF);
}

void init_time() {
    pit_set_freq(100);
    irq_install_handler(0, timer_callback);
}

