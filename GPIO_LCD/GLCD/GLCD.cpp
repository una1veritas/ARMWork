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
	
	void GLCD::PointXY(int16 x, int16 y, uint8 bw) {
		uint8 d;
		Address(1<<(x/CHIP_WIDTH),y/PAGE_HEIGHT, x%CHIP_WIDTH);
		d = ReadData();
		Address(1<<(x/CHIP_WIDTH),y/PAGE_HEIGHT, x%CHIP_WIDTH);
		if ( bw ) 
			WriteData(d | (1<<(y%8)) );
		else
			WriteData(d & ~(1<<(y%8)) );
	}

	
	void GLCD::ClearScreen(uint16 color) {
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
	
