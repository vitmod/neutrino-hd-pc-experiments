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
#include "video_dvbapi.h"

unsigned int system_rev = 0x00;
cVideo *videoDecoder = 0;

/* constructor & destructor */
cVideo::cVideo(int mode, void * hChannel, void * hBuffer)
{
}

cVideo::~cVideo(void)
{
}

void * cVideo::GetDRM(void)
{
	return 0;
}


void * cVideo::GetTVEnc()
{
	return 0;
}


void * cVideo::GetTVEncSD()
{
	return 0;
}


void * cVideo::GetHandle()
{
	return 0;
}

void cVideo::SetAudioHandle(void * handle)
{
}


/* aspect ratio */
int cVideo::getAspectRatio(void)
{
	return 0;
}


void cVideo::getPictureInfo(int &width, int &height, int &rate)
{
}


int cVideo::setAspectRatio(int aspect, int mode)
{
	return 0;
}


/* cropping mode */
int cVideo::getCroppingMode(void)
{
	return 0;
}


int cVideo::setCroppingMode(void)
{
	return 0;
}

/* stream source */
int cVideo::getSource(void)
{
	return 0;
}


int cVideo::setSource(void)
{
	return 0;
}


int cVideo::GetStreamType(void)
{
	return 0; 
}


/* blank on freeze */
int cVideo::getBlank(void)
{
	return 0;
}


int cVideo::setBlank(int enable)
{
	return 0;
}


/* get play state */
int cVideo::getPlayState(void)
{
	return 0;
}


void cVideo::SetDRMDelay(unsigned int delay)
{
}
	

void cVideo::SetVideoDelay(unsigned int delay)
{
}


void cVideo::HandleDRMMessage(int Event, void *pData)
{
}


void cVideo::HandleVideoMessage(void * hHandle, int Event, void *pData)
{
}


VIDEO_DEFINITION cVideo::GetVideoDef(void)
{
	return VIDEO_SD; 
}


/* change video play state */
int cVideo::Start(void * PcrChannel, unsigned short PcrPid, unsigned short VideoPid, void * hChannel)
{
	return 0;
}


int cVideo::Stop(bool blank)
{
	return 0;
}


bool cVideo::Pause(void)
{
	return true;
}


bool cVideo::Resume(void)
{
	return true;
}


int cVideo::LipsyncAdjust()
{
	return 0;
}


int64_t cVideo::GetPTS(void)
{
	return 0;
}


int cVideo::Flush(void)
{
	return 0;
}


/* set video_system */
int cVideo::SetVideoSystem(int video_system, bool remember)
{
	return 0;
}


int cVideo::SetStreamType(VIDEO_FORMAT type)
{
	return 0;
}


void cVideo::SetSyncMode(AVSYNC_TYPE mode)
{
}


void cVideo::ShowPicture(const char * fname)
{
}


void cVideo::StopPicture()
{
}


void cVideo::Standby(unsigned int bOn)
{
}


void cVideo::Pig(int x, int y, int w, int h, int osd_w, int osd_h)
{
}


void cVideo::setContrast(int val)
{
}


bool cVideo::ReceivedDRMDelay(void)
{ 
	return true; 
}


bool cVideo::ReceivedVideoDelay(void)
{
	return true; 
}


void cVideo::SetReceivedDRMDelay(bool Received)
{
}
	
	
void cVideo::SetReceivedVideoDelay(bool Received)
{
}


void cVideo::SetFastBlank(bool onoff)
{
}


void cVideo::SetTVAV(bool onoff)
{
}


void cVideo::SetWideScreen(bool onoff)
{
}


void cVideo::SetVideoMode(analog_mode_t mode)
{
}


void cVideo::SetDBDR(int dbdr)
{
}

