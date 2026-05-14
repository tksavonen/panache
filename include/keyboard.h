// panache/include/keyboard.h

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <interrupts.h>

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
uint8_t keyboard_haskey(void);

static const char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

#endif