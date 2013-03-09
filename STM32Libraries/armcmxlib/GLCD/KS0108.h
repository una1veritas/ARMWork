#ifndef _KS0108_H_
#define _KS0108_H_

#include "armcmx.h"
#include "GLCDController.h"

#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_DISP_START		0xC0

#define LCD_SET_ADDRESS			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_BUSY_FLAG		0x80 

#define CHIP_WIDTH 	64
#define CHIP_HEIGHT 	64
#define CHIP_PAGES 	8
#define PAGE_WIDTH	 	64
#define PAGE_HEIGHT 	8

#define COMMAND 	LOW
#define DATA  HIGH
#define READ 	LOW
#define WRITE HIGH

// Colors
#define NON_INVERTED true

class KS0108 : public GLCDController {
	GPIOPin DNI, RNW;
	GPIOPin ENCLK;
	GPIOPin CS[2];
	GPIOPin D0;
//	uint8 buswidth;
	GPIOMode_TypeDef busmode;
		
	uint16 xyaddress;
	
  public:
  // constant definitions 
  const static uint8 DISPLAY_WIDTH = 128;
  const static uint8 DISPLAY_HEIGHT = 64;

	void busMode(GPIOMode_TypeDef mode);
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
	void select(uint8 chip);
	void writebus(uint8 chip, uint8 di, uint8 val);
	uint8 readbus(uint8 chip, uint8 di);
	void setXY(uint8 x, uint8 y);

	void WriteCommand(uint8 cmd);
	uint8 ReadStatus(void);

public:
	KS0108() {
		DNI = PE7;
		RNW = PD5;
		ENCLK = PD4;
		CS[1] = PD6;  // with resp. to the column byte direction
		CS[0] = PD7;
		D0 = PE8;
		busmode = OUTPUT;
    
    Width = DISPLAY_WIDTH;
    Height = DISPLAY_HEIGHT;
		BkgColor = WHITE;
		FgColor = BLACK;
		Inverted = true;
	}
	
	virtual void init(void);
	virtual void GotoXY(uint8 x, uint8 y);
	virtual uint8 IsBusy(void) { return ReadStatus() & 0x80; }
	virtual void WriteData(uint8 data);
	virtual uint8 ReadData();

	virtual void DisplayOn(void);
	//void SetBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t color= BLACK);
	//void SetDot(int16 x, int16 y, uint8 bw);
	//void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
	//void SetColumnByte(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
};

#endif // _GLCD_H_
