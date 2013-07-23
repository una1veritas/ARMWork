/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	LPCXpresso DMA + DAC Signal Generator
 * File:	dma.c
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

#include "dma.h"
#include "LPC17xx.h"

/* 64 Element Triangle Wave, computed offline and shifted to match format of LPC_DAC->DACR (not setting BIAS bit) */
const unsigned short triangle_buff[64] = {
		0x0000	,
		0x07C0	,
		0x0F80	,
		0x1740	,
		0x1F00	,
		0x26C0	,
		0x2E80	,
		0x3640	,
		0x3E00	,
		0x45C0	,
		0x4D80	,
		0x5540	,
		0x5D00	,
		0x64C0	,
		0x6C80	,
		0x7440	,
		0x7C00	,
		0x83C0	,
		0x8B80	,
		0x9340	,
		0x9B00	,
		0xA2C0	,
		0xAA80	,
		0xB240	,
		0xBA00	,
		0xC1C0	,
		0xC980	,
		0xD140	,
		0xD900	,
		0xE0C0	,
		0xE880	,
		0xF040	,
		0xF800	,
		0xF800	,
		0xF040	,
		0xE880	,
		0xE0C0	,
		0xD900	,
		0xD140	,
		0xC980	,
		0xC1C0	,
		0xBA00	,
		0xB240	,
		0xAA80	,
		0xA2C0	,
		0x9B00	,
		0x9340	,
		0x8B80	,
		0x83C0	,
		0x7C00	,
		0x7440	,
		0x6C80	,
		0x64C0	,
		0x5D00	,
		0x5540	,
		0x4D80	,
		0x45C0	,
		0x3E00	,
		0x3640	,
		0x2E80	,
		0x26C0	,
		0x1F00	,
		0x1740	,
		0x0F80	};

/* 64 Element Sine Wave, computed offline and shifted to match format of LPC_DAC->DACR (not setting BIAS bit) */
const unsigned short sine_buff[64] = {
		0x8000	,
		0x8C80	,
		0x9900	,
		0xA500	,
		0xB100	,
		0xBC40	,
		0xC700	,
		0xD100	,
		0xDA40	,
		0xE2C0	,
		0xEA40	,
		0xF0C0	,
		0xF600	,
		0xFA40	,
		0xFD40	,
		0xFF40	,
		0xFFC0	,
		0xFF40	,
		0xFD40	,
		0xFA40	,
		0xF600	,
		0xF0C0	,
		0xEA40	,
		0xE2C0	,
		0xDA40	,
		0xD100	,
		0xC700	,
		0xBC40	,
		0xB100	,
		0xA500	,
		0x9900	,
		0x8C80	,
		0x8000	,
		0x7380	,
		0x6700	,
		0x5B00	,
		0x4F00	,
		0x43C0	,
		0x3900	,
		0x2F00	,
		0x25C0	,
		0x1D40	,
		0x15C0	,
		0x0F40	,
		0x0A00	,
		0x05C0	,
		0x02C0	,
		0x00C0	,
		0x0040	,
		0x00C0	,
		0x02C0	,
		0x05C0	,
		0x0A00	,
		0x0F40	,
		0x15C0	,
		0x1D40	,
		0x25C0	,
		0x2F00	,
		0x3900	,
		0x43C0	,
		0x4F00	,
		0x5B00	,
		0x6700	,
		0x7380	};

/* 64 Element Square Wave, computed offline and shifted to match format of LPC_DAC->DACR (not setting BIAS bit) */
const unsigned short square_buff[64] = {
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0x2000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	,
		0xE000	};

/* 64 Element Amplitude Modulated Sine Wave, computed offline and shifted to match format of LPC_DAC->DACR (not setting BIAS bit) */
const unsigned short sine2_buff[64]={
		0x8000	,
		0xBAC0	,
		0xDA40	,
		0xE540	,
		0xDFC0	,
		0xCEC0	,
		0xB640	,
		0x9AC0	,
		0x8000	,
		0x6980	,
		0x5940	,
		0x50C0	,
		0x5000	,
		0x5640	,
		0x6200	,
		0x7080	,
		0x8000	,
		0x8E00	,
		0x98C0	,
		0x9EC0	,
		0xA000	,
		0x9C40	,
		0x94C0	,
		0x8AC0	,
		0x8000	,
		0x7600	,
		0x6E00	,
		0x6940	,
		0x6800	,
		0x6A80	,
		0x7000	,
		0x7780	,
		0x8000	,
		0x8800	,
		0x8E40	,
		0x9240	,
		0x9340	,
		0x9140	,
		0x8D00	,
		0x86C0	,
		0x8000	,
		0x7980	,
		0x7440	,
		0x7100	,
		0x7000	,
		0x7180	,
		0x7540	,
		0x7A40	,
		0x8000	,
		0x8580	,
		0x8A00	,
		0x8D00	,
		0x8DC0	,
		0x8C80	,
		0x8940	,
		0x8500	,
		0x8000	,
		0x7B40	,
		0x7740	,
		0x74C0	,
		0x7400	,
		0x7500	,
		0x77C0	,
		0x7B80	};

#if 0 //Random waveform
const unsigned short rand_buff[64]={
		0x17DB	,
		0x759C	,
		0x126D	,
		0xB8DE	,
		0x2D02	,
		0x120C	,
		0x052C	,
		0x8CF5	,
		0xF339	,
		0xC9D4	,
		0x4DD3	,
		0x3019	,
		0x4376	,
		0x93D6	,
		0x9FB9	,
		0x4701	,
		0x1235	,
		0x286E	,
		0xFAD2	,
		0xA558	,
		0x4980	,
		0x19AD	,
		0xE9B7	,
		0x7AC5	,
		0x740D	,
		0xBD76	,
		0x8F5F	,
		0xD9E5	,
		0x15E3	,
		0x6A57	,
		0xF5CC	,
		0x77D6	,
		0xABC6	,
		0xD686	,
		0x170E	,
		0xA772	,
		0x8A07	,
		0x13E2	,
		0x2126	,
		0xE9FA	,
		0x4136	,
		0x1BB4	,
		0xF8C9	,
		0x1A41	,
		0x13C5	,
		0xB03F	,
		0x6272	,
		0xDC42	,
		0xC765	,
		0x9665	,
		0xB1B3	,
		0x7640	,
		0x4F0C	,
		0x7FB2	,
		0xE64F	,
		0x847D	,
		0x22B0	,
		0xA90B	,
		0x1FD0	,
		0x75EA	,
		0x938E	,
		0xC934	,
		0x6DBC	,
		0x213B	};
#endif

/*
 * initDMA:		Initialize GPDMA controller to transfer intial buffer
 * 				and Linked List Item.
 *
 * Arguments:	pList - a pointer to a linked list node which will
 * 				be used after the initial transfer is complete.
 *
 */
void initDMA(dmaLinkedListNode* pList)
{
	LPC_SC->PCONP |= 1<<29;		//Power GPDMA module

	LPC_GPDMA->DMACConfig = 1; 	//Enable GPDMA

	//Clear any previous interrupts
	LPC_GPDMA->DMACIntTCClear = 0xFF;

	//Initialize the channel with previously configured LL;
	LPC_GPDMACH0->DMACCSrcAddr = pList->sourceAddr;
	LPC_GPDMACH0->DMACCDestAddr = pList->destAddr;
	LPC_GPDMACH0->DMACCControl = pList->dmaControl;
	LPC_GPDMACH0->DMACCLLI = (unsigned long int) pList & 0xFFFFFFFC; //Lower bits must be 0

	//Set up all relevant bits
	//SrcPeripheral = 0, DestPeripheral = DAC
	LPC_GPDMACH0->DMACCConfig = 7<<6 | 1<<11;

	//Finally, enable the channel -
	LPC_GPDMACH0->DMACCConfig |= 1<<0;
}

/*
 * updateBuffer:Modify data in DMA buffer. Will block until requested
 * 				buffer is not being accessed by GPDMA, then proceed to
 * 				update contents.
 *
 * Arguments:	pSrcWave - pointer to array of DAC codewords
 * 				delta - step size
 *
 */
void updateBuffer(unsigned short* pSrcWave,unsigned int fractionalDeltaQ16)
{
	unsigned int i;
	unsigned int fractionalIndexQ16 = 0;
	unsigned int lastIndex = 0;
	unsigned long int* pDestWaveBuff;
	dmaLinkedListNode* pNodeToUpdate;

	//Local pointers make use of 'active' DMA LLI...
	pNodeToUpdate = (dmaLinkedListNode*) LPC_GPDMACH0->DMACCLLI;
	pDestWaveBuff = (unsigned long int*) pNodeToUpdate->sourceAddr;

	//Wait until another buffer becomes active
	// !! NOTE !!
	//The highest frequency the project will produce is ~10KHz.
	//This updating method does not make any guarantee as to the duration of real time
	//required to perform an update. As such, higher frequency signals and/or signals
	//with larger buffers may not swap properly.
	while( (dmaLinkedListNode*) LPC_GPDMACH0->DMACCLLI == pNodeToUpdate)
	{
		;
	}

	//Now that another buffer is being transferred, it is safe to modify.
	//At SystemCoreClock=100MHz it is assumed that traversing this loop
	//will require less than the 100uS required to update buffers at 10KHz
	for ( i=0; i<TABLE_ELEMENTS; i++, fractionalIndexQ16 += fractionalDeltaQ16 )
	{
		if ( ((fractionalIndexQ16>>16) &0x3F) < lastIndex)
		{
			//wrap around has occurred, exit loop and update count
			break;
		}

		//Q16 fixed point fractional representation
		pDestWaveBuff[i] = pSrcWave[(fractionalIndexQ16>>16) &0x3F];
		//Record "last" seen index for boundary checking
		lastIndex = (fractionalIndexQ16>>16) &0x3F;
	}

	//If no wrap around has occurred i=64, otherwise i<64
	//This prevents strange waveforms from being loaded when frequency is reduced
	//and/or reset via center joystick position.
	pNodeToUpdate->dmaControl &= ~0xFFF;
	pNodeToUpdate->dmaControl |= BOUND(0,i-1,63);
}
