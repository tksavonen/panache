// panache/drivers/video/framebuffer.c

#include "framebuffer.h"

static uint32_t fb[SCREEN_WIDTH * SCREEN_HEIGHT];
uint32_t* framebuffer = fb;

void fb_init(void) {
    fb_clear(0x00000000); // black
}

void fb_clear(uint32_t color) {
    for (uint32_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        fb[i] = color;
}

void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT)
        fb[y * SCREEN_WIDTH + x] = color;
}

