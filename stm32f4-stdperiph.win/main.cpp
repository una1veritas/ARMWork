/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */

#include <stdio.h>
#include <stdint.h>

#include <stm32f4xx.h>

#include "stm32f4xx_it.h"

#include "armcore.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "spi.h"
#include "i2c.h"
#include "ST7032i.h"

ST7032i lcd;

int main(void) {
	uint16_t bits;
	uint32_t intval = 40;
	uint32_t tnow;
	char tmp[92];

	TIM2_timer_start();

	usart_begin(&Serial3, PC11, PC10, 19200);
	usart_print(&Serial3,
			"Happy are those who know they are spiritually poor; \n");
	usart_print(&Serial3, "The kingdom of heaven belongs to them!\n");
	usart_print(&Serial3, "How many eyes does Mississipi river have?\n");
	usart_print(&Serial3, "Quick brown fox jumped over the lazy dog!\n");
	usart_flush(&Serial3);

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	sprintf(tmp, "SYSCLK = %ld, ", RCC_Clocks.SYSCLK_Frequency);
	usart_print(&Serial3, tmp);
	sprintf(tmp, "HCLK = %ld, ", RCC_Clocks.HCLK_Frequency);
	usart_print(&Serial3, tmp);
	sprintf(tmp, "PCLK1 = %ld, ", RCC_Clocks.PCLK1_Frequency);
	usart_print(&Serial3, tmp);
	sprintf(tmp, "PCLK2 = %ld\r\n", RCC_Clocks.PCLK2_Frequency);
	usart_print(&Serial3, tmp);
//	usart_flush(USART2Serial);

	GPIOMode(PinPort(PD12),
			(PinBit(PD12) | PinBit(PD13) | PinBit(PD14) | PinBit(PD15)), OUTPUT,
			FASTSPEED, PUSHPULL, NOPULL);
	spi_begin(SPI2, PB13, PB14, PB15, PB12);
	digitalWrite(PB12, HIGH);

	i2c_begin(&Wire1, PB9, PB8, 100000);
	lcd.init(&Wire1);
	lcd.begin();
	lcd.setContrast(46);
	lcd.print("Hello?!");       // Classic Hello World!

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

		sprintf(tmp, "%04ld", millis());
		usart_print(&Serial3, tmp);
		usart_print(&Serial3, "\n");

		lcd.setCursor(0, 1);
		lcd.print((const char *)tmp);

		digitalWrite(PB12, LOW);
		spi_transfer(SPI2, (uint8_t *) tmp, 8);
		digitalWrite(PB12, HIGH);

		/*
		 dval = (uint32) (100.0f + 64*sinf( (count % (uint32)(3.14159 * 2 * 32))/32.0f));
		 usart3.println(dval);
		 */
		uint16_t i = 0;
		if (usart_available(&Serial3) > 0) {
			while (usart_available(&Serial3) > 0 && i < 92) {
				tmp[i++] = (char) usart_read(&Serial3);
			}
			tmp[i] = 0;
			usart_print(&Serial3, "> ");
			usart_print(&Serial3, tmp);
			usart_print(&Serial3, "\n");
		}
	}
	return 0;
}

