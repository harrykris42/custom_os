#include "../include/types.h"
#include "physical.h"

static u8* memory_bitmap = NULL;
static u64 total_pages = 0;
static u64 free_pages = 0;

void pmm_init(u64 mem_size) {
    // Simple initialization
    memory_bitmap = (u8*)0x100000;  // 1MB mark
    total_pages = mem_size / PAGE_SIZE;
    free_pages = total_pages;
    
    // Clear bitmap
    for (u64 i = 0; i < (total_pages + 7) / 8; i++) {
        memory_bitmap[i] = 0;
    }
    
    // Mark first 2MB as used
    u64 reserved = 2 * 1024 * 1024 / PAGE_SIZE;
    for (u64 i = 0; i < reserved; i++) {
        u64 byte = i / 8;
        u8 bit = i % 8;
        memory_bitmap[byte] |= (1 << bit);
    }
    
    free_pages -= reserved;
}

u64 pmm_alloc_page() {
    if (free_pages == 0) return 0;
    
    for (u64 i = 0; i < total_pages; i++) {
        u64 byte = i / 8;
        u8 bit = i % 8;
        
        if (!(memory_bitmap[byte] & (1 << bit))) {
            memory_bitmap[byte] |= (1 << bit);
            free_pages--;
            return i * PAGE_SIZE;
        }
    }
    
    return 0;
}

void pmm_free_page(u64 addr) {
    u64 page = addr / PAGE_SIZE;
    
    if (page >= total_pages) return;
    
    u64 byte = page / 8;
    u8 bit = page % 8;
    
    if (memory_bitmap[byte] & (1 << bit)) {
        memory_bitmap[byte] &= ~(1 << bit);
        free_pages++;
    }
}

// Get the total number of free pages
u64 pmm_get_free_pages() {
    return free_pages;
}

// Get the total number of pages
u64 pmm_get_total_pages() {
    return total_pages;
}

// Get memory usage statistics
void pmm_get_stats(u64* total, u64* free, u64* used) {
    *total = total_pages;
    *free = free_pages;
    *used = total_pages - free_pages;
}