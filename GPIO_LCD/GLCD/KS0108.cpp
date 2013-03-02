#include "KS0108.h"
#include "KS0108_params.h"

	void KS0108::chipselect(uint8 id) {
		digitout(CS[0], id == 0 ? LOW : HIGH);
		digitout(CS[1], id == 1 ? LOW : HIGH);
	}
	
	void KS0108::writebus(uint8 chip, uint8 di, uint8 val) {
		chipselect(chip);
		digitout(DNI, di);
		low(RNW);
		delay_us(1);
		high(ENCLK);
//	delay_us(1);
		out(val);
		delay_us(1);
		low(ENCLK);
//		delay_us(1);
//	high(ENCLK);
//		low(RNW);
//		chipselect(0xff);
		delay_us(1);
	}

	uint8 KS0108::readbus(uint8 chip, uint8 di) {
		uint8 val;
		chipselect(chip);
		digitout(DNI, di);
		high(RNW);
		delay_us(1);
		high(ENCLK);
		delay_us(1);
		low(ENCLK);
		val = in();
//		delay_us(1);
//		low(RNW);
//		chipselect(0xff);
		delay_us(1);
		return val;
	}

	void KS0108::WriteCommand(uint8 cmd, uint8 cs) {
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		DBMode(OUTPUT);
		writebus(cs, COMMAND, cmd);
	}
/*
	void KS0108::WriteCommand(uint8 cmd) {
		WriteCommand(cmd, address>>6 & 0x01 );
	}
*/
/*
	void KS0108::WriteData(uint8 data, uint8 cs) {
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		DBMode(OUTPUT);
		writebus(cs, DATA, data); 		
	}
*/
	void KS0108::WriteData(uint8 data) {
		uint8 chip = xyaddress>>6&1;
		WriteCommand(LCD_SET_PAGE | (xyaddress>>7&0x07), chip);
		WriteCommand(LCD_SET_ADDRESS | (xyaddress&0x3f), chip);
//		printf("W[%04x: %01x, %01x, %02x], ", xyaddress, xyaddress>>6&1, xyaddress>>7&0x07, xyaddress&0x3f);
		DBMode(INPUT);
		while ( (ReadStatus(chip) & 0x80) != 0 );
		DBMode(OUTPUT);
		writebus(chip, DATA, data);
		//
		xyaddress++;
	}

	uint8 KS0108::ReadStatus(uint8 cs) {
		return readbus(cs, COMMAND); 
	}

/*
	uint8 KS0108::ReadData(uint8 cs) {
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		res = readbus(cs, DATA); 
		return res;
	}
*/

	uint8 KS0108::ReadData(void) {
		uint8 chip = xyaddress>>6&1;
		WriteCommand(LCD_SET_PAGE | (xyaddress>>7&0x07), chip);
		WriteCommand(LCD_SET_ADDRESS | (xyaddress&0x3f), chip);
//		SetAddress(xyaddress>>6&1, xyaddress>>7&0x07, xyaddress&0x3f);
//		chip = xyaddress>>6&1;
//		printf("R[%04x: %01x, %01x, %02x], ", xyaddress, xyaddress>>6&1, xyaddress>>7&0x07, xyaddress&0x3f);
		DBMode(INPUT);
		while ( (ReadStatus(chip) & 0x80) != 0 );
		uint8 res = readbus(chip, DATA); 
		return res;
	}

	
	void KS0108::DBMode(GPIOMode_TypeDef mode){
		if ( busmode == mode )
			return;
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x00ff<<(D0 & 0x0f);
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = mode;
		GPIO_Init(PinPort(D0), &GPIO_InitStructure);
		busmode = mode;
//		delay_us(1);
	}
	
	void KS0108::init(uint8 inv) {
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		DBMode(OUTPUT);/*
		for(int i = 0; i < buswidth; i++) {
			pinMode(D0+i, OUTPUT);
		}*/
		Inverted = inv;
		On();
		StartAddress();
		// ClearScreen(Inverted ? 0x00 : 0xff);
	}

	void KS0108::On(void) {
		delay(10);
		WriteCommand(LCD_ON, 0);
		delay(10);
		WriteCommand(LCD_ON, 1);
		delay(300);
}

	void KS0108::StartAddress(uint8 pos) {
		WriteCommand(LCD_DISP_START | pos, 0);
		WriteCommand(LCD_DISP_START | pos, 1);
}

	void KS0108::XYAddress(uint8 chip, uint8 page, uint8 column){	
	//printf("A[%04x: %01x, %01x, %02x], ", xyaddress, xyaddress>>6&1, xyaddress>>7&0x07, xyaddress&0x3f);
		xyaddress = (page & 0x07)<<7 | (chip & 0x01)<<6 | (column & 0x3f);
		WriteCommand(LCD_SET_PAGE | page, chip);
		WriteCommand(LCD_SET_ADDRESS | column, chip);
	}
		
	void KS0108::SetDot(int16 x, int16 y, uint8 bw) {
		uint8 d;
		GotoXY(x,y);
		d = ReadData();
		if ( bw ) 
			WriteData(d | (1<<(y%8)) );
		else
			WriteData(d & ~(1<<(y%8)) );
	}
		
/*
#define distance(x, y)  ((x) - (y) >= 0 ? (x)-(y) : (y) - (x) )
#define swap(x, y, t) 	(t = x, y = x, x = t)
	
void KS0108::DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color) {
int16 dx, dy, x,y;
uint8 steep;
int8 error, ystep;
uint16 tmp;

	steep = distance(y1,y2) > distance(x1,x2);  

	if ( steep ) {
		swap(x1, y1, tmp);
		swap(x2, y2, tmp);
	}

	if (x1 > x2) {
		swap(x1, x2, tmp);
		swap(y1, y2, tmp);
	}

	dx = x2 - x1;
	dy = distance(y2,y1);  
	error = dx / 2;
	y = y1;
	if(y1 < y2) ystep = 1;  else ystep = -1;

	for(x = x1; x <= x2; x++) {
		if (steep) SetDot(y,x, color); else SetDot(x,y, color);
   		error = error - dy;
		if (error < 0) {
			y = y + ystep;
			error = error + dx;
    	}
	}
}


void KS0108::DrawVLine(int16 x, int16 y, int16 height, uint8 color){
  // this->FillRect(x, y, 0, length, color);
   SetPixels(x,y,x,y+height,color);
}
	
void KS0108::DrawHLine(int16 x, int16 y, int16 width, uint8 color){
   // this->FillRect(x, y, length, 0, color);
    SetPixels(x,y, x+width, y, color);
}
*/

void KS0108::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
uint8_t mask, pageOffset, h, i, data;
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
		
			if(color == BLACK) {
				data |= mask;
			} else {
				data &= ~mask;
			}
	
			WriteData(data);
		}
	}
}

