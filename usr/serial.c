// panache/usr/serial.c

#include <stdint.h>

#define COM1 0x3F8

void outb(uint16_t port, uint8_t val) {
    asm volatile (
        "outb %%al, %%dx"
        :
        : "a"(val), "d"(port)
    );
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void serial_init() {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

int serial_ready() {
    return inb(COM1 + 5) & 0x20;
}

void serial_write(char c) {
    while (!serial_ready());
    outb(COM1, c);
}

void debug_outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t debug_inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

void debug_serial_init() {
    debug_outb(0x3F8 + 1, 0x00); // disable interrupts
    debug_outb(0x3F8 + 3, 0x80); // enable DLAB
    debug_outb(0x3F8 + 0, 0x03); // baud lo (38400)
    debug_outb(0x3F8 + 1, 0x00); // baud hi
    debug_outb(0x3F8 + 3, 0x03); // 8N1, clear DLAB
    debug_outb(0x3F8 + 2, 0xC7); // enable FIFO
    debug_outb(0x3F8 + 4, 0x0B); // RTS/DSR
}

void debug_serial_char(char c) {
    while (!(debug_inb(0x3F8 + 5) & 0x20));  // wait for transmit empty
    debug_outb(0x3F8, c);
}

void debug_serial_hex(uint32_t val) {
    debug_serial_char('0');
    debug_serial_char('x');
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        debug_serial_char(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
    }
    debug_serial_char('\n');
}

void debug_serial_str(const char *s) {
    while (*s) debug_serial_char(*s++);
}