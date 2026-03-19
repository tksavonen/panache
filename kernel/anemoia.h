// panache/kernel/anemoia.h

#ifndef ANEMOIA_H
#define ANEMOIA_H

#include <stdint.h>

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

#endif