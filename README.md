# panacheOS

A 32-bit x86 hobby OS written in C and x86 Assembly.

*A work in progress!!!*

<img width="1008" height="760" alt="image" src="https://github.com/user-attachments/assets/c7de3b82-8014-429a-9060-3c4ca8b05950" />

## What is it?
panacheOS is a small operating system built from scratch for the x86 architecture.

Right now, it features:
* Multiboot2 boot via GRUB
* Full x86 descriptor table setup
* HW interrupt handling
* Ring 3 userspace and syscalls
* Physical memory manager, paging and a heap allocator
* ELF loader for external programs
* Round-robin scheduler and multitasking
* VESA/VBE framebuffer (1024x768)
* PSF2 font rendering
* Interactive shell!

## Build and run

### Requirements
* ```i686-elf-gcc``` cross-compiler toolchain
* ```nasm``` assembler
* ```grub-mkrescue``` and ```xorriso```
* ```qemu-system-i386``` for running

On Ubuntu/Debian
```bash
sudo apt install nasm xorriso qemu-system-x86 grub-pc-bin grub-common
```
*The cross compiler must be built manually or downloaded. See [OSDev Wiki](https://wiki.osdev.org/GCC_Cross-Compiler) for instructions.*

```bash
# build iso and run in QEMU
make run

# build iso only
make iso

# clean build artifacts
make clean
```

Serial output is logged to ```serial.log``` during execution for debug info.

## Current project structure
* panache/
* arch/i386/
* boot/
* build/
* drivers/
  * vga/
  * video/
* include/
* fonts/
* iso/
* kernel/
* memory/
* proc/
* usr/
  * daemons/
  * executables/
* Makefile
* linker.ld
* README.md
* qemu.log
