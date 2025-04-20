#include "screen64.h"
#include "../kernel/low_level.h"

// Direct VGA memory access - standard location
static volatile u16* const video_memory = (volatile u16*)0xB8000;

// Current cursor position
static u16 cursor_x = 0;
static u16 cursor_y = 0;
static u8 current_color = 0x07; // Light gray on black - basic, safe color

// Set the hardware cursor position
static void update_cursor() {
    u16 pos = cursor_y * VGA_WIDTH + cursor_x;
    
    // Tell the VGA controller we're setting the cursor position
    port_byte_out(0x3D4, 0x0F);  // Low byte index
    port_byte_out(0x3D5, (u8)(pos & 0xFF));
    
    port_byte_out(0x3D4, 0x0E);  // High byte index
    port_byte_out(0x3D5, (u8)((pos >> 8) & 0xFF));
}

// Clear the screen with basic color
void screen_clear() {
    // Clear the entire screen with space characters
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = ' ' | (0x07 << 8); // Light gray on black
    }
    
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

// Print a character at a specific location - basic version
void screen_put_char(char c, u16 x, u16 y, u8 color) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        video_memory[y * VGA_WIDTH + x] = c | (color << 8);
    }
}

// Print a string at the current cursor position
void screen_print(const char* str) {
    if (!str) return;
    
    while (*str) {
        if (*str == '\n') {
            cursor_y++;
            cursor_x = 0;
        } else {
            screen_put_char(*str, cursor_x, cursor_y, current_color);
            cursor_x++;
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        
        // Handle scrolling if needed
        if (cursor_y >= VGA_HEIGHT) {
            // Move lines up
            for (int y = 0; y < VGA_HEIGHT - 1; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    video_memory[y * VGA_WIDTH + x] = video_memory[(y + 1) * VGA_WIDTH + x];
                }
            }
            
            // Clear the bottom line
            for (int x = 0; x < VGA_WIDTH; x++) {
                video_memory[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | (current_color << 8);
            }
            
            cursor_y = VGA_HEIGHT - 1;
        }
        
        str++;
    }
    
    // Update hardware cursor
    update_cursor();
}

// Initialize the screen - simple initialization
void screen_init() {
    // Set basic color
    current_color = 0x07; // Light gray on black
    
    // Clear screen
    screen_clear();
    
    // Initialize cursor position
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

// Print a string at a specific position
void screen_print_at(const char* str, u16 x, u16 y) {
    cursor_x = x;
    cursor_y = y;
    screen_print(str);
}

// Print a single character
void screen_print_char(char c) {
    if (c == '\n') {
        cursor_y++;
        cursor_x = 0;
    } else {
        screen_put_char(c, cursor_x, cursor_y, current_color);
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    // Handle scrolling if needed
    if (cursor_y >= VGA_HEIGHT) {
        // Move lines up
        for (int y = 0; y < VGA_HEIGHT - 1; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                video_memory[y * VGA_WIDTH + x] = video_memory[(y + 1) * VGA_WIDTH + x];
            }
        }
        
        // Clear the bottom line
        for (int x = 0; x < VGA_WIDTH; x++) {
            video_memory[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | (current_color << 8);
        }
        
        cursor_y = VGA_HEIGHT - 1;
    }
    
    update_cursor();
}

// Handle backspace
void screen_backspace() {
    if (cursor_x > 0) {
        cursor_x--;
        screen_put_char(' ', cursor_x, cursor_y, current_color);
        update_cursor();
    }
}

// Set cursor position
void screen_set_cursor(u16 x, u16 y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        cursor_x = x;
        cursor_y = y;
        update_cursor();
    }
}

// Get cursor position
void screen_get_cursor(u16* x, u16* y) {
    *x = cursor_x;
    *y = cursor_y;
}

// Set the text color - simplified
void screen_set_color(u8 bg, u8 fg) {
    // Make sure we're using valid color values (0-15)
    bg &= 0x07; // Use only the first 3 bits for background (0-7)
    fg &= 0x0F; // Use only the first 4 bits for foreground (0-15)
    current_color = (bg << 4) | fg;
}