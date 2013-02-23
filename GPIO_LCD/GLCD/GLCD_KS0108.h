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

class GPIOBus {
public:
	GPIOPin EN; // Enable
	static GPIOPin CS[2];
	static GPIOPin DB[8];

public:
	GPIOBus() {
		EN = PD6;
	}
	void start(void);
	void address(uint8);
	void write(uint8);
		
};

class KS0108 {
	GPIOBus gpiobus;
	GPIOPin RS, RW, NRST;
	
	void init(void);
public:
	KS0108(void) {
		RS = PD4;
		RW = PD5;
		NRST = PD7;
		init();
	}

	static const uint8 CMD = LOW;
	static const uint8 DATA = HIGH;
	static const uint8 READ = LOW;
	static const uint8 WRITE = HIGH;
	
	void start(void);
	void ChipSelect(uint8 chip);
	void WriteCommand(uint8 cmd, uint8 chip);
	void WriteData(uint8 data, uint8 chip);
	void SetAddress(uint8 chip, uint8 page, uint8 column) ;

};

#endif // _GLCD_KS0108_H_
