// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "online_attributes.h"
#include <xtl.h>

struct s_record_header
{
	short salt;
	byte unknown02[0x0a];
	long handle;
};

struct s_key_value
{
	dword key;
	dword value;
};

s_key_value g_4672e0[40];


long __stdcall function_3ad1a6(long handle, long *out, long a, long b);

// @retail 0xb49a0
bool function_0b49a0(long index, real *result)
{
	bool success = false;

	*result = 0.0f;
	if (index != NONE)
	{
		s_record_pool *array = g_4cf78c;
		long absolute_index = index & 0xffff;
		if (absolute_index < array->high_water_index)
		{
			s_record_header *datum = (s_record_header *)(array->data + array->size * absolute_index);
			if (datum->salt != 0 && datum->salt == (index >> 16))
			{
				long raw_hundredths_count;
				if (function_3ad1a6(datum->handle, &raw_hundredths_count, 0, 0) >= 0)
				{
					*result = (real)(dword)raw_hundredths_count * 0.01f;
					success = true;
				}
			}
		}
	}

	return success;
}

// @retail 0xb4a20
dword function_0b4a20(dword key)
{
	dword result = 0x6f;

	for (dword i = 0; i < 40; i++)
	{
		if (g_4672e0[i].key == key)
		{
			result = g_4672e0[i].value;
			break;
		}
	}

	return result;
}

struct s_long7
{
	long v[7];
};

// @retail 0xb4a40
void function_0b4a40(s_long7 *out, const __int64 *in)
{
	memset(out, 0, sizeof(s_long7));
	out->v[0] = (long)in[0];
	out->v[1] = (long)in[1];
	out->v[2] = (long)in[2];
	out->v[3] = (long)in[3];
	out->v[4] = (long)in[4];
	out->v[5] = (long)in[5];
	out->v[6] = (long)in[6];
}

// @retail 0xb4a90
long function_0b4a90(s_entry_pair *out, const s_range_input *in)
{
	long i;

	out[0].a = 0;
	out[0].a_high = 0;
	out[0].b = in->x;
	out[1].a = 0;
	out[1].a_high = 0;
	out[1].b = in->y;
	if (in->has_min)
	{
		out[2].a = 0;
		out[2].b = in->min;
	}
	else
	{
		out[2].a = 0xf00000;
		out[2].b = 0;
	}
	out[2].a_high = 0;
	if (in->has_max)
	{
		out[3].a = 0;
		out[3].b = in->max;
	}
	else
	{
		out[3].a = 0xf00000;
		out[3].b = 0;
	}
	out[3].a_high = 0;
	i = 1;
	long *value = (long *)&out[4].b;
	do
	{
		if (in->has_count && in->count == i)
		{
			value[-2] = 0;
			*(__int64 *)value = 1;
		}
		else
		{
			value[-2] = 0xf00000;
			*(__int64 *)value = 0;
		}
		value[-1] = 0;
		value += 4;
		++i;
	} while (i < 8);
	if (in->has_count && in->count >= 8)
	{
		out[11].a = 0;
		out[11].b = 1;
	}
	else
	{
		out[11].a = 0xf00000;
		out[11].b = 0;
	}
	out[11].a_high = 0;

	return 12;
}

// @retail 0xb4b80
long function_0b4b80(long count, s_property_entry *out, const s_property_input *in)
{
	memset(out, 0, count * sizeof(s_property_entry));

	out[0].key = 1;
	out[0].valid = 1;
	out[0].value = in->v0;
	out[1].key = 2;
	out[1].valid = 1;
	out[1].value = in->v1;
	out[2].key = 3;
	out[2].valid = 1;
	out[2].value = in->v2;
	out[3].key = 5;
	out[3].valid = 1;
	out[3].value = in->v3;
	out[4].key = 6;
	out[4].valid = 1;
	out[4].value = in->v4;
	out[5].key = 7;
	out[5].valid = 1;
	out[5].value = in->v5;
	out[6].key = 8;
	out[6].valid = 1;
	out[6].value = in->flags;
	out[7].key = 9;
	out[7].valid = 1;
	out[7].value = (long)(in->flags & 1);
	out[8].key = 10;
	out[8].valid = 1;
	out[8].value = (long)((in->flags >> 1) & 1);
	out[9].key = 11;
	out[9].valid = 1;
	out[9].value = (long)((in->flags >> 2) & 1);
	out[10].key = 12;
	out[10].valid = 1;
	out[10].value = (long)((in->flags >> 3) & 1);
	out[11].key = 13;
	out[11].valid = 1;
	out[11].value = (long)((in->flags >> 4) & 1);
	out[12].key = 14;
	out[12].valid = 1;
	out[12].value = (long)((in->flags >> 5) & 1);
	out[13].key = 15;
	out[13].valid = 1;
	out[13].value = (long)((in->flags >> 6) & 1);
	out[14].key = 16;
	out[14].valid = 1;
	out[14].value = (long)((in->flags & 0xffffff80) != 0);

	return 15;
}

struct s_block_header
{
	long marker;
	short unknown4;
	word tag;
};

PRIVATE s_block_header *block_init(s_block_header *block, word tag)
{
	if (block)
	{
		block->marker = -1;
		block->unknown4 = 0;
		block->tag = tag;
	}

	return block;
}

PRIVATE s_block_header *block_alloc()
{
	s_block_header *block = (s_block_header *)VirtualAlloc(0, 8, 0x101000, PAGE_READWRITE);

	if (!block)
	{
		GetLastError();
	}

	return block;
}

// @retail 0xb4d50
s_block_header *function_0b4d50(word tag)
{
	s_block_header *block = 0;

	if (g_transport_globals.initialized && g_transport_globals.started)
	{
		block = block_alloc();
		block = block_init(block, tag);
	}

	return block;
}
