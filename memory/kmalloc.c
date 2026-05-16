// panache/memory/kmalloc.c

#include <kmalloc.h>
#include <serial.h>

#define HEAP_MAGIC 0xDEADC0DE

typedef struct block_header {
    uint32_t magic;
    uint32_t size;          // (not including header)
    uint8_t  used;
    struct block_header *next;
    struct block_header *prev;
} block_header_t;

static block_header_t *heap_start = 0;
static uint32_t        heap_end   = 0;

void kmalloc_init(uint32_t start, uint32_t size) {
    heap_start = (block_header_t *)start;
    heap_end   = start + size;

    heap_start->magic = HEAP_MAGIC;
    heap_start->size  = size - sizeof(block_header_t);
    heap_start->used  = 0;
    heap_start->next  = 0;
    heap_start->prev  = 0;

    debug_serial_str("kmalloc: heap=");
    debug_serial_hex(start);
    debug_serial_str(" size=");
    debug_serial_hex(size);
    debug_serial_str("\n");
}

void *kmalloc(uint32_t size) {
    size = (size + 3) & ~3;

    block_header_t *block = heap_start;

    while (block) {
        if (block->magic != HEAP_MAGIC) {
            debug_serial_str("kmalloc: HEAP CORRUPTION at ");
            debug_serial_hex((uint32_t)block);
            debug_serial_str("\n");
            return 0;
        }

        if (!block->used && block->size >= size) {
            // split block if large enough to be worth it
            uint32_t min_split = sizeof(block_header_t) + 16;
            if (block->size >= size + min_split) {
                block_header_t *new_block = (block_header_t *)
                    ((uint8_t *)block + sizeof(block_header_t) + size);

                new_block->magic = HEAP_MAGIC;
                new_block->size  = block->size - size - sizeof(block_header_t);
                new_block->used  = 0;
                new_block->next  = block->next;
                new_block->prev  = block;

                if (block->next)
                    block->next->prev = new_block;

                block->next = new_block;
                block->size = size;
            }

            block->used = 1;
            return (void *)((uint8_t *)block + sizeof(block_header_t));
        }

        block = block->next;
    }

    debug_serial_str("kmalloc: OUT OF MEMORY\n");
    return 0;
}

void kfree(void *ptr) {
    if (!ptr) return;

    block_header_t *block = (block_header_t *)
        ((uint8_t *)ptr - sizeof(block_header_t));

    if (block->magic != HEAP_MAGIC) {
        debug_serial_str("kfree: INVALID POINTER\n");
        return;
    }

    block->used = 0;

    // merge with next block if free
    if (block->next && !block->next->used) {
        block->size += sizeof(block_header_t) + block->next->size;
        block->next  = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    // merge with prev block if free
    if (block->prev && !block->prev->used) {
        block->prev->size += sizeof(block_header_t) + block->size;
        block->prev->next  = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

void kmalloc_dump(void) {
    debug_serial_str("=== heap dump ===\n");
    block_header_t *block = heap_start;
    uint32_t i = 0;
    while (block) {
        debug_serial_str("block ");
        debug_serial_hex(i++);
        debug_serial_str(": addr=");
        debug_serial_hex((uint32_t)block);
        debug_serial_str(" size=");
        debug_serial_hex(block->size);
        debug_serial_str(" used=");
        debug_serial_hex(block->used);
        debug_serial_str("\n");
        block = block->next;
    }
    debug_serial_str("=================\n");
}