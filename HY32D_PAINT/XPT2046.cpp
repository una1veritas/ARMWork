#include "stm32f4xx_it.h"
#include "XPT2046.h"
//#include "SSD1289.h"

#include "spi.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//Pen_Holder Pen_Point;

//unsigned char flag=0;

uint8 TouchScreen::writeByte(uint8 val) {
	return spi_transfer(&bus, val);
}

uint16 TouchScreen::readX(void) {
	uint16 x = 0;
	select();
	//SpiDelay(10);
	writeByte(0x90);
	//SpiDelay(10);  
	x = writeByte(0xFF);
	x <<= 8;
	x += writeByte(0x0);
	deselect();
	//SpiDelay(10);
	x >>= 4;
	//x = x&0xFFF; //fff
	return x;
}

u16 TouchScreen::readY(void) {
	uint16 y = 0;
	select();
//  SpiDelay(10);
	writeByte(0xd0);
	//SpiDelay(10);	 
	y = writeByte(0x0);
	y <<= 8;
	y += writeByte(0x0);
	deselect();
	y >>= 4;
	//y = y & 0xFFF;	 //fff
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
//  SPI_InitTypeDef  SPI_InitStructure;
//  GPIO_InitTypeDef GPIO_InitStruct;  
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

// void GPIOMode(GPIO_TypeDef * port, uint16_t pinbit, GPIOMode_TypeDef mode,
//              GPIOSpeed_TypeDef clk, GPIOOType_TypeDef otype, GPIOPuPd_TypeDef pupd) {

	GPIOMode(PinPort(ncspin), PinBit(ncspin), OUTPUT, MEDSPEED, PUSHPULL,
			PULLUP);

	/*
	 RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);
	 
	 GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	 GPIO_InitStruct.GPIO_Speed=GPIO_Speed_25MHz;
	 GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  

	 GPIO_InitStruct.GPIO_Pin=GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;			
	 GPIO_Init(GPIOB,&GPIO_InitStruct);			                      
	 
	 GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);      //sclk	10	 13
	 GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);	//miso	11	 14
	 GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);	//mosi12	 15
	 
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	 
	 
	 SPI_I2S_DeInit(SPI2);	    
	 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	 SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
	 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
	 SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//SPI_CPOL_Low 	 SPI_CPOL_High
	 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
	 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard	 //SPI_NSS_Soft
	 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; 	//16
	 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	 SPI_InitStructure.SPI_CRCPolynomial = 7; 
	 SPI_Init(SPI2,&SPI_InitStructure); 
	 SPI_Cmd(SPI2,ENABLE); 
	 //CS
	 GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	 GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	 GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
	 GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12; // 3
	 GPIO_Init(GPIOB,&GPIO_InitStruct);    // d  	    			
	 */
	 deselect(); //T_DCS(); 				 
	 //T_PEN				
	 /*
	 GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	 GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	 GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	 GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
	 GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	 GPIO_Init(GPIOD,&GPIO_InitStruct);     
	 */
	GPIOMode(PinPort(irqpin), PinBit(irqpin), INPUT, FASTSPEED, PUSHPULL,
			PULLUP);
			
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

uint8 TouchScreen::readAds7846(void) {
	u8 t, t1, count = 0;
	u16 databuffer[2][10];
	u16 temp = 0;

//	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0) {
		readOnce();
		while (count < 10) {

			readOnce();

			databuffer[0][count] = X;
			databuffer[1][count] = Y;
			count++;

		}

		if (count == 10) {
			do {
				t1 = 0;
				for (t = 0; t < count - 1; t++) {
					if (databuffer[0][t] > databuffer[0][t + 1]) {
						temp = databuffer[0][t + 1];
						databuffer[0][t + 1] = databuffer[0][t];
						databuffer[0][t] = temp;
						t1 = 1;
					}
				}
			} while (t1);
			do {
				t1 = 0;
				for (t = 0; t < count - 1; t++) {
					if (databuffer[1][t] > databuffer[1][t + 1]) {
						temp = databuffer[1][t + 1];
						databuffer[1][t + 1] = databuffer[1][t];
						databuffer[1][t] = temp;
						t1 = 1;
					}
				}
			} while (t1);

			X = 2047
					- ((databuffer[0][3] + databuffer[0][4] + databuffer[0][5])
							/ 3);
			Y = ((databuffer[1][3] + databuffer[1][4] + databuffer[1][5]) / 3);
			flag = 1;
			return 1;
		}
		flag = 0;
//	}
	return 0;

}

void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {

		EXTI_ClearITPendingBit (EXTI_Line6);
		// Read_Ads7846(); 
	}
}

void TouchScreen::convertPos(void) {
	readAds7846();
	X0 = (int) ((Y - 103) / 7.7);
	Y0 = (int) ((X - 104) / 5.56);
	if (X0 > 240) {
		X0 = 240;
	}
	if (Y0 > 320) {
		X0 = 320;
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
