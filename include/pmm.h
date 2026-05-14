// panache/include/pmm.h

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PMM_PAGE_SIZE   4096
#define PMM_MAX_FRAMES  (128 * 1024 * 1024 / PMM_PAGE_SIZE) // 128MB / 4KB = 32768 frames

void     pmm_init(uint32_t mem_start, uint32_t mem_end);
void     pmm_mark_used(uint32_t addr);
void     pmm_mark_free(uint32_t addr);
uint32_t pmm_alloc(void);   // returns physical address of free frame
void     pmm_free(uint32_t addr);

// debug
uint32_t pmm_free_frames(void);

#endif