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

class FSMCBus {
public:
	GPIOPin RS, RW, EN; // FSMC_D8, _NWE, _NOE
	GPIOPin CS1, CS2, NRST;  // FSMC_A16, _A17, FSMC_NWAIT
	static const GPIOPin DB[16]; // FSMC_D0 --- _D7
public:
	FSMCBus(void) {
		RS = PE11;
		RW = PD5;
		EN = PD4; // FSMC_D8, _NWE, _NOE
		CS1 = PD11;
		CS2 = PD12;
		NRST = PD6;  // FSMC_A16, _A17, FSMC_NWAIT
	}
	void init8bus(void);
	void modeConfig(void);
	uint16 write(uint16 w);
		
};

class KS0108 {
	FSMCBus fsmcbus;
	
	void init(void);
public:
	KS0108(void) {
		init();
	}
		
	void start(void);
	void ChipSelect(uint8 chip);
	void WriteCommand(uint8 cmd, uint8 chip);
	void WriteData(uint8 data, uint8 chip);
	void SetAddress(uint8 chip, uint8 page, uint8 column) ;

};

#endif // _GLCD_KS0108_H_
