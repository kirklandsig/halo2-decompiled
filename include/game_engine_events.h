/* GAME_ENGINE_EVENTS.H: a game engine event (unknown_19de80.cpp). An event
   names a cause (a player and a team) and an effect (a player and a team);
   the engines (unknown_072c70.cpp, unknown_2bbf50.cpp, juggernaut.cpp) build
   one and send it. */

#ifndef GAME_ENGINE_EVENTS_H
#define GAME_ENGINE_EVENTS_H

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* 0x24 bytes */
struct s_event
{
	long type;
	long subtype;
	long a;
	long cause_player_index;
	long cause_team;
	long effect_player_index;
	long effect_team;
	long f;
	short g;
};

/* the players (0x21c bytes each), as the events see them */
struct s_event_player
{
	short unknown00;
	word flags;             /* 0x14d8b0 tests its high byte */
	byte unknown04[0x28 - 0x4];
	short local_index;
	byte unknown2a[0x44 - 0x2a];
	word name[0x10];
	byte unknown64[0xc0 - 0x64];
	char team;
	byte unknownc1[0x170 - 0xc1];
	long respawn_ticks;
	byte unknown174[0x1ac - 0x174];
	short lives;
	byte unknown1ae[0x21c - 0x1ae];
};

static inline s_event_player *event_player_get(long player_index)
{
	return (s_event_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_event_player));
}

void function_a7c50(s_event *event);
void function_19eb30(s_event *event);

/* the bodies of 0x19ebb0, 0x19ec10 and 0x19eb90: unknown_19de80.cpp is built
   /Ob1, so its own callers call them, but retail inlines them into 0x162d00 */
inline void game_engine_event_initialize_inline(s_event *event, long type, long subtype)
{
	event->type = type;
	event->subtype = subtype;
	event->a = NONE;
	event->cause_player_index = NONE;
	event->cause_team = NONE;
	event->effect_player_index = NONE;
	event->effect_team = NONE;
	event->f = 0;
	event->g = NONE;
}

inline void game_engine_event_set_effect_player_inline(s_event *event, long player_index)
{
	event->effect_player_index = player_index;
	event->effect_team = event_player_get(player_index)->team;
}

inline void game_engine_event_send_inline(s_event *event)
{
	if (g_4e6948->mode != 4)
	{
		function_a7c50(event);
		function_19eb30(event);
	}
}

s_event *game_engine_event_initialize(long type, long subtype, s_event *event);
void game_engine_event_set_cause_player(long player_index, s_event *event);
void game_engine_event_set_effect_player(s_event *event, long player_index);
void function_19eb90(s_event *event);

#endif
