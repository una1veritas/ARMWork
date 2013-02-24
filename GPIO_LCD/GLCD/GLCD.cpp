#include "GLCD.h"
#include "KS0108_compat.h"

	void GLCD::write(uint8 chip, uint8 di, uint8 val) {
		digitout(DNI, di);
		low(RNW);
		ChipSelect(chip);
		delay_us(1);
		high(ENCLK);
		out(val);
		delay_us(1);
		low(ENCLK);
		delay_us(5);	
}

	void GLCD::begin(void) {
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		for(int i = 0; i < buswidth; i++) {
			pinMode(D0+i, OUTPUT);
		}
		on();
	}

	void GLCD::on(void) {
		WriteCommand(LCD_ON, 1);
		WriteCommand(LCD_DISP_START, 1);
}

	void GLCD::locate(uint8 chip, uint8 page, uint8 column){
		WriteCommand(LCD_SET_PAGE | page%CHIP_PAGES, chip);
		WriteCommand(LCD_SET_ADD | column%PAGE_COLUMNS, chip);
	}
	
	void GLCD::ClearScreen(uint16 color) {
		for(int x = 0; x < 128; x++) {
			for(int y = 0; y < 64; y++) {
				locate(1<<(x/64), y>>3, x % 64);
				WriteData((uint8)color, 1<<(x/64));
			}
		}
	}
	
