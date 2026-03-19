# panacheOS

A 32-bit hobby OS written in Assembly and C.

*A work in progress!!!*

<img width="717" height="396" alt="panache-kernel" src="https://github.com/user-attachments/assets/b12b257d-f1b2-45df-bca2-9c68b01644b6" />

## What is it?
panacheOS is a small operating system built *mostly* from scratch for the x86 architecture.

Right now, it only contains a minimal kernel (known as the ANEMOIA kernel), working system calls and a framebuffer-backed text console.

## Build and run
panacheOS has only been tested in a VM for the time being. It uses i686-elf-gcc and boots via GRUB.

```bash
make
qemu-system-i386 -kernel panacheOS.bin
```
*Adjust builds and targets depending on your setup.*

## Current project structure
* panache
* arch
  * i386
    * enter_user_mode.asm
    * gdt_flush.asm
    * idt_flush.asm
    * k_isr128.asm
    * k_isr177.asm
    * gdt.c
    * interrupts.c
    * stack.c
* boot
  * boot.asm
* build
* drivers
  * vga
    * vga.c
  * video
    * framebuffer.c
    * framebuffer.h
* include
  * gdt.h
  * interrupts.h
  * syscall.h
  * user_api.h
  * util.h
  * vga.h
* iso
* kernel
  * anemoia.c
  * anemoia.h
  * util.c
  * syscall_entry.asm
* usr
  * user_api.c
  * user_main.c
* Makefile
* README.md
* qemu.log

## License

Distributed under the MIT License. See ```LICENSE``` for more.
