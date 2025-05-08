#include "kernel/cpu/gdt.h"
#include "kernel/string.h"
#include "kernel/kprintf.h"

static struct gdt_entry gdt_entries[6];
static struct gdt_ptr gdt_ptr;

extern void gdt_flush(const struct gdt_ptr* gdt_ptr);

static void gdt_set_entry(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	gdt_entries[i].limit_lo   = (limit & 0xFFFF);
	gdt_entries[i].base_lo = (base & 0xFFFF);
	gdt_entries[i].base_mi = (base >> 16) & 0xFF;		
	gdt_entries[i].access = access;
	gdt_entries[i].flags = flags | ((limit >> 16) & 0x0F);
	gdt_entries[i].base_hi = (base >> 24) & 0xFF;
}

void gdt_init(void) {
	log_info("[GDT] Initializing Global Descriptor Table...\n");

	log_debug("[GDT] Entries:\n");

	memset(gdt_entries, 0, sizeof(struct gdt_entry));
	log_debug("[GDT] - #0: NULL (base=0x0, limit=0x0)\n");

	gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xC0);
	log_debug("[GDT] - #1: Kernel Code (base=0x0, limit=0xFFFFF, flags=0x9A, granularity=0xC0)\n");

	gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
	log_debug("[GDT] - #2: Kernel Data (base=0x0, limit=0xFFFFF, flags=0x92, granularity=0xC0)\n");

	gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xC0);
	log_debug("[GDT] - #3: User Code (base=0x0, limit=0xFFFFF, flags=0xFA, granularity=0xC0)\n");

	gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);
	log_debug("[GDT] - #4: User Data (base=0x0, limit=0xFFFFF, flags=0xF2, granularity=0xC0)\n");

	uint32_t num_entries = 0;
	for (uint32_t i = 0; i < sizeof(gdt_entries) / sizeof(struct gdt_entry); i++) {
		if (gdt_entries[i].access != 0 || gdt_entries[i].flags != 0) {
			num_entries = i + 1;
		}
	}

	gdt_ptr.limit = sizeof(struct gdt_entry) * num_entries - 1;
	gdt_ptr.base = (uint32_t) &gdt_entries;

	gdt_flush(&gdt_ptr);
	log_ok("[GDT] Initialized (addr=0x%x, entries=%d)\n",
			(uint32_t) &gdt_ptr, num_entries);
}
