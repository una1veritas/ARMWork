/******************************************************************
 *****                                                        *****
 *****  Name: cs8900.c                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****        adannenb@et.htwk-leipzig.de                     *****
 *****  Func: ethernet packet-driver for use with LAN-        *****
 *****        controller CS8900 from Crystal/Cirrus Logic     *****
 *****                                                        *****
 ******************************************************************/

// Modifications by Code Red Technologies for NXP LPC1776
// Filename changed to ethmac.c as no longer for cs8900

// CodeRed - updated include to match new filename
//#include "cs8900.h"
#include "ethmac.h"

// CodeRed - add additional includes
#include "tcpip.h"
#include "LPC17xx.h"

// CodeRed - new static pointers for receive and transmit
static unsigned short *rxptr;
static unsigned short *txptr;

// CodeRed - function added to write to external ethernet PHY chip
void WriteToPHY (int reg, int writeval)
{
  unsigned int loop;
  // Set up address to access in MII Mgmt Address Register
  LPC_EMAC->MADR = DP83848C_DEF_ADR | reg;
  // Write value into MII Mgmt Write Data Register
  LPC_EMAC->MWTD = writeval;
  // Loop whilst write to PHY completes
  for (loop = 0; loop < MII_WR_TOUT; loop++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) { break; }
  }
}

// CodeRed - function added to read from external ethernet PHY chip
unsigned short ReadFromPHY (unsigned char reg) 
{
  unsigned int loop;
  // Set up address to access in MII Mgmt Address Register
  LPC_EMAC->MADR = DP83848C_DEF_ADR | reg;
  // Trigger a PHY read via MII Mgmt Command Register
  LPC_EMAC->MCMD = MCMD_READ;
  // Loop whilst read from PHY completes
  for (loop = 0; loop < MII_RD_TOUT; loop++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0)  { break; }
  }
  LPC_EMAC->MCMD = 0; // Cancel read
  // Returned value is in MII Mgmt Read Data Register
  return (LPC_EMAC->MRDD);
}



// CodeRed - Init8900() replaced by Init_EthMAC()
/*
// configure port-pins for use with LAN-controller,
// reset it and send the configuration-sequence
// (InitSeq[])

void Init8900(void)
{
  unsigned int i;

  P3SEL = 0x30;                                  // reserve P3.4 and P3.5 for rs232
  P3OUT = IOR | IOW;                             // reset outputs, control lines high
  P3DIR = 0xFF;                                  // port 3 as output (all pins but rs232)

  P5SEL = 0;                                     // select standard port functions
  P5OUT = 0;                                     // reset outputs
  P5DIR = 0xFF;                                  // switch data port to output

  Write8900(ADD_PORT, PP_SelfCTL);
  Write8900(DATA_PORT, POWER_ON_RESET);          // Reset the Ethernet-Controller

  Write8900(ADD_PORT, PP_SelfST);
  while (!(Read8900(DATA_PORT) & INIT_DONE));    // wait until chip-reset is done
  
  for (i = 0; i < sizeof InitSeq / sizeof (TInitSeq); i++) // configure the CS8900
  {
    Write8900(ADD_PORT, InitSeq[i].Addr);
    Write8900(DATA_PORT, InitSeq[i].Data);
  }
}
*/

// Ethernet power/clock control bit in PCONP register
#define PCENET 0x40000000
// Ethernet configuration for PINSEL2, as per user guide section 5.3
#define ENET_PINSEL2_CONFIG 0x50150105
// Ethernet configuration for PINSEL3, as per user guide section 5.4
#define ENET_PINSEL3_CONFIG 0x00000005
// Only bottom byte of PINSEL3 relevant to Ethernet
#define ENET_PINSEL3_MASK 0x0000000F

void Init_EthMAC(void)
{
  unsigned int value, phyid1, phyid2;
  volatile unsigned int loop;
  unsigned phy_in_use = 0;
  unsigned phy_linkstatus_reg;
  unsigned phy_linkstatus_mask;  
  
  // Set Ethernet power/clock control bit
  LPC_SC->PCONP |= PCENET; 

  //Enable Ethernet pins through PINSEL registers
  LPC_PINCON->PINSEL2 = ENET_PINSEL2_CONFIG; 
  LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~(ENET_PINSEL3_MASK)) | ENET_PINSEL3_CONFIG;

  // Set up MAC Configuration Register 1
  LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | 
         MAC1_RES_MCS_RX |MAC1_SIM_RES | MAC1_SOFT_RES;

  // Set up MAC Command Register
  LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;
  
  // Short delay
  for (loop = 100; loop; loop--);

  // Set up MAC Configuration Register 1 to pass all receive frames
  LPC_EMAC->MAC1 = MAC1_PASS_ALL;
  // Set up MAC Configuration Register 2 to append CRC and pad out frames
  LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;

  // Set Ethernet Maximum Frame Register
  LPC_EMAC->MAXF = ETH_MAX_FLEN;
  // Set Collision Window / Retry Register
  LPC_EMAC->CLRT = CLRT_DEF;
  // Set Non Back-to-Back Inter-Packet-Gap Register
  LPC_EMAC->IPGR = IPGR_DEF;

     /* Enable Reduced MII interface. */
    LPC_EMAC->MCFG = MCFG_CLK_DIV64 | MCFG_RES_MII;
     for (loop = 100; loop; loop--);
     LPC_EMAC->MCFG = MCFG_CLK_DIV64;


   // Set MAC Command Register to enable Reduced MII interface
   // and prevent runt frames being filtered out
  LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM | CR_PASS_RX_FILT;


  // Put DP83848C PHY into reset mode
  WriteToPHY (PHY_REG_BMCR, 0x8000);

  // Loop until hardware reset completes
  for (loop = 0; loop < 0x100000; loop++) {
    value = ReadFromPHY (PHY_REG_BMCR);
    if (!(value & 0x8000)) {
      // Reset has completed
      break;
    }
  }

  // Just check this actually is a DP83848C PHY
  phyid1 = ReadFromPHY (PHY_REG_IDR1);
  phyid2 = ReadFromPHY (PHY_REG_IDR2);

  if (((phyid1 << 16) | (phyid2 & 0xFFF0)) == DP83848C_ID) {
	  phy_in_use =  DP83848C_ID;
  }
  else if (((phyid1 << 16) | (phyid2 & 0xFFFF)) == LAN8720_ID) {
	  phy_in_use = LAN8720_ID;
  }

  if (phy_in_use != 0) {	  
	  // Safe to configure the PHY device

    // Set PHY to autonegotiation link speed
    WriteToPHY (PHY_REG_BMCR, PHY_AUTO_NEG);
    // loop until autonegotiation completes
    for (loop = 0; loop < 0x100000; loop++) {
      value = ReadFromPHY (PHY_REG_BMSR);
      if (value & 0x0020) {
        // Autonegotiation has completed
        break;
      }
    }
  }

  phy_linkstatus_reg = PHY_REG_STS;		// Default to DP83848C
  phy_linkstatus_mask = 0x0001; 
  
  if (phy_in_use == LAN8720_ID) {
	  phy_linkstatus_reg = PHY_REG_BMSR;
	  phy_linkstatus_mask = 0x0004;
  }
	  
  // Now check the link status
  for (loop = 0; loop < 0x10000; loop++) {
	value = ReadFromPHY (phy_linkstatus_reg);
    if (value & phy_linkstatus_mask) {
      // The link is on
      break;
    }
  }

  // Now configure for full/half duplex mode 
  if (value & 0x0004) {
    // We are in full duplex is enabled mode
	  LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
	  LPC_EMAC->Command |= CR_FULL_DUP;
	  LPC_EMAC->IPGT     = IPGT_FULL_DUP;
  }
  else {
    // Otherwise we are in half duplex mode
	  LPC_EMAC->IPGT = IPGT_HALF_DUP;
  }

  // Now configure 100MBit or 10MBit mode
  if (value & 0x0002) {
    // 10MBit mode
	  LPC_EMAC->SUPP = 0;
  }
  else {
    // 100MBit mode
	  LPC_EMAC->SUPP = SUPP_SPEED;
  }

  // Now set the Ethernet MAC Address registers
  // NOTE - MAC address must be unique on the network!
  LPC_EMAC->SA0 = (MYMAC_1 << 8) | MYMAC_2; // Station address 0 Reg
  LPC_EMAC->SA1 = (MYMAC_3 << 8) | MYMAC_4; // Station address 1 Reg
  LPC_EMAC->SA2 = (MYMAC_5 << 8) | MYMAC_6; // Station address 2 Reg

 
  // Now initialise the Rx descriptors    
  for (loop = 0; loop < NUM_RX_FRAG; loop++) {
    RX_DESC_PACKET(loop)  = RX_BUF(loop);
    RX_DESC_CTRL(loop)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(loop)    = 0;
    RX_STAT_HASHCRC(loop) = 0;
  }

  // Set up the Receive Descriptor Base address register
  LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
  // Set up the Receive Status Base address register
  LPC_EMAC->RxStatus        = RX_STAT_BASE;
  // Setup the Receive Number of Descriptor register
  LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;
  //  Set Receive Consume Index register to 0
  LPC_EMAC->RxConsumeIndex  = 0;

  // Now initialise the Tx descriptors 
  for (loop = 0; loop < NUM_TX_FRAG; loop++) {
    TX_DESC_PACKET(loop) = TX_BUF(loop);
    TX_DESC_CTRL(loop)   = 0;
    TX_STAT_INFO(loop)   = 0;
  }

  // Set up the Transmit Descriptor Base address register
  LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
  // Set up the Transmit Status Base address register
  LPC_EMAC->TxStatus        = TX_STAT_BASE;
  // Setup the Transmit Number of Descriptor register
  LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;
  //  Set Transmit Consume Index register to 0
  LPC_EMAC->TxProduceIndex  = 0;
 
  // Receive Broadcast and Perfect Match Packets



  LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;					 
 
  // Enable interrupts MAC Module Control Interrupt Enable Register
  LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

  // Reset all ethernet interrupts in MAC module
  LPC_EMAC->IntClear  = 0xFFFF;

  // Finally enable receive and transmit mode in ethernet core
  LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  LPC_EMAC->MAC1     |= MAC1_REC_EN;
}


// CodeRed - Write8900() not needed
// for  RDB1768 port

/*
// writes a word in little-endian byte order to
// a specified port-address

void Write8900(unsigned char Address, unsigned int Data)
{
  P5DIR = 0xFF;                                  // data port to output
  P3OUT = IOR | IOW | Address;                   // put address on bus
  
  P5OUT = Data;                                  // write low order byte to data bus
  P3OUT &= ~IOW;                                 // toggle IOW-signal
  P3OUT = IOR | IOW | (Address + 1);             // and put next address on bus

  P5OUT = Data >> 8;                             // write high order byte to data bus
  P3OUT &= ~IOW;                                 // toggle IOW-signal
  P3OUT |= IOW;
}
*/

// Code Red - updated for LPC1776 port
/*
// writes a word in little-endian byte order to TX_FRAME_PORT

void WriteFrame8900(unsigned int Data)
{
  P5DIR = 0xFF;                                  // data port to output
  P3OUT = IOR | IOW | TX_FRAME_PORT;             // put address on bus
  
  P5OUT = Data;                                  // write low order byte to data bus
  P3OUT &= ~IOW;                                 // toggle IOW-signal
  P3OUT = IOR | IOW | (TX_FRAME_PORT + 1);       // and put next address on bus

  P5OUT = Data >> 8;                             // write high order byte to data bus
  P3OUT &= ~IOW;                                 // toggle IOW-signal
  P3OUT |= IOW;
}
*/

// writes a word in little-endian byte order to TX_BUFFER
void WriteFrame_EthMAC(unsigned short Data)
{
  *txptr++ = Data;
}



// copies bytes from MCU-memory to frame port
// NOTES: * an odd number of byte may only be transfered
//          if the frame is written to the end!
//        * MCU-memory MUST start at word-boundary

// Code Red - rewritten for LPC1776
/*
void CopyToFrame8900(void *Source, unsigned int Size)
{
  P5DIR = 0xFF;                                  // data port to output
  
  while (Size > 1) {
    WriteFrame8900(*((unsigned int *)Source)++);
    Size -= 2;
  }
  
  if (Size)                                      // if odd num. of bytes...
    WriteFrame8900(*(unsigned char *)Source);    // write leftover byte (the LAN-controller
}                                                // ignores the highbyte)
*/

void CopyToFrame_EthMAC(void *Source, unsigned int Size)
{
  unsigned int index;
  unsigned short *pSource;

  pSource = Source;
  Size = (Size + 1) & 0xFFFE;    // round up Size to next even number
  while (Size > 0) {
    WriteFrame_EthMAC(*pSource++);
    Size -= 2;
  }

  index = LPC_EMAC->TxProduceIndex;
  if (++index == NUM_TX_FRAG)
    index = 0;
  LPC_EMAC->TxProduceIndex = index;
}

// CodeRed - Read8900() not needed
// for  LPC1768 port
/*

// reads a word in little-endian byte order from
// a specified port-address

unsigned int Read8900(unsigned char Address)
{
  unsigned int ReturnValue;

  P5DIR = 0x00;                                  // data port to input
  P3OUT = IOR | IOW | Address;                   // put address on bus

  P3OUT &= ~IOR;                                 // IOR-signal low
  
  ReturnValue = P5IN;                            // get low order byte from data bus
  P3OUT = IOR | IOW | (Address + 1);             // IOR high and put next address on bus
  P3OUT &= ~IOR;                                 // IOR-signal low

  ReturnValue |= P5IN << 8;                      // get high order byte from data bus
  
  P3OUT |= IOR;
  
  return ReturnValue;
}
*/
// reads a word in little-endian byte order from RX_FRAME_PORT

// Code Red - ReadFrame8900 rewritten for RDB1768 port
/*
unsigned int ReadFrame8900(void)
{
  unsigned int ReturnValue;

  P5DIR = 0x00;                                  // data port to input
  P3OUT = IOR | IOW | RX_FRAME_PORT;             // access to RX_FRAME_PORT

  P3OUT &= ~IOR;                                 // IOR-signal low
  
  ReturnValue = P5IN;                            // get 1st byte from data bus (low-byte)
  P3OUT = IOR | IOW | (RX_FRAME_PORT + 1);       // IOR high and put next address on bus
  P3OUT &= ~IOR;                                 // IOR-signal low

  ReturnValue |= P5IN << 8;                      // get 2nd byte from data bus (high-byte)
  
  P3OUT |= IOR;
  
  return ReturnValue;
}
*/

// reads a word in little-endian byte order from RX_BUFFER

unsigned short ReadFrame_EthMAC(void)
{
  return (*rxptr++);
}


// reads a word in big-endian byte order from RX_FRAME_PORT
// (useful to avoid permanent byte-swapping while reading
// TCP/IP-data)

// CodeRed - rewritten for LPC1768
/*

unsigned int ReadFrameBE8900(void)
{
  unsigned int ReturnValue;

  P5DIR = 0x00;                                  // data port to input
  P3OUT = IOR | IOW | RX_FRAME_PORT;             // access to RX_FRAME_PORT

  P3OUT &= ~IOR;                                 // IOR-signal low
  
  ReturnValue = P5IN << 8;                       // get 1st byte from data bus (high-byte)
  P3OUT = IOR | IOW | (RX_FRAME_PORT + 1);       // IOR high and put next address on bus
  P3OUT &= ~IOR;                                 // IOR-signal low

  ReturnValue |= P5IN;                           // get 2nd byte from data bus (low-byte)
  
  P3OUT |= IOR;
  
  return ReturnValue;
}
*/

unsigned short ReadFrameBE_EthMAC(void)
{
  unsigned short ReturnValue;

  ReturnValue = SwapBytes (*rxptr++);
  return (ReturnValue);
}

// CodeRed  - not required for RDB1768 port
/*
// reads a word in little-endian byte order from
// a specified port-address
// NOTE: this func. xfers the high-byte 1st, must be used to
//       access some special registers (e.g. RxStatus)

unsigned int ReadHB1ST8900(unsigned char Address)
{
  unsigned int ReturnValue;

  P5DIR = 0x00;                                  // data port to input
  P3OUT = IOR | IOW | (Address + 1);             // put address on bus

  P3OUT &= ~IOR;                                 // IOR-signal low
  
  ReturnValue = P5IN << 8;                       // get high order byte from data bus
  P3OUT = IOR | IOW | Address;                   // IOR high and put next address on bus
  P3OUT &= ~IOR;                                 // IOR-signal low

  ReturnValue |= P5IN;                           // get low order byte from data bus
  
  P3OUT |= IOR;
  
  return ReturnValue;
}
*/


// copies bytes from frame port to MCU-memory
// NOTES: * an odd number of byte may only be transfered
//          if the frame is read to the end!
//        * MCU-memory MUST start at word-boundary

// Code Red - rewritten for LPC1776 port
/*
void CopyFromFrame8900(void *Dest, unsigned int Size)
{
  while (Size > 1) {
    *((unsigned int *)Dest)++ = ReadFrame8900();
    Size -= 2;
  }
  
  if (Size)                                      // check for leftover byte...
    *(unsigned char *)Dest = ReadFrame8900();    // the LAN-Controller will return 0
}                                                // for the highbyte
*/

void CopyFromFrame_EthMAC(void *Dest, unsigned short Size)
{
  unsigned short *pDest; 

  pDest = Dest;
  while (Size > 1) {
    *pDest++ = ReadFrame_EthMAC();
    Size -= 2;
  }
  
  if (Size) {                                         // check for leftover byte...
    *(unsigned char *)pDest = (char)ReadFrame_EthMAC();// the LAN-Controller will return 0
  }                                                   // for the highbyte
}



// does a dummy read on frame-I/O-port
// NOTE: only an even number of bytes is read!

// Code Red - updated for LPC1776
//void DummyReadFrame8900(unsigned int Size)       // discards an EVEN number of bytes
void DummyReadFrame_EthMAC(unsigned short Size)       // discards an EVEN number of bytes
{                                                // from RX-fifo
  while (Size > 1) {
// Code Red - updated for LPC1776	  
//    ReadFrame8900();
    ReadFrame_EthMAC();
    Size -= 2;
  }
}

// requests space in CS8900's on-chip memory for
// storing an outgoing frame

// CodeRed - updated for LPC1768 port
/*
void RequestSend(unsigned int FrameSize)
{
  Write8900(TX_CMD_PORT, TX_START_ALL_BYTES);
  Write8900(TX_LEN_PORT, FrameSize);
}
*/
void RequestSend(unsigned short FrameSize)
{
  unsigned int index;
  index  = LPC_EMAC->TxProduceIndex;
  txptr = (unsigned short *)TX_DESC_PACKET(index);
  TX_DESC_CTRL(index) = FrameSize | TCTRL_LAST;
}

// check if CS8900 is ready to accept the
// frame we want to send

unsigned int Rdy4Tx(void)
{
// Code Red - updated for LPC1768
//  Write8900(ADD_PORT, PP_BusST);
//  return (Read8900(DATA_PORT) & READY_FOR_TX_NOW);

  // One the LPC the ethernet controller transmits
  // much faster than the CPU can load its buffers
  // so will always be ready to accept frame	
  return (1); 
               // 
}

// CodeRed - New function

// Reads  length of received ethernet frame and checks
// if destination address is a broadcast message or not.
// Then returns the frame length
unsigned short StartReadingFrame(void)
{
  unsigned short ReceiveLength;
  unsigned int index;

  index = LPC_EMAC->RxConsumeIndex;
  ReceiveLength = (RX_STAT_INFO(index) & RINFO_SIZE) - 3;
  rxptr = (unsigned short *)RX_DESC_PACKET(index);
  return(ReceiveLength);
}

// CodeRed - new function

void StopReadingFrame(void)
{
  unsigned int index;
  index = LPC_EMAC->RxConsumeIndex;
  if (++index == NUM_RX_FRAG) index = 0;
  LPC_EMAC->RxConsumeIndex = index;
}

// CodeRed - new function to check if frame has been received
unsigned int CheckIfFrameReceived(void)
{ 
  if (LPC_EMAC->RxProduceIndex != LPC_EMAC->RxConsumeIndex) 
    return(1); // Yes, packet received
  else 
    return(0);
}
