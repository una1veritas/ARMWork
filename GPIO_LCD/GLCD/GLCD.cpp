#include "GLCD.h"
#include "KS0108_compat.h"

	void GLCD::ChipSelect(uint8 csbits) {
		digitout(CS[0], csbits & 1 ? LOW : HIGH);
		digitout(CS[1], csbits & 2 ? LOW : HIGH);
	}

	void GLCD::write(uint8 chip, uint8 di, uint8 val) {
		ChipSelect(chip);
		digitout(DNI, di);
		low(RNW);
		delay_us(1);
		high(ENCLK);
//	delay_us(1);
		out(val);
		delay_us(1);
		low(ENCLK);
		delay_us(1);
//	high(ENCLK);
//		low(RNW);
		ChipSelect(0);
		delay_us(1);
		lastcs = chip;
	}

	uint8 GLCD::read(uint8 chip, uint8 di) {
		uint8 val;
		ChipSelect(chip);
		digitout(DNI, di);
		high(RNW);
		delay_us(1);
		high(ENCLK);
		delay_us(1);
		low(ENCLK);
		val = in();
		delay_us(1);
//		low(RNW);
		ChipSelect(0);
		delay_us(1);
		lastcs = chip;
		return val;
	}

	void GLCD::WriteCommand(uint8 cmd, uint8 cs) {
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		DBMode(OUTPUT);
		write(cs, COMMAND, cmd); 
	}
	void GLCD::WriteData(uint8 data, uint8 cs) { 
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		DBMode(OUTPUT);
		write(cs, DATA, data); 
	}
	uint8 GLCD::ReadStatus(uint8 cs) {
		return read(cs, COMMAND); 
	}
	uint8 GLCD::ReadData(uint8 cs) {
		DBMode(INPUT);
		while ( (ReadStatus(cs) & 0x80) != 0 );
		return read(cs, DATA); 
	}

	
	void GLCD::DBMode(GPIOMode_TypeDef mode){
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x00ff<<(D0 & 0x0f);
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = mode;
		GPIO_Init(PinPort(D0), &GPIO_InitStructure);
		busmode = mode;
	}
	
	void GLCD::begin(void) {
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		DBMode(OUTPUT);/*
		for(int i = 0; i < buswidth; i++) {
			pinMode(D0+i, OUTPUT);
		}*/
		On();
	}

	void GLCD::On(void) {
		delay(10);
		WriteCommand(LCD_ON, 1);
		WriteCommand(LCD_DISP_START, 1);
		delay(10);
		WriteCommand(LCD_ON, 2);
		WriteCommand(LCD_DISP_START, 2);
		delay(300);
}

	void GLCD::Address(uint8 chip, uint8 page, uint8 column){
		WriteCommand(LCD_SET_PAGE | page, chip);
//		WriteCommand(LCD_SET_PAGE | page, 2);
		WriteCommand(LCD_SET_ADD | column, chip);
	}
	
	void GLCD::SetDot(uint16 x, uint16 y, uint8 bw) {
		uint8 d;
		GotoXY(x,y);
		d = ReadData();
		GotoXY(x,y);
		if ( bw ) 
			WriteData(d | (1<<(y%8)) );
		else
			WriteData(d & ~(1<<(y%8)) );
	}

	
	void GLCD::ClearScreen(uint8 color) {
		for(int p = 0; p < CHIP_PAGES; p++) {
			for(int chip = 1; chip <= 2; chip++)
				Address(chip, p, 0);
			for(int c = 0; c < PAGE_WIDTH; c++) {
				for(int chip = 1; chip <= 2; chip++) {
					WriteData(color, chip);
				}
			}
		}
	}
	

#define distance(x, y)  ((x) - (y) >= 0 ? (x)-(y) : (y) - (x) )
#define swap(x, y, t) 	(t = x, y = x, x = t)
	
void GLCD::DrawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color) {
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


void GLCD::DrawVLine(int16 x, int16 y, int16 height, uint8 color){
  // this->FillRect(x, y, 0, length, color);
   SetPixels(x,y,x,y+height,color);
}
	
/**
 * Draw a Horizontal Line
 *
 * @param x a value from 0 to GLCD.Width-1
 * @param y a value from 0 to GLCD.Height-1
 * @param width a value from 1 to GLCD.Width-x-1
 * @param color BLACK or WHITE 
 *
 * The line drawn will be width+1 pixels.
 *
 * color is an optional parameter indicating pixel color and defaults to BLACK
 *
 * @note This function was previously named DrawHoriLine() in the ks0108 library
 *
 * @see DrawLine()
 * @see DrawVLine()
 */

void GLCD::DrawHLine(int16 x, int16 y, int16 width, uint8 color){
   // this->FillRect(x, y, length, 0, color);
    SetPixels(x,y, x+width, y, color);
}

void GLCD::SetPixels(int16 x, int16 y, int16 x2, int16 y2, uint8 color) {
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


void GLCD::drawCharacter(const char ch) {
	if ( font[2] == 'F' )
		drawFixedFont(font, ch);
	else
		drawProportionalFont(font, ch);
}

void GLCD::drawFixedFont(const byte font[], char character) {
	uint16 width = font[0];
	uint16 percolumn = (font[1]&0x7f)/8 + ((font[1]&0x7f)%8 ? 1 : 0);
//	boolean transpo = (font[1] & 0x80 ? 1 : 0);
	uint16 idx = 3 + (character - ' ') * width * percolumn;
//	byte t;
	
	if ( (textcursor % 84) + width+2 >= 84 )
		textcursor = (textcursor/84 + 1)*84;
	if ( character == 0x20 && textcursor%84 == 0 )
		return;
	for(int bytepos = 0; bytepos < percolumn; bytepos++) {
		Address(1+(textcursor%128)/64, textcursor/128, textcursor%64);
		WriteData(0x00); //Blank vertical line padding
		for (int i = 0 ; i < width ; i++) {
			WriteData(font[idx + bytepos + i*percolumn]);
		}
		WriteData(0x00); //Blank vertical line padding
	}
	//
	textcursor += width + 2;

}

//This function takes in a character, looks it up in the font table/array
//And writes it to the screen
//Each character is 8 bits tall and 5 bits wide. We pad one blank column of
//pixels on each side of the character for readability.
void GLCD::drawProportionalFont(const byte font[], char character) {
//	byte mxwidth = font[0];
	byte height = font[1];
//	byte proportional = font[2];

	byte percolbytes = height/8 + height%2;
//	byte textrow = textcursor/percolbytes;
	
	uint16 idx = 3;
	byte fontwidth;
	byte ch = 0x20;
	for( ; ch != character; ch++) 
		idx += 1 + font[idx]*percolbytes;
	fontwidth = font[idx];
	idx++;
	
	if ( (textcursor % 84) + fontwidth+2 >= 84 )
		textcursor = (textcursor/84 + 1)*84;
	if ( character == 0x20 && textcursor%84 == 0 )
		return;
	for(int bytepos = 0; bytepos < percolbytes; bytepos++) {
		Address(1+(textcursor%128)/64, textcursor/128, textcursor%64);
		WriteData(0x00); //Blank vertical line padding
		for (int i = 0 ; i < fontwidth ; i++) {
			WriteData(font[idx + bytepos + i*percolbytes]);
		}
		WriteData(0x00); //Blank vertical line padding
	}
	//
	textcursor += fontwidth + 2;
}

//Given a string of characters, one by one is passed to the LCD
void GLCD::drawString(const char *characters) {
  while (*characters)
    drawCharacter(*characters++);
}
