#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"
#include "unknown_0259d0.h"
#include "unknown_13927e.h"
#include <string.h>

// @flags /O2 /arch:SSE /Gr

/* JUGGERNAUT.CPP: the juggernaut game engine, the static engine object at
   0x47d8f0 whose vtable is at 0x453d38. The juggernauts are a mask of
   players (g_510c9c, kept in the multiplayer globals at +0xfc), together with
   the mask of their teams. Killing a juggernaut makes the killer the next
   one; when a juggernaut dies otherwise, a random player takes over.

   The vtable is numbered from its real start here (51 slots), unlike
   unknown_1523c0.h's c_game_engine, whose numbering is shifted by 30 slots; the
   class stands alone until the engine hierarchy is renumbered, with empty
   placeholders for the slots it shares with the other engines. */

struct s_juggernaut_globals
{
	word players;
	word teams;
};

s_juggernaut_globals *g_510c9c;

/* the players (0x21c bytes each), as these functions see them */
struct s_juggernaut_player
{
	byte unknown00[2];
	word flags;
	byte unknown04[0x2c - 0x04];
	long unit_index;
	byte unknown30[0xc0 - 0x30];
	char team;
	byte unknownc1[0x1ac - 0xc1];
	short s1ac;
	byte unknown1ae[0x21c - 0x1ae];
};

/* the iterator over the players of 0x19f240 */
struct s_player_iterator
{
	s_juggernaut_player *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

/* the state the engine sends to the clients: the common part, then the
   juggernauts */
struct s_juggernaut_update
{
	byte common[0x24];
	word players;
};

bool function_19f240(long *iterator);
void function_b58c0(long index, dword mask);
bool function_15b7c0(long a, long b);
bool function_15d770(long player_index);
bool function_15db30(long player_index);
void function_1967d0(long a, long b, long c, long delta);

extern s_game_time_globals *g_510c54;

class c_juggernaut_engine
{
public:
	virtual void v0() {}
	virtual bool v1();
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5(short player_index);
	virtual void v6(long a);
	virtual void v7(long) {}
	virtual void v8() {}
	virtual void v9() {}
	virtual void v10() {}
	virtual void v11() {}
	virtual void v12() {}
	virtual void v13() {}
	virtual void v14(long local_index);
	virtual void v15(long player_index);
	virtual void v16() {}
	virtual void v17() {}
	virtual void v18();
	virtual real v19(short player_index);
	virtual void v20() {}
	virtual void v21() {}
	virtual void v22() {}
	virtual void v23() {}
	virtual void v24() {}
	virtual void v25() {}
	virtual void v26() {}
	virtual bool v27(short team_a, short team_b);
	virtual void v28() {}
	virtual void v29() {}
	virtual void v30(long killer, long victim, long, long);
	virtual void v31() {}
	virtual void v32() {}
	virtual void v33() {}
	virtual void v34() {}
	virtual bool v35(long player_index, long type);
	virtual void v36() {}
	virtual long v37(short player_index, byte *flag);
	virtual void v38() {}
	virtual void v39() {}
	virtual void v40() {}
	virtual void v41(s_juggernaut_update *update) {}
	virtual void v42(dword mask, dword *changed, s_juggernaut_update *update) {}
	virtual bool v43(dword mask, s_juggernaut_update *update) { return false; }
	virtual void v44(long, s_juggernaut_update *update);
	virtual void v45(dword *flags, long, s_juggernaut_update *update);
	virtual bool v46(dword flags, long, s_juggernaut_update *update);
	virtual void v47() {}
	virtual void v48() {}
	virtual void v49() {}
	virtual void v50() {}
};

static inline s_juggernaut_player *juggernaut_player_get(long player_index)
{
	return (s_juggernaut_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_juggernaut_player));
}

static inline void juggernaut_globals_changed()
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
		function_b58c0(g_4e9ae8->value24, 0x20);
}

void function_192a30(long excluded_player_index);

// @retail 0x192b20
void juggernaut_update_teams()
{
	s_player_iterator iterator;

	iterator.data = g_4e8c24;
	g_510c9c->teams = 0;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (function_19f240((long *)&iterator))
	{
		if (g_510c9c->players & (1 << (char)iterator.index))
			g_510c9c->teams |= 1 << iterator.player->team;
	}
}

// @retail 0x192950
bool juggernaut_is(short player_index)
{
	return (bool)(g_510c9c->players & (1 << player_index));
}

// @retail 0x192970
void juggernaut_set(long player_index, bool juggernaut)
{
	long absolute_index = player_index & 0xffff;

	if (juggernaut)
	{
		s_event event;

		event.type = 8;
		event.subtype = 1;
		event.a = NONE;
		event.cause_player_index = player_index;
		event.cause_team = NONE;
		event.effect_player_index = NONE;
		event.effect_team = NONE;
		event.f = 0;
		event.g = NONE;
		if (g_4e6948->mode != 4)
		{
			function_a7c50(&event);
			function_19eb30(&event);
		}
		g_510c9c->players |= 1 << absolute_index;
	}
	else
	{
		g_510c9c->players &= ~(1 << absolute_index);
	}

	juggernaut_globals_changed();
	juggernaut_update_teams();
}

// @retail 0x192a30
void function_192a30(long excluded_player_index)
{
	s_player_iterator iterator;
	long candidates[16];
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (function_19f240((long *)&iterator))
	{
		long player_index = iterator.index;

		if (!(g_510c9c->players & (1 << (player_index & 0xffff))) && excluded_player_index != player_index)
		{
			s_juggernaut_player *player = iterator.player;
			bool waiting = function_15d770(player_index) || function_15db30(player_index);

			if (!waiting && player->unit_index != NONE || !(player->flags & 3))
				candidates[count++] = player_index;
		}
	}

	if (count > 0)
		juggernaut_set(candidates[(long)_random(&g_4e7408->unknown0, __FILE__, __LINE__) % count], true);
}

// @retail 0x1921d0
bool c_juggernaut_engine::v1()
{
	g_510c9c = (s_juggernaut_globals *)&g_4e9ae8->stats;
	memset(g_510c9c, 0, sizeof(s_juggernaut_globals));
	return true;
}

// @retail 0x1921f0
void c_juggernaut_engine::v5(short player_index)
{
	if (g_4e6948->mode != 4)
	{
		g_510c9c->players &= ~(1 << player_index);
		juggernaut_globals_changed();
		juggernaut_update_teams();
	}
}

// @retail 0x192240
void c_juggernaut_engine::v6(long a)
{
	if (g_4e6948->mode != 4)
	{
		s_event event;

		event.type = 8;
		event.subtype = 0;
		event.a = NONE;
		event.cause_player_index = NONE;
		event.cause_team = NONE;
		event.effect_player_index = NONE;
		event.effect_team = NONE;
		event.f = 0;
		event.g = NONE;
		event.a = a;
		function_a7c50(&event);
		function_19eb30(&event);
	}
}

// @retail 0x192420
void c_juggernaut_engine::v15(long player_index)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->w6c == 1 &&
		(g_4e6948->mode == 4 || g_4e9ae8->lc04 == 1) &&
		g_510c54->game_time % g_510c54->field_2_3 == 0)
	{
		long absolute_index = player_index & 0xffff;

		if (g_510c9c->players & (1 << absolute_index) &&
			((s_juggernaut_player *)(g_4e8c24->data + absolute_index * sizeof(s_juggernaut_player)))->unit_index != NONE)
		{
			function_1967d0(absolute_index, 0x28, NONE, 1);
		}
	}
}

// @retail 0x192530
void c_juggernaut_engine::v18()
{
	if (g_4e6948->mode != 4)
	{
		s_player_iterator iterator;
		dword players = 0;
		long count = 0;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (function_19f240((long *)&iterator))
		{
			long player_index = iterator.index;
			long absolute_index = player_index & 0xffff;

			if (g_510c9c->players & (1 << absolute_index))
			{
				word flags = iterator.player->flags;

				if (flags & 2 || flags & 1 && (function_15d770(player_index) || function_15db30(player_index)))
				{
					g_510c9c->players &= ~(1 << absolute_index);
					juggernaut_globals_changed();
					juggernaut_update_teams();
				}
				else
				{
					players |= 1 << absolute_index;
					count++;
				}
			}
		}

		if (g_510c9c->players != (word)players)
		{
			g_510c9c->players = (word)players;
			juggernaut_globals_changed();
		}

		if (count == 0)
			function_192a30(NONE);
	}
}

// @retail 0x1924c0
real c_juggernaut_engine::v19(short player_index)
{
	real result = 1.0f;

	if (g_510c9c->players & (1 << player_index))
	{
		switch (g_4e6948->s230)
		{
		case 0:
			result = 0.75f;
			break;
		case 1:
			result = 1.0f;
			break;
		case 2:
			result = 1.5f;
			break;
		}
	}

	return result;
}

// @retail 0x192670
bool c_juggernaut_engine::v27(short team_a, short team_b)
{
	if (team_a == team_b)
		return false;

	word teams = g_510c9c->teams;
	if (!teams)
		return true;

	bool result = (bool)(teams & (1 << team_a));
	result |= (bool)(teams & (1 << team_b));
	return result;
}

// @retail 0x1922a0
void c_juggernaut_engine::v30(long killer, long victim, long, long)
{
	if (g_4e6948->mode != 4)
	{
		if (killer != victim && victim != NONE && g_510c9c->players & (1 << (char)victim))
		{
			s_event event;

			event.type = 8;
			event.subtype = 2;
			event.a = NONE;
			event.cause_player_index = NONE;
			event.cause_team = NONE;
			event.effect_player_index = NONE;
			event.effect_team = NONE;
			event.f = 0;
			event.g = NONE;
			event.cause_player_index = killer;
			event.effect_player_index = victim;
			function_19eb90(&event);
			juggernaut_set(victim, false);
			if (killer != NONE)
			{
				juggernaut_set(killer, true);
				function_1967d0(killer & 0xffff, 0x26, NONE, 1);
			}
		}
		else if (killer != NONE && g_510c9c->players & (1 << (killer & 0xffff)) &&
			killer != victim && victim != NONE)
		{
			function_15b7c0(1, killer);
			function_1967d0(killer & 0xffff, 0x27, NONE, 1);
		}
		else if (g_4e6948->flags22c_bits.bit3 && killer != NONE && victim != NONE && killer != victim &&
			!juggernaut_is(killer) && !juggernaut_is(victim))
		{
			function_15b7c0(NONE, killer);
		}
		else if (victim != NONE && g_510c9c->players & (1 << victim))
		{
			juggernaut_set(victim, false);
			function_192a30(victim);
		}
	}
}

// @retail 0x1926c0
bool c_juggernaut_engine::v35(long player_index, long type)
{
	bool result = false;

	if (juggernaut_is((short)player_index))
	{
		if (type == 1)
			result = g_4e6948->flags22c_bits.bit2;
		else if (type == 2)
			result = g_4e6948->flags22c_bits.bit4;
		else if (type == 3)
			result = g_4e6948->flags22c_bits.bit6;
	}
	else
	{
		/* the base engine's handler (unknown_1523c0.h numbers it v5) */
		result = ((c_game_engine *)this)->c_game_engine::v5(player_index, type);
	}

	return result;
}

// @retail 0x192750
long c_juggernaut_engine::v37(short player_index, byte *flag)
{
	long result = NONE;

	*flag = 1;
	if (g_510c9c->players & (1 << player_index))
		result = 0xf;

	return result;
}

// @retail 0x192870
void c_juggernaut_engine::v44(long, s_juggernaut_update *update)
{
	memset(update, 0, sizeof(s_juggernaut_update));
	v41(update);
}

// @retail 0x1928b0
void c_juggernaut_engine::v45(dword *flags, long, s_juggernaut_update *update)
{
	dword changed = 0;
	dword mask = *flags & 0x1f;

	if (mask)
		v42(mask, &changed, update);

	if (*flags & 0x20)
	{
		word players = g_510c9c->players;

		if (update->players != players)
		{
			update->players = players;
			changed |= 0x20;
		}
	}

	*flags = changed;
}

// @retail 0x192900
bool c_juggernaut_engine::v46(dword flags, long, s_juggernaut_update *update)
{
	bool result = true;
	dword mask = flags & 0x1f;

	if (mask)
		result = v43(mask, update) != false;

	if (flags & 0x20)
	{
		word players = update->players;

		if (g_510c9c->players != players)
		{
			g_510c9c->players = players;
			juggernaut_update_teams();
		}
	}

	return result;
}

static inline bool game_engine_teams_p27(short team_a, short team_b)
{
	c_engine_peer *engine = g_55e4d0[g_4e9ae8->engine_index];

	return engine && engine->p27(team_a, team_b);
}

// @retail 0x192780
void c_juggernaut_engine::v14(long local_index)
{
	if (local_index != NONE)
	{
		long player_index = g_4e8c20->entries[local_index];

		if (player_index != NONE && !(g_510c9c->players & (1 << (player_index & 0xffff))))
		{
			s_juggernaut_player *player = juggernaut_player_get(player_index);

			if (player->unit_index != NONE)
			{
				s_player_iterator iterator;

				iterator.data = g_4e8c24;
				iterator.absolute_index = NONE;
				iterator.index = NONE;
				while (function_19f240((long *)&iterator))
				{
					long other = iterator.index;

					if (other != player_index)
					{
						if (!game_engine_teams_p27(iterator.player->team, player->team))
						{
							s_marker_list list;

							if (function_162550(other, &list))
								function_24e59f(&list);
						}
					}
				}
			}
		}
	}
}