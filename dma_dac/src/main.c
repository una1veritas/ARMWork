/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	LPCXpresso DMA + DAC Signal Generator
 * File:	main.c
 * Description:
 * 			Refer to AN10917 "Memory to DAC data transfers using the LPC1700's DMA"
 * 			for guidelines on where DMA buffer memory should be placed.
 *
 * 			This project makes use of four wave tables which are
 * 			transferred to the LPC17xx's DAC via the GPDMA controller.
 * 			When used on the LPCXpresso Baseboard the DAC output is
 *			connected to the speaker. By using a spreadsheet to generate
 *			waveforms off line, and convert them into codewords for the
 *			DAC, it becomes quite easy to use the GPDMA's linked list
 *			based scatter-gather to produce a repetitive signal on the DAC.
 *			The GPDMA timing for memory:peripherals is determined by the
 *			peripheral.
 *
 *			In this case, the DAC's counter register determines the rate at
 *			which transfers occur. Because the DAC is being used single
 *			transaction bursts are used.
 *
 *			A single linked list consisting of two nodes was used to double
 *			buffer output data. Both nodes point to each other, causing the
 *			GPDMA to transfer data to the DAC indefinitely.	When a new signal is selected, the GPDMA's
 *			linked list register is polled to determine when a transaction
 *			is complete, at that time the software will then update the
 *			contents of one of the buffers. This process is repeated again
 *			on the second buffer to ensure that the signal being produced
 *			is continuous.
 *
 ***********************************************************************
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
 **********************************************************************/


#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "dma.h"
#include "small_systick.h"

#define JOYSTICK_DOWN	(LPC_GPIO0->FIOPIN & (1<<15)) //Baseboard Joystick Down
#define JOYSTICK_UP 	(LPC_GPIO2->FIOPIN & (1<<3) )
#define JOYSTICK_LEFT 	(LPC_GPIO2->FIOPIN & (1<<4) )
#define JOYSTICK_RIGHT 	(LPC_GPIO0->FIOPIN & (1<<16) )
#define JOYSTICK_CENTER	(LPC_GPIO0->FIOPIN & (1<<17) )
#define TONE_TO_DIVISOR(toneFreq) (100000000/(toneFreq*8*TABLE_ELEMENTS))
#define Q16_MIN			(1<<14)
#define Q16_MAX			(1<<19)
#define Q16_INIT		(1<<16)
#define Q16_STEP		(1<<9)

volatile uint32_t SysTickCount;                            /* counts 10ms timeTicks */

static void initDAC(void)
{
	LPC_PINCON->PINSEL1 = 0x2<<20; 				//Select AOUT function on pin

	LPC_SC->PCLKSEL0 |= 0x3 << 22; 				//DAC Clocked at CCLK/8
	LPC_DAC->DACCNTVAL = TONE_TO_DIVISOR(440);	//Timer rate = 100MHz/(8*444) => Timer/64samples = Waveform freq ~= 440Hz
	LPC_DAC->DACCTRL = 1<<2 | 1<<3; 			//Enable counter and DMA requests
}

static unsigned short * joystickToWave(void)
{
	static unsigned char waveIndex=0;
	static enum {none,right,left,center}state = none;

	//Lazy debouncing
	if (state==none)
	{
		//check for new states
		//Update waveforms on Joystick down button
		if (0 == JOYSTICK_RIGHT )
		{
			waveIndex++;
			state = right;
		}
		else if (0 == JOYSTICK_LEFT )
		{
			waveIndex--;
			state = left;
		}
		else if ( 0 == JOYSTICK_CENTER )
		{
			//Reset
			waveIndex = 0;
			state = center;
		}
	}
	else
	{
		//wait for return to none
		if ( (JOYSTICK_RIGHT) && (JOYSTICK_LEFT) && (JOYSTICK_CENTER) )
			state = none;
	}

	switch( waveIndex & 0x3)
	{
	case 0:
	default:
		return (unsigned short*) &sine_buff[0];
		break;
	case 1:
		return (unsigned short*) &triangle_buff[0];
		break;
	case 2:
		return (unsigned short*) &square_buff[0];
		break;
	case 3:
		return (unsigned short*) &sine2_buff[0];
		break;
	}
}

static void joystickToPhase(unsigned long int *stepDeltaQ16)
{
	unsigned long int localDelta = *stepDeltaQ16;
	//Update waveforms on Joystick up button
	if ( 0 == JOYSTICK_UP )
	{
		localDelta += Q16_STEP; //+= Q16(1/128)
	}
	//cannot assert two positions on joystick at once
	else if (0 == JOYSTICK_DOWN)
	{
		localDelta -= Q16_STEP; //-= Q16(1/128)
	}
	else if ( 0 == JOYSTICK_CENTER)
	{
		//Reset
		localDelta = Q16_INIT;
	}
	*stepDeltaQ16 = BOUND(Q16_MIN,localDelta,Q16_MAX);
	return;
}

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
    SysTickCount++;                       	/* increment counter necessary in Delay() */
}

__attribute__ ((section(".ahb_ram")))
static unsigned long int dma_buff_ping[64];	//Having DMA buffers in RAM allows them to be filled with arbitrary waveforms
__attribute__ ((section(".ahb_ram")))
static unsigned long int dma_buff_pong[64];	//if an application only needs fixed waveforms, simply update LLI's to point to const arrays
__attribute__ ((section(".ahb_ram")))
static dmaLinkedListNode dmaList[2];		//Array of nodes, both will be set to point to each other creating a looping "list"

int main(void)
{
	unsigned char i;

	unsigned long int stepDeltaQ16 = Q16_INIT;	//Initial value: 1.0
	unsigned short* pSrc;
	dmaLinkedListNode* pNodePing = &dmaList[0];
	dmaLinkedListNode* pNodePong = &dmaList[1];

	//SystemInit(); //Called by startup code

	//Make initial image of waveform in both buffers
	//Buffers live in RAM to enable users to populate them with arbitrary waveforms
	//If constant waveforms are all that an application requires simply point LLI structures
	//below to point to FLASH
	pSrc = (unsigned short*) &sine_buff[0];
	for (i=0; i<TABLE_ELEMENTS; i++)
	{
		dma_buff_ping[i] = pSrc[i];
		dma_buff_pong[i] = pSrc[i];
	}

	pNodePing->sourceAddr = (unsigned long int) &dma_buff_ping[0];
	pNodePing->destAddr = (unsigned long int) &LPC_DAC->DACR;
	pNodePing->nextNode = (unsigned long int) pNodePong; //bounce between double buffers forever
	//Size is 64 buffer elements, 32bits each
	//Transferred into DAC peripheral as 32 bits
	//Increment source address, but not destination
	pNodePing->dmaControl = TABLE_ELEMENTS | 0x2<<18 | 0x2<<21 | 1<<26;

	//Different sourceAddr for 2nd LLI node, but otherwise the same
	pNodePong->sourceAddr = (unsigned long int) &dma_buff_pong[0];
	pNodePong->destAddr = (unsigned long int) &LPC_DAC->DACR;
	pNodePong->nextNode = (unsigned long int) pNodePing; //bounce between double buffers forever
	pNodePong->dmaControl = TABLE_ELEMENTS | 0x2<<18 | 0x2<<21 | 1<<26;

	InitSysTick(100);
	initDAC();
	initDMA(pNodePing);

	while(1)
	{
		//go to sleep, wake up in 10ms
		__WFI();

		pSrc=joystickToWave();
		joystickToPhase(&stepDeltaQ16);
		updateBuffer(pSrc,stepDeltaQ16);
		updateBuffer(pSrc,stepDeltaQ16);
	}
	return 0 ;
}
