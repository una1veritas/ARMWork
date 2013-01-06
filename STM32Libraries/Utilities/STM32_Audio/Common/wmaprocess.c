/**
  ******************************************************************************
  * @file    wmaprocess.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-July-2012
  * @brief   This file includes WmaProcessing layer routines
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
#include "wmaprocess.h"

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @defgroup wmaprocess
  * @brief WmaProcess driver modules
  * @{
  */

/** @defgroup WmaProcess_Private_TypesDefinitions
  * @{
  */
static TSpiritWMADec_Prms objWmaPrms;
/**
  * @}
  */

/** @defgroup WmaProcess_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup WmaProcess_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup WmaProcess_Private_Variables
  * @{
  */
static TSpiritASFReader *pObjASFDemux = NULL;
static uint32_t *aiPersist = NULL;
static uint32_t *aiScratch = NULL;
static uint8_t *acBufIn = NULL;

static ESpiritWMA_State wmaState = WMAState_Done;
static TSpiritWMADec *pObjWmaDec = NULL;

/* Variables used by WmaProcess_DecodeData() only */
__IO uint32_t n_decoded_samples = 0;
__IO uint32_t max_input_retry = 0x10; /* avoid application freeze on bad stream */
__IO uint8_t *pBuf = NULL;
__IO uint32_t iBufSize = 0x00;
uint32_t isNewPayload = 0x00;
/**
  * @}
  */

/** @defgroup WmaProcess_Private_FunctionPrototypes
  * @{
  */
fnReadCallback_TypeDef* pWMAReadCallback;
fnSetPositionCallback_TypeDef* pWMASetPosCallback;

static unsigned long callback_for_asf_demux (unsigned long uiOffset,   /* Offset from a file orig */
                                             unsigned long uiNumBytes, /* Number of bytes to read */
                                             unsigned char **ppubBuf,  /* Buffer pointer */
                                             void *user_prm);          /* User parameter */
/**
  * @}
  */

/** @defgroup WmaProcess_Private_Functions
  * @{
  */

/**
  * @brief  Decoder structure initialize.
  * @param  pHeader: pointer to the audio file header tab.
  * @retval None.
  */
uint32_t WmaProcess_DecoderInit(uint8_t* pHeader, 
                                fnReadCallback_TypeDef* pReadCallback, 
                                fnSetPositionCallback_TypeDef* pSetPosCallback)
{
  TSpiritASFReader_WMAPrms objASF_WmaPrms;

  /* Enable the CRC */
  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  CRC->CR = CRC_CR_RESET;
  for (int index=0; index<300; index++);
  
  /* Allocate persistant memory space */
  aiPersist = (uint32_t *)p_malloc(SPIRIT_WMADEC_PERSIST_SIZE_IN_BYTES);
  if (aiPersist == NULL)
  {
    return 1;
  }
  
  /* Allocate the scratch memory space */
  aiScratch = (uint32_t *)malloc(SPIRIT_WMADEC_SCRATCH_SIZE_IN_BYTES); /* Might use p_malloc if SDcard is not used */
  if ( aiScratch == NULL )
  {
    p_free(aiPersist);
    aiPersist = NULL;
    return 2;
  }
  
  /* Allocate temporary buffer for ASF decoding */
  acBufIn = (uint8_t*) malloc(4 * 1024 * sizeof(uint8_t)); /* Might use p_malloc if SDcard is not used */
  if (acBufIn == NULL)
  {
    free(aiScratch);
    aiScratch = NULL;
    p_free(aiPersist);
    aiPersist = NULL;
    return 3;
  }
  
  /* Get the pointer to the data read callback */
  pWMAReadCallback = pReadCallback;
  pWMASetPosCallback = pSetPosCallback;
  
  /* Use the persistent memory space for the WMA Decoder Object */
  pObjWmaDec = (TSpiritWMADec *)aiPersist;
  
  /* Initialize ASF reader */
  pObjASFDemux = SpiritASFReader_Create (1,                       /* Target stream number */
                                         &callback_for_asf_demux, /* data source callback */
                                         NULL);                   /* user parameter */
  
  /* Parse ASF header */
  if (SpiritASFReader_Start(pObjASFDemux) != ASF_OK)
  {
    return 4;
  }
  
  /* Catch decoder initialization parameters */
  SpiritASFReader_GetWMAPrms(pObjASFDemux, &objASF_WmaPrms);
  objWmaPrms.usFormatTag = objASF_WmaPrms.usFormatTag;
  objWmaPrms.usEncoderOpt = objASF_WmaPrms.usEncoderOpt;
  objWmaPrms.uiSampleRateHz = objASF_WmaPrms.uiSampleRateHz;
  objWmaPrms.ubChannelsNumber = objASF_WmaPrms.ubChannelsNumber;
  objWmaPrms.uiAvgBytesPerSecond = objASF_WmaPrms.uiAvgBytesPerSecond;
  objWmaPrms.isMonoOutput = 0;//1;
  
  /* Initialize decoder */
  SpiritWMADec_Init(pObjWmaDec, &objWmaPrms);
  
  /* Update the decoder state */
  wmaState = WMAState_Input;
  
  return 0;
}

/**
  * @brief  Free resources used by the WMA decoder.
  * @param  None.
  * @retval None.
  */
uint32_t WmaProcess_DecoderDeInit(void)
{
  
  WmaProcess_StopMode();
  
  /* Free all used resources */
  if (aiPersist != NULL)
  {
    p_free(aiPersist);
    aiPersist = NULL;
  }
  if (aiScratch != NULL)
  {
    free(aiScratch);
    aiScratch = NULL;
  }
  if (acBufIn != NULL)
  {
    free(acBufIn);
    acBufIn = NULL;
  }
  
  return 0;
}

/**
  * @brief  Provide output PCM stream.
  * @param  OutPutBuffer: output PCM.
  * @param  _none: NULL not used for this decoder.
  * @param  __none: NULL not used for this decoder.
  * @retval Number of PCM samples decoded.
  */
uint32_t WmaProcess_DecodeData(__IO int16_t* OutPutBuffer, uint32_t _none, void * __none)
{
  n_decoded_samples = 0;
  max_input_retry = 0x10; /* avoid application freeze on bad stream */
  iBufSize = 0x00;
  isNewPayload = 0x00;
  
  
  /* Check memory allocation status */
  if ((pObjASFDemux == NULL) || (pObjWmaDec == NULL) || (acBufIn == NULL))
  {
    return 0;
  }
  
  /* Main loop */
  do
  {
    wmaState = SpiritWMADec_GetState(pObjWmaDec);
    if (wmaState == WMAState_Done)
    {
      return 0;
    }
    if (wmaState == WMAState_Input)
    {
      max_input_retry--;
      
      /* Read ASF */
      SpiritASFReader_GetData(pObjASFDemux, (char**)&pBuf, (int*)&iBufSize, (int*)&isNewPayload);
      
      /* Pass new buffer to decoder */
      SpiritWMADec_BufferAttach(pObjWmaDec, (char*)pBuf, iBufSize, isNewPayload);
    }
    else
    {
      /* Process data */
      SpiritWMADec_Decode(pObjWmaDec, (int*)&n_decoded_samples, (short *)OutPutBuffer, aiScratch);
      
      if (n_decoded_samples != 0 )
      {                
        return (n_decoded_samples * sizeof(int16_t));
      }
    }
  }
  while ((n_decoded_samples == 0) && (max_input_retry != 0));

  return 0;
}

/**
  * @brief  Forward the current wma stream
  * @param  None.
  * @retval None.
  */
uint32_t WmaProcess_Forward(void)
{
  uint32_t tmp = 0x00;
  __IO TASF *pASF = ( TASF * )pObjASFDemux;

  tmp = pASF->hdr.fileProp.dataPacketSize;
  pASF->currPacketOffset += ((SEEKSTEP - 1) * tmp);
  pASF->nextPacketOffset = (pASF->currPacketOffset + tmp);
  pASF->nPayloadsProcessed += SEEKSTEP;
  
  return 0;  
}

/**
  * @brief  Backward the current wma stream
  * @param  None.
  * @retval None.
  */
uint32_t WmaProcess_Backward(void)
{
  uint32_t tmp1 = 0x00;
  uint32_t tmp2 = 0x00;
  uint32_t tmp3 = 0x00;
  __IO TASF *pASF = ( TASF * )pObjASFDemux;

  tmp1 = pASF->currPacketOffset;
  tmp2 = pASF->hdr.fileProp.dataPacketSize;
  tmp3 = (SEEKSTEP * tmp2);

  if (((int32_t)((int32_t)tmp1 - ((int32_t)tmp3))) > 0)
  {
    pASF->currPacketOffset -= tmp3;
    pASF->nextPacketOffset = (pASF->currPacketOffset + tmp2);
    pASF->nPayloadsProcessed -= SEEKSTEP;
  }
  
  return 0;  
}

/**
  * @brief  Get the lenght of the current wma stream.
  * @param  None.
  * @retval return the stream lenght.
  */
uint32_t WmaProcess_GetStreamLenght(uint32_t fLength)
{
  if (objWmaPrms.uiAvgBytesPerSecond != 0)
    return (fLength / objWmaPrms.uiAvgBytesPerSecond);
  else
    return 0;
}

/**
  * @brief  Get the elapsed time of the current wma stream.
  * @param  None.
  * @retval return the elapsed stream time.
  */
uint32_t WmaProcess_GetElapsedTime(uint32_t CurrPos)
{
  if (objWmaPrms.uiAvgBytesPerSecond != 0)
    return CurrPos / objWmaPrms.uiAvgBytesPerSecond;
  else
    return 0;
}

/**
  * @brief  Stop decoding the WMA stream.
  * @param  None.
  * @retval 0 if passed, !0 if failed.
  */
uint32_t WmaProcess_StopMode(void)
{
  TSpiritASFReader_WMAPrms objASF_WmaPrms;
  if ((pObjASFDemux == NULL) || (pObjWmaDec == NULL) || (acBufIn == NULL))
  {
    return 1;
  }

  SpiritASFReader_Destroy(pObjASFDemux);
  /* Initialize ASF reader */
  pObjASFDemux = SpiritASFReader_Create (1,                       /* Stream number */
                                         &callback_for_asf_demux, /* data source callback */
                                         NULL);                   /* user parameter */
  
  /* Parse ASF header */
  SpiritASFReader_Start (pObjASFDemux);

  /* Get the decoder initialization parameters */
  SpiritASFReader_GetWMAPrms(pObjASFDemux, &objASF_WmaPrms);
  objWmaPrms.usFormatTag = objASF_WmaPrms.usFormatTag;
  objWmaPrms.usEncoderOpt = objASF_WmaPrms.usEncoderOpt;
  objWmaPrms.uiSampleRateHz = objASF_WmaPrms.uiSampleRateHz;
  objWmaPrms.ubChannelsNumber = objASF_WmaPrms.ubChannelsNumber;
  objWmaPrms.uiAvgBytesPerSecond = objASF_WmaPrms.uiAvgBytesPerSecond;
  /* Initialize decoder */
  SpiritWMADec_Init(pObjWmaDec, &objWmaPrms);

  /* Initialize the decoder state */
  wmaState = WMAState_Input;  
  
  return 0;
}

/**
  * @brief  Get the sampling rate of the current wma stream.
  * @param  None.
  * @retval Return the sampling rate.
  */
uint32_t WmaProcess_GetSamplingRate(void)
{
  return objWmaPrms.uiSampleRateHz;
}

/**
  * @brief  Provide the tag of the current wma stream.
  * @param  pIDTAG: pointer to the Tag structure.
  * @retval None.
  */
uint32_t WmaProcess_RetrieveIDTAGS(TAGS_TypeDef * pIDTAG, FIL* pFile)
{
  /* Call the ASF layer function */
  //SongUtilities_WMATagParser(pIDTAG, pFile, acBufIn);
  
  return 0;
}

/**
  * @brief  Callback function to supply the decoder with wma bitsteram.
  * @param  uiOffset: offset from a file orignal.
  * @param  uiNumBytes: Number of bytes to read.
  * @param  ppubBuf: output Buffer pointer.
  * @param  user_prm: User parameter (ie. input file name).
  * @retval return the number of the bytes read from the file.
  */
static unsigned long callback_for_asf_demux (unsigned long uiOffset,
                                             unsigned long uiNumBytes,
                                             unsigned char **ppubBuf,
                                             void *user_prm)
{
  uint32_t NumBytes = 0;
  
  /* Call the upper layer function */
  if (pWMASetPosCallback(uiOffset) != 0)
  {
    /*uiNumBytes*/ NumBytes = 0;
    *ppubBuf = (unsigned char *)acBufIn;
    return 0;
  }
  
  /* Read the required data from the audio file */
  /*uiNumBytes*/ NumBytes = pWMAReadCallback (acBufIn, uiNumBytes, user_prm);
  *ppubBuf = (unsigned char *)acBufIn;

  return NumBytes; //uiNumBytes;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
