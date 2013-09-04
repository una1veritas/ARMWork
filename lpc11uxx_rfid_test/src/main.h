
#include "armcmx.h"

// Strawberry Linux original lpclcd port maps

#if defined(LPCLCD)
#define LCD_RST     PIO1_25
#define LED_USER    PIO1_6
#define LED_LCDBKLT PIO1_3
#define SW_USERBTN  PIO0_1
#define SW_SDDETECT   PIO1_4 
#define RXD2        PIO0_18
#define TXD2        PIO0_19
#define NFC_IRQ     PIO1_5
#define NFC_RSTPD   PIN_NOT_DEFINED
#define I2C_PULLUP      PIO0_7
#elif defined (CAPPUCCINO)
#include "cappuccino.h"
#define NFC_RSTPD   PIN_NOT_DEFINED
#define NFC_IRQ     PIO0_17
#define LED_USER    LED_SDBUSY
#endif


uint8 readIDInfo(ISO14443 & card, IDData & data);
uint8 writeIDInfo(ISO14443 & card, IDData & data);
void displayIDData(uint8 cardtype, IDData &);

uint8 get_MifareBlock(ISO14443 & card, IDData & data, const uint8_t * key);
uint8 get_FCFBlock(ISO14443 & card, IDData & data);

const static byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const static byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


#define ACCESSBITS(x)     ( ((x)[8])<<4 & 0xff0L | ((x)[7])>>4 & 0x000fL )
#define TRAILERBITS(x)    ((((x)&1L)<<3)<<8 | (((x)>>1&1L)<<3)<<4 | (((x)>>2&1L)<<3))
#define DATABLOCKBITS(x, b)    ((((x)&1L)<<(b&3))<<8 | (((x)>>1&1L)<<(b&3))<<4 | (((x)>>2&1L)<<(b&3)))

enum CMDSTATUS {
  IDLE = 0,
  WRITE,
  READ
};

void parse_do_command(StringStream & stream) ;

struct Tasks {
  uint32 timer_master; 
  uint32 timer_serial;
  uint32 timer_rtc;
  uint32 timer_nfc; 
  uint32 timer_lcdlight;
  
  boolean update() {
    if ( timer_master != millis() ) {
      timer_master = millis();
      if ( timer_serial )
        timer_serial--;
      if ( timer_rtc ) 
        timer_rtc--;
      if ( timer_nfc )
        timer_nfc--;
      if ( timer_lcdlight )
        timer_lcdlight--;
      return true;
    }
    return false;
  }
  
  boolean serial() {
    if ( timer_serial > 0 )
      return false;
    timer_serial = 3;
    return true;
  }
  
  boolean rtc() {
    if ( timer_rtc > 0 )
      return false;
    timer_rtc = 73;
    return true;
  }
  
  boolean nfc() {
    if ( timer_nfc > 0 )
      return false;
    timer_nfc = 667;
    return true;
  }
  
  boolean lcdlight() {
    if ( timer_lcdlight > 0 )
      return false;
    timer_lcdlight = 1000;
    return true;
  }
  
} task;

struct KeyIDRecord {
  char keyid[11];  // 11
  uint32 expdate;  // 4
  uint8 xsum;
  
  uint8 set(char kid[11], uint32 date) {
    int i;
    memcpy(keyid, kid, 11);
    expdate = date;
    xsum = 0;
    for (i = 0; i < 15; i++) {
      xsum ^= ((uint8*)this)[i];
    }
    return xsum;
  }
  
  uint8 checksum() {
    int i;
    uint8 chksum = 0;
    for (i = 0; i < 16; i++) {
      chksum ^= ((uint8*)this)[i];
    }
    return chksum == 0;
  }
  
};
