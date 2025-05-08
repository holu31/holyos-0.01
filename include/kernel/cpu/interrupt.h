#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "kernel/cpu/regs.h"

typedef void (*isr_handler_t)(struct regs *r);

void interrupt_register_handler(uint8_t num, isr_handler_t handler);

#endif // INTERRUPT_H
