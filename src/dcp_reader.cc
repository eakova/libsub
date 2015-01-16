/*
    Copyright (C) 2014 Carl Hetherington <cth@carlh.net>

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

#include "dcp_reader.h"
#include "vertical_reference.h"
#include "xml.h"
#include <libcxml/cxml.h>
#include <libdcp/subtitle_asset.h>

using std::list;
using std::cout;
using std::string;
using boost::shared_ptr;
using namespace sub;

static MetricTime
dcp_to_metric (libdcp::Time t)
{
	return MetricTime (t.h, t.m, t.s, t.e * 1000 / t.tcr);
}

static Colour
dcp_to_colour (libdcp::Color c)
{
	return Colour (float (c.r) / 255, float (c.g) / 255, float (c.b) / 255);
}

/** @class DCPReader
 *  @brief A class to read DCP subtitles.
 */
DCPReader::DCPReader (boost::filesystem::path file)
{
	libdcp::SubtitleAsset asset (file.parent_path().string(), file.leaf().string());
	list<shared_ptr<libdcp::Subtitle> > subs = asset.subtitles ();
	for (list<shared_ptr<libdcp::Subtitle> >::const_iterator i = subs.begin(); i != subs.end(); ++i) {
		RawSubtitle sub;

		sub.vertical_position.proportional = float ((*i)->v_position ()) / 100;
		switch ((*i)->v_align ()) {
		case libdcp::VERTICAL_TOP:
			sub.vertical_position.reference = TOP_OF_SCREEN;
			break;
		case libdcp::VERTICAL_CENTER:
			sub.vertical_position.reference = CENTRE_OF_SCREEN;
			break;
		case libdcp::VERTICAL_BOTTOM:
			sub.vertical_position.reference = BOTTOM_OF_SCREEN;
			break;
		}

		switch ((*i)->h_align ()) {
		case libdcp::HORIZONTAL_LEFT:
			sub.horizontal_position = LEFT;
			break;
		case libdcp::HORIZONTAL_CENTER:
			sub.horizontal_position = CENTRE;
			break;
		case libdcp::HORIZONTAL_RIGHT:
			sub.horizontal_position = RIGHT;
			break;
		}
			
		sub.from.set_metric (dcp_to_metric ((*i)->in ()));
		sub.to.set_metric (dcp_to_metric ((*i)->out ()));
		sub.fade_up = dcp_to_metric ((*i)->fade_up_time ());
		sub.fade_down = dcp_to_metric ((*i)->fade_down_time ());
		
		sub.text = (*i)->text ();
		sub.font = (*i)->font ();
		sub.font_size.set_proportional (float ((*i)->size ()) / (72 * 11));
		switch ((*i)->effect ()) {
		case libdcp::NONE:
			break;
		case libdcp::BORDER:
			sub.effect = BORDER;
			break;
		case libdcp::SHADOW:
			sub.effect = SHADOW;
			break;
		}

		sub.effect_colour = dcp_to_colour ((*i)->effect_color ());
		sub.colour = dcp_to_colour ((*i)->color ());

		/* Hack upon hack upon hack: detokenise <i> ... </i> */

		string original = sub.text;
		sub.text.clear ();
		size_t i = 0;
		while (i < original.length()) {
			size_t const left = original.length() - i;
			if (left >= 3 && original.substr(i, 3) == "<i>") {
				if (!sub.text.empty ()) {
					_subs.push_back (sub);
					sub.text.clear ();
				}
				sub.italic = true;
				i += 3;
			} else if (left >= 4 && original.substr(i, 4) == "</i>") {
				if (!sub.text.empty ()) {
					_subs.push_back (sub);
					sub.text.clear ();
				}
				sub.italic = false;
				i += 4;
			} else {
				sub.text += original[i];
				++i;
			}
		}

		if (!sub.text.empty ()) {
			_subs.push_back (sub);
		}
	}
}
