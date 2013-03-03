#include "armcmx.h"
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

	uint8 KS0108::ReadStatus(uint8 cs) {
		return readbus(cs, COMMAND); 
	}

	void KS0108::WriteData(uint8 data) {
		uint8 chip = pageaddress>>9 & 1;
		DBMode(INPUT);
		delay_us(1);
		while ( (ReadStatus(chip) & 0x80) != 0 );
		DBMode(OUTPUT);
		delay_us(1);
		writebus(chip, DATA, data);
		//
		pageaddress++;
	}

	uint8 KS0108::ReadData(void) {
		uint8 chip = pageaddress>>9 & 1;
		DBMode(INPUT);
		delay_us(1);
		while ( (ReadStatus(chip) & 0x80) != 0 );
		uint8 res = readbus(chip, DATA); 
		pageaddress++;
		return res;
	}
	
	void KS0108::DBMode(GPIOMode_TypeDef mode){
		GPIO_InitTypeDef GPIO_InitStructure;
		if ( busmode == mode )
			return;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x00ff<<(D0 & 0x0f);
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = mode;
		if ( mode == INPUT ) {
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		} else {
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		}
		GPIO_Init(PinPort(D0), &GPIO_InitStructure);
		busmode = mode;
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
		Clear();
		// ClearScreen(Inverted ? 0x00 : 0xff);
	}

	void KS0108::On(void) {
		WriteCommand(LCD_DISP_START, 0);
		WriteCommand(LCD_DISP_START, 1);
		delay(10);
		WriteCommand(LCD_ON, 0);
		WriteCommand(LCD_ON, 1);
		delay(300);
}

	void KS0108::StartAddress(uint8 pos, uint8 chip) {
		WriteCommand(LCD_DISP_START, chip);
	}

	void KS0108::SetPageAddress(uint8 chip, uint8 page, uint8 column){	
		page &= B111;
		column &= B111111;
		pageaddress = (uint16(chip)<<9) | (uint16(page)<<6) | column;
	//printf("A[%04x: %01x, %01x, %02x], ", xyaddress, xyaddress>>6&1, xyaddress>>7&0x07, xyaddress&0x3f);
		WriteCommand(LCD_SET_PAGE | page, chip);
		WriteCommand(LCD_SET_ADDRESS | column, chip);
	}
	
	void KS0108::SetPageAddress(void) {
		WriteCommand(LCD_SET_PAGE | (pageaddress>>6 & 0x7), pageaddress>>9 & 1);
		WriteCommand(LCD_SET_ADDRESS | (pageaddress & 0x3f), pageaddress>>9 & 1);
	}
	
	void KS0108::BackPageAddress(uint8 val) { 
		pageaddress -= val;
		SetPageAddress();
	}
		
	void KS0108::SetDot(int16 x, int16 y, uint8 bw) {
		uint8 d, t;
		if ( x < 0 || x >= DISPLAY_WIDTH 
			|| y < 0 || y >= DISPLAY_HEIGHT )
			return;
		
		GotoXY(x,y);
		t = ReadData();
		BackPageAddress();
		do {
			d = t;
			t = ReadData();
			BackPageAddress();
		} while ( d != t );
		if ( bw ) 
			d |= (1<<(y%PAGE_HEIGHT));
		else
			d &= ~(1<<(y%PAGE_HEIGHT));
		WriteData(d);
	}
	
	

void KS0108::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
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
		if ( ((x+i) % 64) == 0 )
			GotoXY(x+i, y);
		data = ReadData();
		BackPageAddress();
		if(color == BLACK) {
			data |= mask;
		} else {
			data &= ~mask;
		}
		WriteData(data);
//		delay(50);
//			printf("(%d, %d) \n", x+i, y);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		GotoXY(x, y);
		for(i=0; i <width; i++) {
		if ( ((x+i) % 64) == 0 )
			GotoXY(x+i, y);
			WriteData(color);
//		delay(50);
//			printf("(%d, %d) \n", x+i, y);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		GotoXY(x, y+8);
		for(i=0; i < width; i++) {
		if ( ((x+i) % 64) == 0 )
				GotoXY(x+i, y+8);
			data = ReadData();
			BackPageAddress();
		
			if(color == BLACK) {
				data |= mask;
			} else {
				data &= ~mask;
			}
	
			WriteData(data);
		}
	}
}

void KS0108::Clear(uint8 bw) {
	pageaddress = 0;
	for(int cp = 0; cp < 8*2; cp++) {
		SetPageAddress();
		for(int col = 0; col < 64; col++) 
			WriteData(bw);
	}
}
