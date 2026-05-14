// panache/memory/paging.c

#include <paging.h>
#include <serial.h>

#include <stdint.h>

#define IDENTITY_LIMIT (16 * 1024 * 1024) // 16 MiB

static page_directory_entry_t   page_directory[1024] __attribute__((aligned(4096)));
static page_table_entry_t       page_tables[256][1024] __attribute__((aligned(4096)));
static page_table_entry_t       fb_page_table[1024] __attribute__((aligned(4096)));

void init_paging() {
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0;

    // 0x00000000 - 0x40000000
    for (int table = 0; table < 256; table++) {
        for (int page = 0; page < 1024; page++) {
            uint32_t phys = (table * 1024 + page) * 0x1000;
            page_tables[table][page] = phys | PAGE_PRESENT | PAGE_RW | PAGE_USER;
        }
        page_directory[table] = (uint32_t)page_tables[table] | PAGE_PRESENT | PAGE_RW | PAGE_USER;
    }

    uint32_t fb_phys = 0xFD000000;
    for (int page = 0; page < 1024; page++) {
        fb_page_table[page] = (fb_phys + page * 0x1000) | PAGE_PRESENT | PAGE_RW;
    }
    page_directory[1012] = (uint32_t)fb_page_table | PAGE_PRESENT | PAGE_RW;
}

void paging_enable(void) {
    asm volatile(
        "mov %0, %%cr3\n"          
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"  // set bit 31 (PG flag)
        "mov %%eax, %%cr0\n"       // enable paging
        :
        : "r"(page_directory)
        : "eax"
    );
}
