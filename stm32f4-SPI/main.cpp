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
//#include "SPIBus.h"
#include "spi.h"
#include "Boards/stm32f4_discovery.h"
#include "GLCD/PCD8544.h"
#include "GLCD/fonts.h"

spi spi2bus = {SPI2, PB10, PC2, PC3, PB9};
//spi_init(SPI1Bus, SPI1, PA5, PA6, PA7, PA4); //  PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15

PCD8544 nokiaLCD(spi2bus, PB9, PD13, PD12);

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

	
	usart_print(&stdserial, "Basic initialization has been finished.\n");
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	//usart_print(&stdserial, message);
	//usart_print(&stdserial, "\r\n\r\n");
	//usart_flush(&stdserial);

	sprintf(tmp, "Clock frequencies: SYSCLK = %dl, HCLK = %dl, PCLK1 = %dl\r\n", 
	RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency);
	usart_print(&stdserial, tmp); 

	spi_init(&spi2bus, SPI2, PB10, PC2, PC3, PB9);
	spi_begin(&spi2bus);
	//spi_init(SPI1Bus, SPI1, PA5, PA6, PA7, PA4); //  PA5 / PB3, miso = PA6/ PB4, mosi = PA7 / PB5, nSS = PA4 / PA15
	nokiaLCD.init();	
	nokiaLCD.setContrast(0xb5);
	
	nokiaLCD.clear();
	nokiaLCD.drawBitmap(PCD8544::SFEFlame);
	delay(1000);
		
	uint16 shift = 0;
	nokiaLCD.setFont(Chicago_15);

	nokiaLCD.clear();
	nokiaLCD.drawString("Gnu is not Nuke!");

	while(1);
	while (1) {
		nokiaLCD.clear();
		nokiaLCD.cursor(252 - shift );
		nokiaLCD.drawString("GNU is Nuke an Uran 01235y!");
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

