/**
  ******************************************************************************
  * @file    SPI/SPI_EEPROM/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_EEPROM
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
/* Uncomment the following line to enable using LCD screen for messages display */
#define ENABLE_LCD_MSG_DISPLAY

#define sEE_WRITE_ADDRESS1        0x50
#define sEE_READ_ADDRESS1         0x50
#define BUFFER_SIZE1             (countof(Tx1Buffer)-1)
#define BUFFER_SIZE2             (countof(Tx2Buffer)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1Buffer[] = "/* STM32F30x SPI Firmware Library EEPROM driver example: \
                        This firmware provides a basic example of how to use the SPI firmware library and\
                        an associate SPI EEPROM driver to communicate with an SPI EEPROM device \
                        SPI peripheral is configured in Master Mode during write operation and \
                        read operation from SPI EEPROM.*/";

uint8_t Tx2Buffer[] = "/* STM32F30x SPI Firmware Library EEPROM driver example : \
                        SPI2 is interfacing with M95M01 EEPROM */";

uint8_t Rx1Buffer[BUFFER_SIZE1], Rx2Buffer[BUFFER_SIZE2]; 
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint32_t NumDataRead = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */ 
  
  /* Initialize the SPI EEPROM driver ----------------------------------------*/
  sEE_Init();  

  /* First write in the memory followed by a read of the written data --------*/
  /* Write on SPI EEPROM from sEE_WRITE_ADDRESS1 */
  sEE_WriteBuffer(Tx1Buffer, sEE_WRITE_ADDRESS1, BUFFER_SIZE1); 

  /* Wait for EEPROM standby state */
  sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE1;
  
  /* Read from SPI EEPROM from sEE_READ_ADDRESS1 */
  sEE_ReadBuffer(Rx1Buffer, sEE_READ_ADDRESS1, (uint16_t *)(&NumDataRead)); 
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus1 = Buffercmp(Tx1Buffer, Rx1Buffer, BUFFER_SIZE1);

  /* Second write in the memory followed by a read of the written data -------*/
  /* Write on SPI EEPROM from sEE_WRITE_ADDRESS2 */
  sEE_WriteBuffer(Tx2Buffer, sEE_WRITE_ADDRESS2, BUFFER_SIZE2); 

  /* Wait for EEPROM standby state */
  sEE_WaitEepromStandbyState();  
  
  /* Set the Number of data to be read */
  NumDataRead = BUFFER_SIZE2;  
  
  /* Read from SPI EEPROM from sEE_READ_ADDRESS2 */
  sEE_ReadBuffer(Rx2Buffer, sEE_READ_ADDRESS2, (uint16_t *)(&NumDataRead));
  
  /* Check if the data written to the memory is read correctly */
  TransferStatus2 = Buffercmp(Tx2Buffer, Rx2Buffer, BUFFER_SIZE2);

#ifdef ENABLE_LCD_MSG_DISPLAY  
  /* Initialize the LCD screen for information display */
  STM32303C_LCD_Init();
  LCD_Clear(LCD_COLOR_BLUE);  
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, "SMT32F30x FW Library");
  LCD_DisplayStringLine(LCD_LINE_1, "   EEPROM Example   ");
  
  /* TransferStatus1 = PASSED, if the transmitted and received data 
  to/from the EEPROM are the same */
  /* TransferStatus1 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */
  if (TransferStatus1 == PASSED)
  {
    LCD_DisplayStringLine(LCD_LINE_3, " Transfer 1 PASSED  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_3, " Transfer 1 FAILED  ");
  } 
  
  /* TransferStatus2 = PASSED, if the transmitted and received data 
  to/from the EEPROM are the same */
  /* TransferStatus2 = FAILED, if the transmitted and received data 
  to/from the EEPROM are different */
  if (TransferStatus2 == PASSED)
  {
    LCD_DisplayStringLine(LCD_LINE_5, " Transfer 2 PASSED  ");
  }
  else
  {
    LCD_DisplayStringLine(LCD_LINE_5, " Transfer 2 FAILED  ");
  }  
#endif /* ENABLE_LCD_MSG_DISPLAY */
  
  /* Free all used resources */
  sEE_DeInit();

  while (1)
  {
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
