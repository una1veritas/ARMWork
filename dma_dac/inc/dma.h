/***********************************************************************
 * $Id::                                                               $
 *
 * Project:	LPCXpresso DMA + DAC Signal Generator
 * File:	dma.h
 * Description:
 * 			This project makes use of four wavetables which are
 * 			transferred to the LPC17xx's DAC via the GPDMA controller.
 * 			When used on the LPCXpresso Baseboard the DAC output is
 *			connected to the speaker. By using a spreadsheet to generate
 *			waveforms offline, and convert them into codewords for the
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
#ifndef DMA_H_
#define DMA_H_

#define BOUND(l, n, h) ((n)<(l) ? (l) : ((n)>(h) ? (h) : (n)))

typedef struct
{
	unsigned long int sourceAddr;
	unsigned long int destAddr;
	unsigned long int nextNode;
	unsigned long int dmaControl;
}dmaLinkedListNode;

extern const unsigned short triangle_buff[64];
extern const unsigned short sine_buff[64];
extern const unsigned short square_buff[64];
extern const unsigned short sine2_buff[64];
#define TABLE_ELEMENTS 64

void initDMA(dmaLinkedListNode* pList);
void updateBuffer(unsigned short* pSrcWave,unsigned int fractionalDeltaQ16);

#endif /* DMA_H_ */
