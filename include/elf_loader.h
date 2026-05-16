// panache/include/elf_loader.h

#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>

#define ELF_MAGIC   0x464C457F  // ELF magic
#define ET_EXEC     2           // ELF type
#define PT_LOAD     1           // Program header type

// ELF32 header
typedef struct {
    uint8_t  ident[16];   // magic, class, data, version, OS/ABI
    uint16_t type;        // ET_EXEC etc
    uint16_t machine;     // 3 = x86
    uint32_t version;
    uint32_t entry;       // entry point virtual address
    uint32_t phoff;       // program header table offset
    uint32_t shoff;       // section header table offset
    uint32_t flags;
    uint16_t ehsize;      // ELF header size
    uint16_t phentsize;   // program header entry size
    uint16_t phnum;       // number of program headers
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed)) elf32_header_t;

// ELF32 program header
typedef struct {
    uint32_t type;    // PT_LOAD etc
    uint32_t offset;  // offset in file
    uint32_t vaddr;   // virtual address to load at
    uint32_t paddr;   // physical address (usually same as vaddr)
    uint32_t filesz;  // size in file
    uint32_t memsz;   // size in memory (>= filesz, rest zeroed)
    uint32_t flags;   // R/W/X permissions
    uint32_t align;   // alignment
} __attribute__((packed)) elf32_phdr_t;

uint32_t elf_load(uint8_t *elf_data); 

#endif