#ifndef _PCD8544_H_
#define _PCD8544_H_

#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>

#include "cmcore.h"
#include "spi.h"

class PCD8544 {
	spi & spibus;
	GPIOPin pin_SCE;   // 7 //Pin 3 on LCD, ~CS
	GPIOPin pin_RESET; // 6 //Pin 4 on LCD, initiate reset
	GPIOPin pin_DC;    //5 //Pin 5 on LCD, Data/Command
	GPIOPin pin_SDO;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SDIN;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SCLK;  //  3 //Pin 7 on LCD, SCK
	
	uint16 textcursor; // bit column position
	const uint8 * font;
		
	//The DC pin tells the LCD if we are sending a command or data
	static const byte LCD_COMMAND = 0; 
	static const byte LCD_DATA  	= 1;

	//You may find a different size screen, but this one is 84 by 48 pixels
	static const uint16 LCD_X = 84;
	static const uint16 LCD_Y = 48;

	inline void select() {
		spi_setMode(&spibus, SPI_BaudRatePrescaler_32, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_FirstBit_MSB); // mode 3, msb first
		digitalWrite(pin_SCE, LOW);
	}
	
	inline void deselect() {
		digitalWrite(pin_SCE, HIGH);
	}
	
public:
	PCD8544(spi & bus, GPIOPin sce, GPIOPin dc, GPIOPin rst) : spibus(bus) {
		pin_SCE = sce;   // 7 //Pin 3 on LCD, ~CS
		pin_DC = dc;
		pin_RESET = rst;
		textcursor = 0;
		font = Fixed_8w5;
	}
	
	static const byte Fixed_8w5[];
	static const byte SFEFlame[];
//	static const byte SFEFlameBubble [];
//	static const byte awesome[];
	
	//
	void gotoXY(int x, int y);
	void drawBitmap(const byte my_array[], const byte width = LCD_X, const byte height = LCD_Y);
	void drawCharacter(const char character);
	void drawString(const char *characters);
	void clear(void);
	void init(void);

	void setContrast(const uint8 val);
	void begin(void) { init(); }
	void write(byte data_or_command, byte data);
	
	void cursor(int col);
	void setFont(const uint8 f[]) { font = (uint8 *) f; }
	void drawFixedFont(const byte font[], char c);
	void drawProportionalFont(const byte font[], char c);
//	void drawFontString(const byte font[], const byte hight, char *characters);
};

#endif
