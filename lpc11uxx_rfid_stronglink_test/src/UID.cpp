/**
 *  @title:  StrongLink SL018/SL030 RFID reader demo
 *  @author: marc@marcboon.com
 *  @see:    http://www.stronglink.cn/english/sl018.htm
 *  @see:    http://www.stronglink.cn/english/sl030.htm
 *
 *  Arduino to SL018/SL030 wiring:
 *  A4/SDA     2     3
 *  A5/SCL     3     4
 *  5V         4     -
 *  GND        5     6
 *  3V3        -     1
 */

#include "armcmx.h"
#include "USARTSerial.h"

#include "I2CBus.h"
#include "StrongLink_I2C.h"

StrongLink_I2C rfid;
int uid_length = 0;
byte uid[StrongLink_I2C::UID_MAXLENGTH];

void setup() {
  Wire.begin();
  Serial.begin(19200);

  // prompt for tag
  Serial.println("Show me your tag");
}

void loop() {
  int i, n;
  char str[StrongLink_I2C::TYPENAME_MAXLENGTH];

  // start seek mode
  if ( rfid.select() ) {
    if ( memcmp( uid, rfid.uid(), sizeof(uid)) ) {
      uid_length = rfid.uid_length();
      rfid.get_uid(uid);
      for ( i = 0; i < uid_length; i++) {
        Serial.print(uid[i]>>4&0x0f, HEX);
        Serial.print(uid[i]&0x0f, HEX);
        Serial.print(" ");
      }
      if ( rfid.card_type() ) {
        Serial.print(" type: ");
        rfid.get_type_name(str);
        Serial.println(str);
      }
    }
  } 
  else {
    if ( uid_length != 0 ) {
      memset(uid, 0, uid_length);
      uid_length = 0;
      Serial.println("Lost.");
      delay(500);
    }
  } 
  delay(50);
  //  }
}





