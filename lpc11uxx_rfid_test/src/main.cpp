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


uint8 dec8tobcd(uint8 d) {
  return ((d/10%10)<<4) + (d%10);
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

#define SRAM_CS PIO1_23
SPISRAM sram(SPI1, SRAM_CS, SPISRAM::BUS_23LC1024);

CMDSTATUS cmdstatus = IDLE;
uint32 swatch = 0;

void setup() {
  
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
    SD_readkeyid();
  }

  Serial.println("\nSetup finished.\n");
}


int main (void) {
	long lastread = 0, lasttime = 0;
  ISO14443 card, lastcard;
  IDData iddata;
  int i;
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
              displayIDData(card.type, iddata);
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
  uint32_t acc;
  
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
    Serial.println("Auth succeeded.");
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

void displayIDData(uint8 type, IDData & iddata) {
//  uint32_t acc;
  if (type == NFC::CARDTYPE_FELICA_212K ) {
    i2clcd.setCursor(0,0);
    if ( iddata.fcf.division[1] == 0 ) 
      i2clcd.write(iddata.fcf.division, 1);
    else
      i2clcd.write(iddata.fcf.division, 2);
    i2clcd.print('-');
    i2clcd.write(iddata.fcf.pid, 8);
    i2clcd.print('-');
    i2clcd.write((char)iddata.fcf.issue);
    Serial.write(iddata.fcf.division, 2);
    Serial.print('-');
    Serial.write(iddata.fcf.pid, 8);
    Serial.print('-');
    Serial.write((char)iddata.fcf.issue);
    Serial.println();
    //
  } else if (type == NFC::CARDTYPE_MIFARE ) {
    i2clcd.setCursor(0,0);
    i2clcd.write(iddata.iizuka.division, 2);
    i2clcd.print('-');
    i2clcd.write(iddata.iizuka.pid, 8);
    i2clcd.print('-');
    i2clcd.print((char)iddata.iizuka.issue);
    Serial.write(iddata.iizuka.division, 2);
    Serial.print('-');
    Serial.write(iddata.iizuka.pid, 8);
    Serial.print('-');
    Serial.print((char)iddata.iizuka.issue);
    Serial.println();
    /*
    Serial.printBytes(iddata.raw, 16);
    Serial.println();
    Serial.printBytes(iddata.raw+16, 16);
    Serial.println();
    Serial.printBytes(iddata.raw+32, 16);
    Serial.println();
    Serial.printBytes(iddata.raw+48, 16);
    Serial.println();

      Serial.println("Acc. cond.");
      
      Serial.println(TRAILERBITS(B111), BIN);
      Serial.println((DATABLOCKBITS(B111, 0) | DATABLOCKBITS(B111, 1) | DATABLOCKBITS(B111, 2)), BIN);
      acc = ACCESSBITS(iddata.raw+48);
      Serial.println(acc, BIN);
      acc &= ~TRAILERBITS(B111);
      acc |=  TRAILERBITS(B011);
      acc &= ~(DATABLOCKBITS(B111, 0) | DATABLOCKBITS(B111, 1) | DATABLOCKBITS(B111, 2));
      acc |=  DATABLOCKBITS(B010, 0);
      acc |=  DATABLOCKBITS(B000, 1);
      acc |=  DATABLOCKBITS(B000, 2);
      
      acc |= acc<<12;
      acc ^= 0x00000fff;
      Serial.println(acc, BIN);
      Serial.println();
      (iddata.raw+48)[8] = acc>>16 & 0xff;
      (iddata.raw+48)[7] = acc>>8 & 0xff;
      (iddata.raw+48)[6] = acc & 0xff;
      memcpy((iddata.raw+48),factory_a+1, 6);
      memcpy((iddata.raw+48)+10,IizukaKey_b+1, 6);
      Serial.printBytes((iddata.raw+48), 16);
      Serial.println();
      */
    //
  }
  //
  Serial.println();
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
  uint16 count = 0;
  size_t n;
  KeyID id;
  uint32 yy, mm, dd, expdate;
    
  strcpy((char*)tmp, "KEYID.TXT");
	file.open((char*)tmp, SDFatFile::FILE_READ); 
  if ( !file.result() ) {
    Serial << endl << "Contents of file " << (char*)tmp << ": " << endl;
    for (;;) {
      if ( file.gets((TCHAR*) buff, sizeof(buff)) == NULL)
          break;
      if ( file.result() )
        break;
      strm.set(buff, 64);
      if ( buff[0] == '#' )
        // its a comment line.
        continue;
      //
      
      if ( strm.getToken((char*)tmp, 32) != 1 ) 
        continue;
      id.raw[0] = tmp[0]; // assumes 'S'
      n = strm.getToken((char*)tmp, 32);
      if ( n == 0 || n > 8 )
        continue;
      sprintf((char*)id.raw+1, "%08ld", atol((char*)tmp));
      if ( strm.getToken((char*)tmp, 32) != 1 )
        continue;
      id.raw[9] = tmp[0];
      
      expdate = dec8tobcd(strm.parseInt() % 100);
      expdate = expdate<<8 | dec8tobcd(strm.parseInt() % 100);
      expdate = expdate<<8 | dec8tobcd(strm.parseInt() % 100);
      id.setdate(expdate);
      id.setChecksum();
      for(int i = 0; i < 16; i++) {
        Serial.print(id.raw[i], HEX);
        Serial.print(' ');
      }
      
      // write to sram,
      // read then print from sram
      Serial.println();
      count++;
      if ( count % 100 == 0 )
        Serial << count << " " << strm;
    }
    if ( file.result() ) {
      Serial << endl << "Failed while reading." << endl;
      return;
    } else {
      Serial << "Total data count " << count << ". " << endl;
    }
    file.close();
  } else {
    Serial << endl << "Couldn't open " << (char*)tmp << ". " << endl;
    return;
  }

}

void SD_writelog() {
#ifdef FATFS_TEST_WRITE
//    rc = f_open(&Fil, "SD0001.TXT", FA_WRITE | FA_CREATE_ALWAYS);
//    file.open("SD0001.TXT", SDFatFile::FILE_WRITE);
    file.open("SD0001.TXT", SDFatFile::FILE_WRITE);
    if ( file.result() ) { //rc) {
      USART_puts(&usart, "\nCouldn't open SD0001.TXT.\n");
      return;
    }

    swatch = millis();
    // 無限ループでこの関数からは抜けない
    while(1){
      i = sprintf((char*)buff, "%08u ", millis());
      //f_write(&Fil, buff, i, &bw);
      file.write(buff, i);
      //rc = f_write(&Fil, "Strawberry Linux\r\n", 18, &bw);
      file.write((uint8_t *)"Strawberry Linux\r\n", 18);
      //if (rc) 
      if ( file.result() ) 
        break;
      // SDカードに書き出します。
     // f_sync(&Fil);
      file.flush();
      USART_puts(&usart, (const char*)buff);
      USART_puts(&usart, "\n");
      if ( swatch + 000 < millis() ) 
        break;
    }
    //f_close(&Fil);
    file.close();
    USART_puts(&usart, "\nSD File IO test finished.\n");
  	return;
#endif

}


void parse_do_command(StringStream & stream) {
  uint32 tmplong;
  int i;
  
  stream.getToken((char*)tmp, 64);
  if ( match(tmp, "TIME") ) {
    Serial.println("SET TIME");
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
    Serial.println("SET CALENDAR.");
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
    Serial.println("WRITE MODE.");
  } else 
  if ( match(tmp, "TIME") ) {
    if ( stream.available() ) {
      stream.getToken((char*)tmp, 64);
      rtc.time = strtol((char*) tmp, NULL,16);
      rtc.setTime(rtc.time);
    } 
  } else 
  if ( match(tmp, "KEY") ) {
    if ( stream.available() ) {
      for(i = 0; i < 7 && stream.getToken((char*)tmp, 64); i++) {
        if ( match(tmp, "iizuka") ) {
          memcpy(mykey, IizukaKey_b, 7);
          break;
        }
        mykey[i] = strtol((char*) tmp, NULL,16) & 0xff;
      }
    }
    Serial.print("Current key: ");
    Serial.printBytes((uint8*)mykey, 7);
    Serial.println();
  }
  
}

/******************************************************************************
**                            End Of File
******************************************************************************/
