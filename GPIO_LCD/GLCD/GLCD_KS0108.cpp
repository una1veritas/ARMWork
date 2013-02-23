#include "GLCD_KS0108.h"

<<<<<<< HEAD
// initialize constant values
const GPIOPin FSMCBus::DB[] = {
//		PD14, PD15, PD0, PD1, 
		PE7, PE8, PE9, PE10,
		PE11, PE12, PE13, PE14, PE15,
//		PD8, PD9, PD10
};

/*
#define LCD_REG      (*((volatile unsigned short *) 0x60000000)) 
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000)) 
#define LCD_REG_READ      (*((volatile unsigned short *) (0x60000000 | (1<<16)))) 
#define USE_FSMC
#define USE_FSMC_DATABUS
*/

void FSMCBus::init8bus(void)
{
//	GPIOPin RS = PE11, RW = PD5, EN = PD4; // FSMC_D8, _NWE, _NOE
// GPIOPin CS1 = PD11, CS2 = PD12, RST = PD6;  // FSMC_A16, _A17, FSMC_NWAIT
#ifdef USE_FSMC
	GPIOMode(PinPort(PE11), PinBit(PE11), ALTFUNC, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOMode(PinPort(PD5), PinBit(PD4) | PinBit(PD5) | PinBit(PD11) | PinBit(PD12), ALTFUNC, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOAltFunc(PinPort(PE11), PinBit(PE11), GPIO_AF_FSMC);
	GPIOAltFunc(PinPort(PE11), PinBit(PE11), GPIO_AF_FSMC);
#else
//	GPIOMode(PinPort(PE11), PinBit(PE11), OUTPUT, HIGHSPEED, PUSHPULL, NOPULL); 
	GPIOMode(PinPort(PD5), PinBit(PD4) | PinBit(PD5) | PinBit(PD8) | PinBit(PD9), OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
#endif

	//GPIOPin DB[] = { PD14, PD15, PD0, PD1, PE7, PE8, PE9, PE10 }; // FSMC_D0 --- _D7
#ifdef USE_FSMC_DATABUS
	GPIOMode(PinPort(DB[0]), PinBit(DB[0]) | PinBit(DB[1]) | PinBit(DB[2]) | PinBit(DB[3]), ALTFUNC, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOMode(PinPort(DB[4]), PinBit(DB[4]) | PinBit(DB[5]) | PinBit(DB[6]) | PinBit(DB[7]), ALTFUNC,	HIGHSPEED, PUSHPULL, NOPULL);
	GPIOAltFunc(PinPort(DB[0]), PinBit(DB[0]) | PinBit(DB[1]) | PinBit(DB[2]) | PinBit(DB[3]), GPIO_AF_FSMC);	
	GPIOAltFunc(PinPort(DB[4]), PinBit(DB[4]) | PinBit(DB[5]) | PinBit(DB[6]) | PinBit(DB[7]), GPIO_AF_FSMC);
#else
	GPIOMode(PinPort(DB[0]), PinBit(DB[0]) | PinBit(DB[1]) | PinBit(DB[2]) | PinBit(DB[3]), OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
	GPIOMode(PinPort(DB[4]), PinBit(DB[4]) | PinBit(DB[5]) | PinBit(DB[6]) | PinBit(DB[7]) | PinBit(DB[8]), OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
//	GPIOMode(PinPort(DB[13]), PinBit(DB[13]) | PinBit(DB[14]) | PinBit(DB[15]) , OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
#endif
	
	// Reset
	GPIOMode(PinPort(NRST), PinBit(NRST), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);
	digitalWrite(NRST, HIGH);
=======
void GPIOBus::start(void) {
	portMode(DB, OUTPUT);
	portMode(AB, OUTPUT);
	pinMode(CLK, OUTPUT);
>>>>>>> origin/macbook
}

void GPIOBus::address(uint8 a) {
	for(int i = 0; i < 4; i++) {
		digitalWrite(AB[i], a & 1 ? HIGH : LOW );
		a >>= 1;
	}
}

<<<<<<< HEAD
uint16 FSMCBus::write(uint16 w) {
/*
	uint16 val = GPIO_ReadOutputData(GPIOE);
	val &= ~(0x00ff<<7);
	GPIO_Write(GPIOE, val | (w<<7));
	*/
=======
void GPIOBus::write(uint8 w) {
	digitalWrite(CLK, HIGH);
>>>>>>> origin/macbook
	for(int i = 0; i < 8; i++) {
		digitalWrite(DB[i], w & 1 ? HIGH : LOW );
		w >>= 1;
	}
<<<<<<< HEAD
	
	return w;
}

void KS0108::init(void) {	
	fsmcbus.init8bus();
//	fsmcbus.modeConfig();
=======
	digitalWrite(CLK, LOW);
	delay_us(1);
}

void KS0108::init(void) {	
	gpiobus.start();
>>>>>>> origin/macbook
	
	gpiobus.address(0x0f);
	digitalWrite(gpiobus.CLK, LOW);
}

void KS0108::start(void) {
	digitalWrite(nRST, HIGH);
	delay(100);
	digitalWrite(nRST, LOW);
	delay_ms(10);
	digitalWrite(nRST, HIGH);

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
	const uint8 CS1High = 1<<2;
	gpiobus.address(CS1High<<chip | 0x00);
	
<<<<<<< HEAD
	digitalWrite(fsmcbus.EN, LOW);
	digitalWrite(fsmcbus.RS, LOW);
	digitalWrite(fsmcbus.RW, LOW);
//	lcdDataDir(0xFF);
//	EN_DELAY();
	delay_us(1);
	digitalWrite(fsmcbus.EN, HIGH);
	fsmcbus.write(cmd);
	delay_us(1);
	digitalWrite(fsmcbus.EN, LOW);
	delay_us(1);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, HIGH);
//	digitalWrite(fsmcbus.EN, HIGH);
//	digitalWrite(CS1, HIGH);
//	digitalWrite(CS2, HIGH);
=======
	digitalWrite(gpiobus.CLK, LOW);
	delay_us(1);
	gpiobus.write(cmd);
	gpiobus.address(0x0f);
>>>>>>> origin/macbook
	delay_us(2);
}

void KS0108::WriteData(uint8 data, uint8 chip) {
<<<<<<< HEAD
	ChipSelect(chip);
	
	digitalWrite(fsmcbus.EN, LOW);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, LOW);
	delay_us(1);
	digitalWrite(fsmcbus.EN, HIGH);
	fsmcbus.write(data);
	delay_us(1);
	digitalWrite(fsmcbus.EN, LOW);
	delay_us(1);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, HIGH);
//	digitalWrite(fsmcbus.EN, HIGH);
//	digitalWrite(CS1, HIGH);
//	digitalWrite(CS2, HIGH);
=======
	const uint8 CS1High = 1<<2;
	gpiobus.address(CS1High<<chip | 0x01);

	digitalWrite(gpiobus.CLK, LOW);
	delay_us(1);
	gpiobus.write(data);
	gpiobus.address(0x0f);
>>>>>>> origin/macbook
	delay_us(2);
}

void KS0108::SetAddress(uint8 chip, uint8 page, uint8 column) {
	WriteCommand(LCD_SET_PAGE | page, chip);
	WriteCommand(LCD_SET_ADD | column, chip);				// set x address on active chip
}
