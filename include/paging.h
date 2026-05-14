// panache/include/paging.h

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE        4096
#define PAGE_ENTRIES     1024
#define PAGE_PRESENT     0x01
#define PAGE_RW          0x02
#define PAGE_USER        0x04
#define PAGE_4MB         0x80

typedef uint32_t page_table_entry_t;
typedef uint32_t page_directory_entry_t;

typedef struct {
    page_table_entry_t entries[PAGE_ENTRIES];
} page_table_t;

typedef struct {
    page_directory_entry_t entries[PAGE_ENTRIES];
} page_directory_t;

extern page_directory_t *kernel_page_directory;

void init_paging();
void paging_enable(void);

#endif