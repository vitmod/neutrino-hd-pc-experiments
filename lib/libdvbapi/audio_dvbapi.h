/* public header file */

#ifndef _AUDIO_CS_H_
#define _AUDIO_CS_H_

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

#ifndef CS_AUDIO_PDATA
#define CS_AUDIO_PDATA void
#endif

#include "cs_types.h"

class cAudio
{
	public:
		/* construct & destruct */
		cAudio(void * hBuffer, void * encHD, void * encSD);
		~cAudio(void);

		void * GetHandle();
		void * GetDSP();
		void HandleAudioMessage(int Event, void *pData);
		void HandlePcmMessage(int Event, void *pData);
		/* shut up */
		int mute(void);
		int unmute(void);
		int SetMute(int enable);

		/* bypass audio to external decoder */
		int enableBypass(void);
		int disableBypass(void);

		/* volume, min = 0, max = 255 */
		int setVolume(unsigned int left, unsigned int right);
		int getVolume(void);

		/* start and stop audio */
		int Start(void);
		int Stop(void);
		bool Pause(bool Pcm = true);
		bool Resume(bool Pcm = true);
		void SetStreamType(AUDIO_FORMAT type);
		AUDIO_FORMAT GetStreamType(void);
		bool ReceivedAudioDelay(void);
		void SetReceivedAudioDelay(bool set = false);
		unsigned int GetAudioDelay(void);
		void SetSyncMode(AVSYNC_TYPE Mode);

		/* stream source */
		int getSource(void);
		int setSource(int source);
		int Flush(void);

		/* select channels */
		int setChannel(int channel);
		int getChannel(void);
		int PrepareClipPlay(int uNoOfChannels, int uSampleRate, int uBitsPerSample, int bLittleEndian);
		int WriteClip(unsigned char * buffer, int size);
		int StopClip();
		void getAudioInfo(int &type, int &layer, int& freq, int &bitrate, int &mode);
		void SetSRS(int iq_enable, int nmgr_enable, int iq_mode, int iq_level);
		bool IsHdmiDDSupported();
		void SetHdmiDD(bool enable);
		void SetSpdifDD(bool enable);
		void ScheduleMute(bool On);
		void EnableAnalogOut(bool enable);
};

#endif

