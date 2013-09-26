#include <PaSoRi.h>

#define DEVADDR 1
#define CONFVALUE 1

/* Initialize device */
static const byte PASORI_INIT0[] = { 0x62, 0x01, 0x82 };
static const byte PASORI_INIT1[] = { 0x62, 0x02, 0x80, 0x81 }; /* INIT3 */
static const byte PASORI_INIT2[] = { 0x62, 0x22, 0x80, 0xcc, 0x81, 0x88 };
static const byte PASORI_INIT3[] = { 0x62, 0x02, 0x80, 0x81 }; /* INIT1 */
static const byte PASORI_INIT4[] = { 0x62, 0x02, 0x82, 0x87 };
static const byte PASORI_INIT5[] = { 0x62, 0x21, 0x25, 0x58 };
static const byte PASORI_READ2[] = { 0x5a, 0x80 };

static const byte PASORI_RF_ANTENNA_ON[] = { 0xD4, 0x32, 0x01, 0x01 };

byte PaSoRi::begin() {
	Max.powerOn();
	delay(500);
	return init();
}

byte PaSoRi::init() {
	do {
		task();
	} while (Usb.getUsbTaskState() != USB_STATE_CONFIGURING);

	byte rcode; //return code
	ep_record[0] = *(Usb.getDevTableEntry(0, 0)); //copy endpoint 0 parameters
	// PASORI RC-S320
	ep_record[1].epAddr = 0x01;
	ep_record[1].Attr = USB_TRANSFER_TYPE_INTERRUPT;
	ep_record[1].MaxPktSize = 0x08;
	ep_record[1].Interval = 0x01;
	ep_record[1].sndToggle = bmSNDTOG0;
	ep_record[1].rcvToggle = bmRCVTOG0;
	// PASORI RC-S330
	ep_record[4].epAddr = 0x04;
	ep_record[4].Attr = USB_TRANSFER_TYPE_BULK;
	ep_record[4].MaxPktSize = 0x40;
	ep_record[4].Interval = 0x00;
	ep_record[4].sndToggle = bmSNDTOG0;
	ep_record[4].rcvToggle = bmRCVTOG0;

	Usb.setDevTableEntry(DEVADDR, ep_record);

	/* read the device descriptor and check VID and PID*/
	byte buf[DEV_DESCR_LEN];
	rcode = Usb.getDevDescr(DEVADDR, 0, DEV_DESCR_LEN, (char*) buf);
	if (rcode)
		return rcode;
#ifdef DEBUG
	Serial.print(buf[9], HEX);
	Serial.println(buf[8], HEX);
	Serial.print(buf[11], HEX);
	Serial.println(buf[10], HEX);
#endif
	if (buf[8] != PASORI_VID_LO || buf[9] != PASORI_VID_HI)
		return 0xff;
	if (buf[10] == PASORI_PID_S320_LO && buf[11] == PASORI_PID_S320_HI)
		mode = PASORI_S320;
	else if (buf[10] == PASORI_PID_S330_LO && buf[11] == PASORI_PID_S330_HI)
		mode = PASORI_S330;
	else
		return 0xff;
#ifdef DEBUG
	Serial.println(mode == PASORI_S320 ? "RC-S320 detected" : "RC-S330 detected");
#endif

	/* Configure device */
	rcode = Usb.setConf(DEVADDR, 0, CONFVALUE);
	if (rcode)
		return rcode;
	Usb.setUsbTaskState(USB_STATE_RUNNING);

	if (mode == PASORI_S320) {
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT0), PASORI_INIT0);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT1), PASORI_INIT1);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT2), PASORI_INIT2);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT3), PASORI_INIT3);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT4), PASORI_INIT4);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_INIT5), PASORI_INIT5);
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_READ2), PASORI_READ2);
	} else if (mode == PASORI_S330) {
		if (!rcode)
			rcode = send_packet(sizeof(PASORI_RF_ANTENNA_ON),
					PASORI_RF_ANTENNA_ON);
	}
	return rcode;
}

void PaSoRi::task() {
	Max.Task();
	Usb.Task();
}

byte PaSoRi::poll(unsigned short syscode, byte rfu, byte timeslot) {
	byte buf[64];
	int hdr = mode == PASORI_S320 ? 2 : 4;
	if (mode == PASORI_S320) {
		buf[0] = PASORI2_CMD_SEND_PACKET;
		buf[1] = 0x06;
	} else {
		buf[0] = PASORI3_CMD_SEND_PACKET;
		buf[1] = PASORI3_CMD_LIST_PASSIVE;
		buf[2] = 1;
		buf[3] = 1;
	}
	buf[hdr + 0] = FELICA_CMD_POLLING;
	buf[hdr + 1] = syscode >> 8;
	buf[hdr + 2] = syscode & 0xff;
	buf[hdr + 3] = rfu;
	buf[hdr + 4] = timeslot;

	byte rcode = send_packet(hdr + 5, buf);
	if (rcode == 0) {
		hdr = mode == PASORI_S320 ? 2 : 5;
		rcode = recv(sizeof(buf), buf);
		if (rcode == 0) {
			byte size = buf[3];
			if (mode == PASORI_S320 && size < 16)
				return 0xff;
			if (mode == PASORI_S330 && size == 1)
				return 0xff;
			if (mode == PASORI_S330 && buf[5] != PASORI3_ANS_SEND_PACKET)
				return 0xfe;
			if (buf[hdr + 5] != FELICA_ANS_POLLING)
				return 0xfd;
			memcpy(idm, buf + hdr + 6, 8);
			memcpy(pmm, buf + hdr + 14, 8);
		}
	}
	return rcode;
}


byte PaSoRi::listPassiveTarget(byte brtype, byte * payload, byte length) {
	//http://hiro99ma.blogspot.jp/2011/05/rc-s620smifare.html
	byte buf[64];
	int hdr = mode == PASORI_S320 ? 2 : 4;
	if (mode == PASORI_S320) {
		buf[0] = PASORI2_CMD_SEND_PACKET;
		buf[1] = 0x06;
	} else {
		buf[0] = PASORI3_CMD_SEND_PACKET;
		buf[1] = PASORI3_CMD_LIST_PASSIVE;
		buf[2] = 1;
		buf[3] = brtype;
	}
	memcpy(buf+hdr, payload, length);

	byte rcode = send_packet(hdr + length, buf);
#ifdef DEBUG
	Serial.print("hdr = ");
	Serial.println(hdr, HEX);
	Serial.print("send packet rcode = ");
	Serial.println(rcode, HEX);
#endif
	if (rcode == 0) {
		hdr = mode == PASORI_S320 ? 2 : 5;
		rcode = recv(sizeof(buf), buf);
#ifdef DEBUG
		for(int i = 0; i < 16; i++) {
			Serial.print(buf[i], HEX);
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

			const int pre = 5;
			if (buf[pre+1] != PASORI3_CMD_LIST_PASSIVE+1)
				return 0xfd;
			switch(brtype) {
				case 0x00:
					memcpy(idm, buf+pre+6, buf[pre+6]+1);
					break;
				case 0x01:
					memcpy(idm, buf + hdr + 6, 8);
					memcpy(pmm, buf + hdr + 14, 8);
					break;
			}
		}
	}
	return rcode;
}


int PaSoRi::read_without_encryption02(int servicecode, byte addr, byte b[16]) {
	byte buf[64];
	int i, hdr;

	if (mode == PASORI_S320) {
		hdr = 2;
		buf[0] = PASORI2_CMD_SEND_PACKET;
		buf[1] = 16; // size
	} else {
		hdr = 3;
		buf[0] = PASORI3_CMD_SEND_PACKET;
		buf[1] = PASORI3_CMD_SEND_PACKET2;
		buf[2] = 16; // size
	}
	buf[hdr + 0] = FELICA_CMD_READ_WITHOUT_ENCRYPTION;
	memcpy(&buf[hdr + 1], idm, 8);
	buf[hdr + 9] = 1;
	buf[hdr + 10] = servicecode & 0xff;
	buf[hdr + 11] = servicecode >> 8;
	buf[hdr + 12] = 1;
	buf[hdr + 13] = 0x80; // blacklist[0], element size = 2
	buf[hdr + 14] = addr; // blacklist[1]

	byte rcode = send_packet(hdr + 15, buf);
	if (rcode)
		return rcode;
	rcode = recv(sizeof(buf), buf);
	if (rcode)
		return rcode;
	if (mode == PASORI_S320 && buf[5] != PASORI2_ANS_SEND_PACKET)
		return 0xfe;
	if (mode == PASORI_S330 && buf[5] != PASORI3_ANS_SEND_PACKET)
		return 0xfe;
	hdr = mode == PASORI_S320 ? 2 : 4;
	if (buf[hdr + 5] != FELICA_ANS_READ_WITHOUT_ENCRYPTION)
		return 0xfd;
	if (buf[hdr + 14] != 0)
		return buf[hdr + 14] * 256 + buf[hdr + 15]; // error code
	memcpy(b, &buf[hdr + 17], 16);
	return 0;
}

byte PaSoRi::mobile_felica_push_url(int len, const char *url) {
	byte buf[64];
	int i, hdr;

	if (mode == PASORI_S320) {
		hdr = 2;
		buf[0] = PASORI2_CMD_SEND_PACKET;
		buf[1] = len + 19; // size
	} else {
		hdr = 3;
		buf[0] = PASORI3_CMD_SEND_PACKET;
		buf[1] = PASORI3_CMD_SEND_PACKET2;
		buf[2] = len + 19; // size
	}
	buf[hdr + 0] = FELICA_CMD_PUSH;
	memcpy(&buf[hdr + 1], idm, 8);
	buf[hdr + 9] = len + 8;
	buf[hdr + 10] = 0x01; // nr_params
	buf[hdr + 11] = 0x02; // launch browser
	buf[hdr + 12] = len + 2; // param size (LO)
	buf[hdr + 13] = 0x00; // param size (HI)
	buf[hdr + 14] = len; // URL size (LO)
	buf[hdr + 15] = 0x00; // URL size (LO)
	memcpy(&buf[hdr + 16], url, len);
	unsigned short sum = 0;
	for (i = hdr + 10; i < hdr + len + 16; i++)
		sum += buf[i];
	sum = 0x10000 - sum;
	buf[hdr + len + 16] = sum >> 8;
	buf[hdr + len + 17] = sum & 0xff;

	byte rcode = send_packet(hdr + len + 18, buf);
	if (rcode)
		return rcode;
	rcode = recv(sizeof(buf), buf);
	if (rcode)
		return rcode;
	if (mode == PASORI_S320 && buf[5] != PASORI2_ANS_SEND_PACKET)
		return 0xfe;
	if (mode == PASORI_S330 && buf[5] != PASORI3_ANS_SEND_PACKET)
		return 0xfe;
	hdr = mode == PASORI_S320 ? 2 : 4;
	if (buf[hdr + 5] != FELICA_ANS_PUSH)
		return 0xfd;
	return rcode;
}

byte PaSoRi::send_packet(int len, const byte *data) {
	byte buf[128];
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0xff;
	buf[3] = len;
	buf[4] = 0x100 - len;
	memcpy(&buf[5], data, len);
	byte sum = 0;
	for (int i = 0; i < len; i++)
		sum += data[i];
	buf[len + 5] = 0x100 - sum;
	byte rcode = send(len + 7, buf);
	if (rcode)
		return rcode;

	rcode = recv(sizeof(buf), buf); // recv ack
	return rcode;
}

// low level transmission

byte PaSoRi::send(int len, byte *buf) {
	byte rcode;
	if (mode == PASORI_S320)
		rcode = Usb.ctrlReq(DEVADDR, 0,
				USB_SETUP_HOST_TO_DEVICE | USB_SETUP_TYPE_VENDOR, 0, 0, 0, 0,
				len, (char*) buf);
	else
		rcode = Usb.outTransfer(DEVADDR, 4, len, (char*) buf, 2000);

#ifdef DEBUG
	if (rcode) {
		Serial.print("send failed: ret = ");
		Serial.println(rcode, HEX);
	}
	Serial.print(" << ");
	for (int i = 0; i < len; i++) {
		Serial.print((byte)buf[i], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
#endif
	return rcode;
}

byte PaSoRi::recv(int len, byte *buf) {
	byte rcode;
	if (mode == PASORI_S320)
		byte rcode = Usb.inTransfer(DEVADDR, 1, len, (char*) buf, 2000);
	else
		byte rcode = Usb.inTransfer(DEVADDR, 4, len, (char*) buf, 2000);
#ifdef DEBUG
	if (rcode) {
		Serial.print("recv failed: ret = ");
		Serial.println(rcode, HEX);
	}
	Serial.print(" >> ");
	for (int i = 0; i < 32; i++) {
		Serial.print((byte)buf[i], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
#endif
	return rcode;
}
