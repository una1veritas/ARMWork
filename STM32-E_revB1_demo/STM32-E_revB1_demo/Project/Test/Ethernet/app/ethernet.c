/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    $Revision: #1 $
 **************************************************************************/
#include "ethernet.h"
#include <stdio.h>
#include <string.h>
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

typedef union _TranDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  DB            : 1;
    uint32_t  UF            : 1;
    uint32_t  ED            : 1;
    uint32_t  CC            : 4;
    uint32_t  VF            : 1;
    uint32_t  EC            : 1;
    uint32_t  LC            : 1;
    uint32_t  NC            : 1;
    uint32_t  LSC           : 1;
    uint32_t  IPE           : 1;
    uint32_t  FF            : 1;
    uint32_t  JT            : 1;
    uint32_t  ES            : 1;
    uint32_t  IHE           : 1;
    uint32_t                : 3;
    uint32_t  TCH           : 1;
    uint32_t  TER           : 1;
    uint32_t  CIC           : 2;
    uint32_t                : 2;
    uint32_t  DP            : 1;
    uint32_t  DC            : 1;
    uint32_t  FS            : 1;
    uint32_t  LSEG          : 1;
    uint32_t  IC            : 1;
    uint32_t  OWN           : 1;
  };
} TranDesc0_t, * pTranDesc0_t;

typedef union _TranDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  TBS1          :13;
    uint32_t                : 3;
    uint32_t  TBS2          :12;
    uint32_t                : 3;
  };
} TranDesc1_t, * pTranDesc1_t;

typedef union _RecDesc0_t
{
  uint32_t Data;
  struct {
    uint32_t  RMAM_PCE      : 1;
    uint32_t  CE            : 1;
    uint32_t  DE            : 1;
    uint32_t  RE            : 1;
    uint32_t  RWT           : 1;
    uint32_t  FT            : 1;
    uint32_t  LC            : 1;
    uint32_t  IPHCE         : 1;
    uint32_t  LS            : 1;
    uint32_t  FS            : 1;
    uint32_t  VLAN          : 1;
    uint32_t  OE            : 1;
    uint32_t  LE            : 1;
    uint32_t  SAF           : 1;
    uint32_t  DERR          : 1;
    uint32_t  ES            : 1;
    uint32_t  FL            :14;
    uint32_t  AFM           : 1;
    uint32_t  OWN           : 1;
  };
} RecDesc0_t, * pRecDesc0_t;

typedef union _recDesc1_t
{
  uint32_t Data;
  struct {
    uint32_t  RBS1          :13;
    uint32_t                : 1;
    uint32_t  RCH           : 1;
    uint32_t  RER           : 1;
    uint32_t  RBS2          :14;
    uint32_t  DIC           : 1;
  };
} RecDesc1_t, * pRecDesc1_t;

typedef union _EnetDmaDesc_t
{
  uint32_t Data[4];
  // Rx DMA descriptor
  struct
  {
    RecDesc0_t                RxDesc0;
    RecDesc1_t                RxDesc1;
    uint32_t *                   pBuffer;
    union
    {
      uint32_t *                 pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Rx;
  // Tx DMA descriptor
  struct
  {
    TranDesc0_t               TxDesc0;
    TranDesc1_t               TxDesc1;
    uint32_t *                   pBuffer1;
    union
    {
      uint32_t *                 pBuffer2;
      union _EnetDmaDesc_t *  pEnetDmaNextDesc;
    };
  } Tx;
} EnetDmaDesc_t, * pEnetDmaDesc_t;

#pragma data_alignment=4
uint8_t RxBuff[EMAC_MAX_PACKET_SIZE];
#pragma data_alignment=4
uint8_t TxBuff[EMAC_MAX_PACKET_SIZE];

#pragma data_alignment=128
EnetDmaDesc_t EnetDmaRx;

#pragma data_alignment=128
EnetDmaDesc_t EnetDmaTx;


/*************************************************************************
 * Function Name: uip_log
 * Parameters: none
 *
 * Return: none
 *
 * Description: Events loggin
 *
 *************************************************************************/
void uip_log (char *m)
{
  printf("uIP log message: %s\n", m);
}


uint32_t uIPMain(void)
{
uint32_t i;
uip_ipaddr_t ipaddr;
struct timer periodic_timer, arp_timer;

  // Sys timer init 1/100 sec tick
  clock_init(2);

  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);

  // Initialize the ethernet device driver
  // Init MAC
  // Phy network negotiation
  tapdev_init();

  // uIP web server
  // Initialize the uIP TCP/IP stack.
  uip_init();

  // Init WEB server
  uip_ipaddr(ipaddr, 192,168,0,100);
  printf("IP Address: 192.168.0.100\n\r");
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,0,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

  // Initialize the HTTP server.
  httpd_init();

  // Run WEB server and wait any key for exit
  while(1)
  {
    uip_len = tapdev_read(uip_buf);
    if(uip_len > 0)
    {
      if(BUF->type == htons(UIP_ETHTYPE_IP))
      {
	      uip_arp_ipin();
	      uip_input();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */
	      if(uip_len > 0)
        {
	        uip_arp_out();
	        tapdev_send(uip_buf,uip_len);
	      }
      }
      else if(BUF->type == htons(UIP_ETHTYPE_ARP))
      {
        uip_arp_arpin();
	      /* If the above function invocation resulted in data that
	         should be sent out on the network, the global variable
	         uip_len is set to a value > 0. */
	      if(uip_len > 0)
        {
	        tapdev_send(uip_buf,uip_len);
	      }
      }
    }
    else if(timer_expired(&periodic_timer))
    {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++)
      {
      	uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0)
        {
          uip_arp_out();
          tapdev_send(uip_buf,uip_len);
        }
      }
#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0) {
          uip_arp_out();
          tapdev_send();
        }
      }
#endif /* UIP_UDP */
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer))
      {
        timer_reset(&arp_timer);
        uip_arp_timer();
      }
    }

    if(' ' == getchar())
    {
      break;
    }
  }
  return(1);
}


/*******************************************************************************
* Function Name  : ENET_RxDscrInit
* Description    : Initializes the Rx ENET descriptor chain. Single Descriptor
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ENET_RxDscrInit(void)
{
  /* Initialization */
  /* Assign temp Rx array to the ENET buffer */
  EnetDmaRx.Rx.pBuffer = (uint32_t *)RxBuff;

  /* Initialize RX ENET Status and control */
  EnetDmaRx.Rx.RxDesc0.Data = 0;

  /* Initialize the next descriptor- In our case its single descriptor */
  EnetDmaRx.Rx.pEnetDmaNextDesc = &EnetDmaRx;

  EnetDmaRx.Rx.RxDesc1.Data = 0;
  EnetDmaRx.Rx.RxDesc1.RER  = 0; // end of ring
  EnetDmaRx.Rx.RxDesc1.RCH  = 1; // end of ring

  /* Set the max packet size  */
  EnetDmaRx.Rx.RxDesc1.RBS1 = EMAC_MAX_PACKET_SIZE;

  /* Setting the VALID bit */
  EnetDmaRx.Rx.RxDesc0.OWN = 1;
  /* Setting the RX NEXT Descriptor Register inside the ENET */
  ETH->DMARDLAR = (uint32_t)&EnetDmaRx;
  /* Setting the RX NEXT Descriptor Register inside the ENET */
  //ETH_DMARDLAR = (uint32_t)&EnetDmaRx;
}

/*******************************************************************************
* Function Name  : ENET_TxDscrInit
* Description    : Initializes the Tx ENET descriptor chain with single descriptor
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void ENET_TxDscrInit(void)
{
  /* ENET Start Address */
  EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

  /* Next Descriptor Address */
  EnetDmaTx.Tx.pEnetDmaNextDesc = &EnetDmaTx;

  /* Initialize ENET status and control */
  EnetDmaTx.Tx.TxDesc0.TCH  = 1;
  EnetDmaTx.Tx.TxDesc0.Data = 0;
  EnetDmaTx.Tx.TxDesc1.Data = 0;
  /* Tx next set to Tx descriptor base */
  ETH->DMATDLAR = (uint32_t)&EnetDmaTx;

}

void tapdev_init(void)
{
#if 0
uint32_t i;
  // Put the PHY in reset mode
  PhyWrite(PHY_ADDR,0, 0x8000);
  // Delay to assure PHY reset
  for(i=0; i<0xFFFFF; i++);
  // Set PHY operation mode
  PhyWrite(PHY_ADDR,28, 0);
  // Set PHY operation mode
  PhyWrite(PHY_ADDR,0, PHY_OPR_MODE);
  // DMA Init
  ETH->DMABMR |= (1<<8); /*PBL = 1*/
  ETH->DMAIER = 0;
  ETH->DMASR = 0x0001E7FF;
  ETH->DMAOMR = (1<<20); // flush TX FIFO
  __no_operation();
  __no_operation();
  while(ETH->DMAOMR & (1<<20));
  // MAC init
  ETH->MMCCR |= (1<<3); // freeze statitistic
  //ETH->MACCR_bit.APCS = 1; // automatily padding
  //ETH->MACCR_bit.IPCO = 1; // calculate CRC on RX data
  ETH->MACCR = (1<<11) |   // Full duplex
               (1<<14);   // 100 Mb

  ETH->MACFFR = (1<<4) |  // Pass all multicast
                (3<<6);   // Cotrol packets

 // ETH->MACFFR_bit.RA  = 1; // Pass all
 // ETH->MACFFR_bit.PM  = 1; // Pass all

  ETH->MACFCR = 0;     // Flow control (pause)
  ETH->MACVLANTR = 0;  // VLAN
  ETH->MACPMTCSR = 0;  // Power control
  ETH->MACIMR    = 0;  // disable interrupts

  ETH->MACA0HR   = (UIP_ETHADDR5 << 8) + UIP_ETHADDR4;
  ETH->MACA0LR   = (uint32_t)(UIP_ETHADDR3 << 24) +\
                          (UIP_ETHADDR2 << 16) +\
                          (UIP_ETHADDR1 << 8)  +\
                           UIP_ETHADDR0;
  ETH->MACA1HR = 0x0000FFFF;  // disable seccond mac address
  ETH->MACA2HR = 0x0000FFFF;
  ETH->MACA3HR = 0x0000FFFF;
#endif
  ENET_TxDscrInit();
  ENET_RxDscrInit();

  ETH_Start();
/* ETH->DMARPDR = 1;
  ETH->MACCR |= (1<<2) |
                (1<<3);
  
  ETH->DMAOMR |= (1<<1) |
                 (1<<13);*/
}

/*************************************************************************
 * Function Name: tapdev_read
 * Parameters:
 * Return:
 *
 * Description: Read data for MAC/DMA Controller
 *
 *************************************************************************/
uint32_t tapdev_read(void * pPacket)
{
  uint32_t size;
  /*check for validity*/
  if(0 == EnetDmaRx.Rx.RxDesc0.OWN)
  {
    /*Get the size of the packet*/
    size = EnetDmaRx.Rx.RxDesc0.FL; // CRC
    //MEMCOPY_L2S_BY4((u8*)ppkt, RxBuff, size); /*optimized memcopy function*/
    memcpy(pPacket, RxBuff, size);   //string.h library*/
  }
  else
  {
    return(ENET_NOK);
  }
  /* Give the buffer back to ENET */
  EnetDmaRx.Rx.RxDesc0.OWN = 1;
  /* Start the receive operation */
  ETH->DMARPDR = 1;
  /* Return no error */
  return size;
}

/*************************************************************************
 * Function Name: tapdev_send
 * Parameters:
 * Return:
 *
 * Description: Send data to MAC/DMA Controller
 *
 *************************************************************************/
void tapdev_send(void *pPacket, uint32_t size)
{
  while(EnetDmaTx.Tx.TxDesc0.OWN);

  /* Copy the  application buffer to the driver buffer
     Using this MEMCOPY_L2L_BY4 makes the copy routine faster
     than memcpy */
  //MEMCOPY_L2S_BY4((u8*)TxBuff, (u8*)ppkt, size);
  memcpy(TxBuff, pPacket, size);

  /* Assign ENET address to Temp Tx Array */
  EnetDmaTx.Tx.pBuffer1 = (uint32_t *)TxBuff;

  /* Setting the Frame Length*/
  EnetDmaTx.Tx.TxDesc0.Data = 0;
  EnetDmaTx.Tx.TxDesc0.TCH  = 1;
  EnetDmaTx.Tx.TxDesc0.LSEG = 1;
  EnetDmaTx.Tx.TxDesc0.FS   = 1;
  EnetDmaTx.Tx.TxDesc0.DC   = 0;
  EnetDmaTx.Tx.TxDesc0.DP   = 0;

  EnetDmaTx.Tx.TxDesc1.Data = 0;
  EnetDmaTx.Tx.TxDesc1.TBS1 = (size&0xFFF);

  /* Start the ENET by setting the VALID bit in dmaPackStatus of current descr*/
  EnetDmaTx.Tx.TxDesc0.OWN = 1;

  /* Start the transmit operation */
  ETH->DMATPDR = 1;
}

#if 0
uint16_t PhyRead (uint8_t PhyAddr, uint8_t Reg)
{
  
  ETH->MACMIIAR = (1<<0) |
                  (0<<1) |
                  (Reg<<6) |
                  (PhyAddr<<11);
  while(ETH->MACMIIAR & (1<<0));
  return(ETH->MACMIIDR & (0xFFFF));
}

void PhyWrite (uint8_t PhyAddr, uint8_t Reg, uint16_t Data)
{
  ETH->MACMIIDR = Data;
  ETH->MACMIIAR = (1<<0) |
                  (1<<1) |
                  (Reg<<6) |
                  (PhyAddr<<11);
  while(ETH->MACMIIAR & (1<<0));
}
#endif