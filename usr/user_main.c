// panache/usr/user_main.c

#include <user_api.h>
#include <syscall.h>
#include <vga.h>
#include <serial.h>
#include <font.h>
#include <colors.h>
#include <framebuffer.h>

#define SHELL_MAX_INPUT     128
#define SHELL_X             10
#define SHELL_Y             80
#define SHELL_FG            COLOR_CYAN 
#define SHELL_BG            COLOR_BG_DARK
#define SHELL_PROMPT        "panache> "
#define CHAR_WIDTH          14
#define CHAR_HEIGHT         28

static char input[SHELL_MAX_INPUT];
static uint32_t input_len = 0;
static uint32_t cursor_y = SHELL_Y;

void shell_redraw_line(void) {
    // clear the line
    syscall_fill_rect(SHELL_X, cursor_y, 1024 - SHELL_X, CHAR_HEIGHT, SHELL_BG);
    // draw prompt
    syscall_draw_string(SHELL_X, cursor_y, SHELL_PROMPT, COLOR_LIGHT_RED, SHELL_BG);
    // draw input
    uint32_t prompt_width = 9 * CHAR_WIDTH; 
    syscall_draw_string(SHELL_X + prompt_width, cursor_y, input, SHELL_FG, SHELL_BG);
}

void shell_newline(void) {
    cursor_y += CHAR_HEIGHT;
    if (cursor_y > 768 - CHAR_HEIGHT) {
        syscall_clear(SHELL_BG);
        cursor_y = SHELL_Y;
    }
}

void shell_print(const char *str, uint32_t color) {
    syscall_draw_string(SHELL_X, cursor_y, str, color, SHELL_BG);
    shell_newline();
}

void shell_execute(const char *cmd) {
    if (cmd[0] == 0) return; 

    if (cmd[0]=='h' && cmd[1]=='e' && cmd[2]=='l' && cmd[3]=='p' && cmd[4]==0) {
        shell_print("Commands: help, clear, about", 0x00AAAAAA);

    } else if (cmd[0]=='c' && cmd[1]=='l' && cmd[2]=='e' && cmd[3]=='a' && cmd[4]=='r' && cmd[5]==0) {
        syscall_clear(SHELL_BG);
        cursor_y = SHELL_Y;

    } else if (cmd[0]=='a' && cmd[1]=='b' && cmd[2]=='o' && cmd[3]=='u' && cmd[4]=='t' && cmd[5]==0) {
        shell_print("PanacheOS!", COLOR_LIGHT_RED);
        shell_print("Kernel: x86 32-bit, ring 3 userspace", 0x00AAAAAA);

    } else {
        shell_print("Unknown command. Type 'help'.", 0x00FF8800);
    }
}

void user_main() {
    syscall_clear(SHELL_BG);
    syscall_draw_string(10, 10, "PanacheOS v0.1", 0x00FFFFFF, SHELL_BG);
    syscall_draw_string(10, 36, "Type 'help' for commands", 0x00AAAAAA, SHELL_BG);

    shell_redraw_line();

    while (1) {
        if (!syscall_haskey()) continue;
        
        char c = syscall_getkey();
        if (c == '\n') {
            input[input_len] = 0;
            shell_newline();      
            shell_execute(input); 
            input_len = 0;
            input[0]  = 0;
            shell_newline();      
            shell_redraw_line();  // draw fresh prompt
        } else if (c == '\b') {
            // backspace
            if (input_len > 0) {
                input_len--;
                input[input_len] = 0;
                shell_redraw_line();
            }
        } else if (input_len < SHELL_MAX_INPUT - 1) {
            // normal character
            input[input_len++] = c;
            input[input_len]   = 0;
            shell_redraw_line();
        }
    }
}