// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_020560.CPP: the timed effect values, the table of visibility
   slots, a pixel shader and the render state wrappers */

#include "unknown_11c920.h"
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "globals.h"
#include "unknown_0259d0.h"
#include "timed_effect.h"
#include "visibility_slot.h"
#include "unknown_123b30.h"
#include "crc.h"
#include "data_array.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

extern bool g_4ba019;
bool function_143c0(long tag, short index, short stage, real priority);

PRIVATE __forceinline long texture_floor(real value)
{
    long result = (long)value;
    if (value < 0.0f && value != (real)result)
        result--;
    return result;
}

// @retail 0x1b0a0
void __stdcall function_1b0a0(byte *state)
{
    (void)&state;
    dword remaining = *(dword *)(state + 0x1420);
    while (remaining)
    {
        long index;
        __asm
        {
            bsf ecx, remaining
            mov index, ecx
        }
        byte *entry = state + 0x1320 + index * 0x40;
        long tag = *(long *)entry;
        if (tag != NONE)
        {
            long bitmap = 0;
            real time = *(real *)(entry + 0x18);
            if (fabs(time) >= 0.0001f)
            {
                long count = *(long *)(g_4e3b44[tag & 0xffff].bytes + 0x44);
                bitmap = texture_floor((real)fmod(time, count));
            }
            real bias = 0.0f;
            if (!(entry[4] & 1) && !g_4ba019)
                bias = PIN(*(real *)(entry + 0x14) + *(real *)(state + 0x2c) - 0.5f, 0.0f, 4.0f);
            real level = (real)texture_floor(bias);
            level = PIN(level, 0.0f, 2.0f);
            level = (real)(long)level;
            if (*(real *)(entry + 0x10) > level)
            {
                function_143c0(tag, (short)bitmap, (short)index, bias);
                *(real *)(entry + 0x10) = level;
            }
        }
        remaining &= ~(1 << index);
    }
}

struct s_slot_key
{
	byte a;
	long b;
	long c;
	long d;
	long e;
};

struct s_tag_flags
{
	byte unknown00[0x28];
	unsigned short flag0 : 1;
	unsigned short flag1 : 1;
	unsigned short flag2 : 1;
	unsigned short flag3 : 1;
	unsigned short flag4 : 1;
	unsigned short flag5 : 1;
};

dword __cdecl pack_color3f(const color3f *color);

struct hash_table;
void hash_table_initialize(hash_table *table);
void function_20e50(long index);


byte g_51f408;
byte g_51f409;
hash_table *g_51f400;
long g_5234c4;
long g_5234b4[4];
extern __int64 g_485aa0;
byte g_485607;
dword g_4850c8;
real g_5234c8;
word g_485648;
word g_48564a;
word g_48564c;
word g_48564e;
dword g_4b81fc[0xa6];
D3DPIXELSHADERDEF g_484f68;
long g_5234b0;
extern long g_485898;
short g_467010 = NONE;

void function_0222d0(D3DRENDERSTATETYPE state, dword value);

struct s_packed_render_states
{
	byte field_00[0x14];
	byte *data;
};

struct s_render_state_source
{
	byte field_00[0x20];
	s_packed_render_states *states;
};

// @retail 0x18900
void __stdcall function_18900(s_render_state_source const *source, word const *range)
{
	byte const *entry = source->states->data + (*range & 0x1ff) * 5;
	for (long i = 0; i < (*range >> 9); entry += 5, i++)
		D3DDevice_SetRenderStateNotInline((D3DRENDERSTATETYPE)entry[0], *(dword const *)(entry + 1));
}

struct s_render_reset_state
{
    long index;
    byte unknown04[8];
    long fields[5];
    s_packed_render_states *states;
    byte *block;
    byte unknown28[0x40 - 0x28];
    long index40;
    byte unknown44[0x1320 - 0x44];
    struct { long index; byte unknown04[12]; real value; byte unknown14[0x2c]; } slots[4];
};

// @retail 0x16b10
void function_16b10(s_render_reset_state *state)
{
    if (state->block)
        function_18900((s_render_state_source const *)state, (word const *)(state->block + 0x11c));
    state->fields[0] = 0;
    state->fields[1] = 0;
    state->fields[2] = 0;
    state->fields[3] = 0;
    state->fields[4] = 0;
    state->states = 0;
    state->block = 0;
    state->index = NONE;
    state->index40 = NONE;
    for (long i = 0; i < 4; i++)
    {
        state->slots[i].index = NONE;
        state->slots[i].value = 3.0f;
    }
}

extern byte *g_485a80;

// @retail 0x16f60
void function_16f60(byte *state, word const *range)
{
    (void)&range;
    long i = 0;
    byte *values = *(byte **)(*(byte **)(state + 0xc) + 8);
    byte *entry = *(byte **)(*(byte **)(state + 0x10) + 0x24) + (*range & 0x1ff) * 4;
    for (; i < (*range >> 9); entry += 4, ++i)
    {
        byte *slot = state + 0x320 + entry[0] * 0x40;
        long *source = (long *)(values + entry[3] * 12);
        if (source[0] != NONE)
        {
            *(long *)slot = source[0];
            *(real *)(slot + 0x18) = (real)source[1];
            *(long *)(slot + 0x14) = source[2];
        }
        else
        {
            *(long *)slot = *(long *)(g_485a80 + 0x64);
            *(real *)(slot + 0x18) = 0.0f;
            *(real *)(slot + 0x14) = 0.0f;
        }
        *(real *)(slot + 0x10) = 3.0f;
    }
}

// @retail 0x15370
void function_15370(short mode)
{
	switch (mode)
	{
	case 0:
		function_0222d0(D3DRS_STENCILENABLE, FALSE);
		break;
	case 1: case 2:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		function_0222d0(D3DRS_STENCILREF, mode == 1 ? 0 : 255);
		function_0222d0(D3DRS_STENCILMASK, 255);
		function_0222d0(D3DRS_STENCILWRITEMASK, 255);
		break;
	case 3:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		break;
	case 4:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		break;
	case 5:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 2);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 2);
		break;
	case 6:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 3);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		break;
	}
	g_467010 = mode;
}

// @retail 0x15680
void function_15680(short mode)
{
	switch (mode)
	{
	case 0:
		function_0222d0(D3DRS_STIPPLEENABLE, FALSE);
		function_0222d0(D3DRS_SAMPLEALPHA, 0);
		break;
	case 1:
		function_0222d0(D3DRS_STIPPLEENABLE, TRUE);
		function_0222d0(D3DRS_SAMPLEALPHA, 0);
		break;
	case 2:
		function_0222d0(D3DRS_STIPPLEENABLE, FALSE);
		function_0222d0(D3DRS_SAMPLEALPHA, 0x110);
		break;
	}
}

struct s_5093e4
{
	bool flag0;
	bool flag1;
	byte unknown02[2];
	real value4;
	real value8;
	real valuec;
	real value10;
	real value14;
	bool flag18;
	bool flag19;
	bool flag1a;
	byte unknown1b;
	real value1c;
	real value20;
};

extern s_5093e4 *g_5093e4;

// @retail 0x20eb0
void function_020eb0(void)
{
	long size = sizeof(s_5093e4);
	byte *memory = game_state_globals.base_address + game_state_globals.cpu_allocation_size;
	game_state_globals.cpu_allocation_size += size;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
	s_5093e4 *state = (s_5093e4 *)memory;
	g_5093e4 = state;
	state->flag0 = false;
	state->flag1 = false;
	state->value4 = -1.0f;
	state->value8 = -1.0f;
	state->valuec = -1.0f;
	state->value10 = -1.0f;
	state->value14 = -1.0f;
	state->flag18 = true;
	state->flag19 = false;
	state->value1c = -1.0f;
	state->value20 = -1.0f;
	state->flag1a = false;
}

// @retail 0x206a0
void function_0206a0(void)
{
	long frame = g_485aa0;
	if (frame >= 0 && (((g_485898 >= 4 && g_485898 <= 7) && frame == g_5234b0) || frame > g_5234b0 || !g_5234b0))
	{
		for (long i = 0; i < 511; i++)
			g_51f40c.slots[i].valid = false;
		g_5234b0 = frame;
		g_51f408 = true;
		long special_mode = 1;
		if (g_485898 < 4 || g_485898 > 7)
			special_mode = 0;
		g_51f409 = !(byte)special_mode;
	}
	else
		g_51f408 = false;
	g_5234c4 = NONE;
}

// @retail 0x20560
real function_020560(long index)
{
	real result = 0.0f;

	if (g_5093e0 && index >= 0 && index < 32)
	{
		real fraction = 0.0f;

		if (g_5093e0->times[index][1] != g_5093e0->times[index][0])
		{
			fraction = (real)PIN((g_4858a0 - g_5093e0->times[index][0]) / (g_5093e0->times[index][1] - g_5093e0->times[index][0]), 0.0, 1.0);
		}

		result = (1.0f - fraction) * g_5093e0->values[index][0] + g_5093e0->values[index][1] * fraction;
		result = PIN(result, 0.0f, 1.0f);
	}

	return result;
}

// @retail 0x20720
void function_020720(long player)
{
	if (player == NONE)
		return;
	if (g_51f408 && !g_4850c8)
		g_5234c4 = player;
	else
	{
		for (long index = 0; index < g_51f40c.count; index++)
			function_20e50(index);
	}
}

// @retail 0x20770
void function_020770(void)
{
	if (g_51f408 && g_51f409 && g_5234c4 != NONE)
	{
		long frame = g_485aa0;
		long player = g_5234c4;

		if (frame > g_5234b4[player])
		{
			long set = (frame + 1) % 3;

			for (long i = 0; i < g_51f40c.count; i++)
			{
				s_slot *slot = &g_51f40c.slots[i];

				if (player == slot->b)
				{
					if (slot->valid)
					{
						long count = 0;
						if ((1 << (i & 0x1f)) & g_51f40c.bitsets[set][i >> 5])
						{
							ULONGLONG timestamp;

							if (D3DDevice_GetVisibilityTestResult(i * 3 + set, (UINT *)&count, &timestamp) == S_OK)
							{
								if (slot->k > 0)
								{
									real ratio = (real)count / (real)slot->k;
									ratio = PIN(ratio, 0.0f, 1.0f);
									ratio = ratio * 256.0f;
									dword target = (dword)PIN(ratio, 0.0f, 255.0f);

									if (((1 << slot->c) & 0xf) || slot->c == 4)
									{
										bool slow = false;
										bool fast = false;
										dword current;

										if (slot->size >= 24 && *(long *)&slot->data[0] != NONE)
										{
											s_tag_flags *flags = g_4e3b44[*(long *)&slot->data[0] & 0xffff].flags;
											slow = TEST_FIELD_BIT(flags->flag4);
											fast = TEST_FIELD_BIT(flags->flag5);
										}

										current = slot->f;
										if (current > target)
										{
											if (fast)
												slot->f = (target * 15 + current) >> 4;
											else
												slot->f = (current + target) >> 1;
										}
										else if (current < target)
										{
											if (slow)
												slot->f = (current + target) >> 1;
											else
												slot->f = (current * 7 + target) >> 3;
										}
									}
									else
									{
										slot->f = target;
									}

									if (count > 0)
										slot->f = (slot->f < 1) ? 1 : slot->f;
								}
								else
								{
									slot->f = 0;
								}
							}
						}
					}
					else
					{
						function_20e50(i);
					}
				}
			}

			g_5234b4[player] = frame;
		}
	}
}

// @retail 0x209b0
long function_0209b0(s_slot_key *key, const void *data, long size)
{
	if (!g_51f408 || g_5234c4 == NONE || g_485607 || g_4850c8)
		return NONE;

	long index;
	s_slot *slot;

	for (index = 0; index < g_51f40c.count; index++)
	{
		slot = &g_51f40c.slots[index];
		if (slot->used && key->a == slot->a && key->b == slot->b && key->c == slot->c && key->d == slot->d && key->e == slot->e)
			goto found;
	}

	for (index = 0; index < 511; index++)
	{
		slot = &g_51f40c.slots[index];
		if (!slot->used)
			goto create;
	}
	return NONE;

create:
	function_20e50(index);
	slot->a = key->a != 0;
	slot->b = key->b;
	slot->c = key->c;
	slot->d = key->d;
	slot->e = key->e;
	if (index + 1 > g_51f40c.count)
		g_51f40c.count = index + 1;

found:
	slot = &g_51f40c.slots[index];
	if (!slot->valid)
	{
		slot->size = size;
		slot->used = 1;
		slot->valid = 1;
		if (size > 0)
			memcpy(slot->data, data, size);
	}

	return index;
}

// @retail 0x20b40
void function_020b40(long value, long index)
{
	if (g_51f408 && g_5234c4 != NONE)
	{
		long count = g_51f40c.count;

		if (PIN(index, 0, count - 1) == index)
		{
			if (PIN(value, 0, count - 1) == value || value == NONE)
			{
				s_slot *slot = &g_51f40c.slots[index];

				if (value != NONE)
					slot->j = value;
				else
					slot->j = NONE;
			}
		}
	}
}

// @retail 0x20d80
real function_020d80(s_slot *slot, bool skip)
{
	real result = 1.0f;

	if (slot->valid && slot->used)
	{
		result = slot->f * g_5234c8;
		result = PIN(result, 0.0f, 1.0f);

		if (!skip && slot->j != 0x1ff)
		{
			if (PIN(slot->j, 0, g_51f40c.count - 1) == slot->j)
			{
				s_slot *linked = &g_51f40c.slots[slot->j];

				if (linked->j == 0x1ff && linked->b == slot->b && linked->valid && linked->used)
				{
					real linked_result = linked->f * g_5234c8;
					linked_result = PIN(linked_result, 0.0f, 1.0f);
					result = linked_result * result;
				}
			}
		}
	}

	return result;
}

// @retail 0x20f30
void function_020f30(real a, real b)
{
    dword count;
    dword volatile full;
    dword half = 0xc00;

    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x8421;

    b = PIN(b, 0.0f, 1.0f);
    a = PIN(a, 0.0f, 1.0f);

    if (a > 0.0f)
    {
        real x = (a + 1.0f) * 0.5f;
        x = PIN(x, 0.0f, 1.0f);
        real scale = 255.0f;
        long v = 0;
        __asm
        {
            fld x
            fld scale
            fmulp st(1), st
            fistp v
            shl v, 24
        }
        g_484f68.PSConstant1[2] = v;
        full = 0x30c00;
        g_484f68.PSAlphaInputs[2] = 0x1c121d12;
        g_484f68.PSAlphaOutputs[2] = half;
    }
    else
    {
        full = 0x30c00;
        g_484f68.PSAlphaInputs[2] = 0x1c201d20;
        g_484f68.PSAlphaOutputs[2] = full;
    }

    g_484f68.PSAlphaInputs[0] = 0x18201920;
    g_484f68.PSAlphaOutputs[0] = full;
    g_484f68.PSAlphaInputs[1] = 0x1a201b20;
    g_484f68.PSAlphaOutputs[1] = 0x30d00;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;

    count = 3;

    if (b > 0.0f)
    {
        color3f color;
        color.red = b;
        color.green = b;
        color.blue = b;
        dword pixel = pack_color3f(&color);
        g_484f68.PSConstant0[1] = pixel;
        g_484f68.PSConstant0[4] = pixel;
        g_484f68.PSConstant0[7] = pixel;
        g_484f68.PSRGBInputs[0] = 0x8200940;
        g_484f68.PSRGBOutputs[0] = 0x20c00;
        g_484f68.PSRGBInputs[1] = 0xcc012020;
        g_484f68.PSRGBOutputs[1] = full;
        g_484f68.PSRGBInputs[2] = 0xc082c09;
        g_484f68.PSRGBOutputs[2] = half;
        g_484f68.PSRGBInputs[3] = 0xa200b40;
        g_484f68.PSRGBOutputs[3] = 0x20d00;
        g_484f68.PSRGBInputs[4] = 0xcd012020;
        g_484f68.PSRGBOutputs[4] = 0x30d00;
        g_484f68.PSRGBInputs[5] = 0xd0a2d0b;
        g_484f68.PSRGBOutputs[5] = 0xd00;
        g_484f68.PSRGBInputs[6] = 0xc200d40;
        g_484f68.PSRGBOutputs[6] = 0x20400;
        g_484f68.PSRGBInputs[7] = 0xc4012020;
        g_484f68.PSRGBOutputs[7] = 0x30400;
        g_484f68.PSFinalCombinerInputsABCD = 0x40c0d00;
        count = 8;
    }
    else
    {
        g_484f68.PSRGBInputs[0] = 0x8200920;
        g_484f68.PSRGBOutputs[0] = full;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[2] = full;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
    }

    g_484f68.PSCombinerCount = count | 0x11000;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
}

// @retail 0x222d0
void function_0222d0(D3DRENDERSTATETYPE state, dword value)
{
	g_4b81fc[state] = value;
	D3DDevice_SetRenderState(state, value);
}

// @retail 0x224f0
void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value)
{
	D3DDevice_SetTextureStageState(stage, type, value);
}

// @retail 0x226b0
void function_0226b0(void)
{
	if (g_51f400)
		hash_table_initialize(g_51f400);
}

struct s_0226d0_block
{
	long field_00;
	long checksum;
	byte field_08[0x14];
	long handle;
};

struct s_0226d0_structure
{
	byte field_00[8];
	long checksum;
};

// @retail 0x226d0
bool function_0226d0(void)
{
	bool result = false;
	if (g_4e0344 && g_4e0344->count > 0 && g_4e0348)
	{
		s_0226d0_block *block = (s_0226d0_block *)g_4e0344->bsp;
		if (block->handle != NONE && block->checksum == ((s_0226d0_structure *)g_4e0348)->checksum)
			result = true;
	}
	return result;
}

// @retail 0x22710
bool function_022710(void)
{
	bool result = false;

	if (g_48564a == 0 && g_485648 == 0 && g_48564e == 0x280 && g_48564c == 0x1e0 && g_4ba04c <= 1)
		result = true;

	return result;
}

// @retail 0x22750
void function_022750(const real *a, long size, const real *b, real *out, real t)
{
	long count = size * size;

	for (long i = 0; i < count; i++)
		out[i] = (a[i] - b[i]) * t + b[i];
}

// @retail 0x22c30
real function_022c30(const point3f *a, const point3f *c, const point3f *b)
{
	vector3f v1, v2;
	real result = 0.0f;

	v2.i = a->x - c->x;
	v2.j = a->y - c->y;
	v2.k = a->z - c->z;
	v1.i = a->x - b->x;
	v1.j = a->y - b->y;
	v1.k = a->z - b->z;

	real length = (real)sqrt(v1.k * v1.k + v1.j * v1.j + v1.i * v1.i);
	if (length > 0.0001f)
	{
		real inverse = 1.0f / length;
		v1.i *= inverse;
		v1.j *= inverse;
		v1.k *= inverse;
		result = (v1.k * v2.k + v1.j * v2.j + v1.i * v2.i) * inverse;
	}

	return result;
}


struct s_light_sample
{
    point3f position;
    real red[9], green[9], blue[9];
    byte unknown78[0x2c];
    vector3f direction;
    byte unknownb0[0x2c];
};

struct s_light_sample_source
{
    long unknown00;
    s_light_sample *entries;
};

bool g_5093f4;
real g_4b8940[9], g_4b8964[9], g_4b8988[9];
bool spherical_harmonics_evaluate_directional_light(vector3f const *direction, dword order, real red, real green, real blue,
    real *red_result, real *green_result, real *blue_result);

PRIVATE __forceinline void copy_light_sample(s_light_sample const *sample, byte order, real *red, real *green, real *blue, vector3f *direction)
{
    long bytes = order * order * sizeof(real);
    memcpy(red, sample->red, bytes);
    memcpy(green, sample->green, bytes);
    memcpy(blue, sample->blue, bytes);
    *direction = sample->direction;
}

// @retail 0x22cf0
void __stdcall function_22cf0(s_light_sample_source const *source, real *red, real *green, real *blue, byte order,
    vector3f *direction, point3f const *position)
{
    long count = *(long *)((byte *)g_4e0344->bsp + 0x38);
    if (count > 0)
    {
        if (g_5093f4)
        {
            memcpy(red, g_4b8940, sizeof(g_4b8940));
            memcpy(green, g_4b8964, sizeof(g_4b8964));
            memcpy(blue, g_4b8988, sizeof(g_4b8988));
            return;
        }
        if (count == 1)
        {
            copy_light_sample(source->entries, order, red, green, blue, direction);
            return;
        }
        long nearest = 0;
        real closest_distance = 1048576.0f;
        for (long i = 0; i < count; ++i)
        {
            real x = position->x - source->entries[i].position.x;
            real y = position->y - source->entries[i].position.y;
            real z = position->z - source->entries[i].position.z;
            real distance = z * z + x * x + y * y;
            if (distance < closest_distance)
            {
                closest_distance = distance;
                nearest = i;
            }
        }
        s_light_sample const *first = source->entries + nearest;
        vector3f delta;
        delta.i = position->x - first->position.x;
        delta.j = position->y - first->position.y;
        delta.k = position->z - first->position.z;
        long opposite = NONE;
        real other_distance = 1048576.0f;
        for (long j = 0; j < count; ++j)
        {
            if (j != nearest)
            {
                real x = position->x - source->entries[j].position.x;
                real y = position->y - source->entries[j].position.y;
                real z = position->z - source->entries[j].position.z;
                real projection = delta.k * z + delta.j * y + delta.i * x;
                if (projection <= 0.0f)
                {
                    real distance = z * z + y * y + x * x;
                    if (distance < other_distance)
                    {
                        other_distance = distance;
                        opposite = j;
                    }
                }
            }
        }
        real fraction = opposite != NONE
            ? function_022c30(&first->position, position, &source->entries[opposite].position) : 0.0f;
        if (fraction <= 0.0f || opposite == NONE)
            copy_light_sample(first, order, red, green, blue, direction);
        else
        {
            s_light_sample const *second = source->entries + opposite;
            if (fraction >= 1.0f)
                copy_light_sample(second, order, red, green, blue, direction);
            else
            {
                function_022750(second->red, order, first->red, red, fraction);
                function_022750(second->green, order, first->green, green, fraction);
                function_022750(second->blue, order, first->blue, blue, fraction);
                direction->i = (second->direction.i - first->direction.i) * fraction + first->direction.i;
                direction->j = (second->direction.j - first->direction.j) * fraction + first->direction.j;
                direction->k = (second->direction.k - first->direction.k) * fraction + first->direction.k;
            }
        }
    }
    else
    {
        vector3f fallback = { 0.0f, 0.0f, -1.0f };
        spherical_harmonics_evaluate_directional_light(&fallback, order, 0.0f, 1.0f, 0.0f, red, green, blue);
    }
}

real g_4b8494;
long g_467130;
long g_4858b4;
extern D3DPalette *g_484dbc;
extern long g_484dc0[4];
dword *function_1c290(real value);
void function_0496a0(void);
void function_049740(void);

// @retail 0x445d0
void function_445d0(void)
{
	if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
	{
		D3DDevice_SetStipple(function_1c290(1.0f));
		g_4b8494 = 1.0f;
	}
	switch (g_467130)
	{
	case 3:
		D3DDevice_SetPalette(0, g_484dbc);
		g_484dc0[0] = NONE;
		break;
	case 10:
		function_0496a0();
		break;
	case 11:
		function_049740();
		break;
	case 16: break;
	case 17: break;
	}
	g_467130 = 0;
	g_4858b4 = NONE;
}

long g_4b6298;
bool g_4b6294;

__declspec(noinline) void function_1ef70(void);

// @retail 0x1ee60
void function_1ee60(long primitive)
{
	if (g_4b6298 != primitive)
	{
		function_1ef70();
		D3DDevice_SetRenderState(D3DRS_LINEWIDTH, 0x3f800000);
		switch (primitive)
		{
		case 1: D3DDevice::Begin(D3DPT_LINELIST); break;
		case 2: D3DDevice::Begin(D3DPT_LINESTRIP); break;
		case 3: D3DDevice::Begin(D3DPT_TRIANGLELIST); break;
		case 4: D3DDevice::Begin(D3DPT_QUADLIST); break;
		default: __assume(0);
		}
		g_4b6298 = primitive;
	}
}

// @retail 0x1ee50
void function_1ee50(void)
{
	function_1ef70();
	g_4b6294 = false;
}

// @retail 0x1ef70
void function_1ef70(void)
{
	if (g_4b6298)
	{
		real size = 1.0f;
		D3DDevice::End();
		D3DDevice_SetRenderState(D3DRS_LINEWIDTH, *(dword *)&size);
		g_4b6298 = 0;
	}
}

struct s_189a0_stage
{
	byte filters;
	byte mip_filters;
	byte color_flags;
	byte mip_limits;
	byte address_u, address_v, address_w, unknown07;
	dword border_color;
	dword key_color;
	dword color_sign;
	dword lod_bias;
};

// @retail 0x189a0
void function_189a0(byte const *state, word const *range)
{
	word const *const *range_reference = &range;
	byte *definition = *(byte **)(state + 0x20);
	s_189a0_stage *entry = *(s_189a0_stage **)(definition + 0x1c) + (**range_reference & 0x1ff);
	for (long stage = 0; stage < (**range_reference >> 9); ++stage)
	{
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, entry->address_u);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, entry->address_v);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, entry->address_w);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, (dword)entry->mip_filters & 15);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, entry->filters >> 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, entry->mip_filters >> 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, entry->lod_bias);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, entry->mip_limits >> 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, (dword)entry->mip_limits & 15);
		D3DDevice_SetTextureStageState(stage, D3DTSS_COLORKEYOP, entry->color_flags >> 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, entry->color_sign);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, (dword)entry->color_flags & 15);
		D3DDevice_SetTextureStageState(stage, D3DTSS_BORDERCOLOR, entry->border_color);
		D3DDevice_SetTextureStageState(stage, D3DTSS_COLORKEYCOLOR, entry->key_color);
		++entry;
	}
}

// @retail 0x39780
bool function_39780(bool alternate)
{
	D3DPIXELSHADERDEF definition;
	memset(&definition, 0, sizeof(definition));
	definition.PSCombinerCount = 0x11101;
	definition.PSTextureModes = (!alternate) << 15;
	definition.PSInputTexture = 0;
	definition.PSDotMapping = 0;
	definition.PSCompareMode = 0;
	definition.PSConstant1[0] = 0;
	definition.PSConstant0[0] = 0;
	definition.PSAlphaOutputs[0] = 0xc0;
	definition.PSRGBOutputs[0] = 0xc0;
	if (!alternate)
	{
		definition.PSRGBInputs[0] = 0xc4cb0000;
		definition.PSAlphaInputs[0] = 0xd4db1010;
	}
	else
	{
		definition.PSRGBInputs[0] = 0xc4200000;
		definition.PSAlphaInputs[0] = 0xd4301010;
	}
	definition.PSFinalCombinerConstant0 = 0;
	definition.PSFinalCombinerConstant1 = 0;
	definition.PSC0Mapping = 0xffffffff;
	definition.PSC1Mapping = 0xffffffff;
	definition.PSFinalCombinerInputsABCD = 0x0c200000;
	definition.PSFinalCombinerInputsEFG = 0x1c80;
	definition.PSFinalCombinerConstants = 0x1ff;
	D3DDevice_SetPixelShaderProgram(&definition);
	return true;
}

dword __cdecl pack_color4f(color4f const *color);

struct s_1ed70_point
{
    short x, y;
};

// @retail 0x1ed70
void function_1ed70(color4f const *color, s_1ed70_point const *points, volatile short count)
{
    dword packed = pack_color4f(color);
    function_1ee60(2);
    D3DDevice_SetVertexDataColor(9, packed);
    for (short i = 0; i < count; ++i)
        D3DDevice_SetVertexData2s(0, points[i].x, points[i].y);
    function_1ef70();
}

// @retail 0x1ba00
void function_1ba00(byte *state)
{
    byte *definition = *(byte **)(state + 0x20);
    byte *blocks = *(byte **)(definition + 0x2c);
    dword *values = *(dword **)(definition + 0x34);
    byte *selection = *(byte **)(state + 0x24);
    *(dword *)(state + 0x14fc) = *(dword *)(selection + 0xde);
    *(dword *)(state + 0x1504) = *(dword *)(selection + 0xe6);
    *(dword *)(state + 0x1500) = *(dword *)(selection + 0xe2);
    unsigned long output = 0;
    bool final = false;
    long i = 0;
    if ((*(word *)(*(byte **)(state + 0x24) + 0xf6) >> 9) > 0)
    do
    {
        word range = *(word *)(*(byte **)(state + 0x24) + 0xf6);
        short *entry = *(short **)(*(byte **)(state + 0x20) + 0x24) + ((range & 0x1ff) + i) * 2;
        state[0x1520 + i] = (byte)output;
        long block_index = (word)entry[1] & 0x1ff;
        byte *block;
        if (!(entry[0] == NONE))
            block = blocks + (block_index + *(long *)(state + 0x300 + entry[0] * 4)) * 6;
        else
            block = blocks + block_index * 6;
        dword *value = values + (*(word *)(block + 4) & 0x1ff) * 8;
        for (long j = 0; j < (*(word *)(block + 4) >> 9); ++j, value += 8)
        {
            if ((block[2] & 1) && j + 1 == (*(word *)(block + 4) >> 9))
            {
                *(dword *)(state + 0x1444) = value[0];
                *(dword *)(state + 0x1448) = value[2];
                *(dword *)(state + 0x14d0) = value[4];
                *(dword *)(state + 0x14d4) = value[5];
                final = true;
                break;
            }
            *(dword *)(state + 0x14ac + output * 4) = value[0];
            *(dword *)(state + 0x14d8 + output * 4) = value[1];
            *(dword *)(state + 0x1424 + output * 4) = value[2];
            *(dword *)(state + 0x148c + output * 4) = value[3];
            *(dword *)(state + 0x144c + output * 4) = value[4];
            *(dword *)(state + 0x146c + output * 4) = value[5];
            ++output;
        }
        ++i;
    } while (i < (*(word *)(*(byte **)(state + 0x24) + 0xf6) >> 9));
    if (!final)
    {
        *(dword *)(state + 0x1444) = 0xc;
        *(dword *)(state + 0x1448) = 0x2000;
    }
    *(dword *)(state + 0x14f8) = (output < 1 ? 1 : output) | 0x11100;
}

struct s_shader_cache;
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);
extern byte g_51f0f0[0x2d8];

// @retail 0x246a0
bool __stdcall function_246a0(void *context)
{
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x1010101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_BORDER);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x1001;
    g_484f68.PSConstant0[0] = 0xff4c961c;
    g_484f68.PSRGBInputs[0] = 0x1080000;
    g_484f68.PSRGBOutputs[0] = 0x20c0;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0x2000;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x64), 0);
    function_1c710(g_51f0f0);
    return true;
}

typedef long (__stdcall *visibility_draw_callback)(s_slot *slot, byte *data);

// @retail 0x20bb0
void __stdcall function_20bb0(long player, dword mask, long const *indices, long count, bool query, visibility_draw_callback draw)
{
    long frame = (long)g_485aa0;
    if (g_51f408 && g_5234b0 == frame && g_5234c4 != NONE && g_5234c4 == player)
    {
        long set = frame % 3;
        if (!indices)
            count = g_51f40c.count;
        for (long i = 0; i < count; ++i)
        {
            long index = indices ? indices[i] : i;
            s_slot *slot = &g_51f40c.slots[index];
            if (slot->valid && player == slot->b && (mask & (1 << slot->c)))
            {
                if (query)
                {
                    long test = index * 3 + set;
                    HRESULT status = S_OK;
                    if (g_51f40c.bitsets[set][index >> 5] & (1 << (index & 31)))
                    {
                        UINT pixels;
                        ULONGLONG timestamp;
                        status = D3DDevice_GetVisibilityTestResult(test, &pixels, &timestamp);
                        if (status != S_OK)
                            continue;
                    }
                    long total = 0;
                    if (status == S_OK)
                    {
                        D3DDevice_BeginVisibilityTest();
                        total = draw(slot, slot->data);
                        status = D3DDevice_EndVisibilityTest(test);
                        total = PIN(total, 0, 65535);
                    }
                    slot->k = total;
                    if (status == S_OK)
                        g_51f40c.bitsets[set][index >> 5] |= 1 << (index & 31);
                }
                else
                    draw(slot, slot->data);
            }
        }
    }
}


extern long g_4b9ed4;
long __stdcall function_3bf20(s_slot *slot, byte *data);

// @retail 0x3c270
bool __stdcall function_3c270(dword value)
{
    s_slot_key key;
    memset(&key, 0, sizeof(key));
    long player = g_4b9ed4;
    key.a = 0;
    key.b = player;
    key.c = 5;
    key.d = 0;
    key.e = 0;
    long index = function_0209b0(&key, &value, 4);
    if (index == NONE) return false;
    byte &valid = *(byte *)&value;
    valid = false;
    bool available;
    real &amount = *(real *)&key.b;
    if (g_51f408 && g_51f40c.slots[index].valid)
    {
        long frame = ((long)g_485aa0 + 1) % 3;
        amount = g_51f40c.slots[index].f * g_5234c8;
        amount = PIN(amount, 0.0f, 1.0f);
        valid = true;
        available = (g_51f40c.bitsets[frame][index >> 5] & (1 << (index & 31))) == 0;
    }
    else
    {
        amount = 0.0f;
        available = false;
    }
    function_20bb0(player, 0x20, &index, 1, true, function_3bf20);
    if (valid && !available && !(amount > 0.0f)) return false;
    return true;
}

void function_015b70(void);
void function_1e1c0(void);
void function_36ab0(void);

// @retail 0x226a0
void function_226a0(void)
{
    function_015b70();
    function_1e1c0();
    function_36ab0();
}

extern long g_4858b8;
void function_14f60(short stage, short index);
bool function_1ccf0(D3DPIXELSHADERDEF const *program);
void __stdcall function_34a90(long target, short blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real distortion, real scale,
    long count, bool full_surface, bool viewport_textures);

// @retail 0x22070
void __stdcall function_22070(real strength, real exponent)
{
    if (strength > 0.0f)
    {
        strength = (real)PIN((real)pow((double)(strength <= 1.0f ? strength : 1.0f),
            (double)(!(exponent > 0.0f) ? 0.0f : exponent)), 0.0f, 1.0f);
        if (strength > 0.0f)
        {
            function_14f60(0, 18);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
            D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
            memset(&g_484f68, 0, sizeof(g_484f68));
            g_484f68.PSTextureModes = 1;
            g_484f68.PSCombinerCount = 0x11001;
            real alpha_scale = 255.0f;
            long alpha = 0;
            __asm
            {
                fld strength
                fld alpha_scale
                fmulp st(1), st(0)
                fistp alpha
            }
            g_484f68.PSFinalCombinerConstant0 = (dword)alpha << 24;
            g_484f68.PSFinalCombinerInputsABCD = 0x8110000;
            g_484f68.PSFinalCombinerInputsEFG = 0x1100;
            function_1ccf0(&g_484f68);
            function_34a90(g_4858b8, 7, 0x1010101, false, false,
                1.0f, 0.0f, 1.0f, 1, false, false);
        }
    }
}

typedef bool (__stdcall *t_211a0_vertex)(long, real const *, real const *, real *, long);
void function_34d90(long target, real const *weights, short blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real low, real high,
    bool full_surface, t_211a0_vertex vertex);
void __stdcall function_351a0(long texture, long target);

PRIVATE __forceinline void configure_filter_textures(long source)
{
    for (long stage = 0; stage < 4; ++stage)
    {
        function_14f60(stage, (short)source);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    }
}

// @retail 0x21360
long function_21360(long source, real radius, real low, real high, long target,
    long alternate, long passes, long unused, bool configure, bool full_surface, real scale)
{
    (void)&unused;
    radius *= 0.01666f;
    if (alternate == NONE)
        alternate = source;
    if (configure)
        function_020f30(0.0f, scale);
    for (long pass = 0; pass < passes; ++pass)
    {
        real weights[4];
        weights[0] = radius;
        weights[1] = radius * 0.33f;
        weights[2] = radius * -0.33f;
        weights[3] = 0.0f - radius;
        configure_filter_textures(source);
        function_34d90(target, weights, 10, 0x1010101, false, false,
            1.0f, low, high, full_surface, NULL);
        source = target;
        target = alternate;
        alternate = source;
        radius *= 2.0f;
    }
    if (alternate != NONE && alternate != source)
    {
        function_351a0(source, alternate);
        source = alternate;
    }
    return source;
}

// @retail 0x211a0
long function_211a0(long source, real passes, real distortion, real falloff,
    long target, long alternate, long unused, long configure, long full_surface,
    real scale, real offset)
{
    (void)&unused;
    real pass = 0.0f;
    if (alternate == NONE)
        alternate = source;
    if ((byte)configure)
        function_020f30(offset, scale);
    if (passes > 0.0f)
    {
        real growth = (1.0f - falloff) + falloff * 2.0f;
        do
        {
            real fraction = passes - pass;
            if (fraction < 0.0f) fraction = 0.0f;
            else if (fraction > 1.0f) fraction = 1.0f;
            if (fraction > 0.0f)
            {
                configure_filter_textures(source);
                function_34a90(target, 10, 0x1010101, false, false,
                    1.0f, fraction * distortion, 1.0f, 4, (bool)(byte)full_surface, false);
                source = target;
                target = alternate;
                alternate = source;
            }
            distortion *= growth;
            pass += 1.0f;
        } while (pass < passes);
    }
    if (alternate != NONE && alternate != source)
    {
        function_351a0(source, alternate);
        source = alternate;
    }
    return source;
}

// @retail 0x222a0
void function_222a0(void)
{
    function_211a0(10, 2.0f, 2.0f, 0.5f, 11, NONE, 10, 1, 1, 0.0f, 0.0f);
}

void function_15780(long stage, long mode);
dword __cdecl function_131fc0(real alpha);

PRIVATE __forceinline void configure_filter_copy(void)
{
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSFinalCombinerInputsABCD = 8;
    g_484f68.PSFinalCombinerInputsEFG = 0x1800;
    function_1ccf0(&g_484f68);
}

// @retail 0x21a80
void __stdcall function_21a80(real first, real second, real depth)
{
    if (first > 0.0f || second > 0.0f)
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSFinalCombinerInputsEFG = 0x2000;
        g_484f68.PSCombinerCount = 0x11001;
        D3DDevice_SetPixelShaderProgram(&g_484f68);
        function_34a90(18, 10, 0x1000000, true, false, 0.0f - depth,
            0.0f, 1.0f, 0, false, false);
        if (first > 0.0f)
        {
            function_14f60(0, 18);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
            D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
            D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
            configure_filter_copy();
            function_34a90(5, 10, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
            long filtered = function_211a0(5, first, 1.0f, 0.0f, 6, NONE, NONE, 1, 0, 0.0f, 0.0f);
            first = (real)PIN(pow((double)(first > 1.0f ? 1.0f : first), 0.33000001311302185), 0.0, 1.0);
            function_14f60(0, (short)filtered);
            function_15780(0, 4);
            if (first < 1.0f)
            {
                function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
                function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTALPHA);
                function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVCONSTANTALPHA);
                function_0222d0(D3DRS_BLENDCOLOR, function_131fc0(first));
                function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            }
            else function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
            configure_filter_copy();
            function_34a90(18, NONE, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
        }
        if (second > 0.0f)
        {
            function_14f60(0, (short)g_4858b8);
            function_15780(0, 4);
            configure_filter_copy();
            function_34a90(5, 10, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
            long filtered = function_211a0(5, second, 1.0f, 0.0f, 6, NONE, NONE, 1, 0, 0.0f, 0.0f);
            first = (real)PIN(pow((double)(second > 1.0f ? 1.0f : second), 0.33000001311302185), 0.0, 1.0);
            function_14f60(0, (short)filtered);
            function_15780(0, 4);
            if (first < 1.0f)
            {
                function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
                function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTALPHA);
                function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVCONSTANTALPHA);
                function_0222d0(D3DRS_BLENDCOLOR, function_131fc0(first));
                function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            }
            else function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
            configure_filter_copy();
            function_34a90(g_4858b8, NONE, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
        }
        function_14f60(0, 18);
        function_15780(0, 3);
        configure_filter_copy();
        function_34a90(0, 7, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
    }
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
bool g_485a74;
void function_14bc0(short index, short element, bool use_depth);

// @retail 0x214f0
void __stdcall function_214f0(real passes, real distortion, real strength, real falloff,
    real scale, bool blend, bool preserve)
{
    struct { real first, second, alpha_scale, alpha; color4f color; } values;
    values.first = 1.0f;
    values.second = 0.4f;
    if (g_510c50 && ((byte *)g_510c50)[5])
    {
        s_5093e4 *state = g_5093e4;
        if (state->flag1)
        {
            state->flag0 = false;
            state->flag1 = false;
            state->value4 = -1.0f;
            state->value8 = -1.0f;
            state->valuec = -1.0f;
            state->value10 = -1.0f;
            state->value14 = -1.0f;
            state->flag18 = true;
            state->flag19 = false;
            state->value1c = -1.0f;
            state->value20 = -1.0f;
            state->flag1a = false;
        }
        else if (state->flag0)
        {
            if (state->value4 != -1.0f) passes = state->value4;
            if (state->value8 != -1.0f) distortion = state->value8;
            if (state->valuec != -1.0f) strength = state->valuec;
            if (state->value10 != -1.0f) falloff = state->value10;
            if (state->value14 != -1.0f) scale = state->value14;
            blend = state->flag18;
            preserve = state->flag19;
            if (state->value1c != -1.0f) values.first = state->value1c;
            if (state->value20 != -1.0f) values.second = state->value20;
        }
    }
    passes = passes > 0.0f ? passes : 0.0f;
    distortion = PIN(distortion, 0.0f, 1.0f);
    strength = PIN(strength, 0.0f, 1.0f);
    falloff = PIN(falloff, 0.0f, 1.0f);
    scale = PIN(scale, 0.0f, 1.0f);
    volatile real first_input = values.first;
    if (first_input < 0.0f) *(volatile real *)&values.first = 0.0f;
    else if (first_input > 1.0f) *(volatile real *)&values.first = 1.0f;
    values.second = PIN(values.second, 0.0f, 1.0f);
    if (strength > 0.0f || values.first > 0.0f)
    {
        configure_filter_textures(g_4858b8);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSAlphaInputs[0] = 0x18201920;
        g_484f68.PSAlphaInputs[1] = 0x1a201b20;
        g_484f68.PSAlphaInputs[2] = 0x1c201d20;
        g_484f68.PSAlphaOutputs[0] = 0x30c00;
        g_484f68.PSAlphaOutputs[1] = 0x30d00;
        g_484f68.PSAlphaOutputs[2] = 0x30c00;
        g_484f68.PSRGBInputs[0] = 0x8200920;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[0] = 0x30c00;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBOutputs[2] = 0x30c00;
        values.color.alpha = 0.125f;
        values.color.red = distortion;
        values.color.green = distortion;
        values.color.blue = distortion;
        g_484f68.PSConstant0[3] = pack_color4f(&values.color);
        g_484f68.PSRGBInputs[3] = 0xc200140;
        g_484f68.PSRGBOutputs[3] = 0x20d00;
        g_484f68.PSAlphaInputs[3] = 0x1c110000;
        g_484f68.PSAlphaOutputs[3] = 0x10d00;
        // Retail clears the values.alpha conversion slot before rounding.
        values.alpha = 0.0f;
        values.alpha_scale = 255.0f;
        long &packed = *(long *)&distortion;
        packed = 0;
        __asm
        {
            fld values.alpha
            fld values.alpha_scale
            fmulp st(1), st(0)
            fistp distortion
        }
        g_484f68.PSFinalCombinerConstant1 = (dword)packed << 24;
        g_484f68.PSFinalCombinerInputsABCD = ((g_485a74 ? 0x1c : 0) << 24) | 0xf000d;
        g_484f68.PSFinalCombinerInputsEFG = ((g_485a74 ? 0x1d : 0) | 0xc1100) << 8;
        function_1ccf0(&g_484f68);
        function_34a90(7, 10, 0x1010101, false, false, 1.0f, 0.5f, 1.0f, 4, false, false);
        long (*volatile filter)(long, real, real, real, long, long, long, long, long, real, real) = function_211a0;
        long filtered = filter(7, passes, 1.0f, falloff, 8, NONE, NONE, 1, 0, scale, values.second);
        function_14f60(0, (short)filtered);
        function_15780(0, 4);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_SRCBLEND, blend ? 0x307 : 1);
        function_0222d0(D3DRS_DESTBLEND, !preserve);
        function_0222d0(D3DRS_BLENDOP, 0x8006);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        values.color.alpha = values.first;
        values.color.red = strength;
        values.color.green = strength;
        values.color.blue = strength;
        g_484f68.PSConstant0[0] = pack_color4f(&values.color);
        g_484f68.PSRGBOutputs[0] = 0x10c00;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        g_484f68.PSRGBInputs[0] = (g_485a74 ? 0x11 : 0) | 0x8011800;
        function_1ccf0(&g_484f68);
        function_34a90(g_4858b8, NONE, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
        function_14bc0((short)(g_4858b4 != NONE ? g_4858b4 : g_4858b8), 0, true);
    }
}
