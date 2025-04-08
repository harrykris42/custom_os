#include "../include/types.h"
#include "low_level.h"

// Read a byte from an I/O port
u8 port_byte_in(u16 port) {
    u8 result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a byte to an I/O port
void port_byte_out(u16 port, u8 data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Read a word (2 bytes) from an I/O port
u16 port_word_in(u16 port) {
    u16 result;
    __asm__ __volatile__("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a word (2 bytes) to an I/O port
void port_word_out(u16 port, u16 data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}

// Read a long (4 bytes) from an I/O port
u32 port_long_in(u16 port) {
    u32 result;
    __asm__ __volatile__("inl %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a long (4 bytes) to an I/O port
void port_long_out(u16 port, u32 data) {
    __asm__ __volatile__("outl %0, %1" : : "a"(data), "Nd"(port));
}
