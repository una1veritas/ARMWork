/*
 * arduino_io.h
 * this file maps arduino pins to avr ports and pins
 *
 * The header file: pins_arduino.h is used if this exits
 * otherwise the following controllers are defined in this file
 * Arduino (ATmega8,168,328), Mega, Sanguino (ATmega644P)
 * 
 * Thanks to Paul Stoffregen (http://www.pjrc.com/teensy) 
 * for his expertise in Arduino pin mapping macros
 *
 * Warning: when adding new board/procesor types ther are other headers
 * that have similar dependencies most notabily the ones in glcd/config
*/ 

#include "cmcore.h"

#if !(defined(digitalPinToPortReg) && defined(digitalPinToBit))

	// Standard Arduino Pins
#define digitalPinToPortReg(P)  PinPort(P)

#define digitalPinToBit(P) PinBit(P)

#endif

#define lcdPinMode(pin, mode)  pinMode(pin, mode) 
#define lcdfastWrite(pin, pinval) digitalWrite(pin, pinval)

#define lcdDelayMilliseconds(x)  delay_ms(x)

#define lcdIsBusyStatus(status) (status & LCD_BUSY_FLAG)
#define lcdIsResetStatus(status) (status & LCD_RESET_FLAG)

/*
 * Configure the direction of the data pins.
 *	0x00 is for input and 0xff is for output.
 */
#define lcdDataDir(dirbits)					\
	pinMode(glcdData0Pin, dirbits); pinMode(glcdData1Pin, dirbits); pinMode(glcdData2Pin, dirbits); pinMode(glcdData3Pin, dirbits); \
	pinMode(glcdData4Pin, dirbits); pinMode(glcdData5Pin, dirbits); pinMode(glcdData6Pin, dirbits); pinMode(glcdData7Pin, dirbits); 

