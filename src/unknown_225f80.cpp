// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_225F80.CPP: the checkpoint sequence the scripts start (g_4701ec:
   when the game may save, retried for a while) and the lifecycle callbacks
   of entry 66 (g_51ebf8, the time of the last attempt) */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"

long *g_51ebf8;

/* the state at g_510c50 (unknown_13bf00.cpp) as seen here */
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_unknown_225f80_13bf00_view
{
	byte unknown00[5];
	bool active;
};

/* g_4e8c20 as seen here: a flag at +5 */
struct s_unknown_225f80_index_view
{
	byte unknown00[5];
	bool flag5;
};

/* the players (g_4e8c24) as seen here: the unit */
struct s_unknown_225f80_player
{
	byte unknown00[0x2c];
	long unit_index;
};

struct s_unknown_225f80_unit
{
	byte unknown000[0xec];
	real vitality;
	byte unknown0f0[0x106 - 0xf0];
	short unknown106;
};

struct s_unknown_225f80_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	s_unknown_225f80_unit *object;
};

/* the actors (g_4f55f0) as seen here */
struct s_unknown_225f80_actor
{
	byte unknown000[0x18];
	long unknown18;
	byte unknown01c[0x888 - 0x1c];
};

/* whether the game is in a state that blocks a checkpoint: each fills in
   the object responsible */
bool __stdcall function_fa9a0(long *value);
bool __stdcall function_10ca00(long *value);
bool function_1778d0(void);
bool __stdcall function_cc170(long *value);
bool function_1ca630(long *unit_index); /* lane C's, ai.cpp */
bool __stdcall function_14df40(long *value);
bool __stdcall function_14deb0(long *value);
bool __stdcall function_f7a60(long *value);
bool __stdcall function_f7ca0(long *value);
long function_1ca2d0(bool unknown); /* lane C's, ai.cpp */
void function_12b790(void);

PRIVATE inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

long function_146650(void);

PRIVATE inline s_unknown_225f80_unit *unit_try_and_get(long object_index)
{
	s_unknown_225f80_object_header *header = (s_unknown_225f80_object_header *)datum_get_inlined(g_4e0300, object_index);
	s_unknown_225f80_unit *result = NULL;

	if (header && ((1 << header->type) & 3))
		result = header->object;
	return result;
}

PRIVATE inline bool unknown_225f80_attempt_expired(void)
{
	if (*g_51ebf8 == NONE || function_146650() > *g_51ebf8 + game_seconds_to_ticks_round(10.0f))
		return true;
	return false;
}

// @retail 0x225f80
void function_225f80(void)
{
	g_51ebf8 = (long *)function_123d40("unknown", "unknown", sizeof(long));
}

// @retail 0x225fc0
void function_225fc0(void)
{
	*g_51ebf8 = NONE;
	g_4701ec.stage = 0;
}

// @retail 0x225fe0
bool function_225fe0(void)
{
	long value;

	if (function_fa9a0(&value) || function_10ca00(&value) || function_1778d0() || function_cc170(&value) ||
		function_1ca630(&value))
	{
		return false;
	}
	return true;
}

// @retail 0x226270
bool function_226270(void)
{
	return !unknown_225f80_attempt_expired();
}

// @retail 0x2262d0
void function_2262d0(void)
{
	s_record_pool_iterator iterator;
	s_unknown_225f80_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_unknown_225f80_player *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (player->unit_index != NONE)
		{
			s_unknown_225f80_unit *unit = unit_try_and_get(player->unit_index);

			if (unit && 1.0f > unit->vitality)
			{
				unit->vitality = 1.0f;
				unit->unknown106 = 0;
			}
		}
	}
}

// @retail 0x226190
bool function_226190(void)
{
	bool result = true;

	g_4701ec.unknown10 = NONE;
	if (function_226270())
	{
		result = false;
	}
	else
	{
		long actor_index = function_1ca2d0(false);

		if (actor_index != NONE)
		{
			result = false;
			g_4701ec.unknown10 = ((s_unknown_225f80_actor *)g_4f55f0->data)[actor_index & 0xffff].unknown18;
		}
		else if (function_fa9a0(&g_4701ec.unknown10) || function_10ca00(&g_4701ec.unknown10) || function_1778d0() ||
			function_cc170(&g_4701ec.unknown10) || function_14df40(&g_4701ec.unknown10) ||
			function_14deb0(&g_4701ec.unknown10) || ((s_unknown_225f80_index_view *)g_4e8c20)->flag5 ||
			g_4e6948->flag1121 || function_f7a60(&g_4701ec.unknown10) || function_f7ca0(&g_4701ec.unknown10))
		{
			result = false;
		}
	}
	return result;
}

// @retail 0x226050
void function_226050(void)
{
	s_unknown_225f80_13bf00_view *state = (s_unknown_225f80_13bf00_view *)g_510c50;

	if (state && state->active)
	{
		if (g_4701ec.stage == 4)
		{
			if (g_4e6948->flag1121)
			{
				g_4701ec.stage = 0;
			}
			else
			{
				g_4701ec.stage = 0;
				function_12b790();
			}
		}
	}
	else if (g_4701ec.stage > 0)
	{
		bool save = false;
		s_game_time_globals *game_time = g_510c54;

		if (g_4701ec.stage >= 3)
		{
			if (g_4e6948->flag1121)
			{
				g_4701ec.stage = 0;
			}
			else
			{
				save = true;
			}
		}
		else
		{
			if (g_4701ec.unknown4 > 0)
			{
				g_4701ec.unknown4--;
			}
			else
			{
				real seconds;
				long ticks;

				if (function_226190())
				{
					if (++g_4701ec.unknownc >= 3)
						save = true;
				}
				else
				{
					g_4701ec.unknownc = 0;
				}
				seconds = game_time->field_2_3 * 0.33f;
				__asm
				{
					fld seconds
					fistp ticks
				}
				g_4701ec.unknown4 = ticks;
			}
		}

		if (save)
		{
			g_4701ec.stage = 0;
			function_2262d0();
			function_12b790();
			*g_51ebf8 = g_510c54->game_time;
			return;
		}
		if (g_4701ec.stage != 2 &&
			(real)(game_time->game_time - g_4701ec.start_time) * game_time->rate >= 8.0f)
		{
			g_4701ec.stage = 0;
		}
	}
}
