#ifndef _GLCDCONTROLLER_H_
#define _GLCDCONTROLLER_H_

#include "armcmx.h"

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
		
	virtual void Write(uint8 rs, uint8 data) = 0;
	virtual uint8 Read(uint8 rs) = 0;
	virtual uint8 IsBusy(void) = 0;
	void WriteCommand(uint8 cmd) { Write(COMMAND, cmd); }
	uint8 ReadCommand(void) { return Read(COMMAND); }
	void WriteData(uint8 cmd) { Write(~COMMAND, cmd); }
	uint8 ReadData(void) { return Read(~COMMAND); }
	
	virtual void displayOn(void) {}
	virtual void displayOff(void) {}
	void SetAddress(uint8 pg, uint8 col) {}
	void GotoXY(int16 x, int16 y) {}
	void DrawBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t color);
	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
		
};

#endif // _GLCD_H_
