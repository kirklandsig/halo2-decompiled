// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_067E10.CPP: the simulation world (g_4cf77c) and the simulation
   globals' queries (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_123b30.h"
#include "unknown_067e10.h"
#include "unknown_08b110.h"
#include "unknown_0662e0.h"
#include "unknown_075870.h"
#include "unknown_059ad0.h"

struct s_68a90_entry
{
	byte unknown00[8];
	word values[4];
};

struct s_connection_quality_members
{
	long current;
	long unknown04;
	long count;
};

// @retail 0x68a90
bool function_68a90(s_68a90_entry *entry, long *quality)
{
	bool result = false;
	long minimum = g_network_configuration.valuecdc;
	if (g_4cf770)
	{
		c_class_58d20 *session = ((s_simulation_world_owner *)g_4cf780)->session;
		if (session && session->value18 == 2)
		{
			s_connection_quality_members *members = 0;
			if (session->state && session->value4c != NONE)
				members = (s_connection_quality_members *)&session->value4c;
			if (members && members->count > 1)
				result = true;
		}
	}
	for (dword i = 0; i < 4; i++)
	{
		real value;
		switch (i)
		{
		case 0: value = (real)entry->values[0]; break;
		case 1: value = (real)entry->values[1] * 0.1f; break;
		case 2: value = (real)entry->values[2] * 0.1f * 1024.0f; break;
		case 3: value = (real)entry->values[3]; break;
		default: __assume(0);
		}
		long best = g_network_configuration.quality_ranges[i][0];
		long worst = g_network_configuration.quality_ranges[i][1];
		real fraction;
		if (best > worst)
		{
			if (value >= (real)best) fraction = 1.0f;
			else if (value <= (real)worst) fraction = 0.0f;
			else fraction = (value - (real)worst) / (real)(best - worst);
		}
		else
		{
			if (value <= (real)best) fraction = 1.0f;
			else if (value >= (real)worst) fraction = 0.0f;
			else fraction = ((real)worst - value) / (real)(worst - best);
		}
		real scaled = (real)(g_network_configuration.valuecdc - 1) * fraction;
		long rounded;
		__asm
		{
			fld scaled
			fistp rounded
		}
		long level = rounded + 1;
		if (minimum > level)
			minimum = level;
	}
	*quality = minimum;
	return result;
}

#define SIMULATION_WORLD ((c_class_6a600 *)g_4cf77c)
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))

long function_83db0(s_simulation_world_owner *watcher);

// @retail 0x687e0
long function_687e0(void)
{
	long result = 0;
	if (g_4cf770)
		result = function_83db0((s_simulation_world_owner *)g_4cf780);
	return result;
}

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);

struct s_simulation_controller;
void simulation_controller_initialize(s_simulation_controller *controller, c_class_6a600 *world,
	long field_00, long field_04, long field_08, const s_machine_address *machine, const t_player_key *key);
void function_155710(long index);
bool function_78a10(long index, s_network_observer *observer, long *delay, real *rate, long *received_rate, long *loss_percent);

// @retail 0x68800
bool function_68800(c_simulation_view *view, long *delay, long *rate, long *received_rate, long *loss_percent)
{
	s_network_observer *observer = *(s_network_observer **)((byte *)g_4cf780 + 8);
	bool result = false;
	long measured_delay;
	real measured_rate;
	long measured_received;
	long measured_loss;
	if (view && observer && function_78a10(view->channel_index, observer,
		&measured_delay, &measured_rate, &measured_received, &measured_loss))
	{
		*delay = measured_delay;
		real scaled = measured_rate * 10.0f;
		long rounded;
		__asm { fld scaled }
		__asm { fistp rounded }
		*rate = rounded;
		*received_rate = measured_received * 10 / 1024;
		scaled = (real)measured_loss;
		__asm { fld scaled }
		__asm { fistp rounded }
		*loss_percent = rounded;
		result = true;
	}
	return result;
}

struct s_world_player_input
{
	long unknown00;
	t_player_key key;
	byte unknown10[4];
	s_machine_address machine;
	byte unknown1a[0x28 - 0x1a];
	short local_index;
	byte unknown2a[0x21c - 0x2a];
};

// @retail 0x694c0
void function_694c0(c_class_6a600 *world, long player_index)
{
	long const *player_reference = &player_index;
	long index = (word)*player_reference;
	s_world_player_input *player = &((s_world_player_input *)g_4e8c24->data)[index];
	short local_index = player->local_index;
	bool local = local_index != NONE;
	long kind;
	switch (g_4e6948->mode)
	{
	case 1: kind = 0; break;
	case 2: kind = local ? 0 : 3; break;
	case 3: kind = local ? 0 : 3; break;
	case 4: kind = local ? 1 : 5; break;
	case 5: kind = local ? 0 : 4; break;
	default: __assume(0);
	}
	if (local)
		function_155710(local_index);
	simulation_controller_initialize((s_simulation_controller *)&world->players[index], world,
		index, player_index, kind, &player->machine, &player->key);
}

// @retail 0x6a690
long function_6a690(long index, c_class_6a600 *world, long value)
{
	if (index == NONE)
	{
		long i = 0;
		do
		{
			if (world->actors[i].actor_index == NONE)
			{
				index = i;
				break;
			}
			i++;
		} while (i < 16);
	}
	if (index != NONE)
	{
		s_simulation_world_actor *actor = &world->actors[index];
		actor->unknown08 = (long)world;
		actor->actor_index = index;
		actor->unknown04 = value;
		actor->time = NONE;
		function_c6ef0((s_unit_state_c6ef0 *)actor->state);
	}
	return index;
}

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
	long absolute_index = player_index & 0xffff;
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

static __forceinline void function_6ab11(s_allocator_globals *arg_0, s_simulation_block *arg_1)
{
 arg_0->allocator->release(arg_1, NONE);
 arg_0->count--;
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
		function_6ab11(g_4d87f8, block);
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
void function_68350(bool *valid, s_simulation_watcher_state *state)
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

/* Reset the concrete member vtables before releasing the view storage. */
inline c_vtable_450c94::~c_vtable_450c94() {}
inline c_vtable_450d1c::~c_vtable_450d1c() {}
inline c_handle_table_450cd0::~c_handle_table_450cd0() {}
inline c_vtable_450cf4::~c_vtable_450cf4() {}

struct s_flagged;
extern s_flagged *g_4d87ec;
extern s_flagged *g_4d87f0;
void function_85880(c_simulation_view *view);

// @retail 0x693a0
void __stdcall function_693a0(c_class_6a600 *world)
{
 c_simulation_view *view;
 s_view_iterator iterator;
 iterator.mask = NONE;
 iterator.index = 0;
 while (world_next_view(world, &iterator, &view))
 {
  if (view)
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
 }
}

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
			
		}
		else
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

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x680c0
long function_680c0(bool *buffered)
{
	*buffered = false;
	_ReadWriteBarrier();
	long result = 0x7fffffff;
	if (g_4cf770 && SIMULATION_WORLD->state)
		result = function_69300(SIMULATION_WORLD, buffered);
	return result;
}
#pragma function(_ReadWriteBarrier)

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


// @retail 0x6a770
void __stdcall function_6a770(void *pointer)
{
	c_class_6a600 *world = (c_class_6a600 *)pointer;
	world->function_6a600();
	s_record_pool_iterator iterator;
	iterator.data = g_4e8c24;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	byte *player;
	while ((player = data_iterator_next_inlined(&iterator)) != 0)
	{
		if (!(player[2] & 2))
			function_694c0(world, iterator.datum_index);
	}
}

class c_replication_view_storage;
long function_821c0(void **arg_f0f1ad, c_replication_view_storage **out_storage);

// The caller at 0x84630 supplies four stack arguments; the last is unused here.
// Retail body matches with the standard convention. without the marker,
// LTCG changes stack cleanup and loop registers (ten differences).
// No absolute reference to this address exists in retail. caller 0x84630
// pushes all four arguments; taking unused's address did not keep ret 16.
// @retail 0x68580 standard
void __stdcall function_68580(short type, const s_machine_address *address, long value, long unused)
{
	(void)&unused;
	c_replication_view_storage *storage;
	c_simulation_view *view;
	long index = function_821c0((void **)&view, &storage);
	if (index != NONE)
	{
		view->initialize(index, type, (s_simulation_view_data *)storage, address, value);
		c_class_6a600 *world = SIMULATION_WORLD;
		long slot = 0;
		c_simulation_view **local_0 = world->views;
		do
		{
			if (*local_0 == 0)
			{
				world->views[slot] = view;
				world->view_count++;
				break;
			}
			slot++;
			local_0++;
		} while (slot < 15);
		view->world = world;
		view->world_index = slot;
	}
}

long simulation_watcher_find_machine(const s_simulation_world_owner *watcher, const s_machine_address *address);
long samples_trimmed_mean(const long *samples, long count);

// @retail 0x688c0
bool function_688c0(long player_index, s_68a90_entry *entry)
{
	bool changed = false;
	if (g_4cf770)
	{
		byte *player = (byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c;
		s_68a90_entry previous = *entry;
		memset(entry, 0, sizeof(*entry));
		if (!(player[2] & 2))
		{
			const s_machine_address *address = (const s_machine_address *)((byte *)g_4e8c20 + 0x30 + *(short *)(player + 0x1a) * 6);
			long machine = simulation_watcher_find_machine((s_simulation_world_owner *)g_4cf780, address);
			if (machine != NONE)
			{
				c_class_6a600 *world = (c_class_6a600 *)g_4cf77c;
				if (world->state != 3 && world->state != 5)
				{
				if (machine == *(long *)(world->unknown13 + 1))
				{
					long count = 0;
					long delay[16], rate[16], received[16], loss[16];
					s_view_iterator iterator = {0xffffffff, 0};
					c_simulation_view *view;
					while (world_next_view((c_class_6a600 *)g_4cf77c, &iterator, &view))
					{
						if (function_68800(view, &delay[count], &rate[count], &received[count], &loss[count]))
							count++;
					}
					if (count > 0)
					{
						entry->values[0] = (word)samples_trimmed_mean(delay, count);
						entry->values[1] = (word)samples_trimmed_mean(rate, count);
						entry->values[2] = (word)samples_trimmed_mean(received, count);
						entry->values[3] = (word)samples_trimmed_mean(loss, count);
					}
				}
				else
				{
					c_simulation_view *view = function_6ace0(world, machine);
					long delay, rate, received, loss;
					if (function_68800(view, &delay, &rate, &received, &loss))
					{
						entry->values[0] = (word)delay;
						entry->values[1] = (word)rate;
						entry->values[2] = (word)received;
						entry->values[3] = (word)loss;
					}
				}
				}
			}
		}
		changed = memcmp(&previous, entry, sizeof(previous)) != 0;
	}
	return changed;
}

void function_85880(c_simulation_view *view);

// @retail 0x68550
void __stdcall function_68550(c_simulation_view *view)
{
 if (!view->failure_reason)
 {
  view->set_state(0, NONE);
  view->failure_reason = 1;
 }
 function_85880(view);
}

#include "bitstream.h"
#include "network_message_types.h"
void function_87d00(s_bitstream *stream, void *message);
bool function_87e90(s_bitstream *stream, void *message);
void function_847d0(s_simulation_controller *controller, s_player_action *input);

// @retail 0x685f0
bool function_685f0(void *block, long *size, byte *destination, long capacity)
{
 bool result = true;
 s_bitstream stream;
 stream.data = destination;
 stream.size_in_bytes = capacity;
 memset(destination, 0, capacity);
 stream.bit_position = 0;
 stream.checkpoint_count = 0;
 stream.error = false;
 stream.unknown2c = 0;
 stream.unknown30 = 0;
 stream.unknown08 = 1;
 stream.mode = 1;
 function_87d00(&stream, block);
 *size = (stream.bit_position + 7) / 8;
 return result;
}

// @retail 0x68670
bool function_68670(byte *source, long size, void *block)
{
 s_bitstream stream;
 stream.data = source;
 stream.size_in_bytes = size;
 bool result = false;
 memset(block, 0, 0x4048);
 stream.unknown08 = 1;
 stream.mode = 3;
 stream.bit_position = 0;
 stream.checkpoint_count = 0;
 stream.error = false;
 if (function_1959c0(&stream, 32) == 0x64656267)
  stream.error = true;
 else
 {
  stream.bit_position = 0;
  stream.error = false;
 }
 if (function_87e90(&stream, block)) result = true;
 return result;
}

// @retail 0x69040
void function_69040(c_class_6a600 *world, s_player_action *actions, dword mask)
{
 if (world->state != 3)
 {
  for (long i = 0; i < 4; i++, actions++)
  {
   if (mask & (1 << i))
   {
    s_simulation_controller *controller = (s_simulation_controller *)function_6a3b0(world,
     (s_key_450d14 *)&world->local_address, i);
    if (controller) function_847d0(controller, actions);
   }
  }
 }
 else
 {
  c_simulation_view *view = 0;
  s_view_iterator iterator = { 0xa, 0 };
  world_next_view(world, &iterator, &view);
  view->send_player_update(mask, (const s_simulation_player_state *)actions);
 }
}


bool function_862e0(c_simulation_view *view);

static __forceinline bool view_needs_join_buffer(short type)
{
 bool result;
 switch (type)
 {
 case 2: result = true; break;
 case 4: result = false; break;
 default: __assume(0);
 }
 return result;
}

// @retail 0x6a040
long function_6a040(c_class_6a600 *world, c_simulation_view *view)
{
 long result = 0;
 if (view->state < 3)
 {
  bool ready = view_needs_join_buffer(view->type);
  if (!ready)
  {
   dword current = view->player_mask;
   dword players = function_696f0(world);
   ready = (current & players) == players;
  }
  if (ready) view->set_state(3, view->state_id);
 }
 if (view->state == 3)
 {
  if (!view_needs_join_buffer(view->type))
  {
   view->data->unknown39 = true;
   view->set_state(4, view->state_id);
  }
  else if (g_510c54->game_time == 0)
   view->set_state(4, view->state_id);
  else
  {
   bool available = true;
   long count = 0;
   s_view_iterator iterator = {4, 0};
   c_simulation_view *other;
   while (world_next_view(world, &iterator, &other))
    if (other->buffer) count++;
   if (count >= g_network_configuration.valued08) available = false;
   long time = *(long *)world->unknown1208;
   if ((time == NONE || function_75890(time) >= g_network_configuration.valued14) && available)
   {
    if (function_862e0(view))
     view->set_state(4, view->state_id);
    else
    {
     long attempts = view->unknown90;
     *(long *)world->unknown1208 = function_75870();
     if (attempts >= g_network_configuration.valued10) result = 3;
    }
   }
  }
 }
 if (view->state == 4)
 {
  if (!view->function_85cb0()) return 0;
  dword current = view->player_mask;
  dword players = function_696f0(world);
  bool ready = (current & players) == players;
  if (!ready) return 0;
  result = 1;
 }
 if (result == 1)
 {
  if (view->state == 4 && world->unknown18 == 4)
  {
   if (!view_needs_join_buffer(view->type))
    view->has_pending_entity();
   else if (view->buffer)
    view->release_buffer();
   view->set_state(5, view->state_id);
  }
 }
 else if (result == 3 && view->failure_reason == 0)
 {
  if (view->state != 0 || view->state_id != NONE)
  {
   struct { long state; long id; } message;
   memset(&message, 0, sizeof(message));
   view->state = 0;
   view->state_id = NONE;
   message.state = 0;
   message.id = NONE;
   if (view->channel_index != NONE)
    network_observer_send_message(view->observer, 3, view->channel_index, false, 0x25, sizeof(message), &message);
   view->update_established();
  }
  view->failure_reason = 3;
 }
 return result;
}


// @retail 0x69d50
void function_69d50(c_class_6a600 *world)
{
 if (world->state != 1)
 {
  s_view_iterator iterator = {0x14, 0};
  c_simulation_view *view;
  while (world_next_view(world, &iterator, &view))
  {
   if (!view->failure_reason && !view->flag78 && view->state != 5)
   {
    if (view->established()) function_6a040(world, view);
    else function_6a2a0(world, view);
   }
  }
 }
}

// @retail 0x69c80
void function_69c80(c_class_6a600 *world)
{
 dword machines = world->owner->unknown1c;
 dword attempted = 0;
 dword completed = 0;
 long elapsed = world_time_since(world->unknown1c);
 for (long i = 0; i < 16; i++)
 {
  dword bit = 1 << i;
  if ((machines & bit) && i != *(long *)((byte *)world + 0x14))
  {
   c_simulation_view *view = function_6ace0(world, i);
   attempted |= bit;
   if (view && !view->failure_reason)
   {
    if (view->established())
    {
     if ((world->unknown20 & bit) && function_6a040(world, view)) completed |= bit;
    }
    else function_6a2a0(world, view);
   }
  }
 }
 if (!(attempted & (world->unknown20 & ~completed)) || elapsed >= g_network_configuration.valued0c)
  function_6b2a0(world);
}


void function_860b0(c_simulation_view *view, void *block);

// @retail 0x69880
void function_69880(c_class_6a600 *world, void *block)
{
 // Keep the retail stack argument under whole-program optimization.
 void *const *block_reference = &block;
 s_view_iterator iterator = {4, 0};
 c_simulation_view *view;
 while (world_next_view(world, &iterator, &view))
  function_860b0(view, block);
}

// @retail 0x68f30
void function_68f30(c_class_6a600 *world)
{
 if (world->state != 3 && world->state != 5)
 {
  long state = world->unknown18;
  if (!(state >= 4 && state <= 6) && state != 3 && state != 1)
   function_69c50(world);
  if (world->unknown18 == 3) function_69c80(world);
  if (world->unknown18 == 4) function_69d50(world);
  if (world->unknown18 == 5) function_69dd0(world);
  function_6a2e0(world);
 }
 else
 {
  long state = world->unknown18;
  if (!(state >= 4 && state <= 6) && state != 3 && state != 1)
   function_69eb0(world);
  if (world->unknown18 == 3) function_69f10(world);
  if (world->unknown18 != 4 && world->unknown18 != 1) function_69f90(world);
  state = world->unknown18;
  if ((state >= 4 && state <= 6) || state == 3) function_69fe0(world);
  function_6a560(world, false);
 }
 s_view_iterator iterator = {0xffffffff, 0};
 c_simulation_view *view;
 while (world_next_view(world, &iterator, &view)) view->update_baseline();
}


void function_69a00(const byte *input, c_class_6a600 *world, dword mask);
void function_69b50(const byte *input, c_class_6a600 *world, dword mask);

// @retail 0x684e0
void function_684e0(byte *block)
{
 c_class_6a600 *world = SIMULATION_WORLD;
 if (world->state != 3 && world->state != 5)
 {
  if (world->state == 2) function_69880(world, block);
  else if (world->state == 4)
  {
   function_69a00(block + 0xc, world, *(dword *)(block + 8));
   function_69b50(block + 0x610, world, *(dword *)(block + 0x5cc));
  }
 }
 SIMULATION_WORLD->unknown28 = *(long *)block + 1;
}

struct c_entry_table
{
 void function_08a030();
};

struct s_world_disposal_state
{
 void *vtable;
 bool initialized;
 byte unknown05[3];
 long unknown08;
 byte *owner;
 void *definitions;
};

// @retail 0x68e20
void __stdcall function_68e20(c_class_6a600 *world)
{
 function_6b040(world);
 while (world->view_count > 0)
 {
  c_simulation_view *view = world->views[world->view_count - 1];
  if (view)
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
 }
 world->function_6a600();
 world->function_6a6f0();
 if (world->state == 4 || world->state == 5)
 {
  s_world_disposal_state *messages = (s_world_disposal_state *)((byte *)world->distribution + 0xa0ac);
  *(long *)(messages->owner + 4) = 0;
  messages->unknown08 = 0;
  messages->owner = 0;
  messages->definitions = 0;
  messages->initialized = false;
  s_world_disposal_state *entities = (s_world_disposal_state *)((byte *)world->distribution + 0x2098);
  ((c_entry_table *)entities)->function_08a030();
  *(long *)entities->owner = 0;
  entities->initialized = false;
  entities->unknown08 = 0;
  entities->owner = 0;
  entities->definitions = 0;
 }
 world->owner = 0;
 world->distribution = 0;
 world->state = 0;
}

extern bool g_4ed39c;
extern long g_4cf784;
void function_195e90(void);
void function_68c00(s_simulation_world_owner *owner, void *definitions,
 s_simulation_distribution *distribution, c_class_6a600 *world);

// @retail 0x67f60
void function_67f60(void)
{
 if (!g_4ed39c)
 {
  s_simulation_world_owner *watcher = (s_simulation_world_owner *)g_4cf780;
  byte **link = (byte **)watcher->unknown08;
  if (*link)
  {
   *(long *)(*link + 0x80) = 0;
   *link = 0;
  }
  watcher->session = 0;
  watcher->world = 0;
  function_68e20((c_class_6a600 *)g_4cf77c);
  function_195e90();
 }
}

// @retail 0x6ae20
bool function_6ae20(c_class_6a600 *world)
{
 long mode = world->state == 2 ? 2 : 4;
 s_simulation_world_owner *owner = world->owner;
 s_simulation_distribution *distribution = world->distribution;
 long local = *(long *)((byte *)world + 0x14);
 s_machine_address address = world->local_address;
 byte preserve = world->unknown2f;
 function_68e20(world);
 if (g_510ca0) g_510ca0 = false;
 g_4e6948->mode = mode;
 g_4cf778 = false;
 function_68c00(owner, (void *)g_4cf784, distribution, world);
 world->local_address = address;
 world->unknown0c = true;
 *(long *)((byte *)world + 0x14) = local;
 function_6a770(world);
 if (preserve) world->unknown2f = true;
 return true;
}

void function_89a20(s_handle_peers *peers);
void function_d5560(bool skip_existing);
void function_d5640(void);
void function_185a30(void);
void function_185630(void);
void __stdcall function_162060(void *engine);
void function_196470(void);
void function_196780(void);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x6aee0
bool function_6aee0(c_class_6a600 *world)
{
 bool result = false;
 switch (world->state)
 {
 case 3:
  if (!world_receiving_join_data(world))
 {
  s_simulation_world_owner *owner = world->owner;
  s_simulation_distribution *distribution = world->distribution;
  long local = *(long *)((byte *)world + 0x14);
  s_machine_address address = world->local_address;
  byte preserve = world->unknown2f;
  function_68e20(world);
  *(byte *)&g_4e6948->mode = 3;
  _ReadWriteBarrier();
  function_68c00(owner, (void *)g_4cf784, distribution, world);
  world->local_address = address;
  world->unknown0c = true;
  *(long *)((byte *)world + 0x14) = local;
  function_6a770(world);
  if (preserve) world->unknown2f = true;
  g_4cf779 = false;
  world_enter_substate_3(world, 0);
  function_6b2a0(world);
  result = true;
 }
  break;
 case 5:
 {
  function_6b040(world);
  world->state = 4;
  g_4e6948->mode = 5;
  function_89a20((s_handle_peers *)world->distribution);
  function_6a770(world);
  function_d5560(true);
  function_d5640();
  function_185a30();
  function_185630();
  void *engine = g_55e4d0[g_4e9ae8->engine_index];
  if (engine) function_162060(engine);
  if (g_55e4d0[g_4e9ae8->engine_index])
  {
   g_510ca0 = true;
   g_510ca1 = false;
   function_196470();
   function_196780();
  }
  g_4cf779 = false;
  result = true;
 }
  break;
 default: __assume(0);
 }
 return result;
}
#pragma function(_ReadWriteBarrier)

byte g_4cf77a;
void simulation_world_reset_replication(c_class_6a600 *world);
void function_18e700(void);

// @retail 0x687b0
void function_687b0(void)
{
 c_class_6a600 *world = (c_class_6a600 *)g_4cf77c;
 g_4cf77a = true;
 simulation_world_reset_replication(world);
 function_18e700();
 g_4cf779 = false;
 g_4cf77a = false;
}

bool function_83220(s_simulation_world_owner *watcher);
bool function_138860(void);
bool function_138a10(void);
bool function_138840(void);
void __stdcall function_18f1c0(long value);

// @retail 0x680f0
void function_680f0(void)
{
 if (!g_4cf770 || g_4cf772 || !g_4e6948 || !g_4e6948->flag1120) goto done;
 if (g_4cf771)
 {
  function_068750();
  if (g_4cf772) goto done;
 }
 if (g_4cf779)
 {
  function_687b0();
  if (g_4cf772) goto done;
 }
 if (!function_83220((s_simulation_world_owner *)g_4cf780)) function_068750();
 if (g_4cf772) goto done;
 function_68f30((c_class_6a600 *)g_4cf77c);
 if (g_4cf772) goto done;
 {
  c_class_6a600 *world = (c_class_6a600 *)g_4cf77c;
  if ((world->state == 3 || world->state == 5) && world->flag2c)
  {
   function_068750();
   if (g_4cf772) goto done;
  }
 }
 {
  bool (*query_mode)(void) = function_138860;
  bool (*query_pending)(void) = function_138a10;
  bool (*query_network)(void) = function_138840;
  if (query_mode() && query_pending() && !query_network()) function_18f1c0(6);
 }
done:
 return;
}


struct s_a0c4_object;
extern s_a0c4_object *g_4d87e8;
void simulation_watcher_reset(s_simulation_world_owner *watcher);
void function_83000(s_simulation_world_owner *watcher);
void function_195a40(void);

// @retail 0x67ee0
void function_67ee0(void)
{
 if (!g_4ed39c)
 {
  g_4cf771 = false;
  g_4cf772 = false;
  long mode = g_4e6948->mode;
  s_simulation_distribution *distribution = 0;
  if (mode >= 4 && mode <= 5)
   distribution = (s_simulation_distribution *)g_4d87e8;
  function_68c00((s_simulation_world_owner *)g_4cf780, (void *)g_4cf784,
   distribution, (c_class_6a600 *)g_4cf77c);
  s_simulation_world_owner *watcher = (s_simulation_world_owner *)g_4cf780;
  watcher->world = (c_class_6a600 *)g_4cf77c;
  watcher->session = 0;
  *(long *)watcher->unknown08 = 0;
  simulation_watcher_reset(watcher);
  function_83000((s_simulation_world_owner *)g_4cf780);
  function_195a40();
  g_4cf778 = true;
 }
}


struct s_player_object_motion
{
 long object_index;
 point3f position;
 vector3f forward;
 vector3f up;
 vector3f linear;
 vector3f angular;
};
struct s_z_transform_state;
bool function_ab9f0(const s_z_transform_state *state);
void function_82980(s_player_object_motion *motion, long player_index);
bool __stdcall function_84990(s_simulation_controller *controller, s_player_action *output);
bool __stdcall function_8b660(s_simulation_world_actor *actor, long *index, s_unit_state_c6ef0 *state);
c_simulation_view *function_6ad40(c_class_6a600 *world, const s_machine_address *address);

struct s_simulation_input_69110
{
 dword unknown00[2];
 dword player_mask;
 s_player_action field_c_10[16];
 dword actor_mask;
 long actor_indices[16];
 dword actor_states[16][0x1f];
};

// @retail 0x69110
void __stdcall function_69110(c_class_6a600 *world, s_simulation_input_69110 *input)
{
 input->player_mask = 0;
 input->actor_mask = 0;
 for (long i = 0; i < 16; i++)
 {
  s_simulation_world_player *player = &world->players[i];
  if (player->player_index != NONE &&
   function_84990((s_simulation_controller *)player, &input->field_c_10[i]))
   input->player_mask |= 1 << i;
 }
 for (long j = 0; j < 16; j++)
 {
  s_simulation_world_player *player = &world->players[j];
  if (player->player_index != NONE && player->unknown08 == 4)
  {
   c_simulation_view *view = function_6ad40((c_class_6a600 *)player->unknown20,
    (const s_machine_address *)player->unknown18);
   if (view)
   {
    c_vtable_450c94 *source = (c_vtable_450c94 *)((byte *)view->data + 0x5098);
    if (source)
    {
     dword bit = 1 << player->player_index;
     if (source->mask71c & bit)
     {
      s_player_object_motion motion;
      memcpy(&motion, &source->data720[player->player_index], sizeof(motion));
      source->mask71c &= ~bit;
      bool (*validate)(const s_z_transform_state *) = function_ab9f0;
      if (validate((const s_z_transform_state *)&motion))
       function_82980(&motion, player->unknown04);
     }
    }
   }
  }
 }
 for (long k = 0; k < 16; k++)
 {
  s_simulation_world_actor *actor = &world->actors[k];
  if (actor->actor_index != NONE &&
   function_8b660(actor, &input->actor_indices[k], (s_unit_state_c6ef0 *)input->actor_states[k]))
   input->actor_mask |= 1 << k;
 }
}

void __stdcall function_83610(s_simulation_world_owner *watcher, long *count, s_simulation_player_update *updates);

// @retail 0x69260
void function_69260(c_class_6a600 *world, s_simulation_block_data *block)
{
	byte *data = (byte *)block;
	if (world->state != 3)
	{
		block->size = world->unknown28;
		*(long *)(data + 0x4040) = g_510c54->game_time;
		bool *valid = (bool *)(data + 0xdd0);
		s_simulation_watcher_state *state = (s_simulation_watcher_state *)(data + 0xdd4);
		*(dword *)(data + 0x4044) = g_4e7408->unknown0;
		function_68350(valid, state);
		function_83610((s_simulation_world_owner *)g_4cf780, (long *)(data + 0xe38), (s_simulation_player_update *)(data + 0xe3c));
		bool active = function_68250();
		*(bool *)(data + 4) = active;
		if (active)
		{
			function_69110(world, (s_simulation_input_69110 *)block);
			if (world->state != 3 && world->state != 5 && world->flag2e)
			{
				*(bool *)(data + 0x403c) = true;
				world->flag2e = false;
			}
		}
	}
	else
		function_6ac30(world, block);
}

// @retail 0x682e0
void function_682e0(s_simulation_block_data *block)
{
	memset(block, 0, sizeof(*block));
	function_69260(SIMULATION_WORLD, block);
	c_class_6a600 *world = SIMULATION_WORLD;
	if ((world->state == 3 || world->state == 5) &&
		!((world->state == 3 || world->state == 5) && world->flag2c))
	{
		byte *data = (byte *)block;
		if (block->size != world->unknown28 || *(long *)(data + 0x4040) != g_510c54->game_time ||
			*(dword *)(data + 0x4044) != g_4e7408->unknown0)
			world->flag2c = true;
	}
}

struct s_object;
struct s_simulation_player_identity;
s_object *function_badc0(long object_index, dword type_mask);
void function_c6de0(long object_index, void *control);
void function_14c630(dword valid_mask, const s_machine_address *addresses);
bool function_84f30(const s_simulation_player_identity *identity);
bool function_84fb0(const s_simulation_player_update *update);
bool simulation_player_remove_if_left(const s_simulation_player_identity *identity);
bool __stdcall function_85140(const s_simulation_player_update *update);
bool __stdcall function_854c0(const s_simulation_player_update *update);

// @retail 0x683a0
void __stdcall function_683a0(byte *block)
{
	s_simulation_input_69110 *input = (s_simulation_input_69110 *)block;
	for (long i = 0; i < 16; i++)
	{
		if (input->actor_mask & (1 << i))
		{
			long index = input->actor_indices[i];
			if (function_badc0(index, 3))
				function_c6de0(index, input->actor_states[i]);
		}
	}
	if (*(bool *)(block + 0xdd0))
		function_14c630(*(dword *)(block + 0xdd4), (const s_machine_address *)(block + 0xdd8));
	long count = *(long *)(block + 0xe38);
	s_simulation_player_update *update = (s_simulation_player_update *)(block + 0xe3c);
	for (long j = 0; j < count; j++, update++)
	{
		bool result;
		switch (update->type)
		{
		case 0:
			result = function_84f30((const s_simulation_player_identity *)update);
			break;
		case 1:
			result = function_84fb0(update);
			break;
		case 2:
			result = simulation_player_remove_if_left((const s_simulation_player_identity *)update);
			break;
		case 3:
			result = function_85140(update);
			break;
		case 4:
			result = function_854c0(update);
			break;
		default:
			result = false;
			break;
		}
		if (!result)
		{
			g_4cf771 = true;
			break;
		}
	}
	if (*(bool *)(block + 0x403c) &&
		(SIMULATION_WORLD->state == 3 || SIMULATION_WORLD->state == 5))
	{
		for (long k = 0; k < 4; k++)
			g_46e320[k](0);
		g_46e320[4](0);
	}
}

bool simulation_watcher_changed(s_simulation_world_owner *watcher);

// @retail 0x681e0
void function_681e0(void)
{
	s_simulation_block_data block;
	if (g_4cf770 && g_4e6948 && g_4e6948->flag1120 && !g_4cf772 &&
		simulation_watcher_changed((s_simulation_world_owner *)g_4cf780))
	{
		function_682e0(&block);
		function_683a0((byte *)&block);
		function_684e0((byte *)&block);
	}
}
