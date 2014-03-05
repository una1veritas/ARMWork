/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum{SHIELD_NOT_DETECTED = 0, SHIELD_DETECTED}ShieldStatus;

/* Private define ------------------------------------------------------------*/
#define SD_CARD_NOT_FOUND                        0
#define SD_CARD_NOT_FORMATTED                    1
#define SD_CARD_FILE_NOT_SUPPORTED               2
#define SD_CARD_OPEN_FAIL                        3
#define FATFS_NOT_MOUNTED                        4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint8_t BlinkSpeed =0, str[20];
__IO uint8_t JoystickValue = 0;
char* pDirectoryFiles[MAX_BMP_FILES];
FATFS microSDFatFs;

/* Private function prototypes -----------------------------------------------*/
static void SDCard_Config(void);
static void Menu(void);
static void LED2_Blink(void);
static void Display_Images(void);
static ShieldStatus TFT_ShieldDetect(void);
static void TFT_DisplayErrorMessage(uint8_t message);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{ 
  RCC_ClocksTypeDef RCC_Clocks;
  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  /* Check the availability of adafruit 1.8" TFT shield on top of STM32NUCLEO
     board. This is done by reading the state of IO PB.00 pin (mapped to JoyStick
     available on adafruit 1.8" TFT shield). If the state of PB.00 is high then
     the adafruit 1.8" TFT shield is available. */  
  if(TFT_ShieldDetect() == SHIELD_DETECTED)
  {
    /* Initialize SPI Interface to drive the LCD and uSD card available on
       adafruit 1.8" TFT shield. Must be done before LCD and uSD initialization */
    STM_SPI_Init();
    
    /* Initialize LCD mounted on adafruit 1.8" TFT shield */
    LCD_ADAFRUIT_Init(); 
    
    /* Configure SD card */
    SDCard_Config(); 
    
    /* Display on TFT Images existing on SD card */
    Display_Images();   
  }  
  else /* Shield not mounted */
  {   
    LED2_Blink();
  }  
}

/**
  * @brief  SD Card Configuration.
  * @param  None
  * @retval None
  */
static void SDCard_Config(void)
{
  uint32_t counter = 0;
  
  /* Initialize the SD mounted on adafruit 1.8" TFT shield.
     Before to call this function, the application code must call STM_SPI_Init()
     to initialize the SPI Interface used to drive the SD. */
  SD_ADAFRUIT_Init();
  
  /* Check the mounted device */
  if(f_mount(&microSDFatFs, (TCHAR const*)"/", 0) != FR_OK)
  {
    TFT_DisplayErrorMessage(FATFS_NOT_MOUNTED);
  }  
  else
  {
    /* Initialize the Directory Files pointers (heap) */
    for (counter = 0; counter < MAX_BMP_FILES; counter++)
    {
      pDirectoryFiles[counter] = malloc(11); 
    }
  }
}

/**
  * @brief  Displays demonstration menu.
  * @param  None
  * @retval None
  */
static void Menu(void)
{
  JOYState_TypeDef tmp;
  
  /* Set Text color */
  LCD_SetTextColor(LCD_COLOR_RED);
  /* Display message */
  LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)" NUCLEO-F103R8  ");
  LCD_DisplayStringLine(LCD_LINE_4, (uint8_t*)"     DEMO       ");
  
  /* Set Text color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  /* Display message */  
  LCD_DisplayStringLine(LCD_LINE_8,  (uint8_t*)"Display images  ");
  LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"stored under uSD");
  LCD_DisplayStringLine(LCD_LINE_12, (uint8_t*)"on TFT LCD      ");
  
  /* Set Text color */
  LCD_SetTextColor(LCD_COLOR_BLACK);
  /* Display message */ 
  LCD_DisplayStringLine(LCD_LINE_16, (uint8_t*)" Press JOY DOWN   ");
  LCD_DisplayStringLine(LCD_LINE_18, (uint8_t*)" to continue...   ");
 
  /* Wait for JOY_DOWN is pressed */
  while (STM_Get_JOYState() != JOY_DOWN)
  {
  }
  
  /* Set Text color */
  LCD_SetTextColor(LCD_COLOR_BLACK);  
  /* Display message */ 
  LCD_DisplayStringLine(LCD_LINE_6,  (uint8_t*)"                ");
  LCD_DisplayStringLine(LCD_LINE_8,  (uint8_t*)" Press Joystick ");
  
  /* Set Text color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  /* Display message */ 
  LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)" UP for:        ");
  LCD_DisplayStringLine(LCD_LINE_12, (uint8_t*)" Manual Mode    ");
  LCD_DisplayStringLine(LCD_LINE_14, (uint8_t*)" DOWN for:      ");
  LCD_DisplayStringLine(LCD_LINE_16, (uint8_t*)" Automatic Mode ");
  LCD_DisplayStringLine(LCD_LINE_18, (uint8_t*)"                ");
  
  /* Wait for JOY_DOWN or JOY_UP is pressed */
  tmp = JOY_RIGHT;
  while ((tmp != JOY_DOWN) && (tmp != JOY_UP))
  {
    tmp= STM_Get_JOYState();
  }
  
  /* LCD Clear */
  LCD_Clear(LCD_COLOR_WHITE); 
  
  /* JOY_UP is pressed */
  if(tmp == JOY_UP )
  {
    /* Set Text color */
    LCD_SetTextColor(LCD_COLOR_RED);    
    /* Display message */ 
    LCD_DisplayStringLine(LCD_LINE_4,   (uint8_t*)"  Manual Mode   ");
    LCD_DisplayStringLine(LCD_LINE_6,   (uint8_t*)"   Selected     "); 
    
    /* Set Text color */
    LCD_SetTextColor(LCD_COLOR_BLUE);      
    /* Display message */               
    LCD_DisplayStringLine(LCD_LINE_10,  (uint8_t*)"RIGHT=Next image"); 
    LCD_DisplayStringLine(LCD_LINE_12,  (uint8_t*)"LEFT =Previous  ");
    LCD_DisplayStringLine(LCD_LINE_16,  (uint8_t*)"SEL  =Switch to ");
    LCD_DisplayStringLine(LCD_LINE_18,  (uint8_t*)"automatic mode  ");    
    JoystickValue = 2;  
  }
  else /* JOY_DOWN is pressed */
  {
    /* Set Text color */
    LCD_SetTextColor(LCD_COLOR_RED);
    /* Display message */ 
    LCD_DisplayStringLine(LCD_LINE_4,   (uint8_t*)" Automatic Mode  ");
    LCD_DisplayStringLine(LCD_LINE_6,   (uint8_t*)"    Selected     ");
    
    /* Set Text color */
    LCD_SetTextColor(LCD_COLOR_BLUE);     
    /* Display message */ 
    LCD_DisplayStringLine(LCD_LINE_14,  (uint8_t*)" SEL  =Switch to ");
    LCD_DisplayStringLine(LCD_LINE_16,  (uint8_t*)" manual mode     "); 
    JoystickValue = 1;  
    Delay(2000);
  }
}

/**
  * @brief  Blinks LED2 with two frequencies depending on User press button.
  * @param  None
  * @retval None
  */
static void LED2_Blink(void)
{
  /* Configure LED2 on Nucleo */
  STM_EVAL_LEDInit(LED2);
  
  /* Initialize User button on Nucleo */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
  
  /* Initiate BlinkSpeed variable */ 
  BlinkSpeed = 0;  
  while(1)
  {
    /* Test on blink speed */
    if(BlinkSpeed == 0)
    {
      STM_EVAL_LEDToggle(LED2);
      /* Wait for 500ms */      
      Delay(500);      
    }      
    else if(BlinkSpeed == 1)
    {
      STM_EVAL_LEDToggle(LED2);
      /* Wait for 100ms */
      Delay(100); 
    }
    else if(BlinkSpeed == 2)
    {
      STM_EVAL_LEDToggle(LED2);    
      /* wait for 50ms */
      Delay(50);  
    }
  }
}

/**
  * @brief  Displays on TFT Images or error messages when error occurred.
  * @param  None
  * @retval None
  */
static void Display_Images(void)
{    
  uint32_t bmplen = 0x00;
  uint32_t checkstatus = 0x00;
  __IO uint8_t filesnumbers = 0;
  uint8_t joystickstatus = JOY_NONE;
  uint8_t bmpcounter = 0;
  DIR directory;
  FRESULT res;
  
  if(SD_GetStatus() != 0xFF)  
  {
    /* Display message: SD card does not exist */
    TFT_DisplayErrorMessage(SD_CARD_NOT_FOUND);
  }
  else
  {   
    /* Initialize ADC peripheral to detect motion of Joystick available on
    adafruit 1.8" TFT shield */
    STM_ADC_Config();
    
    /* Welcome message */
    Menu(); 
    
    /* Open directory */
    res= f_opendir(&directory, "/");
    if((res != FR_OK))
    {
      if(res == FR_NO_FILESYSTEM)
      {
        /* Display message: SD card not FAT formated */
        TFT_DisplayErrorMessage(SD_CARD_NOT_FORMATTED);    
      }
      else
      {
        /* Display message: Fail to open directory */
        TFT_DisplayErrorMessage(SD_CARD_OPEN_FAIL);  
      }
    }
    
    /* Get number of bitmap files */
    filesnumbers = Storage_GetDirectoryBitmapFiles ("/", pDirectoryFiles);    
    /* Set bitmap counter to display first image */
    bmpcounter = 1; 
    
    while (1)
    {     
      /* Get JoyStick status */    
      joystickstatus = STM_Get_JOYState();
      
      if(joystickstatus == JOY_SEL )
      {      
        JoystickValue++;
        if(JoystickValue == 3)
        {
          JoystickValue = 1;
        }
        joystickstatus = JOY_NONE;
      }
      if(JoystickValue == 1) /* Automatic display mode */
      {
        sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter -1]);
        
        checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
        
        if(checkstatus == 0)
        {
          /* Format the string */
          Storage_OpenReadFile(127, 159, (const char*)str); 
        }
        else if (checkstatus == 1)
        {
          /* Display message: SD card does not exist */
          TFT_DisplayErrorMessage(SD_CARD_NOT_FOUND);
        }
        else
        {
          /* Display message: File not supported */
          TFT_DisplayErrorMessage(SD_CARD_FILE_NOT_SUPPORTED); 
        }
        bmpcounter++;
        if(bmpcounter > filesnumbers)
        {
          bmpcounter = 1;
        }
      }      
      if(JoystickValue == 2) /* Manual display mode */
      {
        if( joystickstatus == JOY_RIGHT )
        {
          if((bmpcounter + 1) > filesnumbers)
          {
            bmpcounter = filesnumbers;
          }
          else
          {
            bmpcounter++;
            sprintf ((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter - 1]);
            
            checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
            
            if(checkstatus == 0)
            {
              /* Format the string */
              Storage_OpenReadFile(127, 159, (const char*)str); 
            }
            else if (checkstatus == 1)
            {
              /* Display message: SD CARD unplugged */
              TFT_DisplayErrorMessage(SD_CARD_NOT_FOUND);
            }
            else
            {
              /* Display message: File not supported */
              TFT_DisplayErrorMessage(SD_CARD_FILE_NOT_SUPPORTED); 
            }            
            joystickstatus = JOY_NONE;
            JoystickValue = 2;          
          }
        }
        else if(joystickstatus == JOY_LEFT )
        {
          if((bmpcounter - 1) <= 1)
          {
            bmpcounter = 1;
          }
          else
          {
            bmpcounter--;
            sprintf ((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter - 1]); 
            checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
            
            if(checkstatus == 0)
            {
              /* Format the string */
              Storage_OpenReadFile(127, 159, (const char*)str); 
            }
            else if (checkstatus == 1)
            {
              /* Display message: SD CARD unplugged */
              TFT_DisplayErrorMessage(SD_CARD_NOT_FOUND);
            }
            else
            {
              /* Display message: File not supported */
              TFT_DisplayErrorMessage(SD_CARD_FILE_NOT_SUPPORTED); 
            }            
          }
          joystickstatus = JOY_NONE;
          JoystickValue = 2;
        }
      }
    }  
  }
}

/**
  * @brief  Check the availability of adafruit 1.8" TFT shield on top of STM32NUCLEO
  *         board. This is done by reading the state of IO PB.00 pin (mapped to 
  *         JoyStick available on adafruit 1.8" TFT shield). If the state of PB.00 
  *         is high then the adafruit 1.8" TFT shield is available.
  * @note   This function should not be called after STM_ADC_Config(), otherwise 
  *         ADC conversion will not work.
  * @param  None
  * @retval SHIELD_DETECTED: 1.8" TFT shield is available
  *         SHIELD_NOT_DETECTED: 1.8" TFT shield is not available
  */
static ShieldStatus TFT_ShieldDetect(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) != SHIELD_NOT_DETECTED)
  {
    return SHIELD_DETECTED;
  }
  else
  {
    return SHIELD_NOT_DETECTED;
  }
}

/**
  * @brief  Displays adequate message on TFT available on adafruit 1.8" TFT shield  
  * @param  message: Error message to be displayed on the LCD.
  *   This parameter can be one of following values:   
  *     @arg SD_CARD_NOT_FOUND: SD CARD is unplugged
  *     @arg SD_CARD_NOT_FORMATTED: SD CARD is not FAT formatted
  *     @arg SD_CARD_FILE_NOT_SUPPORTED: File is not supported
  *     @arg SD_CARD_OPEN_FAIL: Failure to open directory
  *     @arg FATFS_NOT_MOUNTED: FatFs is not mounted
  * @retval None
  */
static void TFT_DisplayErrorMessage(uint8_t message)
{
  /* LCD Clear */
  LCD_Clear(LCD_COLOR_WHITE); 
  /* Set Text and Back colors */
  LCD_SetBackColor(LCD_COLOR_GREY); 
  LCD_SetTextColor(LCD_COLOR_RED);
  
  if(message == SD_CARD_NOT_FOUND)
  {
    /* Display message */ 
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"Please insert     ");
    LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)"                  "); 
    LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"an SD card and   ");
    LCD_DisplayStringLine(LCD_LINE_11, (uint8_t*)"                 "); 
    LCD_DisplayStringLine(LCD_LINE_12, (uint8_t*)"reset the board  "); 
    while(1)
    {
    } 
  }
  if(message == SD_CARD_NOT_FORMATTED)
  {
    /* Display message */
    LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"SD Card is not     ");
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"                   "); 
    LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)" FAT formatted     ");
    LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"                  ");   
    LCD_DisplayStringLine(LCD_LINE_11, (uint8_t*)" Please Format    ");
    LCD_DisplayStringLine(LCD_LINE_12, (uint8_t*)"                  "); 
    LCD_DisplayStringLine(LCD_LINE_13, (uint8_t*)"   the card       ");
    while (1)
    {
    }    
  }
  if(message == SD_CARD_FILE_NOT_SUPPORTED)
  {
    /* Display message */
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"File type not     ");
    LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)"                  ");
    LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"supported        ");
    while(1)
    {
    }    
  }
  if(message == SD_CARD_OPEN_FAIL)
  {
    /* Display message */
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"Open directory    ");
    LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)"                  "); 
    LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"     fails       ");
    while(1)
    {
    }     
  }
  if(message == FATFS_NOT_MOUNTED)
  {
    /* Display message */
    LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"Cannot mount FatFs");
    LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)"                  "); 
    LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)" on Drive        "); 
    while (1)
    {
    }    
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 1 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;
  
  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if(TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

