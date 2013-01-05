// example use of LCDI2C library with the web4robot.com i2C lcd display
// On most Arduino boards, SDA (data line) is on analog input pin 4, and
// SCL (clock line) is on analog input pin 5. 
  
#include <Wire.h>
#include "CharacterLCD.h"
#include "ST7032i.h"

LCD_ST7032i lcd;             // Number of lines and i2c address of the display

void setup() { 

  Wire.begin();
  lcd.begin();                          // Init the display, clears the display 
  lcd.print("Hello World!");       // Classic Hello World!
  lcd.setContrast(42);
  /*
  byte bitmap[] = {1,2,3,4,5,6,7,8};
  lcd.createChar(0, bitmap);
  lcd.setCursor(14,0);
  lcd.write((uint8_t)0);
*/  
  for(int i = 0; i < 4; i++) {
    lcd.noDisplay();
    delay(500);
    lcd.display();
    delay(500);
  }
}

void loop() {
  lcd.setCursor(0,1);
  lcd.print(millis()/(float)1000, 3);
  lcd.setCursor(8,1);
  lcd.print("a0: ");
  lcd.print(analogRead(0)*(float)3.3/1023, 2);
}
