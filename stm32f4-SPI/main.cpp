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
#include "I2CWire.h"
#include "USARTSerial.h"
#include "SPIBus.h"

#include "ChLCD/ST7032i.h"
#include "Boards/stm32f4_discovery.h"

USARTSerial Serial6(USART6, PC7, PC6);
I2CWire Wire1(I2C1, PB8, PB9);
// periph devices
ST7032i lcd(Wire1);
SPIBus SPI1Bus();

int main(void) {
	uint32 tnow;
	char tmp[256];
	uint16_t i = 0;
	
	TIM2_timer_start();

	Serial6.begin(57600);

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
	
	Serial6.println(message);
	Serial6.flush();

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	Serial6.print( "SYSCLK = ");
	Serial6.print(RCC_Clocks.SYSCLK_Frequency);
	Serial6.print( ", HCLK = ");
	Serial6.print( RCC_Clocks.HCLK_Frequency);
	Serial6.print( ", PCLK1 = ");
	Serial6.print( RCC_Clocks.PCLK1_Frequency);
	Serial6.print( ", PCLK2 = ");
	Serial6.print(RCC_Clocks.PCLK2_Frequency);
	Serial6.println();
	Serial6.flush();

	GPIOMode(PinPort(LED1), (PinBit(LED1) | PinBit(LED2) | PinBit(LED3) | PinBit(LED4)), 
					OUTPUT, FASTSPEED, PUSHPULL, NOPULL);

	Wire1.begin(100000);
	lcd.begin();
	lcd.setContrast(42);
	lcd.print("Hello!");       // Classic Hello World
	pinMode(PC3, OUTPUT);
	digitalWrite(PC3, LOW);
	
	for(i = 0; i < 3; i++) {
		delay_ms(500);
		lcd.noDisplay();
		delay_ms(500);
		lcd.display();
	}
	//	lcd.clear();
	tnow = millis();
	
	while (1) {

		if (tnow / 200 != (millis() / 200) ) {
			tnow = millis();
			strncpy(tmp, message+((tnow/200) % messlen), 16);
			tmp[16] = 0;
			Serial6.println(tmp);
			lcd.setCursor(0,1);
			lcd.print(tmp);
			lcd.setCursor(0, 0);
			lcd.print(((tnow/200) % messlen));
		}

		char c = 0;
		if (Serial6.available() > 0) {
			while (Serial6.available() > 0 && i < 92) {
				c = (char) Serial6.read();
				if ( c == '\n' || c == '\r' || c == ' ' ) {
					tmp[i] = 0;
					break;
				}
				tmp[i++] = c;
			}
			if ( strlen(tmp) and (c == 0x0d or c == 0x0a) ) {
				Serial6.print("> ");
				Serial6.print("\n");
				i = 0;
			}
		}

	}
	return 0;
}

