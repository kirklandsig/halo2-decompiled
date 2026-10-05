// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include "unknown_058ee0.h"

struct s_buffer_pair
{
	long first;
	long second;
};

s_buffer_pair g_5093c0;
long g_5093c8;
long g_5093cc;

// @retail 0x13d50
void function_13d50(s_buffer_pair const *values, long mode, long count)
{
	/* The count remains on the stack in retail. */
	(void)&count;
	long first = values->first;
	long second = values->second;
	g_5093c8 = mode;
	g_5093c0.first = first;
	g_5093c0.second = second;
	g_5093cc = count;
}

/* the texture stages: [0] the textures set on the device (0x51f3c8), [1] the
   textures wanted (0x51f3d8) */
IDirect3DBaseTexture8 *g_51f3c8[2][4];
long g_5093d8;
dword g_487288[17][32];
D3DSurface *g_51f3f4;
D3DSurface *g_51f3f8;
byte g_51f3fc;

extern D3DResource *g_509374, *g_509378, *g_50937c, *g_509380;
D3DTexture *g_509354[2];
D3DSurface *g_50935c, *g_509360, *g_509364, *g_509370;
D3DTexture *g_509368, *g_50936c;
D3DSurface *g_509384;
D3DTexture *g_50938c;
D3DSurface *g_509390, *g_509394, *g_509398, *g_50939c, *g_5093a0, *g_5093a4, *g_5093a8;
short g_485602;
real g_4670c8 = 1.0f;
extern byte g_485607, g_5093fc;
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern __int64 g_485aa0;
long g_485af4[4], g_485b04[4];

void *function_01dcf0(long index);
void *function_01dd20(long index, long element);
void *function_01dcc0(long index);
bool function_01dd60(long index, long *width, long *height);
long function_25960(void);
bool function_1cd30(D3DSurface *target, D3DSurface *depth, bool flag);

// @retail 0x14bc0
void function_14bc0(short index, short element, bool use_depth)
{
	D3DSurface *target = NULL;
	D3DSurface *depth = NULL;
	bool flag = false;
	switch (index)
	{
	case 0: target = g_50935c; depth = g_509364; flag = true; break;
	case 4: target = g_509360; flag = true; break;
	case 16: case 29: target = (D3DSurface *)g_509378; break;
	case 17: target = (D3DSurface *)g_509380; break;
	case 3: target = g_509370; depth = g_509364; flag = true; break;
	case 1: target = (D3DSurface *)function_01dcf0(1); depth = g_509364; break;
	case 18: target = (D3DSurface *)function_01dcf0(18); depth = g_509364; break;
	case 9:
		target = g_509384;
		depth = (D3DSurface *)function_01dcf0(9);
		target->Size = depth->Size;
		target->Data = ((D3DSurface *)function_01dcf0(9))->Data;
		break;
	case 10: case 11: case 12: case 25: case 26: case 27:
		target = (D3DSurface *)function_01dcf0(index); depth = NULL; break;
	case 13: target = (D3DSurface *)function_01dd20(13, element); depth = NULL; break;
	case 19: target = (D3DSurface *)function_01dcf0(19); depth = NULL; break;
	case 20: target = (D3DSurface *)function_01dcf0(20); depth = NULL; break;
	case 23: depth = NULL; break;
	case 5: case 6: case 7: case 8: case 15:
		target = (D3DSurface *)function_01dcf0(index); depth = g_509364; break;
	case 33: target = g_509390; depth = g_5093a8; break;
	case 34: target = g_509394; depth = g_5093a8; break;
	case 35: target = g_509398; depth = g_5093a8; break;
	case 36: target = g_50939c; depth = g_5093a8; break;
	case 37: target = g_5093a0; depth = g_5093a8; break;
	case 38: target = g_5093a4; depth = g_5093a8; break;
	case 22: target = (D3DSurface *)function_01dcf0(22); depth = NULL; break;
	case 21: target = (D3DSurface *)function_01dcf0(21); depth = NULL; break;
	case 14: break;
	default: __assume(0);
	}
	function_1cd30(target, use_depth ? depth : NULL, flag);
	D3DVIEWPORT8 viewport;
	if (g_485602 == 2)
	{
		viewport.X = 0; viewport.Y = 0;
		viewport.Width = 128; viewport.Height = 128;
	}
	else if (index == 0 || index == 2 || index == 1 || index == 18 || index == function_25960())
	{
		real scale;
		if (index == function_25960())
		{
			if (g_485607)
			{
				viewport.X = 0; viewport.Y = 0;
				viewport.Width = 640; viewport.Height = 480;
				goto viewport_ready;
			}
			scale = 1.0f;
		}
		else
		{
			if (g_485607 && !g_5093fc)
				scale = g_4670c8 < 0.0625f ? 0.0625f : g_4670c8 > 1.0f ? 1.0f : g_4670c8;
			else
				scale = 1.0f;
		}
		viewport.X = (long)((short)g_48564a * (double)scale);
		viewport.Y = (long)((short)g_485648 * (double)scale);
		viewport.Width = (long)(((short)g_48564e - (short)g_48564a) * (double)scale);
		viewport.Height = (long)(((short)g_48564c - (short)g_485648) * (double)scale);
	}
	else
	{
		D3DSURFACE_DESC desc;
		D3DSurface_GetDesc(target, &desc);
		viewport.X = 0; viewport.Y = 0;
		viewport.Width = desc.Width; viewport.Height = desc.Height;
	}
viewport_ready:
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	D3DDevice_SetViewport(&viewport);
}

// @retail 0x14f60
void function_14f60(short stage, short index)
{
	D3DBaseTexture *texture = NULL;
	long width = 0, height = 0;
	function_01dd60(index, &width, &height);
	g_485af4[stage] = width;
	g_485b04[stage] = height;
	switch (index)
	{
	case 0: texture = g_509354[(g_485aa0 - 1) % 2]; break;
	case 4: texture = g_509354[g_485aa0 % 2]; break;
	case 16: case 29: texture = (D3DBaseTexture *)g_509374; break;
	case 17: texture = (D3DBaseTexture *)g_50937c; break;
	case 3: texture = g_509368; break;
	case 24: texture = g_50936c; break;
	case 1: texture = (D3DBaseTexture *)function_01dcc0(1); break;
	case 18: texture = (D3DBaseTexture *)function_01dcc0(18); break;
	case 19: texture = (D3DBaseTexture *)function_01dcc0(19); break;
	case 23: texture = g_509354[(g_485aa0 - 1) % 2]; break;
	case 13: texture = (D3DBaseTexture *)function_01dcc0(13); break;
	case 20: texture = (D3DBaseTexture *)function_01dcc0(20); break;
	case 33: case 34: case 35: case 36: case 37: case 38: texture = g_50938c; break;
	case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 15: case 21: case 22: case 25: case 26: case 27:
		texture = (D3DBaseTexture *)function_01dcc0(index); break;
	case 14: break;
	default: __assume(0);
	}
	g_51f3c8[1][stage] = texture;
}

struct s_mask_offset
{
	short x;
	short y;
};

s_mask_offset const g_43f188[16] =
{
	{0, 0}, {2, 2}, {0, 2}, {2, 0},
	{1, 1}, {3, 3}, {3, 1}, {1, 3},
	{0, 1}, {2, 3}, {3, 0}, {1, 2},
	{1, 0}, {3, 2}, {0, 3}, {2, 1}
};

// @retail 0x1c1b0
void function_1c1b0(void)
{
	memset(g_487288, 0, sizeof(g_487288));
	for (long count = 0; count <= 16; count++)
	{
		long x;
		dword *mask = g_487288[count];
		for (x = 0; x < 32; x += 4)
		{
			long base = x;
			for (long rows = 8; rows > 0; rows--, base += 128)
			{
				for (long i = 0; i < count; i++)
				{
					long bit = g_43f188[i].y * 32 + base + g_43f188[i].x;
					mask[bit / 32] = mask[bit / 32] | (1 << (bit % 32));
				}
			}
		}
	}
}

// @retail 0x1cd30
bool function_1cd30(D3DSurface *target, D3DSurface *depth, bool flag)
{
	if (g_51f3f4 != target || g_51f3f8 != depth)
	{
		if (g_51f3f4 && target && target->Common == g_51f3f4->Common && target->Format == g_51f3f4->Format && target->Size == g_51f3f4->Size)
			D3DDevice_SetRenderTargetFast(target, depth, 0);
		else
			D3DDevice_SetRenderTarget(target, depth);
		g_51f3f4 = target;
		g_51f3f8 = depth;
		g_51f3fc = flag;
	}
	return true;
}

// @retail 0x1c290
dword *function_1c290(real value)
{
	real scaled = value * 16.0f;
	long index;
	__asm
	{
		fld scaled
		fistp index
	}
	if (index < 0)
		index = 0;
	else if (index > 16)
		index = 16;
	return g_487288[index];
}

struct s_01b050_shader_state
{
	byte field_0000[0x1424];
	D3DPIXELSHADERDEF program;
	bool changed;
};

// @retail 0x1b050
void function_1b050(s_01b050_shader_state *state)
{
	D3DDevice_SetPixelShaderProgram(&state->program);
	state->changed = false;
}

// @retail 0x15180
void function_15180(D3DPIXELSHADERDEF const *program)
{
	D3DDevice_SetPixelShaderProgram(program);
}

// @retail 0x151c0
void function_151c0(real const *constants)
{
	D3DDevice_SetVertexShaderConstantFast(-46, constants, 3);
	g_5093d8 = 0;
}

// @retail 0x1ccf0
bool function_1ccf0(D3DPIXELSHADERDEF const *program)
{
	D3DDevice_SetPixelShaderProgram(program);
	return true;
}

// @retail 0x1cf50
void function_1cf50()
{
	IDirect3DBaseTexture8 **wanted = g_51f3c8[1];
	for (long stage = 0; stage < 4; wanted++, stage++)
	{
		IDirect3DBaseTexture8 **current = wanted - 4;
		if (*current != *wanted)
		{
			D3DDevice_SetTexture(stage, *wanted);
			*current = *wanted;
		}
	}
}

// @retail 0x1cf80
void function_1cf80(void)
{
	for (long stage = 0; stage < 4; stage++)
	{
		D3DDevice_SetTexture(stage, g_51f3c8[1][stage]);
		g_51f3c8[0][stage] = g_51f3c8[1][stage];
	}
}

// @retail 0x1c8a0
bool __stdcall function_1c8a0(D3DPRIMITIVETYPE type, word const *indices, long count)
{
	function_1cf50();
	D3DDevice_DrawIndexedVertices(type, count, indices);
	return true;
}

struct s_597d0_object
{
	byte unknown00[0x741c];
	long field_741c;
};

// @retail 0x597d0
bool function_597d0(s_597d0_object **out)
{
	bool result = false;
	long mode = 0;
	if (g_527330.initialized)
		mode = g_527330.state;

	switch (mode)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		result = false;
		if (g_527330.initialized)
		{
			s_597d0_object *object = g_527330.session_a;
			if (object->field_741c)
			{
				if (out)
					*out = object;
				result = true;
			}
		}
		break;
	case 7:
	case 8:
	case 9:
		result = false;
		if (g_527330.initialized)
		{
			s_597d0_object *object = g_527330.session_b;
			if (object->field_741c)
			{
				if (out)
					*out = object;
				result = true;
			}
		}
		break;
	}
	return result;
}


struct s_shader_entry
{
	long field_00;
	dword input_count;
	word *inputs;
	long byte_count;
	dword *program;
	byte field_14[8];
};

struct s_shader_tag
{
	long field_00;
	dword entry_count;
	s_shader_entry *entries;
};

struct s_shader_program
{
	dword const *program;
	long field_04;
	dword byte_count;
	long field_0c;
	real field_10;
};

dword const g_43f2b0[85] =
{
	0x00152078, 0x00000000, 0x0056e000, 0x7c2a1000, 0x2ca00000,
	0x00000000, 0x0056e0aa, 0x7c021000, 0x23a00000, 0x00000000,
	0x00570000, 0x8c2a1000, 0x2cb00000, 0x00000000, 0x0096e615,
	0x38aaf856, 0x9ca00000, 0x00000000, 0x0096e601, 0x39fefaae,
	0x93a00000, 0x00000000, 0x00970615, 0x38ab1856, 0xdcb00000,
	0x00000000, 0x00d6201b, 0x08363800, 0x20b08800, 0x00000000,
	0x00d6401b, 0x08365800, 0x20b04800, 0x00000000, 0x00d6601b,
	0x08367800, 0x20b02800, 0x00000000, 0x00d6801b, 0x08369800,
	0x20b01800, 0x00000000, 0x02575215, 0xa42b586e, 0x6ca0f81c,
	0x00000000, 0x065740ab, 0xa5575bff, 0x13a10000, 0x00000000,
	0x00576015, 0xb42b7800, 0x2cb00000, 0x00000000, 0x00770015,
	0xa40012fe, 0x3ca00000, 0x00000000, 0x007720ab, 0xa4001006,
	0x73a00000, 0x00000000, 0x00772015, 0xb40012fe, 0x7cb00000,
	0x00000000, 0x0041401a, 0xc4355800, 0x20b0e800, 0x00000000,
	0x0056a015, 0xa42ab800, 0x2090c848, 0x00000000, 0x0056c0bf,
	0xa42ad800, 0x20a0c850, 0x00000000, 0x0056c015, 0xb57ed800,
	0x20a0c858, 0x00000000, 0x0081601a, 0xc5fe286a, 0xf0b0e801,
};

s_shader_program g_4670ec[1] =
{
	{ g_43f2b0, NONE, 0x154, NONE, 1.0f }
};

// @retail 0x1c2e0
dword const *function_1c2e0(long tag, long index, long *count)
{
	dword const *result;
	if (tag != NONE)
	{
		s_shader_entry *entry = &((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entries[index];
		result = entry->program;
		if (count)
			*count = entry->byte_count >> 4;
	}
	else
	{
		result = g_4670ec[index].program;
		if (count)
			*count = g_4670ec[index].byte_count >> 4;
	}
	return result;
}

// @retail 0x1cb20
long function_1cb20(long mode, dword index, long tag)
{
	switch (mode)
	{
	case 1: return 1;
	case 2: return 2;
	case 3:
		if (index + 2 < ((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entry_count)
			return 1;
	case 0: return 0;
	default: __assume(0);
	}
}

// @retail 0x1cb70
dword function_1cb70(long tag, long index)
{
	dword result = 0;
	s_shader_entry *entry = &((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entries[index];
	for (dword i = 0; i < entry->input_count; i++)
		result |= 1 << entry->inputs[i];
	return result;
}

struct s_shader_binding
{
	long field_00;
	long tag;
	long index;
	long field_0c;
};

struct s_shader_stream
{
	long field_00;
	long field_04;
	long field_08;
};

struct s_shader_cache
{
	s_shader_binding bindings[8];
	long active;
	long field_84;
	bool field_88;
	byte field_89[3];
	byte const *wanted[16];
	byte const *current[16];
	byte field_10c[0x100];
	bool descriptors_changed;
	byte field_20d[3];
	s_shader_stream streams[16];
	long stream_count;
	bool streams_changed;
	byte field_2d5[3];
};

// @retail 0x1c4a0
void function_1c4a0(s_shader_cache *state)
{
	state->streams_changed = false;
	state->descriptors_changed = false;
	state->stream_count = 0;
	for (long i = 0; i < 8; i++)
	{
		state->bindings[i].field_0c = NONE;
		state->bindings[i].tag = NONE;
		state->bindings[i].index = NONE;
	}
	state->active = NONE;
	for (long j = 0; j < 16; j++)
		state->wanted[j] = NULL;
	for (long k = 0; k < 16; k++)
	{
		state->streams[k].field_08 = 0;
		state->streams[k].field_04 = 0;
		state->streams[k].field_00 = 0;
	}
}

// @retail 0x1c6b0
void function_1c6b0(void *memory)
{
	s_shader_cache *state = (s_shader_cache *)memory;
	state->stream_count = 0;
	state->streams_changed = true;
	state->descriptors_changed = true;
	for (long i = 0; i < 16; i++)
	{
		state->streams[i].field_08 = 0;
		state->streams[i].field_04 = 0;
		state->streams[i].field_00 = 0;
	}
	state->descriptors_changed = true;
	memset(state->current, 0, sizeof(state->current));
	memset(state->wanted, 0, sizeof(state->wanted));
}

// @retail 0x1c590
void function_1c590(s_shader_cache *state, long tag, long index)
{
	long count;
	dword const *program = function_1c2e0(tag, index, &count);
	state->active = 0;
	if (state->bindings[0].tag != tag || state->bindings[0].index != index)
	{
		state->bindings[0].field_00 = 0;
		state->bindings[0].tag = tag;
		state->bindings[0].index = index;
		state->bindings[0].field_0c = 0;
		D3DDevice_LoadVertexShaderProgram(program, 0);
		state->field_84 = 0;
		state->field_88 = true;
		memset(state->current, 0, sizeof(state->current));
		state->descriptors_changed = true;
		for (long i = 0; i < 16; i++)
		{
			state->streams[i].field_08 = 0;
			state->streams[i].field_04 = 0;
			state->streams[i].field_00 = 0;
		}
		state->streams_changed = true;
		state->stream_count = 0;
	}
}

// @retail 0x1c620
void function_1c620(s_shader_cache *state, long a, long b, long c, byte const *descriptor)
{
	long index = state->stream_count;
	if (state->streams[index].field_04 != b || state->streams[index].field_00 != a || state->streams[index].field_08 != c)
	{
		state->streams[index].field_08 = c;
		state->streams[index].field_04 = b;
		state->streams[index].field_00 = a;
		state->streams_changed = true;
	}
	state->wanted[index] = descriptor;
	if (state->current[index] != descriptor)
		state->descriptors_changed = true;
	state->stream_count++;
}

byte g_485af1;

extern byte g_51f0f0[0x2d8];
extern byte *g_485a80;

struct s_shader_slot
{
	dword unknown00;
	long tag;
};

// @retail 0x1cc30
dword function_1cc30(long index)
{
	s_shader_slot *slot = &(*(s_shader_slot **)(g_485a80 + 0x5c))[index];
	function_1c590((s_shader_cache *)g_51f0f0, slot->tag, 0);
	return function_1cb70(slot->tag, 0);
}

// @retail 0x1c7f0
void function_1c7f0(long index)
{
	/* The binding index occupies a stack slot in retail. */
	long const *reference = &index;
	s_shader_binding *binding = &((s_shader_cache *)g_51f0f0)->bindings[*reference & 0xffff];
	binding->field_0c = NONE;
	binding->tag = NONE;
	binding->index = NONE;
}

// @retail 0x1e8c0
long function_1e8c0(char const *key)
{
	/* This key is passed on the stack by the cache callback interface. */
	char const *const *reference = &key;
	return (*reference)[3] * 59 + (*reference)[2] * 53 + (*reference)[1] * 43 + (*reference)[0] * 17;
}

struct s_cache_key
{
	word key;
	word flags;
};

// @retail 0x1e8f0
long function_1e8f0(s_cache_key const *a, s_cache_key const *b)
{
	/* Both pointers are stack arguments in the cache callback interface. */
	s_cache_key const *const *local_6e666f = &a;
	s_cache_key const *const *right_reference = &b;
	word right_flags = (*right_reference)->flags;
	word left_flags = (*local_6e666f)->flags;
	if ((bool)(right_flags & 1) == (bool)(left_flags & 1) && (*local_6e666f)->key == (*right_reference)->key &&
		(short)((left_flags ^ right_flags) & ~1) == 0)
		return 1;
	return 0;
}

struct s_cache_record
{
	long count;
	byte unknown04[8];
	byte active;
	byte unknown0d[0x63];
};

struct s_cache_record_state
{
	long count;
	dword unknown04;
	s_cache_record *records;
	void *buffer;
	byte unknown10;
	bool available;
	byte unknown12[2];
};

s_cache_record_state g_4b6280;

struct hash_table;
extern hash_table *g_51f400;

struct s_cache_hash_view
{
	byte unknown00[0x34];
	c_data_allocator *allocator;
};

// @retail 0x1e310
void function_1e310(void)
{
	if (g_4b6280.records)
	{
		if (!VirtualFree(g_4b6280.records, 0, MEM_RELEASE)) GetLastError();
		if (!VirtualFree(g_4b6280.buffer, 0, MEM_RELEASE)) GetLastError();
	}
	((s_cache_hash_view *)g_51f400)->allocator->deallocate(g_51f400);
	g_51f400 = 0;
}

// @retail 0x1e2d0
s_cache_record *function_1e2d0(void)
{
	s_cache_record *result = 0;
	if (g_4b6280.count < 1024)
	{
		result = &g_4b6280.records[g_4b6280.count];
		result->count = 0;
		g_4b6280.count++;
		result->active = 0;
	}
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}

// @retail 0x1cdd0
void function_1cdd0(real const *bounds, byte flags)
{
	/* The reference retains the flag argument's retail stack placement. */
	byte const *flags_reference = &flags;
	real constants[12];
	constants[0] = 1.0f;
	constants[1] = 1.0f;
	constants[2] = 1.0f;
	constants[3] = 1.0f;
	constants[4] = 0.0f;
	constants[5] = 0.0f;
	constants[6] = 0.0f;
	constants[7] = 0.0f;
	constants[8] = 1.0f;
	constants[9] = 1.0f;
	constants[10] = 0.0f;
	constants[11] = 0.0f;
	bool active = false;
	if ((*flags_reference & 3) && bounds)
	{
		active = true;
		if (*flags_reference & 1)
		{
			constants[0] = (bounds[1] - bounds[0]) * 0.5f;
			constants[1] = (bounds[3] - bounds[2]) * 0.5f;
			constants[2] = (bounds[5] - bounds[4]) * 0.5f;
			constants[3] = 1.0f;
			constants[4] = (bounds[1] + bounds[0]) * 0.5f;
			constants[5] = (bounds[2] + bounds[3]) * 0.5f;
			constants[6] = (bounds[4] + bounds[5]) * 0.5f;
			constants[7] = 0.0f;
		}
		if (*flags_reference & 2)
		{
			constants[8] = (bounds[7] - bounds[6]) * 0.5f;
			constants[9] = (bounds[9] - bounds[8]) * 0.5f;
			constants[10] = (bounds[6] + bounds[7]) * 0.5f;
			constants[11] = (bounds[9] + bounds[8]) * 0.5f;
		}
	}
	if (active || g_485af1)
	{
		D3DDevice_SetVertexShaderConstantFast(74, constants, 3);
		g_485af1 = active;
	}
}
