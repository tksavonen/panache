// panache/usr/user_main.c

#include <user_api.h>
#include <vga.h>

void user_main() {
    syscall_print("Hello from framebuffer Ring 3!");
    for(;;);
}