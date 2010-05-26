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
#include "audio_dvbapi.h"
#include <iostream>
#include <boost/format.hpp>

cAudio *audioDecoder = 0;

cAudio::cAudio(void * hBuffer, void * encHD, void * encSD)
{
}

cAudio::~cAudio(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}

void * cAudio::GetHandle()
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}

void * cAudio::GetDSP()
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


void cAudio::HandleAudioMessage(int Event, void *pData)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


void cAudio::HandlePcmMessage(int Event, void *pData)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


/* shut up */
int cAudio::mute(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::unmute(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::SetMute(int enable)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


/* bypass audio to external decoder */
int cAudio::enableBypass(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::disableBypass(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


/* volume, min = 0, max = 255 */
int cAudio::setVolume(unsigned int left, unsigned int right)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::getVolume(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


/* start and stop audio */
int cAudio::Start(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::Stop(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


bool cAudio::Pause(bool Pcm)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return true;
}


bool cAudio::Resume(bool Pcm)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return true;
}


void cAudio::SetStreamType(AUDIO_FORMAT type)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


AUDIO_FORMAT cAudio::GetStreamType(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return AUDIO_FMT_AUTO;
}


bool cAudio::ReceivedAudioDelay(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return true;
}


void cAudio::SetReceivedAudioDelay(bool set)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


unsigned int cAudio::GetAudioDelay(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


void cAudio::SetSyncMode(AVSYNC_TYPE Mode)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


/* stream source */
int cAudio::getSource(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::setSource(int source)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::Flush(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


/* select channels */
int cAudio::setChannel(int channel)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::getChannel(void)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::PrepareClipPlay(int uNoOfChannels, int uSampleRate, int uBitsPerSample, int bLittleEndian)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::WriteClip(unsigned char * buffer, int size)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


int cAudio::StopClip()
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return 0;
}


void cAudio::getAudioInfo(int &type, int &layer, int& freq, int &bitrate, int &mode)
{
	type = 0;
	layer = 0;
	freq = 0;
	bitrate = 0;
	mode = 0;
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


void cAudio::SetSRS(int iq_enable, int nmgr_enable, int iq_mode, int iq_level)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


bool cAudio::IsHdmiDDSupported()
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
	return true;
}


void cAudio::SetHdmiDD(bool enable)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


void cAudio::SetSpdifDD(bool enable)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


void cAudio::ScheduleMute(bool On)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}


void cAudio::EnableAnalogOut(bool enable)
{
	std::cout << "cAudio::" << __FUNCTION__ << std::endl;
}

