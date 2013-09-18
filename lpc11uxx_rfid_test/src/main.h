
#include "armcmx.h"

// Strawberry Linux original lpclcd port maps

#if defined(LPCLCD)
#include "lpclcd.h"

#define SD_SPI  SPI0
#define SD_SCK  PIO1_29
#define SD_MISO PIO0_8
#define SD_MOSI PIO0_9
#define SD_CS   PIO0_2
#define SD_DETECT   PIO1_4 

#define NFC_IRQ     PIO1_5
#define NFC_RSTPD   PIN_NOT_DEFINED

#define SRAM_CS PIO1_23

#elif defined (CAPPUCCINO)
#include "cappuccino.h"

#define SD_SPI  SPI0
#define SD_SCK  PIO1_29
#define SD_MISO PIO0_8
#define SD_MOSI PIO0_9
#define SD_CS   PIO0_2
#define SD_DETECT   PIO1_16 

#define NFC_IRQ     PIO0_17
#define NFC_RSTPD   PIN_NOT_DEFINED
#define LED_USER    LED_SDBUSY

#define SRAM_CS PIO1_23
#endif


uint8 readIDInfo(ISO14443 & card, IDData & data);
uint8 writeIDInfo(ISO14443 & card, IDData & data);
void IDDataString(char *, const uint8 cardtype, const IDData &);

uint8 get_MifareBlock(ISO14443 & card, IDData & data, const uint8_t * key);
uint8 get_FCFBlock(ISO14443 & card, IDData & data);

const static byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const static byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


#define ACCESSBITS(x)     ( ((x)[8])<<4 & 0xff0L | ((x)[7])>>4 & 0x000fL )
#define TRAILERBITS(x)    ((((x)&1L)<<3)<<8 | (((x)>>1&1L)<<3)<<4 | (((x)>>2&1L)<<3))
#define DATABLOCKBITS(x, b)    ((((x)&1L)<<(b&3))<<8 | (((x)>>1&1L)<<(b&3))<<4 | (((x)>>2&1L)<<(b&3)))

#define formattimedate(s, t, d)  (sprintf((char*)(s), "20%02x/%02x/%02x-%02x:%02x:%02x", \
        d>>16&0xff, d>>8&0x1f, d&0x3f, t>>16&0x3f, t>>8&0x7f, t&0x7f ) )


void parse_do_command(StringStream & stream);



struct KeyID {
  uint8 raw[16];

  KeyID();
  void setExpdate(uint32 date);
  uint32 getExpdate();  
  uint8 setChecksum();  
  uint8 check();
  
  inline uint32 size() { return 16; }
} keyid;
