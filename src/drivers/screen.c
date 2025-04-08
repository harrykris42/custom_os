#include "screen.h"
#include "../kernel/low_level.h"
#include "../kernel/util.h"

int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char character, int col, int row, char attribute_byte);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

void clear_screen() {
    int screen_size = MAX_ROWS * MAX_COLS;
    char* screen = (char*) VIDEO_ADDRESS;
    
    for (int i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = WHITE_ON_BLACK;
    }
    
    set_cursor_offset(get_offset(0, 0));
}

void print_at(char* message, int col, int row) {
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
    
    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, WHITE_ON_BLACK);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void print(char* message) {
    print_at(message, -1, -1);
}

int print_char(char character, int col, int row, char attribute_byte) {
    unsigned char* vidmem = (unsigned char*) VIDEO_ADDRESS;
    
    if (!attribute_byte) {
        attribute_byte = WHITE_ON_BLACK;
    }
    
    int offset;
    if (col >= 0 && row >= 0) {
        offset = get_offset(col, row);
    } else {
        offset = get_cursor_offset();
    }
    
    // Special character handling
    if (character == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } else if (character == '\b') {
        // Backspace handling - move cursor back and replace with space
        vidmem[offset-2] = ' ';
        vidmem[offset-1] = attribute_byte;
        offset -= 2;
    } else {
        vidmem[offset] = character;
        vidmem[offset+1] = attribute_byte;
        offset += 2;
    }
    
    // Handle scrolling if needed
    if (offset >= MAX_ROWS * MAX_COLS * 2) {
        // Copy lines up
        for (int i = 1; i < MAX_ROWS; i++) {
            memory_copy(
                (char*)(get_offset(0, i) + VIDEO_ADDRESS),
                (char*)(get_offset(0, i-1) + VIDEO_ADDRESS),
                MAX_COLS * 2
            );
        }
        
        // Clear last line
        char* last_line = (char*)(get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS);
        for (int i = 0; i < MAX_COLS * 2; i += 2) {
            last_line[i] = ' ';
            last_line[i+1] = WHITE_ON_BLACK;
        }
        
        offset -= 2 * MAX_COLS;
    }
    
    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    // Use VGA ports to get cursor position
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    
    return offset * 2; // Convert from character offset to video memory offset
}

void set_cursor_offset(int offset) {
    offset /= 2; // Convert from video memory offset to character offset
    
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

int get_offset_row(int offset) {
    return offset / (2 * MAX_COLS);
}

int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}
