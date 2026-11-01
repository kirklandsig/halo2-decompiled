// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_085540.CPP: a simulation world's view onto one remote machine:
   its establishment state machine (states 0..5, each with an id, mirrored by
   the remote end), its replication baseline and its buffered join data
   (lane D) */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_123b30.h"
#include "unknown_075870.h"
#include "unknown_067e10.h"
#include "unknown_0662e0.h"
#include "unknown_0820f0.h"

struct s_sender;
void function_96ed0(s_sender *self);

struct s_view_distribution_senders
{
	s_handle_peers handles;
	byte unknown2048[8];
	dword sender_mask;
	s_sender *senders[15];
};

static __forceinline void view_clear_child(c_vtable_450cf4 *aggregate, long index)
{
	aggregate->unknown24 -= ((long *)aggregate->unknown18)[index];
	aggregate->children[index] = 0;
}

static __forceinline void view_clear_updates(c_vtable_450c94 *updates)
{
	updates->initialized = false;
	updates->source = 0;
}

static inline bool controller_world_is_authority(c_class_6a600 *world)
{
	return world->state != 3 && world->state != 5;
}

// @retail 0x85880
void function_85880(c_simulation_view *view)
{
	if (view->state)
		view->set_state(0, NONE);
	((s_link_connection *)g_4d87d4)[view->unknown3c].callback = 0;
	view->unknown44[0] = 0;
	if (view->type == 3 || view->type == 4)
	{
		s_view_distribution_senders *distribution = (s_view_distribution_senders *)view->world->distribution;
		view_clear_child(&((c_replication_view_storage *)view->data)->aggregate, 1);
		view_clear_child(&((c_replication_view_storage *)view->data)->aggregate, 0);
		((c_replication_view_storage *)view->data)->aggregate.source = 0;
		long index = view->world_index;
		distribution->handles.tables[index]->function_97fe0();
		distribution->handles.tables[index] = 0;
		distribution->handles.table_mask &= ~(1 << index);
		index = view->world_index;
		function_96ed0(distribution->senders[index]);
		distribution->sender_mask &= ~(1 << index);
		distribution->senders[index] = 0;
		view_clear_updates(&((c_replication_view_storage *)view->data)->updates);
		c_vtable_450d1c *sender = &((c_replication_view_storage *)view->data)->sender;
		function_96ed0((s_sender *)sender);
		sender->unknown08 = 0;
		view->data->unknown38 = 0;
		((c_replication_view_storage *)view->data)->aggregate.unknown04[0] = 0;
	}
	s_network_observer_channel *channel = &view->observer->channels[view->channel_index];
	channel->owner_mask &= ~8;
	view->observer = 0;
	view->unknown40 = NONE;
	view->unknown3c = NONE;
	view->channel_index = NONE;
}

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
		result = network_observer_channel_ready(observer, 3, channel_index, 0x2b);
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

static __forceinline void function_8659e(long *arg_0)
{
	arg_0[0] = 0;
	arg_0[1] = 0;
}

// @retail 0x86590
inline void c_simulation_view::release_buffer(void)
{
	if (buffer)
	{
		function_8659e(&unknown9c);
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
	c_class_6a600 *local_0 = world;
	long local_1 = world_index;
	local_0->views[local_1] = 0;
	local_0->view_count--;
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
			long *sequence = &baseline->sequence;
			memset(&baseline->state, 0, sizeof(baseline->state));
			*sequence = 0;
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

#pragma optimize("g", off)
static __forceinline bool function_861c3(const c_simulation_view *arg_0)
{
 bool local_0 = false;
 long local_1 = arg_0->unknown3c;
 if (local_1 != NONE)
  local_0 = *(const bool *)&arg_0->flag75;
 return local_0;
}
#pragma optimize("", on)
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
	if (now_established != function_861c3(this))
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
	bool result = false;

	if (data->unknown39)
	{
		s_simulation_entity_database *database = &world->distribution->field_2098;
		long handle = NONE;

		do
		{
			handle = function_98480(handle, (c_handle_table_450cd0 *)data->handles);
			if (handle == NONE)
				break;
			s_simulation_entity *entity = &database->entities[handle & 0x3ff];
			if (database->definitions->definitions[entity->type]->v6(entity))
				result = true;
		} while (!result);
	}
	return result;
}

// @retail 0x85cb0
bool c_simulation_view::function_85cb0(void)
{
	bool result = false;
 if (type == 3)
  goto local_0;
 if (type == 4)
 {
local_0:
  if (!has_pending_entity())
   result = true;
 }
 else if (!buffer || unknownac <= 0)
  result = true;
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
	if ((baseline->view->type == 3 || baseline->view->type == 4) && function_861c3(baseline->view) && baseline->active && g_510ca0)
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
				network_observer_mark_message(view->observer, 3, view->channel_index, 0x2b);
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
				result = true;
			}
			else
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

long network_observer_attach_channel(s_network_observer *observer, long owner_index, const XNADDR *address);
void function_97f60(s_handle_peers *peers, long index, c_handle_table_450cd0 *table);
void replication_table_attach_sender(s_handle_peers *peers, long index, c_handle_table_450cd0 *table);
void __stdcall function_68550(c_simulation_view *view);
void link_connection_callback_initialize(s_connection_callback *callback, c_connection_client *const *clients,
 void *context, void (__stdcall *function)(void *), long count, const dword *types, bool active);

static __forceinline void view_add_child(c_vtable_450cf4 *aggregate, long index, c_interface_450c94 *child)
{
 aggregate->children[index] = child;
 long count = child->v2();
 aggregate->unknown24 += count;
 ((long *)aggregate->unknown18)[index] = count;
 *((long *)child + 1) = index;
}

struct s_85650
{
 void *field_00;
 void *field_04;
 dword field_08;
 s_sender *field_0c[15];
};

// @retail 0x85650
void function_85650(c_simulation_view *view, s_network_observer *observer, const XNADDR *address, long channel_index)
{
 network_observer_attach_channel(observer, 3, address);
 long connection_index = observer->channels[channel_index].connection_index;
 s_link_connection *connection = &((s_link_connection *)g_4d87d4)[connection_index];
 view->unknown3c = connection_index;
 view->observer = observer;
 view->channel_index = channel_index;
 view->unknown40 = connection->local_sequence;
 view->failure_reason = 0;
 if (view->type == 3 || view->type == 4)
 {
  s_view_distribution_senders *distribution = (s_view_distribution_senders *)view->world->distribution;
  c_replication_view_storage *storage = (c_replication_view_storage *)view->data;
  c_vtable_450cf4 *aggregate = &storage->aggregate;
  *(long *)&aggregate->unknown04[4] = view->world_index;
  aggregate->unknown24 = 0;
  for (long i = 0; i < 3; i++)
  {
   aggregate->children[i] = 0;
   ((long *)aggregate->unknown18)[i] = 0;
  }
  aggregate->source = 0;
  aggregate->unknown04[0] = 1;
  ((c_replication_view_storage *)view->data)->source.world = (s_world_450d14 *)view;
  ((c_replication_view_storage *)view->data)->aggregate.source = &((c_replication_view_storage *)view->data)->source;
  c_vtable_450d1c *sender = &((c_replication_view_storage *)view->data)->sender;
  sender->player = view->world_index;
  sender->requests = 0;
  sender->request_count = 0;
  sender->unknown08 = 1;
  sender->unknown09 = 0;
  sender->pending = 0;
  sender->unknown1c = 0;
  sender->unknown24 = 0;
  s_85650 *owner = (s_85650 *)((byte *)distribution + 0x2048);
  sender->owner = (s_owner_450d1c *)owner;
  view_add_child(&((c_replication_view_storage *)view->data)->aggregate, 0, &((c_replication_view_storage *)view->data)->sender);
  long index = view->world_index;
  s_sender *table_sender = (s_sender *)&((c_replication_view_storage *)view->data)->sender;
  owner->field_08 |= 1 << index;
  owner->field_0c[index] = table_sender;
  function_97f60(&distribution->handles, view->world_index, &((c_replication_view_storage *)view->data)->handles);
  view_add_child(&((c_replication_view_storage *)view->data)->aggregate, 1,
   (c_interface_450c94 *)&((c_replication_view_storage *)view->data)->handles);
  replication_table_attach_sender(&distribution->handles, view->world_index,
   &((c_replication_view_storage *)view->data)->handles);
  ((c_replication_view_storage *)view->data)->updates.reset(
   (c_source_450c94 *)&((c_replication_view_storage *)view->data)->source);
  view_add_child(&((c_replication_view_storage *)view->data)->aggregate, 2,
   &((c_replication_view_storage *)view->data)->updates);
  s_simulation_view_baseline *baseline = &view->data->baseline;
  baseline->view = view;
  *((byte *)baseline + 6) = 0;
  *((byte *)baseline + 5) = 0;
  *((byte *)baseline + 4) = 0;
 }
 dword types[4];
 c_connection_client *clients[4];
 long count = 0;
 if (view->type == 3 || view->type == 4)
 {
  types[0] = 0x1a;
  clients[0] = (c_connection_client *)&((c_replication_view_storage *)view->data)->aggregate;
  count = 1;
 }
 link_connection_callback_initialize((s_connection_callback *)view->unknown44, clients, view,
  (void (__stdcall *)(void *))function_68550, count, types, controller_world_is_authority(view->world));
 connection->callback = (s_connection_callback *)view->unknown44;
 view->flag78 = false;
 view->set_state(view->state, view->state_id);
}


struct s_ring_buffer
{
 void write_wrapped(long count, long offset, const void *src);
 void read_wrapped(long offset, long count, void *dst);
 long write(long count, const void *src);
 long size;
 byte *data;
 long start;
 long used;
};
static inline long view_buffer_write(s_ring_buffer *buffer, long count, const void *source)
{
 long result = NONE;
 if (buffer->used + count <= buffer->size)
 {
  result = (buffer->start + buffer->used) % buffer->size;
  if (count > 0) buffer->write_wrapped(count, result, source);
  buffer->used += count;
 }
 return result;
}
bool function_1995a0(const byte *source, dword source_size, byte *destination, long *compressed_size, dword capacity, long level);

// score 14 (was 14): retail's code apart from two single differences: retail stores kind 1
// between the header's two zero stores (ours after both), and forms the data address with
// `add eax, ebp` (ours `lea ecx, [eax + ebp]`). The loop runs while the writes succeed: the
// unrotated head (zero and kind 1 before the count test, ring base rebuilt each pass) only
// appears when the loop tests a value the failures clear, and the compressed size has to end
// with its block for the remaining count to share its slot.
// Round 81: `while (result)` with the terminal record's break 14 (kept); the same with a named
// `byte *data = destination + offset;` for the data write 2 (the add is retail's; only the kind
// store order is left, so it waits on a match); harness: `= { 1 }`, union or nested-struct
// initialisers, kind set in both arms, `|=`/`++` kind, an init helper, two partial memsets (the
// order is retail's, but the zero then feeds `cmp eax, ecx`), a named count test, the clamp as an
// if, offset declared at the top (worse), write_wrapped parameter orders, /G5-/G7 and /Ob2: none
// moves the kind store.
// Earlier (score 49, the old for-loop body): left were the prologue (retail pushes
// ebp before ebx and copies the count into the compressed size's slot), the loop head (retail sets
// the header's zero and kind 1 at the head, before the count test, and rebuilds the ring base each
// pass; ours peeled the test) and the count and data-address registers in the loop body.
// Retail's unrotated head only appears with a flag tested at the head (a bool set by the count
// test, or `while (!done)` with `done` set by the terminal record), which the compiler keeps;
// with the compressed size scoped to the compressor call, a `remaining` copy made on success and
// single-exit breaks that form gives 20 (aligned 10); with the offset zeroed beside the count
// copy and the offset advanced before the count it gives 14 (aligned 4), but every flag-free
// spelling rotates the loop (128), so it is not kept. Its two single differences: retail sets
// kind 1 between the two zero stores and forms `destination + offset` with `add eax, ebp` (ours
// `lea ecx, [eax + ebp]`).
// Tried: flag 130; plus remaining 58; scoped 51; copy on success 38; failures returned 63;
// single-exit breaks 38; success first 20; then element address 20; offset first 20;
// header = { 1 } 22; flag before the header 125; count before cursors 24; all of it without the
// flag 128; direct view plus writer forceinline 93; unsigned writer result 93; writer reference 93;
// native count 93; unsigned offset 93; word kind 93; named packet record 93; moving payload
// pointer 144; terminal-record do loops 89; positive-chunk finalization 85. On the 20 form: the
// view read through its address or directly 20/20; offset declared at the top 42; `&destination[offset]`,
// a const void cast or `offset + destination` 20 each; remaining declared first 20; a named
// condition or a plain test 128; offset stored before the size 66; ring base first 34;
// `while (result && !done)` 136; `while (!done)` 20; a named chunk size 111; view count first 24;
// offset zeroed with the count copy 18; then offset advanced first 14. On the 14 form: header
// `= { 0 }` 21; `memset(&header, 0, 8)` 14; helper source before count 14; byte data cast 14;
// destination read once through its address 14; a void destination 14; dword offset 14; offset
// declared first 14; word size 15; the terminal record at the loop head with a break 128; a
// break to a terminal record after the loop 159.
// Round 95 (lane 14's 14 form reproduced; one-file harness exact for it): deleting any other
// statement moves neither single, so both are local. `add eax, ebp`: `&destination[offset]`,
// `offset + destination` or a const void cast 14 each; the real s_ring_buffer::write at the data
// write 162 (stays out of line); that method declared inline in its file 14 (LTCG inlines it with
// the same `lea ecx`; 0x869a0 2 -> 99); harness: byte source, forceinline copy, position local,
// size local, kind cast, `memset(&header, 0, 8)`, header declared outside the loop, word kind,
// a pointer-based kind store, `0 < remaining`, a long flag: no change.
// Round 22 (one-file harness with src/unknown_199560.cpp, exact): `header.size = 0;` or
// `header.offset = 0;` after the kind 14/14; the header value-initialised from a named type
// (`= s_header()`, declared or assigned) 24/24; `= { 0, 0, 0 }` then the kind 117.
// Round 84: the terminal arm written first 14; memset and kind 1 in each arm 74; a value-initialised
// header (`= s_header()`) 24 (410 bytes); the clamp as `remaining < 1024 ? remaining : 1024` 17; a
// do/while under `if (result)` 14. Compile-only probes (a constructor, a constant zero header copy,
// a memset of a copy, bitfield or union kinds, `*(char *)&kind`, a pointer memset) all store both
// zeros before the kind; no other retail function stores between an 8-byte memset's two halves.
// Round 89, each with the named data pointer: two partial memsets 18; direct offset zero after
// the first partial memset 112; explicit field stores 118; aggregate `= { 0 }` 9; named-type value
// initialization 17; early data address with two partial memsets 18; header outside the loop 18;
// named positive-count test 18; terminal size memset 18. All restored because none matched.
// Round 137 (lane 4), each with the named data pointer (2): `header.offset = offset;` before the size in both arms
// 56 (the offset stores merge into the head); the offset stored once in the head before the kind 126 (414 bytes;
// stores keep program order); two partial memsets 18 again (head store order is retail's, but the zero is a shared
// constant: `cmp eax, ecx` and the terminal `mov word [esp+0x16], cx`), with `remaining >= 1` 21, `0 < remaining`
// 18; `= { 0 }` in place of the memset 9 (409 bytes); permuter 2.
// @retail 0x86800
bool function_86800(dword capacity, c_simulation_view *view, dword source_size, byte *destination)
{
 bool result;
 long remaining;
 long offset;
 {
  long compressed_size;
  result = function_1995a0(game_state_globals.base_address, source_size, destination, &compressed_size, capacity, 9);
  if (result)
  {
   offset = 0;
   remaining = compressed_size;
  }
 }
 while (result)
 {
  struct { short kind; short size; long offset; } header;
  memset(&header, 0, sizeof(header));
  header.kind = 1;
  if (remaining > 0)
  {
   header.size = (short)(remaining > 1024 ? 1024 : remaining);
   s_ring_buffer *buffer = (s_ring_buffer *)&view->unknown9c;
   header.offset = offset;
   if (view_buffer_write(buffer, sizeof(header), &header) == NONE ||
    view_buffer_write(buffer, header.size, destination + offset) == NONE)
    result = false;
   else
   {
    offset += header.size;
    remaining -= header.size;
    view->unknownac++;
   }
  }
  else
  {
   header.size = 0;
   s_ring_buffer *buffer = (s_ring_buffer *)&view->unknown9c;
   header.offset = offset;
   if (view_buffer_write(buffer, sizeof(header), &header) != NONE)
    view->unknownac++;
   else
    result = false;
   break;
  }
 }
 return result;
}

#include "unknown_080e10.h"
extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);
void function_6a860(c_class_6a600 *world, long *size);

static __forceinline byte *view_allocate_buffer(long size, long owner, long release)
{
 return (byte *)function_80e14(size, owner, 0, release, 30, 0.1f);
}

// score 22 (was 22): ours hoists the time stamp's volatile low store and the header memset's
// low store, and packs start's spill apart from the volatile. The dword local is written before
// the byte local; retail writes the byte first and then the dword into the structure. Tried: the
// volatile and start renamed 22; used zeroed before start 22; the header declared before the
// view stores 22. _ReadWriteBarrier before the first allocation 19 (retail's store order; start's
// spill still apart from the volatile's slot); before the count increment 27; before both
// allocations 19. Each stamp's start spill shares its own volatile's slot in retail (ours pairs
// the two volatiles): the helper's volatile renamed after `start` 22; the helper's result declared
// before start 32 (and 0x6a8a0, 0x80e10, 0x18f260 worse). With the barrier: the header memset
// before `unknownac = 0` 19; `long size` before the count increment 19. A compile-only build
// gives the +0xc/+8 ring stores and the hoisted header store for every statement order.
// Round 140 (lane 4): the ring pointer declared first in the block 25; `unknownac = 0` ahead of the
// ring stores 200 (644 bytes); `header.value` assigned before kind and size 25; the count increment
// after the 0x6a860 call 30 (its store then rides EBP into 0x86800's pushes: retail's interleave,
// wrong call). Shared helper, all nine users identical or worse: the elapsed reader result named 22;
// the reader's local non-volatile 250 (its zero stores vanish).
// @retail 0x862e0
bool function_862e0(c_simulation_view *view)
{
 bool result = false;
 view->unknown90++;
 byte *memory = view_allocate_buffer(0x80000, (long)view, (long)simulation_view_buffer_disposed);
 byte *scratch = view_allocate_buffer(0x40000, 0, 0);
 if (memory)
 {
  if (scratch)
  {
   view->buffer = memory;
   view->unknown98 = 0x80000;
   s_ring_buffer *buffer = (s_ring_buffer *)&view->unknown9c;
   buffer->size = 0x80000;
   buffer->data = memory;
   buffer->start = 0;
   buffer->used = 0;
   view->unknownac = 0;
   struct { short kind; short size; long value; } header;
   memset(&header, 0, sizeof(header));
   result = true;
   header.kind = 0;
   header.size = 0;
   header.value = view->world->unknown28;
   if (view_buffer_write(buffer, sizeof(header), &header) != NONE)
   {
    view->unknownac++;
    long size;
    function_6a860(view->world, &size);
    if (!function_86800(0x40000, view, size, scratch)) result = false;
    view->world->flag11fc = 0;
    g_46e320[4](0);
   }
   else
    result = false;
   if (!result) view->release_buffer();
  }
  else
   function_12d520((long)memory);
 }
 if (scratch) function_12d520((long)scratch);
 return result;
}


bool function_685f0(void *block, long *size, byte *destination, long capacity);

// score 2 (was 2): the constant 0xffff is loaded before the address of the buffer; retail loads
// the address first. The registers that carry the destination and the capacity are the
// callee's choice, and function_685f0 in src/unknown_067e10.cpp is three instructions
// off retail. BLOCKED-BY 0x685f0 (round 14: it takes the destination in eax, retail's
// in edx; its body is contributor code).
// Round 59: unchanged; the callee still reads the destination from eax (`mov edi, eax`, retail
// `mov edi, edx`) and this is its only caller, so no caller form was retried.
// @retail 0x869a0
bool function_869a0(c_simulation_view *view, void *block)
{
 byte encoded[0xffff];
 long encoded_size;
 bool result = false;
 if (function_685f0(block, &encoded_size, encoded, sizeof(encoded)))
 {
  struct { short kind; short size; long sequence; } header;
  memset(&header, 0, sizeof(header));
  header.kind = 2;
  header.size = (short)encoded_size;
  header.sequence = *(long *)block;
  s_ring_buffer *buffer = (s_ring_buffer *)&view->unknown9c;
  if (view_buffer_write(buffer, sizeof(header), &header) != NONE &&
   buffer->write(encoded_size, encoded) != NONE)
  {
   view->unknownac++;
   result = true;
  }
 }
 return result;
}


bool function_68670(byte *source, long size, void *block);

// score 72 (was 72): this round: the case-0 copy of the header value, block- and function-scoped,
// scores 137 both times: the header then sits at retail's esp+0xc and the copy at esp+0x14, but the
// ring cursor stays in esi across the first read and the used count is updated through a register,
// so the read's offset is still forwarded in edx instead of reloaded and the header size still
// lands in si. Retail's dx load of the size kills the remainder left in edx by the idiv, which is
// what forces its reload; ours keeps the remainder for the second read. The permuter ran without
// finding a better form. The header home, the size register, the forwarded cursor and the grouped
// case-1 stores remain.
// Retail treats the read header as reachable through the ring buffer: it reloads
// Earlier (72): retail treats the read header as reachable through the ring buffer: it reloads
// the start after each read call and reloads the header size after the start/used stores. The
// header's value sent directly (its address escapes) gives that; retail still sends a copy at
// esp+0x14 (header at esp+0xc, ours at esp+0x10), interleaves the message's zero and value stores
// and keeps the header size in dx where ours uses si.
// Earlier score 97: the window fields now load in retail's order, but retail subtracts the span
// from 0x200 into eax where we subtract 0x200 from the span, and our ring buffer update reuses
// the start read before the read call instead of reloading it as retail does.
// order (+0x1c before +0x18), so the rest of the block follows differently.
// Earlier: a `used = next - end` local gives retail's window test exactly (135 from the
// listing shift); retail also reloads the buffer start after each read call and loads the header
// size after the start store, as if the call might touch the buffer. Window as shift 97; window
// pointer 97; inline read method 135; header size per use 135; buffer hoisted out of the loop 204.
// Round 182: Self-aligned standard entry, case-workspace scopes and complete ring-field forms do not recover receiver and reader visibility
// Trials 1-8: 179, 172, 179, 172, 164, 159, 164, 159.
// Distinct native byte-buffer class with ordinary/inline/noinline/both reader knowledge:
// 97/352/97/97; with the occupied-span subexpression 135/353/135/135. Original class identity,
// all member declarations, caller body and window form restored; reload/count-update gap remains.
// Round 222: connection-ready bool/byte/short/long on original entry, each 97; each with named
// occupied span 135. Original body restored; reader visibility, cursor reloads and count updates
// remain.
// Earlier note (score 97): retail behaves as if the read call could write any memory: it reloads the
// Earlier: retail behaves as if the read call could write any memory: it reloads the
// buffer start after each read, updates the used count in memory (`add [ebx+0xc], -8`) and loads
// the header size only after the start/used stores; ours keeps the start in ESI across the first
// read and hoists the header size. Retail also subtracts the span from 0x200 into EAX (ours adds
// 0x200 to the reassociated span), and in case 1 keeps two dead zero stores, each just before its
// field's value store. A one-file compile without whole-program optimization (read call opaque)
// gives retail's reloads and late header load.
// Tried: window as shift 97; window pointer 97; inline read method 135; header size per use 135;
// buffer hoisted out of the loop 204; `used = next - end` local (retail's window test exactly, 135
// from the listing shift); self-aligned standard entry, case-workspace scopes and complete ring-field
// forms 159-179; a distinct byte-buffer class with ordinary/inline/noinline/both reader knowledge
// 97/352/97/97, with the occupied-span subexpression 135/353/135/135; connection-ready
// bool/byte/short/long 97 each, each with a named occupied span 135; used update first 97; named next
// start 143; buffer outside the loop 201; payload count as short or read at each use 97; cases in
// order 2, 1, 0 97; header renamed 97; parameter-address carrier removed 179; copy method inline 352,
// inline plus out of line 97.
// Round 17: case 1's offset/size zeroed by a memset of an {offset, size} header member 146 (keeps
// retail's zero stores, aligned diff 53 -> 45, but stores both zeros first), per-field memsets 145;
// with the named span for the window 137 (window exact, aligned 38); on that, used update first 137,
// named short payload size 137, a shared inline read helper mirroring view_buffer_write 137 (same code).
// Round 232: Native signed header bitfields kind/size/both/value/kind-value/size-value/all, then all with occupied-window local
// 97, 97, 97, 97, 97, 97, 97, 135. Original executable source restored.
// Round 107: the message's offset/size cleared with memset (retail's surviving zero stores) 146
// (8 bytes longer); occupied-span local + memset 137; that without the view address line 154; the
// header value sent directly 163; all three 72 (kept); without the memset 120; without the span local 144.
// On the 72 form: short size local 72 (identical); size declared at function scope 72 (identical);
// header and payload declared in the loop 118; without the view address line 168; message size
// assigned before its offset 73.
// Round 33 (lane 10), on the 72 form: the payload read using header.size at each use (no size
// local) 116 (554 bytes); case 1's offset and size each cleared by its own memset just before its
// value store 97; the header declared inside the loop 118 at retail's 553 bytes (the header size
// then takes DX as in retail, but view/size swap ESI/EDI, used is updated through a register and
// the start is forwarded); that with case 0 sending a block-local copy of the value 137; payload
// declared before the header 72 (identical). Retail's frame is exactly header 0xc, value copy 0x14,
// payload, block, message (0x14454 bytes), so case 0 sends a copy; what escapes its header is open.
// DL209: Case/function-scope sequence copies by memcpy/memmove 137/137/153/153; complete
// native/memcpy/memmove/field header copies 147/147/155/142. Separate-copy home returns but reload
// knowledge is lost; all copies restored.
// Round 114 (lane 6, fast): case 0 sending a block-local copy of the value 137 (561 bytes; the header
// then sits at retail's esp+0xc, but the start stays in ESI across the read and `used` is updated
// through a register); the copy declared at function scope 137; header and payload read by an inline
// helper taking the header by pointer (header->size read inside it) 72 (identical code: VC resolves
// the pointer), with the case 0 copy 137; the copy with `used` updated before `start` 137. Retail's
// 0x1a19a0 copies with inline rep movs, so it is no more opaque to LTCG there than here.
// Round 23 (72): case 1's offset and size each cleared by its own memset 97; case 0 sending a
// `long message` copy (header then at esp+0xc as retail, but the start stays in ESI across the
// first read and the used count is not updated in memory) 137; payload declared before the header
// 72; the header alone declared in the loop 118 (553 bytes, the size in DX as retail but the view in
// ESI); the payload alone declared in the loop 118 (553 bytes: case 1's interleaved zero/value
// stores exactly as retail; the view/size ESI/EDI swap, the header at +0x10 and the first update's
// register form remain); that with case 0's `long message` copy 146. On the loop-scoped payload
// form: the buffer pointer declared first in the loop 95 (543 bytes); the occupied span written in
// the window test 146; the header also in the loop after the payload 118; the view address line
// after the header declaration 118.
// Round 49 (lane 16, fast): case 0 sending a memset one-field message 137 (retail's header home
// +0xc and copy +0x14, but the start stays in ESI across the first read and the header size is
// hoisted); the payload count declared before its test and tested as a long 106 (553 bytes, header
// size in DX as in retail, but a 32-bit test and the start forwarded through EAX); that with the
// case-0 message 129; the count declared before the short test 155; case 2's failure through the
// existing fail helper 72 (identical code, kept). Diagnostics: the case-0 message with the header's
// address forced to escape restores retail's reloads and late header load (retail behaves as if the
// read call were opaque) but keeps the size in SI (167); per-field memsets still hoist both zero
// stores 97. Retail's case-1 store interleave follows its registers (value and size share ECX).
// Round 45 (lane 11, fast): retail's frame is the 72 form's plus a case-0 copy (ours pads 4 bytes below
// the header; the compile-only listing shows no other local), and retail's header is address-exposed:
// its size loads only after the start/used stores and the first read reloads the ring fields. Grouping
// the header and the case-0 message in one struct (diagnostic only, a grouping form) gives both: 39 at
// retail's 553 bytes, leaving the header-size register (SI, retail DX, so retail reloads the start before
// the payload read and interleaves case 1's stores) and case 0's copy store, which retail schedules after
// the channel load. On that diagnostic: the size declared before its test 122; a short size 39; the
// header size at each use 125; case 0 written out under the channel test 36 (helper body by hand, the
// copy store then lands after the branch). Real forms: the view-address line after the header and
// payload declarations 72 (same frame); `standard` (self-aligned entry) without the view-address line 168
// (519 bytes, the view is then held in a register), with the case-0 copy 154, with the grouping 154; a
// ring read member (read_wrapped, start advance, used update) inline in this file's ring declaration
// 72 (identical), with the case-0 copy 137; the same member out of line in the ring file 243 (not
// inlined, 466 bytes). What exposes the header in retail without sending it is the open question.
// Round 108 (lane 8; a one-file LTCG harness, this file + 0x1a19a0/0x68670/0x75e80 files, gives the build's 72
// exactly): a compile without whole-program optimization does not give retail's code (retail passes the read and
// send arguments in registers, so retail's object is LTCG). Real (fast): the payload test written `>= 1` 63 (553
// bytes, but `cmp si, 1; jl` where retail has `test dx, dx; jle`: alignment only, not kept); the buffer start read
// into a local for the first read and update 78 (543 bytes); payload declared before the header 72 (identical);
// the kind read into a short local for the switch 72 (identical); case 0 sending a copy of the value made before
// the switch 142 (558 bytes). Harness: case 0's block-local copy 137 (header at retail's +0xc, but the start then
// stays in ESI across the read and `used` is updated through a register), without the view address line 154.
// Round 59 (fast), all with case 0 sending a block-local copy (frame and case 0 then retail's, 137):
// case 1's offset/size each cleared by its own memset 135 (560 bytes); header and payload declared in
// the loop 146; payload alone in the loop 146; the payload size at function scope 137; header and
// payload in the loop with per-field memsets 144. Retail treats the header as escaped (start reloaded
// after the first read, used updated in memory) although its copy keeps &header from the send.
// Round 44 (lane 10, fast): `long size = header.size;` ahead of the short test, used by the payload
// read 155 (506 bytes); case 0 sending a function-scope `long value` copy 137 (561 bytes); the switch
// on `(long)header.kind` 72 (identical); the payload block's used update before the start update 103
// (553 bytes); the buffer pointer declared before the window test 82. Retail's DX is the short size
// live from the test into the cases (reloaded after the payload read); ours coalesces it with the
// long size in ESI.
// Round 31 (m3 lane 2, fast): case 2 as the switch's default (no __assume) 144; the case-0 copy
// with the payload count declared before its test 129; the payload read using header.size at each
// use 116 (554 bytes); the count decrement as the for loop's step 171 (483 bytes); payload declared
// before the header with the case-0 copy 137. Diagnostics (grouping structs, reverted at once): the
// header, the case-0 copy and the payload in one record 49 (frame 0x1445c); the header and the copy
// in one record 39 at 553 bytes (the header escapes through the sent copy: retail's frame, header
// homes and in-memory ring updates). Left there: the size in SI where retail has DX, because ours
// forwards the stored ring start (EDX) into the second read where retail reloads it, and the case-0
// copy scheduling and 0x68670 argument order that follow from it.
// Round 41 (lane 13, --fast): the header declared in the loop 118 (553 bytes: its size takes DX and case 1's interleaved
// stores are retail's, but view/size swap ESI/EDI, `used` is updated through a register and the start is forwarded
// through EAX), at any point of the loop 118, with the payload in the loop too 118; in the loop with the header's `used`
// update first 76 (553: the `used` update is retail's memory add; left: view ESI / size EDI, the remainder moved to EAX
// and forwarded to the payload read, the header at +0x10, the loop-top compare on memory), with the payload's update
// first too 114, the payload's alone 136 (555), with the case-0 copy 137 (561), with the payload in the loop 76; the
// header's `used` update first at function scope 109; the case-0 block-local copy alone 137 (561). All restored.
// @retail 0x865d0
void __stdcall function_865d0(c_simulation_view *view)
{
 // Keep the retail stack argument under whole-program optimization.
 c_simulation_view *const *view_reference = &view;
 struct { short kind; short size; long value; } header;
 byte payload[1024];
 while (view->established() && view->unknownac != 0)
 {
  s_link_connection *connection = link_connection_get(view->unknown3c);
  if (connection->state != 5 || !(connection->flags & 0x10)) break;
  s_network_stream_header *stream = network_stream_get(connection->stream_index);
  long occupied = stream->window.next - stream->window.end;
  if ((512 - occupied) * 32 < 0x600) break;
  s_ring_buffer *buffer = (s_ring_buffer *)&view->unknown9c;
  buffer->read_wrapped(buffer->start, sizeof(header), &header);
  buffer->start = (buffer->start + (long)sizeof(header)) % buffer->size;
  buffer->used -= sizeof(header);
  if (header.size > 0)
  {
   long size = header.size;
   buffer->read_wrapped(buffer->start, size, payload);
   buffer->start = (buffer->start + size) % buffer->size;
   buffer->used -= size;
  }
  switch (header.kind)
  {
  case 0:
   view_send_message(view, 0x29, sizeof(header.value), &header.value);
   break;
  case 1:
   {
    struct { long offset; long size; byte bytes[0xfff8]; } message;
    memset(&message, 0, 2 * sizeof(long)); // the offset and size words
    message.offset = header.value;
    message.size = header.size;
    if (message.size > 0) memcpy(message.bytes, payload, message.size);
    view_send_message(view, 0x2a, message.size + 8, &message);
   }
   break;
  case 2:
   {
    union { __int64 alignment; byte bytes[0x4048]; } block;
    memset(&block, 0, sizeof(block));
    if (!function_68670(payload, header.size, &block))
    {
     view->fail(5);
     return;
    }
    view_send_message(view, 0x27, sizeof(block), &block);
   }
   break;
  default: __assume(0);
  }
  view->unknownac--;
 }
}


// @retail 0x860b0
void function_860b0(c_simulation_view *view, void *block)
{
 if (view->state == 5)
 {
  union { __int64 alignment; byte bytes[0x4048]; } message;
  memcpy(&message, block, sizeof(message));
  view_send_message(view, 0x27, sizeof(message), &message);
 }
 else if (view->buffer)
 {
  if (function_869a0(view, block)) function_865d0(view);
  else if (!view->failure_reason)
  {
   view->set_state(0, NONE);
   view->failure_reason = 5;
  }
 }
}
