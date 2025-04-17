#include "../include/types.h"
#include "../drivers/timer.h"
#include "../kernel/low_level.h"
#include "../cpu/interrupts.h"

// The PIT uses a crystal oscillator running at 1.193182 MHz
#define PIT_FREQUENCY 1193182

// Timer tick counter
static volatile u64 timer_ticks = 0;

// Timer interrupt handler - simplified to avoid conflicts
static void timer_callback(registers_t regs) {
    timer_ticks++;
    
    // Just a minimal ticker for debugging - write to a single spot
    volatile u16* vga = (volatile u16*)0xB8000;
    vga[22 * 80] = 'T' | (0x0F << 8);
    vga[22 * 80 + 1] = 'i' | (0x0F << 8);
    vga[22 * 80 + 2] = 'c' | (0x0F << 8);
    vga[22 * 80 + 3] = 'k' | (0x0F << 8);
}

// Initialize the timer with a specific frequency
void timer_init(u32 frequency) {
    // Register the timer handler
    register_interrupt_handler(32, timer_callback);
    
    // Calculate the divisor
    u32 divisor = PIT_FREQUENCY / frequency;
    
    // Set the PIT to repeating mode
    port_byte_out(0x43, 0x36);  // Command: channel 0, access mode: lobyte/hibyte, mode 3 (square wave)
    
    // Send divisor
    port_byte_out(0x40, divisor & 0xFF);         // Low byte
    port_byte_out(0x40, (divisor >> 8) & 0xFF);  // High byte
}

// Get the current tick count
u64 timer_get_ticks() {
    return timer_ticks;
}

// Sleep for a specified number of milliseconds
void timer_sleep(u32 ms) {
    u64 start_ticks = timer_ticks;
    u64 ticks_to_wait = ms / 10;  // Assuming 100Hz timer
    
    while (timer_ticks - start_ticks < ticks_to_wait) {
        __asm__ __volatile__("hlt");  // Halt until next interrupt
    }
}
