/*
 * PN532_I2C.cpp
 *
 *  Created on: 2012/06/30
 *      Author: sin
 */

#include <string.h>

//#include <Wire.h>
#include "PN532_I2C.h"

//#define PN532DEBUG
//#define MIFAREDEBUG
//#define PN532COMM
//#define FELICADEBUG

PN532::PN532(I2CWire & wirex, byte addr, GPIOPin irq, GPIOPin rst) :
		wire(wirex) {
	i2c_addr = addr;
	pin_irq = irq;
	pin_rst = rst;
	if (pin_irq != PIN_NOT_DEFINED) {
		pinMode(pin_irq, INPUT);
		digitalWrite(pin_irq, HIGH); // pull-up
	}
	if (pin_rst != PIN_NOT_DEFINED) {
		pinMode(pin_rst, OUTPUT);
		digitalWrite(pin_rst, HIGH);
	}
	chksum = 0;
	comm_status = IDLE;
	last_command = IDLE;
	target.IDLength = 0;
}

void PN532::init() {
	// Reset the PN532
	if (pin_rst != 0xff) {
		digitalWrite(pin_rst, LOW);
		delay(50);
		digitalWrite(pin_rst, HIGH);
		delay(150);
	}
	chksum = 0;
	comm_status = IDLE;
	last_command = IDLE;
	target.IDLength = 0;
}

inline void PN532::send(byte d) {
//	Serial.print('_');
//	Serial.print(d, HEX);
//	Serial.print(' ');
	wirewrite(d);
	chksum += d;
}

void PN532::send_ack() {
	wire.beginTransmission(i2c_addr);
	// clk is streched by PN532 to make a pause to resume from power-down
	send(PREAMBLE);
	send(STARTCODE_1);
	send(STARTCODE_2);
	send(0);
	send(0xff);
	wirewrite(POSTAMBLE);
	wire.endTransmission();
}

void PN532::send_nack() {
	wire.beginTransmission(i2c_addr);
	// clk is streched by PN532 to make a pause to resume from power-down
	send(PREAMBLE);
	send(STARTCODE_1);
	send(STARTCODE_2);
	send(0xff);
	send(0);
	wirewrite(POSTAMBLE);
	wire.endTransmission();
}

boolean PN532::checkACKframe(long timeout) {
	// Wait for chip to say its ready!
	if (!IRQ_wait(timeout))
		return false;

	// read acknowledgement
	byte frame_head[] = { PREAMBLE, STARTCODE_1, STARTCODE_2 };
	receivepacket(6);
	//printHexString(packet, 6);
	if ((0 == memcmp(packet, frame_head, 3)) && (packet[5] == 0)) {
		if (packet[3] == 0x00 && packet[4] == 0xff) {
			comm_status = ACK_FRAME_RECEIVED;
			return true; // ack'd command
		} else if (packet[3] == 0xff && packet[4] == 0x00) {
			comm_status = NACK_FRAME_RECEIVED;
			return true; // ack'd command
		}
	}
	comm_status = WRONG_ACK;
	return false;
}

inline byte PN532::receive() {
	byte d = wireread();
	chksum += d;
	return d;
}

byte PN532::receivepacket(int n) {
	byte * p = packet;
	chksum = 0;
	byte i;
	byte len;
	wire.requestFrom((int) i2c_addr, (int) n + 1);
	receive();
	for (i = 0; i < n; i++) {
		// delayMicroseconds(500);
		*p++ = receive();
	}
	// check sum
	if (chksum != 0) {
		comm_status = CHECKSUMERROR;
		return 0;
	}
	// preamble
	if (memcmp(packet, "\x00\x00\xff", 3) != 0
			|| ((packet[3] + packet[4]) & 0xff != 0)) {
#ifdef PN532DEBUG
		Serial.println("received illigale preamble.");
#endif
		comm_status = WRONG_PREAMBLE;
		return 0;
	}
	len = packet[3];
	// postamble
	if (packet[6 + len] != 0) {
#ifdef PN532DEBUG
		Serial.println("termination 0x00 error");
#endif
		return 0;
	}
	memmove(packet, packet + 5, len);
	return len;
}

void PN532::sendpacket(byte len) {
	byte * p = packet;
	chksum = 0;
	wire.beginTransmission(i2c_addr);
	// clk is streched by PN532 to make a pause to resume from power-down
	send(PREAMBLE);
	send(STARTCODE_1);
	send(STARTCODE_2);
	send(len + 1);
	send(~(len + 1) + 1); // LCS, 1-Packet Length Check Sum
	send(HOSTTOPN532); // D4
	for (; len > 0; len--) {
		send(*p++);
	}
	wirewrite(~chksum);
	wirewrite(POSTAMBLE);
	wire.endTransmission();
}

byte PN532::receivepacket() {
	chksum = 0;
	byte i;
	byte n = 0;

	wire.requestFrom((int) i2c_addr, BUFFER_LENGTH);
	receive();
	for (i = 0; i < 6; i++) {
		packet[i] = receive();
	}
	// preamble
	if (memcmp(packet, "\x00\x00\xff", 3) != 0
			|| ((packet[3] + packet[4]) & 0xff != 0)) {
#ifdef PN532DEBUG
		Serial.println("received illigale preamble.");
#endif
		comm_status = WRONG_PREAMBLE;
		return 0;
	}
	n = packet[3];
	if (((packet[3] + packet[4]) & 0xff) != 0x00) {
		comm_status = WRONG_PREAMBLE;
		return 0;
	}
//	wire.requestFrom((int) i2c_addr, (int) n);
	for (; i < n + 5 + 2; i++) {
		packet[i] = receive();
	}
#ifdef PN532DEBUG
	Serial.print("recieve packet Len = ");
	Serial.print(n, DEC);
	Serial.print(", ");
	printHexString(packet, n + 7);
	Serial.println();
	Serial.print("xsum: ");
	Serial.print(chksum, HEX);
	Serial.println();
#endif
	if (chksum != 0) {
		comm_status = CHECKSUMERROR;
#ifdef PN532DEBUG
		Serial.println("!!! Checksum error !!!");
		return 0;
#endif
	}
	if (packet[6 + n] != 0) {
#ifdef PN532DEBUG
		Serial.println("termination 0x00 error");
#endif
		comm_status = WRONG_POSTAMBLE;
		return 0;
	}
	memmove(packet, packet + 5, n);
	return n;
}

boolean PN532::IRQ_ready(void) {
	return (digitalRead(pin_irq) == HIGH) && (pin_irq != 0xff);
}

boolean PN532::IRQ_wait(long timeout) {
//	Serial.println(timeout);
	timeout += millis();
	// Wait for chip to say its ready!
	if (pin_irq == 0xff) {
		return false;
	}
	while (digitalRead(pin_irq) == HIGH) {
		if (timeout < millis()) {
			comm_status = I2CREADY_TIMEOUT;
			/*
			 Serial.print("timeout ");
			 Serial.print(timeout);
			 Serial.print(", millis ");
			 Serial.println(millis());
			 */
			return false;
		}
		delayMicroseconds(500);
	}
	comm_status = REQUEST_RECEIVE;
	return true;
}

boolean PN532::GetFirmwareVersion() {
	packet[0] = COMMAND_GetFirmwareVersion;
	sendpacket(1);
	last_command = COMMAND_GetFirmwareVersion;
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe()) {
#ifdef PN532DEBUG
		Serial.println("Failed to receive ACKframe");
#endif
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}

boolean PN532::GetGeneralStatus() {
	packet[0] = COMMAND_GetGeneralStatus;
	sendpacket(1);
	last_command = COMMAND_GetGeneralStatus;
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe()) {
#ifdef PN532DEBUG
		Serial.println("Failed to receive ACKframe");
#endif
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}

boolean PN532::SAMConfiguration(byte mode, byte timeout, byte use_irq) {
	packet[0] = COMMAND_SAMConfiguration;
	packet[1] = mode; //0x01; // normal mode;
	packet[2] = timeout; //0x14; // timeout 50ms * 20 = 1 second
	packet[3] = use_irq; //0x01; // use IRQ pin!

	sendpacket(4);
	last_command = COMMAND_SAMConfiguration;
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe()) {
#ifdef PN532DEBUG
		Serial.println("SAMConfiguration ACK missed.");
#endif
		comm_status = ACK_NOT_RECEIVED;
		return false;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return true;
}

boolean PN532::PowerDown(byte wkup, byte genirq) {
	byte len = 2;
	packet[0] = COMMAND_PowerDown;
	packet[1] = wkup & ~(1 << 2);
	if (genirq == 1) {
		packet[2] = genirq;
		len = 3;
	}
	sendpacket(len);
	last_command = COMMAND_PowerDown;
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe()) {
		comm_status = ACK_NOT_RECEIVED;
		return false;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return true;
}

byte PN532::InListPassiveTarget(const byte maxtg, const byte brty, byte * data,
		const byte length) {
//	byte inidatalen = 0;
	packet[0] = COMMAND_InListPassiveTarget;
	packet[1] = maxtg; // max 1 cards at once (we can set this to 2 later)
	packet[2] = brty;
	if (length > 0) {
		memcpy(packet + 3, data, length);
	}
#ifdef PN532DEBUG
	Serial.print("InListPassiveTarget << ");
	printHexString(packet, length + 3);
	Serial.println();
#endif
	sendpacket(3 + length);
	last_command = COMMAND_InListPassiveTarget;
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe()) {
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
#ifdef PN532DEBUG
	Serial.println("ACKed.");
#endif
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}

byte PN532::InAutoPoll(const byte pollnr, const byte period, const byte * types,
		const byte typeslen) {
	byte N = min(max(1, typeslen), 15);
	packet[0] = COMMAND_InAutoPoll;
	packet[1] = pollnr; // no. of polls
	packet[2] = period; // no. of 150 ms period
	// the first type N = 1 mandatory
	memcpy(packet + 3, types, N);
#ifdef PN532DEBUG
	Serial.println("InAutoPoll sending");
	printHexString(packet, typeslen + 3);
	Serial.println();
#endif
	last_command = COMMAND_InAutoPoll;
	sendpacket(3 + N);
	comm_status = COMMAND_ISSUED;
	if (!checkACKframe())
		return 0;
	comm_status = ACK_FRAME_RECEIVED;
#ifdef PN532DEBUG
	Serial.println("InAutoPoll ACKed.");
#endif
	return 1;
}

byte PN532::getCommandResponse(byte * resp, const long & wmillis) {
	if (!IRQ_wait(wmillis))
		return 0;

	comm_status = REQUEST_RECEIVE;
	byte count = receivepacket();
#ifdef PN532COMM
	Serial.print("com resp. >> ");
	printHexString(packet, count);
	Serial.println();
#endif
//#undef PN532DEBUG
	if (!(packet[0] == 0xd5 && packet[1] == (last_command + 1))) {
#ifdef PN532DEBUG
		Serial.println("Missmatch with the last command.");
#endif
		comm_status = RESP_COMMAND_MISSMATCH;
		return 0;
	}
	comm_status = RESP_RECEIVED;
	count -= 2;
	memcpy(resp, packet + 2, count);
	return count;
}

byte PN532::getAutoPollResponse(byte * respo) {
//	byte cnt;
	if (!getCommandResponse(respo))
		return 0;
	if (respo[0]) {
		switch (respo[1]) {
		case Type_FeliCa212kb:
			targetSet(respo[1], respo + 3 + 3, 8);
			break;
		case Type_Mifare:
			targetSet(respo[1], respo + 3 + 5, respo[3 + 4]);
			break;
		}
	} else {
		targetClear();
	}
	return respo[0];
}

/*
 byte PN532::felica_getDataExchangeResponse(const byte fcmd, byte * resp) {
 byte count = getCommandResponse(COMMAND_InDataExchange, resp);
 #ifdef FELICADEBUG
 Serial.print("FeliCa >> ");
 printHexString(resp, count);
 Serial.println();
 #endif
 lastStatus = resp[0];
 count = resp[1]-2;
 memcpy(resp, resp+3, count);
 return count;
 }
 */

byte PN532::InDataExchange(const byte Tg, const byte * data,
		const byte length) {

	// Prepare the authentication command //
	packet[0] = COMMAND_InDataExchange; /* Data Exchange Header */
	packet[1] = Tg; /* target number */
	memcpy(packet + 2, data, length);

#ifdef MIFAREDEBUG
	printHexString(packet, length + 2);
	Serial.println();
#endif
	sendpacket(length + 2);
	comm_status = COMMAND_ISSUED;
	last_command = COMMAND_InDataExchange;
	if (!(checkACKframe())) {
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}
/*
 byte PN532::InDataExchange(const byte Tg, const byte fcmd, const byte * data,
 const byte len) {
 // Prepare a Felica command //
 packet[0] = COMMAND_InDataExchange; // Data Exchange Header
 packet[1] = Tg; // target number
 packet[2] = len+2;
 packet[3] = fcmd;
 memcpy(packet + 4, data, len);

 #ifdef PN532COMM
 Serial.print(">> ");
 printHexString(packet, len + 4);
 Serial.println();
 #endif
 sendcc(packet, len + 4);
 if (!(checkACKframe())) {
 return 0;
 }
 return 1;
 }
 */

// InDataExchange for Mifare
byte PN532::InDataExchange(const byte Tg, const byte micmd, const byte blkaddr,
		const byte * data, const byte datalen) {
	// Prepare a Mifare command //
	packet[0] = COMMAND_InDataExchange; /* Data Exchange Header */
	packet[1] = Tg; /* target number */
	packet[2] = micmd;
	packet[3] = blkaddr;
	memcpy(packet + 4, data, datalen);

#ifdef MIFAREDEBUG
	Serial.print("Sending in InDataExchange: ");
	printHexString(packet, datalen + 4);
	Serial.println();
#endif
	sendpacket(datalen + 4);
	comm_status = COMMAND_ISSUED;
	last_command = COMMAND_InDataExchange;
	if (!(checkACKframe())) {
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}

void PN532::targetSet(const byte cardtype, const byte * uid, const byte uidLen) {
	target.NFCType = cardtype;
	target.IDLength = uidLen;
	memcpy(target.UID, uid, uidLen);
}

void PN532::targetClear() {
	target.NFCType = Type_Empty;
	target.IDLength = 0;
	target.UID[0] = 0;
}

byte PN532::mifare_AuthenticateBlock(word blkn, const byte * keyData) {
//	uint8_t len;
	byte tmp[16];

#ifdef MIFAREDEBUG
	Serial.println("In: mifare_AuthenticateBlock");
#endif

	// Prepare the authentication command //
	memcpy(tmp, keyData + 1, 6);
//	memcpy(tmp + 6, uid, max(4, uidLen));
	memcpy(tmp + 6, target.UID, max(4, target.IDLength));

	byte authcmd;
//	byte rescount;
	switch (keyData[0]) {
	case 0:
	case 0xaa:
	case 0x0a:
	case 'A':
	case 'a':
		authcmd = MIFARE_CMD_AUTH_A;
		break;
	case 1:
	case 0x0b:
	case 0xbb:
	case 'B':
	case 'b':
	default:
		authcmd = MIFARE_CMD_AUTH_B;
		break;
	}
	return InDataExchange(1, authcmd, blkn, tmp, target.IDLength + 6);
}

byte PN532::mifare_ReadDataBlock(uint8_t blockNumber, uint8_t * data) {
#ifdef MIFAREDEBUG
	Serial.print("Try to read 16 bytes from block ");
	Serial.println(blockNumber);
#endif

	// Access the Target 1 after polling by InList or AutoPoll
	// Authentication must be succeeded so the uid (and its length) is
	// stored in target.UID
	/* Send the command */
	if (!InDataExchange(1, MIFARE_CMD_READ, blockNumber, packet, 0)) {
#ifdef MIFAREDEBUG
		Serial.println("Failed to receive ACK for read command");
#endif
	}
	byte c = getCommandResponse(packet);
	if (!c ) {
#ifdef MIFAREDEBUG
		Serial.println("Unexpected response");
		printHexString(packet, 26);
		Serial.println();
#endif
		return 0;
	}
//#define MIFAREDEBUG
#ifdef MIFAREDEBUG
	Serial.print("Packet ");
	printHexString(packet, c);
	Serial.println();
#endif
	if (packet[0] != 0) {
		// error.
		return 0;
	}
	memcpy(data, packet + 1, 16);
#ifdef MIFAREDEBUG
	Serial.print("data ");
	Serial.println(blockNumber);
	printHexString(data, 16);
	Serial.println();
#endif
//#undef MIFAREDEBUG
	return 16;
}

/*
 byte PN532::felica_DataExchange(const byte cmd, const byte * data,
 const byte len) {
 // Prepare the command
 packet[0] = COMMAND_InDataExchange;
 packet[1] = 1; // Card number
 packet[2] = len + 2; //2+8; //length of data + 2
 packet[3] = cmd; // FeliCa card command = 0xa0
 memcpy(packet + 4, data, len);

 #ifdef FELICADEBUG
 Serial.print("<< ");
 printHexString(packet, len + 4);
 Serial.println();
 #endif

 // Send the command
 sendcc(packet, len + 4);
 if (!checkACKframe()) {
 return 0;
 }
 return 1;
 }
 */

boolean PN532::InCommunicateThru(const byte * data, const byte len) {
	packet[0] = COMMAND_InCommunicateThru;
	packet[1] = len + 1;
	memcpy(packet + 2, data, len);

#ifdef FELICADEBUG
	Serial.print("Thru: ");
	Serial.print("count = ");
	Serial.print(len + 1, DEC);
	Serial.print("  ");
	printHexString(packet, len + 2);
	Serial.println();
#endif

	/* Send the command */
	sendpacket(len + 2);
	comm_status = COMMAND_ISSUED;
	last_command = COMMAND_InCommunicateThru;
	if (!checkACKframe()) {
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}

byte PN532::felica_Polling(byte * resp, const word syscode) {
	// Polling command, with system code request.
	resp[0] = FELICA_CMD_POLLING;
	resp[1] = syscode & 0xff;
	resp[2] = syscode >> 8 & 0xff;
	resp[3] = 0x01; // request code: request sys code
	resp[4] = 0; // time slot #
	byte result = InCommunicateThru(resp, 5);
	result = getCommunicateThruResponse(resp);
//	printHexString(resp, result);
	if (resp[0] == FELICA_CMD_POLLING + 1) {
		target.IDLength = 8;
		memcpy(target.IDm, resp + 1, target.IDLength);
		target.NFCType = Type_FeliCa212kb;
		return result;
	}
	target.NFCType = Type_Empty;
	target.IDLength = 0;
	return 0;
}

byte PN532::getCommunicateThruResponse(byte * data) {
//	InCommunicateThru(data, len);
	/* Read the response packet */
	int count = getCommandResponse(packet);
	if (!count ) {
		return 0;
	}
#ifdef FELICADEBUG
	Serial.print("Response: ");
	Serial.print("count = ");
	Serial.print(count, DEC);
	Serial.print("   ");
	printHexString(packet, count);
	Serial.println();
#endif
	if (packet[0] != 0) {
		return 0;
	}
	count = packet[1] - 1;
	memcpy(data, packet + 2, count);
	return count;
}

byte PN532::felica_RequestService(byte * resp, const word servcodes[],
		const byte servnum) {
	resp[0] = FELICA_CMD_REQUESTSERVICE;
	memcpy(resp + 1, target.IDm, 8);
	resp[9] = servnum;
	for (int i = 0; i < servnum; i++) {
		resp[10 + 2 * i] = servcodes[i] & 0xff;
		resp[11 + 2 * i] = servcodes[i] >> 8 & 0xff;
	}
	byte count = InCommunicateThru(resp, 10 + 2 * servnum);
	count = getCommunicateThruResponse(resp);
	if (resp[0] == FELICA_CMD_REQUESTSERVICE + 1 && count >= 10) {
		byte svnum = resp[9];
		memmove(resp, resp + 10, svnum * 2);
		return svnum;
	}
	return 0;
}

word PN532::felica_RequestService(const word servcode) {
	byte tmp[14];
	if (!felica_RequestService(tmp, (word*) &servcode, 1)) {
		return 0xffff;
	}
	word servcodever = tmp[11];
	return (servcodever << 8) + tmp[10];
}

byte PN532::felica_RequestSystemCode(byte * resp) {
	resp[0] = FELICA_CMD_REQUESTSYSTEMCODE;
	memcpy(resp + 1, target.IDm, 8);
	InCommunicateThru(resp, 9);
	if (getCommunicateThruResponse(resp) == 0)
		return 0;
	byte n = resp[9];
	memmove(resp, resp + 10, n * 2);
	return n;
}

// Block list accepts only two byte codes.
byte PN532::felica_ReadWithoutEncryption(byte * resp, const word servcode,
		const byte blknum, const word blklist[]) {
	resp[0] = FELICA_CMD_READWITHOUTENCRYPTION;
	memcpy(resp + 1, target.IDm, 8);
	resp[9] = 1;
	resp[10] = servcode & 0xff;
	resp[11] = servcode >> 8 & 0xff;
	resp[12] = blknum;
	byte pos = 13;
	// only two byte array.
	for (int i = 0; i < blknum; i++) {
		// two bytes
		resp[pos++] = (blklist[i] | 0x8000) >> 8 & 0xff;
		resp[pos++] = blklist[i] & 0xff;
	}
	// pos has been incremented after the last substitution
	byte count = InCommunicateThru(resp, pos);
	count = getCommunicateThruResponse(resp);
	if (resp[9] == 0) {
		byte blocks = resp[11];
		memmove(resp, resp + 12, blocks * 16);
		return blocks;
	} else {
		return 0;
	}
}

byte PN532::felica_ReadBlocksWithoutEncryption(byte * resp, const word servcode,
		const byte blknum, const word blklist[]) {
	byte mess[40];
	for (int bno = 0; bno < blknum; bno++) {
		mess[0] = FELICA_CMD_READWITHOUTENCRYPTION;
		memcpy(mess + 1, target.IDm, 8);
		mess[9] = 1;
		mess[10] = servcode & 0xff;
		mess[11] = servcode >> 8 & 0xff;
		mess[12] = 1;
		// two bytes
		mess[13] = (blklist[bno] | 0x8000) >> 8 & 0xff;
		mess[14] = blklist[bno] & 0xff;

		// pos has been incremented after the last substitution
		byte count = InCommunicateThru(mess, 15);
		count = getCommunicateThruResponse(mess);
		if (mess[9] == 0) {
			byte blocks = mess[11];
			memcpy(resp + (16 * bno), mess + 12, blocks * 16);
		} else {
			return 0;
		}
	}
	return blknum;
}

boolean PN532::WriteRegister(word addr, byte val) {
	packet[0] = COMMAND_WriteRegister; /* Data Exchange Header */
	packet[1] = highByte(addr);
	packet[2] = lowByte(addr);
	packet[3] = val;

	sendpacket(4);
	comm_status = COMMAND_ISSUED;
	last_command = COMMAND_WriteRegister;
	if (!(checkACKframe())) {
		comm_status = ACK_NOT_RECEIVED;
		return 0;
	}
	comm_status = ACK_FRAME_RECEIVED;
	return 1;
}
