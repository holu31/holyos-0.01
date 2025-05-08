#ifndef HEAP_H
#define HEAP_H

#include "stddef.h"

void heap_init();
void* kmalloc(size_t size);
void kfree(void* ptr);
void* krealloc(void* ptr, size_t size);
size_t get_allocated_memory();
size_t get_free_memory();
size_t get_heap_size();

#endif // HEAP_H
