/*
 * $Header: /cvs/tuxbox/apps/tuxbox/neutrino/daemons/sectionsd/dmxapi.cpp,v 1.5 2005/01/13 10:48:02 diemade Exp $
 *
 * DMX low level functions (sectionsd) - d-box2 linux project
 *
 * (C) 2003 by thegoodguy <thegoodguy@berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#include <stdio.h>         /* perror      */
#include <string.h>        /* memset      */
#include <sys/ioctl.h>     /* ioctl       */
#include <fcntl.h>         /* open        */
#include <unistd.h>        /* close, read */
#include <dmxapi.h>

#if HAVE_TRIPLEDRAGON
#include <dmx_td.h>
#else
#include <dmx_cs.h>
#endif

#ifndef DO_NOT_INCLUDE_STUFF_NOT_NEEDED_FOR_SECTIONSD
bool setfilter(const int fd, const uint16_t pid, const uint8_t filter, const uint8_t mask, const uint32_t  flags)
{
	struct dmx_sct_filter_params flt;

	memset(&flt, 0, sizeof(struct dmx_sct_filter_params));

	flt.pid              = pid;
	flt.filter.filter[0] = filter;
	flt.filter.mask  [0] = mask;
	flt.timeout          = 0;
	flt.flags            = flags;

	if (::ioctl(fd, DMX_SET_FILTER, &flt) == -1)
	{
		perror("[sectionsd] DMX: DMX_SET_FILTER");
		return false;
	}
	return true;
}
#endif

struct SI_section_TOT_header
{
	unsigned char      table_id                 :  8;
	unsigned char      section_syntax_indicator :  1;
	unsigned char      reserved_future_use      :  1;
	unsigned char      reserved1                :  2;
	unsigned short     section_length           : 12;
	uint64_t UTC_time                 : 40;
	unsigned char      reserved2                :  4;
	unsigned short     descriptors_loop_length  : 12;
}
__attribute__ ((packed)); /* 10 bytes */

struct SI_section_TDT_header
{
	unsigned char      table_id                 :  8;
	unsigned char      section_syntax_indicator :  1;
	unsigned char      reserved_future_use      :  1;
	unsigned char      reserved1                :  2;
	unsigned short     section_length           : 12;
/*	uint64_t UTC_time                 : 40;*/
	UTC_t              UTC_time;
}
__attribute__ ((packed)); /* 8 bytes */

cDemux * dmxUTC;
bool getUTC(UTC_t * const UTC, const bool TDT)
{
	unsigned char filter[DMX_FILTER_SIZE];
	unsigned char mask[DMX_FILTER_SIZE];
	int timeout;
	struct SI_section_TDT_header tdt_tot_header;
	char cUTC[5];
	bool ret = true;

	if(dmxUTC == NULL) {
		dmxUTC = new cDemux();
		dmxUTC->Open(DMX_PSI_CHANNEL);
	}

	memset(&filter, 0, DMX_FILTER_SIZE);
	memset(&mask,   0, DMX_FILTER_SIZE);

	filter[0] = TDT ? 0x70 : 0x73;
	mask  [0] = 0xFF;
	timeout   = 31000;
//	flags     = TDT ? (DMX_ONESHOT | DMX_IMMEDIATE_START) : (DMX_ONESHOT | DMX_CHECK_CRC | DMX_IMMEDIATE_START);

	dmxUTC->sectionFilter(0x0014, filter, mask, 5, timeout);

	if(dmxUTC->Read((unsigned char *) &tdt_tot_header, sizeof(tdt_tot_header)) != sizeof(tdt_tot_header)) {
		perror("[sectionsd] getUTC: read");
		ret = false;
	} else {
		memcpy(cUTC, &tdt_tot_header.UTC_time, 5);
		if ((cUTC[2] > 0x23) || (cUTC[3] > 0x59) || (cUTC[4] > 0x59)) // no valid time
		{
			printf("[sectionsd] getUTC: invalid %s section received: %02x %02x %02x %02x %02x\n",
					TDT ? "TDT" : "TOT", cUTC[0], cUTC[1], cUTC[2], cUTC[3], cUTC[4]);
			ret = false;
		}

		(*UTC) = tdt_tot_header.UTC_time;
	}
	//delete dmxUTC;
	dmxUTC->Stop();

	return ret;
}
