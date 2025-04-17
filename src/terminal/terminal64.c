#include "terminal64.h"
#include "../drivers/screen64.h"
#include "../drivers/keyboard.h"
#include "../include/types.h"

#define CMD_BUFFER_SIZE 256

// Command buffer
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_position = 0;

// String comparison helper
static bool terminal_str_equals(const char* str1, const char* str2) {
    int i = 0;
    
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    
    return (str1[i] == '\0' && str2[i] == '\0');
}

// Initialize the terminal
void terminal_init() {
    // Clear the screen
    screen_clear();
    
    // Display a clean welcome message
    screen_print("CustomOS 64-bit Terminal v1.0\n\n");
    screen_print("Welcome to CustomOS! Type 'help' for available commands.\n\n");
    
    // Display prompt
    screen_print("> ");
    
    // Initialize command buffer
    cmd_position = 0;
    cmd_buffer[0] = '\0';
}

// Enhanced terminal_execute_command function
static void terminal_execute_command(char* cmd) {
    // Skip leading spaces
    while (*cmd == ' ') cmd++;
    
    // Empty command
    if (*cmd == '\0') {
        return;
    }
    
    // Find command end and arguments start
    char* args = cmd;
    while (*args != ' ' && *args != '\0') args++;
    
    // If we have arguments, null-terminate the command and point to args
    bool has_args = (*args != '\0');
    if (has_args) {
        *args = '\0';  // Null-terminate the command
        args++;        // Move to arguments
        
        // Skip leading spaces in arguments
        while (*args == ' ') args++;
    } else {
        args = "";     // No arguments
    }
    
    // Process commands
    if (terminal_str_equals(cmd, "help")) {
        screen_print("\n Available Commands:\n");
        screen_print(" ┌─────────┬───────────────────────────────────┐\n");
        screen_print(" │ help    │ Display this help message         │\n");
        screen_print(" │ clear   │ Clear the screen                  │\n");
        screen_print(" │ about   │ Display information about CustomOS│\n");
        screen_print(" │ echo    │ Display the provided text         │\n");
        screen_print(" │ meminfo │ Display memory information        │\n");
        screen_print(" └─────────┴───────────────────────────────────┘\n");
    }
    else if (terminal_str_equals(cmd, "clear")) {
        // Clear screen completely
        screen_clear();
        
        // Optional: Re-display a minimal header
        screen_print("CustomOS 64-bit Terminal v1.0\n\n");
        
        // Prompt
        screen_print("> ");
    }
    else if (terminal_str_equals(cmd, "about")) {
        screen_print("\n CustomOS 64-bit v1.0\n");
        screen_print(" A simple 64-bit operating system built from scratch\n\n");
        screen_print(" Features:\n");
        screen_print(" • 64-bit long mode operation\n");
        screen_print(" • Protected memory management\n");
        screen_print(" • Keyboard input with polling\n");
        screen_print(" • Command-line interface\n");
        screen_print(" • Basic text-based shell\n\n");
    }
    else if (terminal_str_equals(cmd, "echo")) {
        screen_print(args);
        screen_print("\n");
    }
    else if (terminal_str_equals(cmd, "meminfo")) {
        screen_print("\n Memory Information:\n");
        screen_print(" ┌───────────┬────────────┐\n");
        screen_print(" │ Total RAM │ 128 MB     │\n");
        screen_print(" │ Used      │ [unknown]  │\n");
        screen_print(" │ Free      │ [unknown]  │\n");
        screen_print(" └───────────┴────────────┘\n");
    }
    else {
        screen_print("Unknown command: '");
        screen_print(cmd);
        screen_print("'\nType 'help' for a list of available commands.\n");
    }
}

// Updated terminal_process_keypress function without debug output
void terminal_process_keypress(char key) {
    // No debug output - just process the key
    
    if (key == '\n') {
        // Enter key
        screen_print("\n");
        
        // Null-terminate and execute command
        cmd_buffer[cmd_position] = '\0';
        terminal_execute_command(cmd_buffer);
        
        // Reset buffer
        cmd_position = 0;
        cmd_buffer[0] = '\0';
        
        // Show prompt
        screen_print("> ");
        
    } else if (key == '\b') {
        // Backspace
        if (cmd_position > 0) {
            cmd_position--;
            cmd_buffer[cmd_position] = '\0';
            screen_backspace();
        }
        
    } else if (key >= 32 && key <= 126) {
        // Regular printable character
        if (cmd_position < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_position] = key;
            cmd_position++;
            cmd_buffer[cmd_position] = '\0';
            screen_print_char(key);
        }
    }
}

// Updated terminal_update function without debug output
void terminal_update() {
    // Get the last key press without displaying debug info
    char key = keyboard_get_last_key();
    
    // If we got a key, process it
    if (key != 0) {
        terminal_process_keypress(key);
    }
}