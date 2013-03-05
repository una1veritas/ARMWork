#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "armcmx.h"

// Colors
#define WHITE				0x00
#define BLACK				0xff

class Controller {

	static const uint8 COMMAND = 0;
	
public:
	static uint16 Width;
	static uint16 Height;
	
	uint8 backColor;
	uint8 foreColor;

public:
	
	virtual void init(void) = 0;
		
	virtual uint8 IsBusy(void) = 0;
	virtual void WriteData(uint8 cmd) = 0;
	virtual uint8 ReadData(void) = 0;
	
	virtual void DisplayOn(void) = 0;
	virtual void DisplayOff(void) {}
	virtual void SetAddress(uint8 pg, uint8 col) = 0;
	virtual void GotoXY(int16 x, int16 y) = 0;
	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
//	void SetColumnByte(int16 x, int16 y uint8 bits);
	void SetBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t bw);
		
};

#endif // _GLCD_H_
