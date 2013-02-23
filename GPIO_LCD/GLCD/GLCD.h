#ifndef _GLCD_H_
#define _GLCD_H_

#include "armcmx.h"
	
class GLCD {
	GPIOPin DNI, RNW;
	GPIOPin ENCLK;
	GPIOPin CS[2];
	GPIOPin D0;
	uint8 buswidth;
	
public:
	enum LCDType {
		KS0108
	} lcdtype;
	
	GLCD(LCDType id) : lcdtype(id) {
		DNI = PE7;
		RNW = PD5;
		ENCLK = PD4;
		CS[0] = PD6;
		CS[1] = PD7;
		D0 = PE8;
		buswidth = 8;
	}
	
	void start(void);
	void high(GPIOPin pin) { GPIO_SetBits(PinPort(pin), PinBit(pin)); }
	void low(GPIOPin pin) { GPIO_ResetBits(PinPort(pin), PinBit(pin)); }
	void out(uint16 data) {
		uint16 mask = GPIO_ReadOutputData(PinPort(D0));
		mask &= ~(0x00ff << (D0 & 0x0f));
		GPIO_Write(PinPort(D0), mask | (data<< (D0 & 0x0f))); 
	}
	uint16 in(void) {
		uint16 val = GPIO_ReadInputData(PinPort(D0));
		return (val >> (D0 & 0x0f)) & 0xff;
	}
	void ChipSelect(uint8 chip) {
		high(CS[0]);
		high(CS[1]);
		low(CS[chip]);
	}
	
};

#endif // _GLCD_H_
