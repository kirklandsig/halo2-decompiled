// @flags /O2 /Gr
#include "cluster_mask_storage.h"
#include "material_records_storage.h"
#include "target_cache_storage.h"
#include "shader_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_03F2B0.CPP: visibility lists, resource prediction and index caches */

#include "unknown_11c920.h"
#include "unknown_044e90.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "depth_groups_storage.h"
#include "physical_memory.h"
#include <string.h>
#include <xtl.h>
#include "unknown_030290.h"

void *g_509438;

__declspec(noinline) void function_43990(void);

// @retail 0x43990
void function_43990(void)
{
	physical_memory_new_frame((s_physical_object *)g_509448);
}

extern D3DResource *g_509444;

// @retail 0x439d0
void *function_439d0(long index)
{
    s_physical_object *manager = (s_physical_object *)g_509448;
    s_physical_block *block = &((s_physical_block *)((s_physical_object *)g_509448)->blocks->data)[index & 0xffff];
    D3DResource *resource = g_509444;
    long offset = block->offset << manager->page_shift;
    block->time = manager->time;
    D3DDevice_FlushVertexCache();
    return (void *)((resource->Data | 0x80000000) + offset);
}

void function_1cdd0(real const *bounds, byte flags);
bool function_13d9f0(long type);

// @retail 0x4d7c0
void function_4d7c0(dword const *source, word const *type)
{
    (void)&type;
    long index = source[1];
    if (index != NONE)
    {
        byte *tag = g_4e3b44[index & 0xffff].bytes;
        if (*(long *)(tag + 0x14) > 0)
        {
            byte *entries = *(byte **)(tag + 0x28);
            long flags = *(word *)(entries + ((source[3] >> 9) & 0x1ff) * 0x5c + 0x1a);
            real const *bounds = *(real **)(tag + 0x18);
            if (function_13d9f0(*type))
                flags |= 1;
            function_1cdd0(bounds, (byte)flags);
            return;
        }
    }
    function_1cdd0(0, 0);
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
extern byte g_509415;

__declspec(noinline)
// @retail 0x335c0
byte function_335c0(void)
{
	if (g_510c50 && ((byte *)g_510c50)[5])
		return g_509415;
	return true;
}

// @retail 0x46220
c_allocator *function_46220(void)
{
	return g_480118;
}


// @retail 0x3d3e0
short function_3d3e0(long object_index, bool cached)
{
	if (cached)
		return g_4c1bd4;
	long tag = ((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object[0];
	long next = *(long *)(g_4e3b44[tag & 0xffff].bytes + 0x38);
	return *(long *)(g_4e3b44[next & 0xffff].bytes + 4) != NONE;
}

struct s_word_bit_iterator
{
	short count;
	short index;
	dword remaining;
	dword const *words;
};

PRIVATE __forceinline long first_set_bit(dword mask)
{
	long result;
	__asm
	{
		mov ecx, -1
		bsf ecx, mask
		mov result, ecx
	}
	return result;
}

// @retail 0x44690
bool function_44690(s_word_bit_iterator *iterator, long *out)
{
	/* Retail keeps the output pointer on the stack. */
	(void)&out;
	while (!iterator->remaining)
	{
		if (++iterator->index >= iterator->count)
			break;
		iterator->remaining = iterator->words[iterator->index];
	}
	if (iterator->index < iterator->count)
	{
		long bit = first_set_bit(iterator->remaining);
		*out = iterator->index * 32 + bit;
		iterator->remaining &= ~(1 << bit);
		return true;
	}
	return false;
}

// @retail 0x3f2b0
void function_03f2b0(void)
{
	g_509438 = 0;
}

struct s_visible_index
{
	short index;
	byte unknown02[0x18];
};

struct s_visible_index_list
{
	byte unknown00[0xa6c];
	short count;
	s_visible_index entries[1];
};

class c_entry_list
{
public:
	c_entry_list(long maximum_count);
	bool add(long a, short b, long c, short d);
	short find(long a);
	void swap(short index0, short index1);

private:
	long maximum_count;
	word count;
	short *shorts_b;
	long *longs_a;
	long *longs_c;
	short *shorts_d;
};




struct s_parent_render_object
{
	byte unknown00[0x14];
	long parent;
	byte unknown18[0x92];
	byte attached;
};

// @retail 0x31c20
void function_31c20(s_bit_vector_pool *data, bool alternate, long object_index)
{
	s_bit_vector_pool *const *reference = &data;
	long flags = alternate ? 0x3000 : 0x1000;
	if (object_index != NONE)
	{
		bool again;
		do
		{
			again = false;
			s_parent_render_object *object = (s_parent_render_object *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
			if (object->attached && object->parent != NONE)
			{
				object_index = object->parent;
				again = true;
			}
		} while (again);
		(*reference)->lists[2]->add(object_index, flags, 0, NONE);
	}
}

// @retail 0x3f3f0
void function_3f3f0(void)
{
	s_visible_index_list *list = (s_visible_index_list *)g_547f88.context;
	for (long i = 0; i < list->count; ++i)
	{
		long index = list->entries[i].index;
		g_4c56c0[index >> 5] |= 1 << (index & 31);
	}
}


long g_50943c;
long g_509440;

// @retail 0x3fd70
void function_3fd70(void)
{
	long j = 0;
	g_50943c = j;
	for (long i = 0; i < 16; ++i)
		g_4c6b00[7].values[i] = NONE;
	for (; j < 8; ++j)
	{
		g_4c6b00[j].index = NONE;
		g_4c6b00[j].count = 0;
	}
	g_509440 = 0;
}

// @retail 0x40de0
long function_40de0(long index, bool first, bool second)
{
	/* Retail receives both boolean arguments on the stack. */
	bool const *first_reference = &first;
	bool const *second_reference = &second;
	long result = NONE;
	if (!index)
		result = 0;
	else if (*first_reference)
		result = 1;
	else if (*second_reference)
		result = 2;
	return result;
}

// @retail 0x40e10
long function_40e10(bool first, bool second)
{
	long result = 3;
	if (first)
		result = second ? 0 : 1;
	else if (second)
		result = 2;
	return result;
}

struct s_predicted_resource;
struct s_predicted_resource_block
{
	long count;
	s_predicted_resource *resources;
};

struct s_cluster_resources
{
	byte unknown00[0x84];
	s_predicted_resource_block resources;
	long link_count;
	short *links;
	byte unknown94[0x1c];
};

struct s_cluster_link
{
	short a;
	short b;
	byte unknown04[0x20];
};

struct s_cluster_resource_map
{
	byte unknown00[0x58];
	dword *visibility;
	byte unknown5c[4];
	s_cluster_link *links;
	byte unknown64[0x38];
	long count;
	s_cluster_resources *clusters;
};

bool function_16e5e0(s_predicted_resource_block const *block, short mode);

// @retail 0x3f450
void function_3f450(long cluster_index)
{
	if (cluster_index != NONE)
	{
		s_cluster_resource_map *map = (s_cluster_resource_map *)g_4e0348;
		dword const *visible = map->visibility + ((map->count + 31) >> 5) * (short)cluster_index;
		function_16e5e0(&map->clusters[cluster_index].resources, 0);
		for (long i = 0; i < map->count; ++i)
		{
			if (visible[i >> 5] & (1 << (i & 31)))
				function_16e5e0(&map->clusters[i].resources, 0);
		}
	}
}

// @retail 0x3f500
void function_3f500(long cluster_index)
{
	if (cluster_index != NONE)
	{
		s_cluster_resource_map *map = (s_cluster_resource_map *)g_4e0348;
		dword const *visible = map->visibility + ((map->count + 31) >> 5) * (short)cluster_index;
		function_16e5e0(&map->clusters[cluster_index].resources, 1);
		for (long i = 0; i < map->count; ++i)
		{
			if (g_4c56c0[i >> 5] & (1 << (i & 31)))
			{
				s_cluster_resources *cluster = &map->clusters[i];
				for (long j = 0; j < cluster->link_count; ++j)
				{
					s_cluster_link *link = &map->links[cluster->links[j]];
					short adjacent = link->a == i ? link->b : link->a;
					if ((visible[adjacent >> 5] & (1 << (adjacent & 31))) &&
						!(g_4c56c0[adjacent >> 5] & (1 << (adjacent & 31))))
					{
						cluster = &map->clusters[adjacent];
						function_16e5e0(&cluster->resources, 1);
					}
				}
			}
		}
	}
}

#include "geometry_cache.h"

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

// @retail 0x2d000
long function_2d000(long object_index, long tag, long instance)
{
	long result = 3;
	byte *map;
    byte *definition;
    if (g_4e0344 && g_4e0344->count > 0 &&
        (map = (byte *)g_4e0348) != NULL &&
        (definition = (byte *)g_4e0344->bsp,
		*(long *)(definition + 0x1c) != NONE &&
		*(long *)(definition + 4) == *(long *)(map + 8)))
	{
		if (tag != NONE)
		{
			if (instance == 0x7ff)
				result = 2;
			else
			{
				byte *data = g_4e3b44[tag & 0xffff].bytes;
				byte *tags = *(byte **)(definition + 0x5c);
				if (*(long *)(data + 8) == *(long *)(tags + instance * 12 + 8))
				{
					byte *mappings = *(byte **)(definition + 0x64);
					long section = *(word *)(mappings + instance * 12 + 2);
					byte *sections = *(byte **)(definition + 0x44);
					byte *section_record = sections + section * 0x38;
					if (function_12de70((s_geometry_block_info *)(section_record + 0xc), 3))
						result = 1;
				}
			}
		}
		else if (instance != 0x7ff)
		{
			byte *mappings = *(byte **)(definition + 0x54);
			long section = *(word *)(mappings + instance * 12 + 2);
			byte *sections = *(byte **)(definition + 0x44);
			byte *section_record = sections + section * 0x38;
			if (function_12de70((s_geometry_block_info *)(section_record + 0xc), 3))
			{
				byte *instances = *(byte **)((byte *)g_4e0348 + 0x144);
				result = *(short *)(instances + instance * 0x58 + 0x56) != 0;
			}
		}
        else
            result = 0;
	}
	else if (tag != NONE && object_index != NONE)
	{
		byte *object = (byte *)function_badc0(object_index, NONE);
		if (object && (object[7] & 1))
			result = 2;
	}
	return result;
}

struct s_geometry_section
{
    byte unknown00[0x38];
    s_geometry_block_info block;
};

struct s_geometry_sections
{
    byte unknown00[0x1c];
    long count;
    byte unknown20[8];
    s_geometry_section *sections;
};

// @retail 0x3e320
bool function_3e320(long tag, byte const *indices)
{
    (void)&indices;
    s_geometry_sections *data = (s_geometry_sections *)g_4e3b44[tag & 0xffff].bytes;
    bool result = true;
    for (long i = 0; i < data->count; ++i)
    {
        long index = indices[i];
        if (index != 255)
        {
            byte *section = (byte *)data->sections + index * sizeof(s_geometry_section);
            if (function_12de70((s_geometry_block_info *)(section + 0x38), 3) == false)
                result = false;
        }
    }
    return result;
}

struct s_decal_globals;
extern s_decal_globals *g_4ea94c;
extern long g_4ba038;
bool g_4c8780;
dword g_4c8784;
long g_4c8788, g_4c878c;
long g_4c8790;
void function_144f0(long tag, short stage, long fallback, short fallback_index, short index, real priority);
void function_1cf50(void);

#pragma inline_depth(0)
// score 100 (was 100): this round: no new form. Still BLOCKED-BY 0x144f0 (held file): retail calls
// it with tag/stage/fallback in EAX/ECX/EBX and three stack arguments, our tree uses the standard
// six-argument convention, so the call site and the colour/draw schedule after it differ.
// score 100 (was 100): no frame; hoisted zero and material-call convention, then color/draw scheduling differ.
// Earlier: the record index is scaled by 0x40 in retail and by 0x4 here, and the table retail reads is not the one this tree reads
// Current stride agrees; the hoisted zero, callee convention and color/draw
// scheduling differ. Record view, byte-channel locals, material/color results,
// block-pool/byte-offset lifetimes and typed-record for 100; byte bitmap 98.
// Earlier: BLOCKED-BY 0x144f0, which retail calls with the tag, stage and index in
// EAX/ECX/EBX and three pushes (ours four pushes and the priority in XMM2, zero held in EBX).
// Tag read at every use with all cache-store orders 99; store-order forms without that change
// 100; bitmap read at every use 98. One- and two-point gains rejected. All original cache
// stores and field observations restored.
// Round 414: No frame; loop-wide zero, provider convention and SDK color/draw scheduling remain.
// Material/color/draw workspace scope masks 1-7 scored 100 each; all scopes with native bitmap 98.
// Two-count gain rejected; original scopes and bitmap restored.
// Round 122 (fast; retail reads colour bytes 2/3 from the global, 0/1 from al/ah): manager read at each use 100;
// colour global as a dword/byte union read after the store 98, with the manager change 98 (union with anonymous
// bytes / byte array 98/98); shifts of the global after the store 100; `& 0xff` channels of the global/local 98/100;
// union colour local copied to the global, channels from either 98/98 (unions get a stack copy, `sub esp, 8`).
// @retail 0x44040
void __stdcall function_44040(long cell, short mask)
{
    long handle = ((long *)g_4ea94c)[((short)g_4c8790 << 9) + (short)cell];
    if (handle != NONE)
    {
        do
        {
            byte *entry = (byte *)g_4ea950->data + (handle & 0xffff) * 0x40;
            byte *group = (byte *)g_4ea950->data + (*(long *)(entry + 0x38) & 0xffff) * 0x40;
            if (*(short *)(group + 8) != (short)g_4ba038)
            {
                byte *definition = g_4e3b44[*(long *)(entry + 0x2c) & 0xffff].bytes + 0x6c;
                *(short *)(group + 8) = (short)g_4ba038;
                long tag = *(long *)(definition + 0x20);
                if (g_4c878c != tag || g_4c8788 != *(signed char *)(entry + 0x29))
                {
                    long bitmap = *(signed char *)(entry + 0x29);
                    g_4c8788 = bitmap;
                    g_4c878c = tag;
                    function_144f0(tag, 0, 1, (short)bitmap, 0, 0.0f);
                    function_1cf50();
                }
                dword color = *(dword *)(entry + 0x24);
                if (g_4c8784 != color || g_4c8780)
                {
                    g_4c8784 = color;
                    g_4c8780 = false;
                    D3DDevice_SetVertexData4ub(10, (byte)(color >> 16), (byte)(color >> 8), (byte)color, (byte)(color >> 24));
                    #pragma inline_depth(1)
                    D3DDevice_SetVertexDataColor(11, 0);
                    #pragma inline_depth(0)
                }
                long block_index = *(long *)(entry + 4) & 0xffff;
                long vertex_count = *(short *)(entry + 0x2a) * 4;
                s_physical_object *manager = (s_physical_object *)g_509448;
                s_physical_block *block = (s_physical_block *)manager->blocks->data + block_index;
                dword first_vertex = ((dword)block->offset << manager->page_shift) >> 4;
                D3DDevice_DrawVertices(D3DPT_QUADLIST, first_vertex, vertex_count);
            }
            handle = *(long *)(entry + 0x34);
        }
        while (handle != NONE);
    }
}
#pragma inline_depth()

extern byte *g_4858c4;
long g_485a2c, g_485a64, g_485a68;

// @retail 0x34060
long function_34060(long mode, dword flags)
{
    (void)&flags;
    switch (mode)
    {
    case 0: return g_485a2c;
    case 1: return g_485a2c;
    case 2: return *(short *)(g_4858c4 + 0x8a);
    case 3: return *(short *)(g_4858c4 + 0x80);
    case 4: return g_485a64;
    case 5: return g_485a68;
    case 6:
        if (!(*g_4858c4 & 2) && (bool)((g_4ba014 >> 1) & 1) && (flags & 0x2000))
            return 1;
        return 0;
    case 7: return !((*(dword *)g_4858c4 >> 1) & 1);
    default: __assume(0);
    }
}

struct s_bsp3d;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_leaf_cluster
{
    short cluster;
    byte unknown02[6];
};

struct s_leaf_cluster_map
{
    byte unknown00[0x18];
    s_bsp3d *bsp;
    byte unknown1c[0x10];
    long leaf_count;
    s_leaf_cluster *leaves;
    byte unknown34[0x68];
    long cluster_count;
};

// @retail 0x2b720
bool function_2b720(point3f *point, long *cluster, long *leaf)
{
    long *const *cluster_reference = &cluster;
    s_leaf_cluster_map *map = (s_leaf_cluster_map *)g_4e0348;
    bool result = false;
    long index = function_14a280(map->bsp, 0, point);
    if (index != NONE)
    {
        *leaf = index;
        **cluster_reference = map->leaves[index].cluster;
        result = true;
    }
    else if (*leaf < 0 || *leaf >= map->leaf_count || **cluster_reference < 0 || **cluster_reference >= map->cluster_count)
    {
        *leaf = NONE;
        **cluster_reference = NONE;
    }
    return result;
}

long function_baf80(long object_index);
long function_155760(long index);
long function_14de70(long local_player_index);

struct s_inactive_object_header
{
    byte unknown00[3];
    byte inactive;
    byte unknown04[4];
    byte *object;
};

// score 2 (was 2): object-field load versus table-entry load in the equality;
// test result 23; byte result 18; equality arms 23; const object 2;
// unaliased test result 20; guarded equality 27; table before parent 46;
// outer index with separate guards 2.
// @retail 0x3e9c0
bool function_3e9c0(long object_index)
{
    bool result = false;
    if (object_index != NONE)
    {
        if (g_4b9ed8 != NONE && !function_155760(g_4b9ed8))
        {
            long parent = function_baf80(object_index);
            s_inactive_object_header *header = &((s_inactive_object_header *)g_4e0300->data)[parent & 0xffff];
            if (!header->inactive)
            {
                byte *object = header->object;
                if (function_14de70(g_4b9ed8) == *(long *)(object + 0x13c))
                    result = true;
            }
        }
    }
    return result;
}

extern s_record_pool *g_4e030c;

struct s_render_entry_110
{
    long unknown00;
    long tag;
    byte unknown08[0x4c - 8];
    long object;
    long unknown50;
    short index;
    byte unknown56[0x110 - 0x56];
};

// @retail 0x31520
bool function_31520(long index)
{
    bool result = false;
    if (index != NONE)
    {
        s_render_entry_110 *entry = &((s_render_entry_110 *)g_4e030c->data)[index & 0xffff];
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        bool active = false;
        if (entry->index != NONE)
            active = function_3e9c0(entry->object);
        dword flags = *(dword *)tag;
        if ((!(flags & 0x20) || active) && (!(flags & 0x40) || !active))
            result = true;
    }
    return result;
}

// @retail 0x4c0d0
void function_4c0d0(long object_index, byte *output, long mode)
{
    (void)&mode;
    byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
    if ((signed char)object[0xb1] != -1 && mode == 1)
    {
        for (long i = 0; i < *(long *)(model + 0x70); ++i)
        {
            byte *variant = *(byte **)(model + 0x54) + (signed char)object[0xb1] * 0x38;
            signed char index = (signed char)variant[i + 4];
            if (index != -1)
                ((short *)(output + 0xc2))[i] = *(short *)(*(byte **)(variant + 0x18) + index * 20 + 0x10);
            else
                ((short *)(output + 0xc2))[i] = 0;
        }
    }
    else
        memset(output + 0xc2, 0, 0xa0);
}

#include "unknown_123b30.h"

extern double g_4ba040;
void function_3d270(void);
extern dword g_4ba034;
long g_4ba038, g_4ba03c;
long g_4ba054[4], g_4ba064[4];
struct s_fade_record;
extern s_fade_record *g_50942c;
void function_3b8a0(void);
void function_0167a0(void);

// @retail 0x2b4a0
void function_2b4a0(void)
{
    g_4ba034 = 0;
    g_4ba040 = 0.0;
    g_4ba038 = 0;
    g_4ba03c = 0;
    memset(g_4ba054, 0xff, sizeof(g_4ba054));
    memset(g_4ba064, 0xff, sizeof(g_4ba064));
    function_3b8a0();
    function_3d270();
    function_0167a0();
    g_50942c = (s_fade_record *)function_123d40(0, 0, 0x18);
}

extern real g_45dd38, g_45dd44;

// @retail 0x3f220
point3f *function_3f220(dword a, dword b, dword c, point3f *out)
{
    dword const *reference = &c;
    point3f value;
    real x, y, z;
    x = (real)a * 8.0f;
    value.x = x * g_45dd38;
    y = (real)b * 8.0f;
    value.y = y * g_45dd38;
    z = (real)*reference * 8.0f;
    value.z = z * g_45dd44;
    *out = value;
    return out;
}





// @retail 0x2cb30
void function_2cb30(void)
{
	if (!g_4c0b78.state->active)
	{
		s_2cb30_entry empty = {};
		g_4c0b78.previous = 0;
		g_4c0b78.current = 0;
		g_4c0b78.count = 0;
		g_4c0b78.entries[0] = empty;
		// Retail's forward copy propagates the cleared first entry through the array.
		memcpy(g_4c0b78.entries + 1, g_4c0b78.entries, sizeof(g_4c0b78.entries) - sizeof(g_4c0b78.entries[0]));
		g_4c0b78.entry_count = 0;
	}
	else
		g_4c0b78.current = g_4c0b78.previous;
}



// @retail 0x44940
void *function_44940(short index, bool instance)
{
	(void)&instance;
	s_44940_entry *entry = &g_4ba138[index];
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		return sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 4;
	}
	byte *structure = (byte *)g_4e0348;
	if (!instance)
		return *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
	byte *instances = *(byte **)(structure + 0x144);
	short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
	return *(byte **)(structure + 0x13c) + definition_index * 0xc8;
}

__declspec(noinline) void *function_449e0(short index, bool load, bool instance);

// @retail 0x449e0
void *function_449e0(short index, bool load, bool instance)
{
	bool const *instance_reference = &instance;
	s_44940_entry *entry = &g_4ba138[index];
	void *result = NULL;
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *section = *(byte **)(definition + 0x28) + ((entry->flags >> 9) & 0x1ff) * 0x5c;
		if (!load || function_12de70((s_geometry_block_info *)(section + 0x38), 3))
			result = *(void **)(section + 0x34);
	}
	else
	{
		byte *structure = (byte *)g_4e0348;
		if (!*instance_reference)
		{
			byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
			if (!load || function_12de70((s_geometry_block_info *)(section + 0x28), 3))
				result = *(void **)(section + 0x50);
		}
		else
		{
			byte *instances = *(byte **)(structure + 0x144);
			short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
			byte *section = *(byte **)(structure + 0x13c) + definition_index * 0xc8;
			if (!load || function_12de70((s_geometry_block_info *)(section + 0x28), 3))
				result = *(void **)(section + 0x50);
		}
	}
	return result;
}

PRIVATE __forceinline byte *visible_entry_geometry(short index)
{
	s_44940_entry *entry = &g_4ba138[index];
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		return *(byte **)(sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
	}
	byte *structure = (byte *)g_4e0348;
	byte *data;
	if (!((byte)(entry->unknown00 >> 12) & 1))
	{
		byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
		data = *(byte **)(section + 0x50);
	}
	else
	{
		byte *instances = *(byte **)(structure + 0x144);
		short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
		data = (byte *)*(dword const *)(*(byte **)(structure + 0x13c) + definition_index * 0xc8 + 0x50);
	}
	return data;
}

struct s_render_part_context;
bool function_45ce0(short index, byte *out, short part, short transform_index);
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);

// @retail 0x458d0
void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second)
{
	short const *index_reference = &index;
	byte *geometry = visible_entry_geometry(*index_reference);
	s_44940_entry *entry = &g_4ba138[*index_reference];
	dword *mask = *(dword **)(entry->unknown10 + 4);
	if (first->count > 0 || second->count > 0)
		entry->unknown00 |= 1;
	for (long i = 0; i < first->count; ++i)
	{
		if (!ranges)
		{
			dword bit = (word)first->indices[i];
			mask[bit >> 5] |= 1 << (bit & 31);
		}
		else
		{
			word end = (*(word **)(geometry + 0x34))[(short)first->indices[i + 1] + *(long *)(geometry + 8)];
			for (long j = (*(word **)(geometry + 0x34))[(short)first->indices[i] + *(long *)(geometry + 8)]; j <= end; ++j)
			{
				dword bit = (*(word **)(geometry + 0x34))[(short)j];
				mask[bit >> 5] |= 1 << (bit & 31);
			}
			++i;
		}
	}
	for (long i = 0; i < second->count; ++i)
	{
		dword bit = (word)second->indices[i];
		mask[bit >> 5] |= 1 << (bit & 31);
	}
}

PRIVATE __forceinline void mark_visible_part(byte *geometry, long index, dword *mask)
{
	byte *part = *(byte **)(geometry + 4) + index * 0x48;
	for (long bit = *(short *)(part + 0xa); bit < *(short *)(part + 0xa) + *(short *)(part + 0xc); ++bit)
		mask[bit >> 5] |= 1 << (bit & 31);
}

// @retail 0x45a80
void function_45a80(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second)
{
	short const *index_reference = &index;
	byte *geometry = visible_entry_geometry(*index_reference);
	s_44940_entry *entry = &g_4ba138[*index_reference];
	dword *mask = *(dword **)(entry->unknown10 + 4);
	if (first->count > 0 || second->count > 0)
		entry->unknown00 |= 1;
	for (long i = 0; i < first->count; ++i)
	{
		if (!ranges)
			mark_visible_part(geometry, (word)first->indices[i], mask);
		else
		{
			word end = (*(word **)(geometry + 0x34))[(short)first->indices[i + 1] + *(long *)geometry + 2 * *(long *)(geometry + 8)];
			for (long j = (*(word **)(geometry + 0x34))[(short)first->indices[i] + *(long *)geometry + 2 * *(long *)(geometry + 8)]; j <= end; ++j)
			{
				long part = (*(word **)(geometry + 0x34))[(short)j + 2 * *(long *)(geometry + 8)];
				mark_visible_part(geometry, part, mask);
			}
			++i;
		}
	}
	for (long i = 0; i < second->count; ++i)
		mark_visible_part(geometry, second->indices[i], mask);
}




void function_3d600(void);

struct s_planar_camera_source
{
	point3f origin;
	vector3f normal;
	vector3f horizontal;
	byte unknown24[0x40 - 0x24];
	real offset;
	real depth;
};
struct s_planar_camera;
struct s_timed_effect_globals;
void function_441b0(s_planar_camera_source const *source, s_planar_camera *state, byte const *view);
s_timed_effect_globals *function_01fd20(s_timed_effect_globals *result);
s_bit_vector_pool *function_1320f0(long mode, long pass, bool enabled, long extra,
	byte const *camera, byte const *plane, real distance, void *filter, real offset, long identifier);
void function_133390(s_bit_vector_pool const *data);
void __stdcall function_133520(s_bit_vector_pool *data);
void function_133720(s_bit_vector_pool *data, bool enabled);
void function_44720(void);
void function_2cb30(void);

// @retail 0x2cbc0
void __stdcall function_2cbc0(bool enabled)
{
	s_bit_vector_pool *current = (s_bit_vector_pool *)g_4c0b78.state;
	function_2cb30();
	function_133390(current);
	function_133520(current);
	function_133720(current, enabled);
}

void function_3f2c0(s_planar_camera_source const *source, byte const *view, long object_index, long identifier);

extern byte g_4ba025;
void __stdcall function_3f830(byte const *tree, point3f const *center);
long function_155760(long index);
void function_3f3f0(void);

// score 23 (was 23): the camera setup is called with one argument in a register and three on the stack, retail passes two in registers and two on the stack
// Now only the camera setup's fourth argument differs: retail loads and pushes g_4b9ee4 (0x3f2c0 ends with ret 4 and never reads it); ours drops the unused parameter.
// Tried: the local player getter with a NONE preset 45; with if/else 23 (kept, replaces a goto); the identifier as 0x3f2c0's first parameter 23, second 23; UNREFERENCED_PARAMETER in 0x3f2c0 23.
// @retail 0x2b990
void function_2b990(long local_player_index)
{
	byte const *tree = *(byte **)((byte *)g_4e0348 + 0x238);
	long object_index;
	long player_index = function_14de70(local_player_index);
	if (player_index != NONE && !function_155760(local_player_index))
		object_index = *(long *)((byte *)g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x2c);
	else
		object_index = NONE;
	function_3f2c0((s_planar_camera_source const *)&g_camera_storage, (byte const *)&g_projection_storage, object_index, g_4b9ee4);
	function_3f3f0();
	if (g_4ba025)
		function_3f830(tree, &g_camera_storage.field_0);
}

// @retail 0x3d4f0
void function_3d4f0(bool cached, short cache_index, long *flags,
	long *source_object, long *tag, transform4x3f **transforms, dword *count, long object_index)
{
	short const *cache_reference = &cache_index;
	(void)&flags;
	(void)&source_object;
	(void)&tag;
	(void)&transforms;
	dword *const *count_reference = &count;
	if (cached)
	{
		s_3d4f0_entry *entry = &g_4c1bd8[*cache_reference];
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		*tag = entry->tag;
		*transforms = entry->transforms;
		dword n = *(dword *)(definition + 0x48);
		if (n > 64) n = 64;
		**count_reference = n;
		*flags = entry->flags;
		*source_object = entry->object_index;
	}
	else
	{
		long offset = (object_index & 0xffff) * sizeof(s_render_object_header);
		byte *object = *(byte **)(offset + (dword)g_4e0300->data + 8);
		byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
		byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
		long model_tag = *(long *)(model + 4);
		byte *geometry = g_4e3b44[model_tag & 0xffff].bytes;
		*tag = model_tag;
		object = *(byte **)(offset + (dword)g_4e0300->data + 8);
		**count_reference = (dword)(long)*(short *)(object + 0x114) / sizeof(transform4x3f);
		*transforms = (transform4x3f *)(object + *(short *)(object + 0x116));
		**count_reference = *(dword *)(geometry + 0x48);
		*flags = 0;
		*source_object = object_index;
	}
}

// @retail 0x460d0
bool function_460d0(dword const *mask, short index, long part_index)
{
	// The render-entry index is passed on the stack in retail.
	short const *index_reference = &index;
	dword flags = g_4ba138[*index_reference].unknown00;
	s_44940_entry *entry = &g_4ba138[*index_reference];
	bool result = true;
	if (flags & 0x40)
	{
		byte *data;
		result = false;
		if (entry->tag != NONE)
		{
			byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
			byte *section = *(byte **)(definition + 0x28) + ((entry->flags >> 9) & 0x1ff) * 0x5c;
			data = *(byte **)(section + 0x34);
		}
		else
		{
			byte *structure = (byte *)g_4e0348;
			if (!((byte)(flags >> 12) & 1))
			{
				byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
				data = *(byte **)(section + 0x50);
			}
			else
			{
				byte *instances = *(byte **)(structure + 0x144);
				short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
				data = (byte *)*(dword const *)(*(byte **)(structure + 0x13c) + definition_index * 0xc8 + 0x50);
			}
		}
		byte *part = *(byte **)(data + 4) + part_index * 0x48;
		long first = *(short *)(part + 0xa);
		long count = *(short *)(part + 0xc);
		for (long i = first; i < first + count && !result; ++i)
			result = (mask[i >> 5] & (1 << (i & 31))) != 0;
	}
	return result;
}

/* the render model sections, groups and variants, as read here */
struct s_4c2b0_section
{
    byte unknown00[0x38];
    s_geometry_block_info block;
};

struct s_4c2b0_variant
{
    byte unknown00[4];
    short section_indices[6];
};

struct s_4c2b0_group
{
    byte unknown00[8];
    long variant_count;
    s_4c2b0_variant *variants;
};

struct s_4c2b0_definition
{
    byte unknown00[0x1c];
    long group_count;
    s_4c2b0_group *groups;
    long section_count;
    s_4c2b0_section *sections;
};

// @retail 0x4c2b0
bool function_4c2b0(long tag, byte const *wanted, signed char *current, long level,
    bool request, signed char *sections, bool *fallback)
{
    (void)&wanted; (void)&current; (void)&level;
    (void)&request; (void)&sections; (void)&fallback;
    s_4c2b0_definition *definition = (s_4c2b0_definition *)g_4e3b44[tag & 0xffff].bytes;
    *fallback = false;
    word available = 0;
    bool result = true;
    for (long i = 0; i < definition->group_count; ++i)
    {
        s_4c2b0_group *group = &definition->groups[i];
        long selection = wanted[i];
        if (selection != NONE && selection < group->variant_count)
        {
            s_4c2b0_section *section = &definition->sections[group->variants[selection].section_indices[level]];
            if (request)
            {
                if (function_12de70(&section->block, 3)) available |= 1 << i;
            }
            else
            {
                if (function_12de70(&section->block, 0)) available |= 1 << i;
            }
        }
    }
    for (long i = 0; i < definition->group_count; ++i)
    {
        s_4c2b0_group *group = &definition->groups[i];
        if (available & (1 << i))
        {
            sections[i] = (signed char)group->variants[(signed char)wanted[i]].section_indices[level];
            current[i] = wanted[i];
        }
        else if (wanted[i] != 0xff)
        {
            if (current[i] != -1)
            {
                s_4c2b0_variant *variant = &group->variants[current[i]];
                s_4c2b0_section *section = &definition->sections[variant->section_indices[level]];
                if (function_12de70(&section->block, 0))
                {
                    sections[i] = (signed char)variant->section_indices[level];
                    *fallback = true;
                }
                else result = false;
            }
            else
            {
                *fallback = true;
                sections[i] = -1;
            }
        }
        else sections[i] = -1;
    }
    return result;
}

struct s_light_shape_ab;
bool function_c3140(long index);
bool function_c17f0(long light_index, s_light_shape_ab *shape, bool respect_engine);

// @retail 0x31590
bool function_31590(long index, s_light_shape_ab *shape)
{
    volatile bool enabled = false;
    bool result = false;
    bool *result_reference = &result;
    if (index != NONE && function_c3140(index))
    {
        s_render_entry_110 *entry = &((s_render_entry_110 *)g_4e030c->data)[index & 0xffff];
        byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
        bool visible;
        if (function_31520(index))
        {
            *result_reference = function_c17f0(index, shape, true);
            visible = true;
            if ((*(dword *)definition & 0x400) && g_485602 != 2)
                visible = false;
        }
        else
            visible = enabled;
        *result_reference &= visible;
        *result_reference &= true;
        *result_reference &= *(short *)((byte *)entry + 0x48) != NONE ||
            ((*(dword *)definition & 0x20) && (*(dword *)definition & 0x40000));
    }
    else
        return false;
    return *result_reference;
}

#include <math.h>
struct s_camera;
struct s_tag_data;
real function_50650(real cosine);
real function_13bb90(s_tag_data const *function, real input, real range);
long function_30830(point3f const *a, s_camera const *camera, vector3f const *d,
    point2f const *scale, bool perspective, bool negate, point2f *out);

void function_4ff50(byte const *definition, byte const *state, bool facing, real time, real cosine, real *strength, real *horizontal, real *vertical, real *out);


struct s_137801;
long function_137650(long model_index, long lod, byte const *permutations);
void function_137800(long object_index, long model_index, transform4x3f const *nodes,
    byte const *permutations, long lod, bool alternate, s_137801 *result);

struct s_matrix_workspace
{
    long size;
    byte data[0x27000];
};
s_matrix_workspace g_487b18;

/* the handle of `bytes` (rounded up to dwords) of the frame's render memory, or NONE when it is full */
static inline long render_memory_allocate(long bytes)
{
    long result = NONE;
    long whole_size = bytes;
    long remainder = bytes % 4;
    if (remainder)
        whole_size = bytes - remainder + 4;
    long size = g_487b18.size + whole_size;
    if (size <= 0x27000)
    {
        result = g_487b18.size & 0x0fffffff;
        g_487b18.size = size;
    }
    return result;
}

// score 1 (was 1): the permutation pointer arrives in eax, retail passes it in ecx; the register is fixed by the caller function_4c690, which is not retail's yet
// frame pointer 27; alias analysis off 1; global expressions off 62; inline depths one/zero 1/89; native result/count 1/1; input role names 1;
// the permutations last or first (old order kept for the caller by a forwarding overload) 1/1; a const member of a 16-byte permutation type 1;
// extra_bytes read once through its address 1; (void)&node_count 1; the node-count callee's parameters
// reordered (permutations first, both callers) 1; the count read back through node_count 1; the output
// pointer as a conditional argument 27; the byte count as a conditional expression 54; bytes declared
// before the count call 15. Every parameter position, __stdcall/__cdecl and flag set keeps eax.
// Round 10: model index and lod address-taken 1; node count and extra bytes address-taken 1; the count read
// back through node_count 1; the output pointer as a conditional expression 27; bytes assigned in an else 54.
// One-file LTCG probe (this file + 0x137650/0x137800's files): an entry calling this function directly
// also gets eax, so the caller does not fix it; unused extra parameters are dropped and every position of
// the permutations still gets eax.
// @retail 0x3e380
long function_3e380(long model_index, byte lod, transform4x3f const *nodes,
    byte const *permutations, long *node_count, long extra_bytes)
{
    long count = function_137650(model_index, lod, permutations);
    *node_count = count;
    long bytes = 0;
    if (count > 0)
        bytes = count * 48 + 0x44;
    long result = render_memory_allocate(bytes + extra_bytes);
    if (result != NONE)
    {
        s_137801 *out = NULL;
        if ((result & 0x80000000) == 0)
            out = (s_137801 *)(g_487b18.data + (result & 0x0fffffff));
        function_137800(NONE, model_index, nodes, permutations, lod, false, out);
    }
    return result;
}

#include "unknown_030290.h"

struct s_bit_vector_pool;
dword *function_1332b0(long bit_count, s_bit_vector_pool *data);
long function_016ae0(real value);
void function_30290(point3f const *point, s_view const *view, s_camera const *camera, real radius, box2f *bounds);
byte g_509410, g_509411;


void __stdcall function_44e90(short entry_index, short section_index);


transform4x3f *function_b8c00(long object_index, long *node_count);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
struct s_visibility_sphere_query
{
    point3f center;
    real radius;
};
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);

signed char *function_3d430(long object_index);
__declspec(noinline) void function_4c640(long object_index, signed char value);

extern s_record_pool *g_509434;
extern long g_4ba050;
long __stdcall function_3ddd0(long object_index);
extern long g_4ba01c;
extern bool g_4ba019;
extern byte *g_485a80;
bool object_or_parent_hidden(long object_index);
bool __stdcall function_bab40(long object_index, long name, real *value);

long function_4c450(long object_index, real distance, real const *definition, real *fraction);

short function_4bcc0(byte *output, long object_index, real distance, long override,
    bool *cached, bool force, signed char *level);

// @retail 0x4c190
bool function_4c190(long *count, long object_index, bool defaults, long tag,
    short *level, signed char fallback_level, signed char *sections, bool *fallback)
{
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    byte zero[16];
    byte const *wanted;
    signed char *current;
    if (!defaults)
    {
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        *count = *(short *)(object + 0x118);
        byte const *object_wanted = object + *(short *)(object + 0x11a);
        *count = *count / 10;
        wanted = object_wanted;
        current = function_3d430(object_index);
    }
    else
    {
        memset(zero, 0, sizeof(zero));
        *count = *(long *)(definition + 0x1c);
        current = (signed char *)zero;
        wanted = zero;
    }
    bool result = function_4c2b0(tag, wanted, current, *level, true, sections, fallback);
    if (!result && fallback_level != -1)
    {
        result = function_4c2b0(tag, wanted, current, fallback_level, false, sections, fallback);
        if (result) *level = fallback_level;
    }
    else
        function_4c640(object_index, (signed char)(byte)*level);
    if (!result)
        function_4c640(object_index, -1);
    return result;
}

extern byte g_4670bc;
extern long g_4858b8;
extern D3DPIXELSHADERDEF g_484f68;
extern D3DResource *g_509444;
struct s_render_reset_state;
struct s_shader_cache;
struct s_coefficient_layout;
struct s_lighting_record;
bool __stdcall function_23690(s_coefficient_layout const *layout, short order, long object_index,
    s_137801 const *vertices, long count, byte const *indices, long tag);
bool function_3d480(long object_index, long count, transform4x3f const *matrices, transform4x3f *out);
void function_3dd10(long object_index, bool force);
s_lighting_record *function_3db00(long object_index, real priority);

struct s_batch_cache_4b5d0
{
    byte unknown00[8];
    long handle;
    dword stamp;
    byte lighting[0x54];
};

struct s_133f70;

// score 175 (was 175): frame 0x33f8 agrees; distance staging, nullable-entry/cache adds and result homes remain.
// Round 22 (lane G, machine 3; no code change kept): retail's byte home and its split record base
// (add eax, ecx; lea ebx, [eax + 0xbc]/[eax + 0x44]) are reachable in the source but our compiler folds the
// field offset into the load or into one lea, so the instruction count and the encoded sizes change and the
// tail is compared out of step (byte home 319, split record 318); the 0x23690 call keeps its order in a
// register (another file's callee), so the tail stays one instruction out of step as well. The parameter
// frame matches retail slot for slot (object_index entry+4, distance entry+8, record entry+0x10, read
// from retail's own call site at 0x1339f1), so the prologue differs only in the register and the position
// of the distance load (retail EAX before the force push, ours ECX after it) and in the dead lea retail
// keeps in the 0x4ed284 test.
// Round 15: split entry-count declaration 213; cache levels base with 0xbc at the index 197; native entry
// arithmetic 197; full second-cache record 325; two-step second cache 197; Boolean builder with wide previous 177;
// signed-byte previous 319; direct cache publication 348. Levels offset and Boolean builder with wide previous kept.
// Full frame 0x33f8 agrees; distance staging, nullable entry and cache/result homes remain.
// Round 8 (lane 10, all plain; the reduced image builds this frame differently): bool built 325 (after
// the 0x4c190 call the code is retail's, shifted one instruction by the unfolded record addresses);
// with an inline record getter for g_509434 325 (still folds to one lea); drawn object reused for
// the cache touch, bool/long built 331/224; level pointer typed before the field offset 325 (folds).
// Retail alone among this pool's readers (0x4c640 folds) adds the record and the offset in two steps.
// distance load, nullable-entry address and cache/level scheduling differ;
// distance snapshot 219; wide previous level 219; fallback clear 362; common buffer 219; wide success 254.
// frame 0x33f8 agrees; distance load and cache/result scheduling differ.
// frame 0x33f8 agrees; the count is lockstep: retail adds the cache record base
// before its field offset (`add eax, ecx; lea ebx, [eax + 0xbc]`, also at +0x44) where ours folds one lea,
// keeps a dead entry lea in the 0x4ed284 test, compares the previous level from memory after the two
// slot stores, and tests the 0x4c190 result as a byte; distance staging and cache/result homes differ.
// Round 9 (lane 11, fast): bool builder result 325 (retail length; region of the previous level is then
// one instruction short); with the flag store before the cache store 337; reuse set inside an if 343;
// address-taken previous level 325 (diagnostic); record base through an inline helper 325.
// Retail also sign-extends the level it pushes to 0x23690 (`movsx ecx`), ours pushes a short.
// Distance snapshot 219; wide previous level 219; fallback clear 362; common buffer 219; wide success 254.
// Native reuse 330; native level change 291; short entry index 352; late success 262; short builder
// result 216.
// Direct successful-output slot, entry lifetime: long/long 379, long/short 371,
// short/long 354, short/short 350; loop lifetime 360,372,374,375.
// distance input, nullable-entry address and cache/result scheduling remain.
// Shared level result 378; direct reuse result 316; both 343; builder-first rejection 216;
// observed object for cache/cache-pool calls 224/332; direct cache publication 218;
// direct cache with shared level/reuse results 352. All measured in the full image.
// distance staging, nullable entry and cache/result schedule remain.
// Plain signed-byte/short/word success 213/265/265; short/word reuse 370/370;
// signed-byte level change 213; unsigned failures 213; word entry ordinal 352.
// All eight also checked plain because the reduced frame differs; original restored.
// Native direct output-array groups (handles/counts, object/tags, cache/bytes):
// seven combinations 358f,356f,356f,358f,358f,356f,358f; all with native sections 358f;
// unchanged reduced control 356f; full baseline 213. Original arrays and aliases restored.
// Round 225: Entry query flags/nodes/count and their seven combinations 356f each; all outputs with later
// constant failure-count lifetime 371f. Reduced control 356f versus full 213; original body restored.
// Round 227: Native output short/Boolean/dword fields and their seven combinations 356f each; all
// fields with current-level reference 356f. Reduced control 356f versus full 213; original metadata,
// body and declarations restored.
// Native pooled record at level/batch/usage boundaries 213/213/213; level+batch,
// level+usage, batch+usage, all members and native const level elements each 213 plain
// (all eight 356 fast). Original record access and body restored.
// Round 330: Query/builder/output-query/lighting inline and noinline declarations: eight at 356f, matching
// reduced control (frame 0x3400), versus full 213. All declarations restored; no reduced-only candidate.
// Round 215: Complete per-entry mutable/const frame/view captures 372/372, function-entry
// captures 384/384; shared two-use stamp on those scopes 361/361/383/383 (control 213).
// All introduce a frame difference; complete original observations and body restored.
// Native cache/previous and stamp/handle acceptance orders, logical fallback
// and direct level invalidation: 326/213/326/366/369/367/369/349 fast against expanded control 213.
// Original cache predicate, bitwise fallback and level guard restored.
// Round 139: Shared cache-index homes at level+batch, level+usage, batch+usage and all three
// boundaries each 213; those four with the uninitialized builder result declared at loop entry each
// 213. Broad unchanged control also 213; reduced 0x31590 discrepancy already present. Full frame
// 0x33f8 agrees; distance staging, nullable-entry and cache/result scheduling remain. Original
// executable source restored.
// Complementary failure bool/byte/short/long 235/235/268/259 plain; also complementary
// reuse 326/326/335/326 plain (fast 355/355/344/350). All restored; frame and staging remain.
// Projection branch scope, later failure constant and earlier level constant combinations:
// 356/371/371/356/356/371/371 fast; separate level initialization 356 fast.
// Complete retained cache receiver: null-first/conditional/absent-first/present-first
// 366/374/371/374; same producers with one shared cache-index home 366/374/371/374.
// All add frame bytes; original slot observations and body restored.
// Round 246: common success/failure accounting, bool/byte/short/long result
// at the builder result 240/240/286/279; early result declaration 240/240/286/279.
// Expanded reduced control 213; original independent rejection and body restored.
// Round 191: Query-output flags/nodes/count workspace names, all seven combinations,
// each 213; all three with projection-array name 213. Expanded control 213.
// Original outputs, projection array and executable body restored.
// Lane 4 r98 (all plain, control 213; the score compares instructions in lockstep, so one instruction
// more or less before a region shifts it): retail tests the 0x4c190 result as a bool (`test al, al`);
// `bool built` gives that and retail's 1309 bytes but 325 (the 0x4c190 call setup is one instruction short
// of retail's, so everything after it shifts). With it: 0x100-byte cache records typed (fold the same) 325;
// a nullable record before the levels pointer (real null test) 338; an inline active-entry helper for
// the 0x4ed284 test (null test kept, no dead lea) 308; a view pointer inside the test 325. Retail's
// record + offset adds (`add eax, ecx; lea ebx, [eax + 0xbc]`) and the dead entry lea stay unreproduced.
// Round 224: Signed-short query count/failure count/derived-slot domains, seven combinations,
// then all with short entry ordinal 368/216/367/229/364/229/364/354 fast. Original count/compaction widths restored.
// Round 202: Consistent pooled-index/byte-offset expressions at all three acquisitions: long index, long/dword offset, word index, word+offset, dword+offset, split mask, split multiply 175/175/175/356/356/175/175/175 fast.
// Original executable source restored. Frame 0x33f8 agrees; distance staging, nullable-entry address, cache-member adds and result homes remain.
// Round 14 (lane 10, plain): signed-char previous with signed-char levels 319 (gives retail's byte store,
// byte level load, `cmp byte` test and `movsx ax` publication, but ours folds the cache-slot address into one
// lea where retail adds the record first, so the code after it runs one instruction ahead); with the levels
// pointer at record + 0xbc as 0x4c450 writes it 325 (then both sites fold); stamp compared on the left 319,
// on this body 175; the reuse test through the cache local 320.
// Complete defaults predicate bool/byte/short/long snapshots 291/311/365/368;
// on that predicate lifetime with signed-byte previous level 295/336/337/357.
// Expanded unchanged control 175; original observations and previous level restored.
// Round 262: saved-level short at original/pre-query/function scope 194/195/194;
// signed-byte at pre-query/function/output scope 321/319/321; long at pre-query/function
// scope 176/175. All restored; distance staging and cache/result scheduling remain.
// Round 218: Count/writer/projection/coefficient inline and noinline declarations:
// 175/175/175/175/175/175/175/175 (fast control 175). Original restored.
// Round 237: Actual object type/cache-link/tag receiver fields, all seven combinations, then all with readonly header: eight calibrated scores 175. Original views and complete executable source restored.
// Round 87 (plain; fast now gives 353): the tail from the 0x23690 call on (about 80 of the 175) follows
// that callee's convention: ours passes the level in AX, retail pushes all seven arguments (`ret 0x1c`,
// `movsx ecx` of the level), BLOCKED-BY 0x23690. Head forms: the 0x4ed284 test nested with an entry
// pointer 175, nested without it 175; a cached-flag pointer at every access 175; the header as table
// pointer + masked index 175; entries declared before the failure count 175.
// Round 153 (plain): success declared first 173; inline active-entry getter (retail's dead entry lea) 177; with
// success first 175; plus signed-char previous through a levels pointer 309 (byte forms right, both pool sites fold
// record + offset where retail adds first); 0x23690's order read through its address 182 (ret 0x1c).
// Round 108 (plain control 175, aligned 76; fast control 353): header pointer formed before the
// count store 176; retail's two-step `add eax, ecx; lea ebx, [eax + 0x44]` (and +0xbc) comes back
// when the record address and the member address sit in separate blocks under the same NONE test
// (record set in one `if (index != NONE)`, member in a second; diagnostic, not a kept form): cache
// site 239f, both sites 239f, both with a signed-char previous level 266 plain (aligned 75, retail's
// 382 instructions); cache slot as a ternary 304f (folds, adds a jmp). The tail remains
// BLOCKED-BY 0x23690. All restored.
// Round 184: Native object/tag, handle/count and cache/sections pointer-home names, masks 1-7, then
// all six in a second coherent lexical order: eight calibrated scores 175. Original names, all
// executable statements, native records, flags, query/allocator inputs and call tuples restored.
// Round 265: six lexical orders of entry/failure/compacted-slot long values and
// both uninitialized entry/slot storage orders before the query, 175 each on the expanded control.
// Original declarations, names, compaction, cache observations and complete body restored.
// Round 174: function-scope fallback/reuse/level-change result homes, all seven
// subsets, 175 each; all plus the builder result 175. Original producer points,
// separate iteration storage and complete body restored; distance staging remains.
// DH round 236: full-width coefficient-order interface on the current callee graph, with
// bool/long success, long/signed-byte previous and inner/entry success storage:
// 175/238/319/175/327/238/319/327 on the calibrated caller image (narrow control 353).
// Original interface and entire body restored; distance/cache homes and call transport remain.
// Round 398: complete once-read query object/tag/both 357/352/335; each with
// allocation-scoped projection workspace 357/352/335; immutable joint inputs
// with either workspace 335/335. All original field observations and body restored.
// Round 410: Complete published memory-handle scalar with object/tag/both query captures
// 291/363/352/338; those four with native previous-level byte 294/328/311/342. Original
// handle observations, query inputs, previous level and complete executable body restored.
// Round 193: separate rendering input/shared allocated buffer/direct layout read 241/311/297;
// input+buffer/input+layout/buffer+layout/all 324/298/317/327;
// all with readonly rendering input 327. Complete original body restored.
// Round 122 (plain; fast scores 353 and misleads): type stored through one ternary 175 (aligned 80); named
// flag-byte pointer 175; header before the count store 176; entry-table test nested 175; one shared cache-index
// local for both 0x3ddd0 lookups 175.
// @retail 0x4b5d0
void function_4b5d0(bool force, long override, long object_index, real distance, real priority,
    s_133f70 *record, point3f const *unused, bool project_nodes)
{
    byte *output = (byte *)record;
    transform4x3f projected[255];
    long failed = 0;
    signed char initial_level = -1;
    long entries = function_4bcc0(output, object_index, distance, override,
        (bool *)(output + 0x168), force, &initial_level);
    *(short *)(output + 0x164) = initial_level;
    if (output[0x168] && g_4b9ed8 != NONE && g_4ed284->entries[g_4b9ed8].index != NONE)
        entries = 0;
    *(short *)(output + 0x162) = 0;
    s_render_object_header *header = &((s_render_object_header *)g_4e0300->data)[object_index & 0xffff];
    short type = NONE;
    if (header->unknown00[3] == 6) type = *(short *)((byte *)header->object + 0x134);
    *(short *)(output + 0x166) = type;
    for (long index = 0; index < entries; ++index)
    {
        long slot = index - failed;
        long flags;
        transform4x3f *nodes;
        dword node_count;
        long *source_object = (long *)(output + 0x30) + slot;
        long *tag = (long *)(output + 0x58) + slot;
        long *count = (long *)(output + 0x14) + slot;
        long *handle = (long *)output + slot;
        s_batch_cache_4b5d0 **cache_slot = (s_batch_cache_4b5d0 **)(output + 0x44) + slot;
        bool success = true;
        function_3d4f0(*(bool *)(output + 0x168), (short)index, &flags, source_object,
            tag, &nodes, &node_count, object_index);
        /* the level the object's cache entry was last drawn at (-1 without a mode) */
        long drawn_object = *source_object;
        long previous = -1;
        if (g_4ba050 >= 0 && g_4ba050 < 4)
        {
            byte *levels = NULL;
            long cache_index = function_3ddd0(drawn_object);
            if (cache_index != NONE)
                levels = g_509434->data + (cache_index & 0xffff) * 0x100;
            previous = (signed char)levels[g_4ba050 + 0xbc];
        }
        function_3d430(*source_object);
        s_batch_cache_4b5d0 *cache = NULL;
        long cache_index = function_3ddd0(*source_object);
        if (cache_index != NONE)
            cache = (s_batch_cache_4b5d0 *)(g_509434->data + (cache_index & 0xffff) * 0x100 + 0x44);
        *cache_slot = cache;
        output[slot + 0x6c] = (byte)(flags & 1);
        bool reuse = previous != -1 && *cache_slot && (*cache_slot)->handle != NONE &&
            (*cache_slot)->stamp == ((g_4ba034 & 0x3fffffff) | ((dword)g_4ba050 << 30));
        if (reuse)
            *(short *)(output + 0x164) = previous;
        bool fallback;
        signed char *sections = (signed char *)(output + 0x71 + slot * 16);
        bool built = function_4c190(count, object_index, *(bool *)(output + 0x168), *tag,
            (short *)(output + 0x164), previous, sections, &fallback);
        reuse &= !fallback;
        bool level_changed = *(short *)(output + 0x164) != initial_level;
        if (reuse && level_changed) reuse = false;
        if ((index > 0 && *(short *)(output + 0x164) != initial_level) || !built)
        {
            ++failed;
            continue;
        }
        long total = function_137650(*tag, *(short *)(output + 0x164), (byte const *)sections);
        output[slot + 0x28] = (byte)total;
        *handle = NONE;
        if (!reuse || output[0x168])
        {
            long bytes = 0;
            if (total > 0)
                bytes = total * 48 + 0x44;
            *handle = render_memory_allocate(bytes);
            if (*handle != NONE)
            {
                if (*cache_slot && !output[0x168] && !fallback && !level_changed)
                {
                    (*cache_slot)->stamp = (g_4ba034 & 0x3fffffff) | ((dword)g_4ba050 << 30);
                    (*cache_slot)->handle = *handle;
                }
                if (project_nodes)
                {
                    function_3d480(object_index, node_count, nodes, projected);
                    nodes = projected;
                }
                s_137801 *result = NULL;
                if (*handle != NONE && (*handle & 0x80000000) == 0)
                    result = (s_137801 *)(g_487b18.data + (*handle & 0x0fffffff));
                function_137800(object_index, *tag, nodes, (byte const *)sections,
                    *(short *)(output + 0x164), *(bool *)(output + 0x168), result);
            }
            else
                success = false;
        }
        if (!reuse && success)
        {
            if (*(dword *)(output + 0x16c) & 0x40)
            {
                byte *definition = g_4e3b44[*tag & 0xffff].bytes;
                s_coefficient_layout const *layout = *(s_coefficient_layout **)(definition + 0x78);
                s_137801 const *vertices = NULL;
                if (*handle != NONE && (*handle & 0x80000000) == 0)
                    vertices = (s_137801 const *)(g_487b18.data + (*handle & 0x0fffffff));
                success = function_23690(layout, *(short *)(output + 0x164), *source_object,
                    vertices, *count, (byte const *)sections, *tag);
            }
            function_3dd10(*source_object, false);
            if (*cache_slot)
                memcpy((*cache_slot)->lighting, function_3db00(*source_object, priority), 0x54);
        }
        else if (reuse && success && !output[0x168] && g_4ba050 != NONE)
            *handle = (*cache_slot)->handle;
        if (success)
        {
            ++*(short *)(output + 0x162);
            byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[*source_object & 0xffff].object;
            long cache_index = *(long *)(object + 0xcc);
            if (cache_index != NONE)
            {
                byte *cache = g_509434->data + (cache_index & 0xffff) * 0x100;
                if (*(long *)(cache + 4) == *source_object) *(dword *)(cache + 0xc) = g_4ba034;
            }
        }
        else ++failed;
    }
}
struct s_type_7ba8e9;
void function_16b10(s_render_reset_state *state);
s_type_7ba8e9 *function_137550(long tag, short bitmap);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
void function_14bc0(short target, short element, bool depth);
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
bool function_1ccf0(D3DPIXELSHADERDEF const *program);
dword function_1cc30(long index);
void function_1c620(s_shader_cache *state, long a, long b, long c, byte const *descriptor);
void __stdcall function_1c710(void *state);

void __stdcall function_43a40(long mode);

struct s_masked_list;
typedef void (__stdcall *masked_list_proc)(long value, short mask);
extern dword g_4b8340;
void masked_list_iterate(s_masked_list const *list, long mask, masked_list_proc proc);

// @retail 0x2c370
void function_2c370(long mode)
{
    function_43a40(mode);
    masked_list_iterate(g_547f94, 0xffff, function_44040);
    g_4b8340 = 0;
    D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 0);
}

s_target_cache_storage g_target_cache_storage;

s_material_records_storage g_material_records_storage;
