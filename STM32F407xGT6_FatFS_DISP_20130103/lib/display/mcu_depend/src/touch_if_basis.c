/********************************************************************************/
/*!
	@file			touch_if_basis.c
	@author         Nemui Trinomius (http://nemuisan.blog.bai.ne.jp)
    @version        1.00
    @date           2011.05.30
	@brief          Interface of Touch Panel Hardware Depend Layer			@n
					Based On "BlueScreen"'s Touch Driver Thunks !			@n 
					Currentry ADS7843 ONLY!!!

    @section HISTORY
		2010.05.30	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "touch_if_basis.h"
/* check header file version for fool proof */
#if __TOUCH_IF_BASIS_H != 0x0100
#error "header file version is not correspond!"
#endif

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
	Initalize TouchScreen I/O.
*/
/**************************************************************************/
void TC_IoInit_If(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* OUTPUTS */
	RCC_AHB1PeriphClockCmd(TOUCH_CLK_DCLK, ENABLE);
	RCC_AHB1PeriphClockCmd(TOUCH_CLK_CS, ENABLE);
	RCC_AHB1PeriphClockCmd(TOUCH_CLK_DIN, ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= TOUCH_DCLK;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_Init(TOUCH_PORT_DCLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= TOUCH_CS;
	GPIO_Init(TOUCH_PORT_CS, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= TOUCH_DIN;
	GPIO_Init(TOUCH_PORT_DIN, &GPIO_InitStructure);

	/* INPUTS */
	RCC_AHB1PeriphClockCmd(TOUCH_CLK_DOUT, ENABLE);
	RCC_AHB1PeriphClockCmd(TOUCH_CLK_PENIRQ, ENABLE);

	GPIO_InitStructure.GPIO_Pin 	= TOUCH_DOUT;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(TOUCH_PORT_DOUT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= TOUCH_PENIRQ;
	GPIO_Init(TOUCH_PORT_PENIRQ, &GPIO_InitStructure);

    /* I/O Initial State */
	TOUCH_DCLK_SET();
	TOUCH_DIN_CLR();
	TOUCH_CS_SET();

	/* Enable the PWR APB1 Clock Interface */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
}

/**************************************************************************/
/*! 
	Restore Calibrated Value.
*/
/**************************************************************************/
void TC_Restore_Calivalue(void)
{
	ccx  = RTC_ReadBackupRegister(RTC_BKP_DR9);
	cm1x = RTC_ReadBackupRegister(RTC_BKP_DR10);
	cm2x = RTC_ReadBackupRegister(RTC_BKP_DR11);

	ccy  = RTC_ReadBackupRegister(RTC_BKP_DR12);
	cm1y = RTC_ReadBackupRegister(RTC_BKP_DR13);
	cm2y = RTC_ReadBackupRegister(RTC_BKP_DR14);
}

/**************************************************************************/
/*! 
	Restore Calibrated Value.
*/
/**************************************************************************/
void TC_Store_Calivalue(void)
{
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Write Baby Flag */
	RTC_WriteBackupRegister(RTC_BKP_DR6, 0xA5A5A5A5);
	
	/* Write Caliblated Values */
	RTC_WriteBackupRegister(RTC_BKP_DR9,  ccx);
	RTC_WriteBackupRegister(RTC_BKP_DR10, cm1x);
	RTC_WriteBackupRegister(RTC_BKP_DR11, cm2x);

	RTC_WriteBackupRegister(RTC_BKP_DR12, ccy);
	RTC_WriteBackupRegister(RTC_BKP_DR13, cm1y);
	RTC_WriteBackupRegister(RTC_BKP_DR14, cm2y);

	/* Lock access to BKP Domain */
	PWR_BackupAccessCmd(DISABLE);

}

/* End Of File ---------------------------------------------------------------*/
