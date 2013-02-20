/*
  ks0108_Sanguino.h - Arduino library support for ks0108 and compatable graphic LCDs
  Copyright (c)2008 Michael Margolis All right reserved

  This is the configuration file for mapping Sanguino (ATmega644) pins to the ks0108 Graphics LCD library
*/

#ifndef	KS0108_CONFIG_H
#define KS0108_CONFIG_H

#define ARMCM

#include "armcmx.h"

/*******************************************************************************************/
/* Sanguino/ ATmega644 defines                                                             */
/*******************************************************************************************/

//#define LCD_DATABITS_PD  (PinBit(PD14) | PinBit(PD15) | PinBit(PD0) | PinBit(PD1))
//#define LCD_DATABITS_PE  (PinBit(PE7) | PinBit(PE8) | PinBit(PE9) | PinBit(PE19))


// Command pins assignments:
#define D_I					PB11 // A16 		// RS D/I Bit 
#define R_W					PB12  // nWR 		// RW Bit
#define EN					PB13  // nOE 		// E Bit
#define CSEL1				PB14 // AD8 		// CS1 Bit   // swap pin assignments with CSEL2 if left/right image is reversed
#define CSEL2				PB15 // AD9 		// CS2 Bit

//all command pins are on the same port for slight speed & code size improvement
//#define LCD_CMD_PORT		PORTC		// Command Output Register for pins 16-23
//#define LCD_CNTRL_PORT 		PinPort(PD4)

// data pin assignments- on ATmega644 all data pins are assigned to the same port
//#define dataPins0to7    // bits 0-7 assigned to sanguino pins 0-7
//#define dataPins8to15   // bits 0-7 assigned to sanguino pins 8-15 // note this conflicts with serial UART
//#define dataPins16to23  // bits 0-7 assigned to sanguino pins 16-23 
//#define dataPins24to31  // bits 0-7 assigned to sanguino pins 24-31

/*******************************************************/
/*     end of Sanguino configuration                    */
/*******************************************************/
/*
// these macros  map pins to ports using the defines above
// the following should not be changed unless you really know what your doing 
#ifdef dataPins0to7
#define LCD_DATA_LOW_NBL    B   // port B=pins 0-7 on ATmega466  
#define LCD_DATA_HIGH_NBL   B   // on ATmega644, high and low nibbles are on the same port
#endif
#ifdef dataPins8to15 
#define LCD_DATA_LOW_NBL    D   // port D=pins 8-15 (note serial UART uses 8 and 9) 
#define LCD_DATA_HIGH_NBL   D   
#endif
#ifdef dataPins16to23          
#define LCD_DATA_LOW_NBL    C   // port C=pins 16-23
#define LCD_DATA_HIGH_NBL   C 
#endif
#ifdef dataPins24to31          
#define LCD_DATA_LOW_NBL    A   // port A=pins 24-31 (note these are the analog ports)
#define LCD_DATA_HIGH_NBL   A
#endif
*/

// ATmega644 macros to fast write data to pins, this version only works for pins 0-23
//#define fastWriteHigh(_pin_) ( _pin_ < 8 ?  PORTB |= 1 << (_pin_ & 0x07) : ( _pin_ < 16 ?  PORTD |= 1 << ((_pin_ -8) & 0x07) : PORTC |= 1 << ((_pin_ -16) & 0x07)  ) ) 
//#define fastWriteLow(_pin_) ( _pin_ < 8 ?   PORTB &= ~(1 << (_pin_  & 0x07)) : ( _pin_ < 16 ?  PORTD &= ~(1 << ((_pin_ -8) & 0x07) )  :  PORTC &= ~(1 << ((_pin_ -16) & 0x07) )  ) )
#define fastWriteHigh(p)  digitalWrite(p, HIGH)
#define fastWriteLow(p)  	digitalWrite(p, LOW)

#endif
