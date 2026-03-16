// panache/include/util.h

#ifndef UTIL_H
#define UTIL_H

#include "stdint.h"

void *memset(void *dest, char val, uint32_t count);
void out_port_b(uint16_t port, uint8_t val);
void k_itoa(int value, char* buffer);

struct interrupt_registers {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t csm;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
};

#endif