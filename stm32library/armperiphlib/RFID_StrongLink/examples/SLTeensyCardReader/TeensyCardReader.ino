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
#include <Wire.h>
#include <StrongLinkI2C.h>

StrongLinkI2C rfid(StrongLinkI2C::SL018_ADDRESS, 9);
int uid_length = 0;
byte uid[StrongLinkI2C::UID_MAXLENGTH];

const byte cardkey_a[] = {
  0xAA, 0x4B, 0x61, 0x5A, 0x75, 0x49, 0x69 };
const byte cardkey_b[] = {
  0xBB, 0x63, 0x45, 0x74, 0x55, 0x79, 0x4B };
const byte philips_key[] = { 
  0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void setup() {
  pinMode(9, INPUT);
  
  Wire.begin();
  Serial.begin(19200);

  // prompt for tag
  Serial.println("Show me your tag");
}

void loop() {
  int i, n;
  char str[StrongLinkI2C::TYPENAME_MAXLENGTH];
  byte blk[16];

  // start seek mode
  if ( rfid.detect() && rfid.select() ) {
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
        rfid.type_name(str);
        Serial.println(str);
      }
      if ( rfid.login_sector(1, cardkey_b) 
        && rfid.read_block((1<<2), blk) ) {
        blk[2+8] = 0;
        long id = atol((const char*) (blk+2));
        if ( uid_length > 4 ) {
          Serial.print(*((long *)(uid+4)), HEX);
          Serial.print('.');
        }
        Serial.println(*((long *)uid), HEX);
        Serial.println(id);
        Keyboard.println(id);
      } 
      else {
        Serial.println("login sector or/and read block failed.");
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






