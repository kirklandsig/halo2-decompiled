#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include <string.h>
#include <xmmintrin.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

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
	s_velocity_object *object = ((s_velocity_object_header *)objects->data)[object_index].object;
	dword update_mask = object_velocity_write(object, linear_velocity, angular_velocity);
	if (!skip_update && update_mask)
	{
		long index = ((s_velocity_object_header *)objects->data)[object_index].object->synchronization_index;
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

extern void *g_4de2e0;
extern void *g_4de2e4;
extern void *g_4de2d4;
extern void *g_4de2d8;

extern void *g_4de2e8;

struct s_cluster_partition
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};

void function_1cadf0(s_cluster_partition *partition, long reference_index, long data_index, long payload_size, void const *payload);

// @retail 0xb8b70
void function_b8b70(long object_index)
{
    s_object_partition_header_ab *header = &((s_object_partition_header_ab *)g_4e0300->data)[object_index & 0xffff];
    s_object_partition_view_ab *object = header->object;
    if ((*((byte *)header + 2) & 0x40) && (bool)((object->flags >> 9) & 1))
    {
        s_object_partition_record_ab record;
        function_b88e0(object_index, &record);
        s_cluster_partition partition =
        {
            (long *)g_4de2e0,
            (s_record_pool *)g_4de2e4,
            (s_record_pool *)g_4de2e8
        };
        function_1cadf0(&partition, *(long *)((byte *)object + 0x60), object_index, sizeof(record), &record);
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
long function_b8940(short cluster_index, s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
	short const *cluster_reference = &cluster_index;
	iterator->next = ((long *)g_4de2e0)[*cluster_reference];
	return object_cluster_next_ab((s_record_pool *)g_4de2e4, iterator, record);
}

// @retail 0xb89b0
long function_b89b0(s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
	return object_cluster_next_ab((s_record_pool *)g_4de2e4, iterator, record);
}

// @retail 0xb8a10
long function_b8a10(short cluster_index, s_object_cluster_reference **record, s_object_cluster_iterator *iterator)
{
	short const *cluster_reference = &cluster_index;
	iterator->next = ((long *)g_4de2d4)[*cluster_reference];
	return object_cluster_next_ab((s_record_pool *)g_4de2d8, iterator, record);
}

struct s_placement_defaults_ab
{
	long tag_index;
	long unique_id;
	short bsp_index;
	char type;
	char source;
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
	byte flags_c0;
	byte unknownc1[0x138 - 0xc1];
	short team;
	byte unknown13a[2];
	long player_index;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

// @retail 0xb7930
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner)
{
	s_placement_defaults_ab *placement = (s_placement_defaults_ab *)data;
	memset(placement, 0, sizeof(*placement));
	placement->tag_index = tag_index;
	placement->field_0c = 0;
	placement->flags = 0;
	placement->forward = *g_4687a8;
	placement->up = *g_4687b0;
	placement->scale = 1.0f;
	placement->field_74 = 0;
	placement->field_b4 = NONE;
	placement->bsp_policy = 0;
	s_placement_source_ab *object = (s_placement_source_ab *)function_badc0(object_index, NONE);
	placement->team = NONE;
	placement->player_index = NONE;
	if (object)
	{
		dword flags = *(volatile dword *)&placement->flags;
		placement->object_index = object_index;
		if ((bool)(((dword)object->flags_c0 >> 2) & 1))
			flags |= 0x20;
		else
			flags &= ~0x20;
		placement->flags = flags;
		if ((1 << object->type) & 3)
		{
			placement->player_index = object->player_index;
			placement->team = object->team;
		}
	}
	else
	{
		placement->object_index = NONE;
	}
	if (owner)
		placement->owner = *owner;
	else
	{
		placement->owner.unknown4 = NONE;
		placement->owner.unknown0 = NONE;
		placement->owner.unknown8 = NONE;
	}
	placement->type = NONE;
	placement->source = NONE;
	placement->bsp_index = NONE;
	placement->unique_id = NONE;
	placement->scenario_index = NONE;
	placement->field_b8 = 0;
}

struct s_scenario_identifier_ab
{
	long unique_id;
	short origin_bsp;
	char type;
	char source;
};

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

// @retail 0xb7a40
void *__stdcall function_b7a40(s_scenario_identifier_ab const *identifier, long *index_out)
{
	void *result = 0;
	char source = identifier->source;
	if (source == 1 || source == 0)
	{
		char type = identifier->type;
		s_scenario_type_ab *definition = (s_scenario_type_ab *)g_468630[type];
		if (definition->block_offset != NONE)
		{
			long size = definition->element_size;
			s_scenario_block_ab *block = (s_scenario_block_ab *)((byte *)g_4e0350 + definition->block_offset);
			long count = block->count;
			byte *element = block->elements;
			for (long i = 0; i < count; i++, element += size)
			{
				s_scenario_identifier_ab *current = (s_scenario_identifier_ab *)(element + 0x28);
				bool match = (current->unique_id == identifier->unique_id) & (current->type == type) & (current->source == source);
				if (match && current->source == 0)
					match &= current->origin_bsp == identifier->origin_bsp;
				if (match)
				{
					if (index_out)
						*index_out = i;
					return element;
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
    rotation.left.i = up->j * forward->k - forward->j * up->k;
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
        identity_transform_ab(&temporary);
        temporary.scale = scale;
        function_142a60(&rotation, &temporary, &rotation);
    }
    transform4x3f const *base;
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
        base = out;
    }
    else
        base = &translation;
    function_142a60(base, &rotation, out);
}

extern long g_4e7414;
extern bool g_4e7411;
extern long g_4de2fc;
extern bool g_4de2f8;
extern long g_4de300[0x800];
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, long maximum_count, short *clusters);

static inline bool cluster_sphere_accept_ab(long object, s_object_cluster_reference const *record, dword type_mask, point3f const *position, real radius)
{
    if (type_mask & (1 << record->type))
    {
        long index = object & 0xffff;
        if (g_4de300[index] != g_4de2fc)
        {
            g_4de300[index] = g_4de2fc;
            real z = record->center.z - position->z;
            real y = record->center.y - position->y;
            real x = record->center.x - position->x;
            real combined = record->radius + radius;
            return combined * combined >= z * z + y * y + x * x;
        }
    }
    return false;
}

// @retail 0xbb050
short __stdcall function_bb050(long mask, dword type_mask, void const *location, point3f const *position, real radius, long *objects, short maximum_count)
{
    short volatile count = 0;
    if (!type_mask) type_mask = 0xffffffff;
    if (!mask) mask = NONE;
    volatile short cluster = *(short *)((byte const *)location + 4);
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
            if (cluster_count > 0x200) cluster_count = 0x200;
        }
        else
        {
            cluster_count = 1;
            clusters[0] = cluster;
        }
    }
    ++g_4de2fc;
    g_4de2f8 = true;
    for (short i = 0; i < cluster_count; ++i)
    {
        short cluster = clusters[i];
        s_object_cluster_reference *record = 0;
        if (mask & 1)
        {
            s_object_cluster_iterator iterator;
            for (long object = function_b8940(cluster, &record, &iterator); object != NONE;
                 object = object_cluster_next_ab((s_record_pool *)g_4de2e4, &iterator, &record))
            {
                if (cluster_sphere_accept_ab(object, record, type_mask, position, radius))
                {
                    if (count >= maximum_count) goto done;
                    objects[count++] = object;
                }
            }
        }
        record = 0;
        if (mask & 2)
        {
            s_object_cluster_iterator iterator;
            for (long object = function_b8a10(cluster, &record, &iterator); object != NONE;
                 object = object_cluster_next_ab((s_record_pool *)g_4de2d8, &iterator, &record))
            {
                if (cluster_sphere_accept_ab(object, record, type_mask, position, radius))
                {
                    if (count >= maximum_count) goto done;
                    objects[count++] = object;
                }
            }
        }
    }
done:
    g_4de2f8 = false;
    return count;
}

#include "unknown_1cafc0.h"
#include <math.h>
struct s_16760c_render_model;
bool __stdcall function_bab40(long object_index, long name, real *value);

// @retail 0xbd970
void __stdcall function_bd970(long object_index, s_16760c_render_model *render_model, s_animation_state *state, long node_mask, long node_count, byte *orientations)
{
    short index = NONE;
    c_animation_channel channel;
    for (;;)
    {
        s_graph_tag *graph = graph_tag_get(state->graph_tag_index);
        short next = index + 1;
        if (next >= graph->unknown44_count)
            break;
        index = next;
        s_graph_element44 *entry = &graph->unknown44[index];
        c_type_709360 animation_id = entry->animation_id;
        long name = *(long *)(entry->unknown08 + 4);
        short kind = *(short *)(entry->unknown08 + 2);
        if (animation_id.index != NONE)
            function_1dd9d0(graph, animation_id);
        if (name && state->graph_tag_index != NONE && state->channel_start(&channel, animation_id, NONE, NONE, NONE, NONE, 0x7f))
        {
            s_animation *animation = 0;
            if (channel.animation_id.index != NONE)
                animation = function_1daea0(graph_tag_get(channel.graph_tag_index), channel.animation_id);
            real value;
            function_bab40(object_index, name, &value);
            if (kind == 0)
            {
                channel.set_frame_position((animation->frame_count - 1) * value);
                channel.sample(1.0f, (dword const *)node_mask, node_count, (real_quaternion_transform *)orientations);
            }
            else if (kind == 1)
            {
                real frame = (real)fmod((double)((dword)(g_510c54->game_time + object_index)) * g_510c54->rate * 0.03333333507180214f, (double)animation->frame_count);
                channel.set_frame_position(frame);
                channel.sample(value, (dword const *)node_mask, node_count, (real_quaternion_transform *)orientations);
            }
        }
    }
}

struct rigid_transform_scaled
{
    quaternionf rotation;
    point3f position;
    real scale;
};
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
    transform4x3f *matrices = (transform4x3f *)(object + *(short *)(object + 0x116));
    rigid_transform_scaled *orientations = (rigid_transform_scaled *)(object + *(short *)(object + 0x112));
    long count = (dword)(long)*(short *)(object + 0x110) / sizeof(rigid_transform_scaled);
    long parent_index = *(long *)(object + 0x14);
    transform4x3f *parent = 0;
    bool parent_mirrored = false;
    if (parent_index != NONE)
    {
        parent = function_b8bd0(parent_index, *(char *)(object + 0x18));
        byte *local_be682a = (byte *)((s_object_transform_header *)g_4e0300->data)[parent_index & 0xffff].object;
        parent_mirrored = (*(dword *)(local_be682a + 4) >> 10) & 1;
    }
    transform4x3f root, inverse_root, inverse_parent, relative;
    function_bdc40((point3f *)(object + 0x64), (vector3f *)(object + 0x70), (vector3f *)(object + 0x7c),
        *(real *)(object + 0xa0), (*(dword *)(object + 4) >> 10) & 1, parent, parent_mirrored, &root);
    function_141590(&root, &inverse_root);
    for (long i = 0; i < count; ++i)
    {
        if (!node_mask || (((dword *)node_mask)[i >> 5] & (1 << (i & 31))))
        {
            short node_parent = *(short *)(*(byte **)(model + 0x7c) + i * 0x5c + 4);
            if (node_parent != NONE)
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
void function_109050(long object_index, long a, long b, long c);
struct s_1d9240;
void function_1d9470(s_1d9240 const *p, s_blend_orientation const *targets, long count, dword const *mask,
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
        function_109050(object_index, node_mask, node_count, (long)orientations);
        if (state->unknown64.unknown1 && !(state->unknown64.unknown3 & 2))
        {
            byte *object = (byte *)((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff].object;
            s_blend_orientation *targets = (s_blend_orientation *)(object + *(short *)(object + 0x10e));
            function_1d9470((s_1d9240 const *)&state->unknown64, targets, (short)node_count,
                (dword const *)node_mask, (s_blend_orientation *)orientations);
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
void function_109290(long object_index, long position, long forward, long up);
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
    function_109290(object_index, (long)position_copy, (long)forward_copy, (long)up_copy);
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
bool function_10db60(long object_index);
bool function_10edd0(long object_index);
__declspec(noinline) void function_10ec00(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
__declspec(noinline) void function_10e530(long object_index, s_animation_state *state, long unused,
	dword const *node_mask, long count, real_quaternion_transform *orientations, byte *mask);
void function_1132f0(long object_index, dword const *node_mask, long count, transform4x3f *matrices);
void __stdcall function_1421f0(transform4x3f *out, rigid_transform_scaled const *orientation);

rigid_transform_scaled g_4dc2f0[255];
transform4x3f g_4d8f20[255];

__declspec(noinline) transform4x3f const *__stdcall function_bd610(long object_index,
	byte const *nodes, transform4x3f const *input_matrices);

// Retail 0xbd610; its original stub remains in src/stubs/lane_p.cpp.
transform4x3f const *__stdcall function_bd610(long object_index, byte const *nodes,
	transform4x3f const *input_matrices)
{
	s_object_transform_header *header = &((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff];
	byte *object = (byte *)header->object;
	transform4x3f *result = (transform4x3f *)input_matrices;
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
		(function_10db60(object_index) || function_10edd0(object_index)))
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
// @retail 0xbc070
void __stdcall function_bc070(real a, real b, real c, real d, real e)
{
    (void)&a; (void)&b; (void)&c; (void)&d; (void)&e;
    volatile real *state = (volatile real *)g_4de2f4;
    real horizontal = a * 0.01745329238474369f;
    real vertical = b * 0.01745329238474369f;
    state[0x60 / 4] = (real)(cos(horizontal) * cos(vertical));
    state[0x64 / 4] = (real)(sin(horizontal) * cos(vertical));
    state[0x68 / 4] = (real)sin(vertical);
    state[0x60 / 4] = 0.0f - state[0x60 / 4];
    state[0x64 / 4] = 0.0f - state[0x64 / 4];
    state[0x68 / 4] = 0.0f - state[0x68 / 4];
    state[0x54 / 4] = c;
    state[0x58 / 4] = d;
    state[0x5c / 4] = e;
}


// @retail 0xbbfc0
void __stdcall function_bbfc0(real a, real b, real c, real d, real e)
{
    (void)&a; (void)&b; (void)&c; (void)&d; (void)&e;
    volatile real *state = (volatile real *)g_4de2f4;
    real horizontal = a * 0.01745329238474369f;
    real vertical = b * 0.01745329238474369f;
    state[0x48 / 4] = (real)(cos(horizontal) * cos(vertical));
    state[0x4c / 4] = (real)(sin(horizontal) * cos(vertical));
    state[0x50 / 4] = (real)sin(vertical);
    state[0x48 / 4] = 0.0f - state[0x48 / 4];
    state[0x4c / 4] = 0.0f - state[0x4c / 4];
    state[0x50 / 4] = 0.0f - state[0x50 / 4];
    state[0x3c / 4] = c;
    state[0x40 / 4] = d;
    state[0x44 / 4] = e;
    *(vector3f *)((byte *)state + 0x28) = *(vector3f *)((byte *)state + 0x48);
    state[0x34 / 4] = 0.8f;
    state[0x38 / 4] = 0.8f;
    *(volatile short *)((byte *)state + 0x6c) = NONE;
}

void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disabled);
// @retail 0xb77d0
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity)
{
    vector3f const *volatile *angular_reference = &angular_velocity;
    function_b7740(object_index, linear_velocity, *angular_reference, false);
    function_1c4b00(object_index, (void *)linear_velocity, (void *)*angular_reference, 1);
    if ((*angular_reference && (*angular_reference)->i * (*angular_reference)->i + (*angular_reference)->j * (*angular_reference)->j + (*angular_reference)->k * (*angular_reference)->k > 0.0001f) ||
        (linear_velocity && linear_velocity->i * linear_velocity->i + linear_velocity->j * linear_velocity->j + linear_velocity->k * linear_velocity->k > 0.0001f))
    {
        function_b9b90(object_index, false);
        function_b7360(object_index);
        function_bba20(object_index);
    }
}


// Disabled: retail passes persistent shared orientation/mask storage and reaches 0x142a60's unbound inputs.
#if 0
extern dword g_55eed4, g_55eeb4[8];
extern rigid_transform_scaled *g_4687d8;
void __stdcall function_1d3920(void *component, dword *mask, real amount);
real function_e1670(long object_index);
void function_16d940(void *render_model, void *orientations);
bool function_bf5a0(long object_index);
bool function_bfc90(long object_index);
bool function_bff10(dword const *mask);
void function_bfe80(dword *mask);
void function_bfed0(dword *mask, long node_count);
bool function_b6780(s_animation_state *state);
void *function_1c6440(s_animation_state *state);
void function_1090d0(long object_index, long matrices);

// Retail 0xbd090
void __stdcall function_bd090(long object_index)
{
    s_object_transform_header *header = &((s_object_transform_header *)g_4e0300->data)[object_index & 0xffff];
    byte *object = (byte *)header->object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    long node_count = (word)*(short *)(object + 0x114) / sizeof(transform4x3f);
    transform4x3f *matrices = (transform4x3f *)(object + *(short *)(object + 0x116));
    *(dword *)(object + 4) &= ~0x40000000;
    if (((1 << definition[0]) & 1) && ((byte *)header->object)[0x34b] == 2) return;
    dword mask[8];
    memset(mask, 0, ((node_count + 31) >> 5) * sizeof(dword));
    long component_index = *(long *)(object + 0xb4);
    if (component_index != NONE && (bool)(((dword)object[0xc0] >> 6) & 1))
    {
        byte *component = g_51e9b8->data + (component_index & 0xffff) * 0xa0;
        real amount = ((1 << definition[0]) & 1) ? function_e1670(object_index) : 0.0f;
        function_1d3920(component, mask, amount);
    }
    byte *current = (byte *)header->object;
    bool stored = *(short *)(current + 0x112) != NONE;
    rigid_transform_scaled *orientations = stored
        ? (rigid_transform_scaled *)(object + *(short *)(object + 0x112)) : g_4dc2f0;
    long model_index = *(long *)(definition + 0x38);
    byte *model = model_index == NONE ? 0 : g_4e3b44[model_index & 0xffff].bytes;
    long render_index = model ? *(long *)(model + 4) : NONE;
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
            function_16d940(render_model, orientations);
            if (function_bf5a0(object_index) && !function_bfc90(object_index))
            {
                s_animation_state *state = (s_animation_state *)(object + *(short *)(object + 0x12a));
                if (*(long *)((byte *)state + 0x68) != NONE)
                {
                    if (!(g_55eed4 & 1)) g_55eed4 |= 1;
                    dword *sample_mask = 0;
                    if (stored && !function_bff10((dword *)(model + 0xa4)))
                    {
                        memcpy(g_55eeb4, model + 0xa4, sizeof(g_55eeb4));
                        function_bfe80(g_55eeb4);
                        function_bfed0(g_55eeb4, node_count);
                        sample_mask = g_55eeb4;
                    }
                    function_bdb60(object_index, (s_16760c_render_model *)render_model, state,
                        (long)sample_mask, node_count, (byte *)orientations);
                    if (sample_mask) *(dword *)(object + 4) |= 0x40000000;
                    else *(dword *)(object + 4) &= ~0x40000000;
                    if (*(long *)((byte *)state + 0x68) != NONE && function_b6780(state))
                        root_absolute = (bool)((*(byte *)((byte *)function_1c6440(state) + 0x16) >> 1) & 1);
                }
            }
        }
        if (!*(long *)(render_model + 0x48)) orientations[0] = *g_4687d8;
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
                    if (function_a98b0(object_index, &position_result)) position = position_result;
                    vector3f forward_result, up_result;
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
            function_1090d0(object_index, (long)matrices);
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

