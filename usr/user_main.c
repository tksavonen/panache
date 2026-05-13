// panache/usr/user_main.c

#include <user_api.h>
#include <syscall.h>
#include <vga.h>
#include <serial.h>
#include <font.h>
#include <video/framebuffer.h>

void user_main() {
    syscall_clear(0x00001A1A2E);
    syscall_draw_string(10, 10, "PanacheOS", 0x00FFFFFF, 0x00001A1A2E);
    syscall_draw_string(10, 30, "Hello from user mode!", 0x00FF4444, 0x00001A1A2E);
    while(1);
}