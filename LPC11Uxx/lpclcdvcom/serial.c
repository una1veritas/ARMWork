/*----------------------------------------------------------------------------
 *      Name:    serial.c
 *      Purpose: serial port handling for LPC11xx
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing 
 *      else gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "LPC11Uxx.h"                                   // LPC13xx definitions
#include "type.h"
#include "serial.h"


/*----------------------------------------------------------------------------
  Defines for ring buffers
 *---------------------------------------------------------------------------*/
#define SER_BUF_SIZE               (128)               // serial buffer in bytes (power 2)
#define SER_BUF_MASK               (SER_BUF_SIZE-1ul)  // buffer size mask

/* Buffer read / write macros */
#define SER_BUF_RESET(serBuf)      (serBuf.rdIdx = serBuf.wrIdx = 0)
#define SER_BUF_WR(serBuf, dataIn) (serBuf.data[SER_BUF_MASK & serBuf.wrIdx++] = (dataIn))
#define SER_BUF_RD(serBuf, dataOut) serBuf.rdIdx++; dataOut = serBuf.data[SER_BUF_MASK & (serBuf.rdIdx-1)]; 
#define SER_BUF_EMPTY(serBuf)      (serBuf.rdIdx == serBuf.wrIdx)
#define SER_BUF_FULL(serBuf)       ((SER_BUF_MASK & serBuf.rdIdx) == (SER_BUF_MASK & (serBuf.wrIdx+1)))
#define SER_BUF_COUNT(serBuf)      (SER_BUF_MASK & (serBuf.wrIdx - serBuf.rdIdx))

// buffer type
typedef struct __SER_BUF_T {
  unsigned char data[SER_BUF_SIZE];
  unsigned int wrIdx;
  unsigned int rdIdx;
} SER_BUF_T;

unsigned long          ser_txRestart;                  // NZ if TX restart is required
unsigned short         ser_lineState;                  // ((msr << 8) | (lsr))
SER_BUF_T              ser_out;                        // Serial data buffers
SER_BUF_T              ser_in;

/*----------------------------------------------------------------------------
  open the serial port
 *---------------------------------------------------------------------------*/
void ser_OpenPort (void) {
 
  NVIC_DisableIRQ(UART_IRQn);

#if 1
  LPC_IOCON->PIO0_18 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO0_18 |= 0x01;     /* UART RXD */
  LPC_IOCON->PIO0_19 &= ~0x07;	
  LPC_IOCON->PIO0_19 |= 0x01;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_14 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_14 |= 0x03;     /* UART RXD */
  LPC_IOCON->PIO1_13 &= ~0x07;	
  LPC_IOCON->PIO1_13 |= 0x03;     /* UART TXD */
#endif
#if 0
  LPC_IOCON->PIO1_26 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO1_26 |= 0x02;     /* UART RXD */
  LPC_IOCON->PIO1_27 &= ~0x07;	
  LPC_IOCON->PIO1_27 |= 0x02;     /* UART TXD */
#endif

  /* Enable UART clock */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
  LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */
  return;
}

/*----------------------------------------------------------------------------
  close the serial port
 *---------------------------------------------------------------------------*/
void ser_ClosePort (void) {
  LPC_IOCON->PIO0_18 &= ~0x07;    /*  UART I/O config */
  LPC_IOCON->PIO0_19 &= ~0x07;
  
//  LPC_IOCON->PIO1_14 &= ~0x07;    /*  UART I/O config */
//  LPC_IOCON->PIO1_13 &= ~0x07;
//  LPC_IOCON->PIO1_26 &= ~0x07;    /*  UART I/O config */
//  LPC_IOCON->PIO1_27 &= ~0x07;

  /* Disable the interrupt in the VIC and UART controllers */
  LPC_USART->IER = 0;
  NVIC_DisableIRQ(UART_IRQn);
  return;
}

/*----------------------------------------------------------------------------
  initialize the serial port
 *---------------------------------------------------------------------------*/
void ser_InitPort (unsigned long baudrate, unsigned int  databits,
                  unsigned int  parity,   unsigned int  stopbits) {

  uint8_t lcr_p, lcr_s, lcr_d;
  uint32_t dll;
  uint32_t Fdiv;
  
  switch (databits) {
    case 5:                                            // 5 Data bits
      lcr_d = 0x00;
    break;
    case 6:                                            // 6 Data bits
      lcr_d = 0x01;
    break;
    case 7:                                            // 7 Data bits
      lcr_d = 0x02;
    break;
    case 8:                                            // 8 Data bits
    default:
      lcr_d = 0x03;
    break;
  }

  switch (stopbits) {
    case 1:                                            // 1,5 Stop bits
    case 2:                                            // 2   Stop bits
      lcr_s = 0x04;
    break;
    case 0:                                            // 1   Stop bit
    default:
      lcr_s = 0x00;
    break;
  }

  switch (parity) {
    case 1:                                            // Parity Odd
      lcr_p = 0x08;
    break;
    case 2:                                            // Parity Even
      lcr_p = 0x18;
    break;
    case 3:                                            // Parity Mark
      lcr_p = 0x28;
    break;
    case 4:                                            // Parity Space
      lcr_p = 0x38;
    break;
    case 0:                                            // Parity None
    default:
      lcr_p = 0x00;
    break;
  }

  SER_BUF_RESET(ser_out);                              // reset out buffer
  SER_BUF_RESET(ser_in);                               // reset in buffer
  
  /* Note that the pclk is 24,0 MHz.  (48.0 MHz / 2)         */
  /* 24 MHz PCLK generates also rates for 115200, 57600 baud */
  Fdiv = LPC_SYSCON->UARTCLKDIV;
  dll = ((SystemCoreClock/Fdiv)/16)/baudrate ;	/*baud rate */
  LPC_USART->FDR = 0;                             // Fractional divider not used
  LPC_USART->LCR = 0x80 | lcr_d | lcr_p | lcr_s;  // Data bits, Parity,   Stop bit
  LPC_USART->DLL = dll;                           // Baud Rate depending on PCLK
  LPC_USART->DLM = (dll >> 8);                    // High divisor latch
  LPC_USART->LCR = 0x00 | lcr_d | lcr_p | lcr_s;  // DLAB = 0
  LPC_USART->IER = 0x03;                          // Enable TX/RX interrupts

  LPC_USART->FCR = 0x07;				/* Enable and reset TX and RX FIFO. */
  ser_txRestart = 1;                                   // TX fifo is empty

  /* Enable the UART Interrupt */
  NVIC_EnableIRQ(UART_IRQn);
  return;
}

/*----------------------------------------------------------------------------
  read data from serial port
 *---------------------------------------------------------------------------*/
int ser_Read (char *buffer, const int *length) {
  int bytesToRead, bytesRead;
  
  /* Read *length bytes, block if *bytes are not avaialable	*/
  bytesToRead = *length;
  bytesToRead = (bytesToRead < (*length)) ? bytesToRead : (*length);
  bytesRead = bytesToRead;

  while (bytesToRead--) {
    while (SER_BUF_EMPTY(ser_in));      /* Block until data is avail. if none */
    SER_BUF_RD(ser_in, *buffer++);
  }
  return (bytesRead);  
}

/*----------------------------------------------------------------------------
  write data to the serial port
 *---------------------------------------------------------------------------*/
int ser_Write (const char *buffer, int *length) {
  int  bytesToWrite, bytesWritten;

  bytesToWrite = *length;               /* Write *length bytes                */
  bytesWritten = bytesToWrite;

  while (!SER_BUF_EMPTY(ser_out));      /* Block until space is available     */
  while (bytesToWrite) {
      SER_BUF_WR(ser_out, *buffer++);   /* Read Rx FIFO to buffer             */
      bytesToWrite--;
  }

  if (ser_txRestart) {
    ser_txRestart = 0;
    SER_BUF_RD(ser_out, LPC_USART->THR);  /* Write to the Tx Register         */
  }

  return (bytesWritten); 
}

/*----------------------------------------------------------------------------
  check if character(s) are available at the serial interface
 *---------------------------------------------------------------------------*/
void ser_AvailChar (int *availChar) {

  *availChar = SER_BUF_COUNT(ser_in) | SER_BUF_FULL(ser_in);

}

/*----------------------------------------------------------------------------
  read the line state of the serial port
 *---------------------------------------------------------------------------*/
void ser_LineState (unsigned short *lineState) {

  *lineState = ser_lineState;
  ser_lineState = 0;

}

/*----------------------------------------------------------------------------
  serial port 1 interrupt
 *---------------------------------------------------------------------------*/
void UART_IRQHandler(void) 
{ 
  volatile unsigned long iir;
  
  iir = LPC_USART->IIR;
   
  if ((iir & 0x4) || (iir & 0xC)) {            // RDA or CTI pending
    while (LPC_USART->LSR & 0x01) {             // Rx FIFO is not empty
      SER_BUF_WR(ser_in, LPC_USART->RBR);       // Read Rx FIFO to buffer  
    }
  }
  if ((iir & 0x2)) {                    /* TXMIS pending                      */
    if (SER_BUF_COUNT(ser_out) != 0) {
      SER_BUF_RD(ser_out, LPC_USART->THR); /* Write to the Tx FIFO            */
      ser_txRestart = 0;
    }
	else {
      ser_txRestart = 1;
	}
  }
  ser_lineState = LPC_USART->LSR & 0x1E;        // update linestate
  return;
}


