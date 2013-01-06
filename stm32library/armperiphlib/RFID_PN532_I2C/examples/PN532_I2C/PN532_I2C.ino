#include <Wire.h>
#include "PN532_I2C.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield

PN532 nfc(PN532::I2C_ADDRESS, IRQ);

void setup() {
  Wire.begin();
  nfc.begin();
  
  Serial.begin(19200);
  byte verstr[4];
  nfc.getFirmwareVersion(verstr);
  Serial.print("Found chip PN5 "); 
  Serial.println((verstr[0]) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print((verstr[1]) & 0xFF, HEX); 
  Serial.print('.'); 
  Serial.println((verstr[2]) & 0xFF, HEX);
  Serial.print('.'); 
  Serial.println((verstr[3]) & 0xFF, HEX);
  Serial.println();
}

void loop() {
}


