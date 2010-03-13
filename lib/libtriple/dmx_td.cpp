#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include <string>
#include <hardware/tddevices.h>
#include "dmx_td.h"

cDemux *videoDemux = NULL;
cDemux *audioDemux = NULL;
//cDemux *pcrDemux = NULL;

static const char *DMX_T[] = {
	"",
	"DMX_VIDEO_CHANNEL",
	"DMX_AUDIO_CHANNEL",
	"DMX_PES_CHANNEL",
	"DMX_PSI_CHANNEL",
	"DMX_PIP_CHANNEL",
	"DMX_TP_CHANNEL",
	"DMX_PCR_ONLY_CHANNEL"
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
	fd = -1;
}

cDemux::~cDemux()
{
	fprintf(stderr, "cDemux::%s #%d fd: %d\n", __FUNCTION__, num, fd);
	Close();
}

bool cDemux::Open(DMX_CHANNEL_TYPE pes_type, void * /*hVideoBuffer*/, int uBufferSize)
{
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
	fprintf(stderr, "cDemux::Open #%d pes_type: %s (%d), uBufferSize: %d devname: %s fd: %d\n",
			num, DMX_T[pes_type], pes_type, uBufferSize, devname, fd);

	dmx_type = pes_type;

	if (!pesfds.empty())
	{
		fprintf(stderr, "ERROR! pesfds not empty!\n"); /* TODO: error handling */
		return false;
	}
	if (pes_type == DMX_TP_CHANNEL)
	{
		struct demux_bucket_para bp;
		bp.unloader.unloader_type = UNLOADER_TYPE_TRANSPORT;
		bp.unloader.threshold     = 128;
		ioctl(fd, DEMUX_SELECT_SOURCE, INPUT_FROM_CHANNEL0);
		ioctl(fd, DEMUX_SET_BUFFER_SIZE, 230400);
		ioctl(fd, DEMUX_FILTER_BUCKET_SET, &bp);
		return true;
	}
	if (uBufferSize > 0)
	{
		/* probably uBufferSize == 0 means "use default size". TODO: find a reasonable default */
		if (ioctl(fd, DEMUX_SET_BUFFER_SIZE, uBufferSize) < 0)
			fprintf(stderr, "cDemux::Open DEMUX_SET_BUFFER_SIZE failed (%m)\n");
	}

	return true;
}

void cDemux::Close(void)
{
	if (fd < 0)
	{
		fprintf(stderr, "cDemux::%s #%d: not open!\n", __FUNCTION__, num);
		return;
	}

	for (std::vector<int>::const_iterator i = pesfds.begin(); i != pesfds.end(); ++i)
	{
		fprintf(stderr, "iterator: stopping and closing demux fd %d\n", *i);
		if (ioctl(*i, DEMUX_STOP) < 0)
			perror("DEMUX_STOP");
		if (close(*i) < 0)
			perror("close");
	}
	pesfds.clear();
	ioctl(fd, DEMUX_STOP);
	close(fd);
	fd = -1;
}

bool cDemux::Start(void)
{
	if (fd < 0)
	{
		fprintf(stderr, "cDemux::%s #%d: not open!\n", __FUNCTION__, num);
		return false;
	}

	for (std::vector<int>::const_iterator i = pesfds.begin(); i != pesfds.end(); ++i)
	{
		fprintf(stderr, "iterator: starting demux fd %d\n", *i);
		if (ioctl(*i, DEMUX_START) < 0)
			perror("DEMUX_START");
	}
	ioctl(fd, DEMUX_START);
	return true;
}

bool cDemux::Stop(void)
{
	if (fd < 0)
	{
		fprintf(stderr, "cDemux::%s #%d: not open!\n", __FUNCTION__, num);
		return false;
	}
	for (std::vector<int>::const_iterator i = pesfds.begin(); i != pesfds.end(); ++i)
	{
		fprintf(stderr, "iterator: stopping demux fd %d\n", *i);
		if (ioctl(*i, DEMUX_STOP) < 0)
			perror("DEMUX_STOP");
	}
	ioctl(fd, DEMUX_STOP);
	return true;
}

int cDemux::Read(unsigned char *buff, int len, int timeout)
{
#if 0
	if (len != 4095 && timeout != 10)
		fprintf(stderr, "cDemux::%s #%d fd: %d type: %s len: %d timeout: %d\n",
			__FUNCTION__, num, fd, DMX_T[dmx_type], len, timeout);
#endif
	int rc;
	struct pollfd ufds;
	ufds.fd = fd;
	ufds.events = POLLIN;
	ufds.revents = 0;

	if (timeout > 0)
	{
		rc = ::poll(&ufds, 1, timeout);
		if (!rc)
			return 0; // timeout
		else if (rc < 0)
		{
			/* we consciously ignore EINTR, since it does not happen in practice */
			return -1;
		}
		if (ufds.revents & POLLERR) /* POLLERR means buffer error, i.e. buffer overflow */
		{
			fprintf(stderr, "[cDemux::Read] received POLLERR, fd %d\n", fd);
			return -1;
		}
		if (ufds.revents & POLLHUP) /* we get POLLHUP if e.g. a too big DMX_BUFFER_SIZE was set */
		{
			fprintf(stderr, "[cDemux::Read] received POLLHUP, fd %d\n", fd);
			return -1;
		}
		if (!(ufds.revents & POLLIN)) /* we requested POLLIN but did not get it? */
		{
			fprintf(stderr, "cDemux::%s: not ufds.revents&POLLIN, please report! "
					"revents: 0x%x fd: %d rc: %d '%m'\n", __FUNCTION__, ufds.revents, fd, rc);
			return 0;
		}
	}

	rc = ::read(fd, buff, len);
	//fprintf(stderr, "fd %d ret: %d\n", fd, rc);
	if (rc < 0)
		perror ("[cDemux::Read] read");

	return rc;
}

bool cDemux::sectionFilter(unsigned short pid, const unsigned char * const filter,
			   const unsigned char * const mask, int len, int timeout,
			   const unsigned char * const negmask)
{
	struct demux_filter_para flt;
	memset(&flt, 0, sizeof(flt));

	if (len > FILTER_LENGTH - 2)
		fprintf(stderr, "cDemux::sectionFilter #%d: len too long: %d, FILTER_LENGTH: %d\n", num, len, FILTER_LENGTH);

	flt.pid = pid;
	flt.filter_length = len + 2 * (len > 1); /* only add the two bytes if required */
	flt.filter[0] = filter[0];
	flt.mask[0] = mask[0];
	flt.timeout = timeout;
	memcpy(&flt.filter[3], &filter[1], len - 1);
	memcpy(&flt.mask[3],   &mask[1],   len - 1);
	if (negmask != NULL)
	{
		flt.positive[0] = negmask[0];
		memcpy(&flt.positive[3], &negmask[1], len - 1);
	}

	flt.flags = XPDF_IMMEDIATE_START;

	int to = 0;
	switch (filter[0]) {
	case 0x00: /* program_association_section */
		to = 2000;
		break;
	case 0x01: /* conditional_access_section */
		to = 6000;
		break;
	case 0x02: /* program_map_section */
		to = 1500;
		break;
	case 0x03: /* transport_stream_description_section */
		to = 10000;
		break;
	/* 0x04 - 0x3F: reserved */
	case 0x40: /* network_information_section - actual_network */
		to = 10000;
		break;
	case 0x41: /* network_information_section - other_network */
		to = 15000;
		break;
	case 0x42: /* service_description_section - actual_transport_stream */
		to = 10000;
		break;
	/* 0x43 - 0x45: reserved for future use */
	case 0x46: /* service_description_section - other_transport_stream */
		to = 10000;
		break;
	/* 0x47 - 0x49: reserved for future use */
	case 0x4A: /* bouquet_association_section */
		to = 11000;
		break;
	/* 0x4B - 0x4D: reserved for future use */
	case 0x4E: /* event_information_section - actual_transport_stream, present/following */
		to = 2000;
		break;
	case 0x4F: /* event_information_section - other_transport_stream, present/following */
		to = 10000;
		break;
	/* 0x50 - 0x5F: event_information_section - actual_transport_stream, schedule */
	/* 0x60 - 0x6F: event_information_section - other_transport_stream, schedule */
	case 0x70: /* time_date_section */
		flt.flags  |= (XPDF_NO_CRC); /* section has no CRC */
		//flt.pid     = 0x0014;
		to = 30000;
		break;
	case 0x71: /* running_status_section */
		flt.flags  |= (XPDF_NO_CRC); /* section has no CRC */
		to = 0;
		break;
	case 0x72: /* stuffing_section */
		flt.flags  |= (XPDF_NO_CRC); /* section has no CRC */
		to = 0;
		break;
	case 0x73: /* time_offset_section */
		//flt.pid     = 0x0014;
		to = 30000;
		break;
	/* 0x74 - 0x7D: reserved for future use */
	case 0x7E: /* discontinuity_information_section */
		flt.flags  |= (XPDF_NO_CRC); /* section has no CRC */
		to = 0;
		break;
	case 0x7F: /* selection_information_section */
		to = 0;
		break;
	/* 0x80 - 0x8F: ca_message_section */
	/* 0x90 - 0xFE: user defined */
	/*        0xFF: reserved */
	default:
		break;
//		return -1;
	}
	if (timeout == 0)
		flt.timeout = to;
#if 0
	fprintf(stderr, "cDemux::%s #%d pid:0x%04hx fd:%d type:%s len:%d to:%d flags:%x\n", __FUNCTION__, num, pid, fd, DMX_T[dmx_type], len, flt.timeout,flt.flags);
	fprintf(stderr,"filt: ");for(int i=0;i<FILTER_LENGTH;i++)fprintf(stderr,"%02hhx ",flt.filter[i]);fprintf(stderr,"\n");
	fprintf(stderr,"mask: ");for(int i=0;i<FILTER_LENGTH;i++)fprintf(stderr,"%02hhx ",flt.mask  [i]);fprintf(stderr,"\n");
	fprintf(stderr,"posi: ");for(int i=0;i<FILTER_LENGTH;i++)fprintf(stderr,"%02hhx ",flt.positive[i]);fprintf(stderr,"\n");
#endif
	ioctl (fd, DEMUX_STOP);
	if (ioctl(fd, DEMUX_FILTER_SET, &flt) < 0)
		return false;

	return true;
}

bool cDemux::pesFilter(const unsigned short pid)
{
	demux_pes_para flt;

	if (pid <= 0x0001 && dmx_type != DMX_PCR_ONLY_CHANNEL)
		return false;
	if ((pid >= 0x0002 && pid <= 0x000f) || pid >= 0x1fff)
		return false;

fprintf(stderr, "cDemux::%s #%d pid: 0x%04hx fd: %d type: %s\n", __FUNCTION__, num, pid, fd, DMX_T[dmx_type]);

	if (dmx_type == DMX_TP_CHANNEL)
	{
		unsigned int n = pesfds.size();
		addPid(pid);
		return (n != pesfds.size());
	}
	memset(&flt, 0, sizeof(flt));
	flt.pid = pid;
	flt.output = OUT_DECODER;
	switch (dmx_type) {
	case DMX_PCR_ONLY_CHANNEL:
		flt.pesType = DMX_PES_PCR;
		break;
	case DMX_AUDIO_CHANNEL:
		flt.pesType = DMX_PES_AUDIO;
		break;
	case DMX_VIDEO_CHANNEL:
		flt.pesType = DMX_PES_VIDEO;
		break;
	case DMX_PES_CHANNEL:
		flt.unloader.unloader_type = UNLOADER_TYPE_PAYLOAD;
		flt.unloader.threshold     = 64;
		flt.pesType = DMX_PES_OTHER;
		flt.output  = OUT_MEMORY;
	default:
		flt.pesType = DMX_PES_OTHER;
	}

	return (ioctl(fd, DEMUX_FILTER_PES_SET, &flt) >= 0);
}

void cDemux::SetSyncMode(AVSYNC_TYPE /*mode*/)
{
	fprintf(stderr, "cDemux::%s #%d\n", __FUNCTION__, num);
}

void *cDemux::getBuffer()
{
	fprintf(stderr, "cDemux::%s #%d\n", __FUNCTION__, num);
	return NULL;
}

void *cDemux::getChannel()
{
	fprintf(stderr, "cDemux::%s #%d\n", __FUNCTION__, num);
	return NULL;
}

#if 0
const DMX_CHANNEL_TYPE cDemux::getChannelType(void)
{
	printf("%s:%s (type=%s)\n", FILENAME, __FUNCTION__, DMX_T[type]);
	
	return type; 
}
#endif

void cDemux::addPid(unsigned short Pid)
{
	char devname[32];
	int pfd;
	int ret;
	struct demux_pes_para p;
	if (dmx_type != DMX_TP_CHANNEL)
	{
		fprintf(stderr, "cDemux::%s pes_type!=DMX_TP_CHANNEL (%s) not implemented yet! pid=%hx\n", __FUNCTION__, DMX_T[dmx_type], Pid);
		return;
	}
	if (fd == -1)
		fprintf(stderr, "cDemux::%s bucketfd not yet opened? pid=%hx\n", __FUNCTION__, Pid);
	sprintf(devname, "%s%d", "/dev/" DEVICE_NAME_DEMUX, num);
	pfd = open(devname, O_RDWR);
	if (pfd < 0)
	{
		fprintf(stderr, "cDemux::%s #%d Pid = %hx open failed (%m)\n", __FUNCTION__, num, Pid);
		return;
	}
	fprintf(stderr, "cDemux::%s #%d Pid = %hx pfd = %d\n", __FUNCTION__, num, Pid, pfd);

	p.pid = Pid;
	p.pesType = DMX_PES_OTHER;
	p.output  = OUT_NOTHING;
	p.flags   = 0;
	p.unloader.unloader_type = UNLOADER_TYPE_BUCKET;
	p.unloader.threshold     = 128;

	ioctl(pfd, DEMUX_SELECT_SOURCE, INPUT_FROM_CHANNEL0);
	ret = ioctl(pfd, DEMUX_SET_BUFFER_SIZE, 0x10000); // 64k
	if (ret == -1)
		perror("DEMUX_SET_BUFFER_SIZE");
	else
	{
		ret = ioctl(pfd, DEMUX_FILTER_PES_SET, &p);
		if (ret == -1)
			perror("DEMUX_FILTER_PES_SET");
	}
	if (ret != -1)
		/* success! */
		pesfds.push_back(pfd);
	else
		/* error! */
		close(pfd);
	return;
}

void cDemux::getSTC(int64_t * STC)
{
	fprintf(stderr, "cDemux::%s #%d\n", __FUNCTION__, num);
	*STC = (int64_t)0;
}

#if 0
		void getSTC(int64_t * STC);
		//
		cDemux(int num = 0);
		~cDemux();
#endif
