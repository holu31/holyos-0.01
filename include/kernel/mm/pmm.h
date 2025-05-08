#ifndef PMM_H
#define PMM_H

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#include "kernel/multiboot.h"

#define PMM_BLOCK_SIZE 4096
#define PMM_BITS_PER_ENTRY 32

void pmm_init(multiboot_info_t* mb_info);
void* pmm_alloc_page();
void* pmm_alloc_page_aligned();
void pmm_free_page(void* page);
uint32_t pmm_get_free_page_count();
bool pmm_is_page_free(void* page);

#endif // PMM_H
