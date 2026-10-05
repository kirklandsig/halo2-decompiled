// @flags /O2 /Gr
/* UNKNOWN_083A40.CPP: the simulation watcher (g_4cf780), which tracks the
   game's players for the simulation world (lane D) */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_067e10.h"
#include "globals.h"
#include "unknown_059ad0.h"

// @retail 0x83a40
bool simulation_watcher_get_players(s_simulation_world_owner *watcher, long *unknown1c, dword *player_mask, dword *in_game_mask, dword *state, t_player_key *keys, bool force)
{
	bool result = false;

	if (force || watcher->unknownc30)
	{
		*unknown1c = watcher->unknown1c;
		memcpy(state, watcher->unknown24, sizeof(watcher->unknown24));
		*player_mask = watcher->players.player_mask;
		*in_game_mask = function_84be0(&watcher->players);
		memset(keys, 0, 16 * sizeof(keys[0]));
		for (long i = 0; i < 16; i++)
		{
			if (watcher->players.player_mask & (1 << i))
				memcpy(keys[i], watcher->players.players[i].key, sizeof(keys[i]));
		}
		watcher->unknownc30 = false;
		result = true;
	}
	return result;
}

/* the watcher's machines (unknown1c is the mask of the machines in the game,
   unknown24 their addresses) */
// @retail 0x83b80
long simulation_watcher_find_machine(const s_simulation_world_owner *watcher, const s_machine_address *address)
{
	long result = NONE;
	for (long i = 0; i < 16; i++)
	{
		if ((watcher->unknown1c & (1 << i)) && !memcmp(&((const s_machine_address *)watcher->unknown24)[i], address, sizeof(s_machine_address)))
			result = i;
	}
	return result;
}

/* a player of the session's game (0x13c bytes) */
struct s_simulation_session_player
{
	dword key[3];
	long machine_index;
	long controller_index;
	long unknown14;
	byte unknown18[0x13c - 0x18];
};

/* whether the watcher's record of a player differs from the session's */
// @retail 0x83c20
bool simulation_player_changed(const s_machine_address *machines, dword player_mask, const s_simulation_session_player *players, long player_index, const s_simulation_owner_player *player)
{
	bool result = true;
	if (player_mask & (1 << player_index))
	{
		const s_simulation_session_player *session_player = &players[player_index];
		if (!memcmp(player->key, session_player->key, sizeof(player->key)) &&
			!memcmp(&player->machine, &machines[session_player->machine_index], sizeof(s_machine_address)) &&
			player->controller_index == session_player->controller_index &&
			player->unknown20 == session_player->unknown14)
		{
			result = false;
		}
	}
	return result;
}

/* the slot a player goes in: the slot of a player with its key, else a free
   slot, else the slot of the player that left the game first */
// @retail 0x83c90
void simulation_player_collection_find_slot(const s_type_c67652 *collection, long player_index, const t_player_key *key, long *slot, bool *occupied)
{
	dword bit = 1 << player_index;
	*occupied = (collection->player_mask & bit) != 0;
	*slot = NONE;

	long found = NONE;
	dword player_mask = collection->player_mask;
	for (long i = 0; i < 16; i++)
	{
		const s_simulation_owner_player *player = &collection->players[i];
		if ((player_mask & (1 << i)) && player->flag0c && !memcmp(player, key, sizeof(t_player_key)))
			found = i;
	}
	if (found == player_index)
	{
		*occupied = false;
	}
	else if (found != NONE)
	{
		*slot = found;
		*occupied = false;
	}
	else if ((player_mask & bit) && collection->players[player_index].flag0c)
	{
		long earliest = 0x7fffffff;
		for (long i = 0; i < 16; i++)
		{
			if (!(collection->player_mask & (1 << i)))
			{
				*slot = i;
				*occupied = false;
				return;
			}
			if (collection->players[i].flag0c && collection->players[i].time < earliest)
			{
				*slot = i;
				earliest = collection->players[i].time;
			}
		}
	}
}

static __forceinline bool watcher_machine_present(const s_simulation_world_owner *watcher, const s_machine_address *address)
{
	for (long i = 0; i < 16; i++)
	{
		if ((watcher->unknown1c & (1 << i)) && !memcmp(&((const s_machine_address *)watcher->unknown24)[i], address, sizeof(s_machine_address)))
			return true;
	}
	return false;
}

/* the players whose machines have gone leave the game; then the machine
   table becomes the current one */
// @retail 0x84270
void simulation_watcher_update_machines(s_simulation_world_owner *watcher)
{
	for (long i = 0; i < 16; i++)
	{
		s_simulation_owner_player *player = &watcher->players.players[i];
		if ((watcher->players.player_mask & (1 << i)) && !player->flag0c && !watcher_machine_present(watcher, &player->machine))
		{
			s_simulation_player_update update;
			update.type = 0;
			update.player_index = i;
			memcpy(update.key, player->key, sizeof(update.key));
			simulation_player_collection_apply_update(&watcher->players, &update);

			c_class_6a600 *world = watcher->world;
			if (world->state != 3 && world->state != 5)
			{
				long index = i & 0xffff;
				if (index >= 0 && index < sizeof(world->players) / sizeof(world->players[0]) && world->players[index].player_index != NONE)
				{
					s_simulation_world_player *world_player = &world->players[i];
					if (world_player->flag25)
						world_player->flag25 = false;
					world_player->flag24 = true;
				}
			}
		}
	}
	watcher->unknownbcc = watcher->unknown1c;
	memcpy(watcher->unknownbd0, watcher->unknown24, sizeof(watcher->unknownbd0));
	watcher->unknown18 = NONE;
}

// @retail 0x83bd0
bool simulation_watcher_player_valid(long player_index, const s_simulation_world_owner *watcher, const t_player_key *key)
{
	bool result = false;
	if (watcher->players.player_mask & (1 << player_index))
	{
		const s_simulation_owner_player *player = &watcher->players.players[player_index];
		if (!memcmp(key, player->key, sizeof(t_player_key)) && !player->flag0c)
			result = true;
	}
	return result;
}
/* src/unknown_084a90.cpp */
void simulation_player_collection_clear(s_type_c67652 *collection);
void simulation_player_collection_build(s_type_c67652 *collection);

/* src/unknown_067e10.cpp */
void function_69610(c_class_6a600 *world);

/* starts the watcher over: no machines and no players */
// @retail 0x82fa0
void simulation_watcher_reset(s_simulation_world_owner *watcher)
{
	watcher->unknown10 = NONE;
	watcher->unknown14 = NONE;
	watcher->unknown18 = NONE;
	watcher->unknown20 = NONE;
	watcher->unknown1c = 0;
	memset(watcher->unknown24, 0, sizeof(watcher->unknown24));
	watcher->unknown84 = false;
	simulation_player_collection_clear(&watcher->players);
	watcher->unknownbcc = 0;
	memset(watcher->unknownbd0, 0, sizeof(watcher->unknownbd0));
	watcher->unknownc30 = true;
}

/* the game's machines (g_4e8c20), as the watcher copies them */
struct s_watcher_machines
{
	byte unknown00[0x2c];
	long count;
	dword machines[0x18];
};

/* rebuilds the watcher's players from the game's */
// @retail 0x83510
void simulation_watcher_rebuild_players(s_simulation_world_owner *watcher)
{
	simulation_player_collection_clear(&watcher->players);
	simulation_player_collection_build(&watcher->players);
	s_watcher_machines *machines = (s_watcher_machines *)g_4e8c20;
	watcher->unknownbcc = machines->count;
	memcpy(watcher->unknownbd0, machines->machines, sizeof(watcher->unknownbd0));
	watcher->unknown18 = NONE;
	watcher->unknownc30 = true;
	c_class_6a600 *world = watcher->world;
	if (world->state != 3 && world->state != 5)
		function_69610(world);
}

#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

/* marks a player of the world as changed */
// @retail 0x83570
void simulation_watcher_mark_player(s_simulation_world_owner *watcher, long player_index)
{
	c_class_6a600 *world = watcher->world;
	if (world->state != 3 && world->state != 5)
	{
		if (world_player_get(world, player_index))
		{
			s_simulation_world_player *player = &world->players[player_index];
			if (player->flag25)
				player->flag25 = false;
			player->flag24 = true;
		}
	}
}

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

/* whether the watcher has news for the world: its machines changed, or the
   session's game changed since it last looked */
// @retail 0x835c0
bool simulation_watcher_changed(s_simulation_world_owner *watcher)
{
	bool result = false;
	c_class_6a600 *world = watcher->world;
	if (world && world->state != 3)
	{
		if (watcher->unknown84)
			result = true;
		c_class_58d20 *session = watcher->session;
		if (session && SESSION_STATE_IS_LIVE(session->state) && session->type == 4 && watcher->unknown18 != session->update7618)
			result = true;
	}
	return result;
}
