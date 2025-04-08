#ifndef SHELL_H
#define SHELL_H

// Initialize the shell
void shell_init();

// Process a command string
void shell_process_command(char* command);

// Process a single keypress
void shell_process_key(char key);

#endif
