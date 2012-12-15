/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>

#include <stm32f4xx.h>
<<<<<<< HEAD
#include <stm32f4xx_usart.h>
=======
//#include <stm32f4xx_usart.h>
>>>>>>> origin/win@home

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"
#include "i2c.h"

#include "ST7032i.h"
#include "USARTSerial.h"

ST7032i lcd;
USARTSerial Serial3;

int main(void) {
	uint16_t bits;
	uint32_t intval = 40;
	uint32_t tnow;
	char tmp[92];

	TIM2_timer_start();

	Serial3.begin(USART3, PC11, PC10, 19200);
<<<<<<< HEAD
	Serial3.print("This royal throne of kings, this sceptred isle,\n"); /*
"This earth of majesty, this seat of Mars,\n"
"This other Eden, demi-paradise;\n"
"This fortress, built by nature for herself,\n"
"Against infection, and the hand of war;\n"
"This happy breed of men, this little world;\n"
"This precious stone set in the silver sea,\n"
"Which serves it in the office of a wall,\n"
"Or as a moat defensive to a house,\n"
"Against the envy of less happier lands;\n"); */
=======

	Serial3.print(
			"Happy are those who know they are spiritually poor; \n");
	Serial3.print( "The kingdom of heaven belongs to them!\n");
	Serial3.print( "If thou beest he! But O how fall'n!\n");
	Serial3.print( "How chang'd from him!\n");
>>>>>>> origin/win@home
	Serial3.flush();
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	Serial3.print( "SYSCLK = ");
	Serial3.print(RCC_Clocks.SYSCLK_Frequency);
	Serial3.print( ", HCLK = ");
	Serial3.print( RCC_Clocks.HCLK_Frequency);
	Serial3.print( ", PCLK1 = ");
	Serial3.print( RCC_Clocks.PCLK1_Frequency);
	Serial3.print( ", PCLK2 = ");
	Serial3.print(RCC_Clocks.PCLK2_Frequency);
<<<<<<< HEAD
	Serial3.print(".\n");
=======
	Serial3.print("\n");
>>>>>>> origin/win@home
	Serial3.flush();

	GPIOMode(PinPort(PD12),
			(PinBit(PD12) | PinBit(PD13) | PinBit(PD14) | PinBit(PD15)), OUTPUT,
			FASTSPEED, PUSHPULL, NOPULL);
	spi_begin(SPI2, PB13, PB14, PB15, PB12);
	digitalWrite(PB12, HIGH);

	i2c_begin(&Wire1, PB9, PB8, 100000);
	lcd.init(&Wire1);
	lcd.begin();
	lcd.setContrast(46);
<<<<<<< HEAD
	lcd.print("Dyre arm.");       // Classic Hello World!
=======
	lcd.print("Yappee!");       // Classic Hello World!
>>>>>>> origin/win@home

	bits = GPIO_ReadOutputData(GPIOD );
	GPIOWrite(GPIOD, PinBit(PD13) | (bits & 0x0fff));
	delay_ms(intval);
	tnow = millis() / 1000;
	while (tnow == millis() / 1000)
		;
	tnow = millis() / 1000;

	while (1) {
		bits = GPIO_ReadOutputData(GPIOD );

		GPIOWrite(GPIOD, PinBit(PD13) | (bits & 0x0fff));
		delay_ms(intval);
		GPIOWrite(GPIOD, PinBit(PD14) | (bits & 0x0fff));
		delay_ms(intval);
		GPIOWrite(GPIOD, PinBit(PD15) | (bits & 0x0fff));
		delay_ms(intval);
		GPIOWrite(GPIOD, PinBit(PD12) | (bits & 0x0fff));
		delay_ms(intval);
		//
		bits &= 0x0fff;
		switch ((tnow % 60) / 15) {
		case 3:
			bits |= PinBit(PD12);
		case 2:
			bits |= PinBit(PD15);
		case 1:
			bits |= PinBit(PD14);
		case 0:
		default:
			bits |= PinBit(PD13);
			break;
		}
		GPIOWrite(GPIOD, bits);

		while (tnow == millis() / 1000);
		tnow = millis() / 1000;

		//Serial3.print(tmp);
		Serial3.print(millis());
		Serial3.print("\n");

<<<<<<< HEAD
		sprintf(tmp, "%04ld.", millis());
=======
		sprintf(tmp, "%04ld", millis());
>>>>>>> origin/win@home
		lcd.setCursor(0, 1);
		lcd.print((const char *)tmp);

		digitalWrite(PB12, LOW);
		spi_transfer(SPI2, (uint8_t *) tmp, 8);
		digitalWrite(PB12, HIGH);

		uint16_t i = 0;
		if ( Serial3.available() > 0) {
			while ( Serial3.available() > 0 && i < 92) {
				tmp[i++] = (char) Serial3.read();
			}
			tmp[i] = 0;
			Serial3.print("> ");
			Serial3.print(tmp);
			Serial3.print("\n");
		}

	}
	return 0;
}

