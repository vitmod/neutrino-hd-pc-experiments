/*
	LCD-Daemon  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/



	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __vfd__
#define __vfd__

#include "driver/file.h"

//#define VFD_UPDATE // whatever that is
//#define LCD_UPDATE // whatever that is

typedef enum
{
	VFD_ICON_BAR8       = 0x00000004,
	VFD_ICON_BAR7       = 0x00000008,
	VFD_ICON_BAR6       = 0x00000010,
	VFD_ICON_BAR5       = 0x00000020,
	VFD_ICON_BAR4       = 0x00000040,
	VFD_ICON_BAR3       = 0x00000080,
	VFD_ICON_BAR2       = 0x00000100,
	VFD_ICON_BAR1       = 0x00000200,
	VFD_ICON_FRAME      = 0x00000400,
	VFD_ICON_HDD        = 0x00000800,
	VFD_ICON_MUTE       = 0x00001000,
	VFD_ICON_DOLBY      = 0x00002000,
	VFD_ICON_POWER      = 0x00004000,
	VFD_ICON_TIMESHIFT  = 0x00008000,
	VFD_ICON_SIGNAL     = 0x00010000,
	VFD_ICON_TV         = 0x00020000,
	VFD_ICON_RADIO      = 0x00040000,
	VFD_ICON_HD         = 0x01000001,
	VFD_ICON_1080P      = 0x02000001,
	VFD_ICON_1080I      = 0x03000001,
	VFD_ICON_720P       = 0x04000001,
	VFD_ICON_480P       = 0x05000001,
	VFD_ICON_480I       = 0x06000001,
	VFD_ICON_USB        = 0x07000001,
	VFD_ICON_MP3        = 0x08000001,
	VFD_ICON_PLAY       = 0x09000001,
	VFD_ICON_COL1       = 0x09000002,
	VFD_ICON_PAUSE      = 0x0A000001,
	VFD_ICON_CAM1       = 0x0B000001,
	VFD_ICON_COL2       = 0x0B000002,
	VFD_ICON_CAM2       = 0x0C000001
} vfd_icon;

class CVFD
{
	public:

		enum MODES
		{
			MODE_TVRADIO,
			MODE_SCART,
			MODE_SHUTDOWN,
			MODE_STANDBY,
			MODE_MENU_UTF8,
			MODE_AUDIO
#ifdef VFD_UPDATE
                ,       MODE_FILEBROWSER,
                        MODE_PROGRESSBAR,
                        MODE_PROGRESSBAR2,
                        MODE_INFOBOX
#endif // LCD_UPDATE

		};
		enum AUDIOMODES
		{
			AUDIO_MODE_PLAY,
			AUDIO_MODE_STOP,
			AUDIO_MODE_FF,
			AUDIO_MODE_PAUSE,
			AUDIO_MODE_REV
		};


	public:

		~CVFD() {}
		bool has_lcd;
		void setlcdparameter(void) {}

		static CVFD* getInstance() { return new CVFD(); } /* deleted at shutdown */
		void init(const char * fontfile, const char * fontname) {}

		void setMode(const MODES m, const char * const title = "") {}

		void showServicename(const std::string & name) {} // UTF-8
		void showTime(bool force = false) {}
		/** blocks for duration seconds */
		void showRCLock(int duration = 2) {}
		void showVolume(const char vol, const bool perform_update = true) {}
		void showPercentOver(const unsigned char perc, const bool perform_update = true) {}
		void showMenuText(const int position, const char * text, const int highlight = -1, const bool utf_encoded = false) {}
		void showAudioTrack(const std::string & artist, const std::string & title, const std::string & album) {}
		void showAudioPlayMode(AUDIOMODES m=AUDIO_MODE_PLAY) {}
		void showAudioProgress(const char perc, bool isMuted) {}
		void setBrightness(int) {}
		int getBrightness() { return 0; }

		void setBrightnessStandby(int) {}
		int getBrightnessStandby() { return 0; }

		void setPower(int) {}
		int getPower() { return 0; }

		void togglePower(void) {}

		void setInverse(int) {}
		int getInverse() { return 0; }

		void setMuted(bool) {}

		void resume() {}
		void pause() {}
		void Lock() {}
		void Unlock() {}
		void Clear() {}
		void ShowIcon(vfd_icon icon, bool show) {}
		void ShowText(char * str) {}
#ifdef LCD_UPDATE
        public:
                MODES getMode(void) { return MODE_STANDBY; }

                void showFilelist(int flist_pos = -1,CFileList* flist = NULL,const char * const mainDir=NULL) {}
                void showInfoBox(const char * const title = NULL,const char * const text = NULL,int autoNewline = -1,int timer = -1) {}
                void showProgressBar(int global = -1,const char * const text = NULL,int show_escape = -1,int timer = -1) {}
                void showProgressBar2(int local = -1,const char * const text_local = NULL,int global = -1,const char * const text_global = NULL,int show_escape = -1) {}
#endif // LCD_UPDATE

};


#endif
