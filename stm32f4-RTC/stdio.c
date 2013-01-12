#include <stdio.h>
#include "stm32f4xx_usart.h"
#include "usart.h"

#define STDSERIAL  USART6

/* Exported functions ------------------------------------------------------- */

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//******************************************************************************
// Hosting of stdio functionality through USART6
//******************************************************************************

#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE { 
int handle; /* Add whatever you need here */ 
};
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f)
{
	static int last;

	if ((ch == (int)'\n') && (last != (int)'\r'))
	{
		last = (int)'\r';
  	usart_write(&STDSERIAL, last);
		usart_flush(&STDSERIAL);
	}
	else
		last = ch;

	usart_write(&STDSERIAL, last);

  return(ch);
}

int fgetc(FILE *f)
{
  return((int)usart_read(&STDSERIAL));
}

int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}



/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(USART6));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\r Please enter valid number between 0 and 9 \n\r");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\r Please enter valid number between 0 and %d \n\r", value);
    return 0xFF;
  }
  return index;
}

void _sys_exit(int return_code)
{
label:  goto label;  /* endless loop */
}
