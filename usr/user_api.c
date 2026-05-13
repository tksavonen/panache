// panache/usr/user_api.c

#include <syscall.h>

void syscall_putchar(char c) {
    asm volatile(
        "int $0x80"
        :
        : "a"(SYS_PUT_SERIAL_CHAR), "b"(c)
        : "memory"
    );
}

void syscall_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    asm volatile(
        "int $0x80"
        :
        : "a"(SYS_PUT_PIXEL), "b"(x), "c"(y), "d"(color)
        : "memory"
    );
}

void syscall_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    rect_args_t args = { x, y, w, h, color };
    asm volatile(
        "int $0x80"
        :
        : "a"(SYS_FILL_RECT), "b"(&args)
        : "memory"
    );
}

void syscall_clear(uint32_t color) {
    asm volatile(
        "int $0x80"
        :
        : "a"(SYS_CLEAR_SCREEN), "b"(color)
        : "memory"
    );
}

void syscall_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg) {
    draw_string_args_t args = { x, y, str, fg, bg };
    asm volatile(
        "int $0x80"
        :
        : "a"(SYS_DRAW_STRING), "b"(&args)
        : "memory"
    );
}