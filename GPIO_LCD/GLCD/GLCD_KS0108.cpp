#include "GLCD_KS0108.h"

GPIOPin GPIOBus::CS[2] = { PD0, PD1 }; 
GPIOPin GPIOBus::DB[8]= { PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14 };

void GPIOBus::start(void) {
	for(int i = 0; i < 8; i++) {
		pinMode(DB[i], OUTPUT);
	}
	pinMode(CS[0], OUTPUT);
	pinMode(CS[1], OUTPUT);
	pinMode(EN, OUTPUT);
}

void GPIOBus::address(uint8 chip) {
	uint8 bit = 1<<chip;
	digitalWrite(CS[0], bit & 1 ? LOW : HIGH );
	digitalWrite(CS[1], bit & 2 ? LOW : HIGH );
}

void GPIOBus::write(uint8 w) {
	digitalWrite(EN, HIGH);
	for(int i = 0; i < 8; i++) {
		digitalWrite(DB[i], w & 1 ? HIGH : LOW );
		w >>= 1;
	}
	delay_us(1);
	digitalWrite(EN, LOW);
	delay_us(1);
}

void KS0108::init(void) {
	pinMode(RS, OUTPUT);
	pinMode(RW, OUTPUT);
	pinMode(NRST, OUTPUT);
}

void KS0108::start(void) {
	gpiobus.start();
	
	digitalWrite(NRST, HIGH);
	delay(20);
	digitalWrite(NRST, LOW);
	delay_ms(2);
	digitalWrite(NRST, HIGH);

	delay_ms(300);
	for (uint8_t chip = 0; chip < 2; chip++) {
		WriteCommand(LCD_ON, chip);				// power on
		WriteCommand(LCD_DISP_START, chip);	// display start line = 0
		delay_ms(5);
	}
	delay(300);
	/*
	WriteCommand(0xc0, 0);
	WriteCommand(0xc0, 1);
	*/
}

void KS0108::ChipSelect(uint8 chip) {
	uint8 csbit = 1<<chip;
	gpiobus.address(csbit);
	//delay_us(1);
}

void KS0108::WriteCommand(uint8 cmd, uint8 chip) {
	gpiobus.address(chip);
	digitalWrite(RS, CMD);
	digitalWrite(RW, WRITE);
	
	delay_us(1);
	gpiobus.write(cmd);
	delay_us(2);
}

void KS0108::WriteData(uint8 data, uint8 chip) {
	gpiobus.address(chip);
	digitalWrite(RS, DATA);
	digitalWrite(RW, WRITE);

	delay_us(1);
	gpiobus.write(data);
	delay_us(2);
}

void KS0108::SetAddress(uint8 chip, uint8 page, uint8 column) {
	WriteCommand(LCD_SET_PAGE | page, chip);
	WriteCommand(LCD_SET_ADD | column, chip);				// set x address on active chip
}
