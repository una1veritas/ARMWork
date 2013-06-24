/*
 * PN532_I2C.h
 *
 *  Created on: 2012/06/30
 *      Author: sin
 */

#ifndef PN532_I2C_H_
#define PN532_I2C_H_

#ifdef ARDUINO
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

#else

#include "armcmx.h"
#include "I2CBus.h"

#endif



class PN532 {
/*
	static const byte I2C_READBIT = (0x01);
	static const byte I2C_BUSY = (0x00);
	static const byte I2C_READY = (0x01);
	static const byte I2C_READYTIMEOUT = (20);
*/
	static const byte PREAMBLE = (0x00);
	static const byte STARTCODE_1 = (0x00);
	static const byte STARTCODE_2 = (0xFF);
	static const byte POSTAMBLE = (0x00);

	static const byte HOSTTOPN532 = (0xD4);

	// PN532 Commands
	static const byte COMMAND_GetFirmwareVersion = (0x02);
	static const byte COMMAND_GetGeneralStatus   = (0x04);
	static const byte COMMAND_SAMConfiguration   = (0x14);
	static const byte COMMAND_PowerDown          = (0x16);

	static const byte COMMAND_InListPassiveTarget = (0x4A);
	static const byte COMMAND_InDataExchange = (0x40);
	static const byte COMMAND_InAutoPoll = (0x60);
	static const byte COMMAND_InCommunicateThru = (0x42);

#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_READREGISTER          (0x06)
	static const byte COMMAND_WriteRegister  	= (0x08);
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)

#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

	static const byte PACKBUFFSIZE = 80;
//
	I2CBus & wire;
	GPIOPin pin_irq; // P70_IRQ
	GPIOPin pin_rst;
	byte i2c_addr;
	//
	byte chksum;
	byte packet[PACKBUFFSIZE];
	//
	byte last_command;
	volatile byte comm_status;
	//

	void wirewrite(byte d) {
		wire.write(d);
	}

	byte wireread() {
		return wire.read();
	}

	void send(byte d);
	void sendpacket(byte n);
	byte receive();
	byte receivepacket(int n);
	byte receivepacket();
	boolean checkACKframe(long timeout = 1000);
	boolean IRQ_wait(long timeout = 1000);

	void send_ack();
	void send_nack();

//	boolean sendCommand(byte cmd, long timeout = 1000);
public:

	struct TargetID {
		byte NFCType;
		byte IDLength;
		union {
			byte IDm[8];
			byte UID[7];
		};
	} target;

	static const byte I2C_ADDRESS = (0x48 >> 1);

	// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
//
	static const byte FELICA_CMD_COMMTHRUEX = 0xa0;
	static const byte FELICA_CMD_ECHO = 0xf0;
	static const byte FELICA_CMD_POLLING = 0x00;
	static const byte FELICA_CMD_REQUESTSERVICE = 0x02;
	static const byte FELICA_CMD_ERQUESTRESPONSE = 0x04;
	static const byte FELICA_CMD_READWITHOUTENCRYPTION = 0x06;
	static const byte FELICA_CMD_WRITEWITHOUTENCRYPTION = 0x08;
	static const byte FELICA_CMD_REQUESTSYSTEMCODE = 0x0c;

/*
	static void printHexString(const byte * a, byte len) {
		for (int i = 0; i < len; i++) {
			USARTSerial3.print(a[i] >> 4 & 0x0f, HEX);
			Serial.print(a[i] & 0x0f, HEX);
			Serial.print(" ");
		}
	}
*/

public:

	PN532(I2CBus & wirex, byte addr = I2C_ADDRESS, GPIOPin irq = NOT_A_PIN, GPIOPin rst = NOT_A_PIN);

	void init();
	inline bool begin() {
		init();
    return GetFirmwareVersion();
	}

	enum STATUS_CODE {
		IDLE = 0,
		COMMAND_ISSUED = 0x01,
		REQUEST_RECEIVE = 0x02,
		ACK_FRAME_RECEIVED = 0x03,
		ACK_NOT_RECEIVED = 0x04,
		NACK_FRAME_RECEIVED = 0x05,
		ERROR_FRAME_RECEIVED,
		RESP_COMMAND_MISSMATCH,
		RESP_RECEIVED,
		CHECKSUMERROR = 0xfa,
		I2CREADY_TIMEOUT = 0xfb,
		WRONG_ACK,
		WRONG_PREAMBLE,
		WRONG_POSTAMBLE
	};
	
	byte status() { return comm_status; }
	byte status(const byte b) { return (comm_status = b); }
	byte command() { return last_command; }
	boolean IRQ_ready(void);

	boolean GetFirmwareVersion();
	boolean GetGeneralStatus();
	enum { SAMMODE_NORMAL = 0x01,
		SAMMODE_VIRTUAL_CARD,
		SAMMODE_WIRED_CARD,
		SAMMODE_DUYAL_CARD
	};
	boolean SAMConfiguration(byte mode = SAMMODE_NORMAL, byte timeout = 0x14, byte use_irq =
			0x01);
	boolean PowerDown(byte wkup, byte genirq = 0x01);
	boolean WriteRegister(word addr, byte value);
	inline boolean CPU_PowerMode(const byte mode = 0x00) {
		return WriteRegister(0x02fc, (mode == 0 ? 0x00 : 0x02) ); // p. 12/200, User Manual Rev. 02
	}


	static const byte BaudrateType_106kbitTypeA = 0x00;
	static const byte BaudrateType_212kbitFeliCa = 0x01;
	static const byte BaudrateType_424kbitFeliCa = 0x02;
	static const byte BaudrateType_106kbitTypeB = 0x03;

	static const byte Type_GenericPassiveTypeA = 0x00;
	static const byte Type_GenericPassive212kbFeliCa = 0x01;
	static const byte Type_GenericPassive424kbFeliCa = 0x02;
	static const byte Type_PassiveTypeB = 0x03;
	static const byte Type_Mifare = 0x10;
	static const byte Type_FeliCa212kb = 0x11;
	static const byte Type_FeliCa424kb = 0x12;
	static const byte Type_Empty = 0xff;

	byte InListPassiveTarget(const byte maxtg, const byte brty, byte * data, const byte initlen);
	byte InAutoPoll(const byte pollnr, const byte per, const byte * types,
			const byte typeslen);
	byte getAutoPollResponse(byte * respo);

	byte InDataExchange(const byte Tg, const byte * data, const byte length);
//	byte InDataExchange(const byte Tg, const byte fcmd, const byte * data, const byte len);
	byte InDataExchange(const byte Tg, const byte micmd, const byte blkaddr,
			const byte * data, const byte datalen);

	byte getCommandResponse(byte * resp, const long & wait = 1000);

	byte getListPassiveTarget(byte * data) {
		byte count = getCommandResponse(packet);
		if (!count)
			return 0;
		//	count -= 2; // remove checksum and postamble bytes.
		memcpy(data, packet, count);
		return packet[0];
	}

	void targetSet(const byte cardtype, const byte * uid, const byte uidLen);
	void targetClear();

	byte mifare_AuthenticateBlock(word blockNumber, const byte * keyData);
	byte mifare_ReadDataBlock(uint8_t blockNumber, uint8_t * data);

	boolean InCommunicateThru(const byte * data, const byte len);
	byte getCommunicateThruResponse(byte * data);

	//	byte felica_DataExchange(const byte cmd, const byte * data, const byte len);
	//	byte felica_getDataExchangeResponse(const byte fcmd, byte * resp);
	byte felica_Polling(byte * resp, const word syscode = 0xffff);
	byte felica_RequestService(byte * resp, const word servcodes[],
			const byte servnum);
	word felica_RequestService(const word servcode);
	byte felica_RequestSystemCode(byte * resp);
	// Block list accepts only two byte codes.
	byte felica_ReadWithoutEncryption(byte * resp, const word servcode,
			const byte blknum, const word blklist[]);
	byte felica_ReadBlocksWithoutEncryption(byte * resp, const word servcode,
			const byte blknum, const word blklist[]);
};

#endif /* PN532_I2C_H_ */
