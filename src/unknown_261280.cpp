// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_261280.CPP: choosing a reference for an actor to follow, falling
   back to the one it already follows */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"
#include <float.h>
#include "unknown_25fc30.h"

bool function_2624d0(s_261d20_entry *entry, s_reference reference);
__declspec(noinline) bool function_260160(long actor_index, s_prop_search *search, s_261d20_entry *entry);

/* the request as function_261280 reads it */
struct s_prop_search_request_view
{
	byte unknown000[0x15];
	bool unknown015;
	byte unknown016[0x618 - 0x16];
	bool unknown618;
	byte unknown619[0x620 - 0x619];
	point3f point;
};

PRIVATE __forceinline real point_distance_squared_ordered(point3f const *from, point3f const *to)
{
	vector3f delta;
	vector3d_from_points3d(from, to, &delta);
	real result = delta.k * delta.k;
	result += delta.i * delta.i;
	result += delta.j * delta.j;
	return result;
}

// @retail 0x261280
s_reference function_261280(s_prop_search *search, long actor_index, s_261d20_entry *entry, long *b, byte *buffer, bool *c)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_search_request_view *request = (s_prop_search_request_view *)search;
	s_reference reference;

	if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) && actor->unknown3f0)
	{
		request->unknown015 = false;
	}
	else
	{
		request->unknown015 = true;
	}
	reference = function_2605d0(actor_index, (s_2605d0_request const *)search, (long)entry, (long)b, buffer, c);
	if (REFERENCE_EQUAL(reference, g_470fa0) && !REFERENCE_EQUAL(actor->unknown418, g_470fa0) && actor->unknown3f0)
	{
		reference = actor->unknown418;
		if (entry && function_262b40(reference) && function_2624d0(entry, reference))
		{
			if (request->unknown618)
			{
				entry->distance_squared = point_distance_squared_ordered(&request->point, &entry->point);
			}
			else
			{
				entry->distance_squared = 0.0f;
			}
			if (!function_260160(actor_index, search, entry))
			{
				reference = g_470fa0;
			}
		}
		*b = NONE;
		*c = false;
	}
	return reference;
}


struct s_reference_candidate_view
{
	s_type_c3b527 *location;
	s_reference reference;
	short field08;
	byte unknown0a[2];
	point3f point;
	real distance18;
	vector3f vector1c;
	real distance28;
	real distance2c;
	real distance_squared;
	vector3f vector34;
	vector3f vector40;
	bool flag4c;
	bool flag4d;
	byte unknown4e[2];
	real value50;
	real value54;
	bool flag58;
	bool flag59;
	bool flag5a;
	bool flag5b;
	short field5c;
	byte unknown5e[0x78 - 0x5e];
};

// @retail 0x2624d0
bool function_2624d0(s_261d20_entry *entry, s_reference reference)
{
	bool result = false;
	s_type_c3b527 *location = (s_type_c3b527 *)function_262b40(reference);
	if (location)
	{
		s_reference_candidate_view *candidate = (s_reference_candidate_view *)entry;
		candidate->location = location;
		candidate->reference = reference;
		candidate->field08 = 0;
		candidate->distance18 = FLT_MAX;
		candidate->vector1c = *g_4687a4;
		candidate->distance28 = FLT_MAX;
		candidate->vector40 = *g_4687a4;
		candidate->distance2c = FLT_MAX;
		candidate->vector34 = *g_4687a4;
		candidate->distance_squared = 0.0f;
		candidate->value50 = 0.0f;
		candidate->value54 = 0.0f;
		candidate->flag4c = true;
		candidate->flag4d = false;
		candidate->flag5b = false;
		candidate->flag5a = false;
		candidate->flag58 = false;
		candidate->flag59 = false;
		candidate->field5c = 0;
		function_210850(location, &candidate->point);
		result = true;
	}
	return result;
}


struct s_reference_direction_request
{
	byte unknown000[0x54];
	bool has_direction;
	byte unknown055[0x620 - 0x55];
	point3f point;
};

real function_30bf0(vector3f *vector);

// @retail 0x260530
void function_260530(s_reference_candidate_view *entry, s_reference_direction_request const *request)
{
	vector3f delta;
	vector3d_from_points3d(&request->point, &entry->point, &delta);
	if (length_sq3f(&delta) < 400.0f)
	{
		entry->distance28 = function_30bf0(&delta);
		if (request->has_direction)
			entry->vector34 = delta;
	}
}


long function_262a30(s_reference reference);

struct s_reference_filter_view
{
	byte unknown00[0x56];
	bool allow_a;
	bool require_flag;
	bool allow_b;
	bool allow_any;
};

// @retail 0x2623a0
bool function_2623a0(s_reference_filter_view const *filter, long actor_index, s_actor_view *actor, s_reference reference)
{
 if (!filter) return true;
 bool result;
	if (filter)
	{
		s_262b40_result *entry = function_262b40(reference);
		if (!entry || (!filter->allow_a && !filter->allow_b &&
			(*(long *)((byte *)entry + 0x14) == NONE || !(entry->flags & 0x60))))
			return false;
		bool flag = (bool)(((dword)entry->flags >> 5) & 1);
		if (filter->require_flag)
			result = flag;
		else if (filter->allow_any)
			result = true;
		else
			result = !flag;
		if (result)
		{
			long other_index = function_262a30(reference);
			if (other_index != actor_index && other_index != NONE)
			{
				s_actor_view *other = actor_get(other_index);
				if (other->unknown024 != actor->unknown024)
					return false;
				point3f point;
				function_210850((s_type_c3b527 *)entry, &point);
				real distance = distance3d((point3f *)((byte *)other + 0x238), &point);
				if (distance < 1.0f || distance3d((point3f *)((byte *)actor + 0x238), &point) * 2.0f > distance)
					return false;
			}
		}
		return result;
	}
 return true;
}

struct s_candidate_range
{
    byte unknown00[0x20];
    dword flags;
    byte unknown24[0x38 - 0x24];
    short first;
    short count;
    byte unknown3c[0x88 - 0x3c];
};

struct s_candidate_block
{
    byte unknown00[0x24];
    short structure_index;
    byte unknown26[0xe];
    s_candidate_range *ranges;
};

struct s_29db90;
void function_29db90(short range_index, short block_index, s_29db90 *list);

// @retail 0x262180
short __stdcall function_262180(long actor_index, long block_index, long range_index,
    s_261d20_entry *entries, short *count, long maximum_count, s_prop_search *search, short type)
{
    long const *actor_reference = &actor_index;
    long const *block_reference = &block_index;
    long const *range_reference = &range_index;
    s_261d20_entry *const *entries_reference = &entries;
    short *const *count_reference = &count;
    long const *maximum_reference = &maximum_count;
    s_prop_search *const *search_reference = &search;
    short const *type_reference = &type;
    s_actor_view *actor = actor_get(*actor_reference);
    bool special = actor->unknown26c != NONE;
    if (*block_reference != NONE && *range_reference != NONE)
    {
        s_type_967e20 *context = (s_type_967e20 *)*search_reference;
        if (!context || !context->unknown68c ||
            (*block_reference == context->unknown68e && *range_reference == context->unknown690))
        {
            s_candidate_block *block = &(*(s_candidate_block **)((byte *)g_4e0350 + 0x16c))[*block_reference & 0xffff];
            if (block->structure_index == g_4686c4)
            {
                s_candidate_range *range = &block->ranges[*range_reference];
                dword flags = range->flags;
                if (special == (bool)(flags & 1))
                {
                    bool allowed = (bool)((flags >> 1) & 1);
                    if (context)
                    {
                        if (!special && (context->unknown57 || context->unknown59))
                        {
                            if (context->unknown57 && !allowed)
                                goto done;
                        }
                        else if (allowed)
                            goto done;
                    }
                    if (*count < *maximum_reference)
                    for (long volatile index = (word)range->first; *count < *maximum_reference && (short)index < range->first + range->count; ++index)
                    {
                        s_reference reference;
                        reference.unknown0 = index;
                        reference.unknown2 = (short)*block_reference;
                        if (function_2623a0((s_reference_filter_view const *)context, *actor_reference, actor, reference))
                        {
                            s_reference_candidate_view *entry = (s_reference_candidate_view *)&(*entries_reference)[*count];
                            if (function_2624d0((s_261d20_entry *)entry, reference))
                            {
                                switch (*type_reference)
                                {
                                case 2: ((s_reference_candidate_view *)&(*entries_reference)[*count])->flag5a = true; break;
                                case 1: ((s_reference_candidate_view *)&(*entries_reference)[*count])->flag5b = true; break;
                                }
                                ((s_reference_candidate_view *)&(*entries_reference)[*count])->flag58 =
                                    (bool)(((dword)*(char *)((byte *)((s_reference_candidate_view *)&(*entries_reference)[*count])->location + 0xe) >> 5) & 1);
                                ((s_reference_candidate_view *)&(*entries_reference)[*count])->flag59 =
                                    !(bool)(((dword)*((byte *)((s_reference_candidate_view *)&(*entries_reference)[*count])->location + 0xe) >> 6) & 1);
                                ++*count;
                            }
                        }
                    }
                    if (context && context->unknown56)
                        function_29db90((short)*range_reference, (short)*block_reference, (s_29db90 *)((byte *)context + 0x6a0));
                }
            }
        }
    }
done:
    return **count_reference;
}

void function_260060(long actor_index, long mode, s_type_967e20 *context, s_type_b36ac5 *position);
bool function_2600a0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
void __stdcall function_25f7b0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);

// @retail 0x260160
bool function_260160(long actor_index, s_prop_search *search, s_261d20_entry *entry)
{
    s_type_b36ac5 *position = (s_type_b36ac5 *)entry;
    s_type_967e20 *context = (s_type_967e20 *)search;
    byte *header = (byte *)g_4e0348;
    long count = *(long *)(header + 0xc4);
    long *data = NULL;
    if (count > 0)
        data = *(long **)(header + 0xc8);
    position->score = 0.0f;
    position->unknown50 = 0.0f;
    position->unknown4c = true;
    position->unknown4d = false;
    if (!context->unknown56 && (!data || position->definition->unknown14 < 0 || position->definition->unknown14 >= *data))
    {
        position->unknown4c = false;
        position->unknown4d = true;
    }
    if (position->unknown4c)
    {
        function_260060(actor_index, 1, context, position);
        if (position->unknown4c)
        {
            if (context->unknown618)
                function_25f7b0(actor_index, context, position);
            position->unknown50 = position->score;
            position->unknown4c = function_2600a0(actor_index, context, position);
        }
    }
    return position->unknown4c;
}

byte *ai_scratch_buffer_get(void);
void ai_scratch_buffer_release(byte *buffer);
long function_1e4a50(long index);
bool function_29e050(byte *point, long target_index, s_type_d4fbfa *definition, s_reference reference, long *list);

struct s_squad_iterator
{
    short squad_index;
    short current;
    short next;
    short palette_index;
    word flags;
    bool flag_a;
    bool flag_b;
    bool flag_c;
    byte unknown0d[3];
    void *definition;
};

void function_204ec0(s_squad_iterator *iterator, short squad_index, short flags, short mode);
short function_205010(s_squad_iterator *iterator);

struct s_candidate_position
{
    s_type_c3b527 point;
    byte unknown10[4];
    long sector;
    byte unknown18[8];
};

struct s_candidate_positions_block
{
    byte unknown00[0x28];
    long count;
    s_candidate_position *positions;
    long range_count;
    s_candidate_range *ranges;
};

// @retail 0x2601f0
bool function_2601f0(long actor_index, s_type_c3b527 const *point, long sector, real radius, real path_distance)
{
    s_type_c3b527 const *const *point_reference = &point;
    long const *sector_reference = &sector;
    real const *radius_reference = &radius;
    real const *path_distance_reference = &path_distance;
    s_actor_view *actor = actor_get(actor_index);
    long *pathfinding = NULL;
    if (*(long *)((byte *)g_4e0348 + 0xc4) > 0)
        pathfinding = *(long **)((byte *)g_4e0348 + 0xc8);
    bool volatile result = false;
    if (actor->unknown030 != NONE &&
        ((*sector_reference >= 0 && *sector_reference < *pathfinding) || *((bool *)actor + 0x229)))
    {
        byte *buffer = ai_scratch_buffer_get();
        if (!*((bool *)actor + 0x229) && *path_distance_reference > 0.0f)
        {
            s_path_source source;
            memset(&source, 0, sizeof(source));
            source.radius = *(real *)((byte *)function_1e4a50(actor->unknown054) + 4);
            source.object_index = NONE;
            source.unknown0c = NONE;
            memcpy(&source.point, *point_reference, sizeof(source.point));
            source.unknown04 = true;
            source.has_point = true;
            source.unknown24 = *sector_reference;
            source.unknown45 = true;
            source.unknown48 = *path_distance_reference;
            source.unknown4c = 0.0f;
            s_path_settings settings;
            function_1f9240(actor_index, &settings);
            function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
            function_2715a0(buffer);
        }
        s_squad_iterator iterator;
        function_204ec0(&iterator, (short)actor->unknown030, 15, actor->unknown26c != NONE);
        while (function_205010(&iterator) != NONE)
        {
            s_candidate_positions_block *block = &(*(s_candidate_positions_block **)((byte *)g_4e0350 + 0x16c))[(word)iterator.palette_index];
            s_candidate_range *range = &block->ranges[iterator.current];
            for (short index = range->first; index < range->first + range->count; ++index)
            {
                if (index >= 0 && index < block->count)
                {
                    s_candidate_position *candidate = &block->positions[index];
                    vector3f delta;
                    if ((*point_reference)->output_index == candidate->point.output_index)
                        vector3d_from_points3d(&(*point_reference)->point, &candidate->point.point, &delta);
                    else
                    {
                        point3f start, end;
                        function_210850(*point_reference, &start);
                        function_210850(&candidate->point, &end);
                        vector3d_from_points3d(&start, &end, &delta);
                    }
                    if (*radius_reference * *radius_reference > delta.k * delta.k + delta.j * delta.j + delta.i * delta.i)
                    {
                        if (*((bool *)actor + 0x229))
                        {
                            point3f raised;
                            function_210850(*point_reference, &raised);
                            raised.z += 0.10000000149011612f;
                            if (function_29e050((byte *)&raised, actor->unknown018, (s_type_d4fbfa *)candidate, g_470fa0, NULL))
                            {
                                result = true;
                                goto done;
                            }
                        }
                        else if (*path_distance_reference <= 0.0f)
                        {
                            result = true;
                            goto done;
                        }
                        else
                        {
                            real distance;
                            function_270750(buffer, candidate->sector, (s_actor_point_target *)candidate, &distance, 0, 0);
                            if (*path_distance_reference > distance)
                            {
                                result = true;
                                goto done;
                            }
                        }
                    }
                }
            }
        }
done:
        ai_scratch_buffer_release(buffer);
    }
    return result;
}


struct s_candidate_range_reference
{
    word type;
    word unknown02;
    short block_index;
    short range_index;
};

struct s_candidate_range_list
{
    long count;
    s_candidate_range_reference *entries;
};

// @retail 0x261ec0
bool __stdcall function_261ec0(long actor_index, long squad_index, s_261d20_entry *entries,
    short *count, long maximum_count, s_prop_search *search)
{
    long const *actor_reference = &actor_index;
    long const *squad_reference = &squad_index;
    s_261d20_entry *const *entries_reference = &entries;
    short *const *count_reference = &count;
    long const *maximum_reference = &maximum_count;
    s_prop_search *const *search_reference = &search;
    byte *squad = g_51e9d8->data + (*squad_reference & 0xffff) * 0x98;
    bool volatile result = false;
    if (*(long *)(squad + 0x80) != NONE)
    {
        if (*(char *)(squad + 0x30) >= 0)
        {
            result = true;
            for (short index = 0; index < *(char *)(squad + 0x30); ++index)
                function_262180(*actor_reference, ((short *)(squad + 0x32))[index * 2],
                    ((short *)(squad + 0x34))[index * 2], *entries_reference, *count_reference,
                    *maximum_reference, *search_reference, 0);
        }
    }
    else if (*(short *)(squad + 0x2a) != NONE)
    {
        byte *definition = *(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(squad + 0x2a) * 0x7c;
        if ((*(byte *)(definition + 0x24) & 0x20) && *(short *)(definition + 0x4e) != NONE)
        {
            result = function_261ec0(*actor_reference, *(short *)(definition + 0x4e),
                *entries_reference, *count_reference, *maximum_reference, *search_reference);
            return result;
        }
        s_actor_view *actor = actor_get(*actor_reference);
        s_candidate_range_list *list = *(volatile byte *)(squad + 0x60) ? (s_candidate_range_list *)(definition + 0x5c) : (s_candidate_range_list *)(definition + 0x54);
        short flags = *((bool *)actor + 0x3f2) ? 6 : 4;
        if (!*search_reference || !*((bool *)*search_reference + 0x69c)) flags |= 1;
        if ((flags & 4) && !(flags & 1))
        {
            short index;
            for (index = 0; index < list->count; ++index)
            {
                s_candidate_range_reference *reference = &(*(volatile byte *)(squad + 0x60) ? (s_candidate_range_list *)(definition + 0x5c) :
                    (s_candidate_range_list *)(definition + 0x54))->entries[index];
                if (reference->type == 2) break;
            }
            if (index >= list->count) flags |= 1;
        }
        result = true;
        if (*search_reference) *(short *)((byte *)*search_reference + 0x69e) = flags;
        for (short index = 0; index < list->count; ++index)
        {
            s_candidate_range_reference *reference = &(*(volatile byte *)(squad + 0x60) ? (s_candidate_range_list *)(definition + 0x5c) :
                (s_candidate_range_list *)(definition + 0x54))->entries[index];
            word type = reference->type;
            if (flags & (1 << type))
                function_262180(*actor_reference, reference->block_index, reference->range_index,
                    *entries_reference, *count_reference, *maximum_reference, *search_reference, (short)type);
        }
    }
    else
    {
        short block_index = *(short *)(*(byte **)((byte *)g_4e0350 + 0x164) +
            (*squad_reference & 0xffff) * 0x74 + 0x38);
        if (block_index != NONE)
        {
            s_candidate_block *block = &(*(s_candidate_block **)((byte *)g_4e0350 + 0x16c))[(word)block_index];
            for (short index = 0; index < *(long *)((byte *)block + 0x30); ++index)
                function_262180(*actor_reference, block_index, index, *entries_reference,
                    *count_reference, *maximum_reference, *search_reference, 0);
        }
    }
    return result;
}

#include "unknown_2605d0.h"

// @retail 0x261d20
short __stdcall function_261d20(long actor_index, s_261d20_entry *entries, long maximum_count,
    s_2605d0_request const *request)
{
    s_actor_view *actor = actor_get(actor_index);
    long count = 0;
    if (actor->unknown030 != NONE)
        function_261ec0(actor_index, actor->unknown030, entries, (short *)&count, maximum_count, (s_prop_search *)request);
    long list_index = *(long *)((byte *)actor + 0x3f4);
    if (list_index != NONE)
    {
        if (request && *(short const *)request == 4 && actor->unknown030 != NONE && (short)count > 0)
        {
            byte *squad = g_51e9d8->data + (actor->unknown030 & 0xffff) * 0x98;
            short definition_index = *(short *)(squad + 0x2a);
            if (definition_index != NONE && !*((bool *)actor + 0x3c))
            {
                byte *definition = *(byte **)((byte *)g_4e0350 + 0x244) + definition_index * 0x7c;
                dword flags = *(dword *)(definition + 0x24);
                if (((flags & 0x10) || ((flags & 0x20) && *(short *)(definition + 0x4e) != NONE)) &&
                    *(long *)(squad + 0x80) != NONE)
                    return count;
            }
        }
        byte *list = g_51eca4->data + (list_index & 0xffff) * 0x484;
        for (short index = 0; index < *(short *)(list + 8) && (short)count < maximum_count; ++index)
        {
            s_reference reference = { index, (word)(list_index | 0x8000) };
            if (function_2623a0((s_reference_filter_view const *)request, actor_index, actor, reference) &&
                function_2624d0(&entries[(short)count], reference))
            {
                if (*(short *)(list + 2) == 1)
                    *(short *)((byte *)&entries[(short)count] + 0x5c) = ((short *)(list + 0x444))[index];
                ++count;
            }
        }
    }
    return count;
}

#if 0
// Active versions alter the matched 0x2605d0 caller frame; retain its original stub.
#include "props.h"
#include <string.h>
typedef bool (__stdcall *t_reference_compare)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_reference_compare compare, const void *context);
bool __stdcall function_2600e0(long a, long b, void *unused);
void __stdcall function_25f7b0(long actor_index, s_type_967e20 *context, s_type_b36ac5 *position);
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index);

struct firing_position_post_evaluator
{
	short flags;
	bool (__stdcall *proc)(long, s_type_967e20 *, s_type_b36ac5 *);
};
extern firing_position_post_evaluator g_44adf0[12];
extern s_type_b36ac5 *g_51eca0;
short g_51ec9c;

struct hash_node
{
	void *key;
	dword hash;
	hash_node *next;
	byte data[1];
};
struct hash_table
{
	byte unknown00[0x20];
	dword bucket_count;
	long maximum_count;
	long data_size;
	dword (__stdcall *hash_proc)(const void *key);
	bool (__stdcall *compare_proc)(const void *a, const void *b);
	c_data_allocator *allocator;
	hash_node *free_list;
	hash_node *buckets[1];
};
hash_node *function_13e2d0(hash_table *table, void *key);

PRIVATE inline bool reference_owner_find(hash_table *table, void *key, void *data)
{
	hash_node *node = function_13e2d0(table, key);
	if (node && data)
	{
		memcpy(data, node->data, table->data_size);
		return true;
	}
	return false;
}

// Disabled retail draft 0x260670
s_reference __stdcall function_260670(long actor_index, s_2605d0_request const *request,
	s_261d20_entry *entries, short count, long reported_entry, long reported_owner, byte *scratch, bool *has_path)
{
	byte *actor = (byte *)actor_get(actor_index);
	s_type_967e20 *context = (s_type_967e20 *)request;
	byte *parameters = (byte *)context;
	s_type_b36ac5 *positions = (s_type_b36ac5 *)entries;
	s_reference result = g_470fa0;
	bool within_limit = false;
	bool finite_distance = false;
	short selected = NONE;
	real best_score = 0.0f;
	s_path_source source;
	s_path_settings settings;
	long sorted_indices[512];
	long sort_scratch;
	*has_path = false;
	((byte *)g_4f55d0)[3] = true;
	if (count == 0)
	{
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		return g_470fa0;
	}
	if (parameters[0x618] && parameters[0x53])
	{
		if (parameters[0x56])
		{
			for (short i = 0; i < count; i++)
				function_260530((s_reference_candidate_view *)&positions[i], (s_reference_direction_request *)parameters);
		}
		else if (*(long *)(parameters + 0x64c) != NONE)
		{
			memset(&source, 0, sizeof(source));
			source.radius = *(real *)((byte *)function_1e4a50(*(long *)(actor + 0x54)) + 4);
			source.unknown04 = actor[0x3e4];
			source.object_index = NONE;
			source.unknown0c = NONE;
			function_270590(&source, (s_type_c3b527 *)(parameters + 0x650), *(long *)(parameters + 0x64c));
			source.unknown4c = 0.0f;
			source.unknown45 = true;
			source.unknown48 = 20.0f;
			function_1f9240(actor_index, &settings);
			byte *buffer = ai_scratch_buffer_get();
			function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
			function_2715a0(buffer);
			for (short i = 0; i < count; i++)
			{
				s_type_b36ac5 *position = &positions[i];
				if ((position->unknown58 && parameters[0x5a]) || (position->unknown59 && parameters[0x58]))
					function_260530((s_reference_candidate_view *)position, (s_reference_direction_request *)parameters);
				else
					function_270750(buffer, position->definition->unknown14, (s_actor_point_target *)position->definition,
						&position->unknown28, 0, parameters[0x54] ? (long)&position->unknown34 : 0);
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	else if (parameters[0x54])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown34);
			function_30bf0(&positions[i].unknown34);
		}
	}
	if (parameters[0x55])
	{
		for (short i = 0; i < count; i++)
		{
			vector3d_from_points3d((point3f *)(parameters + 0x620), &positions[i].position, &positions[i].unknown40);
			normalize_inline(&positions[i].unknown40);
		}
	}
	if (!parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		source.unknown45 = true;
		source.unknown48 = *(real *)(parameters + 0x1c);
		if (parameters[0x46] && parameters[0x618])
		{
			long target_object = NONE;
			if (*(long *)(parameters + 0x664) != NONE)
				target_object = prop_ref_get(*(long *)(parameters + 0x664))->object_index;
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(parameters + 0x620);
			*(long *)(source.unknown28 + 0x10) = target_object;
			*(real *)(source.unknown28 + 0x14) = *(real *)(parameters + 0x4c);
			source.unknown44 = !parameters[0x50];
			*(real *)(source.unknown28 + 0x18) = *(real *)(parameters + 0x48);
		}
		else if (*(short *)(actor + 0x358) > 0)
		{
			source.unknown28[0] = true;
			*(point3f *)(source.unknown28 + 4) = *(point3f *)(actor + 0x370);
			*(real *)(source.unknown28 + 0x14) = *(real *)(actor + 0x36c);
			*(long *)(source.unknown28 + 0x10) = *(long *)(actor + 0x360);
			source.unknown44 = true;
			*(real *)(source.unknown28 + 0x18) = 10.0f;
		}
		function_1f9240(actor_index, &settings);
		function_271300((s_type_f17a25 *)scratch, NULL, &settings, &source, 0);
		if (function_2715a0(scratch))
			*has_path = true;
	}
	for (short i = 0; i < count; i++)
	{
		s_type_b36ac5 *position = &positions[i];
		if (parameters[0x618])
		{
			vector3f delta;
			vector3d_from_points3d((point3f *)(parameters + 0x620), &position->position, &delta);
			position->unknown30 = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		}
		vector3f delta;
		vector3d_from_points3d((point3f *)(actor + 0x238), &position->position, &delta);
		real distance_squared = delta.i * delta.i + delta.k * delta.k + delta.j * delta.j;
		real range = *(real *)(parameters + 0x1c);
		if (range * range > distance_squared)
		{
			if (!parameters[0x56] && !(position->unknown58 && parameters[0x5a]) && !(position->unknown59 && parameters[0x58]))
				function_270750(scratch, position->definition->unknown14, (s_actor_point_target *)position->definition,
					&position->unknown18, (long)&position->unknown2c, parameters[0x51] ? (long)&position->unknown1c : 0);
			else
			{
				real perpendicular_squared;
				if (distance_squared > 0.0001f)
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(actor + 0x238), (point3f *)(parameters + 0x620), &target_delta);
					real t = (target_delta.k * delta.k + target_delta.j * delta.j + target_delta.i * delta.i) / distance_squared;
					t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;
					t = 0.0f - t;
					target_delta.i = t * delta.i + target_delta.i;
					target_delta.j = t * delta.j + target_delta.j;
					target_delta.k = t * delta.k + target_delta.k;
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				else
				{
					vector3f target_delta;
					vector3d_from_points3d((point3f *)(parameters + 0x620), (point3f *)(actor + 0x238), &target_delta);
					perpendicular_squared = target_delta.k * target_delta.k + target_delta.j * target_delta.j + target_delta.i * target_delta.i;
				}
				position->unknown2c = (real)sqrt((double)perpendicular_squared);
				real length = (real)sqrt((double)distance_squared);
				if ((double)0.0001f > fabs((double)length))
					length = 0.0f;
				else
				{
					real scale = 1.0f / length;
					delta.i *= scale;
					delta.j *= scale;
					delta.k *= scale;
				}
				position->unknown18 = length;
				if (parameters[0x51])
					position->unknown1c = delta;
			}
		}
		if (*(real *)(parameters + 0x1c) > position->unknown18)
			within_limit = true;
		else
		{
			position->unknown4c = false;
			if (FLT_MAX > position->unknown18)
				finite_distance = true;
		}
	}
	if (!within_limit && !finite_distance && !parameters[0x56] && !actor[0x264] &&
		(*(long *)(actor + 0x28c) == NONE || (*has_path && count > 0 && *(short *)(scratch + 0xae) < 8)))
		++*(short *)(actor + 0x5e8);
	else
		*(short *)(actor + 0x5e8) = 0;
	if (parameters[0x15] && !within_limit)
	{
		dword *seed = (dword *)g_4e7408;
		*seed = *seed * 0x19660d + 0x3c6ef35f;
		selected = (short)(((*seed >> 16) * count) >> 16);
		*has_path = false;
		byte *history_actor = (byte *)actor_get(actor_index);
		*(short *)(history_actor + 0x3fe) = 3;
		byte *history = history_actor + 0x402;
		long remaining = 4;
		do
		{
			*(s_reference *)history = g_470fa0;
			history += 6;
		} while (--remaining);
		if (!function_260160(actor_index, (s_prop_search *)context, &entries[selected]))
			goto done;
	}
	else
	{
		function_260060(actor_index, count, context, positions);
		for (short i = 0; i < count; i++)
			sorted_indices[i] = i;
		g_51ec9c = count;
		g_51eca0 = positions;
		sort_4byte(sorted_indices, count, &sort_scratch, (t_reference_compare)function_2600e0, NULL);
		context->unknown680 = 0.0f;
		bool possible = true;
		for (long evaluator = 0; g_44adf0[evaluator].proc && possible; evaluator++)
		{
			if ((1 << context->type) & g_44adf0[evaluator].flags)
				possible = g_44adf0[evaluator].proc(actor_index, context, NULL);
		}
		parameters[0x67c] = possible;
		for (short i = 0; i < count; i++)
		{
			short index = (short)sorted_indices[i];
			s_type_b36ac5 *position = &positions[index];
			if (!position->unknown4c || (parameters[0x67c] && best_score >= position->score + context->unknown680))
				break;
			if (parameters[0x618])
				function_25f7b0(actor_index, context, position);
			position->unknown50 = position->score;
			if (function_2600a0(actor_index, context, position) && position->score > best_score)
			{
				selected = index;
				best_score = position->score;
			}
		}
		if (selected == NONE)
			goto done;
	}
	if (reported_entry)
		memcpy((void *)reported_entry, &positions[selected], sizeof(s_type_b36ac5));
	result = positions[selected].reference;
	if (reported_owner)
	{
		long owner = NONE;
		reference_owner_find((hash_table *)g_557c6c, *(void **)&result, &owner);
		*(long *)reported_owner = owner != actor_index ? owner : NONE;
	}
	if (actor[0x3f2] && !actor[0x220] && !positions[selected].unknown5b)
		actor[0x3f2] = false;
	if (parameters[0x56])
	{
		function_1f90f0(actor_index, &source);
		s_type_f17a25 *path = (s_type_f17a25 *)scratch;
		path->unknown54 = false;
		path->unknown90 = NONE;
		path->unknownae = 0;
		path->heap_count = 0;
		path->location.unknown00 = 0;
		path->location.unknown02 = 0;
		path->pathfinding = *(long *)((byte *)g_4e0348 + 0xc4) > 0 ? *(void **)((byte *)g_4e0348 + 0xc8) : NULL;
		path->flags = 0;
		path->unknownac = false;
		path->source = source;
		memset(&path->settings, 0, sizeof(path->settings));
		path->unknown14188 = 0;
		memcpy(path->unknown140d4, parameters + 0x6a0, 0xb6);
		*has_path = true;
	}
done:
	return result;
}

#endif

#include "props.h"
void function_1caa40(long object_index, point3f *position);
void function_1e3b00(long object_index, long mode, point3f const *reference, void const *unknown0, void const *unknown1, point3f *position);
void __stdcall function_26c2d0(long prop_index);
long function_baf80(long object_index);
long function_1e4a10(long index);
long function_1e4990(long index);
void *function_1e5280(long actor_index, long key);
long function_1e1f20(long actor_index);
bool function_1e1e50(long actor_index, vector3f *direction);
bool function_267840(long actor_index, long prop_index, vector3f *direction);
struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

// @retail 0x261510
void __stdcall function_261510(long actor_index, s_2605d0_request const *request)
{
 byte *actor = (byte *)actor_get(actor_index);
 byte *parameters = (byte *)request;
 short type = request->type;
 *(real *)(parameters + 0x18) = *(short *)(actor + 0x270) > 0 ? 80.0f : actor[0x229] ? 30.0f : 15.0f;
 if (*(real *)(parameters + 0x1c) == 0.0f) *(real *)(parameters + 0x1c) = *(real *)(parameters + 0x18);
 long unit_index = *(long *)(actor + 0x26c);
 if (unit_index == NONE) unit_index = *(long *)(actor + 0x18);
 function_1caa40(unit_index, (point3f *)(parameters + 0x60c));
 type = request->type;
 parameters[0x53] = type == 5 || type == 8 || type == 3;
 parameters[0x618] = 0;
 if (type == 4) parameters[0x618] = 0;
 else if (parameters[0x20])
 {
  *(point3f *)(parameters + 0x620) = *(point3f *)(parameters + 0x24);
  memcpy(parameters + 0x650, parameters + 0x30, 0x10);
  *(long *)(parameters + 0x64c) = *(long *)(parameters + 0x40);
  parameters[0x618] = 1;
  *(short *)(parameters + 0x660) = *(short *)(parameters + 0x44);
  double x = (double)*(real *)(parameters + 0x620) - *(real *)(actor + 0x238);
  double y = (double)*(real *)(parameters + 0x624) - *(real *)(actor + 0x23c);
  double z = (double)*(real *)(parameters + 0x628) - *(real *)(actor + 0x240);
  *(long *)(parameters + 0x664) = NONE;
  *(long *)(parameters + 0x648) = NONE;
  *(real *)(parameters + 0x678) = 0.0f;
  *(real *)(parameters + 0x61c) = (real)sqrt(z * z + y * y + x * x);
  function_1e3b00(*(long *)(actor + 0x18), 1, (point3f *)(parameters + 0x620), NULL, NULL, (point3f *)(parameters + 0x62c));
  *(point3f *)(parameters + 0x638) = *(point3f *)(parameters + 0x62c);
 }
 else
 {
  long prop_index = *(long *)(actor + (type == 1 ? 0x344 : 0x338));
  if (prop_index != NONE)
  {
   s_prop_node *node = (s_prop_node *)(g_502418->data + (prop_index & 0xffff) * 0x3c);
   s_type_5cfb45 *state = function_25d690(node);
   s_type_f95cd3 *tracking = function_25d740(node);
   byte *object = *(byte **)(g_4e0300->data + (node->object_index & 0xffff) * 12 + 8);
   function_26c2d0(prop_index);
   parameters[0x618] = 1;
   *(point3f *)(parameters + 0x620) = state->position;
   memcpy(parameters + 0x650, &state->unknown48, 0x10);
   *(long *)(parameters + 0x64c) = state->unknown44;
   *(short *)(parameters + 0x660) = *(short *)((byte *)state + 0x2c);
   *(real *)(parameters + 0x61c) = node->unknown28;
   *(long *)(parameters + 0x664) = prop_index;
   *(point3f *)(parameters + 0x62c) = *(point3f *)((byte *)state + 0x30);
   *(long *)(parameters + 0x648) = NONE;
   long root = node->object_index;
   if (!object[0xaa] && *(long *)(object + 0x14) != NONE) root = *(long *)(object + 0x14);
   *(long *)(parameters + 0x648) = function_baf80(root);
   if (type == 2 && ((1 << object[0xaa]) & 3) && node->unknown27 < 1 && tracking && tracking->unknown10 >= 0)
   {
    point3f point;
    function_210850((s_type_c3b527 *)((byte *)tracking + 0x18), &point);
    function_1e3b00(node->object_index, 1, &point, NULL, NULL, (point3f *)(parameters + 0x638));
    long leaf = function_14a280(g_4e033c, 0, (point3f *)(parameters + 0x638));
    if (leaf != NONE) *(short *)(parameters + 0x660) = *(short *)(*(byte **)((byte *)g_4e0348 + 0x30) + leaf * 8);
   }
   else *(point3f *)(parameters + 0x638) = *(point3f *)((byte *)state + 0x10);
   byte *definition = (byte *)function_1e4a10(*(long *)(actor + 0x54));
   *(real *)(parameters + 0x678) = 0.0f;
   if (definition) *(real *)(parameters + 0x678) = *(real *)(definition + 0x64);
   if (node->state >= 3 && !parameters[0x668])
   {
    parameters[0x668] = 1;
    *(vector3f *)(parameters + 0x66c) = tracking->unknown94;
   }
   parameters[0x644] = type == 4 || type == 6;
  }
 }
 if (parameters[0x618] && (type == 0 || type == 3 || type == 2 || type == 6)) parameters[0x55] = 1;
 long weapon = function_1e1f20(actor_index);
 byte *local_67e06b_2 = NULL;
 if (weapon != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (weapon & 0xffff) * 12 + 8);
  local_67e06b_2 = (byte *)function_1e5280(actor_index, *(long *)object);
 }
 if (local_67e06b_2)
 {
  vector3f *direction = (vector3f *)(local_67e06b_2 + 0x60);
  if (direction->i * direction->i + direction->j * direction->j + direction->k * direction->k > 0.0001f)
  {
   parameters[0x5fc] = 1;
   *(vector3f *)(parameters + 0x600) = *direction;
   goto weapon_done;
  }
 }
 parameters[0x5fc] = 0;
weapon_done:
 if (*(short *)(actor + 0x358) > 0)
 {
  long index = *(long *)(actor + 0x368);
  byte *node = g_502418->data + (index & 0xffff) * 0x3c;
  if (*(short *)(node + 0x24) >= 1 && *(real *)(actor + 0x39c) + 3.0f > *(real *)(actor + 0x394)) parameters[0x51] = 1;
 }
 if (*(short *)(actor + 0x270) == 4) { parameters[0x5b] = 1; parameters[0x5c] = 1; }
 parameters[0x56] |= actor[0x229];
 byte *movement = (byte *)function_1e4a50(*(long *)(actor + 0x54));
 if (movement)
 {
  dword flags = *(dword *)movement;
  if (flags & 0x20)
  {
   parameters[0x59] &= (byte)((flags >> 4) & 1);
   if (parameters[0x59]) parameters[0x5a] = 1;
   parameters[0x58] = 1;
  }
  else parameters[0x59] = 0;
  byte *object = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x18) & 0xffff) * 12 + 8);
  if (*(short *)(object + *(short *)(object + 0x346) + 0x36) == 5)
  {
   byte *definition = (byte *)function_1e4990(*(long *)(actor + 0x54));
   if (definition && !(definition[0] & 2) && (movement[0] & 0x20)) parameters[0x56] = 1;
  }
 }
 else { parameters[0x59] = 0; parameters[0x58] = 0; parameters[0x5a] = 0; }
 *(short *)(parameters + 0x274) = 0;
 *(short *)(parameters + 0x276) = 0;
 *(short *)(parameters + 0x278) = 0;
 bool group = false;
 long group_index = *(long *)(actor + 0x7c);
 if (*(short *)(actor + 0x86) >= 5 && group_index != NONE)
  group = *(short *)(g_502420->data + (group_index & 0xffff) * 0x50 + 0x10) > 1;
 switch (type)
 {
 case 0: case 2: case 3: case 6:
  if (group)
  {
   long index = *(long *)(actor + 0x58);
   while (index != NONE && *(short *)(parameters + 0x274) < 32)
   {
    s_prop_node *node = (s_prop_node *)(g_502418->data + (index & 0xffff) * 0x3c);
    long current = index;
    index = node->next_index;
    s_type_5cfb45 *state = function_25d690(node);
    s_type_76cf92 *prop = prop_get(node->prop_index);
    vector3f direction;
    if (node->state >= 1 && !state->unknown5e && !prop->unknown23 && prop->unknown25 && function_267840(actor_index, current, &direction))
    {
     *(short *)(parameters + 0x27c + *(short *)(parameters + 0x274) * 0x1c) = 1;
     *(point3f *)(parameters + 0x280 + *(short *)(parameters + 0x274) * 0x1c) = state->position;
     *(vector3f *)(parameters + 0x28c + *(short *)(parameters + 0x274) * 0x1c) = direction;
     ++*(short *)(parameters + 0x274);
     ++*(short *)(parameters + 0x276);
    }
   }
   group_index = *(long *)(actor + 0x7c);
   if (group_index != NONE)
   {
    index = *(long *)(g_502420->data + (group_index & 0xffff) * 0x50 + 0x18);
    while (*(short *)(parameters + 0x274) < 32 && index != NONE)
    {
     byte *other = g_4f55f0->data + (index & 0xffff) * 0x888;
     long current = index;
     index = *(long *)(other + 0x80);
     vector3f direction;
     if (other != actor && function_1e1e50(current, &direction))
     {
      *(short *)(parameters + 0x27c + *(short *)(parameters + 0x274) * 0x1c) = 0;
      *(point3f *)(parameters + 0x280 + *(short *)(parameters + 0x274) * 0x1c) = *(point3f *)(other + 0x238);
      *(vector3f *)(parameters + 0x28c + *(short *)(parameters + 0x274) * 0x1c) = direction;
      ++*(short *)(parameters + 0x274);
      ++*(short *)(parameters + 0x276);
     }
    }
   }
  }
  break;
 }
 *(short *)(parameters + 0x754) = 0;
}
