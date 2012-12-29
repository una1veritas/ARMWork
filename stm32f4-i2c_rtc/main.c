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
#include "i2c1_drv.h"

#include "st7032i.h"

static USART USerial3;
//ST7032i lcd;

int main(void) {
	uint16_t bits;
	uint32_t intval = 40;
	uint32_t tnow;
	char tmp[92];
	RCC_ClocksTypeDef RCC_Clocks;
	uint16_t i;
	
	TIM2_timer_start();

	usart_begin(&USerial3, USART3, PC11, PC10, 19200);

	usart_print(&USerial3, 
			"Happy are those who know they are spiritually poor; \n"
			"The kingdom of heaven belongs to them!\n");
	usart_flush(&USerial3);
	
	RCC_GetClocksFreq(&RCC_Clocks);

	sprintf(tmp, "SYSCLK = %ul\n", RCC_Clocks.SYSCLK_Frequency);
	usart_print(&USerial3, tmp);
	sprintf(tmp, "PCLK1 = %ul\n", RCC_Clocks.PCLK1_Frequency);
	usart_flush(&USerial3);

	GPIOMode(PinPort(PD12),
			(PinBit(PD12) | PinBit(PD13) | PinBit(PD14) | PinBit(PD15)), OUTPUT,
			FASTSPEED, PUSHPULL, NOPULL);
			/*
	spi_begin(SPI2, PB13, PB14, PB15, PB12);
	digitalWrite(PB12, HIGH);
*/
	I2C1_Init();
//	i2c_begin(&Wire1, PB9, PB8, 100000);
	ST7032i_Init();
//	lcd.begin();
//	lcd.setContrast(46);
ST7032i_Clear();
ST7032i_Print_String("Yapee!");//	lcd.print("Yappee!");       // Classic Hello World!

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
		sprintf(tmp, "%04ld\n", millis());
		usart_print(&USerial3, tmp);

		/*
		digitalWrite(PB12, LOW);
		spi_transfer(SPI2, (uint8_t *) tmp, 8);
		digitalWrite(PB12, HIGH);
*/
		i = 0;
		if (usart_available(&USerial3) > 0) {
			while (usart_available(&USerial3) > 0 && i < 92) {
				tmp[i++] = (char) usart_read(&USerial3);
			}
			tmp[i] = 0;
			usart_print(&USerial3, "> ");
			usart_print(&USerial3, tmp);
			usart_print(&USerial3, "\n");
		}

	}
	return 0;
}

