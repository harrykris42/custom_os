#ifndef TERMINAL64_H
#define TERMINAL64_H

#include "../include/types.h"

// Initialize the terminal
void terminal_init();

// Process a single keypress
void terminal_process_keypress(char key);

// Main terminal update function - call regularly
void terminal_update();

#endif // TERMINAL64_H