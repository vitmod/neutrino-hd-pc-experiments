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

cAudio *audioDecoder = 0;

cAudio::cAudio(void * hBuffer, void * encHD, void * encSD)
{
}

cAudio::~cAudio(void)
{
}

void * cAudio::GetHandle()
{
	return 0;
}

void * cAudio::GetDSP()
{
	return 0;
}


void cAudio::HandleAudioMessage(int Event, void *pData)
{
}


void cAudio::HandlePcmMessage(int Event, void *pData)
{
}


/* shut up */
int cAudio::mute(void)
{
	return 0;
}


int cAudio::unmute(void)
{
	return 0;
}


int cAudio::SetMute(int enable)
{
	return 0;
}


/* bypass audio to external decoder */
int cAudio::enableBypass(void)
{
	return 0;
}


int cAudio::disableBypass(void)
{
	return 0;
}


/* volume, min = 0, max = 255 */
int cAudio::setVolume(unsigned int left, unsigned int right)
{
	return 0;
}


int cAudio::getVolume(void)
{
	return 0;
}


/* start and stop audio */
int cAudio::Start(void)
{
	return 0;
}


int cAudio::Stop(void)
{
	return 0;
}


bool cAudio::Pause(bool Pcm)
{
	return true;
}


bool cAudio::Resume(bool Pcm)
{
	return true;
}


void cAudio::SetStreamType(AUDIO_FORMAT type)
{
}


AUDIO_FORMAT cAudio::GetStreamType(void)
{
	return AUDIO_FMT_AUTO;
}


bool cAudio::ReceivedAudioDelay(void)
{
	return true;
}


void cAudio::SetReceivedAudioDelay(bool set)
{
}


unsigned int cAudio::GetAudioDelay(void)
{
	return 0;
}


void cAudio::SetSyncMode(AVSYNC_TYPE Mode)
{
}


/* stream source */
int cAudio::getSource(void)
{
	return 0;
}


int cAudio::setSource(int source)
{
	return 0;
}


int cAudio::Flush(void)
{
	return 0;
}


/* select channels */
int cAudio::setChannel(int channel)
{
	return 0;
}


int cAudio::getChannel(void)
{
	return 0;
}


int cAudio::PrepareClipPlay(int uNoOfChannels, int uSampleRate, int uBitsPerSample, int bLittleEndian)
{
	return 0;
}


int cAudio::WriteClip(unsigned char * buffer, int size)
{
	return 0;
}


int cAudio::StopClip()
{
	return 0;
}


void cAudio::getAudioInfo(int &type, int &layer, int& freq, int &bitrate, int &mode)
{
}


void cAudio::SetSRS(int iq_enable, int nmgr_enable, int iq_mode, int iq_level)
{
}


bool cAudio::IsHdmiDDSupported()
{
	return true;
}


void cAudio::SetHdmiDD(bool enable)
{
}


void cAudio::SetSpdifDD(bool enable)
{
}


void cAudio::ScheduleMute(bool On)
{
}


void cAudio::EnableAnalogOut(bool enable)
{
}

