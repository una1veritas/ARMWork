#include <Spi.h>
#include <Usb.h>
#include <PaSoRi.h>

PaSoRi pasori;

const char* url = "http://d.hatena.ne.jp/NeoCat/";
const int url_len = strlen(url);

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

  // Polling for FeliCa
  rcode = pasori.poll();
  if (rcode) {
    delay(500);
  } else {
    // Polling successful
    Serial.print("FeliCa detected. Push URL ...");
    pasori.mobile_felica_push_url(url_len, url);
    delay(3000);
  }
}


