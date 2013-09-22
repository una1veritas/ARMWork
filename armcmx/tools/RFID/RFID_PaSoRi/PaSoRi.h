#ifndef _PaSoRi_H_
#define _PaSoRi_H_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif
//#include <Spi.h>
#include <max3421e.h>
#include <Usb_HSv10.h>

//#define DEBUG

#define PASORI_VID_HI 0x05
#define PASORI_VID_LO 0x4c

#define PASORI_PID_S320_HI 0x01
#define PASORI_PID_S320_LO 0xbb
#define PASORI_PID_S330_HI 0x02
#define PASORI_PID_S330_LO 0xe1

#define POLLING_ANY    0xffff
#define POLLING_SUICA  0x0003
#define POLLING_EDY    0xfe00
#define POLLING_FCF    0xfe00

#define FELICA_CMD_POLLING 0
#define FELICA_ANS_POLLING 1
#define FELICA_CMD_REQUEST_RESPONSE 4
#define FELICA_ANS_REQUEST_RESPONSE 5
#define FELICA_CMD_READ_WITHOUT_ENCRYPTION 6
#define FELICA_ANS_READ_WITHOUT_ENCRYPTION 7
#define FELICA_CMD_WRITE_WITHOUT_ENCRYPTION 8
#define FELICA_ANS_WRITE_WITHOUT_ENCRYPTION 9
#define FELICA_CMD_PUSH 0xb0
#define FELICA_ANS_PUSH 0xb1

#define PASORI2_CMD_SEND_PACKET 0x5c
#define PASORI2_ANS_SEND_PACKET 0x5d

#define PASORI3_CMD_SEND_PACKET 0xd4
#define PASORI3_ANS_SEND_PACKET 0xd5

#define PASORI3_CMD_SEND_PACKET2 0x42
#define PASORI3_CMD_LIST_PASSIVE 0x4a

class PaSoRi {
	MAX3421E Max;
	USB Usb;
	enum {
		PASORI_S320, PASORI_S330
	} mode;
	EP_RECORD ep_record[5]; //endpoint record structure for PaSoRi

	byte init();

public:
	byte idm[8];
	byte pmm[8];

public:
	byte begin();
	void task();

	byte poll(unsigned short syscode = POLLING_ANY, byte rfu = 0,
			byte timeslot = 0);
	byte listPassiveTarget(byte brtype, byte * data, byte len);
/*
	const byte* getIDm() {
		return idm;
	}
	const byte* getPMm() {
		return pmm;
	}
	*/
	int read_without_encryption02(int servicecode, byte addr, byte b[16]);
	byte mobile_felica_push_url(int len, const char *url);

	// low level transmission
	byte send_packet(int len, const byte *data);
	byte send(int len, byte *buf);
	byte recv(int len, byte *buf);

	//
	byte sendInDataExchange(byte data[], byte length) {
		byte buf[32];
		int hdr;// = mode == PASORI_S320 ? 2 : 4;
		if (mode == PASORI_S320) {
			hdr = 2;
			buf[0] = PASORI2_CMD_SEND_PACKET;
			buf[1] = 16; // size
		} else {
			buf[0] = PASORI3_CMD_SEND_PACKET;
			buf[1] = 0xa0;
			buf[2] = 0xd0;
			buf[3] = 0x07;
			hdr = 4;
		}
		//
		memcpy(&buf[4], data, length);

		Serial.println("sendInDataExchange send");
		for(int i = 0; i < hdr+length; i++) {
			Serial.print(buf[i]>>4, HEX);
			Serial.print(buf[i]&0x0f, HEX);
			Serial.print(' ');
		}
		Serial.println();

		byte rcode = send_packet(hdr + length, buf);
		if (rcode == 0) {
			hdr = mode == PASORI_S320 ? 2 : 5;
			rcode = recv(sizeof(buf), buf);
#ifdef DEBUG
			Serial.println("sendInDataExchange resp");
			for(int i = 0; i < rcode; i++) {
				Serial.print(buf[i]>>4, HEX);
				Serial.print(buf[i]&0x0f, HEX);
				Serial.print(' ');
			}
			Serial.println();
#endif
			if (rcode == 0) {
				byte size = buf[3];
				if (mode == PASORI_S320 && size < 16)
					return 0xff;
				if (mode == PASORI_S330 && size == 1)
					return 0xff;
				if (mode == PASORI_S330 && buf[5] != PASORI3_ANS_SEND_PACKET)
					return 0xfe;
				if (buf[hdr + 5] != PASORI3_CMD_SEND_PACKET2)
					return 0xfd;
				memcpy(data, buf, rcode);
				//memcpy(idm, buf + hdr + 6, 8);
				//memcpy(pmm, buf + hdr + 14, 8);
			}
		}
		return rcode;
	}

};

#endif
