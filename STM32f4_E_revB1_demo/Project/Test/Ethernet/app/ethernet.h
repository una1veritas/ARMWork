/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler
 *
 *    (c) Copyright IAR Systems 2009
 *
 *    $Revision: #1 $
 **************************************************************************/

//#include "my_includes.h"

#ifndef  __ETHERNET_H
#define  __ETHERNET_H

#include <stdint.h>
#include "stm32f2xx_conf.h"

#include "ethernet.h"
#include "stm32_eth.h"
#include "clock-arch.h"

#include "timer.h"
#include "uip-conf.h"
#include "uipopt.h"
#include "uip_arp.h"
#include "uip.h"
#include "tapdev.h"
#include "httpd.h"


#define PHY_ADDR  0x15

#define EMAC_MAX_PACKET_SIZE 1520
#define ENET_OK  (1)
#define ENET_NOK (0)
// ENET_OperatingMode
#define PHY_OPR_MODE        0x2100      //  Set the full-duplex mode at 100 Mb/s

uint32_t EthernetPhyInit(void);
uint32_t uIPMain(void);
uint16_t PhyRead (uint8_t PhyAddr, uint8_t Reg);
void PhyWrite (uint8_t PhyAddr, uint8_t Reg, uint16_t Data);
void tapdev_init(void);
uint32_t tapdev_read(void * pPacket);
void tapdev_send(void *pPacket, uint32_t size);
void ENET_RxDscrInit(void);
void ENET_TxDscrInit(void);

#endif  /* __ETHERNET_H */
