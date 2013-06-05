/****************************************************************************
 *   $Id:: sci_test.c 5143 2010-10-07 23:06:18Z usb00423                  $
 *   Project: NXP LPC11Uxx Smart Card validation
 *
 *   Description:
 *     This file contains smart card test module main entry.
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

* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' 
* relevant copyright in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.

****************************************************************************/

#include <string.h>
#include "LPC11Uxx.h"
#include "config.h"
#include "usart.h"
#include "timer32.h"
#include "misc_gpio.h"

static volatile uint8_t SendBuffer[USART_BUFSIZE];
static volatile uint8_t RNDc[8];
static uint32_t BaudTable[] =
{
  9600,
  13950,
  27900,
  55800,
  111600,
  115200,
  192000,
  223200
};

static uint8_t TABaudTable[] =
{
  0x11, /* 9600 */
  0x92, /* 13,950 */
  0x93, /* 27,900 */
  0x94, /* 55,800 */
  0x95, /* 111,600 */
  0x18, /* 115,200 */
  0x19, /* 192,000 */
  0x96  /* 223,200 */
};

static uint8_t NibbleBitCount[] = {0, 1, 1, 2, 1 , 2, 2, 3, 1, 2, 2, 3, 2 , 3, 3, 4};

/*****************************************************************************
** Function name:		TAToBaud
**
** Descriptions:		Converts TA byte to corresponding baudrate.
**
** parameters:			ta - byte value TA
** Returned value:		baudrate
**                  	*index - pointer to index in baud tables (if not null)
** 
*****************************************************************************/
static uint32_t TAToBaud(uint8_t ta, int *index)
{
  int i;

  for (i = 0;i < sizeof(TABaudTable) / sizeof(uint8_t);i++)
    if (TABaudTable[i] == ta)
    {
      if (index) *index = i;
      return BaudTable[i];
    }

  return BaudTable[0];
} 

/*****************************************************************************
** Function name:		BaudToTA
**
** Descriptions:		Converts baudrate to corresponding TA byte.
**
** parameters:			baudrate - baud rate to convert
** Returned value:		0 if no matches, otherwise TA byte value
**                  	*index - pointer to index in baud tables (if not null)
** 
*****************************************************************************/
volatile static uint8_t BaudToTA(uint32_t baudrate, int *index)
{
  int i;

  for (i = 0;i < sizeof(TABaudTable) / sizeof(uint8_t);i++)
    if (BaudTable[i] == baudrate)
    {
      if (index) *index = i;
      return TABaudTable[i];
    }

  return 0;
}

/*****************************************************************************
** Function name:		wait_for_extraction
**
** Descriptions:		Wait for card to be extracted. 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
static void wait_for_extraction(void)
{
  /* Turn off clock to device, wait in reset */
  CardClockOff(); /* reset and stop clock */
  USARTIgnore(); /* Make sure RX interrupt does not respond to garbage on data line */
  CardOff();;
  while (isCardInserted()) ; /* Wait for card to be extracted */
}


/*******************************************************************************/
/* Main Program */
int main (void) 
{
  int i;
  int BaudIndex = 0;
  int ExpectedResponse = 0;

  SystemCoreClockUpdate();

  /* Configure USART for the SCI */
  USARTIgnore();
  USARTInit(CARD_BAUD_RATE);	/* baud rate setting for communication with Smart Card */

  /* Ensure timer has power before using the delayMs() routines */
#if (TIME_SOURCE == 0)
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);
#else
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);
#endif

  MiscGpioInit();

  /* Storage of random number used in session key sharing */
  memset((void *)RNDc, 0, sizeof(RNDc));

  while ( 1 )
  {
    /* Turn off clock to device, wait in reset, make sure RX cannot receive garbage */
    USARTIgnore(); /* We don't want random noise from insertion */
    CardOff();
    BaudIndex = 0;
    USARTConfig(BaudTable[BaudIndex], 8, PARITY_EVEN, 1, 4);
    while (!isCardInserted()) ; /* Wait for card to be inserted */
    delay32Ms(TIME_SOURCE, T_INSERTION_DEBOUNCE); /* Verify the card is still there */
    if (!isCardInserted())
      continue;

    /* Indicate that the card has been inserted */
    ShowActivity();
    CardPowerOn(); /* Power on */
    delay32Ms(TIME_SOURCE, T_Ta); /* Wait for power to settle */
    ShowActivity();
    CardClockOn(); /* Start clock at Ta */
    delay32Ms(TIME_SOURCE, T_Ta_Tb); /* Wait until Tb */
    ShowActivity();

    /* Come out of reset, get ready for ATR */
    USARTAccept();
    memset((void *)USARTBuffer, 0, sizeof(USARTBuffer));
    CardDeassertReset();
    delay32Ms(TIME_SOURCE, T_Tb_Tc + (20 * T_BYTE_TIME)); /* We expect ~20ish bytes */
    ShowActivity();
    /* Show the ATR response */
    /* Add TS + T0 + (# bits in T0) + (# historical bytes in T0) */
    ExpectedResponse = 2 + NibbleBitCount[USARTBuffer[1] >> 4] + (USARTBuffer[1] & 0xf);
    if ((USARTCount != ExpectedResponse) || /* Should have got ATR by now, start over if not */
        (USARTBuffer[0] != 0x3B))
    {
      wait_for_extraction();
      continue;
    }
    USARTClear();

    /* is it simply 9600 baud? unsupported codes fall-through*/
    if (TAToBaud(USARTBuffer[2], &BaudIndex) != BaudTable[0])
    {
      int j;
      /* Send the PPS command to start new baud rate */
      i = 0;
      SendBuffer[i++] = 0xff; /* PPSS */
      SendBuffer[i++] = 0x10; /* PPS0 */
      SendBuffer[i++] = USARTBuffer[2]; /* PPS1 */
      SendBuffer[i] = 0;
      for (j = 0;j < i;j++)
        SendBuffer[i] ^= SendBuffer[j]; /* PCK */
      i++;
      ShowActivity();
      USARTSend((uint8_t *)SendBuffer, i);
      delay32Ms(TIME_SOURCE, T_CMD_DONE + ((i + 11) * T_BYTE_TIME)); /* Wait for command to complete */
      ShowActivity();
      if ((USARTCount == i) &&
          !memcmp((void *)SendBuffer, (void *)USARTBuffer, i))
      {
        USARTConfig(BaudTable[BaudIndex], 8, PARITY_EVEN, 1, 4);
      }
      else
      {
        BaudIndex = 0;
      }
      USARTClear();
    }

    /* Send the start session command (0x8084) to receive random number for key transfer */
    i = 0;
    SendBuffer[i++] = 0x80; /* CLA */
    SendBuffer[i++] = 0x84; /* INS */
    SendBuffer[i++] = 0x00; /* P1 */
    SendBuffer[i++] = 0x00; /* P2 */
    SendBuffer[i++] = 0x08; /* P3 */
    ShowActivity();
    USARTSend((uint8_t *)SendBuffer, i);
    delay32Ms(TIME_SOURCE, T_CMD_DONE + ((i + 11) * T_BYTE_TIME)); /* Wait for command to complete */
    ShowActivity();
    if ((USARTCount == 11) && (USARTBuffer[0] == 0x84) && (USARTBuffer[9] == 0x90) && (USARTBuffer[10] == 0x00))
    {
      memcpy((void *)RNDc, (void *)&USARTBuffer[1], sizeof(RNDc));
    }
    USARTClear();

    delay32Ms(TIME_SOURCE, T_IDLE_POWER_DOWN); /* Wait then turn off card and wait for extraction */

    wait_for_extraction();
  }
}

