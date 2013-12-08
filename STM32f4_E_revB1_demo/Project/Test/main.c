/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
//#include <yfuns.h>
#include "includes.h"

#define DLY_100US  1000

Int32U CriticalSecCntr;
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


USART_InitTypeDef USART_InitStructure;
static __IO uint32_t TimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

void SysTickStart(uint32_t Tick)
{
RCC_ClocksTypeDef Clocks;
volatile uint32_t dummy;

  RCC_GetClocksFreq(&Clocks);

  dummy = SysTick->CTRL;  
  SysTick->LOAD = (Clocks.HCLK_Frequency/8)/Tick;
  
  SysTick->CTRL = 1;
}

void SysTickStop(void)
{
    SysTick->CTRL = 0;
}

/* Private function prototypes -----------------------------------------------*/
#ifndef  USE_IAR_CONSOLE
static int MyLowLevelGetchar(void);
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize);
size_t __read(int handle, unsigned char * buffer, size_t size);
#endif

/* Private functions ---------------------------------------------------------*/
int Button_LED_Demo (void)
{
        int x=4;
        STM_EVAL_LEDInit (LED1);
        STM_EVAL_PBInit (BUTTON_WAKEUP, BUTTON_MODE_GPIO);
        printf ("Demo LED: \n\r");
        printf ("LED should be blinking 3 times and then turns off!\n\r");
        
        while (x--)
        {
          STM_EVAL_LEDOn (LED1);
          DelayResolution100us (1000);
          STM_EVAL_LEDOff (LED1);
          DelayResolution100us (1000);
        }
        printf ("Demo Button:\n\r");
        printf ("Press the button to blink the LED again\n\r");
        
        while (!STM_EVAL_PBGetState (BUTTON_WAKEUP));
        while (STM_EVAL_PBGetState (BUTTON_WAKEUP))
        {
            STM_EVAL_LEDOn (LED1);
            DelayResolution100us (250);
            STM_EVAL_LEDOff (LED1);
            DelayResolution100us (750);
        }
        STM_EVAL_LEDOn (LED1);
        return 0;
}

int RTC_Demo(void)
{
RTC_TimeTypeDef time = {
  .RTC_Hours = 0,
  .RTC_Minutes = 0,
  .RTC_Seconds = 0,
  .RTC_H12 = 0
};
 
  /*SysTick to times per second*/
  SysTickStart(1);

  /* Enable LSE */
  RCC_APB1PeriphClockCmd(  RCC_APB1Periph_PWR, ENABLE);
  PWR_BackupAccessCmd(ENABLE);
  /* */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  int Tick = 0;
  
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
    /*Check Ticks*/
    if( SysTick->CTRL & (1<<16))
    {
      if(6 < ++Tick)
      {
        return 1;
      }
    }
  }
  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  //RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForSynchro();

  /* Set RTC prescaler: set RTC period to 1sec */
 // RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForSynchro();
  /*Set time*/
  
  printf ("Please wait...");
  DelayResolution100us (30);
  printf ("RTC intialized!\n\r");
  
  RTC_SetTime(RTC_Format_BIN,&time);
  /*Convert RTC to hour min and sec*/
  RTC_GetTime(RTC_Format_BIN,&time);
  
//  printf("Time %.2d:%.2d:%.2d\r",time.RTC_Hours,time.RTC_Minutes,time.RTC_Seconds);
  printf ("Press space to exit this demo!\n\r");

  Tick = 0;
  
  while(' ' !=  getchar())
  {
    /*Check Ticks*/
    if( SysTick->CTRL & (1<<16))
    {
      RTC_GetTime(RTC_Format_BIN,&time);
      printf("Time %.2d:%.2d:%.2d\r",time.RTC_Hours,time.RTC_Minutes,time.RTC_Seconds);
    }
  }
  
  printf("Time %.2d:%.2d:%.2d\n\r",time.RTC_Hours,time.RTC_Minutes,time.RTC_Seconds);
  //SysTickStop();

  return 0;
}

int Ethernet_Demo()
{
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ETH_InitTypeDef ETH_InitStructure;

  printf("Connect LAN cable and press SPACE\n\r");
         
  while(' ' != getchar());
  

  /* Enable ETHERNET clocks  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                         RCC_AHB1Periph_ETH_MAC_Rx | RCC_AHB1Periph_ETH_MAC_PTP, ENABLE);

  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |	RCC_AHB1Periph_GPIOB | 
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /*Select RMII Interface*/
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
  
  /* ETHERNET pins configuration */
  /* PA
    ETH_RMII_REF_CLK: PA1
    ETH_RMII_MDIO: PA2
    ETH_RMII_MDINT: PA3
    ETH_RMII_CRS_DV: PA7
   */

  /* Configure PA1, PA2, PA3 and PA7*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect PA1, PA2, PA3 and PA7 to ethernet module*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

#ifdef  STM32_E407
  /* PB
    ETH_RMII_TX_EN: PG11
  */

  /* Configure PG11*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Connect PG11 to ethernet module*/
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
#else
  /* PB
    ETH_RMII_TX_EN: PB11
  */

  /* Configure PB11*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect PB11 to ethernet module*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
#endif

  /* PC
    ETH_RMII_MDC: PC1
    ETH_RMII_RXD0: PC4
    ETH_RMII_RXD1: PC5
  */

  /* Configure PC1, PC4 and PC5*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect PC1, PC4 and PC5 to ethernet module*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

  /* PG
    ETH_RMII_TXD0: PG13
    ETH_RMII_TXD1: PG14
  */

  /* Configure PG13 and PG14*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Connect PG13 and PG14 to ethernet module*/
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();
  
  /* Software reset */ 
  ETH_SoftwareReset();
  
  /* Wait for software reset */
  while(ETH_GetSoftwareResetStatus()==SET);

  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);
  
  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;  
  //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;                                      
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;              
  //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;                                                                                  
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;                                                                                  
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;                                                                                                                                                                        
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;                                                                                                       
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;      
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;                                                             
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;      
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;                        
  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;                        
  ETH_InitStructure.ETH_Speed = ETH_Speed_100M;                        

  unsigned int PhyAddr;
    union {
      Int32U    HI_LO;
      struct
      {
        Int16U  LO;
        Int16U  HI;
      };
    } PHYID;
  for(PhyAddr = 1; 32 >= PhyAddr; PhyAddr++)
  { 
    

#ifdef  STM32_E407  // edit SPP for Olimex STM32-E407
    // datasheet for the ks8721bl ethernet controller (http://www.micrel.com/_PDF/Ethernet/datasheets/ks8721bl-sl.pdf)
    // page 20 --> PHY Identifier 1 and 2
    PHYID.HI = ETH_ReadPHYRegister(PhyAddr,2);  // 0x0022
    PHYID.LO = ETH_ReadPHYRegister(PhyAddr,3);  // 0x1619
    if (0x00221619 == PHYID.HI_LO) break;
#else // original for IAR STM32-F407
    PHYID.HI = ETH_ReadPHYRegister(PhyAddr,2);
    PHYID.LO = ETH_ReadPHYRegister(PhyAddr,3) & 0xFFF0;
    if((0x00061c50 == PHYID.HI_LO) || (0x02038460 == PHYID.HI_LO)) break;
//    if((0x0006 == ETH_ReadPHYRegister(PhyAddr,2))
//    && (0x1c50 == (ETH_ReadPHYRegister(PhyAddr,3)&0xFFF0))) break;
    //if((0x0203 == ETH_ReadPHYRegister(PhyAddr,2))
    //&& (0x8460 == (ETH_ReadPHYRegister(PhyAddr,3)&0xFFF0))) break;
#endif
  }
   
  if(32 < PhyAddr)
  {
    printf("Ethernet Phy Not Found\n\r");
    return 1;
  }
  
  /* Configure Ethernet */
  if(0 == ETH_Init(&ETH_InitStructure, PhyAddr))
  {
    printf("Ethernet Initialization Failed\n\r");
    return 1;
  }
 
  printf("Press Space for exit\n\r");
  printf("Check LAN LEDs\n\r");

  /* uIP stack main loop */
  uIPMain();  
 
  TIM_DeInit(TIM2);
  
  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  return 0;  
}

#define PI (float)3.14159
int Audio_Demo()
{
I2S_InitTypeDef I2S_InitStructure; 
GPIO_InitTypeDef GPIO_InitStructure;

int cntr = 0;
int index = 0;
int data;
//int dir = 0;
int volume_r, volume_l;

  /*PLLI2S configure*/
  RCC_PLLI2SConfig(271,2);
  /*Enable PLLI2S*/
  RCC_PLLI2SCmd(ENABLE);
  /*Wait PLLI2S Lock*/
  while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY));
  /*PLLI2S is I2S clock source*/
  RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
  /* Enable I2S3 APB1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
  /* Deinitialize SPI2 peripheral */
  SPI_I2S_DeInit(SPI3); 

	/* I2S2 peripheral configuration */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_24b;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_44k;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;  
  I2S_Init(SPI3, &I2S_InitStructure);

  /* Disable the I2S2 TXE Interrupt */ 
  SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);
 
  /*Pin Configure*/
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

  /* Configure PA15*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Configure PA15*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);

  /* Configure PB3 and PB5*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Connect PB3 and PB5 I2S3 module*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
  /* Configure PC7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect PC7 I2S3 module*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
  /* Enable the SPI2/I2S2 peripheral */
  I2S_Cmd(SPI3, ENABLE);
  /*Systick start*/
  SysTickStart(20);

  while('\x1B' != getchar())
  {
    if(SysTick->CTRL & (1<<16))
    {
      cntr++;
      if(20 > cntr)
      {
        volume_r = ((1<<13)-1);
        volume_l = 0;
      }
      else
      {
        if(25 > cntr)
        {
          volume_l = 0;
          volume_r = 0;
        }
        else
        {
          if(45 > cntr)
          {
            volume_l = ((1<<13)-1);
            volume_r = 0;
          }
          else
          {
            if(50 > cntr)
            {
               volume_l = 0;
               volume_r = 0;
            }
            else cntr = 0;
          }
        }
          
      }
    }
    index+= SAMPLES_NUM * 1000/I2S_AudioFreq_44k;
    data = Sin_Table[(SAMPLES_NUM - 1) & index]*volume_l;
    while(RESET == SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI3,(data>>8) & 0xFFFF);
    while(RESET == SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI3,(data<<8) & 0xFFFF);
    data = Sin_Table[(SAMPLES_NUM - 1) & (index+SAMPLES_NUM/4)]*volume_r;
    while(RESET == SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI3,(data>>8) & 0xFFFF);
    while(RESET == SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI3,(data<<8) & 0xFFFF);   
  }

  return 0;  
}

void USB_USR_Null(void)
{
  
}

void USB_USR_Reset(Int8U speed)
{
  
}


USBD_Usr_cb_TypeDef USR_cb =
{
  USB_USR_Null,
  USB_USR_Reset,
  USB_USR_Null,
  USB_USR_Null,
  USB_USR_Null,
  USB_USR_Null,
  USB_USR_Null,  
};

#define MOUSE_DELTA           4
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
}

void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
}

void USB_Mouse(void)
{

Int8U buf[4];

#define Buttons buf[0]
#define X  buf[1]
#define Y  buf[2]
int D=0, Times=0;
Int8U ButtonsHold = 0;

  ENTR_CRT_SECTION();
  // SysTick end of count event each 0.01s 
  /*SysTick to times per second*/
  SysTickStart(100);
  
  EXT_CRT_SECTION();

  /*Init Buttons*/
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_GPIO);


  X = 0;
  Y = 0;
  Buttons = 0;
  ButtonsHold = 0;
  
  while (!STM_EVAL_PBGetState (BUTTON_WAKEUP))
  {
      switch (D)
      {
      case 0:
        X += MOUSE_DELTA;
        break;
      case 1:
        Y += MOUSE_DELTA;
        break;
      case 2:
        X -= MOUSE_DELTA;
        break;
      case 3:
        Y -= MOUSE_DELTA;
        break;
      }
      
      Times++;
      D = (D + (Times / 20)) % 4;
      Times = Times % 20;

        if(Y || X  || Buttons || (ButtonsHold ^ Buttons))
        {
          // Send report
          USBD_HID_SendReport (&USB_OTG_dev, 
                         buf,
                         4);
          ButtonsHold = Buttons;
          Y = X = 0;
          Buttons = 0;
        }
  }
  while (STM_EVAL_PBGetState (BUTTON_WAKEUP));
}

int USBOTG_2_Demo(void)
{
GPIO_InitTypeDef GPIO_InitStructure;    
NVIC_InitTypeDef NVIC_InitStructure; 


  printf("Connect USB 2 OTG port to PC \n\r");
  printf("Press WAKEUP Button for start\n\r");
  while (!STM_EVAL_PBGetState (BUTTON_WAKEUP));
  while (STM_EVAL_PBGetState (BUTTON_WAKEUP));
  printf("Connect Disconnect USB OTG 2 port from PC \n\r");
  printf("Press WAKEUP Button again for exit\n\r");

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | 
                                GPIO_Pin_14 | 
                                GPIO_Pin_15;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource12, GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS) ; 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS) ;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_OTG_HS, ENABLE) ;  
    
  USBD_Init(&USB_OTG_dev,
            USB_OTG_HS_CORE_ID,
            &USR_desc, 
            &USBD_HID_cb, 
            &USR_cb);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_OUT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_HS_EP1_IN_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   

  USB_Mouse();

  return 0;
}

int USBOTG_1_Demo(void)
{
GPIO_InitTypeDef GPIO_InitStructure;    
NVIC_InitTypeDef NVIC_InitStructure; 

  printf("Connect USB OTG 1 port to PC \n\r");
  printf("Press WAKEUP Button for start\n\r");
  while (!STM_EVAL_PBGetState (BUTTON_WAKEUP));
  while (STM_EVAL_PBGetState (BUTTON_WAKEUP));
  printf("Connect Disconnect OTG 1 port from PC \n\r");
  printf("Press WAKEUP Button again for exit\n\r");
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);  
  
  /* Configure SOF ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8  | 
                                GPIO_Pin_11 | 
                                GPIO_Pin_12;
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_OTG1_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ; 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;
  
  /* Configure  VBUS Pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  
  /* Configure ID pin */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_OTG1_FS) ;   

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ; 


  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc, 
            &USBD_HID_cb, 
            &USR_cb);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  USB_Mouse();
  
  return 0;
}

#define UPDATE_SHOW_DLY       ((Int32U)(0.5 * 2))

int SDCard_Demo(void)
{
int ret = 1, DemoOver = 0;
Int32U Dly = UPDATE_SHOW_DLY;
DiskStatusCode_t StatusHold = (DiskStatusCode_t) -1;
Int32U Size,Tmp;
Int8U buff[SD_DEF_BLK_SIZE];

//  printf("Press Esc for exit\n\r");
  /*SysTick to times per second*/
  SysTickStart(2);
  //
  SdDiskInit();

  __enable_interrupt();

  while(' ' !=  getchar())
  {
    if (DemoOver)
      break;
    if( SysTick->CTRL & (1<<16))
    {
      // Update MMC/SD card status
      SdStatusUpdate();
      if(Dly-- == 0)
      {
        // LCD show
        Dly = UPDATE_SHOW_DLY;
        // Current state of MMC/SD show
        pDiskCtrlBlk_t pMMCDiskCtrlBlk = SdGetDiskCtrlBkl();
        if(StatusHold != pMMCDiskCtrlBlk->DiskStatus)
        {
          StatusHold = pMMCDiskCtrlBlk->DiskStatus;
          switch (pMMCDiskCtrlBlk->DiskStatus)
          {
          case DiskCommandPass:
            // Calculate MMC/SD size [MB]
            Size = (pMMCDiskCtrlBlk->BlockNumb * pMMCDiskCtrlBlk->BlockSize);
            Tmp  = Size/1000000;
            Tmp += ((Size%1000000)<1000000/2)?0:1;
            if(pMMCDiskCtrlBlk->DiskType == DiskMMC)
            {
              printf("MMC Card - %d MB\n\r",Tmp);
            }
            else
            {
              printf("SD Card - %d MB\n\r",Tmp);
            }

            if(pMMCDiskCtrlBlk->WriteProtect == TRUE)
            {
              printf("Card is Write Protected\n\r");
            }

            for(volatile int i = 0; 100000 > i ; i++);
            if(DiskCommandPass == SdDiskIO(buff,pMMCDiskCtrlBlk->BlockNumb-1,1,DiskRead))
            {
              if(0xAA55A55A != *((Int32U *)buff))
              {
                if(FALSE == pMMCDiskCtrlBlk->WriteProtect)
                {
                  *((Int32U *)buff) = 0xAA55A55A;
                  
                  if(DiskCommandPass != SdDiskIO(buff,pMMCDiskCtrlBlk->BlockNumb-1,1,DiskWrite) &&
                     DiskCommandPass != SdDiskIO(buff,pMMCDiskCtrlBlk->BlockNumb-1,1,DiskVerify) )
                    {
                      printf("Card I/O error\n\r");
                      ret = 1;
                      DemoOver = 1;
                    }
                    else
                    {
                      ret = 0;
                      DemoOver = 1;
                    }
                }
              }
              else
              {
                ret = 0;
                DemoOver = 1;
              }
            }
            else
            {
              printf("Card I/O error\n\r");
              ret = 1;
              DemoOver = 1;
            }
            break;
          default:
            printf("Please, Insert Card or press SPACE to interrupt this demo!\n\r");
          }
        }
      }
    }
  }

  __disable_interrupt();

  return ret;
}

struct s_Demo
{
  const char * name;
  int (* Demo)(void);
};

const struct s_Demo Demo[]=
{
  {"RTC",RTC_Demo},
  {"Button and LED", Button_LED_Demo},
  {"Ethernet",Ethernet_Demo},
  {"USB OTG 1 Connector",USBOTG_1_Demo},
  {"USB OTG 2 Connector",USBOTG_2_Demo},
  {"SD Card",SDCard_Demo},
};

void My_GPIOReset (void)
{
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOA, DISABLE);

    //RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    //RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOC, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOD, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOE, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOF, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOG, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOH, DISABLE);

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, ENABLE);
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOI, DISABLE);
}

void Blink (int x)
{
        STM_EVAL_LEDInit (LED1);
        while (x--)
        {
          STM_EVAL_LEDOn (LED1);
          DelayResolution100us (2000);
          STM_EVAL_LEDOff (LED1);
          DelayResolution100us (2000);
        }
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  char c, Flag=0;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */  
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();
  STM_EVAL_PBInit (BUTTON_WAKEUP, BUTTON_MODE_GPIO);

  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  STM_EVAL_COMInit(COM1, &USART_InitStructure);
  printf("\n\r");
  printf("**************************************************\n\r");
  printf("            Olimex-STM32E407\n\r");
  printf("             Demo  MODE!!\n\r");
  printf("**************************************************\n\r");
    
  do
  {
    for(Int32U i = 0;sizeof(Demo)/sizeof(struct s_Demo) > i;)
    {
  
      SysTickStop();
      // USB OTG demos are performed just once.
      if (strcmp ("USB OTG 1 Connector", Demo[i].name) == 0)
        if (!(Flag & 0x01))
        {
          printf ("!!!! This demo is performed just once! !!!!\n\r");
          Flag = Flag | 0x01;
        }
        else
        {
          i++;
          continue;
        }
        
      if (strcmp ("USB OTG 2 Connector", Demo[i].name) == 0)
        if (!(Flag & 0x02))
        {
          printf ("!!!! This demo is performed just once !!!!\n\r");
          Flag = Flag | 0x02;
        }
        else
        {
          i++;
          continue;
        }

      printf("\n\r");
      printf("**************************************************\n\r");
      printf("%s ",Demo[i].name);
      printf("Demo Started\n\r");
  
      Demo[i].Demo();
  
      SysTickStop();
      
      My_GPIOReset ();
      printf("%s ",Demo[i].name);
      printf("Demo End   \n\r");
      printf("**************************************************\n\r");
      
      
      printf("Press WAKEUP button for next demo!\n\r");
      i++;
      while (!STM_EVAL_PBGetState (BUTTON_WAKEUP));
      while (STM_EVAL_PBGetState (BUTTON_WAKEUP));
    }
  
    printf("Demos finished!\n\r");
    printf("Do you want to run the demo again\n\r");
    printf("press Y/y or N/n\n\r");
    do
    {
      c = getchar();
    }
    while((('Y' != c) && ('y' != c) && ('N' != c) && ('n' != c)));
  }
  while (('Y' == c) || ('y' == c));
  
  printf("Demos are finished! The green LED will blink now.\n\r");
  // End of the demo
  while (1)
  {
    Blink (1);
  }
}

/*************************************************************************
 * Function Name: DelayResolution100us
 * Parameters: Int32U Dly
 *
 * Return: none
 *
 * Description: Delay ~ (arg * 100us)
 *
 *************************************************************************/
void DelayResolution100us(Int32U Dly)
{
  for(; Dly; Dly--)
  {
    for(volatile Int32U j = DLY_100US; j; j--)
    {
    }
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#ifndef USE_IAR_CONSOLE
/*************************************************************************
 * Function Name: __write
 * Parameters: Low Level cahracter output
 *
 * Return:
 *
 * Description:
 *
 *************************************************************************/
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
size_t nChars = 0;

  for (/*Empty */; Bufsize > 0; --Bufsize)
  {
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET);
    USART_SendData(EVAL_COM1, * Buf++);
    ++nChars;
  }
  return nChars;
}
/*************************************************************************
 * Function Name: __read
 * Parameters: Low Level cahracter input
 *
 * Return:
 *
 * Description:
 *
 *************************************************************************/
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  int nChars = 0;

  /* This template only reads from "standard in", for all other file
   * handles it returns failure. */
  if (handle != _LLIO_STDIN)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size > 0; --size)
  {
    int c = MyLowLevelGetchar();
    if (c < 0)
      break;

    *buffer++ = c;
    ++nChars;
  }

  return nChars;
}

static int MyLowLevelGetchar(void)
{
int ch;
unsigned int status = EVAL_COM1->SR;

  if(status & USART_FLAG_RXNE)
  {
    ch = USART_ReceiveData(EVAL_COM1);
    if(status & (USART_FLAG_ORE | USART_FLAG_PE | USART_FLAG_FE) )
    {
      return (ch | 0x10000000);
    }
    return (ch & 0xff );
  }
  return -1;
}
#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
