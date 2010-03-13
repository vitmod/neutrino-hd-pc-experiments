#ifndef __RECORD_TD_H
#define __RECORD_TD_H

#include <pthread.h>
#include "dmx_td.h"

typedef enum {
	RECORD_RUNNING,
	RECORD_STOPPED,
	RECORD_FAILED_READ,	/* failed to read from DMX */
	RECORD_FAILED_OVERFLOW,	/* cannot write fast enough */
	RECORD_FAILED_FILE,	/* cannot write to file */
	RECORD_FAILED_MEMORY	/* out of memory */
} record_state_t;

class cRecord
{
	private:
		int file_fd;
		cDemux *dmx;
		pthread_t record_thread;
		bool record_thread_running;
		record_state_t exit_flag;
	public:
		cRecord(int num = 0);
		~cRecord();

		bool Open(int numpids);
		bool Start(int fd, unsigned short vpid, unsigned short *apids, int numpids);
		bool Stop(void);

		void RecordThread();
#if 0
		/* apparently unused */
		void Close(void);
		void RecordNotify(int Event, void *pData);
#endif
};
#endif
