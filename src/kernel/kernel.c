#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../shell/shell.h"

void main() {
    // Clear the screen
    clear_screen();
    
    // Initialize ISRs and IDT
    isr_install();
    
    // Enable interrupts
    irq_install();
    
    // Initialize keyboard
    init_keyboard();
    
    // Initialize shell
    shell_init();
}
