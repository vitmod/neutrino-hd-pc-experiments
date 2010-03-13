#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/types.h>
#include "record_td.h"

#if 0
#include <libeventserver/eventserver.h>
#include <neutrinoMessages.h>
#endif

#define INFO(fmt, args...) fprintf(stderr, "[cRecord:%s:%d] " fmt, __FUNCTION__, __LINE__, ##args)
#if 0	// change for verbose debug output
#define DBG INFO
#else
#define DBG(args...)
#endif

#if 0
static int sync_byte_offset(const unsigned char * buf, const unsigned int len) {
	unsigned int i;
	for (i = 0; i < len; i++)
		if (buf[i] == 0x47)
			return i;
	return -1;
}
#endif

/* helper function to call the cpp thread loop */
void *execute_record_thread(void *c)
{
	cRecord *obj = (cRecord *)c;
	obj->RecordThread();
	return NULL;
}

cRecord::cRecord(int /*num*/)
{
	INFO("\n");
	dmx = NULL;
	record_thread_running = false;
	file_fd = -1;
	exit_flag = RECORD_STOPPED;
}

cRecord::~cRecord()
{
	INFO("calling ::Stop()\n");
	Stop();
	INFO("end\n");
}

bool cRecord::Open(int /*numpids*/)
{
	INFO("\n");
	exit_flag = RECORD_STOPPED;
	return true;
}

#if 0
// unused
void cRecord::Close(void)
{
	INFO("\n");
}
#endif

bool cRecord::Start(int fd, unsigned short vpid, unsigned short * apids, int numpids)
{
	INFO("fd %d, vpid 0x%02x\n", fd, vpid);
	int i;

	if (!dmx)
		dmx = new cDemux(2); /* streamts and streaminfo use demux 1 */

	dmx->Open(DMX_TP_CHANNEL, NULL, 0);
	dmx->pesFilter(vpid);

	for (i = 0; i < numpids; i++)
		dmx->addPid(apids[i]);

	file_fd = fd;
	exit_flag = RECORD_RUNNING;

	i = pthread_create(&record_thread, 0, execute_record_thread, this);
	if (i != 0)
	{
		exit_flag = RECORD_FAILED_READ;
		errno = i;
		INFO("error creating thread! (%m)\n");
		delete dmx;
		dmx = NULL;
		return false;
	}
	record_thread_running = true;
	return true;
}

bool cRecord::Stop(void)
{
	INFO("\n");

	if (exit_flag != RECORD_RUNNING)
		INFO("status not RUNNING? (%d)\n", exit_flag);

	exit_flag = RECORD_STOPPED;
	if (record_thread_running)
		pthread_join(record_thread, NULL);
	record_thread_running = false;

	/* We should probably do that from the destructor... */
	if (!dmx)
		INFO("dmx == NULL?\n");
	else
		delete dmx;
	dmx = NULL;

	if (file_fd != -1)
		close(file_fd);
	else
		INFO("file_fd not open??\n");
	file_fd = -1;
	return true;
}

void cRecord::RecordThread()
{
	INFO("begin\n");
#define BUFSIZE (1 << 19) /* 512 kB */
	ssize_t r = 0;
	int buf_pos = 0;
	uint8_t *buf;
	buf = (uint8_t *)malloc(BUFSIZE);

	if (!buf)
	{
		exit_flag = RECORD_FAILED_MEMORY;
		INFO("unable to allocate buffer! (out of memory)\n");
	}

	dmx->Start();
	while (exit_flag == RECORD_RUNNING)
	{
		if (buf_pos < BUFSIZE)
		{
			r = dmx->Read(buf + buf_pos, BUFSIZE - 1 - buf_pos, 100);
			DBG("buf_pos %6d r %6d / %6d\n", buf_pos, (int)r, BUFSIZE - 1 - buf_pos);
			if (r < 0)
			{
				if (errno != EAGAIN)
				{
					INFO("read failed: %m\n");
					exit_flag = RECORD_FAILED_READ;
					break;
				}
				INFO("EAGAIN\n");
			}
			else
				buf_pos += r;
		}
		else
			INFO("buffer full! Overflow?\n");
		if (buf_pos > (BUFSIZE / 3)) /* start writeout */
		{
			size_t towrite = BUFSIZE / 2;
			if (buf_pos < BUFSIZE / 2)
				towrite = buf_pos;
			r = write(file_fd, buf, towrite);
			if (r < 0)
			{
				exit_flag = RECORD_FAILED_FILE;
				INFO("write error: %m\n");
				break;
			}
			buf_pos -= r;
			memmove(buf, buf + r, buf_pos);
			DBG("buf_pos %6d w %6d / %6d\n", buf_pos, (int)r, (int)towrite);
#if 0
			if (fdatasync(file_fd))
				perror("cRecord::FileThread() fdatasync");
#endif
		}
	}
	dmx->Stop();
	while (buf_pos > 0) /* write out the unwritten buffer content */
	{
		r = write(file_fd, buf, buf_pos);
		if (r < 0)
		{
			exit_flag = RECORD_FAILED_FILE;
			INFO("write error: %m\n");
			break;
		}
		buf_pos -= r;
		memmove(buf, buf + r, buf_pos);
	}
	free(buf);

#if 0
//fixme: do we need it?
	CEventServer eventServer;
	eventServer.registerEvent2(NeutrinoMessages::EVT_RECORDING_ENDED, CEventServer::INITID_NEUTRINO, "/tmp/neutrino.sock");
	stream2file_status2_t s;
	s.status = exit_flag;
	strncpy(s.filename,basename(myfilename),512);
	s.filename[511] = '\0';
	strncpy(s.dir,dirname(myfilename),100);
	s.dir[99] = '\0';
	eventServer.sendEvent(NeutrinoMessages::EVT_RECORDING_ENDED, CEventServer::INITID_NEUTRINO, &s, sizeof(s));
	printf("[stream2file]: pthreads exit code: %i, dir: '%s', filename: '%s' myfilename: '%s'\n", exit_flag, s.dir, s.filename, myfilename);
#endif

	INFO("end");
	pthread_exit(NULL);
}

