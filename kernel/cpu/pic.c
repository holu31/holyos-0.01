#include "kernel/cpu/pic.h"
#include "kernel/cpu/io.h"
#include "kernel/kprintf.h"

#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

void pic_init() {
	log_info("[PIC] Initializing 8259 Programmable Interrupt Controller...\n");

	log_debug("[PIC] Initialization sequence:\n");

	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	log_debug("[PIC] - Sent ICW1 to both PICs (init + ICW4)\n");
	
	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);
	log_debug("[PIC] - ICW2: Master vector offset=0x20-0x27, Slave=0x28-0x2F\n");
	
	outb(PIC1_DATA, 4);
	outb(PIC2_DATA, 2);
	log_debug("[PIC] - ICW3: Slave connected to IRQ2 (ID=2)\n");

	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
	log_debug("[PIC] - ICW4: 8086 mode, normal EOI\n");

	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
	log_debug("[PIC] Initial IRQ masks: Master=0xFF, Slave=0xFF\n");

	log_ok("[PIC] Initialized (master=0x20-0x27, slave=0x28-0x2F)\n");
}

void pic_mask_irq(uint8_t irq) {
	uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t mask = inb(port) | (1 << (irq % 8));
	outb(port, mask);
	kprintf(KERN_DEBUG "[PIC] Masked IRQ%d (port=0x%x, new_mask=0x%x)\n", irq, port, mask);
}

void pic_unmask_irq(uint8_t irq) {
	uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
	uint8_t mask = inb(port) & ~(1 << (irq % 8));
	outb(port, mask);
	kprintf(KERN_DEBUG "[PIC] Unmasked IRQ%d (port=0x%x, new_mask=0x%x)\n", irq, port, mask);
}

void pic_send_eoi(uint8_t irq) {
	if (irq >= 8) {
		outb(PIC2_COMMAND,PIC_EOI);
	}
	outb(PIC1_COMMAND,PIC_EOI);
}
