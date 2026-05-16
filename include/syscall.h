// panache/include/syscall.h

#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// Syscall numbers

#define SYS_PRINT               1
#define SYS_PUT_SERIAL_CHAR     2
#define SYS_PUT_PIXEL           3
#define SYS_FILL_RECT           4
#define SYS_CLEAR_SCREEN        5
#define SYS_DRAW_STRING         6
#define SYS_HAS_KEY             7
#define SYS_GET_KEY             8
#define SYS_GET_TIME            9

// User-mode API

typedef struct {
    uint32_t x, y, w, h, color;
} rect_args_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    const char *str;
    uint32_t fg;
    uint32_t bg;
} draw_string_args_t;

typedef struct {
    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} time_t;

void syscall_putchar(char c);
void syscall_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void syscall_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void syscall_clear(uint32_t color);
void syscall_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);
void syscall_get_time(time_t *t);

#endif