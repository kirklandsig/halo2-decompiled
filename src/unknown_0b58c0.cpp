#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "unknown_067e10.h"

// @flags /O2 /Gr /arch:SSE

/* local views of the simulation world, the object header data and the
   objects (the other files keep their own views of the same data) */
struct s_world_entry
{
	byte flag0 : 1;
	byte flag1 : 1;
	byte flag2 : 1;
	byte : 5;
	byte unknown01[7];
};

struct s_world_state
{
	byte unknown00[0x44];
	s_world_entry entries[1];
};

struct s_world_slot
{
	byte unknown00[0xc];
	dword flags;
	byte unknown10[0x10];
};

struct s_world_pool
{
	byte unknown00[0xc];
	s_world_state *state;
	byte unknown10[4];
	s_world_slot slots[1];
};

struct s_world_data
{
	byte unknown00[0x2098];
	s_world_pool pool;
};

struct s_world
{
	byte unknown00[4];
	s_world_data *data;
	long state;
};

struct c_entry_table;
struct s_handle_peers;
long entity_table_new_entity(c_entry_table *table, long handler_index);
bool entity_table_new_entities(long *identifiers, c_entry_table *table, long count, long const *handler_indices);
void replication_table_update(s_handle_peers *peers, long handle);
void replication_table_update_chain(s_handle_peers *peers, long handle);

// @retail 0xb57d0
long function_b57d0(long handler_index, long object_index)
{
	s_world *world = (s_world *)g_4cf77c;
	long state = world->state;
	long result = NONE;
	if (state == 4 || state == 5)
	{
		if (state != 3 && state != 5)
		{
			s_world_pool *pool = &world->data->pool;
			result = entity_table_new_entity((c_entry_table *)pool, handler_index);
			if (result != NONE)
				*(long *)((byte *)&pool->slots[result & 0x3ff] + 8) = object_index;
		}
	}
	return result;
}

// @retail 0xb5830
bool function_b5830(long *identifiers, long count, long const *handler_indices, long const *object_indices)
{
	bool result = false;
	s_world *world = (s_world *)g_4cf77c;
	long state = world->state;
	if (state == 4 || state == 5)
	{
		if (state != 3 && state != 5)
		{
			s_world_pool *pool = &world->data->pool;
			if (entity_table_new_entities(identifiers, (c_entry_table *)pool, count, handler_indices))
			{
				for (long i = 0; i < count; i++)
					*(long *)((byte *)&pool->slots[identifiers[i] & 0x3ff] + 8) = object_indices[i];
				result = true;
			}
		}
	}
	return result;
}

/* object_index is the object the entity was made for (b57d0's object_index);
   this build doesn't read it, and LTCG drops the argument, but 185a30 passing
   its loop index keeps retail's element-index loop there */
// @retail 0xb5920
void function_b5920(long identifier, long object_index)
{
	s_world *world = (s_world *)g_4cf77c;
	long state = world->state;
	if (state == 4 || state == 5)
	{
		s_world_pool *pool = &world->data->pool;
		long index = identifier & 0x3ff;
		s_world_state *saved_state = pool->state;
		s_world_slot *slot = &pool->slots[index];
		byte flags = *(byte *)&saved_state->entries[index];
		*((byte *)slot + 6) = 0;
		*(long *)((byte *)slot + 8) = NONE;
		if (flags & 4)
		{
			s_world_state *peers = pool->state;
			byte current_flags = *(byte *)&peers->entries[index];
			if (current_flags & 8)
				replication_table_update_chain((s_handle_peers *)peers, identifier);
			else if (!(current_flags & 0x10))
				replication_table_update((s_handle_peers *)peers, identifier);
		}
	}
}

struct s_object
{
	byte unknown00[0x14];
	long parent_index;
	char parent_node;
	byte unknown19[0x64 - 0x19];
	point3f position;
	byte unknown70[0x116 - 0x70];
	short nodes_offset;
};

struct s_object_header
{
	short identifier;
	union
	{
		byte flags;
		struct
		{
			byte flag0 : 1;
			byte flag1 : 1;
			byte : 6;
		};
	};
	byte type;
	byte unknown04[4];
	s_object *object;
};

// @retail 0xb58c0
void function_b58c0(long index, dword mask)
{
	long const *index_reference = &index;
	s_world *world = (s_world *)g_4cf77c;
	long state = world->state;

	if (state == 4 || state == 5)
	{
		if (state != 3 && state != 5)
		{
			s_world_pool *pool = &world->data->pool;

			if (pool->state->entries[*index_reference & 0x3ff].flag2)
			{
				s_world_slot *entry = &pool->slots[*index_reference & 0x3ff];
				entry->flags = entry->flags | mask;
			}
		}
	}
}

// @retail 0xb7360
void function_b7360(long object_index)
{
	s_record_pool *data = g_4e0300;
	s_object_header *headers = (s_object_header *)data->data;
	s_object_header *header = &headers[object_index & 0xffff];

	if (!(header->flags & 2))
	{
		while (object_index != NONE)
		{
			header = &headers[object_index & 0xffff];
			header->flag1 = 1;
			headers = (s_object_header *)data->data;
			object_index = headers[object_index & 0xffff].object->parent_index;
		}
	}
}

PRIVATE __forceinline point3f *transform_parent_point(transform4x3f const *matrix, point3f const *point, point3f *result)
{
    real x = point->x;
    real y = point->y;
    real z = point->z;
    if (matrix->scale != 1.0f)
    {
        x = matrix->scale * x;
        y = matrix->scale * y;
        z = matrix->scale * z;
    }
    result->x = matrix->rotation.up.i * z + matrix->rotation.left.i * y + matrix->rotation.forward.i * x + matrix->position.x;
    result->y = matrix->rotation.up.j * z + matrix->rotation.left.j * y + matrix->rotation.forward.j * x + matrix->position.y;
    result->z = matrix->rotation.up.k * z + matrix->rotation.left.k * y + matrix->rotation.forward.k * x + matrix->position.z;
    return result;
}

// @retail 0xb9dd0
point3f *function_b9dd0(long object_index, point3f *result)
{
    s_object_header *headers = (s_object_header *)g_4e0300->data;
    s_object *object = headers[object_index & 0xffff].object;
    if (object->parent_index == NONE)
    {
        *result = object->position;
        return result;
    }
    s_object *parent = headers[object->parent_index & 0xffff].object;
    transform4x3f *matrix = (transform4x3f *)((byte *)parent + parent->nodes_offset) + object->parent_node;
    return transform_parent_point(matrix, &object->position, result);
}


struct s_entity_definition_ab
{
    short kind;
    byte unknown02[0x38 - 2];
    long model_index;
};
struct s_entity_model_ab
{
    byte unknown00[0x60];
    long count;
    byte *entries;
};

// @retail 0xb5990
long function_b5990(long tag_index, bool flag)
{
    s_entity_definition_ab *definition = (s_entity_definition_ab *)g_4e3b44[tag_index & 0xffff].bytes;
    long result = NONE;
    switch (definition->kind)
    {
    case 0: result = 9; break;
    case 2:
        if (flag) result = 14;
        break;
    case 1: result = flag ? 12 : 15; break;
    case 3: result = 10; break;
    case 4: result = 10; break;
    case 5: result = 13; break;
    case 7: result = 16; break;
    case 8: result = 16; break;
    case 6:
        if (definition->model_index != NONE)
        {
            s_entity_model_ab *model = (s_entity_model_ab *)g_4e3b44[definition->model_index & 0xffff].bytes;
            if (model->count > 0 && (*(long *)(model->entries + 0xbc) > 0 || *(long *)(model->entries + 0xe0) > 0))
                goto model_kind;
        }
        break;

    case 9: break;
    case 10: break;
    case 11:
model_kind:
        result = 11;
        break;
    }
    return result;
}

struct s_event_distribution;
long function_a5930(long index);
long function_a5980(long index);
long simulation_watcher_find_machine(s_simulation_world_owner const *watcher, s_machine_address const *address);
void function_8b4d0(s_event_distribution *distribution, long type, long entity_count,
    long const *entities, dword machine_mask, long size, void const *data, long timeout);

// distributes an event about the given objects to every machine except the one of a player
// @retail 0xb5a70
void __stdcall function_b5a70(long player_index, long type, long count, long object_indices,
    long size, void const *data, long timeout)
{
    long entities[2];
    s_world *world = (s_world *)g_4cf77c;
    long state = world->state;
    if (state == 4 || state == 5)
    {
        s_event_distribution *distribution = (s_event_distribution *)((byte *)world->data + 0xa0ac);
        for (long i = 0; i < count; i++)
        {
            if (state != 3 && state != 5)
                entities[i] = function_a5930(((long const *)object_indices)[i]);
            else
                entities[i] = function_a5980(((long const *)object_indices)[i]);
        }
        if (count <= 0 || entities[0] != NONE)
        {
            dword mask = NONE;
            if (state != 3 && state != 5 && player_index != NONE)
            {
                byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
                if (*(short *)(player + 0x1a) != NONE)
                {
                    s_machine_address const *address = (s_machine_address const *)((byte *)g_4e8c20 + 0x30) + *(short *)(player + 0x1a);
                    long index = simulation_watcher_find_machine(*(s_simulation_world_owner **)g_4cf77c, address);
                    if (index != NONE)
                        mask = ~(1 << index);
                }
            }
            function_8b4d0(distribution, type, count, count > 0 ? entities : 0, mask, size, data, timeout);
        }
    }
}

// distributes an event about the given objects to the machine of one player
// @retail 0xb5ba0
void __stdcall function_b5ba0(long player_index, long type, long count, long object_indices,
    long size, void const *data, long timeout)
{
    long entities[2];
    s_world *world = (s_world *)g_4cf77c;
    long state = world->state;
    if (state == 4 || state == 5)
    {
        s_event_distribution *distribution = (s_event_distribution *)((byte *)world->data + 0xa0ac);
        for (long i = 0; i < count; i++)
        {
            if (state != 3 && state != 5)
                entities[i] = function_a5930(((long const *)object_indices)[i]);
            else
                entities[i] = function_a5980(((long const *)object_indices)[i]);
        }
        if (count <= 0 || entities[0] != NONE)
        {
            if (state != 3 && state != 5 && player_index != NONE)
            {
                byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
                if (*(short *)(player + 0x1a) != NONE)
                {
                    s_machine_address const *address = (s_machine_address const *)((byte *)g_4e8c20 + 0x30) + *(short *)(player + 0x1a);
                    long index = simulation_watcher_find_machine(*(s_simulation_world_owner **)g_4cf77c, address);
                    if (index != NONE)
                        function_8b4d0(distribution, type, count, count > 0 ? entities : 0,
                            1 << index, size, data, timeout);
                }
            }
        }
    }
}
