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
void function_19eae0(long team, word *buffer);
void utf8_string_to_utf16_string(char const *source, word *destination, long count);
bool function_15f120(long player_index, word *buffer, long size, long suppress_score);

bool function_15eaf0();
bool function_15db30(long player_index);

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
// score 679 (was 679): Frame 0x798 versus retail 0x79c. Viewer and coordinate homes and the scroll-limit conversion still differ.
// Earlier reduced check 678.
// Earlier reduced check scored 678.
// Earlier comparison 678 in the r718 fast check.
// MiniMax C (11-20): no new form, no lower score (679): the frame is still 0x798 against retail's 0x79c, the viewer stays in edi and the coordinate homes and the panel's timing reads still sit in this build's slots; the forms that reach retail's frame size score lower on other axes, so the body stands as found.
// Earlier score line, merged (679, was 679; 678 in the r718 fast check): Frame 0x798 versus retail 0x79c. Viewer and coordinate homes and the scroll-limit conversion still differ.
// P622: a one-use caption-phase local at the division gives 671 (match-only, reverted); frame 0x798 versus 0x79c, viewer and origin homes, and the initial divisor spill still differ.
// M3 lane 4 r226 (fast with 0x161420, 0x1600f0, 0x1603f0; control 679): x as long 684; y as short 681; the width folded into x's
// expression (no width local) 679 (same code: left is still read once); `panel->time` as one `flag1128 || score_to_win != 1`
// 701 (2888 bytes); the row total written into the fade count's initialiser (no total local) 710 (2888 bytes). Body unchanged.
// Earlier status line, merged in r127 (679, was 679): Frame 0x798 versus retail 0x79c. Viewer and origin slots,
// scroll-limit conversion and initial divisor spill still differ. Earlier fast check 678.
// One-use caption phase at the division 671 (requires a match, reverted).
// Frame 0x798 versus retail 0x79c. Viewer and coordinate homes,
// the scroll-limit conversion and the initial divisor spill still differ. Earlier reduced check 678.
// One-use caption-phase local at the division 671 (match-only, restored).
// MiniMax I (11-15): no new form, no lower score (679): the player's offset written as its own local compiles to
//   the same bytes as the expression, so the register roles are not reachable from the source here. What still
//   differs: retail scales the player index into ebx before the remaining pushes (mov ebx, [esp+0x7a4]; and ebx,
//   0xffff; imul ebx, ebx, 0x21c) and adds the unit base later, where this build does the whole chain in edi
//   after the pushes; our frame is 0x798 against retail's 0x79c and the homes differ (our scores pointer at
//   [esp+0x50] and the display at [esp+0x70], retail's at [esp+0x34] and [esp+0x74]), so every stack reference
//   in the body is off.
// MiniMax H (11-14): no new form, no lower score (679): retail scales the player index into EBX before the
// remaining pushes (mov ebx, [esp+0x7a4]; and ebx, 0xffff; imul ebx, ebx, 0x21c) and adds the unit base later,
// while this build does the whole chain in EDI after the pushes; writing the offset as its own local
// (`long player_offset = (player_index & 0xffff) * sizeof(s_game_engine_player);`) leaves the code byte for byte
// the same. Our frame is still 0x798 against retail's 0x79c and the homes differ (our scores pointer at
// [esp+0x50] and the display at [esp+0x70], retail's at [esp+0x34] and [esp+0x74]), so every stack reference in
// the body is off.
// MiniMax E (11-01): no new form, no lower score; left: the entry register roles (retail loads the
// player index first), the colour/bounds local homes and the missing bytes.
// Earlier at 679 (was 678 in the r718 fast check): frame 0x798 versus retail 0x79c; early player/panel roles, the scroll clamp (one
// Earlier 679 (was 678 in the r718 fast check): frame 0x798 versus retail 0x79c; early player/panel roles, the scroll clamp (one
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
// Round 476: On the shared-caption rectangle graph, title/caption/team-localization lifetime subsets 1-7 and all with decoder declaration at consumption: eight forms 680 each at 2879 bytes.
// Frame 0x790, viewer/panel roles, clamp and color/coordinate homes stay unchanged. Original body and all existing scalar qualifiers restored.
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
// Round w14E (walk against retail, no logic error found: every field, offset, condition direction, callee,
// argument and loop matches 0x1608e0..0x161416): the hand-expanded 0x15eaf0 (header alternate_score and the team
// activity test) and 0x15db30 (dim predicate, retail's inlined copy with `team == NONE` last) are now calls to the
// helpers 678 -> 678/679/703 one at a time; the row's player index read once into `row_player` (retail keeps
// the full value in [esp+0x38]) brings the three together to 679 at 2879 bytes. Dim set by `if (...) dim = true;`
// after an early `bool dim = false` 679, single expression 681. Inner team search with `long team_row` 702
// (2858 B; retail keeps a 32-bit counter but compares 16 bits). Volatile `height` / `fade_count`: retail keeps them in
// their own slots ([esp+0x28] read at 0x160a10/0x160a52, [esp+0x54] read at 0x160b8a/0x160ed6/0x161174), not
// a match, so Kirk may undo them (no volatile: 721/752 in round 110).
// m3 lane 5 r195 (fast = full 679; aligned / register-masked aligned counts in brackets, base [593, 221]). Retail's frame
// from a push-normalised slot map (offsets after the four saves): 0x10 an x/y point (x stored as soon as it is computed,
// before the scroll clamp; y after it) whose eight bytes later hold the header rectangle; 0x18 the row position, copied
// from the point as one dword (four unused bytes follow it in both builds); 0x20 the score width, later the row alpha;
// 0x27 the inlined team query's dead byte; 0x28 height; 0x30 the row number; 0x34 scores; 0x38..0x43 the row total, then
// the title rectangle (far edges read back from the point after the title-text call, near edges from the x/y registers),
// then the team row's colour and the time width; 0x44 background; 0x54 the fade count; 0x58 foreground; 0x68 dim; 0x6c
// the row's masked player index; 0x70 the display. That is 24 four-byte cells below the display against our 23 (frame
// 0x79c against 0x798): ours has no point and no masked-index cell, and our title rectangle shares the header
// rectangle's slot where retail's cannot (its point is still live there). A compile-only listing (no link step) names
// every local's slot and orders them by use density, lowest offset first; declaration order does not move them.
// Tried: an x/y point with x stored before the clamp, the row position copied from it whole and the title's far edges
// read from it 681 (2888 bytes) [571, 190]; that with background, fade count and foreground declared in retail's slot
// order 681 (same code); with height not volatile 721 (2856) [561, 192]; with the fade count not volatile either 738
// (2840) [554, 195]; with the screen height computed ahead of x as well 717 (2856) [554, 186]. The frame stays 0x798 in
// each; the aligned counts fall but the positional score does not. Body unchanged.
// P595: Frame, viewer register and early coordinate homes still differ. single late upper conversion
// 709, one-read scroll 710, branch stores 677, joined result 710, height before x with late conversion
// 679, height before x with one-read scroll 717, joined scroll reused for y 727, early height with
// scroll reused for y 730. NOT kept: the best two-point reduction leaves the four-byte frame gap and
// falls below the keep bar.
// DH401: Fade divisor initialized before flags, viewport, height or clamp.
// Existing qualifier 701/699/700/700, ordinary const 715/724/725/725.
// All keep the 0x798 frame. The ordinary divisor shortens the body.
// Round 624: native point with scalar row copy 693; whole point copy, title edges from copied row,
// and advance copied row 724/724/724. Early near edges in title rectangle 670, with far edges before title text
// 670, row copy after title initialization 670, and far edges at coordinate producers 670. NOT kept: those
// forms restore the 0x79c frame and viewer EBX by using title fields for the separate origin retail stores at +0x10.
// The separate early origin and viewer homes remain. Early horizontal cursor store 677;
// vertical cursor and advance 698; direct horizontal cursor 675; direct coordinate pair 689;
// aggregate cursor 698; title edges from cursor 698; early title bounds 689;
// caption from title top 689. NOT kept: best gain four, with the frame still 0x798.
// m3 lane 4 r213 (fast; aligned = difflib-aligned differing instructions / the same with registers and slots masked,
// 441 / 132 on the 679 body). LEAD 610 at retail's 2879 bytes (fast and plain; build/r213/lead-0x1608e0-610.patch in
// lane 4's worktree), not kept: it rests on statements and declarations placed in retail's block order and on the test
// for the score width written twice, forms kept only with a match (without those placements 733, with the widths
// written twice as well 721, so the gain does not stand on its own). The lead: no volatile height or fade count; the
// row number counted from 0 and divided by the converted row total at each use (four true divisions, the converted
// total spilled to its own slot as retail); the
// screen height ahead of x (retail loads bottom/top before x's division); an x/y point whose x is stored before the
// scroll clamp, the title edges read from it; the header advance right after the title call; the test for the score
// width inside the name width and again for the score width (retail's two branch sequences); team / player fetched ahead of
// the row alpha and `++row_number` after it (retail's block order); a long inner team counter; bool show_icon;
// `if (panel->team_count)` (retail tests the stored long); the row count named, team count first (retail's sum
// operand roles). With these the entry roles are retail's (player EBX, panel EBP to the header, screen height ESI,
// x EDI, height spilled on its own, name width EBX) at retail's 2879 bytes; aligned 367 / 66.
// Steps: height / fade count not volatile 723 / 707 / 755 (one reciprocal); total converted at each division 737;
// row counted from 0 738; + height ahead of x 722; + point 718; both 687 (frame 0x79c, +0x20); + widths twice 700;
// + field copy 655; + loops in retail's order 611; + row count 610.
// The whole-point copy (retail's dword copy) 694 / 693 / 686 in every spelling (aggregate assignment, memcpy, long
// cast, by-value or returning inline, a point with a constructor or an assignment operator): frame 0x79c, aligned
// 324 / 64 with a named masked index, but the copied dword is kept in a callee-saved register across the title-text
// call, so the panel loses EBP, the height takes ESI and the screen height EBP. Retail reloads the point after that
// call and keeps the panel. On that form: title edges or the advance read from the copy 693 (same code); title edge
// orders 686 each; advance late 683; widths name-last 600 (name width EBP) / time between 686; scores as a
// conditional 540 (2887 bytes, not retail's store order); team by value 702; long icon 691; short inner counter 686;
// display count test 706; (diagnostic) volatile height 685 / 679, with a named limit 601 (player EDI, x EBX);
// (diagnostic) extra panel stores inside the loops 681 / 689.
// On the lead: color initialised from the call 610 (same code); team name through 0x19eae0 815 (out of line);
// name cleared twice 610 (folded); named masked index 609 (aligned 378 / 71); team by value 634 (frame 0x7a8);
// seconds as a conditional 610 (same code); an entry pointer for the player scores 614, declared early 648; long
// show_icon 623 (frame 0x79c, 2860 bytes, aligned 277 / 48); title edges left/top/right/bottom 612,
// left/right/bottom/top 692; advance late 637; widths once 721; short inner counter 611; team after the alpha 613;
// player after the alpha 652; x ahead of the screen height 711; row count unnamed 611; the total as team + player
// with the height's sum player + team 610, the reverse 611; the height's constant first 611; the height from the
// total 691; the total's 2 first 611; `team_count != 0` 611; the height declared first 615; the height's team term
// as an `if` 695, last 611; renaming the score table, the height or the colors moves no slot. On the 700 step
// (point, whole copy, widths twice): a named clamp limit 619; an overflow local for the clamp 691; a named row
// count 700; the sum swapped 698; a scroll reference 683; the time flag as a conditional 674; a short screen
// height 701; the screen height ahead of the width 700; the score table as a conditional 637; the height's terms
// reordered 700; the height ahead of the total 702; bool show_icon 697; a short height 701; a short total 697;
// the total and height through an out-parameter helper 698, the height alone 702, a height pointer local 700;
// a local alias of the panel for the head 700 (same code). Open: the player loop keeps the masked index in EBX
// to the score lookups, so bool show_icon gets a frame byte; retail spills the masked index at the loop top,
// keeps the player offset in EBX to the inlined 0x15db30 and then show_icon in BL.
// Self-review ablation of the lead (each alone removed): the screen height after x 711; the team fetched after the
// alpha 613; the player fetched after the alpha 652; the header advance late 637; the widths once 721; all four
// placements and the increments back in main's order 733, those with the widths twice 721, with only the screen
// height first 741. Left on the lead: frame 0x7a0 against 0x79c, the whole-point copy, scratch and XMM rotation,
// the stack homes.
// m3 lane 4 r214 (fast, with 0x161420): the lead in retail's block order with one score-width test, 679 control,
// lead with the test twice 610: score and time widths then the name width 721 (2852 bytes, aligned 361 / 89); time
// width first 721; the name width as `0xec - time_width - score_width` (the sibling row builders' order) 721; as
// `0xec - (score_width + time_width)` 721; the score width set by an `if` 721; a bool for the score columns used for
// both widths 716. Retail tests the score columns twice and keeps one time-width value (the branchless test is
// shared, the branching one is not), so with one test the code is 27 bytes short. Not kept.
// Team gap shared with row advance 706, branch-selected 713, with guarded advance 706;
// shared team presence 706, with height accumulation 711; shared displayed count and gap
// 704; separate section heights 699, with shared gap 706. The presence forms recover
// the 0x79c frame with the viewer still in EDI. NOT kept: all eight variants add
// nine bytes and raise the score.
// The viewer register, early origin and separate late masked-index homes still differ in the four-byte frame gap.
// Timer-branch score record 681; pre-branch score record 691; shared mask with branch/pre-branch record
// 687/681; native points record 691, with shared mask 681; timer-field offset 717; native record ternary 681.
// The frame still lacks the separate origin word, and the viewer remains in EDI.
// Previous right edge for name/score/time 688/689/705; all three 724; initial caption aggregate 679;
// reused caption top/bottom 761, with all edges advanced 806; branch-produced full height 710;
// independent score visibility for name/caption 687, with explicit caption-width branches 687.
// Round 431: staged horizontal/vertical coordinates, title far edge from row cursor (bottom/right),
// common caption vertical extent/horizontal origin and separate title/caption fade locals all 679.
// The separate origin and viewer homes still differ. Original body restored.
// first division uses the converted count that initializes the retained divisor 687; first division
// uses the immutable converted count that initializes the retained divisor 687; title division uses
// the row total at its expression 687; separate title phase at the division 686; separate caption
// phase at the division 671; separate title and caption phases at both divisions 679; separate title
// delay at its scaled division 686; separate title and caption delays at both scaled divisions 686.
// M3 lane 12 r143 (fast with 0x1600f0, 0x1603f0, 0x161420; control 679), aimed at the scroll clamp (retail reads the user
// index in each arm, EDX then EAX, and converts the limit once after `0 > scroll`; ours hoists one index load above the
// branch and converts the limit twice): a `short user` read in each arm 694 (2888 B: the hoist is gone, two conversions
// remain); that with `real limit` named in the second arm 682 (2879 B: one conversion, but ahead of the zero test); the
// named limit alone 720 (2856 B); the per-arm index with height not volatile 723 (2856 B: PIN takes retail's shape, the
// index load is hoisted again and height stays in ESI); the screen width as `long` 679 (same code).
// Joined zero cases with the original or one converted limit 690/709, with a retained
// admitted scroll 709; nested lower-bound admission 685; default result with shared store
// 742; conditional height selection 687; joined zero result with shared store 710;
// admitted scroll adjusted before store 709. Frame and viewer register still differ.
// The viewer multiply still follows the panel save, leaving the viewer in EDI and the frame four bytes short.
// Separate byte displacement 679, explicit pool base and displacement 679, word viewer slot 714,
// record reference 679, word slot with record reference 714, typed array base 679, word slot
// with byte displacement 714, displacement with record reference 679.
// NOT kept: the word slot changes the prologue without recovering the viewer register or origin word.
// Staged short horizontal/full-width vertical/both/full-width horizontal/short vertical/both short origins
// 679/679/679/684/681/681; in the row position, horizontal 675 and both 689.
// NOT kept: staged row horizontal gains four, still above 30, with the same frame gap.
// MiniMax A (11-18): no new form, no lower score (679): the length agrees with retail's 2879 bytes, but the frame is
// 0x798 against retail's 0x79c; retail scales the player index into ebx and adds the player base before the panel work,
// while this build loads the panel into ebp first and scales the index into edi after the register saves, and the scores
// pointer's home is +0x50 here against retail's +0x34. Retail also keeps the computed x and y in their own word slots at
// +0x10 and +0x12 and copies the pair into +0x18 with one dword move; giving this build the same origin pair and a four
// byte copy into the row position measures 681 at 2888 bytes.
// Round 116 (m3 lane 13, fast with 0x161420, 0x1600f0 and 0x1603f0, plain-confirmed 679): the three `(void)&parameter;`
// statements removed (the same code, the caller still matches; kept as a clean-up); the scroll value as a `real &` bound
// to the global's element for the clamp, the store and y 740 (2840 bytes); x as `left + width / 2 - g_4b9dd2 - 0x86` 679,
// as `left + width / 2 - (g_4b9dd2 + 0x86)` 679, the width as `long` 679 (all the same code: retail reads the window's
// left edge twice, ours once into CX). Re-measured: height not volatile 723, fade count not volatile 707, both 755.
// Round 118 (m3 lane 13, fast with 0x161420 and 0x1600f0, both MATCH): retail reloads its x/y point (+0x10/+0x12) after
// the title-text call for the title's far edges and the row advance (the point's slot is later the header rectangle,
// passed by address), where ours keeps x and y in registers. Title far edges read from row_position 679, the advance as
// `row_position[1] += 0x18` 679, both 679, all four title edges from row_position 679 (all the same code); row_position
// initialised `{ x, (short)y }` 679, with far edges / far edges and += / all edges / += only 679 each; initialised with
// all four title edges from row_position and += 698 (2888 B: the player index then loads into EBX first as retail and
// row_position[1] is reloaded after the call, but the panel takes EBX and the frame stays 0x798).
// M3 lane 14 r120 (fast; control 679, 714 instructions against retail's 718; cut at the 20 calls and masked, 521
// differ with registers kept and 223 without). Between the title's two calls retail has 71 instructions to our 63: it
// keeps 1.0 in XMM2 for the fade division and both clamps (ours loads it three times), stores the row number 2 before
// the widths, computes the score width before the time width (ours the time width first) and advances the row's top in
// its register (`add edi, 0x18`, later `add edi, 0x19` for the left edge). Forms: `long row_number = 2;` ahead of the
// title's text call 685; right after the fade count 685; the row's top advanced in place (`y += 0x18; row_position[1]
// = y;`) 679 (same code); the time width declared before the score width 692 (2888 B, 711 instructions); the title's
// four edges from the row position 679 (same code); that with the in-place advance 679 (same code). Body unchanged.
// Saved horizontal origin through first/remaining/all captions with advance from
// origin or cursor 678/678, 700/700, 694/694. Copy origin after title, before caption
// fade or at caption geometry 680/681. NOT kept: the one-point gain adds nine bytes
// and leaves frame 0x798. Viewer and separate origin homes still differ.
// Named score-buffer member and typed receiver 679/679, typed conditional 719, native team offset 681,
// player score record 691, native team/player records 693, early player record 687, named player fields 691.
// The 0x798 frame and viewer in EDI remain in the retail-length forms.
// M3 lane 2 r115 (fast with 0x161420 and 0x1600f0, both MATCH each time; control 679, aligned 593 / 224 as exact /
// registers and slots masked; body unchanged). Lane 4 r213's lead re-measured on this tree 610 (aligned 531 / 111).
// New on the lead: retail keeps the panel origin twice, as scalars (x EDI, y BX, used for the title's near edges after
// the title-text call) and as an x/y point in memory at +0x10 that is copied whole to the row position before the fade
// (`mov eax, [esp + 0x10]; mov [esp + 0x18], eax`) and read back after that call for the far edges and the row advance.
// Scalars x/y, the point's members set from them, `row_position = origin`, title bounds `{ y, x, origin.y + 0x14,
// origin.x + 0x10c }` and `row_position.y = origin.y + 0x18`: 636 at 2879 bytes with retail's frame 0x79c and the first
// difference at +0x45 (aligned 423 / 116): the head is retail's up to the height and point homes (+0x2c / +0x30 for
// +0x28 / +0x10), but ours forwards the stored x/y to the far edges across the call (`lea ecx, [ebx + 0x14]`, `add edi,
// 0x10c`) where retail reloads the point from memory, so ours reuses the point's cell for the fade count. On that 636
// form, all the same code: y as short; `(void)&origin`; the copy through memcpy, through a pointer local or through a
// long cast; the far edges and the advance read from the row position. The point copied whole without the scalars 693.
// Both points as the two elements of one array 642. Diagnostics (reverted at once): the point's address stored to a
// global 654 at 2900 bytes (the far edges are then reloaded after the title-text call as retail's, but again after the
// title's own call, which retail does not do); the point overlaid on the header rectangle's first two members 640
// (no reload). So retail treats the title-text call, and only it, as writing the point; no source form found for it.
// The 636 form with main's statement order (alpha ahead of the team / player fetch, the increments and the header
// advance where main has them), one score-width test, the short inner counter, long show_icon and the display count
// test 716 (2856 bytes); only the one width test 720 (2849); only the loops in main's order 678 (2863); only the header
// advance late 690; only the three types 640; one width test, late advance and types 731; loops, late advance and
// types 700. Not kept: the 636 form rests on the lead's match-only pieces (the score-width test written twice and
// statements placed in retail's block order).
// Two-word origin at coordinate producers 693, from completed coordinates 693, row copy after title
// 690, original origin through caption bounds 693. Score-source selection first 713, fade phases on
// the row counter 679, published team-count test 681, one displayed-player count for traversal and
// publication 675.
// NOT kept: the displayed-player count recovers frame 0x79c at 2879 bytes and gains four, with viewer
// still in EDI and the independent origin missing.
// The top edge stays live across the scroll bounds, with the viewer and origin homes
// still displaced. Retained short top 679, long top 691, short top and height 693,
// retained left 692, retained top and left 692, vertical origin advance 691,
// early horizontal cursor with retained top 677, named scroll pixels 679.
// NOT kept: the early horizontal cursor gains two at retail length and leaves
// the 0x798 frame. The widened viewport forms grow nine bytes.
// M3 lane 13 r127 (fast with 0x161420, 0x1600f0, 0x1603f0; control 679; 0x1603f0 reads 253 in the reduced image with a
// callee caveat, control included, and matches in the full build). Reading: retail computes the width with a 16-bit
// `sub ax, [left]` and adds the left edge again from memory for x, with the screen height's dword loads between the two;
// ours keeps the left edge in CX for both. Forms: the score flags as `panel->score = count != 0; panel->alternate_score =
// panel->score && !...` 690; the first fade as `2.0f * (alpha - ...)` 679 (same code); the row count named once for the
// total and the height, player count first 679 (same code), team count first 678 (one lower, under the bar; the sum's
// registers turn retail's but nothing else moves); the background as `= { 0 }` 681. Body unchanged.
// MiniMax B (11-20): no new form, no lower score (679): the frame stays 0x798 against retail's 0x79c with the viewer
// still in EDI, and the top edge, the origin and the scroll bounds still have displaced homes; the forms that
// recover the frame gain four at most and need a moved declaration.
// M3 lane 7 r199 (fast): the player's local user index named once and used at the four scroll-offset uses 697; the
// scroll offset through a `real *` formed once 740 (2840 B), through a `real &` 740 (2840 B); the player pointer declared
// after the scores pointer 713 (2888 B), just before the first scroll-offset use 717 (2888 B).
// Caption rectangle holds the original horizontal/vertical/both edges, with row-copy or
// direct title far edges 694/694, 693/693, 714/714. Advance caption top from its original
// edge with vertical/both edges held 715/706.
// NOT kept: the first six recover frame 0x79c at higher scores. The horizontal forms also recover EBX for the viewer.
// The final two return to 0x798
// and shorten the body. The separate ordered origin and score pointer homes still differ.
// Team/player/both row fade quotients 679/679/679, team/player/both scaled offsets
// 679/679/679, both final scales 679, both quotients and final scales 679.
// The 0x798 frame, viewer register and separate origin slot still differ.
// M3 lane 14 r133 (fast with 0x161420, 0x1600f0, 0x1603f0; control 679): the scores pointer as one ternary initialiser 719
// (2888 B); `panel->time` as one ternary on flag1128 708 (2888 B); the screen height as a short 693 (2888 B); the width
// as a long 679 (same code); both score flags cleared first and set under the test with no else arm 694.
// Viewport width, height, both spans, horizontal origin, vertical origin, both origins, horizontal
// span with origin, and both spans with both origins as accumulators.
// Scores 679/693/693/679/679/679/679/693.
// NOT kept: Width and origin updates tie. Word height extends the body and leaves the frame and
// coordinate homes unresolved.
// Height from the row total minus caption rows 708, full row total with inset 708,
// full rows with conditional inset 709, body rows with full caption extent 708.
// Shared signed team count with body/full rows 707/707. Explicit signed player/team
// counts with body extent or conditional inset 707/708.
// NOT kept: all retain 0x798 and raise the score.
// M3 lane 11 r142 (fast with 0x161420/0x1600f0, MATCH; control 679, aligned 593; the fast image leaves six globals out): the
// player's user index read once into a `short` local 733 (2840 B); the scroll offset bound once as `real &` 740 (2840 B); the
// player as `data + index * sizeof` (the event-player getter's shape) 679 (same code). Measured, not changed: the height
// without its `volatile` 723 (2856 B), the fade count without it 707 (2856 B). Body unchanged.
// Separate title/caption fades, aggregate title color and both 679/679/679.
// Shared caption alpha alone, with separate fades, with aggregate color, with both
// or with immutable fades and aggregate color 681 each. Frame 0x798 and the
// viewer and origin homes still differ. Body restored.
// M3 lane 7 r207 (h2m fast with 0x161420, MATCH; control 679): the width computed ahead of the panel's time/score
// stores 707 (2888 B; it gives retail's second read of the window's left edge, the byte stores through the panel being
// possible aliases, but retail's width loads follow those stores); the screen height ahead of x 698; both 701; the
// width as long 679 (same code); the scroll element named once by reference (four uses) 740 (2840 B).
// M3 lane 11 r144 (h2m fast with 0x161420, 0x1600f0, 0x1603f0; control 679, aligned 593, masked 374): the window's left
// edge read as a word in x 679; x as `width / 2 - g_4b9dd2` then `+= left - 0x86` 679; the width through a file-local
// inline taking the rectangle by pointer or by value 679 / 679 (all the same code: the left edge stays in CX for both
// reads); the screen height computed between the width and x, or ahead of the width, 698 / 698 (2888 B, aligned 550,
// masked 355: the height takes ESI and the top edge ECX as retail, the user index is read in each scroll arm with no
// hoist, but the left edge then sits in BP and the panel leaves EBP); on main's body without the height's `volatile`
// (723, 2856 B): the screen height declared ahead of the row total, with x declared there too, or at the function's
// top 723 / 723 / 723 (same code: declaration order does not rank the height against the screen height), the screen
// height between the width and x or ahead of the width 718 / 718 (aligned 553). Reading: retail folds both reads of
// the left edge as memory operands because all seven registers are held there (panel EBP, viewer EBX, x EDI, screen
// height ESI, top edge ECX), and keeps the height in memory from its definition although ESI and EDI are still free at
// that point, so the height ranks under the screen height and x in retail and over them here.
// M3 lane 15 r129 (H2M_FAST with 0x161420, 0x1600f0; control 679): reading of retail's title block: x stays in DI and y
// in BX across the title-text call (0x15f120) for the title's near edges, and the x/y point at +0x10 is re-read after
// that call for the far edges, while the advance (`add edi, 0x18`) reuses the re-read y across the title's own call.
// So the point is memory that 0x15f120 may write and 0x163350 may not. Diagnostic on lane 2's point form (scalars plus
// point, 636 here): the point's address kept in a volatile local 553 (2881 B; the point moves to +0x2c and the far edges
// are still forwarded from the registers, so taking the address alone does not give the re-read; reverted at once).
// On this body: the row's masked player index named for the player record and the two score lookups 680; the scroll
// clamp's else arm as a local read once with the limit named after the zero test 710 (2856 B); x/y stored into a
// two-short array copied to the row position by memcpy, far edges and advance read from it 681 (2888 B); the screen
// height computed between the width and x 698 (2888 B; the window's left edge is still read once into a register,
// where retail reads it again for x).
// M3 lane 7 r209 (h2m fast with 0x161420, MATCH; control 679): the screen height ahead of x with the width folded
// into x's expression 698 (2888 B); the scroll clamp's upper limit named once in the else arm 720 (2856 B: one
// conversion as retail, but computed ahead of the lower test where retail converts after it); the clamp written as
// `else if (scroll < 0) scroll = 0; else { limit; if (scroll > limit) scroll = limit; }` 710 (2856 B); the named limit
// with the screen height ahead of the width 693 (2879 B); that with the fade count not volatile 709 (2856 B).
// M3 lane 16 r174 (h2m fast with 0x161420, 0x1600f0, 0x1603f0; [n] = masked aligned rows, control 679 [570]): the
// named limit re-measured 720 [567]; show_icon as a bool (the row builder's parameter type) 694 [525] (2861 B, frame
// 0x79c as retail: the long show_icon is the frame's missing four bytes, but the bool sits in a stack byte where
// retail keeps it in BL); that with the screen height ahead of the width 680 [506] (2879 B, the screen height in ESI as
// retail, the panel in EBX where retail has EBP), ahead of x 680 [506] (same code); that with the named limit 724
// [576] (2845 B); the bool with the named limit alone 708 [522]. Body unchanged.
// M3 lane 12 r173 (h2m fast with 0x161420, 0x1600f0 MATCH, 0x1603f0 253; control 679): the team colour declared with its
// call result 679; the three string-list lookups in their own named locals 679; the title and header fades in two named
// locals 679 (all the same code); the score and time widths as `= 0` then set in an if 693 (2888 B); the scores pointer
// as one conditional expression 719 (2888 B, first difference +0x0). Body unchanged.
// Existing player accessor for the viewer, row or both 721/685/719; viewer, row or both by reference
// 721/685/719; typed score member with viewer or both 721/719. NOT kept: the accessor preserves a
// sentinel branch after 16-bit masking and leaves the frame at 0x798.
// M3 lane 6 r230 (H2M_FAST with 0x161420, MATCH; control 679; lane 4 r213's LEAD re-applied, 610). Reading: retail's
// origin point at +0x10 shares its slot with the later header bounds rectangle; after the title-text call 0x15f120
// retail reloads x (dword at +0x10) and y (dword at +0x12) for the far edges and the advance, and keeps y across the
// title's own initialize call, so only the call that writes the local title buffer is treated as writing the point. On
// the lead: the origin as a two-short array 610 (same code); that copied whole into the row position ahead of the fade
// 693 (y is then reloaded after 0x15f120 as in retail, x kept in EBP from the copy). On this body: `y` declared short
// 681; the time flag as one `||` expression 701 (2888 B); the scroll clamp's arms swapped 676 (under the bar, an arm
// swap by score: not kept). Body unchanged.
// M3 lane 11 r153 (H2M_FAST with 0x161420, 0x1600f0 and 0x1603f0; control 679; matched 0x1603f0 reads 253 in the reduced
// image with every body unchanged, a caveat on its callee 0x68a90, and check.py --fast agrees): the score table as a
// conditional initialiser 719 (2888 B); the alternate-score flag set by an if/else on the call 682; the clamp test as
// `screen_height > height` 679; the title edges read from the row position 679; the three 1.0f foreground channels
// chained 679 (the last three the same code). Body unchanged.
// M3 lane 13 r147 (h2m fast with 0x161420, MATCH; control 679, difflib-aligned 124 groups): the height's `volatile`
// replaced by a `long const &` bound to a plain local 723 (2856 B, the same code as the plain height: the reference
// folds); the fade count's `volatile` replaced the same way 707 (2856 B, the plain fade count's code); both 755 (2824 B);
// a redundant `if (0 < display.team_count)` ahead of the team loop 691 (2888 B); one ahead of the player loop 679
// (aligned 121: only the row counter's zero store and one scratch register move). Body unchanged.
// M3 lane 14 r146 (H2M_FAST with 0x161420, MATCH; control 679, aligned 597, masked 222): the team loop reading the team
// row's entry at each use (no team local) 706 (2904 B); the player loop reading the row's player at each use 701 (2901 B);
// the language fetch through the existing get_current_language() 679 (same code). Retail's team-name block is the
// existing 0x19eae0 inlined (name[0] cleared twice): called as written 880 (2680 B, out of line); with `inline` on its
// definition 691 (2888 B, aligned 592, masked 220: team EBP and team row EBX as retail, 0x19eae0, 0x19e1c0 and 0x232d77
// unchanged; the row counter increment lands after the name block, retail's sits inside the helper's first block);
// that with the increment ahead of the call 691, with 0x19eae0's `team == NONE ? 8 : team` as a conditional argument
// 691, both 691. KEPT (maintainer decision 2026-11-23 on `inline` for 0x19eae0): the inlined 0x19eae0 call with the fade count no longer volatile 679 (2879 B, aligned 598,
// masked 225; the increment ahead of the call 679 too); with the height also plain 737 (2840 B), the height alone 723.
// M3 lane 2 r142 (H2M_FAST with 0x161420, 0x1600f0, 0x15f120 and 0x163350, MATCH each run; control 679; body unchanged).
// Lane 4 r213's lead rebuilt on this tree 610 (difflib-aligned 376, 72 with registers and offsets masked) and lane 6
// r230's scalar-plus-point form on it 636 (aligned 311, first +0x45). Reading of retail's title block: the near edges
// come from the x and y registers (DI, BX), the far edges and the header advance from two dword loads of the origin
// point made after the title-text call (`mov ecx, [esp + 0x10]`, `mov edi, [esp + 0x12]`), and the header reads the row
// position the same way (a word load for a plain copy, a dword load for a sum, and `bounds.left` read back as a dword
// right after its word store); retail's header rectangle sits at the origin's address and its right and bottom are
// summed from the row position, not from left and top. Tried on the 636 form for the origin's reload: the far edges
// and the advance through a const reference to the origin 636 (same code); through a pointer local 636 (same code);
// the advance written ahead of the title bounds 635; the title bounds through a file-local inline helper that takes the
// origin's address (top, left copied, bottom, right summed) 693, the same without the scalars 693 (the copied dword is
// then kept in a register across the title-text call, lane 4's whole-copy code); the origin as a point view of the
// header rectangle variable (diagnostic, a cast) 640 at frame 0x7a0, still no reload. So neither an address-taking
// inline helper nor sharing the header rectangle's storage makes this build read the origin again after that call;
// lane 6's diagnostic (the origin's address stored in a global) is still the only form that does.
// M3 lane 8 r231 (h2m fast with 0x161420, 0x1600f0 and 0x1603f0; control 679): the scroll clamp through a file-local inline
// function `pin_real(real x, real lower, real upper)` (the shape other files use) 734 (2840 B: the limit is then converted
// ahead of the zero test and the value loaded once, where retail tests the global first and converts in the else arm);
// that with the height not volatile 739 (2840 B); with the title and header fades through it too 734 (2840 B); only the two
// fades through it 679 (same code); the function with `real const &` parameters 738 (2840 B); an in-place form taking
// the scroll element by reference 734 (2840 B). Retail's clamp has the macro's lazy shape, so it stays a macro here.
// Typed title buffer 679; title and caption buffer 679; caption and team-name buffer 679;
// one text buffer for all three 679; shared title and caption rectangle 679; separate optional
// caption rectangles 679; shared row color 679; first palette color passed directly 721.
// NOT kept: the frame and viewer register stay displaced, and direct palette access shortens the body.
// M3 lane 11 r163 (H2M_FAST with 0x161420, 0x1600f0 and 0x1603f0; control 679, difflib-aligned 597; body unchanged).
// Reading of the entry: the allocator colours x (140) ahead of the viewer record (63), and x's range runs from its
// definition to the last header column because every later read of the row position is forwarded from the register, so
// only EBX is free for it and the viewer falls to EDI; retail's x ends at the title (it re-reads the stored point after
// the calls), takes EDI, and the viewer gets EBX with its index scaled ahead of the pushes. Diagnostic, removed at once:
// the row position handed to an undefined function at the end of the body (so that it counts as captured) 667 (2879 B):
// the viewer is then in EBX from +0xc and x in EDI as retail, the frame stays 0x798. So retail treats the row position
// as captured and this build does not. Forms: the title bounds' left and top stored ahead of the title-text call with
// right and bottom summed from them after it 679 (aligned 596); the title bounds' near edges read from the row position
// with the far edges summed from its own left and top 679 (same code); `inline` on 0x1600f0's definition 679, on
// 0x1603f0's 679, on both 679 (the same code each: the two row builders are not what captures the position).
// M3 lane 2 r146 (H2M_FAST with 0x161420, 0x1600f0, 0x1603f0, 0x15f120 and 0x163350; control 679, aligned 436, 149 with
// registers masked; 0x1603f0 reads 253 in the reduced image in every run, control included; body unchanged). Retail keeps
// the origin as a two-short point at +0x10 (x stored ahead of the scroll clamp, y after it), copies it whole into the row
// position at +0x18, and after the title-text call reads the far edges and the header advance from the point with
// double-word loads while the near edges still come from the x and y registers. On this body, with a file-local
// two-short point for the origin: x stored into it ahead of the clamp, y after it, the row position set by one aggregate
// copy (through a cast of the two-short array), the title's far edges and the header advance read from the point 675
// (aligned 442, masked 135: four points, and a cast only to copy); every title edge read from the point 704 (2888 B);
// the far edges from the point with the screen height ahead of x 695 (2888 B, masked 125); the far edges from the point
// and the advance from the y scalar 675 (same code as the first form); the row position as an aggregate initialiser
// `{ x, (short)y }` 679 (same code); the screen height ahead of the width's use with x stored into the row position
// before the clamp 685. Retail's two evaluations of the score-column test (once inside the name width, once for
// the score width in its own stack cell) are a computed value written at each use, so lane 4 r213's lead stays
// match-only.
// Caption rectangle holds horizontal origin. Named score member, read-only bytes, typed buffer and typed players each 688.
// Explicit availability branches 723. Typed teams, both tables and shared engine-state selection each 690. All 2856 B.
// Earlier caption rectangle 679; title rectangle beside row position 679;
// earlier foreground / foreground and caption rectangle 679/679. Height, scores and
// row-total names in two orders 679/679. Entry row and caption declarations, with or
// without separate title declaration, 679/679. Frame, viewer and origin homes still differ.
// Separate caption presence for score/time/both 710/682/710; signed-word widths
// alone or with score/time/both 679/710/688/709. Both with time width subtracted first
// 709. Frame, viewer and origin homes still differ.
// M3 lane AO r1 (fast; control 679): the player row's team byte as a plain `char` copy in place of the reference 725
// (2844 B); the player place as `short` 685; the team row's team as `short` 693 (2888 B); x as `long` 703 (2856 B); the
// width folded into x's expression 679, the row count named once for the total and the height 679 (same code each); the
// header's place and name rectangles with right and bottom summed from the row position 714 (2856 B), the place
// rectangle alone 679 (same code). Body unchanged.
// Panel time, score and alternate-score field references singly, in pairs,
// or all together 679 each; pointers to all three fields 679. NOT kept:
// the field bindings leave frame 0x798, viewer EDI and the origin homes unchanged.
// M3 lane 1 r168 (fast with 0x161420, 0x1600f0 and 0x1603f0; control 679): the player row's masked index named once
// for the player pointer and both score reads 684; the scroll element bound to a `real &` for the clamp and y 755
// (2824 B); the header advance stored right after the title call 685; the team colour initialised at its declaration
// 679; the player flags word read at each of its three uses 679. Body unchanged.
// The 0x798 frame, EDI viewer and independent origin home still differ.
// Title coordinate and caption names 679; viewport and content-height names 679;
// native signed count snapshots 678, also for the team gap 678; common column
// options record 679; branch-selected score/time widths 679/684; score flag
// feeding alternate visibility 709. NOT kept: the count forms gain one point.
// The separate origin word and viewer register remain different.
// Origin memmove at completed coordinates, staged horizontal coordinate,
// or caption setup, far edges or all edges and advance 690/720/690/672/726/721.
// Row-cursor memmove into the title origin 689/679.
// NOT kept: the best transfer scores 672 with frame 0x7a0.
// the separate origin copy requires a byte match.
// The 0x798 frame and independent origin still differ. Signed scroll extent
// with separate or shared row heights 740/746/746, admitted extent first 742,
// late limit 740, range bounds 741, existing height 740, and one scroll read 757.
// NOT kept: the signed extent moves subtraction ahead of the fit test,
// shortens the body and retains the four-byte frame gap.
// M3 lane 16 r191 (h2m fast; control 679, aligned 597, register-masked 227): a two-short origin set from x and y with
// the row position copied from it element by element and the title's far edges and the header advance read from it
// 679 (same code: the copies are forwarded); the row position filled from the origin by a 4-byte memcpy (retail's dword
// copy) 677 (masked 211: the viewer moves to EBX at entry as in retail, x to EDI); that with x stored into the origin
// ahead of the scroll clamp and y after it (retail's store order and branch placement) 675 (masked 209, four points,
// under the bar; the origin homes at +0x34 and is still not re-read after the title-text call); the far edges and the
// advance read from the row position instead 677; the 675 form with the height plain 737 (2840 B, aligned 567, masked
// 203); main's body with the height plain 737 (aligned 561). Retail also divides by the fade count at each fade (0/n,
// 1/n, row/n read from its stack cell) where this build shares one reciprocal. Body unchanged.
// M3 lane 12 r193 (H2M_FAST with 0x161420 and 0x1600f0, MATCH, 0x1603f0 253 in the reduced image; control 679): the team
// row's active test through the existing 0x161e10 770 (2794 B: called, not expanded); the team row's counter increment
// ahead of the name call 679 (same code); the team gap tested on the stored panel count 706 (2858 B); the row total's
// sum written team count first 678; the label text buffer declared at the top 679 (same code). Body unchanged.
// @retail 0x1608e0
void __stdcall function_1608e0(long player_index, s_score_panel_1608e0 *panel, real alpha)
{
	s_game_engine_player *local_player =
		&((s_game_engine_player *)g_4e8c24->data)[player_index & 0xffff];
	byte *scores = NULL;
	if (game_engine_get())
		scores = (byte *)g_4e9ae8 + 0x304;
	s_score_display display;
	function_23f3e0(&display, g_4e6948->flag1128 != false, true);
	long total_rows = display.player_count + display.team_count + 2;
	/* volatile: retail keeps the height in its own stack slot (see the note above) */
	volatile long height = (display.team_count ? 0x15 : 0) +
		(display.player_count + display.team_count) * 0x14 + 0x2c;
	if (g_4e6948->flag1128)
		panel->time = true;
	else
		panel->time = g_4e6948->score_to_win != 1;
	if (*(long *)((byte *)g_4e6948 + 0x188))
	{
		panel->score = true;
		panel->alternate_score = !function_15eaf0();
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
	real fade_count = (real)total_rows;
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
		function_19eae0(team, name);
		++row_number;
		row_position[1] += 0x14;
		color3f color;
		color = function_7f720((short)team);
		byte *entry = scores + team * 0x12;
		short seconds;
		if (g_4e6948->flag1128)
			seconds = *(short *)(entry + 0x1c6);
		else
			seconds = *(short *)(entry + 0x1c4);
		bool active = false;
		if (function_15eaf0() && team >= 0 && team < 8)
			active = (g_4e9ae8->wc & (1 << team)) != 0;
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
		long row_player = display.players[player_row];
		byte *player = g_4e8c24->data + (row_player & 0xffff) * 0x21c;
		++row_number;
		long place = display.player_ranks[player_row] / 2 + 1;
		bool dim = false;

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
		if ((flags & 2) || !(flags & 1) || function_15db30(row_player) || team == NONE)
			dim = true;
		row_position[1] += 0x14;
		long show_icon = team != NONE && *(long *)(player + 0x2c) == NONE && !(flags & 2);
		short seconds;
		if (g_4e6948->flag1128)
			seconds = *(short *)(scores + (row_player & 0xffff) * 0x1c + 6);
		else
			seconds = *(short *)(scores + (row_player & 0xffff) * 0x1c + 4);
		s_player_row_columns *columns = (s_player_row_columns *)panel;
		game_engine_player_row_build(row_position, row_alpha, row_player, (word *)(player + 0x44), place,
			*(short *)(scores + (row_player & 0xffff) * 0x1c + 0x12), seconds, (bool)((flags >> 1) & 1), show_icon, team == NONE,
			&color, dim, &panel->players[player_row].row, columns);
		panel->players[player_row].selected = row_player == player_index;
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

