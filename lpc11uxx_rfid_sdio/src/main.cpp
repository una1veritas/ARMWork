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

ST7032i i2clcd(Wire, LED_LCDBKLT, LCD_RST);
RTC rtc(Wire, RTC::ST_M41T62);


void init() {
  SystemInit();
  GPIOInit();
  start_delay(); // for delay
}

#define STRINGSTREAM
#ifdef STRINGSTREAM
char str64[64];
StringStream stream(str64, sizeof(str64));
#endif

SDFatFile file(SD);
void SD_readparam();
void SD_readkeyid();
void SD_writelog(char *);

#define SRAM
#ifdef SRAM
SPISRAM sram(SPI1, SRAM_CS, SPISRAM::BUS_23LC1024);
#endif

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
  

  // PIO1_6 USR LED //  GPIOSetDir(1, 6, 1 ); //  GPIOSetBitValue( 1, 6, 1);
  pinMode(LED_USER, OUTPUT);
  digitalWrite(LED_USER, HIGH);

#ifdef SRAM
  SPI1.begin();
  Serial.println("SPI1 Bus started.");
  sram.begin();
  Serial.print("SPI SRAM ");
  if ( sram.started() )
    Serial.println("started.");
  else
    Serial.println("seems failed to start.");
  //
#endif
  
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
  char c;
  char line[64];
  StringStream cmdline(line, sizeof(line));
  
  init();

  setup();
  // ---------------------------------------
  rtc.updateCalendar();
  rtc.updateTime();
  formattimedate(str64, rtc.time, rtc.cal);
  Serial.print(str64);

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



void SD_readparam() {
  
  strcpy((char*) tmp32, "CONFIG.TXT");
	file.open((char*)tmp32, SDFatFile::FILE_READ); 
  if ( !file.result() ) {
    Serial.print("Contents of file ");
    Serial.println((char*)tmp32);
    for (;;) {
      
      if ( file.gets((TCHAR*) buf, sizeof(buf)) == NULL || file.result() )
        break;
      if ( buf[0] == '#' ) 
        continue;
      stream.clear();
      stream.write(buf);
      while( stream.getToken(buf, 32) ) {
        Serial.print(buf);
        Serial.print(" ");
      }
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

void SD_readkeyid() {
  uint32 expdate;
  KeyID id;
//  boolean regkeyid = false;
  
  count = 0;
	file.open((char*)"KEYID.TXT", SDFatFile::FILE_READ); 
  if ( !file.result() ) {
    Serial << "Loading key ids." << nl;
    for (;;) {
      if ( file.gets((TCHAR*) buf, sizeof(buf)) == NULL) {
        Serial << "gets" << nl;
        break;
      }
      if ( file.result() ) {
        Serial << "result " << (int) file.result() << nl;
        break;
      }
      if ( buf[0] == '#' )
        // its a comment line.
       continue;
#ifdef DEBUG
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
#else
      stream.clear();
      stream.write(buf);
      stream.getToken(buf, 32);
      count++;
      if ( count % 100 == 0 ) {
        Serial << count << '\t' << buf;
        stream.getToken(buf, 32);
        Serial << ' ' << buf << nl;
      }
#endif
    }
    if ( file.result() ) {
      Serial << nl << "Failed while reading." << nl;
      //regkeyid = false;
    }
    file.close();
    Serial << "Total data count " << count << ". " << nl;
#ifdef SRAM
    sram.write( 0, (uint8*) &count, sizeof(count) ); // count == 0 means no data ve been read
#endif
    } else {
    Serial << nl << "Couldn't open " << (char*)tmp32 << ". " << nl;
    return;
  }

}

void SD_writelog(char * str)  {
  
  file.open("CARDLOG.TXT", SDFatFile::FILE_WRITE);
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
  if ( match(tmp32, "LOADKEYID") ) {
    Serial.println("Loading Key IDs from SD.");
    SD_readkeyid();
    //
#ifdef SRAM
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
#endif
  } else {
    Serial.println("?");
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
