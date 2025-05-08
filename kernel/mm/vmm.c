#include "kernel/mm/vmm.h"
#include "kernel/mm/pmm.h"
#include "kernel/mm/paging.h"
#include "kernel/string.h"
#include "kernel/kprintf.h"

static uint32_t* current_page_directory = 0;

void vmm_init() {
	log_info("[VMM] Initializing Virtual Memory Manager...\n");

	current_page_directory = (uint32_t*)pmm_alloc_page_aligned();
	log_debug("[VMM] Current page directory allocated at 0x%x\n", (uint32_t)current_page_directory);
	memset(current_page_directory, 0, PAGE_SIZE);

	uint32_t* kernel_page_directory = (uint32_t*)pmm_alloc_page_aligned();
	log_debug("[VMM] Kernel page directory allocated at 0x%x\n", (uint32_t)kernel_page_directory);
	memset(kernel_page_directory, 0, PAGE_SIZE);

	log_debug("[VMM] Setting up kernel page directory with identity mapping...\n");
	for (uint32_t i = 0; i < 1024; i++)
		kernel_page_directory[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE;

	current_page_directory[0] = (uint32_t)kernel_page_directory | PAGE_PRESENT | PAGE_WRITE;

	log_info("[VMM] Loading page directory (CR3=0x%x)\n", (uint32_t)current_page_directory);
	load_page_directory(current_page_directory);

	enable_paging();
	log_info("[VMM] Paging enabled (CR0.PG=1)\n");

	log_ok("[VMM] Initialized (kernel_pd=0x%x)\n", (uint32_t)kernel_page_directory);
}

bool vmm_map_page(void* virt, void* phys, uint32_t flags) {
	uint32_t pd_index = ((uint32_t)virt >> 22) & 0x3FF;
	uint32_t pt_index = ((uint32_t)virt >> 12) & 0x3FF;

	if (!(current_page_directory[pd_index] & PAGE_PRESENT)) {
		uint32_t* new_table = (uint32_t*)pmm_alloc_page_aligned();
		if (!new_table) return false;

		memset(new_table, 0, PAGE_SIZE);
		current_page_directory[pd_index] = (uint32_t) new_table | flags | PAGE_PRESENT;		
	}

	uint32_t* page_table = (uint32_t*)(current_page_directory[pd_index] & ~0xFFF);
	page_table[pt_index] = (uint32_t) phys | flags | PAGE_PRESENT;

	flush_tlb(virt);

	return true;
}

void* vmm_alloc_page(uint32_t flags) {
	void* phys = pmm_alloc_page_aligned();
	if (!phys) return 0;

	static uint32_t next_virt = 0xC0000000;
	void* virt = (void*)next_virt;
	next_virt += PAGE_SIZE;

	if (!vmm_map_page(virt, phys, flags)) {
		pmm_free_page(phys);
		return 0;
	}

	return virt;
}

void vmm_free_page(void* virt) {
	uint32_t pd_index = ((uint32_t)virt >> 22) & 0x3FF;
	uint32_t pt_index = ((uint32_t)virt >> 12) & 0x3FF;

	if (current_page_directory[pd_index] & PAGE_PRESENT) {
		uint32_t* page_table = (uint32_t*)(current_page_directory[pd_index] & ~0xFFF);
		void* phys = (void*)(page_table[pt_index] & ~0xFFF);

		pmm_free_page(phys);
		page_table[pt_index] = 0;
		flush_tlb(virt);
	}
}
