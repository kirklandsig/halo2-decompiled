// @flags /O2 /Gr
/* UNKNOWN_03F2B0.CPP: visibility lists, resource prediction and index caches */

#include "unknown_11c920.h"
#include "globals.h"
#include "index_cache_storage.h"
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

long g_4c1bd4;

struct s_render_object_header
{
	byte unknown00[8];
	long *object;
};

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
struct s_bit_vector_pool_sizes
{
	short unknown0;
	short list_sizes[4];
	short record_count;
};

struct s_bit_vector_pool
{
	void *context;
	byte unknown004[8];
	c_entry_list *lists[4];
	long indices[0x80];
	dword flags[0x10];
	dword pool[0x200];
	word pool_used;
	word entry_count;
	dword entries[0x200][4];
	dword flags2a60;
	byte unknown2a64[0x2a88 - 0x2a64];
	plane3f plane;
	byte unknown2a98[0x2acc - 0x2a98];
	byte *records;
	byte unknown2ad0[4];
	s_bit_vector_pool_sizes sizes;
};
extern s_bit_vector_pool g_547f88;
extern dword g_4c56c0[64];

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
	for (long i = 0; i < 16; ++i)
		g_4c6b00[7].values[i] = NONE;
	g_50943c = j;
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
					if (function_12de70((s_geometry_block_info *)(sections + section * 0x38 + 0xc), 3))
						result = 1;
				}
			}
		}
		else if (instance != 0x7ff)
		{
			byte *mappings = *(byte **)(definition + 0x54);
			long section = *(word *)(mappings + instance * 12 + 2);
			byte *sections = *(byte **)(definition + 0x44);
			if (function_12de70((s_geometry_block_info *)(sections + section * 0x38 + 0xc), 3))
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
            result &= function_12de70(&data->sections[index].block, 3);
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
                byte *definition = g_4e3b44[*(long *)(entry + 0x2c) & 0xffff].bytes;
                *(short *)(group + 8) = (short)g_4ba038;
                long tag = *(long *)(definition + 0x8c);
                long bitmap = *(signed char *)(entry + 0x29);
                if (g_4c878c != tag || g_4c8788 != bitmap)
                {
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
                    D3DDevice_SetVertexDataColor(11, 0);
                }
                s_physical_object *manager = (s_physical_object *)g_509448;
                s_physical_block *block = (s_physical_block *)manager->blocks->data + (*(long *)(entry + 4) & 0xffff);
                D3DDevice_DrawVertices(D3DPT_QUADLIST, ((dword)block->offset << manager->page_shift) >> 4, *(short *)(entry + 0x2a) * 4);
            }
            handle = *(long *)(entry + 0x34);
        }
        while (handle != NONE);
    }
}

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
extern long g_4b9ed8;

struct s_inactive_object_header
{
    byte unknown00[3];
    byte inactive;
    byte unknown04[4];
    byte *object;
};

// @retail 0x3e9c0
bool function_3e9c0(long object_index)
{
    bool result = false;
    if (object_index != NONE)
    {
        long index = g_4b9ed8;
        if (index != NONE && !function_155760(index))
        {
            long parent = function_baf80(object_index);
            s_inactive_object_header *header = &((s_inactive_object_header *)g_4e0300->data)[parent & 0xffff];
            if (!header->inactive)
            {
                byte *object = header->object;
                long entry = g_4e8c20->entries[index];
                if (*(long *)(object + 0x13c) == entry)
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
    real z;
    value.x = (real)a * 8.0f * g_45dd38;
    value.y = (real)b * 8.0f * g_45dd38;
    z = (real)*reference * 8.0f;
    value.z = z * g_45dd44;
    *out = value;
    return out;
}


struct s_2cb30_state
{
	byte unknown00[0x2a78];
	long active;
};

struct s_2cb30_entry
{
	dword values[9];
};

struct s_2cb30_globals
{
	long current;
	long previous;
	s_2cb30_state *state;
	union
	{
		byte unknown0c[0x9b0 - 0xc];
		struct
		{
			byte weights[850];
			byte material_indices[850];
			long materials[192];
		};
	};
	long count;
	s_2cb30_entry entries[32];
	long entry_count;
};
s_2cb30_globals g_4c0b78;

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


s_44940_entry g_4ba138[850];

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

struct s_geometry_visibility_list
{
	dword *indices;
	long count;
};

PRIVATE __forceinline byte *visible_entry_geometry(s_44940_entry *entry)
{
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		return *(byte **)(sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
	}
	byte *structure = (byte *)g_4e0348;
	byte *section;
	if (!((entry->unknown00 >> 12) & 1))
		section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
	else
	{
		byte *instances = *(byte **)(structure + 0x144);
		short index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
		section = *(byte **)(structure + 0x13c) + index * 0xc8;
	}
	return *(byte **)(section + 0x50);
}

struct s_render_part_context;
bool function_45ce0(short index, byte *out, short part, short transform_index);
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);

// @retail 0x44c90
void __stdcall function_44c90(long group, dword selection_mask, long level,
    dword and_mask, dword or_mask, dword fallback_and_mask, dword fallback_or_mask, bool force)
{
    long end = (word)(group ? g_4c0b78.current : g_4c0b78.previous);
    long begin = group ? (word)g_4c0b78.previous : 0;
    for (long index = begin; index < end; ++index)
    {
        s_44940_entry *entry = &g_4ba138[(short)index];
        if (!((entry->unknown00 & selection_mask) & 0x1fffff) ||
            ((entry->flags >> 4) & 0x1f) != level || !(entry->unknown00 & 1) || !entry->unknown10[0xa]) continue;
        byte weight = g_4c0b78.weights[index];
        byte material_index = g_4c0b78.material_indices[(short)index];
        long material = material_index == 0xff ? NONE : g_4c0b78.materials[material_index];
        byte *geometry = visible_entry_geometry(entry);
        if (!geometry) continue;
        if ((*(byte **)(entry->unknown10))[0x76] == 0xff)
        {
            and_mask = fallback_and_mask;
            or_mask = fallback_or_mask;
        }
        for (long part = 0; part < *(long *)geometry; ++part)
        {
            byte context[0x2c];
            if (function_45ce0((short)index, context, (short)part, (short)(entry->flags & 0xf)))
            {
                long selected = force || (entry->unknown00 & 0x8000) ? material : NONE;
                function_41040((short)index, part, group, weight, (s_render_part_context *)context,
                    selected, level != 31, and_mask, or_mask);
            }
        }
    }
}

// @retail 0x458d0
void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second)
{
	short const *index_reference = &index;
	(void)&ranges; (void)&first; (void)&second;
	s_44940_entry *entry = &g_4ba138[*index_reference];
	byte *geometry = visible_entry_geometry(entry);
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
			word *mapping = *(word **)(geometry + 0x34);
			long offset = *(long *)(geometry + 8);
			word end = mapping[(short)first->indices[i + 1] + offset];
			for (long j = mapping[(short)first->indices[i] + offset]; j <= end; ++j)
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
	(void)&ranges; (void)&first; (void)&second;
	s_44940_entry *entry = &g_4ba138[*index_reference];
	byte *geometry = visible_entry_geometry(entry);
	dword *mask = *(dword **)(entry->unknown10 + 4);
	if (first->count > 0 || second->count > 0)
		entry->unknown00 |= 1;
	for (long i = 0; i < first->count; ++i)
	{
		if (!ranges)
			mark_visible_part(geometry, (word)first->indices[i], mask);
		else
		{
			word *mapping = *(word **)(geometry + 0x34);
			long offset = *(long *)geometry + 2 * *(long *)(geometry + 8);
			long end = mapping[(short)first->indices[i + 1] + offset];
			for (long j = mapping[(short)first->indices[i] + offset]; j <= end; ++j)
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

struct s_3d4f0_entry
{
	long tag;
	long object_index;
	long flags;
	transform4x3f transforms[64];
};
s_3d4f0_entry g_4c1bd8[4];

#include "unknown_03bcb0.h"

// @retail 0x3d600
void function_3d600(void)
{
	for (long cache = 0; cache < g_4c1bd4; ++cache)
	{
		byte *definition = g_4e3b44[g_4c1bd8[cache].tag & 0xffff].bytes;
		for (long section = 0; section < *(long *)(definition + 0x24); ++section)
		{
			byte *sections = *(byte **)(definition + 0x28);
			function_12de70((s_geometry_block_info *)(sections + section * 0x5c + 0x38), 10);
		}
		for (long material = 0; material < *(long *)(definition + 0x60); ++material)
		{
			long tag = *(long *)(*(byte **)(definition + 0x64) + material * 0x20 + 0xc);
			if (tag == NONE) continue;
			byte *shader = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
			for (long texture = 0; texture < *(long *)(shader + 4); ++texture)
			{
				long bitmap_tag = *(long *)(*(byte **)(shader + 8) + texture * 0xc);
				if (bitmap_tag == NONE) continue;
				byte *bitmap = g_4e3b44[bitmap_tag & 0xffff].bytes;
				for (long index = 0; index < *(long *)(bitmap + 0x44); ++index)
				{
					s_bitmap_predict_view *image = (s_bitmap_predict_view *)(*(byte **)(bitmap + 0x48) + index * 0x74);
					if (image) bitmap_predict_inline(image, 14);
				}
			}
		}
	}
}

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
	byte const *camera, byte const *plane, real distance, real bias, real offset, long identifier);
void function_133390(s_bit_vector_pool const *data);
void __stdcall function_133520(s_bit_vector_pool *data);
void function_133720(s_bit_vector_pool *data, bool enabled);
void function_44720(void);
void function_2cb30(void);
long g_4b9ee4;

// @retail 0x2cbc0
void __stdcall function_2cbc0(bool enabled)
{
	s_bit_vector_pool *current = (s_bit_vector_pool *)g_4c0b78.state;
	function_2cb30();
	function_133390(current);
	function_133520(current);
	function_133720(current, enabled);
}

// @retail 0x3f2c0
void function_3f2c0(s_planar_camera_source const *source, byte const *view, long object_index, long identifier)
{
	byte camera[0x1bc];
	real offset = 0.0f;
	function_441b0(source, (s_planar_camera *)camera, view);
	byte *effect = (byte *)function_01fd20(0);
	bool enabled = false;
	if (effect && effect[0x3c])
	{
		offset = *(real *)(effect + 0x40) - *(real *)(camera + 0x70);
		enabled = true;
	}
	function_3d600();
	s_bit_vector_pool *data = function_1320f0(0, 1, enabled, 0, camera,
		camera + 0x60, 1024.0f, 0.0f, offset, g_4b9ee4);
	if (object_index != NONE)
	{
		struct s_entry_list_view
		{
			long maximum;
			word count;
			short *shorts_b;
			long *longs_a;
			long *longs_c;
			short *shorts_d;
		};
		s_entry_list_view *list = (s_entry_list_view *)data->lists[2];
		bool found = false;
		for (long i = 0; i < (short)list->count && !found; ++i)
			found = list->longs_a[(short)i] == object_index;
		if (!found && list->count < list->maximum - 1)
		{
			list->longs_a[list->count] = object_index;
			list->shorts_b[list->count] = 0;
			list->longs_c[list->count] = 0;
			list->shorts_d[list->count] = NONE;
			++list->count;
		}
	}
	s_bit_vector_pool *current = (s_bit_vector_pool *)g_4c0b78.state;
	function_2cb30();
	function_133390(current);
	function_133520(current);
	function_133720(current, true);
	function_44720();
}

extern vector3f g_4b9dac, g_4b9db8;
extern byte g_4ba025;
extern s_camera g_4b9e14;
extern point3f g_4b9da0;
real g_4b9de0, g_4b9de4;
void __stdcall function_3f830(byte const *tree, point3f const *center);
long function_155760(long index);
void function_3f3f0(void);

// @retail 0x2b990
void function_2b990(long player_index)
{
	byte const *tree = *(byte **)((byte *)g_4e0348 + 0x238);
	long object_index = NONE;
	if (player_index != NONE)
	{
		long player = g_4e8c20->entries[player_index];
		if (player != NONE && !function_155760(player_index))
			object_index = *(long *)((byte *)g_4e8c24->data + (player & 0xffff) * 0x21c + 0x2c);
	}
	s_planar_camera_source source;
	source.origin = g_4b9da0;
	source.normal = g_4b9dac;
	source.horizontal = g_4b9db8;
	source.offset = g_4b9de0;
	source.depth = g_4b9de4;
	s_camera camera = g_4b9e14;
	function_3f2c0(&source, (byte const *)&camera, object_index, g_4b9ee4);
	function_3f3f0();
	if (g_4ba025)
	{
		point3f center = g_4b9da0;
		function_3f830(tree, &center);
	}
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
	s_44940_entry *entry = &g_4ba138[*index_reference];
	bool result = true;
	if (entry->unknown00 & 0x40)
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
			byte *section;
			if (!((byte)(entry->unknown00 >> 12) & 1))
				section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
			else
			{
				byte *instances = *(byte **)(structure + 0x144);
				short definition_index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
				section = *(byte **)(structure + 0x13c) + definition_index * 0xc8;
			}
			data = *(byte **)(section + 0x50);
		}
		byte *part = *(byte **)(data + 4) + part_index * 0x48;
		long first = *(short *)(part + 0xa);
		long count = *(short *)(part + 0xc);
		for (long i = first; i < first + count && !result; ++i)
			result = (mask[i >> 5] & (1 << (i & 31))) != 0;
	}
	return result;
}

// @retail 0x4c2b0
bool function_4c2b0(long tag, byte const *wanted, signed char *current, long level,
    bool request, signed char *sections, bool *fallback)
{
    struct { word available; bool result; } section_status;
    (void)&wanted; (void)&current; (void)&level;
    (void)&request; (void)&sections; (void)&fallback;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    *fallback = false;
    section_status.available = 0;
    section_status.result = true;
    for (long i = 0; i < *(long *)(definition + 0x1c); ++i)
    {
        long selection = wanted[i];
        byte *group = *(byte **)(definition + 0x20) + i * 16;
        if (selection != NONE && selection < *(long *)(group + 8))
        {
            byte *variant = *(byte **)(group + 0xc) + selection * 16;
            short section = ((short *)(variant + 4))[level];
            s_geometry_block_info *block = (s_geometry_block_info *)(*(byte **)(definition + 0x28) + section * 0x5c + 0x38);
            if (request)
            {
                if (function_12de70(block, 3)) section_status.available |= 1 << i;
            }
            else
            {
                if (function_12de70(block, 0)) section_status.available |= 1 << i;
            }
        }
    }
    for (long i = 0; i < *(long *)(definition + 0x1c); ++i)
    {
        byte *group = *(byte **)(definition + 0x20) + i * 16;
        if (section_status.available & (1 << i))
        {
            byte *variant = *(byte **)(group + 0xc) + (signed char)wanted[i] * 16;
            sections[i] = (signed char)((short *)(variant + 4))[level];
            current[i] = wanted[i];
        }
        else if (wanted[i] != 0xff)
        {
            if (current[i] != -1)
            {
                byte *variant = *(byte **)(group + 0xc) + current[i] * 16;
                short section = ((short *)(variant + 4))[level];
                if (function_12de70((s_geometry_block_info *)(*(byte **)(definition + 0x28) + section * 0x5c + 0x38), 0))
                {
                    sections[i] = (signed char)((short *)(variant + 4))[level];
                    *fallback = true;
                }
                else section_status.result = false;
            }
            else
            {
                *fallback = true;
                sections[i] = -1;
            }
        }
        else sections[i] = -1;
    }
    return section_status.result;
}

struct s_light_shape_ab;
bool function_c3140(long index);
bool function_c17f0(long light_index, s_light_shape_ab *shape, bool respect_engine);
extern short g_485602;

// @retail 0x31590
bool function_31590(long index, s_light_shape_ab *shape)
{
    volatile bool enabled = false;
    bool result = false;
    if (index != NONE && function_c3140(index))
    {
        s_render_entry_110 *entry = &((s_render_entry_110 *)g_4e030c->data)[index & 0xffff];
        byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
        bool visible;
        if (function_31520(index))
        {
            result = function_c17f0(index, shape, true);
            visible = true;
            if ((*(dword *)definition & 0x400) && g_485602 != 2)
                visible = false;
        }
        else
            visible = enabled;
        result &= visible;
        result &= true;
        result &= *(short *)((byte *)entry + 0x48) != NONE ||
            ((*(dword *)definition & 0x20) && (*(dword *)definition & 0x40000));
    }
    return result;
}

#include <math.h>
struct s_camera;
struct s_tag_data;
real function_50650(real cosine);
real function_13bb90(s_tag_data const *function, real input, real range);
long function_30830(point3f const *a, s_camera const *camera, vector3f const *d,
    point2f const *scale, bool perspective, bool negate, point2f *out);

// @retail 0x4ff50
void function_4ff50(byte const *definition, byte const *state, bool facing, real time,
    real cosine, real *strength, real *horizontal, real *vertical, real *out)
{
    if (*(long const *)(definition + 0x38) > 0)
    {
        byte const *curve = *(byte **)(definition + 0x3c);
        if (facing)
        {
            real angle = function_50650(cosine) * 0.6366197466850281f;
            if (angle < 0.0f) angle = 0.0f;
            else if (angle > 1.0f) angle = 1.0f;
            real lower = curve ? *(real const *)(curve + 0x14) * 0.6366197466850281f : 0.0f;
            if (0.0f > lower) lower = 0.0f;
            else if (lower > 0.9998999834060669f) lower = 0.9998999834060669f;
            real value = (angle - lower) / (1.0f - lower);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
            *strength = value;
            if (value > 0.0f)
            {
                if (curve && *(real const *)(curve + 0x18) > 0.0f)
                    *strength = (real)pow(value, *(real const *)(curve + 0x18));
                else
                    *strength = 1.0f;
            }
            if (*strength > 0.0f)
            {
                point2f projected;
                if (function_30830((point3f const *)(state + 8), NULL, (vector3f const *)(state + 0x14),
                    NULL, true, true, &projected) && (projected.x != 0.0f || projected.y != 0.0f))
                {
                    double direction = atan2(projected.y, projected.x);
                    *horizontal = (real)cos(direction);
                    *vertical = (real)sin(direction);
                }
            }
        }
        if (curve)
        {
            real base = *(real const *)(curve + 0x10);
            if (*strength > 0.0f)
            {
                real value = function_13bb90((s_tag_data const *)curve, time, 0.0f);
                out[0] = base - (base - value) * *strength;
                value = function_13bb90((s_tag_data const *)(curve + 8), time, 0.0f);
                out[1] = base - (base - value) * *strength;
            }
            else
            {
                out[0] = base;
                out[1] = base;
            }
        }
    }
}


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

// @retail 0x3e380
long function_3e380(long model_index, byte lod, transform4x3f const *nodes,
    byte const *permutations, long *node_count, long extra_bytes)
{
    long count = function_137650(model_index, lod, permutations);
    *node_count = count;
    long bytes = 0;
    if (count > 0)
        bytes = count * 48 + 0x44;
    bytes += extra_bytes;
    long remainder = bytes % 4;
    if (remainder)
        bytes = bytes - remainder + 4;
    long result = NONE;
    long size = g_487b18.size + bytes;
    if (size <= 0x27000)
    {
        result = g_487b18.size & 0x0fffffff;
        g_487b18.size = size;
        if (result != NONE)
        {
            s_137801 *out = NULL;
            if (result >= 0)
                out = (s_137801 *)(g_487b18.data + (result & 0x0fffffff));
            function_137800(NONE, model_index, nodes, permutations, lod, false, out);
        }
    }
    return result;
}

#include "unknown_030290.h"

struct s_bit_vector_pool;
dword *function_1332b0(long bit_count, s_bit_vector_pool *data);
long function_016ae0(real value);
void function_30290(point3f const *point, s_view const *view, s_camera const *camera, real radius, box2f *bounds);
extern point3f g_4b9da0;
extern s_camera g_4b9e14;
extern short g_4b9dd0, g_4b9dd2, g_4b9dd4, g_4b9dd6;
byte g_509410, g_509411;

struct s_42760_entry
{
    real depth;
    point3f position;
    byte unknown10[0x10];
    long type;
};
extern s_42760_entry g_4c152c[32];
extern long g_4c19ac;

// @retail 0x2cbf0
short function_2cbf0(long geometry, byte category, long instance, dword flags,
    long tag, dword key, long material, byte *object, short mask, byte mode,
    void *context, long object_index, real intensity, long group, long group_type,
    bool outside, bool whole, bool first_component, real first_opacity,
    real second_opacity, real depth, byte level, point3f const *position, real radius)
{
    volatile long index = g_4c0b78.current++;
    if (index >= 850 || geometry == NONE)
    {
        --g_4c0b78.current;
        return NONE;
    }
    s_44940_entry *entry = &g_4ba138[(short)index];
    entry->tag = tag;
    entry->unknown08 = key;
    entry->unknown00 = (entry->unknown00 & 0xe0000000) | (flags & 0x1fffff) | ((dword)category << 21);
    entry->flags = (entry->flags & 0xe0000000) |
        (((((instance & 0x7ff) << 9) | (geometry & 0x1ff)) << 5) | (group & 0x1f)) << 4 |
        (material & 0xf);
    *(byte **)entry->unknown10 = object;
    *(void **)(entry->unknown10 + 12) = context;
    entry->unknown10[10] = mode;
    entry->unknown10[11] = (entry->unknown10[11] & 0xf0) | (level & 15);
    long group_index = (entry->flags >> 4) & 31;
    if (group_index != 31)
    {
        if ((dword)g_4c19ac <= (dword)(group_index + 1))
            g_4c19ac = group_index + 1;
        g_4c152c[group_index].type = group_type;
    }
    group_index = (entry->flags >> 4) & 31;
    if (group_index != 31 && object_index != NONE)
    {
        s_42760_entry *group_entry = &g_4c152c[group_index];
        group_entry->depth = depth;
        group_entry->position = *position;
        group_entry->type = group_type;
        box2f *bounds = (box2f *)group_entry->unknown10;
        if (outside || g_509410)
        {
            bounds->x0 = bounds->y0 = -3.4028234663852886e+38f;
            bounds->x1 = bounds->y1 = 3.4028234663852886e+38f;
        }
        else if (whole || g_509411)
        {
            bounds->x0 = bounds->y0 = 3.4028234663852886e+38f;
            bounds->x1 = bounds->y1 = -3.4028234663852886e+38f;
        }
        else
        {
            s_view view;
            view.position = g_4b9da0;
            view.bounds.top = g_4b9dd0;
            view.bounds.left = g_4b9dd2;
            view.bounds.bottom = g_4b9dd4;
            view.bounds.right = g_4b9dd6;
            s_camera camera = g_4b9e14;
            function_30290(position, &view, &camera, radius, bounds);
        }
    }
    *(short *)(entry->unknown10 + 8) = mask;
    g_4c0b78.unknown0c[850 + index] = 0xff;
    if (object_index != NONE && g_4c0b78.count < 192)
    {
        g_4c0b78.unknown0c[850 + index] = (byte)g_4c0b78.count;
        ((long *)(g_4c0b78.unknown0c + 1700))[g_4c0b78.count] = object_index;
        ++g_4c0b78.count;
    }
    real scaled = intensity * 0.25f;
    long value;
    if (function_016ae0(scaled) < 0) value = 0;
    else if (function_016ae0(scaled) > 255) value = 255;
    else value = function_016ae0(scaled);
    g_4c0b78.unknown0c[index] = (byte)value;
    if (object && object_index != NONE)
    {
        real opacity = (first_component ? first_opacity : second_opacity) * 256.0f;
        long amount = (long)opacity;
        if (amount < 0) amount = 0;
        else if (amount > 255) amount = 255;
        object[first_component ? 0x76 : 0x77] = (byte)amount;
    }
    byte *section = (byte *)function_44940((short)index, (bool)((flags >> 12) & 1));
    entry->flags = (entry->flags & 0x1fffffff) | ((dword)*(word *)(section + 0x14) << 29);
    long object_tag = object ? *(long *)object : NONE;
    long kind = function_2d000(object_tag, entry->tag, (entry->flags >> 18) & 0x7ff);
    entry->unknown00 = (entry->unknown00 & 0x1fffffff) | ((dword)kind << 29);
    bool local_99c7da = (bool)((entry->unknown00 >> 12) & 1);
    if (!function_449e0((short)index, entry->tag == NONE, local_99c7da))
    {
        --g_4c0b78.current;
        return NONE;
    }
    s_2cb30_state *state = g_4c0b78.state;
    if (!state->active)
        ++g_4c0b78.previous;
    if (entry->unknown00 & 0x40)
    {
        section = (byte *)function_44940((short)index, local_99c7da);
        *(void **)(entry->unknown10 + 4) = NULL;
        word bits = *(word *)(section + 0x24);
        if (bits > 0)
            *(void **)(entry->unknown10 + 4) = function_1332b0(bits, (s_bit_vector_pool *)state);
        if (!*(void **)(entry->unknown10 + 4))
            entry->unknown00 = (entry->unknown00 & ~0x40) | 1;
    }
    return (short)index;
}


struct s_frustum_set_view;
bool function_165010(s_frustum_set_view const *set, long section_index, point3f const *center, real radius, bool *contained);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
struct s_visibility_plane_query
{
    long count;
    plane3f *planes;
};
struct s_visibility_query_list
{
    dword *indices;
    long count;
    long capacity;
};
void __cdecl function_2dd930(void *data, s_visibility_plane_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);
template<long N> struct s_visibility_query_storage
{
    s_visibility_query_list list;
    dword indices[N];
};
struct s_visibility_query_workspace
{
    s_visibility_query_storage<1024> first, second;
    s_visibility_query_storage<2048> third;
    union
    {
        s_visibility_query_storage<2048> fourth;
        transform4x3f transforms[255];
    };
};

PRIVATE __forceinline void release_visibility_list(s_visibility_query_list *list)
{
    if (list->capacity >= 0)
        g_480118->allocate((long)list->indices, (list->capacity & 0x7fffffff) * 4, 0x12);
}

// @retail 0x44e90
void __stdcall function_44e90(short entry_index, short section_index)
{
    (void)&entry_index; (void)&section_index;
    s_visibility_query_workspace workspace;
    s_44940_entry *entry = &g_4ba138[entry_index];
    byte *geometry = visible_entry_geometry(entry);
    byte *views = *(byte **)g_4c0b78.state->unknown00;
    byte *view_ranges = views + section_index * 0x1a + 0xa6e;
    if (!(*(long *)(geometry + 0x30) > 0)) {
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        long object_index = **(long **)entry->unknown10;
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        long node_count = *(short *)(object + 0x114) / sizeof(transform4x3f);
        transform4x3f const *nodes = (transform4x3f *)(object + *(short *)(object + 0x116));
        for (long i = 0; i < node_count; ++i)
            function_142a60(&nodes[i], (transform4x3f *)(*(byte **)(tag + 0x4c) + i * 0x60 + 0x28), &workspace.transforms[i]);
        dword *visible = *(dword **)(entry->unknown10 + 4);
        for (long part = 0; part < *(long *)(geometry + 8); ++part)
        {
            dword bit = 1 << (part & 31);
            if (!(visible[part >> 5] & bit))
            {
                word bound_index = *(word *)(*(byte **)(geometry + 0xc) + part * 8 + 4);
                byte *bound = *(byte **)(geometry + 0x14) + bound_index * 20;
                transform4x3f const *transform = &workspace.transforms[bound[0x10]];
                point3f local = *(point3f *)bound;
                if (transform->scale != 1.0f)
                {
                    local.x *= transform->scale;
                    local.y *= transform->scale;
                    local.z *= transform->scale;
                }
                point3f center;
                center.x = transform->up.i * local.z + transform->left.i * local.y + transform->forward.i * local.x + transform->position.x;
                center.y = transform->up.j * local.z + transform->left.j * local.y + transform->forward.j * local.x + transform->position.y;
                center.z = transform->up.k * local.z + transform->left.k * local.y + transform->forward.k * local.x + transform->position.z;
                bool contained = false;
                if (function_165010((s_frustum_set_view *)views, section_index, &center, *(real *)(bound + 0xc), &contained))
                {
                    entry->unknown00 |= 1;
                    visible[part >> 5] |= bit;
                }
            }
        }
    } else {
        bool parts = !g_4c0b78.state->active && (entry->unknown00 & 0x1000) &&
            *(long *)(geometry + 0x30) > 2 * *(long *)(geometry + 8);
        byte *query_data = *(byte **)(geometry + 0x2c);
        if (parts)
            query_data += (*(long *)(query_data + 0x20) + 15) & ~15;
        workspace.first.list.indices = workspace.first.indices;
        workspace.first.list.count = 0;
        workspace.first.list.capacity = (long)0x80000400;
        workspace.second.list.indices = workspace.second.indices;
        workspace.second.list.count = 0;
        workspace.second.list.capacity = (long)0x80000400;
        workspace.third.list.indices = workspace.third.indices;
        workspace.third.list.count = 0;
        workspace.third.list.capacity = (long)0x80000800;
        workspace.fourth.list.indices = workspace.fourth.indices;
        workspace.fourth.list.count = 0;
        workspace.fourth.list.capacity = (long)0x80000800;
        __declspec(align(16)) plane3f planes[6];
        s_visibility_plane_query query = { 6, planes };
        for (long view = 0; view < *(short *)views; ++view)
        {
            for (long i = 0; i < ((short *)view_ranges)[view + 1]; ++i)
            {
                long frustum_index = *(short *)(view_ranges + 0xe + view * 2) + i;
                plane3f const *source = (plane3f *)(views + 0x1974 + frustum_index * 0x108 + 0x78);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                for (long j = 0; j < 6; ++j)
                {
                    plane3f p = source[j];
                    if (transform)
                    {
                        if (transform->scale != 0.0f)
                        {
                            p.d -= transform->position.y * p.j + transform->position.z * p.k + transform->position.x * p.i;
                            if (transform->scale != 1.0f)
                                p.d /= transform->scale;
                        }
                        vector3f n = p.n;
                        if (transform->scale != 1.0f)
                        {
                            real inverse = 1.0f / transform->scale;
                            n.i *= inverse; n.j *= inverse; n.k *= inverse;
                        }
                        p.i = transform->forward.k * n.k + transform->forward.j * n.j + transform->forward.i * n.i;
                        p.j = transform->left.k * n.k + transform->left.j * n.j + transform->left.i * n.i;
                        p.k = transform->up.k * n.k + transform->up.j * n.j + transform->up.i * n.i;
                    }
                    p.d = -p.d;
                    planes[j] = p;
                }
                function_2dd930(query_data, &query, &workspace.first.list, &workspace.second.list);
            }
            if (parts)
                function_45a80(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
            else
                function_458d0(entry_index, true, (s_geometry_visibility_list *)&workspace.second.list,
                    (s_geometry_visibility_list *)&workspace.first.list);
        }
        release_visibility_list(&workspace.fourth.list);
        release_visibility_list(&workspace.third.list);
        release_visibility_list(&workspace.first.list);
        release_visibility_list(&workspace.second.list);
    }
}


transform4x3f *function_b8c00(long object_index, long *node_count);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
struct s_visibility_sphere_query
{
    point3f center;
    real radius;
};
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);

// @retail 0x45560
void function_45560(short entry_index, point3f const *center, real radius)
{
    (void)&center; (void)&radius;
    s_44940_entry *entry = &g_4ba138[entry_index];
    byte *geometry = visible_entry_geometry(entry);
    point3f query_center = *center;
    if (geometry)
    {
        if (*(long *)(geometry + 0x30) > 0)
        {
            s_visibility_query_storage<512> first, second;
            first.list.indices = first.indices;
            first.list.count = 0;
            first.list.capacity = (long)0x80000200;
            second.list.indices = second.indices;
            second.list.count = 0;
            second.list.capacity = (long)0x80000200;
            __declspec(align(16)) s_visibility_sphere_query query;
            query.center = *center;
            query.radius = radius;
            function_2ddbd0(*(void **)(geometry + 0x2c), &query, &first.list, &second.list);
            function_458d0(entry_index, true, (s_geometry_visibility_list *)&second.list,
                (s_geometry_visibility_list *)&first.list);
            release_visibility_list(&first.list);
            release_visibility_list(&second.list);
        }
        else
        {
            dword *visible = *(dword **)(entry->unknown10 + 4);
            long cached_node = NONE;
            transform4x3f transform;
            for (long i = 0; i < *(long *)(geometry + 8); ++i)
            {
                dword bit = 1 << (i & 31);
                if (!(visible[i >> 5] & bit))
                {
                    word bound_index = *(word *)(*(byte **)(geometry + 0xc) + i * 8 + 4);
                    if (bound_index == 0xffff)
                    {
                        entry->unknown00 |= 1;
                        visible[i >> 5] |= bit;
                    }
                    else
                    {
                        byte *bound = *(byte **)(geometry + 0x14) + bound_index * 20;
                        point3f position = *(point3f *)bound;
                        if (entry->tag != NONE)
                        {
                            long object_index = **(long **)entry->unknown10;
                            if (object_index != NONE)
                            {
                                long node = bound[0x10];
                                if (node != NONE && node != cached_node)
                                {
                                    long count = 0;
                                    transform4x3f *nodes = function_b8c00(object_index, &count);
                                    if (count)
                                    {
                                        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                                        function_142a60(&nodes[node], (transform4x3f *)(*(byte **)(tag + 0x4c) + node * 0x60 + 0x28), &transform);
                                        function_142700(&transform, center, &query_center);
                                        cached_node = node;
                                    }
                                }
                            }
                        }
                        real dx = query_center.x - position.x;
                        real dy = query_center.y - position.y;
                        real dz = query_center.z - position.z;
                        real combined_radius = *(real *)(bound + 0xc) + radius;
                        if (combined_radius * combined_radius >= dz * dz + dy * dy + dx * dx)
                        {
                            entry->unknown00 |= 1;
                            visible[i >> 5] |= bit;
                        }
                    }
                }
            }
        }
    }
}

// @retail 0x44720
void function_44720(void)
{
    s_2cb30_state *state = g_4c0b78.state;
    volatile word end = (word)(state->active ? g_4c0b78.current : g_4c0b78.previous);
    word index = (word)(state->active ? g_4c0b78.previous : 0);
    while (index < end)
    {
        s_44940_entry *entry = &g_4ba138[(short)index];
        if (!(entry->unknown00 & 1) && (entry->unknown00 & 0x40))
        {
            if (*(long *)((byte *)state + 0x2a7c) == 1)
            {
                point3f center = *(point3f *)((byte *)state + 0x2a68);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                if (transform)
                {
                    if (transform->scale == 0.0f)
                        center.x = center.y = center.z = 0.0f;
                    else
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
                }
                function_45560((short)index, &center, *(real *)((byte *)state + 0x2a74));
            }
            else if (entry->unknown00 & 8)
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

signed char *function_3d430(long object_index);
void function_4c640(long object_index, signed char value);

extern s_record_pool *g_509434;
extern long g_4ba050;
long __stdcall function_3ddd0(long object_index);
extern long g_4c1bd0, g_4ba01c;
extern bool g_4ba019;
extern byte *g_485a80;
bool object_or_parent_hidden(long object_index);
bool __stdcall function_bab40(long object_index, long name, real *value);

// @retail 0x4c450
long function_4c450(long object_index, real distance, real const *definition, real *fraction)
{
    (void)&object_index;
    real defaults[6] = { 10.0f, 8.0f, 6.0f, 4.0f, 2.0f, 0.0f };
    real thresholds[6];
    real upper;
    *fraction = 0.9999f;
    if (definition && definition[3] > 0.0f && definition[3] >= definition[4]
        && definition[4] >= definition[5] && definition[5] >= definition[6]
        && definition[6] >= definition[7] && definition[8] == 0.0f)
    {
        memcpy(thresholds, definition + 3, sizeof(thresholds));
        upper = definition[0];
    }
    else
    {
        memcpy(thresholds, defaults, sizeof(thresholds));
        upper = 0.0f;
    }
    long level = 0;
    while (level < 6)
    {
        real lower = thresholds[level];
        if (distance > lower || lower == 0.0f)
        {
            if (level != 0)
                upper = thresholds[level - 1];
            if (level != 0 || upper > lower)
            {
                real value = (upper - distance) / (upper - lower);
                if (value < 0.0f) value = 0.0f;
                else if (value > 0.9999f) value = 0.9999f;
                *fraction = value;
            }
            else
                *fraction = 0.9999f;
            break;
        }
        ++level;
    }
    signed char previous = -1;
    if (g_4ba050 >= 0 && g_4ba050 < 4)
    {
        byte *slot = NULL;
        long cache = function_3ddd0(object_index);
        if (cache != NONE)
            slot = g_509434->data + (cache & 0xffff) * 0x100 + 0xbc;
        previous = (signed char)slot[g_4ba050];
    }
    if (previous != -1)
    {
        long difference = previous - level;
        long absolute = difference < 0 ? -difference : difference;
        if (absolute == 1)
        {
            if (difference == absolute && *fraction < 0.8f)
                ++level;
            else if (difference == -1 && *fraction > 0.2f)
                --level;
        }
    }
    return level;
}

// @retail 0x4bcc0
short function_4bcc0(byte *output, long object_index, real distance, long override,
    bool *cached, bool force, signed char *level)
{
    (void)&object_index; (void)&distance; (void)&override;
    (void)&cached; (void)&force; (void)&level;
    volatile short result = 0;
    *(dword *)(output + 0x16c) = 0;
    bool volatile eligible = false;
    bool current = false;
    if (!object_or_parent_hidden(object_index))
    {
        eligible = true;
        current = object_index == g_4c1bd0;
    }
    if (cached) *cached = current;
    if (!eligible) return result;
    byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    if (force) *cached = false;
    long model_index = *(long *)(definition + 0x38);
    if (model_index == NONE) return 0;
    byte *model = g_4e3b44[model_index & 0xffff].bytes;
    result = function_3d3e0(object_index, *cached);
    if (result > 0)
    {
        long geometry = *(long *)(model + 4);
        if (!override && geometry != NONE)
        {
            byte *geometry_definition = g_4e3b44[geometry & 0xffff].bytes;
            if (*(long *)(geometry_definition + 0x74) > 0 &&
                function_12de70((s_geometry_block_info *)(*(byte **)(geometry_definition + 0x78) + 0x34), 3))
                *(dword *)(output + 0x16c) |= 0x40;
        }
        *level = *cached ? 4 : -1;
        if (!*cached)
        {
            byte *object_definition = g_4e3b44[*(long *)object & 0xffff].bytes;
            byte *level_definition = g_4e3b44[*(long *)(object_definition + 0x38) & 0xffff].bytes;
            if (g_4ba019) distance *= 0.3f;
            if ((bool)((*(dword *)(object + 4) >> 19) & 1))
                *level = 5;
            else
            {
                if ((bool)((object[0x10a] >> 2) & 1)) distance *= 1.7f;
                real fraction;
                *level = (signed char)function_4c450(object_index, distance, (real const *)(level_definition + 0x28), &fraction);
                long maximum = (*(dword *)(output + 0x16c) & 0x40) ? 4 : g_4ba01c;
                if (*level < 0) *level = 0;
                else if (*level > maximum) *level = (signed char)maximum;
            }
        }
        if (*level == -1)
            result = 0;
        else
        {
            bool transparent = false;
            if (*(dword *)(model + 0x9c) & 1)
            {
                *(dword *)(output + 0x16c) |= 6;
                *(long *)(output + 0x174) = *(long *)(g_485a80 + 0xd0);
                *(real *)(output + 0x184) = 0.99f;
                transparent = true;
            }
            else
            {
                byte *current_object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
                if (((1 << current_object[0xaa]) & 3) && *(real *)(current_object + 0x2b0) > 0.0f)
                {
                    *(dword *)(output + 0x16c) |= 2;
                    *(long *)(output + 0x174) = *(long *)(g_485a80 + 0xd0);
                    *(real *)(output + 0x184) = *(real *)(current_object + 0x2b0);
                    transparent = true;
                }
            }
            if (transparent)
            {
                bool flag2 = (bool)((*(dword *)(model + 0x9c) >> 2) & 1);
                bool flag1 = (bool)((*(dword *)(model + 0x9c) >> 1) & 1);
                if (*cached) { flag2 = true; flag1 = false; }
                if (flag2) *(dword *)(output + 0x16c) |= 8;
                else *(dword *)(output + 0x16c) &= ~8;
                if (flag1) *(dword *)(output + 0x16c) |= 0x10;
                else *(dword *)(output + 0x16c) &= ~0x10;
            }
            if (!override)
            {
                if (*(real *)(object + 0xf4) > 0.0f && *(long *)(model + 0x60) > 0)
                {
                    byte *materials = *(byte **)(model + 0x64);
                    long tag = *(long *)(materials + (*cached ? 0x80 : 0x88));
                    if (tag != NONE)
                    {
                        *(dword *)(output + 0x16c) |= 1;
                        *(long *)(output + 0x170) = tag;
                        *(real *)(output + 0x180) = *(real *)(object + 0xf0);
                    }
                }
                if (*(real *)(object + 0xf0) > 1.0f && *(long *)(model + 0x60) > 0)
                {
                    byte *materials = *(byte **)(model + 0x64);
                    long tag = *(long *)(materials + (*cached ? 0xec : 0xf4));
                    if (tag != NONE)
                    {
                        *(dword *)(output + 0x16c) |= 0x20;
                        *(long *)(output + 0x178) = tag;
                        *(real *)(output + 0x188) = 1.0f - *(real *)(object + 0xf0);
                    }
                }
                if (*(long *)(model + 0xf4) != NONE)
                {
                    real value = 0.0f;
                    if (function_bab40(object_index, *(long *)(model + 0xf8), &value) && value > 0.0f)
                    {
                        *(dword *)(output + 0x16c) |= 0x80;
                        *(long *)(output + 0x17c) = *(long *)(model + 0xf4);
                        *(real *)(output + 0x18c) = value;
                    }
                }
            }
        }
    }
    function_4c0d0(object_index, output, result);
    return result;
}

// @retail 0x4c190
bool function_4c190(long *count, long object_index, bool defaults, long tag,
    short *level, signed char fallback_level, signed char *sections, bool *fallback)
{
    (void)&object_index; (void)&defaults; (void)&tag;
    (void)&level; (void)&fallback_level; (void)&sections; (void)&fallback;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    byte zero[16];
    byte const *wanted;
    signed char *current;
    if (!defaults)
    {
        byte *object = (byte *)((s_render_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        *count = *(short *)(object + 0x118);
        wanted = object + *(short *)(object + 0x11a);
        *count = *count / 10;
        current = function_3d430(object_index);
    }
    else
    {
        *(long *)(zero + 0) = 0;
        *(long *)(zero + 4) = 0;
        *(long *)(zero + 8) = 0;
        *(long *)(zero + 12) = 0;
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

extern byte g_4670bc, g_485b48[];
extern long g_4858b8;
extern D3DPIXELSHADERDEF g_484f68;
extern D3DResource *g_509444;
extern byte g_51f0f0[0x2d8];
struct s_render_reset_state;
struct s_shader_cache;
struct s_coefficient_layout;
struct s_lighting_record;
bool __stdcall function_23690(s_coefficient_layout const *layout, short order, long object_index,
    s_137801 const *vertices, byte const *indices, short count, long tag);
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

// @retail 0x4b5d0
void function_4b5d0(long object_index, long override, real priority, byte *output,
    byte *unused, bool project_nodes, real distance, bool force)
{
    (void)&object_index; (void)&override; (void)&priority;
    (void)&output; (void)&unused; (void)&project_nodes;
    transform4x3f projected[255];
    long failed = 0;
    signed char initial_level = -1;
    long entries = function_4bcc0(output, object_index, distance, override,
        (bool *)(output + 0x168), force, &initial_level);
    *(short *)(output + 0x164) = initial_level;
    if (output[0x168] && g_4b9ed8 != NONE &&
        *(short *)((byte *)g_4ed284 + g_4b9ed8 * 0x94 + 0x42) != NONE)
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
        signed char *sections = (signed char *)(output + 0x71 + slot * 16);
        function_3d4f0(*(bool *)(output + 0x168), (short)index, &flags, source_object,
            tag, &nodes, &node_count, object_index);
        signed char previous = -1;
        if (g_4ba050 >= 0 && g_4ba050 < 4)
        {
            byte *levels = NULL;
            long cache_index = function_3ddd0(*source_object);
            if (cache_index != NONE)
                levels = g_509434->data + (cache_index & 0xffff) * 0x100 + 0xbc;
            previous = (signed char)levels[g_4ba050];
        }
        if (g_4ba050 >= 0 && g_4ba050 < 4) function_3ddd0(*source_object);
        long cache_index = function_3ddd0(*source_object);
        *cache_slot = cache_index == NONE ? NULL :
            (s_batch_cache_4b5d0 *)(g_509434->data + (cache_index & 0xffff) * 0x100 + 0x44);
        output[slot + 0x6c] = (byte)(flags & 1);
        bool cached = previous != -1 && *cache_slot && (*cache_slot)->handle != NONE &&
            (*cache_slot)->stamp == ((g_4ba034 & 0x3fffffff) | ((dword)g_4ba050 << 30));
        if (cached) *(short *)(output + 0x164) = previous;
        bool fallback;
        bool success = function_4c190(count, object_index, *(bool *)(output + 0x168), *tag,
            (short *)(output + 0x164), previous, sections, &fallback);
        bool level_changed = *(short *)(output + 0x164) != initial_level;
        bool reuse = cached && !fallback;
        if (reuse && level_changed) reuse = false;
        if ((index > 0 && level_changed) || !success)
        {
            ++failed;
            continue;
        }
        long total = function_137650(*tag, *(short *)(output + 0x164), (byte const *)sections);
        output[slot + 0x28] = (byte)total;
        *handle = NONE;
        if (!reuse || output[0x168])
        {
            long bytes = total > 0 ? total * 48 + 0x44 : 0;
            long remainder = bytes % 4;
            if (remainder) bytes = bytes - remainder + 4;
            long next = g_487b18.size + bytes;
            if (next <= 0x27000)
            {
                *handle = g_487b18.size & 0x0fffffff;
                g_487b18.size = next;
            }
            if (*handle == NONE) success = false;
            else
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
                if (*handle != NONE && *handle >= 0)
                    result = (s_137801 *)(g_487b18.data + (*handle & 0x0fffffff));
                function_137800(object_index, *tag, nodes, (byte const *)sections,
                    *(short *)(output + 0x164), *(bool *)(output + 0x168), result);
            }
        }
        if (!reuse && success)
        {
            if (*(dword *)(output + 0x16c) & 0x40)
            {
                byte *definition = g_4e3b44[*tag & 0xffff].bytes;
                s_coefficient_layout const *layout = *(s_coefficient_layout **)(definition + 0x78);
                s_137801 const *vertices = NULL;
                if (*handle != NONE && *handle >= 0)
                    vertices = (s_137801 const *)(g_487b18.data + (*handle & 0x0fffffff));
                success = function_23690(layout, *(short *)(output + 0x164), *source_object,
                    vertices, (byte const *)sections, (short)*count, *tag);
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
byte const g_43f7b9[6] = { 0x2d, 0, 2, 3, 0x10, 0xff };

// @retail 0x43a40
void __stdcall function_43a40(long mode)
{
    (void)&mode;
    g_4c8790 = mode;
    g_4c878c = NONE;
    g_4c8788 = 0;
    g_4c8784 = 0;
    g_4c8780 = true;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_14bc0((short)g_4858b8, 0, true);
    long tag = *(long *)(g_485a80 + 0x64);
    if (tag != NONE)
    {
        s_type_7ba8e9 *bitmap = function_137550(tag, 1);
        if (bitmap) function_14390(0, bitmap, 0.0f);
    }
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    if (!(mode == 6)) {
        function_0222d0(D3DRS_CULLMODE, 0x901);
        function_0222d0(D3DRS_ZENABLE, 2);
        function_0222d0(D3DRS_ZFUNC, 0x203);
        function_0222d0(D3DRS_ZWRITEENABLE, 0);
    } else {
        function_0222d0(D3DRS_CULLMODE, 0);
        function_0222d0(D3DRS_ZENABLE, 0);
        function_0222d0(D3DRS_ZFUNC, 0x207);
        function_0222d0(D3DRS_ZWRITEENABLE, 1);
    }
    function_0222d0(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    switch (mode)
    {
    case 0:
    case 1:
        g_484f68.PSRGBInputs[0] = 0x8040000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xf150000;
        g_484f68.PSFinalCombinerInputsEFG = 0xc1c1c00;
        break;
    case 2:
    case 3:
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSRGBInputs[1] = ((mode == 2 ? 0xa0 : 0x20) << 8) | 0x81c003c;
        g_484f68.PSRGBOutputs[1] = 0xc00;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        break;
    case 4:
        g_484f68.PSRGBInputs[0] = 0x8040000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xc1c0000;
        break;
    case 5:
        g_484f68.PSCombinerCount = 0x11002;
        g_484f68.PSConstant0[0] = 0xff;
        g_484f68.PSRGBInputs[0] = 0x8040501;
        g_484f68.PSRGBOutputs[0] = 0xcd;
        g_484f68.PSAlphaInputs[0] = 0x38140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSRGBInputs[1] = 0x182b200b;
        g_484f68.PSRGBOutputs[1] = 0xd00;
        g_484f68.PSAlphaInputs[1] = 0x1c0d0000;
        g_484f68.PSAlphaOutputs[1] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0xf140000;
        g_484f68.PSFinalCombinerInputsEFG = 0xc0d1c00;
        break;
    default:
        g_484f68.PSTextureModes = 0;
        g_484f68.PSConstant1[3] = 0xff0000;
        g_484f68.PSFinalCombinerInputsABCD = 1;
        break;
    }
    function_1ccf0(&g_484f68);
    function_0222d0(D3DRS_ALPHABLENDENABLE, (dword)mode <= 5);
    if ((dword)mode <= 5)
    {
        function_0222d0(D3DRS_SRCBLEND, mode == 2 || mode == 3 ? 0x306 : 1);
        function_0222d0(D3DRS_DESTBLEND, mode == 2 ? 0x300 : mode == 3 || mode == 4 ? 0 : 0x303);
        function_0222d0(D3DRS_BLENDOP, mode == 4 ? 0x8008 : 0x8006);
    }
    function_0222d0(D3DRS_SOLIDOFFSETENABLE, mode != 6);
    if (mode != 6)
    {
        real offset = -200.0f;
        function_0222d0(D3DRS_POLYGONOFFSETZOFFSET, *(dword *)&offset);
        function_0222d0(D3DRS_POLYGONOFFSETZSLOPESCALE, 0);
    }
    function_1cc30(3);
    function_1c620((s_shader_cache *)g_51f0f0, (long)g_509444, 0, 16, g_43f7b9);
    function_1c710(g_51f0f0);
}

struct s_masked_list;
typedef void (__stdcall *masked_list_proc)(long value, short mask);
s_masked_list *g_547f94;
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
