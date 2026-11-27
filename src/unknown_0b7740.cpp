#include "unknown_0b68c0.h"
#include "object_tag_definitions.h"
#include "unknown_11c920.h"
#include "bit_vector.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include "placement_identifier.h"
#include <string.h>
#include <xmmintrin.h>

// @flags /O2 /Ob1 /arch:SSE /Gr
#include "object_masks_storage.h"
#include "slot_handler.h"
#include "sample_mask_storage.h"
#include "matrices_storage.h"
#include "orientations_storage.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"

struct s_object_transform_view
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	char parent_node;
	byte unknown019[0x64 - 0x19];
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown088[0x116 - 0x88];
	short node_matrices_offset;
};

struct s_object_transform_header
{
	byte unknown00[8];
	s_object_transform_view *object;
};

__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position,
	vector3f const *forward, vector3f const *up);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b,
	transform4x3f *result);

inline transform4x3f *function_ba160(long object_index, transform4x3f *matrix);

// @retail 0xba160
transform4x3f *function_ba160(long object_index, transform4x3f *matrix)
{
	transform4x3f *const *matrix_reference = &matrix;
	s_record_pool *objects = g_4e0300;
	s_object_transform_view *object = ((s_object_transform_header *)objects->data)[object_index & 0xffff].object;

	function_1420f0(*matrix_reference, &object->position, &object->forward, &object->up);
	if (object->parent_index != NONE)
	{
		s_object_transform_view *parent = ((s_object_transform_header *)objects->data)[object->parent_index & 0xffff].object;
		transform4x3f *nodes = (transform4x3f *)((byte *)parent + parent->node_matrices_offset);

		function_142a60(&nodes[object->parent_node], matrix, matrix);
	}
	return matrix;
}

struct s_velocity_object
{
	byte unknown000[0x88];
	vector3f linear_velocity;
	vector3f angular_velocity;
	byte unknown0a0[0xd4 - 0xa0];
	long synchronization_index;
};

struct s_velocity_object_header
{
	byte unknown00[8];
	s_velocity_object *object;
};

void function_b58c0(long index, dword mask);

static __forceinline dword object_velocity_write(s_velocity_object *object, vector3f const *linear, vector3f const *angular)
{
    dword mask = 0;
    if (linear)
    {
        object->linear_velocity = *linear;
        mask |= 0x10;
    }
    if (angular)
    {
        object->angular_velocity = *angular;
        mask |= 0x20;
    }
    return mask;
}

// @retail 0xb7740
void function_b7740(long object_index, vector3f const *linear_velocity,
	vector3f const *angular_velocity, bool skip_update)
{
	object_index &= 0xffff;
	s_record_pool *objects = g_4e0300;
	byte *volatile const *header_data = &objects->data;
    s_velocity_object *object = ((s_velocity_object_header *)*header_data)[object_index].object;
	dword update_mask = object_velocity_write(object, linear_velocity, angular_velocity);
	if (!skip_update && update_mask)
	{
		long index = ((s_velocity_object_header *)*header_data)[object_index].object->synchronization_index;
		if (index != NONE)
			function_b58c0(index, update_mask);
	}
}

struct s_object_list;
extern s_object_list *g_4de2f4;

// @retail 0xb8820
long function_b8820()
{
	if (g_4de2f4 && *(byte *)g_4de2f4)
		return 1;
	return 0;
}

struct s_havok_component;
void function_1d1260(s_havok_component *component);
void function_1d1540(s_havok_component *component);

// @retail 0xb8840
void function_b8840(long object_index)
{
	byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
	long component_index = *(long *)(object + 0xb4);
	if (component_index != NONE)
	{
		s_havok_component *component = (s_havok_component *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0);
		function_1d1260(component);
	}
	object[0xc0] &= ~0x40;
}

// @retail 0xb8890
void __stdcall function_b8890(long object_index)
{
	long const *index_reference = &object_index;
	byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[*index_reference & 0xffff].object;
	long component_index = *(long *)(object + 0xb4);
	if (component_index != NONE)
	{
		s_havok_component *component = (s_havok_component *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0);
		function_1d1540(component);
	}
	object[0xc0] |= 0x40;
}

struct s_object_named_value
{
	byte unknown00[8];
	long value;
	byte unknown0c[0x18 - 0xc];
};

struct s_object_named_values
{
	byte unknown00[0x94];
	long count;
	s_object_named_value *entries;
};

// @retail 0xb8c40
long function_b8c40(long object_index, short entry_index)
{
	long result = NONE;
	s_object_transform_view *object = ((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_object_named_values *definition = (s_object_named_values *)g_4e3b44[object->definition_index & 0xffff].bytes;
	if (entry_index >= 0 && entry_index < definition->count)
		result = definition->entries[entry_index].value;
	return result;
}

struct s_object_link_owner
{
	byte unknown00[8];
	s_record_pool *references;
};

struct s_object_link_iterator
{
	s_object_link_owner *owner;
	long index;
};

struct s_object_link_entry
{
	byte unknown00[4];
	short value;
	byte unknown06[2];
	long next;
};

// @retail 0xb8b20
short function_b8b20(s_object_link_iterator *iterator)
{
	short result;
	s_record_pool *references = iterator->owner->references;
	if (iterator->index != NONE)
	{
		long size = references->size;
		byte *data = references->data;
		s_object_link_entry *entry = (s_object_link_entry *)(data + (iterator->index & 0xffff) * size);
		long next = entry->next;
		if (next != NONE)
			_mm_prefetch((char const *)(data + (next & 0xffff) * size), _MM_HINT_T0);
		iterator->index = next;
		result = entry->value;
	}
	else
	{
		result = NONE;
	}
	return result;
}

struct s_object_partition_record_ab
{
	short type;
	word flags;
	point3f centre;
	real radius;
};

struct s_object_partition_view_ab
{
	long tag_index;
	dword flags;
	byte unknown08[0x40 - 8];
	point3f centre;
	real radius;
	byte unknown50[0xaa - 0x50];
	char type;
};

struct s_object_partition_header_ab
{
	byte unknown00[8];
	s_object_partition_view_ab *object;
};

word collision_object_flags(long object_index);

// @retail 0xb88e0
void function_b88e0(long object_index, s_object_partition_record_ab *record)
{
	s_object_partition_view_ab *object = ((s_object_partition_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
	word flags = 0;
	if ((bool)((object->flags >> 9) & 1))
		flags = collision_object_flags(object_index);
	record->type = object->type;
	record->flags = flags;
	record->centre = object->centre;
	record->radius = object->radius;
}





void function_1cadf0(s_cluster_partition *partition, long reference_index, long data_index, long payload_size, void const *payload);

inline void function_b8b70(long object_index);

// @retail 0xb8b70
void function_b8b70(long object_index)
{
    s_object_partition_header_ab *header = &((s_object_partition_header_ab *)g_4e0300->data)[object_index & 0xffff];
    s_object_partition_view_ab *object = header->object;
    if ((*((byte *)header + 2) & 0x40) && (bool)((object->flags >> 9) & 1))
    {
        s_object_partition_record_ab record;
        function_b88e0(object_index, &record);
        // The existing whole partition storage has the same three-pointer layout.
        function_1cadf0((s_cluster_partition *)&g_partition_colliding_storage,
            *(long *)((byte *)object + 0x60), object_index, sizeof(record), &record);
    }
}

struct s_partition_object_link_ab
{
	long salt;
	long object_index;
	long next;
	s_object_partition_record_ab record;
};

struct s_object_cluster_reference
{
	byte type;
	byte unknown01;
	word flags;
	point3f center;
	real radius;
};

struct s_object_cluster_iterator
{
	long next;
};

static __forceinline long object_cluster_next_ab(s_record_pool *array, s_object_cluster_iterator *iterator, s_object_cluster_reference **record)
{
	long result;
	if (iterator->next != NONE)
	{
		long *size = &array->size;
		byte **data = &array->data;
		s_partition_object_link_ab *link = (s_partition_object_link_ab *)(*data + (iterator->next & 0xffff) * *size);
		long next = link->next;
		*record = (s_object_cluster_reference *)&link->record;
		if (next != NONE)
			_mm_prefetch((char const *)(*data + (next & 0xffff) * *size), _MM_HINT_T0);
		iterator->next = next;
		result = link->object_index;
	}
	else
	{
		result = NONE;
	}
	return result;
}

// @retail 0xb8940
long function_b8940(long cluster_index, s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
	iterator->next = ((long *)g_4de2e0)[(short)cluster_index];
	return object_cluster_next_ab((s_record_pool *)g_4de2e4, iterator, record);
}

// @retail 0xb89b0
long function_b89b0(s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
    return object_cluster_next_ab((s_record_pool *)g_4de2e4, iterator, record);
}

// @retail 0xb8a10
long function_b8a10(long cluster_index, s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
	iterator->next = ((long *)g_4de2d4)[(short)cluster_index];
	return object_cluster_next_ab((s_record_pool *)g_4de2d8, iterator, record);
}

struct s_placement_defaults_ab
{
	long tag_index;
	s_scenario_identifier_ab identifier;
	long field_0c;
	long scenario_index;
	byte bsp_policy;
	byte unknown15[3];
	dword flags;
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
	real scale;
	long player_index;
	long object_index;
	long team;
	s_effect_owner owner;
	long field_74;
	byte unknown78[0xb4 - 0x78];
	short field_b4;
	byte unknownb6[2];
	byte field_b8;
	byte unknownb9[0xc4 - 0xb9];
};

struct s_placement_source_ab
{
	byte unknown00[0xaa];
	byte type;
	byte unknownab[0xc0 - 0xab];
	word flags_c0;
	byte unknownc2[0x138 - 0xc2];
	short team;
	byte unknown13a[2];
	long player_index;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

// @retail 0xb7930
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner)
{
	s_placement_defaults_ab *placement = (s_placement_defaults_ab *)data;
	memset(placement, 0, sizeof(*placement));
	placement->tag_index = tag_index;
	placement->field_0c = 0;
	memset(&placement->flags, 0, sizeof(placement->flags));
	placement->forward = *g_4687a8;
	placement->up = *g_4687b0;
	placement->scale = 1.0f;
	placement->field_74 = 0;
	short *unknown_b4 = &placement->field_b4;
	*unknown_b4 = NONE;
	placement->bsp_policy = 0;
	s_placement_source_ab *object = (s_placement_source_ab *)function_badc0(object_index, NONE);
	placement->team = NONE;
	placement->player_index = NONE;
	if (object)
	{
		placement->object_index = object_index;
		SET_FLAG(placement->flags, 5, (bool)((object->flags_c0 >> 2) & 1));
		if ((1 << object->type) & 3)
		{
			placement->player_index = object->player_index;
			placement->team = object->team;
		}
	}
	else
		placement->object_index = NONE;
	if (owner)
		placement->owner = *owner;
	else
	{
		placement->owner.unknown4 = NONE;
		placement->owner.unknown0 = NONE;
		placement->owner.unknown8 = NONE;
	}
	s_scenario_identifier_ab *identifier = &placement->identifier;
	identifier->type = NONE;
	identifier->source = NONE;
	identifier->origin_bsp = NONE;
	identifier->unique_id = NONE;
	placement->scenario_index = NONE;
	placement->field_b8 = 0;
}

struct s_scenario_type_ab
{
	byte unknown00[0xa];
	short block_offset;
	short palette_offset;
	short element_size;
};

struct s_scenario_block_ab
{
	long count;
	byte *elements;
};

#define SCENARIO_TYPE_AB(type) ((s_scenario_type_ab *)g_468630[type])

// @retail 0xb7a40
void *__stdcall function_b7a40(s_scenario_identifier_ab const *identifier, long *index_out)
{
	char source = identifier->source;
	void *result = NULL;
	long source_kind = source;
	if (source_kind == 1 || source_kind == 0)
	{
		char type = identifier->type;
		if (SCENARIO_TYPE_AB(type)->block_offset != NONE)
		{
			long size = SCENARIO_TYPE_AB(type)->element_size;
			s_scenario_block_ab *block = (s_scenario_block_ab *)((byte *)g_4e0350 + SCENARIO_TYPE_AB(type)->block_offset);
			for (long i = 0; i < block->count; i++)
			{
				byte *element = block->elements + i * size;
				s_scenario_identifier_ab *current = (s_scenario_identifier_ab *)(element + 0x28);
				if (placement_identifier_equal(current, identifier))
				{
					if (index_out)
						*index_out = i;
					result = element;
					break;
				}
			}
		}
	}
	return result;
}

static inline void identity_transform_ab(transform4x3f *matrix)
{
    matrix->scale = 1.0f;
    matrix->forward.i = 1.0f;
    matrix->forward.j = 0.0f;
    matrix->forward.k = 0.0f;
    matrix->left.i = 0.0f;
    matrix->left.j = 1.0f;
    matrix->left.k = 0.0f;
    matrix->up.i = 0.0f;
    matrix->up.j = 0.0f;
    matrix->up.k = 1.0f;
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
}

// @retail 0xbdc40
void function_bdc40(point3f const *position, vector3f const *forward, vector3f const *up, real scale, bool mirrored, transform4x3f const *parent, bool parent_mirrored, transform4x3f *out)
{
    transform4x3f translation;
    transform4x3f rotation;
    transform4x3f temporary;
    identity_transform_ab(&translation);
    translation.position = *position;
    rotation.scale = 1.0f;
    rotation.forward = *forward;
    vector3f const *f = forward; /* the first row reads the forward axis through its own pointer */
    rotation.left.i = up->j * f->k - f->j * up->k;
    rotation.left.j = forward->i * up->k - up->i * forward->k;
    rotation.left.k = forward->j * up->i - forward->i * up->j;
    rotation.up = *up;
    rotation.position.x = 0.0f;
    rotation.position.y = 0.0f;
    rotation.position.z = 0.0f;
    if (mirrored)
    {
        rotation.left.i = 0.0f - rotation.left.i;
        rotation.left.j = 0.0f - rotation.left.j;
        rotation.left.k = 0.0f - rotation.left.k;
    }
    if (scale != 1.0f)
    {
        temporary.scale = scale;
        temporary.forward.i = 1.0f;
        temporary.forward.j = 0.0f;
        temporary.forward.k = 0.0f;
        temporary.left.i = 0.0f;
        temporary.left.j = 1.0f;
        temporary.left.k = 0.0f;
        temporary.up.i = 0.0f;
        temporary.up.j = 0.0f;
        temporary.up.k = 1.0f;
        temporary.position.x = 0.0f;
        temporary.position.y = 0.0f;
        temporary.position.z = 0.0f;
        function_142a60(&rotation, &temporary, &rotation);
    }
    if (parent)
    {
        if (parent->scale != 1.0f || parent_mirrored)
        {
            temporary = *parent;
            if (temporary.scale != 1.0f)
            {
                translation.position.x *= temporary.scale;
                translation.position.y *= temporary.scale;
                translation.position.z *= temporary.scale;
                temporary.scale = 1.0f;
            }
            if (parent_mirrored)
            {
                temporary.left.i = 0.0f - temporary.left.i;
                temporary.left.j = 0.0f - temporary.left.j;
                temporary.left.k = 0.0f - temporary.left.k;
            }
            parent = &temporary;
        }
        function_142a60(parent, &translation, out);
        function_142a60(out, &rotation, out);
    }
    else
        function_142a60(&translation, &rotation, out);
}

#include "cluster_marks_storage.h"
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, short maximum_count, short *clusters);

/* collects up to maximum_count objects of the masked types whose bounding
   spheres meet the sphere at position: the colliding and the noncolliding
   cluster lists of every cluster the sphere reaches, each object once */
// @retail 0xbb050
short __stdcall function_bb050(long mask, dword type_mask, void const *location, point3f const *position, real radius, long *objects, short maximum_count)
{
    short count = 0;
    if (!type_mask) type_mask = 0xffffffff;
    if (!mask) mask = NONE;
    short cluster = *(short *)((byte const *)location + 4);
    short cluster_count = 0;
    short clusters[0x200];
    if (cluster != NONE)
    {
        if (radius > 0.0f)
        {
            ++g_4e7414;
            g_4e7411 = true;
            cluster_count = function_14a5b0(cluster, position, radius, 0x200, clusters);
            g_4e7411 = false;
        }
        else
        {
            cluster_count = 1;
            clusters[0] = cluster;
        }
    }
    if (cluster_count > 0x200) cluster_count = 0x200;
    ++g_4de2fc;
    g_4de2f8 = true;
    for (short i = 0; i < cluster_count; ++i)
    {
        short cluster = clusters[i];
        s_object_cluster_iterator colliding_iterator;
        s_object_cluster_iterator noncolliding_iterator;
        if (mask & 1)
        {
            s_object_cluster_reference *record = 0;
            for (long object = function_b8940(cluster, &record, &colliding_iterator); object != NONE;
                 object = object_cluster_next_ab((s_record_pool *)g_4de2e4, &colliding_iterator, &record))
            {
                if (!(type_mask & (1 << record->type))) continue;
                long index = object & 0xffff;
                if (g_4de300[index] == g_4de2fc) continue;
                g_4de300[index] = g_4de2fc;
                real combined = record->radius + radius;
                real dx = record->center.x - position->x;
                real dy = record->center.y - position->y;
                real dz = record->center.z - position->z;
                if (dz * dz + dy * dy + dx * dx <= combined * combined)
                {
                    if (count >= maximum_count)
                    {
                        g_4de2f8 = false;
                        return count;
                    }
                    objects[count] = object;
                    ++count;
                }
            }
        }
        if (mask & 2)
        {
            s_object_cluster_reference *record = 0;
            for (long object = function_b8a10(cluster, &record, &noncolliding_iterator); object != NONE;
                 object = object_cluster_next_ab((s_record_pool *)g_4de2d8, &noncolliding_iterator, &record))
            {
                if (!(type_mask & (1 << record->type))) continue;
                long index = object & 0xffff;
                if (g_4de300[index] == g_4de2fc) continue;
                g_4de300[index] = g_4de2fc;
                real combined = record->radius + radius;
                vector3f delta;
                vector3d_from_points3d(position, &record->center, &delta);
                if (length_sq3f(&delta) <= combined * combined)
                {
                    if (count >= maximum_count)
                    {
                        g_4de2f8 = false;
                        return count;
                    }
                    objects[count] = object;
                    ++count;
                }
            }
        }
    }
    g_4de2f8 = false;
    return count;
}

#include "unknown_1cafc0.h"
#include "channel_start_out_of_line.h"
#include <math.h>
struct s_16760c_render_model;
bool __stdcall function_bab40(long object_index, long name, real *value);

/* a walk over the graph's block at +0x44 (the shape of the block +0x3c
   iterator s_graph_iterator3c): the current element's animation, the name
   its value is read under and how that value is applied */
struct s_graph_iterator44
{
	c_type_709360 animation_id;
	long value_name;
	short kind;
	short index;
	short next_index;
};

/* samples each animation of the state's graph block +0x44 into the
   orientations, positioned by an object value or by the game time */
// @retail 0xbd970 standard
void __stdcall function_bd970(long object_index, s_16760c_render_model *render_model, s_animation_state *state, long node_mask, long node_count, byte *orientations)
{
    s_graph_iterator44 iterator;
    iterator.next_index = NONE;
    c_animation_channel channel;
    for (;;)
    {
        s_graph_tag *graph = graph_tag_get(state->graph_tag_index);
        short index = iterator.next_index + 1;
        if (index >= graph->unknown44_count)
            break;
        iterator.next_index = index;
        s_graph_element44 *entry = &graph->unknown44[index];
        iterator.animation_id = entry->animation_id;
        iterator.value_name = *(long *)(entry->unknown08 + 4);
        iterator.kind = *(short *)(entry->unknown08 + 2);
        if (iterator.animation_id.index != NONE)
            function_1dd9d0(graph, iterator.animation_id);
        if (iterator.value_name && state->graph_tag_index != NONE &&
            channel_start_out_of_line(state, &channel, iterator.animation_id, NONE, NONE, NONE, NONE, 0x7f))
        {
            s_animation *animation = 0;
            if (channel.animation_id.index != NONE)
                animation = function_1daea0(graph_tag_get(channel.graph_tag_index), channel.animation_id);
            real value;
            function_bab40(object_index, iterator.value_name, &value);
            if (iterator.kind == 0)
            {
                channel.set_frame_position((animation->frame_count - 1) * value);
                channel.sample(1.0f, (dword const *)node_mask, node_count, (real_quaternion_transform *)orientations);
            }
            else if (iterator.kind == 1)
            {
                real frame = fmod((real)(dword)(g_510c54->game_time + object_index) * g_510c54->rate * 0.03333333507180214f, (real)animation->frame_count);
                channel.set_frame_position(frame);
                channel.sample(value, (dword const *)node_mask, node_count, (real_quaternion_transform *)orientations);
            }
        }
    }
}


transform4x3f *function_b8bd0(long object_index, short node_index);
void function_141590(transform4x3f const *in, transform4x3f *out);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void orientation_from_matrix4x3(transform4x3f const *matrix, rigid_transform_scaled *out);

// @retail 0xbfa40
void function_bfa40(long object_index, long node_mask)
{
    byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
    if (*(short *)(object + 0x112) == NONE)
        return;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long model_index = *(long *)(definition + 0x38);
    if (model_index == NONE)
        return;
    byte *model = g_4e3b44[model_index & 0xffff].bytes;
    if (*(long *)(model + 4) == NONE)
        return;
    transform4x3f *parent = 0;
    bool parent_mirrored = false;
    transform4x3f *matrices = (transform4x3f *)(object + *(short *)(object + 0x116));
    rigid_transform_scaled *orientations = (rigid_transform_scaled *)(object + *(short volatile *)(object + 0x112));
    long count = (dword)(long)*(short *)(object + 0x110) / sizeof(rigid_transform_scaled);
    long parent_index = *(long *)(object + 0x14);
    if (parent_index != NONE)
    {
        parent = function_b8bd0(parent_index, *(char *)(object + 0x18));
        byte *local_be682a = (byte *)((s_object_transform_header *)g_4e0300->data)[parent_index & 0xffff].object;
        parent_mirrored = (*(dword *)(local_be682a + 4) >> 10) & 1;
    }
    transform4x3f relative, root, inverse_parent, inverse_root;
    function_bdc40((point3f *)(object + 0x64), (vector3f *)(object + 0x70), (vector3f *)(object + 0x7c),
        *(real *)(object + 0xa0), (*(dword *)(object + 4) >> 10) & 1, parent, parent_mirrored, &root);
    function_141590(&root, &inverse_root);
    for (long i = 0; i < count; ++i)
    {
        if (!node_mask || (((dword *)node_mask)[i >> 5] & (1 << (i & 31))))
        {
            short node_parent = *(short *)(*(byte **)(model + 0x7c) + i * 0x5c + 4);
            if ((dword)(long)node_parent != (dword)NONE)
            {
                function_141590(&matrices[node_parent], &inverse_parent);
                function_142a60(&inverse_parent, &matrices[i], &relative);
                function_141f60(&relative.rotation, &orientations[i].rotation);
                orientations[i].position = relative.position;
                orientations[i].scale = relative.scale;
            }
            else
            {
                function_142a60(&inverse_root, &matrices[i], &relative);
                orientation_from_matrix4x3(&relative, &orientations[i]);
            }
        }
    }
}

long bit_vector_highest_set_bit(dword const *bits, long bit_count);
void object_dispatch_adjust_node_orientations(long object_index, long a, long b, long c);
struct s_1d9240;
void function_1d9470(s_1d9240 const *p, long count, dword const *mask, s_blend_orientation const *targets,
    s_blend_orientation *orientations);

// @retail 0xbdb60
void function_bdb60(long object_index, s_16760c_render_model *render_model, s_animation_state *state,
    long node_mask, long node_count, byte *orientations)
{
    if (node_mask)
    {
        long highest = bit_vector_highest_set_bit((dword const *)node_mask, node_count);
        if (highest >= 0 && highest + 1 < node_count)
            node_count = highest + 1;
    }
    if (node_count)
    {
        c_animation_channel *channel = &state->channels[0];
        if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
        {
            function_1daea0(graph_tag_get(channel->graph_tag_index), channel->animation_id);
            state->sample((long)render_model, 1.0f, (dword const *)node_mask,
                (real_quaternion_transform *)orientations, 0, object_index, node_count);
        }
        function_bd970(object_index, render_model, state, node_mask, node_count, orientations);
        object_dispatch_adjust_node_orientations(object_index, node_mask, node_count, (long)orientations);
        if (state->unknown64.unknown1 && !(state->unknown64.unknown3 & 2))
        {
            byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
            s_blend_orientation *targets = (s_blend_orientation *)(object + *(short *)(object + 0x10e));
            function_1d9470((s_1d9240 const *)&state->unknown64, (short)node_count, (dword const *)node_mask, targets,
                (s_blend_orientation *)orientations);
        }
    }
}


bool __stdcall function_1c4c50(long object_index, long node_index, point3f const *point,
    vector3f const *change_a, vector3f const *change_b, long *result_object,
    vector3f *linear, vector3f *angular);
void function_b7360(long object_index);
void function_bba20(long object_index);

// @retail 0xb7880
void __stdcall function_b7880(long object_index, long node_index, point3f const *point,
    vector3f const *impulse, vector3f const *angular_impulse)
{
    long result_object;
    vector3f linear, angular;
    if (function_1c4c50(object_index, node_index, point, impulse, angular_impulse,
        &result_object, &linear, &angular))
    {
        s_velocity_object *object = ((s_velocity_object_header *)g_4e0300->data)[result_object & 0xffff].object;
        *((byte *)object + 0xc1) &= ~1;
        function_b7360(result_object);
        function_bba20(result_object);
        object->linear_velocity = linear;
        object->angular_velocity = angular;
    }
}


struct s_location;
void object_dispatch_moved(long object_index, long position, long forward, long up);
bool __stdcall function_b7430(long object_index, point3f const *position, vector3f const *forward, vector3f const *up,
    s_location const *location, bool a, bool b, bool c, bool d);
void __stdcall function_b8540(long object_index);
void function_b7360(long object_index);

// @retail 0xb75a0
void function_b75a0(long object_index, point3f const *point, vector3f const *forward, vector3f const *up,
    s_location const *location, bool unknown)
{
    point3f saved_point;
    vector3f saved_forward, saved_up;
    point3f *position_copy = 0;
    vector3f *forward_copy = 0;
    vector3f *up_copy = 0;
    if (point)
    {
        saved_point = *point;
        position_copy = &saved_point;
    }
    if (forward)
    {
        saved_forward = *forward;
        forward_copy = &saved_forward;
    }
    if (up)
    {
        saved_up = *up;
        up_copy = &saved_up;
    }
    object_dispatch_moved(object_index, (long)position_copy, (long)forward_copy, (long)up_copy);
    if (!function_b7430(object_index, position_copy, forward_copy, up_copy, location, true, true, unknown, false))
    {
        bool attached = false;
        if (g_4e6948->mode == 4)
        {
            byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
            attached = *(long *)(object + 0xd4) != NONE;
        }
        if (!attached)
            function_b8540(object_index);
    }
    function_b7360(object_index);
}


bool function_0bfe60(dword const *mask, long node);
bool unit_animation_periodic_layer_active(long object_index);
bool function_10edd0(long object_index);
void function_10ec00(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
void function_10e530(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
void function_1132f0(long object_index, dword const *node_mask, long count, transform4x3f *matrices);
void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);


/* samples and overlays an object's node orientations and rebuilds its matrices, when it has any */
// Single exit; the awake test through 0xbfc90 and the bit-30 clear through 0xbfe20; the bit-29 and +0x34b tests
// read the object again through its header, as retail does; `result` ahead of the header, `model` ahead of `state`,
// `sample` ahead of the mask copy and `final_matrices` after it, and the render node count read in the loop test. Each
// is needed for the match (any one undone scores 3 to 280).
bool function_bfc90(long object_index);
void function_0bfe20(dword *flags, long bit, bool value);
// @retail 0xbd610
transform4x3f const *__stdcall function_bd610(long object_index, byte const *nodes,
	transform4x3f const *input_matrices)
{
	transform4x3f const *result = input_matrices;
	s_object_transform_header *header = &((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff];
	if (*(short *)((byte *)header->object + 0x112) != NONE && !function_bfc90(object_index))
	{
		byte *object = (byte *)object_get(object_index);
		s_object_definition *definition = (s_object_definition *)g_4e3b44[*(long *)object & 0xffff].bytes;
		long model_index = definition->model_tag_index;
		if (model_index != NONE)
		{
			byte *model = g_4e3b44[model_index & 0xffff].bytes;
			s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
			long node_count = *(short *)(object + 0x110) / sizeof(rigid_transform_scaled);
			byte *render_model = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
			rigid_transform_scaled *object_orientations = (rigid_transform_scaled *)(object + *(short *)(object + 0x112));
			bool sample = false;
			dword mask[8];
			memcpy(mask, model + 0xa4, sizeof(mask));
			transform4x3f const *final_matrices = input_matrices;
			transform4x3f *matrices = 0;
			rigid_transform_scaled *orientations = 0;
			if ((bool)((*(dword *)(object + 4) >> 30) & 1) &&
				!((bool)((*(dword *)((byte *)header->object + 4) >> 29) & 1)) &&
				(!((1 << definition->object_type) & 1) || !((byte *)header->object)[0x34b]))
			{
				for (long node = 0; !sample && node < *(long *)(render_model + 0x1c); ++node)
				{
					byte index = nodes[node];
					if (index != 0xff && function_0bfe60((dword const *)(model + 0xc4), index))
						sample = true;
				}
				if (sample)
				{
					function_0bfe20((dword *)(object + 4), 30, false);
					function_bdb60(object_index, (s_16760c_render_model *)render_model, state,
						(long)(model + 0xa4), node_count, (byte *)object_orientations);
					orientations = object_orientations;
					matrices = (transform4x3f *)input_matrices;
					final_matrices = input_matrices;
				}
			}
			if (((1 << ((byte *)header)[3]) & 3) &&
				(unit_animation_periodic_layer_active(object_index) || function_10edd0(object_index)))
			{
				memcpy(g_4dc2f0, object_orientations, node_count * sizeof(rigid_transform_scaled));
				memcpy(g_4d8f20, input_matrices, node_count * sizeof(transform4x3f));
				function_10ec00(object_index, state, (long)render_model, 0,
					node_count, (real_quaternion_transform *)g_4dc2f0, (byte *)mask);
				function_10e530(object_index, state, (long)render_model, 0,
					node_count, (real_quaternion_transform *)g_4dc2f0, (byte *)mask);
				matrices = g_4d8f20;
				final_matrices = g_4d8f20;
				orientations = g_4dc2f0;
			}
			if (matrices && orientations)
			{
				for (long node = 1; node < node_count; ++node)
				{
					if (bit_vector_test(mask, node))
					{
						byte *render_node = *(byte **)(render_model + 0x4c) + node * 0x60;
						function_1421f0(&matrices[node], &orientations[node]);
						function_142a60(&matrices[*(short *)(render_node + 4)], &matrices[node], &matrices[node]);
					}
				}
				result = matrices;
			}
			if (final_matrices && ((1 << ((byte *)header)[3]) & 3))
				function_1132f0(object_index, (dword const *)(model + 0xa4), node_count, (transform4x3f *)final_matrices);
		}
	}
	return result;
}

#include <math.h>
/* the point and direction 0xbc070 sets in the list at g_4de2f4 */
struct s_direction_state_bc070
{
	byte unknown00[0x54];
	point3f point;
	vector3f direction;
};

PRIVATE __forceinline void vector3f_negate(vector3f const *vector, vector3f *result)
{
	result->i = 0.0f - vector->i;
	result->j = 0.0f - vector->j;
	result->k = 0.0f - vector->k;
}

/* sets the direction to the reverse of the one two angles in degrees give,
   and sets the point */
// @retail 0xbc070
void __stdcall function_bc070(real a, real b, real c, real d, real e)
{
    s_direction_state_bc070 *state = (s_direction_state_bc070 *)g_4de2f4;
    real vertical = a * 0.01745329238474369f;
    real horizontal = b * 0.01745329238474369f;
    state->direction.i = (real)(cos(horizontal) * cos(vertical));
    state->direction.j = (real)(sin(horizontal) * cos(vertical));
    state->direction.k = (real)sin(vertical);
    vector3f_negate(&state->direction, &state->direction);
    real_point3d_set(&state->point, c, d, e);
}


/* the second point and direction 0xbbfc0 sets in the same list */
struct s_direction_state_bbfc0
{
	byte unknown00[0x28];
	vector3f direction_copy;
	real value34;
	real value38;
	point3f point;
	vector3f direction;
	byte unknown54[0x6c - 0x54];
	short unknown6c;
};

/* the same for the second direction, which it also copies to 0x28; sets the
   floats at 0x34 and 0x38 to 0.8 and the word at 0x6c to NONE */
// @retail 0xbbfc0
void __stdcall function_bbfc0(real a, real b, real c, real d, real e)
{
    s_direction_state_bbfc0 *state = (s_direction_state_bbfc0 *)g_4de2f4;
    real vertical = a * 0.01745329238474369f;
    real horizontal = b * 0.01745329238474369f;
    state->direction.i = (real)(cos(horizontal) * cos(vertical));
    state->direction.j = (real)(sin(horizontal) * cos(vertical));
    state->direction.k = (real)sin(vertical);
    vector3f_negate(&state->direction, &state->direction);
    real_point3d_set(&state->point, c, d, e);
    state->direction_copy = state->direction;
    state->value34 = 0.8f;
    state->value38 = 0.8f;
    state->unknown6c = NONE;
}

void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disabled);
// @retail 0xb77d0
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity)
{
    // Retail keeps the linear velocity parameter on the stack.
    vector3f const *const *linear_reference = &linear_velocity;
    linear_velocity = *linear_reference;
    function_b7740(object_index, linear_velocity, angular_velocity, false);
    function_1c4b00(object_index, (void *)linear_velocity, (void *)angular_velocity, 1);
    if ((linear_velocity && linear_velocity->i * linear_velocity->i + linear_velocity->j * linear_velocity->j + linear_velocity->k * linear_velocity->k > 0.0001f) ||
        (angular_velocity && angular_velocity->i * angular_velocity->i + angular_velocity->j * angular_velocity->j + angular_velocity->k * angular_velocity->k > 0.0001f))
    {
        function_b9b90(object_index, false);
        function_b7360(object_index);
        function_bba20(object_index);
    }
}


// Disabled: retail passes persistent shared orientation/mask storage and reaches 0x142a60's unbound inputs.
#if 1
#include "unknown_1cafc0.h"
#include "unknown_11cb00.h"

dword g_55eed4;
extern real_quaternion_transform *g_4687d8;
void __stdcall function_1d3920(byte *component, dword *mask, real amount);
real function_e1670(long object_index);
struct s_render_model_definition;
void render_model_get_default_orientations(s_render_model_definition const *definition, rigid_transform_scaled *orientations);
bool function_bf5a0(long object_index);
struct s_bit_vector
{
    dword bits[8];
    void function_0bfe80();
    void function_0bfed0(long node_count);
    bool is_empty() const;
};
struct s_index_triple;
bool function_0b6780(s_index_triple const *triple);

void object_dispatch_adjust_root_matrix(long object_index, long matrices);

void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);
bool function_a98b0(long index, vector3f *position);
bool function_a9940(long index, vector3f *up, vector3f *forward);

// score 20 (was 20; 87 before b10C, 97 earlier): ours is 5 bytes longer (1399 against 1394). Left: only the
// no-model branch's parent lookup (ours takes the index in ECX and reloads the pool, retail has the index in ESI
// copied to EAX and the pool still in ECX from the current-object lookup).
// M3 lane 6 r223 (h2m fast with 0xb8bd0/0xbdc40/0xbfa40, all MATCH; control 20 at 1399 B): `inline` added to this
// file's own declaration of 0xb8bd0 with the no-model arm calling it 47 (1385 B, the same code as the plain call: a
// caller-side inline declaration does not make the /Ob1 definition expandable); the render and no-model arms swapped
// under `render_index == NONE` 297 (1388 B); in the no-model arm the flag read first, then the matrix through the 0xb8bd0
// call 19 (1395 B: the index takes ESI across the call and the flag reads the data pointer still in EDI, where retail
// re-reads data from the pool in ECX), that with the defaults in an else arm 23 (1395 B); the else-arm defaults with both
// lookups indexed by `(word)parent_index` 24 (1392 B). Re-measured: the else-arm defaults with the flag first 18.
// Earlier head (at 87, was 87; 97 before): retail was 2 bytes longer. Left then: retail tests the node count for > 0 before
// the node loop from 1 (no loop-head pad), and in the no-model branch keeps the object table pointer
// in ecx and re-reads only its data pointer (parent index in esi); ours now re-reads both there (round
// 81: the parent looked up through the existing object_get/object_node_matrix accessors, once for the
// matrix and once for the mirrored flag, as the render branch does after 0xb8bd0; before that ours
// reused edi, at 97 and 11 bytes short). Round 19: the
// sample-mask helpers 0xbfe80/0xbfed0 written as members of s_bit_vector (as is_empty is) give
// retail's ESI re-set after 0xbfed0, 234 -> 97 (kept).
// Before round 19 (at 234): retail sets the sample-mask
// address in esi again after 0xbfed0 (which never writes esi), tests the node count for
// > 0 before the node loop from 1 (no loop-head pad), and in the no-model branch keeps
// the object table pointer in ecx and re-reads its data pointer (parent index in esi)
// where ours reuses edi. Round 54: the existing flag setter 0xbfe20 called (retail's
// call) 305 positional / closer aligned; 0xbfe80/0xbfed0 inline with separate noinline
// declarations 304; model lookup without the null test 363; component flag read as a
// word 239 kept (all four together); 0xa9940's outputs in retail's order 239 kept;
// sample mask assigned between the mask calls 251; sample mask assigned first and passed
// to the copies 215 (new frame slot shifts the whole frame; rejected); component address
// spellings 239/239. Round 138: the tag table read at each use instead of a local with a
// hand reload after the component call 234 kept (fixes the component/table edi/ebp
// roles); an explicit count > 0 guard around the node loop 233 (the loop then matches,
// but it repeats the loop's test: not kept); the no-model branch through the existing
// 0xb8bd0 241 (stays a call); amount before component 278; component in the call 278;
// mask address as the storage struct 239; a function-local static mask object (gives
// the same guard code as the hand-written one) 239; 0xbfed0 plain or plain with a
// noinline declaration 234/234; mirrored flag before the matrix 234; parent through a
// header pointer 234. Round 140: the parent index shared by both branches 384, with the
// parent and mirrored defaults 361; the sample mask assigned before the copies 168 (the
// mask gets a stack home and esi/edi and every later slot shift: 113 aligned differences
// against 18; rejected as before); 0xbfed0 advancing or reassigning its own flags pointer
// (both still match) 234/234. Round 29: 0xbd090 is 0xbfed0's only retail caller and
// 0xbfed0 never writes esi, so the reload looks like the caller not knowing 0xbfed0's
// registers. 0xbfed0 inline without its noinline declaration 234 (still called, same
// code); a bool for the copied mask passed as `masked ? mask : 0` 242; the mask pointer
// from memcpy's return passed to both calls 255; the null mask declared above the
// static guard 137 (an artefact: the guard test grows an instruction that realigns the
// listing past the still-missing reload; rejected); the mask assigned before the copies
// 168 again (as round 140; rejected). Round 31: the sample mask as the long 0xbdb60 takes 234; one pool
// local for every lookup (0xba160's idiom) 399, from the current-object lookup on 234; the sample mask
// declared at entry 328; the no-model parent index read at each use 234. Round 84 (aligned
// differences are only the esi reload, the count > 0 guard and the no-model lookup): mask copy as a
// struct assignment 234; sample mask as the storage object's address 234; a named model-mask pointer
// for the test and the copy 258; a noinline 0xbfed0 declaration here 234; sample mask from memcpy's
// return 255.
// No-model pool/header-array/parent-header/current-header forms 234 each; mask element address 234;
// all-node root skip/nested 245/245; preincrement while 242.
// return 255. Round 30 (fast): sample mask set in an else arm 250; the no-model parent matrix through a
// file-local inline copy of 0xb8bd0's body 226 at 1392 bytes (its object lookup then reloads the table
// as retail does, but also reloads g_4e0300, which retail keeps in ecx), with the count > 0 guard 222
// at retail's 1394 bytes (left: the esi reload and the g_4e0300 reload; match-only, a one-caller helper);
// on that, the helper plain inline 222, taking the pool 222, a pool local shared with the current-object
// lookup 323, the sample mask through an inline address accessor 222, mirrored flag before it 223.
// Round 150 (fast): this file is /Ob1, so retail's inline parent lookup in the no-model branch is an
// inline-marked accessor, not 0xb8bd0; retail's render branch keeps the table data in EDI across the
// 0xb8bd0 call while the no-model branch re-reads it through the pool in ECX. Sample mask typed as the
// bit vector 234; the empty test through a non-const view 234; render index declared before the model
// 234; a pool local in the no-model branch 234; the parent read through the entry header pointer 385
// (1394 bytes; not a real form). The ESI reload after 0xbfed0 is the reverse of 0x14a8e0's case, where
// retail reuses ESI after a register-argument call that ours re-forms.
// Round 153 (fast): one pool local for the current and no-model lookups 290; volatile stored flag (diagnostic)
// 234; render-model node count named once 252; compile-only, no node-loop form gives retail's count > 0 test.
// Round 108 (fast, aligned differences 20: the esi reload, the count > 0 guard and the no-model
// reload): retail keeps g_4e0300 in ecx and re-reads its data field in the no-model branch, which our
// build does only when a pointer store sits on the path from the current-object lookup (diagnostic
// store in the default-orientation arm, 307; a volatile flag 234 does not). Orientations through an
// inline out-parameter helper 234; render branch nested under the model test with a return 234;
// flag computed from the orientation test 312. The esi reload after 0xbfed0 is unique in retail.
// Round 90: node loop from 0 with `node != 0 &&` in its test 245 (the test stays in the loop; retail's
// missing loop-head pad follows from the 8-byte count guard, not a threaded label); 0xbfed0 plain with
// its flags pointer stepped (still matches) 234; no-model mirrored flag read before the matrix 234;
// 0xb8bd0 marked inline and called in the no-model branch 397 (inlined at every site: matched 0xbcc20
// and 0x11b710 break); node index declared at the top of the render branch 234.
// Round 122 (fast, aligned 16): no-model parent through slot_handler.h's object_get/object_node_matrix 234;
// with the current and render-branch lookups through object_get too 234/234; 0xbfed0 declared inline here 234,
// and with its separate noinline declaration removed 234 (still called, the caller still trusts its ESI).
// Round 47 (m3 lane 1, fast; aligned 16): node loop from 0 skipping node 0 by `continue` / by `node > 0 &&`
// in the mask test 245/245 (no peeling, so no count > 0 guard); the no-model branch written before the render
// branch 313; orientations defaulting to g_4dc2f0 with the stored-orientation arm overriding it 402; the
// stored flag initialised beside the orientation test 398. Diagnostics: 0xbfed0's definition moved after
// this function in this file 234 (still no ESI reload: definition order is not why retail distrusts ESI);
// the no-model data field read as a dword 234 (still reuses EDI: the reload is not a type difference).
// Round 121 (m3 lane 5, fast): retail rematerialises 0x55eeb4 at every use (memcpy destination, both
// mask-call arguments, the sample mask); the stored/empty test as nested ifs 234; the amount by if/else
// 234; orientations defaulted at the declaration with `else stored = false` 402 (1367 bytes); `stored`
// declared first 236; the no-model parent matrix by array indexing 234.
// Round 37 (lane 13, fast): the node-mask tests through the existing bit_vector_test 234 (same code, kept: replaces
// the hand-written bit tests); render/no-model branches swapped in the source 313 (1372 bytes); a short node
// counter 222 (1387 bytes, but aligned 16 -> 120: the frame and every later slot move); `++node` 234; the render
// model's node count named for the default-orientation test and the loop bound 252 (1370 bytes).
// Round 19 (lane 17, fast with 0xbc820/0xbd020/0xbee60/0xbdb60 and all of unknown_0bfd20.cpp): the mask
// helpers as s_bit_vector members, called through `((s_bit_vector *)g_55eeb4)->`, 97 (kept; 0xbfe80,
// 0xbfed0 and 0xbff10 still match). Every form after the old ESI shift scored 234 because the rest of
// the listing was misaligned, so earlier no-model and node-loop forms are worth re-measuring on this
// base. On it: an explicit `count > 0` guard around the node loop 44 at 1390 bytes (diagnostic: it
// repeats the loop's test; the loop then matches and only the no-model branch's data-field reload
// remains), with a pool local or a parent header pointer in the no-model branch 44/44 (same code); the
// pool local without the guard 97. Before the members (at 234): the render-node pointer scoped in the
// mask test 234; model and render index as conditional expressions 340 (1386 bytes); the memset size
// written `sizeof(dword) * (...)` 234.
// Round 22 (lane 17, fast): on the count > 0 guard diagnostic (44, 1390 bytes; only the no-model branch left:
// retail keeps g_4e0300 in ECX and reloads its data field with the parent index in ESI, ours reuses EDI):
// `(void)&` on stored, orientations, model or current 44 each; the stored flag cleared through a pointer to
// it 44; the no-model parent through slot_handler.h's object_get and object_node_matrix 44 (same code each).
// Without the guard: the node loop from 0 with `node != 0 &&` in the mask test 105 (1374 bytes), with
// `if (node == 0) continue;` 105.
// Round 26 (lane 17, fast; retail also reuses EDI for the render branch's parent lookup after the 0xb8bd0
// call, so only the no-model path loses the data field): a pointer local to orientations, stored or
// current read once (diagnostics) 97 each, same code. On the count > 0 guard diagnostic: no-model mirrored
// flag before the matrix 43 (aligned 8 -> 12); parent index read at each use 44; no-model branch first 314;
// render branch ending in `return` with the no-model code after it 44; parent read through
// s_object_transform_view 44; parent header through a file-local inline lookup taking the pool pointer as a
// parameter (diagnostic) 44, same code.
// Round 59 (m3 lane 1, fast, on the count > 0 guard diagnostic, 44 at 1390 bytes, aligned 8, only the no-model lookup
// left): the object table bound to a `void const *const &` temporary 44, from a by-value accessor 44; the parent
// header from a forceinline accessor bound to a const reference 41 (1392 bytes), the parent object the same way 41; one
// function-scope parent index assigned in each branch 44, with the accessor 41. Retail keeps the pool in ECX and the
// parent index in ESI there (ours takes ECX for the index as nothing reads the pool). The guard stays match-only.
// Round 28 (m3 lane 18, fast, on the count > 0 guard diagnostic, 44 at 1390 bytes): `orientations` or `stored`
// escaping through a scratch global (diagnostics) 353/403; the no-model parent matrix through a file-local inline
// node-matrix helper 20 (1399 bytes; its lookup reloads both g_4e0300 and data, retail only data), the helper
// taking the pool 20, with a pool local in the no-model branch 20, with a pool local shared with the current
// lookup 292; a dummy store through the object before the no-model lookup 53 (a pointer store kills g_4e0300
// too). Retail's "pool held, data re-read" is matched sibling 0xb7740's volatile data-field read; find its
// natural source.
// Round 124: cached data-member pointer with a direct no-model pool read 87 (1392 bytes; NOT kept: a pointer
// to the pool's data member read at two uses and the pool read directly at the third, picked by score; body
// restored at 97); with the
// node-count guard 20 (1399, match-only); unified data-member reads 44 (1390, match-only); const pool pointer,
// shared header accessor, header pointer arithmetic, nonconst data-member pointer and typed current view 20 each
// (all with the match-only guard). The guarded forms were restored to the 87 form.
// DH306: actual matrix/orientation/node-record-displacement induction, seven subsets
// 94/210/96/212/230/212/212; matrix-cursor-owned bound 396. NOT kept: best gains only three.
// Original loop and no-model lookup restored; count entry and data reload remain.
// Round 62 (m3 lane 10): node loop under `if (count > 1)` 96 (repeats the loop test, not kept); the loop as
// `while` 97; the render node count named before the root block 212; the current object through the entry
// header pointer 371 (1394 bytes); the model tag index read at each use 97. Original body restored.
// Round 124 (m3 lane 8, fast): the default-orientation arm's two stores swapped 99; render index declared
// before the model 97; `stored` declared beside the orientation test 389 (1370 bytes); the node mask declared
// beside `stored` 97; the no-model parent indexed through `(word)parent_index` 100 (1380 bytes).
// Round 45 (m3 lane 2, fast, base 97): walking matrix/orientation pointers in the node loop 97 (same code); a
// no-model parent header pointer 97; `current` read through `header->object` 371 (1394 bytes, but the current
// lookup no longer reloads g_4e0300); the render-node pointer scoped in the mask test (re-measured) 97; the
// no-model data field read as a dword (re-measured) 97. Retail's `test eax, eax; jle` before the node loop's
// `mov esi, 1; cmp; jle` is still unexplained.
// Lane 18 round 36 (fast, re-measured on the 97 base): a `short` node counter 179 (1389 bytes); the stored/empty
// sample-mask test as nested ifs 97 (same code); the no-model parent matrix through the existing function_b8bd0
// 100 (1378 bytes, stays a call); the default-orientation test as `== 0` with `++node` and `&nodes[node * 0x60]`
// 97 (same code); the render-node pointer declared inside the mask test 97 (same code).
// Round 206 (m3 lane 3, fast, on the 97 base): the node loop as `long node = 0; while (++node < count)` 97 (same
// code, no count > 0 guard); the node index declared at the top of the render branch (re-measured) 97; the parent
// index declared once for both branches (re-measured) 341 (1386 bytes); parent and parent_mirrored declared once
// for both branches 350 (1382); the no-model mirrored flag before the matrix (re-measured) 97 (same code).
// Round 141 (m3 lane 6, fast): no-model mirrored flag read before the parent matrix 97 (same code); the no-model
// matrices argument re-read from object +0x116 90 (1388 bytes, positional only: retail passes the spilled
// matrices pointer from +0x1c); the node loop's parent node index as a short local 244 (1381 bytes); a node matrix
// pointer local in the loop 95; render-node pointer inside the mask test 97 again (repeat).
// Round 68 (m3 lane 10, fast, on the count > 0 guard diagnostic, 44 at 1390 bytes): `stored` escaping through a
// scratch global (diagnostic) 403, and its no-model branch then holds the pool and re-reads only the data field, as
// retail does: a store to an escaped local on the path kills the data load but not g_4e0300. Escaping stored,
// orientations, current or model at the end of the no-model branch 314/363/312/318 (diagnostics). A forceinline
// lookup helper taking the pool 44, a plain inline one 44, one with a pool local inside 44 (all fully CSE'd); a
// named header-array local for the current and render lookups 45; a plain inline node-matrix helper taking the
// pool / the index only / the data member's address 20/20/20 (1399 bytes: reloads g_4e0300 too, as round 28); that
// helper with a mirrored out-parameter 43; pool local plus that helper, with the render lookup through the local or
// not 20/292, without the helper 44/286; a pool local declared after the current lookup with the helper 358 (the
// pool moves to EBP for the whole body); 0xb8bd0 marked inline in its file with the no-model branch calling it 394
// (inlined at both sites). Original body restored.
// Round 206 (m3 lane 3, fast, on the 97 base): the node loop as `long node = 0; while (++node < count)` 97 (same
// code, no count > 0 guard); the node index declared at the top of the render branch (re-measured) 97; the parent
// index declared once for both branches (re-measured) 341 (1386 bytes); parent and parent_mirrored declared once
// for both branches 350 (1382); the no-model mirrored flag before the matrix (re-measured) 97 (same code).
// Round 42 (m3 lane 18, fast, on the count > 0 guard diagnostic, 44 at 1390 bytes): retail has no store or call on
// either path into the no-model branch and its render branch reuses EDI after 0xb8bd0, so its re-read is no kill.
// The data field through a `byte *const *` pointer to it 44; the parent header by pointer addition 44; the parent
// object by byte offset 44 (all same code); parent object, matrix and mirrored flag as conditional initialisations
// 44 (1396); a parent header pointer with its object read at each use, mirrored first, 42 (1392: `lea` of the header).
// Round 87 (m3 lane 16, fast, on the count > 0 guard diagnostic, 44 at 1390): `__assume` in the no-model arm (index != NONE at the
// top or the end, parent object != NULL, pool != NULL before or in the arm, data != NULL) 389 each (1399 bytes: any `__assume` unfolds
// the entry's flag store, the no-model block stays the same); three parent lookups 20 (1399), two 20 (1399); a forceinline lookup taking
// the index as `long const &`, one/two lookups 44/44, a plain inline one, two lookups 44. Original body restored.
// Round 110: split parent base, a one-use parent-node local, pool[0]/dereferenced-pool member spellings,
// parent object/index role names, a function-scope parent object and a parent-node array pointer 97 each
// (same code). NOT kept: no gain; the one-use forms require a byte match. Opening body restored.
// Lane 13 r62 (fast, on the count > 0 guard diagnostic unless noted; 44 at 1390): two lookups of the parent in the no-model
// block's own statements 20 at 1399 (one lookup always folds the data field into EDI, two make the block reload g_4e0300 too,
// whatever the spelling), and the guard itself 44. With one lookup: a lookup helper taking the pool 44; plain 44; index computed
// first in the helper with a pool param / with statements / plain 44/44/44; the index masked in place in the helper, plain /
// with a pool param, 44/44; an unused index param on a mirrored helper, inline / forceinline 44/44; `parent_index` at function
// scope assigned in each arm 44; the test repeated in a second `if` 44; ternaries with one lookup / lookups in each 45 (1391) / 50
// (1402); a header pointer read at both uses 41 (1392). With two: node-matrix helper 20; taking the pool 20, a short node 20,
// a named matrices local 20; pool local inside the arm 20; function-scope index 20; helpers with the index first 20/20; the
// field read at each use 20, at the test 20; `do {} while (false)` between the lookups 20; a named masked index first/second/both
// 20/20/20; `(word)` index on the second/first lookup 377 (1398) / 373 (1397); an empty `if`, an unused local and a dead store
// of the index after the flag read 20/20/20. Parent helpers for matrix and flag, inline / forceinline 62/62 (1410); one helper with
// two out-parameters, inline / forceinline 116 (1359) / 44. Pool local at the current lookup, spelled for current/render/two
// no-model lookups: pool,g,pool,pool 292 (1402); pool everywhere 20; pool,g,g,g 20; pool,pool,g,g 20; pool,pool,copy,copy 20;
// pool,g,copy,copy 292; pool,pool,pool,g 357 (1394); pool,pool,g,pool 358 (1394); g,g,arm-local,g 44; a pool local used only by
// the no-model lookups, two/one 20/20.
// Diagnostics, restored at once: on the two-lookup form a later use of the index (`if (parent_index == 5) *(long *)(object +
// 0x14) = 6;`) after the flag read 40 at 1406: the block up to the flag read is then byte-identical to retail, with the index in
// ESI, `mov eax, esi`, `[ecx + 0x44]`, and g_4e0300 still in ECX. After the call instead 402 (1408). So retail's index stays live past
// the lookup and ECX is never overwritten; what keeps it live with no code is the lead. Sibling 0xb7740 holds its pool
// in EBX and re-reads data after real stores through the object, which this path doesn't have.
// M3 lane 9 r88 (fast, on the count > 0 guard diagnostic, 44 at 1390, aligned+stack 5, unless noted): `stored` cleared through an inline out-parameter helper 97 without the guard and 44 with it (promoted after inlining: no kill of the
// data field); a repeated `if (parent_index != NONE)` with a lookup in each 44; ternaries 50 (1402); two/three lookups in one test 20 (1399: the index takes ECX, so the pool reloads into EDX); the no-model arm first in source
// 315 (1372), with the guard 314, with the guard and two lookups 297. Retail's copy of the index means it stays live past the lookup; every spelling here lets it die in place.
// M3 lane 14 r68 (fast, on the count > 0 guard diagnostic, 44 at 1390): parent node index named before the lookup 44;
// parent/mirrored defaults in an else arm 45 (1391); the masked parent index named 44; declarations index, mirrored,
// parent 46; without the guard, the no-model parent through slot_handler.h's object_get/object_node_matrix 97 (same
// code). The guard byte pattern (`test; jle; mov reg, 1; cmp; jle`) occurs nowhere else in retail.
// Round 57 (m3 lane 2, fast, on the 97 base): masked nodes skipped by `continue` 97 (same code); the no-model parent index
// declared before the defaults 97, with the mirrored flag read first 97 (same code); the no-model 0xbdc40 call written in both
// arms of the parent test 405 (1406 bytes); the render node count bound to a `long const &` for the default test and the
// loop bound 97 (same code).
// Round 81 (m3 lane 10, fast with this file and 0xbfe20/0xbfe80/0xbfed0/0xbff10/0xb8bd0/0xbdc40/0xbdb60, on 97
// unless noted): the no-model parent matrix by array indexing 97 (same code); a named masked parent index 97
// (same code); the render node count through a `long const *` to its field for the default test and the
// loop 97 (same code); the no-model parent through 0xb8bd0 with a fresh flag lookup 100 (1378 bytes, stays a
// call); the parent object local for the matrix with a fresh lookup for the flag 87 (1392 bytes; a mix, not
// kept); the lookup written out at each of its three uses 87 (1392 bytes); object_node_matrix(object_get())
// and object_get() for the flag 87 (1392 bytes, kept, plain-confirmed). On 87: the render branch's flag
// through object_get 87 (same code, kept: drops a one-use local); the current object through object_get 87;
// the no-model flag before the matrix 88; the parent index read at each use 87; the index declared before
// the defaults 87.
// Round 83 (m3 lane 10, fast, on 87): one function-scope parent index assigned in each branch 87 (same code); the no-model
// matrix and mirrored flag under two separate index tests 97 (1383 bytes: data from EDI again); the no-model flag through
// object_header_get 87 (same code); one parent header pointer read at both uses 95 (1385); the no-model defaults set in an
// else arm 93 (1387). Lead from the last: with the defaults out of EAX/EDX at the index load, ours keeps g_4e0300 in ECX and
// re-reads only its data field (`mov eax, [ecx + 0x44]`), as retail; retail's defaults sit before the test, so its index
// takes ESI (copied to EAX for the mask) where ours takes ECX and reloads the pool. What gives the index ESI there is open.
// Round 101 (m3 lane 9, fast, on the count > 0 guard diagnostic, re-measured 20 at 1399 with the current two-lookup
// body): retail does ONE parent lookup there (the parent object in ECX serves the node matrix and the flag), with
// the index in ESI copied to EAX and the pool still in ECX. One parent-object local from object_get(index) 44 at
// 1390 (data reused from EDI); the same with the index field re-read in the lookup 44, with the field read at the
// test and the lookup and no index local 44 (same code each); the two-lookup body with the field re-read in the
// first lookup only 54 at 1406, in the second only 380 at 1402, in both with no index local 20 (same code as the
// local). A re-read of the field does not give the `mov eax, esi` copy here.
// M3 lane 1 r88 (fast with 0xbfe20/0xbfe80/0xbfed0/0xbff10/0xb8bd0/0xbdc40/0xbdb60, all matched): the count > 0 guard
// diagnostic re-measured 20 at 1399 bytes (aligned 8, register-masked 4: only the no-model lookup differs). On the guard:
// the parent index and node read through the object's typed view (`->parent_index`, `->parent_node`) 20 (same code); the
// index `long const` 20 and `dword` 20 (same code); one parent-object local for the matrix with object_get again for the
// flag 44 (1390 B, data reused from EDI; round 101's number, a re-measure); the model pointer by a ternary and the render
// test as `model && model->render != NONE` 312 (1408 B). The no-model branch in every order of its two defaults and the
// index read, matrix and flag in either order, with object_get at each use or one parent-object local (23 forms): matrix
// first 20/22 (the parent default before the mirrored one 20, after it 22; the index read anywhere), flag first 25/27,
// the local 43/44/45/46 (1390 B); the index never takes ESI. Retail's no-model block is 0xb8bd0's own instruction
// sequence with its
// `mov ecx, [g_4e0300]` gone (ECX still holds the pool from the current lookup) and the index copied into EAX, the
// out-of-line function's parameter register: it reads as 0xb8bd0 inlined at this one site, which this file's /Ob1 and
// the callee file's /Ob1 rule out here (round 90 and r122: `inline` on 0xb8bd0 inlines it at both sites).
// Round 180 (m3 lane 4, fast; aligned = difflib count with addresses masked, 10 at 87): the render node count named
// after the root block for the loop bound 146 (1391 bytes, frame slots shift); the bound written first in the loop
// test 87 (aligned 12); one parent-object local from object_get for the no-model matrix and flag 97 (1383 bytes:
// data reused from EDI); diagnostics re-measured on the current body: the count > 0 guard 20 (1399 bytes, aligned 8),
// with the one-object local 44 (1390). The guard stays match-only. The no-model parent index and node read through
// the existing object view's parent fields 87, in the render branch too 87 (same code).
// Round 87 (m3 lane 11, fast): the no-model case first under `if (render_index == NONE) { ...; return; }` with the
// render code un-nested after it 306 (1378 bytes). No existing node-matrix accessor other than 0xb8bd0 exists in a file
// without /Ob1 that LTCG could inline here (headers: only slot_handler.h's object_node_matrix, already used; the
// data_array.h datum helpers add checks retail lacks), so the no-model block is BLOCKED-BY 0xb8bd0 (inlined at this one
// site in retail; both files /Ob1); the count > 0 guard stays match-only.
// b10C (11-17) walk against retail, fast build on r841: statements, offsets, jcc senses, callee arguments (0x1d3920,
// 0xbdb60, 0xbdc40, 0x142a60) and constants all compile to retail's bytes: no behaviour difference found. Kept,
// retail's structure: the node loop sits inside `if (count > 0)`, as retail tests the count at 0xbd50f (`test eax, eax;
// jle 0xbd5f5`) before the loop's own `mov esi, 1; cmp eax, esi`: 87 -> 20 (1399 bytes); 0xbc820, 0xbd020, 0xbee60,
// 0xb8bd0 and 0xbfa40 stay MATCH. Left: the no-model lookup at 0xbd593-0xbd5c8, which is 0xb8bd0's own sequence
// (0xb8bd0-0xb8bf8) without its `mov ecx, [0x4e0300]` (ecx still holds the pool from 0xbd1b2), with the index copied to
// eax (`mov eax, esi`), `[ecx + 0x44]` re-read and the flag read from the same parent object; ours takes the index in
// ecx and reloads the pool into edx. Re-measured on the guard, same code unless noted: the stored flag as a one-element
// array 20, a `headers` local for the current and render-branch lookups 21, the no-model data field read through a
// `byte *volatile const *` (as 0xb7740 does) 20, that with a pool local shared with the current lookup 292 (pool moves
// to edi). Retail's `mov eax, esi` needs the index live past the lookup; no form here gives that.
// M3 lane 1 r119 (fast with 0xb8bd0/0xbdc40/0xbc820/0xbd020/0xbee60/0xbfa40, all MATCH unless noted; control 20,
// aligned 8): the no-model arm calling 0xb8bd0 as the render arm does 47 (1385 B: the index then takes ESI and is
// copied to EAX for the call, as retail's block begins, but the call stays); 0xb8bd0 defined inline and called
// there 394 re-measured (1373 B, both sites inline; the no-model block then has the pool in EBP and the index
// masked in place in ECX, so a truly inlined 0xb8bd0 does not give retail's copy either); a pool local declared at
// the current lookup, spelled for current/render/no-model matrix/no-model flag: pool,pool,pool,g 357 and
// pool,pool,g,pool 358 re-measured, with pool,g,pool,g / pool,g,g,pool / g,g,pool,g / g,g,g,pool / g,pool,pool,g /
// g,pool,g,pool 357 to 358 as well (all 1394 B, retail's length, but aligned 61 to 142: the pool then lives in EBP
// from the first lookup on, which retail's three separate loads rule out); the no-model lookups indexed by
// `(word)parent_index`, both 17 (1396 B, aligned 9: `movzx eax, cx`, not retail's copy and mask; under the keep
// bar and not retail's form), the first only 373 (1397 B, aligned 66), the second only 377 (1398 B, aligned 50),
// one parent-object local from it 47 (1387 B, aligned 12). Diagnostics, restored at once: an unread trailing
// parameter on 0xbdc40 given the parent index in the no-model call 20 (same code here: the argument is dropped
// before registers are chosen, so it keeps nothing live; 0xbdc40 itself 7), given NONE everywhere 20 (0xbdc40 7).
// Retail masks the index in place in the render arm (0xbd24c, where it dies) and copies it first in the no-model
// arm, so there the index is still live after the mask when registers are chosen; what reads it afterwards with no
// instruction is still open.
// M3 lane 4 r217 (fast with 0xb8bd0/0xbdc40/0xbc820/0xbd020/0xbee60/0xbfa40/0xbdb60, all MATCH; control 20): a pool local
// at the top of the no-model branch with one parent-object local 44 (1390 B, data from EDI), with the matrix and flag lookups
// both spelled through it 20 (same code: the local is forwarded into the lookup); the index copied into a local masked in
// place, one parent-object local 44; parent and mirrored flag as two conditionals on `parent_index != NONE` 45 (1391 B); a
// `byte *const *` to the data field declared at the branch top, one parent-object local 44 (forwarded). Diagnostic,
// reverted: that pointer volatile 20 (same code: even with the data re-read forced, the pool is re-read too, so the pool
// in ECX needs a value the allocator keeps live from the current lookup, not a different data-field base).
// Lane 13 r112 (fast, control 20 at 1399 B, aligned 12): the no-model matrix through a file-local node-matrix helper
// that masks its index parameter in place, forceinline / plain inline 20/20; that helper with a named absolute-index
// local 20; that helper with the flag's lookup written out in full 20; the parent index field read at each use with
// that helper 20, in both arms 20, with the existing accessors 20, in the render arm only 20, with a local for the
// test only 20 (same code each); the matrix and the flag under two separate index tests 44 (1390 B, data from EDI);
// the flag as `parent_index != NONE && bit` 42 (1395 B); the index stored back to its field after the flag read 20
// (the store is dropped). Diagnostics, restored at once: the file of 0xb8bd0 built without /Ob1 394 (1373 B: 0xb8bd0
// is then expanded at the render site too and 0x18c3b0 loses its match, so site frequency does not separate the two
// sites in this build); a later compare of the index after the flag read re-measured 40 at 1406 B, aligned 3 (the
// block is then retail's byte for byte). The order inside the lookup follows the pool: held in ECX, the index half is
// computed first (retail); reloaded, the data load comes first (ours). No form here keeps the index live past its mask.
// M3 lane 3 r267 (fast with 0xb8bd0, 0xbdc40, 0xbc820, 0xbd020, 0xbee60, 0xbfa40 and 0xbdb60, MATCH unless noted; base
// 20, aligned 12): the no-model parent and flag left uninitialised and set in both arms of the parent test (`else {
// parent = 0; parent_mirrored = false; }`) 18 at retail's 1394 B, aligned 8: the pool then stays in ECX and the lookup is
// retail's; left: the two defaults sit in the else arm (`jmp`, `xor eax, eax; xor dl, dl`) and the index is in EAX, not
// ESI with retail's copy (two points, not kept). Permuter on that form (900 s, 86 tries): best 18. Also: one
// parent-object local for the matrix and the flag 44 (1390 B); the parent index read before the defaults 20, with the
// defaults swapped 22; the parent default kept and the flag set in an else 290 (1404 B), the flag default kept and the
// parent set in an else 32 (1395 B); the index, or index, parent and flag, declared once for both arms 20/20 (same code);
// the model and render-index tests as one `&&` 316 (1397 B), plus the no-model arm calling 0xb8bd0 158 (1382 B), plus
// `inline` on 0xb8bd0 394 (1373 B: both sites expanded, 0x18c3b0 157). Diagnostic, reverted: `__assume(parent_index !=
// NONE)` after the flag read 392. Retail keeps the pool in ECX while the defaults (EAX, DL) are live across the lookup,
// so its index has to take ESI; ours spends ECX on the index and reloads the pool.
// M3 lane 14 r122 (fast with 0xb8bd0/0xbdc40/0xbc820/0xbd020/0xbee60/0xbfa40/0xbdb60, all MATCH; control 20): the no-model
// lookups through a `long const &` bound to the parent index 20 (same code); one parent-object local read through a
// `const &` to it 44 (1390 B, data from EDI); the declaration defaults kept with a repeated `else { parent = 0;
// parent_mirrored = false; }` 18 (1394 B, the else-arm form above: the initialisers are dropped); the parent node read
// into a local before the parent test 20 (same code); parent, flag and index declared once for both arms with the
// no-model defaults in an else arm 18 (1394 B, same code as the else-arm form).
// M3 lane 12 r151 (fast with 0xb8bd0, 0xbdc40, 0xbc820, 0xbd020, 0xbee60, 0xbfa40 and 0xbdb60, all MATCH; control 20 at
// 1399 B): the no-model test inverted with the defaults in the NONE arm and the lookups in the else 25 (1395 B); the
// else-arm defaults (r267's 18) with the flag read before the matrix 18 (1394 B: the node's `movsx` moves ahead and takes
// ESI), with the flag's object through object_header_get 18 (same code as r267's); 0xb8bd0 called for the matrix with the
// else-arm defaults 48 (1387 B); defaults first with a parent header pointer named in the arm for both reads 41 (1392 B).
// M3 lane 4 r228 (fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): in the no-model arm, the index copied into a
// second named local for both lookups 20; the index bound by `long const &` for the test and both lookups 20; the same
// reference only inside the parent arm 20; a redundant `parent_mirrored = false;` at the top of the parent arm 20; a
// redundant `parent = 0;` there 20 (all the same code). Reading: ECX already holds the pool at the arm's entry in both
// builds (loaded for the current-object lookup, EDI holds its data), and retail re-reads only the data from it while ours
// reloads both, so this is the register choice of the index (ours ECX), not a lost common value.
// M3 lane 12 r157 (fast with 0xb8bd0/0xbdc40/0xbc820/0xbd020/0xbee60/0xbfa40/0xbdb60, all MATCH; control 20 at 1399 B):
// a pool local declared right before the render-index test for both no-model lookups 320 (1408 B); declared before the
// model-index read 20 (same code: the copy is propagated and the global reloaded); the flag through a parent-object
// local read after the matrix lookup 20; both no-model lookups spelled as the current-object lookup (header-array
// cast) 20; the parent index read through the typed object view 20 (same code each). Lead: on 0xbba80 one `const &`
// binding moved a tag entry into retail's EAX but rotated every later scratch choice, and a second binding right
// after it rotated them back (match); untried here.
// M3 lane 12 r159 (fast, control 20 at 1399 B): the no-model matrix through this file's typed header-array lookup with
// the flag through object_get 20; the current-object lookup spelled through object_get 20; the matrix and flag lookups
// through two separate `long const &` bindings of the index 20 (all the same code); the whole no-model block as an
// inline helper taking the object index and the output 123 (stays a call, 1313 B), forceinline 263 (1390 B: the object
// pointer moves to EDI); the flag statement before the matrix with the defaults first 25. Reading: with the defaults
// live in EAX/DL across the index test, retail's index has to take ESI because the pool still holds ECX; ours lets
// the index take ECX and reloads the pool, so the pool's carried value loses its register to the index here.
// M3 lane 15 r119 (H2M fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): one parent-object lookup with the
// data field read through a dword-typed load 44 (1390 B: it still shares EDI's data); the render branch's two
// current-object reads through object_get(object_index) at each use 20 (same code); render_index set to NONE in an else
// arm of the model test 20 (same code); the no-model mirrored default declared before the parent default 22 (the two
// zeroings swap); the no-model parent index read into its own local ahead of the render-index test 378 (entry reorders).
// Diagnostic, restored at once: the no-model block written in both paths (an early `return` after it when the model
// index is NONE, and the else of the render-index test) 403 (1420 B: the two copies are not merged).
// M3 lane 8 r212 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): lane 3's both-arms form re-measured 18
// (1394 B: pool held in ECX, index in EAX). Diagnostics: 1-5 unused locals at the arm top and 1/2/3/5/8 at function top 20
// each (not a symbol-count tie); a conditional local store before the parent test 66, after the lookups 61 (pool still
// reloaded); the parent test's arm, or the whole no-model arm, inside `do { } while (0)` 20/20 (the arm's estimated
// frequency is not the lever). Tried: the flag through object_header_get first, then the matrix 25; parent as a
// conditional on the index test with the flag default and `if` 62 (1411 B); non-const parent 20; index and node through
// the typed object view 20; `dword` index 20; all six declaration orders of parent, flag and index times three spellings
// of the flag test (`(bool)((x >> 10) & 1)`, `(x & 0x400) != 0`, `((x >> 10) & 1) != 0`) 20 or 22; one pool local for the
// current and no-model lookups 292 (1402 B: the pool takes EDI and the render branch re-reads data too). The flag default
// kept with the parent set in an else (listed 32) re-measured: DL live, index in EAX, pool still held in ECX; so the pool is
// dropped only when EAX, ECX and EDX are all wanted, where retail moves the index to ESI instead.
// M3 lane 8 r215 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): this file builds with the default
// /O2 (auto-inlining on); only 0xb8bd0's file is /Ob1. Diagnostics, restored at once: `stored` escaping through a
// file-static pointer 403 (the pool is then held, but in EDI, and the data field is re-read on both paths, the render
// branch's too, where retail re-reads it only in the no-model arm); a store to an escaped scratch local at the top of
// the no-model arm only 410 (the pool is still reloaded: a kill inside the arm does not hold it); the one compiler-setting
// diagnostic, 0xb8bd0's file without /Ob1 and the no-model arm calling 0xb8bd0 as the render arm does, 394 (1373 B: then
// expanded at both sites; 0xbcc20 213, 0x11b710 116, 0x18c3b0 157). Forms: both no-model lookups spelled through
// effects.h's DATUM 20 (same code); one `const &` to the parent's header entry for both reads 41 (1392 B); both through
// object_header_get(...)->object 20 (same code). Reading: retail's block is 0xb8bd0's own code expanded at this one site
// (its parameter copy `mov eax, esi`, the index still live after it), which this build gives only with 0xb8bd0's file
// built without /Ob1, and then at every site: BLOCKED-BY 0xb8bd0 (its file's /Ob1, a build-flag decision).
// M3 lane 5 r229 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): in the no-model arm, a second index
// local for the matrix lookup only 20; a second index local for the flag lookup only 20; the parent object named for the
// flag only, after the matrix lookup 20; the node argument cast to `short` (0xb8bd0's node type) 20 (all the same code);
// the parent object named for the matrix only, the flag through object_get again 44 (1390 B, data from EDI).
// M3 lane 13 r142 (h2m fast with 0xb8bd0/0xbdc40, MATCH unless noted; control 20 at 1399 B): `inline` on 0xb8bd0's own
// definition with the no-model arm calling it 394 (1373 B: both arms' sites expand, and 0xbcc20 228, 0x11b710 116 and
// 0x18c3b0 157 stop matching); a `__forceinline` declaration of 0xb8bd0 in this file with the no-model arm calling it 47
// (1385 B, the plain call's code); the node index named ahead of the parent test 20 (same code); parent and flag as two
// conditional initialisers on the index test 45 (1391 B); the matrix written as a conditional argument of the 0xbdc40
// call with only the flag set under the test 42 (1392 B). Re-measured: the parent object named for the matrix only 44.
// M3 lane 14 r142 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): the no-model parent index and node
// read through the current-object pointer 214 (1408 B), on lane 3's else-arm defaults form 209 (1403 B); on the else-arm
// form (re-measured 18 at 1394 B) the mirrored flag ahead of the matrix 18, `long const` parent index 18, the parent node
// read into a `short` ahead of the test 18 (the else-arm form's code each). Retail keeps the defaults in EAX/DL across the
// test with the pool still in ECX, so its index takes ESI and is copied to EAX in the arm; still BLOCKED-BY 0xb8bd0.
// M3 lane 11 r154 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): on lane 3's else-arm defaults form
// (re-measured 18 at 1394 B: pool held in ECX, index in EAX, the defaults behind a `jmp`), the index read from the field at
// each use 18, the else arm's defaults flag first 18, the flag declared ahead of the parent 18, the parent and flag
// declared after the index 18, both lookups through this file's header-array view 18 (same code each); the parent index
// loaded once ahead of the render-index test for both arms 378 (1399 B), with the else-arm defaults 380 (1401 B: the entry
// reorders); the 0xbdc40 call written in each arm of the parent test (parent and flag declared in the found arm, `0,
// false` in the other) 414 (1421 B); the mirrored flag as `byte` 387 (1397 B). Retail's block is the link-time expansion of
// 0xb8bd0 (its parameter copy `mov eax, esi`); still BLOCKED-BY 0xb8bd0's inline decision.
// M3 lane 13 r143 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): the model and render index as two
// conditional initialisers on the model index 20; `render_index` declared ahead of `model` 20 (both the same code); the
// stored flag initialised from the orientation test itself (one definition) with the orientations as if/else 372 (1398 B);
// `stored` declared after the early return 39; the early-return test reading the object fetched at the top (no second
// lookup) 364 (1392 B). Listing reading: our no-model arm reads the parent node and scales it before the parent object
// lookup (the node argument is evaluated first), retail looks the object up first and reads the node after. All restored.
// M3 lane 3 r298 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): the render arm ending in `return`
// with the no-model code after it (no else) 20; the no-model parent index read from its field at each use 20; both
// together 20 (same code each: the index still takes ECX and the pool is reloaded); the mirrored flag set by an `if`
// 35 (1400 B); the defaults in the `parent_index == NONE` arm of an if/else 25 (1395 B).
// M3 lane 12 r178 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Diagnostic, restored at once: `inline`
// on 0xb8bd0's definition with the render arm's call routed through a noinline wrapper, so that only the no-model site
// expands the real 0xb8bd0: 20, the base's code byte for byte. So the no-model block is not waiting on 0xb8bd0's inline
// decision (the BLOCKED-BY above does not hold): expanded or spelled through the header accessors it is the same code, and
// what is left is the register choice alone (retail: pool kept in ECX, index in ESI and copied to EAX). Second diagnostic,
// restored: one more read of the pool pointer at the end of the no-model arm 371 (1394 B: the pool is then a held value in
// EBP from the current-object lookup on, data re-read through it as retail does, the index still in ECX), so the pool is
// a register candidate that loses to the index here. Matched 0xe6b00 has retail's `mov eax, [ecx + 0x44]` re-read from a
// pool local declared ahead of its first lookup and used by the second; tried here: such a pool local declared just ahead
// of the current-object lookup and used by both no-model lookups 20 (forwarded: same code); the same local read through a
// `s_record_pool *const &` 20 (same code); the no-model flag through the existing bit tester 0xbfe60 46 (1400 B: stays a
// call); the parent and mirrored defaults initialised once ahead of the render-index test for both arms 391 (1411 B); the
// parent index tested on the field with its local declared inside the test 20; a named header entry for the matrix with
// the flag through object_get 20 (both the same code). Re-measured: the arms swapped under `render_index == NONE` 297
// (1388 B: the block keeps the index in ECX there too, so the arm's place does not order the two); a pool local set ahead
// of the current-object lookup, the matrix through it and the flag through the global, 358 (1394 B).
// M3 lane 6 r233 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): the no-model parent index renamed to
// sort ahead of the other locals 20 and after them 20; the index declared `register` 20; the index and its test in their
// own inner block, parent and flag outside 20; the no-model parent and flag renamed to sort first 20 (all the same code:
// names and scopes do not move the index off ECX).
// M3 lane 14 r150 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Reading: up to the arm our code is
// retail's, so ECX still holds the pool there in both builds; ours ends the pool value at the current-object lookup and
// reuses ECX only when nothing overwrites it (the else-arm form), retail's index avoids ECX, so there the pool was a live
// value across the model tests when registers were chosen. Tried: a pool local declared ahead of the current-object
// lookup for that lookup and the render arm's flag lookup, the arm through the accessors 20 (same code: the arm reloads
// the global); that local for the current lookup and one parent-object local in the arm, the render arm through the
// accessor 286 (1399 B: the arm shares EDI's data and the render arm reloads the pool); a pool local at the function's
// top used by the arm only 397 (1405 B); a file-local node-matrix helper that reads its index parameter in two lookups
// 20, one that reassigns its index parameter (`object_index &= 0xffff;`) 20 (both the base's code: the parameter copy
// does not survive and the node is still read ahead of the lookup, where retail reads it after). Diagnostic, restored: an
// arm pool local with a second, different use (a lookup of the current object) 376 (1397 B: the pool is then a held
// value from the current-object lookup on and the arm re-reads only data through it, but in EBP). Re-measured: the
// matrix lookup indexed by the field re-read 54 (1406 B: the two lookups are no longer merged); a pool local at every
// lookup 20; an arm-local pool with one parent-object local 44.
// M3 lane 13 r152 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Allocator reading: the no-model
// parent index takes ECX because ECX is the first free register at its turn (priority 78, before the pool's own value, 34);
// forcing it to ESI gives retail's register but the mask is then done in place, with no `mov eax, esi` copy. With the arm
// calling 0xb8bd0 (re-measured 47, 1385 B) the aligned difference is only the call against retail's expanded body (the index
// in ESI copied to EAX is then retail's), so retail's block reads as that call expanded at link time at this one site.
// Forms with the call, none expanded: defaults in an else arm 48 (1387 B); the flag read ahead of the call 19 (1395 B,
// re-measured); the arms swapped under `render_index == NONE` 309 (1372 B); the render arm ending in `return;` with the
// no-model code after it 47; parent and flag as conditional initialisers on the index 48 (1387 B); the index test
// inverted (`== NONE` arm first with the defaults) 48 (1387 B); the index field read at each use, no local, 47.
// M3 lane 13 r153 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Allocator reading: the pool pointer
// loaded for the current-object lookup is itself a register candidate that reaches the no-model arm (two references,
// priority 9 after the blocks it only lives through), coloured in a late round; by then the no-model index (78) holds ECX
// and the defaults EAX and EDX, so the pool gets nothing and is loaded again. In lane 3's else-arm form (re-measured 18)
// the index (66) takes EAX and the pool gets ECX. Retail's index avoids ECX and is copied before its mask, which is what a
// use of the index after the lookup gives (the listed later-compare diagnostic); forcing the index to ESI alone gives
// retail's length with the mask still done in place. Tried: one parent index read ahead of the render-index test for
// both arms 378 (aligned 93: the pool is then held in EBP from the entry on and the tag table moves to EDX); the no-model
// lookup written once with the index masked as `(dword)parent_index & 0xffff` 44 (1390 B, the one-object form's code:
// data from EDI). Diagnostics, restored at once: `inline` on 0xb8bd0's definition with the no-model arm calling it 394
// (1373 B, re-measured; 0xbcc20 228); the same with the render arm nested inside the model test and ended by `return;`,
// the no-model code after it, 394 (1373 B: both sites still expand, so that nesting does not separate the two sites).
// An empty `do {} while (false);` between the no-model arm's two lookups 20, after the parent test ahead of the 0xbdc40
// call 20 (same code each: the two lookups are still merged and the index still dies at its mask).
// M3 lane 2 r149 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Allocator reading of the diagnostic
// with a later use of the index (re-measured 35 at 1401 B: the block is retail's row for row up to the added compare):
// there the masked value is its own value, copied from the index and then masked (ranked first in the arm, EAX), the
// parent object is placed next (ECX), and the index, still live past the object's load, is placed after both and has
// only ESI left; the pool is placed last and finds ECX free up to the object's load. In the base the index is masked
// in place and then copied, so it dies ahead of the object's load and takes ECX. So retail's index is read after the
// parent object is loaded; a forced ESI alone leaves the in-place mask (re-measured: aligned 9). Forms, none lower: the
// flag's lookup indexed by `(dword)parent_index & 0xffff` 20, by `parent_index & 0xffffu` 20, by `(long)(parent_index &
// 0xffff)` 20 (same code each: the two lookups are still merged ahead of the mask), by `(word)(parent_index & 0xffff)`
// 377 (1398 B); the flag under a nested `if (parent_index != NONE)` 44, as `parent_index != NONE && bit` 44, as
// `parent_index != NONE ? bit : false` 44 (1390 B each: the repeated test is folded early and each lookup then has its
// own block, so the data comes from EDI); the call's parent argument as `parent_index != NONE ? parent : 0` 52 (1405 B:
// the second test is emitted), its mirrored argument as `parent_index != NONE ? parent_mirrored : false` 411 (1415 B).
// Two lookups in one block re-read the data field (retail and the base); one lookup per block shares EDI's.
// M3 lane 2 r151 (fast screen; control 20 at 1399 B). Allocator reading: the no-model index ranks 78 (its load and test with
// six values live, then the in-place mask and the scaled index in the nine-value lookup block) against the pool's 34
// (four references, live and unreferenced over the model tests), so the index takes ECX first and the pool is reloaded.
// Tried: the no-model index read through a `long const &` for the test and both lookups 20, the reference for the
// matrix lookup only 20 (same code each); only the mirrored default in an else arm (parent initialised at its
// declaration) 290 (1404 B); only the parent default in an else arm 32 (1395 B); the flag passed to the 0xbdc40 call as
// `parent_index != NONE && bit` with only the matrix under the test 42 (1395 B).
// M3 lane 3 r310 (fast screen with 0xb8bd0, 0xbdc40, 0xbc820, 0xbd020, 0xbee60, 0xbfa40 and 0xbdb60, all MATCH; control 20
// at 1399 B): the no-model index assigned inside its test 20; a parent-object local with the node read into a named
// long after it, flag through object_get 44 (1390 B); one parent header pointer for both reads 41 (1392 B); the matrix
// through object_header_get's object with the flag through object_get 20, the reverse 20 (same code each). Allocator
// reading: the no-model index ranks 78 and takes ECX, the first free register at its turn (EAX and EDX hold the two
// defaults), so the pool loaded at the current-object lookup is reloaded.
// M3 lane 1 r170 (h2m fast with 0xb8bd0, 0xbdc40, 0xbc820, 0xbd020, 0xbee60, 0xbfa40 and 0xbdb60, all MATCH; control 20 at
// 1399 B). BLOCKED-BY 0xb8bd0: retail's arm reads as that call expanded at link time (the pool kept, data re-read, node
// read after the parent object), which its file's flags rule out here. Forms: the flag read first, then the matrix,
// both through the accessors 25; the parent index and node read through the object view's typed fields 20; the node
// in a named long ahead of the matrix 20; the matrix through object_header_get's object with the flag through
// object_get 20 (re-measured); one parent-object local for both reads 44 (1390 B).
// M3 lane 7 r232 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B). Allocator reading: the pool value is
// one candidate from the entry lookup to the no-model arm (34: five referencing blocks, seven it only lives through); the
// arm's index (78: test block of six values, lookup block of nine) ties the parent default (78) and loses the tie, so it
// takes ECX, the first register free at its turn; the pool would need 45 more, or the index 45 less, to keep ECX. Forms
// in the arm: `long const` index 20; the mirrored default declared ahead of the parent 22; the index declared ahead of
// both defaults 20; the test as `NONE != parent_index` 20; the index typed `dword` 20 (same code but the 22).
// M3 lane 7 r234 (fast screen; control 20 at 1399 B). Allocator reading: the pool is one value from the entry lookup
// to the no-model arm (ranked 34: the entry lookup, the early-return test, the current-object lookup and the arm's
// lookup), the arm's parent default and index rank equal at 78 (the default is the later one, so it takes EAX first and
// the index ECX). Forms in the arm: the object's own mirrored flag named at the arm top for the 0xbdc40 call 92 (1403
// B); the scale named there 100 (1411 B); the parent's flags word in a named dword ahead of the bit test 20; the parent
// node in a named long at the arm top 20; the index and the node both read at the arm top ahead of the defaults 20
// (same code each).
// M3 lane 6 r252 (fast screen with 0xb8bd0, 0xbdc40, 0xbfa40 and 0xbdb60, all MATCH; control 20 at 1399 B). Forms on
// the pool's and the arm index's references: the parent index read from the object at each of its three uses (no
// local) 20; that with the flag read ahead of the matrix 25; the early-return test reading the entry header's object
// 365 (1386 B); the current object taken from the entry header 386 (1408 B); the parent default and mirrored flag
// declared once ahead of the render test for both arms 391 (1411 B). All restored.
// M3 lane 2 r161 (h2m fast with 0xb8bd0/0xbdc40, both MATCH; control 20 at 1399 B): on lane 3's else-arm defaults form,
// one parent-object local for the matrix and the flag 45 (1391 B), the matrix through the existing 0xb8bd0 48 (1387 B),
// the flag read first through object_get with the defaults in the same order 18 (1394 B, that form's code); on the base,
// one parent-object local with the flag read first 43 (1390 B), the parent-object local for the matrix only 44 (1390 B).
// @retail 0xbd090
void __stdcall function_bd090(long object_index)
{
    s_object_transform_header *header = &((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff];
    byte *object = (byte *)header->object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long node_count = (dword)*(short *)(object + 0x114) / sizeof(transform4x3f);
    transform4x3f *matrices = (transform4x3f *)(object + *(short *)(object + 0x116));
    *(dword *)(object + 4) &= ~0x40000000;
    bool stored = true;
    if (((1 << definition[0]) & 1) && ((byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object)[0x34b] == 2) return;
    dword mask[8];
    memset(mask, 0, ((node_count + 31) >> 5) * sizeof(dword));
    long component_index = *(long *)(object + 0xb4);
    if (component_index != NONE && (bool)((*(word *)(object + 0xc0) >> 6) & 1))
    {
        byte *component = g_51e9b8->data + (component_index & 0xffff) * 0xa0;
        real amount = ((1 << definition[0]) & 1) ? function_e1670(object_index) : 0.0f;
        function_1d3920(component, mask, amount);
    }
    byte *current = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
    rigid_transform_scaled *orientations;
    if (*(short *)(current + 0x112) != NONE)
        orientations = (rigid_transform_scaled *)(object + *(short *)(object + 0x112));
    else
    {
        orientations = g_4dc2f0;
        stored = false;
    }
    long model_index = *(long *)(definition + 0x38);
    byte *model = 0;
    long render_index = NONE;
    if (model_index != NONE)
    {
        model = g_4e3b44[model_index & 0xffff].bytes;
        render_index = *(long *)(model + 4);
    }
    if (render_index != NONE)
    {
        byte *render_model = g_4e3b44[render_index & 0xffff].bytes;
        transform4x3f const *parent = 0;
        bool parent_mirrored = false;
        bool root_absolute = false;
        long parent_index = *(long *)(object + 0x14);
        if (parent_index != NONE)
        {
            parent = function_b8bd0(parent_index, *(signed char *)(object + 0x18));
            parent_mirrored = (bool)((*(dword *)((byte *)object_get(parent_index) + 4) >> 10) & 1);
        }
        if (!((bool)((*(dword *)(current + 4) >> 29) & 1))
            && (!((1 << definition[0]) & 1) || !current[0x34b]))
        {
            render_model_get_default_orientations((s_render_model_definition const *)render_model, orientations);
            if (function_bf5a0(object_index) && !function_bfc90(object_index))
            {
                s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
                if (*(long *)((byte *)state + 0x68) != NONE)
                {
                    if (!(g_55eed4 & 1)) g_55eed4 |= 1;
                    dword *sample_mask = 0;
                    if (stored && !((s_bit_vector const *)(model + 0xa4))->is_empty())
                    {
                        memcpy(g_55eeb4, model + 0xa4, sizeof(g_55eeb4));
                        ((s_bit_vector *)g_55eeb4)->function_0bfe80();
                        ((s_bit_vector *)g_55eeb4)->function_0bfed0(node_count);
                        sample_mask = g_55eeb4;
                    }
                    function_bdb60(object_index, (s_16760c_render_model *)render_model, state,
                        (long)sample_mask, node_count, (byte *)orientations);
                    function_0bfe20((dword *)(object + 4), 30, sample_mask != 0);
                    if (*(long *)((byte *)state + 0x68) != NONE && function_0b6780((s_index_triple const *)state))
                        root_absolute = (bool)((*(byte *)((byte *)state->channels[0].function_1c6440() + 0x16) >> 1) & 1);
                }
            }
        }
        if (!*(long *)(render_model + 0x48)) orientations[0] = *(rigid_transform_scaled *)g_4687d8;
        if (!bit_vector_test(mask, 0))
        {
            if (root_absolute)
                function_1421f0(matrices, &orientations[0]);
            else
            {
                point3f position = *(point3f *)(object + 0x64);
                vector3f forward = *(vector3f *)(object + 0x70);
                vector3f up = *(vector3f *)(object + 0x7c);
                if (*(long *)(object + 0xd4) != NONE && (object[0xd8] & 1))
                {
                    point3f position_result;
                    if (function_a98b0(object_index, (vector3f *)&position_result)) position = position_result;
                    vector3f forward_result, up_result;
                    // 0xa9940's first output vector goes to the one read from +0x70
                    if (function_a9940(object_index, &forward_result, &up_result))
                    {
                        forward = forward_result;
                        up = up_result;
                    }
                }
                transform4x3f base, root;
                function_bdc40(&position, &forward, &up, *(real *)(object + 0xa0),
                    (bool)((*(dword *)(object + 4) >> 10) & 1), parent, parent_mirrored, &base);
                function_1421f0(&root, &orientations[0]);
                function_142a60(&base, &root, matrices);
            }
            object_dispatch_adjust_root_matrix(object_index, (long)matrices);
        }
        if (*(long *)(render_model + 0x48) > 0)
        {
            for (long node = 1; node < *(long *)(render_model + 0x48); node++)
            {
                byte *render_node = *(byte **)(render_model + 0x4c) + node * 0x60;
                if (!bit_vector_test(mask, node))
                {
                    function_1421f0(&matrices[node], &orientations[node]);
                    function_142a60(&matrices[*(short *)(render_node + 4)], &matrices[node], &matrices[node]);
                }
            }
        }
    }
    else
    {
        transform4x3f const *parent = 0;
        bool parent_mirrored = false;
        long parent_index = *(long *)(object + 0x14);
        if (parent_index != NONE)
        {
            parent = object_node_matrix(object_get(parent_index), *(signed char *)(object + 0x18));
            parent_mirrored = (bool)((*(dword *)((byte *)object_get(parent_index) + 4) >> 10) & 1);
        }
        function_bdc40((point3f *)(object + 0x64), (vector3f *)(object + 0x70), (vector3f *)(object + 0x7c),
            *(real *)(object + 0xa0), (bool)((*(dword *)(object + 4) >> 10) & 1), parent, parent_mirrored, matrices);
    }
}
#endif


s_orientations_storage g_orientations_storage;

s_matrices_storage g_matrices_storage;

s_sample_mask_storage g_sample_mask_storage;
