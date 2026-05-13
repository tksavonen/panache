// panache/include/serial.h

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init();
void serial_write(char c);

void debug_serial_char(char c);
void debug_serial_str(const char *s);
void debug_serial_hex(uint32_t val);

void debug_outb(uint16_t port, uint8_t val);

#endif