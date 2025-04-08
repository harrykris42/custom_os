#include "../include/types.h"
#include "../drivers/keyboard.h"
#include "../cpu/interrupts.h"
#include "../kernel/low_level.h"

// Keyboard port
#define KEYBOARD_DATA_PORT 0x60

// Last key pressed
static volatile char last_key = 0;

// US QWERTY keyboard layout
static char keyboard_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Keyboard interrupt handler
static void keyboard_callback(registers_t regs) {
    // Read the scancode
    u8 scancode = port_byte_in(KEYBOARD_DATA_PORT);
    
    // Display scancode for debugging
    volatile char* video_mem = (volatile char*)0xB8000 + 22 * 160; // Row 22
    const char* msg = "Key scancode: ";
    int i = 0;
    while (msg[i]) {
        video_mem[i*2] = msg[i];
        video_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Convert scancode to string
    char code_str[4];
    if (scancode < 10) {
        code_str[0] = '0';
        code_str[1] = '0';
        code_str[2] = '0' + scancode;
    } else if (scancode < 100) {
        code_str[0] = '0';
        code_str[1] = '0' + (scancode / 10);
        code_str[2] = '0' + (scancode % 10);
    } else {
        code_str[0] = '0' + (scancode / 100);
        code_str[1] = '0' + ((scancode / 10) % 10);
        code_str[2] = '0' + (scancode % 10);
    }
    code_str[3] = '\0';
    
    // Display scancode
    for (int j = 0; j < 3; j++) {
        video_mem[(i+j)*2] = code_str[j];
        video_mem[(i+j)*2+1] = 0x0F;
    }
    
    // If it's a key release, ignore it
    if (scancode & 0x80) return;
    
    // Map the scancode to ASCII
    last_key = keyboard_map[scancode];
    
    // Display the character
    if (last_key) {
        video_mem = (volatile char*)0xB8000 + 23 * 160; // Row 23
        const char* key_msg = "Key pressed: ";
        i = 0;
        while (key_msg[i]) {
            video_mem[i*2] = key_msg[i];
            video_mem[i*2+1] = 0x0F;
            i++;
        }
        video_mem[i*2] = last_key;
        video_mem[i*2+1] = 0x0F;
    }
}

// Initialize the keyboard
void keyboard_init() {
    // Register the keyboard handler
    register_interrupt_handler(33, keyboard_callback);
}

// Get the last pressed key
char keyboard_get_last_key() {
    return last_key;
}

// Wait for a key press and return its ASCII value
char keyboard_wait_for_key() {
    last_key = 0;
    while (!last_key) {
        __asm__ __volatile__("hlt");  // Halt until next interrupt
    }
    return last_key;
}
