/*****************************************************************************
 *   $Id:: Main.c 5686 2010-11-22 23:10:37Z nxp28548                $		 *
 *   Project: Division API Example											 *
 *																			 *
 *   Description:															 *
 *     This project uses the LPC11Uxx division API library. 				 *
 																			 *
 *																			 *
 ****************************************************************************
 ****************************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include "LPC11Uxx.h"
//#include "lpc_rom_div.h"
#include "power_api.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 80

/*	The value at this address is the entry to ROM Division API.
 	Once it is dereferenced, individual API functions can be used*/
#define LPC_1XXX_ROM_LOC          (0x1FFF1FF8)

ROM **rom;
LPC_ROM_DIV_STRUCT const *pROMDiv;
	
//128 random integers to be used as dividends
const int32_t I32Dividend[128] = {
-60488,-30734,-49047,10477,-38033,-42087,-26846,12177,
-21223,16695,-14641,-21146,-28065,-63818,-32806,-31955,
51851,44721,11942,10631,14640,15402,-36631,49311,
-11185,27209,39860,4160,-43086,-64795,39646,36227,
9414,49789,38500,-19681,7030,-37457,14833,-40626,
-16919,39803,48587,52466,-25588,-53326,-54707,-13375,
35106,-43909,62373,-64136,-64615,-16499,-19361,59163,
-40569,-59710,-41399,-5622,47027,-5958,40766,-50326,
-12738,-19124,-45572,-13584,-40405,18255,-8713,29736,
9335,-65521,64862,-65328,23094,-12466,-48703,16497,
-32584,16326,28216,-21184,49428,-38921,22668,-5411,
50943,62933,-44277,7027,-31864,59469,60244,-5654,
-39887,53527,62682,-60888,19815,-60700,-43044,-22373,
53692,35036,48997,-34920,24844,22455,42731,8986,
1343,-2738,-64052,-12002,-64498,30883,41538,23513,
12972,53852,10253,37127,-42121,-59625,4396,1};

/* All ODD */
const int32_t I32Divisors[10] = {
-65535,
-50973,
-36407,
-21845,
-7281,
7281,
21845,
36407,
50971,
65535};

//128 random integers to be used as dividends
const uint32_t U32Dividend[128] = {
37480,14630,59349,20616,727,29784,56556,53166,
35237,41271,5595,15292,584,64419,45175,16422,
54257,20743,47280,41897,32357,52687,49510,39581,
42111,23909,62764,33169,15120,41470,27970,26685,
53346,48250,16791,1106,52928,61694,48311,27409,
296,15554,50785,56576,59036,58686,42133,23097,
27734,53884,54617,45139,61208,51263,48398,56460,
25747,26194,28351,40838,8283,28159,35392,60440,
16042,32831,39771,55751,57807,20074,49494,22623,
41750,53899,26952,25397,25292,45553,30311,3465,
62089,32014,2318,64519,17685,27594,46654,43949,
21759,25898,56353,41855,41908,49719,38278,36397,
3587,47762,41135,32928,47650,51386,13337,42919,
13589,58831,26206,44312,55993,43156,43659,23484,
7941,55863,49132,3391,14682,40049,5438,2297,
37729,51349,56372,14944,63047,10421,2522,1};

/* All ODD */
const uint32_t U32Divisors[10] = {
1,
7283,
14563,
21845,
29127,
36407,
43691,
50973,
58253,
65535};

static voidUpdateResults(uint32_t avgCyclesLib, uint32_t avgCyclesC, uint32_t opCounter)
{
	char lineBuff[BUF_SIZE];
	sprintf(lineBuff,"Cycles (Std C):%d, [%d Avg]\r\n", 
		avgCyclesC, (*rom)->pROMDiv->uidiv(avgCyclesC,opCounter) );
	print_string( (uint8_t*) lineBuff);
	sprintf(lineBuff,"Cycles (Lib  ):%d, [%d Avg]", 
		avgCyclesLib, (*rom)->pROMDiv->uidiv(avgCyclesLib,opCounter) );
	print_string( (uint8_t*) lineBuff);
	sprintf(lineBuff," [%d%% Increase]\r\n", 
		(*rom)->pROMDiv->uidiv(100*avgCyclesC, avgCyclesLib) );
	print_string( (uint8_t*) lineBuff);
}

int main (void)
{
	uint32_t x,y;
	int32_t  sa, compiler_a;	
	uint32_t ua, compiler_ua;
	uint32_t compiler_amod;
	uint32_t compiler_uamod;
	uint32_t avgCyclesLib;
	uint32_t avgCyclesC;
	uint32_t opCounter;
    idiv_return si_result;
	uidiv_return ui_result;	
 
	SystemCoreClockUpdate();

	UARTInit(9600);
	NVIC_DisableIRQ(UART_IRQn);

	rom = (ROM **) LPC_1XXX_ROM_LOC;     //pointer for ALL ROM calls
	
	avgCyclesLib = 0;
	avgCyclesC = 0;
	opCounter = 0;
	print_string( "[1] Signed Division\r\n" );
	//For each dividend
	for (x=0; x<128; x++)
	{
		//for each divisor
		for (y=0; y<10; y++)
		{
			/* Test signed division on library */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			sa = (*rom)->pROMDiv->sidiv(I32Dividend[x],I32Divisors[y]);
			LPC_CT32B0->TCR = 0;
			avgCyclesLib +=	LPC_CT32B0->TC;	
			
			/* Compiler based signed div */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			compiler_a = I32Dividend[x] / I32Divisors[y];
			LPC_CT32B0->TCR = 0;
			avgCyclesC += LPC_CT32B0->TC;
			
			if( (sa != compiler_a) )
			{					  
				print_string("Failure\r\n");
				while(1);
			}			
			opCounter++;			
		}
	}
	voidUpdateResults(avgCyclesLib, avgCyclesC, opCounter);

	avgCyclesLib = 0;
	avgCyclesC = 0;
	opCounter = 0;
	print_string( "[2] Signed Division w Mod.\r\n" );
	//For each dividend
	for (x=0; x<128;x++)
	{
		//for each divisor
		for (y=0;y<10;y++)
		{
			/* Test signed division on library */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			si_result = (*rom)->pROMDiv->sidivmod(I32Dividend[x],I32Divisors[y]);
			LPC_CT32B0->TCR = 0;
			avgCyclesLib +=	LPC_CT32B0->TC;	
			
			/* Compiler based signed div */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			compiler_amod = I32Dividend[x] % I32Divisors[y];
			LPC_CT32B0->TCR = 0;
			avgCyclesC += LPC_CT32B0->TC;
			
			if( si_result.rem != compiler_amod )
			{
				print_string("Failure\r\n");
				while(1);
			}			
			opCounter++;			
		}
	}
	voidUpdateResults(avgCyclesLib, avgCyclesC, opCounter);


	avgCyclesLib = 0;
	avgCyclesC = 0;
	opCounter = 0;
	print_string( "[3] Unsigned Division\r\n" );
	//For each dividend
	for (x=0; x<128;x++)
	{
		//for each divisor
		for (y=1;y<10;y++)
		{
			/* Test unsigned division on library */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			ua = (*rom)->pROMDiv->uidiv(U32Dividend[x],U32Divisors[y]);
			LPC_CT32B0->TCR = 0;
			avgCyclesLib +=	LPC_CT32B0->TC;	
			
			/* Compiler based unsigned div */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			compiler_ua = U32Dividend[x] / U32Divisors[y];
			LPC_CT32B0->TCR = 0;
			avgCyclesC += LPC_CT32B0->TC;
			
			if( ua != compiler_ua )
			{
				print_string("Failure\r\n");
				while(1);
			}			
			opCounter++;			
		}
	}
	voidUpdateResults(avgCyclesLib, avgCyclesC, opCounter);

	avgCyclesLib = 0;
	avgCyclesC = 0;
	opCounter = 0;
	print_string( "[4] Unsigned Division w Mod.\r\n" );
	//For each dividend
	for (x=0; x<128;x++)
	{
		//for each divisor
		for (y=0;y<10;y++)
		{
			/* Test unsigned division on library */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			ui_result = (*rom)->pROMDiv->uidivmod(U32Dividend[x],U32Divisors[y]);
			LPC_CT32B0->TCR = 0;
			avgCyclesLib +=	LPC_CT32B0->TC;	
			
			/* Compiler based unsigned div */
			LPC_CT32B0->TCR = 2;
			LPC_CT32B0->TCR = 0;
			LPC_CT32B0->TCR = 1;
			compiler_uamod = U32Dividend[x] % U32Divisors[y];
			LPC_CT32B0->TCR = 0;
			avgCyclesC += LPC_CT32B0->TC;
						
			if( ui_result.rem != compiler_uamod )
			{
				print_string("Failure\r\n");
				while(1);
			}			
			opCounter++;			
		}
	}
	voidUpdateResults(avgCyclesLib, avgCyclesC, opCounter);

	print_string("Testing Complete");
	while(1) 
	{
		__WFI();
	}
}
