#include "unknown_11c920.h"
#include "unknown_13927e.h"

// @flags /O1 /Gr

/* UNKNOWN_13927E.CPP: the color of a player's markers, the second marker
   color when 0x22acb4 holds for the player */

/* not decompiled yet (src/stubs/lane_h.cpp) */
bool function_22acb4(long player_index);

// @retail 0x13927e
s_color_bits *function_13927e(long player_index)
{
	s_color_bits *color = (s_color_bits *)&g_468c80[0].red;

	if (function_22acb4(player_index))
		color = (s_color_bits *)&g_468c80[1].red;

	return color;
}
