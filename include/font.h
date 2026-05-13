// panache/include/font.h

#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define PSF2_MAGIC 0x864AB572

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;  
    uint32_t flags;
    uint32_t num_glyphs;
    uint32_t bytes_per_glyph;
    uint32_t height;       
    uint32_t width;        
} __attribute__((packed)) psf2_header_t;

void font_init(void);
void fb_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
void fb_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg);

#endif