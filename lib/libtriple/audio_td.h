/* public header file */

#ifndef _AUDIO_TD_H_
#define _AUDIO_TD_H_

#include <hardware/aud/aud_inf.h>

typedef enum
{
  AUDIO_SYNC_WITH_PTS,
  AUDIO_NO_SYNC,
  AUDIO_SYNC_AUDIO_MASTER
} AUDIO_SYNC_MODE;

typedef enum
{
   AUDIO_FMT_AUTO = 0,
   AUDIO_FMT_MPEG,
   AUDIO_FMT_MP3,
   AUDIO_FMT_DOLBY_DIGITAL,
   AUDIO_FMT_BASIC = AUDIO_FMT_DOLBY_DIGITAL,
   AUDIO_FMT_AAC,
   AUDIO_FMT_AAC_PLUS,
   AUDIO_FMT_DD_PLUS,
   AUDIO_FMT_DTS,
   AUDIO_FMT_AVS,
   AUDIO_FMT_MLP,
   AUDIO_FMT_WMA,
   AUDIO_FMT_ADVANCED = AUDIO_FMT_MLP
} AUDIO_FORMAT;

class cAudio
{
	private:
		int fd;
		bool Muted;

		AUDIO_FORMAT	StreamType;
		AUDIO_SYNC_MODE    SyncMode;
		bool started;

		int volume;

		void openDevice(void);
		void closeDevice(void);

		int do_mute(bool enable);
	public:
		/* construct & destruct */
		cAudio(void *, void *, void *);
		~cAudio(void);

		void *GetHandle() { return NULL; };
		/* shut up */
		int mute(void) { return do_mute(true); };
		int unmute(void) { return do_mute(false); };

		/* volume, min = 0, max = 255 */
		int setVolume(unsigned int left, unsigned int right);
		int getVolume(void) { return volume;}

		/* start and stop audio */
		int Start(void);
		int Stop(void);
		bool Pause(bool Pcm = true);
		void SetStreamType(AUDIO_FORMAT type) { StreamType = type; };
#define AVSYNC_TYPE int
		void SetSyncMode(AVSYNC_TYPE Mode);

		/* select channels */
		int setChannel(int channel);
		int PrepareClipPlay(int uNoOfChannels, int uSampleRate, int uBitsPerSample, int bLittleEndian);
		int WriteClip(unsigned char * buffer, int size);
		int StopClip();
		void getAudioInfo(int &type, int &layer, int& freq, int &bitrate, int &mode);
		void SetSRS(int iq_enable, int nmgr_enable, int iq_mode, int iq_level);
		bool IsHdmiDDSupported() { return false; };
		void SetHdmiDD(bool) { return; };
		void SetSpdifDD(bool enable);
		void ScheduleMute(bool On);
		void EnableAnalogOut(bool enable);
};

#endif

