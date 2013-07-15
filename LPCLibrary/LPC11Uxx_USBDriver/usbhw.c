/****************************************************************************
 *   $Id:: usbhw.c 7760 2011-07-18 17:57:48Z usb00423                        $
 *   Project: NXP LPC11xx USB example
 *
 *   Description:
 *     This file contains LPC11Uxx USB H/W code example which include USB 
 *     initialization, USB interrupt handler, and related APIs for 
 *     USB access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC11Uxx.h"                        /* LPC11Uxx definitions */
#include "type.h"
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbuser.h"
#if POWERDOWN_MODE_USB_WAKEUP
#include "timer16.h"
#endif

/* The BufferUsed flag is to deal with double buffer situation, This is a bit mask 
flag for non-zero EPs. If BufferUsed bit is set, buffer 0 is used, switch to buffer 1. 
If BufferUsed flag is not set, buffer 0 can be used. */
volatile uint32_t BufferUsed = 0;
/* The ActiveFlag is to deal with STALL situation, if STALL happens during
WriteEP(), don't set the ACTIVE bit in EPList CommandStatus immediately but 
this flag only, ACTIVE bit will be set after ClearStallEP() that data is ready
to be sent. This is a bit mask flag for non-zero EPs. */  
volatile uint32_t EPActiveFlag = 0;

/* EP buffer address and length default setting */
volatile EP_LIST EPList[USB_EP_NUM*2]; /* Consider double buffer for non zero EPs */

#if USB_DEBUG
#define LOG_COUNT		0x20

volatile uint32_t DevStatusReset = 0;
volatile uint32_t DevStatusSuspend = 0;
volatile uint32_t DevStatusResume = 0;
volatile uint32_t DevStatusConnect = 0;
volatile uint32_t DevStatusConnectChange = 0;
volatile uint32_t DevStatusSuspendChange = 0;
volatile uint8_t  DevStatusLog[LOG_COUNT];
volatile uint8_t  DevStatusLogCount = 0;
volatile uint8_t  DeviceErrorCode = 0;
#endif

#if LPM_SUPPORT
/* LPM Suspend Interrupt Debugging */
volatile uint32_t DevStatusLPMSuspend = 0;
volatile uint32_t DevStatusLPMRemoteWakeup = 0;
#endif

#if POWERDOWN_MODE_USB_WAKEUP
volatile uint32_t SuspendFlag = 0;
volatile uint32_t USBWakeupInterruptFlag = 0;
extern volatile uint32_t timer16_0_counter;
extern volatile uint32_t WakeupFlag;
#endif

#if USB_SOF_EVENT
volatile uint32_t SOFIRQCount = 0;
#endif

#if USB_FIQ_EVENT
volatile uint32_t SOFFIQCount = 0;
#endif

/*    
 *    Based on the logic EP number and direction bit 7,
 *    when bit 7 is set, it's an IN EP, otherwise,
 *    it's an OUT EP. The return value is the
 *    location of the EP in the interrupt(status/enable/
 *    routing, etc) register.
 *    The bit info. of the interrupt register is
 *    bit 0 is EP0 OUT, bit 1 is EP0 IN,...
 *    bit 28 is EP14 OUT, bit 29 is EP14 IN....
 *    e.g. EPNum(0x80) is EP0 IN, the return
 *    value is 1; EPNum(0x8E) is EP14 IN, the return
 *    value is 29. 
 *    Parameters:      Logical EP with direction bit
 *    Return Value:    EP address in interrupt status.
 */
static uint32_t EPAdr(uint32_t EPNum)
{
  uint32_t val;

  val = (EPNum & 0x0F) << 1;
  if (EPNum & 0x80)
  {
    val += 1;
  }
  return (val);
}

/*    
 *    USB and IO Clock configuration only.
 *
 *    Configure clock and I/Os for USB.
 *    Parameters:      None
 *    Return Value:    None
 */
void USBIOClkConfig( void )
{
  /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  /* Enable AHB clock to the USB block and USB RAM. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((0x1<<14)|(0x1<<27));

  /* Pull-down is needed, or internally, VBUS will be floating. This is to
  address the wrong status in VBUSDebouncing bit in CmdStatus register. It
  happens on the NXP Validation Board only that a wrong ESD protection chip is used. */
  LPC_IOCON->PIO0_3   &= ~0x1F; 
//  LPC_IOCON->PIO0_3   |= ((0x1<<3)|(0x01<<0));	/* Secondary function VBUS */
  LPC_IOCON->PIO0_3   |= (0x01<<0);			/* Secondary function VBUS */
  LPC_IOCON->PIO0_6   &= ~0x07;
  LPC_IOCON->PIO0_6   |= (0x01<<0);			/* Secondary function SoftConn */

#if POWERDOWN_MODE_USB_WAKEUP
  LPC_USB->DEVCMDSTAT &= ~USB_PLL_ON;	/* NeedClk normal mode(not ALWAYS ON) */
  LPC_SYSCON->USBCLKCTRL = 0x1<<1;  /* USB wakeup interrupt on rising edge of Needclk. */
#endif
  return;
}

#if REMOTE_WAKEUP_ENABLE
/*
 *  Remote Wakeup event processing
 *    This routine is called once the remote wakeup event occurs, any 
 *    external event such as push button. If REMOTE_WAKEUP feature is 
 *    enabled, if the device is connected, and suspended, write SUSP
 *    bit to zero to trigger the remote wakeup event.
 *    
 *    Parameters:      None
 *    Return Value:    None
 */

void RemoteWakeupEvent ( void )
{
  if ( USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP )
  {
    USB_WakeUpCfg( TRUE );
#if LPM_SUPPORT
	if ( LPC_USB->DEVCMDSTAT & USB_LPM_SUS )
	{
	  LPC_USB->DEVCMDSTAT &= ~USB_LPM_SUS;
	}
#endif
	if ( LPC_USB->DEVCMDSTAT & USB_DSUS )
	{
	  /* Only clear the SUS bit to initiate the remote wakeup. */
	  LPC_USB->DEVCMDSTAT &= ~USB_DSUS;
	}
  }
  else
  {
	USB_WakeUpCfg( FALSE );
  }
}
#endif

/*
 *  USB Get EP Command/Status Pointer Function
 *    EPNum.0..3: Address
 *    EPNum.7:    Dir
 *    Parameters:      Logical EP with direction bit
 *    Return Value:    Physical Memory location for EP list command/status
 */
uint32_t USB_GetEPCmdStatusPtr( uint32_t EPNum ) {
  uint32_t addr;
  
  addr = (uint32_t)USB_EPLIST_ADDR;
  addr += ((EPAdr(EPNum)*2)<<2);	/* Based on EPCommand/Status List, all EPs are 
									double buffered except CTRL EP 0, size is 32-bit. */
  return (addr);
}

/*
 *  USB EP List Initialize Function
 *    Called by the USB_Init to initialize USB EPList memory
 *    The example below is the simpliest case, every location is pre-configured
 *    with fixed length for each EP. See USB chapter, Endpoint Command/Status
 *    List for more details.
 *     
 *    Parameters:      None
 *    Return Value:    None
 */
void USB_EPInit( void ) {
  uint32_t *addr, data_ptr;
  uint32_t i, j = 0;
      
  addr = (uint32_t *)USB_EPLIST_ADDR;
  LPC_USB->EPLISTSTART = USB_EPLIST_ADDR;
  data_ptr = USB_EPDATA_ADDR;
  LPC_USB->DATABUFSTART = USB_EPDATA_PAGE;

  /* CTRL, BULK or Interrupt IN/OUT EPs, max EP size is 64 */
  /* For EP0, double buffer doesn't apply to CTRL EPs, but, EP0OUTBuf0 is
  for EP0OUT, EP0OUTBuf1 is for SETUP, EP0INBuf0 is for EP0IN, EP0INTBuf1 is
  reserved. Also note: ACTIVE bit doesn't apply to SETUP and Reserved EP buffer. */
  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
  EPList[j].buf_ptr = data_ptr;
  EPList[j++].buf_length = USB_MAX_PACKET0;
  data_ptr += USB_MAX_PACKET0;
  *addr++ = ((uint16_t)(data_ptr>>6));		/* No length field for SETUP */
  EPList[j].buf_ptr = data_ptr;
  EPList[j++].buf_length = USB_SETUP_PACKET_SIZE;
  /* For easier alignment, the data field is aligned to 64 bytes for all EPs. */
  data_ptr += USB_MAX_PACKET0;

  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
  EPList[j].buf_ptr = data_ptr;
  EPList[j++].buf_length = USB_MAX_PACKET0;
  data_ptr += USB_MAX_PACKET0;
#if 0
  /* EP0 IN second buffer(buffer1) is reserved. */
  *addr++ = (USB_MAX_PACKET0<<16)|((uint16_t)(data_ptr>>6));
#else
  addr++;
#endif
  EPList[j].buf_ptr = data_ptr;
  EPList[j++].buf_length = USB_MAX_PACKET0;
  data_ptr += USB_MAX_PACKET0;

  for ( i = 2; i < USB_EP_NUM; i++ )
  {
	/* Buffer 0 */
#if USB_ISO_COMMUNICATION
	*addr++ = EP_DISABLED|EP_ISO_TYPE|(USB_MAX_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	EPList[j].buf_ptr = data_ptr;
	EPList[j++].buf_length = USB_MAX_ISO_SIZE;
	data_ptr += USB_MAX_ISO_SIZE;
#else
	*addr++ = EP_DISABLED|(USB_MAX_NON_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	EPList[j].buf_ptr = data_ptr;
	EPList[j++].buf_length = USB_MAX_NON_ISO_SIZE;
	data_ptr += USB_MAX_NON_ISO_SIZE;
#endif
	/* Buffer 1 */
#if USB_ISO_COMMUNICATION
	*addr++ = EP_DISABLED|EP_ISO_TYPE|(USB_MAX_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	EPList[j].buf_ptr = data_ptr;
	EPList[j++].buf_length = USB_MAX_ISO_SIZE;
	data_ptr += USB_MAX_ISO_SIZE;
#else
	*addr++ = EP_DISABLED|(USB_MAX_NON_ISO_SIZE<<16)|((uint16_t)(data_ptr>>6));
	EPList[j].buf_ptr = data_ptr;
	EPList[j++].buf_length = USB_MAX_NON_ISO_SIZE;
	data_ptr += USB_MAX_NON_ISO_SIZE;
#endif
  }
  return;
}

/*
 *  USB Initialize Function
 *   Called by the User to initialize USB
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_Init (void) {

#if USB_DEBUG
  DevStatusReset = 0;
  DevStatusSuspend = 0;
  DevStatusResume = 0;
  DevStatusConnect = 0;
  DevStatusConnectChange = 0;
  DevStatusSuspendChange = 0;
  DevStatusLogCount = 0;
#endif

  /* Initialize clock and I/O pins for USB. */
  USBIOClkConfig();

#if LPM_SUPPORT
  /* Link Power Management is supported. */
  LPC_USB->DEVCMDSTAT |= USB_LPM;
  LPC_USB->LPM |= (0x2<<4);		/* RESUME duration. */
#endif

#if USB_FIQ_EVENT
  /* At present, only FRAME(ISO) is routed to FIQ. */
  LPC_USB->INTROUTING = FRAME_INT;		/* SOF uses FIQ */
 
  /* Enable the USB FIQ Interrupt */
  NVIC_EnableIRQ(USB_FIQn);
#endif

#if USE_DOUBLE_BUFFER 
  LPC_USB->EPBUFCFG = 0x3FC;
#endif
  /* Enable the USB Interrupt */
  NVIC_EnableIRQ(USB_IRQn);

  USB_Reset();
  return;
}


/*
 *  USB Connect Function
 *   Called by the User to Connect/Disconnect USB
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USB_Connect (uint32_t con) {
  if ( con )
	LPC_USB->DEVCMDSTAT |= USB_DCON;
  else
	LPC_USB->DEVCMDSTAT &= ~USB_DCON;
  return;
}


/*
 *  USB Reset Function
 *   Called automatically on USB Reset
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_Reset (void) {

  BufferUsed = 0;
  EPActiveFlag = 0;

  /* Initialize EP Command/Status List. */
  USB_EPInit();

  LPC_USB->EPINUSE = 0;
  LPC_USB->EPSKIP = 0;

  LPC_USB->DEVCMDSTAT |= USB_EN;
  /* Clear all EP interrupts, device status, and SOF interrupts. */
  LPC_USB->INTSTAT = 0xC00003FF;
  /* Enable all ten(10) EPs interrupts including EP0, note: EP won't be 
  ready until it's configured/enabled when device sending SetEPStatus command 
  to the command engine. */ 
  LPC_USB->INTEN  = DEV_STAT_INT | 0x3FF | (USB_SOF_EVENT ? FRAME_INT : 0);
  return;
}


/*
 *  USB Suspend Function
 *   Called automatically on USB Suspend
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_Suspend (void) {
  /* Performed by Hardware */
#if POWERDOWN_MODE_USB_WAKEUP
  timer16_0_counter = 0;
  enable_timer16( 0 );
  
  if ( SuspendFlag == 0 ) {
	SuspendFlag = 1;
  }
#endif
}


/*
 *  USB Resume Function
 *   Called automatically on USB Resume
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_Resume (void) {
  /* Performed by Hardware */
#if POWERDOWN_MODE_USB_WAKEUP
  disable_timer16( 0 );
  timer16_0_counter = 0;
  if ( SuspendFlag == 1 ) {
	SuspendFlag = 0;
  }
#endif
}


/*
 *  USB Remote Wakeup Function
 *   Called automatically on USB Remote Wakeup
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_WakeUp (void) {

  if (USB_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
	LPC_USB->DEVCMDSTAT &= ~USB_DSUS;
  }
}


/*
 *  USB Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Enable/Disable
 *    Return Value:    None
 */

void USB_WakeUpCfg (uint32_t cfg) {
#if REMOTE_WAKEUP_ENABLE
  if ( cfg == TRUE )
  {
	LPC_USB->DEVCMDSTAT &= ~USB_PLL_ON;	/* NeedClk functional */
	LPC_SYSCON->USBCLKCTRL = 1;
  }
  else
  {
	LPC_USB->DEVCMDSTAT |= USB_PLL_ON;	/* NeedClk always "1" */
	LPC_SYSCON->USBCLKCTRL = 0;
  }
#else
  cfg = cfg;  /* Not needed */
#endif
}


/*
 *  USB Set Address Function
 *    Parameters:      adr:   USB Address
 *    Return Value:    None
 */

void USB_SetAddress (uint32_t adr) {

  LPC_USB->DEVCMDSTAT &= ~0x7F;
  LPC_USB->DEVCMDSTAT |= (USB_EN | adr);
  return;
}


/*
 *  USB Configure Function
 *    Parameters:      cfg:   Configure/Deconfigure
 *    Return Value:    None
 */

void USB_Configure (uint32_t cfg) {
  if ( cfg )
  {
	/* All the non-zero EPs are configured and enabled per configuration
	descriptor. Enable all interrupts. */
	LPC_USB->INTEN  = DEV_STAT_INT | 0x3FF | (USB_SOF_EVENT ? FRAME_INT : 0);
  }
  else
  {
	/* TBD. if the configuration is FALSE, turn off all the non-zero EPs. Only
	CTRL EP interrupts are enabled. */
	LPC_USB->INTEN  = DEV_STAT_INT | 0x03 | (USB_SOF_EVENT ? FRAME_INT : 0); 
  }
  return;
}


/*
 *  Configure USB Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Endpoint Descriptor
 *    Return Value:    None
 */

void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR *pEPD) {
  return;
}



/*
 *  Set Direction for USB Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USB_DirCtrlEP (uint32_t dir) {
  return;
}

/*
 *  Deactivate USB Endpoint
 *    Set the EP bits in the SKIP register and until the bits are
 *    cleared by the H/W. Clear the EP interrupts as well.
 *    It's used for non-zero EPs. For EP0 IN and OUT, clear
 *    the ACTIVE bit in EP Command/Status list will serve the purpose. 
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DeactivateEP (uint32_t EPNum) {
  uint32_t epbit;

  epbit = 0x1 << EPAdr(EPNum);
  LPC_USB->EPSKIP |= epbit;
  while ( LPC_USB->EPSKIP & epbit );
  LPC_USB->INTSTAT = epbit;			/* Clear EP interrupt(s). */
  return;
}

/*
 *  Enable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_EnableEP (uint32_t EPNum) {
  uint32_t *addr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
  *addr &= ~EP_DISABLED;
  if ( EPNum & 0x0F )	/* Non-zero EPs */ 	
  {
	if ( EPNum & 0x80 )		/* For non-zero IN */	
	{
	  if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	  {
		/* For non-zero double buffer EPs, clear EP_DISABLED to both buffer. */
		addr++;
		*addr &= ~EP_DISABLED;
	  }
	}
	else
	{
	  /* For non-zero EP OUT, in addition to clear EP_DISABLED bits,
	  set the ACTIVE bit indicating that EP is ready to read. For
	  double buffered EPs, set ACTIVE bit and clear EP_DISABLED bit
	  for both buffer0 and 1. */
	  *addr |= BUF_ACTIVE;
	  if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	  {
		/* For double buffer. */
		addr++;
		*addr &= ~EP_DISABLED;
		*addr |= BUF_ACTIVE;
	  }
	}
  }
  return;
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_DisableEP (uint32_t EPNum) {
  uint32_t *addr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
  if ( EPNum & 0x0F )	/* Non-zero EPs only, EP0 should never be disabled. */ 	
  {
	*addr |= EP_DISABLED;
	/* For non-zero EPs, if double buffer is used, disable both EP buffers. */
	if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	{
	  addr++;
	  *addr |= EP_DISABLED;
	}
  }
  return;
}


/*
 *  Toggle Reset USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ResetEP (uint32_t EPNum) {
  uint32_t *addr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
  /* Based on EPInUse register to decide which buffer needs to toggle 
  reset. When this happens, the STALL bits need to be cleared for both
  buffer 0 and 1. */
  *addr &= ~EP_STALL;
  *(addr+2) &= ~EP_STALL;
  if ( LPC_USB->EPINUSE & (0x1<<EPAdr(EPNum)) )
  {
	addr++;
  }
  *addr |= EP_RESET;
  return;
}


/*
 *  Set Stall for USB Endpoint
 *    STALL bit(s) can not be set until ACTIVE bit(s) are cleared.
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_SetStallEP (uint32_t EPNum) {
  uint32_t *addr;

  if ( (EPNum & 0x0F) == 0 )
  {
	/* For EP0 IN or OUT, simply clear the ACTIVE and set the STALL bit. */
	addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
	/* STALL bit can't be set until ACTIVE bit is gone. */
	if ( *addr & BUF_ACTIVE )
	{
	  USB_DeactivateEP (EPNum);
	}
	*addr |= EP_STALL;
  }
  else
  {
	/* For non-zero EPs, deactivate the EP first, make buffer inactive
	before setting the STALL bit. It applies to both buffer 0 and 1 if
	double buffer is enabled. */
	addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
	if ( *addr & BUF_ACTIVE )
	{
	  USB_DeactivateEP (EPNum);
	}
	*addr |= EP_STALL;		/* STALL on buffer 0. */		
	if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	{
	  /* If double buffer is enabled, STALL on buffer 1. */ 
	  addr++;
	  if ( *addr & BUF_ACTIVE )
	  {
		USB_DeactivateEP (EPNum);
	  }
	  *addr |= EP_STALL;
	}
  }
  return;
}


/*
 *  Clear Stall for USB Endpoint
 *    For non-zero EP OUT, Toggle Reset should happen once the STALL bit(s)
 *    are cleared. At mean time, ACTIVE bit(s) and data length need to be
 *    reinitialized for EP read. 
 *    For non-zero EP IN, when Write EP occurs while STALL bit is set, 
 *    EPActiveFlag will be set, inside ClearSTALL, when this flag is set, it's
 *    time to set the ACTIVE bit so that the data buffer is ready to write data 
 *    to the EPs.
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USB_ClrStallEP (uint32_t EPNum) {
  uint32_t *addr;

  if ( (EPNum & 0x0F) == 0 )
  {
	/* For EP0 IN and OUT, simply clear the STALL bit. */
	addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
	*addr &= ~EP_STALL;
  }
  else
  {
	addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
	/* For non-zero EPs if double buffer, both STALL bits should be cleared. */
	*addr &= ~EP_STALL;
	*(addr+1) &= ~EP_STALL;	

	/* Based on EPInUse register to decide which buffer needs to toggle reset. */
	if ( LPC_USB->EPINUSE & (0x1<<EPAdr(EPNum)) )
	{
	  /* Buffer 1 is in use. Toggle Reset Buffer 1, otherwise, toggle Reset
	  buffer 0. */
	  addr++;
	}
	*addr |= EP_RESET;
	if ( !(EPNum & 0x80) )
	{
	  /* For non-zero EP OUT, ACTIVE bit and length field need to 
	  be set again after clearing STALL. */
	  *addr &= ~(PKT_LNGTH_MASK<<16);
#if USB_ISO_COMMUNICATION
	  *addr |= ((USB_MAX_ISO_SIZE<<16)| BUF_ACTIVE);
#else
	  *addr |= ((USB_MAX_NON_ISO_SIZE<<16)| BUF_ACTIVE);
#endif
	}
	else
	{
	  /* For non-zero EP IN, the EPActiveFlag will be set when WriteEP() happens 
	  while STALL is set. If so, when ClearSTALL happens, set the ACTIVE bit that
	  data buffer is ready to write data to the EPs. */
	  if ( EPActiveFlag & (0x1U << EPAdr(EPNum)) )
	  {
		*addr |= BUF_ACTIVE;
		EPActiveFlag &= ~(0x1U << EPAdr(EPNum));
	  }
	}
  }
  return;
}

/*
 *  Read USB Endpoint SETUP Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

uint32_t USB_ReadSetupEP (uint32_t EPNum, uint8_t *pData) {
  uint32_t cnt, n;
  uint32_t *addr;
  uint32_t *dataptr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );

  /* Check/Clear STALL on both EP0 IN and OUT when SETUP is received. */
  if ( (*addr & EP_STALL) || ((*addr+2) & EP_STALL) )
  {
	*addr &= ~EP_STALL;
	*(addr+2) &= ~EP_STALL;
  }

  cnt = USB_SETUP_PACKET_SIZE;
  dataptr = (uint32_t *)EPList[1].buf_ptr;
  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed uint32_t *)pData) = *((__packed uint32_t *)dataptr);
    pData += 4;
	dataptr++;
  }

  addr++;		/* Use EP0 buffer 1 for SETUP packet */
  /* Fixed Command/Status location(EPList[1] for SETUP. Reset buffer pointer 
  field, SETUP length is fixed with eight bytes. */
  *addr &= ~0x3FFFFFF;		
  *addr |= (uint16_t)((EPList[1].buf_ptr)>>6);
  return (cnt);
}

/*
 *  Read USB Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

uint32_t USB_ReadEP (uint32_t EPNum, uint8_t *pData) {
  uint32_t cnt, n;
  uint32_t *addr;
  uint32_t *dataptr;
  uint32_t index = 0;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
  index = EPAdr(EPNum) * 2;		/* Double buffer is considered. */
  if ( EPNum & 0x0F )
  {
    /* For EP0 IN/OUT, there is no double buffer. For non-zero
	EP, double buffer is considered. If BufferUsed bit mask is set,
	buffer0 is used, switch to buffer 1, index needs to be changed
	accordingly too. */
	if ( BufferUsed & (0x1U << EPAdr(EPNum)) )
	{
	  addr++;		/* Use buffer 1 */
	  index++;
	}
  }
  	
  cnt = (*addr >> 16) & PKT_LNGTH_MASK;
  /* The NBytes field decrements by H/W with the packet byte each time. */
  cnt = EPList[index].buf_length - cnt;
  dataptr = (uint32_t *)EPList[index].buf_ptr;
  for (n = 0; n < (cnt + 3) / 4; n++) {
	*((__packed uint32_t *)pData) = *((__packed uint32_t *)dataptr);
	pData += 4;
	dataptr++;
  }

  /* Clear buffer after EP read, reset EP length and buffer pointer field */
  *addr &= ~0x3FFFFFF;
  if ( (EPNum & 0x0F) == 0 )
  {
	/* EP0 is single buffer only. */		
	*addr |= ((EPList[index].buf_length<<16) 
		| (uint16_t)((EPList[index].buf_ptr)>>6) | BUF_ACTIVE);
  }
  else
  {
	/* Toggle buffer if double buffer is used for non-zero EPs.  */
	if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	{
	  BufferUsed = LPC_USB->EPINUSE;
	  if ( BufferUsed & (0x1U << EPAdr(EPNum)) )
	  {
		addr++;		/* Set buffer 1 ACTIVE */
		index++;
	  }
	  else
	  {
		addr--;		/* Set buffer 0 ACTIVE */
		index--;
	  }
	}
	*addr |= ((EPList[index].buf_length<<16) 
		| (uint16_t)((EPList[index].buf_ptr)>>6) | BUF_ACTIVE);
  }
  return (cnt);
}


/*
 *  Write USB Endpoint Data
 *    For EP0 OUT, ACTIVE bits will not be set until SETUP and
 *	  EP0 IN have been received. 
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

uint32_t USB_WriteEP (uint32_t EPNum, uint8_t *pData, uint32_t cnt) {
  uint32_t n, index;
  uint32_t *addr;
  uint32_t *dataptr;

  addr = (uint32_t *)USB_GetEPCmdStatusPtr( EPNum );
  index = EPAdr(EPNum) * 2;		/* Double buffer is considered. */
  if ( !(EPNum & 0x0F) )
  {
	/* When EP0 IN is received, set ACTIVE bit on both EP0 IN
	and OUT. */
	*(addr-2) |= BUF_ACTIVE;	/* Set ACTIVE bit on EP0 OUT */
  }
  else
  {
	/* For non-zero EPs, if double buffer is used and EPInUse is set, buffer0 
	is used, otherwise, buffer1 is used. */
	if ( LPC_USB->EPBUFCFG & (0x1U << EPAdr(EPNum)) )
	{
	  if ( LPC_USB->EPINUSE & (0x1U << EPAdr(EPNum)) )
	  {
		addr++;		/* move to buffer1 address in EP command/status list. */
		index++;
	  }
	}
  }

  /* Get EP command/status List, update the length field and data pointer. */
  *addr &= ~0x3FFFFFF;
  cnt &= PKT_LNGTH_MASK;
  *addr |= (cnt<<16)|(uint16_t)((EPList[index].buf_ptr)>>6);

  dataptr = (uint32_t *)EPList[index].buf_ptr;
  /* Stuff the data first, whether send out or not(set ACTIVE bit) is based 
  on STALL condition. */
  for (n = 0; n < (cnt + 3) / 4; n++) {
    *((__packed uint32_t *)dataptr) = *((__packed uint32_t *)pData);
    pData += 4;
	dataptr++;
  }

  if ( (*addr & EP_STALL) && (EPNum & 0x0F) )
  {
	/* This is for MSC class when STALL occurs and non-zero EPs,
	set the ACTIVE flag, but don't do anything until ClearFeature
	to clear STALL, then tranfer the data. */
	EPActiveFlag |= (0x1U << EPAdr(EPNum));
	return (cnt);
  }
  *addr |= BUF_ACTIVE;
  return (cnt);
}

/*
 *  Get USB Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

uint32_t USB_GetFrame (void) {
  return( LPC_USB->INFO & 0x7FF);
}

#if USB_FIQ_EVENT
/* USB FIQ Interrupt Service Routine for SOF */
void USB_FIQHandler( void )
{
  uint32_t disr;

  disr = LPC_USB->INTSTAT;                      /* Device Interrupt Status */
  LPC_USB->INTSTAT = disr;

  if (disr & FRAME_INT) {
	SOFFIQCount++;
  }
  return;
}
#endif

/*
 *  USB Interrupt Service Routine
 *    All the device status, EP, and SOF interrupts are handled here.
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_IRQHandler (void)
{
  uint32_t disr, val, n, m;

  disr = LPC_USB->INTSTAT;         /* Get Interrupt Status and clear immediately. */
  LPC_USB->INTSTAT = disr;

  /* Device Status Interrupt (Reset, Connect change, Suspend/Resume) */
  if (disr & DEV_STAT_INT) {
    val = LPC_USB->DEVCMDSTAT;       /* Device Status */
#if USB_DEBUG
	DevStatusLog[DevStatusLogCount++] = val;
	if ( DevStatusLogCount >= LOG_COUNT )
	{
	  DevStatusLogCount = 0;
	}
	if ( val & USB_DRESET_C )
	  DevStatusReset++;
	if ( val & USB_DCON_C )
	  DevStatusConnectChange++;
	if ( val & USB_DSUS_C ) {
	  DevStatusSuspendChange++;
	if ( val & USB_DCON )
	  DevStatusConnect++;
	if ( val & USB_DSUS )
	  DevStatusSuspend++;
	else
	  DevStatusResume++;
	}
#endif
    if (val & USB_DRESET_C) {               /* Reset */
	  LPC_USB->DEVCMDSTAT |= USB_DRESET_C;
      USB_Reset();
#if   USB_RESET_EVENT
      USB_Reset_Event();
#endif
    }
    if (val & USB_DCON_C) {                 /* Connect change */
	  LPC_USB->DEVCMDSTAT |= USB_DCON_C;
#if   USB_POWER_EVENT
      USB_Power_Event(val & USB_DCON);
#endif
    }
    if (val & USB_DSUS_C) {                 /* Suspend/Resume */
	  LPC_USB->DEVCMDSTAT |= USB_DSUS_C;
      if (val & USB_DSUS) {                 /* Suspend */
        USB_Suspend();
#if     USB_SUSPEND_EVENT
        USB_Suspend_Event();
#endif
      } else {                              /* Resume */
        USB_Resume();
#if REMOTE_WAKEUP_ENABLE
        LPC_SYSCON->USBCLKCTRL = 0;
#endif
#if     USB_RESUME_EVENT
        USB_Resume_Event();
#endif
      }
    }
#if LPM_SUPPORT
	if (val & USB_LPM_SUS) {                /* LPM Suspend */
      DevStatusLPMSuspend++;
    }
	if (val & USB_LPM_REWP) {                /* LPM Suspend */
      DevStatusLPMRemoteWakeup++;
    }
#endif
    goto isr_end;
  }

#if USB_SOF_EVENT
  /* Start of Frame Interrupt */
  if (disr & FRAME_INT) {
    USB_SOF_Event();
	SOFIRQCount++;
  }
#endif

#if USB_ERROR_EVENT
  /* NO error interrupt anymore, below code can be used
  as example to get error status from USBInfo register */
#if USB_DEBUG
    DeviceErrorCode = (LPC_USB->Info >> 11) & 0x0F;
#endif
#endif

  /* Endpoint's Interrupt */
  if (disr & 0x3FF) {
	/* if any of the EP0 through EP9 is set, or bit 0 through 9 on disr */
    for (n = 0; n < USB_EP_NUM; n++) {      /* Check All Endpoints */
	  if (disr & (1 << n)) {
        m = n >> 1;
        if ((n & 1) == 0) {                 /* OUT Endpoint */
          if (n == 0) {                     /* Control OUT Endpoint */
            if ( LPC_USB->DEVCMDSTAT & USB_SETUP_RCVD ) {
 											/* Setup packet is received. */
              if (USB_P_EP[0]) {
                USB_P_EP[0](USB_EVT_SETUP);
				LPC_USB->DEVCMDSTAT |= USB_SETUP_RCVD;
                continue;
              }
            }
          }
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_OUT);
          }
        } else {                            /* IN Endpoint */
          if (USB_P_EP[m]) {
            USB_P_EP[m](USB_EVT_IN);
          }
        }
      }
    }
  }
isr_end:
  return;
}

#if POWERDOWN_MODE_USB_WAKEUP
/*
 *  USB Wakeup Interrupt Service Routine
 *    A dedicated USB wakeup interrupt is handled in LPC11Uxx.
 *    In earlier version, USB interrupt can only wake up MCU
 *    normal sleep mode. When RESUME occurs from the host, this dedicated 
 *    USB wakeup interrupt can be used to wake up MCU from either sleep or 
 *    deep sleep mode.
 *    Parameters:      None
 *    Return Value:    None
 */

void USBWakeup_IRQHandler(void) 
{
#if USB_DEBUG
  uint32_t i, j;

  LPC_GPIO->DIR[1] = 0x1<<1;		/* P1.16..23 defined as Outputs */
  LPC_GPIO->CLR[1] = 0x1<<1;		/* turn off all the LEDs */
  /* Wake up, blink 20 times, and back to normal operation. 
  If suspend and resume anagin, the same sequence will apply. */
  for ( j = 0; j < 5; j++ )
  {
	for ( i = 0; i < 0x200000; i++ );
	LPC_GPIO->SET[1] = 0x1<<1;
	for ( i = 0; i < 0x200000; i++ );
	LPC_GPIO->CLR[1] = 0x1<<1;
  }
#endif
  /* Below clear pending is not needed in C1US as NeedClk can be configured
  as rising or falling edge, not level trigger anymore. */  
//  NVIC_ClearPendingIRQ(USBWakeup_IRQn);   /* If pending, clear USB Activity interrupt. */ 
  NVIC_DisableIRQ(USBWakeup_IRQn);        /* Disable USB activity interrupt */
  if ( WakeupFlag == 1 )
  {
	WakeupFlag = 0;
	USBWakeupInterruptFlag = 1;
  }
  return;
}
#endif

