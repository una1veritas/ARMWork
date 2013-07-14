/**********************************************************************
* $Id$		I2s_Audio.c					2011-06-02
*//**
* @file		I2s_Audio.c
* @brief	This example describes how to use I2S to play a short demo
*			audio data on LPC18xx
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
**********************************************************************/
#include "lpc18xx_cgu.h"
#include "lpc18xx_i2s.h"
#include "lpc18xx_i2c.h"
#include "lpc18xx_scu.h"
#include "lpc18xx_libcfg.h"
#include "uda1380.h"
#include "lpc18xx_gpio.h"
#include "string.h"
#include "debug_frmwrk.h"


/* Example group ----------------------------------------------------------- */
/** @defgroup I2s_Audio	I2s_Audio
 * @ingroup I2S_Examples
 * @{
 */


/************************** PRIVATE DEFINITIONS *************************/
/** Max buffer length */
#define DATA_SIZE			15658
#define BUFFER_SIZE			1048 * 4
#define I2CDEV_UDA1380_ADDR		(0x34>>1)
#define I2CDEV_M LPC_I2C0

#define USEDI2CDEV_M		0

#define UDA1380_CLOCK_CONFIGURE	0x00
/** Max buffer length */

#if (USEDI2CDEV_M == 0)
#define I2CDEV_M LPC_I2C0
#elif (USEDI2CDEV_M == 2)
#define I2CDEV_M LPC_I2C2
#else
#error "Master I2C device not defined!"
#endif
/************************** PRIVATE VARIABLES ***********************/

uint8_t tx_buffer[BUFFER_SIZE];
uint32_t data_offset, buffer_offset,remain_data;
uint32_t tx_offset = 0;
Bool Tx_EndofBuf_F = FALSE;


/************************** PRIVATE FUNCTIONS *************************/
void I2S_Callback(void);
void I2S0_IRQHandler(void);

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		I2S IRQ Handler, call to send data to transmit buffer
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void I2S0_IRQHandler()
{

	uint32_t left 	= 0x00;
	uint32_t right 	= 0x00;

	left = (LPC_I2S0->RXFIFO);
	right = (LPC_I2S0->RXFIFO);

	LPC_I2S0->TXFIFO = left;
	LPC_I2S0->TXFIFO = right;
}


/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Initialize transmit buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Init(void)
{
	buffer_offset = 0;
	data_offset = BUFFER_SIZE;
	remain_data = DATA_SIZE - BUFFER_SIZE;
}

 
/***********************************************************************
 * I2C UDA1380 private functions
 **********************************************************************/
/* UDA1380 Register Address */
typedef enum {
  UDA_EVALM_CLK = 0x00,
  UDA_BUS_CTRL,
  UDA_POWER_CTRL,
  UDA_ANALOG_CTRL,
  UDA_HPAMP_CTRL,
  UDA_MASTER_VOL_CTRL = 0x10,
  UDA_MIXER_VOL_CTRL,
  UDA_MODE_CTRL,
  UDA_MUTE_CTRL,
  UDA_MIXER_FILTER_CTRL,
  UDA_DEC_VOL_CTRL = 0x20,
  UDA_PGA_CTRL,
  UDA_ADC_CTRL,
  UDA_AGC_CTRL,
  UDA_TOTAL_REG
}UDA1380_REG;

/* System Register Data Set */
unsigned short UDA_sys_regs_dat[]={
  /* UDA_EVALM_CLK */
  0xF <<8 | 0x3<<4 | 1,
  /* UDA_BUS_CTRL */
  0x00,
  /* UDA_POWER_CTRL */
  1<<15 | 1 <<13 | 1<<10 | 1<<8 | 0xF,	// Line-in turn on PGA and ADC
//(1 << 13) | (1 << 10) | ( 1 << 8) | (1 << 7) | (1 << 6) | (1 << 4) | (1<<2) | (1 << 1) | (1<<0),//	mic-in
  /* UDA_ANALOG_CTRL */
  0x0000,
  /* UDA_HPAMP_CTRL */
  1<<9 | 2
};

/* System Register Data Set */
unsigned short UDA_interfil_regs_dat[]={
  /* UDA_MASTER_VOL_CTRL */
  0x0000, /* MAX volume */
  /* UDA_MIXER_VOL_CTRL */
  0x0000,
  /* UDA_MODE_CTRL */
  0x0000,
  /* UDA_MUTE_CTRL */
  2<<8 | 2,
  /* UDA_MIXER_FILTER_CTRL */
  0x0000,
};
/* decimator Register Data Set */
unsigned short UDA_decimator_regs_dat[]={
  /* UDA_DEC_VOL_CTRL */
  0x0000,
  /* UDA_PGA_CTRL */
  0x0000,
  /* UDA_ADC_CTRL */
  0x0000,  //line-in
//(1<<3) | (1<<2) | (1<<1) | (7<<8) | (1<<12),//mic-in
  /* UDA_AGC_CTRL */
  0x0000

};


/***********************************************************************
 *
 * Function: delay
 *
 * Purpose: generate a delay
 *
 * Processing:
 *     A local software counter counts up to the specified count.
 *
 * Parameters:
 *    cnt : number to be counted
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void delay(UNS_32 cnt)
{
    UNS_32 i = cnt;
    while (i != 0) i--;
    return;
}

/* UDA register read function */
/***********************************************************************
 *
 * Function: UDA_Reg_read
 *
 * Purpose: UDA register read
 *
 * Processing:
 *     A local software reads from the specified UDA register.
 *
 * Parameters:
 *    UDA register 
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/

unsigned short UDA_Reg_read(UDA1380_REG reg)
{
  	I2C_M_SETUP_Type transferMCfg;
  	unsigned char TXdata, RXdata[2];
  	unsigned short result=0;
	TXdata = reg;
    transferMCfg.sl_addr7bit = I2CDEV_UDA1380_ADDR;
    transferMCfg.tx_data = &TXdata;
    transferMCfg.tx_length = 1;
    transferMCfg.rx_data = &RXdata[0];
    transferMCfg.rx_length = 2;
    transferMCfg.retransmissions_max = 5;
    I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
    result = RXdata[0]<<8 | RXdata[1];
	return result;
}

/* UDA register write function */
/***********************************************************************
 *
 * Function: UDA_Reg_write
 *
 * Purpose: UDA register write
 *
 * Processing:
 *     A local software write to the specified UDA register.
 *
 * Parameters:
 *    UDA register and value
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void UDA_Reg_write(UDA1380_REG reg, unsigned short value)
{
  I2C_M_SETUP_Type transferMCfg;
  UNS_8  tx_data[3];
  tx_data[0] = reg;
  tx_data[1] = value>>8;
  tx_data[2] = value&0xFF;
  transferMCfg.sl_addr7bit = I2CDEV_UDA1380_ADDR;
  transferMCfg.tx_data = &tx_data[0];
  transferMCfg.tx_length = 3;
  transferMCfg.rx_data = NULL;
  transferMCfg.rx_length = 0;
  transferMCfg.retransmissions_max = 5;
  I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
  delay(100);
}
/***********************************************************************
 *
 * Function: UDA1380_init
 *
 * Purpose: UDA1380 initialization
 *
 * Processing:
 *     A local software to initialize UDA1380.
 *
 * Parameters:
 *    None
 *
 * Outputs: bool
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
BOOL_32 UDA1380_init()
{
 	unsigned short temp;

  	/* Reset device */
	UDA_Reg_write((UDA1380_REG)0x7F, 0x00);

    /* Sys regs init */
	UDA_Reg_write(UDA_EVALM_CLK , 0x0502);
  	UDA_Reg_write(UDA_BUS_CTRL, UDA_sys_regs_dat[1]);
    temp = UDA_Reg_read(UDA_BUS_CTRL);
    if(temp!=UDA_sys_regs_dat[1]) return FALSE;
	UDA_Reg_write(UDA_POWER_CTRL , UDA_sys_regs_dat[2]);
    temp = UDA_Reg_read(UDA_POWER_CTRL);
    if(temp!=UDA_sys_regs_dat[2]) return FALSE;
	UDA_Reg_write(UDA_ANALOG_CTRL, UDA_sys_regs_dat[3]);
    temp = UDA_Reg_read(UDA_ANALOG_CTRL);
    if(temp!=UDA_sys_regs_dat[3]) return FALSE;
	UDA_Reg_write(UDA_HPAMP_CTRL, UDA_sys_regs_dat[4]);
    temp = UDA_Reg_read(UDA_HPAMP_CTRL);
    if(temp!=UDA_sys_regs_dat[4]) return FALSE;

  	/* interfilter regs init */
 
    UDA_Reg_write(UDA_MASTER_VOL_CTRL, UDA_interfil_regs_dat[0]);
    temp = UDA_Reg_read(UDA_MASTER_VOL_CTRL);
    if(temp!=UDA_interfil_regs_dat[0]) return FALSE;
	UDA_Reg_write(UDA_MIXER_VOL_CTRL, UDA_interfil_regs_dat[1]);
    temp = UDA_Reg_read(UDA_MIXER_VOL_CTRL);
    if(temp!=UDA_interfil_regs_dat[1]) return FALSE;
	UDA_Reg_write(UDA_MODE_CTRL, UDA_interfil_regs_dat[2]);
    temp = UDA_Reg_read(UDA_MODE_CTRL);
    if(temp!=UDA_interfil_regs_dat[2]) return FALSE;
	UDA_Reg_write(UDA_MUTE_CTRL, UDA_interfil_regs_dat[3]);
    temp = UDA_Reg_read(UDA_MUTE_CTRL);
    if(temp!=UDA_interfil_regs_dat[3]) return FALSE;
	UDA_Reg_write(UDA_MIXER_FILTER_CTRL, UDA_interfil_regs_dat[4]);
    temp = UDA_Reg_read(UDA_MIXER_FILTER_CTRL);
    if(temp!=UDA_interfil_regs_dat[4]) return FALSE;
  
  	/* decimator regs init */

    UDA_Reg_write(UDA_DEC_VOL_CTRL, UDA_decimator_regs_dat[0]);
    temp = UDA_Reg_read(UDA_DEC_VOL_CTRL);
    if(temp!=UDA_decimator_regs_dat[0]) return FALSE;
	UDA_Reg_write(UDA_PGA_CTRL, UDA_decimator_regs_dat[1]);
    temp = UDA_Reg_read(UDA_PGA_CTRL);
    if(temp!=UDA_decimator_regs_dat[1]) return FALSE;
	UDA_Reg_write(UDA_ADC_CTRL, UDA_decimator_regs_dat[2]);
    temp = UDA_Reg_read(UDA_ADC_CTRL);
    if(temp!=UDA_decimator_regs_dat[2]) return FALSE;
	UDA_Reg_write(UDA_AGC_CTRL, UDA_decimator_regs_dat[3]);
    temp = UDA_Reg_read(UDA_AGC_CTRL);
    if(temp!=UDA_decimator_regs_dat[3]) return FALSE;
  
	UDA_Reg_write(UDA_EVALM_CLK, UDA_sys_regs_dat[0]);
	
	return TRUE;
}

/***********************************************************************
 *
 * Function: UDA1380_ConfigI2S_DAI
 *
 * Purpose: UDA1380 configeration for I2S input.
 *
 * Processing:
 *     A local software to configeration for I2S input.
 *
 * Parameters:
 *    wordwidth, mono, ws_sel, ws_halfperiod 
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void UDA1380_ConfigI2S_DAI( uint32_t wordwidth,  uint32_t mono,  uint32_t ws_sel,  uint32_t ws_halfperiod)
{

	LPC_I2S0->DAI = 0x18 | wordwidth | (mono << 2) | (ws_sel << 5) | (ws_halfperiod << 6);
}

/***********************************************************************
 *
 * Function: UDA1380_ConfigI2S_DAO
 *
 * Purpose: UDA1380 configeration for I2S output.
 *
 * Processing:
 *     A local software to configeration for I2S put.
 *
 * Parameters:
 *    wordwidth, mono, ws_sel, ws_halfperiod 
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static  void UDA1380_ConfigI2S_DAO(  uint32_t wordwidth,  uint32_t mono,  uint32_t ws_sel,  uint32_t ws_halfperiod)
{
	LPC_I2S0->DAO = 0x18 | wordwidth | (mono << 2) | (ws_sel << 5) | (ws_halfperiod << 6);
}
/***********************************************************************
 *
 * Function: UDA1380_SetVolume
 *
 * Purpose: setting a volume for UDA1380.
 *
 * Processing:
 *     A local software to setting a volume for UDA1380.
 *
 * Parameters:
 *    	 volume
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void UDA1380_SetVolume( const uint16_t volume )
{
	UDA_Reg_write((UDA1380_REG)UDA1380_REG_DECVOL, volume);
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void) {                       /* Main Program */

/* 		This code will set I2S in MASTER mode, 44100Hz, Mono,16bit
 * 		This example is used to test I2S transmit mode. There is an audio
 * 		data in audiodata.c in mono 44100Hz 16 bit, it will be send out to
 * 		I2S port. User must plug Ext I2S DAC IC to hear sound.
 */

	uint8_t volumeValue=0x00;	  					// current volumeValue
	volatile uint16_t 	volVal2Channels=0;	 				// value of both channels
	SystemInit();
	CGU_Init();
	Buffer_Init();
	

	/* Reset UDA1380 on board Xplorer */
	scu_pinmux(2,10,MD_PDN, FUNC0);
	GPIO_SetDir(0, 1<<14, 1);
	GPIO_ClearValue(0, 1<<14);
	
/* Initialize I2S peripheral ------------------------------------*/
	/* Init I2C */
	I2C_Init(I2CDEV_M, 100000);
	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_M, ENABLE);
	/* Init UDA1380 CODEC */

/* Initialize I2S peripheral Xplorer------------------------------------*/
	scu_pinmux(3,0, MD_PLN_FAST, FUNC2);	 	//i2s TX_SCK
	scu_pinmux(3,1, MD_PLN_FAST, FUNC0);		//i2s TX_WS
	scu_pinmux(3,2, MD_PLN_FAST, FUNC0);	   	 //i2s TX_SDA
	
	scu_pinmux(6,0, MD_PLN_FAST, FUNC4);		//i2s RX_SCK
	scu_pinmux(6,1, MD_PLN_FAST, FUNC3);		 //i2s RX_WS
	scu_pinmux(6,2, MD_PLN_FAST, FUNC3);		//i2s RX_SDA

	/* 32-bits per channel mode */
	LPC_I2S0->DAO |= (1<<4) | (1<<3);
	LPC_I2S0->DAI |= (1<<4) | (1<<3);

	// MCLK rates
	LPC_I2S0->RXRATE = (4<<8) | (19);
	LPC_I2S0->TXRATE = (4<<8) | (19);

	// MCLK for RX
	LPC_I2S0->RXMODE = (1<<1);	// receive master -- generating WS
	
	// MCLK for TX
	LPC_I2S0->TXMODE = (1<<3);	// master mode


	// Bitclock rate is 64xFs = MCLK/4
	LPC_I2S0->TXBITRATE = 3;
	LPC_I2S0->RXBITRATE = 3;


	// 32-bit per channel mode.
	UDA1380_ConfigI2S_DAI(3 /*I2S_32BIT*/, 0 /*STEREO*/, 0/*MASTER*/, 32-1 /*I2S_HALFWORD32*/);
	UDA1380_ConfigI2S_DAO(3 /*I2S_32BIT*/, 0 /*STEREO*/, 0/*MASTER*/, 32-1 /*I2S_HALFWORD32*/);

	/* RESET I2S */
	LPC_I2S0->DAO &= ~((1<<4) | (1<<3));
	LPC_I2S0->DAI &= ~((1<<4) | (1<<3));

	 UDA1380_init();
  
 	volumeValue=0x60;
	volVal2Channels = ((uint16_t)volumeValue << 8) + (uint16_t)volumeValue;		// get value for two channels
	UDA1380_SetVolume( volVal2Channels );						// write new volume value of both channels to volume register	

	/* enable IRQ */
	LPC_I2S0->IRQ = (1<<0) | (4<<8) | (4<<16);
	NVIC_EnableIRQ(I2S0_IRQn);
    
	while(1);
}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}

#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/**
 * @}
 */
