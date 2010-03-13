#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "playback_td.h"
#include "dmx_td.h"
#include "audio_td.h"
#include "video_td.h"

#include <tddevices.h>
#define DVR	"/dev/" DEVICE_NAME_PVR

#define INFO(fmt, args...) fprintf(stderr, "[cPlayback:%s:%d] " fmt, __FUNCTION__, __LINE__, ##args)
#if 0	// change for verbose debug output
#define DBG INFO
#else
#define DBG(args...)
#endif
static int mp_syncPES(uint8_t *, int);
static inline uint16_t get_pid(uint8_t *buf);
static void *start_playthread(void *c);

extern cDemux *videoDemux;
extern cDemux *audioDemux;
extern cVideo *videoDecoder;
extern cAudio *audioDecoder;

cPlayback::cPlayback(int)
{
	INFO("\n");
	thread_started = false;
	inbuf = NULL;
	pesbuf = NULL;
	filelist.clear();
	curr_fileno = -1;
	in_fd = -1;
}

cPlayback::~cPlayback()
{
	INFO("\n");
	Close();
}


bool cPlayback::Open(playmode_t mode)
{
	static const char *PMODE[] = {
		"PLAYMODE_TS",
		"PLAYMODE_FILE"
	};

	INFO("PlayMode = %s\n", PMODE[mode]);
	thread_started = false;
	playMode = mode;
	filetype = FILETYPE_TS;
	numpida = 0;
	memset(&apids, 0, sizeof(apids));
	memset(&ac3flags, 0, sizeof(ac3flags));
	memset(&cc, 0, 256);
	return true;
}

//Used by Fileplay
void cPlayback::Close(void)
{
	INFO("\n");
	playstate = STATE_STOP;
	if (thread_started)
	{
		INFO("before pthread_join\n");
		pthread_join(thread, NULL);
	}
	thread_started = false;
	INFO("after pthread_join\n");
	mf_close();
	filelist.clear();

	if (inbuf)
		free(inbuf);
	inbuf = NULL;
	if (pesbuf)
		free(pesbuf);
	pesbuf = NULL;
	//Stop();
}

bool cPlayback::Start(char *filename, unsigned short vp, int vtype, unsigned short ap, bool _ac3)
{
	struct stat s;
	off_t r;
	vpid = vp;
	apid = ap;
	ac3 = _ac3;
	INFO("name = '%s' vpid 0x%04hx vtype %d apid 0x%04hx ac3 %d filelist.size: %u\n",
		filename, vpid, vtype, apid, ac3, filelist.size());
	if (!filelist.empty())
	{
		INFO("filelist not empty?\n");
		return false;
	}
	if (stat(filename, &s))
	{
		INFO("filename does not exist? (%m)\n");
		return false;
	}
	if (!inbuf)
		inbuf = (uint8_t *)malloc(INBUF_SIZE); /* 256 k */
	if (!inbuf)
	{
		INFO("allocating input buffer failed (%m)\n");
		return false;
	}
	if (!pesbuf)
		pesbuf = (uint8_t *)malloc(INBUF_SIZE / 2); /* 128 k */
	if (!pesbuf)
	{
		INFO("allocating PES buffer failed (%m)\n");
		return false;
	}
	filelist_t file;
	file.Name = std::string(filename);
	file.Size = s.st_size;
	filelist.push_back(file);
	filelist_auto_add();
	if (mf_open(0) < 0)
		return false;

	curr_pos = 0;
	inbuf_pos = 0;
	inbuf_sync = 0;
	r = mf_getsize();

	if (r > INBUF_SIZE)
	{
		if (mp_seekSync(r - INBUF_SIZE) < 0)
			return false;
		inbuf_read();	/* assume that we fill the buffer with one read() */
		for (r = (inbuf_pos / 188) * 188; r > 0; r -= 188)
		{
			pts_end = get_pts(inbuf + r, false);
			if (pts_end > -1)
				break;
		}
	}
	else
		pts_end = -1; /* unknown */

	if (mp_seekSync(0) < 0)
		return false;

	pesbuf_pos = 0;
	inbuf_pos = 0;
	inbuf_sync = 0;
	while (inbuf_pos < INBUF_SIZE / 2 && inbuf_read() > 0) {};
	for (r = 0; r < inbuf_pos - 188; r += 188)
	{
		pts_start = get_pts(inbuf + r, false);
		if (pts_start > -1)
			break;
	}
	pts_curr = pts_start;

	playstate = STATE_PLAY;
	if (pthread_create(&thread, 0, start_playthread, this) != 0)
		INFO("pthread_create failed\n");
	return true;
}

static void *start_playthread(void *c)
{
	cPlayback *obj = (cPlayback *)c;
	obj->playthread();
	return NULL;
}

void cPlayback::playthread(void)
{
	thread_started = true;
	int dmxfd = audioDemux->getFD();
	int dvrfd = open(DVR, O_WRONLY);
	int ret, towrite;
	if (dvrfd < 0)
	{
		INFO("open tdpvr failed: %m\n");
		pthread_exit(NULL);
	}

	ioctl(dmxfd, DEMUX_SELECT_SOURCE, INPUT_FROM_PVR);
	if (ac3)
		audioDecoder->SetStreamType(AUDIO_FMT_DOLBY_DIGITAL);
	else
		audioDecoder->SetStreamType(AUDIO_FMT_MPEG);

	audioDemux->pesFilter(apid);
	videoDemux->pesFilter(vpid);

	audioDemux->Start();
	videoDemux->Start();

	videoDecoder->setBlank(1);
	videoDecoder->Start();
	audioDecoder->Start();

	while(playstate != STATE_STOP)
	{
		if (inbuf_read() < 0)
			break;

		if (playback_speed == 0)
		{
			usleep(1);
			continue;
		}
		towrite = inbuf_pos / 188 * 188; /* TODO: smaller chunks? */
		if (towrite == 0)
			continue;
 retry:
		ret = write(dvrfd, inbuf, towrite);
		if (ret < 0)
		{
			if (errno == EAGAIN && playstate != STATE_STOP)
				goto retry;
			INFO("write dvr failed: %m\n");
			break;
		}
		memmove(inbuf, inbuf + ret, inbuf_pos - ret);
		inbuf_pos -= ret;
	}
	ioctl(dmxfd, DEMUX_SELECT_SOURCE, INPUT_FROM_CHANNEL0);
	audioDemux->Stop();
	videoDemux->Stop();
	audioDecoder->Stop();
	videoDecoder->Stop();
	close(dvrfd);
	pthread_exit(NULL);
}

bool cPlayback::SetAPid(unsigned short pid, bool _ac3)
{
	INFO("pid: 0x%04hx ac3: %d\n", pid, _ac3);
	apid = pid;
	ac3 = _ac3;

	audioDemux->Stop();
	audioDecoder->Stop();
	videoDemux->Stop();
	videoDecoder->Stop(false);

	if (ac3)
		audioDecoder->SetStreamType(AUDIO_FMT_DOLBY_DIGITAL);
	else
		audioDecoder->SetStreamType(AUDIO_FMT_MPEG);
	audioDemux->pesFilter(apid);

	videoDemux->Start();
	audioDemux->Start();
	audioDecoder->Start();
	videoDecoder->Start();
	return true;
}

bool cPlayback::SetSpeed(int speed)
{
	INFO("speed = %d\n", speed);
	if (speed != 0 && playback_speed == 0)
	{
		videoDemux->Stop();
		videoDemux->Start();
		audioDemux->Start();
		audioDecoder->Start();
		videoDecoder->Start();
	}
	playback_speed = speed;
	if (playback_speed == 0)
	{
		audioDecoder->Stop();
		audioDemux->Stop();
		videoDecoder->Stop(false);
	}
	return true;
}

bool cPlayback::GetSpeed(int &speed) const
{
	DBG("\n");
	speed = playback_speed;
	return true;
}

// in milliseconds
bool cPlayback::GetPosition(int &position, int &duration)
{
	int64_t tmppts;
	DBG("\n");
	if (pts_end != -1 && pts_start > pts_end) /* should trigger only once ;) */
		pts_end += 0x200000000ULL;

	if (pts_curr != -1 && pts_curr < pts_start)
		tmppts = pts_curr + 0x200000000ULL - pts_start;
	else
		tmppts = pts_curr - pts_start;
	position = tmppts / 90;
	duration = (pts_end - pts_start) / 90;
	return true;
}

bool cPlayback::SetPosition(int position, bool absolute)
{
	INFO("pos = %d abs = %d\n", position, absolute);
	return true;
}

void cPlayback::FindAllPids(uint16_t *_apids, unsigned short *_ac3flags, uint16_t *_numpida, std::string *language)
{
	INFO("\n");
	_apids = apids;
	_ac3flags = ac3flags;
	*_numpida = numpida;
	language = alang;
}

bool cPlayback::filelist_auto_add()
{
	if (filelist.size() != 1)
		return false;

	const char *filename = filelist[0].Name.c_str();
	char *ext;
	ext = strrchr(filename, '.');	// FOO-xxx-2007-12-31.001.ts <- the dot before "ts"
					// 001.vdr <- the dot before "vdr"
	// check if there is something to do...
	if (! ext)
		return false;
	if (!((ext - 7 >= filename && !strcmp(ext, ".ts") && *(ext - 4) == '.') ||
	      (ext - 4 >= filename && !strcmp(ext, ".vdr"))))
		return false;

	int num = 0;
	struct stat s;
	size_t numpos = strlen(filename) - strlen(ext) - 3;
	sscanf(filename + numpos, "%d", &num);
	do {
		num++;
		char nextfile[strlen(filename) + 1]; /* todo: use fixed buffer? */
		memcpy(nextfile, filename, numpos);
		sprintf(nextfile + numpos, "%03d%s", num, ext);
		if (stat(nextfile, &s))
			break; // file does not exist
		filelist_t file;
		file.Name = std::string(nextfile);
		file.Size = s.st_size;
		INFO("auto-adding '%s' to playlist\n", nextfile);
		filelist.push_back(file);
	} while (true && num < 999);

	return (filelist.size() > 1);
}

/* the mf_* functions are wrappers for multiple-file I/O */
int cPlayback::mf_open(int fileno)
{
	if (filelist.empty())
		return -1;

	if (fileno >= (int)filelist.size())
		return -1;

	mf_close();

	in_fd = open(filelist[fileno].Name.c_str(), O_RDONLY);
	if (in_fd != -1)
		curr_fileno = fileno;

	return in_fd;
}

int cPlayback::mf_close(void)
{
	int ret = 0;
INFO("in_fd = %d curr_fileno = %d\n", in_fd, curr_fileno);
	if (in_fd != -1)
		ret = close(in_fd);
	in_fd = curr_fileno = -1;

	return ret;
}

off_t cPlayback::mf_getsize(void)
{
	off_t ret = 0;
	for (unsigned int i = 0; i < filelist.size(); i++)
		ret += filelist[i].Size;
	return ret;
}

off_t cPlayback::mf_lseek(off_t pos)
{
	off_t offset = 0, lpos = pos, ret;
	unsigned int fileno;
	for (fileno = 0; fileno < filelist.size(); fileno++)
	{
		if (lpos < filelist[fileno].Size)
			break;
		offset += filelist[fileno].Size;
		lpos   -= filelist[fileno].Size;
	}
	if (fileno == filelist.size())
		return -2;	// EOF

	if ((int)fileno != curr_fileno)
	{
		INFO("old fileno: %d new fileno: %d, offset: %lld\n", curr_fileno, fileno, (long long)lpos);
		in_fd = mf_open(fileno);
		if (in_fd < 0)
		{
			INFO("cannot open file %d:%s (%m)\n", fileno, filelist[fileno].Name.c_str());
			return -1;
		}
	}

	ret = lseek(in_fd, lpos, SEEK_SET);
	if (ret < 0)
		return ret;

	return offset + ret;
}

/* gets the PTS at a specific file position from a PES
   ATTENTION! resets buf!  */
int64_t cPlayback::get_PES_PTS(uint8_t *buf, int len, bool last)
{
	int64_t pts = -1;
	int off, plen;
	uint8_t *p;

	off = mp_syncPES(buf, len);

	if (off < 0)
		return off;

	p = buf + off;
	while (off < len - 14 && (pts == -1 || last))
	{
		plen = ((p[4] << 8) | p[5]) + 6;

		switch(p[3])
		{
			int64_t tmppts;
			case 0xe0 ... 0xef:	// video!
				tmppts = get_pts(p, true);
				if (tmppts >= 0)
					pts = tmppts;
				break;
			case 0xbb:
			case 0xbe:
			case 0xbf:
			case 0xf0 ... 0xf3:
			case 0xff:
			case 0xc0 ... 0xcf:
			case 0xd0 ... 0xdf:
				break;
			case 0xb9:
			case 0xba:
			case 0xbc:
			default:
				plen = 1;
				break;
		}
		p += plen;
		off += plen;
	}
	return pts;
}

ssize_t cPlayback::inbuf_read()
{
	if (filetype == FILETYPE_UNKNOWN)
		return -1;
	if (filetype == FILETYPE_TS)
		return read_ts();
	/* FILETYPE_MPG or FILETYPE_VDR */
	return read_mpeg();
}

ssize_t cPlayback::read_ts()
{
	ssize_t toread, ret, sync, off;
	toread = INBUF_SIZE - inbuf_pos;
	bool retry = true;
	/* fprintf(stderr, "%s:%d curr_pos %lld, inbuf_pos: %ld, toread: %ld\n",
		__FUNCTION__, __LINE__, (long long)curr_pos, (long)inbuf_pos, (long)toread); */

	while(true)
	{
		ret = read(in_fd, inbuf + inbuf_pos, toread);
		if (ret == 0 && retry) /* EOF */
		{
			mf_lseek(curr_pos);
			retry = false;
			continue;
		}
		break;
	}
	if (ret < 0)
	{
		INFO("failed: %m\n");
		return ret;
	}
	if (ret == 0)
		return ret;
	inbuf_pos += ret;
	curr_pos += ret;

	sync = sync_ts(inbuf_sync);
	if (sync < 0)
	{
		INFO("cannot sync\n");
		return ret;
	}
	inbuf_sync = sync;
	/* check for A/V PIDs */
	uint16_t pid;
	int i, j;
	bool pid_new;
	int64_t pts;
	// fprintf(stderr, "inbuf_pos: %ld - sync: %ld\n", (long)inbuf_pos, (long)sync);
	int synccnt = 0;
	for (i = 0; i < inbuf_pos - sync - 13;) {
		uint8_t *buf = inbuf + sync + i;
		if (*buf != 0x47)
		{
			synccnt++;
			i++;
			continue;
		}
		if (synccnt)
			INFO("TS went out of sync %d\n", synccnt);
		synccnt = 0;
		if (!(buf[1] & 0x40))	/* PUSI */
		{
			i += 188;
			continue;
		}
		off = 0;
		if (buf[3] & 0x20)	/* adaptation field? */
			off = buf[4] + 1;
		pid = get_pid(buf + 1);
		pid_new = true;
		/* PES signature is at buf + 4, streamtype is after 00 00 01 */
		switch (buf[4 + 3 + off])
		{
		case 0xe0 ... 0xef:	/* video stream */
			if (vpid == 0)
				vpid = pid;
			pts = get_pts(buf + 4 + off, true);
			if (pts < 0)
				break;
			pts_curr = pts;
			if (pts_start < 0)
				pts_start = pts;
			break;
		case 0xbd:		/* private stream 1 - ac3 */
		case 0xc0 ... 0xdf:	/* audio stream */
			if (numpida > 9)
				break;
			for (j = 0; j < numpida; j++) {
				if (apids[j] == pid)
				{
					i += 188;
					continue;
				}
			}
			if (buf[7 + off] == 0xbd)
			{
				if (buf[12 + off] == 0x24)	/* 0x24 == TTX */
					break;
				ac3flags[numpida] = 1;
			}
			else
				ac3flags[numpida] = 0;
			apids[numpida] = pid;
			numpida++;
			break;
		}
		i += 188;
	}

	// fprintf(stderr, "%s:%d ret %ld\n", __FUNCTION__, __LINE__, (long long)ret);
	return ret;
}

ssize_t cPlayback::read_mpeg()
{
	ssize_t toread, ret, sync;
	toread = INBUF_SIZE / 2 - pesbuf_pos;
	bool retry = true;

	while(true)
	{
		ret = read(in_fd, pesbuf + pesbuf_pos, toread);
		if (ret == 0 && retry) /* EOF */
		{
			mf_lseek(curr_pos);
			retry = false;
			continue;
		}
		break;
	}
	if (ret < 0)
	{
		INFO("failed: %m\n");
		return ret;
	}
	pesbuf_pos += ret;
	curr_pos += ret;

	int i;
	int count = 0;
	uint16_t pid = 0;
	while (count < pesbuf_pos - 10)
	{
		sync = mp_syncPES(pesbuf + count, pesbuf_pos - count - 10);
		if (sync < 0)
		{
			INFO("cannot sync\n");
			break;
		}
		if (sync)
			INFO("needed sync\n");
		count += sync;
		uint8_t *ppes = pesbuf + count;
		int av = 0; // 1 = video, 2 = audio
		int64_t pts;
		switch(ppes[3])
		{
			case 0xba:
				// fprintf(stderr, "pack start code, 0x%02x\n", ppes[4]);
				if ((ppes[4] & 0x3) == 1) // ??
				{
					//type = 1; // mpeg1
					count += 12;
				}
				else
					count += 14;
				continue;
				break;
			case 0xbd: // AC3
			{
				int off = ppes[8] + 8 + 1; // ppes[8] is often 0
				if (count + off >= pesbuf_pos)
					break;
				int subid = ppes[off];
				// if (offset == 0x24 && subid == 0x10 ) // TTX?
				if (subid < 0x80 || subid > 0x87)
					break;
				DBG("AC3: ofs 0x%02x subid 0x%02x\n", off, subid);
				//subid -= 0x60; // normalize to 32...39 (hex 0x20..0x27)

				if (numpida > 9)
					break;
				bool found = false;
				for (i = 0; i < numpida; i++) {
					if (apids[i] == subid)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					apids[numpida] = subid;
					ac3flags[numpida] = 1;
					numpida++;
					INFO("found aid: %02x\n", subid);
				}
				pid = subid;
				av = 2;
				break;
			}
			case 0xbb:
			case 0xbe:
			case 0xbf:
			case 0xf0 ... 0xf3:
			case 0xff:
				//skip = (ppes[4] << 8 | ppes[5]) + 6;
				//DBG("0x%02x header, skip = %d\n", ppes[3], skip);
				break;
			case 0xc0 ... 0xcf:
			case 0xd0 ... 0xdf:
			{
				// fprintf(stderr, "audio stream 0x%02x\n", ppes[3]);
				int id = ppes[3]; // - 0xC0; // normalize to 0...31 (hex 0x0...0x1f)
				bool found = false;
				for (i = 0; i < numpida; i++) {
					if (apids[i] == id)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					apids[numpida] = id;
					ac3flags[numpida] = 0;
					numpida++;
					INFO("found aid: %02x\n", id);
				}
				pid = id;
				av = 2;
				break;
			}
			case 0xe0 ... 0xef:
				// fprintf(stderr, "video stream 0x%02x, %02x %02x \n", ppes[3], ppes[4], ppes[5]);
				pid = 0x40;
				av = 1;
				pts = get_pts(ppes, true);
				if (pts < 0)
					break;
				pts_curr = pts;
				if (pts_start < 0)
					pts_start = pts;
				break;
			case 0xb9:
			case 0xbc:
				DBG("%s\n", (ppes[3] == 0xb9) ? "program_end_code" : "program_stream_map");
				//resync = true;
				// fallthrough. TODO: implement properly.
			default:
				//if (! resync)
				//	DBG("Unknown stream id: 0x%X.\n", ppes[3]);
				count++;
				continue;
				break;
		}

		int pesPacketLen = ((ppes[4] << 8) | ppes[5]) + 6;
		if (count + pesPacketLen >= pesbuf_pos)
		{
			INFO("buffer len: %d, pesPacketLen: %d :-(\n", pesbuf_pos - count, pesPacketLen);
			memmove(pesbuf, ppes, pesbuf_pos - count);
			pesbuf_pos -= count;
			break;
		}

		if (av)
		{
			int tsPacksCount = pesPacketLen / 184;
			int rest = pesPacketLen % 184;

			// divide PES packet into small TS packets
			uint8_t pusi = 0x40;
			int j;
			uint8_t *ts = inbuf + inbuf_pos;
			for (j = 0; j < tsPacksCount; j++)
			{
				ts[0] = 0x47;				// SYNC Byte
				ts[1] = pusi;				// Set PUSI if first packet
				ts[2] = pid;				// PID (low)
				ts[3] = 0x10 | (cc[pid] & 0x0F);	// No adaptation field, payload only, continuity counter
				cc[pid]++;
				memcpy(ts + 4, ppes + j * 184, 184);
				pusi = 0x00;				// clear PUSI
				ts += 188;
				inbuf_pos += 188;
			}

			if (rest > 0)
			{
				ts[0] = 0x47;				// SYNC Byte
				ts[1] = pusi;				// Set PUSI or
				ts[2] = pid;				// PID (low)
				ts[3] = 0x30 | (cc[pid] & 0x0F);	// adaptation field, payload, continuity counter
				cc[pid]++;
				ts[4] = 183 - rest;
				if (ts[4] > 0)
				{
					ts[5] = 0x00;
					memset(ts + 6, 0xFF, ts[4] - 1);
				}
				memcpy(ts + 188 - rest, ppes + j * 184, rest);
				inbuf_pos += 188;
			}
		} //if (av)

		memmove(pesbuf, ppes + pesPacketLen, pesbuf_pos - count - pesPacketLen);
		pesbuf_pos -= count + pesPacketLen;
	}
	return ret;
}

//== seek to pos with sync to next proper TS packet ==
//== returns offset to start of TS packet or actual ==
//== pos on failure.                                ==
//====================================================
off_t cPlayback::mp_seekSync(off_t pos)
{
	off_t npos = pos;
	off_t ret;
	uint8_t pkt[188];

	ret = mf_lseek(npos);
	if (ret < 0)
		INFO("lseek ret < 0 (%m)\n");

	while (read(in_fd, pkt, 1) > 0)
	{
		//-- check every byte until sync word reached --
		npos++;
		if (*pkt == 0x47)
		{
			//-- if found double check for next sync word --
			if (read(in_fd, pkt, 188) == 188)
			{
				if(pkt[188-1] == 0x47)
				{
					ret = mf_lseek(npos - 1); // assume sync ok
					if (ret < 0)
						INFO("lseek ret < 0 (%m)\n");
					return ret;
				}
				else
				{
					ret = mf_lseek(npos); // oops, next pkt doesn't start with sync
					if (ret < 0)
						INFO("lseek ret < 0 (%m)\n");
				}
			}
		}

		//-- check probe limits --
		if (npos > (pos + 100 * 188))
			break;
	}

	//-- on error stay on actual position --
	return mf_lseek(pos);
}

int cPlayback::sync_ts(int off)
{
	uint8_t *p;
	int count;

	if (inbuf_pos - 188 < off)
		return -1;

	count = 0;
	p = inbuf + off;
	while (*p != 0x47 || *(p + 188) != 0x47)
	{
		count++;
		p++;
		if (count + off > inbuf_pos - 188)
			return -1;
	}
	return count;
}

/* get the pts value from a TS or PES packet
   pes == true selects PES mode. */
int64_t cPlayback::get_pts(uint8_t *p, bool pes)
{
	if (!pes)
	{
		const uint8_t *end = p + 188;
		if (p[0] != 0x47)
			return -1;
		if (!(p[1] & 0x40))
			return -1;
		if (get_pid(p + 1) != vpid)
			return -1;
		if (!(p[3] & 0x10))
			return -1;

		if (p[3] & 0x20)
			p += p[4] + 4 + 1;
		else
			p += 4;

		if (p + 13 > end)
			return -1;
		/* p is now pointing at the PES header. hopefully */
		if (p[0] || p[1] || (p[2] != 1))
			return -1;
	}

	if ((p[7] & 0x80) == 0) // packets with both pts, don't care for dts
	// if ((p[7] & 0xC0) != 0x80) // packets with only pts
	// if ((p[7] & 0xC0) != 0xC0) // packets with pts and dts
		return -1;
	if (p[8] < 5)
		return -1;
	if (!(p[9] & 0x20))
		return -1;

	int64_t pts =
		((p[ 9] & 0x0EULL) << 29) |
		((p[10] & 0xFFULL) << 22) |
		((p[11] & 0xFEULL) << 14) |
		((p[12] & 0xFFULL) << 7) |
		((p[13] & 0xFEULL) >> 1);

	//int msec = pts / 90;
	//INFO("time: %02d:%02d:%02d\n", msec / 3600000, (msec / 60000) % 60, (msec / 1000) % 60);
	return pts;
}

/* returns: 0 == was already synchronous, > 0 == is now synchronous, -1 == could not sync */
static int mp_syncPES(uint8_t *buf, int len)
{
	int ret = 0;
	while (ret < len - 3)
	{
		if (buf[ret + 2] != 0x01)
		{
			ret++;
			continue;
		}
		if (buf[ret + 1] != 0x00)
		{
			ret += 2;
			continue;
		}
		if (buf[ret] != 0x00)
		{
			ret += 3;
			continue;
		}
		return ret;
	}

	INFO("No valid PES signature found. %d Bytes deleted.\n", ret);
	return -1;
}

static inline uint16_t get_pid(uint8_t *buf)
{
	return (*buf & 0x1f) << 8 | *(buf + 1);
}

