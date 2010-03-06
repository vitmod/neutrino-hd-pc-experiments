/*
 * $Id$
 *
 * (C) 2002-2003 Andreas Oberritter <obi@tuxbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <cstdlib>

//#include <zapit/zapit.h>
#include <zapit/debug.h>
//#include <zapit/settings.h>

#ifdef HAVE_TRIPLEDRAGON
#include <avs/avs_inf.h>
#include <clip/clipinfo.h>
#endif
#include "video_td.h"
#include <hardware/tddevices.h>
#define VIDEO_DEVICE "/dev/" DEVICE_NAME_VIDEO

cVideo * videoDecoder = NULL;
int system_rev = 0;

extern struct Ssettings settings;

cVideo::cVideo(int, void *, void *)
{
	if ((fd = open(VIDEO_DEVICE, O_RDWR)) < 0)
		ERROR(VIDEO_DEVICE);
#ifdef HAVE_TRIPLEDRAGON
	playstate = VIDEO_STOPPED;
	croppingMode = VID_DISPMODE_NORM;
	z[0] = 100;
	z[1] = 100;
	zoomvalue = &z[0];
	const char *blanknames[2] = { "/share/tuxbox/blank_576.mpg", "/share/tuxbox/blank_480.mpg" };
	int blankfd;
	struct stat st;

	for (int i = 0; i < 2; i++)
	{
		blank_data[i] = NULL; /* initialize */
		blank_size[i] = 0;
		blankfd = open(blanknames[i], O_RDONLY);
		if (blankfd < 0)
		{
			WARN("cannot open %s: %m", blanknames[i]);
			continue;
		}
		if (fstat(blankfd, &st) != -1 && st.st_size > 0)
		{
			blank_size[i] = st.st_size;
			blank_data[i] = malloc(blank_size[i]);
			if (! blank_data[i])
				ERROR("cannot malloc memory");
			else if (read(blankfd, blank_data[i], blank_size[i]) != blank_size[i])
			{
				ERROR("short read");
				free(blank_data[i]); /* don't leak... */
				blank_data[i] = NULL;
			}
		}
		close(blankfd);
	}
#endif

#ifdef HAVE_DBOX_HARDWARE
	/* setBlank is not _needed_ on the Dreambox. I don't know about the
	   dbox, so i ifdef'd it out. Not blanking the fb leaves the bootlogo
	   on screen until the video starts. --seife */
	setBlank(true);
#endif
}

cVideo::~cVideo(void)
{
#ifdef HAVE_TRIPLEDRAGON
	playstate = VIDEO_STOPPED;
	for (int i = 0; i < 2; i++)
	{
		if (blank_data[i])
			free(blank_data[i]);
		blank_data[i] = NULL;
	}
#endif
	if (fd >= 0)
		close(fd);
}

int cVideo::setAspectRatio(int aspect, int mode)
{
	fprintf(stderr, "cVideo::setAspectRatio(%d, %d)\n", aspect, mode);
	vidDispSize_t dsize = VID_DISPSIZE_UNKNOWN;
	vidDispMode_t dmode = VID_DISPMODE_NORM;

	/* values are hardcoded in neutrino_menue.cpp, "2" is 14:9 -> not used */
	if (aspect != -1)
	{
		switch(aspect)
		{
		case 1:
			dsize = VID_DISPSIZE_4x3;
			break;
		case 3:
			dsize = VID_DISPSIZE_16x9;
			break;
		default:
			break;
		}
		fop(ioctl, MPEG_VID_SET_DISPSIZE, dsize);
	}
	if (mode != -1)
	{
		switch(mode)
		{
		case DISPLAY_AR_MODE_PANSCAN:
			// dmode = VID_DISPMODE_PANSCAN;
			break;
		case DISPLAY_AR_MODE_LETTERBOX:
			dmode = VID_DISPMODE_LETTERBOX;
			break;
		default:
			break;
		}
		setCroppingMode(dmode);
	}

	return 0;
}

#ifndef HAVE_TRIPLEDRAGON
video_format_t cVideo::getAspectRatio(void)
{
	struct video_status status;
	fop(ioctl, VIDEO_GET_STATUS, &status);
#if HAVE_DVB_API_VERSION < 3
	return status.videoFormat;
#else
	return status.video_format;
#endif
}
#else
int cVideo::getAspectRatio(void)
{
	VIDEOINFO v;
	/* this memset silences *TONS* of valgrind warnings */
	memset(&v, 0, sizeof(v));
	quiet_fop(ioctl, MPEG_VID_GET_V_INFO, &v);
	if (v.pel_aspect_ratio < VID_DISPSIZE_4x3 || v.pel_aspect_ratio > VID_DISPSIZE_UNKNOWN)
	{
		WARN("invalid value %d, returning VID_DISPSIZE_UNKNOWN fd: %d", v.pel_aspect_ratio, fd);
		return VID_DISPSIZE_UNKNOWN;
	}
	/* hack: coolstream apparently is "value > 2 == 16:9" */
	return v.pel_aspect_ratio * 4;
}
#endif

int cVideo::setCroppingMode(vidDispMode_t format)
{
	croppingMode = format;
	const char *format_string[] = { "norm", "letterbox", "unknown", "mode_1_2", "mode_1_4", "mode_2x", "scale", "disexp" };

	if (format >= VID_DISPMODE_NORM && format <= VID_DISPMODE_DISEXP)
		fprintf(stderr, "cVideo::setCroppingMode(%d) => %s\n", format, format_string[format]);
	else
		fprintf(stderr, "cVideo::setCroppingMode(%d) => ILLEGAL format!\n", format);
	//DBG("setting cropping mode to %s", format_string[format]);
	return fop(ioctl, MPEG_VID_SET_DISPMODE, format);
}

#ifndef HAVE_TRIPLEDRAGON
video_stream_source_t cVideo::getSource(void)
{
	struct video_status status;
	fop(ioctl, VIDEO_GET_STATUS, &status);
#if HAVE_DVB_API_VERSION < 3
	return status.streamSource;
#else
	return status.stream_source;
#endif
}
#endif

int cVideo::Start(void * /*PcrChannel*/, unsigned short /*PcrPid*/, unsigned short /*VideoPid*/, void * /*hChannel*/)
{
#ifdef HAVE_TRIPLEDRAGON
	if (playstate == VIDEO_PLAYING)
		return 0;
	playstate = VIDEO_PLAYING;
	fop(ioctl, MPEG_VID_PLAY);
	return fop(ioctl, MPEG_VID_SYNC_ON, VID_SYNC_AUD);
#else
	return fop(ioctl, VIDEO_PLAY);
#endif
}

int cVideo::Stop(bool /*blank*/)
{
#ifdef HAVE_TRIPLEDRAGON
	playstate = VIDEO_STOPPED;
#endif
	return fop(ioctl, MPEG_VID_STOP);
}

#ifndef HAVE_TRIPLEDRAGON
int cVideo::setBlank(int enable)
{
	return fop(ioctl, VIDEO_SET_BLANK, enable);
}
#else
int cVideo::setBlank(int)
{
	/* The TripleDragon has no VIDEO_SET_BLANK ioctl.
	   instead, you write a black still-MPEG Iframe into the decoder.
	   The original software uses different files for 4:3 and 16:9 and
	   for PAL and NTSC. I optimized that a little bit
	 */
	int index = 0; /* default PAL */
	VIDEOINFO v;
	BUFINFO buf;
	memset(&v, 0, sizeof(v));
	quiet_fop(ioctl, MPEG_VID_GET_V_INFO, &v);

	if ((v.v_size % 240) == 0) /* NTSC */
	{
		INFO("NTSC format detected");
		index = 1;
	}

	if (blank_data[index] == NULL) /* no MPEG found */
		return -1;

	/* hack: this might work only on those two still-MPEG files!
	   I diff'ed the 4:3 and the 16:9 still mpeg from the original
	   soft and spotted the single bit difference, so there is no
	   need to keep two different MPEGs in memory
	   If we would read them from disk all the time it would be
	   slower and it might wake up the drive occasionally */
	if (v.pel_aspect_ratio == VID_DISPSIZE_4x3)
		((char *)blank_data[index])[7] &= ~0x10; // clear the bit
	else
		((char *)blank_data[index])[7] |=  0x10; // set the bit

	//WARN("blank[7] == 0x%02x", ((char *)blank_data[index])[7]);

	buf.ulLen = blank_size[index];
	buf.ulStartAdrOff = (int)blank_data[index];
	return fop(ioctl, MPEG_VID_STILLP_WRITE, &buf);
}
#endif

int cVideo::SetVideoSystem(int video_system, bool remember)
{
	fprintf(stderr, "cVideo::setVideoSystem(%d, %d)\n", video_system, remember);
	if (video_system > VID_DISPFMT_SECAM || video_system < 0)
		video_system = VID_DISPFMT_PAL;
        return fop(ioctl, MPEG_VID_SET_DISPFMT, video_system);
}

int cVideo::getPlayState(void)
{
	return playstate;
}

void cVideo::SetVideoMode(analog_mode_t mode)
{
	fprintf(stderr, "cVideo::setVideoMode(%d)\n", mode);
}

void cVideo::ShowPicture(const char * fname)
{
	fprintf(stderr, "cVideo::ShowPicture: %s\n", fname);
}

void cVideo::StopPicture()
{
	fprintf(stderr, "cVideo::StopPicture()\n");
}

void cVideo::Standby(unsigned int bOn)
{
	fprintf(stderr, "cVideo::Standby: %d\n", bOn);
}

int cVideo::getBlank(void)
{
	fprintf(stderr, "cVideo::getBlank\n");
	return 0;
}

#if 0
int cVideo::setVideoOutput(vidOutFmt_t arg)
{
	return fop(ioctl, MPEG_VID_SET_OUTFMT, arg);
}
#endif

/* set zoom in percent (100% == 1:1) */
int cVideo::setZoom(int zoom)
{
	if (zoom == -1) // "auto" reset
		zoom = *zoomvalue;

	if (zoom > 150 || zoom < 100)
		return -1;

	*zoomvalue = zoom;

	if (zoom == 100)
	{
		setCroppingMode(croppingMode);
		return fop(ioctl, MPEG_VID_SCALE_OFF);
	}

	/* the SCALEINFO describes the source and destination of the scaled
	   video. "src" is the part of the source picture that gets scaled,
	   "dst" is the area on the screen where this part is displayed
	   Messing around with MPEG_VID_SET_SCALE_POS disables the automatic
	   letterboxing, which, as I guess, is only a special case of
	   MPEG_VID_SET_SCALE_POS. Therefor we need to care for letterboxing
	   etc here, which is probably not yet totally correct */
	SCALEINFO s;
	memset(&s, 0, sizeof(s));
	if (zoom > 100)
	{
		vidDispSize_t x = (vidDispSize_t)getAspectRatio();
		if (x == VID_DISPSIZE_4x3 && croppingMode == VID_DISPMODE_NORM)
		{
			s.src.hori_size = 720;
			s.des.hori_size = 720 * 3/4 * zoom / 100;
			if (s.des.hori_size > 720)
			{
				/* the destination exceeds the screen size.
				   TODO: decrease source size to allow higher
				   zoom factors (is this useful ?) */
				s.des.hori_size = 720;
				zoom = 133; // (720*4*100)/(720*3)
				*zoomvalue = zoom;
			}
		}
		else
		{
			s.src.hori_size = 2 * 720 - 720 * zoom / 100;
			s.des.hori_size = 720;
		}
		s.src.vert_size = 2 * 576 - 576 * zoom / 100;
		s.des.hori_off = (720 - s.des.hori_size) / 2;
		s.des.vert_size = 576;
	}
/* not working correctly (wrong formula) and does not make sense IMHO
	else
	{
		s.src.hori_size = 720;
		s.src.vert_size = 576;
		s.des.hori_size = 720 * zoom / 100;
		s.des.vert_size = 576 * zoom / 100;
		s.des.hori_off = (720 - s.des.hori_size) / 2;
		s.des.vert_off = (576 - s.des.vert_size) / 2;
	}
 */
	DBG("setZoom: %d%% src: %d:%d:%d:%d dst: %d:%d:%d:%d", zoom,
		s.src.hori_off,s.src.vert_off,s.src.hori_size,s.src.vert_size,
		s.des.hori_off,s.des.vert_off,s.des.hori_size,s.des.vert_size);
	fop(ioctl, MPEG_VID_SET_DISPMODE, VID_DISPMODE_SCALE);
	fop(ioctl, MPEG_VID_SCALE_ON);
	return fop(ioctl, MPEG_VID_SET_SCALE_POS, &s);
}

#if 0
int cVideo::getZoom(void)
{
	return *zoomvalue;
}

void cVideo::setZoomAspect(int index)
{
	if (index < 0 || index > 1)
		WARN("index out of range");
	else
		zoomvalue = &z[index];
}
#endif

void cVideo::Pig(int x, int y, int w, int h, int /*osd_w*/, int /*osd_h*/)
{
	/* x = y = w = h = -1 -> reset / "hide" PIG */
	if (x == -1 && y == -1 && w == -1 && h == -1)
	{
		setZoom(-1);
		return;
	}
	SCALEINFO s;
	memset(&s, 0, sizeof(s));
	s.src.hori_size = 720;
	s.src.vert_size = 576;
	s.des.hori_off = x;
	s.des.vert_off = y;
	s.des.hori_size = w;
	s.des.vert_size = h;
	DBG("setPig src: %d:%d:%d:%d dst: %d:%d:%d:%d",
		s.src.hori_off,s.src.vert_off,s.src.hori_size,s.src.vert_size,
		s.des.hori_off,s.des.vert_off,s.des.hori_size,s.des.vert_size);
	fop(ioctl, MPEG_VID_SET_DISPMODE, VID_DISPMODE_SCALE);
	fop(ioctl, MPEG_VID_SCALE_ON);
	fop(ioctl, MPEG_VID_SET_SCALE_POS, &s);
}

#if 0
int cVideo::VdecIoctl(int request, int arg)
{
	int ret = 0;
	if (fd < 0)
		return -EBADFD;
	DBG("fd: %d req: 0x%08x arg: %d", fd, request, arg);
	ret = ioctl(fd, request, arg);
	if (ret < 0)
		return -errno;
	return ret;
}
#endif

void cVideo::getPictureInfo(int &width, int &height, int &rate)
{
	VIDEOINFO v;
	/* this memset silences *TONS* of valgrind warnings */
	memset(&v, 0, sizeof(v));
	quiet_fop(ioctl, MPEG_VID_GET_V_INFO, &v);
	rate = (int)v.frame_rate;
	width = (int)v.h_size;
	height = (int)v.v_size;
}

void cVideo::SetSyncMode(AVSYNC_TYPE /*Mode*/)
{
	fprintf(stderr, "cVideo::%s\n", __FUNCTION__);
};

int cVideo::SetStreamType(VIDEO_FORMAT type)
{
	const char *aVIDEOFORMAT[] = {
	"VIDEO_FORMAT_MPEG2",
	"VIDEO_FORMAT_MPEG4",
	"VIDEO_FORMAT_VC1",
	"VIDEO_FORMAT_JPEG",
	"VIDEO_FORMAT_GIF",
	"VIDEO_FORMAT_PNG"
	};

	printf("cVideo::SetStreamType - type=%s\n", aVIDEOFORMAT[type]);
#if 0
	int streamtype = VIDEO_STREAMTYPE_MPEG2;

	switch(type)
	{
	default:
	case VIDEO_FORMAT_MPEG2:
		streamtype = VIDEO_STREAMTYPE_MPEG2;
		break;
	case VIDEO_FORMAT_MPEG4:
		streamtype = VIDEO_STREAMTYPE_MPEG4_H264;
		break;
	case VIDEO_FORMAT_VC1:
		streamtype = VIDEO_STREAMTYPE_VC1;
		break;
	}

	if (ioctl(privateData->m_fd, VIDEO_SET_STREAMTYPE, streamtype) < 0)
		printf("VIDEO_SET_STREAMTYPE failed(%m)");
#endif
	return 0;
}

