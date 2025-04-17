#include "../include/types.h"
#include "../cpu/interrupts.h"
#include "../kernel/low_level.h"

// Declare an array of interrupt handlers
isr_handler_t interrupt_handlers[256];

// Declare the IDT
idt_entry_t idt[256];
idt_ptr_t idt_ptr;

// External ISR handlers
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// External IRQ handlers
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// Set an entry in the IDT
static void idt_set_gate(u8 num, u64 handler, u16 selector, u8 flags) {
    idt[num].low_offset = handler & 0xFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].mid_offset = (handler >> 16) & 0xFFFF;
    idt[num].high_offset = (handler >> 32) & 0xFFFFFFFF;
    idt[num].reserved = 0;
}

// Common handler for all ISRs
void isr_handler(registers_t regs) {
    // Debug output
    volatile char* video_mem = (volatile char*)0xB8000 + 20 * 160; // Row 20
    const char* msg = "ISR called: ";
    int i = 0;
    while (msg[i]) {
        video_mem[i*2] = msg[i];
        video_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Convert the interrupt number to a string
    char num_str[3] = "00";
    if (regs.int_no < 10) {
        num_str[0] = '0';
        num_str[1] = '0' + regs.int_no;
    } else {
        num_str[0] = '0' + (regs.int_no / 10);
        num_str[1] = '0' + (regs.int_no % 10);
    }
    
    // Display the interrupt number
    video_mem[i*2] = num_str[0];
    video_mem[i*2+1] = 0x0F;
    video_mem[(i+1)*2] = num_str[1];
    video_mem[(i+1)*2+1] = 0x0F;
    
    // Call the interrupt handler if one exists
    if (interrupt_handlers[regs.int_no]) {
        interrupt_handlers[regs.int_no](regs);
    }
}

// Common handler for all IRQs
// In interrupts.c or wherever your IRQ handler is defined
void irq_handler(registers_t regs) {
    // Don't print any debug messages
    
    // Send EOI (End of Interrupt) to the PICs
    if (regs.int_no >= 40) {
        port_byte_out(0xA0, 0x20);  // Send EOI to slave PIC
    }
    port_byte_out(0x20, 0x20);      // Send EOI to master PIC
    
    // Call handler if one exists
    if (interrupt_handlers[regs.int_no]) {
        interrupt_handlers[regs.int_no](regs);
    }
}

// Register an interrupt handler
void register_interrupt_handler(u8 n, isr_handler_t handler) {
    interrupt_handlers[n] = handler;
}

// Initialize the interrupt system
void interrupts_init() {
    // Set up the IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (u64)&idt;
    
    // Clear the IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
        interrupt_handlers[i] = 0;  // Clear all handlers
    }
    
    // Set up IDT entries for exceptions (ISRs)
    idt_set_gate(0, (u64)isr0, 0x08, 0x8E);
    idt_set_gate(1, (u64)isr1, 0x08, 0x8E);
    idt_set_gate(2, (u64)isr2, 0x08, 0x8E);
    idt_set_gate(3, (u64)isr3, 0x08, 0x8E);
    idt_set_gate(4, (u64)isr4, 0x08, 0x8E);
    idt_set_gate(5, (u64)isr5, 0x08, 0x8E);
    idt_set_gate(6, (u64)isr6, 0x08, 0x8E);
    idt_set_gate(7, (u64)isr7, 0x08, 0x8E);
    idt_set_gate(8, (u64)isr8, 0x08, 0x8E);
    idt_set_gate(9, (u64)isr9, 0x08, 0x8E);
    idt_set_gate(10, (u64)isr10, 0x08, 0x8E);
    idt_set_gate(11, (u64)isr11, 0x08, 0x8E);
    idt_set_gate(12, (u64)isr12, 0x08, 0x8E);
    idt_set_gate(13, (u64)isr13, 0x08, 0x8E);
    idt_set_gate(14, (u64)isr14, 0x08, 0x8E);
    idt_set_gate(15, (u64)isr15, 0x08, 0x8E);
    idt_set_gate(16, (u64)isr16, 0x08, 0x8E);
    idt_set_gate(17, (u64)isr17, 0x08, 0x8E);
    idt_set_gate(18, (u64)isr18, 0x08, 0x8E);
    idt_set_gate(19, (u64)isr19, 0x08, 0x8E);
    idt_set_gate(20, (u64)isr20, 0x08, 0x8E);
    idt_set_gate(21, (u64)isr21, 0x08, 0x8E);
    idt_set_gate(22, (u64)isr22, 0x08, 0x8E);
    idt_set_gate(23, (u64)isr23, 0x08, 0x8E);
    idt_set_gate(24, (u64)isr24, 0x08, 0x8E);
    idt_set_gate(25, (u64)isr25, 0x08, 0x8E);
    idt_set_gate(26, (u64)isr26, 0x08, 0x8E);
    idt_set_gate(27, (u64)isr27, 0x08, 0x8E);
    idt_set_gate(28, (u64)isr28, 0x08, 0x8E);
    idt_set_gate(29, (u64)isr29, 0x08, 0x8E);
    idt_set_gate(30, (u64)isr30, 0x08, 0x8E);
    idt_set_gate(31, (u64)isr31, 0x08, 0x8E);
    
    // Set up IDT entries for hardware interrupts (IRQs)
    idt_set_gate(32, (u64)irq0, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (u64)irq1, 0x08, 0x8E);  // Keyboard
    idt_set_gate(34, (u64)irq2, 0x08, 0x8E);
    idt_set_gate(35, (u64)irq3, 0x08, 0x8E);
    idt_set_gate(36, (u64)irq4, 0x08, 0x8E);
    idt_set_gate(37, (u64)irq5, 0x08, 0x8E);
    idt_set_gate(38, (u64)irq6, 0x08, 0x8E);
    idt_set_gate(39, (u64)irq7, 0x08, 0x8E);
    idt_set_gate(40, (u64)irq8, 0x08, 0x8E);
    idt_set_gate(41, (u64)irq9, 0x08, 0x8E);
    idt_set_gate(42, (u64)irq10, 0x08, 0x8E);
    idt_set_gate(43, (u64)irq11, 0x08, 0x8E);
    idt_set_gate(44, (u64)irq12, 0x08, 0x8E);
    idt_set_gate(45, (u64)irq13, 0x08, 0x8E);
    idt_set_gate(46, (u64)irq14, 0x08, 0x8E);
    idt_set_gate(47, (u64)irq15, 0x08, 0x8E);
    
    // Remap the PIC
    // Initialize the PICs
    port_byte_out(0x20, 0x11);  // Initialize master PIC
    port_byte_out(0xA0, 0x11);  // Initialize slave PIC
    
    // Set vector offsets
    port_byte_out(0x21, 0x20);  // Master PIC starts at 32
    port_byte_out(0xA1, 0x28);  // Slave PIC starts at 40
    
    // Tell PICs their relationship
    port_byte_out(0x21, 0x04);  // Tell master PIC that slave is at IRQ2
    port_byte_out(0xA1, 0x02);  // Tell slave PIC its cascade identity
    
    // Set operation mode
    port_byte_out(0x21, 0x01);  // 8086 mode for master
    port_byte_out(0xA1, 0x01);  // 8086 mode for slave
    
    // Unmask all interrupts
    port_byte_out(0x21, 0x00);  // Enable all on master
    port_byte_out(0xA1, 0x00);  // Enable all on slave
    // Unmask the keyboard interrupt
    port_byte_out(0x21, port_byte_in(0x21) & ~(1 << 1));  // Unmask IRQ1 (keyboard)
    
    // Load the IDT
    __asm__ __volatile__("lidt %0" : : "m"(idt_ptr));
    
    // Enable interrupts
    __asm__ __volatile__("sti");
    
    // Debug output to confirm initialization
    volatile char* video_mem = (volatile char*)0xB8000 + 15 * 160;
    const char* msg = "Interrupts enabled. IDT loaded at: ";
    int i = 0;
    while (msg[i]) {
        video_mem[i*2] = msg[i];
        video_mem[i*2+1] = 0x0F;
        i++;
    }
    
    // Display IDT address
    char addr_str[17];
    u64 addr = (u64)&idt;
    for (int j = 0; j < 16; j++) {
        int digit = (addr >> (60 - j*4)) & 0xF;
        addr_str[j] = digit < 10 ? '0' + digit : 'A' + (digit - 10);
    }
    addr_str[16] = '\0';
    
    for (int j = 0; j < 16; j++) {
        video_mem[(i+j)*2] = addr_str[j];
        video_mem[(i+j)*2+1] = 0x0F;
    }
}
