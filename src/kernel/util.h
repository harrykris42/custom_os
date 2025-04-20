#ifndef UTIL_H
#define UTIL_H

#include "../include/types.h"

// Memory operations
void memory_copy(char* source, char* dest, int nbytes);
void memory_set(char* dest, char val, int nbytes);

// String operations
int str_length(char s[]);
void reverse(char s[]);
void int_to_str(int n, char str[]);
void uint64_to_str(u64 n, char str[]);

#endif