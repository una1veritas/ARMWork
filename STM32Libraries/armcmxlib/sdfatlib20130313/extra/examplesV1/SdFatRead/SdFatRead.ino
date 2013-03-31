/*
 * This sketch reads and prints the file
 * PRINT00.TXT created by SdFatPrint.pde or
 * WRITE00.TXT created by SdFatWrite.pde
 */
#include <SdFat.h>
#include <SdFatUtil.h> // use functions to print strings from flash memory

const uint8_t SD_CHIP_SELECT = SS;

SdFat sd;

SdFile file;

// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

void setup(void) {
  Serial.begin(9600);
  while (!Serial) {}  // wait for Leonardo
  PgmPrintln("Type any character to start");
  while (Serial.read() <= 0) {}
  delay(200);  // Catch Due reset problem
  
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(SD_CHIP_SELECT, SPI_HALF_SPEED)) sd.initErrorHalt();
  
  // open a file
  if (file.open("PRINT00.TXT", O_READ)) {
    Serial.println("Opened PRINT00.TXT");
  }
  else if (file.open("WRITE00.TXT", O_READ)) {
    Serial.println("Opened WRITE00.TXT");    
  }
  else{
    error("file.open failed");
  }
  Serial.println();
  
  // copy file to serial port
  int16_t n;
  uint8_t buf[7];// nothing special about 7, just a lucky number.
  while ((n = file.read(buf, sizeof(buf))) > 0) {
    for (uint8_t i = 0; i < n; i++) Serial.write(buf[i]);
  }
  /* easier way
  int16_t c;
  while ((c = file.read()) > 0) Serial.write((char)c);
  */
  Serial.println("\nDone");
}

void loop(void) {}
