#ifndef VIRTUAL_H
#define VIRTUAL_H

#include "../include/types.h"
#include "physical.h"

// Page table entry flags
#define PAGE_PRESENT  (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_USER     (1ULL << 2)
#define PAGE_HUGE     (1ULL << 7)

// Initialize the virtual memory manager
void vmm_init();

// Map a virtual address to a physical address
bool vmm_map_page(u64 virt_addr, u64 phys_addr, u64 flags);

// Unmap a virtual address
void vmm_unmap_page(u64 virt_addr);

// Get the physical address for a virtual address
u64 vmm_get_physical_address(u64 virt_addr);

// Allocate a page and map it (used by the heap)
void* vmm_alloc_page();

// Free a page and unmap it
void vmm_free_page(void* virt_addr);

#endif
