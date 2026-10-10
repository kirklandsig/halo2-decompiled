// @flags /O2 /Gr /arch:SSE
#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "crc.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_0259d0.h"

struct s_type_1a7926
{
	byte unknown00[8];
	transform4x3f root_matrix;
	byte unknown3c[0x48 - 0x3c];
	short *node_indices;
	byte unknown4c[4];
	transform4x3f *field_50;
};

transform4x3f *function_ba160(long object_index, transform4x3f *matrix);

// @retail 0x20a9a0
bool function_20a9a0(s_type_1a7926 *matrices, long object_index)
{
	bool result = false;
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	(void)&definition;
	long model_index = *(long *)(definition + 0x38);
	if (model_index != NONE)
	{
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		long nodes_index = *(long *)(model + 0x24);
		if (nodes_index != NONE)
		{
			byte *nodes = g_4e3b44[nodes_index & 0xffff].bytes;
			byte *indices = object + *(short *)(object + 0x11a);
			byte *transforms = object + *(short *)(object + 0x116);
			*(long *)&matrices->unknown00[0] = object_index;
			*(long *)&matrices->unknown00[4] = *(long *)(object + 0xb4);
			function_ba160(object_index, &matrices->root_matrix);
			*(long *)&matrices->unknown3c[0] = *(long *)(definition + 0x38);
			*(long *)&matrices->unknown3c[4] = *(long *)(model + 0x24);
			*(byte **)&matrices->unknown3c[8] = model;
			matrices->node_indices = (short *)nodes;
			*(byte **)&matrices->unknown4c[0] = indices;
			matrices->field_50 = (transform4x3f *)transforms;
			result = true;
		}
	}
	return result;
}

byte *g_51e9ec;
dword g_51e9f0;
void *g_51e9f4;

struct s_anim_header
{
	signed char unknown0;
	signed char unknown1;
	short unknown2;
	short unknown4;
	short unknown6;
	byte unknown8[4];
	long unknownc;
};

struct s_anim_data
{
	byte *base;
	s_anim_header *header;
	byte unknown8;
	byte type;
	short count;
};

struct s_vec4 { vector3f v; real w; };
struct real_vector2d_copy
{
	real i, j;
};


PRIVATE __forceinline void function_20aa71(vector3f *arg_0, byte const *arg_1)
{
	((dword *)arg_0)[0] = ((dword const *)arg_1)[0];
	((dword *)arg_0)[1] = ((dword const *)arg_1)[1];
	((dword *)arg_0)[2] = ((dword const *)arg_1)[2];
}

// @retail 0x20aa70
void function_20aa70(vector3f *out, s_anim_data *data, long index_, real *w)
{
	short index = (short)index_;
	s_anim_header *h;
	byte *p;

	*out = *g_4687a4;
	*w = 0.0f;
	switch (data->type)
	{
	case 1:
		h = data->header;
		p = data->base + h->unknown6 + h->unknown1 + index * 8 + h->unknownc + h->unknown0;
		((dword *)out)[0] = ((dword *)p)[0];
		((dword *)out)[1] = ((dword *)p)[1];
		break;
	case 2:
		h = data->header;
		p = data->base + h->unknown6 + h->unknown1 + index * 12 + h->unknownc + h->unknown0;
		((dword *)out)[0] = ((dword *)p)[0];
		((dword *)out)[1] = ((dword *)p)[1];
		*w = ((real *)p)[2];
		break;
	case 3:
		h = data->header;
		p = data->base + h->unknown6 + h->unknown1 + index * 16 + h->unknownc + h->unknown0;
		function_20aa71(out, p);
		*w = ((real *)p)[3];
		break;
	}
}
// @retail 0x20ab60
void function_20ab60(s_anim_data *data, vector3f *sum, real *w)
{
	s_anim_header *h;
	byte *p;
	long i;
	short j;

	*sum = *g_4687a4;
	*w = 0.0f;
	switch (data->type)
	{
	case 1:
		for (i = 0; i < data->count; i++)
		{
			h = data->header;
			j = (short)i;
			p = data->base + h->unknown6 + h->unknown1 + j * 8 + h->unknownc + h->unknown0;
			sum->i = ((real *)p)[0] + sum->i;
			sum->j = ((real *)p)[1] + sum->j;
		}
		break;
	case 2:
		for (i = 0; i < data->count; i++)
		{
			h = data->header;
			j = (short)i;
			p = data->base + h->unknown6 + h->unknown1 + j * 12 + h->unknownc + h->unknown0;
			sum->i = ((real *)p)[0] + sum->i;
			sum->j = ((real *)p)[1] + sum->j;
			*w = ((real *)p)[2] + *w;
		}
		break;
	case 3:
		for (i = 0; i < data->count; i++)
		{
			h = data->header;
			j = (short)i;
			p = data->base + h->unknown6 + h->unknown1 + j * 16 + h->unknownc + h->unknown0;
			sum->i += ((real *)p)[0];
			sum->j += ((real *)p)[1];
			sum->k += ((real *)p)[2];
			*w += ((real *)p)[3];
		}
		break;
	}
	if (data->count - 1 > 0)
	{
		sum->i /= (real)data->count - 1.0f;
		sum->j /= (real)data->count - 1.0f;
		sum->k /= (real)data->count - 1.0f;
		*w /= (real)data->count;
	}
}

// @retail 0x20ad40
void function_20ad40(s_anim_data *data, vector3f *a, vector3f *b, long index)
{
	s_anim_header *h;
	byte *p;

	*a = *(vector3f *)g_468788;
	*b = *g_4687a4;
	h = data->header;
	if (h->unknown4 != 0)
	{
		p = *(byte * volatile *)&data->base + (h->unknown2 + h->unknown6 + h->unknown1 + h->unknownc + h->unknown0);
		p += index * 12;
		*a = *(vector3f *)p;
		if (index + 1 < data->count)
		{
			b->i = ((real *)p)[3] - ((real *)p)[0];
			b->j = ((real *)p)[4] - ((real *)p)[1];
			b->k = ((real *)p)[5] - ((real *)p)[2];
		}
	}
}

// @retail 0x20adf0
void function_20adf0(s_anim_data *data, byte *dest)
{
	s_anim_header *h;
	byte *src;
	long n;
	long i;

	h = data->header;
	if (h->unknown1)
	{
		src = data->base + h->unknown6 + h->unknownc + h->unknown0;
		n = h->unknown1 / 3;
		for (i = 0; i < n; i++)
		{
			dest[i] |= src[i];
			dest[i] |= src[n + i];
			dest[i] |= src[2 * n + i];
		}
	}
	h = data->header;
	if (h->unknown0)
	{
		if (h->unknown6)
		{
			src = data->base + h->unknown6 + h->unknownc;
			n = h->unknown0 / 3;
			for (i = 0; i < n; i++)
			{
				dest[i] |= src[i];
				dest[i] |= src[n + i];
				dest[i] |= src[2 * n + i];
			}
		}
	}
}
// @retail 0x20aee0
void function_20aee0(void)
{
	long size = 0x1880;
	dword offset = (dword)game_state_globals.base_address + game_state_globals.cpu_allocation_size;
	game_state_globals.cpu_allocation_size += 0x1880;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, 4);
	g_51e9f0 = offset;
	void *memory = VirtualAlloc(NULL, 0x1880, 0x101000, PAGE_READWRITE);
	if (!memory)
	{
		GetLastError();
	}
	g_51e9f4 = memory;
	g_51e9ec = (byte *)memory;
}

// @retail 0x20af50
void function_20af50(void)
{
	if (!VirtualFree(g_51e9f4, 0, MEM_RELEASE))
	{
		GetLastError();
	}
	g_51e9f4 = NULL;
	g_51e9f0 = 0;
	g_51e9ec = NULL;
}

// @retail 0x20af90
void function_20af90(void)
{
	memset(g_51e9ec, 0, 0x1880);
}
