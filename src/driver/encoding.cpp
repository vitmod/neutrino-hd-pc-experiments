/*
 * $Header$
 *
 * conversion of character encodings - d-box2 linux project
 *
 * (C) 2003 by thegoodguy <thegoodguy@berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <driver/encoding.h>

std::string Latin1_to_UTF8(const std::string & s)
{
	std::string r;
	
	for (std::string::const_iterator it = s.begin(); it != s.end(); it++)
	{
		unsigned char c = *it;
		if (c == 0x8a) // 0x8a is "vertical tab". Let's just convert to newline
			r += '\n';
		else if (c < 0x80)
			r += c;
		else
		{
			unsigned char d = 0xc0 | (c >> 6);
			r += d;
			d = 0x80 | (c & 0x3f);
			r += d;
		}
	}		
	return r;
}

/* inspired by fontrenderer.cpp */
bool isUTF8(const char *text, int len) // returns true if text is UTF-8
{
	const char *p = text;
	while (p - text < len)
	{
		if ((*p & 0x80) != 0)
		{
			int uc_length;
			if ((*p & 0xe0) == 0xc0)
				uc_length = 1;
			else if ((*p & 0xf0) == 0xe0)
				uc_length = 2;
			else if ((*p & 0xf8) == 0xf0)
				uc_length = 3;
			else			// cf.: http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8
				return false;	// corrupted character or a character with > 4 bytes utf-8 representation

			for (int i = 0; i < uc_length; i++)
			{
				p++;
				if ((*p & 0xc0) != 0x80)
					return false;	// incomplete or corrupted character
				if (p - text >= len)	// make sure we don't exceed the buffer
					return false;
			}
		}
		p++;
	}
	return true;
}

bool isUTF8(const std::string &text)
{
	return (isUTF8(text.c_str(), text.length()));
};

int UTF8ToUnicode(const char * &text, const bool utf8_encoded) // returns -1 on error
{
	int unicode_value;

	if (utf8_encoded && ((((unsigned char)(*text)) & 0x80) != 0))
	{
		int remaining_unicode_length;
		if ((((unsigned char)(*text)) & 0xf8) == 0xf0)
		{
			unicode_value = ((unsigned char)(*text)) & 0x07;
			remaining_unicode_length = 3;
		}
		else if ((((unsigned char)(*text)) & 0xf0) == 0xe0)
		{
			unicode_value = ((unsigned char)(*text)) & 0x0f;
			remaining_unicode_length = 2;
		}
		else if ((((unsigned char)(*text)) & 0xe0) == 0xc0)
		{
			unicode_value = ((unsigned char)(*text)) & 0x1f;
			remaining_unicode_length = 1;
		}
		else                     // cf.: http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8
			return -1;       // corrupted character or a character with > 4 bytes utf-8 representation
		
		for (int i = 0; i < remaining_unicode_length; i++)
		{
			text++;
			if (((*text) & 0xc0) != 0x80)
			{
				remaining_unicode_length = -1;
				return -1;          // incomplete or corrupted character
			}
			unicode_value <<= 6;
			unicode_value |= ((unsigned char)(*text)) & 0x3f;
		}
		if (remaining_unicode_length == -1)
			return -1;                  // incomplete or corrupted character
	}
	else
		unicode_value = (unsigned char)(*text);

	return unicode_value;
}
