// panache/drivers/video/framebuffer.c

#include "framebuffer.h"

uint32_t *fb_addr   = 0;
uint32_t fb_pitch   = 0;
uint32_t fb_width   = 0;
uint32_t fb_height  = 0;

void fb_set_addr(uint32_t *addr, uint32_t pitch, uint32_t width, uint32_t height) {
    fb_addr   = addr;
    fb_pitch  = pitch;
    fb_width  = width;
    fb_height = height;
}

void fb_init(void) {
    fb_clear(0x0000FFFF); 
}

void fb_clear(uint32_t color) {
    for (uint32_t y = 0; y < fb_height; y++) {
        uint32_t *row = (uint32_t *)((uint8_t *)fb_addr + y * fb_pitch);
        for (uint32_t x = 0; x < fb_width; x++)
            row[x] = color;
    }
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height) return;
    uint32_t *pixel = (uint32_t *)((uint8_t *)fb_addr + y * fb_pitch + x * 4);
    *pixel = color;
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t dy = 0; dy < h; dy++)
        for (uint32_t dx = 0; dx < w; dx++)
            fb_put_pixel(x + dx, y + dy, color);
}

