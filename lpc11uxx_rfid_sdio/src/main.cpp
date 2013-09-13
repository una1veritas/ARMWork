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
//#include "PN532_I2C.h"
//#include "ISO14443.h"

#include "PWM0Tone.h"

#include "StringStream.h"

#include "ff.h"
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
static uint16 count = 0;
static byte authkey[8];
KeyID id;
char buf[64];
uint8_t tmp32[32];
char str64[64];
StringStream stream(str64, 64);
  
enum CMDSTATUS {
  IDLE = 0,
  WRITE,
  READ
} cmdstatus = IDLE;

SDFatFile file(SD);
void SD_readparam(const char fname[]);
void SD_readkeyid(const char fname[]);
void SD_writelog(char *);

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

ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);

void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}


void setup() {
//  KeyID id;

  pinMode(SW_USERBTN, INPUT);
  
  Serial.begin(115200, RXD2, TXD2);
  Serial.println("\nUSART Serial started. \nHello.");


  Serial.print("I2C Bus ");
  Wire.begin(); 	/* initialize I2c */
  if ( Wire.status == FALSE )
  	while ( 1 );				/* Fatal error */
  pinMode(I2C_PULLUP, OUTPUT); // pull up by 4k7
  digitalWrite(I2C_PULLUP, LOW);
  
  // I2C lcd
  Serial.print("LCD ");
  while(1)
    if ( i2clcd.begin() ) break;
  i2clcd.clear();
  i2clcd.print("Hello.");
  
  Serial.print("RTC ");
  while(1)
    if ( rtc.begin() ) break;
  Serial.println("started.");
  rtc.update();
  Serial.println(rtc.time, HEX);
  Serial.println(rtc.cal, HEX);
  

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

  SPI1.begin();
  Serial.print("SPI1 Bus ");
  sram.begin();
  Serial.print("Serial SRAM ");
  if ( sram.started() )
    Serial.println("started.");
  else
    Serial.println("start failed.");
  //
  
  Serial.print("SPI0 Bus ");
  SPI0.begin();
  SD.begin();
  Serial.println("SD started.");
  //rtc.updateTime();
  //SD.settime(rtc.time, rtc.cal);
  //Serial.print("result of get_fattime: ");
  //Serial.println(get_fattime(), HEX);

  if ( digitalRead(SW_SDDETECT) == HIGH ) {
    Serial.println("SD slot is empty.");
  } else {
    Serial.println("Performing SD tests...\n");
    SD_readparam("CONFIG.TXT");
  }

  Serial.println("\nSetup finished.\n");
}


int main (void) {
	long lastread = 0, lasttime = 0;
  char c;
  char line[64];
  StringStream cmdline(line, sizeof(line));
  
  init();
  setup();
  PWM0_tone(PIO1_13, 1240, 250);
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  formattimedate(str64, rtc.time, rtc.cal);
  Serial.println(str64);

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



void SD_readparam(const char fname[]) {
 	file.open(fname, FA_READ | FA_OPEN_EXISTING );
  if ( !file.result() ) {
    Serial.print(fname);
    Serial.println(": ");
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
    Serial.println(fname);
    return;
  }  
}

void SD_readkeyid(const char fname[]) {
  uint32 goodthru;
  
  count = 0;
  sram.write( KeyID::COUNT_ADDR, (uint8*) &count, sizeof(count) ); // count == 0 means no data ve been read
	file.open(fname, FA_READ | FA_OPEN_EXISTING); 
  if ( !file.result() ) {
    Serial << fname << ": " << nl;
    for (;;) {
      if ( file.gets((TCHAR*) buf, sizeof(buf)) == NULL) {
        //Serial << "gets failed." << nl;
        break;
      }
      if ( file.result() ) {
        Serial << "error result " << (int) file.result() << nl;
        break;
      }
      if ( buf[0] == '#' )
        // its a comment line.
       continue;
      stream.clear();
      stream.write(buf);
      if ( stream.getToken(buf, 32) == 10 ) {
        strncpy((char*)id.raw, buf, 10);
        stream.getToken(buf, 32);
        goodthru = strtol(buf, 0, 16);
        id.setExpdate(goodthru);
        id.setChecksum();
        sram.write(count<<4, id.raw, 16);
      }
      count++;
      if ( count % 100 == 1 ) {
        id.printTo(Serial);
        Serial.println();
      }
    }
    if ( file.result() ) {
      Serial << nl << "Failed while reading." << nl;
      //regkeyid = false;
    } else {
      Serial << "Total data count " << count << ". " << nl;
      sram.write( KeyID::COUNT_ADDR, (uint8*) &count, sizeof(count) );
    }
    file.close();
  } else {
    Serial << nl << "Couldn't open " << (char*)tmp32 << ". " << nl;
    return;
  }

}

/*
void SD_writelog(char * str)  {
  file.open("CARDLOG.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  if ( file.result() ) { //rc) {
    Serial << nl << "Couldn't open CARDLOG.TXT." << nl;
  } else {
    formattimedate(tmp32, rtc.time, rtc.cal);
    Serial << str << " " << (char*) tmp32 << nl;
    file.write(str);
    file.write(" ");
    file.write((char*)tmp32);
    if ( file.result() == 0 ) {
      file.flush();
    }
    file.close();
  }
  return;
}*/

void SD_writelog(char * str) {
  FRESULT rc;
  int i;
  uint8_t bw;
  rc = f_open(&file.file, "SD0001.TXT", FA_WRITE | FA_OPEN_ALWAYS);
  f_lseek(&file.file, f_size(&file.file));
  if (rc) {
    Serial.print("f_open or f_lseek error ");
    Serial.println(rc);
    goto close_and_exit;
  }
  i = 0;
  // 無限ループでこの関数からは抜けない
  while(i < 100){
    sprintf((char*)tmp32, "Strawberry Linux %d\r\n", i);
    rc = f_write(&file.file, (char*)tmp32, strlen((char*)tmp32), (UINT*)&bw);
    if (rc) {
      Serial.print("f_write error ");
      Serial.println(rc);
      goto close_and_exit;
    }
    Serial.println((char*)tmp32);
    // SDカードに書き出します。
    f_sync(&file.file);
    i++;
  }
  //	return;
close_and_exit:
  f_close(&file.file);
}


void parse_do_command(StringStream & stream) {
  uint32 tl;
  uint32 i, n;
  
  Serial << "> " << stream << nl;
  stream.getToken((char*)tmp32, 32);
  if ( match(tmp32, "TIME") ) {
    Serial.println("Time");
    if ( stream.available() ) {
      Serial.println(" set ");
      stream.getToken((char*)tmp32, 32);
      tl = strtol((char*) tmp32, NULL, 16);
      if ( tl ) {
        Serial.println(tl, HEX);
        rtc.setTime(tl);
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
      tl = strtol((char*) tmp32, NULL, 16);
      if ( tl ) {
        Serial.println(tl, HEX);
        rtc.setCalendar(tl);
      }
    } else {
      Serial.println(rtc.cal, HEX);
    }
  } else 
  if ( match(tmp32, "LOADKEYID") ) {
    Serial << "Loading..." << nl;
    SD_readkeyid("KEYID.TXT");
    //
    sram.read( KeyID::COUNT_ADDR, (uint8*) &count, sizeof(count) );
    Serial << "Key ID count: " << count << nl;
    n = 0;
    tl = millis();
    for(i = 0; i < count; i++) {
      sram.read(i<<4, id.raw, 16);
      if ( ! id.check() ) {
        n++;
        Serial.print(n);
        Serial.print(' ');
        id.printTo(Serial);
        Serial.println();
      }
    }
    tl = millis() - tl;
    Serial << tl << " millis, checksum errors " << n << nl;
  } else 
  if ( match(tmp32, "WRITELOG") ) {
    SD_writelog("Hey!");
  } else {
    Serial.println("?");
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
