#include "armcmx.h"
#include "KS0108.h"
//#include "KS0108_params.h"

	uint16 GLCDController::Width = DISPLAY_WIDTH;
	uint16 GLCDController::Height = DISPLAY_HEIGHT;

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

	void KS0108::WriteCommand(uint8 cmd) {
		DBMode(INPUT);
		while ( IsBusy() );
		DBMode(OUTPUT);
		writebus(0, COMMAND, cmd);
		writebus(1, COMMAND, cmd);
	}

	uint8 KS0108::ReadStatus() {
		return readbus(0, COMMAND) || readbus(1, COMMAND); 
	}

	void KS0108::WriteData(uint8 data) {
		uint8 chip = xyaddress >> 6 & 1;
		WriteCommand(LCD_SET_PAGE | (xyaddress>>7&0x07));
		WriteCommand(LCD_SET_ADDRESS | (xyaddress&0x3f));
		DBMode(INPUT);
		delay_us(1);
		while ( IsBusy() );
		DBMode(OUTPUT);
		delay_us(1);
		writebus(chip, DATA, data);
		//
		xyaddress++;
	}

	uint8 KS0108::ReadData(void) {
		uint8 chip = xyaddress>>6&1;
		WriteCommand(LCD_SET_PAGE | (xyaddress>>7&0x07));
		WriteCommand(LCD_SET_ADDRESS | (xyaddress&0x3f));
		DBMode(INPUT);
		delay_us(1);
		while ( IsBusy() );
		uint8 res = readbus(chip, DATA);
		//WriteCommand(LCD_SET_PAGE | (xyaddress>>7&0x07), chip);
//		WriteCommand(LCD_SET_ADDRESS | (xyaddress&0x3f));
		return res;
	}
	
	void KS0108::DBMode(GPIOMode_TypeDef mode){
		GPIO_InitTypeDef GPIO_InitStructure;
		if ( busmode == mode )
			return;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x00ff<<(D0 & 0x0f);
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = mode;
/*
		if ( mode == INPUT ) {
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		} else {
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		}
*/
		GPIO_Init(PinPort(D0), &GPIO_InitStructure);
		busmode = mode;
	}
	
	void KS0108::init(void) {
		foreColor = 1;
		backColor = 0;
		
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		GPIOMode(PinPort(D0), 0x00ff<<(D0 & 0x0f), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);//DBMode(OUTPUT);
		displayOn();
		// ClearScreen(Inverted ? 0x00 : 0xff);
	}

	void KS0108::displayOn(void) {
		WriteCommand(LCD_DISP_START);
		delay(10);
		WriteCommand(LCD_ON);
		delay(300);
}

/*
	void KS0108::StartAddress(uint8 pos) {
		WriteCommand(LCD_DISP_START | (pos&B00111111));
	}
*/
	void KS0108::SetAddress(uint8 pg, uint8 col){
		pg &= 0x07;
		col &= 0x7f;
		xyaddress = (pg<<7) | col;
//		WriteCommand(LCD_SET_PAGE | pg);
//		WriteCommand(LCD_SET_ADDRESS | (col & 0x3f));
	}
	
/*
void KS0108::Clear(uint8 bw) {
	for(int p = 0; p < 8; p++) {
		SetAddress(p, 0);
		for(int col = 0; col < 128; col++) {
			WriteData(bw);
			//delay(10);
		}
	}
}
*/
