// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_138800.CPP: queries and timers on the game options (g_4e6948) */

#include "unknown_11c920.h"
#include "main_globals.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>

enum
{
	_game_state_campaign = 1,
	_game_state_multiplayer = 2
};

bool g_4f55e7;

void function_123ed0();

/* 0x138800 (unknown_138800_2.cpp): whether a game is in progress */
bool function_138800();

// @retail 0x138820
bool function_138820()
{
	return g_4e6948->state == _game_state_campaign && g_4e6948->flag134;
}

// @retail 0x138840
bool function_138840()
{
	bool result = false;
	long mode = g_4e6948->mode;
	if (mode >= 2 && mode <= 5)
	{
		result = true;
	}
	return result;
}

// @retail 0x138880
bool function_138880()
{
	bool result = false;
	if (g_4e6948->mode == 4)
	{
		result = true;
	}
	return result;
}

// @retail 0x138860
bool function_138860()
{
	return !function_138880();
}

static inline bool function_xe926db()
{
	return g_4e6948->state == _game_state_campaign;
}

static inline short function_xb76edd()
{
	short difficulty = 0;
	if (function_xe926db())
	{
		difficulty = g_4e6948->difficulty;
	}
	return difficulty;
}

// @retail 0x1388a0
bool function_1388a0()
{
	return !(function_xe926db() && (g_4f55e7 || function_xb76edd() == 3));
}

// @retail 0x138960
void function_138960(bool start)
{
	if (start)
	{
		if (!g_4e6948->flag1121)
		{
			g_4e6948->flag1121 = true;
			real seconds = g_510c54->field_2_3 * 5.0f;
			long ticks;
			__asm
			{
				fld seconds
				fistp ticks
			}
			g_4e6948->ticks1124 = ticks;
			function_123ed0();
		}
	}
	else if (g_4e6948->flag1121)
	{
		g_4e6948->flag1121 = false;
	}
}

// @retail 0x1389c0
void function_1389c0()
{
	s_game_options_view *options = g_4e6948;
	if (!options->flag1128)
	{
		options->flag1128 = true;
		real seconds = g_510c54->field_2_3 * 7.0f;
		long ticks;
		__asm
		{
			fld seconds
			fistp ticks
		}
		options->ticks112c = ticks;
		options->flag1129 = false;
	}
}

// @retail 0x138a10
bool function_138a10()
{
	return g_4e6948->flag1128 && g_4e6948->ticks112c == 0;
}

void function_188cd0(void);

// @retail 0x138eb0
void function_138eb0()
{
	s_game_options_view *options = g_4e6948;
	if (options->flag1128 && options->ticks112c > 0)
	{
		options->ticks112c--;
		if (!options->flag1129 && 2.0f >= options->ticks112c * g_510c54->rate)
		{
			function_188cd0();
			g_4e6948->flag1129 = true;
		}
	}
}

// @retail 0x138e40
void function_138e40()
{
	s_game_options_view *options = g_4e6948;
	if (options->state == _game_state_campaign && options->mode != 4)
	{
		bool start;
		if (options->flag134 && !function_1388a0())
		{
			start = ((bool *)g_4e8c20)[5];
		}
		else
		{
			start = ((bool *)g_4e8c20)[4];
		}
		function_138960(start);

		if (options->flag1121)
		{
			if (options->ticks1124 > 0)
			{
				options->ticks1124--;
			}
			if (options->ticks1124 == 0)
			{
				main_globals.unknown6f = true;
			}
		}
	}
}

/* ---- the cluster the game focuses on ---- */

#include "object_queries.h"
#include "unknown_0259d0.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
void function_11bed0(s_location *location, point3f const *point);

/* the scenario's points (0x40 bytes each) */
struct s_scenario_point
{
	byte unknown00[0x28];
	point3f position;
	byte unknown34[0xc];
};

struct s_scenario_points_view
{
	byte unknown00[0x1ec];
	s_scenario_point *points;
};

struct s_object_location_view
{
	byte unknown00[4];
	dword : 8;
	dword has_location : 1;
	dword : 23;
	byte unknown08[0x20];
	s_location location;
};

struct s_object_header_location_view
{
	byte unknown00[8];
	s_object_location_view *object;
};

enum
{
	_focus_none = 0,
	_focus_object,
	_focus_cluster
};

// @retail 0x138a40
void function_138a40(short point_index)
{
	if (point_index == NONE)
	{
		g_4e6948->value11fa = _focus_none;
		return;
	}

	s_location location;
	function_11bed0(&location, &((s_scenario_points_view *)g_4e0350)->points[point_index].position);
	if (location.cluster_index == NONE)
	{
		g_4e6948->value11fa = _focus_none;
	}
	else
	{
		g_4e6948->value11fa = _focus_cluster;
		g_4e6948->cluster11fc = location.cluster_index;
	}
}

// @retail 0x138ab0
short function_138ab0()
{
	s_game_options_view *options = g_4e6948;
	short result = NONE;

	switch (options->value11fa)
	{
	case _focus_object:
		if (function_badc0(options->value11fc, NONE))
		{
			long object_index = function_baf80(options->value11fc);
			s_object_location_view *object = ((s_object_header_location_view *)g_4e0300->data)[object_index & 0xffff].object;
			if (TEST_FIELD_BIT(object->has_location) && object->location.cluster_index != NONE)
			{
				return object->location.cluster_index;
			}
		}
		else
		{
			options->value11fa = _focus_none;
		}
		break;
	case _focus_cluster:
		return options->cluster11fc;
	}

	return result;
}

/* a controller's state: the buttons down, then 16 sticks (0x5c bytes each) */
struct s_controller_sticks_view
{
	byte unknown00[8];
	dword mask;
	byte unknown0c[4];
	struct
	{
		real x;
		real y;
		byte unknown08[0x54];
	} sticks[16];
};

/* stirs the game's random seed with the controller input */
// @retail 0x138db0
void function_138db0(s_controller_sticks_view const *input)
{
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == _game_state_campaign)
	{
		dword *seed = &g_4e7408->unknown0;
		for (unsigned long i = 0; i < 16; i++)
		{
			if (input->mask & (1 << i))
			{
				long count = 0;
				if (input->sticks[i].y > 0.0f)
				{
					count = 1;
				}
				else if (0.0f > input->sticks[i].y)
				{
					count = 2;
				}
				if (input->sticks[i].x > 0.0f)
				{
					count |= 4;
				}
				else
				{
					count |= 8;
				}
				for (; count > 0; count--)
				{
					_random(seed, __FILE__, __LINE__);
				}
			}
		}
	}
}
bool function_19f240(long *iterator);

// @retail 0x138fa0
bool function_138fa0(long type)
{
	bool result = false;
	struct
	{
		byte *datum;
		s_record_pool *data;
		long datum_index;
		long index;
	} iterator;
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (function_19f240((long *)&iterator))
	{
		count++;
	}

	if (count <= 8 && g_4e6948->value1130 < 3)
	{
		switch (type)
		{
		case 0:
			result = true;
			break;
		case 1:
			result = g_4e6948->value1130 < 2;
			break;
		default:
			__assume(0);
		}
	}

	return result;
}
long __stdcall function_209f00(char const *name);
void __stdcall function_209c80(long index);

/* reads the name on the second line of d:\launch.txt and launches it */
// @retail 0x138f10
void function_138f10(void)
{
	FILE *file = fopen("d:\\launch.txt", "r");

	if (file)
	{
		char name[256];

		if (!fscanf(file, "%*s\n%*s\n") && fgets(name, 255, file))
		{
			char *end = strpbrk(name, "\r\n\t");
			long index;

			if (end)
			{
				*end = 0;
			}
			index = function_209f00(name);
			if (index != NONE)
			{
				function_209c80(index);
			}
		}
		fclose(file);
	}
}
