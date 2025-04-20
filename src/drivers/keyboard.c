#include "keyboard.h"
#include "../kernel/low_level.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Scan code constants
#define SCAN_LEFT_SHIFT 0x2A
#define SCAN_RIGHT_SHIFT 0x36
#define SCAN_LEFT_SHIFT_RELEASE 0xAA
#define SCAN_RIGHT_SHIFT_RELEASE 0xB6
#define SCAN_CAPS_LOCK 0x3A

// Special key codes for arrow keys
#define SPECIAL_LEFT_ARROW 0x4B
#define SPECIAL_RIGHT_ARROW 0x4D

// US keyboard layout - regular (unshifted) keys
static char keyboard_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// US keyboard layout - shifted keys
static char keyboard_map_shifted[128] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char last_key = 0;
static bool shift_pressed = false;
static bool caps_lock_on = false;
static bool extended_key_mode = false;  // Flag for extended key sequences

void keyboard_init() {
    // Initialize state
    last_key = 0;
    shift_pressed = false;
    caps_lock_on = false;
    extended_key_mode = false;
    
    // Flush the keyboard buffer
    u8 status = port_byte_in(KEYBOARD_STATUS_PORT);
    while (status & 1) {
        port_byte_in(KEYBOARD_DATA_PORT);
        status = port_byte_in(KEYBOARD_STATUS_PORT);
    }
}

// Determines if a key is a letter (a-z, A-Z)
static bool is_letter(u8 scancode) {
    return (scancode >= 0x10 && scancode <= 0x19) ||  // Q-P row
           (scancode >= 0x1E && scancode <= 0x26) ||  // A-L row
           (scancode >= 0x2C && scancode <= 0x32);    // Z-M row
}

// Updated keyboard_poll function with arrow key support
void keyboard_poll() {
    // Check if data is available
    u8 status = port_byte_in(KEYBOARD_STATUS_PORT);
    
    // If bit 0 is set, there's data to read
    if (status & 1) {
        u8 scancode = port_byte_in(KEYBOARD_DATA_PORT);

        // Check for extended key sequence (E0 prefix)
        if (scancode == 0xE0) {
            extended_key_mode = true;
            return;
        }
        
        // If we're in extended key mode, handle extended keys
        if (extended_key_mode) {
            extended_key_mode = false;  // Reset extended key mode
            
            // Only process key presses (not releases)
            if (!(scancode & 0x80)) {
                // Check for arrow keys
                if (scancode == 0x4B) {  // Left arrow key
                    last_key = SPECIAL_LEFT_ARROW;
                    return;
                }
                else if (scancode == 0x4D) {  // Right arrow key
                    last_key = SPECIAL_RIGHT_ARROW;
                    return;
                }
                // Ignore other extended keys for now
            }
            return;
        }
        
        // Handle regular keys
        if (scancode == SCAN_LEFT_SHIFT || scancode == SCAN_RIGHT_SHIFT) {
            shift_pressed = true;
        } 
        else if (scancode == SCAN_LEFT_SHIFT_RELEASE || scancode == SCAN_RIGHT_SHIFT_RELEASE) {
            shift_pressed = false;
        }
        else if (scancode == SCAN_CAPS_LOCK) {
            caps_lock_on = !caps_lock_on;
        }
        else if (!(scancode & 0x80)) {  // Key press event (not release)
            // Get the appropriate character
            char key = 0;
            
            if (scancode < 128) {
                if (is_letter(scancode)) {
                    // For letters, shift XOR caps lock determines case
                    if (shift_pressed ^ caps_lock_on) {
                        key = keyboard_map_shifted[scancode];
                    } else {
                        key = keyboard_map[scancode];
                    }
                } else {
                    // Non-letter keys only respond to shift
                    if (shift_pressed) {
                        key = keyboard_map_shifted[scancode];
                    } else {
                        key = keyboard_map[scancode];
                    }
                }
                
                if (key) {
                    last_key = key;
                }
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

// Return current shift state
bool keyboard_is_shift_pressed() {
    return shift_pressed;
}

// Return current caps lock state
bool keyboard_is_caps_lock_on() {
    return caps_lock_on;
}

// Check if key is left arrow
bool keyboard_is_left_arrow(char key) {
    return key == SPECIAL_LEFT_ARROW;
}

// Check if key is right arrow
bool keyboard_is_right_arrow(char key) {
    return key == SPECIAL_RIGHT_ARROW;
}