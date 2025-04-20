#include "terminal64.h"
#include "../drivers/screen64.h"
#include "../drivers/keyboard.h"
#include "../include/types.h"

#define CMD_BUFFER_SIZE 256
#define CMD_HISTORY_SIZE 10  // Store up to 10 commands in history

// Command buffer
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_position = 0;
static bool prompt_shown = false;

// Command history
static char cmd_history[CMD_HISTORY_SIZE][CMD_BUFFER_SIZE];
static int history_count = 0;          // Number of commands in history

// Basic string comparison (simplified)
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

// Add a command to history
static void add_to_history(const char* cmd) {
    // Don't add empty commands
    if (cmd[0] == '\0') {
        return;
    }
    
    // Don't add duplicate of the most recent command
    if (history_count > 0 && terminal_str_equals(cmd_history[history_count - 1], cmd)) {
        return;
    }
    
    // If history is full, shift everything down
    if (history_count == CMD_HISTORY_SIZE) {
        for (int i = 0; i < CMD_HISTORY_SIZE - 1; i++) {
            int j;
            for (j = 0; j < CMD_BUFFER_SIZE; j++) {
                cmd_history[i][j] = cmd_history[i + 1][j];
                if (cmd_history[i][j] == '\0') break;
            }
            cmd_history[i][j] = '\0';
        }
        history_count--;
    }
    
    // Add the new command to history
    int i;
    for (i = 0; cmd[i] != '\0' && i < CMD_BUFFER_SIZE - 1; i++) {
        cmd_history[history_count][i] = cmd[i];
    }
    cmd_history[history_count][i] = '\0';
    
    history_count++;
}

// Initialize the terminal
void terminal_init() {
    // Clear the screen
    screen_clear();
    
    // Display welcome message
    screen_print("CustomOS 64-bit Terminal v1.0\n\n");
    screen_print("Welcome to CustomOS! Type 'help' for available commands.\n\n");
    
    // Display prompt
    screen_print("> ");
    prompt_shown = true;
    
    // Initialize command buffer
    cmd_position = 0;
    cmd_buffer[0] = '\0';
    
    // Initialize history
    history_count = 0;
}

// Execute a Command
static void terminal_execute_command(char* cmd) {
    // Skip leading spaces
    while (*cmd == ' ') cmd++;
    
    // Add to history first (before we potentially modify cmd)
    add_to_history(cmd);
    
    // Empty command
    if (*cmd == '\0') {
        // Show the prompt for the next command
        screen_print("> ");
        prompt_shown = true;
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
        screen_print("Available Commands:\n");
        screen_print("help    - Display this help message\n");
        screen_print("clear   - Clear the screen\n");
        screen_print("about   - Display information about CustomOS\n");
        screen_print("echo    - Display the provided text\n");
        screen_print("history - Show command history\n");
    }
    else if (terminal_str_equals(cmd, "clear")) {
        // Clear screen completely
        screen_clear();
        
        // Optional: Re-display a minimal header
        screen_print("CustomOS 64-bit Terminal v1.0\n\n");
    }
    else if (terminal_str_equals(cmd, "about")) {
        screen_print("CustomOS 64-bit v1.0\n");
        screen_print("A simple 64-bit operating system built from scratch\n\n");
        screen_print("Features:\n");
        screen_print("- 64-bit long mode operation\n");
        screen_print("- Protected memory management\n");
        screen_print("- Keyboard input with shift support\n");
        screen_print("- Command history\n");
        screen_print("- Command-line interface\n");
        screen_print("- Basic text-based shell\n");
    }
    else if (terminal_str_equals(cmd, "echo")) {
        screen_print(args);
        screen_print("\n");
    }
    else if (terminal_str_equals(cmd, "history")) {
        // Display command history
        if (history_count == 0) {
            screen_print("No commands in history.\n");
        } else {
            for (int i = 0; i < history_count; i++) {
                // Convert index to string (simple conversion for small numbers)
                char index_str[4] = {'0' + i / 10, '0' + i % 10, '.', ' '};
                if (i < 10) { // Only single digit
                    index_str[0] = ' ';
                    index_str[1] = '0' + i;
                }
                
                screen_print(index_str);
                screen_print(cmd_history[i]);
                screen_print("\n");
            }
        }
    }
    else {
        screen_print("Unknown command: ");
        screen_print(cmd);
        screen_print("\nType 'help' for a list of commands.\n");
    }
    
    // Always show the prompt after executing a command
    screen_print("> ");
    prompt_shown = true;
}

// Process a keypress
void terminal_process_keypress(char key) {
    if (key == '\n') {
        // Enter key
        screen_print("\n");
        
        // Null-terminate and execute command
        cmd_buffer[cmd_position] = '\0';
        prompt_shown = false;  // Clear prompt flag before executing command
        terminal_execute_command(cmd_buffer);
        
        // Reset buffer
        cmd_position = 0;
        cmd_buffer[0] = '\0';
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

// Update the terminal
void terminal_update() {
    // If prompt needs to be shown, show it
    if (!prompt_shown) {
        screen_print("> ");
        prompt_shown = true;
    }
    
    // Get the last key press
    char key = keyboard_get_last_key();
    
    // If we got a key, process it
    if (key != 0) {
        terminal_process_keypress(key);
    }
}