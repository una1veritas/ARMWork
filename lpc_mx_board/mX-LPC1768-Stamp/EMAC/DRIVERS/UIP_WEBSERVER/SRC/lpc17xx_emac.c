#include <string.h>
#include "lpc17xx_emac.h"

#include "uip-conf.h"
#include "uipopt.h"

/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take 
  longer to connect, but it works for 10MBit and 100MBit physical links.     */
  
/* Local Function Prototypes */  
static void   rx_descr_init(void);
static void   tx_descr_init(void);
static void write_PHY (UNS_32 PhyReg, UNS_32 Value);
static UNS_16 read_PHY (UNS_8 PhyReg) ;

/*--------------------------- EMAC_Init ---------------------------------*/

BOOL_32 EMAC_Init(void)
{
   /* Initialize the EMAC ethernet controller. */
  UNS_32 regv,tout,id1,id2;

   /* Power Up the EMAC controller. */
   LPC_SC->PCONP |= 0x40000000;

   /* Enable P1 Ethernet Pins. */
   LPC_PINCON->PINSEL2 = 0x50150105;
   LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Reset all EMAC internal modules. */
  LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
             MAC1_SIM_RES | MAC1_SOFT_RES;
  LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

  /* A short delay after reset. */
  for (tout = 100; tout; tout--);

  /* Initialize MAC control registers. */
  LPC_EMAC->MAC1 = MAC1_PASS_ALL;
  LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  LPC_EMAC->MAXF = ETH_MAX_FLEN;
  LPC_EMAC->CLRT = CLRT_DEF;
  LPC_EMAC->IPGR = IPGR_DEF;

/*PCLK=18MHz, clock select=6, MDC=18/6=3MHz */  
   /* Enable Reduced MII interface. */
   LPC_EMAC->MCFG = MCFG_CLK_DIV20 | MCFG_RES_MII;
   for (tout = 100; tout; tout--);
   LPC_EMAC->MCFG = MCFG_CLK_DIV20;

  /* Enable Reduced MII interface. */
  LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM | CR_PASS_RX_FILT;

  /* Reset Reduced MII Logic. */
   LPC_EMAC->SUPP = SUPP_RES_RMII | SUPP_SPEED;
   for (tout = 100; tout; tout--);
   LPC_EMAC->SUPP = SUPP_SPEED;

  /* Put the PHY in reset mode */
  write_PHY (PHY_REG_BMCR, 0x8000);
	for (tout = 1000; tout; tout--);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 0x100000; tout++) {
  	regv = read_PHY (PHY_REG_BMCR);
#if defined (KEIL_BOARD_MCB17XX) 
    if (!(regv & 0x8000)) 
#else
	#error "No board!"
#endif
	{
      /* Reset complete */
      break;
    }
  }
  if (tout >= 0x100000)
  	return FALSE; /* reset failed */

  /* Check if this is a DP83848C PHY. */
  id1 = read_PHY (PHY_REG_IDR1);
  id2 = read_PHY (PHY_REG_IDR2);
  
#if defined (KEIL_BOARD_MCB17XX)   
	  if (((id1 << 16) | (id2 & 0xFFF0)) != DP83848C_ID)
#else
	#error "No board"
#endif		  
  	return FALSE;
  	
	/* Configure the PHY device */
      /* Configure the PHY device */
#if defined (_10MBIT_)
      /* Connect at 10MBit */
      write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
#elif defined (_100MBIT_)
      /* Connect at 100MBit */
      write_PHY (PHY_REG_BMCR, PHY_FULLD_100M);
#else
      /* Use autonegotiation about the link speed. */
      write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
      /* Wait to complete Auto_Negotiation. */
      for (tout = 0; tout < 0x100000; tout++) {
         regv = read_PHY (PHY_REG_BMSR);
         if (regv & 0x0020) {
            /* Autonegotiation Complete. */
            break;
         }
      }
#endif
	if (tout >= 0x100000)
		return FALSE; // auto_neg failed

  /* Check the link status. */
  for (tout = 0; tout < 0x10000; tout++) {
#if defined (KEIL_BOARD_MCB17XX)
    regv = read_PHY (PHY_REG_STS);
    if (regv & 0x0001) 
#else
	#error "No board"
#endif 
	{   	
      /* Link is on. */
      break;
    }
  }

  if (tout >= 0x10000)
  	return FALSE;

  /* Configure Full/Half Duplex mode. */
#if defined (KEIL_BOARD_MCB17XX)   
  if (regv & 0x0004) 
#else
	#error "No board"
#endif
	{  	
    /* Full duplex is enabled. */
    LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
    LPC_EMAC->Command |= CR_FULL_DUP;
    LPC_EMAC->IPGT     = IPGT_FULL_DUP;
  }
  else {
    /* Half duplex mode. */
    LPC_EMAC->IPGT = IPGT_HALF_DUP;
  }

  /* Configure 100MBit/10MBit mode. */
#if defined (KEIL_BOARD_MCB17XX)  
  if (regv & 0x0002) {
#else
	#error "No baord"
#endif   	  	
    /* 10MBit mode. */
    LPC_EMAC->SUPP = 0;
  }
  else {
    /* 100MBit mode. */
    LPC_EMAC->SUPP = SUPP_SPEED;
  }

  /* Set the Ethernet MAC Address registers */
//  MAC_SA0 = (MYMAC_6 << 8) | MYMAC_5;
//  MAC_SA1 = (MYMAC_4 << 8) | MYMAC_3;
//  MAC_SA2 = (MYMAC_2 << 8) | MYMAC_1;
  LPC_EMAC->SA0 = (UIP_ETHADDR1<<8) | UIP_ETHADDR0;
  LPC_EMAC->SA1 = (UIP_ETHADDR3<<8) | UIP_ETHADDR2;
  LPC_EMAC->SA2 = (UIP_ETHADDR5<<8) | UIP_ETHADDR4;

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
  tx_descr_init ();

  /* Receive Broadcast and Perfect Match Packets */
  LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

  /* Enable EMAC interrupts. */
  //EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

  /* Reset all interrupts */
  //EMAC->IntClear  = 0xFFFF;

  /* Enable receive and transmit mode of MAC Ethernet core */
  LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  LPC_EMAC->MAC1     |= MAC1_REC_EN;

   /* Configure VIC for EMAC interrupt. */
   //VICVectAddrxx = (UNS_32)xx;
   
  return TRUE;
}

/*--------------------------- write_PHY -------------------------------------*/

static void write_PHY (UNS_32 PhyReg, UNS_32 Value)
{
  unsigned int tout;

  LPC_EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  LPC_EMAC->MWTD = Value;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }   
}

/*--------------------------- read_PHY -------------------------------------*/

static UNS_16 read_PHY (UNS_8 PhyReg)
{
  UNS_32 tout;

  LPC_EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  LPC_EMAC->MCMD = MCMD_READ;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }
  LPC_EMAC->MCMD = 0;
  return (LPC_EMAC->MRDD);   
}

/*--------------------------- EMAC_ReadPacket ---------------------------------*/

UNS_32 EMAC_ReadPacket(void * pPacket)
{
	UNS_32 Index = LPC_EMAC->RxConsumeIndex;
	UNS_32 size;

  if(Index == LPC_EMAC->RxProduceIndex)
  {
    return(0);
  }

  size = (RX_STAT_INFO(Index) & 0x7ff)+1; 
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;
  	
  memcpy(pPacket,(unsigned int *)RX_BUF(Index),size);

  if(++Index > LPC_EMAC->RxDescriptorNumber)
  {
    Index = 0;
  }
  LPC_EMAC->RxConsumeIndex = Index;
  
  return(size);
}

/*--------------------------- EMAC_SendPacket ---------------------------------*/

BOOL_32 EMAC_SendPacket(void *pPacket, UNS_32 size)
{
	UNS_32 	Index;
	UNS_32	IndexNext = LPC_EMAC->TxProduceIndex + 1;
	
  if(size == 0)
  {
    return(TRUE);
  }
  if(IndexNext > LPC_EMAC->TxDescriptorNumber)
  {
    IndexNext = 0;
  }
 
  if(IndexNext == LPC_EMAC->TxConsumeIndex)
  {
    return(FALSE);
  }
  Index = LPC_EMAC->TxProduceIndex;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  memcpy((unsigned int *)TX_BUF(Index),pPacket,size);  
  TX_DESC_CTRL(Index) &= ~0x7ff;
  TX_DESC_CTRL(Index) |= (size - 1) & 0x7ff; 

  LPC_EMAC->TxProduceIndex = IndexNext;	

  return(TRUE);
}

/*--------------------------- rx_descr_init ---------------------------------*/

static void rx_descr_init (void)
{
  UNS_32 i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
  LPC_EMAC->RxStatus        = RX_STAT_BASE;
  LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  LPC_EMAC->RxConsumeIndex  = 0;
}


/*--------------------------- tx_descr_init ---------------------------------*/

static void tx_descr_init (void) {
  UNS_32 i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    TX_DESC_PACKET(i) = TX_BUF(i);
    TX_DESC_CTRL(i)   = (1UL<<31) | (1UL<<30) | (1UL<<29) | (1UL<<28) | (1UL<<26) | (ETH_FRAG_SIZE-1);
    TX_STAT_INFO(i)   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
  LPC_EMAC->TxStatus        = TX_STAT_BASE;
  LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  LPC_EMAC->TxProduceIndex  = 0;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
