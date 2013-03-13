#include "armcmx.h"
#include "KS0108.h"
//#include "KS0108_params.h"


	void KS0108::select(uint8 id) {
		digitout(CS[0], id == 0 ? LOW : HIGH);
		digitout(CS[1], id == 1 ? LOW : HIGH);
	}
	
	void KS0108::writebus(uint8 chip, uint8 di, uint8 val) {
		select(chip);
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
//		high(RNW);
//		select(0xff);
//		delay_us(1);
	}

	uint8 KS0108::readbus(uint8 chip, uint8 di) {
		uint8 val;
		select(chip);
		digitout(DNI, di);
		high(RNW);
		delay_us(1);
		high(ENCLK);
		delay_us(1);
		low(ENCLK);
		val = in();
//		delay_us(1);
//		low(RNW);
//		select(0xff);
		delay_us(1);
		return val;
	}

	void KS0108::WriteCommand(const uint8 cmd) {
		busMode(INPUT);
		while ( isbusy() != 0);
		busMode(OUTPUT);
		writebus(0, COMMAND, cmd);
		writebus(1, COMMAND, cmd);
	}

	uint8 KS0108::isbusy(void) {
		return (readbus(0, COMMAND) & LCD_BUSY_FLAG) != 0 || (readbus(1, COMMAND) & LCD_BUSY_FLAG) != 0; 
	}

	void KS0108::WriteData(uint8 data) {
		uint8 chip = (yxaddress % DISPLAY_WIDTH)/ CHIP_WIDTH;
		busMode(INPUT);
		delay_us(1);
		while ( isbusy() );
		busMode(OUTPUT);
		delay_us(1);
		writebus(chip, DATA, data);
    yxaddress++;
	}

	uint8 KS0108::ReadData(void) {
		uint8 chip = (yxaddress % DISPLAY_WIDTH)/ CHIP_WIDTH;
		busMode(INPUT);
		delay_us(1);
		while ( isbusy() );
		uint8 res = readbus(chip, DATA);
    yxaddress++;
		return res;
	}
	
	void KS0108::busMode(GPIOMode_TypeDef mode){
		GPIO_InitTypeDef GPIO_InitStructure;
		if ( busmode == mode )
			return;
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = 0x00ff<<(D0 & 0x0f);
		GPIO_InitStructure.GPIO_Mode = mode;
		GPIO_Init(PinPort(D0), &GPIO_InitStructure);
		busmode = mode;
	}
	
	void KS0108::init(void) {
		pinMode(DNI, OUTPUT);
		pinMode(RNW, OUTPUT);
		pinMode(ENCLK, OUTPUT);
		pinMode(CS[0], OUTPUT);
		pinMode(CS[1], OUTPUT);
		GPIOMode(PinPort(D0), 0x00ff<<(D0 & 0x0f), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);//DBMode(OUTPUT);

		GLCDController::init();
	}

	void KS0108::DisplayOn(void) {
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
	void KS0108::setXY(uint8 x, uint8 y){
    yxaddress = (uint16(y&(DISPLAY_HEIGHT-1))<<7) | uint16(x & (DISPLAY_WIDTH-1));
	}
	
	void KS0108::GotoXY(uint8 x, uint8 y) {
		setXY(x, y);
		WriteCommand(LCD_SET_PAGE | (yxaddress<<1)>>(8+3));
		WriteCommand(LCD_SET_ADDRESS | (yxaddress&(CHIP_WIDTH-1)));
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
