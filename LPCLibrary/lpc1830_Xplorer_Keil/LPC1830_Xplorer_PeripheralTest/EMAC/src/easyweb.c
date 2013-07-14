/******************************************************************
 *****                                                        *****
 *****  Name: easyweb.c                                       *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: implements a dynamic HTTP-server by using       *****
 *****        the easyWEB-API                                 *****
 *****                                                        *****
 ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lpc18xx.H"                    /* lpc43xx definitions                */
#include "lpc18xx_scu.h"
#include "lpc18xx_adc.h"
#include "lpc18xx_cgu.h"
#include "lpc18xx_libcfg_default.h"
#include "EMAC.h"         // Keil: *.c -> *.h    // ethernet packet driver
#define extern            // Keil: Line added for modular project management

#include "easyweb.h"

#include "tcpip.h"        // Keil: *.c -> *.h    // easyWEB TCP/IP stack
#include "webpage.h"                             // webside for our HTTP server (HTML)



/* Example group ----------------------------------------------------------- */
/** @defgroup EMAC_Easy_Web	Easy_Web
 * @ingroup EMAC_Examples
 * @{
 */


unsigned int res = 0;
unsigned int pagecounter = 100;
int led_d2;
int led_d1;

#define IRC_OSC     (12000000UL)        /* Internal RC oscillator frequency   */
uint32_t SystemFrequency = IRC_OSC * 6; /*!< System Clock Frequency (Core Clock)  */
uint32_t AHBSystemFrequency = IRC_OSC * 6;

extern void TCPClockHandler(void);

#define FRAMESIZE	60	//1514

void init_memory(void) {
   int i;
   char * pMem = (char *)0x20008000;

   for (i=0;i<10000;i++) {
	  (*pMem++) = 0x00;

   }
}

void Emac_init(void)
{
	CGU_EnableEntity(CGU_CLKSRC_32KHZ_OSC, ENABLE);			// Enable 32 kHz & 1 kHz on osc32k

	CGU_EnableEntity(CGU_CLKSRC_ENET_RX_CLK, ENABLE);
	CGU_EnableEntity(CGU_CLKSRC_ENET_TX_CLK, ENABLE);
#if MII
	CGU_EntityConnect(CGU_CLKSRC_ENET_TX_CLK, CGU_BASE_PHY_TX);
	CGU_EntityConnect(CGU_CLKSRC_ENET_RX_CLK, CGU_BASE_PHY_RX);
#else
	CGU_SetDIV(CGU_CLKSRC_IDIVA, 1);
	CGU_EntityConnect(CGU_CLKSRC_ENET_TX_CLK, CGU_CLKSRC_IDIVA);
	CGU_EntityConnect(CGU_CLKSRC_IDIVA, CGU_BASE_PHY_TX);
	CGU_EntityConnect(CGU_CLKSRC_IDIVA, CGU_BASE_PHY_RX);
#endif
  TCPLowLevelInit();
  HTTPStatus = 0;                                // clear HTTP-server's flag register
  TCPLocalPort = TCP_PORT_HTTP;                  // set port we want to listen to
}

// This function implements a very simple dynamic HTTP-server.
// It waits until connected, then sends a HTTP-header and the
// HTML-code stored in memory. Before sending, it replaces
// some special strings with dynamic values.
// NOTE: For strings crossing page boundaries, replacing will
// not work. In this case, simply add some extra lines
// (e.g. CR and LFs) to the HTML-code.

void Test_ethernet()
{
 if (!(SocketStatus & SOCK_ACTIVE)) TCPPassiveOpen();   // listen for incoming TCP-connection
    DoNetworkStuff();                                      // handle network and easyWEB-stack events
    HTTPServer();
}

void HTTPServer(void)
{
  if (SocketStatus & SOCK_CONNECTED)             // check if somebody has connected to our TCP
  {
    if (SocketStatus & SOCK_DATA_AVAILABLE)      // check if remote TCP sent data
      TCPReleaseRxBuffer();                      // and throw it away

    if (SocketStatus & SOCK_TX_BUF_RELEASED)     // check if buffer is free for TX
    {
      if (!(HTTPStatus & HTTP_SEND_PAGE))        // init byte-counter and pointer to webside
      {                                          // if called the 1st time
        HTTPBytesToSend = sizeof(WebSide) - 1;   // get HTML length, ignore trailing zero
        PWebSide = (unsigned char *)WebSide;     // pointer to HTML-code
      }

      if (HTTPBytesToSend > MAX_TCP_TX_DATA_SIZE)     // transmit a segment of MAX_SIZE
      {
        if (!(HTTPStatus & HTTP_SEND_PAGE))           // 1st time, include HTTP-header
        {
          memcpy(TCP_TX_BUF, GetResponse, sizeof(GetResponse) - 1);
          memcpy(TCP_TX_BUF + sizeof(GetResponse) - 1, PWebSide, MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1);
          HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
          PWebSide += MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
        }
        else
        {
          memcpy(TCP_TX_BUF, PWebSide, MAX_TCP_TX_DATA_SIZE);
          HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE;
          PWebSide += MAX_TCP_TX_DATA_SIZE;
        }

        TCPTxDataCount = MAX_TCP_TX_DATA_SIZE;   // bytes to xfer
        InsertDynamicValues();                   // exchange some strings...
        TCPTransmitTxBuffer();                   // xfer buffer
      }
      else if (HTTPBytesToSend)                  // transmit leftover bytes
      {
        memcpy(TCP_TX_BUF, PWebSide, HTTPBytesToSend);
        TCPTxDataCount = HTTPBytesToSend;        // bytes to xfer
        InsertDynamicValues();                   // exchange some strings...
        TCPTransmitTxBuffer();                   // send last segment
        TCPClose();                              // and close connection
        HTTPBytesToSend = 0;                     // all data sent
      }

      HTTPStatus |= HTTP_SEND_PAGE;              // ok, 1st loop executed
    }
  }
  else
    HTTPStatus &= ~HTTP_SEND_PAGE;               // reset help-flag if not connected
}

// searches the TX-buffer for special strings and replaces them
// with dynamic values (AD-converter results)

void InsertDynamicValues(void)
{
  unsigned char *Key;
	 unsigned char *RX_Key;
           char NewKey[5];
  unsigned int i;

  if (TCPTxDataCount < 4) return;                     // there can't be any special string

  Key = TCP_TX_BUF;
	RX_Key =TCP_RX_BUF;
	
  for (i = 0; i < (TCPTxDataCount - 3); i++)
  {
				if(*(RX_Key++) == 'D')
					if(*(RX_Key++) == '3')
						if(*(RX_Key++) == '=')
							if(*(RX_Key++) == 'O')
								if(*(RX_Key) == 'N')
										led_d1=1;
							else if (*(RX_Key++) == 'F')
										if (*(RX_Key++) == 'F')
											led_d1=0;

    if (*Key == 'A')
     if (*(Key + 1) == 'D')
       if (*(Key + 3) == '%')
         if (*(Key + 2) == '1')
         {
						sprintf(NewKey, "%3u", ++pagecounter);    // increment and insert page counter
						memcpy(Key, NewKey, 3);
						*(Key + 3) = ' ';
				}
    Key++;
  }
}

/**
 * @}
 */
