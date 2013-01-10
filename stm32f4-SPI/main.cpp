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
#include <stm32f4xx_usart.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "gpio.h"
#include "delay.h"

#include "usart.h"
#include "spi.h"

#include "Boards/stm32f4_discovery.h"

#include "GLCD/Nokia5110.h"

USART Serial6;
SPIBuffer SPI1Buffer;
Nokia5110 nokiaLCD(&SPI1Buffer, PA3, PA2);

int main(void) {
	char tmp[256];

	const char message[] = 
			"This royal throne of kings, this scepter'd isle, \n"
			"This earth of majesty, this seat of Mars, \n"
			"This other Eden, demi-paradise, \n"
			"This fortress built by Nature for herself\n"
			"Against infection and the hand of war, \n"
			"This happy breed of men, this little world,\n" 
			"This precious stone set in the silver sea, \n"
			"Which serves it in the office of a wall, \n"
			"Or as a moat defensive to a house, \n"
			"Against the envy of less happier lands, \n"
			"This blessed plot, this earth, this realm, this England,";
	const uint16 messlen = strlen(message);
	
	TIM2_timer_start();
	usart_begin(&Serial6, USART6, PC7, PC6, 57600);
	spi_begin(&SPI1Buffer, SPI1, PA5, PA6, PA7, PA4); //  PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	
	nokiaLCD.init();
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	usart_print(&Serial6, message);
	usart_print(&Serial6, "\r\n\r\n");
	usart_flush(&Serial6);


	sprintf(tmp, "Clock frequencies: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl\r\n", 
		RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);
	usart_print(&Serial6, tmp); 

	GPIOMode(PinPort(LED1), (PinBit(LED1) | PinBit(LED2) | PinBit(LED3) | PinBit(LED4)), 
					OUTPUT, FASTSPEED, PUSHPULL, NOPULL);
					
  nokiaLCD.clear();
  nokiaLCD.drawBitmap(Nokia5110::SFEFlame);
  delay(1000);

	while (1) {
	}
}

