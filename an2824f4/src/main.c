/**
  ******************************************************************************
  * @file OptimizedI2Cexamples/src/main.c
  * @author  MCD Application Team
  * @version  V4.0.0
  * @date     06/18/2010
  * @brief  Main program body
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
#include "stm32f4xx.h"
#include "I2CRoutines.h"


/** @addtogroup Optimized I2C examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
/* Buffer of data to be received by I2C1 */
uint8_t Buffer_Rx1[255];
/* Buffer of data to be transmitted by I2C1 */
uint8_t Buffer_Tx1[255] = {0x5, 0x6,0x8,0xA};
/* Buffer of data to be received by I2C2 */
uint8_t Buffer_Rx2[255];
/* Buffer of data to be transmitted by I2C2 */
uint8_t Buffer_Tx2[255] = {0xF, 0xB, 0xC,0xD};
extern __IO uint8_t Tx_Idx1 , Rx_Idx1;
extern __IO uint8_t Tx_Idx2 , Rx_Idx2;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval : None
  */
int main(void)

{
    
      
    NVIC_Configuration();
    I2C_LowLevel_Init(I2C2);
    I2C_LowLevel_Init(I2C1);
  
/* Use I2C1 as Master which is communicating with I2C1 of another STM32F10x device */
while(1)
 {
     I2C_Master_BufferWrite(I2C1, Buffer_Tx1,120,Interrupt, 0x28);
     I2C_Master_BufferRead(I2C1,Buffer_Rx1,120,Polling, 0x28);
 } 
   
/* Use I2C1 as Slave */
/*! When using Slave with DMA, uncomment //#define SLAVE_DMA_USE in the stm32f10x_it.c file.*/
 /*I2C_Slave_BufferReadWrite(I2C1, DMA);

 while(1); */

}






/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval : None
  */
void NVIC_Configuration(void)
{

    /* 1 bit for pre-emption priority, 3 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_SetPriority(I2C1_EV_IRQn, 0x00); 
    NVIC_EnableIRQ(I2C1_EV_IRQn);

    NVIC_SetPriority(I2C1_ER_IRQn, 0x01); 
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    
    
    NVIC_SetPriority(I2C2_EV_IRQn, 0x00);
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    NVIC_SetPriority(I2C2_ER_IRQn, 0x01); 
    NVIC_EnableIRQ(I2C2_ER_IRQn);
 
}

#ifdef  USE_FULL_ASSERT


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
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


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
