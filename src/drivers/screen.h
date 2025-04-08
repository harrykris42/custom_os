#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

// Screen device I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void clear_screen();
void print_at(char* message, int col, int row);
void print(char* message);
int print_char(char character, int col, int row, char attribute_byte);
int get_cursor_offset();
void set_cursor_offset(int offset);

#endif
