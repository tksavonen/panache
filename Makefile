CC := i686-elf-gcc
AS := nasm

SRC_DIRS := kernel drivers arch/i386 usr memory
BUILD_DIR := build
ISO_DIR := iso

CFLAGS := -ffreestanding -O2 -Wall -Wextra -Ikernel -Idrivers -Iarch -Iusr -Imemory -Iinclude
CFLAGS += -std=gnu99
LDFLAGS := -T linker.ld -ffreestanding -O2 -nostdlib

C_SRCS := $(shell find $(SRC_DIRS) -name "*.c")
ASM_SRCS := $(shell find boot $(SRC_DIRS) -name "*.asm")

OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS)) \
        $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ASM_SRCS))

KERNEL := kernel.bin

UNAME_S := $(shell uname)

QEMU := qemu-system-i386
RUN_CMD := $(QEMU) -kernel $(KERNEL) -serial stdio

ifeq ($(UNAME_S),Linux)
    USE_GRUB := yes
    RUN_CMD := $(QEMU) -cdrom panacheOS.iso -serial stdio -vga std
endif

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(AS) -f elf32 $< -o $@

# ISO target — only works on Linux
ifeq ($(USE_GRUB),yes)
iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot
	cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	grub-mkrescue -o panacheOS.iso $(ISO_DIR)
else
iso:
	@echo "Skipping ISO creation: GRUB not available on this OS."
endif

run: $(KERNEL)
	$(RUN_CMD)

clean:
	rm -rf $(BUILD_DIR) $(KERNEL) panacheOS.iso
