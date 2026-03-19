// panache/drivers/video/framebuffer.h

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

void fb_init(void);                                             // Initialize the framebuffer
void fb_clear(uint32_t color);                                  // Clear the framebuffer to a color
void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color);      // Draw a pixel in the framebuffer
void fb_flush(void);                                            // Flush framebuffer to VGA / real video memory

extern uint32_t* framebuffer;

#endif