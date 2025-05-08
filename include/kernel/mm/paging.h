#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_PRESENT	0x01
#define PAGE_WRITE	0x02
#define PAGE_USER	0x04

#define PAGE_SIZE	4096

typedef uint32_t page_table_entry_t;
typedef uint32_t page_directory_entry_t;

void load_page_directory(uint32_t* pd);
void enable_paging();
void flush_tlb(void* addr);

#endif // PAGING_H
