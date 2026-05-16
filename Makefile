CC 				:= i686-elf-gcc
AS 				:= nasm
LD 				:= i686-elf-gcc
OBJCOPY 		:= i686-elf-objcopy

USER_CC 		:= gcc
USER_LD 		:= ld

BUILD_DIR 		:= build
ISO_DIR 		:= iso

KERNEL 			:= kernel.bin
ISO_KERNEL 		:= $(ISO_DIR)/boot/kernel.bin

QEMU 			:= qemu-system-i386
QEMUFLAGS 		:= -cdrom panacheOS.iso -serial file:serial.log -vga std

CFLAGS 			:= -ffreestanding -O2 -Wall -Wextra -std=gnu99 
CFLAGS 			+= -Iinclude

LDFLAGS 		:= -T linker.ld -ffreestanding -nostdlib

USER_CFLAGS 	:= -m32 -ffreestanding -nostdlib -nostdinc
USER_CFLAGS 	+= -fno-pic
USER_CFLAGS 	+= -fno-asynchronous-unwind-tables
USER_CFLAGS 	+= -fno-ident
USER_CFLAGS 	+= -Iinclude

USER_LD_SCRIPT := KERNEL_IGNORE_COMP/build-elf.ld

KERNEL_C_SRCS := $(shell find kernel arch drivers memory proc fonts -name "*.c")
KERNEL_C_SRCS += usr/elf_loader.c
KERNEL_C_SRCS += usr/user_main.c
KERNEL_C_SRCS += usr/user_api.c
KERNEL_C_SRCS += usr/serial.c
KERNEL_C_SRCS += $(shell find usr/daemons -name "*.c")
KERNEL_ASM_SRCS := $(shell find boot kernel arch proc -name "*.asm")

KERNEL_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRCS)) $(patsubst %.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SRCS))

USER_API_SRCS := usr/user_api.c usr/serial.c
USER_API_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(USER_API_SRCS))

USER_PROGRAMS := blinky:0x00800000

prog_name = $(firstword $(subst :, ,$1))
prog_addr = $(word 2,$(subst :, ,$1))

EMBEDDED_OBJS := $(foreach prog,$(USER_PROGRAMS),\
	$(BUILD_DIR)/usr/executables/$(call prog_name,$(prog))_elf.o)

.PHONY: all iso run clean

all: iso

$(KERNEL): $(KERNEL_OBJS) $(EMBEDDED_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "LD $@"

iso: $(KERNEL)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_KERNEL)
	grub-mkrescue -o panacheOS.iso $(ISO_DIR)
	@echo "ISO panacheOS.iso"

run: iso
	$(QEMU) $(QEMUFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(KERNEL) panacheOS.iso serial.log
	@echo "CLEAN done"

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "CC $<"

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf32 $< -o $@
	@echo "AS $<"

define BUILD_USER_PROGRAM

USER_$(1)_SRC     := usr/executables/$(1).c
USER_$(1)_OBJ     := $(BUILD_DIR)/usr/executables/$(1).o
USER_$(1)_ELF     := $(BUILD_DIR)/usr/executables/$(1).elf
USER_$(1)_BIN_C   := $(BUILD_DIR)/usr/executables/$(1)_elf.c
USER_$(1)_BIN_OBJ := $(BUILD_DIR)/usr/executables/$(1)_elf.o

$$(USER_$(1)_OBJ): $$(USER_$(1)_SRC)
	@mkdir -p $$(dir $$@)
	$$(USER_CC) $$(USER_CFLAGS) -c $$< -o $$@

$$(USER_$(1)_ELF): $$(USER_$(1)_OBJ) $$(USER_API_OBJS)
	@mkdir -p $$(dir $$@)
	$$(USER_LD) \
		-m elf_i386 \
		-T $$(USER_LD_SCRIPT) \
		-Ttext=$(2) \
		-o $$@ $$^

$$(USER_$(1)_BIN_C): $$(USER_$(1)_ELF)
	xxd -i -n "$(1)_elf" $$< > $$@

$$(USER_$(1)_BIN_OBJ): $$(USER_$(1)_BIN_C)
	@mkdir -p $$(dir $$@)
	$$(CC) $$(CFLAGS) -c $$< -o $$@

endef

$(foreach prog,$(USER_PROGRAMS),$(eval $(call BUILD_USER_PROGRAM,$(call prog_name,$(prog)),$(call prog_addr,$(prog)))))