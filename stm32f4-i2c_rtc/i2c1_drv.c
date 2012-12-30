/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    File name   : i2c1_drv.c
 *    Description : I2C1 Master mode driver
 *
 *    History :
 *    1. Date        : September 8, 2009
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
 **************************************************************************/
#define I2C1_DRV_GLOBAL
#include "i2c1_drv.h"
#include "stm32f4xx_i2c.h"

static boolean s_I2C_NotUsed;
static volatile boolean s_Done, s_Error;
static uint8 s_SlaveAddr;
static volatile uint32 s_Size;
static uint8* s_pData;

/*************************************************************************
 * Function Name: I2C1_ErrIntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: I2C1 error interrupt handler
 *
 *************************************************************************/
void I2C1_ER_IRQHandler(void)
{
  if(I2C_EVENT_SLAVE_ACK_FAILURE & I2C_GetLastEvent(I2C1))
  {
    // Generate Stop condition (return back to slave mode)
    I2C_GenerateSTOP(I2C1,ENABLE);
    I2C_ClearFlag(I2C1,I2C_FLAG_AF);
  }
  s_Done = true;
  s_Error = true;
}

/*************************************************************************
 * Function Name: I2C1_EvnIntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: I2C1 event interrupt handler
 *
 *************************************************************************/
void I2C1_EV_IRQHandler(void)
{
  switch (I2C_GetLastEvent(I2C1))
  {
  case I2C_EVENT_MASTER_MODE_SELECT:
    I2C_SendData(I2C1,s_SlaveAddr);
    if(0 == s_Size)
    {
      I2C_GenerateSTOP(I2C1,ENABLE);
      s_Done = true;
    }
    break;
  case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
  case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
    if(0 == s_Size)
    {
      s_Done = true;
    }
    else
    {
      --s_Size;
      I2C_SendData(I2C1,*s_pData++);
      if(0 == s_Size)
      {
        I2C_GenerateSTOP(I2C1,ENABLE);
      }
    }
    break;
  case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
    if (1 == s_Size)
    {
      I2C_AcknowledgeConfig(I2C1,DISABLE);
      I2C_GenerateSTOP(I2C1,ENABLE);
    }
    break;
  case I2C_EVENT_MASTER_BYTE_RECEIVED:
    if (1 == --s_Size)
    {
      I2C_GenerateSTOP(I2C1,ENABLE);
      I2C_AcknowledgeConfig(I2C1,DISABLE);
    }
    *s_pData++ = I2C_ReceiveData(I2C1);
    if (0 == s_Size)
    {
      s_Done = true;
    }
    break;
  }
}

/*************************************************************************
 * Function Name: I2C1_Init
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init I2C1 interface
 *
 *************************************************************************/
void I2C1_Init (void)
{
I2C_InitTypeDef  I2C_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

  s_I2C_NotUsed = true;

  // Enable clock

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_GPIOB, DISABLE);

  I2C_DeInit(I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  RCC_APB1PeriphClockCmd(  RCC_APB1Periph_I2C1, ENABLE);

  // I2C configuration
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_OwnAddress1 = 0xAA;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C1_SPEED;

  // I2C Peripheral Enable
  I2C_Cmd(I2C1, ENABLE);

  // Apply I2C configuration after enabling it
  I2C_Init(I2C1, &I2C_InitStructure);

  // Enable the I2C1 Events Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C1_INTR_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C1_INTR_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable the I2C1 Errors Interrupt
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = I2C1_INTR_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = I2C1_INTR_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable interrupts from I2C1 module
  I2C_ITConfig(I2C1, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR, ENABLE);
}

/*************************************************************************
 * Function Name: I2C1_Open
 * Parameters: none
 *
 * Return: boolean
 *
 * Description: Init I2C1 interface open
 *
 *************************************************************************/
boolean I2C1_Open (void)
{
  return(0 != AtomicExchange(false, &s_I2C_NotUsed));
}

/*************************************************************************
 * Function Name: I2C1_Close
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init I2C1 interface release
 *
 *************************************************************************/
void I2C1_Close (void)
{
  AtomicExchange(true, &s_I2C_NotUsed);
}

/*************************************************************************
 * Function Name: I2C1_DataTransfer
 * Parameters: Int8U SlaveAddr, pInt8U pData, Int32U Size
 *
 * Return: none
 *
 * Description: I2C1 transfer data to/from slave
 *
 *************************************************************************/
boolean I2C1_DataTransfer (uint8 SlaveAddr, uint8* pData, uint32 Size)
{
  s_Error = false;
  s_Done  = false;
  s_SlaveAddr = SlaveAddr;
  s_Size = Size;
  s_pData = pData;

  I2C_AcknowledgeConfig(I2C1,ENABLE);
  I2C_GenerateSTART(I2C1,ENABLE);

  while(false == s_Done);

  return(!s_Error);
}

