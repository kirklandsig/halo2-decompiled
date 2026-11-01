// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1600F0.CPP: the end of the game engine code (0x1600f0..0x163080):
   queries on the multiplayer globals (g_4e9ae8) and the current engine
   object (g_55e4d0), and the time text the engines draw */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "engine_peer.h"
#include <string.h>
struct short_rectangle2d;
void function_1fa50(short_rectangle2d const *bounds, word const *text, void const *clip, void const *position, long line_gap, real scale, long color, void const *shadow);

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define FLOOR(n, floor) ((n) < (floor) ? (floor) : (n))

/* the multiplayer globals' fields read here that globals.h does not name */
struct s_mp_globals_view
{
	byte unknown000[0xf4];
	byte flagsf4;
	byte unknown0f5[0x304 - 0xf5];
	byte unknown304[4];
};

/* the players (0x21c bytes) */
struct s_game_engine_player
{
	byte unknown000[0x28];
	short local_user_index;
	byte unknown02a[0xc0 - 0x2a];
	char team;
	byte unknown0c1[0x21c - 0xc1];
};

/* the objects, as read here */
struct s_game_engine_object
{
	long definition_index;
};

struct s_game_engine_object_header
{
	byte unknown00[8];
	s_game_engine_object *object;
};

struct s_game_engine_object_definition
{
	byte unknown000[0x290];
	short unknown290;
};

real function_242140(long object_index);
void function_1a0180(long tag_index, long string_handle, word *buffer);

static inline c_engine_peer *game_engine_get(void)
{
	return g_55e4d0[g_4e9ae8->engine_index];
}

__forceinline s_game_engine_player *score_player_get(long player_index)
{
	s_game_engine_player *result = NULL;
	if (player_index != NONE)
	{
		result = &((s_game_engine_player *)g_4e8c24->data)[player_index & 0xffff];
	}
	return result;
}

// @retail 0x161b60
bool function_161b60(long player_index)
{
	c_engine_peer *engine = game_engine_get();
	bool result = true;

	if (engine && player_index != NONE)
	{
		s_game_engine_player *player = score_player_get(player_index);
		short local_user_index = player->local_user_index;

		if (local_user_index != NONE)
		{
			real fraction = (real)g_4e9af0.timers[local_user_index] * g_510c54->rate * 4.0f;

			if (PIN(fraction, 0.0f, 1.0f) >= 1.0f)
			{
				result = false;
			}
		}
	}
	return result;
}

struct s_161c90
{
	byte unknown00[0x44];
	long type;
};

// @retail 0x161c90
long function_161c90(s_161c90 const *p)
{
	long result = 1;

	switch (p->type)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 7:
	case 8:
	case 9:
		result = 2;
		break;
	case 5:
	case 6:
		break;
	}
	return result;
}

/* 0x161e10 and 0x161eb0 are in unknown_15e410.cpp, 0x161e60 and 0x162030 in
   unknown_161e60.cpp */

// @retail 0x161ef0
void function_161ef0(long string_handle, word *buffer)
{
	s_tag_header_globals *globals = g_4e034c;

	if (globals && globals->index != NONE)
	{
		long string_list_index = *(long *)(*(byte **)(g_4e3b44[globals->index & 0xffff].bytes + 4) + 0x1c);

		if (string_list_index != NONE)
		{
			function_1a0180(string_list_index, string_handle, buffer);
		}
	}
}

// @retail 0x162420
void function_162420(void)
{
	long i;

	g_4e9ae8->value24 = NONE;
	g_4e9ae8->value28 = NONE;
	for (i = 0; i < 16; i++)
	{
		g_4e9ae8->slots[i] = NONE;
	}
}

// @retail 0x162b10
bool function_162b10(long object_index)
{
	bool result = false;

	if (g_4e6948->mode_180 == 9 && object_index != NONE)
	{
		s_game_engine_object *object = ((s_game_engine_object_header *)g_4e0300->data)[object_index & 0xffff].object;
		s_game_engine_object_definition *definition = (s_game_engine_object_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;

		if (definition->unknown290 == 1)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x162b70
real function_162b70(long object_index)
{
	if (g_4e6948->mode_180 == 9)
	{
		return function_242140(object_index);
	}
	return 0.0f;
}

// @retail 0x163040
bool function_163040(long index)
{
	bool result = false;

	if (game_engine_get())
	{
		result = (((s_mp_globals_view *)g_4e9ae8)->flagsf4 & (1 << index)) != 0;
	}
	return result;
}

// @retail 0x1630b0
void function_1630b0(long *values, long value)
{
	long i;

	for (i = 0; i < 16; i++)
	{
		values[i] = value;
	}
}

word *function_1630e0(word *buffer, const word *format, ...);

// @retail 0x161be0
void game_engine_format_time(long seconds, word *text)
{
	word minutes_text[0x100];
	word seconds_text[0x100];
	long minutes = seconds / 60;
	long remainder = seconds - minutes * 60;

	minutes_text[0] = 0;
	seconds_text[0] = 0;
	if (minutes == 0)
	{
		function_1630e0(minutes_text, (const word *)L" ");
	}
	else
	{
		function_1630e0(minutes_text, (const word *)L"%d", minutes);
	}
	if (remainder <= 9)
	{
		function_1630e0(seconds_text, (const word *)L"0%d", remainder);
	}
	else
	{
		function_1630e0(seconds_text, (const word *)L"%d", remainder);
	}
	function_1630e0(text, (const word *)L"%s:%s", minutes_text, seconds_text);
}
/* the players, as the respawn code reads them */
struct s_game_engine_respawn_player
{
	byte unknown000[2];
	word flags;
	byte unknown004[0x28 - 0x4];
	short local_user_index;
	byte unknown02a[2];
	long unit_index;
	byte unknown030[0x190 - 0x30];
	long respawn_time;
	byte unknown194[0x1b0 - 0x194];
	long spectated_player_index;
	byte unknown1b4[0xc4 + 0x100 - 0x1b4];
};

struct s_game_engine_player_view
{
	byte unknown000[2];
	word flags;
	byte unknown004[0x28 - 0x4];
	short local_user_index;
	byte unknown02a[0xc4 - 0x2a];
	bool active;
};

void function_b58c0(long index, dword mask);

static inline s_game_engine_respawn_player *game_engine_player_get(long player_index)
{
	return (s_game_engine_respawn_player *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
}

/* the multiplayer globals' per player entries (0x18 bytes at +0x558) */
struct s_game_engine_player_info
{
	bool active;
	byte unknown01[3];
	point3f position;
	short timer_active;
	short timer;
	char state;
	byte state_ticks;
	char next_state;
	byte unknown17;
};

// @retail 0x162a30
void function_162a30(long player_index)
{
	long index = player_index & 0xffff;
	s_game_engine_player_info *info = (s_game_engine_player_info *)&g_4e9ae8->players[index];

	if (g_4e6948->mode != 4)
	{
		long previous = info->state;

		if (info->state_ticks != 0 && --info->state_ticks == 0)
		{
			if (info->next_state != 0)
			{
				real ticks;
				long ticks_long;

				info->state = info->next_state;
				ticks = g_510c54->field_2_3 * 0.5f;
				__asm
				{
					fld ticks
					fistp ticks_long
				}
				info->state_ticks = (byte)ticks_long;
				info->next_state = 0;
			}
			else
			{
				info->state = 0;
				info->state_ticks = 0;
			}
		}
		if (previous != info->state && game_engine_get())
		{
			long slot = g_4e9ae8->slots[(short)index];

			if (slot != NONE)
			{
				function_b58c0(slot, 4);
			}
		}
	}
	if (info->timer_active != 0)
	{
		long time = info->timer - 1;

		time = time > 0 ? time : 0;
		info->timer = (short)time;
		if ((short)time == 0)
		{
			info->active = false;
			info->timer_active = 0;
			info->timer = 0;
		}
	}
}

// @retail 0x162bf0
void function_162bf0(long player_index, long spectated_player_index)
{
	s_game_engine_respawn_player *player = game_engine_player_get(player_index);

	if (player->spectated_player_index == NONE)
	{
		player->spectated_player_index = spectated_player_index;
		if (game_engine_get())
		{
			long slot = g_4e9ae8->slots[(short)player_index];

			if (slot != NONE)
			{
				function_b58c0(slot, 0x40);
			}
		}
	}
}

byte *record_pool_lookup(s_record_pool *data, long datum_index);

// @retail 0x162c50
bool function_162c50(long player_index, long *spectated_player_index)
{
	bool result = false;
	s_game_engine_respawn_player *player = game_engine_player_get(player_index);

	if (player->unit_index == NONE)
	{
		long other_index = player->spectated_player_index;

		if (other_index != NONE)
		{
			s_game_engine_player_view *other = (s_game_engine_player_view *)record_pool_lookup(g_4e8c24, other_index);

			if (other && other->active && other->local_user_index == NONE && !(other->flags & 2))
			{
				long ticks = g_510c54->field_2_3 * 10;

				if (!(player->flags & 0x1000))
				{
					player->flags |= 0x1000;
					player->respawn_time = g_510c54->game_time;
				}
				if (g_510c54->game_time - player->respawn_time < ticks)
				{
					*spectated_player_index = other_index;
					result = true;
				}
				goto done;
			}
		}
	}
	player->flags &= ~0x1000;
	player->respawn_time = 0;
	done:
	return result;
}

/* an iterator over the players (0x19f240 skips the inactive ones) */
struct s_game_engine_player_iterator
{
	byte *datum;
	s_record_pool *data;
	long datum_index;
	long index;
};

bool player_iterator_next(long *iterator);

short g_4e9b38[8][16];

// @retail 0x162fd0
long function_162fd0(long index)
{
	s_game_engine_player_iterator iterator;
	long result = NONE;
	short best = 0x7fff;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next((long *)&iterator))
	{
		long value = g_4e9b38[index][iterator.datum_index & 0xffff];

		if (value >= 4 && value < best)
		{
			best = (short)value;
			result = iterator.datum_index;
		}
	}
	return result;
}

/* the voice state (lane D's network code): whether it runs, and per player
   the masks of the players heard */
extern bool g_4c99b8;
extern bool g_476fcc;
bool function_53750(long player_index);



static inline long local_user_next_162de0(long user_index)
{
	long result = NONE;
	long start = user_index == NONE ? 0 : user_index + 1;

	for (long i = start; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

static inline bool voice_hears_mask_162de0(long listener_index, long talker_index)
{
	dword muted = 0;

	if (g_527104.settings.initialized)
		muted = g_527104.settings.unknownc8[listener_index];

	dword heard = 0;

	if (g_527104.settings.initialized)
		heard = g_527104.settings.unknown108[listener_index];
	return ((muted | heard) & (1 << talker_index)) != 0;
}

static inline bool voice_talker_heard_162de0(long listener_index, long talker_index)
{
	if (!g_4c99b8 || !g_476fcc)
		return false;
	if (!function_53750(talker_index))
		return false;
	return voice_hears_mask_162de0(listener_index, talker_index);
}
/* counts, per local user, the frames each other player has been talking */
// @retail 0x162de0
void function_162de0(void)
{
	struct { dword talking; dword users; s_game_engine_player_iterator iterator; } state;
	long user_index;

	state.users = 0;

	for (user_index = local_user_next_162de0(NONE); user_index != NONE; user_index = local_user_next_162de0(user_index))
	{
		if (user_index != NONE)
		{
			long player_index = g_4e8c20->entries[user_index];

			if (player_index != NONE)
			{
				long other_index;

				state.iterator.index = NONE;
				state.iterator.datum_index = NONE;
				state.talking = 0;
				state.iterator.data = g_4e8c24;
				while (player_iterator_next((long *)&state.iterator))
				{
					long talker = state.iterator.datum_index;

					if (talker != player_index && *(short *)(state.iterator.datum + 0x28) == NONE)
					{
						if (voice_talker_heard_162de0(player_index & 0xffff, talker & 0xffff))
						{
							g_4e9b38[user_index][talker & 0xffff]++;
							state.talking |= 1 << (talker & 0xffff);
						}
					}
				}
				for (other_index = 0; other_index < 16; other_index++)
				{
					if (!((word)state.talking & (1 << other_index)))
					{
						g_4e9b38[user_index][other_index] = 0;
					}
				}
				state.users |= 1 << user_index;
			}
		}
	}
	short (*row)[16];

	for (user_index = 0, row = g_4e9b38; user_index < 4; user_index++, row++)
	{
		if (!((word)state.users & (1 << user_index)))
		{
			memset(row, 0, sizeof(*row));
		}
	}
}
/* game options fields read here */
struct s_game_options_time_view
{
	byte unknown000[0x190];
	long time_limit;
};

struct s_mp_globals_time_view
{
	byte unknown000[0x70];
	long start_time;
};

bool g_55e754;

#include "game_engine_events.h"
#include "data_array.h"

// @retail 0x162470
long function_162470(bool flag)
{
	long time_limit = ((s_game_options_time_view *)g_4e6948)->time_limit;
	long elapsed = FLOOR(g_510c54->game_time - ((s_mp_globals_time_view *)g_4e9ae8)->start_time, 0);
	long time_left = FLOOR(g_510c54->field_2_3 * time_limit - elapsed, 0);
	long previous_time_left = time_left;

	time_left = g_55e4d0[g_4e9ae8->engine_index]->p25(time_left, flag, true);
	if (flag)
	{
		if (time_left != previous_time_left)
		{
			if (!g_55e754)
			{
				s_event event;

				game_engine_event_initialize_inline(&event, 0, 0x25);
				if (g_4e6948->mode != 4)
				{
					function_a7c50(&event);
					function_19eb30(&event);
				}
				g_55e754 = true;
			}
		}
		else
		{
			g_55e754 = false;
		}
	}
	return time_left;
}

point3f *function_b9dd0(long object_index, point3f *result);
real function_11ce20(vector3f const *a, vector3f const *b);
bool player_iterator_next_with_unit(long *iterator);
long function_187450(long player_index);

struct s_game_engine_unit_view
{
	byte unknown000[0x88];
	vector3f velocity;
};

// @retail 0x161cd0
bool function_161cd0(long object_index)
{
	s_game_engine_player_iterator iterator;
	point3f position;
	bool result = false;

	function_b9dd0(object_index, &position);
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	while (player_iterator_next_with_unit((long *)&iterator))
	{
		long unit_index = ((s_game_engine_respawn_player *)iterator.datum)->unit_index;
		s_game_engine_unit_view *unit = (s_game_engine_unit_view *)((s_game_engine_object_header *)g_4e0300->data)[unit_index & 0xffff].object;
		vector3f direction;
		vector3f velocity = unit->velocity;
		point3f unit_position;

		function_b9dd0(unit_index, &unit_position);
		direction.i = position.x - unit_position.x;
		direction.j = position.y - unit_position.y;
		direction.k = 0.0f;
		velocity.k = 0.0f;
		if (direction.i * direction.i + direction.j * direction.j < 100.0f &&
			velocity.i * velocity.i + velocity.j * velocity.j > 2.25f &&
			function_11ce20(&velocity, &direction) < 0.2617994f)
		{
			result = true;
			break;
		}
	}
	return result;
}

struct s_game_engine_player_time_view
{
	byte unknown000[0x28];
	short local_user_index;
	byte unknown02a[0x1a0 - 0x2a];
	long target_index;
	byte unknown1a4[0x1aa - 0x1a4];
	short target_time;
};

// @retail 0x161f30
void function_161f30(long player_index)
{
	struct
	{
		long fast_ticks;
		real ticks;
		long slow_ticks;
	} rounding;
	s_game_engine_player_time_view *player = (s_game_engine_player_time_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long target_index = NONE;

	rounding.ticks = 256.0f / (g_510c54->field_2_3 * 0.25f);
	__asm
	{
		fld rounding.ticks
		fistp rounding.fast_ticks
	}
	rounding.ticks = 256.0f / g_510c54->field_2_3;
	__asm
	{
		fld rounding.ticks
		fistp rounding.slow_ticks
	}
	if (player->local_user_index != NONE)
	{
		target_index = function_187450(player->local_user_index);
	}
	if (player->target_index != target_index)
	{
		if (player->target_time > 0 && target_index == NONE)
		{
			player->target_time = player->target_time - rounding.slow_ticks > 0 ? player->target_time - rounding.slow_ticks : 0;
		}
		else if (player->target_time > 0 && target_index != NONE)
		{
			player->target_time = player->target_time - rounding.fast_ticks > 0 ? player->target_time - rounding.fast_ticks : 0;
		}
		if (player->target_time == 0)
		{
			player->target_index = target_index;
		}
	}
	else
	{
		long time = player->target_time;

		if (time + rounding.fast_ticks > 0x100)
		{
			time = 0x100;
		}
		else
		{
			time += rounding.fast_ticks;
		}
		player->target_time = (short)time;
	}
}
// @retail 0x1628f0
void function_1628f0(long player_index, char state)
{
	long index = player_index & 0xffff;
	s_game_engine_player_info *info = (s_game_engine_player_info *)&g_4e9ae8->players[index];

	if (g_4e6948->mode != 4)
	{
		long previous = info->state;

		if (state == 0)
		{
			info->state = 0;
			info->state_ticks = 0;
			info->next_state = 0;
		}
		else if (info->state == 0)
		{
			info->state = state;
			real duration_0 = g_510c54->field_2_3 * 0.5f;
			long converted_0;
			__asm
			{
				fld duration_0
				fistp converted_0
			}
			info->state_ticks = (byte)converted_0;
		}
		else if (state != info->state)
		{
			if (state != info->next_state)
			{
				if (info->next_state == 0)
				{
					info->next_state = state;
				}
				else
				{
					if (info->state != 3)
					{
						info->state = info->next_state;
						real duration_1 = g_510c54->field_2_3 * 0.5f;
			long converted_1;
						__asm
						{
							fld duration_1
							fistp converted_1
						}
						info->state_ticks = (byte)converted_1;
					}
					info->next_state = state;
				}
			}
		}
		else if (state == 3 || info->next_state != 3)
		{
			real duration_2 = g_510c54->field_2_3 * 0.5f;
			long converted_2;
			__asm
			{
				fld duration_2
				fistp converted_2
			}
			info->state_ticks = (byte)converted_2;
		}
		if (previous != info->state && game_engine_get())
		{
			long slot = g_4e9ae8->slots[(short)index];

			if (slot != NONE)
			{
				function_b58c0(slot, 4);
			}
		}
	}
}

class c_class_58d20;
struct s_session_machine_address;
long network_session_find_member_by_machine(c_class_58d20 *session, const s_session_machine_address *address);
bool network_session_host_boot_member(c_class_58d20 *session, long member_index);

/* the simulation watcher (g_4cf780), as read here */
struct s_162d00_watcher
{
	byte unknown00[0xc];
	c_class_58d20 *session;
};

// @retail 0x162d00
void game_engine_boot_player(long player_index)
{
	byte *player = datum_get_inlined(g_4e8c24, player_index);

	if (player)
	{
		s_event event;
		c_class_58d20 *session;
		long member_index;

		game_engine_event_initialize_inline(&event, 0, 0x26);
		game_engine_event_set_effect_player_inline(&event, player_index);
		game_engine_event_send_inline(&event);
		session = ((s_162d00_watcher *)g_4cf780)->session;
		member_index = network_session_find_member_by_machine(session, (s_session_machine_address const *)(player + 0x14));
		if (member_index != NONE)
		{
			network_session_host_boot_member(session, member_index);
		}
	}
}

/* the scenario's kill planes, as read here (the first one's height at +8) */
struct s_162b90_kill_plane
{
	byte unknown00[8];
	real height;
};

struct s_162b90_scenario_view
{
	byte unknown000[0x318];
	long kill_plane_count;
	s_162b90_kill_plane *kill_planes;
};

void function_10da60(long item_index, point3f *position);

/* true when an item fell below the scenario's kill height */
// @retail 0x162b90
bool function_162b90(long item_index)
{
	bool result = false;
	s_162b90_scenario_view *scenario = (s_162b90_scenario_view *)g_4e0350;

	if (scenario->kill_plane_count > 0)
	{
		s_162b90_kill_plane *kill_plane = scenario->kill_planes;

		if (kill_plane->height != 0.0f && item_index)
		{
			point3f position;

			function_10da60(item_index, &position);
			if (kill_plane->height > position.z)
			{
				result = true;
			}
		}
	}
	return result;
}

#include "unknown_163110.h"

/* a score row of the game engine's scoreboard: the place, the name, the
   score and the time */
struct s_game_engine_score_row
{
	s_text_widget_a place;
	s_text_widget_b name;
	s_text_widget_a score;
	s_text_widget_a time;
};

struct s_score_row_columns
{
	bool time;
	bool score;
};

struct s_160xxx_options_view
{
	byte unknown000[0x180];
	long mode;
};

/* builds a row of the scoreboard at a position */
// @retail 0x1600f0
void game_engine_score_row_build(short const *position, s_score_row_columns const *columns, color3f const *color, real alpha,
	word const *name, long place, long score, long seconds, bool dim, s_game_engine_score_row *row)
{
	long score_width;
	long time_width;
	long name_width;
	color4f local_e2f2b4;
	color4f field_24;
	s_short_rectangle bounds;
	s_text_buffer buffer;
	word time_text[0x100];

	buffer.text[0] = 0;
	time_text[0] = 0;
	score_width = columns->score ? 0x32 : 0;
	time_width = columns->time ? 0x36 : 0;
	name_width = 0xec - time_width - score_width;
	local_e2f2b4.alpha = alpha * 0.25f;
	local_e2f2b4.red = color->red;
	local_e2f2b4.green = color->green;
	local_e2f2b4.blue = color->blue;
	field_24.alpha = alpha;
	field_24.red = color->red * 0.3f + 0.7f;
	field_24.green = color->green * 0.3f + 0.7f;
	field_24.blue = color->blue * 0.3f + 0.7f;
	if (dim)
	{
		field_24.red *= 0.4f;
		field_24.green *= 0.4f;
		field_24.blue *= 0.4f;
	}

	text_buffer_format(&buffer, (const word *)L"%d", place);
	bounds.left = position[0];
	bounds.top = position[1];
	bounds.right = bounds.left + 0x19;
	bounds.bottom = bounds.top + 0x14;
	row->place.initialize(&bounds, &local_e2f2b4, &field_24, buffer.text, 6, true);

	bounds.left = position[0] + 0x19;
	bounds.top = position[1];
	bounds.right = bounds.left + (short)name_width;
	bounds.bottom = bounds.top + 0x14;
	row->name.initialize(&bounds, &local_e2f2b4, &field_24, name, 0x14, false);

	if (score_width)
	{
		text_buffer_format(&buffer, (const word *)L"%d", score);
		bounds.left = position[0] + (short)name_width + 0x19;
		bounds.top = position[1];
		bounds.right = bounds.left + (short)score_width;
		bounds.bottom = bounds.top + 0x14;
		row->score.initialize(&bounds, &local_e2f2b4, &field_24, buffer.text, 6, true);
	}
	else
	{
		row->score.valid = false;
	}

	if (time_width)
	{
		long mode = ((s_160xxx_options_view *)g_4e6948)->mode;

		if (mode >= 3 && (mode <= 4 || mode == 8))
		{
			game_engine_format_time(seconds, time_text);
		}
		else
		{
			function_1630e0(time_text, (const word *)L"%d", seconds);
		}
		bounds.left = position[0] + (short)score_width + (short)name_width + 0x19;
		bounds.top = position[1];
		bounds.right = bounds.left + (short)time_width;
		bounds.bottom = bounds.top + 0x14;
		row->time.initialize(&bounds, &local_e2f2b4, &field_24, time_text, 6, true);
	}
	else
	{
		row->time.valid = false;
	}
}

/* a player's row of the scoreboard: the place, the player's icon, the name,
   the score, the time and the connection quality */
struct s_game_engine_player_row
{
	s_text_widget_a place;
	s_text_widget_a icon;
	s_text_widget_b name;
	s_text_widget_a score;
	s_text_widget_a time;
	s_text_widget_c connection;
};

struct s_player_row_columns
{
	bool time;
	byte unknown01;
	bool score;
};

#include <wchar.h>

void function_159130(long seconds, word *buffer);
void function_22d2ee(word *string, long maximum_length);
void unicode_string_copy(word *destination, const word *source, long maximum_count);
bool function_15b2f0(void);
struct s_68a90_entry;
bool function_68a90(s_68a90_entry *entry, long *quality);

struct s_68a90_entry
{
	byte unknown00[0x10];
};

struct s_game_engine_globals_connections_view
{
	byte unknown000[0x6dc];
	s_68a90_entry connections[16];
};

/* builds a player's row of the scoreboard at a position */
// @retail 0x1603f0
void game_engine_player_row_build(short const *position, real alpha, long player_index, word const *name, long place,
	long score, long seconds, bool hide_connection, bool show_icon, bool hide_place, color3f const *color, bool dim,
	s_game_engine_player_row *row, s_player_row_columns const *columns)
{
	long score_width;
	long time_width;
	long name_width;
	long quality;
	color4f local_e2f2b4;
	color4f field_24;
	s_short_rectangle bounds;
	s_text_buffer buffer;
	word name_text[0x100];
	word parsed_name[0x100];
	word time_text[0x100];

	buffer.text[0] = 0;
	time_text[0] = 0;
	score_width = columns->score ? 0x32 : 0;
	time_width = columns->time ? 0x36 : 0;
	name_width = 0xd8 - time_width - score_width;
	local_e2f2b4.alpha = alpha * 0.25f;
	local_e2f2b4.red = color->red;
	local_e2f2b4.green = color->green;
	local_e2f2b4.blue = color->blue;
	field_24.alpha = alpha;
	field_24.red = color->red * 0.3f + 0.7f;
	field_24.green = color->green * 0.3f + 0.7f;
	field_24.blue = color->blue * 0.3f + 0.7f;
	if (dim)
	{
		field_24.red *= 0.4f;
		field_24.green *= 0.4f;
		field_24.blue *= 0.4f;
	}

	if (!hide_place)
	{
		text_buffer_format(&buffer, (const word *)L"%d", place);
		bounds.left = position[0];
		bounds.top = position[1];
		bounds.right = bounds.left + 0x19;
		bounds.bottom = bounds.top + 0x14;
		row->place.initialize(&bounds, &local_e2f2b4, &field_24, buffer.text, 6, true);
	}
	else
	{
		row->place.valid = false;
	}
	if (!hide_place)
	{
		bounds.left = position[0] + 0x19;
		bounds.top = position[1];
		bounds.right = bounds.left + 0x14;
		bounds.bottom = bounds.top + 0x14;
		row->icon.initialize(&bounds, &local_e2f2b4, &field_24, (const word *)L"", 1, false);
		if (show_icon && function_15b2f0())
			row->icon.text[0] = 1;
		else
			row->icon.text[0] = dim ? 2 : 0;
		*(long *)row->icon.unknown34 = player_index;
		row->icon.color_b.alpha = alpha;
	}
	else
	{
		row->icon.valid = false;
	}

	name_text[0] = 0;
	unicode_string_copy(name_text, name, 0x100);
	unicode_string_copy(parsed_name, name_text, 0x100);
	function_22d2ee(parsed_name, 0x100);
	unicode_string_copy(name_text, parsed_name, 0x100);
	/* the name width goes uncast into the name and score columns; a (short) there moves the score
	   width ahead of it in the time and connection sums */
	bounds.left = position[0] + 0x2d;
	bounds.top = position[1];
	bounds.right = bounds.left + name_width;
	bounds.bottom = bounds.top + 0x14;
	row->name.initialize(&bounds, &local_e2f2b4, &field_24, name_text, 0x14, false);

	if (score_width && !hide_place)
	{
		text_buffer_format(&buffer, (const word *)L"%d", score);
		bounds.left = position[0] + name_width + 0x2d;
		bounds.top = position[1];
		bounds.right = bounds.left + (short)score_width;
		bounds.bottom = bounds.top + 0x14;
		row->score.initialize(&bounds, &local_e2f2b4, &field_24, buffer.text, 6, true);
	}
	else
	{
		row->score.valid = false;
	}

	if (time_width && !hide_place)
	{
		function_159130(seconds, time_text);
		bounds.left = position[0] + (short)name_width + (short)score_width + 0x2d;
		bounds.top = position[1];
		bounds.right = bounds.left + (short)time_width;
		bounds.bottom = bounds.top + 0x14;
		row->time.initialize(&bounds, &local_e2f2b4, &field_24, time_text, 6, true);
	}
	else
	{
		row->time.valid = false;
	}

	row->connection.valid = false;
	if (!hide_connection)
	{
		quality = 1;
		if (function_68a90(&((s_game_engine_globals_connections_view *)g_4e9ae8)->connections[player_index & 0xffff],
			&quality))
		{
			bounds.left = position[0] + (short)time_width + (short)name_width + (short)score_width + 0x2d;
			bounds.top = position[1];
			bounds.right = bounds.left + 7;
			bounds.bottom = bounds.top + 0x14;
			row->connection.initialize(&bounds, &local_e2f2b4, &field_24, (const word *)L"", 2, true);
			row->connection.text[0] = (word)quality;
			row->connection.color_b.alpha = alpha;
		}
	}
}

#include "unknown_030290.h"
#include "unknown_07f720.h"
#include <xtl.h>

long function_11ca80(long value);
long function_19fd00(long team_index);
void utf8_string_to_utf16_string(char const *source, word *destination, long count);
bool function_15f120(long player_index, word *buffer, long size, long suppress_score);

bool function_15eaf0();

struct s_score_display
{
	long players[16];
	short teams[8];
	char player_ranks[16];
	char team_ranks[8];
	short player_count;
	short team_count;
};
void function_23f3e0(s_score_display *display, long mode, bool fallback);

struct s_string_reference
{
	long string_handle;
	long offset;
};
struct s_string_table
{
	s_string_reference *references;
	char *data;
	long count;
	long data_size;
	long references_offset;
	long data_offset;
	bool loaded;
	byte field_19[3];
};
char const *string_table_find(s_string_table *table, long first, long count, long handle);

struct s_score_player_row_1608e0
{
	s_game_engine_player_row row;
	bool selected;
	byte field_1c5[3];
};
struct s_score_panel_1608e0
{
	bool time;
	bool score;
	bool alternate_score;
	byte field_3;
	s_text_widget_d title;
	s_text_widget_b place;
	s_text_widget_b name;
	s_text_widget_b score_label;
	s_text_widget_b time_label;
	s_text_widget_b field_270;
	s_game_engine_score_row teams[8];
	long team_count;
	s_score_player_row_1608e0 players[16];
	long player_count;
};

PRIVATE inline long score_string_list_1608e0()
{
	long result = NONE;
	if (g_4e034c && g_4e034c->index != NONE)
	{
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		result = *(long *)(*(byte **)(definition + 4) + 0x1c);
	}
	return result;
}
// score 680 (was 680): frame 0x790 versus retail 0x79c; early player/panel roles, the scroll clamp (one
// limit conversion in retail) and coordinate/color homes remain.
// Earlier (score 689, was 689): baseline frame 0x78c versus retail 0x79c; early player/panel roles and coordinate/color homes remain.
// this round's reduced listing shows the frame at 0x790 against retail's 0x79c
// (2858 bytes against 2879), so every stack home is twelve bytes off and the absolute addresses are
// compared instead of masked; retail also computes the player index in ebx before the pushes, ours in
// edi after them.
// 689); the frame, the player and coordinate registers and the fade clamps still differ.
// the local player sits in EDI (retail EBX) and the
// x coordinate in EBX (retail EDI); the color locals' slots (retail keeps background below foreground)
// and the shared 0.0/1.0 registers of the fade clamps differ.
// Tried: player register and scalar homes; scalar names 817; header/name reuse 818; header/decoder
// reuse 818; both short team walks 811; short player walk 835; short outer team walk 818; short rank
// search 810; explicit scroll clamp 810; title workspace scope 810; bounds-field arithmetic 810;
// global optimization off 2237; size optimization 1348; viewer pointer name 810; viewer lookup at
// first scroll use 848; short displayed-row number 811; short row rank 810; direct team activity
// predicate 826; separate workspaces 816; ordinary locals 879; native player 810; team field
// reference 805 (plain); direct color output 844; native direct team field 820 (fast); viewer field
// at use 808; team score fields at use 805; player score fields at use 801 (plain); dword flag carrier
// 808; foreground aggregate construction 805; short/word total rows 843/843; native viewport height
// 815; existing horizontal/vertical/both position components 820/814/808; native team index 803;
// short team and row total 826; external flag reference 801; reused no-team result 802;
// player/team/both output cursors 811/834/842; chained RGB stores 801; native team-color output 835;
// existing string-list forceinline 801 (fast); existing table/list copy helpers,
// ordinary/inline/force-inline 801 each; new table/list helper boundaries with reused local-user
// index 836 each; separate score/time/both caption-list results 801/801/801; scoped name lookup
// alone/with score/with time/with both 801 each; all three separate immutable caption results 801;
// isolated score-table, row-total, height, fade-row, viewport-height, points-width, clock-width and
// label-width names 801 each; uniform fade fraction/offset/remainder and combinations
// 813,801,801,813,813,801,813; full-width initial predicates 785 plain (changed one-use captures
// excluded); isolated dim-icon/player-teams/activity/dim-icon-teams/all compound carriers
// 786,801,801,786,786; all compounds with captures removed 792; captures removed alone 806; dim/icon
// alone 801,789; coherent color/appearance/widget inline combinations and joint noinline 789 each;
// Boolean helper header/player/activity/header-player/header-activity/player-activity/all
// 789/804/789/804/789/804/804; byte helper counterparts 801/780/789/791/801/780/791; Boolean query
// at both player/both-row uses 766/766; byte result observed once 791; Boolean header with byte rows
// 780; shared fade/row-alpha/both: opening 789 each, byte graph 780 each, direct-query graph 766
// each; native icon byte-player/byte-rows/direct-query 791/791/777; repeated team-byte snapshot on
// direct queries 766 (frame 0x7a0); scroll clamp reading the global in PIN (no scroll local) 756
// (kept); that with height not volatile 800; with the clamp limit named 783; fade count not
// volatile 769; background renamed to sort after foreground 756; background declared first 756.
// Frame 0x798 versus 0x79c; viewer/coordinate/scalar homes remain.
// Earlier Frame 0x794/0x79c and viewer/coordinate/scalar homes remain.
// Earlier Frame 0x794 versus 0x79c and viewer/scalar homes remain; direct player-score fields keep native values.
// player register and scalar homes; scalar names 817;
// header/name reuse 818; header/decoder reuse 818; both short team walks 811;
// short player walk 835; short outer team walk 818; short rank search 810.
// explicit scroll clamp 810; title workspace scope 810; bounds-field arithmetic 810;
// global optimization off 2237; size optimization 1348.
// viewer pointer name 810; viewer lookup at first scroll use 848; short displayed-row number 811;
// short row rank 810; direct team activity predicate 826.
// Earlier frame 0x794 versus 0x79c; player/scalar homes remain. Separate workspaces 816; ordinary locals 879; native player 810; team field reference 805 (plain); direct color output 844; native direct team field 820 (fast).
// Viewer field at use 808; team score fields at use 805; player score fields at use 801 (plain);
// dword flag carrier 808; foreground aggregate construction 805.
// Short/word total rows 843/843; native viewport height 815; existing horizontal/vertical/both position components 820/814/808;
// native team index 803; short team and row total 826. Original executable source restored.
// Round 126: External flag reference 801; reused no-team result 802; player/team/both output cursors
// 811/834/842; chained RGB stores 801; native team-color output 835; existing string-list forceinline 801
// (fast); all restored.
// Round 332: Existing table/list copy helpers, ordinary/inline/force-inline 801 each;
// new table/list helper boundaries with reused local-user index 836 each. All restored.
// Round 166: Separate score/time/both caption-list results 801/801/801; scoped name
// lookup alone/with score/with time/with both 801 each; all three separate immutable
// caption results 801. Each result has its validity test and lookup use; original shared
// list and every header/row operation restored.
// External flag reference 801; reused no-team result 802; player/team/both output cursors
// Isolated score-table, row-total, height, fade-row, viewport-height, points-width, clock-width and label-width names 801 each.
// Round 137: uniform fade fraction/offset/remainder and combinations 813,801,801,813,813,801,813;
// full-width initial predicates 785 plain (changed one-use captures excluded);
// isolated dim-icon/player-teams/activity/dim-icon-teams/all compound carriers 786,801,801,786,786;
// all compounds with captures removed 792; captures removed alone 806;
// dim/icon alone 801,789. Icon alone retains frame 0x794;
// adding dim gives only three further points and frame 0x790, so that addition is rejected.
// Round 141: coherent color/appearance/widget inline combinations and joint noinline 789 each;
// 789/804/789/804/789/804/804; byte helper counterparts 801/780/789/791/801/780/791;
// Boolean query at both player/both-row uses 766/766; byte result observed once 791;
// Boolean header with byte rows 780. Shared fade/row-alpha/both: opening 789 each, byte graph
// 780 each, direct-query graph 766 each; native icon byte-player/byte-rows/direct-query
// 791/791/777; repeated team-byte snapshot on direct queries 766 (frame 0x7a0).
// Parallel long/int seconds 785/785; immutable conditional seconds 765; uninitialized
// seconds before colors/at row entry 785/785; long seconds with short place/team/both
// 782/768/765. One-point gains restored; original scalar widths and tests retained.
// Round 190: Existing language/team-label/caption inline declarations singly, in three pairs and
// together 766 each; all three separate noinline declarations 766. All original declarations,
// workspaces and body restored; frame 0x798/0x79c and viewer/scalar homes remain.
// Round 218: Completed collector counts cached, player/team/both long 795/782/832;
// native short 795/789/813; mixed short-long/long-short 832/813. All restored.
// Frame/viewer/coordinate homes remain; promoted player-only count recovers frame 0x79c but regresses.
// Repeated viewer field as short/long/reference 801/788/773; retained options receiver 793; viewer
// short/long/reference with options 825/789/803; viewer short and retained time mode 815.
// Frame/viewer/coordinate homes remain.
// Round 220: Complete header/team-localization/player-appearance lexical scopes, all seven
// subsets and all with read-only viewer/score receivers, 756 each. Original body restored.
// DH round 215: Row-geometry/team-RGB/player-RGB home identities, all seven subsets,
// and all three with the heading-rectangle identity 756 each. Frame/viewer/coordinate homes
// unchanged; every original operation and body restored.
// Round 30: Entry title/caption/team-localization arrays, all seven subsets, then entry collector
// output as well: 756 each; original declaration lifetimes restored.
// Round 88: user index local for the scroll clamp 776; scroll element reference 798; width folded
// into x 756; scores declared before the player 756; short screen height 759. Retail reads the
// rectangle's left word again for x where ours reuses the width's register.
// Round 169: Native score-table/team-element receivers, mutable/readonly combinations 772;
// immutable native score-table selection 799. One duplicate readonly source excluded.
// Original byte observations restored; frame/viewer/coordinate homes remain.
// Round 267: Complete rectangle edge/extent/position producers, value/pointer/reference outputs
// 776/712/688/711/688/711/744/678 (single-caller helpers restored). Five native rectangle
// objects with ordinary/readonly and scalar/position inputs 689/689/694/694; implicit native
// field conversions 689/689/694/694. Ordinary scalar-input objects kept, plain 689 at 2879 bytes.
// Round 394: Complete dim team-last/complementary-brightness/both 758/758/760; icon denial
// 756, with team-last/brightness/both 758/758/760; recurring flag field 756. Original body restored.
// Round 271: entry native rectangle declarations for title/fixed/optional caption groups,
// every subset and all with complete rectangle identities, 689 each. Original producers,
// native object lifetimes and body restored; frame 0x790 versus 0x79c remains.
// Round 403: native rectangles; ordinary height/fade storage and direct first-color
// output, masks 1-7: 734/713/753/726/764/742/783; all without unused parameter
// addresses 783. Original scalars, color copy and complete body restored.
// Round 203: Shared body-row sum/header bottom/both 689 each; all common column starts alone/with row
// sum/with bottom/with both 726 at 2826 bytes; complete graph with signed-word body-row sum 728. All
// actual expression locals are reused, but they do not recover frame 0x79c, viewer EBX, coordinate/color
// homes or clamp registers. Original complete body and five native rectangles restored.
// Round 419: separate title foreground 689; separate title background 689; separate title colors 689;
// separate team output 687; title foreground/team output 688; title background/team output 687; all
// appearance outputs 687; immutable title/all outputs 687. Frame/viewer/coordinate/color homes remain;
// one- and two-point reductions rejected. Original body restored.
// Round 428: native row-position horizontal/vertical/both 693/691/676; native title buffer 689;
// title with horizontal/vertical/both 693/691/676; all with promoted narrowed width 676.
// Coordinate-to-array forms restored without a match; original 0x790 frame and viewer/color homes remain.
// Round 42: pooled player fields at use 732; completed collector player index at use
// 689 (plain kept, 2879 bytes); word outer ordinals 690; pooled fields/index 721;
// pooled fields/word ordinals 726; index/word ordinals 703 (seven-point gain rejected);
// all three and native position initialization 753 each. Frame 0x790 versus 0x79c,
// viewer/coordinate/color homes remain; original pooled receiver and long walks retained.
// DL round 206: native score/time/both/name/score-name/time-name/all column widths
// 699/701/696/689/699/691/699; all with the existing player getter 721. The combined
// width form revisits the earlier pre-rectangle graph on the current 0x78c frame.
// Original long widths, native rectangles and body restored.
// Round 110: header place/name/score/time bounds as one rectangle filled field by field from
// row_position (0x1600f0's style; retail reuses one rectangle at its lowest slot and reads
// row_position for every header widget) 680 (kept, aligned 432/441 from 461/468). Score-width test
// written again inside the name width (retail evaluates `score || alternate` twice) 699 (2906 bytes,
// aligned 458/458). On the kept form: x/y as a two-short origin copied whole into row_position, title
// bounds from it (retail's dword copy at 0x10 -> 0x18) 729 (2906 bytes, aligned 420/426); height not
// volatile 721 (2858 bytes, aligned 411/426); height and fade count not volatile 752 (2826 bytes,
// aligned 405/428); title bounds field by field from x/y or from row_position 680/680; the origin
// point with neither volatile 720 (2858 bytes, aligned 391/406: the best structure so far).
// Round 25 (fast, equal to the full build here): retail keeps the fade count in its own slot (0x44, not
// reloaded), a dim byte (0x58, 0 stored early and 1 later), the masked player index (0x5c) and a byte with
// dead stores at each inlined 0x15eaf0 (0x17); its team-name block is the existing 0x19eae0 with 0x1a0180
// inlined in it (name[0] cleared twice). Dim as false then set 680; team name through 0x19eae0 874 (kept out
// of line, 2682 bytes); team activity through 0x15eaf0 681; dim false declared after the place 681; masked
// player index named for the pointer and the score lookups 717, with the early dim 708.
// Round 461: complete native viewport copy/const-copy/assignment with long/short
// screen height 687 each; field aggregate with long height 649 (plain kept), short
// height 718. Frame improves to 0x794; viewer/clamp/color homes remain, 2906/2879 bytes. Coordinator review: the
// field aggregate is a copy of the global kept only with a match (the natural copies give 687) and it moves the
// size off retail's, so the body is back to its earlier form (680).
// Round 265: library scroll/header-alpha/body-row-alpha bounds and their pairs/all
// 697/727/719/729/725/743/817; all with upper-first nesting 789. Body-row bounds
// recover frame 0x79c but regress to 719 at 2922 bytes; original full body restored.
// Round 38 (lane 15, fast; aligned = LCS-aligned differing instructions, baseline 628): fade count declared
// (uninitialized) at the top 680 (identical code); screen height computed before x 693 (aligned 588: the
// clamp limit is then read and converted once as retail, but the panel moves to EBX and left stays in BP);
// screen height first of all 693; the player lookup written at each of its four uses (no pointer local) 725;
// x/y into a two-short origin with row_position copied from it by memcpy and the title bounds from the
// origin, on the height-first order, 721 (2906 bytes). Original body restored.
// Round 41 (lane 14, fast): retail's dim predicate is the existing 0x15db30 inlined (its fresh player lookup and
// +0x2c/+0x1ac tests byte for byte) with `team == NONE` last; dim is cleared before the team search and set
// later; the row's raw and masked player indices are loaded once. Bool show_icon 695; with early dim 703; with a
// masked index local 715 (frame 0x794); `|| function_15db30(display.players[player_row]) || team == NONE` 698
// (2903 B), as early dim 695, with bool show_icon 688, plus a `row_player_index` local read once per row 698
// (frame 0x798, inlined call in retail's shape, but place takes EBX and the offset spills), int show_icon with
// the row local 680 (2879 B), with a named masked index too 700.
// @retail 0x1608e0
void __stdcall function_1608e0(long player_index, s_score_panel_1608e0 *panel, real alpha)
{
	(void)&player_index;
	(void)&panel;
	(void)&alpha;
	s_game_engine_player *local_player =
		&((s_game_engine_player *)g_4e8c24->data)[player_index & 0xffff];
	byte *scores = NULL;
	if (game_engine_get())
		scores = (byte *)g_4e9ae8 + 0x304;
	s_score_display display;
	function_23f3e0(&display, g_4e6948->flag1128 != false, true);
	long total_rows = display.player_count + display.team_count + 2;
	volatile long height = (display.team_count ? 0x15 : 0) +
		(display.player_count + display.team_count) * 0x14 + 0x2c;
	if (g_4e6948->flag1128)
		panel->time = true;
	else
		panel->time = g_4e6948->score_to_win != 1;
	if (*(long *)((byte *)g_4e6948 + 0x188))
	{
		panel->score = true;
		bool teams = false;
		if (game_engine_get())
			teams = g_4e6948->flags184.bit0;
		panel->alternate_score = !teams;
	}
	else
	{
		panel->score = false;
		panel->alternate_score = false;
	}
	short width = (short)(g_4b9dd8.right - g_4b9dd8.left);
	short x = (short)(width / 2 -
		g_4b9dd2 + g_4b9dd8.left - 0x86);
	long screen_height = (short)(g_4b9dd8.bottom - g_4b9dd8.top);
	if (height < screen_height)
		(real &)g_4e9af0.unknown04[4 + local_player->local_user_index * 4] = 0.0f;
	else
		(real &)g_4e9af0.unknown04[4 + local_player->local_user_index * 4] =
			PIN((real &)g_4e9af0.unknown04[4 + local_player->local_user_index * 4], 0.0f, (real)(height - screen_height));
	long y = (short)(g_4b9dd8.top -
		(long)(real &)g_4e9af0.unknown04[4 + local_player->local_user_index * 4] - g_4b9dd0);
	short row_position[2];
	row_position[0] = x;
	row_position[1] = y;
	real volatile fade_count = (real)total_rows;
	color4f foreground;
	color4f background = { 0.0f, 0.0f, 0.0f, 0.0f };
	real fade = (alpha - 0.0f / fade_count * 0.5f) * 2.0f;
	foreground.alpha = PIN(fade, 0.0f, 1.0f);
	foreground.red = 1.0f;
	foreground.green = 1.0f;
	foreground.blue = 1.0f;
	word title[0x50];
	function_15f120(player_index, title, 0x50, 0);
	s_short_rectangle title_bounds = { y, x, y + 0x14, x + 0x10c };
	panel->title.initialize(&title_bounds, &background, &foreground, title, 0x50, false);
	fade = (alpha - 1.0f / fade_count * 0.5f) * 2.0f;
	foreground.alpha = PIN(fade, 0.0f, 1.0f);
	foreground.red = 0.125f;
	foreground.green = 0.125f;
	foreground.blue = 0.125f;
	background.alpha = foreground.alpha;
	background.red = 1.0f;
	background.green = 1.0f;
	background.blue = 1.0f;
	long row_number = 2;
	long score_width = panel->score || panel->alternate_score ? 0x32 : 0;
	long time_width = panel->time ? 0x36 : 0;
	long name_width = 0xec - score_width - time_width;
	word text[0x100];
	text[0] = 0;
	row_position[1] = y + 0x18;
	s_short_rectangle bounds;
	bounds.left = row_position[0];
	bounds.top = row_position[1];
	bounds.right = bounds.left + 0x19;
	bounds.bottom = bounds.top + 0x14;
	panel->place.initialize(&bounds, &background, &foreground, (word const *)L"", 0x14, false);
	bounds.left = row_position[0] + 0x19;
	bounds.top = row_position[1];
	bounds.right = bounds.left + (short)name_width;
	bounds.bottom = bounds.top + 0x14;
	long list = score_string_list_1608e0();
	if (list != NONE) function_1a0180(list, 0x7000105, text);
	panel->name.initialize(&bounds, &background, &foreground, text, 0x14, false);
	if (score_width)
	{
		bounds.left = row_position[0] + (short)name_width + 0x19;
		bounds.top = row_position[1];
		bounds.right = bounds.left + (short)score_width;
		bounds.bottom = bounds.top + 0x14;
		list = score_string_list_1608e0();
		if (list != NONE) function_1a0180(list, 0x70000fd, text);
		panel->score_label.initialize(&bounds, &background, &foreground, text, 0x14, true);
	}
	else
		panel->score_label.valid = false;
	if (time_width)
	{
		bounds.left = row_position[0] + (short)score_width + (short)name_width + 0x19;
		bounds.top = row_position[1];
		bounds.right = bounds.left + (short)time_width;
		bounds.bottom = bounds.top + 0x14;
		list = score_string_list_1608e0();
		if (list != NONE) function_1a0180(list, 0x8000102, text);
		panel->time_label.initialize(&bounds, &background, &foreground, text, 0x14, true);
	}
	else
		panel->time_label.valid = false;
	panel->field_270.valid = false;
	for (long team_row = 0; team_row < display.team_count; ++team_row)
	{
		real row_fade = (alpha - (real)row_number / fade_count * 0.5f) * 2.0f;
		real row_alpha = PIN(row_fade, 0.0f, 1.0f);
		long team = display.teams[team_row];
		word name[0x100];
		name[0] = 0;
		byte *definition = g_4e3b44[g_4e034c->index & 0xffff].bytes;
		long team_names = *(long *)(*(byte **)(definition + 4) + 0xc);
		++row_number;
		if (team_names != NONE)
		{
			long handle = function_19fd00(team == NONE ? 8 : team);
			if (g_47ff38 == NONE)
				g_47ff38 = function_11ca80(XGetLanguage());
			long language = g_47ff38;
			byte *names = g_4e3b44[team_names & 0xffff].bytes;
			s_string_table *table = (s_string_table *)((byte *)g_4e034c + 0x188 + language * 0x1c);
			if (table->loaded)
			{
				word converted[0x100];
				char const *source = string_table_find(table, *(short *)(names + 0x10 + language * 4),
					*(short *)(names + 0x12 + language * 4), handle);
				utf8_string_to_utf16_string(source, converted, 0x100);
				wcsncpy((wchar_t *)name, (wchar_t *)converted, 0xff);
				name[0xff] = 0;
			}
		}
		row_position[1] += 0x14;
		color3f color;
		color = *function_7f720(&color, (short)team);
		byte *entry = scores + team * 0x12;
		short seconds;
		if (g_4e6948->flag1128)
			seconds = *(short *)(entry + 0x1c6);
		else
			seconds = *(short *)(entry + 0x1c4);
		bool active = false;
		if (game_engine_get())
		{
			byte teams = ((byte *)g_4e6948)[0x184] & 1;
			if (teams && team >= 0 && team < 8)
				active = (g_4e9ae8->wc & (1 << team)) != 0;
		}
		game_engine_score_row_build(row_position, (s_score_row_columns const *)panel, &color, row_alpha, name,
			display.team_ranks[team_row] / 2 + 1, *(short *)(entry + 0x1d2), seconds,
			!active, &panel->teams[team_row]);
	}
	panel->team_count = display.team_count;
	if (display.team_count) row_position[1] += 0x15;
	for (long player_row = 0; player_row < display.player_count; ++player_row)
	{
		real row_fade = (alpha - (real)row_number / fade_count * 0.5f) * 2.0f;
		real row_alpha = PIN(row_fade, 0.0f, 1.0f);
		byte *player = g_4e8c24->data + (display.players[player_row] & 0xffff) * 0x21c;
		++row_number;
		long place = display.player_ranks[player_row] / 2 + 1;

		char const &team = *(char *)(player + 0xc0);
		if (function_15eaf0() && team != NONE)
		{
			for (short team_row = 0; team_row < display.team_count; ++team_row)
				if (team == display.teams[team_row])
				{
					place = display.team_ranks[team_row] / 2 + 1;
					break;
				}
		}
		color3f colors[4];
		function_7f790(function_15eaf0() ? team : NONE, team == NONE, (s_player_appearance *)(player + 0x84), colors);
		color3f color = colors[0];
		word flags = *(word *)(player + 2);
		bool dim = (flags & 2) || !(flags & 1) || team == NONE ||
			(g_4e6948->value1b8 > 0 && *(long *)(player + 0x2c) == NONE &&
			*(short *)(player + 0x1ac) == 0);
		row_position[1] += 0x14;
		long show_icon = team != NONE && *(long *)(player + 0x2c) == NONE && !(flags & 2);
		short seconds;
		if (g_4e6948->flag1128)
			seconds = *(short *)(scores + (display.players[player_row] & 0xffff) * 0x1c + 6);
		else
			seconds = *(short *)(scores + (display.players[player_row] & 0xffff) * 0x1c + 4);
		s_player_row_columns *columns = (s_player_row_columns *)panel;
		game_engine_player_row_build(row_position, row_alpha, display.players[player_row], (word *)(player + 0x44), place,
			*(short *)(scores + (display.players[player_row] & 0xffff) * 0x1c + 0x12), seconds, (bool)((flags >> 1) & 1), show_icon, team == NONE,
			&color, dim, &panel->players[player_row].row, columns);
		panel->players[player_row].selected = display.players[player_row] == player_index;
	}
	panel->player_count = display.player_count;
}


#include "font_loading.h"
class c_1fa50
{
public:
    void function_1fb10(short_rectangle2d const *, real) const;
};
void function_163400(s_text_widget_a const *widget);
void connection_widget_draw(s_text_widget_c const *widget);
struct s_engine_border_state;
void function_15ff80(s_engine_border_state const *state);
void function_36580(void);
void function_36880(s_short_rectangle const *rectangle, color4f const *color);
void function_13ec70(color4f const *color);
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;


PRIVATE __forceinline void score_background_161420(byte const *widget, long valid)
{
    if (widget[valid])
        function_36880((s_short_rectangle const *)widget, (color4f const *)(widget + 0x14));
}

PRIVATE __forceinline void score_text_161420(byte const *widget, long valid)
{
    if (widget[valid])
    {
        g_4e73a0.style = NONE;
        g_4e73a0.flags = 0;
        g_4e73a0.justification = widget[8] ? 2 : 0;
        function_13ec70((color4f const *)(widget + 0x24));
        function_1fa50((short_rectangle2d const *)(widget + 0xa), (word const *)(widget + 0x38), NULL, NULL, 0, 1.0f, 0, NULL);
    }
}

// The text drawer 0x1fa50 is a free function taking the bounds before the text (0x1fa30's own register
// convention: bounds in EAX, text in ECX), so each text call loads the text before the bounds push, and the
// team-text loop is indexed like the other three (lane 5 round 110's finding).
// @retail 0x161420
void function_161420(long player_index, real alpha)
{
    s_score_panel_1608e0 panel;
    function_1608e0(player_index, &panel, alpha);
    for (long i = 0; i < panel.player_count; ++i)
        function_163400(&panel.players[i].row.icon);
    function_36580();
    byte const *data = (byte const *)&panel;
    score_background_161420(data + 0xe0, 0x60);
score_background_161420(data + 0x144, 0x60);
score_background_161420(data + 0x1a8, 0x60);
score_background_161420(data + 0x20c, 0x60);
score_background_161420(data + 0x270, 0x60);
    for (long team = 0; team < panel.team_count; ++team)
    {
        score_background_161420((byte const *)&panel.teams[team].place, 0x44);
        score_background_161420((byte const *)&panel.teams[team].name, 0x60);
        score_background_161420((byte const *)&panel.teams[team].score, 0x44);
        score_background_161420((byte const *)&panel.teams[team].time, 0x44);
    }
    for (long player = 0; player < panel.player_count; ++player)
    {
        byte const *row = (byte const *)&panel.players[player];
        if (*(short *)(row + 0x80) == 2) score_background_161420(row + 0x48, 0x44);
        score_background_161420(row, 0x44);
        score_background_161420(row + 0x90, 0x60);
        score_background_161420(row + 0xf4, 0x44);
        score_background_161420(row + 0x13c, 0x44);
        score_background_161420(row + 0x184, 0x3c);
        connection_widget_draw((s_text_widget_c const *)(row + 0x184));
        if (row[0x1c4]) function_15ff80((s_engine_border_state const *)row);
    }
    s_33a0b_default const *backdrop_tint = g_4686d4;
    color4f const *label_tint = g_4686cc;
    font_get(g_4e28f4[6]);
    g_4e73a0.font = 6;
    function_13ec70(label_tint);
    if (backdrop_tint)
    {
        g_4e73a0.shadow = true;
        g_4e73a0.field_24 = *(color4f const *)backdrop_tint;
    }
    else g_4e73a0.shadow = false;
    g_4e73a0.style = NONE;
    g_4e73a0.justification = 0;
    g_4e73a0.flags = 0;
    score_text_161420(data + 4, 0xd8);
    score_text_161420(data + 0xe0, 0x60);
score_text_161420(data + 0x144, 0x60);
score_text_161420(data + 0x1a8, 0x60);
score_text_161420(data + 0x20c, 0x60);
    for (long team = 0; team < panel.team_count; ++team)
    {
        score_text_161420((byte const *)&panel.teams[team].place, 0x44);
        score_text_161420((byte const *)&panel.teams[team].name, 0x60);
        score_text_161420((byte const *)&panel.teams[team].score, 0x44);
        score_text_161420((byte const *)&panel.teams[team].time, 0x44);
    }
    for (long player = 0; player < panel.player_count; ++player)
    {
        score_text_161420((byte const *)&panel.players[player].row.place, 0x44);
        score_text_161420((byte const *)&panel.players[player].row.name, 0x60);
        score_text_161420((byte const *)&panel.players[player].row.score, 0x44);
        score_text_161420((byte const *)&panel.players[player].row.time, 0x44);
        score_text_161420((byte const *)&panel.players[player].row.connection, 0x3c);
    }
}

