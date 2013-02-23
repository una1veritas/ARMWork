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
	GPIOPin CLK; // FSMC_D8, _NWE, _NOE
	GPIOPin AB[4];  // FSMC_A16, _A17, FSMC_NWAIT
	GPIOPin DB[8]; // FSMC_D0 --- _D7
public:
<<<<<<< HEAD
	FSMCBus(void) {
		RS = PE15;
		RW = PD5;
		EN = PD4; // FSMC_D8, _NWE, _NOE
		CS1 = PD8;
		CS2 = PD9;
		NRST = PD6;  // FSMC_A16, _A17, FSMC_NWAIT
=======
	GPIOBus() {}
	GPIOBus(GPIOPin data[], GPIOPin addr[], GPIOPin en) {
		init(data, addr, en);
>>>>>>> origin/macbook
	}
	void init(GPIOPin data[], GPIOPin addr[], GPIOPin en) {
		CLK = en; //PD4; // FSMC_D8, _NWE, _NOE
		for(int i = 0; i < 4; i++)
			AB[i] = addr[i]; //0 => D/I, 1 => R/nW, 2 -- => CS1, CS2, 
		for(int i = 0; i < 8; i++)
			DB[i] = data[i]; //0 -- 7 => data
	}
	void start(void);
	void address(uint8);
	void write(uint8);
		
};

class KS0108 {
	GPIOBus gpiobus;
	GPIOPin nRST;
	
	void init(void);
public:
	KS0108(void) {
		GPIOPin data[] = { PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14 };
		GPIOPin address[] = { PD4, PD5, PD0, PD1 };
		gpiobus.init(data, address, PD6);
		nRST = PD8;
	}
		
	void start(void);
	void ChipSelect(uint8 chip);
	void WriteCommand(uint8 cmd, uint8 chip);
	void WriteData(uint8 data, uint8 chip);
	void SetAddress(uint8 chip, uint8 page, uint8 column) ;

};

#endif // _GLCD_KS0108_H_
