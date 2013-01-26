/*
 * main.c
 *
 *  Created on: 2012/10/08
 *      Author: sin
 */
/*
http://www.element14.com/community/groups/development-tools/blog/2012/05/26/stm32f4-discovery-hy32d-tft-lcd

LCD pin name
 STM32F4 Pin number
 STM32F4 pin name
 
D0 61 PD14
D1 62 PD15

D2 81 PD0
D3 82 PD1

D4 38 PE7
D5 39 PE8
D6 40 PE9
D7 41 PE10
D8 42 PE11
D9 43 PE12
D10 44 PE13
D11 45 PE14
D12 46 PE15
D13 55 PD8
D14 56 PD9
D15 57 PD10
CS 88 PD7   (NE1)
RS 60 PD11 (FSMC-A16)
RD 85 PD4   (NOE)
WR 86 PD5   (NWE)
RESET  +3V(1K Pull up) or GPIO

TP_IRQ 		PD6 user defined interrupt pin
TP_SCK 52 PB13  SPI2SCK
TP_SI 54 PB15   SPI2MOSI
TP_SO 53 PB14   SPI2MISO
TP_CS 51 PB12 	SPI2NSS
 
BLVDD 3V3
BLVSS GND
BL_CNTL 69 PA10  user defined pwm pin
 
*/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include <stm32f4xx.h>

#include "stm32f4xx_it.h"

#define DEFAULTUSART USART6
#include "armcore.h"
#include "spi.h"

#include "TouchPanel/XPT2046.h"
#include "GLCD/SSD1289.h"
//#include "touch_7846.h"

SPIPort spi2;
char tmp[128];

int main(void) {
	int x, y;
	uint16 hi;
	RCC_ClocksTypeDef RCC_Clocks;
	
	armcore_init();
	
	usart_init(&stdserial, USART6, PC7, PC6, 57600);
	
	usart_print(&stdserial, "Hello, baby.\n");
	usart_flush(&stdserial);

	RCC_GetClocksFreq(&RCC_Clocks);

	sprintf(tmp, "SYSCLK = %ld, HCLK = %ld, PCLK1 = %ld, PCLK2 = %ld\n\n", 
							RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	usart_print(&stdserial, tmp);
	usart_print(&stdserial, "Now we're going to test XPT2046 Resistive membrane touch panel sensor.\n");
	usart_flush(&stdserial);
	
  delay(1000);
  LCD_Init();
  delay(1000);
	TP_Init(&spi2, PB13, PB15, PB14, PB12, PD6);
  LCD_Clear(DGRAY);
	
	/*
  TouchPanel_Calibrate();
	
  LCD_SetTextColor(WHITE);
  LCD_SetBackColor(BLACK);
//	LCD_SetCursor(10, 10);
	LCD_StringLine(10, 10, "This is a pen.");
  delay(1000);
*/	
	while (1) {
		TP_select(); 
		delay_us(1);
		spi_transfer(TPStruct.port, CHX);
		hi = spi_transfer(TPStruct.port,0);
		x = (hi<<4) | spi_transfer(TPStruct.port,0);
		delay_us(1); 
		spi_transfer(TPStruct.port,CHY);
		hi = spi_transfer(TPStruct.port,0);
		y = (hi<<4) | spi_transfer(TPStruct.port,0);
		TP_deselect();
		//
		sprintf(tmp, "x = %d, y = %d\n", x, y);
		usart_print(&stdserial, tmp);
		delay(1000);
	}
}

void TouchPanel_Calibrate(void)
{
  uint8_t i;
  Coordinate * Ptr;

  for(i=0;i<3;i++)
  {
   LCD_Clear(BLACK);
   //GUI_Text(44,10,"Touch crosshair to calibrate",0xffff,BLACK);
   usart_print(&stdserial, "Touch crosshair to calibrate");
   delay_ms(500);
   //DrawCross(TPStruct.DisplaySample[i].x, TPStruct.DisplaySample[i].y);
		sprintf(tmp, "x = %d, y = %d\n", TPStruct.DisplaySample[i].x, TPStruct.DisplaySample[i].y);
   usart_print(&stdserial, tmp);
   do
   {
     Ptr=Read_Ads7846();
   }
   while( Ptr == (void*)0 );
   TPStruct.ScreenSample[i].x= Ptr->x; 
	 TPStruct.ScreenSample[i].y= Ptr->y;
		sprintf(tmp, "Ptr->x = %d, Ptr->y = %d\n", Ptr->x, Ptr->y);
   usart_print(&stdserial, tmp);
  }
  setCalibrationMatrix( &TPStruct.DisplaySample[0],&TPStruct.ScreenSample[0],&TPStruct.matrix );
  LCD_Clear(BLACK);
}

/*******************************************************************************
* Function Name  : TP_DrawPoint
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
{
  LCD_SetPoint(Xpos,Ypos,BLUE);     // Center point /
  LCD_SetPoint(Xpos+1,Ypos,BLUE);
  LCD_SetPoint(Xpos,Ypos+1,BLUE);
  LCD_SetPoint(Xpos+1,Ypos+1,BLUE);	
}	
*/
/*******************************************************************************
* Function Name  : DrawCross
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
  LCD_DrawLine(Xpos-15,Ypos,Xpos-2,Ypos,0xffff);
  LCD_DrawLine(Xpos+2,Ypos,Xpos+15,Ypos,0xffff);
  LCD_DrawLine(Xpos,Ypos-15,Xpos,Ypos-2,0xffff);
  LCD_DrawLine(Xpos,Ypos+2,Xpos,Ypos+15,0xffff);
  
  LCD_DrawLine(Xpos-15,Ypos+15,Xpos-7,Ypos+15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos-15,Ypos+7,Xpos-15,Ypos+15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos-15,Ypos-15,Xpos-7,Ypos-15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos-15,Ypos-7,Xpos-15,Ypos-15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos+7,Ypos+15,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos+15,Ypos+7,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));

  LCD_DrawLine(Xpos+7,Ypos-15,Xpos+15,Ypos-15,RGB565CONVERT(184,158,131));
  LCD_DrawLine(Xpos+15,Ypos-15,Xpos+15,Ypos-7,RGB565CONVERT(184,158,131));	  	
}	
*/
