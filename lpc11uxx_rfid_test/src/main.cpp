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

static uint16 count = 0;

uint32 dectobcd(uint32 val) {
  uint32 t = 0;
  int i;
  for(i = 0; i < 8 && val > 0; i++) {
    t |= (val % 10)<<(i*4);
    val /= 10;
  }
  return t;
}

ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);
PN532  nfcreader(Wire, PN532::I2C_ADDRESS, NFC_IRQ, NFC_RSTPD);
const byte NFCPolling[] = {
  NFC::BAUDTYPE_212K_F,
  NFC::BAUDTYPE_106K_A,
};

byte mykey[8];

void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}

uint8 task_serial(void);
char buff[64];
uint8_t tmp[32];
StringStream strm(buff, 64);

SDFatFile file(SD);
void SD_readparam();
void SD_readkeyid();
void SD_writelog(char *);

SPISRAM sram(SPI1, SRAM_CS, SPISRAM::BUS_23LC1024);

CMDSTATUS cmdstatus = IDLE;
uint32 swatch = 0;

void setup() {
  KeyID id;

  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\nUSART Serial started. \nHello.");


  Serial.print("I2C Bus ");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  Serial.println("started.");
  pinMode(I2C_PULLUP, OUTPUT); // pull up by 4k7
  digitalWrite(I2C_PULLUP, LOW);
  
  // I2C lcd
  Serial.print("I2C LCD ");
  while(1)
    if ( i2clcd.begin() ) break;
  Serial.println("started.");
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print("I2C RTC ");
  while(1)
    if ( rtc.begin() ) break;
  Serial.println("started.");
  rtc.update();
  Serial.println(rtc.time, HEX);
  Serial.println(rtc.cal, HEX);
  
  Serial.print("I2C NFC PN532 ");
  nfcreader.begin();
  while (1) {
    if ( nfcreader.GetFirmwareVersion() && nfcreader.getCommandResponse((uint8_t*)tmp) ) 
      break;
    delay(1000);
  }
  Serial << "ver. " << tmp[0] << " firm. " << tmp[1] << " rev. " << tmp[2];
  Serial.print(" support ");
  Serial.print(tmp[3], BIN);
  Serial.println(" started, ");
  if ( !nfcreader.SAMConfiguration() ) {
		Serial.println("....SAMConfiguration failed. Halt.\n");
		while (1);
	}
  Serial.println("SAM Configured.");
  
  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

  SPI1.begin();
  Serial.println("SPI1 Bus started.");
  sram.begin();
  Serial.print("SPI SRAM ");
  if ( sram.started() )
    Serial.println("started.");
  else
    Serial.println("seems failed to start.");
  //
  
//  delay(5000);
  
  SPI0.begin();
  SD.begin();

  rtc.updateTime();
  SD.settime(rtc.time, rtc.cal);
  Serial.print("result of get_fattime: ");
  Serial.println(get_fattime(), HEX);

  if ( digitalRead(SW_SDDETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Card is in SD slot.");
    Serial.println("Performing tests...\n");
    SD_readparam();
  }

  Serial.println("\nSetup finished.\n");
}


int main (void) {
	long lastread = 0, lasttime = 0;
  ISO14443 card, lastcard;
  IDData iddata;
  char c;
//  char msg[32];
  
  init();
  PWM0_tone(PIO1_13, 800, 250);
  setup();
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  sprintf((char*)tmp, "%02x:%02x:%02x\n%02x/%02x/'%02x\n", 
                rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
                rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
  Serial.print((char*)tmp);

  memcpy(mykey, IizukaKey_b, 7);
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
          sprintf((char*)tmp, "%02x %02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        else
          sprintf((char*)tmp, "%02x:%02x", rtc.time>>16&0x3f, rtc.time>>8&0x7f);
        i2clcd.print((char*)tmp);
        sprintf((char*)tmp, " %02x/%02x/20%02x", rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
        i2clcd.print((char*)tmp);
      }
    }
    
		if ( task.nfc() ) {
      if ( cmdstatus == IDLE || cmdstatus == READ ) {
        if ( nfcreader.InAutoPoll(1, 1, NFCPolling, 2) and nfcreader.getAutoPollResponse(tmp) ) {
          // NbTg, type1, length1, [Tg, ...]
          card.set(nfcreader.target.NFCType, tmp);
          i2clcd.backlightLow();
          if ( cmdstatus == IDLE and (millis() - lastread > 2000 and (millis() - lastread > 5000 or lastcard != card)) ) {
            lastread = millis();
            lastcard = card;
            Serial.print(card);
            sprintf((char*)tmp, " [%02x:%02x:%02x %02x/%02x/20%02x]", 
                  rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
                  rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
            Serial.println((char*)tmp);
            //
            if ( readIDInfo(card, iddata) ) {
              displayIDData((char*)tmp, card.type, iddata);
              i2clcd.setCursor(0,0);
              i2clcd.print((char*)tmp);
              Serial << (char*)tmp << endl;
              SD_writelog((char*)tmp);
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
        if ( nfcreader.InListPassiveTarget(1,NFC::BAUDTYPE_106K_A, tmp, 0) 
        and (nfcreader.getListPassiveTarget(tmp) > 0) ) {
          // tmp must be +1 ed.
          Serial.printBytes(tmp, 16);
          card.set(NFC::CARDTYPE_MIFARE, tmp+1);
          Serial.println(card);
          //get_MifareBlock(card, (IDData &) tmp, mykey);
          //nfcreader.printBytes(tmp, 16);
          
          strcpy((char*)iddata.iizuka.division, "1S");
          strcpy((char*)iddata.iizuka.pid, "82831626");
          iddata.iizuka.issue = '1';
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
      strm.flush();
      lastread = millis();
      do {
        while ( !Serial.available() );
        c = Serial.read();
        if ( c == '\n' || c == '\r' || c == 0 )
          break;
        strm.write(c);
      } while ( millis() < lastread + 10000 );
      if ( strm.length() > 0 ) {
        Serial.print("Request: ");
        Serial.println(strm);
        //
        parse_do_command(strm);
      }
    }
    
  }
  
}


uint8 writeIDInfo(ISO14443 & card, IDData & data) {
  uint8 res;
  uint8_t tmp[16];
//  uint32_t acc;
  
  if ( card.type != NFC::CARDTYPE_MIFARE )
    return 0;
  
  if ( nfcreader.mifare_AuthenticateBlock(4, mykey) 
  && nfcreader.getCommandResponse(&res) && res == 0) {
    Serial.println("Authenticated.");
    if ( nfcreader.mifare_ReadAccessConditions(7>>2, tmp) ) {
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
      if ( get_MifareBlock(card, data, mykey) == 0 ) {
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
  
  strcpy((char*) tmp, "CONFIG.TXT");
	file.open((char*)tmp, SDFatFile::FILE_READ); 
  if ( !file.result() ) {
    Serial << endl << "Contents of file " << (char*)tmp  << ": " << endl;
    for (;;) {
      
      if ( file.gets((TCHAR*) buff, sizeof(buff)) == NULL || file.result() )
        break;
      strm.set(buff, 64);
      if ( strm.peek() == '#' ) {
        Serial.println("Comment: ");
      }
      Serial << strm;
    }
    if ( file.result() ) {
      Serial << endl << "Failed while reading." << endl;
      return;
    }
    file.close();
  } else {
    Serial << endl << "Couldn't open " << (char*)tmp << ". " << endl;
    return;
  }  
}

void SD_readkeyid() {
  size_t len;
  uint32 expdate;
  KeyID id;
  boolean regkeyid = false;
  
  count = 0;
  strcpy((char*)tmp, "KEYID.TXT");
	file.open((char*)tmp, SDFatFile::FILE_READ); 
  if ( !file.result() ) {
    Serial << endl << "Contents of file " << (char*)tmp << ": " << endl;
    for (;;) {
      if ( file.gets((TCHAR*) buff, sizeof(buff)) == NULL)
          break;
      if ( file.result() )
        break;
      if ( buff[0] == '#' )
        // its a comment line.
       continue;
      strm.set(buff, 64);
      len = strm.getToken((char*)tmp, 32);
      if ( match(tmp, "REGKEY") ) {
        regkeyid = true;
        continue;
      } else if ( match((char*)tmp, "END") ) {
        break;
      } else if ( regkeyid && len == 10 ) {
        strncpy((char*)id.raw, (char*) tmp, 10);
        expdate = dectobcd(strm.parseInt());
        if ( expdate < 0x20000000 )
          continue;
        id.setExpdate(expdate);
        id.setChecksum();
        sram.write(id.storeAddress(count), id.raw, 16);
#ifdef DEBUG
        for(int i = 0; i < 16; i++) {
          Serial.print(id.raw[i], HEX);
          Serial.print(' ');
        }
        Serial.println();
#endif 
        count++;
      } else {
        Serial.println("Error!");
      }
    }
    if ( file.result() ) {
      Serial << endl << "Failed while reading." << endl;
      //regkeyid = false;
    }
    file.close();
      if ( regkeyid == true ) {
        Serial << "Total data count " << count << ". " << endl;
      } else {
        count = 0;
      }
      sram.write( 0, (uint8*) &count, sizeof(count) );
  } else {
    Serial << endl << "Couldn't open " << (char*)tmp << ". " << endl;
    return;
  }

}

void SD_writelog(char * str)  {
  
  file.open("CARDLOG.TXT", SDFatFile::FILE_WRITE);
  if ( file.result() ) { //rc) {
    Serial << endl << "Couldn't open CARDLOG.TXT." << endl;
    return;
  }

  file.write(str);
  sprintf((char*)tmp, " [%02x:%02x:%02x %02x/%02x/20%02x]", 
        rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
        rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
  Serial << str << endl;
  file.write(" ");
  file.write((char*)tmp);
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
  
  stream.getToken((char*)tmp, 64);
  if ( match(tmp, "TIME") ) {
    Serial.println("Set time");
    if ( stream.available() ) {
      stream.getToken((char*)tmp, 64);
      tmplong = strtol((char*) tmp, NULL, 16);
      if ( tmplong ) {
        Serial.println(tmplong, HEX);
        rtc.setTime(tmplong);
      }
    } else {
      Serial.println(rtc.time, HEX);
    }
  } else 
  if ( match(tmp, "CAL") ) {
    Serial.println("Set calendar");
    if ( stream.available() ) {
      stream.getToken((char*)tmp, 64);
      tmplong = strtol((char*) tmp, NULL, 16);
      if ( tmplong ) {
        Serial.println(tmplong, HEX);
        rtc.setCalendar(tmplong);
      }
    } else {
      Serial.println(rtc.cal, HEX);
    }
  } else 
  if ( match(tmp, "WRITE") ) {
    cmdstatus = WRITE;
    swatch = 0;
    Serial.println("Into write mode.");
  } else 
  if ( match(tmp, "LOADKEYID") ) {
    Serial.println("Loading Key IDs from SD.");
    SD_readkeyid();
    //
    sram.read( 0, (uint8*) &count, sizeof(count) );
    n = 0;
    swatch = millis();
    for(i = 0; i < count; i++) {
      sram.read(id.storeAddress(i), id.raw, 16);
      strncpy((char*)tmp, (char*)id.raw, 10);
      tmp[10] = 0;
      if ( ! id.check() ) 
        n++;
    }
    swatch = millis() - swatch;
    Serial.print("Scan time in millis: ");
    Serial.println(swatch);
    Serial.print("SRAM stored key checksum errors: ");
    Serial.print(n);
  } else 
  if ( match(tmp, "AUTHKEY") ) {
    if ( stream.available() ) {
      for(i = 0; i < 7 && stream.getToken((char*)tmp, 64); i++) {
        if ( match(tmp, "iizuka") ) {
          memcpy(mykey, IizukaKey_b, 7);
          break;
        }
        mykey[i] = strtol((char*) tmp, NULL,16) & 0xff;
      }
    }
    Serial.print("Card authentication key: ");
    Serial.printBytes((uint8*)mykey, 7);
    Serial.println();
  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
