/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stm32f4xx.h>

#include "stm32f4xx_it.h"

#include "gpio.h"
#include "delay.h"
#include "usart.h"
#include "adc3dma.h"

int main(void) {
	uint16_t bits;
	uint32_t intval = 40;
	uint32_t tnow;
	char tmp[92];
	USART Serial;

	TIM2_timer_start();

	usart_begin(USART3, &Serial, PC11, PC10, 19200);
	usart_print(&Serial,
			"Happy are those who know they are spiritually poor; \n");
	usart_print(&Serial, "The kingdom of heaven belongs to them!\n");
	usart_print(&Serial, "How many eyes does Mississipi river have?\n");
	usart_print(&Serial, "Quick brown fox jumped over the lazy dog!\n");
	usart_flush(&Serial);

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	sprintf(tmp, "SYSCLK = %ld, ", RCC_Clocks.SYSCLK_Frequency);
	usart_print(&Serial, tmp);
	sprintf(tmp, "HCLK = %ld, ", RCC_Clocks.HCLK_Frequency);
	usart_print(&Serial, tmp);
	sprintf(tmp, "PCLK1 = %ld, ", RCC_Clocks.PCLK1_Frequency);
	usart_print(&Serial, tmp);
	sprintf(tmp, "PCLK2 = %ld\r\n", RCC_Clocks.PCLK2_Frequency);
	usart_print(&Serial, tmp);
	//
	sprintf(tmp, "ADC1 = %lx, ADC2 = %lx, ADC3 = %lx\r\n", (uint32_t)ADC1, (uint32_t)ADC2, (uint32_t)ADC3);
	usart_print(&Serial, tmp);
//	usart_flush(USART2Serial);

	GPIOMode(PinPort(PD12),
			(PinBit(PD12) | PinBit(PD13) | PinBit(PD14) | PinBit(PD15)), OUTPUT,
			FASTSPEED, PUSHPULL, NOPULL);

	ADC3_CH12_DMA_Config();
	  ADC_SoftwareStartConv(ADC3);

	bits = GPIO_ReadOutputData(GPIOD );
	GPIOWrite(GPIOD, PinBit(PD13) | (bits & 0x0fff));
	delay_ms(intval);
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

		while (tnow == millis() / 1000)
			;
		tnow = millis() / 1000;

		sprintf(tmp, "%04ld ", millis());
		usart_print(&Serial, tmp);

		uint16_t i = 0;

		for(i = 0; i < 8; i++) {
			ADC3ConvertedVoltage = ADC3ConvertedValue *3300/0xFFF;
			sprintf(tmp, "%04ld ", ADC3ConvertedVoltage);
			usart_print(&Serial, tmp);
			delay_ms(5);
		}
		usart_print(&Serial, "\n");
		if (usart_available(&Serial) > 0) {
			while (usart_available(&Serial) > 0 && i < 92) {
				tmp[i++] = (char) usart_read(&Serial);
			}
			tmp[i] = 0;
			usart_print(&Serial, "> ");
			usart_print(&Serial, tmp);
			usart_print(&Serial, "\n");
		}
	}
	return 0;
}

