#include "../include/types.h"
#include "../memory/physical.h"
#include "../memory/kmalloc.h"
#include "../cpu/interrupts.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen64.h"
#include "../terminal/terminal64.h"
#include "../kernel/low_level.h"

// Simple function to test direct keyboard input
void test_keyboard_input() {
    // Status of keyboard controller
    u8 status = port_byte_in(0x64);
    
    // Debug output directly to video memory
    volatile u16* video_mem = (volatile u16*)0xB8000;
    
    // Display status byte
    char status_hex[3];
    status_hex[0] = "0123456789ABCDEF"[(status >> 4) & 0x0F];
    status_hex[1] = "0123456789ABCDEF"[status & 0x0F];
    
    video_mem[14 * 80] = 'K' | (0x0F << 8);
    video_mem[14 * 80 + 1] = 'B' | (0x0F << 8);
    video_mem[14 * 80 + 2] = 'D' | (0x0F << 8);
    video_mem[14 * 80 + 3] = ':' | (0x0F << 8);
    video_mem[14 * 80 + 4] = status_hex[0] | (0x0F << 8);
    video_mem[14 * 80 + 5] = status_hex[1] | (0x0F << 8);
    
    // Check if data is available
    if (status & 1) {
        u8 data = port_byte_in(0x60);
        
        // Display data byte
        char data_hex[3];
        data_hex[0] = "0123456789ABCDEF"[(data >> 4) & 0x0F];
        data_hex[1] = "0123456789ABCDEF"[data & 0x0F];
        
        video_mem[14 * 80 + 7] = 'D' | (0x0F << 8);
        video_mem[14 * 80 + 8] = ':' | (0x0F << 8);
        video_mem[14 * 80 + 9] = data_hex[0] | (0x0F << 8);
        video_mem[14 * 80 + 10] = data_hex[1] | (0x0F << 8);
    }
}

// Function to write a string directly to video memory
void print(const char* str, int row) {
    volatile char* video_mem = (volatile char*)0xB8000;
    video_mem += row * 160; // 80 columns * 2 bytes per character
    
    while (*str) {
        *video_mem = *str;
        video_mem++;
        *video_mem = 0x0F; // White on black
        video_mem++;
        str++;
    }
}

// Function to convert an integer to a string
void int_to_string(u64 num, char* str) {
    int i = 0;
    
    // Handle 0 explicitly
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    // Process individual digits
    while (num > 0) {
        str[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Terminate string
    str[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void kernel_main() {
    // Clear screen
    for (int i = 0; i < 80*25; i++) {
        volatile char* video_mem = (volatile char*)0xB8000;
        video_mem[i*2] = ' ';
        video_mem[i*2+1] = 0x0F;
    }
    
    // Display welcome message
    
    // Initialize physical memory manager
    pmm_init(128 * 1024 * 1024); // Assume 128MB RAM
    
    // Initialize memory allocator
    kmalloc_init();
    
    // Initialize interrupt system;
    interrupts_init();
    
    // Initialize timer (100Hz)
    timer_init(100);
    
    // Initialize keyboard
    keyboard_init();
    
    // Testing memory allocation
    void* ptr1 = kmalloc(1024);
    void* ptr2 = kmalloc(2048);
    void* ptr3 = kmalloc(4096);
    kfree(ptr2);
    void* ptr4 = kmalloc(1024);
    
    // Initialize screen
    screen_init();
    
    // Wait a moment
    for (u64 i = 0; i < 50000000; i++) {
        __asm__ __volatile__("nop");
    }
    
    // Initialize terminal
    terminal_init();

    // Main loop with both interrupt-based and direct polling
    while (1) {
        // Poll keyboard (no debug output)
        keyboard_poll();
        
        // Process any keys through the terminal
        terminal_update();
        
        // Small delay to reduce CPU usage
        for (volatile int i = 0; i < 10000; i++);
    }
}