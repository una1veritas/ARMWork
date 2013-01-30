#ifndef _NOKIA5110_H_
#define _NOKIA5110_H_

#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>

#include "cmcore.h"
#include "SPIBus.h"

class Nokia5110 {
	GPIOPin pin_SCE;   // 7 //Pin 3 on LCD, ~CS
	GPIOPin pin_RESET; // 6 //Pin 4 on LCD, initiate reset
	GPIOPin pin_DC;    //5 //Pin 5 on LCD, Data/Command
	GPIOPin pin_SDO;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SDIN;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SCLK;  //  3 //Pin 7 on LCD, SCK

	SPIBus * spibus;
	uint16 textcursor; // bit column position
	uint8 fontid;
	
	//The DC pin tells the LCD if we are sending a command or data
	static const byte LCD_COMMAND = 0; 
	static const byte LCD_DATA  	= 1;

	//You may find a different size screen, but this one is 84 by 48 pixels
	static const uint16 LCD_X = 84;
	static const uint16 LCD_Y = 48;

	inline void select() {
		spibus->setMode(SPI_BaudRatePrescaler_64, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_FirstBit_MSB); // mode 3, msb first
		digitalWrite(pin_SCE, LOW);
	}
	
	inline void deselect() {
		digitalWrite(pin_SCE, HIGH);
	}
	
public:
	Nokia5110(SPIBus * spix, GPIOPin sce, GPIOPin dc, GPIOPin rst) {
		spibus = spix;
		pin_SCE = sce;   // 7 //Pin 3 on LCD, ~CS
		pin_DC = dc;
		pin_RESET = rst;
		textcursor = 0;
		fontid = FIXED8x5;
	}

	enum {
		FIXED8x5,
		CHICAGO10
	};
	
	static const byte SFEFlame[];
//	static const byte SFEFlameBubble [];
//	static const byte awesome[];
	static const byte ascii8x5[];
	static const byte Chicago10x15[];
	
	//
	void gotoXY(int x, int y);
	void drawBitmap(const byte my_array[]);
	void drawCharacter(char character);
	void drawString(char *characters);
	void clear(void);
	void init(void);
	void begin(void) { init(); }
	void write(byte data_or_command, byte data);
	
	void cursor(int col);
	void selectFont(uint8 id) { fontid = id; }
	void drawFixedFont(const byte font[], char c);
	void drawProportionalFont(const byte font[], char c);
//	void drawFontString(const byte font[], const byte hight, char *characters);
};

#endif
