#include "GLCD_KS0108.h"

void FSMCBus::init8bus(void)
{
//	GPIOPin RS = PE11, RW = PD5, EN = PD4; // FSMC_D8, _NWE, _NOE
// GPIOPin CS1 = PD11, CS2 = PD12, RST = PD6;  // FSMC_A16, _A17, FSMC_NWAIT
	GPIOMode(PinPort(PE11), PinBit(PE11), //ALTFUNC, 
	OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
//	GPIOAltFunc(PinPort(PE11), PinBit(PE11), GPIO_AF_FSMC);
	GPIOMode(PinPort(PD5), PinBit(PD4) | PinBit(PD5) | PinBit(PD11) | PinBit(PD12), 
					//	ALTFUNC, 
					OUTPUT, HIGHSPEED, PUSHPULL, NOPULL);
//	GPIOAltFunc(PinPort(PD4), PinBit(PD4) | PinBit(PD5) | PinBit(PD11) | PinBit(PD12), 
//						GPIO_AF_FSMC);

	//GPIOPin DB[] = { PD14, PD15, PD0, PD1, PE7, PE8, PE9, PE10 }; // FSMC_D0 --- _D7
	GPIOMode(PinPort(DB[0]), PinBit(DB[0]) | PinBit(DB[1]) | PinBit(DB[2]) | PinBit(DB[3]), 
										//		 ALTFUNC, 
												OUTPUT, 
	HIGHSPEED, PUSHPULL, NOPULL);
//	GPIOAltFunc(PinPort(DB[0]), PinBit(DB[0]) | PinBit(DB[1]) | PinBit(DB[2]) | PinBit(DB[3]), GPIO_AF_FSMC);	
	GPIOMode(PinPort(DB[4]), PinBit(DB[4]) | PinBit(DB[5]) | PinBit(DB[6]) | PinBit(DB[7]), 
										//		 ALTFUNC, 
	 OUTPUT, 
	HIGHSPEED, PUSHPULL, NOPULL);
//	GPIOAltFunc(PinPort(DB[4]), PinBit(DB[4]) | PinBit(DB[5]) | PinBit(DB[6]) | PinBit(DB[7]), GPIO_AF_FSMC);
	
	// Reset
	GPIOMode(PinPort(NRST), PinBit(NRST), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);
	digitalWrite(NRST, HIGH);
}


void FSMCBus::modeConfig(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* FSMC Read/Write Timing */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 5;  /* Address Setup Time  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 5;     /* Data Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* FSMC Write Timing */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 15;//2;//1;   /* Address Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 15;//2;//1;	     /* Data Setup Time */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
	
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  

}

uint16 FSMCBus::write(uint16 w) {
	for(int i = 0; i < 8; i++)
		digitalWrite(DB[i], (w>>i&1 ? HIGH : LOW) );
	return w;
}

void KS0108::init(void) {
	
	fsmcbus.init8bus();
	fsmcbus.modeConfig();
	
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, HIGH);
	digitalWrite(fsmcbus.EN, LOW);
	digitalWrite(fsmcbus.CS1, HIGH);
	digitalWrite(fsmcbus.CS2, HIGH);
}

void KS0108::start(void) {
	delay_ms(300);
	for (uint8_t chip = 0; chip < 2; chip++) {
		WriteCommand(LCD_ON, chip);				// power on
		WriteCommand(LCD_DISP_START, chip);	// display start line = 0
		delay_ms(5);
	}
	delay(300);
	
	WriteCommand(0xc0, 0);
	WriteCommand(0xc0, 1);
}

void KS0108::ChipSelect(uint8 chip) {
	if ( chip == 1 ) {
		digitalWrite(fsmcbus.CS1, LOW);
		digitalWrite(fsmcbus.CS2, HIGH);
	} else if ( chip == 0 ) {
		digitalWrite(fsmcbus.CS1, HIGH);
		digitalWrite(fsmcbus.CS2, LOW);
	}
	delay_nop(50);
}

void KS0108::WriteCommand(uint8 cmd, uint8 chip) {
	ChipSelect(chip);
	
	digitalWrite(fsmcbus.EN, LOW);
	digitalWrite(fsmcbus.RS, LOW);
	digitalWrite(fsmcbus.RW, LOW);
//	lcdDataDir(0xFF);
//	EN_DELAY();
	delay_nop(50);
	digitalWrite(fsmcbus.EN, HIGH);
	fsmcbus.write(cmd);
	delay_nop(50);
	digitalWrite(fsmcbus.EN, LOW);
	delay_nop(20);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, HIGH);
//	digitalWrite(EN, HIGH);
//	digitalWrite(CS1, HIGH);
//	digitalWrite(CS2, HIGH);
	delay_nop(20);
}

void KS0108::WriteData(uint8 data, uint8 chip) {
	ChipSelect(chip);
	
	digitalWrite(fsmcbus.EN, LOW);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, LOW);
	delay_nop(50);
	digitalWrite(fsmcbus.EN, HIGH);
	fsmcbus.write(data);
	delay_nop(50);
	digitalWrite(fsmcbus.EN, LOW);
	delay_nop(20);
	digitalWrite(fsmcbus.RS, HIGH);
	digitalWrite(fsmcbus.RW, HIGH);
//	digitalWrite(EN, HIGH);
//	digitalWrite(CS1, HIGH);
//	digitalWrite(CS2, HIGH);
	delay_nop(20);
}

void KS0108::SetAddress(uint8 chip, uint8 page, uint8 bp) {
	uint8 cmd;
	cmd = LCD_SET_PAGE | page;
	WriteCommand(cmd, chip);
	cmd = LCD_SET_ADD | bp;
	WriteCommand(cmd, chip);				// set x address on active chip
}
