/*
	Neutrino-GUI  -   DBoxII-Project

	copyright 2010 Carsten Juttner <cajay@gmx.net>

	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdint.h>
#include <linux/dvb/dmx.h>
#include "dmx_dvbapi.h"
#include <zapit/settings.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

cDemux *videoDemux = 0;
cDemux *audioDemux = 0;

cDemux::cDemux(int num) : mLen(0), mPesType(DMX_VIDEO_CHANNEL)
{ /* don't do anything here, we may need multiple handles internally */
	std::cout << "Demux::" << __FUNCTION__ << boost::format("num:%d ") % num << std::endl;
}


cDemux::~cDemux()
{
	std::cout << "cDemux::" << __FUNCTION__ << " " << std::endl;
}


bool cDemux::Open(DMX_CHANNEL_TYPE pes_type, void * hVideoBuffer, int uBufferSize)
{
	std::cout << "cDemux::" << __FUNCTION__ << boost::format(" pes_type %d hVideoBuffer %p uBuffersize %d") % pes_type % hVideoBuffer % uBufferSize << std::endl;
	mPesType = pes_type;
	switch(mPesType) 
	{
	case DMX_PES_VIDEO:
		std::cout << "video demux" << std::endl;
		break;
	case DMX_PES_AUDIO:
		std::cout << "audio demux" << std::endl;
		break;
	default:
		std::cout << "other demux" << std::endl;
		break;
	}
	return true;
}


void cDemux::Close(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
}


bool cDemux::Start(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	std::vector<int>::iterator it;
	bool success = true;
	for(it = mHandleList.begin(); it != mHandleList.end(); ++it)
	{
		int res = ioctl(*it, DMX_START);
		if(res < 0)
		{
			std::cout << boost::format("cDemux::Start error setting DMX_START: '%s'") % strerror(errno) << std::endl;
			success = false;
		}
	}
	return success;
}


bool cDemux::Stop(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	std::vector<int>::iterator it;
	bool success = true;
	for(it = mHandleList.begin(); it != mHandleList.end(); ++it)
	{
		int res = ioctl(*it, DMX_STOP);
		if(res < 0)
		{
			std::cout << boost::format("cDemux::Start error setting DMX_START: '%s'") % strerror(errno) << std::endl;
			success = false;
		}
	}
	return success;
}


int cDemux::Read(unsigned char *buff, int len, int Timeout)
{
	//std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	int res = 0;
	struct pollfd pfd;
	memset(&pfd, 0, sizeof(struct pollfd));
	pfd.fd = mHandleList[0]; /* user will always read from the first handle */
	pfd.events = POLLIN | POLLPRI;
		
	res = poll(&pfd, 1, Timeout);
	if(res > 0)
	{
		if(pfd.revents & (POLLIN | POLLPRI))
		{
			res = read(mHandleList[0], buff, len);
			if(res < 0)
			{
				std::cout << boost::format("cDemux::Read error reading. '%s'") % strerror(errno) << std::endl;
			}
			else
			{
				std::cout << boost::format("cDemux::Read len %d == %d") % len % res << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format("cDemux::Read POLLERR. '%s'") % strerror(errno) << std::endl;
			return 0;
		}
	}
	else
	{ /* timeout or error */
		if(res < 0 )
		{ /* do not spam if no timeout was requested */
			std::cerr << boost::format("cDemux::Read Error waiting for data. '%s'") % strerror(errno) << std::endl;
		}
	}

	return res;
}


void cDemux::SignalRead(int len)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
}


unsigned short cDemux::GetPID(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return 0;
}


const unsigned char *cDemux::GetFilterTID(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return 0;
}


const unsigned char *cDemux::GetFilterMask(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return 0;
}


bool cDemux::sectionFilter(unsigned short Pid, const unsigned char * const Tid, const unsigned char * const Mask, int len, int Timeout, const unsigned char * const nMask)
{
	int res = 0;
	int sechandle = 0;
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	struct dmx_sct_filter_params sctFilterParams;

	Stop();
	closeAll(); /* close and remove all existing handles */
	bool success = openReal(sechandle);
	if(!success)
	{
		std::cerr << boost::format("cDemux::sectionFilter error setting parameters for pid %04x:'%s'") % Pid % strerror(errno) << std::endl;
	}

	if(success)
	{
		std::stringstream val, msk;
		std::cout << boost::format("cDemux::sectionFilter: pid 0x%04x (%d bytes)") % Pid % len << std::endl;
		for(int i = 0; i < len; ++i)
		{
			val << boost::format("%02x ") % (unsigned int)Tid[i];
			msk << boost::format("%02x ") % (unsigned int)Mask[i];
		}
		std::cout << "  val:" << val.str() << std::endl;
		std::cout << "  msk:" << msk.str() << std::endl;
		
		mLen = len;
		memset(&sctFilterParams, 0, sizeof(struct dmx_sct_filter_params));
		memcpy(&sctFilterParams.filter.filter, Tid, len);
		memcpy(&sctFilterParams.filter.mask, Mask, len);
		sctFilterParams.pid = Pid;
		sctFilterParams.flags = DMX_ONESHOT | DMX_CHECK_CRC | DMX_IMMEDIATE_START;

		switch (Tid[0]) {
		case 0x00: /* program_association_section */
			sctFilterParams.timeout = 2000;
			break;

		case 0x01: /* conditional_access_section */
			sctFilterParams.timeout = 6000;
			break;

		case 0x02: /* program_map_section */
			sctFilterParams.timeout = 1500;
			break;

		case 0x03: /* transport_stream_description_section */
			sctFilterParams.timeout = 10000;
			break;

		/* 0x04 - 0x3F: reserved */

		case 0x40: /* network_information_section - actual_network */
			sctFilterParams.timeout = 10000;
			break;

		case 0x41: /* network_information_section - other_network */
			sctFilterParams.timeout = 15000;
			break;

		case 0x42: /* service_description_section - actual_transport_stream */
			sctFilterParams.timeout = 10000;
			break;

		/* 0x43 - 0x45: reserved for future use */

		case 0x46: /* service_description_section - other_transport_stream */
			sctFilterParams.timeout = 10000;
			break;

		/* 0x47 - 0x49: reserved for future use */

		case 0x4A: /* bouquet_association_section */
			sctFilterParams.timeout = 11000;
			break;

		/* 0x4B - 0x4D: reserved for future use */

		case 0x4E: /* event_information_section - actual_transport_stream, present/following */
			sctFilterParams.timeout = 2000;
			break;

		case 0x4F: /* event_information_section - other_transport_stream, present/following */
			sctFilterParams.timeout = 10000;
			break;

		/* 0x50 - 0x5F: event_information_section - actual_transport_stream, schedule */
		/* 0x60 - 0x6F: event_information_section - other_transport_stream, schedule */

		case 0x70: /* time_date_section */
			sctFilterParams.flags  &= (~DMX_CHECK_CRC); /* section has no CRC */
			sctFilterParams.pid     = 0x0014;
			sctFilterParams.timeout = 30000;
			break;

		case 0x71: /* running_status_section */
			sctFilterParams.flags  &= (~DMX_CHECK_CRC); /* section has no CRC */
			sctFilterParams.timeout = 0;
			break;

		case 0x72: /* stuffing_section */
			sctFilterParams.flags  &= (~DMX_CHECK_CRC); /* section has no CRC */
			sctFilterParams.timeout = 0;
			break;

		case 0x73: /* time_offset_section */
			sctFilterParams.pid     = 0x0014;
			sctFilterParams.timeout = 30000;
			break;

		/* 0x74 - 0x7D: reserved for future use */

		case 0x7E: /* discontinuity_information_section */
			sctFilterParams.flags  &= (~DMX_CHECK_CRC); /* section has no CRC */
			sctFilterParams.timeout = 0;
			break;

		case 0x7F: /* selection_information_section */
			sctFilterParams.timeout = 0;
			break;

		/* 0x80 - 0x8F: ca_message_section */
		/* 0x90 - 0xFE: user defined */
		/*        0xFF: reserved */
		default:
			return -1;
		}
	}
	if(success)
	{
		res = ioctl(sechandle, DMX_SET_FILTER, &sctFilterParams);
		if (res < 0)
		{
			std::cerr << boost::format("cDemux::sectionFilter error setting parameters for pid %04x:'%s'") % Pid % strerror(errno) << std::endl;
			success = false;
		}
	}
		
	return !res;
}


bool cDemux::pesFilterSet(const unsigned short Pid, int dvr)
{
	int res = 0;
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	struct dmx_pes_filter_params pesFilterParams;

	int handle;
	res = !openReal(handle);
	if(!res)
	{
		memset(&pesFilterParams, 0, sizeof(struct dmx_pes_filter_params));
		pesFilterParams.pid = Pid;
		pesFilterParams.input = DMX_IN_FRONTEND;
		if(dvr) 
		{
			pesFilterParams.output = DMX_OUT_TS_TAP; // goes to dvr
			pesFilterParams.pes_type = DMX_PES_OTHER;
		}
		else 
		{
			pesFilterParams.output = DMX_OUT_TAP;
			pesFilterParams.pes_type = DMX_PES_OTHER;
		}
		
		res = ioctl(handle, DMX_SET_BUFFER_SIZE, 0x400000);
		if(res < 0)
		{
			std::cerr << boost::format("cDemux::pesFilter error setting buffersize for pid %04x:'%s'") % Pid << strerror(errno) << std::endl;
		}
		
		
		res = ioctl(handle, DMX_SET_PES_FILTER, &pesFilterParams);
		if(res < 0)
		{
			std::cerr << boost::format("cDemux::pesFilter error setting parameters for pid %04x:'%s'") % Pid << strerror(errno) << std::endl;
		}
	}
	return !res;
}


bool cDemux::pesFilter(const unsigned short Pid)
{
	int handle;

	Stop();
	closeAll(); /* close and remove all existing handles */

	pesFilterSet(Pid, true);

	if(mPesType == DMX_VIDEO_CHANNEL)
	{
		/* stream for DVR, too */
		pesFilterSet(0x00, true);
	}
}


void cDemux::SetSyncMode(AVSYNC_TYPE mode)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
}


void * cDemux::getBuffer()
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return 0;
}


void * cDemux::getChannel()
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return 0;
}


DMX_CHANNEL_TYPE cDemux::getChannelType(void)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
	return DMX_VIDEO_CHANNEL;
}


void cDemux::addPid(unsigned short Pid)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
}


void cDemux::getSTC(int64_t * STC)
{
	std::cout << "cDemux::" << __FUNCTION__ << std::endl;
}


bool cDemux::openReal(int &handle)
{
	handle = open(DEMUX_DEVICE, O_RDWR);
	if(handle < 0)
	{
		std::cerr << "cDemux::" << __FUNCTION__ << boost::format(": Error opening demux: %s") % strerror(errno) << std::endl;
	}
	else
	{
		mHandleList.push_back(handle);
	}
	return (handle >= 0);
}


void cDemux::closeAll()
{
	if(!mHandleList.empty())
	{
		std::vector<int>::iterator it;
		for(it = mHandleList.begin(); it != mHandleList.end(); ++it)
		{
			close(*it);
		}
		mHandleList.clear();
	}
}

