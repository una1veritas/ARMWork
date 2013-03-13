#include "armcmx.h"
#include "GLCDController.h"


	void GLCDController::init() {
		DisplayOn();
		// ClearScreen(Inverted ? 0x00 : 0xff);
	}

	void GLCDController::SetDot(int16 x, int16 y, uint8 bw) {
		uint8 d;
		if ( x < 0 || x >= Width() || y < 0 || y >= Height() )
			return;
		
		GotoXY(x,y);
		d = ReadData();
		if ( bw ) 
			d |= (1<<(y % Height()));
		else
			d &= ~(1<<(y % Height() ));
		GotoXY(x,y);
		WriteData(d);
	}
	
	

void GLCDController::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
  uint8 mask, pageOffset, h, i, data;
  uint8 height = y2-y+1;
  uint8 width = x2-x+1;
	
	pageOffset = y % 8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;

  // top fractional region	
	for(i=0; i < width; i++) {
    GotoXY(x+i, y);
		data = ReadData();
		if(color == BLACK) {
			data |= mask;
		} else {
			data &= ~mask;
		}
    GotoXY(x+i, y);
		WriteData(data);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		for(i=0; i <width; i++) {
      GotoXY(x+i, y);
			WriteData(color);
		}
	}
	
  // bottom fractional region
	if(h < height) {
		mask = ~(0xFF << (height-h));
		for(i=0; i < width; i++) {
      GotoXY(x+i, y+8);
			data = ReadData();
		
			if( FgColor ) {
				data |= mask;
			} else {
				data &= ~mask;
			}
			GotoXY(x+i, y+8);
			WriteData(data);
		}
	}
}

void GLCDController::MovePixels(int16 x, int16 y, int16 x2, int16 y2, int16 dx, int16 dy) {
  uint8 mask, pageOffset, h, i, data;
  uint8 height = y2-y+1;
  uint8 width = x2-x+1;
  uint16 data16;/*
  int16 xl = max(0, min(left, right)), xr = min(max(left, right), Width()-1);
  int16 yt = max(0, min(top, bottom)), yb = min(max(top, bottom), Height()-1);
  int16 dx = xtransf % Width();
  int16 dy = ytransf % Height();*/

	pageOffset = y % 8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;

  // top fractional region	
	for(i=0; i < width; i++) {
    GotoXY(x+i, y);
		data16 = ReadData();
    data16 <<= 8;
    GotoXY(x+i, y+8);
		data16 |= ReadData();
    data16 >>= (y%8);
    data16 <<= (y+dy)%8;
    GotoXY(x+i+dx, y+dy);
		data = ReadData();
    data16 = (data16 & mask) | (data & ~mask);
    GotoXY(x+i+dx, y+dy);
		WriteData(data16);
	}

  uint8 th = h;
  uint8 ty = y;
		for(i=0; i <width; i++) {
      h = th;
      y = ty;
      GotoXY(x+i, y+8);
      data16 = ReadData();
	while(h+8 <= height) {
		h += 8;
		y += 8;
//      GotoXY(x+i, y);
//      data16 = ReadData();
      data16 <<= 8;
      GotoXY(x+i, y+8);
      data16 |= ReadData();
      data16 >>= (y%8);
      GotoXY(x+i+dx, y+dy);
			WriteData(data16);
		}
	}
  h = th;
  y = ty;
	
  // bottom fractional region
	if(h < height) {
		mask = ~(0xFF << (height-h));
		for(i=0; i < width; i++) {
      GotoXY(x+i, y);
      data16 = ReadData();
      data16 <<= 8;
      GotoXY(x+i, y+8);
      data16 |= ReadData();
      data16 >>= (y%8);
		
			GotoXY(x+i, y+8);
      data = ReadData();
      data16 = (data16 & mask) | (data & ~mask);      
			GotoXY(x+i, y+8);
			WriteData(data);
		}
	}
}
