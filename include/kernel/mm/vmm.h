#ifndef VMM_H
#define VMM_H

#include "stdint.h"
#include "stdbool.h"

void vmm_init();
bool vmm_map_page(void* virt, void* phys, uint32_t flags);
void* vmm_alloc_page(uint32_t flags);
void vmm_free_page(void* virt);

#endif // VMM_H
