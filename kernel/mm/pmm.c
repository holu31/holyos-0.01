#include "kernel/mm/pmm.h"
#include "kernel/string.h"
#include "kernel/cpu/spinlock.h"
#include "kernel/kprintf.h"

static spinlock_t pmm_lock;
static uint32_t* pmm_bitmap = 0;
static uint32_t pmm_bitmap_size = 0;
static uint32_t pmm_max_blocks = 0;
static uint32_t pmm_used_blocks = 0;

static void pmm_set_bit(uint32_t bit) {
	pmm_bitmap[bit / PMM_BITS_PER_ENTRY] |= (1 << (bit % PMM_BITS_PER_ENTRY));
}

static void pmm_clear_bit(uint32_t bit) {
	pmm_bitmap[bit / PMM_BITS_PER_ENTRY] &= ~(1 << (bit % PMM_BITS_PER_ENTRY));
}

static int pmm_test_bit(uint32_t bit) {
	return pmm_bitmap[bit / PMM_BITS_PER_ENTRY] & (1 << (bit % PMM_BITS_PER_ENTRY));
}

static bool is_page_aligned(void* addr) {
	return ((uint32_t)addr & (PMM_BLOCK_SIZE - 1)) == 0;
}

void pmm_init(multiboot_info_t *mb_info) {
	spinlock_init(&pmm_lock);
	log_info("[PMM] Initializing Physical Memory Manager...\n");

	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
	multiboot_memory_map_t* mmap_end = (multiboot_memory_map_t*)(mb_info->mmap_addr + mb_info->mmap_length);

	uint32_t max_mem = 0;
	uint32_t available_mem = 0;

	log_debug("[PMM] Memory map:\n");
	while (mmap < mmap_end) {
		const char* type_str = (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) ? "AVAILABLE" : "RESERVED";
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			uint32_t end_addr = mmap->addr + mmap->len;
			if (end_addr > max_mem) max_mem = end_addr;
			available_mem += mmap->len;
		}
		log_debug("[PMM] - 0x%x-0x%x: %s (type=%d, len=%d KiB)\n",
				(uint32_t)mmap->addr, 
				(uint32_t)(mmap->addr + mmap->len - 1),
				type_str,
				mmap->type,
				(uint32_t)(mmap->len / 1024));
		mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
	}

	pmm_max_blocks = max_mem / PMM_BLOCK_SIZE;
	pmm_bitmap_size = (pmm_max_blocks + PMM_BITS_PER_ENTRY - 1) / PMM_BITS_PER_ENTRY;

	log_info("[PMM] Memory stats:\n");
	log_info("[PMM] - Total memory: %d MiB (%d blocks)\n", max_mem / (1024*1024), pmm_max_blocks);
	log_info("[PMM] - Available: %d MiB\n", 
			available_mem / (1024*1024));
	log_info("[PMM] - Block size: %d KiB\n", PMM_BLOCK_SIZE / 1024);
	log_info("[PMM] - Bitmap size: %d bytes (%d entries)\n",
			pmm_bitmap_size * sizeof(uint32_t), pmm_bitmap_size);

	mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
	while (mmap < mmap_end) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE
			&& mmap->len >= pmm_bitmap_size * sizeof(uint32_t)) {
			pmm_bitmap = (uint32_t*)(uint32_t)mmap->addr;
			break;
		}
		mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
	}

	log_debug("[PMM] Bitmap allocated at 0x%x\n", (uint32_t)pmm_bitmap);
	memset(pmm_bitmap, 0xFF, pmm_bitmap_size * sizeof(uint32_t));

	mmap = (multiboot_memory_map_t*)mb_info->mmap_addr;
	while (mmap < mmap_end) {
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			uint32_t start_block = mmap->addr / PMM_BLOCK_SIZE;
			uint32_t end_block = (mmap->addr + mmap->len) / PMM_BLOCK_SIZE;
			log_debug("[PMM] Marking blocks %d-%d as free\n", start_block, end_block-1);

			for (uint32_t block = start_block; block < end_block; block++) {
				pmm_clear_bit(block);
			}
		}
		mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
	}

	uint32_t bitmap_start = (uint32_t)pmm_bitmap / PMM_BLOCK_SIZE;
	uint32_t bitmap_end = bitmap_start + pmm_bitmap_size * sizeof(uint32_t) / PMM_BLOCK_SIZE + 1;

	log_debug("[PMM] Marking bitmap area (blocks %d-%d) as used\n", bitmap_start, bitmap_end-1);

	for (uint32_t block = bitmap_start; block < bitmap_end; block++) {
		pmm_set_bit(block);
		pmm_used_blocks++;
	}

	log_ok("[PMM] Initialized (%d%% used)\n",
			(pmm_used_blocks * 100) / pmm_max_blocks);
}

static void* pmm_alloc_page_internal(bool aligned_only) {
	spinlock_lock(&pmm_lock);

	for (uint32_t block = 0; block < pmm_max_blocks; block++) {
		if (!pmm_test_bit(block)) {
			void* addr = (void*)(block * PMM_BLOCK_SIZE);

			if (aligned_only && !is_page_aligned(addr)) {
				continue;
			}

			pmm_set_bit(block);
			pmm_used_blocks++;
			spinlock_unlock(&pmm_lock);
			return addr;
		}
	}

	spinlock_unlock(&pmm_lock);
	return 0;
}

void* pmm_alloc_page() {
	return pmm_alloc_page_internal(false);	
}

void* pmm_alloc_page_aligned() {
	return pmm_alloc_page_internal(true);
}

void pmm_free_page(void* page) {
	uint32_t block = (uint32_t)page / PMM_BLOCK_SIZE;
	if (block >= pmm_max_blocks) return;

	if (pmm_test_bit(block)) {
		pmm_clear_bit(block);
		pmm_used_blocks--;
	}
}

uint32_t pmm_get_free_page_count() {
	return pmm_max_blocks - pmm_used_blocks;
}

bool pmm_is_page_free(void* page) {
	if ((uint32_t)page % PMM_BLOCK_SIZE != 0) {
		return false;
	}

	uint32_t block = (uint32_t)page / PMM_BLOCK_SIZE;
	if (block >= pmm_max_blocks) {
		return false;
	}

	return !pmm_test_bit(block);
}
