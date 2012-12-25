/**
  ******************************************************************************
  * @file    stm32f2xx_flash.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    10/15/2010
  * @brief   This file provides all the FLASH firmware functions.
  * 
  * This driver provides functions to configure and program the Flash memory of all STM32F2xx devices 
  * These functions are split in 3 groups:
  * 
  * 1/ FLASH_Interface configuration_functions, this group includes the management of following features:
  * - Set the latency
  * - Enable/Disable the prefetch buffer
  * - Enable/Disable the Instruction cache and the Data cache
  * - Reset the instruction cache and the Data cache
  * - Enable/Disable the flash interrupt sources.
  *  
  * 2/ FLASH_Memory Programming_functions, this group includes all needed functions
  * to erase and program the main memory:
  * - Lock and Unlock the Flash interface.
  * - Erase function: Erase sector, erase all sectors.
  * - Program functions: Byte, Half-word, word and double word.
  *  
  * 3/ FLASH_Option Bytes Programming_functions, this group includes all needed functions to:
  * - Set/Reset the write protection
  * - Set the Read protection Level
  * - Set the BOR level
  * - Program the user option Bytes: 
  * - Program the OTP Bytes
  * - Launch the Option Bytes loader
  *  
  * 4/ FLASH_FLAG management_Functions, this group includes all needed functions to
  * - Get flags status
  * - Clear flags
  * - Get Flash operation status
  * - Wait for last flash operation      
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
#include "stm32f2xx_flash.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */



/** @defgroup FLASH 
  * @brief FLASH driver modules
  * @{
  */ 

/** @defgroup FLASH_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup FLASH_Private_Defines
  * @{
  */ 
#define SECTOR_MASK               ((uint32_t)0xFFFFFF07)
/* Delay definition */   
#define ERASE_TIMEOUT             ((uint32_t)0xFFFFFFFF)
#define PROGRAM_TIMEOUT           ((uint32_t)0xFFFFFFFF)
/**
  * @}
  */ 


/** @defgroup FLASH_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup FLASH_Private_Variables
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup FLASH_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup FLASH_Private_Functions
  * @{
  */ 

/**
@code  
 *******************************************************************************
                   FLASH_Interface configuration_Functions
 *******************************************************************************  
   FLASH_Interface configuration_Functions, includes the following functions:
   - void FLASH_SetLatency(uint32_t FLASH_Latency):
     To correctly read data from Flash memory, the number of wait states (LATENCY) 
     must be correctly programmed according to the frequency of the CPU clock 
     (HCLK) and the supply voltage of the device.
 ---------------------------------------------------------------------------------     
 | Latency       |                HCLK clock frequency (MHz)                      |
 |               |----------------------------------------------------------------|     
 |               | voltage range  | voltage range | voltage range | voltage range |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.62 V - 2.1 V|
 |---------------|----------------|---------------|---------------|---------------|              
 |0WS(1CPU cycle)|0 < HCLK <= 30  |0 < HCLK <= 24 |0 < HCLK <= 18 |0 < HCLK <= 16 |
 |---------------|----------------|---------------|---------------|---------------|   
 |1WS(2CPU cycle)|30 < HCLK <= 60 |24 < HCLK <= 48|18 < HCLK <= 36|16 < HCLK <= 32| 
 |---------------|----------------|---------------|---------------|---------------|   
 |2WS(3CPU cycle)|60 < HCLK <= 90 |48 < HCLK <= 72|36 < HCLK <= 54|32 < HCLK <= 48|
 |---------------|----------------|---------------|---------------|---------------| 
 |3WS(4CPU cycle)|90 < HCLK <= 120|72 < HCLK <= 96|54 < HCLK <= 72|48 < HCLK <= 64|
 |***************|****************|***************|***************|****************|***************|
 |               | voltage range  | voltage range | voltage range | voltage range  | External Vpp  |
 |               | 2.7 V - 3.6 V  | 2.4 V - 2.7 V | 2.1 V - 2.4 V | 1.62 V - 2.1 V |     9 V       |
 |---------------|----------------|---------------|---------------|----------------|---------------| 
 |Max Parallelism|      x32       |     x16       |               x8               |      x64      |              
 |---------------|----------------|---------------|---------------|----------------|---------------|   
 |PSIZE[1:0]     |      10        |      01       |              00                |      11       |
 --------------------------------------------------------------------------------------------------     
   - void FLASH_PrefetchBufferCmd(FunctionalState NewState)
   - void FLASH_InstructionCacheCmd(FunctionalState NewState)
   - void FLASH_DataCacheCmd(FunctionalState NewState)
   - void FLASH_InstructionCacheReset(void)
   - void FLASH_DataCacheReset(void)
   - void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
   
   All these functions don't need the unlock sequence.   
@endcode
*/
 
/**
  * @brief  Sets the code latency value.
  * @param  FLASH_Latency: specifies the FLASH Latency value.
  *   This parameter can be one of the following values:
  *     @arg FLASH_Latency_0: FLASH Zero Latency cycle
  *     @arg FLASH_Latency_1: FLASH One Latency cycle
  *     @arg FLASH_Latency_2: FLASH Two Latency cycles
  *     @arg FLASH_Latency_3: FLASH Three Latency cycles
  * @retval None
  */
void FLASH_SetLatency(uint32_t FLASH_Latency)
{
  /* Check the parameters */
  assert_param(IS_FLASH_LATENCY(FLASH_Latency));
  
  /* Perform Byte access to FLASH_ACR[8:0] to set the Latency value */
  *(__IO uint8_t *)ACR_BYTE0_ADDRESS = (uint8_t)FLASH_Latency;
}

/**
  * @brief  Enables or disables the Prefetch Buffer.
  * @param  NewState: new state of the Prefetch Buffer.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FLASH_PrefetchBufferCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  /* Enable or disable the Prefetch Buffer */
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_PRFTEN;
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_PRFTEN);
  }
}

/**
  * @brief  Enables or disables the Instruction Cache feature.
  * @param  NewState: new state of the Instruction Cache.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FLASH_InstructionCacheCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_ICEN;
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_ICEN);
  }
}

/**
  * @brief   Enables or disables the Data Cache feature.
  * @param  NewState: new state of the Data Cache.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void FLASH_DataCacheCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if(NewState != DISABLE)
  {
    FLASH->ACR |= FLASH_ACR_DCEN;
  }
  else
  {
    FLASH->ACR &= (~FLASH_ACR_DCEN);
  }
}

/**
  * @brief  Resets the Instruction Cache.
  * @note  This function must be used only when the Instruction Cache is disabled.  
  * @param  None
  * @retval None
  */
void FLASH_InstructionCacheReset(void)
{
  FLASH->ACR |= FLASH_ACR_ICRST;
}

/**
  * @brief  Resets the Data Cache.
  * @note  This function must be used only when the Data Cache is disabled.  
  * @param  None
  * @retval None
  */
void FLASH_DataCacheReset(void)
{
  FLASH->ACR |= FLASH_ACR_DCRST;
}

/**
  * @brief  Enables or disables the specified FLASH interrupts.
  * @param  FLASH_IT: specifies the FLASH interrupt sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg FLASH_IT_ERROR: FLASH Error Interrupt
  *     @arg FLASH_IT_EOP: FLASH end of operation Interrupt
  * @retval None 
  */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FLASH_IT(FLASH_IT)); 
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if(NewState != DISABLE)
  {
    /* Enable the interrupt sources */
    FLASH->CR |= FLASH_IT;
  }
  else
  {
    /* Disable the interrupt sources */
    FLASH->CR &= ~(uint32_t)FLASH_IT;
  }
}

/**
@code  
 *******************************************************************************
                   FLASH_Memory Programming_functions
 *******************************************************************************  
   The FLASH Memory Programming functions, includes the following functions:
    - void FLASH_Unlock(void);
    - void FLASH_Lock(void);
    - FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector);
    - FLASH_Status FLASH_EraseAllSectors(void);
    - FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
    - FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
    - FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
    - FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data);
   
   Any operation of erase or program should follow these steps:
   - Call the FLASH_Unlock() function to enable the flash control register access
   - Call the desired function to erase sector (s) or program data
   - Call the FLASH_Lock() to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation)
    
@endcode
*/


/**
  * @brief  Unlocks the FLASH Registers access.
  * @param  None
  * @retval None
  */
void FLASH_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* Authorize the FLASH Registers access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
  }  
}

/**
  * @brief  Locks the FLASH Registers access.
  * @param  None
  * @retval None
  */
void FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
}

/**
  * @brief  Erases a specified FLASH Sector.
  * @note   - To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  *         - Call the FLASH_Lock() to disable the flash control register access 
  *          (recommended to protect the FLASH memory against possible unwanted operation)  
  * @param  FLASH_Sector: The Sector number to be erased.
  *   This parameter can be a value between FLASH_Sector_0 and FLASH_Sector_11 
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_SECTOR(FLASH_Sector));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  { 
    /* if the previous operation is completed, proceed to erase the sector */
    FLASH->CR &= SECTOR_MASK;
    FLASH->CR |= FLASH_CR_SER | FLASH_Sector;
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);
    
    /* if the erase operation is completed, disable the SER Bit */
    FLASH->CR &= (~FLASH_CR_SER);
    FLASH->CR &= SECTOR_MASK; 
  }
  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Erases all FLASH Sectors.
  * @note   To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_EraseAllSectors(void)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to erase all sectors */
     FLASH->CR |= FLASH_CR_MER;
     FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);

    /* if the erase operation is completed, disable the MER Bit */
    FLASH->CR &= (~FLASH_CR_MER);

  }   
  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Programs a Double word at a specified address.
  * @note   - To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  *         - Call the FLASH_Lock() to disable the flash control register access 
  *          (recommended to protect the FLASH memory against possible unwanted operation) 
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT. 
  */
FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_DOUBLE_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint64_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a word at a specified address.
  * @note   - To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  *         - Call the FLASH_Lock() to disable the flash control register access 
  *          (recommended to protect the FLASH memory against possible unwanted operation) 
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint32_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a half word at a specified address.
  * @note   - To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  *         - Call the FLASH_Lock() to disable the flash control register access 
  *          (recommended to protect the FLASH memory against possible unwanted operation)    
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_HALF_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint16_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Programs a byte at a specified address.
  * @note   - To correctly run this function, the FLASH_Unlock() function
  *           must be called before.
  *         - Call the FLASH_Lock() to disable the flash control register access 
  *          (recommended to protect the FLASH memory against possible unwanted operation)    
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_ADDRESS(Address));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_BYTE;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint8_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);

  } 
  /* Return the Program Status */
  return status;
}

/**
@code  
 ******************************************************************************* 
                   FLASH_Option Bytes Programming_functions
 *******************************************************************************  
   The FLASH_Option Bytes Programming_functions, includes the following functions:
   - void FLASH_OB_Unlock(void);
   - void FLASH_OB_Lock(void);
   - void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState);
   - void FLASH_OB_RDPConfig(uint8_t OB_RDP);
   - void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY);
   - void FLASH_OB_BORConfig(uint8_t OB_BOR);
   - FLASH_Status FLASH_ProgramOTP(uint32_t Address, uint32_t Data);							
   - FLASH_Status FLASH_OB_Launch(void);							
   - uint32_t FLASH_OB_GetUser(void);							
   - uint8_t FLASH_OB_GetWRP(void);							
   - uint8_t FLASH_OB_GetRDP(void);							
   - uint8_t FLASH_OB_GetBOR(void);
   
   Any operation of erase or program should follow these steps:
   1/ Call the FLASH_OB_Unlock() function to enable the Flash option control register access
   2/ Call one or several functions to program the desired option bytes 
      - void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState) => to Enable/Disable 
        the desired sector write protection
      - void FLASH_OB_RDPConfig(uint8_t OB_RDP) => to set the desired read Protection Level
      - void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY) => to configure 
        the user option Bytes: IWDG, STOP and the Standby.
      - void FLASH_OB_BORConfig(uint8_t OB_BOR) => to Set the BOR level 
      - FLASH_Status FLASH_ProgramOTP(uint32_t Address, uint32_t Data) => to program the OTP bytes			 
   3/ Once all needed option bytes to be programmed are correctly written, call the
     FLASH_OB_Launch(void) function to launch the Option Bytes programming process.
     Note: When changing the WDG mode from HW to SW or from SW to HW 
     a system reset is needed to make the change effective.  
   4/ Call the FLASH_OB_Lock() to disable the Flash option control register access (recommended
      to protect the option Bytes against possible unwanted operations)
    
@endcode
*/

/**
  * @brief  Unlocks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Unlock(void)
{
  if((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != RESET)
  {
    /* Authorizes the Option Byte register programming */
    FLASH->OPTKEYR = FLASH_OPT_KEY1;
    FLASH->OPTKEYR = FLASH_OPT_KEY2;
  }  
}

/**
  * @brief  Locks the FLASH Option Control Registers access.
  * @param  None
  * @retval None
  */
void FLASH_OB_Lock(void)
{
  /* Set the OPTLOCK Bit to lock the FLASH Option Byte Registers access */
  FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
}

/**
  * @brief  Enables or disables the write protection of the desired sectors
  * @note   - To correctly program the WRP option bytes, the FLASH_OB_Unlock() function
  *         must be called before this function.
  *         - Once the WRP option bytes are correctly written, the FLASH_OB_Launch ()
  *         must be called to load the new option bytes.
  *         - Call the FLASH_OB_Lock() to disable the Flash option control register access
  *          (recommended to protect the option Bytes against possible unwanted operations)  
  * @param  OB_WRP: specifies the address of the sectors to be write
  *   protected or unprotected. This parameter can be:
  *     @arg A value between OB_WRP_Sector0 and OB_WRP_Sector11                      
  *     @arg OB_WRP_AllSector
  * @param  Newstate: new state of the Write Protection.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
void FLASH_OB_WRPConfig(uint32_t OB_WRP, FunctionalState NewState)
{ 
  FLASH_Status status = FLASH_COMPLETE;
  
  /* Check the parameters */
  assert_param(IS_OB_WRP(OB_WRP));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
    
  status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);

  if(status == FLASH_COMPLETE)
  { 
    if(NewState != DISABLE)
    {
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS &= (~OB_WRP);
    }
    else
    {
      *(__IO uint16_t*)OPTCR_BYTE2_ADDRESS |= (uint16_t)OB_WRP;
    }
  }
}

/**
  * @brief  Sets the read protection level.
  * @note   - To correctly program the RDP level, the FLASH_OB_Unlock() function
  *         must be called before this function.
  *         - Once the RDP level is correctly written, the FLASH_OB_Launch ()
  *         must be called to load the new option bytes.
  *         - Call the FLASH_OB_Lock() to disable the Flash option control register access
  *          (recommended to protect the option Bytes against possible unwanted operations)  
  * @param  OB_RDP: specifies the read protection level.
  *   This parameter can be:
  *     @arg OB_RDP_Level_0: No protection
  *     @arg OB_RDP_Level_1: Read protection of the memory
  *     @arg OB_RDP_Level_2: Full chip protection 
  *      !!!Warning!!! When enabling OB_RDP level 2 it's no more possible
  *                    to go back to level 1 or 0 
  * @retval None
  */
void FLASH_OB_RDPConfig(uint8_t OB_RDP)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_OB_RDP(OB_RDP));

  status = FLASH_WaitForLastOperation(ERASE_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    *(__IO uint8_t*)OPTCR_BYTE1_ADDRESS = OB_RDP;

  }
}

/**
  * @brief  Programs the FLASH User Option Byte: IWDG_SW / RST_STOP / RST_STDBY.
  * @note   - To correctly program the USER option bytes, the FLASH_OB_Unlock() function
  *         must be called before this function.
  *         - Once the USER option bytes are correctly written, the FLASH_OB_Launch ()
  *         must be called to load the new option bytes.
  *         - Call the FLASH_OB_Lock() to disable the Flash option control register access
  *          (recommended to protect the option Bytes against possible unwanted operations)         
  * @param  OB_IWDG: Selects the IWDG mode
  *   This parameter can be one of the following values:
  *     @arg OB_IWDG_SW: Software IWDG selected
  *     @arg OB_IWDG_HW: Hardware IWDG selected
  * @param  OB_STOP: Reset event when entering STOP mode.
  *   This parameter can be one of the following values:
  *     @arg OB_STOP_NoRST: No reset generated when entering in STOP
  *     @arg OB_STOP_RST: Reset generated when entering in STOP
  * @param  OB_STDBY: Reset event when entering Standby mode.
  *   This parameter can be one of the following values:
  *     @arg OB_STDBY_NoRST: No reset generated when entering in STANDBY
  *     @arg OB_STDBY_RST: Reset generated when entering in STANDBY
  * @retval None
  */
void FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP, uint8_t OB_STDBY)
{
  uint8_t optiontmp = 0xFF;
  FLASH_Status status = FLASH_COMPLETE; 
  /* Check the parameters */
  assert_param(IS_OB_IWDG_SOURCE(OB_IWDG));
  assert_param(IS_OB_STOP_SOURCE(OB_STOP));
  assert_param(IS_OB_STDBY_SOURCE(OB_STDBY));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  { 
    optiontmp = (uint8_t)(OB_IWDG | OB_STDBY | OB_STOP)|((*(__IO uint8_t *)OPTCR_BYTE0_ADDRESS) & 0x0F); 
    /* Enable the Option Bytes Programming operation */
    *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS = optiontmp; 

  }  
}

/**
  * @brief  Sets the BOR Level.
  * @note   - To correctly program the BOR level, the FLASH_OB_Unlock() function
  *         must be called before this function.
  *         - Once the BOR level is correctly written, the FLASH_OB_Launch ()
  *         must be called to load the new option bytes.
  *         - Call the FLASH_OB_Lock() to disable the Flash option control register access
  *          (recommended to protect the option Bytes against possible unwanted operations)   
  * @param  OB_BOR: specifies the Option Bytes BOR Reset Level.
  *   This parameter can be one of the following values:
  *     @arg OB_BOR_Level_3: FLASH BOR Level 3 : Supply voltage ranges from 2.70 to 3.60 V
  *     @arg OB_BOR Level_2: FLASH BOR Level 2 : Supply voltage ranges from 2.40 to 2.70 V
  *     @arg OB_BOR Level_1: FLASH BOR Level 1 : Supply voltage ranges from 2.10 to 2.40 V
  *     @arg OB_BOR Level_Off: FLASH BOR Off : Supply voltage ranges from 1.62 to 2.10 V.
  * @retval None
  */
void FLASH_OB_BORConfig(uint8_t OB_BOR)
{
  /* Check the parameters */
  assert_param(IS_OB_BOR(OB_BOR));

  /* Set the BOR Level */
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS &= (~FLASH_OPTCR_BOR_LEV);
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS |= OB_BOR;

}

/**
  * @brief  Programs a word at a specified address of OTP.
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramOTP(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_FLASH_OTP_ADDRESS(Address));
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;
  
    *(__IO uint32_t*)Address = Data;
        
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

    /* if the program operation is completed, disable the PG Bit */
    FLASH->CR &= (~FLASH_CR_PG);
  } 
  /* Return the Program Status */
  return status;
}

/**
  * @brief  Launch the option byte loading.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION, 
  *   FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_Launch(void)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Set the OPTSTRT bit in OPTCR register */
  *(__IO uint8_t *)OPTCR_BYTE0_ADDRESS |= FLASH_OPTCR_OPTSTRT;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);

  return status;
}

/**
  * @brief  Returns the FLASH User Option Bytes values.
  * @param  None
  * @retval The FLASH User Option Bytes values:IWDG_SW(Bit0), RST_STOP(Bit1)
  *   and RST_STDBY(Bit2).
  */
uint8_t FLASH_OB_GetUser(void)
{
  /* Return the User Option Byte */
  return (uint8_t)(FLASH->OPTCR >> 5);
}

/**
  * @brief  Returns the FLASH Write Protection Option Bytes value.
  * @param  None
  * @retval The FLASH Write Protection  Option Bytes value
  */
uint16_t FLASH_OB_GetWRP(void)
{
  /* Return the Flash write protection Register value */
  return (*(__IO uint16_t *)(OPTCR_BYTE2_ADDRESS));
}

/**
  * @brief  Returns the FLASH Read Protection level.
  * @param  None
  * @retval FLASH ReadOut Protection Status:
  *  - SET(when OB_RDP_Level_1 is set)
  *  - RESET(when OB_RDP_Level_0 is set)
  */
FlagStatus FLASH_OB_GetRDP(void)
{
  FlagStatus readstatus = RESET;
  if ((*(__IO uint8_t*)(OPTCR_BYTE1_ADDRESS) != (uint8_t)OB_RDP_Level_0))
  {
    readstatus = SET;
  }
  else
  {
    readstatus = RESET;
  }
  return readstatus;
}

/**
  * @brief  Returns the FLASH BOR level.
  * @param  None
  * @retval The FLASH BOR level
  */
uint8_t FLASH_OB_GetBOR(void)
{
  /* Return the Falsh BOR level */
  return (((*(__IO uint8_t *)(OPTCR_BYTE0_ADDRESS) & 0x0C) >> 2));
}

/**
  * @brief  Checks whether the specified FLASH flag is set or not.
  * @param  FLASH_FLAG: specifies the FLASH flag to check.
  *   This parameter can be one of the following values:
  *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag 
  *     @arg FLASH_FLAG_OPERR: FLASH operation Error flag 
  *     @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag 
  *     @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag
  *     @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *     @arg FLASH_FLAG_PGSERR: FLASH Programming Sequence error flag
  *     @arg FLASH_FLAG_BSY: FLASH Busy flag
  * @retval The new state of FLASH_FLAG (SET or RESET).
  */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_FLASH_GET_FLAG(FLASH_FLAG));
  if((FLASH->SR & FLASH_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the new state of FLASH_FLAG (SET or RESET) */
  return bitstatus; 
}

/**
  * @brief  Clears the FLASH’s pending flags.
  * @param  FLASH_FLAG: specifies the FLASH flags to clear.
  *   This parameter can be any combination of the following values:
  *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag 
  *     @arg FLASH_FLAG_OPERR: FLASH operation Error flag 
  *     @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag 
  *     @arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag 
  *     @arg FLASH_FLAG_PGPERR: FLASH Programming Parallelism error flag
  *     @arg FLASH_FLAG_PGSERR: FLASH Programming Sequence error flag
  * @retval None
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
  /* Check the parameters */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG));
  
  /* Clear the flags */
  FLASH->SR = FLASH_FLAG;
}

/**
  * @brief  Returns the FLASH Status.
  * @param  None
  * @retval FLASH Status: The returned value can be: FLASH_BUSY, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_ERROR_OPERATION
     or FLASH_COMPLETE
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    if(FLASH->SR & FLASH_FLAG_WRPERR)
    { 
      flashstatus = FLASH_ERROR_WRP;
    }
    else 
    {
      if((FLASH->SR & (uint32_t)0xEF) != (uint32_t)0x00)
      {
        flashstatus = FLASH_ERROR_PROGRAM; 
      }
      else
      {
        if(FLASH->SR & FLASH_FLAG_OPERR)
        {
          flashstatus = FLASH_ERROR_OPERATION;
        }
        else
        {
          flashstatus = FLASH_COMPLETE;
        }
      }
    }
  }
  /* Return the Flash Status */
  return flashstatus;
}

/**
  * @brief  Waits for a Flash operation to complete or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_OPERATION, 
  *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the Flash Status */
  status = FLASH_GetStatus();
  /* Wait for a Flash operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetStatus();
    Timeout--;
  }
  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

/**
  * @}
  */ 


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
