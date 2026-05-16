// panache/kernel/anemoia.c
// ----- THE PANACHEOS KERNEL -----

#include <vga.h>
#include <framebuffer.h>
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
#include <elf_loader.h>
#include <process.h>
#include <scheduler.h>
#include <cmos.h>

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
        case SYS_GET_TIME: {
            cmos_time_t t;
            cmos_get_time(&t);
            cmos_time_t *user_t = (cmos_time_t *)ebx;
            user_t->seconds = t.seconds;
            user_t->minutes = t.minutes;
            user_t->hours   = t.hours;
            user_t->day     = t.day;
            user_t->month   = t.month;
            user_t->year    = t.year;
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

    // ------ MEMORY CHECKS ------ 
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

    debug_serial_str("\ntesting pm alloc ...\n\n");
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
    uint32_t frame4 = pmm_alloc();  
    debug_serial_str("after free+alloc=");
    debug_serial_hex(frame4);
    debug_serial_str("\n");

    debug_serial_str("free frames=");
    debug_serial_hex(pmm_free_frames());
    debug_serial_str("\npm alloc OK\n\n");

    // allocate 1MB of heap (256 pages)
    uint32_t heap_start_addr = 0x00600000;
    uint32_t heap_size = 256 * 4096;  

    for (uint32_t addr = heap_start_addr; 
        addr < heap_start_addr + heap_size; 
        addr += 4096) {
        pmm_mark_used(addr);
    }

    kmalloc_init(heap_start_addr, heap_size);

    debug_serial_str("testing kmalloc...\n");
    uint32_t *a = kmalloc(16);
    uint32_t *b = kmalloc(64);
    uint32_t *c = kmalloc(256);

    debug_serial_str("a="); debug_serial_hex((uint32_t)a);
    debug_serial_str("\nb="); debug_serial_hex((uint32_t)b);
    debug_serial_str("\nc="); debug_serial_hex((uint32_t)c);
    *a = 0x12345678;
    *b = 0xDEADBEEF;
    debug_serial_str("\n*a="); debug_serial_hex(*a);
    debug_serial_str("\n*b="); debug_serial_hex(*b);

    kfree(b);
    uint32_t *d = kmalloc(32); 
    debug_serial_str("\nd="); debug_serial_hex((uint32_t)d);

    kmalloc_dump();
    debug_serial_str("kmalloc OK\n\n");

    font_init();
    debug_serial_str("font init\n");
    keyboard_init();
    debug_serial_str("keyboard init\n");

    // ------ EXE CHECKS ------ 
    process_init();
    scheduler_init();

    debug_serial_str("\ntesting proc 1 ...\n");

    uint32_t shell_kstack = pmm_alloc();
    paging_map(shell_kstack, shell_kstack, PAGE_PRESENT | PAGE_RW);
    uint32_t shell_ustack_phys = pmm_alloc();
    uint32_t shell_ustack_virt = 0x00500000;
    paging_map(shell_ustack_virt - 0x1000, shell_ustack_phys, 
            PAGE_PRESENT | PAGE_RW| PAGE_USER);

    uint32_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    extern void user_main(void);
    process_t *shell = process_create(
        "shell",
        (uint32_t)user_main,    // entry point
        cr3,                    // shares kernel page dir
        shell_ustack_virt,      // user stack top
        shell_kstack + 0x1000   // kernel stack top
    );

    current_pid = shell->pid;
    shell->state = PROCESS_RUNNING;
    debug_serial_str("test proc 1 OK\n\n");
    debug_serial_str("testing proc 2 ...\n");

    extern unsigned char blinky_elf[];
    uint32_t blinky_entry = elf_load(blinky_elf);
    debug_serial_str("blinky entry=");
    debug_serial_hex(blinky_entry);
    debug_serial_str("\n");

    // verify the entry point is readable
    uint8_t *ep = (uint8_t *)blinky_entry;
    debug_serial_str("first byte at entry=");
    debug_serial_hex(*ep);
    debug_serial_str("\n");

    uint32_t blinky_kstack_phys = pmm_alloc();
    paging_map(blinky_kstack_phys, blinky_kstack_phys, PAGE_PRESENT | PAGE_RW);
    uint32_t blinky_ustack_phys = pmm_alloc();
    uint32_t blinky_ustack_virt = 0x00900000;  
    paging_map(blinky_ustack_virt - 0x1000, blinky_ustack_phys, PAGE_PRESENT | PAGE_RW | PAGE_USER);

    process_t *blinky = process_create("blinky", blinky_entry, cr3, blinky_ustack_virt,
        blinky_kstack_phys + 0x1000);

    debug_serial_str("proc 2 OK\n\n");

    debug_serial_str("jumping to user mode ...\n");
    debug_serial_str("starting scheduler ...\n");
    asm volatile(
        "mov $0x23, %%ax  \n"
        "mov %%ax, %%ds   \n"
        "mov %%ax, %%es   \n"
        "mov %%ax, %%fs   \n"
        "mov %%ax, %%gs   \n"
        "push $0x23       \n"   
        "push %1          \n"  
        "pushf            \n"
        "pop %%eax        \n"
        "or $0x200, %%eax \n"   
        "push %%eax       \n"   
        "push $0x1b       \n"   
        "push %0          \n"  
        "iret             \n"
        :
        : "r"(shell->entry), "r"(shell->user_stack_top)
        : "eax"
    );
     __builtin_unreachable();
}
