// panache/kernel/anemoia.c
// ----- THE PANACHEOS KERNEL -----

#include <vga.h>
#include <gdt.h>
#include <interrupts.h>
#include <syscall.h>
#include <mb.h>
#include <stdint.h>

#define SYS_PRINT 1

extern uint8_t kernel_stack[4096];
extern uint32_t kernel_stack_top;

extern uint8_t user_stack[4096];
extern uint32_t user_stack_top;

extern void enter_user_mode(void);

struct tss_entry_struct tes;

static uint8_t  *fb_addr;
static uint32_t fb_pitch;
static uint32_t fb_width;
static uint32_t fb_height;
static uint8_t  fb_bpp;

static void parse_multiboot2(uint32_t mb_info_addr) {
    struct mb2_info *info = (struct mb2_info *)mb_info_addr;
    uint8_t *ptr = (uint8_t *)info + 8;

    while (1) {
        struct mb2_tag *tag = (struct mb2_tag *)ptr;

        if (tag->type == 0) 
            break; 
        
        if (tag->type == 8) { 
            struct mb2_tag_framebuffer *fb =
                (struct mb2_tag_framebuffer *)tag;

            fb_addr   = (uint8_t *)(uintptr_t)fb->addr;
            fb_pitch  = fb->pitch;
            fb_width  = fb->width;
            fb_height = fb->height;
            fb_bpp    = fb->bpp;
        }

        ptr += (tag->size + 7) & ~7;
    }
}

void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    switch (eax) {
        case SYS_PRINT: {
            const char* str = (const char*)ebx;
            k_print(str);
            break;
        }
        default:
            k_print("Unknown syscall\n");
            break;
    }
}

void kernel_main(uint32_t mb_magic, uint32_t mb_info_addr) {
    (void)mb_magic;
    parse_multiboot2(mb_info_addr);
    
	k_reset();
	k_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
	k_print("You are in the ANEMOIA kernel.");
    k_new_line();
    k_set_color(COLOR_WHITE, COLOR_BLACK);

	init_gdt();
	init_idt();

    k_print("Leaving kernel space...");
    k_new_line();

    asm volatile("cli");
    enter_user_mode();
    __builtin_unreachable();
}
