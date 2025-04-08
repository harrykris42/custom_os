#ifndef HEAP_H
#define HEAP_H

#include "../include/types.h"
#include "virtual.h"

// Initialize the heap
void heap_init();

// Allocate memory
void* kmalloc(size_t size);

// Free memory
void kfree(void* ptr);

// Reallocate memory
void* krealloc(void* ptr, size_t size);

#endif
