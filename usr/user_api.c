// panache/usr/user_api.c

#include <syscall.h>

// Print string
void syscall_print(const char* str) {
    asm volatile(
        "int $0x80"
        :
        : "a"(1), "b"(str)
        : "memory"
    );
}