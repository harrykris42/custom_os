#ifndef TIMER_H
#define TIMER_H

#include "../include/types.h"

// Initialize the timer with a specific frequency in Hz
void timer_init(u32 frequency);

// Get the number of ticks since the system started
u64 timer_get_ticks();

// Sleep for a specified number of milliseconds
void timer_sleep(u32 ms);

#endif
