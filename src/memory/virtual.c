#include "../include/types.h"
#include "physical.h"
#include "virtual.h"

// Page table structure pointers
static u64* pml4_table = NULL;

// Helper function to get various page table indices
static inline u64 pml4_index(u64 addr) { return (addr >> 39) & 0x1FF; }
static inline u64 pdpt_index(u64 addr) { return (addr >> 30) & 0x1FF; }
static inline u64 pd_index(u64 addr)   { return (addr >> 21) & 0x1FF; }
static inline u64 pt_index(u64 addr)   { return (addr >> 12) & 0x1FF; }

// Get or create page table
static u64* get_next_level(u64* table, u64 index, bool create) {
    if (!(table[index] & PAGE_PRESENT)) {
        if (!create) {
            return NULL;
        }
        
        // Allocate a new page table
        u64 new_table_phys = pmm_alloc_page();
        if (!new_table_phys) {
            return NULL; // Out of memory
        }
        
        // Clear the new table
        u64* new_table = (u64*)new_table_phys;
        for (int i = 0; i < 512; i++) {
            new_table[i] = 0;
        }
        
        // Add entry to the parent table
        table[index] = new_table_phys | PAGE_PRESENT | PAGE_WRITABLE;
    }
    
    // Return the next level table
    return (u64*)(table[index] & ~0xFFF);
}

// Initialize the virtual memory manager
void vmm_init() {
    // Use the existing PML4 table
    u64 cr3_value;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3_value));
    pml4_table = (u64*)(cr3_value & ~0xFFF);
}

// Map a virtual address to a physical address
bool vmm_map_page(u64 virt_addr, u64 phys_addr, u64 flags) {
    // Ensure addresses are page-aligned
    virt_addr &= ~0xFFF;
    phys_addr &= ~0xFFF;
    
    // Get or create page tables
    u64* pdpt = get_next_level(pml4_table, pml4_index(virt_addr), true);
    if (!pdpt) return false;
    
    u64* pd = get_next_level(pdpt, pdpt_index(virt_addr), true);
    if (!pd) return false;
    
    u64* pt = get_next_level(pd, pd_index(virt_addr), true);
    if (!pt) return false;
    
    // Set the page table entry
    pt[pt_index(virt_addr)] = phys_addr | flags | PAGE_PRESENT;
    
    // Invalidate TLB for this address
    __asm__ __volatile__("invlpg (%0)" : : "r"(virt_addr) : "memory");
    
    return true;
}

// Unmap a virtual address
void vmm_unmap_page(u64 virt_addr) {
    // Ensure address is page-aligned
    virt_addr &= ~0xFFF;
    
    // Get the page tables
    u64* pdpt = get_next_level(pml4_table, pml4_index(virt_addr), false);
    if (!pdpt) return;
    
    u64* pd = get_next_level(pdpt, pdpt_index(virt_addr), false);
    if (!pd) return;
    
    u64* pt = get_next_level(pd, pd_index(virt_addr), false);
    if (!pt) return;
    
    // Clear the page table entry
    pt[pt_index(virt_addr)] = 0;
    
    // Invalidate TLB for this address
    __asm__ __volatile__("invlpg (%0)" : : "r"(virt_addr) : "memory");
}

// Get the physical address for a virtual address
u64 vmm_get_physical_address(u64 virt_addr) {
    u64 page_offset = virt_addr & 0xFFF;
    
    // Get the page tables
    u64* pdpt = get_next_level(pml4_table, pml4_index(virt_addr), false);
    if (!pdpt) return 0;
    
    u64* pd = get_next_level(pdpt, pdpt_index(virt_addr), false);
    if (!pd) return 0;
    
    u64* pt = get_next_level(pd, pd_index(virt_addr), false);
    if (!pt) return 0;
    
    // Get the page table entry
    u64 pte = pt[pt_index(virt_addr)];
    if (!(pte & PAGE_PRESENT)) return 0;
    
    // Return the physical address
    return (pte & ~0xFFF) | page_offset;
}

// Allocate a page and map it
void* vmm_alloc_page() {
    // Debug message
    volatile char* debug_mem = (volatile char*)0xB8000 + 12 * 160; // Row 12
    const char* msg = "VMM: Allocating page...";
    int i = 0;
    while (msg[i]) {
        debug_mem[i*2] = msg[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Find a free virtual address (this is a simple implementation)
    static u64 next_vaddr = 0x400000; // 4MB (above kernel space)
    
    u64 virt_addr = next_vaddr;
    next_vaddr += PAGE_SIZE;
    
    // Debug message
    debug_mem = (volatile char*)0xB8000 + 13 * 160; // Row 13
    char addr_str[20] = "VMM: vaddr=";
    char* ptr = addr_str + 11;
    
    // Convert address to hex string (simplified)
    u64 temp = virt_addr;
    for (int j = 0; j < 8; j++) {
        int digit = temp % 16;
        if (digit < 10)
            ptr[7-j] = '0' + digit;
        else
            ptr[7-j] = 'A' + (digit - 10);
        temp /= 16;
    }
    ptr[8] = '\0';
    
    i = 0;
    while (addr_str[i]) {
        debug_mem[i*2] = addr_str[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Allocate a physical page
    u64 phys_addr = pmm_alloc_page();
    
    // Debug message
    debug_mem = (volatile char*)0xB8000 + 14 * 160; // Row 14
    char phys_str[20] = "VMM: paddr=";
    ptr = phys_str + 11;
    
    // Convert address to hex string (simplified)
    temp = phys_addr;
    for (int j = 0; j < 8; j++) {
        int digit = temp % 16;
        if (digit < 10)
            ptr[7-j] = '0' + digit;
        else
            ptr[7-j] = 'A' + (digit - 10);
        temp /= 16;
    }
    ptr[8] = '\0';
    
    i = 0;
    while (phys_str[i]) {
        debug_mem[i*2] = phys_str[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    if (!phys_addr) {
        debug_mem = (volatile char*)0xB8000 + 15 * 160; // Row 15
        const char* err_msg = "VMM: Failed to allocate physical page!";
        i = 0;
        while (err_msg[i]) {
            debug_mem[i*2] = err_msg[i];
            debug_mem[i*2+1] = 0x0F;
            i++;
        }
        return NULL; // Out of memory
    }
    
    // Map the virtual address to the physical address
    bool success = vmm_map_page(virt_addr, phys_addr, PAGE_WRITABLE);
    
    // Debug message
    debug_mem = (volatile char*)0xB8000 + 15 * 160; // Row 15
    const char* map_msg = success ? "VMM: Mapping successful" : "VMM: Mapping failed!";
    i = 0;
    while (map_msg[i]) {
        debug_mem[i*2] = map_msg[i];
        debug_mem[i*2+1] = 0x0F;
        i++;
    }
    
    if (!success) {
        pmm_free_page(phys_addr);
        return NULL;
    }
    
    return (void*)virt_addr;
}

// Free a page and unmap it
void vmm_free_page(void* virt_addr) {
    u64 phys_addr = vmm_get_physical_address((u64)virt_addr);
    if (phys_addr) {
        vmm_unmap_page((u64)virt_addr);
        pmm_free_page(phys_addr);
    }
}
