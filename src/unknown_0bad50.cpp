#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_0bad50.h"
#include "object_iterator.h"
#include "unknown_16d180.h"
#include <string.h>

// @flags /O2 /Gr
#include "object_masks_storage.h"
#include "partition_colliding_storage.h"
#include "partition_noncolliding_storage.h"

struct s_object_blocks_ab
{
	long tag_index;
	byte unknown04[0xb1 - 4];
	char variant;
	byte unknownb2[0x118 - 0xb2];
	short regions_size;
	short regions_offset;
	byte unknown11c[0x12a - 0x11c];
	short animation_offset;
};

struct s_object_blocks_header_ab
{
	byte unknown00[8];
	s_object_blocks_ab *object;
};

void __stdcall function_ba6f0(long object_index, long region_index, long state, bool flag);

// @retail 0xbf890
void function_bf890(long object_index, byte const *states)
{
	short regions_size;
	byte *data = g_4e0300->data;
	s_object_blocks_ab volatile *object = ((s_object_blocks_header_ab *)data)[(long)object_index & 0xffff].object;
	regions_size = object->regions_size;
	byte const *values;
	long count = regions_size / 10;
	values = *&states;
	for (long i = 0; count > i; ++i)
	{
		long state;
		state = values[i];
		function_ba6f0(object_index, i, state, false);
	}
}

struct s_object_variant_definition_ab
{
	byte unknown00[0x30];
	string_handle variant;
	byte unknown34[4];
	long model_index;
};

void function_b7360(long object_index);

__declspec(noinline) void function_ba3d0(long object_index);

// @retail 0xba3d0
void function_ba3d0(long object_index)
{
	s_object_blocks_ab *object = ((s_object_blocks_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
	byte *state = (byte *)object + object->animation_offset + 0x64;
	state[1] = 0;
	state[0] = 0;
	state[3] = 0;
	function_b7360(object_index);
}

static __forceinline s_object_blocks_ab *object_blocks_get_ab(long object_index)
{
    return ((s_object_blocks_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
}

struct s_region_change_ab
{
    char variant;
    byte state;
    byte flags;
    byte unknown03;
    long stamp;
};

void function_17b1d0(long object_index);
void function_1c54b0(char const *variants, long object_index);
void function_1c5710(long object_index);

// @retail 0xba410
void __stdcall function_ba410(long object_index, long region_name, long arg_dbe893)
{
    char previous[16];
    s_object_blocks_header_ab *header = (s_object_blocks_header_ab *)g_4e0300->data + (object_index & 0xffff);
    s_object_blocks_ab *object = header->object;
    s_object_variant_definition_ab *definition = (s_object_variant_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
    if (definition->model_index != NONE)
    {
        long region_index = function_16d1d0(definition->model_index, (string_handle)region_name);
        char *regions = (char *)object + object->regions_offset;
        long count = object->regions_size / 10;
        memcpy(previous, regions, count);
        if (((byte *)header)[3] == 6 && *(short *)((byte *)object + 0x1a) != NONE &&
            !(((byte *)object)[0x12c] & 2))
            function_1c5710(object_index);
        for (long i = 0; i < count; i++)
        {
            if (i == region_index || !region_name)
            {
                long variant = function_16d220(i, definition->model_index, (string_handle)arg_dbe893);
                if (variant != NONE || !arg_dbe893)
                {
                    regions[i] = (char)variant;
                    s_region_change_ab *change = (s_region_change_ab *)(regions + count * 2) + i;
                    change->variant = 0xff;
                    change->state = 0;
                    change->flags = 0;
                    change->stamp = NONE;
                }
            }
        }
        function_17b1d0(object_index);
        function_1c54b0(previous, object_index);
    }
}

struct s_region_permutation_choice;
bool object_change_region_permutations(long object_index, long model_index, long variant,
    long region_index, long state, bool force, long a, short b, char *regions, s_region_permutation_choice *changes);

// @retail 0xba6f0
void __stdcall function_ba6f0(long object_index, long region_index, long state, bool force)
{
    char previous[16];
    s_object_blocks_header_ab *header = (s_object_blocks_header_ab *)g_4e0300->data + (object_index & 0xffff);
    s_object_blocks_ab *object = header->object;
    s_object_variant_definition_ab *definition = (s_object_variant_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
    if (definition->model_index != NONE)
    {
        long count = object->regions_size / 10;
        char *regions = (char *)object + object->regions_offset;
        s_region_change_ab *changes = (s_region_change_ab *)(regions + count * 2);
        memcpy(previous, regions, count);
        if (((byte *)header)[3] == 6 && *(short *)((byte *)object + 0x1a) != NONE &&
            !(((byte *)object)[0x12c] & 2))
            function_1c5710(object_index);
        object_change_region_permutations(object_index, definition->model_index, object->variant, region_index,
            state, force, NONE, 0, regions, (s_region_permutation_choice *)changes);
        function_17b1d0(object_index);
        function_1c54b0(previous, object_index);
        function_b7360(object_index);
    }
}

// @retail 0xba540
void function_ba540(long object_index, string_handle name)
{
	s_object_blocks_ab *object = object_blocks_get_ab(object_index);
	s_object_variant_definition_ab *definition = (s_object_variant_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
	string_handle resolved = name;
	if (!resolved)
		resolved = definition->variant;
	object->variant = (char)function_16d180(definition->model_index, resolved);
}

/* the count comes first: callers set up the register arguments last to first, and retail's
   callers (0xba590, 0xd9f70, 0xea6b0) lea the states (edi) before the count (esi) */
// @retail 0xba690
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b)
{
	s_object_blocks_ab *object = ((s_object_blocks_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
	*state_count = object->regions_size;
	byte *regions = (byte *)object + object->regions_offset;
	*state_count /= 10;
	if (states)
		*states = regions;
	if (a)
		*a = (long)(regions + *state_count);
	if (b)
		*b = (long)(regions + 2 * *state_count);
}

void render_model_choose_permutations(long render_model_index, long variant_index,
    char *permutation_indices, s_region_permutation_choice *region_choices, dword fixed_region_mask);

// MiniMax lane B (2026-10-27): 0xba690 takes its pointers in registers (count in esi, states in edi), so
// its own bytes are the same for either parameter order; only the callers' setup order shows it.
// With the count first (coldspear, 2026-10-30) this caller matches, and so does 0xd9f70.
// @retail 0xba590
void __stdcall function_ba590(long object_index, dword fixed_region_mask)
{
    byte *states;
    long count;
    long saved_states;
    long choices;
    char previous[16];
    s_object_blocks_ab *object = object_blocks_get_ab(object_index);
    s_object_variant_definition_ab *definition = (s_object_variant_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
    function_ba690(object_index, &count, &states, &saved_states, &choices);
    memcpy(previous, states, count);
    if (definition->model_index != NONE)
        render_model_choose_permutations(definition->model_index, object->variant,
            (char *)states, (s_region_permutation_choice *)choices, fixed_region_mask);
    else
    {
        for (long i = 0; i < count; ++i)
        {
            states[i] = 0;
            s_region_change_ab *choice = &((s_region_change_ab *)choices)[i];
            choice->variant = 0xff;
            choice->state = 0;
            choice->flags = 0;
        }
    }
    memcpy((void *)saved_states, states, count);
    function_17b1d0(object_index);
    function_1c54b0(previous, object_index);
}

#define OBJECT_HEADER(index) ((s_object_header *)(g_4e0300->data + g_4e0300->size * (index)))

__declspec(noinline) bool function_bad50(long object_index, long index, point3f *out);

// @retail 0xbad50
bool function_bad50(long object_index, long index, point3f *out)
{
	s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	byte *base = object->markers_offset + (byte *)object;
	bool result = false;
	long count = (long)((dword)(long)object->markers_size / 12) / 2;

	if (index >= 0 && index < count)
	{
		*out = *(point3f *)(base + (count + index) * 12);
		result = true;
	}
	return result;
}

// @retail 0xbadc0
s_object *function_badc0(long object_index, dword type_mask)
{
	s_object_header *header = 0;

	if (object_index != NONE)
	{
		long index = object_index & 0xffff;
		if (index < g_4e0300->high_water_index)
		{
			s_object_header *h = OBJECT_HEADER(index);
			if (h->identifier && h->identifier == (object_index >> 16))
				header = h;
		}
	}

	s_object *result = 0;
	if (header && (type_mask & (1 << header->type)))
		result = header->object;
	return result;
}

// @retail 0xbae20
s_object *function_bae20(long object_index, dword type_mask)
{
	s_object_header *header = 0;

	if (object_index != NONE)
	{
		long index = object_index & 0xffff;
		if (index >= 0 && index < g_4e0300->high_water_index)
		{
			s_object_header *h = OBJECT_HEADER(index);
			if (h->identifier && h->identifier == (object_index >> 16))
				header = h;
		}
	}

	s_object *result = 0;
	if (header && (type_mask & (1 << header->type)))
		result = header->object;
	return result;
}

// @retail 0xbae80
void function_bae80(s_type_f1af8e *iterator, dword type_mask, byte flags)
{
	iterator->signature = 0x86868686;
	if (!type_mask)
		type_mask = NONE;
	iterator->type_mask = type_mask;
	iterator->flags = flags;
	iterator->index = 0;
	iterator->object_index = NONE;
}

// @retail 0xbaeb0
s_object *function_baeb0(s_type_f1af8e *iterator)
{
	short index = iterator->index;
	s_object_header *header = (s_object_header *)(g_4e0300->data + index * 12);
	s_object *result = 0;

	while (index < g_4e0300->high_water_index)
	{
		short identifier = header->identifier;
		long object_index = (identifier << 16) | index;
		index++;
		if (identifier && (header->flags & iterator->flags) == iterator->flags && (iterator->type_mask & (1 << header->type)))
		{
			iterator->object_index = object_index;
			result = header->object;
			break;
		}
		header++;
	}

	iterator->index = index;
	return result;
}

__declspec(noinline) long function_baf40(long object_index);

// @retail 0xbaf40
long function_baf40(long object_index)
{
	if (object_index != NONE)
	{
		do
		{
			s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			if (!TEST_FIELD_BIT(object->flag26))
				break;
			object_index = object->parent_index;
		}
		while (object_index != NONE);
	}
	return object_index;
}

__declspec(noinline) long function_baf80(long object_index);

// @retail 0xbaf80
long function_baf80(long object_index)
{
	long result = NONE;

	while (object_index != NONE)
	{
		result = object_index;
		object_index = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object->parent_index;
	}
	return result;
}

bool function_183910(long component_index, point3f const *origin, point3f *point, vector3f *normal);

// @retail 0xbaff0
void function_baff0(long object_index, point3f const *origin, point3f *point, vector3f *normal)
{
	byte *object = (byte *)((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	long component_index = *(long *)(object + 0xb4);
	if (component_index == NONE || !function_183910(component_index, origin, point, normal))
	{
		*point = *(point3f *)(object + 0x30);
		*normal = *g_4687b0;
	}
}

// @retail 0xbafb0
bool function_bafb0(long object_index, long ancestor_index)
{
	long current = NONE;

	while (object_index != NONE && current != ancestor_index)
	{
		current = object_index;
		object_index = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object->parent_index;
	}
	return ancestor_index != NONE && current == ancestor_index;
}

void function_3dd10(long object_index, bool force);

// @retail 0xbacc0
bool function_bacc0(long object_index, long index, point3f const *point)
{
    bool result = false;
    s_object *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    point3f *points = (point3f *)((byte *)object + object->markers_offset);
    long count = (long)((dword)(long)object->markers_size / 12) / 2;
    if (index >= 0 && index < count)
    {
        points[index] = *point;
        points[index + count] = *point;
        function_3dd10(object_index, true);
        result = true;
    }
    return result;
}

#include <xmmintrin.h>

struct s_cluster_link_ab
{
    long salt;
    long object_index;
    long next;
};

static __forceinline long cluster_link_next_ab(s_record_pool *array, long *next)
{
    long result;
    if (*next != NONE)
    {
        long size = array->size;
        byte *data = array->data;
        s_cluster_link_ab *link = (s_cluster_link_ab *)(data + (*next & 0xffff) * size);
        long following = link->next;

        if (following != NONE)
            _mm_prefetch((char const *)(data + (following & 0xffff) * size), _MM_HINT_T0);
        *next = following;
        result = link->object_index;
    }
    else
    {
        result = NONE;
    }
    return result;
}

// @retail 0xbb430
short function_bb430(long mask, short cluster_count, short const *clusters, short maximum, long *objects)
{
    short count = 0;
    if (!mask)
        mask = NONE;
    ++g_4de2fc;
    g_4de2f8 = true;
    for (short i = 0; i < cluster_count; i++)
    {
        short cluster = clusters[i];
        long next;
        long object;

        if (mask & 1)
        {
            next = ((long *)g_4de2e0)[cluster];
            object = cluster_link_next_ab((s_record_pool *)g_4de2e4, &next);
            while (object != NONE)
            {
                long index = object & 0xffff;
                if (g_4de300[index] != g_4de2fc)
                {
                    g_4de300[index] = g_4de2fc;
                    if (count >= maximum)
                    {
                        g_4de2f8 = false;
                        return count;
                    }
                    objects[count++] = object;
                }
                object = cluster_link_next_ab((s_record_pool *)g_4de2e4, &next);
            }
        }
        if (mask & 2)
        {
            next = ((long *)g_4de2d4)[cluster];
            object = cluster_link_next_ab((s_record_pool *)g_4de2d8, &next);
            while (object != NONE)
            {
                long index = object & 0xffff;
                if (g_4de300[index] != g_4de2fc)
                {
                    g_4de300[index] = g_4de2fc;
                    if (count >= maximum)
                    {
                        g_4de2f8 = false;
                        return count;
                    }
                    objects[count++] = object;
                }
                object = cluster_link_next_ab((s_record_pool *)g_4de2d8, &next);
            }
        }
    }
    g_4de2f8 = false;
    return count;
}

struct s_object_functions_definition_ab
{
    byte unknown00[0x64];
    long count;
    s_object_function_ab *functions;
};
bool object_dispatch_query_function_value(long object_index, long name, real *value, bool *enabled);
bool function_10aac0(long object_index, long name, real *value, long *index);
real function_13bb90(s_tag_data const *function, real input, real range);
bool __stdcall function_bab40(long object_index, long name, real *value);

bool function_ba8c0(s_object_function_ab const *function, long object_index, real *out);


// @retail 0xba7f0
void __stdcall function_ba7f0(long object_index, long region_index, long bit_index, long bit_mask)
{
    char previous[16];
    s_object_blocks_ab *object = ((s_object_blocks_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
    s_object_variant_definition_ab *definition = (s_object_variant_definition_ab *)g_4e3b44[object->tag_index & 0xffff].bytes;
    long model_index = *(long volatile *)&definition->model_index;
    if (model_index != NONE)
    {
        char *regions = (char *)object + object->regions_offset;
        long count = object->regions_size / 10;
        memcpy(previous, regions, count);
        object_change_region_permutations(object_index, model_index, object->variant, region_index,
            NONE, false, bit_index, (short)bit_mask, regions,
            (s_region_permutation_choice *)(regions + count * 2));
        function_17b1d0(object_index);
        function_1c54b0(previous, object_index);
    }
}
