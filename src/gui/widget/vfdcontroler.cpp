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

#include <gui/widget/vfdcontroler.h>

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>
#include <driver/screen_max.h>

#include <gui/color.h>
#include <gui/widget/messagebox.h>
#include "gui/widget/icons.h"

#include <global.h>
#include <neutrino.h>

#include <math.h>

#define BRIGHTNESSFACTOR 0.15 // 0 - 15

CVfdControler::CVfdControler(const neutrino_locale_t Name, CChangeObserver* Observer)
{
	frameBuffer	= CFrameBuffer::getInstance();
	hheight		= g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight		= g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	font_info	= SNeutrinoSettings::FONT_TYPE_MENU;
	observer	= Observer;
	name		= Name;
	
	// calculate width
	lwidth		= g_Font[font_info]->getRenderWidth(g_Locale->getText (LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY));
	int tmpwidth 	= g_Font[font_info]->getRenderWidth(g_Locale->getText (LOCALE_LCDCONTROLER_BRIGHTNESS));
	
	if (tmpwidth > lwidth) {
		lwidth = tmpwidth;
	}
	// reuse tmpwidth as dummy
	tmpwidth = g_Font[font_info]->getRenderWidth("15");
	
	width		= w_max((15 + lwidth + 15 + 120 + 10 + tmpwidth + 10 + 15), 0);
	height		= h_max(hheight+ mheight* 3+ +mheight/2, 0);
	x		= frameBuffer->getScreenX() + ((frameBuffer->getScreenWidth()-width) >> 1);
	y		= frameBuffer->getScreenY() + ((frameBuffer->getScreenHeight()-height)>>1);

	brightness = CVFD::getInstance()->getBrightness();
	brightnessstandby = CVFD::getInstance()->getBrightnessStandby();
}

void CVfdControler::setVfd()
{
	CVFD::getInstance()->setBrightness(brightness);
	CVFD::getInstance()->setBrightnessStandby(brightnessstandby);
}

int CVfdControler::exec(CMenuTarget* parent, const std::string &)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int selected, res = menu_return::RETURN_REPAINT;
	unsigned int brightness_alt, brightnessstandby_alt;

	if (parent)
	{
		parent->hide();
	}
	brightness_alt = CVFD::getInstance()->getBrightness();
	brightnessstandby_alt = CVFD::getInstance()->getBrightnessStandby();
	selected = 0;

	setVfd();
	paint();

	uint64_t timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] == 0 ? 0xFFFF : g_settings.timing[SNeutrinoSettings
::TIMING_MENU]);

	bool loop=true;
	while (loop)
	{
		g_RCInput->getMsgAbsoluteTimeout( &msg, &data, &timeoutEnd, true );

		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] == 0 ? 0xFFFF : g_settings.timing[SNeutrinoSettings
::TIMING_MENU]);

		switch ( msg )
		{
		case CRCInput::RC_down:
			if(selected < 2) // max entries
			{
				paintSlider(x + 10, y + hheight    , brightness       , BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS       , false);
				paintSlider(x + 10, y + hheight + mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, false);
				selected++;
				switch (selected) {
				case 0:
					paintSlider(x+ 10, y+ hheight, brightness, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS, true);
					break;
				case 1:
					paintSlider(x+ 10, y+ hheight+ mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, true);
					CVFD::getInstance()->setMode(CVFD::MODE_STANDBY);
					break;
				case 2:
					frameBuffer->paintBoxRel(x, y+hheight+mheight*2+mheight/2, width, mheight, COL_MENUCONTENTSELECTED_PLUS_0, RADIUS_LARGE);
					g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+10, y+hheight+mheight*3+mheight/2, width, g_Locale->getText(LOCALE_OPTIONS_DEFAULT), COL_MENUCONTENTSELECTED, 0, true); // UTF-8
					break;
				}
			}
			break;

		case CRCInput::RC_up:
			if (selected > 0) {
				paintSlider(x + 10, y + hheight    , brightness       , BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS       , false);
				paintSlider(x + 10, y + hheight + mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, false);
				selected--;
				switch (selected) {
				case 0:
					paintSlider(x+ 10, y+ hheight, brightness, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS, true);
					CVFD::getInstance()->setMode(CVFD::MODE_TVRADIO);
					break;
				case 1:
					paintSlider(x+10, y+hheight+mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, true);
					CVFD::getInstance()->setMode(CVFD::MODE_STANDBY);
					frameBuffer->paintBoxRel(x, y+hheight+mheight*2+mheight/2, width, mheight, COL_MENUCONTENT_PLUS_0, RADIUS_LARGE, CORNER_BOTTOM);
					g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+10, y+hheight+mheight*3+mheight/2, width, g_Locale->getText(LOCALE_OPTIONS_DEFAULT), COL_MENUCONTENT, 0, true); // UTF-8
					break;
				case 2:
					break;
				}
			}
			break;

			case CRCInput::RC_right:
				switch (selected) {
					case 0:
						if (brightness < 15) {
							brightness ++;
							paintSlider(x+10, y+hheight, brightness, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS, true);
							setVfd();
						}
						break;
					case 1:
						if (brightnessstandby < 15) {
							brightnessstandby ++;
							paintSlider(x+10, y+hheight+mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, true);
							setVfd();
						}
						break;
				}
				break;

			case CRCInput::RC_left:
				switch (selected) {
					case 0:
						if (brightness > 0) {
							brightness--;
							paintSlider(x+10, y+hheight, brightness, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS, true);
							setVfd();
						}
						break;
					case 1:
						if (brightnessstandby > 0) {
							brightnessstandby--;
							paintSlider(x+10, y+hheight+mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, true);
							setVfd();
						}
						break;
				}
				break;

			case CRCInput::RC_home:
				if ( ((brightness != brightness_alt) || (brightnessstandby != brightnessstandby_alt) ) &&
				     (ShowLocalizedMessage(name, LOCALE_MESSAGEBOX_DISCARD, CMessageBox::mbrYes, CMessageBox::mbYes | CMessageBox::mbCancel) == CMessageBox::mbrCancel))
					break;

				brightness = brightness_alt;
				brightnessstandby = brightnessstandby_alt;
				setVfd();
				loop = false;
				break;

			case CRCInput::RC_ok:
				if (selected==2) {
					brightness		= DEFAULT_VFD_BRIGHTNESS;
					brightnessstandby	= DEFAULT_VFD_STANDBYBRIGHTNESS;
					selected		= 0;
					setVfd();
					paint();
					break;
				}

			case CRCInput::RC_timeout:
				loop = false;
				break;

			case CRCInput::RC_sat:
			case CRCInput::RC_favorites:
				break;
			default:
				if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all )
				{
					loop = false;
					res = menu_return::RETURN_EXIT_ALL;
				}
		}
	}

	hide();

	if(observer)
		observer->changeNotify(name, NULL);

	return res;
}

void CVfdControler::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CVfdControler::paint()
{
	CVFD::getInstance()->setMode(CVFD::MODE_TVRADIO);

	frameBuffer->paintBoxRel(x, y, width, hheight, COL_MENUHEAD_PLUS_0, RADIUS_LARGE, CORNER_TOP);//round
	frameBuffer->paintBoxRel(x, y+hheight, width, height-hheight, COL_MENUCONTENT_PLUS_0, RADIUS_LARGE, CORNER_BOTTOM);//round

	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x+10,y+hheight, width, g_Locale->getText(name), COL_MENUHEAD, 0, true); // UTF-8

	paintSlider(x+10, y+hheight, brightness, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESS, true);
	paintSlider(x+10, y+hheight+mheight, brightnessstandby, BRIGHTNESSFACTOR, LOCALE_LCDCONTROLER_BRIGHTNESSSTANDBY, false);

	frameBuffer->paintHLineRel(x+10, width-20, y+hheight+mheight*2+mheight/4, COL_MENUCONTENT_PLUS_3);
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(x+10, y+hheight+mheight*3+mheight/2, width, g_Locale->getText(LOCALE_OPTIONS_DEFAULT), COL_MENUCONTENT, 0, true); // UTF-8
}

void CVfdControler::paintSlider(int px, int py, unsigned int spos, float factor, const neutrino_locale_t text, bool selected)
{
	int startx = lwidth + 15;
	char wert[5];

	frameBuffer->paintBoxRel(px + startx, py, 120, mheight, COL_MENUCONTENT_PLUS_0);
	frameBuffer->paintIcon(NEUTRINO_ICON_VOLUMEBODY, px + startx, py+2+mheight/4);
	frameBuffer->paintIcon(selected ? NEUTRINO_ICON_VOLUMESLIDER2BLUE : NEUTRINO_ICON_VOLUMESLIDER2, (int)(px + (startx+3)+(spos / factor)), py+mheight/4);

	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(px, py+mheight, width, g_Locale->getText(text), COL_MENUCONTENT, 0, true); // UTF-8
	sprintf(wert, "%3d", spos); // UTF-8 encoded
	frameBuffer->paintBoxRel(px + startx + 120 + 10, py, 50, mheight, COL_MENUCONTENT_PLUS_0);
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(px + startx + 120 + 10, py+mheight, width, wert, COL_MENUCONTENT, 0, true); // UTF-8
}
