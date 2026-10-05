// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0350E0.CPP: format conversion and geometry helpers */

#include "unknown_11c920.h"
#include "globals.h"

extern bool g_4b9ee9;
extern long g_4b9eec;

struct s_cluster_tag_entry
{
	dword unknown00;
	long tag;
};

struct s_cluster_tag_table
{
	byte unknown00[8];
	long count;
	s_cluster_tag_entry *entries;
};

// @retail 0x3eb70
real function_3eb70(void)
{
	real result = 1.0f;
	if (g_4b9ee9 && g_4b9eec != NONE)
	{
		long index = NONE;
		s_cluster_tag_table *table = (s_cluster_tag_table *)g_4e0350;
		if ((short)g_4b9eec >= 0 && (short)g_4b9eec < table->count)
			index = table->entries[(short)g_4b9eec].tag;
		byte *data = 0;
		if (index != NONE)
			data = g_4e3b44[index & 0xffff].bytes;
		result = *(real *)(data + 0x14);
	}
	return result;
}

// @retail 0x350e0
void function_350e0(real *out, real const *a, real const *b, real x)
{
	real scale;
	real v;
	real w;

	out[0] = (a[1] - a[0]) * b[0] + a[0];
	out[1] = (a[3] - a[2]) * b[1] + a[2];

	scale = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
	v = (scale * x - g_485ad4.lo * scale) / x * 16777215.0f;
	if (0.0f > v)
		v = 0.0f;
	else if (v > 16777215.0f)
		v = 16777215.0f;
	out[2] = v;

	w = x / g_485ad4.hi * 16777215.0f;
	if (0.0f > w)
		w = 0.0f;
	else if (w > 16777215.0f)
		w = 16777215.0f;
	out[3] = w;
}

// @retail 0x35510
long function_35510(long format)
{
	switch (format)
	{
	case 0: return 1;
	case 1: return 8;
	case 2: return 7;
	case 3: return 2;
	case 4: return 15;
	case 5: return 3;
	case 12: return 5;
	case 14: return 6;
	case 6: return 12;
	case 7: return 13;
	case 18: return 14;
	case 8: return 10;
	case 9: return 11;
	case 10: return 17;
	case 11: return 16;
	case 13: return 18;
	case 16: return 19;
	case 15: return 20;
	case 17: return 21;
	default: __assume(0);
	}
}

// @retail 0x35790
long function_35790(long format)
{
	switch (format)
	{
	case 0: return 0x12;
	case 1: return 0x22;
	case 2: return 0x32;
	case 3: return 0x42;
	case 4: return 0x14;
	case 5: return 0x24;
	case 6: return 0x34;
	case 7: return 0x44;
	case 8: return 0x15;
	case 9: return 0x25;
	case 10: return 0x35;
	case 11: return 0x45;
	case 12: return 0x11;
	case 13: return 0x21;
	case 14: return 0x31;
	case 15: return 0x41;
	case 16: return 0x16;
	case 17: return 0x40;
	default: __assume(0);
	}
}

// @retail 0x35850
long function_35850(long format)
{
	switch (format)
	{
	case 2: return 12;
	case 3: return 16;
	case 4: return 1;
	case 5: return 2;
	case 6: return 3;
	case 10: return 6;
	case 1: return 8;
	case 0: return 4;
	case 7: return 4;
	case 8: return 2;
	case 9: return 4;
	case 11: return 8;
	case 12: return 2;
	case 13: return 4;
	case 14: return 6;
	case 15: return 8;
	case 16: return 4;
	case 17: return 4;
	default: __assume(0);
	}
}

// @retail 0x4dcd0
vector3f *function_4dcd0(vector3f *out, dword packed)
{
	vector3f value;
	value.i = (packed & 15) * (1.0f / 15.0f);
	value.j = ((packed >> 4) & 15) * (1.0f / 15.0f);
	value.k = ((packed >> 8) & 15) * (1.0f / 15.0f);
	value.i = value.i * 2.0f - 1.0f;
	value.j = value.j * 2.0f - 1.0f;
	value.k = value.k * 2.0f - 1.0f;
	*out = value;
	return out;
}

// @retail 0x4dd70
vector3f *function_4dd70(vector3f *out, dword packed)
{
	vector3f value;
	value.i = (packed & 2047) * (1.0f / 2047.0f);
	value.j = ((packed >> 11) & 2047) * (1.0f / 2047.0f);
	value.k = (packed >> 22) * (1.0f / 1023.0f);
	value.i = value.i * 2.0f - 1.0f;
	value.j = value.j * 2.0f - 1.0f;
	value.k = value.k * 2.0f - 1.0f;
	*out = value;
	return out;
}

struct s_format_element
{
	byte unknown00[0x2c];
	short format;
	byte unknown2e[0x2e];
};

struct s_format_element_table
{
	byte unknown00[0x28];
	s_format_element *elements;
};

// @retail 0x4cb80
long function_4cb80(long element_index, long tag_index)
{
	long result = NONE;
	if (tag_index != NONE)
	{
		s_format_element_table *table = (s_format_element_table *)g_4e3b44[tag_index & 0xffff].bytes;
		if (table)
			result = table->elements[element_index].format;
	}
	return result;
}

struct s_format_block
{
	byte unknown00[0xc];
	long size;
	byte unknown10[0xc];
};

struct s_format_block_table
{
	dword unknown00;
	dword count;
	s_format_block *blocks;
};

// @retail 0x4cbb0
long function_4cbb0(long tag_index, dword block_index)
{
	if (tag_index != NONE)
	{
		s_format_block_table *table = (s_format_block_table *)g_4e3b44[tag_index & 0xffff].bytes;
		if (block_index < table->count && (dword)(table->blocks[block_index].size >> 4) > 0)
			return 1;
	}
	return 0;
}

// @retail 0x3fd20
bool function_3fd20(plane3f const *planes, point3f const *point, real radius)
{
	real extent = radius * 1.7320508f;
	for (long i = 0; i < 6; ++i)
	{
		if (plane_distance_to_point(&planes[i], point) >= extent)
			return true;
	}
	return false;
}

struct s_primitive_header
{
	byte type;
	byte unknown01[3];
	long size;
	long capacity;
};

struct s_primitive_storage
{
	dword unknown00;
	byte *data;
};

struct s_primitive_buffer
{
	byte flags;
	byte unknown01[0x33];
	s_primitive_storage *storage;
};

struct s_primitive_group
{
	word unknown00;
	word buffer_index;
	dword unknown04;
	word *counts;
};

struct s_primitive_definition
{
	byte unknown00[0x44];
	s_primitive_buffer *buffers;
	byte unknown48[0xc];
	s_primitive_group *groups;
	byte unknown58[0xc];
	s_primitive_group *groups_alt;
};

// @retail 0x4dc60
s_primitive_header *function_4dc60(s_primitive_definition *data, long index, bool first, long element)
{
	s_primitive_header *result = 0;
	s_primitive_group *group;
	word const *count;
	if (first)
	{
		group = &data->groups[index];
		count = group->counts;
	}
	else
	{
		group = &data->groups_alt[index];
		count = &group->counts[element];
	}
	s_primitive_buffer *buffer = &data->buffers[group->buffer_index];
	if (buffer->flags & 2)
	{
		result = (s_primitive_header *)(buffer->storage->data + 0x20);
		long size = result->type == 0x2e ? 4 * *count : 3 * *count;
		result->size = size;
		result->capacity = size;
	}
	return result;
}
