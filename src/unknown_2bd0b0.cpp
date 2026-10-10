#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"
#include "engine_peer.h"
#include "unknown_13927e.h"

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_2BD0B0.CPP: the game engine whose vtable is at 0x45c878 (the third
   engine object at 0x47fc88): its slots 0..28, which unknown_1523c0.h numbers
   v22..v50 (its slots from 30 on are c_game_engine_a's in
   unknown_2bd960.cpp), and the helpers they use. The engine keeps a list of
   the distinct marker groups (marker types 11..18) to pick from at random,
   and its state (g_51ecc8, defined by unknown_2bd960.cpp) in the multiplayer
   globals at +0xfc. */

struct s_state_2bd;
struct s_polygon_2be;
extern s_state_2bd *g_51ecc8;

/* the players (0x21c bytes) */
struct s_player_2bd0
{
	short identifier;
	byte unknown02[0x2c - 2];
	long object_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x1b8 - 0xc1];
	short time_inside;
	byte unknown1ba[0x21c - 0x1ba];
};

struct s_game_options_2bd0
{
	byte unknown00[0x1128];
	bool flag1128;
};

short g_5092e8;
short g_5092f0[8];

void function_2bd960();
bool function_2be880(long player_index, s_polygon_2be *polygon);

class c_game_engine_45c878 : public c_game_engine
{
public:
	virtual bool v23();
	virtual bool v25();
	virtual void v28(long);
	virtual void v35(long);
	virtual void v36(long);
	virtual void v40();
	virtual void v37(long);
};

static inline s_player_2bd0 *player_get_2bd0(long player_index)
{
	return (s_player_2bd0 *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_player_2bd0));
}

// @retail 0x2bd0b0
long function_2bd0b0(long excluded)
{
	dword *seed = &g_4e7408->unknown0;
	short count = g_5092e8;
	long result = NONE;

	*seed = *seed * 0x19660d + 0x3c6ef35f;
	short first = (short)(((*seed >> 16) * count) >> 16);

	for (short i = 0; i < count; i++)
	{
		short index = (short)((first + i) % count);

		if (excluded != g_5092f0[index])
		{
			return g_5092f0[index];
		}
	}
	return result;
}

// @retail 0x2bd140
bool c_game_engine_45c878::v23()
{
	s_palette_source_globals *globals = g_4e0350;
	byte *state = (byte *)g_4e9ae8 + 0xfc;

	g_51ecc8 = (s_state_2bd *)state;
	memset(state, 0, 0x1dc);
	g_5092e8 = 0;
	for (short i = 0; i < globals->marker_count; i++)
	{
		s_marker_entry const *const entry = &globals->marker_entries[i];
        s_marker_entry const *const *entry_reference = &entry;
        short type = (*entry_reference)->key_a;

		if (type >= 11 && type <= 18)
		{
			bool found = false;

			for (short j = 0; j < g_5092e8; j++)
			{
				if (g_5092f0[j] == type - 11)
				{
					found = true;
					break;
				}
			}
			if (!found)
				g_5092f0[g_5092e8++] = type - 11;
		}
	}
	function_2bd960();
	return true;
}

/* the state as v25 sets it */
struct s_state_2bd0
{
	byte unknown000[0x1a0];
	long hill_index;
	long time_to_move;
};

void hill_set(s_polygon_2be *hill, long index);

// @retail 0x2bd200
bool c_game_engine_45c878::v25()
{
	s_state_2bd0 *state = (s_state_2bd0 *)g_51ecc8;

	state->hill_index = 0;
	state->time_to_move = g_4e6948->s230 * g_510c54->field_2_3;
	hill_set((s_polygon_2be *)state, 0);
	return true;
}

// @retail 0x2bd260
void c_game_engine_45c878::v28(long a)
{
	s_event event;

	game_engine_event_initialize_inline(&event, 6, 0);
	event.a = a;
	game_engine_event_send_inline(&event);
}

void hill_marker(long local_player, s_polygon_2be *hill);
bool function_19f240(long *iterator);

/* a player iterator: the current player before the iterator (0x19f240) */
struct s_player_iterator_2bd0
{
	s_player_2bd0 *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

/* whether the engine counts the two teams as friends */
static inline bool game_engine_teams_friendly_2bd0(short team, short other_team)
{
	bool result = false;
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];

	if (engine)
	{
		result = engine->p27(team, other_team);
	}
	return result;
}

/* the hill's marker, and in team games the markers of the other teams'
   players */
// @retail 0x2bd820
void c_game_engine_45c878::v36(long local_player)
{
	long player_index = NONE;

	if (local_player != NONE)
	{
		player_index = g_4e8c20->entries[local_player];
	}
	hill_marker(local_player, (s_polygon_2be *)g_51ecc8);
	if (player_index != NONE)
	{
		s_player_2bd0 *player = player_get_2bd0(player_index);

		if (player->object_index != NONE)
		{
			s_player_iterator_2bd0 iterator;

			iterator.data = g_4e8c24;
			iterator.absolute_index = NONE;
			iterator.index = NONE;
			while (function_19f240((long *)&iterator))
			{
				long other = iterator.index;

				if (other != player_index && !game_engine_teams_friendly_2bd0(iterator.player->team, player->team))
				{
					s_marker_list list;

					if (function_162550(other, &list))
					{
						function_24e59f(&list);
					}
				}
			}
		}
	}
}

// @retail 0x2bd2c0
void c_game_engine_45c878::v37(long player_index)
{
	s_player_2bd0 *player = player_get_2bd0(player_index);

	if (player->object_index != NONE &&
		!((s_game_options_2bd0 *)g_4e6948)->flag1128 &&
		g_4e6948->mode != 4 &&
		function_2be880(player_index, (s_polygon_2be *)g_51ecc8))
	{
		player->time_inside++;
	}
	else
	{
		player->time_inside = 0;
	}
}

// @retail 0x2bd330
long function_2bd330(word player_mask)
{
	s_record_pool *players = g_4e8c24;
	long team_mask = 0;

	for (long i = 0; i < 16; i++)
	{
		if (player_mask & (1 << i))
		{
			if (i != NONE && i >= 0 && i < players->high_water_index)
			{
				s_player_2bd0 *player = (s_player_2bd0 *)(players->data + players->size * i);

				if (player->identifier != 0 && player->team != NONE)
					team_mask |= 1 << player->team;
			}
		}
	}
	return team_mask;
}

bool function_19f300(long *iterator);
bool function_15eaf0();

PRIVATE __forceinline long find_hill_team_player_2bd(word team_mask)
{
    s_player_iterator_2bd0 iterator;
    iterator.data = g_4e8c24;
    iterator.absolute_index = NONE;
    iterator.index = NONE;
    while (function_19f300((long *)&iterator))
    {
        if (iterator.player->time_inside && iterator.player->team != NONE &&
            (team_mask & (1 << iterator.player->team)))
            return iterator.index;
    }
    return NONE;
}

// @retail 0x2bd460
void function_2bd460(long before_players, long after_players)
{
 struct { long saved_before; s_event event; } frame;
    word before = (word)function_2bd330(before_players);
 frame.saved_before = before;
    volatile word after = (word)function_2bd330(after_players);
    if (before != after && after != 0)
    {
        long volatile multiple = (after - 1) & after;
        if (!multiple)
        {
            long player = find_hill_team_player_2bd(after);
            s_event &event = frame.event;
            game_engine_event_initialize_inline(&event, 6, function_15eaf0() ? 5 : 1);
            if (player != NONE)
            {
                event.cause_player_index = player;
                event.cause_team = player_get_2bd0(player)->team;
            }
            game_engine_event_send_inline(&event);
        }
        before = (word)frame.saved_before;
        if (multiple && before && !((before - 1) & before))
        {
            word added = before ^ after;
            if (added)
            {
                long player = find_hill_team_player_2bd(added);
                s_event &event = frame.event;
                game_engine_event_initialize_inline(&event, 6, function_15eaf0() ? 6 : 2);
                if (player != NONE)
                    game_engine_event_set_cause_player(&event, player);
                function_19eb90(&event);
            }
        }
    }
}

void function_2be670(s_polygon_2be *hill);

// @retail 0x2bd240
void c_game_engine_45c878::v35(long unused)
{
    long count = *(long *)((byte *)g_51ecc8 + 0x60);
    if (count >= 4 && !(count & 1))
        function_2be670((s_polygon_2be *)g_51ecc8);
}

void __stdcall function_a7810(dword mask);
void function_2beee0();

// @retail 0x2bd680
void c_game_engine_45c878::v40()
{
    if (!((s_game_options_2bd0 *)g_4e6948)->flag1128 && g_4e6948->mode != 4)
    {
        s_player_iterator_2bd0 iterator;
        word occupants = 0;
        iterator.data = g_4e8c24;
        iterator.absolute_index = NONE;
        iterator.index = NONE;
        while (function_19f300((long *)&iterator))
        {
            if (iterator.player->time_inside)
                occupants |= 1 << (byte)iterator.index;
        }
        word previous = *(word *)((byte *)g_51ecc8 + 0x1a8);
        if (occupants != previous)
        {
            function_2bd460(previous, occupants);
            *(word *)((byte *)g_51ecc8 + 0x1a8) = occupants;
            function_a7810(0x40);
        }
        function_2beee0();
        if (*(short *)((byte *)g_4e6948 + 0x230))
        {
            s_state_2bd0 *state = (s_state_2bd0 *)g_51ecc8;
            if (--state->time_to_move <= 0)
            {
                /* Keep the timer store before the following state read. */
                *(volatile long *)&state->time_to_move = *(short *)((byte *)g_4e6948 + 0x230) * g_510c54->field_2_3;
                long next = function_2bd0b0(*(volatile long *)&state->hill_index);
                if (next == NONE)
                    goto changed;
                for (;;)
                {
                    hill_set((s_polygon_2be *)state, next);
                    long count = *(long *)((byte *)state + 0x60);
                    if (count >= 4 && !(count & 1))
                        break;
                    next = function_2bd0b0(state->hill_index);
                    if (next == NONE)
                        goto changed;
                }
                if (state->hill_index != next)
                {
                    s_event event;
                    game_engine_event_initialize_inline(&event, 6, 4);
                    function_19eb90(&event);
                    ((s_state_2bd0 *)g_51ecc8)->hill_index = next;
                }
            changed:
                function_a7810(0x20);
            }
        }
    }
}
