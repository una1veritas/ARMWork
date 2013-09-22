#include <Spi.h>
#include <Usb.h>

#include <PaSoRi.h>

PaSoRi pasori;

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");

  byte rcode = pasori.begin(); // initialize PaSoRi
  if (rcode != 0) {
    Serial.print("PaSoRi initialization failed! : rcode = ");
    Serial.println(rcode, HEX);
    while (1); // stop
  }
}

void loop()
{
  byte rcode, i;
  pasori.task(); // call this at every loop
  static short syscode = POLLING_EDY;

  // Polling for SFC or Edy each time
  syscode = syscode == POLLING_SUICA ? POLLING_EDY : POLLING_SUICA;
  rcode = pasori.poll(syscode);
  if (rcode) {
    delay(500);
  } else {
    // Polling successful
    Serial.print("FeliCa detected. IDm=");
    for (i = 0; i < 8; i++) {
      Serial.print(pasori.getIDm()[i], HEX);
      Serial.print(" ");
    }
    Serial.print("  PMm=");
    for (i = 0; i < 8; i++) {
      Serial.print(pasori.getPMm()[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");

    if (syscode == POLLING_SUICA) {
      // Read SFC (Suica etc.)
      Serial.println("<< SFC >>");
      readSFC();
    } else {
      // Read Edy
      Serial.println("<< Edy >>");
      readEdy();
    }
    delay(3000);
  }
}

int readSFC()
{
  byte b[16];
  int ret = -1;
  for (int i = 0; i < 32; i++) {
    int rcode = pasori.read_without_encryption02(0x090f,i,b);
    if (rcode) {
      Serial.print("rcode = ");
      Serial.println(rcode, HEX);
      break;
    }
    Serial.print(i, DEC);
    Serial.print(": ");
    Serial.print(b[11]*256+b[10]);
    Serial.print(" YEN  ");
    if (i == 0) ret = (unsigned int)b[11]*256+b[10];
    
    // date
    Serial.print(2000+((b[4]>>1)&0x7f), DEC);
    Serial.print(".");
    Serial.print(((b[4]&0x01)<<3)|((b[5]>>5)&0x07), DEC);
    Serial.print(".");
    Serial.print(b[5]&0x1F, DEC);
    
    // from/to
    Serial.print(" from:");
    Serial.print(b[6], HEX);
    Serial.print("-");
    Serial.print(b[7], HEX);
    Serial.print(" to:");
    Serial.print(b[8], HEX);
    Serial.print("-");
    Serial.print(b[9], HEX);

    Serial.println("");
  }
  return ret;
}

int readEdy()
{
  byte b[16];
  int ret = -1;
  for (int i = 0; i < 32; i++) {
    int rcode = pasori.read_without_encryption02(0x170F,i,b);
    if (rcode) {
      Serial.print("rcode = ");
      Serial.println(rcode, HEX);
      break;
    }
    Serial.print(i, DEC);
    Serial.print(": ");
    Serial.print(b[14]*256+b[15], DEC);
    Serial.println(" YEN");
    if (i == 0) ret = (unsigned int)b[14]*256+b[15];
  }
  return ret;
}

