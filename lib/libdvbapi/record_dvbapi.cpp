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
#include "record_dvbapi.h"

cRecord::cRecord(int num)
{
}


cRecord::~cRecord()
{
}


bool cRecord::Open(int numpids)
{
}


void cRecord::Close(void)
{
}


bool cRecord::Start(int fd, unsigned short vpid, unsigned short * apids, int numpids)
{
	return true;
}


bool cRecord::Stop(void)
{
	return true;
}


void cRecord::RecordNotify(int Event, void *pData)
{
}
