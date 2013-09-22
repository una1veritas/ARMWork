#ifndef	StrongLinkI2C_h
#define	StrongLinkI2C_h

#if defined(ARDUINO)
#if ARDUINO >= 100
#include <Arduino.h>
#define twiwrite(x) 	Wire.write(x)
#define twiread(x) 		Wire.read(x)
#else
#include "WProgram.h"
#define twiwrite(x) 	Wire.send(x)
#define twiread(x) 		Wire.receive(x)
#endif
#elif defined (ARMCMX)
#define twiwrite(x) Wire.write(x)
#define twiread(x)  Wire.read(x)
#define PROGMEM 
#define prog_char char
//#define pgm_read_byte(x)  *((uint8_t *)(x))
//#define strcpy_P(d,s) strcpy(d,s)
#endif

class StrongLink_I2C {
	// constants
private:
	static const byte CMD_IDLE = 0x00;
	static const byte CMD_SELECT = 0x01;
	static const byte CMD_LOGIN = 0x02;
	static const byte CMD_READ16 = 0x03;
	static const byte CMD_WRITE16 = 0x04;
	static const byte CMD_READ_VALUE = 0x05;
	static const byte CMD_WRITE_VALUE = 0x06;
	static const byte CMD_WRITE_KEY = 0x07;
	static const byte CMD_INC_VALUE = 0x08;
	static const byte CMD_DEC_VALUE = 0x09;
	static const byte CMD_COPY_VALUE = 0x0A;
	static const byte CMD_READ4 = 0x10;
	static const byte CMD_WRITE4 = 0x11;
	static const byte CMD_DOWNLOAD_KEY = 0x12;
	static const byte CMD_LOGIN_VIA_STORED_KEY = 0x13;
	static const byte CMD_SEEK = 0x20;
	static const byte CMD_SET_LED = 0x40;
	static const byte CMD_SLEEP = 0x50;
	static const byte CMD_RESET = 0xFF;
	static const byte CMD_GET_FIRMWARE_VERSION = 0xF0;

	static const byte OK = 0x00;
	static const byte NO_TAG = 0x01;
	static const byte LOGIN_OK = 0x02;
	static const byte LOGIN_FAIL = 0x03;
	static const byte READ_FAIL = 0x04;
	static const byte WRITE_FAIL = 0x05;
	static const byte CANT_VERIFY = 0x06;
	static const byte ADDRESS_OVERFLOW = 0x08;
	static const byte DOWNLOAD_KEY_FAIL = 0x09;
	static const byte COLLISION = 0x0A;
	static const byte KEY_FAIL = 0x0C;
	static const byte NO_LOGIN = 0x0D;
	static const byte NO_VALUE = 0x0E;
	//
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

	PROGMEM static const prog_char Type_Names[];

public:
	static const byte MIFARE_1K = 1;
	static const byte MIFARE_1K_UIDX = 2;
	static const byte MIFARE_ULTRALIGHT = 3;
	static const byte MIFARE_4K = 4;
	static const byte MIFARE_4K_UIDX = 5;
	static const byte MIFARE_DESFIRE = 6;
	static const byte MIFARE_OTHER = 10;

	const static byte UID_MAXLENGTH = 10;
	const static byte CARDINFO_LENGTH = UID_MAXLENGTH + 2;
	const static byte TYPENAME_MAXLENGTH = 12;

	PROGMEM static const prog_char Transport_key_A[7];
	PROGMEM static const prog_char Transport_key_B[7];
	PROGMEM static const prog_char Philips_key[7];

private:
	const static int PACKET_SIZE = 20;

	//
	byte address;
	byte pin_tag;
	byte pin_input;

	struct PacketInfo {
		byte length;
		byte command;
		byte status;
		byte data[PACKET_SIZE - 3];
	} packet;

	void send_command(byte, byte = 0);
	void transmit();
	byte receive(byte);

public:
	// card
	struct CardInfo {
		byte uid_length;
		byte uid[UID_MAXLENGTH];
		byte type;
	} card;

public:
	const static byte SL018_ADDRESS = 0x50;

	StrongLink_I2C(byte addr = SL018_ADDRESS, byte tag_pin = 0xff,
			byte wakeup_pin = 0xff);
	void init();
	void begin() {
		init();
	}

	byte status() {
		return packet.status;
	}
	boolean operation_succeed() {
		return packet.status == Operation_succeed;
	}
	boolean login_succeed() {
		return packet.status == Login_succeed;
	}
	boolean detect() {
		if (pin_tag == 0xff)
			return false;
		return !digitalRead(pin_tag);
	}
	boolean select();
	byte * uid() {
		return card.uid;
	}
	byte uid_length() {
		return card.uid_length;
	}
	byte card_type() {
		return card.type;
	}
	CardInfo & card_info() {
		return card;
	}
	char * get_type_name(char * s, const byte tp);
	char* get_type_name(char * s) { return get_type_name(s, card.type); }

	void get_uid(byte * buf) {
		memcpy(buf, card.uid, UID_MAXLENGTH);
	}
	void get_card_info(CardInfo & buf) {
		memcpy((void*) &buf, (void*) &card, CARDINFO_LENGTH);
	}
	boolean identical(CardInfo & c) {
		return memcmp((void*) &c, (void*) &card, CARDINFO_LENGTH) == 0;
	}
	boolean set_led(const byte onoff);

	boolean get_firmware_version(char buf[]);
	byte firmware_version();

	boolean login_sector(const byte, const byte *);
	boolean read_block(byte baddr, byte blk[]);
	boolean write_block(byte baddr, const byte blk[]);
	boolean read_value(byte baddr, long & lval);
	//  byte * get_data() { return transdata; }

	long value() {
		return *((long*) packet.data);
	}
	boolean manipulate_value(byte cmd, byte baddr, long & lval);
	byte increment_value(byte baddr, long & lval) {
		return manipulate_value(CMD_INC_VALUE, baddr, lval);
	}
	boolean decrement_value(byte baddr, long & lval) {
		return manipulate_value(CMD_DEC_VALUE, baddr, lval);
	}
	boolean initialize_value(byte baddr, long & lval) {
		return manipulate_value(CMD_WRITE_VALUE, baddr, lval);
	}
	boolean copy_value(byte addra, byte addrb);

	boolean write_master_key(const byte sector, const byte * typekey);
	boolean read_access_condition(byte blk, byte mode[]);
	boolean write_trailer(const byte blk, const byte key_a[6],
			const byte mode[4], const byte key_b[6]);
};

#endif
