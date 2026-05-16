// blinky.c 
// compiled separately from the kernel

void _start() {
    unsigned int color = 0x00FF0000;
    unsigned int tick  = 0;

    while (1) {
        struct {
            unsigned int x, y, w, h, color;
        } args = { 800, 10, 200, 50, color };

        asm volatile(
            "int $0x80"
            :
            : "a"(4), "b"(&args)  // SYS_FILL_RECT 
            : "memory"
        );

        for (volatile int i = 0; i < 100000; i++);

        tick++;
        if (tick % 3 == 0)       color = 0x00FF0000;    // red
        else if (tick % 3 == 1)  color = 0x0000FF00;    // green
        else                      color = 0x000000FF;   // blue
    }
}