/**
  ******************************************************************************
  * @file    stm322xg_eval_lcd.c
  * @author  MCD Application Team
  * @version V4.6.2
  * @date    22-July-2011
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *          and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *          of STM322xG-EVAL evaluation board(MB786) RevA and RevB.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm324xg_eval_lcd.h"
#include "../Common/fonts.c"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM322xG_EVAL
  * @{
  */
    
/** @defgroup STM322xG_EVAL_LCD 
  * @brief This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320)
  *        and AM240320D5TOQW01H (LCD_ILI9325) Liquid Crystal Display Modules
  *        of STM322xG_EVAL board.
  * @{
  */ 

/** @defgroup STM322xG_EVAL_LCD_Private_TypesDefinitions
  * @{
  */  /*
typedef struct
{
  __IO uint16_t LCD_REG;
  __IO uint16_t LCD_RAM;
} LCD_TypeDef;	*/
/**
  * @}
  */ 
/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
//#define DISP_HOR_RESOLUTION				320
//#define DISP_VER_RESOLUTION				240

//#define DISP_ORIENTATION					0
#define DISP_ORIENTATION					90
//#define DISP_ORIENTATION					180
//#define DISP_ORIENTATION					270

/* Private define ------------------------------------------------------------*/

#if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )

#define  MAX_X  320
#define  MAX_Y  240   

#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )

#define  MAX_X  240
#define  MAX_Y  320   

#endif

/** @defgroup STM322xG_EVAL_LCD_Private_Defines
  * @{
  */
/* Note: LCD /CS is NE3 - Bank 3 of NOR/SRAM Bank 1~4 */	 
#define LCD_BASE           ((uint32_t)(0x6F000000 | 0x00010000))
#define LCD                ((LCD_TypeDef *) LCD_BASE)
#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
/**
  * @}
  */ 

/** @defgroup STM322xG_EVAL_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))     
/**
  * @}
  */ 
    
/** @defgroup STM322xG_EVAL_LCD_Private_Variables
  * @{
  */ 
static sFONT *LCD_Currentfonts;

  /* Global variables to set the written text color */
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
  
/**
  * @}
  */ 


/** @defgroup STM322xG_EVAL_LCD_Private_FunctionPrototypes
  * @{
  */ 
#ifndef USE_Delay
static void delay_ms(__IO uint32_t nCount);
#endif /* USE_Delay*/
static void PutPixel(int16_t x, int16_t y);
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed);


/**
  * @}
  */ 
/* Private define ------------------------------------------------------------*/

#define LCD_REG              (*((volatile unsigned short *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x60020000)) /* RS = 1 */

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t DeviceCode = 0;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */	   
#define  HX8347D    10 /* 0x0047 */
#define  HX8347A    11 /* 0x0047 */	
#define  LGDP4535   12 /* 0x4535 */  
#define  SSD2119    13 /* 3.5 LCD 0x9919 */

/** @defgroup STM322xG_EVAL_LCD_Private_Functions
  * @{
  */ 

/**
  * @brief  DeInitializes the LCD.
  * @param  None
  * @retval None
  */
void LCD_DeInit(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
	
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 

	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                   +

	+-------------------+--------------------+
	*/
	/* GPIOD configuration */
		/* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.11(A16), PD.14(D0), PD.15(D1) as alternate function push pull */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | 
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */	
	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
	                             GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void STM324xG_LCD_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//LCD_CS PD7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOD , GPIO_Pin_7);		 //CS=0;
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* Configure the FSMC Parallel interface -------------------------------------*/
  LCD_FSMCConfig();

  delay_ms(5); /* delay 50 ms */

  /* Read the LCD ID */
  DeviceCode = LCD_ReadReg(0x00);  

  /* Check if the LCD is ILI9320 Controller */
  if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )	
	{
		DeviceCode = ILI9325;
		LCD_WriteReg(0x00e7,0x0010);      
		LCD_WriteReg(0x0000,0x0001);  	/* start internal osc */
		LCD_WriteReg(0x0001,0x0100);     
		LCD_WriteReg(0x0002,0x0700); 	/* power on sequence */
		LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 	/* importance */
		LCD_WriteReg(0x0004,0x0000);                                   
		LCD_WriteReg(0x0008,0x0207);	           
		LCD_WriteReg(0x0009,0x0000);         
		LCD_WriteReg(0x000a,0x0000); 	/* display setting */        
		LCD_WriteReg(0x000c,0x0001);	/* display setting */        
		LCD_WriteReg(0x000d,0x0000); 			        
		LCD_WriteReg(0x000f,0x0000);
		/* Power On sequence */
		LCD_WriteReg(0x0010,0x0000);   
		LCD_WriteReg(0x0011,0x0007);
		LCD_WriteReg(0x0012,0x0000);                                                                 
		LCD_WriteReg(0x0013,0x0000);                 
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0010,0x1590);   
		LCD_WriteReg(0x0011,0x0227);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012,0x009c);                  
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013,0x1900);   
		LCD_WriteReg(0x0029,0x0023);
		LCD_WriteReg(0x002b,0x000e);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020,0x0000);                                                            
		LCD_WriteReg(0x0021,0x0000);           
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0030,0x0007); 
		LCD_WriteReg(0x0031,0x0707);   
		LCD_WriteReg(0x0032,0x0006);
		LCD_WriteReg(0x0035,0x0704);
		LCD_WriteReg(0x0036,0x1f04); 
		LCD_WriteReg(0x0037,0x0004);
		LCD_WriteReg(0x0038,0x0000);        
		LCD_WriteReg(0x0039,0x0706);     
		LCD_WriteReg(0x003c,0x0701);
		LCD_WriteReg(0x003d,0x000f);
		delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0050,0x0000);        
		LCD_WriteReg(0x0051,0x00ef);   
		LCD_WriteReg(0x0052,0x0000);     
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0xa700);        
		LCD_WriteReg(0x0061,0x0001); 
		LCD_WriteReg(0x006a,0x0000);
		LCD_WriteReg(0x0080,0x0000);
		LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0082,0x0000);
		LCD_WriteReg(0x0083,0x0000);
		LCD_WriteReg(0x0084,0x0000);
		LCD_WriteReg(0x0085,0x0000);
		  
		LCD_WriteReg(0x0090,0x0010);     
		LCD_WriteReg(0x0092,0x0000);  
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0110);
		LCD_WriteReg(0x0097,0x0000);        
		LCD_WriteReg(0x0098,0x0000);  
		/* display on sequence */    
		LCD_WriteReg(0x0007,0x0133);
		
		LCD_WriteReg(0x0020,0x0000);
		LCD_WriteReg(0x0021,0x0000);
	}
	else if( DeviceCode == 0x9320 || DeviceCode == 0x9300 )
	{
	    DeviceCode = ILI9320;
	    LCD_WriteReg(0x00,0x0000);
		LCD_WriteReg(0x01,0x0100);	/* Driver Output Contral */
		LCD_WriteReg(0x02,0x0700);	/* LCD Driver Waveform Contral */
		LCD_WriteReg(0x03,0x1018);	/* Entry Mode Set */
		
		LCD_WriteReg(0x04,0x0000);	/* Scalling Contral */
	    LCD_WriteReg(0x08,0x0202);	/* Display Contral */
		LCD_WriteReg(0x09,0x0000);	/* Display Contral 3.(0x0000) */
		LCD_WriteReg(0x0a,0x0000);	/* Frame Cycle Contal.(0x0000) */
	    LCD_WriteReg(0x0c,(1<<0));	/* Extern Display Interface Contral */
		LCD_WriteReg(0x0d,0x0000);	/* Frame Maker Position */
		LCD_WriteReg(0x0f,0x0000);	/* Extern Display Interface Contral 2. */
		
	    delay_ms(100);  /* delay 100 ms */		
		LCD_WriteReg(0x07,0x0101);	/* Display Contral */
	    delay_ms(100);  /* delay 100 ms */		
	
		LCD_WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	/* Power Control 1.(0x16b0)	*/
		LCD_WriteReg(0x11,0x0007);								/* Power Control 2 */
		LCD_WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				/* Power Control 3.(0x0138)	*/
		LCD_WriteReg(0x13,0x0b00);								/* Power Control 4 */
		LCD_WriteReg(0x29,0x0000);								/* Power Control 7 */
		
		LCD_WriteReg(0x2b,(1<<14)|(1<<4));
			
		LCD_WriteReg(0x50,0);       /* Set X Start */
		LCD_WriteReg(0x51,239);	    /* Set X End */
		LCD_WriteReg(0x52,0);	    /* Set Y Start */
		LCD_WriteReg(0x53,319);	    /* Set Y End */
		
		LCD_WriteReg(0x60,0x2700);	/* Driver Output Control */
		LCD_WriteReg(0x61,0x0001);	/* Driver Output Control */
		LCD_WriteReg(0x6a,0x0000);	/* Vertical Srcoll Control */
		
		LCD_WriteReg(0x80,0x0000);	/* Display Position? Partial Display 1 */
		LCD_WriteReg(0x81,0x0000);	/* RAM Address Start? Partial Display 1 */
		LCD_WriteReg(0x82,0x0000);	/* RAM Address End-Partial Display 1 */
		LCD_WriteReg(0x83,0x0000);	/* Displsy Position? Partial Display 2 */
		LCD_WriteReg(0x84,0x0000);	/* RAM Address Start? Partial Display 2 */
		LCD_WriteReg(0x85,0x0000);	/* RAM Address End? Partial Display 2 */
		
	    LCD_WriteReg(0x90,(0<<7)|(16<<0));	/* Frame Cycle Contral.(0x0013)	*/
		LCD_WriteReg(0x92,0x0000);	/* Panel Interface Contral 2.(0x0000) */
		LCD_WriteReg(0x93,0x0001);	/* Panel Interface Contral 3. */
	    LCD_WriteReg(0x95,0x0110);	/* Frame Cycle Contral.(0x0110)	*/
		LCD_WriteReg(0x97,(0<<8));	
		LCD_WriteReg(0x98,0x0000);	/* Frame Cycle Contral */
	
	    LCD_WriteReg(0x07,0x0173);
	}
	else if( DeviceCode == 0x9331 )
	{
	    DeviceCode = ILI9331;
		LCD_WriteReg(0x00E7, 0x1014);
		LCD_WriteReg(0x0001, 0x0100);   /* set SS and SM bit */
		LCD_WriteReg(0x0002, 0x0200);   /* set 1 line inversion */
		LCD_WriteReg(0x0003, 0x1030);   /* set GRAM write direction and BGR=1 */
		LCD_WriteReg(0x0008, 0x0202);   /* set the back porch and front porch */
	    LCD_WriteReg(0x0009, 0x0000);   /* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(0x000A, 0x0000);   /* FMARK function */
		LCD_WriteReg(0x000C, 0x0000);   /* RGB interface setting */
		LCD_WriteReg(0x000D, 0x0000);   /* Frame marker Position */
		LCD_WriteReg(0x000F, 0x0000);   /* RGB interface polarity */
		/* Power On sequence */
		LCD_WriteReg(0x0010, 0x0000);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
		LCD_WriteReg(0x0011, 0x0007);   /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x0012, 0x0000);   /* VREG1OUT voltage	*/
		LCD_WriteReg(0x0013, 0x0000);   /* VDV[4:0] for VCOM amplitude */
	    delay_ms(200);  /* delay 200 ms */		
		LCD_WriteReg(0x0010, 0x1690);   /* SAP, BT[3:0], AP, DSTB, SLP, STB	*/
		LCD_WriteReg(0x0011, 0x0227);   /* DC1[2:0], DC0[2:0], VC[2:0] */
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0012, 0x000C);   /* Internal reference voltage= Vci	*/
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0013, 0x0800);   /* Set VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x0029, 0x0011);   /* Set VCM[5:0] for VCOMH */
		LCD_WriteReg(0x002B, 0x000B);   /* Set Frame Rate */
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0020, 0x0000);   /* GRAM horizontal Address */
		LCD_WriteReg(0x0021, 0x0000);   /* GRAM Vertical Address */
		/* Adjust the Gamma Curve */
		LCD_WriteReg(0x0030, 0x0000);
		LCD_WriteReg(0x0031, 0x0106);
		LCD_WriteReg(0x0032, 0x0000);
		LCD_WriteReg(0x0035, 0x0204);
		LCD_WriteReg(0x0036, 0x160A);
		LCD_WriteReg(0x0037, 0x0707);
		LCD_WriteReg(0x0038, 0x0106);
		LCD_WriteReg(0x0039, 0x0707);
		LCD_WriteReg(0x003C, 0x0402);
		LCD_WriteReg(0x003D, 0x0C0F);
		/* Set GRAM area */
		LCD_WriteReg(0x0050, 0x0000);   /* Horizontal GRAM Start Address */
		LCD_WriteReg(0x0051, 0x00EF);   /* Horizontal GRAM End Address */
		LCD_WriteReg(0x0052, 0x0000);   /* Vertical GRAM Start Address */
		LCD_WriteReg(0x0053, 0x013F);   /* Vertical GRAM Start Address */
		LCD_WriteReg(0x0060, 0x2700);   /* Gate Scan Line */
		LCD_WriteReg(0x0061, 0x0001);   /*  NDL,VLE, REV */
		LCD_WriteReg(0x006A, 0x0000);   /* set scrolling line */
		/* Partial Display Control */
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		/* Panel Control */
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);
		LCD_WriteReg(0x0007,0x0021);		
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0007,0x0061);
	    delay_ms(50);  /* delay 50 ms */		
		LCD_WriteReg(0x0007,0x0133);    /* 262K color and display ON */
	}
	else if( DeviceCode == 0x9919 )
	{
	    DeviceCode = SSD2119;
	    /* POWER ON &RESET DISPLAY OFF */
		LCD_WriteReg(0x28,0x0006);
		LCD_WriteReg(0x00,0x0001);		
		LCD_WriteReg(0x10,0x0000);		
		LCD_WriteReg(0x01,0x72ef);
		LCD_WriteReg(0x02,0x0600);
		LCD_WriteReg(0x03,0x6a38);	
		LCD_WriteReg(0x11,0x6874);
		LCD_WriteReg(0x0f,0x0000);    /* RAM WRITE DATA MASK */
		LCD_WriteReg(0x0b,0x5308);    /* RAM WRITE DATA MASK */
		LCD_WriteReg(0x0c,0x0003);
		LCD_WriteReg(0x0d,0x000a);
		LCD_WriteReg(0x0e,0x2e00);  
		LCD_WriteReg(0x1e,0x00be);
		LCD_WriteReg(0x25,0x8000);
		LCD_WriteReg(0x26,0x7800);
		LCD_WriteReg(0x27,0x0078);
		LCD_WriteReg(0x4e,0x0000);
		LCD_WriteReg(0x4f,0x0000);
		LCD_WriteReg(0x12,0x08d9);
		/* Adjust the Gamma Curve */
		LCD_WriteReg(0x30,0x0000);
		LCD_WriteReg(0x31,0x0104);	 
		LCD_WriteReg(0x32,0x0100);	
	    LCD_WriteReg(0x33,0x0305);	
	    LCD_WriteReg(0x34,0x0505);	 
		LCD_WriteReg(0x35,0x0305);	
	    LCD_WriteReg(0x36,0x0707);	
	    LCD_WriteReg(0x37,0x0300);	
		LCD_WriteReg(0x3a,0x1200);	
		LCD_WriteReg(0x3b,0x0800);		 
	    LCD_WriteReg(0x07,0x0033);
	}
	else if( DeviceCode == 0x1505 || DeviceCode == 0x0505 )
	{
	    DeviceCode = R61505U;
	   /* second release on 3/5  ,luminance is acceptable,water wave appear during camera preview */
	    LCD_WriteReg(0x0007,0x0000);
	    delay_ms(50);  /* delay 50 ms */		
	    LCD_WriteReg(0x0012,0x011C);    /* why need to set several times?	*/
	    LCD_WriteReg(0x00A4,0x0001);    /* NVM */
	    LCD_WriteReg(0x0008,0x000F);
	    LCD_WriteReg(0x000A,0x0008);
	    LCD_WriteReg(0x000D,0x0008);    
	    /* GAMMA CONTROL */
	    LCD_WriteReg(0x0030,0x0707);
	    LCD_WriteReg(0x0031,0x0007); 
	    LCD_WriteReg(0x0032,0x0603); 
	    LCD_WriteReg(0x0033,0x0700); 
	    LCD_WriteReg(0x0034,0x0202); 
	    LCD_WriteReg(0x0035,0x0002); 
	    LCD_WriteReg(0x0036,0x1F0F);
	    LCD_WriteReg(0x0037,0x0707); 
	    LCD_WriteReg(0x0038,0x0000); 
	    LCD_WriteReg(0x0039,0x0000); 
	    LCD_WriteReg(0x003A,0x0707); 
	    LCD_WriteReg(0x003B,0x0000); 
	    LCD_WriteReg(0x003C,0x0007); 
	    LCD_WriteReg(0x003D,0x0000); 
	    delay_ms(50);  /* delay 50 ms */		
	    LCD_WriteReg(0x0007,0x0001);
	    LCD_WriteReg(0x0017,0x0001);    /* Power supply startup enable */
	    delay_ms(50);  /* delay 50 ms */		
	    /* power control */
	    LCD_WriteReg(0x0010,0x17A0); 
	    LCD_WriteReg(0x0011,0x0217);    /* reference voltage VC[2:0]   Vciout = 1.00*Vcivl */
	    LCD_WriteReg(0x0012,0x011E);    /* Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?	*/
	    LCD_WriteReg(0x0013,0x0F00);    /* VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl */
	    LCD_WriteReg(0x002A,0x0000);  
	    LCD_WriteReg(0x0029,0x000A);    /* Vcomh = VCM1[4:0]*Vreg1out    gate source voltage?? */
	    LCD_WriteReg(0x0012,0x013E);    /* power supply on */
	    /* Coordinates Control */
	    LCD_WriteReg(0x0050,0x0000);
	    LCD_WriteReg(0x0051,0x00EF); 
	    LCD_WriteReg(0x0052,0x0000); 
	    LCD_WriteReg(0x0053,0x013F); 
	    /* Pannel Image Control */
	    LCD_WriteReg(0x0060,0x2700); 
	    LCD_WriteReg(0x0061,0x0001); 
	    LCD_WriteReg(0x006A,0x0000); 
	    LCD_WriteReg(0x0080,0x0000); 
	    /* Partial Image Control */
	    LCD_WriteReg(0x0081,0x0000); 
	    LCD_WriteReg(0x0082,0x0000); 
	    LCD_WriteReg(0x0083,0x0000); 
	    LCD_WriteReg(0x0084,0x0000); 
	    LCD_WriteReg(0x0085,0x0000); 
	    /* Panel Interface Control */
	    LCD_WriteReg(0x0090,0x0013);      /* frenqucy */	
	    LCD_WriteReg(0x0092,0x0300); 
	    LCD_WriteReg(0x0093,0x0005); 
	    LCD_WriteReg(0x0095,0x0000); 
	    LCD_WriteReg(0x0097,0x0000); 
	    LCD_WriteReg(0x0098,0x0000); 
	  
	    LCD_WriteReg(0x0001,0x0100); 
	    LCD_WriteReg(0x0002,0x0700); 
	    LCD_WriteReg(0x0003,0x1030); 
	    LCD_WriteReg(0x0004,0x0000); 
	    LCD_WriteReg(0x000C,0x0000); 
	    LCD_WriteReg(0x000F,0x0000); 
	    LCD_WriteReg(0x0020,0x0000); 
	    LCD_WriteReg(0x0021,0x0000); 
	    LCD_WriteReg(0x0007,0x0021); 
	    delay_ms(200);  /* delay 200 ms */		
	    LCD_WriteReg(0x0007,0x0061); 
	    delay_ms(200);  /* delay 200 ms */		
	    LCD_WriteReg(0x0007,0x0173); 
	}							 
	else if( DeviceCode == 0x8989 )
	{
	    DeviceCode = SSD1289;
	    LCD_WriteReg(0x0000,0x0001);    delay_ms(50);   /* Enable LCD Oscillator */
	    LCD_WriteReg(0x0003,0xA8A4);    delay_ms(50);   
	    LCD_WriteReg(0x000C,0x0000);    delay_ms(50);   
	    LCD_WriteReg(0x000D,0x080C);    delay_ms(50);   
	    LCD_WriteReg(0x000E,0x2B00);    delay_ms(50);   
	    LCD_WriteReg(0x001E,0x00B0);    delay_ms(50);   
	    LCD_WriteReg(0x0001,0x2B3F);    delay_ms(50);   /* 320*240 0x2B3F */
	    LCD_WriteReg(0x0002,0x0600);    delay_ms(50);
	    LCD_WriteReg(0x0010,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0011,0x6070);    delay_ms(50);
	    LCD_WriteReg(0x0005,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0006,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0016,0xEF1C);    delay_ms(50);
	    LCD_WriteReg(0x0017,0x0003);    delay_ms(50);
	    LCD_WriteReg(0x0007,0x0133);    delay_ms(50);         
	    LCD_WriteReg(0x000B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x000F,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0041,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0042,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0048,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0049,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x004A,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x004B,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0044,0xEF00);    delay_ms(50);
	    LCD_WriteReg(0x0045,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0046,0x013F);    delay_ms(50);
	    LCD_WriteReg(0x0030,0x0707);    delay_ms(50);
	    LCD_WriteReg(0x0031,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0032,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0033,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x0034,0x0507);    delay_ms(50);
	    LCD_WriteReg(0x0035,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0036,0x0204);    delay_ms(50);
	    LCD_WriteReg(0x0037,0x0502);    delay_ms(50);
	    LCD_WriteReg(0x003A,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x003B,0x0302);    delay_ms(50);
	    LCD_WriteReg(0x0023,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0024,0x0000);    delay_ms(50);
	    LCD_WriteReg(0x0025,0x8000);    delay_ms(50);
	    LCD_WriteReg(0x004f,0);
	    LCD_WriteReg(0x004e,0);
	}
	else if( DeviceCode == 0x8999 )
	{
		DeviceCode = SSD1298;		
		LCD_WriteReg(0x0028,0x0006);
		LCD_WriteReg(0x0000,0x0001);
		LCD_WriteReg(0x0003,0xaea4);    /* power control 1---line frequency and VHG,VGL voltage */
		LCD_WriteReg(0x000c,0x0004);    /* power control 2---VCIX2 output voltage */
		LCD_WriteReg(0x000d,0x000c);    /* power control 3---Vlcd63 voltage */
		LCD_WriteReg(0x000e,0x2800);    /* power control 4---VCOMA voltage VCOML=VCOMH*0.9475-VCOMA */
		LCD_WriteReg(0x001e,0x00b5);    /* POWER CONTROL 5---VCOMH voltage */   
		LCD_WriteReg(0x0001,0x3b3f);     
		LCD_WriteReg(0x0002,0x0600);
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x6830);
		LCD_WriteReg(0x0005,0x0000);
		LCD_WriteReg(0x0006,0x0000);
		LCD_WriteReg(0x0016,0xef1c);  
		LCD_WriteReg(0x0007,0x0033);    /* Display control 1 */
		/* when GON=1 and DTE=0,all gate outputs become VGL */
		/* when GON=1 and DTE=0,all gate outputs become VGH */
		/* non-selected gate wires become VGL */     
		LCD_WriteReg(0x000b,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		LCD_WriteReg(0x0041,0x0000);
		LCD_WriteReg(0x0042,0x0000);
		LCD_WriteReg(0x0048,0x0000);
		LCD_WriteReg(0x0049,0x013f);
		LCD_WriteReg(0x004a,0x0000);
		LCD_WriteReg(0x004b,0x0000); 
		LCD_WriteReg(0x0044,0xef00);	/* Horizontal RAM start and end address */
		LCD_WriteReg(0x0045,0x0000);	/* Vretical RAM start address */
		LCD_WriteReg(0x0046,0x013f);	/* Vretical RAM end address */ 
		LCD_WriteReg(0x004e,0x0000);	/* set GDDRAM x address counter */
		LCD_WriteReg(0x004f,0x0000);    /* set GDDRAM y address counter */   
		/* y control */
		LCD_WriteReg(0x0030,0x0707);
		LCD_WriteReg(0x0031,0x0202);
		LCD_WriteReg(0x0032,0x0204);
		LCD_WriteReg(0x0033,0x0502);
		LCD_WriteReg(0x0034,0x0507);
		LCD_WriteReg(0x0035,0x0204);
		LCD_WriteReg(0x0036,0x0204);
		LCD_WriteReg(0x0037,0x0502);
		LCD_WriteReg(0x003a,0x0302);
		LCD_WriteReg(0x003b,0x0302); 
		LCD_WriteReg(0x0023,0x0000);
		LCD_WriteReg(0x0024,0x0000);
		LCD_WriteReg(0x0025,0x8000);
		LCD_WriteReg(0x0026,0x7000);
		LCD_WriteReg(0x0020,0xb0eb);
		LCD_WriteReg(0x0027,0x007c);
	}
	else if( DeviceCode == 0x5408 )
	{
		DeviceCode = SPFD5408B;
		
		LCD_WriteReg(0x0001,0x0100);	  /* Driver Output Contral Register */ 
		LCD_WriteReg(0x0002,0x0700);      /* LCD Driving Waveform Contral */
		LCD_WriteReg(0x0003,0x1030);
		
		LCD_WriteReg(0x0004,0x0000);	  /* Scalling Control register */
		LCD_WriteReg(0x0008,0x0207);	  /* Display Control 2 */
		LCD_WriteReg(0x0009,0x0000);	  /* Display Control 3 */
		LCD_WriteReg(0x000A,0x0000);	  /* Frame Cycle Control */
		LCD_WriteReg(0x000C,0x0000);	  /* External Display Interface Control 1 */
		LCD_WriteReg(0x000D,0x0000);      /* Frame Maker Position */
		LCD_WriteReg(0x000F,0x0000);	  /* External Display Interface Control 2 */
		delay_ms(50);
		LCD_WriteReg(0x0007,0x0101);	  /* Display Control */
		delay_ms(50);
		LCD_WriteReg(0x0010,0x16B0);      /* Power Control 1 */
		LCD_WriteReg(0x0011,0x0001);      /* Power Control 2 */
		LCD_WriteReg(0x0017,0x0001);      /* Power Control 3 */
		LCD_WriteReg(0x0012,0x0138);      /* Power Control 4 */
		LCD_WriteReg(0x0013,0x0800);      /* Power Control 5 */
		LCD_WriteReg(0x0029,0x0009);	  /* NVM read data 2 */
		LCD_WriteReg(0x002a,0x0009);	  /* NVM read data 3 */
		LCD_WriteReg(0x00a4,0x0000);  
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00EF);
		LCD_WriteReg(0x0052,0x0000);
		LCD_WriteReg(0x0053,0x013F);
		   
		LCD_WriteReg(0x0060,0x2700);	  /* Driver Output Control */

		LCD_WriteReg(0x0061,0x0003);	  /* Driver Output Control */
		LCD_WriteReg(0x006A,0x0000);	  /* Vertical Scroll Control */
		
		LCD_WriteReg(0x0080,0x0000);	  /* Display Position – Partial Display 1 */
		LCD_WriteReg(0x0081,0x0000);	  /* RAM Address Start – Partial Display 1 */
		LCD_WriteReg(0x0082,0x0000);	  /* RAM address End - Partial Display 1 */
		LCD_WriteReg(0x0083,0x0000);	  /* Display Position – Partial Display 2 */
		LCD_WriteReg(0x0084,0x0000);	  /* RAM Address Start – Partial Display 2 */
		LCD_WriteReg(0x0085,0x0000);	  /* RAM address End – Partail Display2 */
		LCD_WriteReg(0x0090,0x0013);	  /* Frame Cycle Control */
		LCD_WriteReg(0x0092,0x0000); 	  /* Panel Interface Control 2 */
		LCD_WriteReg(0x0093,0x0003);	  /* Panel Interface control 3 */
		LCD_WriteReg(0x0095,0x0110);	  /* Frame Cycle Control */
		LCD_WriteReg(0x0007,0x0173);
	}
	else if( DeviceCode == 0x4531 )
	{	
		DeviceCode = LGDP4531;
		/* Setup display */
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x10,0x0628);
		LCD_WriteReg(0x12,0x0006);
		LCD_WriteReg(0x13,0x0A32);
		LCD_WriteReg(0x11,0x0040);
		LCD_WriteReg(0x15,0x0050);
		LCD_WriteReg(0x12,0x0016);
		delay_ms(50);
		LCD_WriteReg(0x10,0x5660);
		delay_ms(50);
		LCD_WriteReg(0x13,0x2A4E);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0300);	
		LCD_WriteReg(0x03,0x1030);		
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x30,0x0000);
		LCD_WriteReg(0x31,0x0402);
		LCD_WriteReg(0x32,0x0106);
		LCD_WriteReg(0x33,0x0700);
		LCD_WriteReg(0x34,0x0104);
		LCD_WriteReg(0x35,0x0301);
		LCD_WriteReg(0x36,0x0707);
		LCD_WriteReg(0x37,0x0305);
		LCD_WriteReg(0x38,0x0208);
		LCD_WriteReg(0x39,0x0F0B);
		delay_ms(50);
		LCD_WriteReg(0x41,0x0002);
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x90,0x0119);
		LCD_WriteReg(0x92,0x010A);
		LCD_WriteReg(0x93,0x0004);
		LCD_WriteReg(0xA0,0x0100);
		delay_ms(50);
		LCD_WriteReg(0x07,0x0133);
		delay_ms(50);
		LCD_WriteReg(0xA0,0x0000);
	}
	else if( DeviceCode == 0x4535 )
	{	
		DeviceCode = LGDP4535;	
		LCD_WriteReg(0x15, 0x0030); 	 /* Set the internal vcore voltage */                                              
		LCD_WriteReg(0x9A, 0x0010); 	 /* Start internal OSC */
		LCD_WriteReg(0x11, 0x0020);	     /* set SS and SM bit */
		LCD_WriteReg(0x10, 0x3428);	     /* set 1 line inversion */
		LCD_WriteReg(0x12, 0x0002);	     /* set GRAM write direction and BGR=1 */ 
		LCD_WriteReg(0x13, 0x1038);	     /* Resize register */
		delay_ms(40); 
		LCD_WriteReg(0x12, 0x0012);	     /* set the back porch and front porch */
		delay_ms(40); 
		LCD_WriteReg(0x10, 0x3420);	     /* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(0x13, 0x3045);	     /* FMARK function */
		delay_ms(70); 
		LCD_WriteReg(0x30, 0x0000);      /* RGB interface setting */
		LCD_WriteReg(0x31, 0x0402);	     /* Frame marker Position */
		LCD_WriteReg(0x32, 0x0307);      /* RGB interface polarity */
		LCD_WriteReg(0x33, 0x0304);      /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x34, 0x0004);      /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x35, 0x0401);      /* VREG1OUT voltage */
		LCD_WriteReg(0x36, 0x0707);      /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x37, 0x0305);      /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(0x38, 0x0610);      /* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(0x39, 0x0610);      /* VREG1OUT voltage */
		LCD_WriteReg(0x01, 0x0100);      /* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(0x02, 0x0300);      /* VCM[4:0] for VCOMH */
		LCD_WriteReg(0x03, 0x1030);      /* GRAM horizontal Address */
		LCD_WriteReg(0x08, 0x0808);      /* GRAM Vertical Address */
		LCD_WriteReg(0x0A, 0x0008);		
		LCD_WriteReg(0x60, 0x2700);	     /* Gate Scan Line */
		LCD_WriteReg(0x61, 0x0001);	     /* NDL,VLE, REV */
		LCD_WriteReg(0x90, 0x013E);
		LCD_WriteReg(0x92, 0x0100);
		LCD_WriteReg(0x93, 0x0100);
		LCD_WriteReg(0xA0, 0x3000);
		LCD_WriteReg(0xA3, 0x0010);
		LCD_WriteReg(0x07, 0x0001);
		LCD_WriteReg(0x07, 0x0021);
		LCD_WriteReg(0x07, 0x0023);
		LCD_WriteReg(0x07, 0x0033);
		LCD_WriteReg(0x07, 0x0133);
	} 		 		
	else if( DeviceCode == 0x0047 )
	{
		DeviceCode = HX8347D;
		/* Start Initial Sequence */
		LCD_WriteReg(0xEA,0x00);                          
		LCD_WriteReg(0xEB,0x20);                                                     
		LCD_WriteReg(0xEC,0x0C);                                                   
		LCD_WriteReg(0xED,0xC4);                                                    
		LCD_WriteReg(0xE8,0x40);                                                     
		LCD_WriteReg(0xE9,0x38);                                                    
		LCD_WriteReg(0xF1,0x01);                                                    
		LCD_WriteReg(0xF2,0x10);                                                    
		LCD_WriteReg(0x27,0xA3);                                                    
		/* GAMMA SETTING */
		LCD_WriteReg(0x40,0x01);                           
		LCD_WriteReg(0x41,0x00);                                                   
		LCD_WriteReg(0x42,0x00);                                                   
		LCD_WriteReg(0x43,0x10);                                                    
		LCD_WriteReg(0x44,0x0E);                                                   
		LCD_WriteReg(0x45,0x24);                                                  
		LCD_WriteReg(0x46,0x04);                                                  
		LCD_WriteReg(0x47,0x50);                                                   
		LCD_WriteReg(0x48,0x02);                                                    
		LCD_WriteReg(0x49,0x13);                                                  
		LCD_WriteReg(0x4A,0x19);                                                  
		LCD_WriteReg(0x4B,0x19);                                                 
		LCD_WriteReg(0x4C,0x16);                                                 
		LCD_WriteReg(0x50,0x1B);                                                   
		LCD_WriteReg(0x51,0x31);                                                    
		LCD_WriteReg(0x52,0x2F);                                                     
		LCD_WriteReg(0x53,0x3F);                                                    
		LCD_WriteReg(0x54,0x3F);                                                     
		LCD_WriteReg(0x55,0x3E);                                                     
		LCD_WriteReg(0x56,0x2F);                                                   
		LCD_WriteReg(0x57,0x7B);                                                     
		LCD_WriteReg(0x58,0x09);                                                  
		LCD_WriteReg(0x59,0x06);                                                 
		LCD_WriteReg(0x5A,0x06);                                                   
		LCD_WriteReg(0x5B,0x0C);                                                   
		LCD_WriteReg(0x5C,0x1D);                                                   
		LCD_WriteReg(0x5D,0xCC);                                                   
		/* Power Voltage Setting */
		LCD_WriteReg(0x1B,0x18);                                                    
		LCD_WriteReg(0x1A,0x01);                                                    
		LCD_WriteReg(0x24,0x15);                                                    
		LCD_WriteReg(0x25,0x50);                                                    
		LCD_WriteReg(0x23,0x8B);                                                   
		LCD_WriteReg(0x18,0x36);                           
		LCD_WriteReg(0x19,0x01);                                                    
		LCD_WriteReg(0x01,0x00);                                                   
		LCD_WriteReg(0x1F,0x88);                                                    
		delay_ms(50);
		LCD_WriteReg(0x1F,0x80);                                                  
		delay_ms(50);
		LCD_WriteReg(0x1F,0x90);                                                   
		delay_ms(50);
		LCD_WriteReg(0x1F,0xD0);                                                   
		delay_ms(50);
		LCD_WriteReg(0x17,0x05);                                                    
		LCD_WriteReg(0x36,0x00);                                                    
		LCD_WriteReg(0x28,0x38);                                                 
		delay_ms(50);
		LCD_WriteReg(0x28,0x3C);                                                
	}
	else if( DeviceCode == 0x7783 )
	{
		DeviceCode = ST7781;
		/* Start Initial Sequence */
		LCD_WriteReg(0x00FF,0x0001);
		LCD_WriteReg(0x00F3,0x0008);
		LCD_WriteReg(0x0001,0x0100);
		LCD_WriteReg(0x0002,0x0700);
		LCD_WriteReg(0x0003,0x1030);  
		LCD_WriteReg(0x0008,0x0302);
		LCD_WriteReg(0x0008,0x0207);
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000A,0x0000);
		LCD_WriteReg(0x0010,0x0000);  
		LCD_WriteReg(0x0011,0x0005);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		delay_ms(50);
		LCD_WriteReg(0x0010,0x12B0);
		delay_ms(50);
		LCD_WriteReg(0x0011,0x0007);
		delay_ms(50);
		LCD_WriteReg(0x0012,0x008B);
		delay_ms(50);	
		LCD_WriteReg(0x0013,0x1700);
		delay_ms(50);	
		LCD_WriteReg(0x0029,0x0022);		
		LCD_WriteReg(0x0030,0x0000);
		LCD_WriteReg(0x0031,0x0707);
		LCD_WriteReg(0x0032,0x0505);
		LCD_WriteReg(0x0035,0x0107);
		LCD_WriteReg(0x0036,0x0008);
		LCD_WriteReg(0x0037,0x0000);
		LCD_WriteReg(0x0038,0x0202);
		LCD_WriteReg(0x0039,0x0106);
		LCD_WriteReg(0x003C,0x0202);
		LCD_WriteReg(0x003D,0x0408);
		delay_ms(50);				
		LCD_WriteReg(0x0050,0x0000);		
		LCD_WriteReg(0x0051,0x00EF);		
		LCD_WriteReg(0x0052,0x0000);		
		LCD_WriteReg(0x0053,0x013F);		
		LCD_WriteReg(0x0060,0xA700);		
		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x0090,0x0033);				
		LCD_WriteReg(0x002B,0x000B);		
		LCD_WriteReg(0x0007,0x0133);
	}
	else	/* special ID */
	{
		DeviceCode = 0x0123;
		DeviceCode = LCD_ReadReg(0x67);
		if( DeviceCode == 0x0047 )
		{
	        DeviceCode = HX8347A;
	        LCD_WriteReg(0x0042,0x0008);   
	        /* Gamma setting */  
		    LCD_WriteReg(0x0046,0x00B4); 
		    LCD_WriteReg(0x0047,0x0043); 
		    LCD_WriteReg(0x0048,0x0013); 
		    LCD_WriteReg(0x0049,0x0047); 
		    LCD_WriteReg(0x004A,0x0014); 
		    LCD_WriteReg(0x004B,0x0036); 
		    LCD_WriteReg(0x004C,0x0003); 
		    LCD_WriteReg(0x004D,0x0046); 
		    LCD_WriteReg(0x004E,0x0005);  
		    LCD_WriteReg(0x004F,0x0010);  
		    LCD_WriteReg(0x0050,0x0008);  
		    LCD_WriteReg(0x0051,0x000a);  
		    /* Window Setting */
		    LCD_WriteReg(0x0002,0x0000); 
		    LCD_WriteReg(0x0003,0x0000); 
		    LCD_WriteReg(0x0004,0x0000); 
		    LCD_WriteReg(0x0005,0x00EF); 
		    LCD_WriteReg(0x0006,0x0000); 
		    LCD_WriteReg(0x0007,0x0000); 
		    LCD_WriteReg(0x0008,0x0001); 
		    LCD_WriteReg(0x0009,0x003F); 
		    delay_ms(10); 
		    LCD_WriteReg(0x0001,0x0006); 
		    LCD_WriteReg(0x0016,0x00C8);   
		    LCD_WriteReg(0x0023,0x0095); 
		    LCD_WriteReg(0x0024,0x0095); 
		    LCD_WriteReg(0x0025,0x00FF); 
		    LCD_WriteReg(0x0027,0x0002); 
		    LCD_WriteReg(0x0028,0x0002); 
		    LCD_WriteReg(0x0029,0x0002); 
		    LCD_WriteReg(0x002A,0x0002); 
		    LCD_WriteReg(0x002C,0x0002); 
		    LCD_WriteReg(0x002D,0x0002); 
		    LCD_WriteReg(0x003A,0x0001);  
		    LCD_WriteReg(0x003B,0x0001);  
		    LCD_WriteReg(0x003C,0x00F0);    
		    LCD_WriteReg(0x003D,0x0000); 
		    delay_ms(20); 
		    LCD_WriteReg(0x0035,0x0038); 
		    LCD_WriteReg(0x0036,0x0078); 
		    LCD_WriteReg(0x003E,0x0038); 
		    LCD_WriteReg(0x0040,0x000F); 
		    LCD_WriteReg(0x0041,0x00F0);  
		    LCD_WriteReg(0x0038,0x0000); 
		    /* Power Setting */ 
		    LCD_WriteReg(0x0019,0x0049);  
		    LCD_WriteReg(0x0093,0x000A); 
		    delay_ms(10); 
		    LCD_WriteReg(0x0020,0x0020);   
		    LCD_WriteReg(0x001D,0x0003);   
		    LCD_WriteReg(0x001E,0x0000);  
		    LCD_WriteReg(0x001F,0x0009);   
		    LCD_WriteReg(0x0044,0x0053);  
		    LCD_WriteReg(0x0045,0x0010);   
		    delay_ms(10);  
		    LCD_WriteReg(0x001C,0x0004);  
		    delay_ms(20); 
		    LCD_WriteReg(0x0043,0x0080);    
		    delay_ms(5); 
		    LCD_WriteReg(0x001B,0x000a);    
		    delay_ms(40);  
		    LCD_WriteReg(0x001B,0x0012);    
		    delay_ms(40);   
		    /* Display On Setting */ 
		    LCD_WriteReg(0x0090,0x007F); 
		    LCD_WriteReg(0x0026,0x0004); 
		    delay_ms(40);  
		    LCD_WriteReg(0x0026,0x0024); 
		    LCD_WriteReg(0x0026,0x002C); 
		    delay_ms(40);   
		    LCD_WriteReg(0x0070,0x0008); 
		    LCD_WriteReg(0x0026,0x003C);  
		    LCD_WriteReg(0x0057,0x0002); 
		    LCD_WriteReg(0x0055,0x0000); 
		    LCD_WriteReg(0x0057,0x0000); 
		}	 
	}  						
    delay_ms(50);   /* delay 50 ms */		
  LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  _BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}


/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
  uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;
  /* Send the string character by character on lCD */
  while (((refcolumn + 1)& 0xFFFF) >= LCD_Currentfonts->Width)
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, ' ');
    /* Decrement the column position by 16 */
    refcolumn -= LCD_Currentfonts->Width;
  }
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
  uint32_t index=0;
	
	if( DeviceCode == HX8347D || DeviceCode == HX8347A )
	{
		LCD_WriteReg(0x02,0x00);                                                  
		LCD_WriteReg(0x03,0x00);  
		                
		LCD_WriteReg(0x04,0x00);                           
		LCD_WriteReg(0x05,0xEF);  
		                 
		LCD_WriteReg(0x06,0x00);                           
		LCD_WriteReg(0x07,0x00);    
		               
		LCD_WriteReg(0x08,0x01);                           
		LCD_WriteReg(0x09,0x3F);     
	}
	else
	{	
		LCD_SetCursor(0,0); 
	}	

	LCD_WriteRAM_Prepare();
	for( index = 0; index < MAX_X * MAX_Y; index++ )
	{
		LCD_WriteRAM(Color);
	}  
}

/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
	
 	uint16_t temp = Xpos;

			 Xpos = Ypos;
			 Ypos = ( MAX_X - 1 ) - temp;  

	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
		
	#endif

  switch( DeviceCode )
  {
     default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Xpos );     
          LCD_WriteReg(0x0021, Ypos );     
	      break; 

     case SSD1298: 	 /* 0x8999 */
     case SSD1289:   /* 0x8989 */
	      LCD_WriteReg(0x004e, Xpos );      
        LCD_WriteReg(0x004f, Ypos );          
	      break;  

     case HX8347A: 	 /* 0x0047 */
     case HX8347D: 	 /* 0x0047 */
	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
	      LCD_WriteReg(0x03, Xpos );  

	      LCD_WriteReg(0x06, Ypos>>8 );                           
	      LCD_WriteReg(0x07, Ypos );    
	
	      break;     
     case SSD2119:	 /* 3.5 LCD 0x9919 */
	      break;  
  }
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, i = 0;
  uint16_t  Xaddress = 0;
  Xaddress = Xpos;
  
  LCD_SetCursor(Xaddress, Ypos);
  
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < LCD_Currentfonts->Width; i++)
    {
  
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> i)) == 0x00) &&(LCD_Currentfonts->Width <= 12))||
        (((c[index] & (0x1 << i)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))

      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      } 
    }
    Xaddress++;
    LCD_SetCursor(Xaddress, Ypos);
  }
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{
  uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by 16 */
    refcolumn -= LCD_Currentfonts->Width;
    /* Point on the next character */
    ptr++;
  }
}

/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X bottom left position.
  * @param  Ypos: specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width: display window width.
  * @retval None
  */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  /* Horizontal GRAM Start Address */
  if(Xpos >= Height)
  {
    LCD_WriteReg(LCD_REG_80, (Xpos - Height + 1));
  }
  else
  {
    LCD_WriteReg(LCD_REG_80, 0);
  }
  /* Horizontal GRAM End Address */
  LCD_WriteReg(LCD_REG_81, Xpos);
  /* Vertical GRAM Start Address */
  if(Ypos >= Width)
  {
    LCD_WriteReg(LCD_REG_82, (Ypos - Width + 1));
  }  
  else
  {
    LCD_WriteReg(LCD_REG_82, 0);
  }
  /* Vertical GRAM End Address */
  LCD_WriteReg(LCD_REG_83, Ypos);
  LCD_SetCursor(Xpos, Ypos);
}

/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  LCD_WriteReg(LCD_REG_3, 0x1018);    
}

/**
  * @brief  Displays a line.
  * @param Xpos: specifies the X position.
  * @param Ypos: specifies the Y position.
  * @param Length: line length.
  * @param Direction: line direction.
  *   This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction)
{
  uint32_t i = 0;
  
  LCD_SetCursor(Xpos, Ypos);
  if(Direction == LCD_DIR_HORIZONTAL)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM(TextColor);
    }
  }
  else
  {
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
      LCD_WriteRAM(TextColor);
      Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);
  
  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;/* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    LCD_WriteRAM(TextColor);
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/**
  * @brief  Displays a mono-color picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t *Pict)
{
  uint32_t index = 0, i = 0;
  LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1)); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void LCD_WriteBMP(uint32_t BmpAddress)
{
  uint32_t index = 0, size = 0;
  /* Read bitmap size */
  size = *(__IO uint16_t *) (BmpAddress + 2);
  size |= (*(__IO uint16_t *) (BmpAddress + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (BmpAddress + 10);
  index |= (*(__IO uint16_t *) (BmpAddress + 12)) << 16;
  size = (size - index)/2;
  BmpAddress += index;
  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(LCD_REG_3, 0x1008);
 
  LCD_WriteRAM_Prepare();
 
  for(index = 0; index < size; index++)
  {
    LCD_WriteRAM(*(__IO uint16_t *)BmpAddress);
    BmpAddress += 2;
  }
 
  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  LCD_WriteReg(LCD_REG_3, 0x1018);
}

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  LCD_SetTextColor(TextColor);

  LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
  LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);
  
  LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);

  Width -= 2;
  Height--;
  Ypos--;

  LCD_SetTextColor(BackColor);

  while(Height--)
  {
    LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);    
  }

  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);

  CurX = 0;
  CurY = Radius;
  
  LCD_SetTextColor(BackColor);

  while (CurX <= CurY)
  {
    if(CurY > 0) 
    {
      LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos + CurX, Ypos + CurY, 2*CurY, LCD_DIR_HORIZONTAL);
    }

    if(CurX > 0) 
    {
      LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos + CurY, Ypos + CurX, 2*CurX, LCD_DIR_HORIZONTAL);
    }
    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }

  LCD_SetTextColor(TextColor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  Displays an uni-line (between two points).
  * @param  x1: specifies the point 1 x position.
  * @param  y1: specifies the point 1 y position.
  * @param  x2: specifies the point 2 x position.
  * @param  y2: specifies the point 2 y position.
  * @retval None
  */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* The difference between the x's */
  deltay = ABS(y2 - y1);        /* The difference between the y's */
  x = x1;                       /* Start x off at the first pixel */
  y = y1;                       /* Start y off at the first pixel */
  
  if (x2 >= x1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    PutPixel(x, y);             /* Draw the current pixel */
    num += numadd;              /* Increase the numerator by the top of the fraction */
    if (num >= den)             /* Check if numerator >= denominator */
    {
      num -= den;               /* Calculate the new numerator value */
      x += xinc1;               /* Change the x as appropriate */
      y += yinc1;               /* Change the y as appropriate */
    }
    x += xinc2;                 /* Change the x as appropriate */
    y += yinc2;                 /* Change the y as appropriate */
  }
}

/**
  * @brief  Displays an poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLine(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0;

  if(PointCount < 2)
  {
    return;
  }

  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    LCD_DrawUniLine(X, Y, Points->X, Points->Y);
  }
}

/**
  * @brief  Displays an relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @param  Closed: specifies if the draw is closed or not.
  *           1: closed, 0 : not closed.
  * @retval None
  */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount, uint16_t Closed)
{
  int16_t X = 0, Y = 0;
  pPoint First = Points;

  if(PointCount < 2)
  {
    return;
  }  
  X = Points->X;
  Y = Points->Y;
  while(--PointCount)
  {
    Points++;
    LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
    X = X + Points->X;
    Y = Y + Points->Y;
  }
  if(Closed)
  {
    LCD_DrawUniLine(First->X, First->Y, X, Y);
  }  
}

/**
  * @brief  Displays a closed poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLine(Points, PointCount);
  LCD_DrawUniLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
}

/**
  * @brief  Displays a relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
  * @brief  Displays a closed relative poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount)
{
  LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}


/**
  * @brief  Displays a  full poly-line (between many points).
  * @param  Points: pointer to the points array.
  * @param  PointCount: Number of points.
  * @retval None
  */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount)
{
  /*  public-domain code by Darel Rex Finley, 2007 */
  uint16_t  nodes = 0, nodeX[MAX_POLY_CORNERS], pixelX = 0, pixelY = 0, i = 0,
  j = 0, swap = 0;
  uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

  IMAGE_LEFT = IMAGE_RIGHT = Points->X;
  IMAGE_TOP= IMAGE_BOTTOM = Points->Y;

  for(i = 1; i < PointCount; i++)
  {
    pixelX = POLY_X(i);
    if(pixelX < IMAGE_LEFT)
    {
      IMAGE_LEFT = pixelX;
    }
    if(pixelX > IMAGE_RIGHT)
    {
      IMAGE_RIGHT = pixelX;
    }
    
    pixelY = POLY_Y(i);
    if(pixelY < IMAGE_TOP)
    { 
      IMAGE_TOP = pixelY;
    }
    if(pixelY > IMAGE_BOTTOM)
    {
      IMAGE_BOTTOM = pixelY;
    }
  }
  
  LCD_SetTextColor(BackColor);  

  /*  Loop through the rows of the image. */
  for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOTTOM; pixelY++) 
  {  
    /* Build a list of nodes. */
    nodes = 0; j = PointCount-1;

    for (i = 0; i < PointCount; i++) 
    {
      if (((POLY_Y(i)<(double) pixelY) && (POLY_Y(j)>=(double) pixelY)) || \
          ((POLY_Y(j)<(double) pixelY) && (POLY_Y(i)>=(double) pixelY)))
      {
        nodeX[nodes++]=(int) (POLY_X(i)+((pixelY-POLY_Y(i))*(POLY_X(j)-POLY_X(i)))/(POLY_Y(j)-POLY_Y(i))); 
      }
      j = i; 
    }
  
    /* Sort the nodes, via a simple "Bubble" sort. */
    i = 0;
    while (i < nodes-1) 
    {
      if (nodeX[i]>nodeX[i+1]) 
      {
        swap = nodeX[i]; 
        nodeX[i] = nodeX[i+1]; 
        nodeX[i+1] = swap; 
        if(i)
        {
          i--; 
        }
      }
      else 
      {
        i++;
      }
    }
  
    /*  Fill the pixels between node pairs. */
    for (i = 0; i < nodes; i+=2) 
    {
      if(nodeX[i] >= IMAGE_RIGHT) 
      {
        break;
      }
      if(nodeX[i+1] > IMAGE_LEFT) 
      {
        if (nodeX[i] < IMAGE_LEFT)
        {
          nodeX[i]=IMAGE_LEFT;
        }
        if(nodeX[i+1] > IMAGE_RIGHT)
        {
          nodeX[i+1] = IMAGE_RIGHT;
        }
        LCD_SetTextColor(BackColor);
        LCD_DrawLine(pixelY, nodeX[i+1], nodeX[i+1] - nodeX[i], LCD_DIR_HORIZONTAL);
        LCD_SetTextColor(TextColor);
        PutPixel(pixelY, nodeX[i+1]);
        PutPixel(pixelY, nodeX[i]);
        /* for (j=nodeX[i]; j<nodeX[i+1]; j++) PutPixel(j,pixelY); */
      }
    }
  } 

  /* draw the edges */
  LCD_SetTextColor(TextColor);
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @param  LCD_RegValue: value to write to the selected register.
  * @retval None
  */
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD_RAM = LCD_RegValue;
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD_RAM);
}

/**
  * @brief  Prepare to write to the LCD RAM.
  * @param  None
  * @retval None
  */
void LCD_WriteRAM_Prepare(void)
{
  LCD_REG = 0x0022;
}

/**
  * @brief  Writes to the LCD RAM.
  * @param  RGB_Code: the pixel color in RGB mode (5-6-5).
  * @retval None
  */
void LCD_WriteRAM(uint16_t RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD_RAM = RGB_Code;
}

/**
  * @brief  Reads the LCD RAM.
  * @param  None
  * @retval LCD RAM Value.
  */
uint16_t LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  //LCD_REG = LCD_REG_34; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  return LCD_RAM;
}

/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void LCD_PowerOn(void)
{
/* Power On sequence ---------------------------------------------------------*/
  LCD_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
  LCD_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
  LCD_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude*/
  delay_ms(20);                 /* Dis-charge capacitor power voltage (200ms) */
  LCD_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
  LCD_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
  delay_ms(5);                  /* Delay 50 ms */
  LCD_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
  delay_ms(5);                  /* Delay 50 ms */
  LCD_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
  LCD_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */
  delay_ms(5);                  /* Delay 50 ms */
  LCD_WriteReg(LCD_REG_7, 0x0173);  /* 262K color and display ON */
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOn(void)
{
  /* Display On */
  LCD_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void LCD_DisplayOff(void)
{
  /* Display Off */
  LCD_WriteReg(LCD_REG_7, 0x0); 
}

/**
  * @brief  Configures LCD Control lines (FSMC Pins) in alternate function mode.
  * @param  None
  * @retval None
  */
void LCD_CtrlLinesConfig(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);
	
	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 

	/*-- GPIOs Configuration ------------------------------------------------------*/
	/*
	+-------------------+--------------------+------------------+------------------+
	+                       SRAM pins assignment                                   +

	+-------------------+--------------------+
	*/
	/* GPIOD configuration */
		/* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
	 PD.10(D15), PD.11(A16), PD.14(D0), PD.15(D1) as alternate function push pull */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | 
	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
								 GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
	 PE.14(D11), PE.15(D12) as alternate function push pull */	
	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
	                             GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**
  * @brief  Configures the Parallel interface (FSMC) for LCD(Parallel mode)
  * @param  None
  * @retval None
  */
void LCD_FSMCConfig(void)
{
  	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
//	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 


	/* FSMC写速度设置 */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 15;//1;   /* 地址建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 15;//1;	   /* 数据建立时间  */
	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC 访问模式 */
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	
	/* Enable FSMC Bank4_SRAM Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

/**
  * @brief  Displays a pixel.
  * @param  x: pixel x.
  * @param  y: pixel y.  
  * @retval None
  */
static void PutPixel(int16_t x, int16_t y)
{ 
  if(x < 0 || x > 239 || y < 0 || y > 319)
  {
    return;  
  }
  LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
}


#ifndef USE_Delay
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void delay_ms(__IO uint32_t nCount)
{
  	uint16_t i,j; 
	for( i = 0; i < nCount; i++ )
	{ 
		for( j = 0; j < 1141; j++ );
	}
}
#endif /* USE_Delay*/
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
