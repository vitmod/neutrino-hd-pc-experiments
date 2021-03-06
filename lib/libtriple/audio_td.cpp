#include <cstdio>
#include <cstdlib>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include <hardware/tddevices.h>
#define AUDIO_DEVICE "/dev/" DEVICE_NAME_AUDIO
#include "audio_td.h"

cAudio * audioDecoder = NULL;

cAudio::cAudio(void *, void *, void *)
{
	fd = -1;
	openDevice();
	Muted = false;
}

cAudio::~cAudio(void)
{
	closeDevice();
}

void cAudio::openDevice(void)
{
	if (fd < 0)
	{
		if ((fd = open(AUDIO_DEVICE, O_RDWR)) < 0)
			fprintf(stderr, "cAudio::openDevice: open failed (%m)\n");
	}
	else
		fprintf(stderr, "cAudio::openDevice: already open (fd = %d)\n", fd);
}

void cAudio::closeDevice(void)
{
	if (fd >= 0)
		close(fd);
	fd = -1;
}

int cAudio::do_mute(bool enable)
{
	int ret;
	Muted = enable;
	ret = ioctl(fd, MPEG_AUD_SET_MUTE, Muted);
	if (ret < 0)
		fprintf(stderr, "cAudio::%s(%d) failed (%m)\n", __FUNCTION__, (int)enable);
	return ret;
}

int map_volume(const int volume)
{
	unsigned char vol = volume;
	if (vol > 100)
		vol = 100;

//	vol = (invlog63[volume] + 1) / 2;
	vol = 31 - vol * 31 / 100;
	return vol;
}

int cAudio::setVolume(unsigned int left, unsigned int right)
{
//	int avsfd;
	int ret;
	int vl = map_volume(left);
	int vr = map_volume(right);
	int v = map_volume((left + right) / 2);
//	if (settings.volume_type == CControld::TYPE_OST || forcetype == (int)CControld::TYPE_OST)
	{
		AUDVOL vol;
		vol.frontleft  = vl;
		vol.frontright = vr;
		vol.rearleft   = vl;
		vol.rearright  = vr;
		vol.center     = v;
		vol.lfe        = v;
		ret = ioctl(fd, MPEG_AUD_SET_VOL, &vol);
		if (ret < 0)
			fprintf(stderr, "cAudio::setVolume MPEG_AUD_SET_VOL failed (%m)\n");
		return ret;
	}
#if 0
	else if (settings.volume_type == CControld::TYPE_AVS || forcetype == (int)CControld::TYPE_AVS)
	{
		if ((avsfd = open(AVS_DEVICE, O_RDWR)) < 0)
			perror("[controld] " AVS_DEVICE);
		else {
			if (ioctl(avsfd, IOC_AVS_SET_VOLUME, v))
				perror("[controld] IOC_AVS_SET_VOLUME");
			close(avsfd);
			return 0;
		}
	}
	fprintf(stderr, "CAudio::setVolume: invalid settings.volume_type = %d\n", settings.volume_type);
	return -1;
#endif
}

int cAudio::Start(void)
{
	int ret;
	ret = ioctl(fd, MPEG_AUD_PLAY);
	/* this seems to be not strictly necessary since neutrino
	   re-mutes all the time, but is certainly more correct */
	ioctl(fd, MPEG_AUD_SET_MUTE, Muted);
	return ret;
}

int cAudio::Stop(void)
{
	return ioctl(fd, MPEG_AUD_STOP);
}

bool cAudio::Pause(bool /*Pcm*/)
{
	return true;
};

void cAudio::SetSyncMode(AVSYNC_TYPE /*Mode*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void cAudio::SetStreamType(AUDIO_FORMAT type)
{
	int bypass_disable;
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	StreamType = type;

	if (StreamType != AUDIO_FMT_DOLBY_DIGITAL && StreamType != AUDIO_FMT_MPEG)
		fprintf(stderr, "cAudio::%s unhandled AUDIO_FORMAT %d\n", __FUNCTION__, StreamType);

	bypass_disable = (StreamType != AUDIO_FMT_DOLBY_DIGITAL);
	setBypassMode(bypass_disable);
};

int cAudio::setChannel(int /*channel*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	return 0;
};

int cAudio::PrepareClipPlay(int /*uNoOfChannels*/, int /*uSampleRate*/, int /*uBitsPerSample*/, int /*bLittleEndian*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	return 0;
};

int cAudio::WriteClip(unsigned char * /*buffer*/, int /*size*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	return 0;
};

int cAudio::StopClip()
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	return 0;
};

void cAudio::getAudioInfo(int &type, int &layer, int &freq, int &bitrate, int &mode)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
	unsigned int atype;
	scratchl2 i;
	if (ioctl(fd, MPEG_AUD_GET_DECTYP, &atype) < 0)
		perror("cAudio::getAudioInfo MPEG_AUD_GET_DECTYP");
	if (ioctl(fd, MPEG_AUD_GET_STATUS, &i) < 0)
		perror("cAudio::getAudioInfo MPEG_AUD_GET_STATUS");

	type = atype;
#if 0
/* this does not work, some of the values are negative?? */
	AMPEGStatus A;
	memcpy(&A, &i.word00, sizeof(i.word00));
	layer   = A.audio_mpeg_layer;
	mode    = A.audio_mpeg_mode;
	bitrate = A.audio_mpeg_bitrate;
	switch(A.audio_mpeg_frequency)
#endif
	layer   = (i.word00 >> 17) & 3;
	mode    = (i.word00 >> 6)  & 3;
	bitrate = (i.word00 >> 12) & 3;
	switch((i.word00 >> 10) & 3)
	{
	case 0:
		freq = 44100;
		break;
	case 1:
		freq = 48000;
		break;
	case 2:
		freq = 32000;
		break;
	default:
		freq = 0;
		break;
	}
	//fprintf(stderr, "type: %d layer: %d freq: %d bitrate: %d mode: %d\n", type, layer, freq, bitrate, mode);
};

void cAudio::SetSRS(int /*iq_enable*/, int /*nmgr_enable*/, int /*iq_mode*/, int /*iq_level*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void cAudio::SetSpdifDD(bool /*enable*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void ScheduleMute(bool /*On*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void EnableAnalogOut(bool /*enable*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void cAudio::setBypassMode(bool disable)
{
	/* disable = true: audio is MPEG, disable = false: audio is AC3 */
	if (disable)
	{
		ioctl(fd, MPEG_AUD_SET_MODE, AUD_MODE_MPEG);
		return;
	}
	/* dvb2001 does always set AUD_MODE_DTS before setting AUD_MODE_AC3,
	   this might be some workaround, so we do the same... */
	ioctl(fd, MPEG_AUD_SET_MODE, AUD_MODE_DTS);
	ioctl(fd, MPEG_AUD_SET_MODE, AUD_MODE_AC3);
	return;
	/* all those ioctl aways return "invalid argument", but they seem to
	   work anyway, so there's no use in checking the return value */
}

#if 0
int CAudio::enableBypass(void)
{
	return setBypassMode(0);
}

int CAudio::disableBypass(void)
{
	return setBypassMode(1);
}

int CAudio::setSource(audio_stream_source_t source)
{
	return quiet_fop(ioctl, AUDIO_SELECT_SOURCE, source);
}

#ifndef HAVE_TRIPLEDRAGON
audio_stream_source_t CAudio::getSource(void)
{
	struct audio_status status;
	fop(ioctl, AUDIO_GET_STATUS, &status);
	return status.stream_source;
}
#endif

int CAudio::setChannel(audio_channel_select_t channel)
{
	return fop(ioctl, AUDIO_CHANNEL_SELECT, channel);
}

#ifndef HAVE_TRIPLEDRAGON
audio_channel_select_t CAudio::getChannel(void)
{
	struct audio_status status;
	fop(ioctl, AUDIO_GET_STATUS, &status);
	return status.channel_select;
}
#endif
#endif
