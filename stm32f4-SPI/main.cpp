/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>

#include "stm32f4xx_it.h"

#include "cmcore.h"
#include "spi.h"
#include "Boards/stm32f4_discovery.h"
//#include "GLCD/PCD8544.h"

usart Serial6;
spi spi1;
//SPIBus SPI1Bus(SPI1, PA5, PA6, PA7, PA4);
//PCD8544 nokiaLCD(SPI1Bus, PA4, PA3, PA2);

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
	
	cmcore_init();
	usart_init(&Serial6, USART6, PC7, PC6);
	usart_begin(&Serial6, 57600);

	//	SPI1Bus.begin();
	spi_init(&spi1, SPI1, PA5, PA6, PA7, PA4);
	spi_setClockDivier(&spi1, SPI_CLOCK_DIV256);
	digitalWrite(PA4, LOW);
	tmp[0] = spi_transfer(&spi1, 0x5A);
	spi_transfer(&spi1, tmp[0]);
	spi_transfer(&spi1, 0x22);
digitalWrite(PA4, HIGH);

	usart_print(&Serial6, "Basic initialization has been finished.\n");
	
//	nokiaLCD.init();
	
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
					
//		nokiaLCD.clear();
//		nokiaLCD.drawBitmap(PCD8544::SFEFlame);
		delay(1000);
		
	uint16 shift = 0;
//	nokiaLCD.selectFont(PCD8544::CHICAGO10);
	
	while (1) {
//		nokiaLCD.clear();
//		nokiaLCD.cursor(252 - shift );
//		nokiaLCD.drawString("GNU is Nuke an Uran 01235y!");
//		nokiaLCD.drawFont(Nokia5110::Chicago10x15, 'A');
//		nokiaLCD.drawFont(Nokia5110::Chicago10x15, 'W');
		shift++;
		shift %= 252;
		delay(250);

/*
		if ( millis() / 125 != shift ) {
			shift = millis()/ 125;
			nokiaLCD.clear();
			nokiaLCD.gotoXY(7- shift%7,0);
			strncpy(tmp, message+((shift/7) % messlen), 48);
			tmp[48] = 0;
			nokiaLCD.drawString(tmp);
//		usart_print(&Serial6, tmp);
//		usart_print(&Serial6, "\r\n");
		}
		*/
	}
}

