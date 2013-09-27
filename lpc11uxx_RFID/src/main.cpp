/*
 * LPCLDC ported to Keil uV
 *
 */

#include <string.h>
#include "LPC11Uxx.h"
//#include "type.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "SPI.h"
#include "I2CBus.h"
#include "ST7032i.h"
#include "RTC.h"

#include "ISO14443.h"

#include "PWM0Tone.h"

#include "StringBuffer.h"
#include "SDFatFs.h"
#include "SPISRAM.h"
#include "ff.h"

#include "main.h"
#include "Task.h"


#define match(x,y)  (strcasecmp((char*)(x), (char*)(y)) == 0)
#define matchn(x,y,n)  (strcasencmp((char*)(x), (char*)(y), (n)) == 0)

#ifdef _LPCXPRESSO_CRP_
#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif


/*******************************************************************************
**   Main Function  main()
*******************************************************************************/

// status variables
namespace global {
  static Task task;
  static byte authkey[8];
  char buf[64];
  uint8_t tmp32[32];
  uint32 swatch = 0;
  bool logon = false;
  enum CMDSTATUS {
    IDLE = 0,
    WRITE,
    READ
  } cmdstatus = IDLE;
};
using namespace global;

char str64[64];
StringBuffer stream(str64, 64);


struct KeyID {
  uint8 raw[16];

  KeyID();
  void setExpdate(uint32 date);
  uint32 getExpdate();  
  uint8 setChecksum();  
  uint8 check();
  
  inline uint32 size() { return 16; }
} keyid;


ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);

SDFatFs SD(SPI0, SD_CS, SD_DETECT, LED_USER);
SDFile file(SD);
void SD_readParams();
uint32 SD_loadKeyID();
void SD_writelog(uint32 date, uint32 time, const char * msg);
void printIDCard(char * str, uint16 len, const ISO14443Card & card, const IDFormat & id);

SPISRAM sram(SPI1, SRAM_CS, SPISRAM::BUS_23LC1024);


uint32 dectobcd(uint32 val) {
  uint32 t = 0;
  int i;
  for(i = 0; i < 8 && val > 0; i++) {
    t |= (val % 10)<<(i*4);
    val /= 10;
  }
  return t;
}


void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}


void setup() {
  KeyID id;

  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\n\nUSART Serial started. \nHello.");

  Serial.print("Starting I2C...");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  pinMode(I2C_PULLUP, OUTPUT); // pull up by 4k7
  digitalWrite(I2C_PULLUP, LOW);
  
  // I2C lcd
  Serial.print(" LCD");
  while(1)
    if ( i2clcd.begin() ) break;
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print(", RTC");
  while(1)
    if ( rtc.begin() ) break;
  rtc.update();
  sprintf((char*)tmp32, "20%02x/%02x/%02x-%02x:%02x:%02x", rtc.cal>>16&0xff, rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f );
  Serial.print(" [");
  Serial.print((char*)tmp32);
  Serial.print("] ");
  
  Serial.print(", NFC Reader ");
  nfcreader.begin();
#if defined(USE_PN532)
  while (1) {
    if ( nfcreader.GetFirmwareVersion() && nfcreader.getCommandResponse((uint8_t*) tmp32) ) 
      break;
    delay(1000);
  }
  Serial << "[ver. " << (char) tmp32[0] << " firm. " << tmp32[1] << " rev. " << tmp32[2] << "]";
  if ( !nfcreader.SAMConfiguration() ) {
		Serial.println("....SAMConfiguration failed. Halt.\n");
		while (1);
	}
#endif
  Serial.println(".\n");
  
  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

  Serial.print("Starting SPI1...");
  SPI1.begin();
  Serial.print(" Serial SRAM");
  sram.begin();
  if ( sram.started() )
    Serial.println(".");
  else
    Serial.println("... seems failed starting.");
  //
  
//  delay(5000);
  
  Serial.println("Starting SPI0...");
  SPI0.begin();
  SD.begin();
  Serial.println(" SD Card.");

  rtc.updateTime();
  SD.set_datetime(rtc.cal, rtc.time);
  Serial << "get_fattime returned ";
  Serial.print(get_fattime(), HEX);
  Serial.println(". ");

  if ( digitalRead(SD_DETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Reading parameters... ");
    SD_readParams();
  }

  Serial.println("\nSetup finished.\n");
}


int main (void) {
	long lastread = 0, lasttime = 0;
  ISO14443Card card, lastcard;
  IDFormat iddata;
  char c;
  char line[64];
  StringBuffer cmdline(line, 64);
  
  init();
  PWM0_tone(PIO1_13, 880, 250);
  setup();
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  
  memcpy(authkey, IizukaKey_b, 7);
  lastread = millis();
  
  while (1){    /* Loop forever */

    task.update();
    
    // update clock values before polling cards
    if ( task.rtc() ) {
      rtc.updateTime();
      if ( lasttime != rtc.time ) {
        lasttime = rtc.time;
        rtc.updateCalendar();
        i2clcd.setCursor(0, 1);
        if ( rtc.time & 1 ) {
          digitalWrite(LED_USER, LOW);
          sprintf((char*)tmp32, "%02x %02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        } else {
          digitalWrite(LED_USER, HIGH);
          sprintf((char*)tmp32, "%02x:%02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        }
        i2clcd.print((char*)tmp32);
        sprintf((char*)tmp32, " %02x/%02x/20%02x", rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
        i2clcd.print((char*)tmp32);
      }
    }
    
		if ( task.nfc() ) {
      if ( cmdstatus == IDLE || cmdstatus == READ ) {
#if defined(USE_PN532)
        if ( nfcreader.InAutoPoll(1, 1, NFCPolling, 2) and nfcreader.getAutoPollResponse(tmp32) ) {
          // NbTg, type1, length1, [Tg, ...]
          card.set(nfcreader.target.type, tmp32);
#elif defined(USE_SL030)
        if ( nfcreader.detect() && nfcreader.select() ) {
          card = nfcreader.target;
#endif
          i2clcd.backlightLow();
          if ( cmdstatus == IDLE and (millis() - lastread > 2000 and (millis() - lastread > 5000 or lastcard != card)) ) {
            lastread = millis();
            lastcard = card;
            //
            if ( getIDInfo(card, iddata, authkey) ) {
              printIDCard(buf, 64, card, iddata);
              i2clcd.setCursor(0,0);
              i2clcd.write((uint8*)buf, 13);
              //stream.clear();
              //stream.write((char*)buf);
              sprintf((char*)tmp32, "20%02x/%02x/%02x-%02x:%02x:%02x", rtc.cal>>16&0xff, rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f );
              Serial.print((char*)tmp32);
              Serial.print(" ");
              Serial.println(buf);
              if ( logon )
                SD_writelog(date, time, buf);
              PWM0_tone(PIO1_13, 1047, 100);
              PWM0_tone(PIO1_13, 1318, 100);
            } else {
              Serial.println("UNKNOWN CARD.");
            }
          }
        }
#ifdef IMPLEMENT_WRITE_MODE
      } else if ( cmdstatus == WRITE ) {
        if ( swatch == 0 ) {
          Serial.println("Enters into write mode.");
          swatch = millis();
        }
        if ( nfcreader.InListPassiveTarget(1,NFC::BAUDTYPE_106K_A, tmp32, 0) 
        and (nfcreader.getListPassiveTarget(tmp32) > 0) ) {
          // tmp must be +1 ed.
          Serial.printBytes(tmp32, 16);
          card.set(NFC::CARDTYPE_MIFARE, tmp32+1);
          Serial.println(card);
          //get_MifareBlock(card, (IDData &) tmp, mykey);
          //nfcreader.printBytes(tmp, 16);
          
          strcpy((char*)iddata.iizuka.division, "1S");
          strcpy((char*)iddata.iizuka.pid, "82541854");
          iddata.iizuka.issue = '1';
          putIDInfo(card, iddata, authkey);
          //
          cmdstatus = IDLE;
        } else {
          if ( swatch + 10000 < millis() ) {
            Serial.println("Write mode timed out.");
            cmdstatus = IDLE;
          }
        }
#endif
      }
    }
    
    if ( task.lcdlight() ) {
      if (millis() - lastread > 5000 ) {
        i2clcd.setCursor(0, 0);
        i2clcd.print("                ");
        i2clcd.backlightHigh();
      }
    }
    
    if ( task.serial() && Serial.available() ) {
      while ( Serial.available() ) {
        c = Serial.read();
        if ( c == 0 )
          break;
        if ( c == '\n' && cmdline.peek() == '\r' ) 
          break;        
        cmdline.write(c);
        if ( cmdline.is_full() )
          break;
      }
      if ( (c == '\n' || c == '\r') && cmdline.length() > 0 ) {
        //
        parse_do_command(cmdline);
        cmdline.clear();
      } 
    }
    
  }
  
}

/*--------*/

void SD_readParams() {
  SD.mount();
	file.open("CONFIG.TXT", FA_READ | FA_OPEN_EXISTING ); 
  if ( ! file.error ) {
    Serial.println("Contents of file: ");
    for (;;) {
      
      if ( !file.getLine((TCHAR*) stream.string(), stream.capacity()) || file.error )
        break;
      stream.reset();
      if ( stream.peek() == '#' ) 
        continue;
      stream.getToken(buf, 32);
      Serial.print(buf);
      if ( match(buf, "log") && stream.getToken(buf, 32) > 0 ) {
        if ( match(buf, "on") ) {
          logon = true;
          Serial.print(" on");
        } else {
          logon = false;
        }
      }
      Serial.println();
    }
    if ( file.error ) {
      Serial.println("\nFailed while reading.\n");
    }
    file.close();
  } else {
    Serial.print("\nCouldn't open file.");
  }  
  SD.unmount();
}

uint32 SD_loadKeyID() {
  uint32 expdate;
  uint32 count = 0;
  KeyID id;
//  boolean regkeyid = false;

  SD.mount();
	file.open("KEYID.TXT", FA_READ | FA_OPEN_EXISTING ); 
  if ( !file.error ) {
    Serial << "Loading key ids." << nl;
    for (;;) {
      if ( !file.getLine((TCHAR*) stream.string(), stream.capacity()) ) {
        break;
      }
      if ( file.error ) {
        break;
      }

      stream.reset();     
      if ( ((char)stream.peek()) == '#' ) 
        continue;

      if ( stream.getToken((char*)keyid.raw, 10)
        && stream.getToken(buf, 8) ) {
          expdate = strtol(buf, 0, 16);
          
          keyid.setExpdate(expdate);
          keyid.setChecksum();
          count++;
          sram.write(count*16, keyid.raw, 16);
      } else {
        Serial.print("Key ID parse error on ");
        Serial.println(count);
      }
    }
    if ( file.error ) {
      Serial << nl << "Failed while reading." << nl;
      //regkeyid = false;
    }
    file.close();
    Serial << "Total data count " << count << ". " << nl;
    strncpy((char*)keyid.raw, "DCOUNT0000", 10);
    keyid.setExpdate(count);
    keyid.setChecksum();
    sram.write(0, (uint8*) &keyid, 16); // count == 0 means no data ve been read
  } else {
    Serial << nl << "Couldn't open KEYID.TXT. " << nl;
  }
  SD.unmount();
  return count;
}

void SD_writelog(uint32 date, uint32 time, const char * msg)  {
  char buf[32];
  SD.set_datetime(date, time);
  SD.mount();
  file.open("CARDLOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  if ( file.error ) {
    Serial.println("Open file CARDLOG.TXT failed.");
    return;
  }
  file.seek(file.size());
  if ( file.error ) 
    Serial.println("Seek error.");
  sprintf(buf, "20%02x/%02x/%02x-%02x:%02x:%02x", date>>16&0xff, date>>8&0x1f, date&0x3f, time>>16&0x3f, time>>8&0x7f, time&0x7f );
  file.print((char*)buf);
  file.print(' ');
  file.print(msg);
  file.print("\r\n");
  if ( !file.error ) 
    file.flush();
  file.close();
  SD.unmount();
  return;
}


void scanKeyDB(const char id[10]) {
  uint32 count = 0;
  uint32 start, end, i;
  int length;
  int errorno;
  int diff;
  char buf[16];
  
  swatch = millis();
  for(i = 0; i < 32; i++) {
    sram.read( 0, (uint8*) &keyid, sizeof(keyid) );
    if ( strncmp((char*)keyid.raw, "DCOUNT", 6) == 0 ) {
      count = keyid.getExpdate();
      break;
    }
  }
  if ( count == 0 )
    return;
  length = strlen(id);
  length = (length > 10 ? 10 : length);
  errorno = 0;
  start = 0;
  end = count;
  while ( start < end ) {
    i = (start + end)>>1;
    sram.read(keyid.size()*i, keyid.raw, 16);
    if ( keyid.check() )
      diff = strncmp((char*)keyid.raw, id, length);
    else {
      errorno++;
      break;
    }
    if ( diff < 0 ) {
      start = i + 1;
    } else if ( diff > 0 ) {
      end = i;
    } else
    if ( diff == 0 ) {
      strncpy(buf, (char*) keyid.raw, 10);
      buf[10] = 0;
      Serial.print(buf);
      Serial.print(" ");
      Serial.print(keyid.getExpdate(), HEX);
      Serial.println();
      break;
    }
  }
  swatch = millis() - swatch;
  Serial.print("Scan time in millis: ");
  Serial.println(swatch);
  Serial.print("SRAM stored key checksum errors: ");
  Serial.print(errorno);
  Serial.println("\n");
}

void parse_do_command(StringBuffer & stream) {
  uint32 tmplong;
  int i;
  uint32 n;
  
  Serial.print("> ");
  stream.getToken((char*)tmp32, 32);
  Serial.print((char*)tmp32);
  Serial.print(' ');
  if ( match(tmp32, "TIME") ) {
    if ( stream.available() && stream.getToken((char*)tmp32, 32) ) {
      Serial.println((char*)tmp32);
      tmplong = strtol((char*) tmp32, NULL, 16);
      if ( tmplong ) 
        rtc.setTime(tmplong);
    }
    rtc.updateTime();
    Serial.println();
    Serial.println(rtc.time, HEX);
  } else 
  if ( match(tmp32, "CAL") ) {
    if ( stream.available() && stream.getToken((char*) tmp32, 64) ) {
      Serial.print((char*)tmp32);
      tmplong = strtol((char*) tmp32, NULL, 16);
      if ( tmplong ) 
        rtc.setCalendar(tmplong);
    }
    rtc.updateCalendar();
    Serial.println();
    Serial.println(rtc.cal, HEX);
  } else 
  if ( match(tmp32, "WRITE") ) {
    cmdstatus = WRITE;
    swatch = 0;
    Serial.println();
  } else 
  if ( match(tmp32, "LOADKEYS") ) {
    n = SD_loadKeyID();
    Serial.print("Loaded ");
    Serial.print(n);
    Serial.println(" key IDs.");
  } else 
  if ( match(tmp32, "FIND") ) {
    if ( stream.available() && stream.getToken((char*)tmp32, 16) ) {
      Serial.print("Search ");
      Serial.println((char*)tmp32);
      scanKeyDB((char*)tmp32);
    }
  } else 
  if ( match(tmp32, "LOG" ) ) {
    if ( stream.available() && stream.getToken((char*)tmp32, 16) ) {
      if ( match(tmp32, "ON") ) 
        logon = true;
      else
        logon = false;
      Serial.println();
    }
    Serial.print("Card logging ");
    if ( logon ) 
      Serial.println("on.");
    else
      Serial.println("off.");
    
  } else
  if ( match(tmp32, "AUTHKEY") ) {
    if ( stream.available() ) {
      for(i = 0; i < 7 && stream.getToken((char*)tmp32, 64); i++) {
        if ( match(tmp32, "iizuka") ) {
          memcpy(authkey, IizukaKey_b, 7);
          break;
        }
        authkey[i] = strtol((char*) tmp32, NULL,16) & 0xff;
      }
    }
    Serial.print("Card auth. key: ");
    Serial.printBytes(authkey, 7);
    Serial.println();
  } else if ( strlen((char*)tmp32) == 0 ) {
    Serial.println();
  } else {
    Serial.println("?");
  }
}


/****/

KeyID::KeyID() {
  memset(raw, 0x20, 10);
  setExpdate(0);
  raw[14] = 0;
  raw[15] = 0;
}

void KeyID::setExpdate(uint32 date) {
  raw[10] = date>>24 & 0xff;
  raw[11] = date>>16 & 0xff;
  raw[12] = date>>8 & 0xff;
  raw[13] = date & 0xff;
}

uint32 KeyID::getExpdate() {
  uint32 d;
  d = raw[10];
  d = (d<<8) | raw[11];
  d = (d<<8) | raw[12];
  d = (d<<8) | raw[13];
  return d;
}

uint8 KeyID::setChecksum() {
  int i;
  uint8 xsum = 0;
  for (i = 0; i < 15; i++) {
    xsum ^= raw[i];
  }
  raw[15] = xsum;
  return raw[15];
}

uint8 KeyID::check() {
  int i;
  uint8 chksum = 0;
  for (i = 0; i < 16; i++) {
    chksum ^= raw[i];
  }
  return chksum == 0;
}


void printIDCard(char * str, uint16 len, const ISO14443Card & card, const IDFormat & id) {
  StringBuffer sbuf(str, len);
  int i;
  if ( card.type == NFC::CARDTYPE_FELICA_212K ) {
    sbuf.write(id.fcf.division[0]);
    sbuf.write('-');
    for(i = 0; i < 8; i++)
      sbuf.write(id.fcf.pid[i]);
    sbuf.write('-');
    sbuf.write(id.fcf.issue);
  } else if ( card.type == NFC::CARDTYPE_MIFARE ) {
    //sbuf.write(id.iizuka.division[0]);
    sbuf.write(id.iizuka.division[1]);
    sbuf.write('-');
    for(i = 0; i < 8; i++)
      sbuf.write(id.iizuka.pid[i]);
    sbuf.write('-');
    sbuf.write(id.iizuka.issue);
  }
  //*str = 0;
  sbuf.write(' ');
  
  switch(card.type) {
  case NFC::CARDTYPE_MIFARE:
  case NFC::CARDTYPE_MIFARE_DESFIRE:
    sbuf.write("Mifare");
    if ( card.type == NFC::CARDTYPE_MIFARE_DESFIRE ) 
      sbuf.write(" DESFire");
    else if ( card.atqa == NFC::ATQA_MIFARE_ULTRALIGHT )
      sbuf.write(" Ultralight");
    else if ( card.atqa == NFC::ATQA_MIFARE_CLASSIC1K )
      sbuf.write(" Classic");
    else if ( card.atqa == NFC::ATQA_MIFARE_CLASSIC4K )
      sbuf.write(" Classic 4k");
    break;
  case NFC::CARDTYPE_FELICA_212K:
    sbuf.write("FeliCa 212kb");
    break;
  case NFC::CARDTYPE_FELICA_424K:
    sbuf.write("FeliCa 424kb");
    break;
  case NFC::CARDTYPE_EMPTY:
    sbuf.write("Type Empty");
    break;
  default:
    sbuf.write("Unknown (");
    sbuf.print(card.type, DEC);
    sbuf.write(")");
    break;
  }
  sbuf.write(' ');
  for(int i = 0; i < card.IDLength; i++) {
    if ( i > 0 ) sbuf.write('-');
    sbuf.print(card.ID[i]>>4, HEX);
    sbuf.print(card.ID[i]&0x0f, HEX);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
