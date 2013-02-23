#ifndef _GLCD_KS0108_H_
#define _GLCD_KS0108_H_

#include "armcmx.h"
	
#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_DISP_START		0xC0

#define LCD_SET_ADD			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_BUSY_FLAG		0x80 

// Colors
#define BLACK				0xFF
#define WHITE				0x00

#define CHIP_WIDTH 	64
#define CHIP_HEIGHT 	64
#define DISPLAY_WIDTH 	128
#define DISPLAY_HEIGHT 	64

#define CMD 	LOW
#define DATA  HIGH
#define READ 	LOW
#define WRITE HIGH

class KS0108 {
	GPIOPin RS, RW, EN;
	GPIOPin CS1, CS2;
public:
	KS0108(void) {
		RS = PD4;
		RW = PD5;
		EN = PD6;
		CS1 = PD0;
		CS2 = PD1;
	}
	
	void start(void);
	
};

#endif // _GLCD_KS0108_H_
