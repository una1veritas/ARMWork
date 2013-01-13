//******************************************************************************
// Hosting of stdio functionality through USART6
//******************************************************************************

#include <stdio.h>

#include <stm32f4xx_usart.h>

#define STDUSART  USART6

#if defined(__KEIL__)
#error "11111"

#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE { 
int handle; // Add whatever you need here 
};
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(STDUSART, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(STDUSART, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
/*
{
	usart_write(&STDSERIAL, ch);
  return(ch);
}
*/
int fgetc(FILE *f)
{
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(STDUSART, USART_FLAG_RXNE) == RESET)
  {}
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  return USART_ReceiveData(STDUSART);
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
	usart_write(&STDSERIAL, ch);
  return(ch);
}

#endif
