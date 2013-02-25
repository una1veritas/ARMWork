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
	GPIOMode_TypeDef busmode;
	uint8 lastcs;
	
public:
	enum LCDType {
		KS0108
	} lcdtype;
	
	GLCD(LCDType id) : lcdtype(id) {
		DNI = PE7;
		RNW = PD5;
		ENCLK = PD4;
		CS[1] = PD6;  // with resp. to the column byte direction
		CS[0] = PD7;
		D0 = PE8;
		buswidth = 8;
		busmode = OUTPUT;
		lastcs = 0;
	}
	
	void begin(void);
	void DBMode(GPIOMode_TypeDef mode);
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
	void ChipSelect(uint8 chip);
	
	void write(uint8 chip, uint8 di, uint8 val);
	uint8 read(uint8 chip, uint8 di);
	void WriteCommand(uint8 cmd, uint8 cs);
	inline void WriteCommand(uint8 cmd) { WriteCommand(cmd, lastcs); }
	void WriteData(uint8 data, uint8 cs);
	inline void WriteData(uint8 data) { WriteData(data, lastcs); }
	uint8 ReadStatus(uint8 cs);
	inline uint8 ReadStatus() { return ReadStatus(lastcs); }
	uint8 ReadData(uint8 cs);
	inline uint8 ReadData() { return ReadData(lastcs); }
	
	void On(void);
	void Address(uint8 chip, uint8 page, uint8 column);
	void GotoXY(int16 x, int16 y) {
		Address(1<<(x/CHIP_WIDTH),y/PAGE_HEIGHT, x%CHIP_WIDTH);
	}
	void PointXY(int16 x, int16 y, uint8 bw);
	void ClearScreen(uint16 color = 0x00);
};

#endif // _GLCD_H_
