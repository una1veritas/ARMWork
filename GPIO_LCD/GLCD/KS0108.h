#ifndef _GLCD_H_
#define _GLCD_H_

#include "armcmx.h"
#include "Print.h"
#include "KS0108_params.h"

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
	uint16 xyaddress;
	
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
	void StartAddress(uint8 pos = 0x00);
	void XYAddress(uint8 chip, uint8 page, uint8 column);	
	void GotoXY(int16 x, int16 y) {
		XYAddress(x/CHIP_WIDTH,y/PAGE_HEIGHT, x%CHIP_WIDTH);
	}
//	void SetDot(int16 x, int16 y, uint8 bw);
//	void ClearScreen();
//	void DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 bw = BLACK);
//	void DrawVLine(uint8_t x, uint8_t y, uint8_t height, uint8_t color= BLACK);
//	void DrawHLine(uint8_t x, uint8_t y, uint8_t width, uint8_t color= BLACK);
//	void SetPixels(uint8_t x, uint8_t y,uint8_t x2, uint8_t y2, uint8_t color);
	
//	void ClearScreen(uint8 color = WHITE);
//	void DrawVLine(int16 x, int16 y, int16 height, uint8 color= BLACK);
//	void DrawHLine(int16 x, int16 y, int16 width, uint8 color= BLACK);
//	void DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color= BLACK);
//	void DrawRect(int16 x, int16 y, int16 width, int16 height, uint8 color= BLACK);
//	void DrawRoundRect(int16 x, int16 y, int16 width, int16 height, int16 radius, uint8_t color= BLACK);
//	void FillRect(int16 x, int16 y, int16 width, int16 height, uint8_t color= BLACK);
//	void InvertRect(int16 x, int16 y, int16 width, int16 height);
//	void DrawCircle(int16 xCenter, int16 yCenter, int16 radius, uint8_t color= BLACK);	
//	void FillCircle(int16 xCenter, int16 yCenter, int16 radius, uint8_t color= BLACK);	
	void DrawBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t color= BLACK);

	void DrawBitmapXBM(const uint8 * bitmapxbm, int16 x, int16 y, uint8_t color= BLACK);
//	void DrawBitmapXBM_P(uint8_t width, uint8_t height, uint8_t *xbmbits, uint8_t x, uint8_t y, uint8_t fg_color, uint8_t bg_color);

	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
//	uint8_t ReadData(void);        // now public
//  void WriteData(uint8_t data); 

};

#endif // _GLCD_H_
