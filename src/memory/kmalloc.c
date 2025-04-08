#include "../include/types.h"
#include "physical.h"
#include "kmalloc.h"

// Simple bump allocator
static void* next_free = (void*)0x400000;  // 4MB mark

void kmalloc_init() {
    // Nothing to do for a bump allocator
}

void* kmalloc(size_t size) {
    // Round up to 8-byte alignment
    size = (size + 7) & ~7;
    
    void* result = next_free;
    next_free = (void*)((u64)next_free + size);
    
    return result;
}

void kfree(void* ptr) {
    // Can't really free with a bump allocator
    // This is just a stub
}
