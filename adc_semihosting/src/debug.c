/***********************************************************************
 * $Id::                                                               $
 *
 * File:	debug.c: Debug routines for LPCXpresso projects
 * Description:
 * 			Uses LGPLv2 small printf by Georges Menie and Christian Ettinger
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#include <stdarg.h>
#include <stdint.h>
#include "debug.h"
#include "small_printf.h"
#include "small_utils.h"
#include "LPC17xx.h"

#ifdef DEBUG
// Defining this symbol enables semihosting in the Code Red debugger
extern void __CR_SEMIHOST () __attribute__ ((weak, alias
("_debug_printf_flush")));

static char debug_write_buf[DEBUG_OUTPUT_BUFFER_SIZE];
static uint8_t debug_buf_read_index, debug_buf_write_index;

int _debug_printf_flush()
{
	uint8_t len, written;

	len = debug_buf_read_index <= debug_buf_write_index
			? debug_buf_write_index - debug_buf_read_index
			: DEBUG_OUTPUT_BUFFER_SIZE - debug_buf_read_index;

	if(!len)
		return 0;

	// The following if() disables semihosted writes when there is no hosted debugger
	// Otherwise, the target will halt when the semihost __write is called
	if(ISDEBUGACTIVE())
	    written = __write(0, &debug_write_buf[debug_buf_read_index], len);
	else
            written = 0;

	debug_buf_read_index = (debug_buf_read_index + written) % DEBUG_OUTPUT_BUFFER_SIZE;

	if(written != len)
		return written;
	return _debug_printf_flush() + written;
}

int _debug_putchar(char c)
{
	uint8_t buffer_has_room = 1;

	if((debug_buf_write_index+1)%DEBUG_OUTPUT_BUFFER_SIZE  == debug_buf_read_index
#if defined(FLUSH_ON_NL)
                || c == '\n')
#else
	        )
#endif
	{
            buffer_has_room = _debug_printf_flush();
	}

	if(buffer_has_room)
	{
	    debug_write_buf[debug_buf_write_index] = c;
	    debug_buf_write_index =
	            (debug_buf_write_index + 1) % DEBUG_OUTPUT_BUFFER_SIZE;
	    return 1;
	}
	return 0;
}

void _debug_printf(const char *format, ...)
{
    va_list args;

    va_start( args, format );
    printf_format( _debug_putchar, format, args );
}

static char debug_read_buf[DEBUG_INPUT_BUFFER_SIZE];
char *_debug_getstr(int *len)
{
    volatile int n = 0;;

    _debug_printf_flush();

    for(n=0;n<DEBUG_INPUT_BUFFER_SIZE;n++)
        debug_read_buf[n] = 0;

    if(ISDEBUGACTIVE())
        __read(0, debug_read_buf, DEBUG_INPUT_BUFFER_SIZE-1);

    if(len)
        *len = small_strlen(debug_read_buf);
    return debug_read_buf;
}
#endif

#ifndef DEBUG
char *non_debug_input_buf[1];
#endif

#ifdef DEBUG
void _debug_puts(const char *s)
{
	while(*s)
	{
		_debug_putchar(*(s++));
	}
	_debug_putchar('\n');
}
#endif
