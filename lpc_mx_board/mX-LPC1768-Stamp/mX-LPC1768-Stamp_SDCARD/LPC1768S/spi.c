/********************************************************************/
/*                                                                  */
/* Filename:  SPI.c                                                 */
/*                                                                  */
/* Description:                                                     */
/* This header file contains prototypes and constants used to       */
/* access the SPI port for access to the FLASH card.                */
/*                                                                  */
/* Global Functions:                                                */
/*     SPI_Init()             - Initialize the SPI Port             */
/*     SPI_Read()             - Read from the SPI Port              */
/*     SPI_Write()            - Write to the SPI Port               */
/*     SPI_WriteRead()        - Write NULL then read the SPI Port   */
/*                                                                  */
/* Local Functions:                                                 */
/*     SPI_ClearBuffer()      - Clear Out the RX buffer             */
/*                                                                  */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/* 04-AUG-2005  Debug SPI Chip Selection.                           */
/* 27-AUG-2005  SPI Communications Testing and Debug.               */
/* 06-SEP-2005  Modified for PCB Layout.                            */
/********************************************************************/
#ifndef  SPI_C
#define  SPI_C


//-----------------Include Common Header Files--------------------------
#include "lpc17xx.h"

//-----------------Include BuckyMeter Header Files----------------------
#include "SPI.h"
#include "type.h"

/********************************************************************/
/*                                                                  */
/* Global Function:  void SPI_ChipSelect(Select)                    */
/*                                                                  */
/* This function Sets or Clears the FLASH Memory Chip Select.       */
/*                                                                  */
/* The Actual memory chip uses inverted logic.  This routine        */
/* implements the reversed logic.  The caller specifies 1 to        */
/* Chip Select, and 0 to DeSelect.                                  */
/*                                                                  */
/* Parameters                                                       */
/*     uchar Select            - Chip Select                        */
/*                               0 = Low (Chip DeSelect)            */
/*                               1 = Hi  (Chip Select)              */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 01-JUL-2005  Buckymeter LPC2138.                                 */
/********************************************************************/
void SPI_ChipSelect(uint8_t Select)
{
   if (Select)  {
      SPI_PORT_DATA_CLR_REG = SPI_PORT_CHIP_SELECT;   //Clear bit

   }  else  {
      SPI_PORT_DATA_SET_REG = SPI_PORT_CHIP_SELECT;   //Set   bit

   }
}

/********************************************************************/
/*                                                                  */
/* Local  Function:  void  SPI_Init(void)                           */
/*                                                                  */
/* This function initializes the SPI hardware and Chip Select port. */
/*                                                                  */
/* Parameters                                                       */
/*     NONE                                                         */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/* 04-AUG-2005  Debug SPI Chip Selection.                           */
/********************************************************************/
void SPI_Init(void)
{
   uint8_t Status;
   uint8_t Data;

   LPC_PINCON->PINSEL0 |= 0xC0000000;
   LPC_PINCON->PINSEL1 |= 0x0000003F;

	/* initialize SPI UART */
   SPI_PORT_DATA_SET_REG  = SPI_PORT_DATA_REG_INIT;   /* Initialize Chip Select High */

   LPC_SPI->SPCCR = SPI_CLOCK_DIVISOR;                       /* SPI Clock Divisor        */
   LPC_SPI->SPCR  = SPI_CONFIGURATION;                       /* SPI Config Register      */
   Status  = LPC_SPI->SPSR;                                  /* Clear Pending Status     */
   Data    = LPC_SPI->SPDR;                                  /* Clear Read Data Register */
}


/********************************************************************/
/*                                                                  */
/* Local  Function:  uchar SPI_ClearBuffer(void)                    */
/*                                                                  */
/* This function clears out the SPI read buffer in preparation for  */
/* incoming data.                                                   */
/*                                                                  */
/* Parameters                                                       */
/*     uchar SPI_ClearBuffer   - Completion Status                  */
/*                               0 = Request Successfully Completed */
/*                               1 = Request Failed (Timeout)       */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/********************************************************************/
uint8_t SPI_ClearBuffer(void)
{
   uint8_t rbuf;     //Place holder to clear Rx Buffer
   uint8_t timeout;  //Read Function Completion Status
   timeout = SPI_Read (&rbuf);
   return(timeout);
}

/********************************************************************/
/*                                                                  */
/* Global Function:  uchar SPI_Write(uchar bytes, uchar tx[])       */
/*                                                                  */
/* This function Sends command bytes to the SPI BUS.                */
/*                                                                  */
/* Parameters                                                       */
/*     uchar SPI_Write         - Completion Status                  */
/*                               0 = Request Successfully Completed */
/*                               1 = Request Failed (Timeout)       */
/*     uchar bytes             - Number of bytes to transmit        */
/*     uchar tx[]              - Array of Bytes to Send             */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/* 27-AUG-2005  SPI Communications Testing and Debug.               */
/********************************************************************/
uint8_t SPI_Write(uint16_t bytes, uint8_t tx[])
{
   uint8_t timeout;  //Completion Status
   uint8_t junk;     //Dummy Variable to clear Rx Buffer
   uint16_t i;        //Local Byte Counter
   uint32_t j;        //Timeout Failure Counter


   if (bytes)  {
      timeout = 0;
      for (i=0; ( (!timeout) && (i<bytes) ); i++)  {
         junk = LPC_SPI->SPSR;       //Clear Transfer Complete Flag
         junk = LPC_SPI->SPDR;       //Clear RX buffer
	      LPC_SPI->SPDR = tx[i];      //Transmit Character(s)

         for (j=0; ( (j<SPI_TIMEOUT) && bit_is_clear(LPC_SPI->SPSR, SPIF) ); j++) {};
         if ( bit_is_clear(LPC_SPI->SPSR, SPIF) )  {
   	      timeout = 1;	
         }
      }

   }  else  {

      timeout = 1;
   }
   return(timeout);
}





/********************************************************************/
/*                                                                  */
/* Global Function:  uchar SPI_Read(uchar *rx)                      */
/*                                                                  */
/* This function Receives one data bytes from the SPI BUS.          */
/*                                                                  */
/* Parameters                                                       */
/*     uchar SPI_Read          - Completion Status                  */
/*                               0 = Request Successfully Completed */
/*                               1 = Request Failed (Timeout)       */
/*     uchar *rx               - Reference for received byte        */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/* 27-AUG-2005  SPI Communications Testing and Debug.               */
/********************************************************************/
uint8_t SPI_Read (uint8_t *rx)
{
   uint8_t timeout;  //Function Completion Status
   uint32_t i;        //Local Timeout Counter


   timeout = 0;
   for(i=0;( (i<SPI_TIMEOUT) && bit_is_clear(LPC_SPI->SPSR, SPIF) ); i++) {};
   if ( bit_is_clear(LPC_SPI->SPSR, SPIF) )  {
      timeout = 1;
   }

   *rx = LPC_SPI->SPDR;           //Get Receive Data Byte


   return(timeout);
}





/********************************************************************/
/*                                                                  */
/* Global Function:  uchar SPI_WriteRead(ushort bytes, uchar rx[])  */
/*                                                                  */
/* This function transmits NULL bytes to receive data back from     */
/* the SPI BUS.                                                     */
/*                                                                  */
/* Parameters                                                       */
/*     uchar SPI_Read          - Completion Status                  */
/*                               0 = Request Successfully Completed */
/*                               1 = Request Failed (Timeout)       */
/*     ushort bytes            - Number of bytes to receive         */
/*     uchar  rx[]             - Array of Bytes to Send             */
/*                                                                  */
/********************************************************************/
/*Edit Log                                                          */
/*    DATE                   COMMENTS                               */
/* 04-JUL-2005  Buckymeter LPC2138.                                 */
/* 27-AUG-2005  SPI Communications Testing and Debug.               */
/********************************************************************/
uint8_t SPI_WriteRead (uint16_t bytes, uint8_t rx[])
{
   uint8_t NullOpCode[1];
   uint8_t timeout;  //Function Completion Status
   uint16_t b;        //Local Byte Counter
   uint32_t i;        //Local Timeout Counter


   if (bytes)  {
      timeout = 0;
      NullOpCode[0] = 0x00;

      for (b=0; ( (!timeout) && (b<bytes) ); b++)  {
         timeout = SPI_Write(1, NullOpCode);
         for(i=0;( (i<SPI_TIMEOUT) && bit_is_clear(LPC_SPI->SPSR, SPIF) ); i++) {};
         if ( bit_is_clear(LPC_SPI->SPSR, SPIF) )  {
   	      timeout = 1;
         }

         *rx++ = LPC_SPI->SPDR; 
      }

   }  else  {
      timeout = 1;

   }


   return(timeout);
}


#undef  SPI_C
#endif






