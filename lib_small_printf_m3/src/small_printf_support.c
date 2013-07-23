/***********************************************************************
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

/*
 * NXP Semiconductors- 6/12/2010
 * Based on LGPL code- see following header
 *
Copyright 2001, 2002 Georges Menie (www.menie.org)
stdarg version contributed by Christian Ettinger

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
putchar is the only external dependency for this file,
if you have a working putchar, leave it commented out.
If not, uncomment the define below and
replace outbyte(c) by your own function call.

#define putchar(c) outbyte(c)
*/

#undef LIB_FLOAT_PRINTF

#include <stdint.h>
#include <stdarg.h>
#include "small_printf.h"
#include "small_utils.h"

#define PAD_RIGHT 1
#define PAD_ZERO 2

int prints(const fp_printf_write_func printf_write, const char *string, int width, int pad)
{
    register int pc = 0, padchar = ' ';

    if (width > 0) {
        register int len = 0;
        register const char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for ( ; width > 0; --width) {
            printf_write(padchar);
                ++pc;
        }
    }
    for ( ; *string ; ++string) {
        printf_write(*string);
            ++pc;
    }
    for ( ; width > 0; --width) {
        printf_write(padchar);
            ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

int printi(const fp_printf_write_func printf_write, int i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(printf_write, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) {
        t = u % b;
        if( t >= 10 )
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if( width && (pad & PAD_ZERO) ) {
            printf_write('-');
            ++pc;
            --width;
        }
        else {
            *--s = '-';
        }
    }

    return pc + prints (printf_write, s, width, pad);
}

// Not reentrant
static char *nsprintf_buffer;
static int nsprintf_buffer_length;
static int nsprintf_buffer_index;

void nsprintf_write_init(char *buffer, int buffer_length)
{
    nsprintf_buffer = buffer;
    nsprintf_buffer_length = buffer_length;
    nsprintf_buffer_index = 0;
}

int nsprintf_write(char c)
{
    if(nsprintf_buffer_index >= (nsprintf_buffer_length-2))
        return 0;

    nsprintf_buffer[nsprintf_buffer_index++] = c;
    nsprintf_buffer[nsprintf_buffer_index] = 0;
    return 1;
}

