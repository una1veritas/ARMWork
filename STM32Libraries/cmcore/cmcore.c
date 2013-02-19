
#include "cmcore.h"
#include "usart.h"


#if defined (STDSERIAL)
#if (STDSERIAL == 3)
#define STDUSART 	USART3
#define STDRX 		PB11
#define STDTX 		PB10
#define STDBAUD 	57600
#warning "using STDSERIAL USART3"
#elif (STDSERIAL == 6)
#define STDUSART 	USART6
#define STDRX 		PC7
#define STDTX 		PC6
#define STDBAUD 	57600
#warning "using STDSERIAL USART6"
#endif
#endif

void cmcore_init() {
	// starting delay/millis system by timer2
	TIM2_delaytimer_start();
#ifdef STDSERIAL
	usart_init(&stdserial, STDUSART, STDRX, STDTX);
	usart_begin(&stdserial, STDBAUD);
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
