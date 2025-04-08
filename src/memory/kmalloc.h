#ifndef KMALLOC_H
#define KMALLOC_H

#include "../include/types.h"

// Initialize the kernel memory allocator
void kmalloc_init();

// Allocate memory
void* kmalloc(size_t size);

// Free memory (stub - this simple allocator can't really free)
void kfree(void* ptr);

#endif
