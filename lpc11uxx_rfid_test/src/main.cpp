/*
 * LPCLDC ported to Keil uV
 *
 */

#include "LPC11Uxx.h"
//#include "type.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "SPI.h"
#include "I2CBus.h"
#include "ST7032i.h"
#include "RTC.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "PWM0Tone.h"

#include "StringStream.h"
#include "SDFatFs.h"
#include "SPISRAM.h"

#include "main.h"

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
  static uint16 count = 0;
  static byte authkey[8];
  char buf[64];
  uint8_t tmp32[32];
  uint32 swatch = 0;
  
  enum CMDSTATUS {
    IDLE = 0,
    WRITE,
    READ
  } cmdstatus = IDLE;
};

using namespace global;

uint32 dectobcd(uint32 val) {
  uint32 t = 0;
  int i;
  for(i = 0; i < 8 && val > 0; i++) {
    t |= (val % 10)<<(i*4);
    val /= 10;
  }
  return t;
}

char str64[64];
StringStream stream(str64, 64);


ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);
PN532  nfcreader(Wire, PN532::I2C_ADDRESS, NFC_IRQ, NFC_RSTPD);
const byte NFCPolling[] = {
  NFC::BAUDTYPE_212K_F,
  NFC::BAUDTYPE_106K_A,
};


void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}


//SDFatFs SD(SD_SPI, SD_CS, SD_DETECT, LED_USER);
SDFatFile file(SD);
void SD_readparam();
void SD_readkeyid(const char[]);
void SD_writelog(char *);

SPISRAM sram(SPI1, SRAM_CS, SPISRAM::BUS_23LC1024);

void setup() {
  char str64[64];
  KeyID id;

  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\n\nUSART Serial started. \nHello.");


  Serial.print("Starting I2C Bus, ");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  pinMode(I2C_PULLUP, OUTPUT); // pull up by 4k7
  digitalWrite(I2C_PULLUP, LOW);
  
  // I2C lcd
  Serial.print("LCD, ");
  while(1)
    if ( i2clcd.begin() ) break;
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print("RTC, ");
  while(1)
    if ( rtc.begin() ) break;
  rtc.update();
  formattimedate(str64, rtc.time, rtc.cal);
  Serial.println(str64);
  
  Serial.print("NFC PN532 ");
  nfcreader.begin();
  while (1) {
    if ( nfcreader.GetFirmwareVersion() && nfcreader.getCommandResponse((uint8_t*) tmp32) ) 
      break;
    delay(1000);
  }
  Serial << "ver. " << (char) tmp32[0] << " firm. " << tmp32[1] << " rev. " << tmp32[2];
  Serial.print(" supporting ");
  Serial.print(tmp32[3], BIN);
  if ( !nfcreader.SAMConfiguration() ) {
		Serial.println("....SAMConfiguration failed. Halt.\n");
		while (1);
	}
  Serial.println(".\n");
  
  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

  SPI1.begin();
  Serial.println("Starting SPI1 Bus, ");
  sram.begin();
  if ( sram.started() )
    Serial.print("Serial SRAM ");
  else
    Serial.println("seems failed to start.");
  //
  Serial.println(".");
  
//  delay(5000);
  
//  SPI0.begin();
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  Serial.println("Starting SPI0 Bus, ");
  SD.begin();
  Serial.println("SD Card.");

  rtc.updateTime();
  SD.settime(rtc.time, rtc.cal);
  Serial.print("get_fattime returned ");
  Serial.println(get_fattime(), HEX);

  if ( digitalRead(SD_DETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Reading parameters from SD\n");
    SD_readparam();
  }

  Serial.println("\nSetup finished.\n");
}


int main (void) {
	long lastread = 0, lasttime = 0;
  ISO14443 card, lastcard;
  IDData iddata;
  char c;
  char line[64];
  StringStream cmdline(line, 64);
  
  init();
  PWM0_tone(PIO1_13, 800, 250);
  setup();
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  formattimedate(tmp32, rtc.time, rtc.cal);
  Serial.println((char*)tmp32);

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
        if ( rtc.time & 1 ) 
          sprintf((char*)tmp32, "%02x %02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        else
          sprintf((char*)tmp32, "%02x:%02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        i2clcd.print((char*)tmp32);
        sprintf((char*)tmp32, " %02x/%02x/20%02x", rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
        i2clcd.print((char*)tmp32);
      }
    }
    
		if ( task.nfc() ) {
      if ( cmdstatus == IDLE || cmdstatus == READ ) {
        if ( nfcreader.InAutoPoll(1, 1, NFCPolling, 2) and nfcreader.getAutoPollResponse(tmp32) ) {
          // NbTg, type1, length1, [Tg, ...]
          card.set(nfcreader.target.NFCType, tmp32);
          i2clcd.backlightLow();
          if ( cmdstatus == IDLE and (millis() - lastread > 2000 and (millis() - lastread > 5000 or lastcard != card)) ) {
            lastread = millis();
            lastcard = card;
            Serial.print(card);
            formattimedate(tmp32, rtc.time, rtc.cal);
            Serial.println((char*)tmp32);
            //
            if ( readIDInfo(card, iddata) ) {
              displayIDData((char*)tmp32, card.type, iddata);
              i2clcd.setCursor(0,0);
              i2clcd.print((char*)tmp32);
              Serial << (char*)tmp32 << nl;
              SD_writelog((char*)tmp32);
            } else {
              Serial.println("UNKNOWN CARD.");
            }
          }
        }
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
          
<<<<<<< HEAD
          strcpy((char*)iddata.iizuka.division, "1G");
          strcpy((char*)iddata.iizuka.pid, "10000002");
          iddata.iizuka.issue = '2';
=======
          strcpy((char*)iddata.iizuka.division, "1S");
          strcpy((char*)iddata.iizuka.pid, "82541854");
          iddata.iizuka.issue = '1';
>>>>>>> ea2df371496c2a42d9f58d0a240ff2a604d89693
          writeIDInfo(card, iddata);
          //
          cmdstatus = IDLE;
        } else {
          if ( swatch + 10000 < millis() ) {
            Serial.println("Write mode timed out.");
            cmdstatus = IDLE;
          }
        }
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
        if ( c == '\n' || c == '\r' || c == 0 )
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


uint8 writeIDInfo(ISO14443 & card, IDData & data) {
  uint8 res;
//  uint32_t acc;
  
  if ( card.type != NFC::CARDTYPE_MIFARE )
    return 0;
  
  if ( nfcreader.mifare_AuthenticateBlock(4, authkey) 
  && nfcreader.getCommandResponse(&res) && res == 0) {
    Serial.println("Authenticated.");
    if ( nfcreader.mifare_ReadAccessConditions(7>>2, tmp32) ) {
/*
      acc = ACCESSBITS(tmp);
      Serial.println("Original trailer cond.");
      Serial.printBytes(tmp, 16);
      Serial.println();
      Serial.println(acc, BIN);
      acc &= ~TRAILERBITS(B111);
      acc |=  TRAILERBITS(B011);
      acc &= ~(DATABLOCKBITS(B111, 0) | DATABLOCKBITS(B111, 1) | DATABLOCKBITS(B111, 2));
      acc |=  ( DATABLOCKBITS(B110, 0) | DATABLOCKBITS(B110, 1) | DATABLOCKBITS(B110, 2));
      if ( nfcreader.mifare_WriteAccessConditions(7>>2, acc, factory_a+1, IizukaKey_b+1) ) {
        Serial.println("Succeeded to write trailer block.");
        acc = nfcreader.mifare_ReadAccessConditions(7>>2, tmp);
      } else {
        Serial.println("Trailer block write failed.");
        return 0;
      }
  */    
      if ( nfcreader.mifare_WriteBlock(4, data.raw) )
        Serial.println("Write to data block succeeded.");
      else {
        Serial.println("Data block write failed.");
        return 0;
      }
      
    } else {
      Serial.println("Read trailer block failed.");
      return 0;
    }
    return 1;
  } else {
    Serial.println("Auth block failed.");
  }
  return 0;
}

uint8 readIDInfo(ISO14443 & card, IDData & data) {
  data.clear();
  switch (card.type) {
    case NFC::CARDTYPE_MIFARE:
      if ( get_MifareBlock(card, data, authkey) == 0 ) {
        Serial.println("Unknown Mifare, not an ID.");
        card.clear();
        return 0;
      }
      break;
    case NFC::CARDTYPE_FELICA_212K:
      if ( get_FCFBlock(card, data) == 0 ) {
        Serial.println("Unknown FeliCa, not an FCF.");
        card.clear();
        return 0;
      }
      break;
    default:
      Serial.println("Not supported as ID card.");
      card.clear();
      return 0;
	}
	return 1;
}

uint8 get_FCFBlock(ISO14443 & card, IDData & data) {
  word syscode = 0x00FE;
  int len;
  byte c;

  // Polling command, with system code request.
  len = nfcreader.felica_Polling(data.raw, syscode);
  if ( len == 0 ) 
    return 0;
  
  // low-byte first service code.
  // Suica, Nimoca, etc. 0x090f system 0x0300
  // Edy service 0x170f (0x1317), system 0x00FE // 8280
  // FCF 1a8b
  word servcode = 0x1a8b;
  word scver = nfcreader.felica_RequestService(servcode);
#ifdef DEBUG
  Serial.print("scver = ");
  Serial.println(scver, HEX);
#endif
  if ( scver == 0xffff ) 
    return 0;
  //
  //printf("%04x ver %04x.\n", servcode, scver);
  word blist[] = { 0, 1, 2, 3};
  c = nfcreader.felica_ReadBlocksWithoutEncryption(data.raw, servcode, (byte) 4, blist);
  if ( c == 0 ) {
    //printf("\nfailed reading FCF blocks. \n");
    return 0;
  }
//  printf("\n--- End of FCF reading ---\n\n");
  return 1;
}


uint8 get_MifareBlock(ISO14443 & card, IDData & data, const uint8_t * key) {
  uint8 res;
  nfcreader.targetSet(0x10, card.ID, card.IDLength);
         /* Note !!! Once failed to authentication, card's state will be back to the initial state, 
        So the InListPassivTarget or InAutoPoll should be issued again. */

  if ( nfcreader.mifare_AuthenticateBlock(4, key) 
  && nfcreader.getCommandResponse(&res) && res == 0) {
#ifdef DEBUG
    Serial.println("Auth succeeded.");
#endif
    if ( nfcreader.mifare_ReadBlock(4, data.raw)
      && nfcreader.mifare_ReadBlock(5, data.raw+16)
      && nfcreader.mifare_ReadBlock(6, data.raw+32)
      && nfcreader.mifare_ReadBlock(7, data.raw+48) )
        return 1;
    Serial.println("Read data blocks failed. ");
  } 
  Serial.println("Auth failed to read blocks.");
  return 0;
}

void displayIDData(char * str, uint8 type, IDData & iddata) {
  int i;
  if (type == NFC::CARDTYPE_FELICA_212K ) {
    *str++ = iddata.fcf.division[0];
    *str++ = '-';
    for(i = 0; i < 8; i++)
      *str++ = iddata.fcf.pid[i];
    *str++ = '-';
    *str++ = iddata.fcf.issue;
  } else if (type == NFC::CARDTYPE_MIFARE ) {
    *str++ = iddata.iizuka.division[0];
    *str++ = iddata.iizuka.division[1];
    *str++ = '-';
    for(i = 0; i < 8; i++)
      *str++ = iddata.iizuka.pid[i];
    *str++ = '-';
    *str++ = iddata.iizuka.issue;
  }
  *str = 0;
  //
}


void SD_readparam() {
  strcpy((char*) tmp32, "CONFIG.TXT");
	file.open((char*)tmp32, FA_READ); 
  if ( !file.result() ) {
    Serial.print("Contents of file ");
    Serial.println((char*)tmp32);
    for (;;) {
      
      if ( file.gets((TCHAR*) buf, 64) == NULL || file.result() )
        break;
      if ( buf[0] == '#' ) 
        continue;
      stream.clear();
      stream.write(buf);
      stream.getToken(buf, 32);
      Serial.print(buf);
      Serial.print(": ");
      /*
      while( stream.getToken(buf, 32) ) {
        Serial.print(buf);
        Serial.print(" ");
      }
      */
      Serial.println();
    }
    if ( file.result() ) {
      Serial.println("\nFailed while reading.\n");
      return;
    }
    file.close();
  } else {
    Serial.print("\nCouldn't open ");
    Serial.println((char*)tmp32);
    return;
  }  
}

void SD_readkeyid(const char fname[]) {
  uint32 expdate;
  KeyID id;
//  boolean regkeyid = false;

  count = 0;
	file.open(fname, FA_READ); 
  if ( !file.result() ) {
    Serial << "Loading key ids." << nl;
    for (;;) {
      if ( file.gets((TCHAR*) buf, sizeof(buf)) == NULL) {
        break;
      }
      if ( file.result() ) {
        break;
      }

      stream.clear();
      stream.write(buf);
      
      if ( ((char)stream.peek()) == '#' ) 
        continue;

      if ( stream.getToken(buf, 32) ) {
        // buf contains dvi
        if ( count % 100 == 0 ) {
          Serial.print(count);
          Serial.print(" ");
          Serial.print(buf);
        }
        stream.getToken(buf, 32);
        // buf contains date
        expdate = strtol(buf, 0, 16);
        if ( count % 100 == 0 ) {
          Serial.print(" ");
          Serial.println(expdate, HEX);
        }
      }
      count++;

/*
      strm.clear();
      strm.write(buf);
      if ( strm.getToken((char*)tmp32, 32) == 10 ) {
        strncpy((char*)id.raw, (char*) tmp32, 10);
        expdate = dectobcd(strm.parseInt());
        if ( expdate < 0x20000000 )
          continue;
        id.setExpdate(expdate);
        id.setChecksum();
        sram.write(id.storeAddress(count), id.raw, 16);
        Serial.print(count);
        Serial.print(" ");
        for(int i = 0; i < 10; i++) {
          Serial.print((char) id.raw[i]);
        //  Serial.print(' ');
        }
        Serial.println();
        count++;
      } else {
        Serial.println("Error!");
      }
      */
    }
    if ( file.result() ) {
      Serial << nl << "Failed while reading." << nl;
      //regkeyid = false;
    }
    file.close();
    Serial << "Total data count " << count << ". " << nl;
    sram.write( 0, (uint8*) &count, sizeof(count) ); // count == 0 means no data ve been read
    } else {
      Serial << nl << "Couldn't open " << fname << ". " << nl;
    return;
  }

}

void SD_writelog(char * str)  {
  
  file.open("CARDLOG.TXT",FA_WRITE);
  if ( file.result() ) { //rc) {
    Serial << nl << "Couldn't open CARDLOG.TXT." << nl;
    return;
  }

  formattimedate(tmp32, rtc.time, rtc.cal);
  Serial << str << " " << (char*) tmp32 << nl;
  file.write(str);
  file.write(" ");
  file.write((char*)tmp32);
  if ( file.result() == 0 ) {
    file.flush();
  }
  file.close();
  return;
}


void parse_do_command(StringStream & stream) {
  uint32 tmplong;
  int i, n;
  KeyID id;
  
  Serial.print("> ");  
  stream.getToken((char*)tmp32, 32);
  if ( match(tmp32, "TIME") ) {
    Serial.println("Time");
    if ( stream.available() ) {
      Serial.println(" set ");
      stream.getToken((char*)tmp32, 32);
      tmplong = strtol((char*) tmp32, NULL, 16);
      if ( tmplong ) {
        Serial.println(tmplong, HEX);
        rtc.setTime(tmplong);
      }
    } else {
      Serial.println(rtc.time, HEX);
    }
  } else 
  if ( match(tmp32, "CAL") ) {
    Serial.println("Calendar");
    if ( stream.available() ) {
      Serial.println(" set ");
      stream.getToken((char*) tmp32, 64);
      tmplong = strtol((char*) tmp32, NULL, 16);
      if ( tmplong ) {
        Serial.println(tmplong, HEX);
        rtc.setCalendar(tmplong);
      }
    } else {
      Serial.println(rtc.cal, HEX);
    }
  } else 
  if ( match(tmp32, "WRITE") ) {
    cmdstatus = WRITE;
    swatch = 0;
    Serial.println("Into write mode.");
  } else 
  if ( match(tmp32, "LOADKEYID") ) {
    Serial.println("Loading Key IDs from SD.");
    SD_readkeyid("KEYID.TXT");
    //
    sram.read( 0, (uint8*) &count, sizeof(count) );
    n = 0;
    swatch = millis();
    for(i = 0; i < count; i++) {
      sram.read(id.storeAddress(i), id.raw, 16);
      strncpy((char*) tmp32, (char*)id.raw, 10);
      tmp32[10] = 0;
      if ( ! id.check() ) 
        n++;
    }
    swatch = millis() - swatch;
    Serial.print("Scan time in millis: ");
    Serial.println(swatch);
    Serial.print("SRAM stored key checksum errors: ");
    Serial.print(n);
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
    Serial.print("Card authentication key: ");
    Serial.printBytes(authkey, 7);
    Serial.println();
  } else {
    Serial.println("?");
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
