/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : sd_card_mode.h
 *    Description : SD/MMC diver include file
 *
 *    History :
 *    1. Date        : 30, July 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #1 $
 **************************************************************************/

#include "includes.h"

#ifndef __SD_CARD_MODE_H
#define __SD_CARD_MODE_H

#ifdef SD_CARD_MODE_GLOBAL
#define SD_CARD_MODE_EXTERN
#else
#define SD_CARD_MODE_EXTERN  extern
#endif

#define SD_DISK_LUN  0
#define SD_DEF_BLK_SIZE   512

// Hardware depends definitions
#define IdentificationModeClock   400KHZ
#define PC_PCSPI1                 (1<<10)
#define PC_PCSPI0                 (1<<8)

// SD Maximum Block Rad Access Time
#define RD_TIME_OUT               100LL   // ms
// SD Maximum Block Write Access Time
#define WR_TIME_OUT               250LL   // ms

typedef enum _SdState_t
{
  SdOk = 0, SdNoPresent, SdNoResponse, SdCardError, SdMiscompare, SdDmaError
} SdState_t;

typedef enum _SdCmdInd_t
{
  CMD0 = 0, // Resets the MultiMediaCard

  CMD1,     // Activates the card’s initialization process

  CMD2,     // Asks all cards to send its card identification (CID)

  CMD3,     // Set relative address

  CMD7,     // Select/deselect card the card is selected by its own relative
            // address and deselected by any other address; address 0 deselects all.

  CMD8,     // Send Interface Condition Command
            // for HC cards only

  CMD9,     // Asks the selected card to send its card-specific data (CSD)

  CMD10,    // Asks the selected card to send its card identification (CID)

  CMD12,    // Stop transmission on multiple block read

  CMD13,    // Asks the selected card to send its status register

  CMD16,    // Selects a block length (in bytes) for all following block commands (read and write)

  CMD17,    // Reads a block of the size selected by the SET_BLOCKLEN command

  CMD18,    // Continuously transfers data blocks from card to host
            // until interrupted by a Stop command or the requested number of data blocks transmitted

  CMD24,    // Writes a block of the size selected by the SET_BLOCKLEN command

  CMD25,    // Continuously writes blocks of data until a ‘Stop Tran’
            // Token or the requested number of blocks received

  CMD27,    // Programming of the programmable bits of the CSD

  CMD28,    // If the card has write protection features, this
            // command sets the write protection bit of the
            // addressed group. The properties of write protection
            // are coded in the card specific data (WP_GRP_SIZE).

  CMD29,    // If the card has write protection features, this
            // command clears the write protection bit of the addressed group

  CMD30,    // If the card has write protection features, this
            // command asks the card to send the status of the write protection bits

  CMD32,    // Sets the address of the first sector of the erase group

  CMD33,    // Sets the address of the last sector in a continuous
            // range within the selected erase group, or the address
            // of a single sector to be selected for erase

  CMD34,    // Removes one previously selected sector from the erase selection

  CMD35,    // Sets the address of the first erase group within a range to be selected for erase

  CMD36,    // Sets the address of the last erase group within a
            // continuous range to be selected for erase

  CMD37,    // Removes one previously selected erase group from the erase selection.

  CMD38,    // Erases all previously selected sectors

  CMD42,    // Used to set/reset the password or lock/unlock the
            // card. The size of the Data Block is defined by the SET_BLOCK_LEN command

  CMD55,    // Notifies the card that the next command is an
            // application specific command rather than a standard command.

  CMD56,    // Used either to transfer a Data Block to the card or
            // to get a Data Block from the card for general
            // purpose/application specific commands. The size
            // of the Data Block is defined with SET_BLOCK_LEN command

  ACMD6,    // Defines the data bus width (’00’=1bit or’10’=4 bits bus)
            // (Only for SD)
  ACMD41,   // Activates the card’s initialization process (Only for SD)

  CMD_END   // End of commands index
} SdCmdInd_t;

#define OcrReg       0x40FF8000
#define OcrReg_HC    0x40000000
#define Cmd8Reg      0x000001AA

// Card Status register
#define READY_FOR_DATA    (  1UL <<  8)
#define CURRENT_STATE     (0xFUL <<  9)
#define ERASE_RESET       (  1UL << 13)
#define CARD_ECC_DISABLED (  1UL << 14)
#define WP_ERASE_SKIP     (  1UL << 15)
#define CID_CSD_OVERWRITE (  1UL << 16)
#define ERROR             (  1UL << 17)
#define ILLEGAL_COMMAND   (  1UL << 22)
#define COM_CRC_ERROR     (  1UL << 23)
#define WP_VIOLATION      (  1UL << 26)
#define ERASE_PARAM       (  1UL << 27)
#define ERASE_SEQ_ERROR   (  1UL << 28)
#define BLOCK_LEN_ERROR   (  1UL << 29)
#define ADDRESS_ERROR     (  1UL << 30)
#define OUT_OF_RANGE      (  1UL << 31)
// Card states
#define CARD_IDLE         (0UL << 9)
#define CARD_READY        (1UL << 9)
#define CARD_IDENT        (2UL << 9)
#define CARD_STBY         (3UL << 9)
#define CARD_TRAN         (4UL << 9)
#define CARD_DATA         (5UL << 9)
#define CARD_RCV          (6UL << 9)
#define CARD_PRG          (7UL << 9)
#define CARD_DIS          (8UL << 9)

typedef enum _SdAgmType_t
{
  SdNoArg = 0, SdBlockLen, SdDataAdd, SdRelAddr, SdOcr,
} SdAgmType_t;

typedef enum _MmcRespType_t
{
  SdNoResp = 0,SdR1,SdR1b,SdR2,SdR3,SdR7
} SdRespType_t;

typedef struct _SdCommads_t
{
  Int8U         TxData;
  SdAgmType_t  Arg;
  SdRespType_t Resp;
} SdCommads_t;

typedef union _DataCtrl_t
{
  Int32U Data;
  struct
  {
    Int32U ENABLE     : 1;
    Int32U DIRECTION  : 1;
    Int32U MODE       : 1;
    Int32U DMAENABLE  : 1;
    Int32U BLOCKSIZE  : 4;
    Int32U            :24;
  };
} DataCtrl_t, *pDataCtrl_t;

extern void DelayResolution100us(Int32U Delay);
extern Int32U SYS_GetFpclk(Int32U Periphery);

#define SdDly_1ms(Delay) DelayResolution100us((10 * Delay))

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
Boolean _SdPresent (void);

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
Boolean _SdWriteProtect (void);

/*************************************************************************
 * Function Name: _SdSetClockFreq
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Set SD/MMC ckl frequency
 *
 *************************************************************************/
static
Int32U _SdSetClockFreq (Int32U Frequency);

/*************************************************************************
 * Function Name: _SdInit
 * Parameters: none
 * Return: none
 *
 * Description: Init MCI module and Cart Present and Write Protect pins
 *
 *************************************************************************/
static
void _SdInit (void);

/*************************************************************************
 * Function Name: _SdPowerDown
 * Parameters: none
 * Return: none
 *
 * Description: Set power down state
 *
 *************************************************************************/
static
void _SdPowerDown (void);

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
SdState_t _SdSendCmd(SdCmdInd_t ComdInd,pInt32U pArg);

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
SdState_t _SdInitMedia (void);

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
void _SdCsdImplemet (void);

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
SdState_t _SdRead(pInt8U pData, Int32U Add, Int32U Length);

/*************************************************************************
 * Function Name: _SdWrite
 * Parameters: const Int8U * pData, Int32U Add, Int32U Length
 *
 * Return: SdState_t
 *
 * Description: Write to SD/MMC
 *
 *************************************************************************/
static inline
SdState_t _SdWrite(const Int8U * pData, Int32U Add, Int32U Length);

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
SdState_t _SdVerify(const Int8U * pData, Int32U Add, Int32U Length);

/*************************************************************************
 * Function Name: SdGetLastError
 * Parameters: none
 *
 * Return: Int32U
 *
 * Description: Return Last error
 *
 *************************************************************************/
Int32U SdGetLastError(void);

/*************************************************************************
 * Function Name: SdStatusUpdate
 * Parameters: none
 *
 * Return: none
 *
 * Description: Update status of SD/MMC card
 *
 *************************************************************************/
void SdStatusUpdate (void);

/*************************************************************************
 * Function Name: SdDiskInit
 * Parameters:  none
 *
 * Return: none
 *
 * Description: Init MMC/SD disk
 *
 *************************************************************************/
void SdDiskInit (void);

/*************************************************************************
 * Function Name: SdGetDiskCtrlBkl
 * Parameters:  none
 *
 * Return: pDiskCtrlBlk_t
 *
 * Description: Return pointer to control block structure of the disk
 *
 *************************************************************************/
pDiskCtrlBlk_t SdGetDiskCtrlBkl (void);

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
                           Int32U BlockNum, DiskIoRequest_t IoRequest);

#endif // __SD_CARD_MODE_H
