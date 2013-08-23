/*
 * LPCLDC ported to Keil uV
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPC11Uxx.h"
#include "type.h"
//#include "xprintf.h"
//#include "systick.h"

#include "armcmx.h"
#include "USARTSerial.h"
#include "I2CBus.h"
#include "ST7032i.h"
#include "RTC.h"
#include "PN532_I2C.h"
#include "ISO14443.h"

#include "StringStream.h"

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

// Strawberry Linux original lpclcd port maps

#if defined(LPCLCD)
#define LCD_RST     PIO1_25
#define NFC_IRQ     PIO1_5
#define NFC_RSTPD   PIN_NOT_DEFINED
#define LED_USER    PIO1_6
#define LED_LCDBKLT PIO1_3
#define SW_USERBTN  PIO0_1
#define RXD2        PIO0_18
#define TXD2        PIO0_19
#elif defined (CAPPUCCINO)
#include "cappuccino.h"
#define NFC_RSTPD   PIN_NOT_DEFINED
#define NFC_IRQ     PIO0_17
#define LED_USER    LED_SDBUSY
#endif


ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);
PN532  nfcreader(Wire, PN532::I2C_ADDRESS, NFC_IRQ, NFC_RSTPD);
const byte NFCPolling[] = {
  NFC::BAUDTYPE_212K_F,
  NFC::BAUDTYPE_106K_A,
};

uint8 readIDInfo(ISO14443 & card, IDData & data);
uint8 writeIDInfo(ISO14443 & card, IDData & data);
void displayIDData(uint8 cardtype, IDData &);

uint8 get_MifareBlock(ISO14443 & card, IDData & data, const uint8_t * key);
uint8 get_FCFBlock(ISO14443 & card, IDData & data);

const static byte IizukaKey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const static byte factory_a[] = {
  0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
byte mykey[8];

void init() {
  SystemInit();
  GPIOInit();
  // systick initialize
  start_delay(); // for delay
  
  // initialize xprintf
  //xfunc_out = (void(*)(unsigned char))i2clcd_data;
}

uint8 task_serial(void);
char rxbuf[32];
uint16 rxix;

char msg[32];
uint8_t tmp[32];

struct {
  uint32 master; 
  uint32 serial; 
  uint32 rtc;
  uint32 nfc; 
  uint32 lcdlight;
  uint32 serial_period; 
  uint32 rtc_period;
  uint32 nfc_period; 
  uint32 lcdlight_period; 
} task = { 0, 0, 0, 0, 0, 3, 73, 667, 1000 };

enum CMDSTATUS {
  IDLE = 0,
  WRITE,
  READ
};

void setup() {
  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\nUSART Serial started. \nHello.");

  Serial.print("I2C Bus ");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  Serial.println("started.");
  pinMode(PIO0_7, OUTPUT); // pull up by 4k7
  digitalWrite(PIO0_7, LOW);
  
  // I2C液晶を初期化します
  Serial.print("I2C LCD ");
  while(1)
    //if(!i2clcd_init(0x27)) break;   // 初期化完了ならwhileを抜ける
    if ( i2clcd.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  Serial.println("started.");
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print("I2C RTC ");
  while(1)
    if ( rtc.begin() ) break;
    // 失敗したら初期化を永遠に繰り返す
  Serial.println("started.");
  rtc.updateTime();
  Serial.println(rtc.time, HEX);
  
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
}


int main (void) {
	long lastread = 0, swatch = 0, lasttime = 0;
  ISO14443 card, lastcard;
  IDData iddata;
 
  uint8 cmdstatus = IDLE;
 	char streambuf[64];
  StringStream stream(streambuf, 64);

  init();
  setup();
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  sprintf(msg, "%02x:%02x:%02x\n%02x/%02x/'%02x\n", 
                rtc.time>>16&0x3f, rtc.time>>8&0x7f, rtc.time&0x7f, 
                rtc.cal>>8&0x1f, rtc.cal&0x3f, rtc.cal>>16&0xff);
  Serial.print(msg);

  memcpy(mykey, IizukaKey_b, 7);
  lastread = millis();
  
  while (1){    /* Loop forever */

    if ( task.master != millis() ) {
      if ( task.serial )
        task.serial--;
      if ( task.rtc ) 
        task.rtc--;
      if ( task.nfc )
        task.nfc--;
      if ( task.lcdlight )
        task.lcdlight--;
      task.master = millis();
    }

    // update clock values before polling cards
    if ( task.rtc == 0 ) {
      task.rtc = task.rtc_period;
      //
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
    
		if ( task.nfc == 0 ) {
      task.nfc = task.nfc_period;
      //
      if ( cmdstatus == IDLE || cmdstatus == READ ) {
        if ( nfcreader.InAutoPoll(1, 1, NFCPolling, 2) and nfcreader.getAutoPollResponse(tmp) ) {
          // NbTg, type1, length1, [Tg, ...]
          card.setPassiveTarget(nfcreader.target.NFCType, tmp);
          i2clcd.backlightLow();
          if ( cmdstatus == IDLE and (millis() - lastread > 2000 and (millis() - lastread > 5000 or lastcard != card)) ) {
            lastread = millis();
            lastcard = card;
            Serial.print("\nBy InAutoPoll, ");
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
          Serial.println("Write mode.");
          swatch = millis();
        }
        if ( nfcreader.InListPassiveTarget(1,NFC::BAUDTYPE_106K_A, tmp, 0) 
        and nfcreader.getListPassiveTarget(tmp) ) {
          card.setPassiveTarget(NFC::CARDTYPE_MIFARE, tmp);
          get_MifareBlock(card, (IDData &) tmp, factory_a);
          nfcreader.printBytes(tmp, 16);
          Serial.println();
          iddata.iizuka.division[0] = '1';
          iddata.iizuka.division[0] = 'S';
          strcpy((char*)iddata.iizuka.pid, "82541854");
          iddata.iizuka.issue = '1';
          writeIDInfo(card, iddata);
          cmdstatus = IDLE;
        } else {
          if ( swatch + 5000 < millis() ) {
            Serial.println("Write mode timed out.");
            swatch = 0;
            cmdstatus = IDLE;
          }
        }
      }
    }
    
    if ( task.lcdlight == 0 ) {
      task.lcdlight = task.lcdlight_period;
      if (millis() - lastread > 5000 ) {
        i2clcd.setCursor(0, 0);
        i2clcd.print("                ");
        i2clcd.backlightHigh();
      }
    }
    
    if ( task.serial == 0 ) {
      if ( task_serial() ) {
        stream.flush();
        stream.readLineFrom(rxbuf, 64);
        rxix = 0;
        Serial.print("Request: ");
        Serial.println(rxbuf);
        //
        stream.getToken((char*)tmp, 64);
        if ( match(tmp, "WRITE") ) {
          cmdstatus = WRITE;
          Serial.println("WRITE MODE.");
        } else 
        if ( match(tmp, "TIME") ) {
          if ( stream.available() ) {
            stream.getToken((char*)tmp, 64);
            rtc.time = strtol((char*) tmp, NULL,16);
            rtc.setTime(rtc.time);
          }
          rtc.updateTime();
          Serial.print("Current time: ");
          Serial.println(rtc.time, HEX);
        }
      }
      //
      task.serial = task.serial_period;
    }
    
  }
  
}


uint8 writeIDInfo(ISO14443 & card, IDData & data) {
  uint8 res;
  
  if ( card.type != NFC::CARDTYPE_MIFARE )
    return 0;
  
  if ( nfcreader.mifare_AuthenticateBlock(4, factory_a) 
  && nfcreader.getCommandResponse(&res) && res == 0) {
//    nfcreader.mifare_ReadDataBlock(4, data.raw);
    get_MifareBlock(card, data, factory_a);
    Serial.print("Authenticate response: ");
    Serial.println(res);
    return 1;
  } else {
    Serial.println("Auth block ack failed.");
  }
  return 0;
}

uint8 readIDInfo(ISO14443 & card, IDData & data) {
  uint16_t n;
	switch (card.type) {
    case NFC::CARDTYPE_MIFARE:
      for (n = 0; n < 3 and get_MifareBlock(card, data, mykey) == 0; n++ )
        delay(20);
      if ( n >= 3 ) {
        Serial.println("Unknown Mifare, not an ID.");
        card.clear();
        return 0;
      }
      break;
    case NFC::CARDTYPE_FELICA_212K:
      for (n = 0; n < 3 and get_FCFBlock(card, data) == 0; n++ )
        delay(20);
      if ( n >= 3) {
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
    Serial.println("Auth succeeded to read blocks.");
    nfcreader.mifare_ReadDataBlock(4, data.raw);
    nfcreader.mifare_ReadDataBlock(5, data.raw+16);
    nfcreader.mifare_ReadDataBlock(6, data.raw+32);
    return 1;
  } /* else 
  
  if ( nfcreader.mifare_AuthenticateBlock(4, factory_a) 
  && nfcreader.getCommandResponse(&res) && res == 0) {
    Serial.println("Auth by factory_a");
    nfcreader.mifare_ReadDataBlock(4, data.raw);
    nfcreader.mifare_ReadDataBlock(5, data.raw+16);
    nfcreader.mifare_ReadDataBlock(6, data.raw+32);
    return 1;
  }
  */
  Serial.println("Auth failed to read blocks.");
  return 0;
}

uint8 task_serial(void) {
  int c = 0;

  if ( !Serial.available() ) 
    return 0;

  while ( Serial.available() > 0 ) {
    c = Serial.read();
    rxbuf[rxix] = c;
    if ( c == '\n' || c == '\r' || c == 0 )
      break;
    rxix++;
  }
  rxbuf[rxix] = 0;
  
  if ( (c == '\n' || c == '\r') && rxix > 0 ) 
    return rxix;
  
  return 0;
}

void displayIDData(uint8 type, IDData & iddata) {
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
    Serial.write(iddata.iizuka.pid, 8);
    Serial.print('-');
    Serial.print((char)iddata.iizuka.issue);
    //
    //
    Serial.println();
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
