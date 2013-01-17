#ifndef _NOKIA5110_H_
#define _NOKIA5110_H_

#include "armcore.h"
#include "spibus.h"

struct PCD8544 {
	GPIOPin pin_SCE;   // 7 //Pin 3 on LCD, ~CS
	GPIOPin pin_RESET; // 6 //Pin 4 on LCD, initiate reset
	GPIOPin pin_DC;    //5 //Pin 5 on LCD, Data/Command
	GPIOPin pin_SDO;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SDIN;  //  4 //Pin 6 on LCD, MOSI
	GPIOPin pin_SCLK;  //  3 //Pin 7 on LCD, SCK

	SPIBus * spibus;
} LCD;

	//The DC pin tells the LCD if we are sending a command or data
#define LCD_COMMAND = 0 
#define LCD_DATA  	= 1

//You may find a different size screen, but this one is 84 by 48 pixels
#define LCD_X 84
#define LCD_Y 48

	static const byte SFEFlame[];
//	static const byte SFEFlameBubble [];
//	static const byte awesome[];
	static const byte ascii8x5[];
	static const byte Chicago10x15[];
	
//
void LCD_Init(void);
	//
	void gotoXY(int x, int y);
	void drawBitmap(const byte my_array[]);
	void drawCharacter(char character);
	void drawString(char *characters);
	void LCD_Clear(void);
	void init(void);
	void begin(void) { init(); }
	void write(byte data_or_command, byte data);
	
	#endif
