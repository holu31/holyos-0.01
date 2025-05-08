#include <kernel/cpu/interrupt.h>
#include "kernel/cpu/pic.h"
#include <kernel/kprintf.h>

static isr_handler_t interrupt_handlers[48];

void interrupt_register_handler(uint8_t num, isr_handler_t handler) {
	if (num >= 48) {
		log_err("[INT] Invalid interrupt number: %d\n", num);
		return;
	}
	interrupt_handlers[num] = handler;

	if (num >= 32 && num < 48) {
		pic_unmask_irq(num - 32);
	}
}

void interrupt_handler(struct regs *r) {
	if (interrupt_handlers[r->int_no]) {
		interrupt_handlers[r->int_no](r);
	} else {
		log_err("[INT] Unhandled interrupt %d!\n", r->int_no);
		for (;;) asm volatile ("hlt");
	}

	if (r->int_no >= 32 && r->int_no < 48) {
		pic_send_eoi(r->int_no - 32);
	}
}
