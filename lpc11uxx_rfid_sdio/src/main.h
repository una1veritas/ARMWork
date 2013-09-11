
#include "armcmx.h"
#include "Print.h"

// Strawberry Linux original lpclcd port maps

#if defined(LPCLCD)
#include "lpclcd.h"
#define SW_SDDETECT   PIO1_4 
#define NFC_IRQ     PIO1_5
#define NFC_RSTPD   PIN_NOT_DEFINED

#define SRAM_CS PIO1_23

#elif defined (CAPPUCCINO)
#include "cappuccino.h"

#define NFC_IRQ     PIO0_17
#define NFC_RSTPD   PIN_NOT_DEFINED
#define LED_USER    LED_SDBUSY

#define SRAM_CS PIO1_23
#endif

#define formattimedate(s, t, d)  (sprintf((char*)(s), " [%02x:%02x:%02x %02x/%02x/20%02x]", \
                  t>>16&0x3f, t>>8&0x7f, t&0x7f, d>>8&0x1f, d&0x3f, d>>16&0xff) )


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



struct KeyID : public Printable {
  uint8 raw[16];
  
static const uint32 COUNT_ADDR = 0x10000;

KeyID() {
    memset(raw, 0x20, 10);
    setExpdate(0);
    raw[14] = 0;
    raw[15] = 0;
  }
  
  void setExpdate(uint32 date) {
    raw[10] = date>>24 & 0xff;
    raw[11] = date>>16 & 0xff;
    raw[12] = date>>8 & 0xff;
    raw[13] = date & 0xff;
  }

  uint32 getExpdate() {
    uint32 d;
    d = raw[10];
    d = (d<<8) | raw[11];
    d = (d<<8) | raw[12];
    d = (d<<8) | raw[13];
    return d;
  }
  
  uint8 setChecksum() {
    int i;
    uint8 xsum = 0;
    for (i = 0; i < 15; i++) {
      xsum ^= raw[i];
    }
    raw[15] = xsum;
    return raw[15];
  }
  
  uint8 check() {
    int i;
    uint8 chksum = 0;
    for (i = 0; i < 16; i++) {
      chksum ^= raw[i];
    }
    return chksum == 0;
  }

  virtual size_t printTo(Print & pr) const {
    int i;
    for(i = 0; i < 9; i++)
      pr.print((char)raw[i]);
    pr.print('-');
    pr.print((char)raw[9]);
    pr.print('-');
    for(i = 10; i < 14; i++) {
      pr.print((int)raw[i]>>4, HEX);
      pr.print((int)raw[i]&0x0f, HEX);
    }
    return 19;
  }
  
};
