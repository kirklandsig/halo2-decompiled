// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_139296.CPP: per-player interface state (built for size) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_07f720.h"
#include <string.h>
#include <math.h>
#include <xmmintrin.h>

struct s_player_view
{
	byte unknown00[0x28];
	short user_index;
};

struct s_user_interface_state
{
	real value00;
	byte unknown04[0x6c - 0x04];
};

struct s_510c4c;
extern s_510c4c *g_510c4c;

/* the interface state cleared each game: draw state, then a value per user */
struct s_4e6950
{
	byte unknown00[0x70];
	real user_values[4];
};

s_4e6950 g_4e6950;

struct s_4e69d0
{
	long indices[7];
	byte unknown1c[0x270 - 0x1c];
};

s_4e69d0 g_4e69d0[4];

// @retail 0x139296
void function_139296(long user_index, real value)
{
	if (user_index >= 0 && user_index < 4)
	{
		g_4e6950.user_values[user_index] = value;
	}
}

// @retail 0x1392a9
real function_1392a9(long user_index)
{
	if (user_index >= 0 && user_index < 4)
	{
		return g_4e6950.user_values[user_index];
	}
	return 1.0f;
}

// @retail 0x13a6e8
void function_13a6e8(long player_index, real amount)
{
	s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long user_index = player->user_index;
	if (user_index != NONE)
	{
		s_user_interface_state *state = (s_user_interface_state *)(user_index * sizeof(s_user_interface_state) + (byte *)g_510c4c);
		state->value00 = state->value00 - amount;
	}
}

// @retail 0x13ac42
void function_13ac42(long index)
{
	memset(&g_4e69d0[index], 0, sizeof(s_4e69d0));
	g_4e69d0[index].indices[0] = NONE;
	g_4e69d0[index].indices[1] = NONE;
	g_4e69d0[index].indices[2] = NONE;
	g_4e69d0[index].indices[3] = NONE;
	g_4e69d0[index].indices[4] = NONE;
	g_4e69d0[index].indices[5] = NONE;
	g_4e69d0[index].indices[6] = NONE;
}

// @retail 0x13ac30
void function_13ac30(void)
{
	for (long index = 0; index < 4; index++)
	{
		function_13ac42(index);
	}
}

void *function_123d40(char const *name, char const *type, long size);

/* the new hud's game state (g_510c4c, 0x1e4 bytes) */
struct s_new_hud_user
{
	real value00;
	real value04;
	long index08;
	byte unknown0c[0x50 - 0x0c];
	long unknown50[6];
	long unknown68;
};

struct s_new_hud_globals
{
	s_new_hud_user users[4];
	byte unknown1b0[4];
	long player_index;
	byte unknown1b8[0x1c0 - 0x1b8];
	s_player_appearance appearance;
	short unknown1d0;
	bool unknown1d2;
	bool unknown1d3;
	bool unknown1d4;
	byte unknown1d5[0x1d8 - 0x1d5];
	real current;
	real target;
	real rate;
};

struct s_13a720_status
{
	byte unknown00[0x1e];
	word flags;
	byte unknown20[8];
	real value28;
	real value2c;
	byte unknown30[0xad - 0x30];
	bool flagad;
	bool flagae;
};

struct s_interface_sound_block;
bool function_13cb40(void);
long function_14de70(long local_player_index);
bool function_15eb20(long player_index);
void function_22beb5(long local_player_index, dword state, s_interface_sound_block const *block,
	long *sounds, word *playing);

// @retail 0x13a720
void function_13a720(long user_index, s_13a720_status const *status)
{
	s_new_hud_user *user = &((s_new_hud_globals *)g_510c4c)->users[user_index];
	dword flags = 0;
	if (!(status->flags & 4) && !(0.0f >= status->value28) && !function_13cb40())
	{
		if (user->value00 != -1.0f && function_15eb20(function_14de70(user_index)))
		{
			flags = (*(word volatile const *)&status->flags >> 9) & 1;
			if (user->value00 > status->value2c)
				flags |= 2;
			else
				flags &= ~2;
			if (status->value2c < 0.25f && status->value2c > 0.0f)
				flags |= 4;
			else
				flags &= ~4;
			if (status->value2c == 0.0f)
				flags |= 8;
			else
				flags &= ~8;
			if (status->flagad)
				flags |= 0x100;
			else
				flags &= ~0x100;
			if (status->flagae)
				flags |= 0x200;
			else
				flags &= ~0x200;
		}
		if (g_4e6948->state == 2 && status->value2c >= 1.0f)
			flags &= ~2;
	}
	function_22beb5(user_index, flags, (s_interface_sound_block *)((byte *)g_510c94 + 0x418),
		user->unknown50, (word *)&user->unknown68);
}

struct s_13b164_status
{
	byte unknown00[0xc];
	long index0c;
	long index10;
	byte unknown14[0x4c - 0x14];
	dword flags;
	byte unknown50[0xab - 0x50];
	bool flagab;
	byte unknownac[0xe3 - 0xac];
	bool flage3;
};

struct s_13b164_pair
{
	long object_index;
	long seat_index;
	byte unknown08[0x1c - 8];
	real value;
};

struct s_13b164_object
{
	long definition_index;
	byte unknown04[0xaa - 4];
	byte type;
	byte unknownab[0x138 - 0xab];
	short team;
	byte unknown13a[0x1c8 - 0x13a];
	s_13b164_pair pair;
};

struct s_13b164_header
{
	byte unknown00[8];
	s_13b164_object *object;
};

struct s_object;
struct s_entry_pair;
s_object *function_badc0(long object_index, dword type_mask);
bool function_106320(s_entry_pair *pair);
bool function_1df560(short team_a, short team_b);

// @retail 0x13b164
void function_13b164(long object_index, s_13b164_status *status)
{
	s_13b164_object *object = ((s_13b164_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_13b164_pair *pair = &object->pair;
	status->flags = 0;
	if (pair->object_index != NONE)
	{
		s_13b164_object *other = (s_13b164_object *)function_badc0(pair->object_index, NONE);
		if (other)
		{
			byte *definition = g_4e3b44[other->definition_index & 0xffff].bytes;
			if (((1 << other->type) & 3) && !function_1df560(object->team, other->team))
				status->flags |= 1;
			long model_index = *(long *)(definition + 0x38);
			if (model_index != NONE && pair->seat_index != NONE && pair->value >= 1.0f)
			{
				byte *model = g_4e3b44[model_index & 0xffff].bytes;
				if (pair->seat_index < *(long *)(model + 0x68))
				{
					byte *seat = *(byte **)(model + 0x6c) + pair->seat_index * 0x1c;
					if (seat[0x14] & 4)
						status->flags |= 0x10;
					if (seat[0x14] & 8)
						status->flags |= 4;
				}
			}
		}
		if (((status->index0c != NONE && status->flagab) || (status->index10 != NONE && status->flage3)) &&
			function_106320((s_entry_pair *)pair) && pair->value >= 1.0f)
			status->flags |= 8;
	}
}

struct s_13ad48_item
{
	byte unknown00[6];
	short value06;
	short value08;
	short value0a;
	short value0c;
	byte unknown0e[2];
	real value10;
	real value14;
	real value18;
	byte unknown1c[4];
	bool flag20;
	byte unknown21[3];
	bool flag24;
	bool flag25;
	bool flag26;
	byte unknown27;
	real value28;
};

// @retail 0x13ad48
void function_13ad48(long user_index, byte const *selectors, real *values, byte const *status, s_13ad48_item const *item)
{
	for (long i = 0; i < 4; i++)
	{
		real value = 0.0f;
		switch (selectors[i])
		{
		case 0: value = 0.0f; break;
		case 1: value = 1.0f; break;
		case 2: value = g_510c54->game_time * g_510c54->rate; break;
		case 3: value = function_1392a9(user_index); break;
		case 16: value = *(real const *)(status + 0x2c); break;
		case 17: value = *(real const *)(status + 0x28); break;
		case 18: value = *(real const *)(status + 0x20) >= 1.0f ? 1.0f : 0.0f; break;
		case 19: value = *(short const *)(status + 0x34) == 0 ? 1.0f : 0.0f; break;
		case 20: value = *(short const *)(status + 0x38); break;
		case 21: value = *(short const *)(status + 0x3a); break;
		case 22: value = *(real const *)(status + 0x2c) < 0.2f ? g_510c54->game_time * g_510c54->rate : 0.0f; break;
		case 24: value = 1.0f - *(real const *)(status + 0x44); break;
		case 32: value = 0.0f; break;
		case 33: value = 0.0f; break;
		case 48: value = item ? item->value06 : 0.0f; break;
		case 49: value = item ? item->value14 : 0.0f; break;
		case 50: value = item ? 100.0f - item->value10 * 100.0f : 0.0f; break;
		case 51: value = item ? item->value08 : 0.0f; break;
		case 52: value = item ? item->value18 : 0.0f; break;
		case 53: value = item && item->flag20 ? 1.0f : 0.0f; break;
		case 54: value = item && item->value0a ? (real)item->value06 / item->value0a : 0.0f; break;
		case 55: value = (item ? item->flag20 : false) ? g_510c54->game_time * g_510c54->rate : 0.0f; break;
		case 56: value = item ? 1.0f - item->value10 : 0.0f; break;
		case 57: value = item ? item->value28 : 0.0f; break;
		case 64: value = *(real const *)(status + 0x1cc); break;
		case 65: value = *(real const *)(status + 0x218); break;
		case 66: value = *(real *)((byte *)&((s_new_hud_globals *)g_510c4c)->users[user_index] + 0x28); break;
		case 67: value = *(real const *)(status + 0x220); break;
		}
		values[i] = value;
	}
}

bool function_22acb4(long player_index);

// @retail 0x13a050
void function_13a050(byte const *status, long user_index, long type, s_13ad48_item const **item_out,
	word *first_out, word *second_out, word *third_out, word *fourth_out)
{
	s_13ad48_item const *item;
	switch (type)
	{
	case 1: item = (s_13ad48_item const *)(status + 0x88); break;
	case 2: item = (s_13ad48_item const *)(status + 0xc0); break;
	case 3: item = (s_13ad48_item const *)(status + 0xf8); break;
	case 4: item = (s_13ad48_item const *)(status + 0x50); break;
	default: item = NULL; break;
	}
	word first = 1;
#define SET_CONDITION(bits, mask, condition) if (condition) { bits |= mask; } else { bits &= ~mask; }
	SET_CONDITION(first, 2, *(short const *)(status + 0x36) == NONE);
	SET_CONDITION(first, 4, *(short const *)(status + 0x36) == 0);
	SET_CONDITION(first, 8, *(short const *)(status + 0x36) == 1);
	SET_CONDITION(first, 0x10, status[0x30]);
	SET_CONDITION(first, 0x20, status[0x31]);
	SET_CONDITION(first, 0x40, *(short const *)(status + 0x3c) == NONE);
	SET_CONDITION(first, 0x80, *(short const *)(status + 0x3c) == 0);
	SET_CONDITION(first, 0x100, *(short const *)(status + 0x3c) == 1);
	SET_CONDITION(first, 0x100, *(short const *)(status + 0x3c) == 1);
	SET_CONDITION(first, 0x200, status[0x32]);
	SET_CONDITION(first, 0x400, status[0x3e]);
	SET_CONDITION(first, 0x800, status[0x3f]);
	SET_CONDITION(first, 0x1000, status[0x40]);
	SET_CONDITION(first, 0x2000, function_22acb4(user_index));
	dword flags = *(dword const *)(status + 0x4c);
	word second = (char)flags & 1;
	SET_CONDITION(second, 2, flags & 8);
	SET_CONDITION(second, 4, flags & 0x10);
	SET_CONDITION(second, 8, flags & 4);
	SET_CONDITION(second, 0x10, flags & 2);
	word third = 0;
	SET_CONDITION(third, 1, type == 1);
	SET_CONDITION(third, 2, type == 2);
	SET_CONDITION(third, 4, type == 3);
	SET_CONDITION(third, 0x40, item ? item->flag20 : false);
	third &= ~0x80;
	if (item)
	{
		if (item->value0a && !item->value06 && item->value0c && !item->value08)
			third |= 0x80;
		if (item->value10 >= 1.0f)
			third |= 0x80;
	}
	SET_CONDITION(third, 0x100, item ? item->flag24 : false);
	SET_CONDITION(third, 0x200, item ? item->flag25 : false);
	SET_CONDITION(third, 0x400, item ? item->flag26 : false);
	bool multiplayer = g_4e6948->state == 2;
	word fourth = status[0x131] == 0 ? 1 : 0;
	SET_CONDITION(fourth, 2, status[0x131] == 1);
	SET_CONDITION(fourth, 4, multiplayer && !status[0x132]);
	SET_CONDITION(fourth, 8, multiplayer && status[0x132]);
	SET_CONDITION(fourth, 0x10, status[0x133]);
	SET_CONDITION(fourth, 0x20, !status[0x133]);
	SET_CONDITION(fourth, 0x40, status[0x174]);
	SET_CONDITION(fourth, 0x80, !status[0x174]);
	SET_CONDITION(fourth, 0x100, status[0x1d0]);
	SET_CONDITION(fourth, 0x200, !status[0x1d0]);
	SET_CONDITION(fourth, 0x400, status[0x21c]);
	SET_CONDITION(fourth, 0x800, status[0x224]);
#undef SET_CONDITION
	*item_out = item;
	*first_out = first;
	*second_out = second;
	*third_out = third;
	*fourth_out = fourth;
}

void function_22a648(void);

// @retail 0x139152
void new_hud_initialize_for_new_map(void)
{
	s_new_hud_globals *globals = (s_new_hud_globals *)g_510c4c;

	memset(globals, 0, sizeof(s_new_hud_globals));
	globals->current = 1.0f;
	globals->target = 1.0f;
	globals->rate = 0.0f;
	globals->player_index = NONE;
	globals->unknown1d0 = NONE;
	globals->unknown1d2 = true;
	globals->unknown1d4 = true;
	globals->unknown1d3 = true;
	for (long i = 0; i < 4; i++)
	{
		memset(globals->users[i].unknown50, 0xff, sizeof(globals->users[i].unknown50));
		globals->users[i].value00 = -1.0f;
		globals->users[i].value04 = -1.0f;
		globals->users[i].index08 = NONE;
	}
	memset(&globals->appearance, 0, sizeof(globals->appearance));
	function_22a648();
	function_13ac30();
}

dword __cdecl pack_color3f(const color3f *color);

dword g_502234[4];
byte g_502244;

// @retail 0x1392f8
void function_1392f8(s_player_appearance const *appearance)
{
	s_player_appearance const *const *appearance_reference = &appearance;
	color3f colors[4];

	function_7f790(NONE, false, appearance, colors);
	for (long i = 0; i < 4; i++)
	{
		g_502234[i] = pack_color3f(&colors[i]);
	}
	((s_new_hud_globals *)g_510c4c)->appearance = **appearance_reference;
	g_502244 = 4;
}

struct s_new_hud_player
{
	byte unknown00[0x84];
	s_player_appearance appearance;
};

// @retail 0x1392c5
void function_1392c5(long player_index)
{
	((s_new_hud_globals *)g_510c4c)->player_index = player_index;
	if (player_index != NONE)
	{
		s_new_hud_player *player = (s_new_hud_player *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
		function_1392f8(&player->appearance);
	}
}

/* the per-user interface state in the game state (called by function_19170f) */
// @retail 0x139130
void function_139130(void)
{
	g_510c4c = (s_510c4c *)function_123d40("new hud", NULL, 0x1e4);
	memset(&g_4e6950, 0, sizeof(g_4e6950));
	function_13ac30();
}

struct s_name_buffer
{
	wchar_t name[256];
};

void function_08cc20(s_name_buffer *buffer, const wchar_t *name);

struct s_510c4c_view
{
	byte unknown000[0x1bc];
	byte *strings;
	byte unknown1c0[0x1d2 - 0x1c0];
	bool flag1d2;
};

// @retail 0x13934d
void function_13934d(s_name_buffer *buffer, long string_handle)
{
	byte *strings = ((s_510c4c_view *)g_510c4c)->strings;
	if (strings)
	{
		wchar_t const *name;
		switch (string_handle)
		{
		case 0xe42d:
			name = (wchar_t const *)(strings + 0x1bc);
			break;
		case 0xe42e:
			name = (wchar_t const *)(strings + 0x208);
			break;
		case 0xe42f:
			name = (wchar_t const *)(strings + 0x134);
			break;
		case 0xe430:
			name = (wchar_t const *)(strings + 0x176);
			break;
		default:
			name = NULL;
			break;
		}
		if (name)
		{
			function_08cc20(buffer, name);
			return;
		}
	}
	buffer->name[0] = 0;
}

long function_155760(long index);
bool function_155d60(long index);
extern long g_4b9ed8;
bool g_4f55e2;

// @retail 0x13939b
bool function_13939b()
{
	long index = g_4b9ed8;
	return (function_155760(index) != 3 || function_155d60(index)) &&
		function_155760(index) != 2 &&
		((s_510c4c_view *)g_510c4c)->flag1d2 &&
		!(g_4e6948->state == 1 ? g_4f55e2 : false);
}

short g_4b9dd4;
short g_4b9dd6;
extern short g_4b9dd0;
extern short g_4b9dd2;
byte function_016a90();

// @retail 0x13a690
long function_13a690(long mode)
{
	long result = 0;
	short width = g_4b9dd6 - g_4b9dd2;
	short top = g_4b9dd0;
	short bottom = g_4b9dd4;

	if (width < 640 || (short)(bottom - top) < 480)
	{
		if (width < 640 && (short)(bottom - top) < 480)
		{
			result = 2;
		}
		else
		{
			result = 1;
			if (function_016a90() && mode == 3)
			{
				result = 2;
			}
		}
	}

	return result;
}
struct s_510c4c_fade_view
{
	byte unknown000[0x1d8];
	real current;
	real target;
	real rate;
};

long function_1469f0(real seconds);

struct s_interface_pulse
{
	char delay;
	char ticks;
	char repeats;
	byte unknown03;
};

// @retail 0x200891
real function_200891(s_interface_pulse const *pulse, bool rising)
{
	(void)&rising;
	real fraction;
	if (!pulse->ticks)
		fraction = 1.0f;
	else
		fraction = (real)pulse->ticks * g_510c54->rate * 1.5151515007019043f;
	double result;
	if (rising)
		result = fraction;
	else
		result = fraction - 0.5f;
	result *= 2.0f;
	if (result < 0.0f)
		result = 0.0f;
	else if (result > 1.0f)
		result = 1.0f;
	return result;
}

// @retail 0x2008fc
real function_2008fc(s_interface_pulse const *pulse, bool enabled)
{
	(void)&enabled;
	real result;
	if (enabled)
		result = function_200891(pulse, true);
	else
		result = 1.0f;
	return result;
}

PRIVATE __forceinline s_new_hud_user *pulse_user(long index)
{
	return &((s_new_hud_globals *)g_510c4c)->users[index];
}

// @retail 0x2003dc
void function_2003dc(long user_index)
{
	char *ticks = (char *)pulse_user(user_index) + 0x2d;
	for (long i = 0; i < 9; i++, ticks += 4)
	{
		if (ticks[-1] > 0)
		{
			if (--ticks[-1] == 0)
			{
				if (--ticks[1] > 0)
					ticks[-1] = (char)function_1469f0(0.5f);
				else
					ticks[0] = (char)function_1469f0(0.66f);
			}
			else if (ticks[0])
			{
				ticks[0] += 2;
				if ((real)ticks[0] * g_510c54->rate > 0.66f)
					ticks[0] = 0;
			}
		}
		else if (ticks[0] > 0)
			ticks[0]--;
	}
}

// @retail 0x13b306
void __stdcall function_13b306(real target, real seconds)
{
	if (seconds == 0.0f)
	{
		s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
		data->current = target;
		data->target = target;
		data->rate = 0.0f;
	}
	else
	{
		long ticks = function_1469f0(seconds);
		if (ticks <= 1)
			ticks = 1;
		s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
		data->target = target;
		data->rate = (target - data->current) / ticks;
	}
}

/* moves the value towards its target at its rate, stopping there */
// @retail 0x13b285
void function_13b285()
{
	s_510c4c_fade_view *data = (s_510c4c_fade_view *)g_510c4c;
	if (data->target > data->current)
	{
		data->current += (real)fabs(data->rate);
		if (data->current > data->target)
		{
			data->current = data->target;
			data->rate = 0.0f;
		}
	}
	else if (data->current > data->target)
	{
		data->current -= (real)fabs(data->rate);
		if (data->target > data->current)
		{
			data->current = data->target;
			data->rate = 0.0f;
		}
	}
}

/* the conditions of an interface element: masks of which one must match
   and none of the other may */
struct s_condition_masks
{
	word required[4];
	word excluded[4];
	byte minimum_value;
	byte minimum_a;
	byte minimum_b;
};

struct s_condition_subject
{
	byte unknown00[6];
	short a;
	short b;
	byte unknown0a[6];
	real value;
};

// @retail 0x13ac87
bool function_13ac87(s_condition_masks const *masks, word first, word second, word fourth, word third, s_condition_subject const *subject)
{
	if (subject)
	{
		if (masks->minimum_value > subject->value)
		{
			third |= 8;
		}
		else
		{
			third &= ~8;
		}
		if (subject->a < masks->minimum_a)
		{
			third |= 0x10;
		}
		else
		{
			third &= ~0x10;
		}
		if (subject->b < masks->minimum_b)
		{
			third |= 0x20;
		}
		else
		{
			third &= ~0x20;
		}
	}

	if ((masks->required[0] & first) || (masks->required[1] & second) || (masks->required[2] & third) || (masks->required[3] & fourth))
	{
		if (!(masks->excluded[0] & first) && !(masks->excluded[1] & second) && !(masks->excluded[2] & third) && !(masks->excluded[3] & fourth))
		{
			return true;
		}
	}
	return false;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

struct s_ammunition_state
{
	byte unknown00[8];
	short rounds;
	byte unknown0a[2];
	short magazine;
	byte unknown0e[2];
	real charge;
	byte unknown14[0xd];
	bool flag21;
};

struct s_ammunition_definition
{
	byte unknown00[0x1a];
	short maximum_rounds;
	real minimum_charge;
};

/* the state an ammunition counter shows */
static __forceinline long function_13b084(s_ammunition_state const *arg_1)
{
    real volatile local_1 = *(real volatile const *)&arg_1->charge * 100.0f;
    return _mm_cvtt_ss2si(_mm_set_ss(local_1));
}

// @retail 0x13b083
long function_13b083(s_ammunition_state const *state, long definition_index)
{
	long result = NONE;
	if (definition_index != NONE)
	{
		s_ammunition_definition *definition = (s_ammunition_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		if (*(short volatile const *)&state->magazine == 0 && 100 - PIN(function_13b084(state), 0, 100) == 0)
		{
			result = 4;
		}
		else if (*(short volatile const *)&state->magazine == 0 && definition->minimum_charge >= (1.0f - state->charge) * 100.0f)
		{
			result = 3;
		}
		else if (state->rounds == 0)
		{
			result = 2;
		}
		else if (state->rounds > definition->maximum_rounds || state->flag21)
		{
			result = 7;
		}
		else
			result = 1;
	}
	return result;
}

void function_1a0180(long tag_index, long string_handle, word *buffer);

/* copies one of the HUD's message strings into a buffer of 0x100 characters */
// @retail 0x13925f
void function_13925f(long string_handle, word *buffer)
{
	s_hud_globals_definition *definition = g_510c94;

	buffer[0] = 0;
	if (definition && definition->string_list != NONE)
	{
		function_1a0180(definition->string_list, string_handle, buffer);
	}
}

struct s_weapon_status_magazine
{
	bool active;
	bool idle;
	short loaded;
	short loaded_maximum;
	short unloaded;
	short total_maximum;
};

struct s_weapon_status
{
	real value;
	real heat;
	bool flag8;
	bool flag9;
	byte unknown0a[2];
	real fraction;
	bool charging;
	bool target_available;
	bool target_charging;
	bool target_ready;
	real target_fraction;
	point3f target_position;
	short magazine_count;
	s_weapon_status_magazine magazines[2];
};


void function_100520(long weapon_index, s_weapon_status *status);

struct s_hud_weapon_record
{
	long object_index;
	bool valid;
	byte unknown05;
	short loaded;
	short unloaded;
	short loaded_maximum;
	short total_maximum;
	byte unknown0e[2];
	real heat;
	real value;
	real fraction;
	byte unknown1c[4];
	bool flag20;
	bool flag21;
	bool flag22;
	bool flag23;
	bool flag24;
	bool flag25;
	bool flag26;
	byte unknown27;
	real target_fraction;
	point3f target_position;
};

// @retail 0x13afa9
void function_13afa9(long object_index, s_hud_weapon_record *record, long *definition_out)
{
	s_13b164_object *object = ((s_13b164_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_weapon_status status;
	function_100520(object_index, &status);
	record->flag20 = status.flag8;
	record->loaded = status.magazines[0].loaded;
	record->loaded_maximum = status.magazines[0].loaded_maximum;
	record->unloaded = status.magazines[0].unloaded;
	record->total_maximum = status.magazines[0].total_maximum;
	record->flag21 = status.magazines[0].active;
	record->flag22 = status.flag9;
	record->flag23 = status.charging;
	record->flag24 = status.target_available;
	record->flag25 = status.target_charging;
	record->value = status.value;
	record->heat = status.heat;
	record->fraction = status.fraction;
	record->object_index = object_index;
	record->flag26 = status.target_ready;
	record->valid = true;
	record->target_fraction = status.target_fraction;
	record->target_position = status.target_position;
	*definition_out = *(long *)((byte *)g_4e3b44[(*(long *)object) & 0xffff].data + 0x2b4);
}


struct s_13a532
{
    byte field_0[8];
    long field_8;
    byte *field_c;
    long field_10;
    byte *field_14;
    byte field_18[8];
    long field_20;
    byte *field_24;
};

struct s_text_widget;
struct s_text_widget_state;
bool function_22a9bc(s_text_widget const *arg_1, long arg_2,
    s_text_widget_state const *arg_3, color4f const *arg_4);
bool function_22b7dd(byte const *arg_1, long arg_2);
bool function_22af8d(long arg_1, byte const *arg_2);
struct s_22b8e2;
void function_22b8e2(s_22b8e2 const *arg_0, long arg_1, color4f const *arg_2);
void function_22aff1(long arg_1, byte const *arg_2, byte const *arg_3, real const *arg_4);
void function_22aa16(long arg_1, byte const *arg_2, byte const *arg_3, real const *arg_4);

// @retail 0x13a3c6
bool function_13a3c6(long arg_1, long arg_2, byte const *arg_3, long arg_4)
{
	bool local_1 = true;
	s_13a532 const *local_2 = (s_13a532 const *)g_4e3b44[arg_1 & 0xffff].bytes;
    word local_3 = 0;
    word local_4 = 0;
    word local_5 = 0;
    word local_6 = 0;
    s_13ad48_item const *local_7;
    function_13a050(arg_3, arg_2, arg_4, &local_7, &local_3, &local_4, &local_5, &local_6);
    if (local_2->field_20 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_20;
        do
        {
            byte const *local_9 = local_2->field_24 + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_5, (s_condition_subject const *)local_7))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
                bool local_12 = function_22b7dd(local_9, arg_2);
                if (!local_12) local_1 = local_12;
            }
            local_8 += 0x50;
        }
        while (--local_11);
    }
    if (local_2->field_8 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_8;
        do
        {
            byte const *local_9 = local_2->field_c + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_5, (s_condition_subject const *)local_7))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
                bool local_12 = function_22af8d(arg_2, arg_3);
                if (!local_12) local_1 = local_12;
            }
            local_8 += 0x64;
        }
        while (--local_11);
    }
    if (local_2->field_10 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_10;
        do
        {
            byte const *local_9 = local_2->field_14 + local_8;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_5, (s_condition_subject const *)local_7))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, local_7);
                bool local_12 = function_22a9bc((s_text_widget const *)local_9, arg_2, (s_text_widget_state const *)arg_3, (color4f const *)local_10);
                if (!local_12) local_1 = local_12;
            }
            local_8 += 0x54;
        }
        while (--local_11);
    }
	return local_1;
}

// @retail 0x13a532
void function_13a532(long arg_1, long arg_2, byte const *arg_3, long arg_4)
{
	s_13a532 const *local_2 = (s_13a532 const *)g_4e3b44[arg_1 & 0xffff].bytes;
    word local_3 = 0;
    word local_4 = 0;
    union s_13a533 { dword field_0; word field_1; };
    s_13a533 local_5;
    local_5.field_0 = 0;
    word local_6 = 0;
    function_13a050(arg_3, arg_2, arg_4, (s_13ad48_item const **)&arg_4, &local_3, &local_4, &local_5.field_1, &local_6);
    if (local_2->field_20 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_20;
        do
        {
            byte const *local_9 = local_2->field_24 + local_8;
            word local_12 = (word)*(dword volatile *)&local_5.field_0;
            s_condition_subject const *local_13 = (s_condition_subject const *)*(long volatile *)&arg_4;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_12, local_13))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, (s_13ad48_item const *)arg_4);
                function_22b8e2((s_22b8e2 const *)local_9, arg_2, (color4f const *)local_10);
            }
            local_8 += 0x50;
        }
        while (--local_11);
    }
    if (local_2->field_8 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_8;
        do
        {
            byte const *local_9 = local_2->field_c + local_8;
            word local_12 = (word)*(dword volatile *)&local_5.field_0;
            s_condition_subject const *local_13 = (s_condition_subject const *)*(long volatile *)&arg_4;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_12, local_13))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, (s_13ad48_item const *)arg_4);
                function_22aff1(arg_2, arg_3, local_9, local_10);
            }
            local_8 += 0x64;
        }
        while (--local_11);
    }
    if (local_2->field_10 > 0)
    {
        long local_8 = 0;
        long local_11 = local_2->field_10;
        do
        {
            byte const *local_9 = local_2->field_14 + local_8;
            word local_12 = (word)*(dword volatile *)&local_5.field_0;
            s_condition_subject const *local_13 = (s_condition_subject const *)*(long volatile *)&arg_4;
            if (function_13ac87((s_condition_masks const *)(local_9 + 8), local_3, local_4,
                local_6, local_12, local_13))
            {
                real local_10[4];
                function_13ad48(arg_2, local_9 + 4, local_10, arg_3, (s_13ad48_item const *)arg_4);
                function_22aa16(arg_2, arg_3, local_9, local_10);
            }
            local_8 += 0x54;
        }
        while (--local_11);
    }
}

// @retail 0x1393f3
bool function_1393f3(long arg_1, byte *arg_2)
{
	bool local_1 = true;
	((s_510c4c_view *)g_510c4c)->strings = arg_2;
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x8) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x8), arg_1, arg_2, 4)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0xc) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0xc), arg_1, arg_2, 1)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x10) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x10), arg_1, arg_2, 2)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x18) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x18), arg_1, arg_2, 0)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x0) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x0), arg_1, arg_2, 0)) local_1 = false;
	}
	if (*(long const *)(arg_2 + 0x4) != NONE)
	{
		if (!function_13a3c6(*(long const *)(arg_2 + 0x4), arg_1, arg_2, 0)) local_1 = false;
	}
	return local_1;
}

// @retail 0x1394b9
void function_1394b9(long arg_1, byte *arg_2)
{
	((s_510c4c_view *)g_510c4c)->strings = arg_2;
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3);
	}
	if (*(long const *)(arg_2 + 0x8) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x8), arg_1, arg_2, 4);
	}
	if (*(long const *)(arg_2 + 0xc) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0xc), arg_1, arg_2, 1);
	}
	if (*(long const *)(arg_2 + 0x10) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x10), arg_1, arg_2, 2);
	}
	if (*(long const *)(arg_2 + 0x14) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x14), arg_1, arg_2, 3);
	}
	if (*(long const *)(arg_2 + 0x18) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x18), arg_1, arg_2, 0);
	}
	if (*(long const *)(arg_2 + 0x0) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x0), arg_1, arg_2, 0);
	}
	if (*(long const *)(arg_2 + 0x4) != NONE)
	{
		function_13a532(*(long const *)(arg_2 + 0x4), arg_1, arg_2, 0);
	}
}

void function_13992a(byte *arg_1, long arg_2);
void function_2003dc(long arg_1);
void function_13aa27(long arg_1, byte const *arg_2, byte *arg_3, long arg_4, byte const *arg_5);
bool function_14ddc0(long arg_1);
void function_13b285(void);
long players_first_active_local_player(void);
long function_14de10(long arg_1);

__forceinline long function_13a839(long const *arg_1)
{
    return *(long const volatile *)arg_1;
}

// @retail 0x13a838
void __stdcall function_13a838(long arg_1)
{
    long local_13 = function_13a839(&arg_1);
    byte *local_1 = (byte *)g_510c4c + local_13 * 0x6c;
    byte local_2[0x270];
    function_2003dc(local_13);
    function_13992a(local_2, local_13);
    function_13a720(local_13, (s_13a720_status const *)local_2);
    if (local_2[0x130])
    {
        real local_3 = local_2[0x133] ? 1.0f : 0.0f;
        if (*(real *)(local_1 + 0x28) != local_3)
        {
            real local_4 = 2.0f / g_510c54->field_2_3;
            if (local_3 > *(real *)(local_1 + 0x28))
            {
                real local_5 = *(real *)(local_1 + 0x28) + local_4;
                *(real *)(local_1 + 0x28) = local_5 > 1.0f ? 1.0f : local_5;
            }
            else
            {
                real local_5 = *(real *)(local_1 + 0x28) - local_4;
                *(real *)(local_1 + 0x28) = local_5 > 0.0f ? local_5 : 0.0f;
            }
        }
    }
    long local_6 = NONE;
    if (function_14ddc0(local_13))
    {
        long local_7 = function_14de70(local_13);
        local_6 = *(long *)(g_4e8c24->data + (local_7 & 0xffff) * 0x21c + 0x2c);
    }
    if (local_6 != NONE)
    {
        byte const *local_8 = *(byte **)(g_4e0300->data + (local_6 & 0xffff) * 12 + 8);
        real local_9 = *(real *)(local_8 + 0xf0);
        if (*(real *)local_1 > local_9)
        {
            if (*(real *)(local_1 + 4) < 0.0f || *(real *)(local_1 + 4) > 1.0f)
                *(long *)(local_1 + 8) = g_510c54->game_time;
            long local_10 = *(long *)(local_1 + 8);
            if (0.5f > (g_510c54->game_time - local_10) * g_510c54->rate)
                *(real *)(local_1 + 4) = 0.0f;
            else
            {
                *(real *)local_1 = local_9;
                *(real *)(local_1 + 4) += (g_510c54->game_time - local_10) * g_510c54->rate;
            }
        }
        else
        {
            bool local_11 = local_9 > *(real *)local_1;
            *(real *)local_1 = local_9;
            if (local_11)
                *(real *)(local_1 + 4) = -1.0f;
            else if (*(real *)(local_1 + 4) > 0.0f)
                *(real *)(local_1 + 4) += (g_510c54->game_time - *(long *)(local_1 + 8)) * g_510c54->rate;
            *(long *)(local_1 + 8) = g_510c54->game_time;
        }
    }
    long local_12 = *(long *)(local_2 + 0xc);
    if (local_12 != NONE)
        function_13aa27(local_13, local_2 + 0x88, local_1 + 0x18, local_12, local_2);
    local_12 = *(long *)(local_2 + 0x10);
    if (local_12 != NONE)
        function_13aa27(local_13, local_2 + 0xc0, local_1 + 0x20, local_12, local_2);
}

// @retail 0x1391ed
void function_1391ed(void)
{
    word local_1 = 0;
    function_13b285();
    for (long local_2 = players_first_active_local_player(); local_2 != NONE; local_2 = function_14de10(local_2))
    {
        local_1 |= 1 << local_2;
        function_13a838(local_2);
    }
    for (long local_3 = 0; local_3 < 4; local_3++)
    {
        if (!(local_1 & (1 << local_3)))
        {
            s_new_hud_user *local_4 = &((s_new_hud_globals *)g_510c4c)->users[local_3];
            function_22beb5(local_3, 0, (s_interface_sound_block *)((byte *)g_510c94 + 0x418),
                local_4->unknown50, (word *)&local_4->unknown68);
        }
    }
}
