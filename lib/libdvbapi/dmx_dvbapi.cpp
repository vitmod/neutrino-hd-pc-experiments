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

#include <stdlib.h>
#include <stdint.h>
#include "dmx_dvbapi.h"

cDemux *videoDemux = 0;
cDemux *audioDemux = 0;

cDemux::cDemux(int num)
{
}


cDemux::~cDemux()
{
}


bool cDemux::Open(DMX_CHANNEL_TYPE pes_type, void * hVideoBuffer, int uBufferSize)
{
	return true;
}


void cDemux::Close(void)
{
}


bool cDemux::Start(void)
{
	return true;
}


bool cDemux::Stop(void)
{
	return true;
}


int cDemux::Read(unsigned char *buff, int len, int Timeout)
{
	return 0;
}


void cDemux::SignalRead(int len)
{
}


unsigned short cDemux::GetPID(void)
{
	return 0;
}


const unsigned char *cDemux::GetFilterTID(void)
{
	return 0;
}


const unsigned char *cDemux::GetFilterMask(void)
{
	return 0;
}


bool cDemux::sectionFilter(unsigned short Pid, const unsigned char * const Tid, const unsigned char * const Mask, int len, int Timeout, const unsigned char * const nMask)
{
	return true;
}


bool cDemux::pesFilter(const unsigned short Pid)
{
	return true;
}


void cDemux::SetSyncMode(AVSYNC_TYPE mode)
{
}


void * cDemux::getBuffer()
{
	return 0;
}


void * cDemux::getChannel()
{
	return 0;
}


DMX_CHANNEL_TYPE cDemux::getChannelType(void)
{
	return DMX_VIDEO_CHANNEL;
}


void cDemux::addPid(unsigned short Pid)
{
}


void cDemux::getSTC(int64_t * STC)
{
}

