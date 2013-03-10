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
		WriteData(d);
	}
	
	

void GLCDController::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
  uint8 mask, pageOffset, h, i, data;
  //	x = min(max(x, 0), DISPLAY_WIDTH-1);
  //	y = min(max(y, 0), DISPLAY_HEIGHT-1);
  //	x2 = min(max(x2, 0), DISPLAY_WIDTH-1);
  //	y2 = min(max(y2, 0), DISPLAY_HEIGHT-1);
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
	
  // bottom fractional region
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

void GLCDController::MovePixels(const int16 left, const int16 top, const int16 right, const int16 bottom, const int16 xtransf, const int16 ytransf) {
  uint16 data;
  int16 xl = max(0, min(left, right)), xr = min(max(left, right), Width()-1);
  int16 yt = max(0, min(top, bottom)), yb = min(max(top, bottom), Height()-1);
  int16 dx = xtransf % Width();
  int16 dy = ytransf % Height();
  
  if (dx <= 0 && dy < 0) {
    // xl and yt are the dest sides
    printf("src y = %d to %d, dst y = %d to %d, ", yt, yb, yt+dy, dy+yb );
    printf("dx = %d, dy = %d\n", dx, dy);
    uint16 x = xl;
    uint8 offset;
    uint16 ydst;
    for ( ; x <= xr; x++) {
      for (uint16 ysrc = yt ; ysrc <= yb; ysrc += (8 - (ysrc % 8))) {
        ydst = ysrc + dy;
        GotoXY(x, ysrc);
        data = ReadData();
        data <<= 8;
        GotoXY(x, ysrc+ 8);
        data |= ReadData();
        if ( ysrc == yt && (ysrc % 8) != 0 ) {
          offset = yt % 8;
          data = (((data<<offset)>>8)&0xff)>>(ydst%8);
          GotoXY(x, ydst);
          WriteData(data&0xff);
        } else {
          offset = ((yt % 8) + 8 - (ydst % 8)) % 8;
          data = ((data<<offset)>>8)&0xff;
          GotoXY(x, ydst);
          WriteData(data);
        }
      }
    }
    printf("xr = %d\n", xr);
  }
  printf("\n");
}
