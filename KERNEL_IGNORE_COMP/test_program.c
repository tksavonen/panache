// test_program.c 
// compiled separately from the kernel

void _start() {
    asm volatile(
        "int $0x80"
        :
        : "a"(2), "b"('H')  // SYS_PUT_SERIAL_CHAR = 2
        : "memory"
    );
    asm volatile(
        "int $0x80"
        :
        : "a"(2), "b"('i')
        : "memory"
    );
    asm volatile(
        "int $0x80"
        :
        : "a"(9)  // SYS_EXIT
        : "memory"
    );
    while(1);
}