// panache/kernel/util.c

#include <util.h>

#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>

void *memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--) {
        *temp++ = val;
    }
    return temp;
}

// Write 8 bits of data to an I/O port address, enabling direct communication with HW.
void out_port_b(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void k_itoa(int value, char* buffer) {
    char temp[32];
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (is_negative) {
        temp[i++] = '-';
    }

    int j = 0;
    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}


