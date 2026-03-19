# MAKEFILE for panacheOS

CC := i686-elf-gcc
AS := nasm

SRC_DIRS := kernel drivers arch/i386 usr
BUILD_DIR := build
ISO_DIR := iso

CFLAGS := -ffreestanding -O2 -Wall -Wextra -Ikernel -Idrivers -Iarch -Iusr -Iinclude
CFLAGS += -std=gnu99
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib

C_SRCS := $(shell find $(SRC_DIRS) -name "*.c")
ASM_SRCS := $(shell find boot $(SRC_DIRS) -name "*.asm")

OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS)) \
        $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SRCS))

KERNEL := kernel.bin

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(AS) -f elf32 $< -o $@

iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	grub-mkrescue -o panacheOS.iso $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom panacheOS.iso

clean:
	rm -rf $(BUILD_DIR) $(KERNEL) panacheOS.iso
