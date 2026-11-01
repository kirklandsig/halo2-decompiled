// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_13BF00.CPP: the lifecycle callbacks of entry 53 */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_030290.h"
#include "language.h"
#include <string.h>

struct s_unknown_ids
{
	long values[4];
};

struct s_unknown_13bf00
{
	real unknown0;
	bool flag4;
	bool flag5;
	byte unknown06[2];
	s_unknown_ids ids;
	byte unknown18[0xa];
	bool flag22;
	byte unknown23;
};

s_unknown_13bf00 *g_510c50;

PRIVATE __forceinline void ids_clear(s_unknown_ids *ids)
{
	memset(ids, 0xff, sizeof(*ids));
}

// @retail 0x13bf00
void function_13bf00(void)
{
	g_510c50 = (s_unknown_13bf00 *)function_123d40("unknown", "unknown", sizeof(s_unknown_13bf00));
	memset(g_510c50, 0, sizeof(*g_510c50));
}

// @retail 0x13bf60
void function_13bf60(void)
{
	s_unknown_13bf00 *data = g_510c50;

	memset(data, 0, sizeof(*data));
	ids_clear(&data->ids);
	if (g_4e6948->state == 1)
	{
		data->flag4 = true;
		data->unknown0 = 1.0f;
	}
}

// @retail 0x13bfc0
void function_13bfc0(void)
{
	if (g_4e6948->state != 1)
	{
		g_510c50->flag4 = false;
	}
	g_510c50->flag5 = false;
	g_510c50->flag22 = false;
}

/* the titles showing: a title index and the ticks it has shown */
struct s_title_state
{
	short index;
	short ticks;
};

struct s_13bf00_view
{
	real fade;
	bool fading_in;
	byte unknown05[3];
	s_title_state titles[4];
	long timer_index;
	real timer;
};

struct s_title_definition
{
	byte unknown00[0x1c];
	real fade_in_time;
	real up_time;
};

struct s_4e0350_titles_view
{
	byte unknown000[0x1f0];
	long title_count;
	s_title_definition *titles;
};

// @retail 0x13c1e0
void __stdcall function_13c1e0(short title_index, real seconds)
{
	s_13bf00_view *data = (s_13bf00_view *)g_510c50;
	short i;
	for (i = 0; i < 4; i++)
	{
		if (data->titles[i].index == NONE)
		{
			break;
		}
	}
	if (i < 4)
	{
		data->titles[i].index = title_index;
		real ticks = g_510c54->field_2_3 * seconds;
		long rounded;
		__asm
		{
			fld ticks
			fistp rounded
		}
		data->titles[i].ticks = (short)-rounded;
	}
}

static inline void timer_update()
{
	s_13bf00_view *data = (s_13bf00_view *)g_510c50;
	if (data->timer_index)
	{
		data->timer -= g_510c54->rate;
		if (0.0f >= data->timer)
		{
			memset(&data->timer_index, 0, 8);
		}
	}
}

// @retail 0x13c680
void function_13c680(void)
{
	s_game_time_globals *game_time = g_510c54;
	timer_update();

	s_13bf00_view *data = (s_13bf00_view *)g_510c50;
	if (data)
	{
		s_4e0350_titles_view *globals = (s_4e0350_titles_view *)g_4e0350;
		if (data->fading_in || data->fade > 0.0f)
		{
			real fade;
			if (data->fading_in)
			{
				fade = game_time->rate + data->fade;
				data->fade = fade;
				if (fade > 1.0f)
				{
					fade = 1.0f;
				}
			}
			else
			{
				fade = data->fade - game_time->rate;
				data->fade = fade;
				if (!(fade > 0.0f))
				{
					fade = 0.0f;
				}
			}
			data->fade = fade;
		}

		s_title_state *title = data->titles;
		long count = 4;
		do
		{
			short index = title->index;
			long pinned = index < 0 ? 0 : (index > globals->title_count - 1 ? globals->title_count - 1 : index);
			if (pinned == index)
			{
				s_title_definition *definition = &globals->titles[index];
				title->ticks++;
				real duration = definition->up_time + definition->fade_in_time;
				if (title->ticks * game_time->rate >= duration)
				{
					title->index = NONE;
					title->ticks = NONE;
				}
			}
			title++;
		} while (--count);
	}
}

// @retail 0x13cb40
bool function_13cb40(void)
{
	bool result = false;
	if (g_510c50)
	{
		result = g_510c50->flag5;
	}
	return result;
}

void function_1a0180(long tag_index, long string_handle, word *buffer);

struct s_4e0350_strings_view
{
	byte unknown000[0x35c];
	long strings_tag_index;
};

// @retail 0x13cb50
void function_13cb50(long string_handle, real seconds)
{
	if (g_4e0350)
	{
		word string[256];
		string[0] = 0;
		function_1a0180(((s_4e0350_strings_view *)g_4e0350)->strings_tag_index, string_handle, string);
		if (string[0])
		{
			s_13bf00_view *data = (s_13bf00_view *)g_510c50;
			data->timer_index = string_handle;
			data->timer = seconds + 1.5f;
		}
	}
}

/* the window bounds (unknown_1a1c3e.cpp, unknown_139296.cpp) */

bool g_485ac2;

static __forceinline short real_to_short(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return (short)result;
}

static __forceinline real function_13ccf1(void)
{
	real result = 0.0f;
	if ((g_4e6948 && g_4e6948->flag && g_4e6948->index != NONE && g_4e6948->state == 3) || !g_485ac2)
	{
		result = g_510c50->unknown0 * 0.125f;
	}
	return result;
}

/* the screen between the letterbox bars, and the two bars */
// @retail 0x13ccf0
void function_13ccf0(short_rectangle2d *screen, short_rectangle2d *top_bar, short_rectangle2d *bottom_bar)
{
	real letterbox = function_13ccf1();
	real height;

	height = (real)(g_4b9dd4 - g_4b9dd0);
	screen->left = real_to_short((real)g_4b9dd2);
	screen->right = real_to_short((real)g_4b9dd6);
	letterbox *= height;
	screen->top = real_to_short((real)g_4b9dd0 + letterbox);
	screen->bottom = real_to_short((real)g_4b9dd4 - letterbox);
	top_bar->left = real_to_short((real)g_4b9dd2);
	top_bar->right = real_to_short((real)g_4b9dd6);
	top_bar->top = real_to_short((real)g_4b9dd0);
	top_bar->bottom = real_to_short((real)g_4b9dd0 + letterbox);
	bottom_bar->left = real_to_short((real)g_4b9dd2);
	bottom_bar->right = real_to_short((real)g_4b9dd6);
	bottom_bar->top = real_to_short((real)g_4b9dd4 - letterbox);
	bottom_bar->bottom = real_to_short((real)g_4b9dd4);
}

/* a player profile's subtitle setting (0x1e0 bytes; s_player_profile_settings
   in screen_widgets.h) */
struct s_subtitle_profile_view
{
	byte unknown000[0x151];
	byte subtitles;
	byte unknown152[0x1e0 - 0x152];
};

struct s_subtitle_slot_view
{
	dword flags0 : 4;
	dword signed_in : 1;
	dword : 27;
	byte unknown004[0x18 - 0x4];
	s_subtitle_profile_view profile;
	byte unknown1f8[0xc70 - 0x1f8];
};

struct s_language_globals_view
{
	byte unknown000[0xac];
	long language;
};

static inline s_subtitle_slot_view *subtitle_slot_get(long index)
{
	return index != NONE ? (s_subtitle_slot_view *)&g_54e8e0[index] : NULL;
}

/* the slot after this one (player_slot_iterator.cpp) */
long player_slot_iterate_next(long index);

/* whether a signed in player wants subtitles: always, or when the map's
   language isn't the game's */
// @retail 0x13cbf0
bool function_13cbf0(void)
{
	long map_language;
	long index;

	bool result = false;
	get_current_language();
	map_language = 0;
	if (g_4e0350)
	{
		s_language_globals_view *globals = (s_language_globals_view *)g_4e034c;
		if (globals->language >= 0 && globals->language < 9)
		{
			map_language = globals->language;
		}
	}

	for (index = 0; index != NONE; index = player_slot_iterate_next(index))
	{
		if (TEST_FIELD_BIT(g_54e8e0[index].flag4))
		{
			s_subtitle_slot_view *slot = subtitle_slot_get(index);
			s_subtitle_profile_view profile;

			if (slot && (*(byte *)slot & 0x10))
			{
				profile = slot->profile;
			}
			else
			{
				memset(&profile, 0, sizeof(profile));
			}
			if (profile.subtitles == 1)
			{
				result = true;
			}
			else if (profile.subtitles == 0 && g_47ff38 != map_language)
			{
				result = true;
			}
		}
	}

	return result;
}
