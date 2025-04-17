#include "screen64.h"

// Direct VGA memory access
static volatile u16* const video_memory = (volatile u16*)0xB8000;

// Current cursor position
static u16 cursor_x = 0;
static u16 cursor_y = 0;
static u8 current_color = VGA_DEFAULT_COLOR;

// Clear the screen with a specific character and color
void screen_clear() {
    volatile u16* video_memory = (volatile u16*)VIDEO_MEMORY;
    
    // Clear the entire screen including any debug areas
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = ' ' | (current_color << 8);
    }
    
    cursor_x = 0;
    cursor_y = 0;
    screen_set_cursor(cursor_x, cursor_y);
}

// Print a character at a specific location
void screen_put_char(char c, u16 x, u16 y, u8 color) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        video_memory[y * VGA_WIDTH + x] = c | (color << 8);
    }
}

// Print a string at the current cursor position
void screen_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            cursor_y++;
            cursor_x = 0;
        } else {
            screen_put_char(*str, cursor_x, cursor_y, VGA_DEFAULT_COLOR);
            cursor_x++;
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        
        // Handle scrolling if needed
        if (cursor_y >= VGA_HEIGHT) {
            // Just reset to top for now
            cursor_y = 0;
        }
        
        str++;
    }
}

// Initialize the screen
void screen_init() {
    screen_clear();
    
    // Display test message
    const char* test = "Screen driver initialized";
    for (int i = 0; test[i]; i++) {
        screen_put_char(test[i], i, 0, VGA_DEFAULT_COLOR);
    }
}

// Other screen functions can be simplified or removed for now
void screen_print_at(const char* str, u16 x, u16 y) {
    cursor_x = x;
    cursor_y = y;
    screen_print(str);
}

void screen_print_char(char c) {
    if (c == '\n') {
        cursor_y++;
        cursor_x = 0;
    } else {
        screen_put_char(c, cursor_x, cursor_y, VGA_DEFAULT_COLOR);
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    // Handle scrolling if needed
    if (cursor_y >= VGA_HEIGHT) {
        cursor_y = 0; // Just wrap for now
    }
}

void screen_backspace() {
    if (cursor_x > 0) {
        cursor_x--;
        screen_put_char(' ', cursor_x, cursor_y, VGA_DEFAULT_COLOR);
    }
}

void screen_set_cursor(u16 x, u16 y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        cursor_x = x;
        cursor_y = y;
    }
}

void screen_get_cursor(u16* x, u16* y) {
    *x = cursor_x;
    *y = cursor_y;
}

// We'll skip hardware cursor updates for now to simplify