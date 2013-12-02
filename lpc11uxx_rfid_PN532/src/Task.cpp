#include "Task.h"

boolean Task::update() {
  if ( timer_master != millis() ) {
    timer_master = millis();
    if ( timer_serial )
      timer_serial--;
    if ( timer_rtc ) 
      timer_rtc--;
    if ( timer_nfc )
      timer_nfc--;
    if ( timer_lcdlight )
      timer_lcdlight--;
    return true;
  }
  return false;
}

boolean Task::serial() {
  if ( timer_serial > 0 )
    return false;
  timer_serial = 3;
  return true;
}

boolean Task::rtc() {
  if ( timer_rtc > 0 )
    return false;
  timer_rtc = 73;
  return true;
}

boolean Task::nfc() {
  if ( timer_nfc > 0 )
    return false;
  timer_nfc = 667;
  return true;
}

boolean Task::lcdlight() {
  if ( timer_lcdlight > 0 )
    return false;
  timer_lcdlight = 1000;
  return true;
}

