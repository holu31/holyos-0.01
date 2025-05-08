#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mi;
	uint8_t access;
	uint8_t flags;
	uint8_t base_hi;
} __attribute__((packed));

struct gdt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void gdt_init(void);

#endif // GDT_H
