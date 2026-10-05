// @flags /O2 /arch:SSE /Ob1 /Gr
/* UNKNOWN_03BCB0.CPP: predicting one bitmap's texture (unknown_03bcb0.h);
   its own /Ob1 file because retail calls it out of line from 0x16e5e0 */

#include "unknown_11c920.h"
#include "unknown_03bcb0.h"
#include <xtl.h>
#include "globals.h"

// @retail 0x3bcb0
void function_3bcb0(s_bitmap_data *bitmap)
{
	bitmap_predict_inline((s_bitmap_predict_view *)bitmap, 0xe);
}

struct s_sort_record
{
	word index;
	word key;
	byte unknown04;
	byte group;
	word subkey;
	word value08;
	word unknown0a;
	dword value0c;
	dword value10;
	dword unknown14;
};

struct s_record_source
{
	bool (__stdcall *fill)(long, void *, long, long, long, void *, s_sort_record *);
	dword unknown04;
	long key;
	void *context;
	dword flags;
	byte value14;
	byte value15;
	word unknown16;
};

struct s_record_sources
{
	dword unknown00;
	long count;
	dword flags;
	long first;
	dword unknown10;
	s_record_source *sources;
};

class c_type_4e7709;
c_type_4e7709 *function_137bd0(long tag_index);

class c_record_reference_view
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void slot4() = 0;
	virtual void slot5() = 0;
	virtual void slot6() = 0;
	virtual void slot7() = 0;
	virtual void slot8() = 0;
	virtual void slot9() = 0;
	virtual long *reference() = 0;
};

PRIVATE __forceinline byte *record_format_groups(long tag_index)
{
	long *reference;
	dword kind = *(dword *)g_4e3b44[(short)tag_index].unknown00;
	if (kind == 0x5052544d || kind == 0x70727433)
	{
		c_record_reference_view *provider = (c_record_reference_view *)function_137bd0(tag_index);
		reference = provider->reference();
	}
	else
		reference = *(long **)(g_4e3b44[tag_index & 0xffff].bytes + 0x24);
	return *(byte **)(g_4e3b44[*reference & 0xffff].bytes + 0x5c);
}

typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);

// @retail 0x3b9d0
bool function_3b9d0(s_record_sources *data, long tag_index, long pass, dword and_mask, dword or_mask,
	t_record_fill fill, dword value04, void *context, byte value14)
{
	bool result = false;
	if (data->count < (long)data->unknown00)
	{
		byte *groups = record_format_groups(tag_index);
		dword flags = *(dword *)(*(byte **)(groups + 4) + pass * 10 + 2);
		s_record_source *source = &data->sources[data->count++];
		source->fill = fill;
		source->unknown04 = value04;
		source->context = context;
		source->key = tag_index;
		source->value14 = value14;
		source->value15 = (byte)pass;
		source->flags = (flags & and_mask) | or_mask;
		data->flags |= source->flags;
		result = true;
	}
	return result;
}

s_record_sources g_4c1a48[3];

// @retail 0x3b8a0
void function_3b8a0(void)
{
	g_4c1a48[0].unknown00 = 0xa00;
	g_4c1a48[0].count = 0;
	void *buffer = VirtualAlloc(0, 0xf000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[0].sources = (s_record_source *)buffer;
	g_4c1a48[1].unknown00 = 0x400;
	g_4c1a48[1].count = 0;
	buffer = VirtualAlloc(0, 0x6000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[1].sources = (s_record_source *)buffer;
	g_4c1a48[2].unknown00 = 0x200;
	g_4c1a48[2].count = 0;
	buffer = VirtualAlloc(0, 0x3000, 0x101000, PAGE_READWRITE);
	if (!buffer) GetLastError();
	g_4c1a48[2].sources = (s_record_source *)buffer;
}

// @retail 0x3b950
void function_3b950(void)
{
	if (g_4c1a48[0].sources)
	{
		if (!VirtualFree(g_4c1a48[0].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[0].sources = 0;
	}
	if (g_4c1a48[1].sources)
	{
		if (!VirtualFree(g_4c1a48[1].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[1].sources = 0;
	}
	if (g_4c1a48[2].sources)
	{
		if (!VirtualFree(g_4c1a48[2].sources, 0, MEM_RELEASE)) GetLastError();
		g_4c1a48[2].sources = 0;
	}
}

// @retail 0x3ba90
long function_3ba90(long bit, s_record_sources const *data)
{
	long result;
	/* Retail keeps the source-table pointer on the stack. */
	s_record_sources const *const *reference = &data;
	result = 1 << bit;
	result &= (*reference)->flags;
	return !result;
}

// @retail 0x3bb80
bool __stdcall function_3bb80(void const *a, void const *b, void const *context)
{
	s_sort_record const *left = (s_sort_record const *)a;
	s_sort_record const *right = (s_sort_record const *)b;
	if (left->value10 > right->value10) return true;
	if (left->value10 < right->value10) return false;
	if (left->key > right->key) return true;
	if (left->key < right->key) return false;
	if (left->group > right->group) return true;
	if (left->group < right->group) return false;
	if (left->value08 > right->value08) return true;
	if (left->value08 < right->value08) return false;
	return false;
}

// @retail 0x3bbd0
bool __stdcall function_3bbd0(void const *a, void const *b, void const *context)
{
	s_sort_record const *left = (s_sort_record const *)a;
	s_sort_record const *right = (s_sort_record const *)b;
	if (left->group > right->group) return true;
	if (left->group < right->group) return false;
	if (left->subkey > right->subkey) return true;
	if (left->subkey < right->subkey) return false;
	if (left->value10 > right->value10) return true;
	if (left->value10 < right->value10) return false;
	if (left->key > right->key) return true;
	if (left->key < right->key) return false;
	if (left->value0c > right->value0c) return true;
	if (left->value0c < right->value0c) return false;
	if (left->index > right->index) return true;
	if (left->index < right->index) return false;
	return false;
}

typedef bool (__stdcall *t_record_compare)(void const *, void const *, void const *);
void function_13da70(void *elements, unsigned long count, unsigned long element_size, t_record_compare compare, void const *context);

// @retail 0x3bab0
long function_3bab0(s_record_sources *data, void *context, long mode, s_sort_record *out)
{
	/* All four arguments occupy stack slots in retail. */
	(void)&data;
	(void)&context;
	(void)&mode;
	(void)&out;
	long count = 0;
	for (long i = data->first; i < data->count; ++i)
	{
		s_record_source *source = &data->sources[i];
		if (source->flags & (1 << mode))
		{
			long key = source->key;
			long value15 = source->value15;
			s_sort_record *record = &out[count++];
			record->index = (word)i;
			record->key = (word)source->key;
			if (!source->fill(key, context, value15, mode, source->value14, source->context, record))
				--count;
		}
	}
	t_record_compare compare = function_3bb80;
	if (mode == 5)
		compare = function_3bbd0;
	function_13da70(out, count, sizeof(s_sort_record), compare, data);
	return count;
}
