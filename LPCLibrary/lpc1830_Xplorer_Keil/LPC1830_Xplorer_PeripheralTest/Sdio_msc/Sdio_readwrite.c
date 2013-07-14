/**********************************************************************
* $Id$		Sdio_readwrite.c		2011-06-02
*//**
* @file		Sdio_readwrite.c
* @brief	This example describes how to read and write from/to SD Card
* 			It also measures read and write speed.
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
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
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "lpc_sdmmc.h"
#include "lpc18xx_sdif.h"
#include "lpc18xx_sdmmc.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_timer.h"
#include "lpc18xx_libcfg_default.h"
#include "debug_frmwrk.h"



/* Example group ----------------------------------------------------------- */
/** @defgroup SDIO_readwrite		SDIO_readwrite
 * @ingroup SDIO_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS ***********************/
/* CAUTION!!!!!!! READ THIS FIRST!!!!!!!
   Only enable write mode if you are using a SD/MMC card that can
   affort to lose data. This example actively writes to the card and
   will overwrite any data there corrupting files or file data. You
   may need to re-format your card after using this example with
   write enable. Use with care! */
#define ENABLE_WRITE_MODE

/* If you want to try multiple sector read/write capability, enable
   this define. Multiple read/write uses a different command vs the
   single sector read write for faster data transfer rates */
#define USE_MULTPLE_TRANSFER

/* Maximum multiple sector transfer size, 128 max limited byy size
   of memory */
#define MULT_XFER_MAX_SECTORS 28

/* I've found it's easier to understand the pullup/pulldown defines
   seperate instead of using the combo MD_* macros */
#define MD_ENAB_PD (1<<3) /* Enable pull down resistor at pad */
#define MD_DIS_PU (1<<4) /* Disable pullup resistor at pad */

/* SD clock/data pins have fast slew rate, not glitch filter,
   buffered input, and no pulldown or pullup. Note that the board
   already has pullups on the necessary SD signals */
#define SDFASTINOUTPIN (MD_DIS_PU | MD_EZI | MD_EHS | MD_ZI)


#ifdef USE_MULTPLE_TRANSFER
static uint32_t lbuff[(MULT_XFER_MAX_SECTORS * MMC_SECTOR_SIZE) / sizeof(uint32_t)];
#else
static uint32_t lbuff[MMC_SECTOR_SIZE / sizeof(uint32_t)];
#endif
volatile uint32_t u32Milliseconds;
static volatile uint32_t mci_status, Leddelay = 500;


/* Keeping errorc global makes it easier to see it in the debugger */
int32_t errorc;
uint32_t tstartr, tstopr, tstartw, tstopw;
struct _mci_card_struct sdcardinfo;
static volatile int32_t sdio_wait_exit = 0;

/************************** PRIVATE FUNCTION *************************/
void print_menu(void);
static int verify_data(int sectors, int initval, int inc);
static void prep_data(int initval, int inc);
void sd_entry(void);
Bool Sdio_verification = TRUE;
/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Print menu
 * @param[in]	None
 * @return 		None
 **********************************************************************/


/*******************************************************************************
* @brief		Wait for card program to finish
* @param[in]    None
* @return		None
*******************************************************************************/
 static void wait_for_program_finish(void)
{
	/* Poll card for program completion */
	while (sdmmc_get_state() != SDMMC_TRAN_ST);
}


/*******************************************************************************
* @brief		SDIO interrupt handler callback
* @param[in]    rinsts Optional input parameter
* @return		Return value is 0, not currently used
*******************************************************************************/
static uint32_t sdmmc_irq(uint32_t rinsts)
{
	/* Set wait exit flag to tell wait function we are ready. In an RTOS,
	   this would trigger wakeup of a thread waiting for the IRQ. */
	NVIC_DisableIRQ(SDIO_IRQn);
	sdio_wait_exit = 1;

	return 0;
}

/*********************************************************************//**
* @brief 		Sets up the SD event driven wakeup
* @param[in]	bits Status bits to poll for command completion
* @return 		None
**********************************************************************/
static void sdmmc_setup_wakeup(uint32_t bits)
{
    /* Wait for IRQ - for an RTOS, you would pend on an event here
	   with a IRQ based wakeup. */
	NVIC_ClearPendingIRQ(SDIO_IRQn);
	sdio_wait_exit = 0;
	LPC_SDMMC->INTMASK = bits;
	NVIC_EnableIRQ(SDIO_IRQn);
}

/*********************************************************************//**
* @brief 		A better wait callback for SDMMC driven by the IRQ flag
* @param[in]	bits Status bits to poll for command completion
* @return 		0 on success, or failure condition (-1)
**********************************************************************/
static uint32_t sdmmc_irq_driven_wait(uint32_t bits)
{
    uint32_t status;

	/* Wait for event, would be nice to have a timeout, but keep it
	   simple */
	while (sdio_wait_exit == 0);

	/* Get status and clear interrupts */
	status = LPC_SDMMC->RINTSTS;
	LPC_SDMMC->RINTSTS = status;
	LPC_SDMMC->INTMASK = 0;

	return status;
}

/*********************************************************************//**
* @brief 		Delay callback for timed SDIF/SDMMC functions
* @param[in]	time Number of milliSeconds to wait
* @return 		None
**********************************************************************/
void sdmmc_waitms(uint32_t time)
{
	/* In an RTOS, the thread would sleep allowing other threads to
	   run. For standalone operation, we just spin on a timer */
	TIM_Waitus(time * 1000);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		Main SDIO program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
void Sdio_init(void)
{

	uint32_t dms, sdio_clk;
	int32_t loops, blk;
	uint8_t buf[50];
	errorc = 1;

	/* Configure the IO's for the LEDs */
		/* Setup muxing for SD interface */
		/*Xplorer*/
	scu_pinmux(0x1 ,11 , SDFASTINOUTPIN, FUNC7);      /* P1.11 SDIO D2 */
	scu_pinmux(0x1 ,10 , SDFASTINOUTPIN, FUNC7);      /* P1.10 SDIO D1 */
	scu_pinmux(0x1 ,9 , SDFASTINOUTPIN, FUNC7);      /* P1.9 SDIO D0 */
	scu_pinmux(0x1 ,6, SDFASTINOUTPIN, FUNC7);      /* P1.6 SDIO command */
	scu_pinmux(0x1 ,12 , SDFASTINOUTPIN, FUNC7);      /* P1.12 SDIO D3 */

	LPC_SCU->SFSCLK_2 = 0x04;						/*	CLK2 Enable*/


	CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_SDIO);

	/* Generate interrupt @ 100 Hz */

	/* The SDIO driver needs to know the SDIO clock rate */
	sdio_clk = CGU_GetPCLKFrequency(CGU_PERIPHERAL_SDIO);

	/* This init sdio with sdio_clk */
	sdif_init(sdio_clk, sdmmc_irq);

	/* Wait for a card to be inserted (note CD is not on the
	   SDMMC power rail and can be polled without enabling
	   SD slot power */

	/* Allow some time for the power supply to settle and the
	   card to fully seat in the slot */
	dms = u32Milliseconds + 250;
	while (dms > u32Milliseconds);

	/* Enumerate the card once detected. Note this function may
	   block for a little while. */
	if (!sdmmc_acquire(sdmmc_setup_wakeup, sdmmc_irq_driven_wait,
		sdmmc_waitms, &sdcardinfo)) {
		errorc = -1;
		goto error_exit;
	}
	lpc_printf("SD card detected and initialized..!\r\n");
	/* Setup card specific callbacks - use driver functions, but these can
	   be changed to custom functions or unique functions per slot. These
	   aren't used in the drivers, so setup of these is optional, but they
	   are setup here to be complete. */
	sdcardinfo.sdck_det = sdif_card_ndetect;
	sdcardinfo.sdck_wp = sdif_card_wp_on;
	sdcardinfo.sd_setpow = sdif_power_onoff;
	sdcardinfo.sd_setrst = sdif_reset;
	memset(lbuff, 0, sizeof(lbuff));
	if (sdmmc_read_blocks((void *) lbuff, 0, 0) == 0) {
		errorc = -2;
		goto error_exit;
	}
#ifdef ENABLE_WRITE_MODE
	lpc_printf("Writing data to sector 1 of SD card and verifying:\r\n");
	/* Write a single sector of data (512 bytes) */
	if (!sdcardinfo.sdck_wp()) {
		prep_data(0, 1);
		/* Warning: This may corrupt SD card data! */
		if (sdmmc_write_blocks((void *) lbuff, 1, 1) == 0) {
			errorc = -3;
			goto error_exit;
		}
		/* Wait for write to finish (at the card) */
		wait_for_program_finish();
	}
#endif
	memset(lbuff, 0, sizeof(lbuff));
	/* Read a single sector of data (512 bytes) */
	if (sdmmc_read_blocks((void *) lbuff, 1, 1) == 0) {
		errorc = -4;
		goto error_exit;
	}
#ifdef ENABLE_WRITE_MODE
	/* Verify data and halt if an error occurs */
	if (!sdcardinfo.sdck_wp()) {
		if (verify_data(1, 0, 1) == 0) {
			errorc = -5;
			goto error_exit;
		}
		lpc_printf("Verified!\r\n");
	}
#endif

#ifdef USE_MULTPLE_TRANSFER
#ifdef ENABLE_WRITE_MODE
 	lpc_printf("Writing data in Multitransfer mode and verifying:\r\n");
	/* Write data using multiple sector write */
	if (!sdcardinfo.sdck_wp()) {
		prep_data(0x10, 3);
		/* Warning: This may corrupt SD card data! */
		if (sdmmc_write_blocks((void *) lbuff, 1, MULT_XFER_MAX_SECTORS) == 0) {
			errorc = -6;
			goto error_exit;
		}
		wait_for_program_finish();
	}
#endif
	memset(lbuff, 0, sizeof(lbuff));
	/* Read data using multiple sector read */
	if (sdmmc_read_blocks((void *) lbuff, 1, MULT_XFER_MAX_SECTORS) == 0) {
		errorc = -7;
		goto error_exit;
	}

#ifdef ENABLE_WRITE_MODE
	/* Verify data and halt if an error occurs */
	if (!sdcardinfo.sdck_wp()) {
		if (verify_data(MULT_XFER_MAX_SECTORS, 0x10, 3) == 0) {
			errorc = -8;
			goto error_exit;
		}
		lpc_printf("Verified!\r\n");
	}
#endif
   	lpc_printf("measure continuous read speed...\r\n");
	tstartr = u32Milliseconds;
	loops = 1000;
	blk = 64;
	while (loops-- > 0) {
		if (sdmmc_read_blocks((void *) lbuff, blk, (blk + MULT_XFER_MAX_SECTORS - 1)) == 0) {
			errorc = -8;
			goto error_exit;
		}

		blk += MULT_XFER_MAX_SECTORS;
	}
	tstopr = u32Milliseconds;
	memset(buf, 0, sizeof(buf));
	lpc_printf("read speed = %d kB/s\r\n", MULT_XFER_MAX_SECTORS*512*1000/(tstopr-tstartr));

#ifdef ENABLE_WRITE_MODE
	lpc_printf("measure continuous write speed...\r\n");

	if (!sdcardinfo.sdck_wp()) {
		tstartw = u32Milliseconds;
		loops = 200;
		blk = 64;
		while (loops-- > 0) {
			if (sdmmc_write_blocks((void *) lbuff, blk, (blk + MULT_XFER_MAX_SECTORS - 1)) == 0) {
				errorc = -9;
				goto error_exit;
			}
			blk += MULT_XFER_MAX_SECTORS;
			wait_for_program_finish();
		}
		tstopw = u32Milliseconds;
	}
	memset(buf, 0, sizeof(buf));
	lpc_printf("write speed = %d kB/s\r\n", MULT_XFER_MAX_SECTORS*512*200/(tstopw-tstartw));
#endif
#endif
	goto exit;
	error_exit:
	lpc_printf("SD Card read/write operation not succeeded.\r\n");
	Sdio_verification = FALSE;
exit:
	return;
}
void Test_sdcard(void)
{
	if (Sdio_verification!=FALSE)
	{
		#ifdef ENABLE_WRITE_MODE
			/* Write a single sector of data (512 bytes) */
			if (!sdcardinfo.sdck_wp())
			{
				prep_data(0, 1);
				/* Warning: This may corrupt SD card data! */
				if (sdmmc_write_blocks((void *) lbuff, 1, 1) == 0) 
				{
					errorc = -3;
					goto error_exit1;
				}
				/* Wait for write to finish (at the card) */
				wait_for_program_finish();
			}
		#endif
		memset(lbuff, 0, sizeof(lbuff));
		/* Read a single sector of data (512 bytes) */
		if (sdmmc_read_blocks((void *) lbuff, 1, 1) == 0) 
		{
			errorc = -4;
			goto error_exit1;
		}
		#ifdef ENABLE_WRITE_MODE
		/* Verify data and halt if an error occurs */
		if (!sdcardinfo.sdck_wp()) 
		{
			if (verify_data(1, 0, 1) == 0) 
			{
				errorc = -5;
				goto error_exit1;
			}
			lpc_printf("SD card Verified!");
			Sdio_verification = TRUE;
			goto exit1;
		}
		#endif	
	

exit1:
	return;	
}	
error_exit1:
	lpc_printf("\n\rSD Card read/write operation not succeeded.");
	Sdio_verification = FALSE;
}


/*******************************************************************************
* @brief		Generates a data pattern in a buffer
* @param[in]    initval Initial value
* @param[in]    inc Increment value
* @return		None
*******************************************************************************/
static void prep_data(int initval, int inc)
{
	int i;
	unsigned char *p = (unsigned char *) lbuff;

	for (i = 0; i < sizeof(lbuff); i++) {
		p[i] = ((unsigned char) initval) & 0xff;
		initval += inc;
	}
}

/*******************************************************************************
* @brief		Verifies a data pattern in a buffer
* @param[in]	sectors Numbers of sectors to check, 512 bytes per sector
* @param[in]    initval Initial value
* @param[in]    inc Increment value
* @return		0 if the verify fails, otherwise !0
*******************************************************************************/
static int verify_data(int sectors, int initval, int inc)
{
	int i = 0, passed = 1;
	uint8_t *cbuf = (uint8_t *) lbuff;
	uint8_t tab;

	while (i < (sectors * MMC_SECTOR_SIZE)) {
	tab = cbuf[i];
		if (cbuf[i] != (uint8_t) (initval & 0xff)){
			passed = 0;break;
			}
		initval += inc;
		i++;
	}
	tab = tab;
	return passed;
}

/**
 * @}
 */
