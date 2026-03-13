// PanacheOS/drivers/vga/vga.c

#include "vga.h"

static uint16_t column = 0;
static uint16_t line = 0;
static uint16_t* const vga = (uint16_t* const) 0xB8000;

// Default color scheme -- light grey on black
static inline uint16_t vga_entry_color(uint8_t fg, uint8_t bg)
{
    return (fg << 8) | (bg << 12);
}

static uint16_t current_color;
static uint16_t def_color = (COLOR_GREY << 8) | (COLOR_BLACK << 12);

// Initialize VGA buffer -- Reset all columns empty and to their default colors.
void k_reset() {
	line = 0; column = 0;
	current_color = def_color;

	for (uint16_t y = 0; y < SCREEN_HEIGHT; y++) {
		for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
			vga[y * SCREEN_WIDTH + x] = ' ' | current_color;
		}
	}
}

void k_new_line() {
	if (line < SCREEN_HEIGHT - 1) {
		line++; column = 0;
	}
	else {
		k_scroll_up();
		column = 0;
	}
}

void k_scroll_up() {
	for (uint16_t y = 1; y < SCREEN_HEIGHT; y++) {
		for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
			vga[(y - 1) * SCREEN_WIDTH + x] = vga[y * SCREEN_WIDTH + x];
		}
	}

	for (uint16_t x = 0; x < SCREEN_WIDTH; x++) {
		vga[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = ' ' | current_color;
	}
}

void k_print(const char* s) {
	while (*s) {
		switch (*s) {
			case '\n':
				k_new_line();
				break;
			case '\r':
				column = 0;
				break;
			case '\t':
				if (column == SCREEN_WIDTH) {
					k_new_line();
				}
					uint16_t tablen = 4 - (column % 4);
					while (tablen != 0) {
						vga[line * SCREEN_WIDTH + (column++)] = ' ' | current_color;
						tablen--;
					}
					break;
			default:
				if (column == SCREEN_WIDTH) {
					k_new_line();
				}
				vga[line * SCREEN_WIDTH + (column++)] = *s | current_color;
				break;
		}
		s++;
	}
}

void k_set_color(uint8_t fg, uint8_t bg) {
	current_color = (fg << 8) | (bg << 12);
}
