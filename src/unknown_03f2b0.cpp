// @flags /O2 /Gr
/* UNKNOWN_03F2B0.CPP: visibility lists, resource prediction and index caches */

#include "unknown_11c920.h"
#include "globals.h"
#include "physical_memory.h"

void *g_509438;

// @retail 0x43990
void function_43990(void)
{
	physical_memory_new_frame((s_physical_object *)g_509448);
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
extern byte g_509415;

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

short g_4c1bd4;

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

struct s_index_cache
{
	long index;
	short count;
	short unknown06;
	long values[256];
};

s_index_cache g_4c6b00[8];
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
