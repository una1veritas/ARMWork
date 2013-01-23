/*********************************************************************************************************
*
* File                : TouchPanel.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "armcore.h"
#include "spi.h"

/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT 
{
   uint16_t x;
   uint16_t y;
} Coordinate;


typedef struct Matrix 
{						
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
} Matrix ;

typedef struct __TouchPadStruct {
	SPIPort * port;
	GPIOPin pin_cs, pin_req;
 Coordinate ScreenSample[3];
 Coordinate DisplaySample[3];
 Matrix matrix ;
 Coordinate  display ;
} TouchPadStruct;

/* Private variables ---------------------------------------------------------*/

static TouchPadStruct TPStruct;

/* Private define ------------------------------------------------------------*/

#define	CHX 	0x90
#define	CHY 	0xd0


void TP_CS(boolean x); // 	((x) ? GPIO_SetBits(GPIOC,GPIO_Pin_4) : GPIO_ResetBits(GPIOC,GPIO_Pin_4))
boolean TP_REQ(void); //#define TP_INT_IN   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)

/* Private function prototypes -----------------------------------------------*/				
void TP_Init(SPIPort * port, GPIOPin sck, GPIOPin si, GPIOPin so, GPIOPin cs, GPIOPin req);	
Coordinate *Read_Ads7846(void);
void TouchPanel_Calibrate(void);
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos);
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr);
FunctionalState getDisplayPoint(Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr );

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


