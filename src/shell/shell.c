#include "shell.h"
#include "../drivers/screen.h"
#include "../kernel/util.h"

#define CMD_BUFFER_SIZE 256
static char cmd_buffer[CMD_BUFFER_SIZE];
static int cmd_buffer_pos = 0;

// Command handlers
static void cmd_help();
static void cmd_clear();
static void cmd_about();
static void cmd_echo(char* args);

void shell_init() {
    // Print welcome message
    print("=======================================================\n");
    print("                  CustomOS Shell v0.1                  \n");
    print("=======================================================\n");
    print("Type 'help' for a list of available commands\n");
    print("> ");
    
    // Initialize command buffer
    cmd_buffer_pos = 0;
}

void shell_process_key(char key) {
    if (key == '\n') {
        // Enter key pressed - process command
        print("\n");
        
        // Null-terminate the command
        cmd_buffer[cmd_buffer_pos] = '\0';
        
        // Process the command
        shell_process_command(cmd_buffer);
        
        // Reset buffer for next command
        cmd_buffer_pos = 0;
        
        // Display prompt for next command
        print("> ");
    } else if (key == '\b') {
        // Backspace key pressed
        if (cmd_buffer_pos > 0) {
            cmd_buffer_pos--;
            print("\b \b");  // Erase the character
        }
    } else {
        // Regular key pressed
        if (cmd_buffer_pos < CMD_BUFFER_SIZE - 1) {
            // Add to buffer
            cmd_buffer[cmd_buffer_pos++] = key;
            
            // Echo the character
            char str[2] = {key, '\0'};
            print(str);
        }
    }
}

// String comparison helper function
static int str_eq(char* str1, char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0;
        }
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0');
}

void shell_process_command(char* input) {
    // Skip leading spaces
    while (*input == ' ') input++;
    
    // Find the end of the command
    char* args = input;
    while (*args != ' ' && *args != '\0') args++;
    
    // Split command and arguments
    int has_args = (*args != '\0');
    if (has_args) {
        *args = '\0';  // Terminate the command
        args++;        // Move to the arguments
        
        // Skip leading spaces in arguments
        while (*args == ' ') args++;
    } else {
        args = "";     // No arguments
    }
    
    // Process command
    if (str_eq(input, "help")) {
        cmd_help();
    } else if (str_eq(input, "clear")) {
        cmd_clear();
    } else if (str_eq(input, "about")) {
        cmd_about();
    } else if (str_eq(input, "echo")) {
        cmd_echo(args);
    } else if (*input == '\0') {
        // Empty command, just display a new prompt
    } else {
        print("Unknown command: ");
        print(input);
        print("\nType 'help' for a list of commands\n");
    }
}

// Command implementations
static void cmd_help() {
    print("Available commands:\n");
    print("  help   - Display this help message\n");
    print("  clear  - Clear the screen\n");
    print("  about  - Display information about CustomOS\n");
    print("  echo   - Display the arguments\n");
}

static void cmd_clear() {
    clear_screen();
}

static void cmd_about() {
    print("CustomOS v0.1\n");
    print("A simple operating system built from scratch\n");
    print("Features:\n");
    print("- 32-bit protected mode\n");
    print("- Custom bootloader\n");
    print("- Interrupt handling\n");
    print("- Keyboard driver\n");
    print("- Simple command shell\n");
}

static void cmd_echo(char* args) {
    print(args);
    print("\n");
}
