#include "stm32f4xx_it.h"
#include "ads7846.h"
//#include "SSD1289.h"

#include "spi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//Pen_Holder Pen_Point;
//unsigned char flag=0;

uint16 TouchScreen::readX(void) {
	uint16 x = 0;
	select();
	delay_us(1);
	spi_transfer(&bus, CMD_RDX);
	delay_us(1);
	x = spi_transfer(&bus, 0);
	x <<= 8;
	x |= spi_transfer(&bus, 0);
	deselect();
	x >>= 4;
	return x;
}

uint16 TouchScreen::readY(void) {
	uint16 y = 0;
	select();
	delay_us(1);
	spi_transfer(&bus, CMD_RDY);
	delay_us(1);
	y = spi_transfer(&bus, 0);
	y <<= 8;
	y |= spi_transfer(&bus, 0);
	deselect();
	y >>= 4;
	return y;
}

void NVIC_TOUCHConfiguration(void) {
	/*
	 NVIC_InitTypeDef NVIC_InitStructure;
	 
	 #ifdef  VECT_TAB_RAM  									   
	 NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	 #else   							 
	 NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	 #endif  
	 
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
	 //	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;  //ﾊｹﾓﾃﾍ箚ｿﾖﾐｶﾏ10~15
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	 NVIC_Init(&NVIC_InitStructure); 	*/
}

void TouchScreen::init(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

// void GPIOMode(GPIO_TypeDef * port, uint16_t pinbit, GPIOMode_TypeDef mode,
//              GPIOSpeed_TypeDef clk, GPIOOType_TypeDef otype, GPIOPuPd_TypeDef pupd) {

	GPIOMode(PinPort(ncspin), PinBit(ncspin), OUTPUT, MEDSPEED, PUSHPULL, PULLUP);
	deselect(); 
	GPIOMode(PinPort(irqpin), PinBit(irqpin), INPUT, FASTSPEED, PUSHPULL, PULLUP);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TouchScreen::begin(uint16 w, uint16 h, uint8 d) {
	init();
  shortside = min(w, h);
  longside = max(h, w);
  setOrientation(d);
}

void TouchScreen::setOrientation(uint8 d) {
  mode = d;
}


void TouchScreen::getRawXY(void) {
	select();
	delay_us(1);
	spi_transfer(&bus, CMD_RDX);
	delay_us(1);
	Xraw = spi_transfer(&bus, 0);
	Xraw <<= 8;
	Xraw |= spi_transfer(&bus, 0);

	spi_transfer(&bus, CMD_RDY);
	delay_us(1);
	Yraw = spi_transfer(&bus, 0);
	Yraw <<= 8;
	Yraw |= spi_transfer(&bus, 0);
	deselect();
	Xraw >>= 4;
	Yraw >>= 4;
}

uint8 TouchScreen::readADC(void) {
	uint8 count;
//	uint16 rx[10], ry[10];
	uint16 tx, ty, ptx, pty;

	getRawXY();
	tx /*= rx[0]*/ = Xraw;
	ty /*= ry[0] */= Yraw;
	count = 1;
	while (count < 10) {
		getRawXY();
//		rx[count] = Xraw;
//		ry[count] = Yraw;
		count++;
		ptx = tx + Xraw;
		pty = ty + Yraw;
		ptx >>= 1;
		pty >>= 1;
		if ( abs(tx - ptx) <= 1 and abs(ty - pty) <= 1 ) {
			tx = ptx;
			ty = pty;
			break;
		}
		tx = ptx;
		ty = pty;
	}
	Xraw = tx;
	Yraw = ty;
	return 0;
}

void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {

		EXTI_ClearITPendingBit (EXTI_Line6);
		// Read_Ads7846(); 
	}
}

void TouchScreen::update(void) {
  int16 tx, ty;
  // the origin 0, 0 is at the opposite corner to the 4-wire connector
  // shorter direction is Y
	readADC();
	tx = (int) ((Xraw - xoffset) / xfact);
	ty = (int) ((Yraw - yoffset) / yfact);

	if (tx >= longside) {
		tx = longside - 1;
	} else if ( tx < 0 ) {
		tx = 0;
	}
	if (ty >= shortside) {
		ty = shortside - 1;
	} else if ( ty < 0 ) {
		ty = 0;
	}

  if ( mode == PORTRAIT ) {
    x = ty;
    y = tx;
  } else if ( mode == PORTRAIT_REV ) {
    x = shortside -ty;
    y = longside - tx;
  } else if ( mode == LANDSCAPE_REV ) {
    x = tx;
    y = shortside - ty;
  } else {
    x = longside - tx;
    y = ty;
  }
}

/*
 void TouchScreen::adjust(void)
 {								 
 uint16_t pos_temp[4][2];
 uint8_t  cnt=0;	
 uint16_t d1,d2;
 uint32_t tem1,tem2;
 float fac; 	   
 cnt=0;				
 LCD_SetTextColor(BLUE);
 LCD_SetTextColor(WHITE);
 LCD_Clear(WHITE);
 Drow_Touch_Point(20,20);
 xfac=0;
 while(1)
 {
 if(1==1)
 //if(Pen_Point.Key_Sta==Key_Down)
 {
 if(Read_Ads7846())
 {  								   
 pos_temp[cnt][0]=Pen_Point.X;
 pos_temp[cnt][1]=Pen_Point.Y;
 cnt++;
 Delay(0xFFFF);
 }			 
 switch(cnt)
 {			   
 case 1:
 LCD_Clear(WHITE);
 Drow_Touch_Point(210,20);
 break;
 case 2:
 LCD_Clear(WHITE);
 Drow_Touch_Point(20,300);
 break;
 case 3:
 LCD_Clear(WHITE);
 Drow_Touch_Point(210,300);
 break;
 case 4:
 tem1=abs(pos_temp[0][0]-pos_temp[1][0]);
 tem2=abs(pos_temp[0][1]-pos_temp[1][1]);
 tem1*=tem1;
 tem2*=tem2;
 d1=sqrt(tem1+tem2);
 
 tem1=abs(pos_temp[2][0]-pos_temp[3][0]);
 tem2=abs(pos_temp[2][1]-pos_temp[3][1]);
 tem1*=tem1;
 tem2*=tem2;
 d2=sqrt(tem1+tem2);
 fac=(float)d1/d2;
 if(fac<0.75f||fac>1.25f||d1==0||d2==0)
 {
 cnt=0;
 LCD_Clear(WHITE);
 Drow_Touch_Point(20,20);
 continue;
 }
 tem1=abs(pos_temp[0][0]-pos_temp[2][0]);
 tem2=abs(pos_temp[0][1]-pos_temp[2][1]);
 tem1*=tem1;
 tem2*=tem2;
 d1=sqrt(tem1+tem2);
 
 tem1=abs(pos_temp[1][0]-pos_temp[3][0]);
 tem2=abs(pos_temp[1][1]-pos_temp[3][1]);
 tem1*=tem1;
 tem2*=tem2;
 d2=sqrt(tem1+tem2);
 fac=(float)d1/d2;
 if(fac<0.75f||fac>1.25f)
 {
 cnt=0;
 LCD_Clear(WHITE);
 Drow_Touch_Point(20,20);
 continue;
 }
 tem1=abs(pos_temp[1][0]-pos_temp[2][0]);
 tem2=abs(pos_temp[1][1]-pos_temp[2][1]);
 tem1*=tem1;
 tem2*=tem2;
 d1=sqrt(tem1+tem2);
 
 tem1=abs(pos_temp[0][0]-pos_temp[3][0]);
 tem2=abs(pos_temp[0][1]-pos_temp[3][1]);
 tem1*=tem1;
 tem2*=tem2;
 d2=sqrt(tem1+tem2);
 fac=(float)d1/d2;
 if(fac<0.75f||fac>1.25f)
 {
 cnt=0;
 LCD_Clear(WHITE);
 Drow_Touch_Point(20,20);
 continue;
 }
 Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);
 Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;

 Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);
 Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;
 LCD_Clear(WHITE);
 return ;


 }
 }
 }
 }
 */
