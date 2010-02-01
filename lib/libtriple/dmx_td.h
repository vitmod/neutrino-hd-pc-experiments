#ifndef __DEMUX_TD_H
#define __DEMUX_TD_H

#include <stdlib.h>
extern "C" {
#include <sys/ioctl.h>
#include <hardware/xp/xp_osd_user.h>
}
#define DMX_FILTER_SIZE FILTER_LENGTH

typedef enum
{
	DMX_VIDEO_CHANNEL = DMX_PES_VIDEO,
	DMX_AUDIO_CHANNEL = DMX_PES_AUDIO,
	DMX_PCR_ONLY_CHANNEL = DMX_PES_PCR,
	DMX_PES_CHANNEL,
	DMX_PSI_CHANNEL,
	DMX_PIP_CHANNEL,
	DMX_TP_CHANNEL,
} DMX_CHANNEL_TYPE;

class cDemux
{
	private:
		int num;
		int fd;
		DMX_CHANNEL_TYPE dmx_type;
	public:

		bool Open(DMX_CHANNEL_TYPE pes_type, void * x = NULL, int y = 0);
		void Close(void);
		bool Start(void);
		bool Stop(void);
		int Read(unsigned char *buff, int len, int Timeout = 0);
		bool sectionFilter(unsigned short pid, const unsigned char * const filter, const unsigned char * const mask, int len, int Timeout = 0, const unsigned char * const negmask = NULL);
		bool pesFilter(const unsigned short pid);
#define AVSYNC_TYPE int
		void SetSyncMode(AVSYNC_TYPE mode);
		void * getBuffer();
		void * getChannel();
		const DMX_CHANNEL_TYPE getChannelType(void) { return dmx_type; };
		void addPid(unsigned short pid);
		void getSTC(int64_t * STC);
		//
		cDemux(int num = 0);
		~cDemux();
};

#endif //__DEMUX_H
