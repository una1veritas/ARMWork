//******************************************************************************
// Hosting of stdio functionality through USART6
//******************************************************************************

#include <stdio.h>
#include <stm32f4xx_usart.h>

#include "stdiodef.h"

#if defined(__arm__)
#ifdef STDSERIAL

#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE { 
int handle; // Add whatever you need here 
};
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
	usart_write(&stdserial, ch);
  return(ch);
}

int fgetc(FILE *f)
{
	while (!usart_available(&stdserial));
	return usart_read(&stdserial);
}

int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}

void _sys_exit(int return_code)
{
label:  goto label;  /* endless loop */
}

#elif defined(__GNUC__) 

int __io_putchar(int ch)
{
	usart_write(&stdserial, ch);
  return(ch);
}

#endif
#endif
