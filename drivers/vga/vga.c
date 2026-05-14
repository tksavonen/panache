// panache/drivers/vga/vga.c

#include <vga.h>
#include <util.h>

#include <stdarg.h>

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

	for (uint16_t y = 0; y < VGA_SCREEN_HEIGHT; y++) {
		for (uint16_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
			vga[y * VGA_SCREEN_WIDTH + x] = ' ' | current_color;
		}
	}
}

void k_new_line() {
	if (line < VGA_SCREEN_HEIGHT - 1) {
		line++; column = 0;
	}
	else {
		k_scroll_up();
		column = 0;
	}
}

void k_scroll_up() {
	for (uint16_t y = 1; y < VGA_SCREEN_HEIGHT; y++) {
		for (uint16_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
			vga[(y - 1) * VGA_SCREEN_WIDTH + x] = vga[y * VGA_SCREEN_WIDTH + x];
		}
	}

	for (uint16_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
		vga[(VGA_SCREEN_HEIGHT - 1) * VGA_SCREEN_WIDTH + x] = ' ' | current_color;
	}
}

void k_print(const char* s, ...) {
    __gnuc_va_list args;
    va_start(args, s);

    while (*s) {
        if (*s == '%') {
            s++;

            if (*s == 'd') {
                int val = va_arg(args, int);
                char buf[32];
                k_itoa(val, buf);
                k_print(buf);   
            }
            else if (*s == 'u') {
                unsigned val = va_arg(args, unsigned);
                char buf[32];
                k_itoa((int)val, buf);
                k_print(buf);
            }
            else if (*s == 'c') {
                char c = (char)va_arg(args, int);
                char str[2] = {c, 0};
                k_print(str);
            }
            else if (*s == 's') {
                char* str = va_arg(args, char*);
                k_print(str);
            }
            else {
                vga[line * VGA_SCREEN_WIDTH + (column++)] = *s | current_color;
            }
        }
        else {
            switch (*s) {
                case '\n': k_new_line(); break;
                case '\r': column = 0; break;
                case '\t': {
                    uint16_t tablen = 4 - (column % 4);
                    while (tablen--) {
                        vga[line * VGA_SCREEN_WIDTH + (column++)] = ' ' | current_color;
                    }
                } break;
                default:
                    if (column == VGA_SCREEN_WIDTH) k_new_line();
                    vga[line * VGA_SCREEN_WIDTH + (column++)] = *s | current_color;
                    break;
            }
        }

        s++;
    }

    va_end(args);
}

void k_set_color(uint8_t fg, uint8_t bg) {
	current_color = (fg << 8) | (bg << 12);
}

// convert integer to hex string
void itox(uint32_t val, char* buf) {
    const char* hex = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buf[2 + i] = hex[(val >> (28 - i * 4)) & 0xF];
    }
    buf[10] = 0;
}

// prints a 32-bit value in hex
void k_print_hex(uint32_t val) {
    char buf[11];   // "0x" + 8 hex digits + null terminator
    itox(val, buf);
    k_print(buf);   // use your existing k_print
}
