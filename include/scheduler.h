// panache/include/scheduler.h

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <process.h>
#include <interrupts.h>

#include <stdint.h>

void scheduler_init(void);
void scheduler_tick(struct interrupt_registers *regs);       
void scheduler_start(void);       
void context_switch(uint32_t *old_esp, uint32_t new_esp, uint32_t cr3);
process_t *scheduler_next(void);  

#endif