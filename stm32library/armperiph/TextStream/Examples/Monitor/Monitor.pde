#include "SoftwareSerial.h"
#include "Monitor.h"

SoftwareSerial nssport(2,3);
Monitor mon(Serial);
Monitor gps(nssport);

char buffer[128];

void setup() {
  mon.begin(9600);
  mon << "Hi." << mon.endl;

//  pinMode(2,INPUT);
//  pinMode(3,OUTPUT);
  gps.begin(4800);
}

void loop() {
  char c;
  if ( mon.readToken(buffer)) {
    mon << "got " << buffer << mon.endl;
  }
  while ( gps.available() ) {
    gps.readToken(buffer);
    mon << buffer << mon.endl;
  }
}

