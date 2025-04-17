#ifndef PHYSICAL_H
#define PHYSICAL_H

#include "../include/types.h"

// The physical memory manager works with 4KB pages
#define PAGE_SIZE 4096

// Initialize the physical memory manager
void pmm_init(u64 mem_size);

// Allocate a physical page, returns the physical address
u64 pmm_alloc_page();

// Free a previously allocated page
void pmm_free_page(u64 addr);

// Get the total number of free pages
u64 pmm_get_free_pages();

// Get the total number of pages
u64 pmm_get_total_pages();

// Get memory usage statistics
void pmm_get_stats(u64* total, u64* free, u64* used);

#endif