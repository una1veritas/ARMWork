
#include "cmcore.h"

GPIOPin RS = PB11, RW = PB12, EN = PB13;
GPIOPin CS1 = PB14, CS2 = PB15;

#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_DISP_START		0xC0

#define LCD_SET_ADD			0x40
#define LCD_SET_PAGE		0xB8

#define LCD_BUSY_FLAG		0x80 

// Colors
#define BLACK				0xFF
#define WHITE				0x00

#define CHIP_WIDTH 	64
#define CHIP_HEIGHT 	64
#define DISPLAY_WIDTH 	128
#define DISPLAY_HEIGHT 	64

void WriteCommand(uint8 cmd, uint8 chip);
void WriteData(uint8 data, uint8 chip);
void gotoXYByte(uint16, uint16);
void ChipSelect(uint8 chip);

int main(void) {
	cmcore_init();
	
	printf("Hello!\n");
	
	GPIOMode(PinPort(RS), PinBit(RS) | PinBit(RW) | PinBit(EN), OUTPUT, FASTSPEED, PUSHPULL, PULLUP);
	GPIOMode(PinPort(CS1), PinBit(CS1) | PinBit(CS2), OUTPUT, FASTSPEED, PUSHPULL, PULLUP);
	GPIOMode(PinPort(PE7), PinBit(PE7) | PinBit(PE8) | PinBit(PE9) | PinBit(PE10) | PinBit(PE11) | PinBit(PE12) | PinBit(PE13) | PinBit(PE14), OUTPUT, FASTSPEED, PUSHPULL, PULLUP);
	GPIOMode(PinPort(PD12), PinBit(PD12), OUTPUT, FASTSPEED, PUSHPULL, PULLUP);
	
	digitalWrite(RS, HIGH);
	digitalWrite(RW, HIGH);
	digitalWrite(EN, HIGH);
	digitalWrite(CS1, HIGH);
	digitalWrite(CS2, HIGH);

	delay_ms(10);
	
	for (uint8_t chip = 0; chip < 2; chip++) {
		delay_ms(10);
		WriteCommand(LCD_ON, chip);				// power on
		WriteCommand(LCD_DISP_START, chip);	// display start line = 0
	}
	delay(50);
	
	printf("done 1.\n");
	uint8 x = 0, y = 0;
	for(x = 0; x < 128; x++) {
		for(y = 0; y < 64; y+=8) {
			gotoXYByte(x, y);
			WriteData(0x00, x/CHIP_WIDTH);
		}
	}
	x = 40;
	for(y = 0; y < 64; y++) {
		gotoXYByte(x, y);
		WriteData((uint8)0x01<<(x%8), x/CHIP_WIDTH);
		x++;
	}

	while (1) {
		delay(500);
		digitalToggle(PD12);
	}
}

void ChipSelect(uint8 chip) {
	uint16 csbit = 1 << chip;
	digitalWrite(CS1, HIGH);
	digitalWrite(CS2, HIGH);
	if ( csbit & 1 ) {
		digitalWrite(CS1, LOW);
	} else if ( csbit & 2 )
		digitalWrite(CS2, LOW);
	delay_us(2);
}

void WriteCommand(uint8 cmd, uint8 chip) {
	ChipSelect(chip);
	
	digitalWrite(RS, LOW);
	digitalWrite(RW, LOW);
	digitalWrite(EN, LOW);
//	lcdDataDir(0xFF);
//	EN_DELAY();
	delay_us(2);
	GPIO_Write(GPIOE, ((uint16)cmd)<<7);
	digitalWrite(EN, HIGH);
	delay_us(1);
	digitalWrite(EN, LOW);
	delay_us(2);
	digitalWrite(RS, HIGH);
	digitalWrite(RW, HIGH);
}

void WriteData(uint8 data, uint8 chip) {
	ChipSelect(chip);
	
	digitalWrite(RS, HIGH);
	digitalWrite(RW, LOW);
	digitalWrite(EN, LOW);
	delay_us(2);
	GPIO_Write(GPIOE, ((uint16)data)<<7);
	digitalWrite(EN, HIGH);
	delay_us(1);
	digitalWrite(EN, LOW);
	delay_us(2);
	digitalWrite(RW, HIGH);
}

void gotoXYByte(uint16 x, uint16 y) {
	uint8 cmd, chip;
	cmd = LCD_SET_PAGE | (y/8);
	for (chip = 0; chip < DISPLAY_WIDTH/CHIP_WIDTH; chip++) {
		WriteCommand(cmd, chip);
	}
	chip = x / CHIP_WIDTH;
	x = x % CHIP_WIDTH;
	cmd = LCD_SET_ADD | x;
	WriteCommand(cmd, chip);				// set x address on active chip
}
