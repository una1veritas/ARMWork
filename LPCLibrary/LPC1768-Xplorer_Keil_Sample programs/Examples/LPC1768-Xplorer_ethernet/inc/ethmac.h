/******************************************************************
 *****                                                        *****
 *****  Name: cs8900.h                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****        adannenb@et.htwk-leipzig.de                     *****
 *****  Func: header-file for cs8900.c                        *****
 *****                                                        *****
 ******************************************************************/

// Modifications by Code Red Technologies for NXP LPC1768
// Filename changed to ethmac.h as no longer for cs8900

// CodeRed - update for new header filename
//#ifndef __CS8900_H
//#define __CS8900_H
#ifndef __ETHMAC_H
#define __ETHMAC_H

#define MYMAC_1              1                   // our ethernet (MAC) address
#define MYMAC_2              2                   // (MUST be unique in LAN!)
#define MYMAC_3              3
#define MYMAC_4              4
#define MYMAC_5              5
#define MYMAC_6              6


// CodeRed - commented out original CS8900 defines
/*
#define IOR                  BIT6                // CS8900's ISA-bus interface pins
#define IOW                  BIT7

// definitions for Crystal CS8900 ethernet-controller
// based on linux-header by Russel Nelson

#define PP_ChipID            0x0000              // offset 0h -> Corp-ID
                                                 // offset 2h -> Model/Product Number
                                                 // offset 3h -> Chip Revision Number

#define PP_ISAIOB            0x0020              // IO base address
#define PP_CS8900_ISAINT     0x0022              // ISA interrupt select
#define PP_CS8900_ISADMA     0x0024              // ISA Rec DMA channel
#define PP_ISASOF            0x0026              // ISA DMA offset
#define PP_DmaFrameCnt       0x0028              // ISA DMA Frame count
#define PP_DmaByteCnt        0x002A              // ISA DMA Byte count
#define PP_CS8900_ISAMemB    0x002C              // Memory base
#define PP_ISABootBase       0x0030              // Boot Prom base
#define PP_ISABootMask       0x0034              // Boot Prom Mask

// EEPROM data and command registers
#define PP_EECMD             0x0040              // NVR Interface Command register
#define PP_EEData            0x0042              // NVR Interface Data Register

// Configuration and control registers
#define PP_RxCFG             0x0102              // Rx Bus config
#define PP_RxCTL             0x0104              // Receive Control Register
#define PP_TxCFG             0x0106              // Transmit Config Register
#define PP_TxCMD             0x0108              // Transmit Command Register
#define PP_BufCFG            0x010A              // Bus configuration Register
#define PP_LineCTL           0x0112              // Line Config Register
#define PP_SelfCTL           0x0114              // Self Command Register
#define PP_BusCTL            0x0116              // ISA bus control Register
#define PP_TestCTL           0x0118              // Test Register

// Status and Event Registers
#define PP_ISQ               0x0120              // Interrupt Status
#define PP_RxEvent           0x0124              // Rx Event Register
#define PP_TxEvent           0x0128              // Tx Event Register
#define PP_BufEvent          0x012C              // Bus Event Register
#define PP_RxMiss            0x0130              // Receive Miss Count
#define PP_TxCol             0x0132              // Transmit Collision Count
#define PP_LineST            0x0134              // Line State Register
#define PP_SelfST            0x0136              // Self State register
#define PP_BusST             0x0138              // Bus Status
#define PP_TDR               0x013C              // Time Domain Reflectometry

// Initiate Transmit Registers
#define PP_TxCommand         0x0144              // Tx Command
#define PP_TxLength          0x0146              // Tx Length

// Adress Filter Registers
#define PP_LAF               0x0150              // Hash Table
#define PP_IA                0x0158              // Physical Address Register

// Frame Location
#define PP_RxStatus          0x0400              // Receive start of frame
#define PP_RxLength          0x0402              // Receive Length of frame
#define PP_RxFrame           0x0404              // Receive frame pointer
#define PP_TxFrame           0x0A00              // Transmit frame pointer

// Primary I/O Base Address. If no I/O base is supplied by the user, then this
// can be used as the default I/O base to access the PacketPage Area.
#define DEFAULTIOBASE        0x0300

// PP_RxCFG - Receive  Configuration and Interrupt Mask bit definition - Read/write
#define SKIP_1               0x0040
#define RX_STREAM_ENBL       0x0080
#define RX_OK_ENBL           0x0100
#define RX_DMA_ONLY          0x0200
#define AUTO_RX_DMA          0x0400
#define BUFFER_CRC           0x0800
#define RX_CRC_ERROR_ENBL    0x1000
#define RX_RUNT_ENBL         0x2000
#define RX_EXTRA_DATA_ENBL   0x4000

// PP_RxCTL - Receive Control bit definition - Read/write
#define RX_IA_HASH_ACCEPT    0x0040
#define RX_PROM_ACCEPT       0x0080
#define RX_OK_ACCEPT         0x0100
#define RX_MULTCAST_ACCEPT   0x0200
#define RX_IA_ACCEPT         0x0400
#define RX_BROADCAST_ACCEPT  0x0800
#define RX_BAD_CRC_ACCEPT    0x1000
#define RX_RUNT_ACCEPT       0x2000
#define RX_EXTRA_DATA_ACCEPT 0x4000

// PP_TxCFG - Transmit Configuration Interrupt Mask bit definition - Read/write
#define TX_LOST_CRS_ENBL     0x0040
#define TX_SQE_ERROR_ENBL    0x0080
#define TX_OK_ENBL           0x0100
#define TX_LATE_COL_ENBL     0x0200
#define TX_JBR_ENBL          0x0400
#define TX_ANY_COL_ENBL      0x0800
#define TX_16_COL_ENBL       0x8000

// PP_TxCMD - Transmit Command bit definition - Read-only and
// PP_TxCommand - Write-only
#define TX_START_5_BYTES     0x0000
#define TX_START_381_BYTES   0x0040
#define TX_START_1021_BYTES  0x0080
#define TX_START_ALL_BYTES   0x00C0
#define TX_FORCE             0x0100
#define TX_ONE_COL           0x0200
#define TX_NO_CRC            0x1000
#define TX_RUNT              0x2000

// PP_BufCFG - Buffer Configuration Interrupt Mask bit definition - Read/write
#define GENERATE_SW_INTERRUPT      0x0040
#define RX_DMA_ENBL                0x0080
#define READY_FOR_TX_ENBL          0x0100
#define TX_UNDERRUN_ENBL           0x0200
#define RX_MISS_ENBL               0x0400
#define RX_128_BYTE_ENBL           0x0800
#define TX_COL_COUNT_OVRFLOW_ENBL  0x1000
#define RX_MISS_COUNT_OVRFLOW_ENBL 0x2000
#define RX_DEST_MATCH_ENBL         0x8000

// PP_LineCTL - Line Control bit definition - Read/write
#define SERIAL_RX_ON         0x0040
#define SERIAL_TX_ON         0x0080
#define AUI_ONLY             0x0100
#define AUTO_AUI_10BASET     0x0200
#define MODIFIED_BACKOFF     0x0800
#define NO_AUTO_POLARITY     0x1000
#define TWO_PART_DEFDIS      0x2000
#define LOW_RX_SQUELCH       0x4000

// PP_SelfCTL - Software Self Control bit definition - Read/write
#define POWER_ON_RESET       0x0040
#define SW_STOP              0x0100
#define SLEEP_ON             0x0200
#define AUTO_WAKEUP          0x0400
#define HCB0_ENBL            0x1000
#define HCB1_ENBL            0x2000
#define HCB0                 0x4000
#define HCB1                 0x8000

// PP_BusCTL - ISA Bus Control bit definition - Read/write
#define RESET_RX_DMA         0x0040
#define MEMORY_ON            0x0400
#define DMA_BURST_MODE       0x0800
#define IO_CHANNEL_READY_ON  0x1000
#define RX_DMA_SIZE_64K      0x2000
#define ENABLE_IRQ           0x8000

// PP_TestCTL - Test Control bit definition - Read/write
#define LINK_OFF             0x0080
#define ENDEC_LOOPBACK       0x0200
#define AUI_LOOPBACK         0x0400
#define BACKOFF_OFF          0x0800
#define FDX_8900             0x4000

// PP_RxEvent - Receive Event Bit definition - Read-only
#define RX_IA_HASHED         0x0040
#define RX_DRIBBLE           0x0080
#define RX_OK                0x0100
#define RX_HASHED            0x0200
#define RX_IA                0x0400
#define RX_BROADCAST         0x0800
#define RX_CRC_ERROR         0x1000
#define RX_RUNT              0x2000
#define RX_EXTRA_DATA        0x4000
#define HASH_INDEX_MASK      0xFC00              // Hash-Table Index Mask (6 Bit)

// PP_TxEvent - Transmit Event Bit definition - Read-only
#define TX_LOST_CRS          0x0040
#define TX_SQE_ERROR         0x0080
#define TX_OK                0x0100
#define TX_LATE_COL          0x0200
#define TX_JBR               0x0400
#define TX_16_COL            0x8000
#define TX_COL_COUNT_MASK    0x7800

// PP_BufEvent - Buffer Event Bit definition - Read-only
#define SW_INTERRUPT         0x0040
#define RX_DMA               0x0080
#define READY_FOR_TX         0x0100
#define TX_UNDERRUN          0x0200
#define RX_MISS              0x0400
#define RX_128_BYTE          0x0800
#define TX_COL_OVRFLW        0x1000
#define RX_MISS_OVRFLW       0x2000
#define RX_DEST_MATCH        0x8000

// PP_LineST - Ethernet Line Status bit definition - Read-only
#define LINK_OK              0x0080
#define AUI_ON               0x0100
#define TENBASET_ON          0x0200
#define POLARITY_OK          0x1000
#define CRS_OK               0x4000

// PP_SelfST - Chip Software Status bit definition
#define ACTIVE_33V           0x0040
#define INIT_DONE            0x0080
#define SI_BUSY              0x0100
#define EEPROM_PRESENT       0x0200
#define EEPROM_OK            0x0400
#define EL_PRESENT           0x0800
#define EE_SIZE_64           0x1000

// PP_BusST - ISA Bus Status bit definition
#define TX_BID_ERROR         0x0080
#define READY_FOR_TX_NOW     0x0100

// The following block defines the ISQ event types
#define ISQ_RX_EVENT         0x0004
#define ISQ_TX_EVENT         0x0008
#define ISQ_BUFFER_EVENT     0x000C
#define ISQ_RX_MISS_EVENT    0x0010
#define ISQ_TX_COL_EVENT     0x0012

#define ISQ_EVENT_MASK       0x003F              // ISQ mask to find out type of event

// Ports for I/O-Mode
#define RX_FRAME_PORT        0x0000
#define TX_FRAME_PORT        0x0000
#define TX_CMD_PORT          0x0004
#define TX_LEN_PORT          0x0006
#define ISQ_PORT             0x0008
#define ADD_PORT             0x000A
#define DATA_PORT            0x000C

#define AUTOINCREMENT        0x8000              // Bit mask to set Bit-15 for autoincrement

// EEProm Commands
#define EEPROM_WRITE_EN      0x00F0
#define EEPROM_WRITE_DIS     0x0000
#define EEPROM_WRITE_CMD     0x0100
#define EEPROM_READ_CMD      0x0200

// Receive Header of each packet in receive area of memory for DMA-Mode
#define RBUF_EVENT_LOW       0x0000              // Low byte of RxEvent
#define RBUF_EVENT_HIGH      0x0001              // High byte of RxEvent
#define RBUF_LEN_LOW         0x0002              // Length of received data - low byte
#define RBUF_LEN_HI          0x0003              // Length of received data - high byte
#define RBUF_HEAD_LEN        0x0004              // Length of this header

// CodeRed - end of original CS8900 defines
*/

// *******
// CodeRed -  defines for LPC1768 ethernet
// *******

/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define NUM_RX_FRAG         4           /* Num.of RX Fragments 4*1536= 6.0kB */
#define NUM_TX_FRAG         2           /* Num.of TX Fragments 3*1536= 4.6kB */
#define ETH_FRAG_SIZE       1536        /* Packet Fragment size 1536 Bytes   */

#define ETH_MAX_FLEN        1536        /* Max. Ethernet Frame Size          */

/* EMAC variables located in AHB SRAM bank 1*/
// Below is base address for first silicon
//#define RX_DESC_BASE        0x20004000
// Below is base address for production silicon
#define RX_DESC_BASE        0x2007c000
#define RX_STAT_BASE        (RX_DESC_BASE + NUM_RX_FRAG*8)
#define TX_DESC_BASE        (RX_STAT_BASE + NUM_RX_FRAG*8)
#define TX_STAT_BASE        (TX_DESC_BASE + NUM_TX_FRAG*8)
#define RX_BUF_BASE         (TX_STAT_BASE + NUM_TX_FRAG*4)
#define TX_BUF_BASE         (RX_BUF_BASE  + NUM_RX_FRAG*ETH_FRAG_SIZE)

/* RX and TX descriptor and status definitions. */
#define RX_DESC_PACKET(i)   (*(unsigned int *)(RX_DESC_BASE   + 8*i))
#define RX_DESC_CTRL(i)     (*(unsigned int *)(RX_DESC_BASE+4 + 8*i))
#define RX_STAT_INFO(i)     (*(unsigned int *)(RX_STAT_BASE   + 8*i))
#define RX_STAT_HASHCRC(i)  (*(unsigned int *)(RX_STAT_BASE+4 + 8*i))
#define TX_DESC_PACKET(i)   (*(unsigned int *)(TX_DESC_BASE   + 8*i))
#define TX_DESC_CTRL(i)     (*(unsigned int *)(TX_DESC_BASE+4 + 8*i))
#define TX_STAT_INFO(i)     (*(unsigned int *)(TX_STAT_BASE   + 4*i))
#define RX_BUF(i)           (RX_BUF_BASE + ETH_FRAG_SIZE*i)
#define TX_BUF(i)           (TX_BUF_BASE + ETH_FRAG_SIZE*i)

/* MAC Configuration Register 1 */
#define MAC1_REC_EN         0x00000001  /* Receive Enable                    */
#define MAC1_PASS_ALL       0x00000002  /* Pass All Receive Frames           */
#define MAC1_RX_FLOWC       0x00000004  /* RX Flow Control                   */
#define MAC1_TX_FLOWC       0x00000008  /* TX Flow Control                   */
#define MAC1_LOOPB          0x00000010  /* Loop Back Mode                    */
#define MAC1_RES_TX         0x00000100  /* Reset TX Logic                    */
#define MAC1_RES_MCS_TX     0x00000200  /* Reset MAC TX Control Sublayer     */
#define MAC1_RES_RX         0x00000400  /* Reset RX Logic                    */
#define MAC1_RES_MCS_RX     0x00000800  /* Reset MAC RX Control Sublayer     */
#define MAC1_SIM_RES        0x00004000  /* Simulation Reset                  */
#define MAC1_SOFT_RES       0x00008000  /* Soft Reset MAC                    */

/* MAC Configuration Register 2 */
#define MAC2_FULL_DUP       0x00000001  /* Full Duplex Mode                  */
#define MAC2_FRM_LEN_CHK    0x00000002  /* Frame Length Checking             */
#define MAC2_HUGE_FRM_EN    0x00000004  /* Huge Frame Enable                 */
#define MAC2_DLY_CRC        0x00000008  /* Delayed CRC Mode                  */
#define MAC2_CRC_EN         0x00000010  /* Append CRC to every Frame         */
#define MAC2_PAD_EN         0x00000020  /* Pad all Short Frames              */
#define MAC2_VLAN_PAD_EN    0x00000040  /* VLAN Pad Enable                   */
#define MAC2_ADET_PAD_EN    0x00000080  /* Auto Detect Pad Enable            */
#define MAC2_PPREAM_ENF     0x00000100  /* Pure Preamble Enforcement         */
#define MAC2_LPREAM_ENF     0x00000200  /* Long Preamble Enforcement         */
#define MAC2_NO_BACKOFF     0x00001000  /* No Backoff Algorithm              */
#define MAC2_BACK_PRESSURE  0x00002000  /* Backoff Presurre / No Backoff     */
#define MAC2_EXCESS_DEF     0x00004000  /* Excess Defer                      */

/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP       0x00000015  /* Recommended value for Full Duplex */
#define IPGT_HALF_DUP       0x00000012  /* Recommended value for Half Duplex */

/* Non Back-to-Back Inter-Packet-Gap Register */
#define IPGR_DEF            0x00000012  /* Recommended value                 */

/* Collision Window/Retry Register */
#define CLRT_DEF            0x0000370F  /* Default value                     */

/* PHY Support Register */
#define SUPP_SPEED          0x00000100  /* Reduced MII Logic Current Speed   */

/* Test Register */
#define TEST_SHCUT_PQUANTA  0x00000001  /* Shortcut Pause Quanta             */
#define TEST_TST_PAUSE      0x00000002  /* Test Pause                        */
#define TEST_TST_BACKP      0x00000004  /* Test Back Pressure                */

/* MII Management Configuration Register */
#define MCFG_SCAN_INC       0x00000001  /* Scan Increment PHY Address        */
#define MCFG_SUPP_PREAM     0x00000002  /* Suppress Preamble                 */
#define MCFG_CLK_SEL        0x0000001C  /* Clock Select Mask                 */
#define MCFG_RES_MII        0x00008000  /* Reset MII Management Hardware     */

#define MCFG_CLK_DIV28      0x0000001C  /* MDC = hclk / 28 */
#define MCFG_CLK_DIV36      0x00000020
#define MCFG_CLK_DIV64		0x0000003c

/* MII Management Command Register */
#define MCMD_READ           0x00000001  /* MII Read                          */
#define MCMD_SCAN           0x00000002  /* MII Scan continuously             */

#define MII_WR_TOUT         0x00050000  /* MII Write timeout count           */
#define MII_RD_TOUT         0x00050000  /* MII Read timeout count            */

/* MII Management Address Register */
#define MADR_REG_ADR        0x0000001F  /* MII Register Address Mask         */
#define MADR_PHY_ADR        0x00001F00  /* PHY Address Mask                  */

/* MII Management Indicators Register */
#define MIND_BUSY           0x00000001  /* MII is Busy                       */
#define MIND_SCAN           0x00000002  /* MII Scanning in Progress          */
#define MIND_NOT_VAL        0x00000004  /* MII Read Data not valid           */
#define MIND_MII_LINK_FAIL  0x00000008  /* MII Link Failed                   */

/* Command Register */
#define CR_RX_EN            0x00000001  /* Enable Receive                    */
#define CR_TX_EN            0x00000002  /* Enable Transmit                   */
#define CR_REG_RES          0x00000008  /* Reset Host Registers              */
#define CR_TX_RES           0x00000010  /* Reset Transmit Datapath           */
#define CR_RX_RES           0x00000020  /* Reset Receive Datapath            */
#define CR_PASS_RUNT_FRM    0x00000040  /* Pass Runt Frames                  */
#define CR_PASS_RX_FILT     0x00000080  /* Pass RX Filter                    */
#define CR_TX_FLOW_CTRL     0x00000100  /* TX Flow Control                   */
#define CR_RMII             0x00000200  /* Reduced MII Interface             */
#define CR_FULL_DUP         0x00000400  /* Full Duplex                       */

/* Status Register */
#define SR_RX_EN            0x00000001  /* Enable Receive                    */
#define SR_TX_EN            0x00000002  /* Enable Transmit                   */

/* Transmit Status Vector 0 Register */
#define TSV0_CRC_ERR        0x00000001  /* CRC error                         */
#define TSV0_LEN_CHKERR     0x00000002  /* Length Check Error                */
#define TSV0_LEN_OUTRNG     0x00000004  /* Length Out of Range               */
#define TSV0_DONE           0x00000008  /* Tramsmission Completed            */
#define TSV0_MCAST          0x00000010  /* Multicast Destination             */
#define TSV0_BCAST          0x00000020  /* Broadcast Destination             */
#define TSV0_PKT_DEFER      0x00000040  /* Packet Deferred                   */
#define TSV0_EXC_DEFER      0x00000080  /* Excessive Packet Deferral         */
#define TSV0_EXC_COLL       0x00000100  /* Excessive Collision               */
#define TSV0_LATE_COLL      0x00000200  /* Late Collision Occured            */
#define TSV0_GIANT          0x00000400  /* Giant Frame                       */
#define TSV0_UNDERRUN       0x00000800  /* Buffer Underrun                   */
#define TSV0_BYTES          0x0FFFF000  /* Total Bytes Transferred           */
#define TSV0_CTRL_FRAME     0x10000000  /* Control Frame                     */
#define TSV0_PAUSE          0x20000000  /* Pause Frame                       */
#define TSV0_BACK_PRESS     0x40000000  /* Backpressure Method Applied       */
#define TSV0_VLAN           0x80000000  /* VLAN Frame                        */

/* Transmit Status Vector 1 Register */
#define TSV1_BYTE_CNT       0x0000FFFF  /* Transmit Byte Count               */
#define TSV1_COLL_CNT       0x000F0000  /* Transmit Collision Count          */

/* Receive Status Vector Register */
#define RSV_BYTE_CNT        0x0000FFFF  /* Receive Byte Count                */
#define RSV_PKT_IGNORED     0x00010000  /* Packet Previously Ignored         */
#define RSV_RXDV_SEEN       0x00020000  /* RXDV Event Previously Seen        */
#define RSV_CARR_SEEN       0x00040000  /* Carrier Event Previously Seen     */
#define RSV_REC_CODEV       0x00080000  /* Receive Code Violation            */
#define RSV_CRC_ERR         0x00100000  /* CRC Error                         */
#define RSV_LEN_CHKERR      0x00200000  /* Length Check Error                */
#define RSV_LEN_OUTRNG      0x00400000  /* Length Out of Range               */
#define RSV_REC_OK          0x00800000  /* Frame Received OK                 */
#define RSV_MCAST           0x01000000  /* Multicast Frame                   */
#define RSV_BCAST           0x02000000  /* Broadcast Frame                   */
#define RSV_DRIB_NIBB       0x04000000  /* Dribble Nibble                    */
#define RSV_CTRL_FRAME      0x08000000  /* Control Frame                     */
#define RSV_PAUSE           0x10000000  /* Pause Frame                       */
#define RSV_UNSUPP_OPC      0x20000000  /* Unsupported Opcode                */
#define RSV_VLAN            0x40000000  /* VLAN Frame                        */

/* Flow Control Counter Register */
#define FCC_MIRR_CNT        0x0000FFFF  /* Mirror Counter                    */
#define FCC_PAUSE_TIM       0xFFFF0000  /* Pause Timer                       */

/* Flow Control Status Register */
#define FCS_MIRR_CNT        0x0000FFFF  /* Mirror Counter Current            */

/* Receive Filter Control Register */
#define RFC_UCAST_EN        0x00000001  /* Accept Unicast Frames Enable      */
#define RFC_BCAST_EN        0x00000002  /* Accept Broadcast Frames Enable    */
#define RFC_MCAST_EN        0x00000004  /* Accept Multicast Frames Enable    */
#define RFC_UCAST_HASH_EN   0x00000008  /* Accept Unicast Hash Filter Frames */
#define RFC_MCAST_HASH_EN   0x00000010  /* Accept Multicast Hash Filter Fram.*/
#define RFC_PERFECT_EN      0x00000020  /* Accept Perfect Match Enable       */
#define RFC_MAGP_WOL_EN     0x00001000  /* Magic Packet Filter WoL Enable    */
#define RFC_PFILT_WOL_EN    0x00002000  /* Perfect Filter WoL Enable         */

/* Receive Filter WoL Status/Clear Registers */
#define WOL_UCAST           0x00000001  /* Unicast Frame caused WoL          */
#define WOL_BCAST           0x00000002  /* Broadcast Frame caused WoL        */
#define WOL_MCAST           0x00000004  /* Multicast Frame caused WoL        */
#define WOL_UCAST_HASH      0x00000008  /* Unicast Hash Filter Frame WoL     */
#define WOL_MCAST_HASH      0x00000010  /* Multicast Hash Filter Frame WoL   */
#define WOL_PERFECT         0x00000020  /* Perfect Filter WoL                */
#define WOL_RX_FILTER       0x00000080  /* RX Filter caused WoL              */
#define WOL_MAG_PACKET      0x00000100  /* Magic Packet Filter caused WoL    */

/* Interrupt Status/Enable/Clear/Set Registers */
#define INT_RX_OVERRUN      0x00000001  /* Overrun Error in RX Queue         */
#define INT_RX_ERR          0x00000002  /* Receive Error                     */
#define INT_RX_FIN          0x00000004  /* RX Finished Process Descriptors   */
#define INT_RX_DONE         0x00000008  /* Receive Done                      */
#define INT_TX_UNDERRUN     0x00000010  /* Transmit Underrun                 */
#define INT_TX_ERR          0x00000020  /* Transmit Error                    */
#define INT_TX_FIN          0x00000040  /* TX Finished Process Descriptors   */
#define INT_TX_DONE         0x00000080  /* Transmit Done                     */
#define INT_SOFT_INT        0x00001000  /* Software Triggered Interrupt      */
#define INT_WAKEUP          0x00002000  /* Wakeup Event Interrupt            */

/* Power Down Register */
#define PD_POWER_DOWN       0x80000000  /* Power Down MAC                    */

/* RX Descriptor Control Word */
#define RCTRL_SIZE          0x000007FF  /* Buffer size mask                  */
#define RCTRL_INT           0x80000000  /* Generate RxDone Interrupt         */

/* RX Status Hash CRC Word */
#define RHASH_SA            0x000001FF  /* Hash CRC for Source Address       */					                                                         
#define RHASH_DA            0x001FF000  /* Hash CRC for Destination Address  */

/* RX Status Information Word */
#define RINFO_SIZE          0x000007FF  /* Data size in bytes                */
#define RINFO_CTRL_FRAME    0x00040000  /* Control Frame                     */
#define RINFO_VLAN          0x00080000  /* VLAN Frame                        */
#define RINFO_FAIL_FILT     0x00100000  /* RX Filter Failed                  */
#define RINFO_MCAST         0x00200000  /* Multicast Frame                   */
#define RINFO_BCAST         0x00400000  /* Broadcast Frame                   */
#define RINFO_CRC_ERR       0x00800000  /* CRC Error in Frame                */
#define RINFO_SYM_ERR       0x01000000  /* Symbol Error from PHY             */
#define RINFO_LEN_ERR       0x02000000  /* Length Error                      */
#define RINFO_RANGE_ERR     0x04000000  /* Range Error (exceeded max. size)  */
#define RINFO_ALIGN_ERR     0x08000000  /* Alignment Error                   */
#define RINFO_OVERRUN       0x10000000  /* Receive overrun                   */
#define RINFO_NO_DESCR      0x20000000  /* No new Descriptor available       */
#define RINFO_LAST_FLAG     0x40000000  /* Last Fragment in Frame            */
#define RINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

#define RINFO_ERR_MASK     (RINFO_FAIL_FILT | RINFO_CRC_ERR   | RINFO_SYM_ERR | \
                            RINFO_LEN_ERR   | RINFO_ALIGN_ERR | RINFO_OVERRUN)

/* TX Descriptor Control Word */
#define TCTRL_SIZE          0x000007FF  /* Size of data buffer in bytes      */
#define TCTRL_OVERRIDE      0x04000000  /* Override Default MAC Registers    */
#define TCTRL_HUGE          0x08000000  /* Enable Huge Frame                 */
#define TCTRL_PAD           0x10000000  /* Pad short Frames to 64 bytes      */
#define TCTRL_CRC           0x20000000  /* Append a hardware CRC to Frame    */
#define TCTRL_LAST          0x40000000  /* Last Descriptor for TX Frame      */
#define TCTRL_INT           0x80000000  /* Generate TxDone Interrupt         */

/* TX Status Information Word */
#define TINFO_COL_CNT       0x01E00000  /* Collision Count                   */
#define TINFO_DEFER         0x02000000  /* Packet Deferred (not an error)    */
#define TINFO_EXCESS_DEF    0x04000000  /* Excessive Deferral                */
#define TINFO_EXCESS_COL    0x08000000  /* Excessive Collision               */
#define TINFO_LATE_COL      0x10000000  /* Late Collision Occured            */
#define TINFO_UNDERRUN      0x20000000  /* Transmit Underrun                 */
#define TINFO_NO_DESCR      0x40000000  /* No new Descriptor available       */
#define TINFO_ERR           0x80000000  /* Error Occured (OR of all errors)  */

/* ENET Device Revision ID */
#define OLD_EMAC_MODULE_ID  0x39022000  /* Rev. ID for first rev '-'         */

/* DP83848C PHY Registers */
#define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
#define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
#define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
#define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
#define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
#define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
#define PHY_REG_ANER        0x06        /* Auto-Neg. Expansion Register      */
#define PHY_REG_ANNPTR      0x07        /* Auto-Neg. Next Page TX            */

/* PHY Extended Registers */
#define PHY_REG_STS         0x10        /* Status Register                   */
#define PHY_REG_MICR        0x11        /* MII Interrupt Control Register    */
#define PHY_REG_MISR        0x12        /* MII Interrupt Status Register     */
#define PHY_REG_FCSCR       0x14        /* False Carrier Sense Counter       */
#define PHY_REG_RECR        0x15        /* Receive Error Counter             */
#define PHY_REG_PCSR        0x16        /* PCS Sublayer Config. and Status   */
#define PHY_REG_RBR         0x17        /* RMII and Bypass Register          */
#define PHY_REG_LEDCR       0x18        /* LED Direct Control Register       */
#define PHY_REG_PHYCR       0x19        /* PHY Control Register              */
#define PHY_REG_10BTSCR     0x1A        /* 10Base-T Status/Control Register  */
#define PHY_REG_CDCTRL1     0x1B        /* CD Test Control and BIST Extens.  */
#define PHY_REG_EDCR        0x1D        /* Energy Detect Control Register    */

#define PHY_FULLD_100M      0x2100      /* Full Duplex 100Mbit               */
#define PHY_HALFD_100M      0x2000      /* Half Duplex 100Mbit               */
#define PHY_FULLD_10M       0x0100      /* Full Duplex 10Mbit                */
#define PHY_HALFD_10M       0x0000      /* Half Duplex 10MBit                */
#define PHY_AUTO_NEG        0x3000      /* Select Auto Negotiation           */

#define DP83848C_DEF_ADR    0x0100      /* Default PHY device address        */
#define DP83848C_ID         0x20005C90  /* PHY Identifier                    */



/*************************************************
 * CodeRed - PHY definitions for RDB1768 rev 2
 * which uses SMSC LAN8720 PHY instead of DP83848C
 *************************************************/
#define LAN8720_ID          0x0007C0F1  /* PHY Identifier                    */




// Code Red - not required for RDB1768 port
/*
// typedefs
typedef struct {                                 // struct to store CS8900's
  unsigned int Addr;                             // init-sequence
  unsigned int Data;
} TInitSeq;
*/

// Code Red - moved into tcpip.c
/*
// constants
const unsigned char MyMAC[] =                    // "M1-M2-M3-M4-M5-M6"
{
  MYMAC_1, MYMAC_2, MYMAC_3,
  MYMAC_4, MYMAC_5, MYMAC_6
};
*/

// Code Red - not required for RDB1768 port
/*
const TInitSeq InitSeq[] =
{
  PP_IA, MYMAC_1 + (MYMAC_2 << 8),               // set our MAC as Individual Address
  PP_IA + 2, MYMAC_3 + (MYMAC_4 << 8),
  PP_IA + 4, MYMAC_5 + (MYMAC_6 << 8),
  PP_LineCTL, SERIAL_RX_ON | SERIAL_TX_ON,       // configure the Physical Interface
  PP_RxCTL, RX_OK_ACCEPT | RX_IA_ACCEPT | RX_BROADCAST_ACCEPT
};
*/

// prototypes

// CodeRed - updated for LPC1768 port
/*
void Init8900(void);
void Write8900(unsigned char Address, unsigned int Data);
void WriteFrame8900(unsigned int Data);
unsigned int Read8900(unsigned char Address);
unsigned int ReadFrame8900(void);
unsigned int ReadHB1ST8900(unsigned char Address);
unsigned int ReadFrameBE8900(void);
void CopyToFrame8900(void *Source, unsigned int Size);
void CopyFromFrame8900(void *Dest, unsigned int Size);
void DummyReadFrame8900(unsigned int Size);
void RequestSend(unsigned int FrameSize);
unsigned int Rdy4Tx(void);
*/

void Init_EthMAC(void);
unsigned short ReadFrameBE_EthMAC(void);
void CopyToFrame_EthMAC(void *Source, unsigned int Size);
void CopyFromFrame_EthMAC(void *Dest, unsigned short Size);
void DummyReadFrame_EthMAC(unsigned short Size);
void RequestSend(unsigned short FrameSize);
unsigned int Rdy4Tx(void);
unsigned short StartReadingFrame(void);
void StopReadingFrame(void);
unsigned int CheckIfFrameReceived(void); 

#endif

