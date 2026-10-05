#include "unknown_11c920.h"
#include "font_loading.h"
#include "globals.h"
#include "unknown_0259a0.h"
#include "unknown_13927e.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_162550.CPP: the marker list the multiplayer engines show over a
   player: placed at the head marker of the player's unit and colored by team,
   or, with no unit, at the player's last position in the multiplayer globals
   with a fill of w12 / w10. Fills a marker list for 24e59f and says whether
   there is one. */

/* the players (0x21c bytes each), as this file sees them */
struct s_marker_player
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x88 - 0x30];
	byte type;
	byte unknown89[0x21c - 0x89];
};

/* the constant colors (pointers to white, grey, black, red, blue, yellow) */
extern point3f *g_468710;
extern point3f *g_468718;
color3f *g_468714;
color3f *g_46871c;
color3f *g_468724;
color3f *g_46872c;

bool g_4c99b8;
bool g_476fcc;
long g_4c9888;
extern long g_4ba04c;
extern long g_4b9ed8;

/* not decompiled yet (src/stubs/lane_h.cpp) */
bool function_53750(long player_index);

static inline bool function_4c9888_is_one()
{
	return g_4c9888 == 1;
}

static inline s_marker_player *marker_player_get(long player_index)
{
	return (s_marker_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_marker_player));
}

// @retail 0x162550
bool function_162550(long player_index, s_marker_list *list)
{
	long absolute_index = player_index & 0xffff;
	s_marker_player *player = marker_player_get(player_index);
	s_player_info *info = &g_4e9ae8->players[absolute_index];
	bool result = true;
	s_font_header *font = font_get(g_4e28f4[(g_4ba04c <= 1) + 5]);
	short height = 10;

	if (font)
		height = font->leading_height + font->descending_height + font->ascending_height;

	list->r14 = 0.05f;
	list->b0 = false;
	list->b1 = false;
	list->l4 = 1;
	list->r18 = 0.0f;
	list->r1c = (real)height;
	list->l20 = player->unit_index;
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 1;
	list->items[0].kind = 3;
	list->items[0].a = *(s_color_bits *)g_468718;
	list->items[0].b = *(s_color_bits *)g_468718;
	list->items[0].r = 0.0f;
	list->items[0].index = player_index;

	if (player->unit_index != NONE)
	{
		s_object_marker marker;

		list->b0 = info->b14 != 0;
		function_b8d30(player->unit_index, 0x4000095, &marker, 1, false);
		list->position = marker.matrix.position;

		s_color_bits *color = (s_color_bits *)&g_468c80[0].red;
		if (g_4b9ed8 != NONE)
		{
			long local_player_index = g_4e8c20->entries[g_4b9ed8];

			if (local_player_index != NONE)
			{
				s_marker_player *local_player = marker_player_get(local_player_index);

				if (local_player->type == 1 || local_player->type == 3)
					color = (s_color_bits *)&g_468c80[1].red;
			}
		}
		list->color24 = *color;

		if (g_4c99b8 && g_476fcc && function_4c9888_is_one() && function_53750(absolute_index))
		{
			list->b1 = true;
			list->color30 = *(s_color_bits *)g_468710;
		}
		else
		{
			switch ((char)info->b14)
			{
			case 0:
				list->color30 = *function_13927e(g_4b9ed8);
				break;
			case 1:
				list->b1 = true;
				list->color30 = *(s_color_bits *)g_46872c;
				break;
			case 2:
				list->b1 = true;
				list->color30 = *(s_color_bits *)g_468710;
				break;
			case 4:
				list->b1 = true;
				list->color30 = *(s_color_bits *)g_468724;
				break;
			case 5:
				list->b1 = true;
				list->color30 = *(s_color_bits *)g_468714;
				break;
			case 3:
			default:
				list->b1 = true;
				list->color30 = *(s_color_bits *)g_46871c;
				break;
			}
		}
	}
	else if (info->b0 && info->w10)
	{
		list->position = info->v;
		list->color24 = *(s_color_bits *)g_46871c;
		list->color30 = *(s_color_bits *)g_46871c;
		list->items[0].kind = 4;
		list->r40 = 1.0f - (real)(short)info->w12 / (real)(short)info->w10;
		list->items[0].a = *(s_color_bits *)g_46871c;
		list->items[0].b = *(s_color_bits *)g_46871c;
	}
	else
	{
		result = false;
	}

	return result;
}
