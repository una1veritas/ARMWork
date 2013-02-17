
#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

#include "cmcore.h"

#include "spi.h"
#include "utility/stm32f4_discovery.h"

spi spi2bus = {SPI2, PB10, PC2, PC3, PB9};

void setup();
void loop();

uint8 transfer(uint8 * val, uint16 n) {
	spi_setDataMode(&spi2bus, SPI_MODE_Low1Edge);
	spi_setClockDivier(&spi2bus, SPI_CLOCK_DIV256);
	spi_setBitOrder(&spi2bus, SPI_LSBFIRST);
	digitalWrite(PB9, HIGH);
	delay(3);
	for(int i = 0; i < n; i++)
		spi_transfer(&spi2bus, val[i]);
	delay(1);
	digitalWrite(PB9, LOW);
}

int main(void) {
	
	setup();
	
forever:
	loop();
	goto forever;
}

void setup() {
	char tmp[64];
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
	usart_flush(&stdserial);

	spi_init(&spi2bus, SPI2, PB10, PC2, PC3, PB9);
	spi_begin(&spi2bus);
	digitalWrite(PB9, LOW);
	
	spi_setDataMode(&spi2bus, SPI_MODE_Low1Edge);
	spi_setClockDivier(&spi2bus, SPI_CLOCK_DIV256);
	spi_setBitOrder(&spi2bus, SPI_LSBFIRST);
	digitalWrite(PB9, HIGH);
	delay(3);
	spi_transfer(&spi2bus, 0x01);
	spi_transfer(&spi2bus, 0xaa);
	delay(1);
	digitalWrite(PB9, LOW);
	
}

void loop() {
}
