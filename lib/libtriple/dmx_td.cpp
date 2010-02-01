#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <hardware/tddevices.h>
#include "dmx_td.h"

static const char *aDMXCHANNELTYPE[] = {
	"",
	"DMX_VIDEO_CHANNEL",
	"DMX_AUDIO_CHANNEL",
	"DMX_PES_CHANNEL",
	"DMX_PSI_CHANNEL",
	"DMX_PIP_CHANNEL",
	"DMX_TP_CHANNEL",
	"DMX_PCR_ONLY_CHANNEL",
};


cDemux::cDemux(int n)
{
	if (n < 0 || n > 2)
	{
		fprintf(stderr, "ERROR: cDemux::cDemux, n invalid (%d)\n", n);
		num = 0;
	}
	else
		num = n;
}

bool cDemux::Open(DMX_CHANNEL_TYPE pes_type, void * /*hVideoBuffer*/, int uBufferSize)
{
	fprintf(stderr, "cDemux::Open pes_type = %s (%d), uBufferSize = %d\n", aDMXCHANNELTYPE[pes_type], pes_type, uBufferSize);
	char devname[32];
	if (fd > -1)
		fprintf(stderr, "cDemux::Open FD ALREADY OPENED? fd = %d\n", fd);
	sprintf(devname, "%s%d", "/dev/" DEVICE_NAME_DEMUX, num);
	fd = open(devname, O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "cDemux::Open %s: %m", devname);
		return false;
	}
	if (ioctl(fd, DEMUX_SET_BUFFER_SIZE, uBufferSize) < 0)
		fprintf(stderr, "cDemux::Open DEMUX_SET_BUFFER_SIZE failed (%m)\n");

	return true;
}

void cDemux::Close(void)
{
	if (fd < 0)
		fprintf(stderr, "cDemux::Close: not open!\n");
	else
	{
		ioctl(fd, DEMUX_STOP);
		close(fd);
		fd = -1;
	}
}

bool cDemux::Start(void)
{
	if (fd < 0)
		fprintf(stderr, "cDemux::Start: not open!\n");
	else
		ioctl(fd, DEMUX_START);

	return true;
}

bool cDemux::Stop(void)
{
	if (fd < 0)
		fprintf(stderr, "cDemux::Stop: not open!\n");
	else
		ioctl(fd, DEMUX_STOP);

	return true;
}

int cDemux::Read(unsigned char *buff, int len, int /*Timeout*/)
{
	return read(fd, buff, len);
}

bool cDemux::sectionFilter(unsigned short pid, const unsigned char * const filter,
			   const unsigned char * const mask, int len, int timeout,
			   const unsigned char * const negmask)
{
	struct demux_filter_para flt;

	ioctl(fd, DEMUX_STOP);
	memset(&flt, 0, sizeof(flt));

	if (len > FILTER_LENGTH - 2)
		fprintf(stderr, "cDemux::sectionFilter: len too long: %d, FILTER_LENGTH: %d\n", len, FILTER_LENGTH);

	flt.pid = pid;
	flt.filter_length = len + 2;
	flt.filter[0] = filter[0];
	flt.mask[0] = mask[0];
	flt.timeout = timeout;
	memcpy(&flt.filter[3], filter + 1, len - 1);
	memcpy(&flt.mask[3],   mask + 1,   len - 1);
	if (negmask != NULL)
	{
		flt.positive[0] = negmask[0];
		memcpy(&flt.positive[3], negmask + 1, len - 1);
	}

	if (ioctl(fd, DEMUX_FILTER_SET, &flt) < 0)
		return false;

	return true;
}

bool cDemux::pesFilter(const unsigned short pid)
{
	demux_pes_para flt;

	if (pid <= 0x0001 && dmx_type != DMX_PES_PCR)
		return false;
	if ((pid >= 0x0002 && pid <= 0x000f) || pid >= 0x1fff)
		return false;

	memset(&flt, 0, sizeof(flt));
	flt.pid = pid;
	flt.output = OUT_DECODER;
	flt.pesType = (PesType)dmx_type;

	return (ioctl(fd, DEMUX_FILTER_PES_SET, &flt) >= 0);
}

#if 0
		void SetSyncMode(AVSYNC_TYPE mode);
		void addPid(unsigned short Pid);
		void getSTC(int64_t * STC);
		//
		cDemux(int num = 0);
		~cDemux();
#endif
