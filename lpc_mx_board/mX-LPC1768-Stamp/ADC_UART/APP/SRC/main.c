/*****************************************************************************
 *   uarttest.c:  main C entry file for NXP LPC17xx Family Microprocessors
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.05.27  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "lpc17xx.h"
#include "type.h"
#include "uart.h"
#include "adc.h"
#include <stdio.h>

extern volatile uint32_t UART3Count;
extern volatile uint8_t UART3Buffer[BUFSIZE];
extern volatile uint32_t UART1Count;
extern volatile uint8_t UART1Buffer[BUFSIZE];
extern volatile uint32_t ADC0Value[ADC_NUM];
extern volatile uint32_t ADC0IntDone;

/*****************************************************************************
**   Main Function  main()
This program has been test on Keil LPC1700 board.
*****************************************************************************/
int main (void)
{
  char buf[6];
  unsigned int adc_value = 0;

  SystemInit();

  UARTInit(3, 38400);	/* baud rate setting */
  UARTInit(1, 38400);	/* baud rate setting */
  ADCInit( ADC_CLK );

  while (1) 
  {				/* Loop forever */
	  adc_value = ADC0Read( 5 );
	  sprintf(buf, "%d\r\n", adc_value);
	  UARTSend( 3, (uint8_t *)buf, sizeof(buf));
	  UARTSend( 0, (uint8_t *)buf, sizeof(buf));
  }
}

/*****************************************************************************
**                            End Of File
*****************************************************************************/
