// panache/memory/pmm.c

#include <pmm.h>
#include <serial.h>

#include <stdint.h>

static uint32_t pmm_bitmap[PMM_MAX_FRAMES / 32];
static uint32_t pmm_total_frames = 0;
static uint32_t pmm_used_frames  = 0;

static void bitmap_set(uint32_t frame) {
    pmm_bitmap[frame / 32] |= (1 << (frame % 32));
}

static void bitmap_clear(uint32_t frame) {
    pmm_bitmap[frame / 32] &= ~(1 << (frame % 32));
}

static uint32_t bitmap_test(uint32_t frame) {
    return pmm_bitmap[frame / 32] & (1 << (frame % 32));
}

void pmm_init(uint32_t mem_start, uint32_t mem_end) {
    // mark everything as used initially
    for (int i = 0; i < PMM_MAX_FRAMES / 32; i++)
        pmm_bitmap[i] = 0xFFFFFFFF;

    pmm_total_frames = (mem_end - mem_start) / PMM_PAGE_SIZE;
    pmm_used_frames  = pmm_total_frames;

    // mark usable range as free
    uint32_t frame_start = mem_start / PMM_PAGE_SIZE;
    for (uint32_t i = 0; i < pmm_total_frames; i++) {
        bitmap_clear(frame_start + i);
        pmm_used_frames--;
    }

    debug_serial_str("pmm_init start=");
    debug_serial_hex(mem_start);
    debug_serial_str(" end=");
    debug_serial_hex(mem_end);
    debug_serial_str("\nframe_start=");
    debug_serial_hex(frame_start);
    debug_serial_str(" total=");
    debug_serial_hex(pmm_total_frames);
    debug_serial_str("\n");
}

void pmm_mark_used(uint32_t addr) {
    uint32_t frame = addr / PMM_PAGE_SIZE;
    if (!bitmap_test(frame)) {
        bitmap_set(frame);
        pmm_used_frames++;
    }
}

void pmm_mark_free(uint32_t addr) {
    uint32_t frame = addr / PMM_PAGE_SIZE;
    if (bitmap_test(frame)) {
        bitmap_clear(frame);
        pmm_used_frames--;
    }
}

uint32_t pmm_alloc(void) {
    for (uint32_t i = 0; i < PMM_MAX_FRAMES / 32; i++) {
        if (pmm_bitmap[i] == 0xFFFFFFFF) continue;  // all used, skip
        for (uint32_t j = 0; j < 32; j++) {
            if (!(pmm_bitmap[i] & (1 << j))) {
                uint32_t frame = i * 32 + j;
                bitmap_set(frame);
                pmm_used_frames++;
                return frame * PMM_PAGE_SIZE;  // return physical address
            }
        }
    }
    return 0;  // out of memory
}

void pmm_free(uint32_t addr) {
    pmm_mark_free(addr);
}

uint32_t pmm_free_frames(void) {
    return pmm_total_frames - pmm_used_frames;
}