#include "../include/types.h"
#include "../memory/physical.h"
#include "../memory/kmalloc.h"
#include "../cpu/interrupts.h"
#include "../drivers/timer.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen64.h"
#include "../terminal/terminal64.h"
#include "../kernel/low_level.h"

void kernel_main() {
    // Clear screen immediately
    volatile u16* video_mem = (volatile u16*)0xB8000;
    for (int i = 0; i < 80*25; i++) {
        video_mem[i] = ' ' | (0x07 << 8);  // Light gray on black
    }
    
    // Initialize systems in the correct order
    
    // 1. Memory management first
    pmm_init(128 * 1024 * 1024); // Assume 128MB RAM
    kmalloc_init();
    
    // 2. Interrupt system
    interrupts_init();
    
    // 3. Screen driver before terminal
    screen_init();
    
    // 4. Initialize devices
    timer_init(100);
    keyboard_init();
    
    // 5. Initialize terminal
    // Wait a moment to ensure all systems are stable
    for (volatile u64 i = 0; i < 500000; i++) {
        __asm__ __volatile__("nop");
    }
    
    // Initialize terminal
    terminal_init();

    // Main loop
    while (1) {
        // Poll keyboard
        keyboard_poll();
        
        // Process any keys through the terminal
        terminal_update();
        
        // Small delay to reduce CPU usage
        for (volatile int i = 0; i < 1000; i++) {
            __asm__ __volatile__("nop");
        }
    }
}