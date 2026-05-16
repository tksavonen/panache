// panache/proc/process.c

#include <process.h>
#include <kmalloc.h>
#include <pmm.h>
#include <serial.h>

#include <stdint.h>

process_t process_table[MAX_PROCESSES];
uint32_t  current_pid = 0;
static uint32_t next_pid = 1;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROCESS_DEAD;
        process_table[i].pid   = 0;
    }
    debug_serial_str("proc table init\n");
}

// fake initial kernel stack for a new process
static void process_entry_trampoline(void) {
    process_t *p = process_get_current();

    if (!p) {
        debug_serial_str("trampoline: NULL process!\n");
        asm volatile("hlt");
    }

    asm volatile(
        "mov $0x23, %%ax  \n"
        "mov %%ax, %%ds   \n"
        "mov %%ax, %%es   \n"
        "mov %%ax, %%fs   \n"
        "mov %%ax, %%gs   \n"
        "push $0x23       \n"   
        "push %1          \n"  
        "pushf            \n"
        "pop %%eax        \n"
        "or $0x200, %%eax \n"   
        "push %%eax       \n"   
        "push $0x1b       \n"   
        "push %0          \n"  
        "sti              \n"
        "iret             \n"
        :
        : "r"(p->entry), "r"(p->user_stack_top)
        : "eax"
    );
}

process_t *process_create(const char *name, uint32_t entry, uint32_t cr3,
                           uint32_t user_stack_top, uint32_t kernel_stack_top) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_DEAD) {
            process_t *p = &process_table[i];

            p->pid              = next_pid++;
            p->state            = PROCESS_READY;
            p->entry            = entry;
            p->eip              = entry;
            p->cr3              = cr3;
            p->user_stack_top   = user_stack_top;
            p->kernel_stack_top = kernel_stack_top;

            int j = 0;
            while (name[j] && j < 31) { p->name[j] = name[j]; j++; }
            p->name[j] = 0;

            // set up fake kernel stack 
            uint32_t *kstack = (uint32_t *)kernel_stack_top;
            *(--kstack) = (uint32_t)process_entry_trampoline;
            *(--kstack) = 0; 
            *(--kstack) = 0;  
            *(--kstack) = 0;  
            *(--kstack) = 0;  
            p->esp = (uint32_t)kstack;

            // verify
            uint32_t *verify = (uint32_t *)p->esp;
            debug_serial_str("process: created '");
            debug_serial_str(p->name);
            debug_serial_str("' pid=");
            debug_serial_hex(p->pid);
            debug_serial_str("\n");

            return p;  
        }
    }

    debug_serial_str("process: table full!\n");
    return 0;
}

void process_exit(uint32_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].state = PROCESS_DEAD;
            debug_serial_str("process: pid=");
            debug_serial_hex(pid);
            debug_serial_str(" exited\n");
            return;
        }
    }
}

process_t *process_get_current(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == current_pid)
            return &process_table[i];
    }
    return 0;
}