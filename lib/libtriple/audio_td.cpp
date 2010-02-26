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

int cAudio::mute(void)
{
	int ret;
	ret = ioctl(fd, MPEG_AUD_SET_MUTE, 1);
	if (ret < 0)
		fprintf(stderr, "cAudio::mute failed (%m)\n");
	return ret;
}

int cAudio::unmute(void)
{
	int ret;
	ret = ioctl(fd, MPEG_AUD_SET_MUTE, 0);
	if (ret < 0)
		fprintf(stderr, "cAudio::unmute failed (%m)\n");
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
	return ioctl(fd, MPEG_AUD_PLAY);
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

int cAudio::setChannel(int /*channel*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

int cAudio::PrepareClipPlay(int /*uNoOfChannels*/, int /*uSampleRate*/, int /*uBitsPerSample*/, int /*bLittleEndian*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

int cAudio::WriteClip(unsigned char * /*buffer*/, int /*size*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

int cAudio::StopClip()
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
};

void cAudio::getAudioInfo(int &/*type*/, int &/*layer*/, int &/*freq*/, int &/*bitrate*/, int &/*mode*/)
{
	fprintf(stderr, "cAudio::%s\n", __FUNCTION__);
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

#if 0
int CAudio::setBypassMode(int disable)
{
	/* disable = 1 actually means: audio is MPEG, disable = 0 is audio is AC3 */
	if (disable)
		return quiet_fop(ioctl, MPEG_AUD_SET_MODE, AUD_MODE_MPEG);

	/* dvb2001 does always set AUD_MODE_DTS before setting AUD_MODE_AC3,
	   this might be some workaround, so we do the same... */
	quiet_fop(ioctl, MPEG_AUD_SET_MODE, AUD_MODE_DTS);
	return quiet_fop(ioctl, MPEG_AUD_SET_MODE, AUD_MODE_AC3);

	/* all those ioctl aways return "invalid argument", but they seem to
	   work nevertheless, that's why I use quiet_fop here */
}

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
