// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_067E10.CPP: the simulation world (g_4cf77c) and the simulation
   globals' queries (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_123b30.h"
#include "unknown_067e10.h"
#include "unknown_0662e0.h"
#include "unknown_075870.h"

#define SIMULATION_WORLD ((c_class_6a600 *)g_4cf77c)
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

// @retail 0x814f0
c_class_6a600::c_class_6a600()
{
	c_class_6a600 *world = this;
	s_simulation_world_player *player = world->players;
	for (long i = 0; i < 16; i++, player++)
	{
		player->player_index = NONE;
		player->unknown04 = NONE;
		player->unknown08 = NONE;
		memset(player->key, 0, sizeof(player->key));
		player->unknown20 = 0;
		player->flag25 = false;
		player->flag24 = false;
	}
	s_simulation_world_actor *actor = world->actors;
	for (long j = 0; j < 16; j++, actor++)
	{
		actor->actor_index = NONE;
		actor->unknown04 = NONE;
		actor->unknown08 = 0;
		actor->unknown0c[0] = 0;
	}
	world->state = 0;
}

/* an iteration over the world's views: the views whose type bit is set in mask */
struct s_view_iterator
{
	dword mask;
	long index;
};

// @retail 0x69470
bool world_next_view(c_class_6a600 *world, s_view_iterator *iterator, c_simulation_view **out)
{
	bool result = false;

	while (iterator->index >= 0)
	{
		if (iterator->index >= 15)
			break;
		c_simulation_view *view = world->views[iterator->index++];
		if (view && (iterator->mask & (1 << view->type)))
		{
			*out = view;
			result = true;
			break;
		}
	}
	return result;
}


// @retail 0x67e10
bool function_67e10(c_class_6a600 *world)
{
	return world->state != 3 && world->state != 5;
}

// @retail 0x68250
bool function_68250(void)
{
	bool result = false;
	if (g_4cf770)
	{
		if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->index != NONE && !g_4cf772 && SIMULATION_WORLD->unknown18 == 4)
			result = true;
		else
			result = false;
	}
	return result;
}

// @retail 0x68290
bool function_68290(void)
{
	bool result = false;
	if (g_4cf770 && !g_4cf772)
	{
		c_class_6a600 *world = SIMULATION_WORLD;
		if (world->state)
			result = world->unknown18 != 4;
	}
	return result;
}

// @retail 0x690d0
void function_690d0(c_class_6a600 *world, long actor_index, const dword *state)
{
	if (world->state != 3 && world->state != 5)
	{
		s_simulation_world_actor *actor = &world->actors[actor_index];
		if (actor->actor_index != NONE)
		{
			actor->time = g_510c54->game_time;
			memcpy(actor->state, state, sizeof(actor->state));
		}
	}
}

// @retail 0x69580
void function_69580(c_class_6a600 *world, long player_index)
{
	long absolute_index = (word)player_index;
	s_simulation_world_player *player = &world->players[absolute_index];
	player->unknown20 = 0;
	player->player_index = NONE;
	player->unknown04 = NONE;
	player->unknown08 = NONE;

	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = 0x14;
	iterator.index = 0;
	while (world_next_view(world, &iterator, &view))
	{
		if (view->unknown3c != NONE && view->flag75)
			view->player_mask &= ~(1 << absolute_index);
	}
}

// @retail 0x69610
void function_69610(c_class_6a600 *world)
{
	for (short i = 0; i < 16; i++)
	{
		s_simulation_world_player *player = &world->players[i];
		if (player->player_index != NONE && player->flag25)
			player->flag25 = 0;
	}
}

// @retail 0x696d0
bool function_696d0(c_class_6a600 *world, long player_index)
{
	bool result = false;
	s_simulation_world_player *player = &world->players[player_index & 0xffff];
	if (player->player_index != NONE)
		result = player->flag25;
	return result;
}

// @retail 0x696f0
dword function_696f0(c_class_6a600 *world)
{
	dword mask = 0;
	for (long i = 0; i < 16; i++)
	{
		s_simulation_world_player *player = &world->players[i];
		if (player->player_index != NONE && player->flag25)
			mask |= 1 << i;
	}
	return mask;
}

// @retail 0x6a480
dword function_6a480(c_class_6a600 *world)
{
	dword mask = 0;
	for (long i = 0; i < 16; i++)
	{
		if (world->players[i].player_index != NONE)
			mask |= 1 << i;
	}

	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = 0x14;
	iterator.index = 0;
	while (world_next_view(world, &iterator, &view))
	{
		bool established = view->type == 2 ? view->state >= 5 : view->state >= 3;
		if (established)
			mask &= view->player_mask;
	}
	return mask;
}

// @retail 0x6a600
void c_class_6a600::function_6a600(void)
{
	for (long i = 0; i < 16; i++)
	{
		s_simulation_world_player *player = &players[i];
		if (player->player_index != NONE)
		{
			player->player_index = NONE;
			player->unknown04 = NONE;
			player->unknown08 = NONE;
			player->unknown20 = 0;
		}
	}
}

// @retail 0x6a6f0
void c_class_6a600::function_6a6f0(void)
{
	for (long i = 0; i < 16; i++)
	{
		s_simulation_world_actor *actor = &actors[i];
		if (actor->actor_index != NONE)
		{
			actor->actor_index = NONE;
			actor->unknown04 = NONE;
			actor->unknown08 = 0;
		}
	}
}

// @retail 0x6a860
void function_6a860(c_class_6a600 *world, long *size)
{
	for (short i = 0; i < 4; i++)
		g_46e320[i](0);
	world->flag11fc = 1;
	*size = 0x3fe000;
}

// @retail 0x6a990
bool world_buffer_append(c_class_6a600 *world, long size, const void *data, long offset)
{
	bool result = false;
	if (world_receiving_join_data(world) && world->buffer_size == offset)
	{
		byte *buffer = world->buffer;
		if (buffer && offset >= 0 && size > 0 && offset + size <= 0x40000)
		{
			memcpy(buffer + offset, data, size);
			world->buffer_size += size;
			result = true;
		}
	}
	return result;
}

/* the texture cache lends its memory (unknown_12c0d0.cpp) */
long function_12d400(long type, long size, long user_data, long update, long release);
void function_12bf00(void);
void function_199520(dword flags);
void function_199540(dword flags);

/* decompresses the join data into the game state (not decompiled yet:
   src/stubs/lane_d.cpp) */
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size);

// @retail 0x6a8a0
bool world_buffer_allocate(c_class_6a600 *world)
{
	bool result = false;
	byte *buffer;
	buffer = (byte *)function_12d400(1, 0x40000, 0, 0, 0);
	world->buffer = buffer;
	if (buffer != NULL)
	{
		world->buffer_size = 0;
		result = true;
	}
	return result;
}

// @retail 0x6ab10
void function_6ab10(c_class_6a600 *world)
{
	s_simulation_block *block = world->first_block;
	while (block)
	{
		s_simulation_block *next = block->next;
		long info;
		if (!g_4d87f8->allocator->get_info(block, &info))
			info = NONE;
		s_allocator_globals *globals = g_4d87f8;
		globals->allocator->release(block, NONE);
		globals->count--;
		block = next;
	}
	world->first_block = 0;
	world->last_block = 0;
	world->block_count = 0;
	world->unknown1210 = NONE;
	world->unknown120c = 0;
}

// @retail 0x6ab90
bool function_6ab90(c_class_6a600 *world, const s_simulation_block_data *data)
{
	s_allocator_globals *globals = g_4d87f8;
	s_simulation_block *block = (s_simulation_block *)globals->allocator->allocate(sizeof(s_simulation_block), 0, 0);
	if (!block)
	{
		globals->allocator->compact(0);
		block = (s_simulation_block *)globals->allocator->allocate(sizeof(s_simulation_block), 0, 0);
	}
	if (block)
		globals->count++;
	if (!block)
		return false;

	if (world->last_block)
		world->last_block->next = block;
	else
		world->first_block = block;
	block->next = 0;
	world->block_count++;
	world->last_block = block;
	block->data = *data;
	world->unknown1210 = data->size;
	return true;
}

// @retail 0x6ac30
void function_6ac30(c_class_6a600 *world, s_simulation_block_data *data)
{
	s_simulation_block *block = world->first_block;
	*data = block->data;
	world->first_block = block->next;
	if (world->last_block == block)
		world->last_block = 0;
	long info;
	g_4d87f8->allocator->get_info(block, &info);
	s_allocator_globals *globals = g_4d87f8;
	globals->allocator->release(block, NONE);
	globals->count--;
	world->block_count--;
	world->unknown120c++;
}

// @retail 0x6acb0
c_simulation_view *function_6acb0(c_class_6a600 *world)
{
	s_view_iterator iterator;
	c_simulation_view *view = 0;
	iterator.mask = 0xa;
	iterator.index = 0;
	world_next_view(world, &iterator, &view);
	return view;
}

// @retail 0x6ace0
c_simulation_view *function_6ace0(c_class_6a600 *world, long value)
{
	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = 0x14;
	iterator.index = 0;
	c_simulation_view *result = 0;
	while (world_next_view(world, &iterator, &view))
	{
		if (view->unknown1c == value)
		{
			result = view;
			break;
		}
	}
	return result;
}

// @retail 0x6ad40
c_simulation_view *function_6ad40(c_class_6a600 *world, const s_machine_address *address)
{
	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = 0x14;
	iterator.index = 0;
	while (world_next_view(world, &iterator, &view))
	{
		s_machine_address view_address = view->address;
		if (!memcmp(&view_address, address, sizeof(s_machine_address)))
			return view;
	}
	return 0;
}

// @retail 0x6adc0
c_simulation_view *function_6adc0(c_class_6a600 *world, long value)
{
	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = NONE;
	iterator.index = 0;
	c_simulation_view *result = 0;
	while (world_next_view(world, &iterator, &view))
	{
		if (view->unknown3c == value)
		{
			result = view;
			break;
		}
	}
	return result;
}

// @retail 0x686e0
bool simulation_machine_is_ready(const s_machine_address *address)
{
	bool result = false;
	if (g_4cf770)
	{
		c_class_6a600 *world = SIMULATION_WORLD;
		if (world->state)
		{
			s_machine_address local_address = world->local_address;
			if (!memcmp(address, &local_address, sizeof(s_machine_address)))
			{
				result = true;
			}
			else
			{
				c_simulation_view *view = function_6ad40(world, address);
				if (view)
					result = view->flag78;
			}
		}
	}
	else
	{
		result = true;
	}
	return result;
}

// @retail 0x6a2e0
void function_6a2e0(c_class_6a600 *world)
{
	dword established = function_6a480(world);
	for (long i = 0; i < 16; i++)
	{
		s_simulation_world_player *player = &world->players[i];
		if (player->player_index != NONE && !player->flag25)
		{
			s_simulation_owner_player *owner_player = &world->owner->players.players[i];
			dword key[3];
			key[0] = player->key[0];
			key[1] = player->key[1];
			key[2] = player->key[2];
			if ((world->owner->players.player_mask & (1 << i)) && !memcmp(key, owner_player->key, sizeof(key)) && !owner_player->flag0c && (established & (1 << i)) && !player->flag24)
				player->flag25 = true;
		}
	}
}

static inline long world_time_get(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

// @retail 0x6b2a0
void function_6b2a0(c_class_6a600 *world)
{
	world->unknown30 = 0;
	if (world->flag24)
	{
		world->flag25 = false;
	}
	else
	{
		long value = world->unknown28;
		if (world->state == 3)
		{
			function_6ab10(world);
			world->unknown1210 = value - 1;
			world->unknown120c = value;
		}
		world->flag24 = true;
	}
	switch (world->unknown18)
	{
	case 3:
		world->unknown18 = 4;
		break;
	case 4:
		world->time34 = world_time_get();
		world->unknown18 = 4;
		break;
	default:
		world->unknown18 = 4;
		break;
	}
}

/* the machine table (g_4e8c20): a mask of the machines present and their
   addresses */
struct s_machine_table
{
	byte unknown00[0x2c];
	dword machine_mask;
	s_machine_address machines[16];
};

/* a player (g_4e8c24, 0x21c bytes): its machine and its index on it */
struct s_player_datum
{
	byte unknown00[0x1a];
	short machine_index;
	short controller_index;
	byte unknown1e[0x21c - 0x1e];
};

struct s_match_450d14;
struct s_key_450d14;

// @retail 0x6a3b0
s_match_450d14 *__stdcall function_6a3b0(void *table, s_key_450d14 *key, long index)
{
	c_class_6a600 *world = (c_class_6a600 *)table;
	s_machine_table *machine_table = (s_machine_table *)g_4e8c20;
	dword machine_mask = machine_table->machine_mask;
	s_machine_address machines[16];
	memcpy(machines, machine_table->machines, sizeof(machines));

	long machine_index = NONE;
	for (long i = 0; i < 16; i++)
	{
		if ((machine_mask & (1 << i)) && !memcmp(key, &machines[i], sizeof(s_machine_address)))
		{
			machine_index = i;
			break;
		}
	}
	if (machine_index != NONE)
	{
		for (long j = 0; j < 16; j++)
		{
			if (world->players[j].player_index != NONE)
			{
				s_player_datum *datum = (s_player_datum *)g_4e8c24->data + (world->players[j].unknown04 & 0xffff);
				if (datum->machine_index == machine_index && datum->controller_index == index)
					return (s_match_450d14 *)&world->players[j];
			}
		}
	}
	return 0;
}

// @retail 0x6a7f0
void function_6a7f0(c_class_6a600 *world, s_key_450d14 *key, dword controller_mask, const s_simulation_player_state *states)
{
	for (long i = 0; i < 4; i++, states++)
	{
		if (controller_mask & (1 << i))
		{
			s_simulation_world_player *player = (s_simulation_world_player *)function_6a3b0(world, key, i);
			if (player && player->unknown08 == 3)
			{
				player->state = *states;
				player->state_time = g_510c54->game_time;
			}
		}
	}
}

// @retail 0x682c0
bool function_0682c0()
{
	return g_4cf770 && g_4cf772;
}

/* the simulation watcher (g_4cf780) as 0x68350 sees it */
struct s_simulation_watcher
{
	byte unknown00[0x1c];
	long unknown1c;
	byte unknown20[4];
	dword unknown24[0x18];
	bool unknown84;
};

struct s_simulation_watcher_state
{
	long unknown00;
	dword unknown04[0x18];
};

void simulation_watcher_update_machines(s_simulation_world_owner *watcher);

// @retail 0x68350
void function_68350(s_simulation_watcher_state *state, bool *valid)
{
	s_simulation_watcher *watcher = (s_simulation_watcher *)g_4cf780;
	*valid = false;
	if (watcher->unknown84)
	{
		state->unknown00 = watcher->unknown1c;
		memcpy(state->unknown04, watcher->unknown24, sizeof(state->unknown04));
		simulation_watcher_update_machines((s_simulation_world_owner *)watcher);
		*valid = true;
		watcher->unknown84 = false;
	}
}

/* frees a block (not decompiled yet: src/stubs/memory.cpp) */
void function_12d520(long a);

// @retail 0x6aae0
void world_buffer_dispose(c_class_6a600 *world)
{
	function_12d520((long)world->buffer);
	world->buffer = 0;
	world->buffer_size = NONE;
}

// @retail 0x6aa20
bool world_buffer_complete(c_class_6a600 *world, long size)
{
	long const *size_reference = &size;
	bool result = false;
	if (world_receiving_join_data(world) && world->buffer_size == *size_reference && world->buffer)
	{
		long decompressed_size;
		byte *destination = game_state_globals.base_address;
		function_199520(0);
		if (function_199740(world->buffer, *size_reference, destination, &decompressed_size) && decompressed_size == 0x3fe000)
			result = true;
		else
			function_12bf00();
		function_199540(0);
		function_12d520((long)world->buffer);
		world->buffer = 0;
		world->buffer_size = NONE;
	}
	return result;
}

static __forceinline void world_change_substate(c_class_6a600 *world, long substate)
{
	if (substate != 4)
	{
		world->flag25 = false;
		if (world->flag24)
		{
			world->flag24 = false;
			if (world->state == 3)
				function_6ab10(world);
		}
	}
	switch (world->unknown18)
	{
	case 3:
		if (substate != 4)
		{
			if (world_receiving_join_data(world))
				world_buffer_dispose(world);
			world->unknown30++;
		}
		break;
	case 4:
		world->time34 = world_time_get();
		break;
	}
	world->unknown18 = substate;
}

// @retail 0x6b160
void world_set_substate(c_class_6a600 *world, long substate)
{
	world_change_substate(world, substate);
}

// @retail 0x6b1f0
void world_enter_substate_3(c_class_6a600 *world, long value)
{
	world_change_substate(world, 3);
	world->unknown1c = world_time_get();
	world->unknown20 = value;
}

static inline bool world_substate_active(long substate)
{
	return substate >= 4 && substate <= 6 || substate == 3;
}

// @retail 0x6b310
void function_6b310(c_class_6a600 *world)
{
	long substate = world->unknown18;
	if (substate == 4)
		world_set_substate(world, 5);
	else if (world_substate_active(substate) && substate != 6)
		world_set_substate(world, 6);
}

// @retail 0x6b350
void function_6b350(c_class_6a600 *world)
{
	long substate = world->unknown18;
	if (world_substate_active(substate) && substate != 6)
		world_set_substate(world, 6);
}

// @retail 0x69c50
void function_69c50(c_class_6a600 *world)
{
	if (world->state == 1)
	{
		world_enter_substate_3(world, 0);
		function_6b2a0(world);
	}
	else
	{
		world_enter_substate_3(world, world->owner->unknown1c);
	}
}

/* the first view onto an authority (a type 1 or 3 view); retail inlines
   0x6acb0 into the world code */
static inline c_simulation_view *world_get_authority_view(c_class_6a600 *world)
{
	s_view_iterator iterator;
	c_simulation_view *view = 0;
	iterator.mask = 0xa;
	iterator.index = 0;
	world_next_view(world, &iterator, &view);
	return view;
}

static inline long world_time_since(long time)
{
	return world_time_get() - time;
}

byte g_4cf778;
byte g_4cf779;

/* not decompiled yet (src/stubs/lane_d.cpp) */
void __stdcall function_693a0(c_class_6a600 *world);

/* the authority's player keys message (type 0x26) */
struct s_simulation_player_keys_message
{
	dword player_mask;
	dword in_game_mask;
	dword keys[16][3];
};

// @retail 0x6a560
void function_6a560(c_class_6a600 *world, bool force)
{
	c_simulation_view *view = world_get_authority_view(world);
	if (view && view->established())
	{
		dword state[0x18];
		s_simulation_player_keys_message message;
		long unknown1c;
		if (simulation_watcher_get_players(world->owner, &unknown1c, &message.player_mask, &message.in_game_mask, state, message.keys, force) && view->channel_index != NONE)
			network_observer_send_message(view->observer, 3, view->channel_index, false, 0x26, sizeof(message), &message);
	}
}

// @retail 0x6b090
void simulation_world_view_established(c_class_6a600 *world, c_simulation_view *view, bool established)
{
	if (established && view == world_get_authority_view(world))
	{
		if (g_4cf778)
		{
			g_4cf778 = false;
			function_6a560(world, true);
			return;
		}
		g_4cf779 = true;
		function_6a560(world, true);
	}
}

// @retail 0x6b040
void function_6b040(c_class_6a600 *world)
{
	function_693a0(world);
	long substate = world->unknown18;
	if (world_substate_active(substate) && substate != 2)
	{
		world_set_substate(world, 2);
		world->unknown1c = world_time_get();
	}
}

// @retail 0x69f10
void function_69f10(c_class_6a600 *world)
{
	c_simulation_view *view = world_get_authority_view(world);
	if (view)
	{
		if (view->flag78)
		{
			function_6b2a0(world);
			return;
		}
		if (world_time_since(world->unknown1c) < g_network_configuration.valued0c)
			return;
	}
	function_6b040(world);
}

// @retail 0x6b110
void simulation_world_view_synchronized(c_class_6a600 *world, c_simulation_view *view, bool synchronized)
{
	if (synchronized && view == world_get_authority_view(world) && world->unknown18 == 3)
		function_69f10(world);
}

// @retail 0x69eb0
void function_69eb0(c_class_6a600 *world)
{
	c_simulation_view *view = world_get_authority_view(world);
	if (view && view->unknown3c != NONE && view->failure_reason == 0)
	{
		view->set_state(1, NONE);
		world_enter_substate_3(world, 0);
	}
}

// @retail 0x69f90
void function_69f90(c_class_6a600 *world)
{
	long elapsed = world_time_since(world->time34);
	if ((world->unknown30 >= g_network_configuration.valued18 || elapsed >= g_network_configuration.valued1c) && world->unknown18 != 1)
	{
		function_6b040(world);
		world_set_substate(world, 1);
	}
}

// @retail 0x69fe0
void function_69fe0(c_class_6a600 *world)
{
	c_simulation_view *view = world_get_authority_view(world);
	if (view && (view->established() || world->unknown18 == 3))
		return;
	function_6b040(world);
}

/* the game's main update (not decompiled yet: src/stubs/lane_d.cpp) */
void function_137fe0(void);

long g_4cf774;

// @retail 0x69790
long function_69790(c_class_6a600 *world)
{
	long result = 0x7fffffff;
	c_simulation_view *best = 0;
	c_simulation_view *stalled = 0;
	long minimum = 0x7fffffff;
	s_view_iterator iterator;

	{
		c_simulation_view *view;
		iterator.mask = 4;
		iterator.index = 0;
		while (world_next_view(world, &iterator, &view))
		{
			if (view->flag78 && view->unknown84 < minimum)
			{
				best = view;
				minimum = view->unknown84;
			}
		}
	}
	if (best)
	{
		long remaining = minimum - world->unknown28 + 0x81;
		if (remaining <= 0)
		{
			result = 0;
			stalled = best;
		}
		else
		{
			result = remaining;
		}
	}
	{
		c_simulation_view *view;
		iterator.mask = 4;
		iterator.index = 0;
		while (world_next_view(world, &iterator, &view))
			view->set_unknown88(view == stalled);
	}
	return result;
}

// @retail 0x69300
inline long function_69300(c_class_6a600 *world, bool *buffered)
{
	long result = 0;
	*buffered = false;
	if (world->flag24)
	{
		result = 0x7fffffff;
		switch (world->state)
		{
		case 1:
			break;
		case 2:
			result = function_69790(world);
			break;
		case 3:
			result = world->unknown1210 - world->unknown120c + 1;
			*buffered = true;
			break;
		case 4:
			break;
		case 5:
			break;
		default:
			__assume(0);
		}
	}
	return result;
}

// @retail 0x69350
void function_69350(c_class_6a600 *world, bool value)
{
	world->flag25 = value;
	if (value)
	{
		bool buffered;
		while (function_69300(world, &buffered) > 0)
			function_137fe0();
	}
}

// @retail 0x680c0
long function_680c0(bool *buffered)
{
	*buffered = false;
	long result = 0x7fffffff;
	if (g_4cf770 && SIMULATION_WORLD->state)
		result = function_69300(SIMULATION_WORLD, buffered);
	return result;
}

static inline void world_reset_to_substate_1(c_class_6a600 *world)
{
	if (world->unknown18 != 1)
	{
		function_6b040(world);
		world_set_substate(world, 1);
	}
}

// @retail 0x68750
void function_068750(void)
{
	if (SIMULATION_WORLD->state && !g_4cf772)
	{
		g_4cf772 = true;
		g_4cf774 = world_time_get();
		world_reset_to_substate_1(SIMULATION_WORLD);
	}
}

// @retail 0x6a2a0
void function_6a2a0(c_class_6a600 *world, c_simulation_view *view)
{
	if (view->state != 2)
	{
		if (view->state == 1 && view->remote_state == 1)
			view->set_state(2, world->unknown38++);
		else if (view->state != 1)
			view->set_state(1, NONE);
	}
}

// @retail 0x69640
bool simulation_world_player_valid(long player_index, c_class_6a600 *world, const t_player_key *key)
{
	long index = player_index & 0xffff;
	bool result = false;
	if (index >= 0 && index < NUMBEROF(world->players))
	{
		s_simulation_world_player *player = &world->players[index];
		if (player->player_index != NONE)
		{
			t_player_key player_key;
			memcpy(player_key, player->key, sizeof(player_key));
			if (!memcmp(key, player_key, sizeof(player_key)) && simulation_watcher_player_valid(index, world->owner, key))
				result = true;
		}
	}
	return result;
}

/* the establishment message (type 0x25; unknown_085540.cpp) */
struct s_simulation_view_establishment
{
	long state;
	long id;
};

/* c_simulation_view::set_state (unknown_085540.cpp), which retail inlines
   here */
static inline void view_set_state(c_simulation_view *view, long new_state, long id)
{
	bool valid;

	if (new_state < 2)
		valid = id == NONE;
	else if (new_state == 2)
		valid = id >= 0;
	else
		valid = id == view->state_id && new_state == view->state + 1;

	if (!valid)
	{
		if (view->failure_reason == 0)
		{
			view->set_state(0, NONE);
			view->failure_reason = 7;
		}
	}
	else if (view->state != new_state || view->state_id != id)
	{
		s_simulation_view_establishment message;
		memset(&message, 0, sizeof(message));
		view->state_id = id;
		message.id = id;
		view->state = new_state;
		message.state = new_state;
		if (view->channel_index != NONE)
			network_observer_send_message(view->observer, 3, view->channel_index, false, 0x25, sizeof(message), &message);
		view->update_established();
	}
}

/* the established views go back to state 2 */
// @retail 0x69dd0
void function_69dd0(c_class_6a600 *world)
{
	s_view_iterator iterator;
	c_simulation_view *view;
	iterator.mask = NONE;
	iterator.index = 0;
	while (world_next_view(world, &iterator, &view))
	{
		if (view->failure_reason == 0 && view->state > 2)
			view_set_state(view, 2, view->state_id);
	}
}

// @retail 0x698e0
bool simulation_world_queue_block(c_class_6a600 *world, const s_simulation_block_data *data)
{
	bool result = false;
	long expected = world->unknown1210 + 1;

	if (!world_receiving_join_data(world))
	{
		if (data->size < expected)
			return result;
		if (data->size == expected && (world->unknown18 == 4 || world->flag25))
		{
			if (function_6ab90(world, data))
			{
				while (world->flag25 && g_4e6948 && g_4e6948->flag1120 && !(g_4cf770 && g_4cf772) && !world->flag2c)
				{
					bool buffered;
					if (function_69300(world, &buffered) <= 0)
						break;
					function_137fe0();
				}
				result = true;
			}
			else
			{
				g_4cf771 = true;
			}
			return result;
		}
	}
	world->flag2c = true;
	return result;
}
