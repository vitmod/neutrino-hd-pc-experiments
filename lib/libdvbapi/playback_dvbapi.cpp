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
#include "playback_dvbapi.h"

cPlayback::cPlayback(int num)
{
}

cPlayback::~cPlayback()
{
}


void cPlayback::PlaybackNotify (int  Event, void *pData, void *pTag)
{
}


void cPlayback::DMNotify(int Event, void *pTsBuf, void *Tag)
{
}


bool cPlayback::Open(playmode_t PlayMode)
{
	return true;
}


void cPlayback::Close(void)
{
}


bool cPlayback::Start(char * filename, unsigned short vpid, int vtype, unsigned short apid, bool ac3)
{
	return true;
}


bool cPlayback::Stop(void)
{
	return true;
}


bool cPlayback::SetAPid(unsigned short pid, bool ac3)
{
	return true;
}


bool cPlayback::SetSpeed(int speed)
{
	return true;
}


bool cPlayback::GetSpeed(int &speed) const
{
	return true;
}


bool cPlayback::GetPosition(int &position, int &duration)
{
	return true;
}


bool cPlayback::GetOffset(off64_t &offset)
{
	return true;
}


bool cPlayback::SetPosition(int position, bool absolute)
{
	return true;
}


bool cPlayback::IsPlaying(void) const
{
	return true;
}


bool cPlayback::IsEnabled(void) const
{
	return true;
}


void * cPlayback::GetHandle(void)
{
	return 0;
}


void * cPlayback::GetDmHandle(void)
{
	return 0;
}


int cPlayback::GetCurrPlaybackSpeed(void) const
{
	return 0;
}


void cPlayback::FindAllPids(uint16_t *apids, unsigned short *ac3flags, uint16_t *numpida, std::string *language)
{
}

//
