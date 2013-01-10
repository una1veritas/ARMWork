#ifndef _NOKIA5110_H_
#define _NOKIA5110_H_

#include "armcore.h"
#include "gpio.h"
#include "delay.h"
#include "SPI.h"

class Nokia5110 {
	GPIOPin pin_SCE;   // 7 //Pin 3 on LCD, ~CS
	GPIOPin pin_RESET; // 6 //Pin 4 on LCD, initiate reset
	GPIOPin pin_DC;    //5 //Pin 5 on LCD, Data/Command
	GPIOPin pin_SDO;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SDIN;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SCLK;  //  3 //Pin 7 on LCD, SCK

	SPIBuffer * SPIBx;
	
	//The DC pin tells the LCD if we are sending a command or data
	static const byte LCD_COMMAND = 0; 
	static const byte LCD_DATA  	= 1;

	//You may find a different size screen, but this one is 84 by 48 pixels
	static const uint16 LCD_X = 84;
	static const uint16 LCD_Y = 48;

public:
	Nokia5110(SPIBuffer * spix, GPIOPin sce, GPIOPin dc, GPIOPin rst) {
		SPIBx = spix;
		pin_SCE = sce;   // 7 //Pin 3 on LCD, ~CS
		pin_DC = dc;
		pin_RESET = rst;
	}
	
	static const byte ASCII[][5];
	static const byte SFEFlame[];
	static const byte SFEFlameBubble [];
	static const byte awesome[];

	void gotoXY(int x, int y);
	void drawBitmap(const byte my_array[]);
	void drawCharacter(char character);
	void drawString(char *characters);
	void clear(void);
	void init(void);
	void write(byte data_or_command, byte data);
};

#endif
