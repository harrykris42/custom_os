#ifndef SCREEN64_H
#define SCREEN64_H

#include "../include/types.h"

// Video memory constants
#define VIDEO_MEMORY       0xB8000
#define VGA_WIDTH          80
#define VGA_HEIGHT         25
#define VGA_DEFAULT_COLOR  0x0F  // White text on black background

// Text colors
#define VGA_COLOR_BLACK    0x0
#define VGA_COLOR_BLUE     0x1
#define VGA_COLOR_GREEN    0x2
#define VGA_COLOR_CYAN     0x3
#define VGA_COLOR_RED      0x4
#define VGA_COLOR_MAGENTA  0x5
#define VGA_COLOR_BROWN    0x6
#define VGA_COLOR_LGRAY    0x7
#define VGA_COLOR_DGRAY    0x8
#define VGA_COLOR_LBLUE    0x9
#define VGA_COLOR_LGREEN   0xA
#define VGA_COLOR_LCYAN    0xB
#define VGA_COLOR_LRED     0xC
#define VGA_COLOR_LMAGENTA 0xD
#define VGA_COLOR_YELLOW   0xE
#define VGA_COLOR_WHITE    0xF

// Screen I/O ports for cursor control
#define CURSOR_PORT_CMD    0x3D4
#define CURSOR_PORT_DATA   0x3D5

// Function prototypes
void screen_init();
void screen_clear();
void screen_print(const char* str);
void screen_print_at(const char* str, u16 x, u16 y);
void screen_print_char(char c);
void screen_print_char_at(char c, u16 x, u16 y, u8 color);
void screen_set_color(u8 fg, u8 bg);
void screen_set_cursor(u16 x, u16 y);
void screen_get_cursor(u16* x, u16* y);
void screen_scroll();
void screen_newline();
void screen_backspace();
void screen_put_char(char c, u16 x, u16 y, u8 color);

#endif // SCREEN64_H