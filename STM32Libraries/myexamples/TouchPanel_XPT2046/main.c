/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include <stm32f4xx.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "spibus.h"
#include "usart.h"

#include "TouchPanel/XPT2046.h"

USARTPort ser6;
SPIBus spi1;

int main(void) {
	char tmp[128];
	RCC_ClocksTypeDef RCC_Clocks;
	
	armcore_init();
	
	usart_init(&ser6, USART6, PC7, PC6, 19200);
	
	usart_print(&ser6, "Hello, baby.\n");

	RCC_GetClocksFreq(&RCC_Clocks);

	sprintf(tmp, "SYSCLK = %ld, HCLK = %ld, PCLK1 = %ld, PCLK2 = %ld\n\n", 
							RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);
	usart_print(&ser6, tmp);
	usart_print(&ser6, "Now we're going to test XPT2046 Resistive membrane touch panel sensor.\n");
	
	while (1);
}

