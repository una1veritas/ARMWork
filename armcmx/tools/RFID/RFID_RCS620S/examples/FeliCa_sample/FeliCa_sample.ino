#include <RCS620S.h>
 
#define COMMAND_TIMEOUT 400
#define POLLING_INTERVAL 500
#define LED_PIN 13
 
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RCS620S rcs620s(Serial1);
 
void setup() { 
  int ret;
 
  pinMode(LED_PIN, OUTPUT);   // for Polling Status
  digitalWrite(LED_PIN, LOW); 
 
   while( !Serial) ;
  Serial.begin(9600);      // for RC-S620/S
//  Serial1.begin(115200);
//  lcd.begin(16, 2);           // for LCD
 
  // initialize RC-S620/S
  ret = rcs620s.initDevice();
  while (!ret) {}             // blocking
}
 
void loop() {
  int ret, i;
 
  // Polling
  digitalWrite(LED_PIN, HIGH);
  rcs620s.timeout = COMMAND_TIMEOUT;
  ret = rcs620s.polling();
 
  //lcd.clear();
 
  if(ret) {
    Serial.print("IDm: ");
    //lcd.print("IDm:");
    //lcd.setCursor(0, 1);
    for(i = 0; i < 8; i++)
    {
      if(rcs620s.idm[i] / 0x10 == 0) Serial.print(0); //lcd.print(0);
      //lcd.print(rcs620s.idm[i], HEX);
      Serial.print(rcs620s.idm[i], HEX);
    }
  } else {
    Serial.print("polling...");
//    lcd.print("Polling...");
  }
 
  rcs620s.rfOff();
  digitalWrite(LED_PIN, LOW);
  delay(POLLING_INTERVAL);
}
