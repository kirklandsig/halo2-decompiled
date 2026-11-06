// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_044720.CPP: marking the visible parts of the listed entries (built
   with SSE, apart from its neighbours in unknown_03f2b0.cpp) */

#include "cluster_mask_storage.h"
#include "material_records_storage.h"
#include "target_cache_storage.h"
#include "shader_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "physical_memory.h"
#include <string.h>
#include <xtl.h>
#include "unknown_030290.h"




struct s_word_bit_iterator
{
	short count;
	short index;
	dword remaining;
	dword const *words;
};

struct s_geometry_visibility_list
{
	dword *indices;
	long count;
};

/* the geometry of an entry, as one result; entry_flags is the entry's first word,
   which every caller has already read */
PRIVATE __forceinline byte *visible_entry_geometry(s_44940_entry *entry, dword entry_flags)
{
	byte *data;
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		data = *(byte **)(sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
	}
	else
	{
		byte *structure = (byte *)g_4e0348;
		if (!((byte)(entry_flags >> 12) & 1))
		{
			byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
			data = *(byte **)(section + 0x50);
		}
		else
		{
			byte *instances = *(byte **)(structure + 0x144);
			short index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
			byte *section = *(byte **)(structure + 0x13c) + index * 0xc8;
			data = *(byte **)(section + 0x50);
		}
	}
	return data;
}

struct s_render_part_context;
bool function_45ce0(short index, byte *out, short part, short transform_index);
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);

struct s_visibility_query_list
{
    dword *indices;
    long count;
    long capacity;
};
template<long N> struct s_visibility_query_storage
{
    s_visibility_query_list list;
    dword indices[N];
};

PRIVATE __forceinline void release_visibility_list(s_visibility_query_list *list)
{
    if (!(list->capacity & 0x80000000))
        g_480118->allocate((long)list->indices, (list->capacity & 0x7fffffff) * 4, 0x12);
}

transform4x3f *function_b8c00(long object_index, long *node_count);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
struct s_visibility_sphere_query
{
    point3f center;
    real radius;
};
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);

bool function_44690(s_word_bit_iterator *iterator, long *out);
void __stdcall function_44e90(short entry_index, short section_index);
void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);
void function_45560(short entry_index, point3f const *center, real radius);

// @retail 0x44720
void function_44720(void)
{
    s_2cb30_state *state = g_4c0b78.state;
    long end = g_4c0b78.previous;
    if (state->active) end = g_4c0b78.current;
    end = (word)end;
    word index = (word)(state->active ? g_4c0b78.previous : 0);
    while (index < end)
    {
        dword entry_flags = g_4ba138[(short)index].unknown00;
        if (!(entry_flags & 1) && (entry_flags & 0x40))
        {
            s_44940_entry *entry = &g_4ba138[(short)index];
            if (*(long *)((byte *)state + 0x2a7c) == 1)
            {
                point3f center = *(point3f *)((byte *)state + 0x2a68);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                if (transform)
                {
                    if (transform->scale != 0.0f)
                    {
                        point3f local;
                        local.x = center.x - transform->position.x;
                        local.y = center.y - transform->position.y;
                        local.z = center.z - transform->position.z;
                        if (transform->scale != 1.0f)
                        {
                            real inverse = 1.0f / transform->scale;
                            local.x *= inverse; local.y *= inverse; local.z *= inverse;
                        }
                        center.x = transform->forward.k * local.z + transform->forward.j * local.y + transform->forward.i * local.x;
                        center.y = transform->left.k * local.z + transform->left.j * local.y + transform->left.i * local.x;
                        center.z = transform->up.k * local.z + transform->up.j * local.y + transform->up.i * local.x;
                    }
                    else
                    {
                        center.x = 0.0f;
                        center.y = 0.0f;
                        center.z = 0.0f;
                    }
                }
                function_45560((short)index, &center, *(real *)((byte *)state + 0x2a74));
            }
            else if (entry_flags & 8)
            {
                long section = *(short *)(entry->unknown10 + 8);
                dword const *words = (dword const *)((byte *)g_4c0b78.state + (section + 0xa6) * 16);
                s_word_bit_iterator iterator = { 4, 0, words[0], words };
                long visible;
                while (function_44690(&iterator, &visible))
                    function_44e90((short)index, (short)visible);
            }
            else
                function_44e90((short)index, *(short *)(entry->unknown10 + 8));
        }
        ++index;
    }
}
/* draws the visible parts of the entries a selection mask and level pick */
// @retail 0x44c90
void __stdcall function_44c90(long group, dword selection_mask, long level,
    dword and_mask, dword or_mask, dword fallback_and_mask, dword fallback_or_mask, bool force)
{
    long end = (word)(!group ? g_4c0b78.previous : g_4c0b78.current);
    long begin = !group ? 0 : (word)g_4c0b78.previous;
    for (long index = begin; index < end; ++index)
    {
        dword entry_flags = g_4ba138[(short)index].unknown00;
        s_44940_entry *entry = &g_4ba138[(short)index];
        if (!((entry_flags & selection_mask) & 0x1fffff) ||
            ((entry->flags >> 4) & 0x1f) != level || !(entry_flags & 1) || entry->unknown10[0xa] <= 0) continue;
        byte weight = g_4c0b78.weights[index];
        long transform_index = entry->flags & 0xf;
        byte material_index = g_4c0b78.material_indices[(short)index];
        long material = NONE;
        if (material_index != 0xff)
            material = g_4c0b78.materials[material_index];
        byte *geometry = visible_entry_geometry(&g_4ba138[(short)index], entry_flags);
        if (!geometry) continue;
        if ((*(byte **)(entry->unknown10))[0x76] == 0xff)
        {
            and_mask = fallback_and_mask;
            or_mask = fallback_or_mask;
        }
        for (long part = 0; part < *(long *)geometry; ++part)
        {
            byte context[0x2c];
            if (function_45ce0((short)index, context, (short)part, (short)transform_index))
            {
                long selected = force || (entry->unknown00 & 0x8000) ? material : NONE;
                function_41040((short)index, part, group, weight, (s_render_part_context *)context,
                    selected, level != 31, and_mask, or_mask);
            }
        }
    }
}
/* marks the parts of an entry within a sphere: through the entry's tree when
   it has one, otherwise by testing each part's bounding sphere, moving the
   center into the part's node first */
// Round 154 (m3 lane 6): the `bound[0x10] != NONE` test is retail's own (movzx of the node byte, then cmp ecx, -1;
// a byte never equals NONE but the compiler keeps the compare); without it 0x45560 scores 79.
// @retail 0x45560
void function_45560(short entry_index, point3f const *center, real radius)
{
    long index = entry_index;
    byte *geometry = visible_entry_geometry(&g_4ba138[index], g_4ba138[index].unknown00);
    s_44940_entry *entry = &g_4ba138[index];
    point3f point = *center;
    if (geometry)
    {
        if (*(long *)(geometry + 0x30) > 0)
        {
            __declspec(align(16)) s_visibility_query_storage<512> first, second;
            s_visibility_sphere_query query;
            second.list.indices = second.indices;
            second.list.count = 0;
            second.list.capacity = (long)0x80000200;
            first.list.indices = first.indices;
            first.list.count = 0;
            first.list.capacity = (long)0x80000200;
            point3f query_center;
            query_center.x = center->x;
            query_center.y = center->y;
            query_center.z = center->z;
            void *const &tree = *(byte **)(geometry + 0x2c);
            query.center.x = query_center.x;
            query.center.y = query_center.y;
            query.center.z = query_center.z;
            query.radius = radius;
            function_2ddbd0(tree, &query, &first.list, &second.list);
            function_458d0((short)index, true, (s_geometry_visibility_list *)&second.list,
                (s_geometry_visibility_list *)&first.list);
            release_visibility_list(&first.list);
            release_visibility_list(&second.list);
        }
        else
        {
            dword *visible = *(dword **)(entry->unknown10 + 4);
            long cached_node = NONE;
            for (long i = 0; i < *(long *)(geometry + 8); ++i)
            {
                if (!(visible[i >> 5] & (1 << (i & 31))))
                {
                    byte *element = *(byte **)(geometry + 0xc) + i * 8;
                    bool intersects = true;
                    if (*(word *)(element + 4) != 0xffff)
                    {
                        byte *bound = *(byte **)(geometry + 0x14) + *(word *)(element + 4) * 20;
                        point3f position = *(point3f *)bound;
                        long tag = entry->tag;
                        if (tag != NONE)
                        {
                            long object_index = **(long **)entry->unknown10;
                            /* retail's own code compares the node byte with NONE (`movzx; cmp ecx, -1; je`) */
                            if (object_index != NONE && bound[0x10] != NONE && bound[0x10] != cached_node)
                            {
                                long count = 0;
                                transform4x3f *nodes = function_b8c00(object_index, &count);
                                if (count)
                                {
                                    transform4x3f transform;
                                    byte *definition = g_4e3b44[tag & 0xffff].bytes;
                                    function_142a60(&nodes[bound[0x10]], (transform4x3f *)(*(byte **)(definition + 0x4c) + bound[0x10] * 0x60 + 0x28), &transform);
                                    function_142700(&transform, center, &point);
                                    cached_node = bound[0x10];
                                }
                            }
                        }
                        real combined_radius = *(real *)(bound + 0xc) + radius;
                        real dx = point.x - position.x;
                        real dy = point.y - position.y;
                        real dz = point.z - position.z;
                        intersects = dx * dx + dy * dy + dz * dz <= combined_radius * combined_radius;
                    }
                    if (intersects)
                    {
                        entry->unknown00 |= 1;
                        visible[i >> 5] |= 1 << (i & 31);
                    }
                }
            }
        }
    }
}
