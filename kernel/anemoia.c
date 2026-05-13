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

    while (1) {
        struct mb2_tag *tag = (struct mb2_tag *)ptr;

        if (tag->type == 0) 
            break; 
        
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

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
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
        default:
            debug_serial_str("Unrecognized syscall\n");
            break;
    }
}

void debug_marker(uint32_t id) {
    serial_write('0' + (id & 0xF));
}

void kernel_main(uint32_t mb_magic, uint32_t mb_info_addr) {
    debug_serial_init();

    init_gdt();
    init_idt();

    parse_multiboot2(mb_info_addr);

    font_init();

    enter_user_mode();
    __builtin_unreachable();
}
