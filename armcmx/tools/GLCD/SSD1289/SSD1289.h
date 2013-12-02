#ifndef __SSD1289_H
#define __SSD1289_H

#include "stm32f4xx.h"
#include "AsciiLib.h"

// armcmx delay
#include "delay.h"

#define Delay(x)  delay_us((x)/10)

typedef struct 
{
  int16_t X;
  int16_t Y;
} Point, * pPoint;   

class SSD1289 {
  
#define REG_DRIVER_OUTPUT_CONTROL 	0x01
#define REG_DISPLAY_CONTROL 	0x07
#define REG_ENTRY_MODE 				0x11
#define REG_RAM_DATA			 		0x22
#define REG_SET_GDDRAM_X  		0x4E
#define REG_SET_GDDRAM_Y  		0x4F

#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

#define   BLACK        0x0000
#define   NAVY         0x000F
#define   DGREEN       0x03E0
#define   DCYAN        0x03EF
#define   MAROON       0x7800
#define   PURPLE       0x780F
#define   OLIVE        0x7BE0
#define   GREY         0xF7DE
#define   LGRAY        0xC618
#define   DGRAY        0x7BEF
#define   BLUE         0x001F
#define   GREEN        0x07E0
#define   CYAN         0x07FF
#define   RED          0xF800
#define   MAGENTA      0xF81F
#define   YELLOW       0xFFE0
#define   WHITE        0xFFFF

#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

#define LCD_PIXEL_HEIGHT          0x0140
#define LCD_PIXEL_WIDTH         0x00F0

#define ASSEMBLE_RGB(R ,G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3)) 

  const static uint16 BIT_RL = 1<<14; // right left
  const static uint16 BIT_REV = 1<<13;
//  const static uint16 BIT_CAD = 1<<12;
//  const static uint16 BIT_BGR = 1<<11;
//  const static uint16 BIT_SM = 1<<10; // interlace
  const static uint16 BIT_TB = 1<<9;  // top bottom
  const static uint16 DRIVEROUTPUT_BASE = 0x093F; // BGR, MUX8,MUX5:0
//  const static uint16 BIT_HINCREMENT = 1<<4;
//  const static uint16 BIT_VINCREMENT = 1<<5;
  const static uint16 BIT_AM = 1<<3;
  const static uint16 ENTRYMODE_BASE = 0x6070;
  
private:
	GPIOPin NRSTpin;
	uint16 textCursorX, textCursorY;
	uint16 deviceCode;

//	uint16 driveroutput, entrymode;
  uint8 reg_setx, reg_sety;
	uint16 width, height;

private:
	void TIM_Config(void);
	void CtrlLinesConfig(void);
	void FSMCConfig(void);

public:
  /* Global variables to set the written text color */
	uint16_t fgColor ;
	uint16_t bgColor ;
	uint16_t charsize ;
	uint16_t TimerPeriod  ;
	uint16_t Channel3Pulse ;

	void WriteRAM_Prepare(void);
	void WriteRAM(uint16_t RGB_Code);
	void WriteReg(uint8_t Reg, uint16_t RegValue);	
	uint16 ReadReg(uint8_t LCD_Reg);
	void WriteBMP(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint8_t *bitmap);

	void PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);

  enum DISPLAY_ORIGIN {
    PORTRAIT = 0, // connecter ribbon side is up
    LANDSCAPE,
    PORTRAIT_REV,
    LANDSCAPE_REV,
  };
  
public:
	SSD1289() {
		NRSTpin = PC5;
		
		textCursorX = 0;
		textCursorY = 0;
		deviceCode = 0;
    reg_setx = REG_SET_GDDRAM_X;
    reg_sety = REG_SET_GDDRAM_Y;
    width = LCD_PIXEL_WIDTH;
    height = LCD_PIXEL_HEIGHT;
		//
    //driveroutput = DRIVEROUTPUT_BASE | BIT_RL | BIT_REV | BIT_BGR | BIT_TB;
    //sentrymode = ENTRYMODE_BASE | ID1_VINCREMENT | ID0_HINCREMENT | AM_VERTICAL_LOWER;
    //
		fgColor = 0x0000;
		bgColor = 0xFFFF;
		charsize = 12;
		TimerPeriod    = 0;
		Channel3Pulse  = 0;
	}
		
	void init();
	void start(void);
	void DisplayOn(void);
	void DisplayOff(void);
	void BackLight(int percent);
	void displayOrientation(uint8 d);
	
	uint16 device(void) { return deviceCode; }
	
	void SetColors(uint16_t _TextColor, uint16_t _BackColor); 
	void GetColors(uint16_t *_TextColor, uint16_t *_BackColor);
	void SetTextColor(uint16_t Color);
	void SetBackColor(uint16_t Color);
	void clear(uint16_t Color);
	void clear() { clear(bgColor); }
	void SetCursor(uint16_t Xpos, uint16_t Ypos);

	void textSize(uint16_t size);
	void textCursor(uint16 c, uint16 r) { textCursorX = c*charsize; textCursorY = r*charsize; }
	void PutChar(int16_t PosX, int16_t PosY, char c);
	void print(char c);
	void StringLine(uint16_t PosX, uint16_t PosY, const char *str);
	void print(const char *str);

	void PutPixel(int16_t x, int16_t y);
	void PutPixel(int16_t x, int16_t y,int16_t c);

	void DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
	void DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
	void DrawSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);
	void DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
	void DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
	void DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
	void PolyLine(pPoint Points, uint16_t PointCount);
	void PolyLineRelative(pPoint Points, uint16_t PointCount);
	void ClosedPolyLine(pPoint Points, uint16_t PointCount);
	void ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
	void FillPolyLine(pPoint Points, uint16_t PointCount);
	void SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
	void DrawFullSquare(uint16_t Xpos, uint16_t Ypos, uint16_t a);

};

#endif 
