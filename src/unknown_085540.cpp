// @flags /O2 /Ob1 /Gr
/* UNKNOWN_085540.CPP: a simulation world's view onto one remote machine:
   its establishment state machine (states 0..5, each with an id, mirrored by
   the remote end), its replication baseline and its buffered join data
   (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_075870.h"
#include "unknown_067e10.h"
#include "unknown_0662e0.h"

/* the establishment message (type 0x25) */
struct s_simulation_view_establishment
{
	long state;
	long id;
};

byte g_510ca2;

/* the next handle of a handle table (unknown_096ed0.cpp) */
class c_handle_table_450cd0;
long function_98480(long handle, c_handle_table_450cd0 *self);

/* frees a block (not decompiled yet: src/stubs/memory.cpp) */
void function_12d520(long a);

/* the time source; retail inlines function_75870 here */
static inline long view_time_get(void)
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

static inline long view_time_since(long time)
{
	return view_time_get() - time;
}

static inline void view_send_message(c_simulation_view *view, long message_type, long message_size, const void *message)
{
	if (view->channel_index != NONE)
		network_observer_send_message(view->observer, 3, view->channel_index, false, message_type, message_size, message);
}

// @retail 0x859b0
bool c_simulation_view::channel_ready(void)
{
	bool result;
	if (channel_index != NONE)
		result = network_observer_channel_ready(observer, channel_index, 0x2b);
	else
		result = true;
	return result;
}

// @retail 0x85a70
void c_simulation_view::set_state(long new_state, long id)
{
	bool valid;

	if (new_state < 2)
		valid = id == NONE;
	else if (new_state == 2)
		valid = id >= 0;
	else
		valid = id == state_id && new_state == state + 1;

	if (!valid)
	{
		fail(7);
	}
	else if (state != new_state || state_id != id)
	{
		s_simulation_view_establishment message;
		memset(&message, 0, sizeof(message));
		state_id = id;
		message.id = id;
		state = new_state;
		message.state = new_state;
		view_send_message(this, 0x25, sizeof(message), &message);
		update_established();
	}
}

// @retail 0x862c0
inline void c_simulation_view::fail(long reason)
{
	if (failure_reason == 0)
	{
		set_state(0, NONE);
		failure_reason = reason;
	}
}

// @retail 0x86590
inline void c_simulation_view::release_buffer(void)
{
	if (buffer)
	{
		unknown9c = 0;
		unknowna0 = 0;
		function_12d520((long)buffer);
		buffer = 0;
		unknown98 = 0;
	}
}

/* the callback a buffer's owner calls when it goes away */
// @retail 0x86aa0
void __stdcall simulation_view_buffer_disposed(byte *buffer, c_simulation_view *view)
{
	if (view->type == 2 && view->buffer && view->buffer == buffer)
		view->release_buffer();
}

// @retail 0x85600
void c_simulation_view::detach(void)
{
	if (type == 2 && buffer)
		release_buffer();
	world->views[world_index] = 0;
	world->view_count--;
	world = 0;
	world_index = NONE;
}

// @retail 0x85540
void c_simulation_view::initialize(long unknown04_, short type_, s_simulation_view_data *data_, const s_machine_address *address_, long unknown1c_)
{
	unknown04 = unknown04_;
	data = data_;
	type = type_;
	address = *address_;
	unknown1c = unknown1c_;
	world = 0;
	world_index = NONE;
	observer = 0;
	channel_index = NONE;
	failure_reason = 0;
	state = 0;
	state_id = NONE;
	remote_state = 0;
	remote_id = NONE;
	unknown3c = NONE;
	unknown40 = NONE;
	flag78 = false;
	if ((1 << type) & 0x14)
		player_mask = 0;
	if (type_ == 2)
	{
		unknown80 = NONE;
		unknown84 = NONE;
		unknown90 = 0;
		buffer = 0;
		unknown98 = 0;
		unknownac = 0;
		unknown88 = false;
	}
	else if (type_ == 1)
	{
		unknownb0 = 0;
	}
}

// @retail 0x86b40
void simulation_view_baseline_set_active(s_simulation_view_baseline *baseline, bool active)
{
	if (active)
	{
		if ((1 << baseline->view->type) & 0x14)
		{
			memset(&baseline->state, 0, sizeof(baseline->state));
			baseline->time = NONE;
			baseline->sequence = 0;
			baseline->active = true;
		}
		else
		{
			memset(&baseline->state, 0, sizeof(baseline->state));
			baseline->sequence = 0;
			g_510ca2 = true;
			baseline->unknown06 = true;
		}
	}
	else
	{
		if (baseline->active)
			baseline->active = false;
		if (baseline->unknown06)
		{
			g_510ca2 = false;
			baseline->unknown06 = false;
		}
	}
}

// @retail 0x861c0
void c_simulation_view::update_established(void)
{
	bool now_established = false;
	bool synchronized = false;

	if (world && unknown3c != NONE && state_id == remote_id)
	{
		now_established = state >= 2 && remote_state >= 2;
		synchronized = state >= 5 && remote_state >= 5;
	}
	if (now_established != established())
	{
		flag75 = now_established;
		if (!now_established)
		{
			if ((1 << type) & 0x14)
			{
				player_mask = 0;
				if (type == 2)
				{
					unknown80 = NONE;
					unknown84 = NONE;
					if (buffer)
						release_buffer();
				}
			}
			else if (type == 1)
			{
				unknownb0 = 0;
			}
		}
		if (type == 3 || type == 4)
			simulation_view_baseline_set_active(&data->baseline, now_established);
		simulation_world_view_established(world, this, now_established);
	}
	if (flag78 != synchronized)
	{
		flag78 = synchronized;
		simulation_world_view_synchronized(world, this, synchronized);
	}
}

/* sends the view's state again */
static __forceinline void view_send_establishment(c_simulation_view *view)
{
	s_simulation_view_establishment message;
	memset(&message, 0, sizeof(message));
	message.state = view->state;
	message.id = view->state_id;
	view_send_message(view, 0x25, sizeof(message), &message);
}

/* the remote end's establishment message */
// @retail 0x85b20
bool c_simulation_view::handle_establishment(long new_state, long new_id)
{
	bool result = false;

	if (world && unknown3c != NONE)
	{
		long previous_state = remote_state;
		long previous_id = remote_id;
		remote_state = new_state;
		remote_id = new_id;
		bool resend = false;
		if (failure_reason == 0)
		{
			if ((1 << type) & 0x14)
			{
				if (state < 2)
				{
					if (new_state >= 2 || new_id != NONE)
						resend = true;
				}
				else if (state == 2)
				{
					if (new_state != 2 || new_id != state_id)
					{
						if (previous_state == 2 && previous_id == state_id)
							fail(7);
						else if (new_state >= 1)
							resend = true;
						else
							set_state(1, NONE);
					}
				}
				else if (new_state == 0)
				{
					fail(6);
				}
				else if (new_id != state_id || new_state <= 2 || new_state > state || new_state != previous_state + 1)
				{
					fail(7);
				}
			}
			else if (new_state == 2 && state_id == NONE)
			{
				set_state(new_state, new_id);
			}
			else if (new_state >= 2 && state_id == new_id)
			{
				set_state(new_state, new_id);
			}
			else if (state >= 2)
			{
				fail(6);
			}
			else if (previous_state == 0 && new_state > 0)
			{
				resend = true;
			}
			if (resend)
				view_send_establishment(this);
		}
		update_established();
		result = true;
	}
	return result;
}

// @retail 0x86140
void c_simulation_view::set_unknown88(bool value)
{
	if (value && !unknown88)
		time8c = view_time_get();
	unknown88 = value;
	if (value && view_time_since(time8c) >= 2000)
		fail(4);
}

// @retail 0x85f50
bool c_simulation_view::handle_player_update(bool failed, long a, long b, dword controller_mask, const s_simulation_player_state *states)
{
	bool result = false;

	if (failed)
	{
		fail(2);
		result = true;
	}
	else if (a >= unknown80 && b >= unknown84 && b < world->unknown28)
	{
		if (world->unknown18 == 4)
		{
			unknown84 = b;
			unknown80 = a;
			function_6a7f0(world, (s_key_450d14 *)&address, controller_mask, states);
		}
		result = true;
	}
	return result;
}

// @retail 0x86ad0
bool c_simulation_view::has_pending_entity(void)
{
	if (data->unknown39)
	{
		s_simulation_entity_database *database = &world->distribution->field_2098;
		long handle = NONE;
		while ((handle = function_98480(handle, (c_handle_table_450cd0 *)data->handles)) != NONE)
		{
			s_simulation_entity *entity = &database->entities[handle & 0x3ff];
			if (database->definitions->definitions[entity->type]->v6(entity))
				return true;
		}
	}
	return false;
}

// @retail 0x85cb0
bool c_simulation_view::function_85cb0(void)
{
	bool result = false;
	if (type == 3 || type == 4)
	{
		if (!has_pending_entity())
			result = true;
	}
	else if (!buffer || unknownac <= 0)
	{
		result = true;
	}
	return result;
}
/* the input update message (type 0x2b) */
struct s_simulation_input_update_message
{
	long id;
	long sequence;
	s_input_update update;
};

/* the input record code (unknown_1967d0.cpp) */
void function_197360(s_input_record *record);
void __stdcall function_198540(const s_input_record *baseline, const s_input_record *record, s_input_update *update);

// @retail 0x86c50
void simulation_view_baseline_send(s_simulation_view_baseline *baseline)
{
	s_input_record record;
	function_197360(&record);
	if (memcmp(&record, &baseline->state, sizeof(record)) != 0 && !baseline->state.flag0)
	{
		s_simulation_input_update_message message;
		memset(&message, 0, sizeof(message));
		message.id = baseline->view->state_id;
		message.sequence = baseline->sequence;
		function_198540(&baseline->state, &record, &message.update);
		view_send_message(baseline->view, 0x2b, sizeof(message), &message);
		memcpy(&baseline->state, &record, sizeof(record));
		baseline->sequence++;
	}
	baseline->time = view_time_get();
}

// @retail 0x86bc0
void simulation_view_baseline_update(s_simulation_view_baseline *baseline)
{
	if ((baseline->view->type == 3 || baseline->view->type == 4) && baseline->view->established() && baseline->active && g_510ca0)
	{
		if (baseline->time == NONE || !baseline->state.flag0 && g_510cb1 || function_75890(baseline->time) > g_network_configuration.valued00)
		{
			c_simulation_view *view = baseline->view;
			if (!view->channel_ready())
			{
				simulation_view_baseline_send(baseline);
			}
			else if (view->channel_index != NONE)
			{
				network_observer_mark_message(view->observer, view->channel_index, 0x2b);
			}
		}
	}
}

// @retail 0x859d0
void c_simulation_view::update_baseline(void)
{
	if (failure_reason == 0)
	{
		if (type == 3 || type == 4)
		{
			if (data->unknown3a)
				fail(9);
			else if (data->unknown5079)
				fail(10);
			else if (data->baseline.unknown04)
				fail(11);
		}
		if (failure_reason == 0 && (type == 3 || type == 4))
			simulation_view_baseline_update(&data->baseline);
	}
}
/* the players' update message (type 0x28) */
struct s_simulation_player_update_message
{
	long sequence;
	long field_0_4;
	bool buffering;
	byte unknown09[3];
	dword controller_mask;
	s_simulation_player_state states[4];
};

// @retail 0x85fc0
void c_simulation_view::send_player_update(dword controller_mask, const s_simulation_player_state *states)
{
	if (flag78)
	{
		s_simulation_player_update_message message;
		memset(&message, 0, sizeof(message));
		message.sequence = unknownb0++;
		message.field_0_4 = world->unknown28 - 1;
		bool buffering = false;
		if (world->state == 3 || world->state == 5)
			buffering = world->flag2c;
		message.buffering = buffering;
		message.controller_mask = controller_mask;
		for (long i = 0; i < 4; i++)
		{
			if (controller_mask & (1 << i))
				message.states[i] = states[i];
		}
		view_send_message(this, 0x28, sizeof(message), &message);
	}
}
// @retail 0x85d00
bool c_simulation_view::update_player_mask(dword player_mask, dword valid_mask, const t_player_key *keys)
{
	bool result = false;
	if (established())
	{
		dword mask = 0;
		for (long i = 0; i < 16; i++)
		{
			if ((player_mask & (1 << i)) && (valid_mask & (1 << i)) && simulation_world_player_valid(i, world, &keys[i]))
				mask |= 1 << i;
		}
		bool synchronized = type == 2 ? state >= 5 : state >= 3;
		if (synchronized && (~mask & function_696f0(world)))
			fail(8);
		this->player_mask = mask;
		result = true;
	}
	return result;
}
/* the authority starts sending the join data: the client buffers it from
   update number field_0_4 on */
// @retail 0x85dc0
bool c_simulation_view::join_data_begin(long field_0_4)
{
	bool result = false;
	if (world->unknown18 == 3)
	{
		if (!world_receiving_join_data(world))
		{
			if (world_buffer_allocate(world))
			{
				c_class_6a600 *world = this->world;
				world->unknown28 = field_0_4;
				if (world->state == 3)
				{
					function_6ab10(world);
					world->unknown1210 = field_0_4 - 1;
					world->unknown120c = field_0_4;
				}
				world->flag24 = true;
				function_69350(this->world, true);
				return true;
			}
			fail(5);
		}
	}
	return result;
}

/* a chunk of the join data (size > 0), or its end (size == 0, offset is the
   total size) */
// @retail 0x85ed0
bool c_simulation_view::join_data_receive(long offset, const void *data, long size)
{
	bool result = false;
	c_class_6a600 *world = this->world;
	if (world_receiving_join_data(world))
	{
		if (size > 0)
			result = world_buffer_append(world, size, data, offset);
		else
			result = world_buffer_complete(world, offset);
		if (!result)
			fail(5);
	}
	return result;
}

/* the input record module (src/unknown_1967d0.cpp, lane H) */
void function_1988e0(s_input_record *record, s_input_update *update);
void function_1973f0(s_input_record *record);

/* a baseline update from the remote authority, for the establishment with
   this id: an old id is ignored (true), an update out of sequence marks the
   baseline stale */
// @retail 0x85e70
bool c_simulation_view::baseline_update(long id, long sequence, const s_input_update *update)
{
	bool result = false;
	if (id == state_id)
	{
		s_simulation_view_baseline *baseline = &data->baseline;
		if (baseline->unknown06)
		{
			if (sequence && sequence != baseline->sequence)
			{
				baseline->unknown04 = true;
			}
			else
			{
				function_1988e0(&baseline->state, (s_input_update *)update);
				function_1973f0(&baseline->state);
				baseline->sequence = sequence + 1;
				result = true;
			}
		}
	}
	else if (id < state_id)
	{
		result = true;
	}
	return result;
}
