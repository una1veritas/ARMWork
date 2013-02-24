#ifndef _GLCD_H_
#define _GLCD_H_

#include "armcmx.h"
#include "KS0108_compat.h"

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
	
	void begin(void);
	inline void high(GPIOPin pin) { GPIO_SetBits(PinPort(pin), PinBit(pin)); }
	inline void low(GPIOPin pin) { GPIO_ResetBits(PinPort(pin), PinBit(pin)); }
	inline void digitout(GPIOPin pin, uint8 bval) { GPIO_WriteBit(PinPort(pin), PinBit(pin), bval? Bit_SET : Bit_RESET ); }
	inline void out(uint16 data) {
		uint16 mask = GPIO_ReadOutputData(PinPort(D0));
		mask &= ~(0x00ff << (D0 & 0x0f));
		GPIO_Write(PinPort(D0), mask | (data<< (D0 & 0x0f))); 
	}
	inline uint16 in(void) {
		uint16 val = GPIO_ReadInputData(PinPort(D0));
		return (val >> (D0 & 0x0f)) & 0xff;
	}
	void ChipSelect(uint8 chip) {
		chip & 1 ? low(CS[0]) : high(CS[0]);
		chip & 2 ? low(CS[1]) : high(CS[1]);
	}
	
	void write(uint8 chip, uint8 di, uint8 val);
	void WriteCommand(uint8 cmd, uint8 cs) { write(cs, COMMAND, cmd); }
	void WriteData(uint8 data, uint8 cs) { write(cs, DATA, data); }
	void On(void);
	void Address(uint8 chip, uint8 page, uint8 column);
	void ClearScreen(uint16 color = 0x00);
};

#endif // _GLCD_H_
