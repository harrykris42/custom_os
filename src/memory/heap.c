#include "../include/types.h"
#include "physical.h"
#include "virtual.h"
#include "heap.h"

// Block header structure
typedef struct block_header {
    size_t size;                 // Size of the block (including header)
    bool is_free;                // Is this block free?
    struct block_header* next;   // Next block in the list
    struct block_header* prev;   // Previous block in the list
} block_header_t;

// The start of the heap
static block_header_t* heap_start = NULL;

// Initialize the heap
void heap_init() {
    // Output debug message
    volatile char* debug_mem = (volatile char*)0xB8000 + 8 * 160; // Row 8
    const char* msg1 = "Heap: Starting initialization...";
    int i = 0;
    while (msg1[i]) {
        debug_mem[i*2] = msg1[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Allocate the first page for the heap
    void* page = vmm_alloc_page();
    
    // Output debug message about page allocation
    debug_mem = (volatile char*)0xB8000 + 9 * 160; // Row 9
    const char* msg2 = "Heap: Page allocated";
    i = 0;
    while (msg2[i]) {
        debug_mem[i*2] = msg2[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    if (!page) {
        // Failed to allocate heap
        debug_mem = (volatile char*)0xB8000 + 10 * 160; // Row 10
        const char* msg_err = "Heap: Failed to allocate page!";
        i = 0;
        while (msg_err[i]) {
            debug_mem[i*2] = msg_err[i];
            debug_mem[i*2+1] = 0x0F;
            i++;
        }
        return;
    }
    
    // Initialize the first block
    heap_start = (block_header_t*)page;
    heap_start->size = PAGE_SIZE - sizeof(block_header_t);
    heap_start->is_free = true;
    heap_start->next = NULL;
    heap_start->prev = NULL;
    
    // Output debug message about successful initialization
    debug_mem = (volatile char*)0xB8000 + 10 * 160; // Row 10
    const char* msg3 = "Heap: Initialization complete";
    i = 0;
    while (msg3[i]) {
        debug_mem[i*2] = msg3[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
}

// Rest of your heap implementation...
