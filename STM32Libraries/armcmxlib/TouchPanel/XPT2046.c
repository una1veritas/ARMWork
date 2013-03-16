/*********************************************************************************************************
*
* File                : TouchPanel.c
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

/* Includes ------------------------------------------------------------------*/
#include "armcore.h"
#include "XPT2046.h"
//#include "SysTick/systick.h"
//#include "LCD/LCD.h"

/* Private variables ---------------------------------------------------------*/
/*
Matrix matrix ;
Coordinate  display ;


Coordinate ScreenSample[3];

Coordinate DisplaySample[3] =   {
                                            { 45, 45 },
											{ 270, 90},
                                            { 100,190}
	                            } ;
*/
/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2

static TouchPadStruct TPStruct;// = {0,0,0,0,0, {{0,0}, {0,0}, {0,0}}, {{45,45}, {270,90},{100, 190}}, {}, {}};

	// 	((x) ? GPIO_SetBits(GPIOC,GPIO_Pin_4) : GPIO_ResetBits(GPIOC,GPIO_Pin_4))
void TP_CS(boolean x) {
	if (x) 
		digitalWrite(TPStruct.pin_cs, SET);
	else
		digitalWrite(TPStruct.pin_cs, RESET);
}

void TP_select(void) {
		digitalWrite(TPStruct.pin_cs, RESET);
}

void TP_deselect(void) {
		digitalWrite(TPStruct.pin_cs, SET);
}

//#define TP_INT_IN   GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
boolean TP_req(void) {
	return GPIO_ReadInputDataBit(PinPort(TPStruct.pin_req), PinBit(TPStruct.pin_req));
}

/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void ADS7843_SPI_Init(void) 
{ 
  SPI_InitTypeDef  SPI_InitStructure;
//debug
	usart_print(&stdserial, "ads7843_spi_init is enabling SPI.\n");
	usart_flush(&stdserial);
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  /* DISABLE SPI2 */ 
  SPI_Cmd(TPStruct.port->SPIx, DISABLE); 
  /* SPI2 Config -------------------------------------------------------------*/ 
	spi_setMode(TPStruct.port, SPI_BaudRatePrescaler_64, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_FirstBit_MSB);
/*
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(TPStruct.port->SPIx, &SPI_InitStructure); 
	*/
  /* Enable SPI2 */ 
  SPI_Cmd(TPStruct.port->SPIx, ENABLE); 
} 

/*******************************************************************************
* Function Name  : TP_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_Init(SPIPort * port, GPIOPin sck, GPIOPin si, GPIOPin so, GPIOPin cs, GPIOPin req)
{ 
  GPIO_InitTypeDef GPIO_InitStruct;
//debug
	usart_print(&stdserial, "TP_Init is enabling TP.\n");
	usart_flush(&stdserial);

	TPStruct.port = port;
	TPStruct.pin_cs = cs;
	TPStruct.pin_req = req;
	spi_init(TPStruct.port, SPI2, sck, si, so, cs);  
  //TP_CS(1); 
  //ADS7843_SPI_Init(); 
  SPI_Cmd(TPStruct.port->SPIx, DISABLE); 
	spi_setMode(TPStruct.port, SPI_BaudRatePrescaler_64, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_FirstBit_MSB);
  SPI_Cmd(TPStruct.port->SPIx, ENABLE);
	GPIOMode(PinPort(req), PinBit(req), GPIO_Mode_IN, GPIO_Speed_50MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL);
} 


/*******************************************************************************
* Function Name  : DelayUS
* Description    : 
* Input          : - cnt:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*
static void DelayUS(vu32 cnt)
{
  uint16_t i;
  for(i = 0;i<cnt;i++)
  {
     uint8_t us = 12;
     while (us--)
     {
       ;   
     }
  }
}
*/
/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 
* Input          : - cmd: 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
  /* Wait for SPI2 Tx buffer empty */ 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI2 data */ 
  SPI_I2S_SendData(SPI2,cmd); 
  /* Wait for SPI2 data reception */ 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI2 received data */ 
  SPI_I2S_ReceiveData(SPI2); 
} 



/*******************************************************************************
* Function Name  : RD_AD
* Description    : 
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  unsigned short buf,temp; 
  /* Wait for SPI2 Tx buffer empty */ 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI2 data */ 
  SPI_I2S_SendData(SPI2,0x0000); 
  /* Wait for SPI2 data reception */ 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI2 received data */ 
  temp=SPI_I2S_ReceiveData(SPI2); 
  buf=temp<<8; 
  delay_us(1); 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI2 data */ 
  SPI_I2S_SendData(SPI2,0x0000); 
  /* Wait for SPI2 data reception */ 
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read SPI2 received data */ 
  temp=SPI_I2S_ReceiveData(SPI2); 
  buf |= temp; 
  buf>>=3; 
  buf&=0xfff; 
  return buf; 
} 


/*******************************************************************************
* Function Name  : Read_X
* Description    : Read ADS7843 ADC X 
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  delay_us(1); 
  WR_CMD(CHX); 
  delay_us(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*******************************************************************************
* Function Name  : Read_Y
* Description    : Read ADS7843 ADC Y
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  delay_us(1); 
  WR_CMD(CHY); 
  delay_us(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 


/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : Read ADS7843
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  delay_us(1); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 


	
/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : Get TouchPanel X Y
* Input          : None
* Output         : None
* Return         : Coordinate *
* Attention		 : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen;
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};
  
  do
  {		   
    TP_GetAdXY(TP_X,TP_Y);  
	buffer[0][count]=TP_X[0];  
	buffer[1][count]=TP_Y[0];
	count++;  
  }
  while(!TP_req() && count<9);  /* TP_INT_IN  */
  if(count==9)   /* Average X Y  */ 
  {
	/* Average X  */
  temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
	temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
	temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;

	m0=temp[0]-temp[1];
	m1=temp[1]-temp[2];
	m2=temp[2]-temp[0];

	m0=m0>0?m0:(-m0);
  m1=m1>0?m1:(-m1);
	m2=m2>0?m2:(-m2);

	if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;

	if(m0<m1)
	{
	  if(m2<m0) 
	    screen.x=(temp[0]+temp[2])/2;
	  else 
	    screen.x=(temp[0]+temp[1])/2;	
	}
	else if(m2<m1) 
	  screen.x=(temp[0]+temp[2])/2;
	else 
	  screen.x=(temp[1]+temp[2])/2;

	/* Average Y  */
  temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
	temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
	temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
	m0=temp[0]-temp[1];
	m1=temp[1]-temp[2];
	m2=temp[2]-temp[0];
	m0=m0>0?m0:(-m0);
	m1=m1>0?m1:(-m1);
	m2=m2>0?m2:(-m2);
	if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;

	if(m0<m1)
	{
	  if(m2<m0) 
	    screen.y=(temp[0]+temp[2])/2;
	  else 
	    screen.y=(temp[0]+temp[1])/2;	
    }
	else if(m2<m1) 
	   screen.y=(temp[0]+temp[2])/2;
	else
	   screen.y=(temp[1]+temp[2])/2;

	return &screen;
  }  
  return 0; 
}
	 

/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : Calculate K A B C D E F
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{

  FunctionalState retTHRESHOLD = ENABLE ;
  /* K£½(X0£­X2) (Y1£­Y2)£­(X1£­X2) (Y0£­Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = DISABLE;
  }
  else
  {
    /* A£½((XD0£­XD2) (Y1£­Y2)£­(XD1£­XD2) (Y0£­Y2))£¯K	*/
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
	/* B£½((X0£­X2) (XD1£­XD2)£­(XD0£­XD2) (X1£­X2))£¯K	*/
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C£½(Y0(X2XD1£­X1XD2)+Y1(X0XD2£­X2XD0)+Y2(X1XD0£­X0XD1))£¯K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D£½((YD0£­YD2) (Y1£­Y2)£­(YD1£­YD2) (Y0£­Y2))£¯K	*/
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E£½((X0£­X2) (YD1£­YD2)£­(YD0£­YD2) (X1£­X2))£¯K	*/
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F£½(Y0(X2YD1£­X1YD2)+Y1(X0YD2£­X2YD0)+Y2(X1YD0£­X0YD1))£¯K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : Touch panel X Y to display X Y
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  FunctionalState retTHRESHOLD =ENABLE ;

  if( matrixPtr->Divider != 0 )
  {
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 


/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
