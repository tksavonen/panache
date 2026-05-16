// panache/proc/scheduler.c

#include <scheduler.h>
#include <process.h>
#include <gdt.h>
#include <interrupts.h>
#include <serial.h>

extern struct tss_entry_struct tss_entry;

void scheduler_init(void) {
    debug_serial_str("scheduler init\n");
}

process_t *scheduler_next(void) {
    // round-robin
    uint32_t start = current_pid;
    int i = 0;

    int cur_idx = 0;
    for (int j = 0; j < MAX_PROCESSES; j++) {
        if (process_table[j].pid == current_pid) {
            cur_idx = j;
            break;
        }
    }

    // look for next ready process
    for (i = 1; i <= MAX_PROCESSES; i++) {
        int idx = (cur_idx + i) % MAX_PROCESSES;
        if (process_table[idx].state == PROCESS_READY ||
            process_table[idx].state == PROCESS_RUNNING) {
            return &process_table[idx];
        }
    }

    return 0;  // no process ready
}

void scheduler_tick(struct interrupt_registers *regs) {
     if (current_pid > 16) {
        debug_serial_str("CORRUPTION DETECTED\n");
        asm volatile("hlt");
        return;
     }

    process_t *cur  = process_get_current();
    process_t *next = scheduler_next();

    if (!next || !cur) return;
    if (next->pid == current_pid) return;  

    // save process state from IRQ frame
    cur->eip   = regs->eip;
    if (cur->state == PROCESS_RUNNING)
        cur->state = PROCESS_READY;

    next->state = PROCESS_RUNNING;
    current_pid = next->pid;

    tss_entry.esp0 = next->kernel_stack_top;

    context_switch(&cur->esp, next->esp, next->cr3);
}