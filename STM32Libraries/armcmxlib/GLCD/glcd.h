#ifndef	GLCD_H
#define GLCD_H

#include <inttypes.h>
//arm #include <avr/pgmspace.h>
#include "armcmx.h"
#include "Print.h"

#include "gText.h"
#include "GLCDController.h"

#define GLCD_VERSION 3 // software version of this library


/*
 * Note that while for now all these typedefs are the same they
 * may not be in the future since each is used to point to a different type of data
 */
typedef const uint8* Image; // a glcd format bitmap (includes width & height)
//typedef const uint8_t* ImageXBM_t; // a "xbm" format bitmap (includes width & height)

// the first two bytes of bitmap data are the width and height
#define ImageWidth(bitmap)  (*bitmap)  
#define ImageHeight(bitmap)  (*(bitmap+1))  

typedef const uint8* Font;  	

/**
 * @class glcd
 * @brief Functions for GLCD
 */
class glcd : public Print {
  private:
		GLCDController & gc;
		gText tx;
	
  public:
		glcd(GLCDController & dcont);
	
/** @name CONTROL FUNCTIONS
 * The following control functions are available
 */
/*@{*/
	// Control functions
	virtual void init(); //uint8_t invert = NON_INVERTED);

	virtual void GotoXY(uint8_t x, uint8_t y);  // overrride for GotoXY in device class
	inline virtual void WriteData(uint8 cmd) { gc.WriteData(cmd); }
	inline virtual uint8 ReadData(void) { return gc.ReadData(); }

	void SetDisplayMode(uint8_t mode); //NON_INVERTED or INVERTED,   was SetInverted(uint8_t invert);
/*@}*/
	
/** @name DRAWING FUNCTIONS
 * The following graphic functions are available
 */
/*@{*/
	// Graphic Functions
	void ClearScreen(uint8_t color = WHITE);
	void DrawVLine(uint8_t x, uint8_t y, uint8_t height, uint8_t color= BLACK);
	void DrawHLine(uint8_t x, uint8_t y, uint8_t width, uint8_t color= BLACK);
	void DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color= BLACK);
	void DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color= BLACK);
	void DrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color= BLACK);
	void FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color= BLACK);
	void InvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void DrawCircle(uint8_t xCenter, uint8_t yCenter, uint8_t radius, uint8_t color= BLACK);	
	void FillCircle(uint8_t xCenter, uint8_t yCenter, uint8_t radius, uint8_t color= BLACK);	
	void DrawBitmap(Image bitmap, uint8_t x, uint8_t y, uint8_t color = BLACK);

	// Font Functions
	void SelectFont(Font_t font, uint8_t color=BLACK) { tx.SelectFont(font, color); }  //, FontCallback callback=ReadPgmData); // default arguments added, callback now last arg
	void SetFontColor(uint8_t color) { tx.SetFontColor(color); } // new method
//	int PutChar(uint8_t c);
	// text functions
	virtual size_t write(uint8_t c) { return tx.write(c); }  // character output for print base class
	using Print::write;

/*
#ifdef DOXYGEN
*/
	/*
	 * Define functions to get them to show up properly
	 * in doxygen
	 */
	 /*
	void SetDot(uint8_t x, uint8_t y, uint8_t color) { lcdc.SetDot(x,y,color); }
	void SetPixels(uint8_t x, uint8_t y,uint8_t x1, uint8_t y1, uint8_t color) { lcdc.SetPixels(x,y,x1,y1, color); }
	uint8_t ReadData(void) { return lcdc.ReadData(); }        // now public
  void WriteData(uint8_t data) { lcdc.WriteData(data); } 
	*/
		/*
#else
	using glcd_Device::SetDot;
	using glcd_Device::SetPixels;
	using glcd_Device::ReadData; 
	using glcd_Device::WriteData; 
#endif
*/

/*@}*/

	//Device Properties - these are read only constants	 
//	static const uint8_t Width = DisplayController::Width; //DISPLAY_WIDTH; 	/**< Display width in pixels */
//	static const uint8_t Height = DISPLAY_HEIGHT;	/**< Display height in pixels */
//	static const uint8_t Right = DISPLAY_WIDTH-1;	/**< Right most pixel on Display (equals Width -1)*/
//	static const uint8_t Bottom = DISPLAY_HEIGHT-1; /**< Bottom most pixel on Display (equals Height -1)*/
//	static const uint8_t CenterX = DISPLAY_WIDTH/2;	/**< Horizontal center pixel on Display (equals Width/2)*/
//	static const uint8_t CenterY = DISPLAY_HEIGHT/2;/**< Vertical center pixel on Display (equals Height/2)*/
	
};

//extern glcd GLCD;
#endif
