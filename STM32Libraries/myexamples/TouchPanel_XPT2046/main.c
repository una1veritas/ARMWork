/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */
/*
http://www.element14.com/community/groups/development-tools/blog/2012/05/26/stm32f4-discovery-hy32d-tft-lcd

LCD pin name
 STM32F4 Pin number
 STM32F4 pin name
 
D0 61 PD14
D1 62 PD15

D2 81 PD0
D3 82 PD1

D4 38 PE7
D5 39 PE8
D6 40 PE9
D7 41 PE10
D8 42 PE11
D9 43 PE12
D10 44 PE13
D11 45 PE14
D12 46 PE15
D13 55 PD8
D14 56 PD9
D15 57 PD10
CS 88 PD7   (NE1)
RS 60 PD11 (FSMC-A16)
RD 85 PD4   (NOE)
WR 86 PD5   (NWE)
RESET  +3V(1K Pull up) or GPIO

TP_IRQ 		PD6 user defined interrupt pin
TP_SCK 52 PB13  SPI2SCK
TP_SI 54 PB15   SPI2MOSI
TP_SO 53 PB14   SPI2MISO
TP_CS 51 PB12 	SPI2NSS
 
BLVDD 3V3
BLVSS GND
BL_CNTL 69 PA10  user defined pwm pin
 
*/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include <stm32f4xx.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "spi.h"

#include "TouchPanel/XPT2046.h"

USARTPort ser6;
SPIPort spi2;

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
	
	TP_Init(&spi2, PB13, PB15, PB14, PB12, PD6);
	
	while (1);
}

