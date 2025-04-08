#ifndef TYPES_H
#define TYPES_H

// Basic integer types
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

// Standard type aliases
typedef u64                size_t;
typedef s64                ssize_t;
typedef u64                uintptr_t;
typedef s64                intptr_t;

// Boolean type
typedef enum { false = 0, true = 1 } bool;

// NULL definition
#define NULL ((void*)0)

#endif // TYPES_H
