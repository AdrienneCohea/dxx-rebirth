/*
 * Portions of this file are copyright Rebirth contributors and licensed as
 * described in COPYING.txt.
 * Portions of this file are copyright Parallax Software and licensed
 * according to the Parallax license below.
 * See COPYING.txt for license details.

THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Graphical routines for drawing solid scanlines.
 *
 */

#include <algorithm>
#include <string.h>
#include "gr.h"
#include "grdef.h"

namespace dcx {

static void gr_linear_darken(uint8_t *const dest, unsigned darkening_level, unsigned count, const gft_array1 &fade_table)
{
	auto &t = fade_table[darkening_level];
	const auto predicate = [&](const uint8_t c) { return t[c]; };
	std::transform(dest, dest + count, dest, predicate);
}

#define gr_linear_stosd(D,C,N)	memset(D,C,N)

#ifdef OGL
static
#endif
void gr_uscanline(unsigned x1, unsigned x2, unsigned y)
{
		switch(TYPE)
		{
		case BM_LINEAR:
#ifdef OGL
		case BM_OGL:
#endif
			{
				const auto data = &DATA[ROWSIZE * y + x1];
				const auto cv_fade_level = grd_curcanv->cv_fade_level;
				const auto count = x2 - x1 + 1;
				if (cv_fade_level >= gr_fade_table.size())
					gr_linear_stosd(data, static_cast<uint8_t>(COLOR), count);
				else
					gr_linear_darken(data, cv_fade_level, count, gr_fade_table);
			}
			break;
		}
}

void gr_scanline(int x1, int x2, unsigned y)
{
	if (y >= MAXY)
		return;

	if (x2 < x1)
		std::swap(x1, x2);

	if (x1 > MAXX) return;
	if (x2 < MINX) return;

	if (x1 < MINX) x1 = MINX;
	if (x2 > MAXX) x2 = MAXX;
	gr_uscanline(x1, x2, y);
}

}
