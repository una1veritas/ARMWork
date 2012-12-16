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

#include "armcore.h"
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
	uint16 i;

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

	uint32_t sum;
	while (1) {
		sum = 0;
		for(i = 0; i < 8; i++) {
			sum += ADC3ConvertedValue; // *3300/0xFFF;
			delay_us(333);
		}
		sum /= 8;

		tmp[0] = 0;
		bits = 0;
		if ( sum>>9 ) {
			bits |= PinBit(PD13);
		}
		if ( sum>>10 ) {
			bits |= PinBit(PD14);
		}
		if ( sum>>11 ) {
			bits |= PinBit(PD15);
		}
		if ( sum>>12) {
			bits |= PinBit(PD12);
		}
		//
		GPIOWrite(GPIOD, bits);

		if ( (millis() % 1000) < 5 ) {
			sprintf(tmp, " %06ld %06ld\n", millis(), sum);
			usart_print(&Serial, tmp);
		}
	}
	return 0;
}

