// panache/drivers/keyboard/keyboard.c

#include <keyboard.h>
#include <util.h>
#include <interrupts.h>

#include <stdint.h>

static uint8_t key_buffer[256];
static uint8_t key_read  = 0;
static uint8_t key_write = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void keyboard_handler(struct interrupt_registers *regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) return;

     if (scancode < sizeof(scancode_map)) {
        char c = scancode_map[scancode];

        if (c)
            key_buffer[key_write++] = c;
     }
}

void keyboard_init(void) {
    irq_install_handler(1, keyboard_handler);
}

char keyboard_getchar(void) {
    if (key_read == key_write) return 0; 
    return key_buffer[key_read++];
}

uint8_t keyboard_haskey(void) {
    return key_read != key_write;
}