// Modified keyboard.c for aggressive polling
#include "keyboard.h"
#include "../kernel/low_level.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Simple US keyboard layout
static char keyboard_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char last_key = 0;

void keyboard_init() {
    // Basic initialization
    volatile u16* video_mem = (volatile u16*)0xB8000;
    for (int i = 0; i < 12; i++) {
        video_mem[5 * 80 + i] = "KBD POLLING"[i] | (0x0F << 8);
    }
}

// Updated keyboard_poll function without visible debug output
void keyboard_poll() {
    // Check if data is available
    u8 status = port_byte_in(KEYBOARD_STATUS_PORT);
    
    // If bit 0 is set, there's data to read
    if (status & 1) {
        u8 scancode = port_byte_in(KEYBOARD_DATA_PORT);
        
        // Ignore key releases (top bit set)
        if (!(scancode & 0x80)) {
            char key = keyboard_map[scancode];
            
            if (key) {
                // Store for terminal processing
                last_key = key;
            }
        }
    }
}

// Get the last key (and clear it)
char keyboard_get_last_key() {
    char key = last_key;
    last_key = 0;
    return key;
}