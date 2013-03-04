#ifndef _GLCDCONTROLLER_H_
#define _GLCDCONTROLLER_H_

#include "armcmx.h"

// Colors
#define WHITE				0x00
#define BLACK				0xff

class GLCDController {

	static const uint8 COMMAND = 0;
	
public:
	static uint16 Width;
	static uint16 Height;
	
	uint8 backColor;
	uint8 foreColor;

public:
	
	void begin(void) { init(); }
	virtual void init(void) = 0;
		
	virtual uint8 IsBusy(void) = 0;
	virtual void WriteCommand(uint8 cmd) = 0;
	virtual uint8 ReadStatus(void) = 0;
	virtual void WriteData(uint8 cmd) = 0;
	virtual uint8 ReadData(void) = 0;
	
	virtual void displayOn(void) = 0;
	virtual void displayOff(void) {}
	virtual void SetAddress(uint8 pg, uint8 col) = 0;
	virtual void GotoXY(int16 x, int16 y) = 0;
	void DrawBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t color);
	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
		
};

#endif // _GLCD_H_
