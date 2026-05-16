// panache/include/kmalloc.h

#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>

void  kmalloc_init(uint32_t heap_start, uint32_t heap_size);
void *kmalloc(uint32_t size);
void  kfree(void *ptr);
void  kmalloc_dump(void);  // debug: print heap state

#endif