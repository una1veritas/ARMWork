/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/demo.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    17-June-2011
  * @brief   This file includes the menu navigation driver for the
  *          STM3210C-EVAL demonstration.
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
#include "demo.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct time_t
{
  uint8_t sec_l;
  uint8_t sec_h;
  uint8_t min_l;
  uint8_t min_h;
  uint8_t hour_l;
  uint8_t hour_h;
};
struct time_t TimeStruct,TimesStruct;

const uint8_t buf[EEPROM_PAGE_SIZE];   
uint8_t RBuffer[10];
uint8_t WBuffer[10];
__IO uint32_t sEERAddr   = 0;
__IO uint32_t sEEWAddr   = 0;
uint32_t sEENumData = 10;

__IO uint32_t Temperature = 0;

__IO uint16_t WReportNum   = 1;
__IO uint16_t CurrentReportNum = 0;

__IO uint32_t NextRep  = 0;
__IO uint32_t NoRep  = 0;

uint8_t Message[24];

__IO uint32_t PressedKey = 0;
__IO uint32_t ReadKey = 0;

__IO uint32_t ExitMutex  = 0;
__IO uint32_t GetMutex   = 0;
__IO uint32_t ShowMutex  = 0;
__IO uint32_t EraseMutex = 0;
__IO uint32_t SaveReportCmd = 0;
__IO uint32_t DisplayTimeout = 0;

__IO uint32_t AsynchPrediv, SynchPrediv = 0 ;
  
/* Private function prototypes -----------------------------------------------*/
static void TIM7_Config(void);
static void RTC_Config(void);
static void Calendar_TimePreAdjust(void);
static uint32_t Calendar_TimeRegulate(void);
static uint8_t Calendar_ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin);
static void Set_Time(void);
#ifdef USE_STM322xG_EVAL
static void Get_Time(void);
#else
static void Get_Time(uint32_t TimeVar);
#endif /* USE_STM322xG_EVAL */
static void Report_Display(uint8_t* pBuff );
static void Delay(uint32_t nCount);


/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Initialize resources used for this example 
  * @param  None
  * @retval None
  */
void Demo_Init(void)
{  
  /* Initialize LEDs and push-buttons mounted on STM3210X-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  STM_EVAL_PBInit(Button_KEY, BUTTON_MODE_EXTI);
  
#ifdef USE_STM32100E_EVAL
  STM_EVAL_PBInit(Button_RIGHT, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(Button_LEFT, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(Button_UP, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(Button_DOWN, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(Button_SEL, BUTTON_MODE_EXTI);
#endif /* USE_STM32100E_EVAL */
  
  /* Initialize the LCD */
#ifdef USE_STM322xG_EVAL
  STM322xG_LCD_Init();  
#elif defined (USE_STM3210C_EVAL)
  STM3210C_LCD_Init();
#elif defined (USE_STM32100E_EVAL)
  STM32100E_LCD_Init();  
#endif /* USE_STM322xG_EVAL */
  
  LCD_Clear(White);
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);    
  LCD_DisplayStringLine(Line0, (uint8_t*)"CPAL FW  I2C Adv_Exp");
  
  /* Configure the IO Expander */
  if(IOE_Config() == IOE_OK)
  { 
    LCD_DisplayStringLine(Line1, (uint8_t*)"   IO Expander OK   ");
    Delay(0x3FFFFF);
  }
  else
  {
    LCD_DisplayStringLine(Line4, (uint8_t*)"IO Expander FAILED ");
    LCD_DisplayStringLine(Line5, (uint8_t*)" Please Reset the  ");
    LCD_DisplayStringLine(Line6, (uint8_t*)"    The board      ");
    while (1);
  }
  
#ifdef USE_STM32100E_EVAL
  /* Initialize LM75 TSENSOR */
  LM75_Config();
  
  /* Get LM75 TSENSOR Status */
  if(LM75_GetStatus() == SUCCESS)
  { 
    LCD_DisplayStringLine(Line1, (uint8_t*)"   TempSensor OK    ");
    Delay(0x3FFFFF);
  }
  else
  {
    LCD_DisplayStringLine(Line4, (uint8_t*)" TempSensor FAILED ");
    LCD_DisplayStringLine(Line5, (uint8_t*)" Please Reset the  ");
    LCD_DisplayStringLine(Line6, (uint8_t*)"    The board      ");
    while (1);
  }
#endif /* USE_STM32100E_EVAL */
  
  /* Initialize sEE_DevStructure */
  sEE_DevStructure.sEEAddress = EEPROM_ADDRESS;
  sEE_DevStructure.sEEPageSize = EEPROM_PAGE_SIZE;
  sEE_DevStructure.sEEMemoryAddrMode = sEE_OPT_16BIT_REG;
  
  /* Initialize sEE peripheral */
  sEE_StructInit(&sEE_DevStructure);
  sEE_Init(&sEE_DevStructure); 
  
  /* Leds Control blocks */
  LCD_SetTextColor(Blue);
  LCD_DrawRect(200, 310, 30, 60);
  LCD_SetTextColor(Red);
  LCD_DrawRect(200, 230, 30, 60);
  LCD_SetTextColor(Green);
  LCD_DrawRect(200, 150, 30, 60);
  LCD_SetTextColor(Black);
  LCD_DrawRect(200, 70, 30, 60);
  
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
  /* Enable the Touch Screen and Joystick interrupts */
  IOE_ITConfig(IOE_ITSRC_JOYSTICK | IOE_ITSRC_TSC);
#else
  /* Enable the Touch Screen  interrupts */
  IOE_ITConfig(IOE_ITSRC_TSC);
#endif /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */
  
  /* RTC Configuration */
  RTC_Config();
  
  /* Config TIM7 */ 
  TIM7_Config();   
  
  /* Leds Control blocks */
  LCD_SetTextColor(Blue);
  LCD_DrawRect(200, 310, 30, 60);
  LCD_SetTextColor(Red);
  LCD_DrawRect(200, 230, 30, 60);
  LCD_SetTextColor(Green);
  LCD_DrawRect(200, 150, 30, 60);
  LCD_SetTextColor(Black);
  LCD_DrawRect(200, 70, 30, 60);
}

/**
  * @brief  Display the navigation menu.
  * @param  None
  * @retval None
  */
void Display_Menu (void)
{  
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);
  LCD_DisplayStringLine(Line1, (uint8_t*)"      Main Menu     ");
  LCD_DisplayStringLine(Line2, (uint8_t*)"Up   :  Show Report ");
  LCD_DisplayStringLine(Line3, (uint8_t*)"Down :  Erase EEPROM");
  LCD_DisplayStringLine(Line4, (uint8_t*)"Right:  Show Temp   ");
  LCD_DisplayStringLine(Line5, (uint8_t*)"Left :  Show Time   ");
  LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
  LCD_DisplayStringLine(Line7, (uint8_t*)"--TouchScreen zone--");
}

/**
  * @brief  Erase EEPROM by writing zeros.
  * @param  None
  * @retval None
  */
void Erase_EEPROM(void)
{   
  uint32_t waddr = 0;
  
  ExitMutex = 0;
  EraseMutex = 1;
   
  /* LCD Display */
  LCD_DisplayStringLine(Line1, (uint8_t*)"                    ");
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line2, (uint8_t*)"   Erasing EEPROM   ");
  LCD_DisplayStringLine(Line3, (uint8_t*)"  Wait Until Erase  ");
  LCD_DisplayStringLine(Line4, (uint8_t*)"  Operation finish  ");
  LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
  
  while ((waddr < EEPROM_SIZE) && (sEE_DevStructure.sEEState == sEE_STATE_IDLE))
  {  
    /* Erase EEPROM */
    sEE_WriteBuffer(&sEE_DevStructure, (uint8_t*)buf, waddr, EEPROM_PAGE_SIZE) ;
    
    while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR));
    
    waddr += EEPROM_PAGE_SIZE;
    
    /* Clear IOE Pending IT */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
    IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);
    IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
    IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
#else
    IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);    
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */    
  }
  
  if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
  {
    sEE_DevStructure.sEEState = sEE_STATE_IDLE;
    LCD_DisplayStringLine(Line3, (uint8_t*)"  Erase Operation   ");
    LCD_DisplayStringLine(Line4, (uint8_t*)"       Failed       ");
  }
  else
  {
    LCD_DisplayStringLine(Line3, (uint8_t*)"   Erase Operation  ");
    LCD_DisplayStringLine(Line4, (uint8_t*)"     is Complete    ");
  }
  
  ExitMutex = 0;
  LCD_DisplayStringLine(Line5, (uint8_t*)"  Press Key to Exit ");
  while(ExitMutex == 0);
  
  /* Show Menu */
  Display_Menu();  
  
  /* update variable */
  sEEWAddr = 0;
  sEERAddr = 0;
  WReportNum = 1;
  EraseMutex = 0;
}

/**
  * @brief  Read Report from EEPROM .
  * @param  None
  * @retval None
  */
void Get_Report(void)
{   
    GetMutex = 1;
    
   /* If EEPROM Read Address reaches EEPROM size, reset this Address to 0 */
    if ((sEERAddr + 10) > EEPROM_SIZE)
    {
      sEERAddr = 0;     
    }  
    
    /* LCD Display */
    LCD_DisplayStringLine(Line1, (uint8_t*)"                    ");
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(Line2, (uint8_t*)"                    ");
    LCD_DisplayStringLine(Line3, (uint8_t*)"                    ");
    LCD_DisplayStringLine(Line4, (uint8_t*)"                    ");
    LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
    LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
    
    do
    {  
      /* Read report from EEPROM */
      sEE_ReadBuffer(&sEE_DevStructure, RBuffer, sEERAddr, sEENumData);
      
      while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR));
      
      if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
      {
        sEE_DevStructure.sEEState = sEE_STATE_IDLE;
      }
      
      NextRep = 0;
      NoRep = 0;
      ShowMutex = 0;
      
      /* Clear IOE Pending IT */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
      IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);
      IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
      IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
#else
      IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);    
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */
      
      ExitMutex = 0;
      
      /* if a report is read from EEPROM */
      if (*RBuffer == 0xA5)
      {
        CurrentReportNum = *(RBuffer+1);
        CurrentReportNum = CurrentReportNum<<8;
        CurrentReportNum|= *(RBuffer+2);
        
        sprintf((char*)Message, "Report Number :%d    ", CurrentReportNum );
        LCD_DisplayStringLine(Line2, Message);
        
        do
        {       
          if (ShowMutex == 0)
          {
            /* Display report */
            Report_Display(RBuffer+3);
            LCD_DisplayStringLine(Line5, (uint8_t*)" Sel  : Next Report ");
            LCD_DisplayStringLine(Line6, (uint8_t*)" Key  : Exit menu   ");
            ShowMutex = 1;
          }      
        }while((ExitMutex == 0) && (NextRep == 0));
      }
      else
      {
        LCD_SetBackColor(White);
        LCD_DisplayStringLine(Line1, (uint8_t*)"                    ");
        LCD_SetBackColor(Blue);
        LCD_SetTextColor(Black);
        LCD_DisplayStringLine(Line2, (uint8_t*)"                    ");
        LCD_DisplayStringLine(Line3, (uint8_t*)"  No New Reports ! ");
        LCD_DisplayStringLine(Line4, (uint8_t*)"                    ");
        LCD_DisplayStringLine(Line5, (uint8_t*)" Press Key to Exit! ");
        LCD_SetBackColor(White);
        LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
        
        NoRep = 1;
        ShowMutex = 1;
        while(ExitMutex == 0);
      }
      
      if((NextRep == 1) && (NoRep == 0))
      {
        /* If EEPROM Read Address reaches EEPROM size, reset this Address to 0 */ 
        if ((sEERAddr + 10) > EEPROM_SIZE)
        {
          sEERAddr = 0;  
        } 
        else
        {    
          sEERAddr += 10;
        }
      }  
      
    }while(NextRep == 1);
    
    GetMutex = 0; 
    
    /* Display Menu */
    Display_Menu();  
}

/**
  * @brief  Write Report in EEPROM .
  * @param  None
  * @retval None
  */
void Save_Report(void)
{ 
  GetMutex = 1;
  
  /*  if Erase operation is not on going */
  if (EraseMutex == 0)
  {
    /* If EEPROM Write Address reaches EEPROM size, reset this Address to 0 */ 
    if ((sEEWAddr + 10) > EEPROM_SIZE)
    {
      sEEWAddr = 0;  
      WReportNum = 1;
    }
    
    *(WBuffer)= 0xA5;
    *(WBuffer+1)= (uint8_t)((WReportNum & 0xFF00)>>8) ;
    *(WBuffer+2)= (uint8_t)(WReportNum & 0x00FF);

    
    /* Read time from RTC */  
#ifdef USE_STM322xG_EVAL
    Get_Time();
#else
    Get_Time(RTC_GetCounter());
#endif /* USE_STM322xG_EVAL */
        
    /* Read Temperature */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
    Temperature = IOE_TempSens_GetData (); 
    
    /* Get temperature value in °C */
    Temperature = Temperature - 273;
#else
    Temperature = LM75_ReadTemp();  
    
    if (Temperature <= 256)
    {
      /* Get temperature value in °C */
      Temperature >>= 1;
    }
    else
    {
      Temperature = 0;
    }
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */
    
    /* Affect report to Buffer */
    *(WBuffer+3) = (uint8_t)TimeStruct.hour_h;
    *(WBuffer+4) = (uint8_t)TimeStruct.hour_l;
    *(WBuffer+5) = (uint8_t)TimeStruct.min_h;
    *(WBuffer+6) = (uint8_t)TimeStruct.min_l;
    *(WBuffer+7) = (uint8_t)TimeStruct.sec_h;
    *(WBuffer+8) = (uint8_t)TimeStruct.sec_l;
    *(WBuffer+9) = (uint8_t)Temperature;
    
    Delay(0xFF);
    
    /* Write report in EEPROM */
    sEE_WriteBuffer(&sEE_DevStructure, WBuffer, sEEWAddr, sEENumData); 
    
    while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR));
    
    if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
    {
      sEE_DevStructure.sEEState = sEE_STATE_IDLE;
    }
    else
    {
      LCD_SetBackColor(White);
      LCD_SetTextColor(Blue);
      LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
    }
    
    /* Clear IOE Pending IT */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
    IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);
    IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
    IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
#else
    IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);    
#endif  /* USE_STM3210C_EVAL */
    
    WReportNum ++;
    sEEWAddr += 10;  
    GetMutex = 0;
  }
}                              

/**
  * @brief  Show temperature value 
  * @param  None
  * @retval None
  */
void Show_Temperature(void)
{ 
  ExitMutex = 0;
  
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
  Temperature = IOE_TempSens_GetData ();      
  
  /* Get temperature value in °C */
  Temperature = Temperature - 273;
#else
  Temperature = LM75_ReadTemp();   
  
  if (Temperature <= 256)
  {
    /* Get temperature value in °C */
    Temperature >>= 1;
  }
  else
  {
    Temperature = 0;
  }
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL*/
  
  /* Clear IOE Pending IT */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
  IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);
  IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
  IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
#else
  IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);    
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */
  
  
  sprintf((char*)Message, "  Temperature =  %d  ",(int)Temperature);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line1, (uint8_t*)"                    ");
  LCD_SetBackColor(Blue);
  LCD_DisplayStringLine(Line2, (uint8_t*)"Current Temperature ");
  LCD_DisplayStringLine(Line3, (uint8_t*)"                    ");
  LCD_DisplayStringLine(Line4, Message);
  LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);
  LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
  
  DisplayTimeout = TIMEOUT_DISPLAY;
  
  while(ExitMutex == 0);
  
  Display_Menu();   
}

/**
  * @brief  Show time on LCD 
  * @param  None
  * @retval None
  */
void Show_Time(void)
{ 
  ExitMutex = 0;
  
  /* Read time from RTC */  
#ifdef USE_STM322xG_EVAL
  Get_Time();
#else
  Get_Time(RTC_GetCounter());
#endif /* USE_STM322xG_EVAL */
  
  /* Clear IOE Pending IT */
#if defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL)
  IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);
  IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
  IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
#else
  IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);    
#endif  /* USE_STM3210C_EVAL || USE_STM322xG_EVAL */
  
  sprintf((char*)Message, "      %d%d:%d%d:%d%d       ",TimeStruct.hour_h,TimeStruct.hour_l,TimeStruct.min_h,TimeStruct.min_l,TimeStruct.sec_h,TimeStruct.sec_l);
  LCD_SetTextColor(White);
  LCD_DisplayStringLine(Line1, (uint8_t*)"                    ");
  LCD_SetBackColor(Blue);
  LCD_DisplayStringLine(Line2, (uint8_t*)"    Current Time    ");
  LCD_DisplayStringLine(Line3, (uint8_t*)"                    ");
  LCD_DisplayStringLine(Line4, Message);
  LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);
  LCD_DisplayStringLine(Line6, (uint8_t*)"                    ");
  
  DisplayTimeout = TIMEOUT_DISPLAY;
  
  while(ExitMutex == 0);
  
  Display_Menu(); 
}

/**
  * @brief  Set Time 
  * @param  None
  * @retval None
  */
static void Set_Time(void)
{  
#ifdef USE_STM322xG_EVAL
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_InitTypeDef RTC_InitStructure;
  uint32_t time_seconds;
  
  time_seconds = Calendar_TimeRegulate();
  
  RTC_TimeStruct.RTC_Hours = time_seconds / 3600;
  time_seconds = time_seconds % 3600;
  
  RTC_TimeStruct.RTC_Minutes = time_seconds / 60;
  time_seconds = time_seconds % 60;  
  
  RTC_TimeStruct.RTC_Seconds = time_seconds ;  
  
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
  
  /* Indicator for the RTC configuration */
  RTC_WriteBackupRegister(RTC_BKP_DR0,0x3220);
  
  /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv    = AsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv	=  SynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
  /* RTC init */
  RTC_Init(&RTC_InitStructure); 
#else
  RTC_SetCounter(Calendar_TimeRegulate());
  
  BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
#endif /* USE_STM322xG_EVAL */ 
}

/**
  * @brief  Get Time From RTC
  * @param  TimeVar : RTC counter value or None
  * @retval None
  */
#ifdef USE_STM322xG_EVAL
static void Get_Time(void)
#else
static void Get_Time(uint32_t TimeVar)
#endif /* USE_STM322xG_EVAL */
{     
#ifdef USE_STM322xG_EVAL
  RTC_TimeTypeDef RTC_TimeStruct;
  uint8_t NumHours = 0;  
  
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  
  NumHours = (uint8_t)(RTC_TimeStruct.RTC_Hours);
  
  /* get time hours */
  TimeStruct.hour_h = (uint8_t)(NumHours / 10);
  
  TimeStruct.hour_l=(uint8_t)(NumHours % 10);
  
  /* get time minutes */
  TimeStruct.min_h=(uint8_t)(RTC_TimeStruct.RTC_Minutes / 10);
  
  TimeStruct.min_l=(uint8_t)(RTC_TimeStruct.RTC_Minutes % 10);
  
  /* get time seconds */
  TimeStruct.sec_h=(uint8_t)(RTC_TimeStruct.RTC_Seconds / 10);
  
  TimeStruct.sec_l=(uint8_t)(RTC_TimeStruct.RTC_Seconds % 10);
  
#else
  
  /* if Time is 23:59:59 reset RTC */
  if(TimeVar >= 0x0001517F )
  { 
    RTC_SetCounter(0x0);
    RTC_WaitForLastTask();
    TimeVar = 0;
  }
  
  /* get time hours */
  TimeStruct.hour_h=(uint8_t)( TimeVar / 3600)/10;
  
  TimeStruct.hour_l=(uint8_t)(((TimeVar)/3600)%10);
  
  /* get time minutes */
  TimeStruct.min_h=(uint8_t)(((TimeVar)%3600)/60)/10;
  
  TimeStruct.min_l=(uint8_t)(((TimeVar)%3600)/60)%10;
  
  /* get time seconds */
  TimeStruct.sec_h=(uint8_t)(((TimeVar)%3600)%60)/10;
  
  TimeStruct.sec_l=(uint8_t)(((TimeVar)%3600)%60)%10;
#endif /* USE_STM322xG_EVAL */ 
  
}

/**
  * @brief  Show Report on LCD
  * @param  None
  * @retval None
  */
static void Report_Display(uint8_t* pBuff )
{    
  TimesStruct.hour_h = * pBuff;
  TimesStruct.hour_l = *(pBuff+1);
  TimesStruct.min_h  = *(pBuff+2);
  TimesStruct.min_l  = *(pBuff+3);
  TimesStruct.sec_h  = *(pBuff+4);
  TimesStruct.sec_l  = *(pBuff+5);
  Temperature        = *(pBuff+6);   
  
  sprintf((char*)Message, "  Temperature = %d  ", (int)Temperature);
  LCD_DisplayStringLine(Line3, Message);
  sprintf((char*)Message, "  Time  : %d%d:%d%d:%d%d  ",TimesStruct.hour_h,TimesStruct.hour_l,TimesStruct.min_h,TimesStruct.min_l,TimesStruct.sec_h,TimesStruct.sec_l);
  LCD_DisplayStringLine(Line4, Message);
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/**
  * @brief  Configure RTC with 1sec 
  * @param  None
  * @retval None
  */
static void RTC_Config(void)
{
#ifdef USE_STM322xG_EVAL
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  /* Exit initialization mode */
  RTC_ExitInitMode(); 
  
  /* If RTC is not configured */
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x3220)
  {    
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(Line3, (uint8_t*)"    Time  Config    ");
    LCD_DisplayStringLine(Line4, (uint8_t*)" Select : Press SEL ");
    LCD_DisplayStringLine(Line5, (uint8_t*)" Abort  : Press Key ");
    
    ReadKey = 1;
    
    while (1)
    {
      if (PressedKey == SEL)
      {
        ReadKey = 0;
        /* Adjust Time */
        Calendar_TimePreAdjust();
        return;
      }
      else if (PressedKey == KEY)
      {
        ReadKey = 0;
        return;
      }
    }    
  }
  
#else
  
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
  
  /* If RTC is not configured */
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    
    LCD_DisplayStringLine(Line3, (uint8_t*)"                    ");
    LCD_DisplayStringLine(Line4, (uint8_t*)"RTC Config PLZ Wait.");
    LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
    
    /* Reset Backup Domain */
    BKP_DeInit();
    
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}
    
    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    
    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
    LCD_SetBackColor(Blue);
    LCD_SetTextColor(White);
    LCD_DisplayStringLine(Line3, (uint8_t*)"    Time  Config    ");
    LCD_DisplayStringLine(Line4, (uint8_t*)" Select : Press SEL ");
    LCD_DisplayStringLine(Line5, (uint8_t*)" Abort  : Press Key ");
    
    ReadKey = 1;
    
    while (1)
    {
      if (PressedKey == SEL)
      {
        ReadKey = 0;
        /* Adjust Time */
        Calendar_TimePreAdjust();
        return;
      }
      else if (PressedKey == KEY)
      {
        ReadKey = 0;
        return;
      }
    } 
  }
#endif
}


/**
  * @brief  Returns the time entered by user, using demo board keys.
  * @param  None
  * @retval None
  */
static void Calendar_TimePreAdjust(void)
{   
  LCD_SetBackColor(White);
  LCD_SetTextColor(Red);
  LCD_DisplayStringLine(Line4, (uint8_t*)"    Adjust Time     ");
  LCD_DisplayStringLine(Line3, (uint8_t*)"                    ");
  LCD_DisplayStringLine(Line6, (uint8_t*)"        :  :        ");
  LCD_DisplayStringLine(Line5, (uint8_t*)"                    ");
  
  /* Change the current time */
  Set_Time();
  
  /* Clear Line7 */
  LCD_ClearLine(Line7);
  
  /* Clear Line8 */
  LCD_ClearLine(Line8);   
}

/**
  * @brief  Returns the time entered by user, using menu navigation keys.
  * @param  None
  * @retval Current time RTC counter value
  */
static uint32_t Calendar_TimeRegulate(void)
{
  uint32_t Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;
  
  /* Read time hours */
  Tmp_HH = Calendar_ReadDigit(223, TimeStruct.hour_h, 0x2, 0x0);  
  if (Tmp_HH == 2)
  {
    if (TimeStruct.hour_l > 3)
    {
      TimeStruct.hour_l = 0;
    }
    Tmp_HH = Tmp_HH * 10 + Calendar_ReadDigit(207, TimeStruct.hour_l, 0x3, 0x0);
  }
  else
  {
    Tmp_HH = Tmp_HH * 10 + Calendar_ReadDigit(207, TimeStruct.hour_l, 0x9, 0x0);
  }
  
  /* Read time  minutes */
  Tmp_MM = Calendar_ReadDigit(175, TimeStruct.min_h, 5, 0x0);
  Tmp_MM = Tmp_MM * 10 + Calendar_ReadDigit(159, TimeStruct.min_l, 0x9, 0x0);
  
  /* Read time seconds */
  Tmp_SS = Calendar_ReadDigit(127, TimeStruct.sec_h, 5, 0x0);
  Tmp_SS = Tmp_SS * 10 + Calendar_ReadDigit(111, TimeStruct.sec_l, 0x9, 0x0);
  
  /* Return the value to store in RTC counter register */
  return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}


/**
  * @brief  Reads digit entered by user, using menu navigation keys.
  * @param  None
  * @retval Digit value
  */
static uint8_t Calendar_ReadDigit(uint8_t ColBegin, uint8_t CountBegin, uint8_t ValueMax, uint8_t ValueMin)
{
  __IO uint32_t tmpValue = 0;
  
  PressedKey = NOKEY;
  
  /* Set the Text Color */
  LCD_SetTextColor(Red);
  
  /* Initialize tmpValue */
  tmpValue = CountBegin;
  
  /* Display  */
  LCD_DisplayChar(Line6, ColBegin, (tmpValue + 0x30));
  
  /* Endless loop */
  while (1)
  {  
    ReadKey = 1;
    
    /* If "UP" pushbutton is pressed */
    if (PressedKey == UP)
    {
      ReadKey = 0;
      PressedKey = NOKEY;
      
      /* Increase the value of the digit */
      if (tmpValue == ValueMax)
      {
        tmpValue = (ValueMin - 1);
      }
      
      /* Display new value */
      LCD_DisplayChar(Line6, ColBegin, ((++tmpValue) + 0x30));
    }
    
    /* If "DOWN" pushbutton is pressed */    
    if (PressedKey == DOWN)
    {
      ReadKey = 0;
      PressedKey = NOKEY;
      
      /* Decrease the value of the digit */
      if (tmpValue == ValueMin)
      {
        tmpValue = (ValueMax + 1);
      }
      
      /* Display new value */
      LCD_DisplayChar(Line6, ColBegin, ((--tmpValue) + 0x30));
    }
    
    /* If "SEL" pushbutton is pressed */
    if (PressedKey == SEL)
    {
      ReadKey = 0;
      PressedKey = NOKEY;
      
      /* Set the Back Color */
      LCD_SetBackColor(White);
      /* Set the Text Color */
      LCD_SetTextColor(Red);
      /* Display new value */
      LCD_DisplayChar(Line6, ColBegin, (tmpValue + 0x30));
      /* Return the digit value and exit */
      return tmpValue;
    }
  }
}


/**
  * @brief  Configures TIM7 and associated resources to generate an update interrupt each 50 ms.
  * @param  None
  * @retval None
  */
static void TIM7_Config(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* TIM7 is used to generate a periodic interrupt. in Each 20 interrupts a Report containing
  Temperature value and time is stored in EEPROM and in each one Leds are toggled */
  
  /* TIM7 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);
  
  /* Enable the TIM7 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure TIM7 to generate interrupt each 50ms */
  TIM_TimeBaseStructure.TIM_Period  = 50000;
#ifdef USE_STM322xG_EVAL
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/2000000)-1;
#else
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/1000000)-1; 
#endif /* USE_STM322xG_EVAL */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  
  /* TIM7 IT enable */
  TIM_ITConfig(TIM7, TIM_IT_Update , ENABLE);
  
  /* TIM7 enable counter */
  TIM_Cmd(TIM7, ENABLE);  
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
