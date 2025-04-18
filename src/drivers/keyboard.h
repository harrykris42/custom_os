#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../include/types.h"

// Initialize the keyboard
void keyboard_init();

void keyboard_direct_test();

// Get the last pressed key (ASCII value)
char keyboard_get_last_key();

// Wait for a key press and return its ASCII value
char keyboard_wait_for_key();

void keyboard_test_direct();

void keyboard_poll();

#endif
