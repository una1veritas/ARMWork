#ifndef _GLCD_H_
#define _GLCD_H_

#include "armcmx.h"
#include "Print.h"

//#include "KS0108_params.h"
#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_DISP_START		0xC0

#define LCD_SET_ADDRESS			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_BUSY_FLAG		0x80 

// Colors
#define BLACK				0xFF
#define WHITE				0x00
#define NON_INVERTED true

#define CHIP_WIDTH 	64
#define CHIP_HEIGHT 	64
#define DISPLAY_WIDTH 	128
#define DISPLAY_HEIGHT 	64
#define CHIP_PAGES 	8
#define PAGE_WIDTH	 	64
#define PAGE_HEIGHT 	8

#define COMMAND 	LOW
#define DATA  HIGH
#define READ 	LOW
#define WRITE HIGH

class KS0108 : public Print {
	GPIOPin DNI, RNW;
	GPIOPin ENCLK;
	GPIOPin CS[2];
	GPIOPin D0;
	uint8 buswidth;
	GPIOMode_TypeDef busmode;
		
	uint8 backColor;
	uint8 foreColor;
	//uint8 page, column;
	uint16 pageaddress;
	
	uint16 textcursor; // bit column position
	uint8 * font;

public:
	static const uint16 Width = 128;
	static const uint16 Height = 64;


	uint8 Inverted;

public:
	KS0108() {
		DNI = PE7;
		RNW = PD5;
		ENCLK = PD4;
		CS[1] = PD6;  // with resp. to the column byte direction
		CS[0] = PD7;
		D0 = PE8;
		buswidth = 8;
		busmode = OUTPUT;
		backColor = 0x00;
		
		Inverted = true;
	}
	
	void begin(void) { init(Inverted); }
	void init(uint8 inv);
	
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
	void chipselect(uint8 chip);
	void writebus(uint8 chip, uint8 di, uint8 val);
	uint8 readbus(uint8 chip, uint8 di);
	
	void WriteCommand(uint8 cmd, uint8 cs);
	uint8 ReadStatus(uint8 cs);
//	void WriteCommand(uint8 cmd);
//	void WriteData(uint8 data, uint8 cs);
	void WriteData(uint8 data);
//	uint8 ReadStatus();
//	uint8 ReadData(uint8 cs);
	uint8 ReadData();
	
	void On(void);
	void StartAddress(uint8 pos, uint8 chip);
//	void SetAddress(uint8, uint8);
//	void SetPage(uint8, uint8);
	void SetPageAddress(uint8 chip, uint8 page, uint8 column);
	void SetPageAddress(void);
	void GotoXY(int16 x, int16 y) {
		SetPageAddress(x/CHIP_WIDTH,y/PAGE_HEIGHT, x%CHIP_WIDTH);
		//printf("GotoXY(%d, %d) [%d, %d, %d] \n", x, y, x/CHIP_WIDTH, y/PAGE_HEIGHT, x%CHIP_WIDTH);
	}
	void BackPageAddress(uint8 val = 1);
	void DrawBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t color= BLACK);
	void DrawBitmapXBM(const uint8 * bitmapxbm, int16 x, int16 y, uint8_t color= BLACK);

	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
	void Clear(uint8 bw = WHITE);
};

#endif // _GLCD_H_
