#ifndef _GLCDController_H_
#define _GLCDController_H_

#include "armcmx.h"

// Colors
#define WHITE				0x00
#define BLACK				0xff

class GLCDController {

	static const uint8 COMMAND = 0;

public:
	uint8 BkgColor;
	uint8 FgColor;
	uint8 Inverted;

public:	
  GLCDController(void) : BkgColor(WHITE), FgColor(BLACK), Inverted(false) {}
    
	virtual void init(void);
	virtual void DisplayOn(void) = 0;
	virtual void DisplayOff(void) {}
	virtual uint16 Width(void) = 0;
  virtual uint16 Height(void) = 0;
  
	virtual uint8 IsBusy(void) = 0;
	virtual void WriteData(uint8 cmd) = 0;
	virtual uint8 ReadData(void) = 0;
	virtual void GotoXY(uint8 x, uint8 y) = 0;
	
	void SetDot(int16 x, int16 y, uint8 bw);
	void SetPixels(int16 x, int16 y, int16 x1, int16 y1, uint8 bw);
//	void SetColumnByte(int16 x, int16 y uint8 bits);
	void SetBitmap(const uint8* bitmap, int16 x, int16 y, uint8_t bw);
  void MovePixels(const int16 left, const int16 top, const int16 right, const int16 bottom, const int16 xtransf, const int16 ytransf);
    
};

#endif // _GLCDController_H_
