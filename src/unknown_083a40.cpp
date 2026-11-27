// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_083A40.CPP: the simulation watcher (g_4cf780), which tracks the
   game's players for the simulation world (lane D) */

#include "unknown_11c920.h"
#include <string.h>
#include "unknown_067e10.h"
#include "unknown_08b110.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"
#include "unknown_075870.h"

extern char const *g_4e9bb8;
long map_location_get(char const *map_name);

extern bool g_4d8ba0;
bool network_session_manager_get_any_session(c_class_58d20 **session);

struct s_watcher_session_link
{
	byte unknown00[0x80];
	s_simulation_world_owner *watcher;
	long index84;
	long index88;
};

struct s_watcher_machine_options
{
	byte unknown00[0x270];
	dword mask;
	s_machine_address machines[16];
	bool local_valid;
	s_machine_address local;
};

struct s_watcher_membership
{
	long host;
	long unknown04;
	long count;
	s_session_member members[16];
};

static inline s_watcher_membership *watcher_session_membership(c_class_58d20 *session)
{
	s_watcher_membership *result = 0;
	if (session->state && session->value4c != NONE)
		result = (s_watcher_membership *)&session->value4c;
	return result;
}

static inline void function_83001(c_class_6a600 *world, const s_machine_address *address)
{
	world->local_address = *address;
	world->unknown0c = 1;
}

// @retail 0x83000
void function_83000(s_simulation_world_owner *watcher)
{
	s_watcher_machine_options *options = (s_watcher_machine_options *)g_4e6948;
	if (g_4e6948->state == 1 || g_4e6948->state == 2)
	{
		c_class_58d20 *session = 0;
		if (g_4d8ba0 && network_session_manager_get_any_session(&session) &&
			watcher_session_membership(session))
			watcher->session = session;
	}
	if (watcher->session)
	{
		s_watcher_membership *members = 0;
		long local = NONE;
		s_watcher_session_link *link = 0;
		if (watcher->session->state && watcher->session->value4c != NONE)
		{
			local = watcher->session->current_member;
			members = (s_watcher_membership *)&watcher->session->value4c;
		}
		if (g_527330.initialized)
			link = (s_watcher_session_link *)g_527330.unknown48;
		*(s_watcher_session_link **)watcher->unknown08 = link;
		link->watcher = watcher;
		link->index84 = NONE;
		link->index88 = NONE;
		dword mask = (1 << members->count) - 1;
		s_machine_address machines[16];
		memset(machines, 0, sizeof(machines));
		for (long i = 0; i < members->count; i++)
			machines[i] = *(s_machine_address *)((byte *)&members->members[i] + 10);
		watcher->unknown10 = watcher->session->update7618;
		watcher->unknown14 = members->host;
		watcher->unknown1c = mask;
		watcher->unknown20 = local;
		memcpy(watcher->unknown24, machines, sizeof(machines));
		function_83001(watcher->world, &((s_machine_address *)watcher->unknown24)[local]);
		*(long *)(watcher->world->unknown13 + 1) = watcher->unknown20;
		watcher->unknown84 = true;
		watcher->unknownc30 = true;
	}
	else
	{
		watcher->unknown1c = options->mask;
		memcpy(watcher->unknown24, options->machines, sizeof(options->machines));
		watcher->unknown84 = false;
		watcher->unknown20 = NONE;
		if (options->local_valid)
		{
			for (long i = 0; i < 16; i++)
			{
				if ((watcher->unknown1c & (1 << i)) &&
					!memcmp(&((s_machine_address *)watcher->unknown24)[i], &options->local, sizeof(s_machine_address)))
					watcher->unknown20 = i;
			}
		}
		if (watcher->unknown20 != NONE)
		{
			function_83001(watcher->world, &((s_machine_address *)watcher->unknown24)[watcher->unknown20]);
			*(long *)(watcher->world->unknown13 + 1) = watcher->unknown20;
		}
	}
}

// @retail 0x83db0
long function_83db0(s_simulation_world_owner *watcher)
{
	long result = 0;
	if (watcher->world)
	{
		if (watcher->world->state == 1)
		{
			result = 1;
			goto done;
		}
		if (g_4e9bb8)
		{
			long location = map_location_get(g_4e9bb8);
			if (location != 3 && location != 4)
			{
				result = 2;
				goto done;
			}
		}
		if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
		{
			c_class_6a600 *world = watcher->world;
			if (world->state != 3 && world->state != 5)
			{
				switch (world->unknown18)
				{
				case 3: result = 4; break;
				case 5: result = 5; break;
				case 6: result = 6; break;
				case 4: result = 3; break;
				}
				if (result) goto done;
			}
			else
			{
				switch (world->unknown18)
				{
				case 6: result = 13; break;
				case 4: result = 3; break;
				case 3: result = 10; break;
				}
				if (result) goto done;
			}
			if (watcher->session)
			{
				switch (watcher->session->state)
				{
				case 1: result = 11; break;
				case 2: result = 12; break;
				case 3: result = 9; break;
				case 4: result = 13; break;
				case 5: result = 4; break;
				case 6: result = 6; break;
				case 7: result = 5; break;
				case 8: result = 7; break;
				case 9: result = 8; break;
				}
				if (result) goto done;
			}
		}
	}
done:
	return result;
}

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
	byte unknown18[0xa8 - 0x18];
	dword configuration[0x24];
	byte unknown138[0x13c - 0x138];
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
// score 27 (was 27): the table pointer is read from a different stack slot and the counter starts
// in EAX; retail reads it from [esp+0x18] and starts the counter in EBX.
// @retail 0x83c90
void simulation_player_collection_find_slot(const s_type_c67652 *collection, long player_index, const t_player_key *key, long *slot, bool *occupied)
{
	long local_2;
	long found = NONE;
	*occupied = (collection->player_mask & (1 << player_index)) != 0;
	*slot = NONE;

	dword bit = 1 << player_index;
	local_2 = 0;
	dword player_mask = collection->player_mask;
	for (; local_2 < 16; local_2++)
	{
		const s_simulation_owner_player *player = &collection->players[local_2];
		if ((player_mask & (1 << local_2)) && player->flag0c && !memcmp(player, key, sizeof(t_player_key)))
			found = local_2;
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
#pragma optimize("g", off)
static __forceinline s_simulation_world_player *function_84272(c_class_6a600 *arg_0, long arg_1)
{
 s_simulation_world_player *local_0 = 0;
 if (arg_1 >= 0 && arg_1 < sizeof(arg_0->players) / sizeof(arg_0->players[0]))
 {
  s_simulation_world_player *local_1 = &arg_0->players[arg_1];
  if (local_1->player_index != NONE)
   local_0 = local_1;
 }
 return local_0;
}
#pragma optimize("", on)
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
				if (function_84272(world, index) != 0)
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
#pragma auto_inline(off)
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
#pragma auto_inline(on)

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

bool network_session_channel_has_member(c_class_58d20 *session, long channel_index);
bool function_67e10(c_class_6a600 *world);
c_simulation_view *function_6acb0(c_class_6a600 *world);
c_simulation_view *function_6ad40(c_class_6a600 *world, const s_machine_address *address);
void __stdcall function_68580(short type, const s_machine_address *address, long value, long unused);
void function_85650(c_simulation_view *view, s_network_observer *observer, const XNADDR *address, long channel_index);

// @retail 0x84630
bool function_84630(long channel_index, s_simulation_world_owner *watcher, long value, const XNADDR *address)
{
 volatile bool result = false;
 c_class_6a600 *world = watcher->world;
 if (world->unknown2f && channel_index != NONE &&
  network_session_channel_has_member(watcher->session, channel_index))
 {
  s_network_observer_channel *channel = &(*(s_network_observer **)watcher->unknown08)->channels[channel_index];
  if (channel->state == 7)
  {
   s_link_connection *connection = &((s_link_connection *)g_4d87d4)[channel->connection_index];
   if (connection->state == 5)
   {
    short type;
    switch (world->state)
    {
    case 2: type = 2; break;
    case 4: type = 4; break;
    case 3: type = 1; break;
    case 5: type = 3; break;
    default: __assume(0);
    }
    s_machine_address machine = *(const s_machine_address *)((const byte *)*(const XNADDR *const volatile *)&address + 0xa);
    function_68580(type, &machine, value, 0);
    world = watcher->world;
    c_simulation_view *view;
    if (function_67e10(world))
     view = function_6ad40(world, &machine);
    else
     view = function_6acb0(world);
    if (view)
    {
     function_85650(view, *(s_network_observer **)watcher->unknown08, address, channel_index);
     result = true;
    }
   }
  }
 }
 return result;
}

struct s_view_iterator
{
 dword mask;
 long index;
};
bool world_next_view(c_class_6a600 *world, s_view_iterator *iterator, c_simulation_view **out);

// @retail 0x84560
bool function_84560(s_simulation_world_owner *watcher)
{
 c_simulation_view *view = 0;
 s_view_iterator iterator = { 10, 0 };
 world_next_view(watcher->world, &iterator, &view);
 if (!view || view->failure_reason)
  function_6b040(watcher->world);
 if (watcher->world->unknown18 != 1)
 {
 view = 0;
 iterator.mask = 10;
 iterator.index = 0;
 world_next_view(watcher->world, &iterator, &view);
 if (!view)
 {
  c_class_58d20 *session = watcher->session;
  long member = session->member_index;
  long channel = NONE;
  if (member >= 0 && member < session->member_count)
  {
   byte *entry = (byte *)session + 0x72dc + member * 20;
   if (entry[1])
    channel = *(long *)(entry + 4);
  }
  function_84630(channel, watcher, member, (const XNADDR *)&session->members[member]);
 }
 return true;
 }
 return false;
}

void simulation_player_collection_swap(s_type_c67652 *collection, long player_index, long other_index, s_simulation_player_update *update);

// score 28 (was 28): the address of the record is computed in EDX and the zero kept in EBX;
// retail computes it in ECX and stores it first, so the block order differs.
// inline/noinline swap declaration 32; apply-update declaration 32; both 32;
// collection preheader 277; const session 32; const collection 32; array-base key 32;
// late key pointer 45; occupant reference 45; collection reference 32;
// machine pointer renamed 32; second-loop index renamed 32.
// The rest is register choice: retail loads `updates` after pushing the slot
// for the swap call and adds the collection first for the occupant's key.
// Tried: player pointer first in the loop 32, player before source 32,
// collection hoisted out of the loop 277, swap update through a local or an
// index 32, `updates + ++*count - 1` 32, occupant key through
// watcher->players 294, through player 291, `(collection->players + i)` 32.
// The session player's configuration as a real member at 0xa8 32 (same code); the mask test
// wrapping the body instead of `continue` 32 (same code); `configuration` reused for the last copy
// 113; one function-scope player pointer for both loops 32 (same code); `&watcher->players` written
// at each use instead of `collection` 294.
// J185: signed-word first/second/conflict ordinal subsets 291/257/292/179/281/288/313;
// byte conflict ordinal 196. Native narrow counters grow the body; original counters, source observations,
// exact key/copy expressions, call roles and all helper declarations restored.
// Round 35: the swap callee declared (collection, player, update, other) with its definition and
// this call, 36 (the swap call becomes retail's row for row: slot pushed before the updates load;
// the swap callee still matches; the last record's two source reads then trade registers and the
// occupied key address keeps its order); on it the last record's reads swapped 47, read before the
// count increment 40, the occupied key through `collection->players + i` 36, its index stored
// before its type 38.
// Round 35 (one-file harness, exact; aligned/positional 9/16): `++*count;` before the swap call 9/16;
// `&updates[(*count)++]` 16/20; the occupant key via `(&collection->players[i])->key` 9/16; the four
// loop-top declarations in the other 11 orders 9/16; all 23 other parameter orders of the swap
// callee (it stays matched) 9-17; the player as `&collection->players[i]` 153 (mask too 120).
// Round 5: the occupied record's key address is one byte shorter here ((offset + cursor) + collection
// with add/lea against retail's (collection + offset) + cursor with lea/lea), which shifts every later
// jump and call by a byte up to the loop pad. The swap's count increment as its own statement 32 and
// its update named first 32 (both identical code); the occupant key from `&collection->players[i]`
// 32 (identical); the conflict scan through `collection` 263; `collection` declared before the
// player-mask test 32.
// Round 103 (one-file harness exact: 0 rows differ from the real listing): `slot`/`occupied`
// declared at the loop top 32, with the round-35 swap order 36; the occupied key as
// `&collection->players[i].key` 32 (36); the occupied index stored before its type 34 (38);
// the session-player source declared before `collection` with the swap order 36; the player-mask
// test through `collection` 197 (1063 bytes).
// Round 19 (lane 11), on the swap order above: the swap's update through the local `update` 36;
// the occupied key through `watcher->players.players[i]` 292; the second loop's player and mask
// through `collection` 271, its player alone 267. With it the occupied key is still
// (offset + cursor) + collection where retail adds (collection + offset) first.
// three spots in the second loop, all register choice. The preheader builds the
// configuration cursor in EDX and stores it late (retail ECX, stored at once); `updates` is loaded
// before the swap call's slot push (retail after it); the occupied record's key address is
// associated (offset + cursor) + collection with add/lea, one byte shorter than retail's
// (collection + offset) + cursor with lea/lea, which shifts every later jump and call up to the loop pad.
// Tried: inline/noinline swap declaration 32; apply-update declaration 32; both 32; collection
// preheader 277; const session 32; const collection 32; array-base key 32; late key pointer 45;
// occupant reference 45; collection reference 32; machine pointer renamed 32; second-loop index
// renamed 32; player pointer first in the loop 32, player before source 32, collection hoisted out of
// the loop 277, swap update through a local or an index 32, `updates + ++*count - 1` 32, occupant key
// through watcher->players 294, through player 291, `(collection->players + i)` 32; the session
// player's configuration as a real member at 0xa8 32 (same code); the mask test wrapping the body
// instead of `continue` 32 (same code); `configuration` reused for the last copy 113; one
// function-scope player pointer for both loops 32 (same code); `&watcher->players` written at each use
// instead of `collection` 294; signed-word first/second/conflict ordinal subsets
// 291/257/292/179/281/288/313; byte conflict ordinal 196; the swap's count increment as its own
// statement 32 and its update named first 32 (both identical code); the occupant key from
// `&collection->players[i]` 32 (identical); the conflict scan through `collection` 263; `collection`
// declared before the player-mask test 32. The swap callee declared (collection, player, update,
// other) with its definition and this call 36 (the swap call becomes retail's row for row; the last
// record's two source reads then trade registers and the occupied key keeps its order); on it the
// last record's reads swapped 47, read before the count increment 40, the occupied key through
// `collection->players + i` 36, its index stored before its type 38.
// One-file harness (this file + unknown_084a90.cpp, exact; aligned/positional 9/16): `++*count;`
// before the swap call 9/16; `&updates[(*count)++]` 16/20; the occupant key via
// `(&collection->players[i])->key` 9/16; the four loop-top declarations in the other 11 orders 9/16;
// all 23 other parameter orders of the swap callee 9-17; the player as `&collection->players[i]` 153
// (mask too 120). Round 36 (harness): the 24 swap orders crossed with the occupant key as
// `&collection->players[i]` or `watcher->players.players[i].key` and with the last record read at its
// stores: best 9/16 (watcher key 156-180, reads at the stores 40-45); the occupied block as an inline
// remove helper in four parameter orders, inline or forceinline 9/16 (identical); `collection` scoped
// to the else branch 9/16; the first loop's increment as a statement 9/16, its index stored first
// 10/18, its key through `watcher->players` 9/16, the mark call given the update's index 104/281; the
// closing stamp through the session local 41/62; permuter (600 s, 75 tries) none better.
// Round 19 (lane 13): `collection` declared at the function top or right after the session 228/228
// (1088 bytes); declared at the top and assigned in the loop 32 (same code); `occupied` declared before
// `slot` 32; the swap order above with the top-declared collection 36 (registers masked 8 against 10: the
// occupied key's association, the preheader cursor store and the last record's reads remain).
// Round 31 (lane 10), one-file harness (this file + unknown_084a90.cpp, exact): on the swap order
// above, `collection` declared after `source` 36; the occupied key as `&collection->players[i].key`,
// `(&collection->players[i])->key` or `(collection->players + i)->key` 36 each; the last record's
// reads before the update pointer 40, swapped 47, read at their stores 110 (1053 bytes); every
// combination of those 32 forms 36-110 (real --fast agrees: 36/36/110, 0x84e90 still matches).
// Round 177: `collection` declared in each branch where it is used 32 (same code; 36 on the swap order
// above, aligned 24 against 14: the swap call becomes retail's, the occupied block's ECX/EDX and the last
// record's two source reads swap); the machine pointer taken after the `unknown14 == NONE` test 186 (1071
// bytes); the conflict scan's `memcmp(...) == 0` 32; the machine table filled with memcpy 32; the last
// record's machine copied with memcpy 32 (all same code).
// Round 26 (lane 15, fast): each update record as its own block-scoped local (the changed, removed and added
// records), all three or only the removal, with role names or all named `update`, 32/32/32 (same code); on the
// swap order above 36 (aligned 18/17 either way); the configuration pointer declared in the branch that compares
// it 32, on the swap order 36.
// Round 36 (lane 10, fast): one function-scope update pointer for both loops 32; `slot`/`occupied` at
// function scope 32; the occupied record through its own scoped pointer 32, the last record likewise 32,
// both 32 (all same code). Retail loads `updates` last in the swap and occupied set-ups and groups
// (collection + offset) first in the occupied key, as if that sum were treated as loop-invariant.
// Round 54 (lane 12, fast with 0x69260/0x84e90): retail loads the occupied key's IV offset before the
// `updates` pointer; the occupied key pointer named before the update 49 (ours still adds offset + cursor
// first); `++*count;` as its own statement at the four second-loop sites 32 (same code); both 49; the
// swap callee as (collection, player, update, other) with the key pointer first 44, with the count
// statements 36.
// Round 59 (lane 12, one-file harness with unknown_084a90.cpp, exact: 32): `long unknown14` read
// before the NONE test and used in the test, the conflict scan and the last record 104 (1054 bytes,
// fast agrees), with the last record's controller index read at its store 108, with the scan
// reading the field 104; the find-slot/swap/remove/add tail as an inline helper on the collection
// 555 (not inlined), forceinline 32 (same code); the occupied key copied from
// `&collection->players[i]` 32 and from `collection->players[update->player_index].key` 32 (same code).
// Round 36 (lane 10, fast): one function-scope update pointer for both loops 32; `slot`/`occupied` at
// function scope 32; the occupied record through its own scoped pointer 32, the last record likewise 32,
// both 32 (all same code). Retail loads `updates` last in the swap and occupied set-ups and groups
// (collection + offset) first in the occupied key, as if that sum were treated as loop-invariant.
// Round 42 (lane 1, fast with 0x69260/0x84e90): the three spots share one pattern: retail reuses the
// register just freed (ECX for the preheader cursor before the offset, ECX for `updates` after the slot
// push) where ours takes another free one (EDX), so the scheduler moves the store or the load. One
// function-scope index for the three loops 32; the swap given `&watcher->players` 32; a block-scoped
// update pointer in each second-loop block 32; the session player as `session->players + i` 32 (all
// identical code); the conflict scan without its flag (`break`, then `if (j < 16) continue;`) 230.
// Round 120 (lane 6, fast): `machines` declared at function scope 32; `collection` declared after `source`
// 32; a `player_mask` local for the second loop's test and the conflict scan 32; the live test through the
// existing function_058d70 32; the second loop's player fields read at each use 32 (all identical code).
// Round 34 (lane 13, LTCG harness, base 16 positional = 32): the session read before the initial count
// harness 23 (aligned 18); the initial count read inside the live-session block harness 50 (1063 bytes); the
// configuration as `const byte *` 32; the member count in a local for the machine copy 32; the swap's
// update named in its own block 32 (the last three identical code).
// Round 24 (m3 lane 2): `configuration` declared inside the branch that compares it 32 (same code); the
// compare's operands swapped (`memcmp(player->configuration, configuration, ...)`) 157; the first
// branch's type stored before its index 32; `update` declared at the function top 32, `slot` and
// `occupied` there 32 (all same code).
// The remaining spots are the preheader cursor (ECX/EDX), the swap call's `updates` load and the
// occupied key's association; the calls flagged after it are the one-byte shift, not a callee change.
// Round 107 (lane 8, fast; aligned = difflib count): the session player's configuration as a real member
// read at each use (no `configuration` pointer local; the last record's `source + 0xa8` too) 28 (aligned 11,
// kept: the preheader cursor now takes ECX and is stored at once as retail); left: `updates` loaded before
// the swap call's slot push (retail after it, in ECX) and the occupied key (retail (collection + offset) +
// cursor; ours (offset + cursor) + collection, with `updates`/offset loads in the other registers). On 28:
// the swap callee as (collection, player, update, other) at its definition and call 32 (aligned 21: the
// swap call and the occupied loads become retail's, the key sum keeps our grouping, so the key copy and the
// last record's `updates` load trade ECX/EDX); `collection` defined before the loop 277 (1072 bytes), with
// that swap order 276. On 32: the memcmp `continue` as an if block 32 (same code); the conflict flag in the
// scan's loop test 310 (1077 bytes); an early-return session guard 32 (same code). On 28 (after the full
// check): the swap's update through the `update` local 28 (same code); the occupied key through the loop-top
// `player` 291.
// Round 148 (fast, on the swap order (collection, player, update, other) at definition, declaration
// and call: 32, the swap call and the occupied block's head as retail): a dword second-loop index 32
// (`jb` for retail's `jl`); the occupied key as `&collection->players[i]` 32 and `collection` taken in
// the else branch only 32 (both same code); `collection` declared before the second loop 276 (1072 bytes:
// the key becomes a spilled `collection + 4` and the cursor offset goes); 1, 2, 3 or 8 unused locals at
// the function top, the second loop's top or before `slot` 32 each (code-neutral numbering probes, so the
// key sum's grouping is no symbol-order tie: retail ranks `collection` with the loop invariants).
// Round 109 (lane 8, fast; aligned = difflib count, base 8): on the swap order above (36 in its notes, 32 now,
// aligned 15): the second-loop player as a const pointer 32 (28 without the swap order, same code); the occupied
// record's key copied before its type/index stores 37; the occupied record's update as `update = updates +
// *count; ++*count;` 32 (1056 bytes, aligned 9: its head indexes by the old count, but after the key the copy
// takes retail's EDX/ECX and the last record's reads become retail's, so the last record's register trade on the
// swap order follows the key copy's registers); the last record's two source reads before its update pointer
// 36; `collection` declared before the second loop 276 (1072 bytes); the occupied key through
// `watcher->players.players[i].key` 292 (frame 0x8c). Permuter (600 s, 74 tries) none better.
// Round 111 (lane 8, fast; one-file LTCG harness exact, 28/32): on the swap order above the occupied
// block's head is retail's and only its key sum differs ((offset + cursor) + collection against
// retail's (collection + offset) + cursor; the key copy and the last record's reads then trade
// registers). `slot`/`occupied` initialized to NONE/false 168 (1074 bytes), with the swap order 168; the
// swap test as `slot != i && slot != NONE` 322 (1091 bytes); the occupied key as `i[collection->players]`
// on the swap order 32 (same code as `(i + collection->players)`, `&...key[0]` and `key + 0`, harness);
// the key copies as 12-byte struct assignments (diagnostic cast) 28 (same code); the occupied key from
// the loop-top `player` 291 (harness, either swap order); `if (occupied == true)` 110 (harness).
// Round 71: on the swap order, a multi-use player-index local 32; direct unknown14/controller/both
// 107/107/106; const value locals 32; collection declared at function scope or const 32/32;
// inline swap and inline swap/apply-update 32/32. Original body and declarations restored.
// Round 56 (lane 11, fast; positional/aligned, base 28/11): `&watcher->players` at every use with no collection local
// 294/225 (1064 bytes, frame 0x8c); the occupied key as `(byte *)collection + i * size + 4` 28/11, `(byte *)players +
// i * size` 28/11, `i * size + (byte *)players` 28/11, a dword-pointer index 28/11 (all same code); a named occupant
// pointer at the top of the occupied block 45/29. Reading: the key's tree has the product on the left here, so its
// offset + cursor stays grouped; retail groups the base with the offset, as if the product were a shared temporary.
// Round 125 (m3 lane 7, fast; positional/aligned/masked, base 28/8/4): on the swap order 32/16/5: occupied block through a
// forceinline helper taking the update or the update pointer, key copy through a forceinline helper, collection taken
// only in the else arm, `(collection->players + i)->key`, a named occupant pointer, a second collection local in the
// occupied block, a const collection pointer: 32 each (same code); without the swap order: helpers 28/8/4, named
// occupant 41/20/7, second collection local 28/8/4.
// Round 133 (fast): both spots are one ECX/EDX step (retail's `updates` load takes ECX after the slot push in the
// swap set-up and EDX after the inverse-base load in the occupied one; ours the other way), and the occupied key
// keeps (inverse base + cursor) + collection. The find-slot key through a named `t_player_key` pointer 28; find-slot
// given `&watcher->players` 28; the swap's update through a block-scoped pointer 28; `collection` declared after
// `source` and `player` 28 (all same code); a const `collection` before the second loop 277 (1072 bytes).
// Round 67 (m3 lane 14, fast with 0x84e90/0x69260): the swap callee as (collection, player, update, other)
// 32 (retail pushes the slot before loading `updates`, its argument order; 0x84e90 stays matched), with the
// occupied key through a const reference 32, with the occupied update as `updates + ++*count - 1` 32; on
// the current order the occupied key as `&collection->players[i].key` 28 and that update form 28 (same code).
// Round 140 (lane 8, fast with 0x83c90/0x84e90/0x84c90/0x69260): `if (occupied != false)` 28; `collection`
// as a const pointer 28; `slot` and `occupied` declared in the other order 28, at the loop top 28; the
// session-mask test wrapping the body in place of its `continue` 28 (all same code); `collection` at
// function scope, with or without the first loop using it, 228/228 (1088 bytes, frame slots move). Retail's two loop
// invariants: [esp+0x10] is the cursor (watcher + 0xb0 + i * 0xb4, the player's configuration) and
// [esp+0x34] is minus its start (0xffffff50 - watcher), so retail's key is (collection - start) + cursor + 4
// and ours (cursor - start) + collection + 4: retail pairs `collection` with the other invariant.
// Round 79 (fast): the inverse start is used only by the occupied key (the strength-reduced i * 0xb4); any
// watcher-based key folds into the cursor (cursor - 0x24, frame 0x8c), so retail's base is opaque yet ranked
// invariant. Every else-branch use through `&watcher->players` 294 (1064 bytes); only the key as
// `(&watcher->players)->players[i].key` 294; the swap order above 32 (re-measured), with the else-branch calls
// through `&watcher->players` 32; `slot`/`occupied` at the else branch's top 28 (same code); the second loop's
// `update` declared `= NULL` 28 (same code).
// Round 175 (fast with 0x69260/0x84e90/0x84c90): the swap's update as `updates + ++*count - 1` 28, with the occupied
// update the same way 28 (both same code); the occupied key through a const reference to `collection->players[i]` 41;
// the swap's update through a block-scoped pointer 28 (same code); the occupied key as
// `watcher->players.players[i].key` 294 (1064 bytes).
// Round 106 (lane 9, fast with 0x84e90/0x69260/0x84c90/0x83c90), all on the swap order (collection, player, update,
// other) at definition, declaration and call (32, the swap call's `updates` load then retail's): find_slot given
// `&watcher->players` 32; the swap's update in the `update` local in its own block 32; the occupant key pointer
// taken before the swap call 222; `slot`/`occupied` declared at the else arm's top 32; the conflict scan through
// `collection` (players and mask) 264. Swap order restored to main's.
// Round 157 (m3 lane 8, fast with 0x83c90/0x84e90/0x84c90/0x69260): the count through a `long &` reference to `*count`
// at every use 28; `updates` through a base-pointer local at every use 28 (both same code).
// Round 115 (m3 lane 9, fast with 0x84e90/0x84c90/0x83c90/0x69260): reading: ours flattens the occupied key as inverse
// base, cursor, collection and retail as collection, inverse base, cursor, so retail's tree had the collection ahead of
// the index product. The occupied key through an inline player accessor given `&watcher->players` 294 (1064 bytes: the
// argument is substituted and the key folds into the cursor, as the direct expression does), given `collection` 28
// (same code); that accessor with every else-branch call given `&watcher->players` 294, forceinline 294, with no
// collection local in the whole second loop 294; one collection variable assigned in each branch 28, assigned only
// after the conflict scan with the first branch's call given `&watcher->players` 28, the occupied key indexed by the
// record's stored player index 28 (all three same code).
// Round 176 (m3 lane 6, fast; all restored at 28): the occupied record's update as `&updates[*count]; ++*count;` 51 (1056 bytes);
// the swap's update as `&updates[(*count)++]` 40 (1056); the last record's update as `&updates[*count]; ++*count;` 42 (1056);
// `source` as `&((const s_simulation_session_player *)session->players)[i]` 28 (same code). Permuter (fast, anneal, context
// 0x84e90/0x69260; reduced image agrees at 28): about 30 tries in 8 minutes, none better (stopped at the run's time limit).
// The occupied record's apply-update given `&watcher->players` 28 (same code); the swap test as `slot != NONE && i != slot`
// 29.
// Round 101 (m3 lane 11, fast with the file's functions and 0x84e90/0x84c90/0x69260, all others MATCH): an unused local at
// the top 28 (same code). On the swap order (collection, player, update, other) at definition, declaration and call
// (re-measured 32: the swap call and the occupied block's head are retail's; left are the occupied key's grouping, ours
// (inverse + cursor) + collection, retail (collection + inverse) + cursor, and the last record's load order): the last
// record's controller read declared before unknown14 43; the occupied key indexed by `update->player_index` 32, as
// `&update->key` / `&...key` 32, `collection` defined before the session-mask `continue` 32, `collection` through an
// inline accessor returning `&watcher->players` 32 (all four the same code as the swap-order base). Reading: the key sum
// is canonical for every spelling; only the operand ranks move it, and none of these moves `collection` below the
// hoisted inverse base. Restored to main's order.
// M3 lane 16 r121 (fast with 0x84e90/0x84c90/0x83c90/0x69260): the else arm from the slot search on as a forceinline
// helper given `&watcher->players` as its collection parameter 28 (same code: the argument is substituted); the second
// loop's collection as a reference 28 (same code); one function-scope counter for the three loops 28 (same code); the
// loop-top player as `&collection->players[i]` 267 (1064 bytes); an occupant pointer `&collection->players[i]` at the loop
// top for the occupied key 278 (1092 bytes).
// Round 108 (m3 lane 1, fast with 0x84e90/0x69260/0x84c90/0x83c90, all MATCH): `collection` from the watcher parameter
// read once through its own address 28 (same code: the base was already unfolded); the whole second loop as a
// forceinline helper given `&watcher->players` as a collection parameter defined ahead of the loop 277 (1072 bytes:
// the collection preheader code); the swap guarded by nested `slot != NONE` / `slot != i` tests 28; `updates` copied
// into a const local used by every record 28; the swap's count increment as its own statement (re-measured on this
// base) 28.
// Round 107 (m3 lane 11, fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH): no loop-top player pointer (its
// fields read at each use in the test and the compare) 32 (the preheader cursor goes back to EDX, the key keeps ours);
// the player pointer declared inside the first arm only, the test reading the field 32; the occupied record's type
// stored after its key copy 39; a named occupant pointer before the update (round 56's form, re-measured) 45; the two
// arms swapped under the negated test 28 (same code); `source` and `player` declared above the session-mask
// `continue` 251. Reading: a key product that does not dominate (no loop-top player) still keeps our grouping.
// Round 187 (m3 lane 6, fast with 0x84e90/0x69260, which matched in each), on the swap order (collection, player,
// update, other) at definition, declaration and call (32: the swap call is retail's): the occupied key copied before
// its type and index stores 37; the occupied key through a `players` array pointer (`collection->players`) 30 (the
// last record's reads then agree; the key sum is (offset + cursor) + (collection + 4)), that pointer taken before the
// update 30; a const `collection` defined before the second loop 276 (1072 bytes: the key then gets its own pointer
// induction variable in a stack slot, which retail does not have, so retail's collection is not loop-invariant).
// M3 lane 5 r190 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all matched each time): the swap callee as (collection,
// player, update, other) at definition, declaration and call, re-measured 32 (the swap call and the occupied block's head are
// retail's; left is the occupied key, ours (inverse + cursor) + collection + 4 against retail's (collection + inverse) + cursor
// + 4, and the ECX/EDX trades that follow it). On that order: `collection` defined (non-const) just before the second loop
// 276 (1072 B), at the function top after the session 221 (1088 B), both giving the key its own induction variable; the key
// as `(byte *)collection->players + offset` with a named `offset = i * size` 32, as `(byte *)collection + (i * size + 4)`
// 32 (both the same code: every spelling is canonicalised with the scaled index first). Restored to main's order.
// M3 lane 10 r114 (fast with 0x69260/0x84e90/0x84c90/0x83c90/0x83570/0x83c20, all matched each time): the occupied key as
// `(collection->players + i)->key` 28 (same code); `collection` declared once before the second loop and assigned at the
// loop top 28 (same code); on the swap order (collection, player, update, other) at definition, declaration and call,
// re-measured 32: that key spelling 32, the key as `(byte *)collection + 4 + i * sizeof(player)` 32 (both the same code);
// on main's order, separate `removal`/`addition` record pointers for the occupied and last records (the first arm's
// pointer block-scoped) 103 (the last record's loads reorder).
// M3 lane 14 r107 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all matched each time), the #1225 scratch-rotation idea
// on the swap set-up (retail loads `updates` into ECX after the slot push): a forced-inline update allocator
// (`&updates[++*count - 1]`) at all five sites 28; the slot through a forced-inline accessor at the swap call 28; the
// swap's update base through a forced-inline identity accessor 28; its index through a forced-inline `++*count - 1`
// accessor 28; for the occupied key, `collection` bound by reference (`s_type_c67652 *&`) at the key copy 28 (all the
// same code).
// M3 lane 11 r114 (fast with 0x84e90, which matched in each): on the swap order (collection, player, update, other) at
// definition, declaration and call (re-measured 32): the occupied key through a `players` array pointer taken in the
// occupied block 30 (re-measured; the last record's reads agree), that pointer taken at the loop top 280 (1088 bytes),
// right after the slot search 27 (it sinks to the key; the key sum is (offset + cursor) + (collection + 4) where retail
// adds (collection + offset) first, so only the occupied block's head differs), after the machine pointer 320 (1089
// bytes); a 600 s annealing permuter run on the 30 form (seed 114, 84 tries) none below 30. Restored to main's order.
// M3 lane 13 r110: no trial (baseline 28 at 1062 bytes, first difference +0x28). Every form I could derive is
// already listed above, so none was re-run. Reading added from retail: `collection` is formed once, in ESI, right
// before the slot search (`add esi, 0x88` on the reloaded watcher) and stays there across the search and swap calls
// and through the occupied record; ESI is then reused for the last record's source and watcher + 0x88 is formed
// again for the final apply, whose tail retail shares with the first arm. So retail's `collection` behaves like a
// common subexpression of `&watcher->players` held over that region (which ranks it with the invariants in the key
// sum) rather than a loop-top variable; no listed form gives that without folding the key into the cursor.
// M3 lane 1 r121 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): `collection` declared in the else arm
// just before the slot search, the first arm's apply given `&watcher->players`, 28 (same code); that on the swap order
// (collection, player, update, other) 32 (same code as the swap-order base); on the swap order a `players` array pointer
// (`collection->players`) declared ahead of the slot search for the occupied key 27 (1062 bytes: every row but the
// occupied block's head is retail's; the key sum is (offset + cursor) + (collection + 4)); the same pointer declared in
// the occupied block with the late `collection` 30; the pointer ahead of the slot search on main's order 33. Reading:
// retail's sum is (collection + offset) + cursor + 4, as if the key's tree were `collection + i * size` with the base
// first, so the strength-reduced index flattens into it; ours keeps the index's replacement (offset + cursor) whole.
// M3 lane 10 r122 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): the second loop's first mask test
// through `collection->player_mask` 193 (1063 bytes); the conflict scan's mask test through it 193 (1063); the conflict
// scan's player as `&collection->players[j]` 263; a `players` array pointer (`collection->players`) declared right after
// the slot search on main's order 33 (the key is then (offset + cursor) + (collection + 4)); diagnostic, not keepable (a
// comma loop): the second loop's player as a pointer stepped with `i` 299 (1095 bytes; the key is then a sum of two
// spilled values, so the shared `i * size` is what keeps our (offset + cursor) whole).
// M3 lane 8 r193 (compile-only screen of the occupied key's grouping, which reproduces ours: the inverse start and the cursor
// are summed first and `collection`, a compiler temporary there, is added last; then fast with 0x84e90/0x84c90/0x83c90/0x69260,
// all MATCH): 0-40 initialised dead locals at the function top (K = 0, 1, 2, 4, 8, 12, 13, 14, 16, 20 ... 40) leave the
// grouping as ours, so it is no symbol-count tie; `collection` declared in the else arm before the slot search with the
// first arm's apply given `&watcher->players`, no loop-top player pointer, the swap order (collection, player, update,
// other), and those three together keep the grouping (screen); the three together in the real build 36.
// M3 lane 10 r124 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): the occupied record's update as
// `++*count; update = &updates[*count - 1];` 28 (same code); that on the swap order (collection, player, update, other) at
// definition, declaration and call 32; `collection` defined above the session-mask `continue` 28 (same code); on the swap
// order a `players` array pointer declared ahead of the slot search, re-measured 27 (the occupied block's head: ours
// (inverse + cursor) + (collection + 4), retail (collection + inverse) + cursor + 4), with the occupied update as above 27;
// the conflict scan as `j < 16 && !conflict` with no break 310 (1077 bytes). Diagnostic (a comma loop, not keepable): a
// byte offset stepped beside `i` for the occupied key 283 (1080 bytes: the offset keeps its own induction variable).
// M3 lane 7 r190 (fast with 0x84e90/0x84c90/0x83c90/0x69260/0x83c20/0x83570, all MATCH each time): `collection` spelled as
// the watcher plus 0x88 by byte offset 28; `collection` a `* const` local 28; the find-slot outputs declared at the function
// top 28; the occupied key read through a `const &` bound to `collection` 28 (all the same code); that binding on the swap
// order (collection, player, update, other) at definition, declaration and call 32 (the swap set-up is retail's, the key
// sum stays ours). Reading: of the 28, only the swap set-up (6 rows, 2 register swaps) and the occupied key's head differ;
// ours is 1 byte shorter there (`add` + `lea` against retail's two `lea`s), so every later jump, the two later apply calls
// and the closing pad shift by one byte and count as well. All restored.
// M3 lane 12 r151 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; compile-only screen of the key, which
// reproduces ours as (inverse + cursor) + collection + 4): `collection` taken through a reference bound to
// `watcher->players` 28 (same code); the occupied key as `(byte *)(i * size) + (long)collection + 4` 28 (same code); on the
// swap order (collection, player, update, other) at definition, declaration and call: that key 32, a players-array pointer
// named between the swap and the occupied test 321 (1090 bytes), the key through an element pointer cast from `(byte
// *)collection + 4` 32. Screen only (same grouping): `collection` declared in the else arm, `(collection->players + i)`,
// a `players` array pointer before the slot search, a `(dword)i` index; `collection` before the second loop or at the top
// gives the key its own induction variable. Reading: a user pointer local as the base (`players`) is also added last, so
// the grouping is not a temporary-numbering tie; retail's (collection + inverse) is the shape of a whole-address reduction
// with `collection` treated as invariant, which every in-loop definition here loses.
// M3 lane 10 r132 (compile-only key screen, which reproduces ours, then fast with 0x84e90/0x84c90/0x83c90/0x69260/0x83c20/
// 0x83570, all MATCH): the find-slot/swap/remove/add tail as a forceinline helper given `&watcher->players` (no collection
// local in the else arm) 28; that with the first arm's apply also given `&watcher->players` and no collection local at
// all 28; the conflict scan as a forceinline bool helper 28; both helpers 28; the two arms swapped (the else arm first,
// the test negated) 28 (all the same code). Screen only: the watcher taken as `void *` and cast to a local (0x83370's
// form) keeps the grouping; `collection` before the second loop, or at the top and used by the first loop's apply too,
// gives the key its own induction variable (a fourth one: retail has i, the cursor and the session player only). In
// every in-loop form the screen names the base a compiler temporary (watcher + 0x88) and adds it last. All restored.
// M3 lane 11 r132 (fast with 0x84e90/0x84c90/0x83c90/0x69260/0x83c20/0x83570, all MATCH; a one-object LTCG screen of the file with
// 0x84e90's file, checking the swap set-up and the key grouping): the occupied remove step as a forceinline helper given (collection,
// i, update) 28 (same code); `collection` at function scope set before both loops 228 (1088 B, the key gets its own induction
// variable); `collection` at function scope set at the second loop's top and in the first loop's update block 28; the key from
// `(byte *)collection + 4 + i * sizeof(player)` 28, through `((s_simulation_owner_player *)((byte *)collection + 4))[i]` 28 (same
// code). Re-measured the swap order (collection, player, update, other) 32: the swap set-up and the occupied block's load order are
// then retail's, left the key sum and the final block's two source loads (ECX/EDX swapped). Screen only, same code:
// `&collection->players[i].key[0]`, a `(dword)i` subscript, the byte sum with the offset last. All restored.
// M3 lane 6 r210 (fast): the second loop's `player` and `source` declared ahead of `collection` 28 (same code); `player`
// as `&collection->players[i]` 267 (1064 B); the swap's update named first (`update = &updates[++*count - 1];` then the
// call) 28 (same code); the occupied key copied from `&collection->players[i]` 28 (same code); the occupied key copied
// through the loop's `player` pointer (the same element) 291.
// M3 lane 7 r199 (fast with 0x84e90/0x84c90/0x83c90, all MATCH): the occupied key read through
// `watcher->players.players[i]` with `collection` kept for the calls 294 (1064 B); the swap's count step as its own
// `++*count;` with `&updates[*count - 1]` 28 (same code); the swap's update as `&updates[(*count)++]` 40 (1056 B); the
// occupied key through a named `occupant` element pointer 41; the swap's update as `updates + ++*count - 1` 28 (same
// code).
// M3 lane 16 r156 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): the occupied key copied element by
// element from `collection->players[i].key[0..2]` 280 (1066 B; diagnostic, a copy written out); a const occupant pointer
// (`collection->players + i`) named after the type and index stores 41; the swap's update named from `&updates[*count]`
// before a separate `++*count;` 40 (1056 B); the occupied key from `&collection->players[i].key[0]` with
// `sizeof(t_player_key)` 28 (same code); the swap's update as `updates + ++*count - 1` 28 (same code). Reading (from the
// preheader): the two stack cursors are watcher + 0xb0 stepped by 0xb4 and its negation -(watcher + 0xb0), so the key's
// `i * 0xb4` is their sum; retail adds `collection` to the negated start first, ours adds the two cursors first.
// M3 lane 12 r164 (fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): the occupied record's index stored after
// its key copy 36, its type after the key copy 39; a `players` array pointer named right after the slot search for the
// occupied key on main's order 33; the key as `(collection->players + i)->key` with `sizeof(t_player_key)` 28 (same code).
// On the swap order (collection, player, update, other) at definition, declaration and call, re-measured 32: the occupied
// key through a `players` array pointer named right after or right before the slot search 27/27 (a gain of one from a
// one-use pointer and a parameter reorder whose own callee matches either way: not kept). All restored.
// M3 lane 8 r210 (fast with 0x84e90/0x84c90/0x83c90/0x69260/0x83c20/0x83570, all MATCH; control 28): re-measured the swap
// callee as (collection, player, update, other) at definition, declaration and call 32 (swap call and occupied head
// retail's; the key grouping then leaves the key in ECX and the destination in EDX, and the last record's `updates` and
// field loads swap registers); on that order `controller_index` declared before `unknown14` 43, no last-record locals
// (fields read at their stores) 106 (1053 B), the occupied key as `&collection->players[i].key[0]` 32 (same code), the
// last record's two locals read at the else arm's top 275 (1087 B); diagnostic on that order: 1 to 8 unused
// initialised locals at the top 32 each (no symbol-count tie). All restored.
// M3 lane 16 r159 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): the occupied key indexed by the
// stored `update->player_index` 103 (the store is forwarded on the swap order (collection, player, update, other), 32);
// on that swap order: `collection` dropped from the slot search and swap (both given `&watcher->players`) and declared
// after the swap, just before the occupied test, 32; the occupied key through a `t_player_key const &` bound to
// `collection->players[i].key` 32; a named occupant pointer taken ahead of the count step 40. The key sum stays (inverse
// + cursor) + collection + 4 in each; the swap order alone re-measured 32.
// M3 lane 16 r161 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; main's swap order): a named element
// offset (`i * sizeof(player)`) shared by the loop-top player and the occupied key 294 (1083 B); a const `players` array
// pointer named at the occupied block's top, the key as `occupant[i].key`, 36 (the key keeps our grouping); a const
// occupant element pointer named right after the slot search 152 (1073 B), after the swap just before the occupied test
// 205 (1069 B); the occupant element bound to a const reference at the else arm's top 329 (1097 B).
// M3 lane 7 r204 (h2m fast with 0x84e90/0x84c90/0x83c90, all MATCH each time): re-measured the swap callee as
// (collection, player, update, other) at definition, declaration and call 32 (swap set-up and the occupied block's load
// order retail's; left the key grouping, the key then in ECX where retail has EDX, and the last record's controller load
// ahead of `updates`). On that order: the last record's count step as its own statement, the two source locals, then
// `update = &updates[*count - 1]` 32; `collection` a const pointer declared ahead of the session-mask test 32;
// `collection` declared only in the else arm (the first arm's apply given `&watcher->players`) 32; the occupied key
// copied by array addresses (`&update->key`, `&collection->players[i].key`) 32; the occupied record as
// `&updates[*count]` then `++*count;` 32 (1056 B). Compile-only key screen (the file without /GL; the grouping
// reproduced): `collection` at the loop top, through a reference, as a const pointer, the key through `&players[0] + i`
// or a byte sum: all (inverse + cursor) + collection + 4.
// M3 lane 12 r168 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): `bool conflict;` declared, then
// cleared 28 (same code); the machine index kept in a long with `&machines[index]` at both uses 175 (1073 B), that on the swap
// order (collection, player, update, other) 174; the rest of the else arm nested under `if (source->unknown14 != NONE)` in place
// of its `continue` 28 (same code); plain check: the occupied record's removal as a function of the collection's own file
// (type, index, key copy, apply), called with the update, 28 (inlined at link time to the same code). Compile-only key screen
// (reproduces ours: the base is a front-end temporary added after the inverse + cursor pair): `collection` above the
// `continue`, in the else arm before the slot search, after the swap, or at function scope assigned at the loop top all keep our
// sum; a collection loaded from memory each pass (diagnostic) is added last too; an invariant base (defined before the loop, or a
// parameter, diagnostic) gets its own induction variable in every key spelling. All restored.
// M3 lane 7 r206 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): `collection` declared just ahead of
// the slot search (the first arm's apply given `&watcher->players`) 28; the slot search, swap and occupied step as a
// file-local forceinline helper taking (collection, i, source, count, updates) 28 (same code); on the swap order
// (collection, player, update, other) each of those 32 (the swap order's code); `collection` a const pointer set
// between the two loops 277 (1072 B, the key's own induction variable). Compile-only: the helper's parameter and the
// late declaration keep (inverse + cursor) + collection + 4.
// M3 lane 13 r138 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; compile-only key screen, which
// reproduces ours: the cursor and the inverse start are SR temporaries summed first, `collection` a hoisted temporary
// added last; `collection` before the loop gets its own induction variable): the occupied block re-pointing the loop-top
// `player` at `&collection->players[i]` for the key 41; the second loop's counter typed `dword` 28 (same code); the
// machine pointer formed after the `unknown14 == NONE` continue 182 (1071 B); the swap order (collection, player, update,
// other) re-measured 32 (on main's order the last record's loads are retail's; on the swap order ours loads the
// second-declared source read first, the controller, where retail loads `updates`, unknown14, then the controller it
// spills); on the swap order the two source reads declared controller first 43 (still the second-declared read first).
// Screen only (all ours): the key as `(collection->players + i)->key`, as `(byte *)collection + (4 + i * size)`, as
// `((byte *)collection + 4) + i * size`, from `&players[0].key[0]` plus a scaled index. All restored.
// M3 lane 4 r245 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time): re-measured the swap callee as
// (collection, player, update, other) 32; the conflict scan reading through `collection` (`collection->players[j]`,
// `collection->player_mask`) 263 (the second loop's preheader and cursors are rebuilt), on the swap order 264; the
// occupied record's apply given `&watcher->players` 28, the last record's 28 (retail rebuilds it from the parameter there
// too; both the same code), both on the swap order 32. All restored.
// M3 lane 7 r212 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; main's swap order): the occupied key
// bound to a `t_player_key const &` and copied from it 41; the key copied by element addresses with `sizeof(t_player_key)`
// 28; the swap test as nested ifs (slot valid, then slot differs) 28; the occupied record in its own block-local update
// pointer 28; the swap's update named in a block-local pointer inside the swap test 28 (the 28s the same code).
// M3 lane 14 r143 (h2m fast with 0x84e90/0x84c90/0x83c90, all MATCH each time; control 28): the swap's update through a
// `const &` bound to `updates` 28 (same code); the occupied key indexed by the just-stored `update->player_index` 28
// (forwarded, same code); the occupied index stored after the key copy 36; a const occupant pointer to
// `collection->players[i]` formed ahead of the slot search 222 (1073 B, its own induction variable); the last record's two
// source reads named at the else arm's top and used by the conflict scan 274 (1085 B). All restored.
// M3 lane 3 r296 (H2M_FAST with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; control 28; of the 28, the 11
// jumps, two call displacements and the closing pad follow from the key sum being one byte shorter in ours): the
// occupied key read through `watcher->players.players[i].key` 294 (1064 B, the key gets its own induction variable); no
// `collection` local (`&watcher->players` at each use) 294; the swap's count step as its own statement with the call
// passing `&updates[*count - 1]` 28 (same code); the loop-top player formed as `&collection->players[i]` 267 (1064 B);
// the occupied record as `updates + (++*count - 1)` 28 (same code). All restored.
// M3 lane 13 r144 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; control 28): `collection`
// renamed `watcher_players` (a name-order probe for the key sum) 28 (same code); the session player pointer declared
// ahead of the session-mask `continue` 251; the tail after the conflict scan nested under `if (!conflict)` 28 (same
// code); the occupied record's count step as `*count += 1;` 28 (same code); the swap's and the occupied record's
// updates as `&updates[(*count)++]` 63 (1051 B). All restored.
// M3 lane 1 r156 (h2m fast; control 28): the occupied key copied with `sizeof(t_player_key)` 28 (same code); the last
// record's apply given `&watcher->players` re-measured 28. Diagnostics (reverted): the occupied key read at `players[slot]`
// 289 (1051 B, frame 0x8c: the inverse-start slot disappears, so that induction value exists only for the occupied
// key); the occupied key copy removed 329 (1013 B); the swap call removed 262 (1021 B); each also drops a frame slot,
// so neither spot can be isolated by deleting the other.
// M3 lane 1 r158 (h2m fast with 0x84e90, MATCH each time; control 28): the occupied key through a file-local forceinline
// accessor returning `&collection->players[index].key` 28 (same code), with the swap order (collection, player, update,
// other) 32; the second loop without the `player` local (flag and configuration read through
// `watcher->players.players[i]`) 32; the second loop's first mask test through `collection` 193 (1063 B: the mask is no
// longer cached and the loop head reorders); on the swap order, `collection` declared at the else arm's top (the first
// arm's apply given `&watcher->players`) 32. Compile-only key screen (exact for the grouping here): each of those keeps
// (inverse + cursor) + collection + 4; the player local formed from `collection` folds the key into its own pointer.
// Reading: retail's sum is SR's rebasing of `collection + i * 0xb4` onto the configuration cursor, (collection - start) +
// cursor, computed in place; ours rebases only `i * 0xb4` and adds the in-loop `collection` after it, and an invariant
// base gets its own induction variable instead, so retail's base was invariant to the address analysis without being
// strength-reduced on its own.
// M3 lane 8 r226 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH each time; control 28; reference-bound
// locals, which kept a sum apart in 0x211cc0): `collection` through a reference bound to a pointer local defined
// ahead of the second loop 277 (1072 B: the key still gets its own induction variable, so the binding is seen
// through); the occupied key through a reference-bound element pointer built in two steps (`entry =
// collection->players; entry += i;`) 47 (the pointer is formed ahead of the type and index stores and the sum is
// still (inverse + cursor) first). Compile-only (the occupied block alone, which reproduces ours): `collection`
// assigned through a reference at the loop top, the key indexed by a reference-bound copy of `i`, the loop-top
// player assigned through a reference: all the same code as the base, so in this file the bindings are folded
// before the key sum is ordered. All restored.
// M3 lane 1 r161 (h2m fast with 0x84e90/0x84c90/0x83c90, all MATCH each time; control 28): one function-scope `long i`
// for the three loops 28; the session-player source declared ahead of `collection` 28; the loop-top player declared
// first, then source, then `collection` 28; the occupied key copied from `&collection->players[i].key` with
// `sizeof(t_player_key)` 28; `machines` declared at the function's top 28; `slot` and `occupied` declared at the
// function's top 28 (all the same code: the key sum stays (inverse + cursor) + collection). Body unchanged.
// M3 lane 15 r140 (h2m fast with 0x84e90/0x84c90/0x83c90/0x69260, all MATCH; control 28): the conflict scan as a
// `continue` chain (mask and flag, then the machine compare, then the controller/unknown test) 28; the swap test nested
// with `slot != i` outside 322 (1091 B); the swap test held in a named bool 28; the last record's machine copied with
// memcpy 28; the machine pointer as `machines + source->machine_index` 28 (the 28s the same code). Body unchanged.
// M3 lane 6 r238 (H2M_FAST with 0x84e90, 0x84c90, 0x83370, 0x83c90 and 0x83570, MATCH each time; control 28). Two
// causes. (1) The swap call: ours forms the update's address before it pushes the slot, retail pushes the slot and then
// loads `updates` into the freed ECX; that is the argument order of a callee whose third parameter is the update and
// whose last is the other index. (2) The occupant's key address `collection + X + Y + 4` (Y the walking player pointer,
// X the loop-entry value that turns it into i * 0xb4): the optimiser orders the terms of a sum by an internal rank,
// highest first, ours X, Y, collection (two late temporaries outrank the named pointer), retail collection, X, Y; the
// collection term keeps its low rank whether the pointer is const, assigned after its declaration or written as a byte
// offset from the watcher. Forms: the swap count bumped into a local in a statement of its own with
// `&updates[local - 1]` in the call 28; `++*count;` as its own statement and `&updates[*count - 1]` 28 (same code each); a
// post-increment into an index local 120 (1056 B). LEAD, not kept (the count rises): 0x84e90's parameters as (collection,
// player, update, other) in its definition, its declaration and its one call (0x84e90 still MATCH) 32; the swap call's
// argument setup is then retail's exactly and the occupied block's three reloads take retail's registers, so only cause
// (2) is left, whose wrong pairing then swaps ECX/EDX through the key copy. On that lead: the occupant key through
// `watcher->players.players[i]` 292 (1064 B), through `player` 291, `(collection->players + i)->key` 32,
// `&collection->players[i]` as the copy's source 32, collection declared at function scope 221 (1088 B), the conflict
// loop reading the collection pointer 264, collection declared inside the else arm (the first arm passing
// `&watcher->players`) 32, declared just ahead of the slot search 32, both with the conflict loop on it 264.
// M3 lane 12 r191 (h2m fast with 0x84e90/0x84c90/0x83c90, all MATCH each time; control 28): `occupied` initialised false
// 137 (1066 B); `slot` initialised NONE 149 (1069 B); the swap test written slot-differs first (diagnostic) 322 (1091 B);
// `occupied` declared ahead of `slot` 28; both declared ahead of the machine pointer 28 (both the same code). The key sum
// is the same ranking as 0x81780's lone difference (src/unknown_081780.cpp): ours adds the compiler's stepped terms
// first and the named base last, retail the base first; a compile-only screen of that loop over twenty-odd optimisation
// and debug-information options never flips it.
// Roasted-Codes (#1519 comment, 2026-11-26; fast reduced-image scores): the occupied key's index taken as a pointer
// difference, `player - watcher->players.players`, 239 (1099 B; the exact division survives as a multiply by 0xb60b60b7
// and a `sar`).
// M3 lane 1 r172 (h2m fast with 0x84e90/0x84c90/0x83c90, MATCH each run; control 28). Reading of the compiler's sum ranking:
// the occupied key's terms sort by rank, highest first; the two loop temporaries (the inverse start and the stepped player
// address, created late) rank 0xdf40 and 0xd840 above `collection` (a named local, 0x180), and no named variable can rank
// above them, so retail's collection-first sum needs the collection term to be a late temporary itself. Forms: the occupied
// key read as `watcher->players.players[i].key` 294 (1064 B, the player loads merge); the occupied player through a pointer
// local `&collection->players[i]` 41; the loop's player pointer formed from `collection` 267 (1064 B), with the mask test
// also through `collection` 271 (1063 B); `&watcher->players` at each use (no local) re-measured by rank only (the sum
// splits into two-term adds). The (collection, player, update, other) swap order re-measured 32 (left: the key sum, then
// the key/destination ECX/EDX pairing and the last record's two reads). All restored.
// M3 lane 2 r156 (h2m fast with 0x84e90/0x84c90/0x83c90, all MATCH each time; control 28): the occupied key's sum in the
// compiler's second reassociation pass sorts its terms by rank, the inverse start and the stepped configuration cursor
// (both created by strength reduction) above `collection` (an early named symbol), then the constant. Forms: the
// occupied record's index stored before its type 30; `collection` as `s_type_c67652 *const` 28; the occupied key copied
// from `&collection->players[i]` (the element, key first) 28; the machine fill bounded by a `member_count` local 28 (the
// three 28s the same code); the last record's machine stored ahead of its key copy 45. All restored.
// @retail 0x83610
void __stdcall function_83610(s_simulation_world_owner *watcher, long *count, s_simulation_player_update *updates)
{
 long initial_count = *count;
 c_class_58d20 *session = watcher->session;
 if (session && SESSION_STATE_IS_LIVE(session->state) && session->type == 4 && watcher->unknown18 != session->update7618)
 {
  s_machine_address machines[16];
  for (long i = 0; i < session->member_count; i++)
   machines[i] = *(s_machine_address *)((byte *)&session->members[i] + 10);
  for (long i = 0; i < 16; i++)
  {
   s_simulation_owner_player *player = &watcher->players.players[i];
   if ((watcher->players.player_mask & (1 << i)) && !player->flag0c &&
    simulation_player_changed(machines, session->player_mask, (const s_simulation_session_player *)session->players, i, player))
   {
    s_simulation_player_update *update = &updates[++*count - 1];
    update->type = 0;
    update->player_index = i;
    memcpy(update->key, player->key, sizeof(update->key));
    simulation_player_collection_apply_update(&watcher->players, update);
    simulation_watcher_mark_player(watcher, i);
   }
  }
  for (long i = 0; i < 16; i++)
  {
   if (!(session->player_mask & (1 << i)))
    continue;
   s_type_c67652 *collection = &watcher->players;
   const s_simulation_session_player *source = (const s_simulation_session_player *)&session->players[i];
   s_simulation_owner_player *player = &watcher->players.players[i];
   s_simulation_player_update *update;
   if ((watcher->players.player_mask & (1 << i)) && !player->flag0c)
   {
    if (!memcmp(source->configuration, player->configuration, sizeof(player->configuration)))
     continue;
    update = &updates[++*count - 1];
    update->player_index = i;
    update->type = 4;
    memcpy(update->key, source->key, sizeof(update->key));
    memcpy(update->configuration, source->configuration, sizeof(update->configuration));
    simulation_player_collection_apply_update(collection, update);
   }
   else
   {
    const s_machine_address *machine = &machines[source->machine_index];
    if (source->unknown14 == NONE)
     continue;
    bool conflict = false;
    for (long j = 0; j < 16; j++)
    {
     const s_simulation_owner_player *other = &watcher->players.players[j];
     if ((watcher->players.player_mask & (1 << j)) && !other->flag0c &&
      !memcmp(&other->machine, machine, sizeof(*machine)) &&
      (other->controller_index == source->controller_index || other->unknown20 == source->unknown14))
     {
      conflict = true;
      break;
     }
    }
    if (conflict)
     continue;
    long slot;
    bool occupied;
    simulation_player_collection_find_slot(collection, i, &source->key, &slot, &occupied);
    if (slot != NONE && slot != i)
     simulation_player_collection_swap(collection, i, slot, &updates[++*count - 1]);
    if (occupied)
    {
     update = &updates[++*count - 1];
     update->type = 2;
     update->player_index = i;
     memcpy(update->key, collection->players[i].key, sizeof(update->key));
     simulation_player_collection_apply_update(collection, update);
    }
    update = &updates[++*count - 1];
    long unknown14 = source->unknown14;
    long controller_index = source->controller_index;
    update->player_index = i;
    update->type = 3;
    memcpy(update->key, source->key, sizeof(update->key));
    update->machine = *machine;
    update->field_2_2 = false;
    update->controller_index = controller_index;
    update->unknown20 = unknown14;
    memcpy(update->configuration, source->configuration, sizeof(update->configuration));
    simulation_player_collection_apply_update(collection, update);
   }
  }
  watcher->unknown18 = watcher->session->update7618;
  watcher->unknownc30 = *count > initial_count;
 }
}


void function_14c630(dword valid_mask, const s_machine_address *addresses);
void players_set_local_machine(const s_machine_address *address);

// @retail 0x83370
void __stdcall function_83370(void *watcher_, dword flags)
{
 s_simulation_world_owner *watcher = (s_simulation_world_owner *)watcher_;
 if (!(flags & 4))
 {
  s_watcher_machines *source = (s_watcher_machines *)g_4e8c20;
  dword mask = source->count;
  const dword *machines = source->machines;
  c_class_6a600 *world = watcher->world;
  s_machine_address addresses[16];
  s_machine_address local = world->local_address;
  long index = NONE;
  memcpy(addresses, machines, sizeof(addresses));
  for (long i = 0; i < 16; i++)
   if ((mask & (1 << i)) && !memcmp(&addresses[i], &local, sizeof(local)))
    index = i;
  if (index == NONE)
   players_set_local_machine(0);
  else
   players_set_local_machine(&addresses[index]);
  watcher->unknown84 = true;
  simulation_watcher_rebuild_players(watcher);
 }
 else
 {
  byte *source = (byte *)g_4e8c20;
  bool valid = *(bool *)(source + 0x90);
  s_machine_address local;
  if (valid)
   local = *(s_machine_address *)(source + 0x91);
  watcher->unknown1c = 1;
  watcher->unknown20 = 0;
  memset(watcher->unknown24, 0, sizeof(watcher->unknown24));
  if (valid)
   ((s_machine_address *)watcher->unknown24)[watcher->unknown20] = local;
  function_83001(watcher->world, &((s_machine_address *)watcher->unknown24)[watcher->unknown20]);
  *(long *)(watcher->world->unknown13 + 1) = watcher->unknown20;
  function_14c630(watcher->unknown1c, (s_machine_address *)watcher->unknown24);
  players_set_local_machine(&((s_machine_address *)watcher->unknown24)[watcher->unknown20]);
  watcher->unknown84 = false;
  simulation_watcher_rebuild_players(watcher);
 }
}

inline c_vtable_450c94::~c_vtable_450c94() {}
inline c_vtable_450d1c::~c_vtable_450d1c() {}
inline c_handle_table_450cd0::~c_handle_table_450cd0() {}
inline c_vtable_450cf4::~c_vtable_450cf4() {}
struct s_flagged;
extern s_flagged *g_4d87ec;
extern s_flagged *g_4d87f0;
void function_85880(c_simulation_view *view);

// @retail 0x843f0
bool __stdcall function_843f0(s_simulation_world_owner *watcher, long current_member)
{
 c_simulation_view **view_address = (c_simulation_view **)&current_member;
 dword present = 0;
 s_view_iterator iterator;
 iterator.mask = NONE;
 iterator.index = 0;
 while (world_next_view(watcher->world, &iterator, view_address))
 {
  c_simulation_view *view = *view_address;
  if (view->failure_reason)
  {
   if (view->type)
   {
    if (view->channel_index != NONE) function_85880(view);
    if (view->world) view->detach();
    view->type = 0;
   }
   c_replication_view_storage *storage = (c_replication_view_storage *)view->data;
   long index = view->unknown04;
   record_pool_release((s_record_pool *)g_4d87ec, index);
   if (storage)
   {
    storage->~c_replication_view_storage();
    record_pool_release((s_record_pool *)g_4d87f0, index);
   }
  }
  else present |= 1 << view->unknown1c;
 }
 if (watcher->world->unknown18 != 1)
 {
 c_class_58d20 *session = watcher->session;
 long local = session->current_member;
 for (long i = 0; i < session->member_count; i++)
 {
  if (i != local && !(present & (1 << i)))
  {
   long channel = NONE;
   c_class_58d20 *current = watcher->session;
   if (i >= 0 && i < current->member_count)
   {
    byte *entry = (byte *)current + 0x72dc + i * 20;
    if (entry[1]) channel = *(long *)(entry + 4);
   }
   function_84630(channel, watcher, i, (const XNADDR *)&session->members[i]);
  }
 }
 return true;
 }
 return false;
}

static __forceinline long watcher_find_current_machine(dword mask, const s_machine_address *machines, s_machine_address address)
{
 for (long i = 0; i < 16; i++)
  if ((mask & (1 << i)) && !memcmp(&machines[i], &address, sizeof(address))) return i;
 return NONE;
}
void function_12d520(long address);

// @retail 0x83f00
bool __stdcall function_83f00(s_simulation_world_owner *watcher, long update, long count, long host, const s_session_member *members)
{

 bool local_2 = true;
 if (!(watcher->unknown10 == update))
 {

  dword mask = (1 << count) - 1;
  s_machine_address machines[16];
  memset(machines, 0, sizeof(machines));
  for (long i = 0; i < count; i++) machines[i] = *(const s_machine_address *)((const byte *)&members[i] + 10);
  c_class_6a600 *world = watcher->world;
  long local = watcher_find_current_machine(mask, machines, world->local_address);
  if (local == NONE) { local_2 = false; goto local_3; }
  if (local != *(long *)((byte *)world + 0x14)) *(long *)((byte *)world + 0x14) = local;
  c_simulation_view *view;
  s_view_iterator iterator;
  iterator.mask = NONE;
  iterator.index = 0;
  while (world_next_view(watcher->world, &iterator, &view))
  {
   long member = watcher_find_current_machine(mask, machines, view->address);
   if (member == NONE)
   {
    if (view->type)
    {
     if (view->channel_index != NONE) function_85880(view);
     if (view->world)
     {
      if (view->type == 2 && view->buffer)
      {
       view->unknown9c = 0;
       view->unknowna0 = 0;
       function_12d520((long)view->buffer);
       view->buffer = 0;
       view->unknown98 = 0;
      }
      c_class_6a600 *local_0 = view->world;
      local_0->views[view->world_index] = 0;
      local_0->view_count--;
      view->world_index = NONE;
      view->world = 0;
     }
     view->type = 0;
    }
    c_replication_view_storage *storage = (c_replication_view_storage *)view->data;
    long index = view->unknown04;
    record_pool_release((s_record_pool *)g_4d87ec, index);
    if (storage)
    {
     storage->~c_replication_view_storage();
     record_pool_release((s_record_pool *)g_4d87f0, index);
    }
   }
   else if (member != view->unknown1c) view->unknown1c = member;
  }
  world = watcher->world;
  if (world->state != 3 && world->state != 5)
  {
   for (long i = 0; i < 16; i++)
   {
    world = watcher->world;
    s_simulation_world_player *player = &world->players[i & 0xffff];
    if (player->player_index != NONE && player->flag25 &&
     watcher_find_current_machine(mask, machines, *(const s_machine_address *)world->players[i].unknown18) == NONE)
    {
     if (world->players[i].flag25) world->players[i].flag25 = false;
     world->players[i].flag24 = true;
    }
   }
  }
  long local_5 = *(volatile long *)&host;
  watcher->unknown10 = update;
  watcher->unknown1c = mask;
  watcher->unknown14 = local_5;
  watcher->unknown20 = *(long *)((byte *)watcher->world + 0x14);
  memcpy(watcher->unknown24, machines, sizeof(machines));
  watcher->unknown84 = true;
  watcher->unknownc30 = true;


 }
local_3:
 return local_2;
}

struct s_network_session_membership;
s_network_session_membership *function_5a680(c_class_58d20 *session, long *current_member, long *member_index);
bool function_058d70(c_class_58d20 *session);
bool function_058d90(c_class_58d20 *session);
bool function_6ae20(c_class_6a600 *world);
bool function_6aee0(c_class_6a600 *world);
void function_6b310(c_class_6a600 *world);
void function_6b350(c_class_6a600 *world);

// @retail 0x83220
bool function_83220(s_simulation_world_owner *watcher)
{
 bool result = true;
 c_class_6a600 *world = watcher->world;
 if (!world) goto failed;
 if (world->state == 1) goto done;
 if (!g_527330.initialized || (g_527330.state != 3 && g_527330.state != 8)) goto failed;
 {
  c_class_58d20 *session = watcher->session;
  if (!session || !session->state) goto failed;
  bool live;
  if (session->function_058d20()) live = session->state == 5;
  else live = function_058d70(session);
  if (live)
  {
   result = session->function_058d20();
   bool authority = function_67e10(world);
   if (!result && authority)
   {
    if (!function_6ae20(world)) goto failed;
   }
   else if (result && !authority)
   {
    if (!function_6aee0(world)) goto failed;
   }
   if (function_058d90(watcher->session))
   {
    if (result) function_6b310(watcher->world);
    else function_6b350(watcher->world);
   }
   long member_index, current_member;
   s_watcher_membership *members = (s_watcher_membership *)function_5a680(watcher->session, &current_member, &member_index);
   long update = watcher->session->update7618;
   if (!function_83f00(watcher, update, members->count, members->host, members->members)) goto failed;
   if (function_67e10(watcher->world)) result = function_843f0(watcher, current_member);
   else result = function_84560(watcher);
  }
  else function_6b040(world);
 }
 return result;
failed:
 result = false;
done:
 return result;
}
