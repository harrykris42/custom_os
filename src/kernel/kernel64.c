#include "../include/types.h"
#include "../memory/physical.h"
#include "../memory/kmalloc.h"
#include "../cpu/interrupts.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"

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
    print("CustomOS 64-bit - Kernel Initialized", 0);
    print("----------------------------------------", 1);
    
    // Initialize physical memory manager
    print("Initializing physical memory manager...", 2);
    pmm_init(128 * 1024 * 1024); // Assume 128MB RAM
    print("Physical memory manager initialized!", 2);
    
    // Initialize memory allocator
    print("Initializing memory allocator...", 3);
    kmalloc_init();
    print("Memory allocator initialized!", 3);
    
    // Initialize interrupt system
    print("Initializing interrupt system...", 4);
    interrupts_init();
    print("Interrupt system initialized!", 4);
    
    // Initialize timer (100Hz)
    print("Initializing timer...", 5);
    timer_init(100);
    print("Timer initialized! (100Hz)", 5);
    
    // Initialize keyboard
    print("Initializing keyboard...", 6);
    keyboard_init();
    print("Keyboard initialized!", 6);
    
    // Testing memory allocation
    print("Testing memory allocation...", 8);
    void* ptr1 = kmalloc(1024);
    void* ptr2 = kmalloc(2048);
    void* ptr3 = kmalloc(4096);
    kfree(ptr2);
    void* ptr4 = kmalloc(1024);
    print("Memory allocation successful!", 8);
    
    // Interactive section
    print("System is now running. Press keys to test the keyboard.", 10);
    print("Current timer ticks: 0", 11);
    
    // Main loop
    u64 last_tick = 0;
    while (1) {
        // Update timer display if ticks have changed
        u64 current_ticks = timer_get_ticks();
        if (current_ticks != last_tick) {
            last_tick = current_ticks;
            char tick_str[20];
            int_to_string(current_ticks, tick_str);
            print("Current timer ticks: ", 11);
            print(tick_str, 11);
        }
        
        // Halt CPU until next interrupt
        __asm__ __volatile__("hlt");
    }
}
