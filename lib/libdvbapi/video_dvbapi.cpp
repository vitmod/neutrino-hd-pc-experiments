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
#include <iostream>
#include <boost/format.hpp>

unsigned int system_rev = 0x00;
cVideo *videoDecoder = 0;

/* constructor & destructor */
cVideo::cVideo(int mode, void * hChannel, void * hBuffer)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}

cVideo::~cVideo(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}

void * cVideo::GetDRM(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void * cVideo::GetTVEnc()
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void * cVideo::GetTVEncSD()
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void * cVideo::GetHandle()
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}

void cVideo::SetAudioHandle(void * handle)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


/* aspect ratio */
int cVideo::getAspectRatio(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void cVideo::getPictureInfo(int &width, int &height, int &rate)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	width = 4096;
	height = 2160;
	rate = 24;
}


int cVideo::setAspectRatio(int aspect, int mode)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


/* cropping mode */
int cVideo::getCroppingMode(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::setCroppingMode(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}

/* stream source */
int cVideo::getSource(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::setSource(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::GetStreamType(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0; 
}


/* blank on freeze */
int cVideo::getBlank(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::setBlank(int enable)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


/* get play state */
int cVideo::getPlayState(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void cVideo::SetDRMDelay(unsigned int delay)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}
	

void cVideo::SetVideoDelay(unsigned int delay)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::HandleDRMMessage(int Event, void *pData)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::HandleVideoMessage(void * hHandle, int Event, void *pData)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


VIDEO_DEFINITION cVideo::GetVideoDef(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return VIDEO_SD; 
}


/* change video play state */
int cVideo::Start(void * PcrChannel, unsigned short PcrPid, unsigned short VideoPid, void * hChannel)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::Stop(bool blank)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


bool cVideo::Pause(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return true;
}


bool cVideo::Resume(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return true;
}


int cVideo::LipsyncAdjust()
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int64_t cVideo::GetPTS(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::Flush(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


/* set video_system */
int cVideo::SetVideoSystem(int video_system, bool remember)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


int cVideo::SetStreamType(VIDEO_FORMAT type)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return 0;
}


void cVideo::SetSyncMode(AVSYNC_TYPE mode)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::ShowPicture(const char * fname)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::StopPicture()
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::Standby(unsigned int bOn)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::Pig(int x, int y, int w, int h, int osd_w, int osd_h)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::setContrast(int val)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


bool cVideo::ReceivedDRMDelay(void)
{ 
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return true; 
}


bool cVideo::ReceivedVideoDelay(void)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
	return true; 
}


void cVideo::SetReceivedDRMDelay(bool Received)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}
	
	
void cVideo::SetReceivedVideoDelay(bool Received)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::SetFastBlank(bool onoff)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::SetTVAV(bool onoff)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::SetWideScreen(bool onoff)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::SetVideoMode(analog_mode_t mode)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}


void cVideo::SetDBDR(int dbdr)
{
	std::cout << "cVideo::" << __FUNCTION__ << std::endl;
}

