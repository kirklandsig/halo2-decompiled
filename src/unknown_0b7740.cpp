#include "unknown_0b68c0.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include "placement_identifier.h"
#include <string.h>
#include <xmmintrin.h>

// @flags /O2 /Ob1 /arch:SSE /Gr
#include "object_masks_storage.h"
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

// Activating this body changes shared helper register conventions and loses
// seven existing exact matches. Preserve the closest draft until those
// helper conventions can be reconciled with their owners.
#if 0

__declspec(noinline) bool function_0bfe60(dword const *mask, long node);
bool unit_animation_periodic_layer_active(long object_index);
bool function_10edd0(long object_index);
__declspec(noinline) void function_10ec00(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
__declspec(noinline) void function_10e530(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
void function_1132f0(long object_index, dword const *node_mask, long count, transform4x3f *matrices);
void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);


__declspec(noinline) transform4x3f const *__stdcall function_bd610(long object_index,
	byte const *nodes, transform4x3f const *input_matrices);

// Retail 0xbd610
transform4x3f const *__stdcall function_bd610(long object_index, byte const *nodes,
	transform4x3f const *input_matrices)
{
	transform4x3f *result = (transform4x3f *)input_matrices;
	word header_index = (word)object_index;
	s_object_transform_header *header = &((s_object_transform_header *)g_4e0300->data)[header_index];
	byte *object = (byte *)header->object;
	transform4x3f *matrices = 0;
	rigid_transform_scaled *orientations = 0;
	if (*(short *)(object + 0x112) == NONE)
		return input_matrices;
	if (object_index != NONE && object[0xb3] > 0)
		return input_matrices;
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	long model_index = *(long *)(definition + 0x38);
	if (model_index == NONE)
		return input_matrices;
	s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
	long node_count = *(short *)(object + 0x110) / sizeof(rigid_transform_scaled);
	byte *model = g_4e3b44[model_index & 0xffff].bytes;
	byte *render_model = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
	rigid_transform_scaled *object_orientations = (rigid_transform_scaled *)(object + *(short *)(object + 0x112));
	dword mask[8];
	memcpy(mask, model + 0xa4, sizeof(mask));
	volatile bool sample = false;
	if ((bool)((*(dword *)(object + 4) >> 30) & 1) &&
		!((bool)((*(dword *)((byte *)header->object + 4) >> 29) & 1)) &&
		(!((1 << definition[0]) & 1) || !((byte *)header->object)[0x34b]))
	{
		long render_node_count = *(long *)(render_model + 0x1c);
		for (long node = 0; node < render_node_count; ++node)
		{
			byte index = nodes[node];
			if (index != 0xff && function_0bfe60((dword const *)(model + 0xc4), index))
				sample = true;
			if (sample)
				break;
		}
		if (sample)
		{
			*(dword *)(object + 4) &= 0xbfffffff;
			function_bdb60(object_index, (s_16760c_render_model *)render_model, state,
				(long)(model + 0xa4), node_count, (byte *)object_orientations);
			orientations = object_orientations;
			matrices = (transform4x3f *)input_matrices;
			result = matrices;
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
		result = g_4d8f20;
		orientations = g_4dc2f0;
	}
	if (matrices && orientations)
	{
		for (long node = 1; node < node_count; ++node)
		{
			if (mask[node >> 5] & (1 << (node & 31)))
			{
				byte *render_node = *(byte **)(render_model + 0x4c) + node * 0x60;
				function_1421f0(&matrices[node], &orientations[node]);
				function_142a60(&matrices[*(short *)(render_node + 4)], &matrices[node], &matrices[node]);
			}
		}
		result = matrices;
	}
	if (result && ((1 << ((byte *)header)[3]) & 3))
		function_1132f0(object_index, (dword const *)(model + 0xa4), node_count, result);
	return result;
}

#endif

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
bool function_bfc90(long object_index);
struct s_bit_vector
{
    dword bits[8];
    bool is_empty() const;
};
void function_0bfe80(dword *mask);
void function_0bfed0(long node_count, dword *mask);
void function_0bfe20(dword *flags, long bit, bool value);
struct s_index_triple;
bool function_0b6780(s_index_triple const *triple);

void object_dispatch_adjust_root_matrix(long object_index, long matrices);

void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);
bool function_a98b0(long index, vector3f *position);
bool function_a9940(long index, vector3f *up, vector3f *forward);

// score 234 (was 234): retail is 11 bytes longer. Left: retail sets the sample-mask
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
            byte *local_be682a_3 = (byte *)((s_object_transform_header *)g_4e0300->data)[parent_index & 0xffff].object;
            parent_mirrored = (bool)((*(dword *)(local_be682a_3 + 4) >> 10) & 1);
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
                        function_0bfe80(g_55eeb4);
                        function_0bfed0(node_count, g_55eeb4);
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
        if (!(mask[0] & 1))
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
        for (long node = 1; node < *(long *)(render_model + 0x48); node++)
        {
            byte *render_node = *(byte **)(render_model + 0x4c) + node * 0x60;
            if (!(mask[node >> 5] & (1 << (node & 31))))
            {
                function_1421f0(&matrices[node], &orientations[node]);
                function_142a60(&matrices[*(short *)(render_node + 4)], &matrices[node], &matrices[node]);
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
            byte *local_be682a_3 = (byte *)((s_object_transform_header *)g_4e0300->data)[parent_index & 0xffff].object;
            parent = (transform4x3f *)(local_be682a_3 + *(short *)(local_be682a_3 + 0x116)) + *(signed char *)(object + 0x18);
            parent_mirrored = (bool)((*(dword *)(local_be682a_3 + 4) >> 10) & 1);
        }
        function_bdc40((point3f *)(object + 0x64), (vector3f *)(object + 0x70), (vector3f *)(object + 0x7c),
            *(real *)(object + 0xa0), (bool)((*(dword *)(object + 4) >> 10) & 1), parent, parent_mirrored, matrices);
    }
}
#endif


s_orientations_storage g_orientations_storage;

s_matrices_storage g_matrices_storage;

s_sample_mask_storage g_sample_mask_storage;
