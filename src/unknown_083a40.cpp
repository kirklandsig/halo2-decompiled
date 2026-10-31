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

// score 32 (was 32): the address of the record is computed in EDX and the zero kept in EBX;
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
// Round 54 (lane 12, fast with 0x69260/0x84e90): retail loads the occupied key's IV offset before the
// `updates` pointer; the occupied key pointer named before the update 49 (ours still adds offset + cursor
// first); `++*count;` as its own statement at the four second-loop sites 32 (same code); both 49; the
// swap callee as (collection, player, update, other) with the key pointer first 44, with the count
// statements 36.
// Round 36 (lane 10, fast): one function-scope update pointer for both loops 32; `slot`/`occupied` at
// function scope 32; the occupied record through its own scoped pointer 32, the last record likewise 32,
// both 32 (all same code). Retail loads `updates` last in the swap and occupied set-ups and groups
// (collection + offset) first in the occupied key, as if that sum were treated as loop-invariant.
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
   const void *configuration = (const byte *)source + 0xa8;
   s_simulation_owner_player *player = &watcher->players.players[i];
   s_simulation_player_update *update;
   if ((watcher->players.player_mask & (1 << i)) && !player->flag0c)
   {
    if (!memcmp(configuration, player->configuration, sizeof(player->configuration)))
     continue;
    update = &updates[++*count - 1];
    update->player_index = i;
    update->type = 4;
    memcpy(update->key, source->key, sizeof(update->key));
    memcpy(update->configuration, configuration, sizeof(update->configuration));
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
    memcpy(update->configuration, (const byte *)source + 0xa8, sizeof(update->configuration));
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
