/* Dagobert: Most parts taken from old neutrino stream2file.cpp */

/*
 * $Id: stream2file.cpp,v 1.32 2009/04/02 07:53:53 seife Exp $
 * 
 * streaming to file/disc
 * 
 * Copyright (C) 2004 Axel Buehning <diemade@tuxbox.org>,
 *                    thegoodguy <thegoodguy@berlios.de>
 *
 * based on code which is
 * Copyright (C) 2001 TripleDES
 * Copyright (C) 2000, 2001 Marcus Metzler <marcus@convergence.de>
 * Copyright (C) 2002 Andreas Oberritter <obi@tuxbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 */
#include <stdio.h>

#include "record_td.h"
#include "dmx_td.h"

#include <xp/xp_osd_user.h>
#include <driver/stream2file.h>

static const char * FILENAME = "record_td.cpp";

static cDemux *dmx = NULL;

static int sync_byte_offset(const unsigned char * buf, const unsigned int len) {
	unsigned int i;
	for (i = 0; i < len; i++)
		if (buf[i] == 0x47)
			return i;
	return -1;
}

void cRecord::FileThread()
{
	ringbuffer_data_t vec[2];
	size_t readsize;
	ssize_t written;

printf("%s:%s >\n", FILENAME, __FUNCTION__);
	ringbuffer_t * ringbuf = ringbuffer;
	while (exit_flag == STREAM2FILE_STATUS_RUNNING)
	{
		ringbuffer_get_read_vector(ringbuf, &(vec[0]));
		readsize = vec[0].len + vec[1].len;
		if (!readsize)
		{
			usleep(1000);
			continue;
		}

		while (1)
		{
			written = write(file_fd, vec[0].buf, vec[0].len);
			if (written < 0)
			{
				if (errno == EAGAIN)
				{
					usleep(1000);
					continue;
				}
				exit_flag = STREAM2FILE_STATUS_WRITE_FAILURE;
				perror("[stream2file]: error in write");
				break; /* will break out of both loops due to exit_flag */
			}

			ringbuffer_read_advance(ringbuf, written);
			if (vec[0].len == (size_t)written)
			{
				if (vec[1].len == 0)
					break;
				vec[0] = vec[1];
				vec[1].len = 0;
			}
			else
			{
				vec[0].len -= written;
				vec[0].buf += written;
			}
		}
		if (fdatasync(file_fd))
			perror("cRecord::FileThreadfdatasync");
	}
printf("%s:%s <\n", FILENAME, __FUNCTION__);
	pthread_exit(NULL);
}

/* helper function to call the cpp thread loop */
void* execute_file_thread(void *c)
{
	cRecord *obj=(cRecord*)c;
printf("%s:%s >\n", FILENAME, __FUNCTION__);
	obj->FileThread();
printf("%s:%s <\n", FILENAME, __FUNCTION__);
	return NULL;
}

void cRecord::DMXThread()
{
	pthread_t file_thread;
	ringbuffer_data_t vec[2];
	ssize_t todo = 0;
	ssize_t todo2;
	ssize_t r = 0;
#if 0
	ssize_t written;
	unsigned char buf[TS_SIZE];
	int offset = 0;
#endif
printf("%s:%s >\n", FILENAME, __FUNCTION__);

	ringbuffer_t * ringbuf = ringbuffer_create(1 << 20); /* 1MB */

	if (!ringbuf)
	{
		exit_flag = STREAM2FILE_STATUS_WRITE_OPEN_FAILURE;
		printf("[stream2file]: error allocating ringbuffer! (out of memory?)\n"); 
	}
	else
		fprintf(stderr, "[stream2file] allocated ringbuffer size: %ld\n", (long)ringbuffer_write_space(ringbuf));

	ringbuffer = ringbuf;

	if (pthread_create(&file_thread, 0, execute_file_thread, this) != 0)
	{
		exit_flag = STREAM2FILE_STATUS_WRITE_OPEN_FAILURE;
		printf("[stream2file]: error creating file_thread! (out of memory?)\n"); 
	}

	dmx->Start();
#if 0
	while (exit_flag == STREAM2FILE_STATUS_RUNNING)
	{
		r = dmx->Read(&(buf[0]), TS_SIZE);
		if (r < 0)
		{
			perror("stream2file read DMX");
			exit_flag = STREAM2FILE_STATUS_READ_FAILURE;
			break;
		}
		if (r > 0)
		{
			offset = sync_byte_offset(&(buf[0]), r);
			if (offset != -1)
				break;
		}
	}

	if (exit_flag == STREAM2FILE_STATUS_RUNNING)
	{
		written = ringbuffer_write(ringbuf, (char *)&(buf[offset]), r - offset);
		// TODO: Retry
		if (written != r - offset) {
			printf("PANIC: wrote less than requested to ringbuffer, written %d, requested %d\n", written, r - offset);
			exit_flag = STREAM2FILE_STATUS_BUFFER_OVERFLOW;
		}
		todo = IN_SIZE - (r - offset);
	}

	/* IN_SIZE > TS_SIZE => todo > 0 */
#endif
	todo = IN_SIZE;
	while (exit_flag == STREAM2FILE_STATUS_RUNNING)
	{
		ringbuffer_get_write_vector(ringbuf, &(vec[0]));
		//fprintf(stderr, "ringbuffer space: %u\n", vec[0].len + vec[1].len);
		todo2 = todo - vec[0].len;
		if (todo2 < 0)
			todo2 = 0;
		else
		{
			if (((size_t)todo2) > vec[1].len)
			{
				printf("cRecord::DMXThread: ringbuffer full. available %d, needed %d\n", vec[0].len + vec[1].len, todo + todo2);
				exit_flag = STREAM2FILE_STATUS_BUFFER_OVERFLOW;
				break;
			}
			todo = vec[0].len;
		}

		while (exit_flag == STREAM2FILE_STATUS_RUNNING)
		{
			r = dmx->Read((unsigned char *)vec[0].buf, todo, 100);
			if (r > 0)
			{
				ringbuffer_write_advance(ringbuf, r);
				if (todo == r)
				{
					if (todo2 == 0)
						break;
					todo = todo2;
					todo2 = 0;
					vec[0].buf = vec[1].buf;
				}
				else
				{
					vec[0].buf += r;
					todo -= r;
				}
			}
			if (r < 0 && errno != EAGAIN)
			{
				perror("cRecord::DMXThread read failed");
				exit_flag = STREAM2FILE_STATUS_READ_FAILURE;
				break;
			}
		}
		todo = IN_SIZE;
	}

	delete dmx;
	dmx = NULL;

	pthread_join(file_thread, NULL);

	if (ringbuf)
		ringbuffer_free(ringbuf);

#ifdef needed
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

printf("%s:%s <\n", FILENAME, __FUNCTION__);
	pthread_exit(NULL);
}


cRecord::cRecord(int num)
{
	printf("%s:%s num = %d\n", FILENAME, __FUNCTION__, num);
//fixme: what is the meaning of num? should it be the demuxer number?   
}

cRecord::~cRecord()
{
	printf("%s:%s\n", FILENAME, __FUNCTION__);
}

bool cRecord::Open(int numpids)
{
	printf("%s:%s numpids %d\n", FILENAME, __FUNCTION__, numpids);

	exit_flag = STREAM2FILE_STATUS_IDLE;

	return true;
}

void cRecord::Close(void)
{
	printf("%s:%s\n", FILENAME, __FUNCTION__);
}

/* helper function to call the cpp thread loop */
void *execute_demux_thread(void *c)
{
	cRecord *obj=(cRecord*)c;
printf("%s:%s>\n", FILENAME, __FUNCTION__);
	obj->DMXThread();
printf("%s:%s<\n", FILENAME, __FUNCTION__);
	return NULL;
}

bool cRecord::Start(int fd, unsigned short vpid, unsigned short * apids, int numpids)
{
	printf("%s:%s: fd %d, vpid 0x%02x\n", FILENAME, __FUNCTION__, fd, vpid);

	if (!dmx)
		dmx = new cDemux(2); /* streamts and streaminfo use demux 1 */

	dmx->Open(DMX_TP_CHANNEL, NULL, 0);
	dmx->pesFilter(vpid);

	for (int i = 0; i < numpids; i++)
		dmx->addPid(apids[i]);

	file_fd = fd;
	exit_flag = STREAM2FILE_STATUS_RUNNING;

	if (pthread_create(&demux_thread[0], 0, execute_demux_thread, this) != 0)
	{
		exit_flag = STREAM2FILE_STATUS_WRITE_OPEN_FAILURE;
		printf("[stream2file]: error creating thread! (out of memory?)\n");
		return false; 
	}
	time(&record_start_time);

	printf("record start time: %lu \n", record_start_time);
	return true;
}

bool cRecord::Stop(void)
{
	printf("%s:%s\n", FILENAME, __FUNCTION__);

	if (exit_flag == STREAM2FILE_STATUS_RUNNING)
	{
		time(&record_end_time);
		printf("record time: %lu \n",record_end_time-record_start_time);

		exit_flag = STREAM2FILE_STATUS_IDLE;
		if (file_fd != -1)
			close(file_fd);
		else
			fprintf(stderr, "%s:%s file_fd not open??\n", FILENAME, __FUNCTION__);
		file_fd = -1;
		return true;
	}

	if (file_fd != -1)
		close(file_fd);
	else
		fprintf(stderr, "%s:%s file_fd not open??\n", FILENAME, __FUNCTION__);
	file_fd = -1;
	return false;
}

void cRecord::RecordNotify(int Event, void * /*pData*/)
{
	printf("%s:%s event %d\n", FILENAME, __FUNCTION__, Event); 
//fixme: dont know what this is for? maybe we must connect to the event queues? not sure...
}
