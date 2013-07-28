/****************************************************************************
 *   $Id:: io_event.c 7410 2011-05-26 16:37:15Z usb00423                    $
 *   Project: NXP LPC11xx GPIO example
 *
 *   Description:
 *     This file contains GPIO code example which include GPIO 
 *     initialization, GPIO interrupt handler, and related APIs for 
 *     GPIO access. These I/O examples are used as remote wakeup source 
 *     for USB only.
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
#include "LPC11Uxx.h"			/* LPC11xx Peripheral Registers */
#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "io_event.h"

volatile uint32_t int0_counter = 0;
#if USB_WORKAROUND
volatile uint32_t VBusLow = 0;
#endif

/*****************************************************************************
** Function name:		FLEX_INT0_IRQHandler
**
** Descriptions:		Use one GPIO pin as interrupt source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void FLEX_INT0_IRQHandler(void)
{
#if REMOTE_WAKEUP_ENABLE
  RemoteWakeupEvent();
#endif
#if USB_WORKAROUND
  VBusLow = 1;
#endif
  if ( LPC_GPIO_PIN_INT->IST & (0x1<<0) )
  {
	int0_counter++;
	LPC_GPIO_PIN_INT->IST |= (0x1<<0);
  }
  return;
}

#if USB_WORKAROUND
/*****************************************************************************
** Function name:		SetVBUSInterrupt
**
** Descriptions:		Set VBUS debounce as interrupt
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SetVBUSInterrupt (void)
{
  /* Enable AHB clock to the GPIO domain. */
  if ( !(LPC_SYSCON->SYSAHBCLKCTRL & (1<<6)) )
  {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
  }

  /* Enable AHB clock to the FlexInt, GroupedInt domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<19) | (1<<23) | (1<<24));

  /* Route p0.3(VBUS) to FLEX0 */
  LPC_SYSCON->PINTSEL[0] = 3;	/* P0.3  */

  /* use port0_3 as input event, interrupt test. */
  GPIOSetDir( PORT0, 3, 0 );
  /* INT0, edge trigger, falling edge. */
  GPIOSetInterrupt( 0, 0, 0 );
  GPIOIntEnable( 0, 0 );

  NVIC_EnableIRQ(FLEX_INT0_IRQn);
  return;
}
#endif

#if REMOTE_WAKEUP_ENABLE
/*****************************************************************************
** Function name:		SetRemoteWakeupSource
**
** Descriptions:		Set Remote Wakeup Source
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void SetRemoteWakeupSource (void)
{
 /* Enable AHB clock to the GPIO domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

  /* Enable AHB clock to the FlexInt, GroupedInt domain. */
  LPC_SYSCON->SYSAHBCLKCTRL |= ((1<<19) | (1<<23) | (1<<24));

  /* Route p0.1 to FLEX0 */
  LPC_SYSCON->PINTSEL[0] = 1;	/* P0.1  */

  /* use port0_1 as input event, interrupt test. */
  GPIOSetDir( PORT0, 1, 0 );
  /* INT0, edge trigger, falling edge. */
  GPIOSetInterrupt( 0, 0, 0 );
  GPIOIntEnable( 0, 0 );

  NVIC_EnableIRQ(FLEX_INT0_IRQn);
  return;
}
#endif

/*****************************************************************************
** Function name:		GPIOSetInterrupt
**
** Descriptions:		Set interrupt sense, event, etc.
**						sense: edge or level, 0 is edge, 1 is level 
**						event/polarity: 0 is active low/falling, 1 is high/rising.
**
** parameters:			channel #, sense, polarity(event)
** Returned value:		None
** 
*****************************************************************************/
void GPIOSetInterrupt( uint32_t ch_num, uint32_t sense, uint32_t event )
{
  if ( sense == 0 )
  {
	LPC_GPIO_PIN_INT->ISEL &= ~(0x1<<ch_num);	/* Edge trigger */
	if ( event == 0 )
	{
	  LPC_GPIO_PIN_INT->IENF |= (0x1<<ch_num);	/* faling edge */	
	}
	else
	{
	  LPC_GPIO_PIN_INT->IENR |= (0x1<<ch_num);	/* Rising edge */
	} 
  }
  else
  {
	LPC_GPIO_PIN_INT->ISEL |= (0x1<<ch_num);	/* Level trigger. */
	if ( event == 0 )
	{
	  LPC_GPIO_PIN_INT->IENF &= ~(0x1<<ch_num);	/* active-low interrupt enabled. */
	}
	else
	{
	  LPC_GPIO_PIN_INT->IENF |= (0x1<<ch_num);	/* active-high interrupt enabled. */
	}
  }
  return;
}

/*****************************************************************************
** Function name:		GPIOIntEnable
**
** Descriptions:		Enable Interrupt Mask for a port pin.
**
** parameters:			channel num, event(0 is falling edge, 1 is rising edge)
** Returned value:		None
** 
*****************************************************************************/
void GPIOIntEnable( uint32_t ch_num, uint32_t event )
{
  if ( event == 0 )
  {
	LPC_GPIO_PIN_INT->SIENF = (0x1<<ch_num);	/* faling edge enabled. */	
  }
  else
  {
	LPC_GPIO_PIN_INT->SIENR = (0x1<<ch_num);	/* Rising edge enabled */
  } 
  return;
}

/*****************************************************************************
** Function name:		GPIOGetPinValue
**
** Descriptions:		Read Current state of port pin, PIN register value
**
** parameters:			port num, bit position
** Returned value:		None
**
*****************************************************************************/
uint32_t GPIOGetPinValue( uint32_t portNum, uint32_t bitPosi )
{
  uint32_t regVal = 0;	

  if( bitPosi < 0x20 )
  {	
	if ( LPC_GPIO->PIN[portNum] & (0x1<<bitPosi) )
	{
	  regVal = 1;
	}
  }
  else if( bitPosi == 0xFF )
  {
	regVal = LPC_GPIO->PIN[portNum];
  }
  return ( regVal );		
}

/*****************************************************************************
** Function name:		GPIOSetBitValue
**
** Descriptions:		Set/clear a bit in a specific position
**
** parameters:			port num, bit position, bit value
** 						
** Returned value:		None
**
*****************************************************************************/
void GPIOSetBitValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal )
{
  if ( bitVal )
  {
	LPC_GPIO->SET[portNum] = 1<<bitPosi;
  }
  else
  {
	LPC_GPIO->CLR[portNum] = 1<<bitPosi;
  }
  return;
}

/*****************************************************************************
** Function name:		GPIOSetDir
**
** Descriptions:		Set the direction in GPIO port
**
** parameters:			portNum, bit position, direction (1 out, 0 input)
** Returned value:		None
**
*****************************************************************************/
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
  if( dir )
  {
	LPC_GPIO->DIR[portNum] |= (1<<bitPosi);
  }
  else
  {
	LPC_GPIO->DIR[portNum] &= ~(1<<bitPosi);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
