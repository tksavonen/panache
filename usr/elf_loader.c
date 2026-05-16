// panache/usr/elf_loader.h

#include <elf_loader.h>
#include <pmm.h>
#include <paging.h>
#include <kmalloc.h>
#include <serial.h>

#include <stdint.h>

static void elf_memcpy(uint8_t *dst, uint8_t *src, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) dst[i] = src[i];
}

static void elf_memset(uint8_t *dst, uint8_t val, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) dst[i] = val;
}

uint32_t elf_load(uint8_t *elf_data) {
    elf32_header_t *header = (elf32_header_t *)elf_data;

    // verify magic
    if (*(uint32_t *)header->ident != ELF_MAGIC) {
        debug_serial_str("elf: bad magic\n");
        return 0;
    }

    // verify 32-bit exe
    if (header->type != ET_EXEC) {
        debug_serial_str("elf: not executable\n");
        return 0;
    }

    debug_serial_str("testing ELF ...\n\n");
    debug_serial_str("elf: entry=");
    debug_serial_hex(header->entry);
    debug_serial_str("elf: phnum=");
    debug_serial_hex(header->phnum);

    for (uint16_t i = 0; i < header->phnum; i++) {
        elf32_phdr_t *phdr = (elf32_phdr_t *)
            (elf_data + header->phoff + i * header->phentsize);

        if (phdr->type != PT_LOAD) continue; 

        debug_serial_str("elf: segment vaddr=");
        debug_serial_hex(phdr->vaddr);
        debug_serial_str(" filesz=");
        debug_serial_hex(phdr->filesz);
        debug_serial_str(" memsz=");
        debug_serial_hex(phdr->memsz);

        uint32_t pages_needed = (phdr->memsz + 0xFFF) / 0x1000;
        for (uint32_t p = 0; p < pages_needed; p++) {
            uint32_t phys = pmm_alloc();
            uint32_t virt = (phdr->vaddr & ~0xFFF) + p * 0x1000;

            if (!phys) {
                debug_serial_str("elf: out of memory\n");
                return 0;
            }

            // map virtual → physical in current page directory
            paging_map(virt, phys, PAGE_PRESENT | PAGE_RW | PAGE_USER);
        }

        elf_memcpy(
            (uint8_t *)phdr->vaddr,
            elf_data + phdr->offset,
            phdr->filesz
        );

        if (phdr->memsz > phdr->filesz) {
            elf_memset(
                (uint8_t *)phdr->vaddr + phdr->filesz,
                0,
                phdr->memsz - phdr->filesz
            );
        }
    }

    debug_serial_str("elf: loaded OK\n\n");
    return header->entry;
}