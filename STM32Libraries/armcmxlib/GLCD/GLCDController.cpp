#include "armcmx.h"
#include "GLCDController.h"


	void GLCDController::init() {
		FgColor = BLACK;
		BkgColor = WHITE;
		Inverted = true;
		DisplayOn();
		// ClearScreen(Inverted ? 0x00 : 0xff);
	}

	void GLCDController::SetDot(int16 x, int16 y, uint8 bw) {
		uint8 d;
		if ( x < 0 || x >= Width 
			|| y < 0 || y >= Height )
			return;
		
		GotoXY(x,y);
		d = ReadData();
		if ( bw ) 
			d |= (1<<(y%Height));
		else
			d &= ~(1<<(y%Height));
		WriteData(d);
	}
	
	

void GLCDController::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
uint8_t mask, pageOffset, h, i, data;
//	x = min(max(x, 0), DISPLAY_WIDTH-1);
//	y = min(max(y, 0), DISPLAY_HEIGHT-1);
//	x2 = min(max(x2, 0), DISPLAY_WIDTH-1);
//	y2 = min(max(y2, 0), DISPLAY_HEIGHT-1);
uint8_t height = y2-y+1;
uint8_t width = x2-x+1;
	
	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;
	
	GotoXY(x, y);
	for(i=0; i < width; i++) {
		data = ReadData();
		if(color == BLACK) {
			data |= mask;
		} else {
			data &= ~mask;
		}
		WriteData(data);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		GotoXY(x, y);
		for(i=0; i <width; i++) {
			WriteData(color);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		GotoXY(x, y+8);
		for(i=0; i < width; i++) {
			data = ReadData();
		
			if( FgColor ) {
				data |= mask;
			} else {
				data &= ~mask;
			}
	
			WriteData(data);
		}
	}
}

