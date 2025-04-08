#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "../include/types.h"

// Define the structure for register state when an interrupt occurs
typedef struct {
    u64 ds;                                  // Data segment selector
    u64 rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax; // Preserved by caller
    u64 int_no, err_code;                    // Interrupt number and error code
    u64 rip, cs, rflags, user_rsp, ss;       // Pushed by processor automatically
} registers_t;

// IDT entry structure
typedef struct {
    u16 low_offset;
    u16 selector;
    u8 ist;
    u8 flags;
    u16 mid_offset;
    u32 high_offset;
    u32 reserved;
} __attribute__((packed)) idt_entry_t;

// IDT pointer structure
typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) idt_ptr_t;

// Define IRQ numbers
#define IRQ0 32   // Timer
#define IRQ1 33   // Keyboard
#define IRQ2 34   // Cascade for 8259A Slave controller
#define IRQ3 35   // Serial port 2
#define IRQ4 36   // Serial port 1
#define IRQ5 37   // Parallel port 2/3 or sound card
#define IRQ6 38   // Floppy disk controller
#define IRQ7 39   // Parallel port 1
#define IRQ8 40   // Real-time clock
#define IRQ9 41   // Legacy SCSI or NIC
#define IRQ10 42  // Available
#define IRQ11 43  // Available
#define IRQ12 44  // PS/2 mouse
#define IRQ13 45  // FPU, coprocessor or inter-processor
#define IRQ14 46  // Primary ATA channel
#define IRQ15 47  // Secondary ATA channel

// Function pointer type for interrupt handlers
typedef void (*isr_handler_t)(registers_t);

// Initialize the interrupt system
void interrupts_init();

// Register a handler for a specific interrupt
void register_interrupt_handler(u8 n, isr_handler_t handler);

#endif
