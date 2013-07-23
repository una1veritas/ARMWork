/*
===============================================================================
 File small_printf.h: Header file for small printf by Georges Menie and Christian Ettinger
 Project DebugTips_1343: Demonstration of debugging techniques for LPCXpresso
 Copyright (C) 2010 NXP Semiconductors
===============================================================================
*/

#ifndef SMALLPRINTF_H_INCLUDED
#define SMALLPRINTF_H_INCLUDED
#include <stdarg.h>

typedef int (*fp_printf_write_func)(char c);


extern int func_printf_nofloat(const fp_printf_write_func printf_write, const char *format, ...)
	__attribute__ ((format (printf, 2, 3)));

extern int printf_format_nofloat(const fp_printf_write_func printf_write, const char *format, va_list varg);

// We don't implement sprintf because of the risk of buffer overruns
extern int snprintf_nofloat(char *buffer, int buffer_length, const char *format, ...)
    __attribute__ ((format (printf, 3, 4)));

extern int func_printf_float(const fp_printf_write_func printf_write, const char *format, ...)
	__attribute__ ((format (printf, 2, 3)));

extern int printf_format_float(const fp_printf_write_func printf_write, const char *format, va_list varg);

// We don't implement sprintf because of the risk of buffer overruns
extern int snprintf_float(char *buffer, int buffer_length, const char *format, ...)
    __attribute__ ((format (printf, 3, 4)));

// Internal functions defined in small_printf_support.c
// Used by small_printf.c and small_printf_float.c which should be identical except for
// #define enabling and disabling float at the top
int prints(const fp_printf_write_func printf_write, const char *string, int width, int pad);
int printi(const fp_printf_write_func printf_write, int i, int b, int sg, int width, int pad, int letbase);
void nsprintf_write_init();
int nsprintf_write(char c);

#ifdef LIB_FLOAT_PRINTF
#define func_printf(...)	( func_printf_float(__VA_ARGS__))
#define printf_format(...) 	( printf_format_float(__VA_ARGS__))
#define snprintf(...) 		( snprintf_float(__VA_ARGS__))
#else
#define func_printf(...) 	( func_printf_nofloat(__VA_ARGS__))
#define printf_format(...) 	( printf_format_nofloat(__VA_ARGS__))
#define snprintf(...) 		( snprintf_nofloat(__VA_ARGS__))
#endif

#endif
