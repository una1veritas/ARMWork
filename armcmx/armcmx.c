
#include "armcmx.h"
#include "usart.h"


#ifdef STDSERIAL
#if defined(LPC11UXX)
#elif defined(STM32F10X_MD)
#if (STDSERIAL == 2)
usart stdserial = { USART2, PA2, PA1, 57600, WORDLENGTH_8BIT | FLOW_NONE | PARITY_NONE | STOPBITS_1 };
#elif (STDSERIAL == 1 )
usart stdserial = { USART1, PA10, PA9, 57600, WORDLENGTH_8BIT | FLOW_NONE | PARITY_NONE | STOPBITS_1 };
#endif
#endif
#endif

void armcmx_init(void) {
	// starting delay/millis system by timer2
#ifndef LPC11UXX
	TIM2_delaytimer_start();
#endif
#ifdef STDSERIAL
	usart_init(&stdserial);
#else
	#warning "No STDSERIAL defined."
#endif
}

#ifdef STDSERIAL

#if defined(__arm__)
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
