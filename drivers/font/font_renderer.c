// panache/drivers/font/font_renderer.c

#include <font.h>
#include <video/framebuffer.h>

extern unsigned char Uni2_Terminus12x6_psf[];
extern unsigned int Uni2_Terminus12x6_psf_len;

static psf2_header_t *font_header;
static uint8_t *font_bitmaps;

void font_init(void) {
    font_header  = (psf2_header_t *)Uni2_Terminus12x6_psf;
    font_bitmaps = Uni2_Terminus12x6_psf + font_header->header_size;
}

void fb_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg) {
    uint32_t bpg    = font_header->bytes_per_glyph;
    uint32_t width  = font_header->width;
    uint32_t height = font_header->height;
    uint8_t *glyph = font_bitmaps + (uint8_t)c * bpg;

    uint32_t bytes_per_row = (width + 7) / 8;

    for (uint32_t row = 0; row < height; row++) {
        for (uint32_t col = 0; col < width; col++) {
            uint8_t byte = glyph[row * bytes_per_row + col / 8];
            uint8_t bit  = byte & (0x80 >> (col % 8));
            fb_put_pixel(x + col, y + row, bit ? fg : bg);
        }
    }
}

void fb_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t fg, uint32_t bg) {
    if (!font_header) {
        debug_serial_str("font_header is NULL!\n");
        return;
    }

    uint32_t cx = x;
    while (*str) {
        fb_draw_char(cx, y, *str, fg, bg);
        cx += font_header->width;
        str++;
    }
}