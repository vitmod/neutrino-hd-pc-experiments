/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <mntent.h>

#include <gui/dboxinfo.h>
#include <gui/widget/progressbar.h>

#include <global.h>

#include <driver/fontrenderer.h>
#include <driver/screen_max.h>
#include <driver/rcinput.h>

#include <sys/sysinfo.h>
#include <sys/vfs.h>

static const int FSHIFT = 16;              /* nr of bits of precision */
#define FIXED_1         (1<<FSHIFT)     /* 1.0 as fixed-point */
#define LOAD_INT(x) ((x) >> FSHIFT)
#define LOAD_FRAC(x) LOAD_INT(((x) & (FIXED_1-1)) * 100)
#define ROUND_RADIUS 9
CDBoxInfoWidget::CDBoxInfoWidget()
{
	frameBuffer = CFrameBuffer::getInstance();
	hheight     = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight     = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	// width       = 600;
	// height      = hheight+13*mheight+ 10;
	width = 0;
	height = 0;
	x = 0;
	y = 0;
}


int CDBoxInfoWidget::exec(CMenuTarget* parent, const std::string &)
{
	if (parent)
	{
		parent->hide();
	}
	paint();

	int res = g_RCInput->messageLoop();

	hide();
	return res;
}

void CDBoxInfoWidget::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CDBoxInfoWidget::paint()
{
	const int headSize = 5;
	const char *head[headSize] = {"Filesystem", "Size", "Used", "Available", "Use%"};
	int fontWidth = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getWidth();
	int sizeOffset = fontWidth * 7;//9999.99M
	int percOffset = fontWidth * 3 ;//100%
	int nameOffset = fontWidth * 9;//WWWwwwwwww
	int offsetw = nameOffset+ (sizeOffset+10)*3 +10+percOffset+10;
	offsetw += 20;
	width = offsetw + 10 + 120;
	height = hheight + 6 * mheight;

	struct statfs s;
	FILE *          mountFile;
	struct mntent * mnt;

	/* this is lame, as it duplicates code. OTOH, it is small and fast enough...
	   The algorithm is exactly the same as below in the display routine */
	if ((mountFile = setmntent("/proc/mounts", "r")) == NULL) {
		perror("/proc/mounts");
	} else {
		while ((mnt = getmntent(mountFile)) != NULL) {
			if (::statfs(mnt->mnt_dir, &s) == 0) {
				switch (s.f_type)	/* f_type is long */
				{
				case 0xEF53L:		/*EXT2 & EXT3*/
				case 0x6969L:		/*NFS*/
				case 0xFF534D42L:	/*CIFS*/
				case 0x517BL:		/*SMB*/
				case 0x52654973L:	/*REISERFS*/
				case 0x65735546L:	/*fuse for ntfs*/
				case 0x58465342L:	/*xfs*/
				case 0x4d44L:		/*msdos*/
					break;
				case 0x72b6L:		/*jffs2*/
					if (strcmp(mnt->mnt_fsname, "rootfs") == 0)
						continue;
					height += mheight;
					break;
				default:
					continue;
				}
				height += mheight;
			}
		}
		endmntent(mountFile);
	}

	width = w_max(width, 0);
	height = h_max(height, 0);
	x = getScreenStartX(width);
	y = getScreenStartY(height);

	fprintf(stderr, "CDBoxInfoWidget::CDBoxInfoWidget() x = %d, y = %d, width = %d height = %d\n", x, y, width, height);
	int ypos=y;
	int i = 0;
	frameBuffer->paintBoxRel(x, ypos, width, hheight, COL_MENUHEAD_PLUS_0, ROUND_RADIUS, CORNER_TOP);
	frameBuffer->paintBoxRel(x, ypos+ hheight, width, height- hheight, COL_MENUCONTENT_PLUS_0, ROUND_RADIUS, CORNER_BOTTOM);

	ypos+= hheight + (mheight >>1);
	FILE* fd = fopen("/proc/cpuinfo", "rt");
	if (fd==NULL) {
		printf("error while opening proc-cpuinfo\n" );
	} else {
		char *buffer=NULL;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&buffer, &len, fd)) != -1) {
			if (!(strncmp(const_cast<char *>("Hardware"),buffer,8))) {
				char *t=rindex(buffer,'\n');
				if (t)
					*t='\0';

				std::string hw;
				char *p=rindex(buffer,':');
				if (p)
					hw=++p;
				hw+=" Info";
				g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x+10, y + hheight+1, width - 10, hw.c_str(), COL_MENUHEAD, 0, true); // UTF-8
				break;
			}
			i++;
			if (i > 2)
				continue;
			g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+ 10, ypos+ mheight, width - 10, buffer, COL_MENUCONTENT);
			ypos+= mheight;
		}
		fclose(fd);
		if (buffer)
			free(buffer);
	}
	char *ubuf=NULL, *sbuf=NULL;
	int buf_size=256;
	ubuf = new char[buf_size];
	sbuf = new char[buf_size];

	if (sbuf != NULL && ubuf != NULL) {
		int updays, uphours, upminutes;
		struct sysinfo info;
		struct tm *current_time;
		time_t current_secs;
		memset(sbuf, 0, 256);
		time(&current_secs);
		current_time = localtime(&current_secs);

		sysinfo(&info);

		snprintf( ubuf,buf_size, "%2d:%02d%s  up ",
			  current_time->tm_hour%12 ? current_time->tm_hour%12 : 12,
			  current_time->tm_min, current_time->tm_hour > 11 ? "pm" : "am");
		strcat(sbuf, ubuf);
		updays = (int) info.uptime / (60*60*24);
		if (updays) {
			snprintf(ubuf,buf_size, "%d day%s, ", updays, (updays != 1) ? "s" : "");
			strcat(sbuf, ubuf);
		}
		upminutes = (int) info.uptime / 60;
		uphours = (upminutes / 60) % 24;
		upminutes %= 60;
		if (uphours)
			snprintf(ubuf,buf_size,"%2d:%02d, ", uphours, upminutes);
		else
			snprintf(ubuf,buf_size,"%d min, ", upminutes);
		strcat(sbuf, ubuf);

		snprintf(ubuf,buf_size, "load: %ld.%02ld, %ld.%02ld, %ld.%02ld\n",
			 LOAD_INT(info.loads[0]), LOAD_FRAC(info.loads[0]),
			 LOAD_INT(info.loads[1]), LOAD_FRAC(info.loads[1]),
			 LOAD_INT(info.loads[2]), LOAD_FRAC(info.loads[2]));
		strcat(sbuf, ubuf);
		ypos+= mheight/2;
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+ 10, ypos+ mheight, width - 10, sbuf, COL_MENUCONTENT);
		ypos+= mheight;

		snprintf(ubuf,buf_size, "memory total %dKb, free %dKb", (int) info.totalram/1024, (int) info.freeram/1024);
		ypos+= mheight/2;
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+ 10, ypos+ mheight, width - 10, ubuf, COL_MENUCONTENT);
		ypos+= mheight;
		int headOffset=0;
		int mpOffset=0;
		bool rec_mp=false;
		if ((mountFile = setmntent("/proc/mounts", "r")) == 0 ) {
			perror("/proc/mounts");
		}
		else {
			float gb=0;
			char c=' ';
			while ((mnt = getmntent(mountFile)) != 0) {
				if (::statfs(mnt->mnt_dir, &s) == 0) {
					switch (s.f_type)
					{
					case (int) 0xEF53:      /*EXT2 & EXT3*/
					case (int) 0x6969:      /*NFS*/
					case (int) 0xFF534D42:  /*CIFS*/
					case (int) 0x517B:      /*SMB*/
					case (int) 0x52654973:  /*REISERFS*/
					case (int) 0x65735546:  /*fuse for ntfs*/
					case (int) 0x58465342:  /*xfs*/
					case (int) 0x4d44:      /*msdos*/
						gb = 1024.0*1024.0;
						c = 'G';
						break;
					case (int) 0x72b6:	/*jffs2*/
						if (strcmp(mnt->mnt_fsname, "rootfs") == 0)
							continue;
						// paint mount head
						for (int j = 0; j < headSize; j++) {
							switch (j)
							{
							case 0:
								headOffset = 10;
								break;
							case 1:
								headOffset = nameOffset + 20;
								break;
							case 2:
								headOffset = nameOffset + sizeOffset+10 +20;
								break;
							case 3:
								headOffset = nameOffset + (sizeOffset+10)*2+15;
								break;
							case 4:
								headOffset = nameOffset + (sizeOffset+10)*3+15;
								break;
							}
							g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+ headOffset, ypos+ mheight, width - 10, head[j], COL_MENUCONTENTINACTIVE);
						}
						ypos+= mheight;

						gb = 1024.0;
						c = 'M';
						break;
					default:
						continue;
					}
					if ( s.f_blocks > 0 ) {
						long	blocks_used;
						long	blocks_percent_used;
						blocks_used = s.f_blocks - s.f_bfree;
						blocks_percent_used = (long)(blocks_used * 100.0 / (blocks_used + s.f_bavail) + 0.5);
						//paint mountpoints
						for (int j = 0; j < headSize; j++) {
							switch (j)
							{
							case 0: {
								if (s.f_type != 0x72b6)
								{
									char *p1=NULL, *p2=NULL;
									p1=strchr(g_settings.network_nfs_recordingdir+1,'/') ;
									p2=strchr(mnt->mnt_dir+1,'/') ;
									if (p2) {
										if (strstr(p1,p2)) {

											rec_mp = true;
										}
									}
									else {
										if (strstr(g_settings.network_nfs_recordingdir,mnt->mnt_dir)) {
											rec_mp = true;
										}
									}
								}
								mpOffset = 10;
								snprintf(ubuf,buf_size,"%-10.10s",basename(mnt->mnt_fsname));
							}
							break;
							case 1:
								mpOffset = nameOffset + 10;
								snprintf(ubuf,buf_size,"%7.2f%c", (s.f_blocks * (s.f_bsize / 1024.0)) / gb, c);
								break;
							case 2:
								mpOffset = nameOffset+ (sizeOffset+10)*1+10;
								snprintf(ubuf,buf_size,"%7.2f%c", ((s.f_blocks - s.f_bfree)  * (s.f_bsize / 1024.0)) / gb, c);
								break;
							case 3:
								mpOffset = nameOffset+ (sizeOffset+10)*2+10;
								snprintf(ubuf,buf_size,"%7.2f%c", s.f_bavail * (s.f_bsize / 1024.0) / gb, c);
								break;
							case 4:
								mpOffset = nameOffset+ (sizeOffset+10)*3+10;
								snprintf(ubuf,buf_size,"%4ld%c", blocks_percent_used,'%');
								break;
							}
							g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x + mpOffset, ypos+ mheight, width - 10, ubuf, rec_mp ? COL_MENUHEAD+2:COL_MENUCONTENT);
							rec_mp = false;
						}
						int pbw = width - offsetw - 10;
//fprintf(stderr, "width: %d offsetw: %d pbw: %d\n", width, offsetw, pbw);
						if (pbw > 8) /* smaller progressbar is not useful ;) */
						{
							CProgressBar pb(g_settings.progressbar_color, -1, -1, 30, 100, 70, true);
							pb.paintProgressBarDefault(x+offsetw, ypos+3, pbw, mheight-10, blocks_percent_used, 100);
						}
						ypos+= mheight;
					}
					i++;
				}
				if (ypos > y + height - mheight)	/* the screen is not high enough */
					break;				/* todo: scrolling? */
			}
			endmntent(mountFile);
		}
	}
	if (sbuf)
		delete[] sbuf;
	if (ubuf)
		delete[] ubuf;

}
