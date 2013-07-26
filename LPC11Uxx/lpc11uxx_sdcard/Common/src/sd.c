/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : sd.c                                                             *
* Revision : Initial developement                                             *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card.                                *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

/*****************************************************************************/
#include "sd.h"
/*****************************************************************************/
esint8 sd_Init()
{
	esint16 i;
	euint8 resp;

	/* Try to send reset command up to 100 times */
	i=100;
	do{
		sd_Command(0, 0, 0);
		resp=sd_Resp8b();
	}
	while(resp!=1 && i--);

	if(resp!=1){
		if(resp==0xff){
			return(-1);
		}
		else{
			sd_Resp8bError(resp);
			return(-2);
		}
	}

	/* Wait till card is ready initialising (returns 0 on CMD1) */
	/* Try up to 32000 times. */
	i=32000;
	do{
		sd_Command(1, 0, 0);

		resp=sd_Resp8b();
		if(resp!=0)
			sd_Resp8bError(resp);
	}
	while(resp==1 && i--);

	if(resp!=0){
		sd_Resp8bError(resp);
		return(-3);
	}

	return(0);
}
esint8 sd_State()
{
	eint16 value;

	sd_Command(13, 0, 0);
	value=sd_Resp16b();

	switch(value)
	{
		case 0x000:
			return(1);
//			break;
		case 0x0001:
//			DBG((TXT("Card is Locked.\n")));
			break;
		case 0x0002:
//			DBG((TXT("WP Erase Skip, Lock/Unlock Cmd Failed.\n")));
			break;
		case 0x0004:
//			DBG((TXT("General / Unknown error -- card broken?.\n")));
			break;
		case 0x0008:
//			DBG((TXT("Internal card controller error.\n")));
			break;
		case 0x0010:
//			DBG((TXT("Card internal ECC was applied, but failed to correct the data.\n")));
			break;
		case 0x0020:
//			DBG((TXT("Write protect violation.\n")));
			break;
		case 0x0040:
//			DBG((TXT("An invalid selection, sectors for erase.\n")));
			break;
		case 0x0080:
//			DBG((TXT("Out of Range, CSD_Overwrite.\n")));
			break;
		default:
			if(value>0x00FF)
				sd_Resp8bError((euint8) (value>>8));
			else
//				DBG((TXT("Unknown error: 0x%x (see SanDisk docs p5-14).\n"),value));
			break;
	}
	return(-1);
}


/*****************************************************************************/

void sd_Command(euint8 cmd, euint16 paramx, euint16 paramy)
{
	if_spiSend(0xff);

	if_spiSend(0x40 | cmd);
	if_spiSend((euint8) (paramx >> 8)); /* MSB of parameter x */
	if_spiSend((euint8) (paramx)); /* LSB of parameter x */
	if_spiSend((euint8) (paramy >> 8)); /* MSB of parameter y */
	if_spiSend((euint8) (paramy)); /* LSB of parameter y */

	if_spiSend(0x95); /* Checksum (should be only valid for first command (0) */

	if_spiSend(0xff); /* eat empty command - response */
}
/*****************************************************************************/

euint8 sd_Resp8b()
{
	euint8 i;
	euint8 resp;

	/* Respone will come after 1 - 8 pings */
	for(i=0;i<8;i++){
		resp = if_spiSend(0xff);
		if(resp != 0xff)
			return(resp);
	}

	return(resp);
}
/*****************************************************************************/

euint16 sd_Resp16b()
{
	euint16 resp;

	resp = ( sd_Resp8b() << 8 ) & 0xff00;
	resp |= if_spiSend(0xff);

	return(resp);
}
/*****************************************************************************/


void sd_Resp8bError(euint8 value)
{
	switch(value)
	{
		case 0x40:
//			DBG((TXT("Argument out of bounds.\n")));
			break;
		case 0x20:
//			DBG((TXT("Address out of bounds.\n")));
			break;
		case 0x10:
//			DBG((TXT("Error during erase sequence.\n")));
			break;
		case 0x08:
//			DBG((TXT("CRC failed.\n")));
			break;
		case 0x04:
//			DBG((TXT("Illegal command.\n")));
			break;
		case 0x02:
//			DBG((TXT("Erase reset (see SanDisk docs p5-13).\n")));
			break;
		case 0x01:
//			DBG((TXT("Card is initialising.\n")));
			break;
		default:
//			DBG((TXT("Unknown error 0x%x (see SanDisk docs p5-13).\n"),value));
			break;
	}
}
/*****************************************************************************/

