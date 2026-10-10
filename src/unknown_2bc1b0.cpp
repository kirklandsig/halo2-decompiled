#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1523c0.h"
#include "data_array.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_2BC1B0.CPP: the game engine whose vtable is at 0x45c7a8 (the second
   engine object at 0x47fc84): its slots 30..50, which unknown_1523c0.h numbers
   v0..v20 (its slots 0..28 are c_game_engine_derived's v22..v50 in
   unknown_072c70.cpp), and the helpers they use. The engine keeps up to three
   marker indices in its state (g_51ecc4, defined by unknown_072c70.cpp). */

struct s_game_engine_data;
extern s_game_engine_data *g_51ecc4;

/* the engine's state, as these functions read it */
struct s_state_2bc1
{
	byte unknown00[0xc];
	long markers[3];
};

long function_19f3c0(long player_index, long type);
bool function_15eaf0();
void function_15b930(long player_index, bool by_team, long counter, long delta);

class c_game_engine_45c7a8 : public c_game_engine
{
public:
	virtual void v0(long, long, bool, long);
	virtual void v2(long, long);
	virtual bool v5(long, long);
	virtual long v7(long, byte *);
};

// @retail 0x2bc1b0
bool function_2bc1b0(point3f *position, long index)
{
	bool marker_found_flag = false;
	long marker_index = ((s_state_2bc1 *)g_51ecc4)->markers[index];

	if (marker_index != NONE)
	{
		s_marker_entry *marker = &g_4e0350->marker_entries[marker_index];

		if (position)
		{
			*position = marker->position;
		}
		marker_found_flag = true;
	}
	return marker_found_flag;
}

// @retail 0x2bcc10
void c_game_engine_45c7a8::v0(long player_index, long other_player_index, bool flag, long)
{
	if (function_19f3c0(other_player_index, 2) != NONE && !flag && player_index != NONE && player_index != other_player_index)
	{
		function_15b930(player_index, function_15eaf0(), 0x1a, 1);
	}
	if (player_index != NONE && !flag && player_index != other_player_index && function_19f3c0(player_index, 2) != NONE)
	{
		function_15b930(player_index, function_15eaf0(), 0x19, 1);
	}
}

// @retail 0x2bcc90
bool c_game_engine_45c7a8::v5(long player_index, long type)
{
	bool result = false;

	if (function_19f3c0(player_index, 2) != NONE)
	{
		if (type == 2)
			result = g_4e6948->s232 == 0;
		else if (type == 1)
			result = g_4e6948->flags22c_bits.bit1;
		else if (type == 3)
			result = g_4e6948->flags22c_bits.bit2;
	}
	else
		result = c_game_engine::v5(player_index, type);
	return result;
}

// @retail 0x2bcd20
long c_game_engine_45c7a8::v7(long player_index, byte *b)
{
	long result = NONE;

	*b = 1;
	if (function_19f3c0(player_index, 2) != NONE)
		result = 0xe;
	return result;
}

struct s_team_entry;
struct s_spawn_influence_list;
s_team_entry *function_15e410(short team);
void function_23ba10(long type, s_spawn_influence_list *list, point3f const *point);

struct s_object_header_2bc1
{
	dword unknown00[2];
	byte *object;
};

struct s_item_position_2bc1
{
	byte unknown00[0x30];
	point3f position;
};

// @retail 0x2bcd50
void c_game_engine_45c7a8::v2(long unused, long list_pointer)
{
	for (long i = 0; i < g_4e6948->s230; i++)
	{
		long *entry = (long *)function_15e410((short)i);

		if (entry && *entry != NONE)
		{
			long object_index = *entry;
			s_item_position_2bc1 *object = (s_item_position_2bc1 *)((s_object_header_2bc1 *)g_4e0300->data)[object_index & 0xffff].object;

			function_23ba10(6, (s_spawn_influence_list *)list_pointer, &object->position);
			function_23ba10(5, (s_spawn_influence_list *)list_pointer, &object->position);
		}
	}
}

bool function_15b7c0(long delta, long player_index);
long function_19fc70(dword player_index);
void function_19f470(long player_index, long score);

// @retail 0x2bce70
void function_2bce70(long player_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->w6c == 1 &&
		(g_4e6948->mode == 4 || g_4e9ae8->lc04 == 1))
	{
		long old_score = function_19fc70(player_index);

		if (function_15b7c0(1, player_index))
		{
			bool by_team = false;

			if (g_55e4d0[g_4e9ae8->engine_index])
			{
				by_team = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
			}
			function_15b930(player_index, by_team, 0x17, 1);
		}
		long score = function_19fc70(player_index);
		if (old_score != score)
		{
			function_19f470(player_index, score);
		}
	}
}
