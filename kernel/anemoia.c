// panache/kernel/anemoia.c
// ----- THE PANACHEOS KERNEL -----

#include <vga.h>
#include <gdt.h>
#include <interrupts.h>
#include <syscall.h>
#include <serial.h>
#include <mb.h>
#include <util.h>
#include <font.h>
#include <keyboard.h>
#include <paging.h>
#include <pmm.h>
#include <video/framebuffer.h>

#include <stdint.h>

#define SYS_PRINT 1

extern uint8_t kernel_stack[65536];
extern uint32_t kernel_stack_top;

extern uint8_t user_stack[4096];
extern uint32_t user_stack_top;

extern void enter_user_mode(void);

static void parse_multiboot2(uint32_t mb_info_addr) {
    struct mb2_info *info = (struct mb2_info *)mb_info_addr;
    uint8_t *ptr = (uint8_t *)info + 8;
    uint8_t *end = (uint8_t *)info + info->total_size;

    while (ptr < end) {
        struct mb2_tag *tag = (struct mb2_tag *)ptr;

        if (tag->type == 0) 
            break; 

        if (tag->type == 6) {
            struct mb2_tag_mmap *mmap = (struct mb2_tag_mmap *)tag;
            uint8_t *entry = (uint8_t *)mmap + 16;  
            uint8_t *end   = (uint8_t *)mmap + mmap->size;

            while (entry < end) {
                struct mb2_mmap_entry *e = (struct mb2_mmap_entry *)entry;
                entry += mmap->entry_size;
            }
        }
        
        if (tag->type == 8) {
            struct mb2_tag_framebuffer *fb = (struct mb2_tag_framebuffer *)tag;
            debug_serial_str("FOUND FB\n");
            fb_set_addr(
                (uint32_t *)(uint32_t)fb->addr,  // 0xFD000000
                fb->pitch,
                fb->width,
                fb->height
            );
        }
        ptr += (tag->size + 7) & ~7;
    }
}

uint32_t syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    switch (eax) {
        case SYS_PUT_SERIAL_CHAR:
            serial_write((char)ebx);
            break;
        case SYS_PUT_PIXEL:
            fb_put_pixel(ebx, ecx, edx);
            break;
        case SYS_FILL_RECT: {
            rect_args_t *args = (rect_args_t *)ebx;
            fb_fill_rect(args->x, args->y, args->w, args->h, args->color);
            break;
        }
        case SYS_CLEAR_SCREEN:
            fb_clear(ebx);
            break;
        case SYS_DRAW_STRING: {
            draw_string_args_t *args = (draw_string_args_t *)ebx;
            fb_draw_string(args->x, args->y, args->str, args->fg, args->bg);
            break;
        }
        case SYS_HAS_KEY:
            return keyboard_haskey();  
        case SYS_GET_KEY:
            return keyboard_getchar(); 
        default:
            debug_serial_str("Unrecognized syscall\n");
            break;
    }
}

void debug_marker(uint32_t id) {
    serial_write('0' + (id & 0xF));
}

extern uint8_t __bss_end;

void kernel_main(uint32_t mb_magic, uint32_t mb_info_addr) {
    debug_serial_init();
    debug_serial_str("serial init\n");

    parse_multiboot2(mb_info_addr);
    debug_serial_str("multiboot2 init\n");

    init_gdt();
    debug_serial_str("gdt init\n");
    init_idt();
    debug_serial_str("idt init\n");

    init_paging();  
    paging_enable();
    pmm_init(0x00100000, 0x07FE0000);  // 1MB - 126MB
    debug_serial_str("paging init\n");
    // mark kernel itself as used so we don't allocate over it
    extern uint8_t __bss_end; 
    uint32_t kernel_end = (uint32_t)&__bss_end;
    for (uint32_t addr = 0x00100000; addr < 0x00500000; addr += PMM_PAGE_SIZE) {
        pmm_mark_used(addr);
    }

    debug_serial_str("testing pm alloc ...\n");
    uint32_t frame1 = pmm_alloc();
    uint32_t frame2 = pmm_alloc();
    uint32_t frame3 = pmm_alloc();
    debug_serial_str("alloc1=");
    debug_serial_hex(frame1);
    debug_serial_str("\nalloc2=");
    debug_serial_hex(frame2);
    debug_serial_str("\nalloc3=");
    debug_serial_hex(frame3);
    debug_serial_str("\n");

    pmm_free(frame2);
    uint32_t frame4 = pmm_alloc();  // should reuse frame2's address
    debug_serial_str("after free+alloc=");
    debug_serial_hex(frame4);
    debug_serial_str("\n");

    debug_serial_str("free frames=");
    debug_serial_hex(pmm_free_frames());
    debug_serial_str("\npm alloc OK\n");

    font_init();
    keyboard_init();

    enter_user_mode();
}

