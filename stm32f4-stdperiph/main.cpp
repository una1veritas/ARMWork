/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>

#include <stm32f4xx.h>
#include <stm32f4xx_usart.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "i2c.h"

#include "ST7032i.h"
#include "USARTSerial.h"

ST7032i lcd;
USARTSerial Serial3(USART3);

int main(void) {
	uint16_t bits;
	uint32_t intval = 40;
	uint32_t tnow;
	char tmp[92];

	TIM2_timer_start();

	Serial3.begin(PC11, PC10, 19200);

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
	
	Serial3.println(message);
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
	Serial3.println();
	Serial3.flush();

	GPIOMode(PinPort(PD12),
			(PinBit(PD12) | PinBit(PD13) | PinBit(PD14) | PinBit(PD15)), OUTPUT,
			FASTSPEED, PUSHPULL, NOPULL);

	i2c_begin(&I2C1Buffer, I2C1, PB9, PB8, 100000);
	lcd.init(&I2C1Buffer);
	lcd.begin();
	lcd.setContrast(46);
	lcd.print("Whoooeee!");       // Classic Hello World!

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
		Serial3.print((float)millis()/1000, 3);
		Serial3.print(" ");
		Serial3.print(lcd.row());
		Serial3.print(" ");
		Serial3.print(lcd.column());
		Serial3.print(" ");		Serial3.write((uint8_t *)message+((millis()/1000)%(messlen-16)), 16);
		Serial3.println();
		//		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.write((uint8_t *)message+((millis()/1000)%(messlen-16)), 16);
		lcd.setCursor(0, 1);
		lcd.print((float)millis()/1000, 3);
		
		uint16_t i = 0;
		if (Serial3.available() > 0) {
			while (Serial3.available() > 0 && i < 92) {
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

