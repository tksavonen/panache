// panache/include/process.h

#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES       16
#define PROCESS_STACK_SIZE  4096

typedef enum {
    PROCESS_DEAD    = 0,
    PROCESS_READY   = 1,
    PROCESS_RUNNING = 2,
    PROCESS_BLOCKED = 3,
} process_state_t;

typedef struct {
    uint32_t pid;
    char     name[32];
    process_state_t state;

    uint32_t esp;                   // kernel stack pointer
    uint32_t eip;                   // instruction pointer
    uint32_t cr3;                   // page directory physical address

    // stacks
    uint32_t kernel_stack_top;      // for TSS esp0 on switch
    uint32_t user_stack_top;

    // ELF entry point
    uint32_t entry;
} process_t;

extern process_t process_table[MAX_PROCESSES];
extern uint32_t  current_pid;

void process_init(void);
process_t *process_create(const char *name, uint32_t entry, uint32_t cr3,
                           uint32_t user_stack_top, uint32_t kernel_stack_top);
void process_exit(uint32_t pid);
process_t *process_get_current(void);

#endif