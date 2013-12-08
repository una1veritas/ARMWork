/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : sd_card_mode.c
 *    Description : SD/MMC diver
 *
 *    History :
 *    1. Date        : 30, July 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
 **************************************************************************/
#define SD_CARD_MODE_GLOBAL
#include "sd_card_mode.h"

#define SDIO_STATIC_FLAGS              (0x000005FFUL)
#define SDIO_CMD0TIMEOUT               (0x00002710UL)
#define SDIO_FIFO_Address              (0x40012C80UL)

#define _CSD_GET_TRAN_SPEED_EXP()      (_MmcSdCsd[ 0]&0x07)
#define _CSD_GET_TRAN_SPEED_MANT()    ((_MmcSdCsd[ 0]&0xF8)>>3 )
#define _CSD_GET_NSAC()                (_MmcSdCsd[ 1]          )
#define _CSD_GET_TAAC_EXP()            (_MmcSdCsd[ 2]&0x7)
#define _CSD_GET_TAAC_MANT()          ((_MmcSdCsd[ 2]&0xF8)>>3 )
#define _CSD_GET_R2W_FACTOR()         ((_MmcSdCsd[15]&0x1C)>>2 )
#define _CSD_GET_READ_BL_LEN()         (_MmcSdCsd[ 6]&0x0F)
#define _CSD_GET_C_SIZE()            (((_MmcSdCsd[ 5]&0x03)<<10) + (_MmcSdCsd[4]<<2) + ((_MmcSdCsd[11]&0xc0)>>6))
#define _CSD_GET_C_SIZE_MULT()       (((_MmcSdCsd[10]&0x03)<<1 ) +((_MmcSdCsd[9]&0x80)>>7))
#define _CSD_GET_PERM_WRITE_PROTECT() ((_MmcSdCsd[13]&0x20)>>5 )
#define _CSD_GET_TMP_WRITE_PROTECT()  ((_MmcSdCsd[13]&0x10)>>4 )

#define _CSD_2_0_GET_C_SIZE()        (((_MmcSdCsd[4]&0x0F)<<16) + (_MmcSdCsd[11]<<8) + _MmcSdCsd[10])

const Int32U _SdTransfExp[] =
{
     10000UL,
    100000UL,
   1000000UL,
  10000000UL,
         0UL,
         0UL,
         0UL,
         0UL,
};

const Int32U _SdAccessTime [] =
{
        1UL,
       10UL,
      100UL,
     1000UL,
    10000UL,
   100000UL,
  1000000UL,
 10000000UL,
};

const Int32U _SdCsdMant[] =
{
  0UL,10UL,12UL,13UL,15UL,
  20UL,25UL,
  30UL,35UL,
  40UL,45UL,
  50UL,55UL,
  60UL,
  70UL,
  80UL,
};

const SdCommads_t _SdCmd[CMD_END] =
{
  // CMD0
  {0x00,SdNoArg     ,SdNoResp },
  // CMD1
  {0x01,SdOcr       ,SdR3 },
  // CMD2
  {0x02,SdNoArg     ,SdR2 },
  // CMD3
  {0x03,SdRelAddr   ,SdR1 },
  // CMD7
  {0x07,SdRelAddr   ,SdR1 },
  // CMD8
  {0x08,SdDataAdd   ,SdR7 },
  // CMD9
  {0x09,SdRelAddr   ,SdR2 },
  // CMD10
  {0x0A,SdRelAddr   ,SdR2 },
  // CMD12
  {0x0C,SdNoArg     ,SdR1b},
  // CMD13
  {0x0D,SdRelAddr   ,SdR1 },
  // CMD16
  {0x10,SdBlockLen  ,SdR1 },
  // CMD17
  {0x11,SdDataAdd   ,SdR1 },
  // CMD18
  {0x12,SdDataAdd   ,SdR1 },
  // CMD24
  {0x18,SdDataAdd   ,SdR1 },
  // CMD25
  {0x19,SdDataAdd   ,SdR1 },
  // CMD27
  {0x1B,SdOcr       ,SdR1 },
  // CMD28
  {0x1C,SdDataAdd   ,SdR1b},
  // CMD29
  {0x1D,SdDataAdd   ,SdR1b},
  // CMD30
  {0x1E,SdDataAdd   ,SdR1 },
  // CMD32
  {0x20,SdDataAdd   ,SdR1 },
  // CMD33
  {0x21,SdDataAdd   ,SdR1 },
  // CMD34
  {0x22,SdDataAdd   ,SdR1 },
  // CMD35
  {0x23,SdDataAdd   ,SdR1 },
  // CMD36
  {0x24,SdDataAdd   ,SdR1 },
  // CMD37
  {0x25,SdDataAdd   ,SdR1 },
  // CMD38
  {0x26,SdNoArg     ,SdR1b},
  // CMD42
  {0x2A,SdNoArg     ,SdR1b},
  // CMD55
  {0x37,SdRelAddr   ,SdR1 },
  // CMD56
  {0x38,SdNoArg     ,SdR1 },
  // ACMD46
  {0x06,SdDataAdd   ,SdR1 },
  // ACMD41
  {0x29,SdDataAdd   ,SdR3 },
};

DiskCtrlBlk_t _SdDskCtrlBlk;
Int32U _SdLastError,_Tnac,_Twr;
Boolean _bSdChanged;
#pragma data_alignment = 4
Int8U _MmcSdCsd[16];

static Int32U _CardRCA;
static Boolean _bSdPermWriteProtect;
static Boolean _bHC;

/*************************************************************************
 * Function Name: _SdPresent
 * Parameters: none
 * Return: Boolean - true cart present
 *                 - false cart no present
 *
 * Description: SD/MMC present check
 *
 *************************************************************************/
static
Boolean _SdPresent (void)
{
  return(Bit_SET == GPIO_ReadInputDataBit(SD_CP_PORT,SD_CP_PIN));
}

/*************************************************************************
 * Function Name: _SdWriteProtect
 * Parameters: none
 * Return: Boolean - true cart is protected
 *                 - false cart no protected
 *
 * Description: SD/MMC write protect status
 *
 *************************************************************************/
static
Boolean _SdWriteProtect (void)
{
  return(Bit_RESET == GPIO_ReadInputDataBit(SD_WP_PORT,SD_WP_PIN));
}

/*************************************************************************
 * Function Name: _SdSetClockFreq
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Set SD/MMC ckl frequency
 *
 *************************************************************************/
static
Int32U _SdSetClockFreq (Int32U Frequency)
{
RCC_ClocksTypeDef RCC_Clocks;
SDIO_InitTypeDef SDIO_InitStructure;
Int32U Div;

  RCC_GetClocksFreq(&RCC_Clocks);

  for(Div = 2; Div <= 256; ++Div)
  {
    if((Frequency * Div) > RCC_Clocks.PCLK2_Frequency)
    {
      break;
    }
  }

  SDIO_InitStructure.SDIO_ClockDiv = Div;
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Enable;
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);

  // Return real frequency
  return(RCC_Clocks.PCLK2_Frequency/Div);
}

/*************************************************************************
 * Function Name: _SdInit
 * Parameters: none
 * Return: none
 *
 * Description: Init MCI module and Cart Present and Write Protect pins
 *
 *************************************************************************/
static
void _SdInit (void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
  // GPIO enable clock 
  RCC_AHB1PeriphClockCmd(SD_CP_CLK | SD_WP_CLK | SD_CMD_CLK | SD_CLK_CLK | \
                         SD_D0_CLK | SD_D1_CLK | SD_D2_CLK | SD_D3_CLK, ENABLE);

  // Init CP pin
  GPIO_InitStructure.GPIO_Pin = SD_CP_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SD_CP_PORT, &GPIO_InitStructure);

  // Init WP pin
  GPIO_InitStructure.GPIO_Pin = SD_WP_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SD_WP_PORT, &GPIO_InitStructure);

  // Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin
  GPIO_InitStructure.GPIO_Pin = SD_D0_PIN | SD_D1_PIN | SD_D2_PIN | \
                                SD_D3_PIN | SD_CLK_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // Configure PD.02 CMD line
  GPIO_InitStructure.GPIO_Pin = SD_CMD_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SD_CMD_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(SD_CMD_PORT, SD_CMD_SOURCE, GPIO_AF_SDIO);
  GPIO_PinAFConfig(SD_CLK_PORT, SD_CLK_SOURCE, GPIO_AF_SDIO);
  GPIO_PinAFConfig(SD_D0_PORT, SD_D0_SOURCE, GPIO_AF_SDIO);
  GPIO_PinAFConfig(SD_D1_PORT, SD_D1_SOURCE, GPIO_AF_SDIO);
  GPIO_PinAFConfig(SD_D2_PORT, SD_D2_SOURCE, GPIO_AF_SDIO);
  GPIO_PinAFConfig(SD_D3_PORT, SD_D3_SOURCE, GPIO_AF_SDIO);

  /* Enable the SDIO AHB Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

  // Enable the DMA2 Clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  SDIO_DeInit();
}

/*************************************************************************
 * Function Name: _SdPowerDown
 * Parameters: none
 * Return: none
 *
 * Description: Set power down state
 *
 *************************************************************************/
static
void _SdPowerDown (void)
{
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  // Clock Freq. Identification Mode < 400kHz
  _SdSetClockFreq(IdentificationModeClock);
}

/*************************************************************************
 * Function Name: _SdSendCmd
 * Parameters: SdCmdInd_t ComdInd,Int32U Arg
 *
 * Return: SdState_t
 *
 * Description: SD/MMC commands implement
 *
 *************************************************************************/
static
SdState_t _SdSendCmd(SdCmdInd_t ComdInd,pInt32U pArg)
{
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
Int32U Status;
Int32U timeout;
  // Send Command
  SDIO_CmdInitStructure.SDIO_CmdIndex = _SdCmd[ComdInd].TxData;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
  SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;

  if(pArg != NULL)
  {
    switch(_SdCmd[ComdInd].Resp)
    {
    case SdR2:
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
      break;
    case SdR1:
    case SdR1b:
    case SdR3:
    case SdR7:
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    }
  }

  // Send command's arguments
  if(_SdCmd[ComdInd].Arg != SdNoArg)
  {
    SDIO_CmdInitStructure.SDIO_Argument = *pArg;
  }
  else
  {
    SDIO_CmdInitStructure.SDIO_Argument = 0;
  }

  // Clear all the static flags
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  SDIO_SendCommand(&SDIO_CmdInitStructure);

  // Wait command respond
  if(CMD0 == ComdInd)
  {
    timeout = SDIO_CMD0TIMEOUT;
    do
    {
      if(0 == --timeout)
      {
        // Clear all the static flags
        return(SdNoResponse);
      }
      Status = SDIO->STA;
    }
    while (0 == (Status & SDIO_FLAG_CMDSENT));
  }
  else
  {
    do
    {
      Status = SDIO->STA;
    }
    while(!(Status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)));
  }

  SDIO_ClearFlag(SDIO_FLAG_CMDSENT);

  if (Status & SDIO_FLAG_CTIMEOUT)
  {
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(SdNoResponse);
  }

  if (Status & SDIO_FLAG_CCRCFAIL)
  {
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    switch(_SdCmd[ComdInd].TxData)
    {
    // Ignore CRC Error
    case  1: // CMD1
    case 41: // ACMD42
    case 12: // CMD12
      break;
    default:
      return(SdCardError);
    }
  }

  if(pArg != NULL)
  {
    switch (_SdCmd[ComdInd].Resp)
    {
    case SdNoResp:
      break;
    case SdR3:
    case SdR7:
      *pArg = SDIO_GetResponse(SDIO_RESP1);
      break;
    case SdR2:
      *pArg++ = SDIO_GetResponse(SDIO_RESP1);
      *pArg++ = SDIO_GetResponse(SDIO_RESP2);
      *pArg++ = SDIO_GetResponse(SDIO_RESP3);
      *pArg++ = SDIO_GetResponse(SDIO_RESP4);
      break;
    default:
      // Check response received is of desired command
      if (SDIO_GetCommandResponse() != _SdCmd[ComdInd].TxData)
      {
        return(SdCardError);
      }
      *pArg = SDIO_GetResponse(SDIO_RESP1);
    }
  }
  SDIO_ClearFlag(SDIO_FLAG_CMDREND);
  return(SdOk);
}

/*************************************************************************
 * Function Name: _SdInitMedia
 * Parameters: none
 *
 * Return: SdState_t
 *
 * Description: SD/MMC detect and initialize
 *
 *************************************************************************/
static
SdState_t _SdInitMedia (void)
{
Int32U i,res;
volatile Int32U Dly;
Int8U MmcSdCid[16];

  _Tnac = 1;
  if(!_SdPresent())
  {
    if(SDIO_PowerState_OFF != SDIO_GetPowerState())
    {
      // Set to power down state
      _SdPowerDown();
    }
    return(SdNoPresent);
  }

  _SdPowerDown();
  SdDly_1ms(100);

  // Set Power State to ON
  SDIO_SetPowerState(SDIO_PowerState_ON);
  SdDly_1ms(100);

  // Enable SDIO Clock
  SDIO_ClockCmd(ENABLE);

  if(_SdSendCmd(CMD0,NULL) != SdOk)
  {
    return(SdNoResponse);
  }

  // Determinate Card type SD or MMC
  _SdDskCtrlBlk.DiskType = DiskSD_Spec2_0;
  res = Cmd8Reg;
  if(SdOk == _SdSendCmd(CMD8,&res))
  {
    _bHC = TRUE;
  }
  else
  {
    _SdDskCtrlBlk.DiskType = DiskSD_Spec1_x;
  }
  for(i=100;i;--i)
  {
    res = 0;
    if((_SdSendCmd(CMD55,&res) == SdOk) &&
       (res & 0x100))
    {
      res = _bHC?OcrReg | OcrReg_HC:OcrReg;
      if((_SdSendCmd(ACMD41,&res) == SdOk) &&
         (res & 0x80000000))
      {
        // SD card is find
        _bHC = 0 != (res & OcrReg_HC);
        break;
      }
    }
    else
    {
      _SdDskCtrlBlk.DiskType = DiskMMC;
      // CMD1 for MMC Init sequence
      // will be complete within 500ms
      res = OcrReg;
      if(_SdSendCmd(CMD1,&res) ==  SdOk && (res & 0x80000000))
      {
        // MMC card is find
        _bHC = FALSE;
        break;
      }
    }

    SdDly_1ms(10);
  }


  if(i == 0)
  {
    return(SdNoResponse);
  }

  // Read CID
  if(_SdSendCmd(CMD2,(pInt32U)MmcSdCid) != SdOk)
  {
    return(SdNoResponse);
  }

  // Set address
  _CardRCA = (_SdDskCtrlBlk.DiskType == DiskMMC)?0x00010000:0x00000000;
  if(_SdSendCmd(CMD3,&_CardRCA) != SdOk)
  {
    return(SdNoResponse);
  }
  if(DiskMMC != _SdDskCtrlBlk.DiskType)
  {
    _CardRCA &= 0xFFFF0000;
  }
  else
  {
    _CardRCA  = 0x00010000;
  }

  // Read CSD
  _MmcSdCsd[0] = 0;
  _MmcSdCsd[1] = 0;
  _MmcSdCsd[2] = _CardRCA >> 16;
  _MmcSdCsd[3] = _CardRCA >> 24;
  if(_SdSendCmd(CMD9,(pInt32U)_MmcSdCsd) != SdOk)
  {
    return(SdNoResponse);
  }

  // Implement CSD data
  _SdCsdImplemet();

  // Enter in TRAN state
  res = _CardRCA;
  if(_SdSendCmd(CMD7,&res) != SdOk)
  {
    return(SdNoResponse);
  }
  res = _CardRCA;
  if(SdOk != _SdSendCmd(CMD13,&res))
  {
    return(SdNoResponse);
  }
  else if(!(res & READY_FOR_DATA) ||
          ((res & CURRENT_STATE) != CARD_TRAN))
  {
    return(SdCardError);
  }

  // Set Block size
  res = _SdDskCtrlBlk.BlockSize;
  if(_SdSendCmd(CMD16,&res))
  {
    return(SdNoResponse);
  }

  if(DiskMMC != _SdDskCtrlBlk.DiskType)
  {
    // Use wide bus for SD
    res = _CardRCA;
    if((_SdSendCmd(CMD55,&res) != SdOk) ||
       !(res & 0x100))
    {
      return(SdCardError);
    }
    // Set bus width 4bits
    res = 2;
    if(_SdSendCmd(ACMD6,&res) != SdOk)
    {
      return(SdCardError);
    }
    SDIO->CLKCR |= 1UL << 11;
  }
  return(SdOk);
}

/*************************************************************************
 * Function Name: _SdCsdImplemet
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Implement data from CSD
 *
 *************************************************************************/
static
void _SdCsdImplemet (void)
{
Int32U Freq;
Int64U Tmp;
  // Calculate SPI max clock
  Freq = _SdTransfExp[_CSD_GET_TRAN_SPEED_EXP()] * _SdCsdMant[_CSD_GET_TRAN_SPEED_MANT()];
  Freq = _SdSetClockFreq(Freq);
  if(DiskMMC == _SdDskCtrlBlk.DiskType)
  {
    // Calculate Time outs for MMC cards
    Tmp = _SdAccessTime[_CSD_GET_TAAC_EXP()] * _SdCsdMant[_CSD_GET_TAAC_MANT()];
    Tmp /= 10000; // us
    // Freq [Hz], Tmp[1 us], *10
    Tmp = (Freq*Tmp)/100000LL;
    // NSAC*100*10
    Tmp += 1000*_CSD_GET_NSAC();
    // Max time out
    _Tnac = Tmp;
    _Twr  = Tmp * (1<<_CSD_GET_R2W_FACTOR());
  }
  else
  {
    // Calculate Time outs for SD cards
    // Freq [Hz], RD_TIME_OUT[ms]
    Tmp = Freq/1000;
    _Tnac = Tmp * RD_TIME_OUT;
    _Twr  = Tmp * WR_TIME_OUT;
  }
  // Calculate Block size and Block Number
  _SdDskCtrlBlk.BlockSize = 1<<_CSD_GET_READ_BL_LEN();
  if(_bHC)
  {
    _SdDskCtrlBlk.BlockNumb = (_CSD_2_0_GET_C_SIZE()+1) * 1024;
  }
  else
  {
    _SdDskCtrlBlk.BlockNumb = (_CSD_GET_C_SIZE()+1)*(4<<_CSD_GET_C_SIZE_MULT());
  }
  if(_SdDskCtrlBlk.BlockSize != SD_DEF_BLK_SIZE)
  {
    // because Windows support only 512 bytes block
    _SdDskCtrlBlk.BlockNumb *= (_SdDskCtrlBlk.BlockSize>>9);
    _SdDskCtrlBlk.BlockSize  = SD_DEF_BLK_SIZE;
  }
  // Set Write Protect
  _bSdPermWriteProtect = _CSD_GET_PERM_WRITE_PROTECT() |\
                          _CSD_GET_TMP_WRITE_PROTECT();
  _SdDskCtrlBlk.WriteProtect = _SdWriteProtect() |\
                              _bSdPermWriteProtect;
}

/*************************************************************************
 * Function Name: _SdRead
 * Parameters: pInt8U pData, Int32U Add, Int32U Length
 *
 * Return: SdState_t
 *
 * Description: Read from SD/MMC
 *
 *************************************************************************/
static inline
SdState_t _SdRead(pInt8U pData, Int32U Add, Int32U Length)
{
DMA_InitTypeDef DMA_InitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;
SdState_t Status = SdOk;
Int32U Data;
Int32U StatusReg;

  if(Length == 0)
  {
    return(Status);
  }

  // Init data path
  SDIO_DataInitStructure.SDIO_DataTimeOut   = _Tnac;
  SDIO_DataInitStructure.SDIO_DataLength    = _SdDskCtrlBlk.BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
  SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  // DMA init
  DMA_InitStructure.DMA_Channel             = SD_SDIO_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)SDIO_FIFO_Address;
  DMA_InitStructure.DMA_DIR                 = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize          = _SdDskCtrlBlk.BlockSize / 4;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority            = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;;
  DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_INC4;

  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  while(Length)
  {
    // Set data size for one block
    Data = Add;
    Status = _SdSendCmd(CMD17,&Data);
    if(SdOk != Status)
    {
      return(Status);
    }

    if(!(Data & READY_FOR_DATA) ||
       ((Data & CURRENT_STATE) != CARD_TRAN))
    {
      _SdSendCmd(CMD12,NULL);
      return(SdCardError);
    }

    // DMA2 Channel4 Config
    DMA_InitStructure.DMA_Memory0BaseAddr = (Int32U)pData;

    // DMA2 Stream6 disable
    DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

    // Clear DMA flags
    DMA_ClearFlag(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TCIF | SD_SDIO_DMA_FLAG_TEIF | \
                                     SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_DMEIF | \
                                     SD_SDIO_DMA_FLAG_FEIF);

    // Clear all SDIO related flags
    SDIO_ClearFlag(  SDIO_IT_DTIMEOUT
                   | SDIO_IT_STBITERR
                   | SDIO_IT_DCRCFAIL
                   | SDIO_IT_RXOVERR
                   | SDIO_IT_DATAEND
                   | SDIO_IT_DBCKEND);

    // DMA2 Channel4 Config
    DMA_Init(SD_SDIO_DMA_STREAM, &DMA_InitStructure);

    // DMA2 Channel4 enable
    DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);

    SDIO_DMACmd(ENABLE);

    // wait receive complete
    while (DMA_GetFlagStatus(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TCIF) == RESET)
    {
      StatusReg = SDIO->STA;
      if(StatusReg & (  SDIO_IT_DTIMEOUT
                      | SDIO_IT_STBITERR
                      | SDIO_IT_DCRCFAIL
                      | SDIO_IT_RXOVERR))
      {
        break;
      }
    }

    if(SET == DMA_GetFlagStatus(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TEIF))
    {
      return(SdDmaError);
    }

    StatusReg = SDIO->STA;

    // data timeout
    if (StatusReg & SDIO_IT_DTIMEOUT)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdNoResponse);
    }

    // Start bit not detected on all data signals in wide bus mode.
    if (StatusReg & SDIO_IT_STBITERR)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    // CRC error
    if (StatusReg & SDIO_IT_DCRCFAIL)
    {
      // clear error flag
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    // rx overrun
    if (StatusReg & SDIO_IT_RXOVERR)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    if(_bHC)
    {
      ++Add;
    }
    else
    {
      Add += _SdDskCtrlBlk.BlockSize;
    }
    --Length;
    pData += _SdDskCtrlBlk.BlockSize;

    // data end
    if(!Length)
    {
      return(Status);
    }
    // loop read next block
  }

  return(Status);
}

/*************************************************************************
 * Function Name: _SdWrite
 * Parameters: const Int8U * pData, Int32U Add, Int32U Length
 *
 * Return: SdState_t
 *
 * Description: Write to SD/MMC
 *
 *************************************************************************/
Int32U Data1;
Int32U StatusReg;
static inline
SdState_t _SdWrite(const Int8U * pData, Int32U Add, Int32U Length)
{
SdState_t Status = SdOk;
Int32U Data;
DMA_InitTypeDef DMA_InitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;

  //Data1 = 0xDEDDABBA;

  if(!Length)
  {
    return(Status);
  }

  // Init data path
  SDIO_DataInitStructure.SDIO_DataTimeOut   = _Twr;
  SDIO_DataInitStructure.SDIO_DataLength    = _SdDskCtrlBlk.BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_512b;
  SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  // DMA init
  DMA_InitStructure.DMA_Channel             = SD_SDIO_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr  = (u32)SDIO_FIFO_Address;
  DMA_InitStructure.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize          = _SdDskCtrlBlk.BlockSize / 4;
  DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc           = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode                = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority            = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode            = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;;
  DMA_InitStructure.DMA_MemoryBurst         = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst     = DMA_PeripheralBurst_INC4;

  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  while(Length)
  {
    Data = Add;
    Status = _SdSendCmd(CMD24,&Data);
    if(SdOk != Status)
    {
      return(Status);
    }

    if(!(Data & READY_FOR_DATA) ||
       ((Data & CURRENT_STATE) != CARD_TRAN))
    {
      _SdSendCmd(CMD12,NULL);
      return(SdCardError);
    }

    // set destination address
    DMA_InitStructure.DMA_Memory0BaseAddr = (Int32U)pData;

    // DMA2 Stream6 disable
    DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

    // Clear DMA flags
    DMA_ClearFlag(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TCIF | SD_SDIO_DMA_FLAG_TEIF | \
                                     SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_DMEIF | \
                                     SD_SDIO_DMA_FLAG_FEIF);

    // Clear all SDIO related flags
    SDIO_ClearFlag(  SDIO_IT_DTIMEOUT
                   | SDIO_IT_STBITERR
                   | SDIO_IT_DCRCFAIL
                   | SDIO_IT_TXUNDERR
                   | SDIO_IT_DATAEND
                   | SDIO_IT_DBCKEND);

    // DMA2 Channel4 Config
    DMA_Init(SD_SDIO_DMA_STREAM, &DMA_InitStructure);

    // DMA2 Channel4 enable
    DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);

    SDIO_DMACmd(ENABLE);

    // wait transfer complete
    do
    {
      StatusReg = SDIO->STA;
    }
    while( (DMA_GetFlagStatus(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TCIF) == RESET
             || (0 == (StatusReg & SDIO_IT_DBCKEND)))
           && (0 == (StatusReg & (  SDIO_IT_DTIMEOUT
                                  | SDIO_IT_STBITERR
                                  | SDIO_IT_DCRCFAIL
                                  | SDIO_IT_TXUNDERR))));


    if(SET == DMA_GetFlagStatus(SD_SDIO_DMA_STREAM,SD_SDIO_DMA_FLAG_TEIF))
    {
      return(SdDmaError);
    }

    StatusReg = SDIO->STA;
    // data timeout
    if (StatusReg & SDIO_IT_DTIMEOUT)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdNoResponse);
    }

    // Start bit not detected on all data signals in wide bus mode.
    if (StatusReg & SDIO_IT_STBITERR)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    // CRC error
    if (StatusReg & SDIO_IT_DCRCFAIL)
    {
      // clear error flag
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    // tx underrun
    if (StatusReg & SDIO_IT_TXUNDERR)
    {
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
      return(SdCardError);
    }

    if(_bHC)
    {
      ++Add;
    }
    else
    {
      Add += _SdDskCtrlBlk.BlockSize;
    }
    --Length;
    pData += _SdDskCtrlBlk.BlockSize;

    // wait write until write is done
    do
    {
      Data1 = _CardRCA;
      if(_SdSendCmd(CMD13,&Data1) != SdOk)
      {
        continue;
      }
      Data1 >>= 9;
      Data1 &= 0xF;
    }
    while(  (7 == Data1)
          ||(6 == Data1));


    // data end
    if(!Length)
    {
      return(Status);
    }
    // loop write next block
  }

  return(Status);
}

/*************************************************************************
 * Function Name: MmcVerify
 * Parameters: const Int8U * pData, Int32U Add, Int32U Length
 *
 * Return: SdState_t
 *
 * Description: Verify data in SD/MMC
 *
 *************************************************************************/
static inline
SdState_t _SdVerify(const Int8U * pData, Int32U Add, Int32U Length)
{
#pragma segment="USB_DMA_RAM"
#pragma location="USB_DMA_RAM"
__no_init static Int8U TempBuffer[2048];  // maximum block length is 2048
SdState_t Status = SdOk;

  while(Length)
  {
    Status = _SdRead(TempBuffer,Add,_SdDskCtrlBlk.BlockSize);
    if(Status != SdOk)
    {
      break;
    }
    for(Int32U i = 0; i < _SdDskCtrlBlk.BlockSize; ++i)
    {
      if(TempBuffer[i] != *pData++)
      {
        return(SdMiscompare);
      }
    }
    if(_bHC)
    {
      --Length;
      ++Add;
    }
    else
    {
      Length -= _SdDskCtrlBlk.BlockSize;
      Add += _SdDskCtrlBlk.BlockSize;
    }
    pData += _SdDskCtrlBlk.BlockSize;
  }
  return(Status);
}

/*************************************************************************
 * Function Name: SdGetLastError
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return Last error
 *
 *************************************************************************/
Int32U SdGetLastError(void)
{
  return(_SdLastError);
}

/*************************************************************************
 * Function Name: SdStatusUpdate
 * Parameters: none
 *
 * Return: none
 *
 * Description: Update status of SD/MMC card
 *
 *************************************************************************/
void SdStatusUpdate (void)
{
Int32U Data = _CardRCA;
  // Update WP state
  _SdDskCtrlBlk.WriteProtect = _SdWriteProtect() |\
                               _bSdPermWriteProtect;
  if(!_SdPresent())
  {
    _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
    _SdPowerDown();
    return;
  }

  if((_SdDskCtrlBlk.DiskStatus != DiskCommandPass))
  {
    switch (_SdInitMedia())
    {
    case SdOk:
      _SdDskCtrlBlk.DiskStatus = DiskCommandPass;
      _SdDskCtrlBlk.MediaChanged = TRUE;
      break;
    case SdCardError:
      _SdDskCtrlBlk.DiskStatus = DiskNotReady;
      break;
    default:
      _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
      break;
    }
  }
  else if(_SdSendCmd(CMD13,&Data) != SdOk)
  {
    _SdDskCtrlBlk.DiskStatus = DiskNotReady;
  }
  else if (!(Data & READY_FOR_DATA))
  {
    _SdDskCtrlBlk.DiskStatus = DiskNotReady;
  }
}

/*************************************************************************
 * Function Name: SdDiskInit
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Init MMC/SD disk
 *
 *************************************************************************/
void SdDiskInit (void)
{
  _SdDskCtrlBlk.BlockNumb =\
  _SdDskCtrlBlk.BlockSize =\
  _SdLastError           = 0;
  // Init SPI
  _SdInit();
  // Media Init
  switch (_SdInitMedia())
  {
  case SdOk:
    _SdDskCtrlBlk.DiskStatus = DiskCommandPass;
    _SdDskCtrlBlk.MediaChanged = TRUE;
    break;
  case SdCardError:
    _SdDskCtrlBlk.DiskStatus = DiskNotReady;
    break;
  default:
    _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
    break;
  }
}

/*************************************************************************
 * Function Name: SdGetDiskCtrlBkl
 * Parameters:  none
 *
 * Return: pDiskCtrlBlk_t
 *
 * Description: Return pointer to control block structure of the disk
 *
 *************************************************************************/
pDiskCtrlBlk_t SdGetDiskCtrlBkl (void)
{
  return(&_SdDskCtrlBlk);
}

/*************************************************************************
 * Function Name: SdDiskIO
 * Parameters: pInt8U pData,Int32U BlockStart,
 *             Int32U BlockNum, DiskIoRequest_t IoRequest
 *
 * Return: DiskStatusCode_t
 *
 * Description: MMC/SD disk I/O
 *
 *************************************************************************/
DiskStatusCode_t SdDiskIO (pInt8U pData,Int32U BlockStart,
                              Int32U BlockNum, DiskIoRequest_t IoRequest)
{
  if((pData == NULL) || (BlockStart+BlockNum > _SdDskCtrlBlk.BlockNumb))
  {
    return(DiskParametersError);
  }
  if (_SdDskCtrlBlk.DiskStatus != DiskCommandPass)
  {
    return(_SdDskCtrlBlk.DiskStatus);
  }
  switch (IoRequest)
  {
  case DiskWrite:
    if(_SdDskCtrlBlk.WriteProtect)
    {
      return(DiskParametersError);
    }
    switch (_SdWrite(pData,
                     _bHC?BlockStart:BlockStart*_SdDskCtrlBlk.BlockSize,
                     BlockNum))
    {
    case SdOk:
      break;
    case SdCardError:
      _SdDskCtrlBlk.DiskStatus = DiskNotReady;
      break;
    default:
      _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  case DiskRead:
    switch (_SdRead(pData,
                   _bHC?BlockStart:BlockStart*_SdDskCtrlBlk.BlockSize,
                   BlockNum))
    {
    case SdOk:
      break;
    case SdCardError:
      _SdDskCtrlBlk.DiskStatus = DiskNotReady;
      break;
    default:
      _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  case DiskVerify:
    switch (_SdVerify(pData,
                     _bHC?BlockStart:BlockStart*_SdDskCtrlBlk.BlockSize,
                     BlockNum))
    {
    case SdOk:
      break;
    case SdMiscompare:
      return(DiskMiscompareError);
    case SdCardError:
      _SdDskCtrlBlk.DiskStatus = DiskNotReady;
      break;
    default:
      _SdDskCtrlBlk.DiskStatus = DiskNotPresent;
      break;
    }
    break;
  default:
    return(DiskParametersError);
  }
  return(_SdDskCtrlBlk.DiskStatus);
}

