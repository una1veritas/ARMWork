
#include "armcmx.h"

struct Task {
  uint32 timer_master; 
  uint32 timer_serial;
  uint32 timer_rtc;
  uint32 timer_nfc; 
  uint32 timer_lcdlight;
  
  boolean update();
  boolean serial();
  boolean rtc();
  boolean nfc();  
  boolean lcdlight();  
};

