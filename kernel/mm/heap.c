#include "kernel/mm/heap.h"
#include "kernel/mm/vmm.h"
#include "kernel/mm/pmm.h"
#include "kernel/mm/paging.h"
#include "kernel/cpu/spinlock.h"
#include "kernel/string.h"
#include "kernel/kprintf.h"

#define HEAP_MAGIC		0xDEADBEEF
#define HEAP_MIN_SIZE		0x10000	// 64 KiB
#define HEAP_EXPAND_SIZE	0x10000
#define HEAP_ALIGNMENT		16

typedef struct heap_block {
    uint32_t magic;
    size_t size;
    struct heap_block* next;
    uint8_t used;
} heap_block_t;

static heap_block_t* heap_start = 0;
static heap_block_t* heap_end = 0;
static size_t heap_size = 0;
static spinlock_t heap_lock;

static void heap_expand(size_t size) {
	size_t required_size = size + sizeof(heap_block_t);
	size_t pages = (required_size + HEAP_EXPAND_SIZE - 1) / HEAP_EXPAND_SIZE;
	
	for (size_t i = 0; i < pages; i++) {
		void* new_mem = vmm_alloc_page(PAGE_PRESENT | PAGE_WRITE);
		if (!new_mem) return;

		heap_block_t* new_block = (heap_block_t*)new_mem;
		new_block->magic = HEAP_MAGIC;
		new_block->size = PAGE_SIZE - sizeof(heap_block_t);
		new_block->next = 0;

		heap_end->next = new_block;
		heap_end = new_block;
		heap_size += PAGE_SIZE;
	}
}

static heap_block_t* find_free_block(size_t size) {
	for (heap_block_t* current = heap_start;
			current; current = current->next) {
		if (!current->used && current->size >= size)
			return current;
	}

	return 0;
}

static void split_block(heap_block_t* block, size_t size) {
	heap_block_t* new_block = (heap_block_t*)((uint32_t)block + sizeof(heap_block_t) + size);

	new_block->magic = HEAP_MAGIC;
	new_block->size = block->size - size - sizeof(heap_block_t);
	new_block->next = block->next;
	new_block->used = 0;

	block->size = size;
	block->next = new_block;

	if (block == heap_end) {
		heap_end = new_block;
	}
}

static void merge_blocks() {
	heap_block_t* current = heap_start;

	while (current && current->next) {
		if (!current->used && !current->next->used) {
			current->size += sizeof(heap_block_t) + current->next->size;
			current->next = current->next->next;

			if (current->next == 0) {
				heap_end = current;
			}
		} else {
			current = current->next;
		}
	}
}

void heap_init() {
	log_info("[HEAP] Initializing kernel heap...\n");
	
	spinlock_init(&heap_lock);	

	heap_start = (heap_block_t*)vmm_alloc_page(PAGE_PRESENT | PAGE_WRITE);
	if (!heap_start) {
		log_err("[HEAP] Failed to allocate initial heap page!\n");
		return;
	}
	
	heap_start->magic = HEAP_MAGIC;
	heap_start->size = PAGE_SIZE - sizeof(heap_block_t);
	heap_start->next = 0;
	heap_start->used = 0;

	heap_end = heap_start;
	heap_size = PAGE_SIZE;

	log_debug("[HEAP] Initial heap block: size=%d bytes, magic=0x%x\n", 
			heap_start->size, heap_start->magic);
    
	log_ok("[HEAP] Kernel heap initialized at 0x%x\n", (uint32_t) heap_start);
}

void* kmalloc(size_t size) {
	if (size == 0) return 0;

	spinlock_lock(&heap_lock);

	size = (size + HEAP_ALIGNMENT - 1) & ~(HEAP_ALIGNMENT - 1);
	heap_block_t* block = find_free_block(size);

	if (!block) {
		heap_expand(size + + sizeof(heap_block_t));
		block = find_free_block(size);
		if (!block) {
			spinlock_unlock(&heap_lock);
			return 0;
		}
	}

	if (block->size >= size + sizeof(heap_block_t) + HEAP_ALIGNMENT) {
		split_block(block, size);
	}

	block->used = 1;
	void* ptr = (void*)((uint32_t)block + sizeof(heap_block_t));

	spinlock_unlock(&heap_lock);
	return ptr;
}

void kfree(void* ptr) {
	if (!ptr) return;

	spinlock_lock(&heap_lock);

	heap_block_t* block = (heap_block_t*)((uint32_t)ptr - sizeof(heap_block_t));

	if (block->magic != HEAP_MAGIC) {
		spinlock_unlock(&heap_lock);
		return;
	}

	block->used = 0;
	merge_blocks();

	spinlock_unlock(&heap_lock);
}

void* krealloc(void* ptr, size_t size) {
	if (!ptr) return kmalloc(size);
	if (size == 0) {
		kfree(ptr);
		return 0;
	}

	spinlock_lock(&heap_lock);

	heap_block_t* block = (heap_block_t*)((uint32_t)ptr - sizeof(heap_block_t));

	if (block->magic != HEAP_MAGIC) {
		spinlock_unlock(&heap_lock);
		return 0;
	}

	size = (size + HEAP_ALIGNMENT - 1) & ~(HEAP_ALIGNMENT - 1);

	if (block->size >= size) {
		if (block->size >= size + sizeof(heap_block_t) + HEAP_ALIGNMENT) {
			split_block(block, size);
		}
		spinlock_unlock(&heap_lock);
		return ptr;
	}

	void* new_ptr = kmalloc(size);
	if (!new_ptr) {
		spinlock_unlock(&heap_lock);
		return 0;
	}

	memcpy(new_ptr, ptr, block->size);

	block->used = 0;
	merge_blocks();

	spinlock_unlock(&heap_lock);
	return new_ptr;
}

size_t get_allocated_memory() {
	size_t total = 0;
	heap_block_t* current = heap_start;

	spinlock_lock(&heap_lock);

	while (current) {
		if (current->used) {
			total += current->size;
		}
		current = current->next;
	}

	spinlock_unlock(&heap_lock);
	return total;
}

size_t get_free_memory() {
	size_t total = 0;
	heap_block_t* current = heap_start;

	spinlock_lock(&heap_lock);

	while (current) {
		if (!current->used) {
			total += current->size;
		}
		current = current->next;
	}

	spinlock_unlock(&heap_lock);
	return total;
}

size_t get_heap_size() {
	spinlock_lock(&heap_lock);
	size_t size = heap_size;
	spinlock_unlock(&heap_lock);
	return size;
}
