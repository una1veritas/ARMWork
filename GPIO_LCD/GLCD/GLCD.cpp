#include "GLCD.h"
#include "KS0108_compat.h"

	void GLCD::write(uint8 chip, uint8 di, uint8 val) {
		digitout(DNI, di);
		ChipSelect(chip);
		low(RNW);
		delay_us(1);
		high(ENCLK);
		delay_us(1);
		out(val);
		delay_us(1);
		low(ENCLK);
		delay_us(1);
		high(ENCLK);
		high(RNW);
		ChipSelect(0);
		delay_us(10);
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
		On();
	}

	void GLCD::On(void) {
		delay(5);
		WriteCommand(LCD_ON, 1);
		WriteCommand(LCD_ON, 2);
		delay(300);
		WriteCommand(LCD_DISP_START, 1);
		WriteCommand(LCD_DISP_START, 2);
		delay(10);
}

	void GLCD::Address(uint8 chip, uint8 page, uint8 column){
		WriteCommand(LCD_SET_PAGE | page, 1);
		WriteCommand(LCD_SET_PAGE | page, 2);
		WriteCommand(LCD_SET_ADD | column, chip);
	}
	
	void GLCD::ClearScreen(uint16 color) {
		for(int chid = 0; chid < 2; chid++) {
			for(int p = 0; p < CHIP_PAGES; p++) {
				for(int c = 0; c < PAGE_COLUMNS; c++) {
					Address(1<<chid, p, c);
					WriteData(color, 1<<chid);
				}
			}
		}
	}
	
