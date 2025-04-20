#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../include/types.h"

// Initialize the keyboard
void keyboard_init();

// Poll keyboard for input
void keyboard_poll();

// Get the last pressed key (ASCII value)
char keyboard_get_last_key();

// Return current shift state
bool keyboard_is_shift_pressed();

// Return current caps lock state
bool keyboard_is_caps_lock_on();

// Check if key is left arrow
bool keyboard_is_left_arrow(char key);

// Check if key is right arrow
bool keyboard_is_right_arrow(char key);

#endif