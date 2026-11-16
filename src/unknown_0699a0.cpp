#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_08b110.h"
#include "unknown_096ed0.h"
#include "unknown_067e10.h"
#include "network_message_types.h"
#include <string.h>

// @flags /O2 /Ob1 /Gr

struct s_simulation_controller
{
	long field_00;
	long field_04;
	long field_08;
	t_player_key key;
	s_machine_address machine;
	byte field_1e[2];
	c_class_6a600 *world;
	bool field_24;
	bool field_25;
	byte field_26[2];
	long field_28;
	s_player_action action;
};

void player_action_initialize(s_player_action *action);

static inline bool controller_world_is_authority(c_class_6a600 *world)
{
	return world->state != 3 && world->state != 5;
}

#pragma inline_depth(0)
static __forceinline void function_84751(s_player_action *arg_0)
{
 player_action_initialize(arg_0);
}
#pragma inline_depth(8)
// @retail 0x84750
void simulation_controller_initialize(s_simulation_controller *controller, c_class_6a600 *world,
	long field_00, long field_04, long field_08, const t_player_key *key, const s_machine_address *machine)
{
	controller->field_00 = field_00;
	controller->field_04 = field_04;
	controller->field_08 = field_08;
	memcpy(controller->key, *key, sizeof(controller->key));
	controller->machine = *machine;
	controller->field_24 = false;
	controller->world = world;
	controller->field_25 = !controller_world_is_authority(world);
	controller->field_28 = NONE;
	function_84751(&controller->action);
}

struct s_replication_sender_view
{
	byte unknown00[0xc];
	c_vtable_450d1c *senders[15];
};

static __forceinline void function_89eb3(s_allocator_globals *arg_0, void *arg_1);

static __forceinline void release_block(void *block)
{
	long local_0;
	if (!g_4d87f8->allocator->get_info(block, &local_0))
		local_0 = NONE;
	function_89eb3(g_4d87f8, block);
}

// @retail 0x89f20
void replication_node_start(s_node_450d1c *node, s_owner_450d1c *owner, dword mask)
{
	bool sent = false;
	node->unknown00 = 1;
	node->time = g_510548 ? g_51054c : GetTickCount();
	long i = 0;
	do
	{
		if (mask & (1 << i))
		{
			c_vtable_450d1c *sender = ((s_replication_sender_view *)owner)->senders[i];
			if (sender)
			{
				node->active_mask |= 1 << sender->player;
				sender->pending++;
				sent = true;
			}
		}
		i++;
	} while (i < 15);
	if (!sent)
	{
		owner->manager->v3(node);
		s_node_450d1c **link = &owner->head;
		if (*link)
		{
			do
			{
				s_node_450d1c *current = *link;
				if (current == node)
				{
					*link = node->next;
					break;
				}
				link = &current->next;
			} while (*link);
		}
		owner->count--;
		if (node->data)
			release_block(node->data);
		long info;
		if (!g_4d87f8->allocator->get_info(node, &info))
			info = NONE;
		s_allocator_globals *globals = g_4d87f8;
		globals->allocator->release(node, NONE);
		globals->count--;
	}
}

struct s_699a0_entry
{
	long state;
	long handle;
	byte unknown08[0x88];
};
struct s_699a0_table
{
	byte unknown00[0x8fc];
	s_699a0_entry entries[16];
};

// @retail 0x699a0
long function_699a0(void *table, long index)
{
	long handle = NONE;
	if (index < 16)
	{
		byte *record = datum_get_absolute_inlined(g_4e8c24, index);
		if (record)
			handle = *(long *)(record + 0x2c);
	}
	else if (index < 32)
	{
		long slot = index - 16;
		if (((s_699a0_table *)table)->entries[slot].state != NONE)
			handle = ((s_699a0_table *)table)->entries[slot].handle;
	}
	return handle;
}

// @retail 0x89e70
void function_89e70(s_owner_450d1c *owner, s_node_450d1c *node)
{
	s_node_450d1c **link = &owner->head;
	if (*link != 0)
	{
		do
		{
			s_node_450d1c *current = *link;
			if (current == node)
			{
				*link = node->next;
				break;
			}
			link = &current->next;
		}
		while (*link != 0);
	}
	owner->count--;
	if (node != 0)
		function_89eb0(node, 1);
}
/* frees a node and, when asked, the node itself */
#pragma optimize("g", off)
static __forceinline void function_89eb3(s_allocator_globals *arg_0, void *arg_1)
{
	arg_0->allocator->release(arg_1, NONE);
	if (arg_1)
		arg_0->count--;
}
#pragma optimize("", on)

static __forceinline void free_block(void *block)
{
	long local_0;
	g_4d87f8->allocator->get_info(block, &local_0);
	function_89eb3(g_4d87f8, block);
}

// @retail 0x89eb0
s_node_450d1c *function_89eb0(s_node_450d1c *node, long flags)
{
	if (node->data)
		free_block(node->data);
	if (flags & 1)
		free_block(node);
	return node;
}

/* adds a new node at the end of the owner's list */
// @retail 0x89df0
s_node_450d1c *function_89df0(s_owner_450d1c *owner)
{
	s_node_450d1c *node = (s_node_450d1c *)handle_allocate(sizeof(s_node_450d1c));
	if (node)
	{
		*(volatile long *)&node->unknown04 = NONE;
		*(volatile long *)&node->timeout = NONE;
		*(volatile long *)&node->unknown00 = 0;
		*(volatile long *)&node->unknown10 = NONE;
		*(volatile long *)&node->unknown14 = NONE;
		node->data = 0;
		node->size = 0;
		node->active_mask = 0;
		node->done_mask = 0;
		s_node_450d1c **link = &owner->head;
		while (*link)
			link = &(*link)->next;
		*link = node;
		node->next = 0;
		owner->count++;
		return node;
	}
	return 0;
}

/* gives a node a copy of some data */
// @retail 0x8b0a0
bool function_8b0a0(s_node_450d1c *node, long size, const void *source)
{
	void *block = handle_allocate(size);
	bool result = false;
	if (block)
	{
		node->size = size;
		node->data = block;
		memcpy(block, source, size);
		result = true;
	}
	return result;
}

struct s_view_iterator
{
	dword mask;
	long index;
};
bool world_next_view(c_class_6a600 *world, s_view_iterator *iterator, c_simulation_view **out);

struct s_event_distribution
{
	byte unknown00[8];
	c_class_6a600 *world;
	s_owner_450d1c *owner;
	s_handlers_450cb8 *definitions;
	s_datums_450cb8 *entities;
};

// @retail 0x8b4d0
void function_8b4d0(s_event_distribution *distribution, long type, long entity_count,
	const long *entities, dword machine_mask, long size, const void *data, long timeout)
{
	dword mask = 0;
	s_view_iterator iterator = { (dword)NONE, 0 };
	c_simulation_view *view;
	while (world_next_view(distribution->world, &iterator, &view))
	{
		if (view->established() && view->unknown1c >= 0 && view->unknown1c < 16 &&
			(machine_mask & (1 << view->unknown1c)))
			mask |= 1 << view->world_index;
	}
	if (mask)
	{
		s_node_450d1c *node = function_89df0(distribution->owner);
		if (node)
		{
			c_handler_450cb8 *definition = distribution->definitions->handlers[type];
			node->unknown04 = type;
			node->timeout = timeout;
			if (entity_count > 0 && entities[0] != NONE)
				node->unknown10 = entities[0];
			for (long i = 1; i < entity_count; i++)
			{
				if (entities[i] != NONE)
					(&node->unknown10)[i] = entities[i];
			}
			bool data_copied = true;
			if (size > 0)
				data_copied = function_8b0a0(node, size, data);
			if (data_copied)
			{
				if (definition->v4())
				{
					for (long i = 0; i < entity_count; i++)
					{
						if (entities[i] != NONE)
						{
							s_datum_450cb8 *datum = &distribution->entities->data[entities[i] & 0x3ff];
							datum->counter++;
						}
					}
				}
				replication_node_start(node, distribution->owner, mask);
			}
			else
			{
				function_89e70(distribution->owner, node);
			}
		}
	}
}


struct s_object_relevance_result
{
 real first;
 real second;
 long object_index;
 long identifier;
};
struct s_weapon_activity_result
{
 byte unknown00[0x18];
 bool active[2][2];
 bool update_relevance;
 byte unknown1d[3];
 s_object_relevance_result relevance;
 bool consumed[2];
};
void function_824d0(const byte *input, s_weapon_activity_result *output, long object_index);
void function_823d0(s_weapon_activity_result *output, const byte *input, long object_index);

// @retail 0x69b50
void function_69b50(c_class_6a600 *world, dword mask, const byte *input)
{
 long i = 0;
 const byte *cursor = input;
 s_simulation_world_actor *actor = world->actors;
 for (; i < 16; i++, cursor += 0x7c, actor++)
 {
  if (actor->actor_index != NONE && (mask & (1 << i)))
  {
   s_weapon_activity_result action;
   function_824d0(cursor, &action, actor->unknown04);
   long index = actor->actor_index + 16;
   s_view_iterator iterator = { 0x10, 0 };
   c_simulation_view *view;
   while (world_next_view(world, &iterator, &view))
   {
    byte *destination = (byte *)view->data + 0x5098;
    if (destination)
    {
     memcpy(destination + 0x18 + index * 0x34, &action, 0x34);
     *(dword *)(destination + 0x10) |= 1 << index;
    }
   }
  }
 }
}

/* parameters in 0x69b50's order: retail's caller sets the input up before
   both pushes, as the last parameter */
// @retail 0x69a00
void function_69a00(c_class_6a600 *world, dword mask, const byte *input)
{
 for (long i = 0; i < 16; i++)
 {
  s_simulation_world_player *player = &world->players[i];
  if (player->player_index != NONE && (mask & (1 << i)))
  {
   byte *datum = g_4e8c24->data + (player->unknown04 & 0xffff) * 0x21c;
   s_weapon_activity_result action;
   function_823d0(&action, input + i * 0x5c, *(long *)(datum + 0x2c));
   if (*(long *)(datum + 0x2c) != NONE)
   {
    long index = player->player_index;
    s_machine_address address = *(s_machine_address *)world->players[i].unknown18;
    s_view_iterator iterator = { 0x10, 0 };
    c_simulation_view *view;
    while (world_next_view(world, &iterator, &view))
    {
     s_machine_address other = view->address;
     if (memcmp(&other, &address, sizeof(address)))
     {
      byte *destination = (byte *)view->data + 0x5098;
      if (destination)
      {
       memcpy(destination + 0x18 + index * 0x34, &action, 0x34);
       *(dword *)(destination + 0x10) |= 1 << index;
      }
     }
    }
   }
  }
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
bool function_828b0(long player_index, s_player_object_motion *result);
void function_1947a0(s_bitstream *stream);

// @retail 0x847d0
void function_847d0(s_simulation_controller *controller, s_player_action *input)
{
 if (controller->field_08 != 1 && controller->field_08 != 2)
 {
  byte buffer[0x5c];
  s_bitstream stream;
  stream.data = buffer;
  stream.size_in_bytes = sizeof(buffer);
  stream.unknown08 = 1;
  stream.mode = 1;
  memset(buffer, 0, sizeof(buffer));
  stream.bit_position = 0;
  stream.checkpoint_count = 0;
  stream.error = false;
  stream.unknown2c = 0;
  stream.unknown30 = 0;
  function_86f90(&stream, input);
  long size = (stream.bit_position + 7) / 8;
  stream.size_in_bytes = size;
  long remainder = stream.size_in_bytes % stream.unknown08;
  if (remainder) stream.size_in_bytes += stream.unknown08 - remainder;
  stream.mode = 2;
  function_1947a0(&stream);
  function_874c0(&stream, &controller->action);
  controller->field_28 = g_510c54->game_time;
 }
 else
 {
  c_vtable_450c94 *destination = 0;
  c_simulation_view *view = 0;
  s_view_iterator iterator = { 0xa, 0 };
  world_next_view(controller->world, &iterator, &view);
  if (view) destination = (c_vtable_450c94 *)((byte *)view->data + 0x5098);
  controller->field_28 = g_510c54->game_time;
  controller->action = *input;
  s_weapon_activity_result action;
  function_823d0(&action, (const byte *)&controller->action,
   *(long *)(g_4e8c24->data + (controller->field_04 & 0xffff) * 0x21c + 0x2c));
  s_player_object_motion motion;
  bool valid = function_828b0(controller->field_04, &motion);
  if (destination)
  {
   long index = controller->field_00;
   memcpy(&destination->data18[index], &action, sizeof(action));
   destination->active_mask |= 1 << index;
   if (controller->field_08 == 1 && valid)
    destination->set_data720(index, (const s_dword40 *)&motion);
  }
 }
}


c_simulation_view *function_6acb0(c_class_6a600 *world);
c_simulation_view *function_6ad40(c_class_6a600 *world, const s_machine_address *address);
void function_825e0(const s_weapon_activity_result *input, s_player_action *action);
bool function_14d0a0(const s_player_action *action);
real g_4ced44;


