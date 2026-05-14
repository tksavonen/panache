// panache/include/interrupts.h

#ifndef IDT_H
#define IDT_H

#include <util.h>

#include <stdint.h>

#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43
#define PIT_FREQUENCY   1193180

struct idt_entry_struct {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
}__attribute__((packed));

struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
}__attribute__((packed));

struct anemoia_time {
    uint64_t ticks;
    uint64_t seconds;
    uint64_t millis;
};

static struct anemoia_time ktime = {0};

uint64_t k_uptime_seconds();
uint64_t k_uptime_millis();
uint64_t k_uptime_ticks();

typedef void (*irq_handler_t)(struct interrupt_registers* regs);
void init_idt();
void set_idt_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void isr_handler(struct interrupt_registers* regs);
void irq_install_handler(int irq, irq_handler_t handler);
extern void isr128(void);
extern void isr177(void);
void init_time();
void sleep(uint64_t ms);

#endif