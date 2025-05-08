#include "kernel/cpu/idt.h"
#include "kernel/string.h"
#include "kernel/kprintf.h"

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr;

extern void idt_load(const struct idt_ptr* ptr);

extern uint32_t isr_stub_table[];
extern uint32_t irq_stub_table[]; 

void idt_set_entry(uint32_t i, uint32_t base, uint16_t sel, uint8_t flags) {
	idt_entries[i].base_lo = (base & 0xFFFF);
	idt_entries[i].sel = sel;
	idt_entries[i].zero = 0;
	idt_entries[i].flags = flags | 0x60;
	idt_entries[i].base_hi = (base >> 16) & 0xFFFF;	
}

void idt_init() {
	log_info("[IDT] Initializing Interrupt Descriptor Table...\n");
	
	idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
	idt_ptr.base = (uint32_t) &idt_entries;
	log_debug("[IDT] Pointer configured: limit=0x%x, base=0x%x\n", 
			idt_ptr.limit, idt_ptr.base);

	memset(idt_entries, 0, sizeof(struct idt_entry));
	
	for (int i = 0; i < 32; i++) {
		idt_set_entry(i, isr_stub_table[i], 0x08, 0x8E);
	}

	for (int i = 0; i < 16; i++) {
		idt_set_entry(32 + i, irq_stub_table[i], 0x08, 0x8E);
	}

	idt_load(&idt_ptr);
	log_ok("[IDT] Successfully initialized and loaded\n");
}
