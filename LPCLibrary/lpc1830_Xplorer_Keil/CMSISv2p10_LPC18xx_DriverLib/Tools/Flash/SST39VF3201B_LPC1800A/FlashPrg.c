/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright KEIL ELEKTRONIK GmbH 2003 - 2006                         */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.C:  Flash Programming Functions adapted                   */
/*               for SST39x3201B (16-bit Bus) on the LPC18xx and       */
/*               LPC18xx board version 2 from Hitex					   */
/*                                                                     */
/***********************************************************************/

#include "..\FlashOS.H"        // FlashOS Structures
#include "LPC18xx.h"                    /* LPC18xx definitions                */
#include "config.h"
//#include "lpc18xx_scu.h"
//#include "lpc18xx_clkpwr.h"

// Defines
#define M8(adr)  (*((volatile unsigned char  *) (adr)))
#define M16(adr) (*((volatile unsigned short *) (adr)))
#define M32(adr) (*((volatile unsigned long  *) (adr)))

#define STACK_SIZE   64        // Stack Size


// Parameter definitions

union fsreg 	// Flash Status Register
{                  
  struct b  
  {
    unsigned int q0:1;
    unsigned int q1:1;
    unsigned int q2:1;
    unsigned int q3:1;
    unsigned int q4:1;
    unsigned int q5:1;
    unsigned int q6:1;
    unsigned int q7:1;
  } b;
  unsigned int v;
} fsr;

unsigned long base_adr;



/************************************************************************
 * Check if Program/Erase completed
 *    Parameter:      adr:  Block Start Address
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int Polling (unsigned long adr) 
{
  unsigned int q6;

  // Check Toggle Bit
  do 
  {
    fsr.v = M8(adr);
    q6 = fsr.b.q6;
    fsr.v = M8(adr);
  } 
  while (fsr.b.q6 != q6);

  return (0);              // Done

}


/************************************************************************
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int Init (unsigned long adr, unsigned long clk, unsigned long fnc) 
{	
	base_adr = adr;

	// Disable interrupts
	__disable_irq();

	// Reset chip
	//LPC_RGU->RESET_CTRL0 |= 1<<10 | 1<<16 | 1<<21 | 1<<28; // reset some peripherals

	// Configure clocks and stuff- use 12 meg irc
	//CLKPWR_Init();
	SystemInit();
	/* Set PL160M @ 6*12=72MHz */
	SetPL160M(SRC_IRC, 6);							
	/* Run base M3 clock from PL160M, div by 1 = no division */
	SetClock(BASE_M3_CLK, SRC_PL160M_0, DIV1);

	// Set all EMI related pins to the correct function
	// Data bus (16 bits wide)
	scu_pinmux(1,7,MD_PLN_FAST,FUNC3); // D0
	scu_pinmux(1,8,MD_PLN_FAST,FUNC3); // D1
	scu_pinmux(1,9,MD_PLN_FAST,FUNC3); // D2
	scu_pinmux(1,10,MD_PLN_FAST,FUNC3);// D3
	scu_pinmux(1,11,MD_PLN_FAST,FUNC3);// D4
	scu_pinmux(1,12,MD_PLN_FAST,FUNC3);// D5
	scu_pinmux(1,13,MD_PLN_FAST,FUNC3);// D6
	scu_pinmux(1,14,MD_PLN_FAST,FUNC3);// D7
	scu_pinmux(5,4,MD_PLN_FAST,FUNC2); // D8
	scu_pinmux(5,5,MD_PLN_FAST,FUNC2); // D9
	scu_pinmux(5,6,MD_PLN_FAST,FUNC2); // D10
	scu_pinmux(5,7,MD_PLN_FAST,FUNC2); // D11
	scu_pinmux(5,0,MD_PLN_FAST,FUNC2); // D12
	scu_pinmux(5,1,MD_PLN_FAST,FUNC2); // D13
	scu_pinmux(5,2,MD_PLN_FAST,FUNC2); // D14
	scu_pinmux(5,3,MD_PLN_FAST,FUNC2); // D15

	// Address bus
	scu_pinmux(2,9,MD_PLN_FAST,FUNC3);  // A0 (not used)
	scu_pinmux(2,10,MD_PLN_FAST,FUNC3); // A1 on LPC18xx connected to A0 on flash
	scu_pinmux(2,11,MD_PLN_FAST,FUNC3); // A2
	scu_pinmux(2,12,MD_PLN_FAST,FUNC3); // A3
	scu_pinmux(2,13,MD_PLN_FAST,FUNC3); // A4
	scu_pinmux(1,0,MD_PLN_FAST,FUNC2);  // A5
	scu_pinmux(1,1,MD_PLN_FAST,FUNC2);  // A6
	scu_pinmux(1,2,MD_PLN_FAST,FUNC2);  // A7
	scu_pinmux(2,8,MD_PLN_FAST,FUNC3);  // A8
	scu_pinmux(2,7,MD_PLN_FAST,FUNC3);  // A9
	scu_pinmux(2,6,MD_PLN_FAST,FUNC2);  // A10
	scu_pinmux(2,2,MD_PLN_FAST,FUNC2);  // A11
	scu_pinmux(2,1,MD_PLN_FAST,FUNC2);  // A12
	scu_pinmux(2,0,MD_PLN_FAST,FUNC2);  // A13
	scu_pinmux(6,8,MD_PLN_FAST,FUNC1);  // A14
	scu_pinmux(6,7,MD_PLN_FAST,FUNC1);  // A15
	scu_pinmux(0xD,16,MD_PLN_FAST,FUNC2);  // A16
	scu_pinmux(0xD,15,MD_PLN_FAST,FUNC2);  // A17
	scu_pinmux(0xE,0,MD_PLN_FAST,FUNC3);  // A18
	scu_pinmux(0xE,1,MD_PLN_FAST,FUNC3);  // A19
	scu_pinmux(0xE,2,MD_PLN_FAST,FUNC3);  // A20

	// misc signals
	scu_pinmux(1,6,MD_PLN_FAST,FUNC3);
	scu_pinmux(1,3,MD_PLN_FAST,FUNC3);
	scu_pinmux(1,5,MD_PLN_FAST,FUNC3);
	scu_pinmux(1,4,MD_PLN_FAST,FUNC3);
	scu_pinmux(6,3,MD_PLN_FAST,FUNC3);
	scu_pinmux(6,6,MD_PLN_FAST,FUNC1);
	scu_pinmux(0xD,12,MD_PLN_FAST,FUNC2);
	scu_pinmux(0xD,13,MD_PLN_FAST,FUNC2);
	scu_pinmux(0xD,11,MD_PLN_FAST,FUNC2);
	scu_pinmux(0xD,10,MD_PLN_FAST,FUNC2);

	// initialise the control pin signal registers
	// 70ns 16-bit Flash on CS0  
	*(uint32_t *)0x40005000 = 0x00000001;      // Enable
	*(uint32_t *)0x40005200 = 0x00000081;      // CS0: 16 bit, WE + buffer
	*(uint32_t *)0x40005208 = 0x00000004;      // CS0: WAITOEN = 1
	*(uint32_t *)0x4000520C = 0x00000009;      // CS0: WAITRD = 6 

  	return (0);
}



/************************************************************************
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int UnInit (unsigned long fnc) 
{
  	return (0);
}



/************************************************************************
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int EraseChip (void) 
{

  // Start Chip Erase Command
  M16(base_adr + (0x555 << 1)) = 0x00AA;
  M16(base_adr + (0x2AA << 1)) = 0x0055;
  M16(base_adr + (0x555 << 1)) = 0x0080;
  M16(base_adr + (0x555 << 1)) = 0x00AA;
  M16(base_adr + (0x2AA << 1)) = 0x0055;
  M16(base_adr + (0x555 << 1)) = 0x0010;

  return (Polling(base_adr));  // Wait until Erase completed
}


/************************************************************************
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int EraseSector (unsigned long adr) 
{

  // Start Erase Sector Command
  M16(base_adr + (0x555 << 1)) = 0x00AA;
  M16(base_adr + (0x2AA << 1)) = 0x0055;
  M16(base_adr + (0x555 << 1)) = 0x0080;
  M16(base_adr + (0x555 << 1)) = 0x00AA;
  M16(base_adr + (0x2AA << 1)) = 0x0055;
  M16(adr) = 0x0050;

  return (Polling(adr));       // Wait until Erase completed
}


/************************************************************************
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 ************************************************************************/

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) 
{
  int i;

  for (i = 0; i < ((sz+1)/2); i++)  
  {
	// Start Program Command
    M16(base_adr + (0x555 << 1)) = 0x00AA;
    M16(base_adr + (0x2AA << 1)) = 0x0055;
    M16(base_adr + (0x555 << 1)) = 0x00A0;
    M16(adr) = *((unsigned short *) buf);
    if (Polling(adr) != 0) return (1);
    buf += 2;
    adr += 2;
  }
  return (0);
}

