#include "../include/types.h"
#include "../drivers/timer.h"
#include "../kernel/low_level.h"
#include "../cpu/interrupts.h"

// The PIT uses a crystal oscillator running at 1.193182 MHz
#define PIT_FREQUENCY 1193182

// Timer tick counter
static volatile u64 timer_ticks = 0;

// Timer interrupt handler
static void timer_callback(registers_t regs) {
    timer_ticks++;
    
    // Display ticks for debug purposes
    volatile char* video_mem = (volatile char*)0xB8000 + 11 * 160 + 20 * 2; // Row 11, Col 20
    
    // Convert ticks to a string
    char ticks_str[10];
    u64 temp = timer_ticks;
    int i = 0;
    
    // Handle 0 case
    if (temp == 0) {
        ticks_str[0] = '0';
        i = 1;
    } else {
        // Convert to string (reverse order)
        while (temp > 0) {
            ticks_str[i++] = '0' + (temp % 10);
            temp /= 10;
        }
    }
    
    // Null terminate
    ticks_str[i] = '\0';
    
    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = ticks_str[j];
        ticks_str[j] = ticks_str[i - j - 1];
        ticks_str[i - j - 1] = temp;
    }
    
    // Display the ticks
    for (int j = 0; j < i; j++) {
        video_mem[j*2] = ticks_str[j];
        video_mem[j*2+1] = 0x0F;
    }
    
    // Clear any remaining characters
    for (int j = i; j < 10; j++) {
        video_mem[j*2] = ' ';
        video_mem[j*2+1] = 0x0F;
    }
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
