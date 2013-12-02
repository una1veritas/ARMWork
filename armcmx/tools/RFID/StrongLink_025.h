#include <avr/pgmspace.h>

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SoftwareSerial.h>

class SL025 {
      
public:
    static const byte Operation_succeed = 0x00;
    static const byte No_tag = 0x01;
    static const byte Login_succeed = 0x02;
    static const byte Login_fail = 0x03;
    static const byte Read_fail = 0x04;
    static const byte Write_fail = 0x05;
    static const byte Unable_to_read_after_write = 0x06;
    static const byte Address_overflow = 0x08;
    static const byte Not_authenticate = 0x0D;
    static const byte Not_a_value_block = 0x0E;
    static const byte Checksum_error = 0xF0;
    static const byte Command_code_error = 0xF1;
    
    PROGMEM static const prog_char Transport_key_A[7];
    PROGMEM static const prog_char Transport_key_B[7];
    PROGMEM static const prog_char Philips_key[7];
    
private:
	SoftwareSerial rwport;
	byte pin_tagdetect;
	byte pin_wakeinput;
	
	// Card, reader
	byte uid[12];
	byte type;
	byte sector;
	byte accessKey[7];
	
	// protocol status
	byte command, status, checksum;
  byte buffer[18];

    PROGMEM static const prog_char typeNames[];

//    boolean bitAt(byte[], int);

public:
  SL025(byte rx, byte tx, byte detect = 0xff, byte wake = 0xff);
  
  void begin();
  void send(byte cmd, byte * data, int n);
  byte receive(byte * buf);
  
  boolean detectTag();
  boolean select_card();
  byte loggingSector() { return sector; }
  void showStatus();
  byte responseStatus() { return status; }
  
  byte uid_size() { return uid[0]; }
  byte * card_uid() { return uid+1; }
	
  boolean login_sector(byte sec, byte key[7]);
  boolean login_sector(byte sec);
  byte read_block(byte blk, byte * buf);
  void write_block(byte blk, byte * buf);
  byte cardType() { return type; }
  byte * getCardTypeName(byte * buf);
  
  word getAccessCondition();
  void setKey(const byte *, boolean pgm = false);
  byte * key();
          
  byte initialize_value(byte , long & );
  byte read_value(byte , long &);
  byte decrement_value(byte , long &);
  byte increment_value(byte , long &);
  void manage_read_led(byte code);
};

