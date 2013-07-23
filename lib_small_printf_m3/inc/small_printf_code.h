/*
 * NXP Semiconductors- 6/12/2010
 * Based on LGPL code- see following header
 * Changelog:
 * 	Added * printf specifier to get field width from argument list
 *  Printf functions accept function pointer to output routine
 *  Duplicate float / non-float functions allow creation of static library (linker pulls in either func)
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

#include <stdint.h>
#include <stdarg.h>
#include "small_printf.h"
#include "small_utils.h"

#define PAD_RIGHT 1
#define PAD_ZERO 2

#ifdef LIB_FLOAT_PRINTF
//****************************************************************************
//  This version returns the length of the output string.
//  It is more useful when implementing a walking-string function.
//****************************************************************************
unsigned dbl2stri(char *outbfr, double dbl, unsigned dec_digits)
{
   static char local_bfr[81] ;
   char *output = (outbfr == 0) ? local_bfr : outbfr ;

   //*******************************************
   //  extract negative info
   //*******************************************
   int negative = (dbl < 0.0) ? 1 : 0 ;
   if (negative) {
      *output++ = '-' ;
      dbl *= -1.0 ;
   }

   //**************************************************************************
   //  construct fractional multiplier for specified number of digits.
   //  Note that we draw one digit more than requested in order
   //  to handle last-digit rounding.
   //**************************************************************************
   uint32_t mult = 10 ;
   uint32_t idx ;
   for (idx=0; idx < dec_digits; idx++)
      mult *= 10 ;

   // printf("mult=%u\n", mult) ;
   int wholeNum = (int) dbl ;
   int decimalNum = (int) ((dbl - wholeNum) * mult);
   //  round off low digit
   decimalNum += 5 ;
   decimalNum /= 10 ;

   //*******************************************
   //  convert integer portion
   //*******************************************
   char tbfr[40] ;
   idx = 0 ;
   while (wholeNum != 0) {
      tbfr[idx++] = '0' + (wholeNum % 10) ;
      wholeNum /= 10 ;
   }
   // printf("%.3f: whole=%s, dec=%d\n", dbl, tbfr, decimalNum) ;
   if (idx == 0) {
      *output++ = '0' ;
   } else {
      while (idx > 0) {
         *output++ = tbfr[idx-1] ;  //lint !e771
         idx-- ;
      }
   }
   if (dec_digits > 0) {
      *output++ = '.' ;

      //*******************************************
      //  convert fractional portion
      //*******************************************
      idx = 0 ;
      while (decimalNum != 0) {
         tbfr[idx++] = '0' + (decimalNum % 10) ;
         decimalNum /= 10 ;
      }
      //  pad the decimal portion with 0s as necessary;
      //  We wouldn't want to report 3.093 as 3.93, would we??
      while (idx < dec_digits) {
         tbfr[idx++] = '0' ;
      }
      // printf("decimal=%s\n", tbfr) ;
      if (idx == 0) {
         *output++ = '0' ;
      } else {
         while (idx > 0) {
            *output++ = tbfr[idx-1] ;
            idx-- ;
         }
      }
   }
   *output = 0 ;

   //  prepare output
   output = (outbfr == 0) ? local_bfr : outbfr ;
   return small_strlen(output) ;
}
#endif

int printf_format(const fp_printf_write_func printf_write,
        	const char *format, va_list varg)
{
    int post_decimal ;
    int width, pad ;
    unsigned dec_width = 0 ;
    int pc = 0;
    char scr[2];

        for (; *format != 0; ++format) {
                if (*format == '%') {
                        ++format;
                        width = pad = 0;
                        if (*format == '\0')
                                break;
                        if (*format == '%')
                                goto out;
                        if (*format == '-') {
                                ++format;
                                pad = PAD_RIGHT;
                        }
                        while (*format == '0') {
                                ++format;
                                pad |= PAD_ZERO;
                        }
         post_decimal = 0 ;
         if (*format == '.'  || *format == '*'
         || (*format >= '0' &&  *format <= '9')) {

            while (1) {
                if (*format == '*') {
                   width = va_arg(varg, int);
                   format++;
               } else if (*format == '.') {
                  post_decimal = 1 ;
                  dec_width = 0 ;
                  format++ ;
               } else if ((*format >= '0' &&  *format <= '9')) {
                  if (post_decimal) {
                     dec_width *= 10;
                     dec_width += *format - '0';
                  } else {
                     width *= 10;
                     width += *format - '0';
                  }
                  format++ ;
               } else {
                  break;
               }
            }
         }
         if (*format == 'l')
            ++format;
         switch (*format) {
         case 's':
            {
            char *s = (char *) va_arg(varg, char *);   //lint !e740
            // printf("[%s] w=%u\n", s, width) ;
            pc += prints (printf_write, s ? s : "(null)", width, pad);
            }
            break;
         case 'd':
            pc += printi (printf_write, va_arg(varg, int), 10, 1, width, pad, 'a');
            break;
         case 'x':
            pc += printi (printf_write, va_arg(varg, int), 16, 0, width, pad, 'a');
            break;
         case 'X':
            pc += printi (printf_write, va_arg(varg, int), 16, 0, width, pad, 'A');
            break;
         case 'p':
            printf_write('0');
            printf_write('x');
            pad |= PAD_ZERO;
            width = 8;
            pc += printi (printf_write, va_arg(varg, int), 16, 0, width, pad, 'A');
            break;
         case 'u':
            pc += printi (printf_write, va_arg(varg, int), 10, 0, width, pad, 'a');
            break;
         case 'c':
            /* char are converted to int then pushed on the stack */
            scr[0] = (char)va_arg(varg, int);
            scr[1] = '\0';
            pc += prints (printf_write, scr, width, pad);
            break;

#ifdef LIB_FLOAT_PRINTF
            case 'f':
            {
            double dbl = (double)va_arg(varg, double);
            char bfr[81] ;
            // unsigned slen =
            dbl2stri(bfr, dbl, dec_width) ;
            // stuff_talkf("[%s], width=%u, dec_width=%u\n", bfr, width, dec_width) ;
            pc += prints (printf_write, bfr, width, pad);
            }
            break;
#endif

         default:
        	 printf_write('%');
        	 printf_write(*format);
            break;
         }
                }
                else {
                 out:
                 printf_write(*format);
                        ++pc;
                }
        }
        return pc;
}

int func_printf(const fp_printf_write_func printf_write, const char *format, ...)	
{
	va_list args;

	va_start( args, format );
	return printf_format( printf_write, format, args );
}

// Not reentrant due to nsprintf_write counter variables (see small_printf_support.c)
int nsprintf(char *buffer, int buffer_length, const char *format, ...)
{
    va_list args;

    va_start( args, format );

    nsprintf_write_init(buffer, buffer_length);
    return printf_format( nsprintf_write, format, args );
}

#ifdef TEST_PRINTF
int main(void)
{
char *ptr = "Hello world!";
char *np = 0;
int i = 5;
unsigned int bs = sizeof(int)*8;
int mi;
char buf[80];

mi = (1 << (bs-1)) + 1;
printf("%s\n", ptr);
printf("printf test\n");
printf("%s is null pointer\n", np);
printf("%d = 5\n", i);
printf("%d = - max int\n", mi);
printf("char %c = 'a'\n", 'a');
printf("hex %x = ff\n", 0xff);
printf("hex %02x = 00\n", 0);
printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
printf("%d %s(s)%", 0, "message");
printf("\n");
printf("%d %s(s) with %%\n", 0, "message");
sprintf(buf, "justif: \"%-10s\"\n", "left"); printf("%s", buf);
sprintf(buf, "justif: \"%10s\"\n", "right"); printf("%s", buf);
sprintf(buf, " 3: %04d zero padded\n", 3); printf("%s", buf);
sprintf(buf, " 3: %-4d left justif.\n", 3); printf("%s", buf);
sprintf(buf, " 3: %4d right justif.\n", 3); printf("%s", buf);
sprintf(buf, "-3: %04d zero padded\n", -3); printf("%s", buf);
sprintf(buf, "-3: %-4d left justif.\n", -3); printf("%s", buf);
sprintf(buf, "-3: %4d right justif.\n", -3); printf("%s", buf);

return 0;
}

/*
* if you compile this file with
*   gcc -Wall $(YOUR_C_OPTIONS) -DTEST_PRINTF -c printf.c
* you will get a normal warning:
*   printf.c:214: warning: spurious trailing `%' in format
* this line is testing an invalid % at the end of the format string.
*
* this should display (on 32bit int machine) :
*
* Hello world!
* printf test
* (null) is null pointer
* 5 = 5
* -2147483647 = - max int
* char a = 'a'
* hex ff = ff
* hex 00 = 00
* signed -3 = unsigned 4294967293 = hex fffffffd
* 0 message(s)
* 0 message(s) with %
* justif: "left      "
* justif: "     right"
*  3: 0003 zero padded
*  3: 3    left justif.
*  3:    3 right justif.
* -3: -003 zero padded
* -3: -3   left justif.
* -3:   -3 right justif.
*/

#endif
