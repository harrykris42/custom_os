#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H

#include "../include/types.h"

// Read a byte from an I/O port
u8 port_byte_in(u16 port);

// Write a byte to an I/O port
void port_byte_out(u16 port, u8 data);

// Read a word (2 bytes) from an I/O port
u16 port_word_in(u16 port);

// Write a word (2 bytes) to an I/O port
void port_word_out(u16 port, u16 data);

// Read a long (4 bytes) from an I/O port
u32 port_long_in(u16 port);

// Write a long (4 bytes) to an I/O port
void port_long_out(u16 port, u32 data);

#endif
