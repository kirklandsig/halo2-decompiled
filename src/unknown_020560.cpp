// @flags /O2 /arch:SSE /Gr
#include "palette_storage.h"
#include "shader_storage.h"
#include "render_state_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_020560.CPP: the timed effect values, the table of visibility
   slots, a pixel shader and the render state wrappers */

#include "unknown_11c920.h"
#include "frame_storage.h"
#include "depth_parameters_storage.h"
#include <string.h>
#include <math.h>
#include <xtl.h>
#include "sampler_state.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "timed_effect.h"
#include "visibility_slot.h"
#include "unknown_123b30.h"
#include "crc.h"
#include "data_array.h"

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

namespace D3D
{
	struct CDevice
	{
		static dword m_PushSegmentSize;
	};

	dword WINAPI SetFence(dword flags);
	dword volatile *WINAPI MakeRequestedSpace(dword fallback_size, dword requested_size);
}

extern bool g_4ba019;
bool function_143c0(long tag, short index, short stage, real priority);

/* Rounds toward negative infinity: truncates, then subtracts one when the
   value was negative and not already whole. */
PRIVATE __forceinline long texture_floor(real value)
{
    __asm
    {
        movss xmm0, value
        cvttss2si eax, xmm0
        cvtsi2ss xmm1, eax
        cmpneqss xmm1, xmm0
        cmpltss xmm0, g_45dbd8
        andps xmm0, xmm1
        movmskps ecx, xmm0
        sub eax, ecx
    }
}

/* The mip level is an integer index: the floored bias pinned to 0..2 and truncated. Comparing
   and storing it as that index, converted where the float is needed, is what matches. */
// @retail 0x1b0a0
void __stdcall function_1b0a0(byte *state)
{
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
        if (*(long *)entry != NONE)
        {
            long bitmap = 0;
            real bias = 0.0f;
            if (!(fabs(*(real *)(entry + 0x18)) < 0.0001f))
            {
                long count = *(long *)(g_4e3b44[*(long *)entry & 0xffff].bytes + 0x44);
                bitmap = texture_floor((real)fmod(*(real *)(entry + 0x18), count));
            }
            if (!(entry[4] & 1))
            {
                real adjusted = 0.0f;
                if (!g_4ba019)
                    adjusted = PIN(*(real *)(entry + 0x14) + *(real *)(state + 0x2c) - 0.5f, 0.0f, 4.0f);
                bias = adjusted;
            }
            real level = (real)texture_floor(bias);
            long level_index = (long)PIN(level, 0.0f, 2.0f);
            if (*(real *)(entry + 0x10) > (real)level_index)
            {
                function_143c0(*(long *)entry, (short)bitmap, (short)index, bias);
                *(real *)(entry + 0x10) = (real)level_index;
            }
        }
        remaining &= ~(1 << index);
    }
}

struct s_slot_key
{
	byte a;
	union
	{
		long b;
		real amount; /* 0x3c270 keeps its clamped amount here once the lookup is done */
	};
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


hash_table *g_51f400;
extern __int64 g_485aa0;
s_depth_parameters_13450 g_depth_parameters_storage;
real g_5234c8;
dword g_4b81fc[0xa6];
D3DPIXELSHADERDEF g_484f68;
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
    long i = 0;
    byte *values = *(byte **)(*(byte **)(state + 0xc) + 8);
    byte *entry = *(byte **)(*(byte **)(state + 0x10) + 0x24) + (*range & 0x1ff) * 4;
    for (; i < (*range >> 9); entry += 4, ++i)
    {
        byte *slot = state + 0x320 + entry[0] * 0x40;
        if (*(long *)(values + entry[3] * 12) != NONE)
        {
            long *source = (long *)(values + entry[3] * 12);
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
		g_467010 = mode;
		return;
	case 1: case 2:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		function_0222d0(D3DRS_STENCILREF, mode == 1 ? 0 : 255);
		function_0222d0(D3DRS_STENCILMASK, 255);
		function_0222d0(D3DRS_STENCILWRITEMASK, 255);
		g_467010 = mode;
		return;
	case 3:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		g_467010 = mode;
		return;
	case 4:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		g_467010 = mode;
		return;
	case 5:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 2);
		function_0222d0(D3DRS_STENCILMASK, 1);
		function_0222d0(D3DRS_STENCILWRITEMASK, 2);
		g_467010 = mode;
		return;
	case 6:
		function_0222d0(D3DRS_STENCILENABLE, TRUE);
		function_0222d0(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		function_0222d0(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		function_0222d0(D3DRS_STENCILREF, 0);
		function_0222d0(D3DRS_STENCILMASK, 3);
		function_0222d0(D3DRS_STENCILWRITEMASK, 0);
		g_467010 = mode;
		return;
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
	bool found = false;

	for (index = 0; index < g_51f40c.count; index++)
	{
		slot = &g_51f40c.slots[index];
		if (slot->used && key->a == slot->a && key->b == slot->b && key->c == slot->c && key->d == slot->d && key->e == slot->e)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		for (index = 0; index < 511; index++)
		{
			slot = &g_51f40c.slots[index];
			if (!slot->used)
			{
				found = true;
				break;
			}
		}
		if (found)
		{
			function_20e50(index);
			slot->a = key->a != 0;
			slot->b = key->b;
			slot->c = key->c;
			slot->d = key->d;
			slot->e = key->e;
			if (index + 1 > g_51f40c.count)
				g_51f40c.count = index + 1;
		}
	}

	if (!found)
		return NONE;

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

PRIVATE __forceinline dword function_20f32(dword inputs, dword stage_outputs)
{
    dword outputs = 0x30c00;
    g_484f68.PSAlphaInputs[0] = 0x18201920;
    g_484f68.PSAlphaOutputs[0] = outputs;
    g_484f68.PSAlphaInputs[1] = 0x1a201b20;
    g_484f68.PSAlphaOutputs[1] = 0x30d00;
    g_484f68.PSAlphaInputs[2] = inputs;
    g_484f68.PSAlphaOutputs[2] = stage_outputs;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    return outputs;
}

// @retail 0x20f30
void function_020f30(real b, real a)
{
    dword count;
    dword outputs;
    dword half = 0xc00;

    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x8421;

    b = PIN(b, 0.0f, 1.0f);
    a = PIN(a, 0.0f, 1.0f);

    if (a > 0.0f)
    {
        real value = (a + 1.0f) * 0.5f;
        real x;
        if (value < 0.0f) x = 0.0f;
        else
        {
            x = 1.0f;
            if (!(value > 1.0f)) x = value;
        }
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
        outputs = function_20f32(0x1c121d12, half);
    }
    else
        outputs = function_20f32(0x1c201d20, 0x30c00);

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
        g_484f68.PSRGBOutputs[1] = outputs;
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
        g_484f68.PSRGBOutputs[0] = outputs;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[2] = outputs;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
    }

    g_484f68.PSCombinerCount = count | 0x11000;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
}

// @retail 0x222d0
void function_0222d0(D3DRENDERSTATETYPE state, dword value)
{
	g_4b81fc[state] = value;
	D3DDevice::SetRenderState(state, value);
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

__declspec(noinline)
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

/* inline: both retail callers (0x22850, 0x22cf0) keep the fraction in memory and reload XMM0 before every call */
// @retail 0x22750
inline void function_022750(const real *a, long size, const real *b, real *out, real t)
{
	long count = size * size;

	for (long i = 0; i < count; i++)
		out[i] = (a[i] - b[i]) * t + b[i];
}

// @retail 0x22c30
real function_022c30(const point3f *a, const point3f *c, const point3f *b)
{
	vector3f v1, v2;
	vector3f *v1_reference = &v1;
	real result;

	v2.i = a->x - c->x;
	v2.j = a->y - c->y;
	v2.k = a->z - c->z;
	result = 0.0f;
	v1_reference->i = a->x - b->x;
	v1_reference->j = a->y - b->y;
	v1_reference->k = a->z - b->z;

	real length = (real)sqrt(v1_reference->k * v1_reference->k + v1_reference->j * v1_reference->j + v1_reference->i * v1_reference->i);
	if (length > 0.0001f)
	{
		real inverse = 1.0f / length;
		v1_reference->i *= inverse;
		v1_reference->j *= inverse;
		v1_reference->k *= inverse;
		result = (v1_reference->k * v2.k + v1_reference->j * v2.j + v1_reference->i * v2.i) / length;
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

real distance_sq3f(point3f const *a, point3f const *b);
PRIVATE __forceinline void copy_light_sample(s_light_sample const *sample, byte order, real *red, real *green, real *blue, vector3f *direction)
{
    long bytes = order * order * sizeof(real);
    memcpy(red, sample->red, bytes);
    memcpy(green, sample->green, bytes);
    memcpy(blue, sample->blue, bytes);
    *direction = sample->direction;
}

// score 418 (was 418; 439 earlier): The 0x1c frame lacks the second scan's offset spill and pass counter at retail's 0x2c.
// M3 lane GB r3 (fast with 0x22750, 0x22c30, 0x24550, all MATCH unless noted; control 418, 2094/2122 B, +0x2, frame 0x1c; 18 compiled trials, body unchanged): real_point3d_set for delta/offset on 418 base 418/418 (same code), for delta on source->entries + first ahead 554 (2112 B, frame 0x20); union .n[0..2] for delta on entries 554, for offset on 418 base 433 (2122 B, frame 0x28); direct delta members on entries + first ahead 554; entries + first ahead + second=NULL then if 554; shared 2-caller forwarding helper point3f_difference with 0x22c30 (breaks 0x22c30 at 36, diagnostic only) on entries + first ahead 401 (2124 B, +0x9, frame 0x2c, al 119), with samples for delta/first/second and entries in scan 2 401 (same code), with late first 564 (2130 B, +0x9, frame 0x2c, al 117), on 418 base 417 (2112 B, frame 0x28), with length_sq3f of second diff 550 (frame 0x20: loses offset home when distance_sq3f is replaced by local length_sq3f); nested count==1 else + vector3f_assign fallback 419; typed s_light_sample_source count read 418; first from entries before samples 439; opposite before scan 1 on 418 572; consistent samples before count==1 625; entries + distance_sq3f(position, sample) 561.
// MiniMax A (11-20): no new form, no lower score (418): the fast check's reduced image leaves two of this function's calls unresolved, so it builds 1121 bytes against retail's 2122 and cannot measure the body at all - the plain score stays 418 with the 0x18 frame against retail's 0x2c (retail's homes: the opposite sample, the nearest index, a pass counter, the sample list, the offset vector and the delta vector), the query input, the native vector and ordinal and the fraction homes, and the scans' registers; the position input still arrives in EAX where retail's caller sets it in EDX, a parameter order that may not change here.
// Earlier (418, was 418; 439 earlier):
//
// Frame 0x1c/0x2c (r116); scan 2 walks one sample pointer where retail walks two with a spilled pass counter, and the
// offset vector has no home (retail stores offset.x at +0x24 each pass); delta takes XMM6/spill/XMM7 where retail has XMM5-7.
// Frame 0x18/0x2c; query input, native vector/ordinal and fraction homes remain.
// MiniMax A (11-18): no new form, no lower score (547): retail opens a 0x2c frame with homes for the opposite sample,
// the nearest index, a pass counter, the sample list, the offset vector and the delta vector, where this build opens
// 0x18 and keeps the count in edx with no home, and retail takes the position pointer as its third parameter (edx)
// while this build takes it in eax - a parameter order that may not change here.
// Earlier (547): Frame 0x18/0x2c; query input, native vector/ordinal and fraction homes remain.
// Earlier header (merged r139): Frame 0x18/0x2c; query input, native vector/ordinal and fraction homes remain.
// MiniMax S (11-17): no new form, no lower score (547): retail opens a 0x2c frame with homes for the opposite sample, the nearest index, a pass counter, the sample list, the offset vector and the delta vector, where ours opens 0x18 and keeps the count in edx with no home; retail's nearest scan is a four-wide unrolled pass over the samples with two induction pointers and the closest index in edi, where ours walks a pointer and keeps the closest sample.
// MiniMax D (11-17): no new form, no lower score (547, was 547); retail's 0x2c frame still needs its
// offset.x home at +0x24, the dead store each pass, the pass counter at +0x18 and the hoisted entries
// pointer in scan 2, and retail's EBX/EAX pair against our ECX/EDX stays. The permuter (30 tries,
// annealed) found no better form.
// MiniMax D (11-05): no new form, no lower score; the argument slots agree with retail's (the source
// pointer in the first stack slot, the order byte in the fifth), so the whole difference is the frame,
// 0x18 against retail's 0x2c.
// MiniMax E (11-06): no lower score; retail takes the position pointer as its third parameter (EDX)
// and pops six stack arguments, so retail's declaration has nine parameters against our seven; the
// call sites are in another file and the parameter order may not change, so this cannot be reached here.
// b6O (11-06) walk against retail, block by block, all scores fast build with 0x24550 in the image (BASE 547, 2122 bytes):
// block 1 (0x22cf0-0x22d18, count from the structure bsp and the override flag), the count == 1 copy (0x22d26-0x22d95),
// the override copies (0x234b6-0x234ec) and the fallback call (0x234ef-0x23537: direction (0, 0, -1), colour (0, 1, 0)):
// same fields, directions, constants and arguments as the draft. Nearest scan (0x22da0-0x22f6b): z*z + x*x + y*y,
// strict `distance < closest`, 1048576.0f start, matches. Opposite scan (0x22f6f-0x232e0): delta = position - first
// (z, y, x subtracted then read), projection (z*dk + y*dj) + x*di tested `<= 0.0f` (retail's `comiss 0, p; jb skip` at
// 0x23010), distance (z*z + y*y) + x*x, strict `<`: matches. Blend (0x23440-0x234b3): (second - first) * fraction +
// first per component, fraction `<= 0` copies first (0x2330c), `>= 1.0f` copies second (0x2337d): matches. No behaviour
// difference found in the arithmetic or the branches.
// Tried, not kept (same behaviour, retail's shape): both scans count an index (`i`, with `nearest` and `opposite` as
// indices) instead of walking sample pointers: retail keeps the nearest index in EDI and tests `i != nearest` as
// `cmp eax, edi` (0x22fd0, 0x23250), and turns `opposite` into a pointer only after the scan (0x232e9 imul), where the
// old pointer scans made VC divide the pointer difference by 0xdc (a magic multiply); `second` is a pointer that is NULL
// for opposite == NONE and tested again before the call, which is retail's `cmp esi, -1; je` followed by `add esi, ecx;
// je` (0x232e4-0x232f3), the position pointer now arrives in EDX as retail's caller sets it (0x239a1) where the old body
// had it in EAX; and the entries field is read at each use (retail reloads it at 0x22db5, 0x22f0d and 0x22f6f, caches it
// only for the second scan at 0x22faa and reads it again at 0x2322a). Scores: old body 547; index shape written out 639
// (2057 bytes); with the nearest distance as the existing distance_sq3f (0x24550, its body was written out in the draft)
// 583 (2085 bytes; the scan is then byte for byte retail's); delta through vector3d_from_points3d 583 (same
// code). In the full build on r803 that body also moves the caller 0x23690 330 -> 354, and with this batch's
// unknown_01cf50.cpp changes it costs the matched 0x41640 (the link-time coin flip), so the pointer body stays; the shape is
// retail's, and the old body scores lower only by accident.
// Also tried: scan 2's distance through distance_sq3f with samples read through a local after the first scan
// 458 (2070 bytes, Round 130's body): it adds x*x before y*y (z*z + x*x + y*y), but retail adds (z*z + y*y) + x*x
// (0x23021-0x2303a), a rounding difference, so it is not retail's evaluation order; scan 2 through offset =
// vector3d_from_points3d + dot3f with the distance written k, j, i 652 (2033 bytes), with a samples local after scan 1
// used by the scan, `first` and `second` 671, by the scan alone 582 (1995 bytes; no gain), by the scan and `second` 671; a
// vector3f offset in scan 2 639 (same code as scalars). Still missing: the 0x2c frame (retail homes offset.x at +0x24, a
// dead store at 0x23013 each pass, and delta.k at +0x38), nearest kept in EDI in the unrolled part, one pass counter
// at +0x18, and the hoisted entries pointer in scan 2; dot3f/length_sq3f are not used for the projection and distance
// because their sums run i, j, k where retail's run k, j, i.
// Earlier: Frame 0x18/0x2c; query input, native vector/ordinal and fraction homes remain.
// Earlier: Frame 0x18/0x2c, input/selection homes and scan registers differ (override layout and
// fraction retention now as retail).
// Native point-difference producer subsets nearest/anchor/both/opposite/nearest+opposite/
// anchor+opposite/all: 626/627/626/685/668/685/668; all plus shared dot 668 (exact-only).
// The one-point nearest gains are ineligible; original scalar producers and arithmetic restored.
// Frame 0x1c versus 0x2c; count/selection/projection and fraction homes differ.
// Round 428: complete squared products in both searches, projection products and both 627 each;
// actual selected-record common copy tail, with either/both products and common distance storage,
// 748 each at 1944/2122 bytes. All original sums, copy paths and body restored.
// Round 44: nearest/opposite real cursors with ordinal bounds, alone/both, 646/750/765;
// actual pointer-end traversal alone/both 696/629/798; nearest ordinal/opposite pointer
// 604 (kept, plain; 2138/2122 bytes), inverse 618. Frame shrinks to 0x18; the position
// input arrives in EAX rather than EDX; search/fraction homes remain.
// Round 450: one sample-table observation on cursor/end traversal 594; complete query
// value 729; both 727; live count 604, with table/query/both 594/729/727; entry delta 604.
// Singleton/multi-record shared table scope 589 (kept, plain; 2127/2122 bytes).
// Round 248: pointer-end scans with direct ordinals, pointer/ordinal exclusions and shared
// table ends 736/723/713/737/679/693/735/850; all original traversals restored.
// Round 62 (m3 lane 16): the override copies as the else arm of `if (!g_5093f4)` (retail places them after the
// blend) 581; with the implied `|| opposite == NONE` dropped from the copy-first test 570 (kept, plain); fraction
// assigned under `if (opposite != NONE)` 570; `second` as `opposite != NONE ? samples + opposite : NULL` tested
// before the call (retail's `add esi, ecx; je`) 576; the fraction pieces alone 584; index second scan
// (`j != nearest`) 726, with the fraction pieces 731. Retail-shaped body (index scans over
// `source->entries[i]` with `i != nearest`, ternary `second`, override else arm): 661 at 2033 bytes (LCS-aligned
// 564 against the old body's 987): position stays in EDX as retail, but VC shares one zero register for the
// count test, `nearest` and `i` (`xor eax, eax; cmp ebx, eax` where retail has `test ebx, ebx` and separate
// `xor edi, edi`/`xor eax, eax`) and keeps `nearest` in memory; with that body's first scan declaring y, x, z
// (retail's x, y, z load order) 661; with vector3d_from_points3d/dot3f/length_sq3f in the second scan 661; a short
// `nearest` (diagnostic: no zero merge, `nearest` in EDI, loop 1's registers as retail, but a movsx) 651, aligned
// 294. Retail's 0x2c frame holds two vector3f homes (an x spill at +0x24 and delta.k at +0x38, which shares the
// fallback vector's slots); every vector spelling here scalarizes fully (frame 0x20).
// On the kept 570 body: first scan indexed (`samples + i`) 583; entries read inside the count == 1 arm (aligned 961 ->
// 602, but source and position then arrive in EAX/ECX) 597; both 612; the retail-shaped body with the kept tail 664
// (aligned 425); that body with one `samples` local 715.
// Round 261: Coherent interpolation inline declaration, alone/with fraction-provider inline, each with
// original/unsigned selected ordinals/zero-default fraction/both result forms: 608 each (2145/2122
// bytes). All declarations and original body restored; count/selection/fraction homes and frame
// remain.
// Round 74 (m3 lane 12, fast): the existing cross-file distance_sq3f (0x24550) for scan 1 on this body 570 (same
// code). Retail-shaped index body (scan 1 through distance_sq3f, delta/offset through vector3d_from_points3d, dot3f,
// length_sq3f, `second` ternary then `fraction = 0; if (second) fraction = ...`) 654 (2033 bytes, aligned 307);
// one shared index 654; scan 2's distance through distance_sq3f 576 (2094 bytes, aligned 253: retail's y, z, x loads
// and projection order); with delta assigned k, i, j 608 (2062 bytes, aligned 246: retail's delta loads); either
// dot3f argument order same code; offset written out 583/831 (frame 0x30, entries re-read per component); offset at
// block scope 576/608; `nearest` declared before the count == 1 test 575/607. Still missing there: offset's own
// 12-byte home at +0x14 (retail stores x into it each iteration), delta.k at +0x28, nearest in EDI, and the
// fraction reloaded around 0x22750 (ours keeps it in xmm2 in every form).
// Round 297: Nearest/opposite/both ordinal-first candidate publications 570 each; native unsigned selected ordinals with all four publication pairs 570 each; shared true distance reduction 570. Original state updates, ordinal domains, reductions and complete body restored; frame/input and selected-sample/fraction homes remain.
// Round 299: Nearest/opposite/both selected pointer results 547/803/745; zero-default fraction pairs
// 547/803/745; both pointers with indexed nearest traversal 763/763. One-use nearest-result aliases restored.
// Direct actual selected-record state, no alias, 547; keep this ninth form, plain at exact 2122 bytes with
// all 11610 matches preserved. Original opposite ordinal, traversal, scalar trees and copy/blend paths
// retained. Frame/input, scan and fraction homes remain.
// Round 301: Direct opposite ordinal on nearest-pointer result, counter/pointer-end bounds 815/700; nearest
// indexed 782/685; complete query value 654/614, with indexed nearest 623/602. Original visits, float
// trees and body restored; frame 0x18/0x2c, position transport and scan expansion remain.
// Round 464: actual fallback-vector entry declaration/entry initialization/branch const/
// branch setter with common-exit/nested singleton joins: 547/594/547/547/547/594/547/547.
// Frame remains 0x18 versus 0x2c; original fallback, singleton exit and full body restored.
// Round 506: final direction scaled-difference shared/native producers with late/entry vector,
// dead search-delta reuse or shared setter 557/557/557/547/557/557/557/557. Original
// vector/scalar producers and entire body restored; frame/input and fraction homes remain.
// Round 511: complete sample-position captures nearest/anchor/opposite subsets through
// memcpy 742/589/781/701/873/729/900; all through memmove 1483. Original
// body restored; native frame/input, scan and fraction homes remain.
// Round 136 (m3 lane 6, fast): second scan loading y, z, x 592 (2112 B); the fraction as `= 0; if (opposite !=
// NONE)` 547 (same code); opposite declared after its distance 547 (same code); entries read at the count == 1
// call with the samples local after it 592 (2127 B); y, z, x loads with the if-form fraction 592.
// Also round 136: delta through vector3d_from_points3d 547; scan 2's offset through vector3d_from_points3d, dot3f
// and length_sq3f 547 (both same score at 2122 B).
// DH294: Joint count/selected-ordinal/blend-fraction lexical priorities, seven subsets and all
// with native vector-owner names, 547 each. Original names and complete body restored.
// DH313: Single own-input first-consumer lvalues source/red/green/blue/order/direction/position
// 547 each; entry-owned query-parameter pointer 563. NOT kept: exact-only own-address/alias
// forms fail byte agreement. Original body restored; frame/input, scan and fraction homes remain.
// J242: Existing nearest-distance producer with native anchor/opposite/dot/length consumers, eight
// complete forms 552 each at 2111/2122 bytes. NOT kept: all regress and shorten the exact-length
// starting body. Original searches, scalar groups, copy/interpolation paths and temporary
// declaration restored; frame 0x18/0x2c, query/selection and fraction homes remain.
// Round 140 (m3 lane 6, plain): index body reading `source->entries[i]` at each use, `nearest` index,
// delta assigned k, i, j, `second` ternary then `fraction = 0; if (second)`: 639 (2057 B); scan 1's sum
// as `z*z + y*y + x*x` 639 (same code: VC orders the terms itself); scan 1 through the cross-file
// distance_sq3f 583 (2085 B: retail's x, y, z loads and the esi/ecx pointer pair); with scan 2 through
// vector3d_from_points3d/dot3f/length_sq3f 654 (2033 B, aligned 301); offset declared before scan 2 654
// (same code); scan 2's distance through distance_sq3f with the projection through
// vector3d_from_points3d/dot3f 609 (2062 B, aligned 239: projection and distance share the
// subtractions as in retail, source in ESI); dot3f arguments swapped 609 (same code). Lead: that 609 body
// still lacks the 0x2c frame's offset home (+0x24, retail stores x there each pass) and delta's home
// (delta.k reloaded from +0x38), and `nearest` stays in memory where retail keeps EDI.
// Round 78 (m3 lane 9, fast): the count test inverted with the fallback first 591; delta declared
// at function scope 547 (same code); scan 1 as a pointer-end loop 609 (2143 bytes); `second`
// named before the fraction and passed to 0x22c30 599 (2133 bytes); scan 1's pointer step in the
// for header 547 (same code). Retail's 0x2c frame: opposite +0x10, nearest +0x14, a pass counter
// +0x18, count +0x1c, samples +0x20, a vector home at +0x24 (x stored each pass) and delta.k +0x38.
// Round 144 (m3 lane 6, fast with 0x24550's source in the reduced image, which reproduces the plain check;
// [LCS-aligned] this body [961]): `inline` on 0x22750's definition 560 [943] (2137 bytes): both retail callers
// spill the blend fraction and reload XMM0 before every 0x22750 call (0x22850: `movss xmm0, [esp+0x14]`) and with
// `inline` ours does too (the direction blend then takes retail's XMM1/XMM0 roles), but it is not keepable alone:
// 0x22850 went 237 -> 244 (fast, CAVEAT). With `inline`: scan 2 indexed (`samples + i != first`, `opposite = i`)
// 736 (1958 bytes); both scans indexed 739; the retail-shaped index body (`source->entries[i]` at each use, scan 1
// through distance_sq3f, a `nearest` index, delta/offset through vector3d_from_points3d, dot3f/length_sq3f,
// `i != nearest`) plain 642 [406] (2048 bytes; 1036 without `inline` and distance_sq3f unresolved); distance_sq3f
// given (sample, position) 636 [283]; scan 2's distance through distance_sq3f 561 [217] (2109 bytes); then `second`
// as `opposite != NONE ? &entries[opposite] : NULL`, `fraction = 0; if (second) fraction = ...` 554 [216] (2112
// bytes); on that `nearest` after the closest distance 555, the index declared first 554, `nearest` before the count
// == 1 test 553, `offset` outside scan 2 554, `delta` at function scope 554; per-loop indices on the 636 form 636.
// The 554 form is off by one from the top: VC shares one zero register for the count test, `nearest` and the index
// (`xor eax, eax; cmp ebx, eax`, retail `test ebx, ebx` and later `xor edi, edi`/`xor eax, eax`), keeps `nearest` in
// memory (retail EDI with a home), and the frame is 0x20 against 0x2c (retail's offset and delta vector homes).
// Y556: One point-difference workspace reused across both searches, entry/positive/enabled/
// multiple-record scope, shared setter/point-difference producers: 552/552/552/552/552/552/552/552.
// NOT kept: no byte match or qualifying retail frame/shape recovery; original body restored.
// Round 130 (m3 lane 8, fast; LCS-aligned counts in brackets, this body [506]): retail-shaped index scans with
// `source->entries[i]` at each use 639 [406]; with a samples local read after scan 1 705 [351]; scan 1 through
// distance_sq3f (0x24550) on `source->entries[i]` and scan 2's offset through vector3d_from_points3d + dot3f and
// its distance through distance_sq3f, with the samples local after scan 1 for first, scan 2 and second, `second`
// as a ternary then `fraction = 0; if (second)`: 457 [225] (2070 B, scan 1 byte-identical to retail, position
// in EDX as retail's caller sets it; not kept: the entries field read directly in scan 1 and through the local
// after it is a reload mix), plain check agreed, caller 0x23690 481 -> 490. Consistent forms of it: entries
// read directly everywhere 577 [207] (2094 B, retail's scan-2 pass counter and ESI/ECX pointers, but a shared
// zero register and nearest in memory), the local before scan 1 651 [265], before the count == 1 test 656.
// On the 457 form: scan 2's distance as length_sq3f(&offset) 549; delta assigned k, i, j 479, j, k, i 478;
// dot3f either order or its sum written k, j, i 457 (same code); offset/delta declared at function scope,
// jointly, after first or before samples 457 each (same code); one shared index 457; nearest declared before
// closest_distance 459; `if (i == nearest) continue;` 457. Retail's 0x2c frame keeps delta.i/j in XMM5/6
// with delta.k at +0x38 and stores offset.i to its own home at +0x24; the 457 form spills delta.j/k instead.
// F362: Existing sample copy as const input member, forced/ordinary expansion,
// byte/dword order and pointer/reference direction: 547/799/547/719/547/799/547/719.
// NOT kept: frame/query/search homes stay wrong; outlined copies shrink away from retail.
// Round 404: Actual coefficient-count long/dword/word/size_t producers 547/547/601/547; with separate
// byte extents 547/547/605/547. Original copy span/helper and body restored; frame/input,
// vector/selection and fraction homes remain.
// B410: Existing copier direction memcpy/memmove 547/601; coefficient memmove with
// aggregate/memcpy/memmove direction 649/649/662; native sample reference with direction
// memcpy/memmove or coefficient memmove 547/601/649. NOT kept: neutral or nonretail CRT
// transfer shape and shorter extent. Original copier, all calls and complete body restored.
// P308: Direct failing-candidate skips at nearest/projection/opposite-distance reductions,
// all seven subsets 547 each; all with count/override guard exits 570 at 2109 bytes.
// NOT kept: ties/regression; original searches, guards, helpers and complete body restored.
// P310: Existing coefficient copier returns native direction pointer/reference or sample
// pointer/reference: forced 547/547/547/547; ordinary 621/621/620/620. NOT kept: ties or
// outlining regressions; original full copier, direction consumer and complete body restored.
// Y572: Complete scalar query inputs at nearest/anchor/opposite boundaries, seven
// subsets then immutable full inputs: 605/547/615/605/585/615/585/585. NOT kept:
// no byte match; native query reloads and 0x2c frame remain. Original full body restored.
// Y574: Existing component-array unit-scale difference at nearest/anchor/opposite, seven subsets
// then early uninitialized native vector declarations: 922/547/1247/922/1634/1247/1634/1634. NOT
// kept: anchor ties; search forms suppress native four-wide expansion and regress or shrink;
// native 0x2c frame/query/ordinal/fraction homes remain. Original complete body and helpers
// restored.
// J250: Sole-caller fraction formal orders a/b/c, c/a/b, c/b/a, b/a/c, b/c/a
// 587/547/547/587/587; original-order stdcall/cdecl/inline 547/547/547. NOT kept:
// the 587 forms lose the matched fraction provider and move away from native extent.
// Query/frame/fraction homes remain; pointer-to-ordinal quotient also consumes EDX.
// Original provider contract and target body restored; prior ordinal-loop forms not repeated.
// M3 lane 4 r165 (fast, 0x24550's source in the image; count/aligned/bytes): index scans through distance_sq3f with (position, sample)
// 503/a306/2084, (sample, position) in scan 1 473/a276/2084, in both 457/a225/2070 (round 130's form, a reload mix), in scan 2 only
// 487/a255/2070; entries read at each use everywhere 577/a207/2094: delta orders kij and kji 609/a212/2062, jik and jki 608/a206/2062, ikj
// 578/a210/2094; nearest declared first 576/a207; length_sq3f 654/a287/2033, with dot3f written out 652/a277, with dot3f swapped 654/a287;
// vector3d_from_points3d with a written-out length 652/a277; offset written out with length_sq3f or written out 583/a350/2085, with
// distance_sq3f 761/a375/2289; scalars 583/a350/2085, with distance_sq3f 760/a339/2286. Const references bound to temporaries (the form
// that matched 0x45560): samples 577/a207 (same code as at each use), first 476/a254/2052, both 610/a221/2062, samples with jik/kij
// 608/a206, 609/a212, at-each-use first 610/a221, with jik 579/a221/2094. Nearest indexed at each use with no first pointer 610/a379/2156,
// kij 610/a382, jik 610/a379, delta through vector3d_from_points3d 657/a302/2018, first kept beside it 598/a395/2162, kij 599/a395. Scan 2
// reading offset as above with 1 to 7 dead locals at the top 577/a207 each (no numbering effect). Retail's two vector homes come from two
// spills (offset.x +0x24, stored each pass, about 28 B of the length gap; delta.k +0x38); ours spills delta.j and k, or nothing with
// length_sq3f (delta in xmm3-5 where retail colours xmm5-7). Its `lea ebp, [eax + ecx]` keeps the scaled index in a scratch register.
// J255: Coordinate-range subtraction at nearest/anchor/opposite producers, seven
// standard transform subsets 876/598/1237/912/1533/1362/1554; direct loops throughout 1471.
// NOT kept: all regress and shorten away from 2122 bytes. Original scalar/vector
// producers, reduction trees, includes and full body restored; frame/query/ordinal/fraction homes remain.
// F390: Existing three-use coefficient copier standard forward range copies at red/green/blue,
// seven subsets, then complete backward copies: 615/676/569/580/672/572/579/622. NOT kept: all
// regress; partial forms retain frame 0x18, complete forms shrink it to 0x14 versus native 0x2c.
// Original CRT copies, direction, helper and complete body restored; native
// P335: Standard component-array inner products at nearest/projection/opposite distance,
// seven subsets then complete graph with anchor transform: 845/1136/1138/1444/1417/1088/1402/1438. NOT kept:
// all regress and suppress native scan expansion. Original reductions, includes and complete body restored.
// P342: Standard binary component transforms at nearest/anchor/opposite producers, seven subsets then
// scan-phase output declarations: 876/598/1237/912/1533/1362/1554/1554. NOT kept: no native frame/scan
// recovery or byte match; original producers, includes and whole body restored. Frame 0x18 versus 0x2c,
// query/vector/ordinal/fraction homes remain.
// M380: Actual offset/delta aggregate initializers at nearest/anchor/opposite,
// seven subsets then immutable full vectors: 547 each at 2122 bytes. NOT kept: native
// frame/query/search homes remain; original scalar producers and complete body restored.
// Round 151 (fast; body restored): the index body re-measured: entries direct everywhere 576 (2094 bytes), samples local read after scan 1 459 (2070); a mix, kept only on a match; the local
// used by scan 2 is what gives retail's head and nearest in EDI (ldd/ddl 576; lld/dll/dld 459). On the direct body: `count >= 1` 532 (2096; no shared zero, but `cmp ebx, 1; jl` where retail has
// `test ebx, ebx; jle`: not kept), with `opposite` declared first 527 (2091), other_distance first 543, `source->entries + nearest` 532, delta or first at the top 532, a plain second pointer with
// `opposite != NONE` 533 (2089), length_sq3f(&offset) 610; nearest at function top 575, at the arm 575, after closest 577; one shared index 576; `1 == count` 576. Offset fields written j, k, i 779
// (2335; frame 0x2c, but the subtractions no longer share with the distance), jki/opposite first 772, kji 779. A cross-file difference (function_28e774) for delta, offset or both 459 (same code).
// Retail's frame: opposite +0x10, nearest +0x14, pass counter +0x18, samples +0x20, offset home +0x24 (x spilled before each `jb`), delta home +0x30 (k spilled at +0x38).
// T60: Actual source/query/direction object-reference contracts, seven subsets and
// all with the existing copier sample reference: eight at 547/2122. NOT kept:
// native query register, vector/ordinal homes and frame 0x2c are not recovered.
// Original interfaces, both caller sites, copier and complete body restored.
// Round 75 (m3 lane 10, fast with 0x24550/0x22750/0x22c30/0x146020/0x23690; [difflib-aligned]): index scans reading
// `source->entries[i]` at each use with scalar offsets 639 [406] (2057 B), the scan-2 offset as a vector3f 639; round
// 130's mix (scan 1 through distance_sq3f, a samples local after it) 457 [225] (2070 B; scan 1 byte for byte retail's,
// nearest in EDI), its consistent direct form 577 [207] (2094 B): VC reuses the remainder loop's `source->entries` load
// for `first` and the second scan, so that register costs `nearest` its EDI in scan 1 (hence the shared zero and the
// memory nearest); retail reloads the field after scan 1. On the mix: scan 2 as y, z, x scalars with the kji
// projection and distance 551 [280] (119 bytes short), the vector offset with a kji distance 549 [267], with length_sq3f 549,
// with delta assigned k, i, j as well 549 [271] (all 2005 B: the distance no longer shares the subtractions).
// Y602: Coordinate/reduction/minimum scalar lexical identities, then immutable original-site scratch:
// 547/547/547/547/547/547/547/547.
// NOT kept: no byte match or qualifying native-shape repair; original complete body and helpers restored.
// Y607: Existing scalar coordinate scratch shared across both searches, x/y/z subsets
// then all with shared candidate distance: 547/547/547/547/547/547/547/547. NOT kept: shared
// lifetime forms do not match. Original complete body restored; native frame/query/ordinal/fraction homes remain.
// M403: Native sample position/coefficient-triplet/direction readonly members,
// seven subsets then readonly table input: eight at 547/2122. NOT kept: native
// frame/query/vector/ordinal/fraction homes unchanged; original record and body restored.
// N391: Nearest/opposite cutoffs and paired candidate distances in double, seven subsets:
// 593/630/706/648/692/716/714; all plus projection 820. NOT kept: all regress;
// original scalar domains, expressions and full body restored. Native vector/query homes remain.
// N393: Native nearest/anchor/opposite coordinate-difference double residency, seven
// subsets 648/640/730/645/731/665/753; all plus projection 753. NOT kept: larger,
// unmatched storage domains; original scalar graph restored. Frame/query/vector homes remain.
// M3 lane 12 r136 (fast with 0x24550, 0x22c30, 0x22750, 0x146020 and the caller 0x23690; the plain check agrees on the kept form):
// kept at the same count, 547 (2122 B): delta through the existing vector3d_from_points3d, and scan 2's offset, projection and
// distance through vector3d_from_points3d, dot3f and length_sq3f, in place of their bodies written out as scalars (the emitted sums
// are (y*dj + z*dk) + x*di and (z*z + y*y) + x*x, the same values as retail's; 0x23690 stays 330). Scan 1 through distance_sq3f on
// this body 552 (2111 B), with the other three as well 552; the delta alone 547; scan 2 alone 547 (other registers).
// The retail-shaped index body re-measured 654 (2033 B; register-masked aligned 118 against this body's 397): its first differences
// are `nearest` kept in memory (retail EDI with a home at +0x14) and one zero register shared by the count test, `nearest` and the
// index. On it: `nearest` and `opposite` as a two-long array 654 (same code, frame 0x18); `if (0 < count)` around scan 1 635
// (2039 B); `nearest` through a reference 654 (same code); scan 1 in an inline helper returning the index 1130 (1572 B, the scans
// are no longer unrolled); dot3f with its arguments swapped 654 (same code).
// F417: Remaining-count cursor in nearest/opposite/both passes 575/594/662.
// Postfix admission in nearest/opposite/both 922/1279/1633. Shared remaining count
// 662, shared count and cursor 662. NOT kept: the 0x18 frame remains and postfix
// admission removes the four-wide expansion. Original passes and body restored.
// Round 622: inclusive nearest cursor 590; inclusive indexed nearest 614; opposite ordinal with cursor 792;
// indexed opposite result 727; shared inclusive bound with cursors 770; shared inclusive bound with indices 699;
// cursor searches with ordinal exclusion 779; index searches with ordinal exclusion 684.
// All eight inclusive-bound forms regress. Each inclusive opposite scan shortens the body by at least 130 bytes.
// Two indexed scans with ordinal exclusion get a 0x1c frame and retain four-wide strides at 1992 bytes. The
// baseline query register, vector homes and 0x18 frame remain.
// Round 625: typed sample-position copies at nearest/anchor/opposite and nearest+anchor/
// nearest+opposite/anchor+opposite/all 742/589/701/781/873/729/900; nearest copy through the
// squared-distance helper 892. Existing sample record for active BSP count 547.
// Opposite distance with current bound as conditional/default/explicit fallback 605/645/605.
// NOT kept: the native copies expand the extent or remove the four-wide scan, and the
// candidate-result forms add work to the rejected projection path. The 0x18 frame remains.
// Fraction inside opposite-ordinal/resolved-pointer admission; lower-then-upper tests
// 560/568, upper-then-lower 561/562, partial-before-endpoint 560/569,
// shared selected-endpoint copy 673/672. Frame stays 0x18.
// M3 lane 15 r99 (fast with 0x24550, 0x22c30, 0x22750, 0x146020 and the caller 0x23690; [n] = aligned differing lines, this
// body 547 [961]). The index body re-measured (entries read at each use, scan 1 and scan 2's distance through distance_sq3f
// with the sample first, delta and offset through vector3d_from_points3d, dot3f, a `nearest` index, the ternary second, then
// `fraction = 0; if (second)`): 577 (2094 B) [234], 0x23690 330 -> 354. On it: scan 2's offset through the by-value
// vector3f_assign with the three differences as arguments 761 (2289 B, frame 0x30) [623]: the offset gets a home, but the
// subtractions are no longer shared with the distance (the same code as the offset written out); the delta through
// vector3f_assign as well 761 (same code); the delta alone 577 (same code as the index body); the fallback vector through
// vector3f_assign 578 [247] (on this body 547, same code: the fallback is already float stores); `dword` indices 484 (2110 B)
// [244], a positional accident (unsigned jb/jae where retail has jl/jge), not kept; an index per loop 577 (same code); `first`
// as `&source->entries[nearest]` 577 (same code). Retail read again for the homes: nearest is written to +0x14 only after
// each part of scan 1 and reloaded in scan 2 (once inside the remainder loop's update, at 0x232c3), opposite is stored at
// every update, the offset's x is stored each pass and never read, and the delta's k is the only delta component with a
// store (reloaded into XMM7 after each unrolled pass); the fallback vector is float stores with two `push 0` immediates,
// so the integer zero our index body shares between the count test, nearest and the index does not come from the fallback.
// Round 630: direct opposite winner with indexed records, nearest pointer/ordinal 742/712;
// with a count-bounded cursor 830/781. Convert the winning pointer once after the walk,
// pointer-bounded 711/735, count-bounded 766/762. The frame and query register differences remain.
// M3 lane 12 r146 (fast with 0x24550, 0x22c30, 0x22750, 0x146020 and the caller 0x23690, which stays 330): on this
// pointer body the entries field read at each use (no samples local) 592 (2150 B); only in scan 2 and for the second
// sample 615 (2145 B); the samples local read again after scan 1 547 (same code); the first sample's position in a named
// pointer for the delta 547 (same code); the position's components read into locals before scan 1 605 (2081 B). A `/Gd`
// first-pass compile (the pass LTCG takes register arguments from) keeps the position pointer in a callee-saved register
// in each of these, where retail's EDX needs a scratch register there.
// M3 lane 1 r123 (fast with 0x23690, 0x24550, 0x22750, 0x22c30, 0x146020): retail's two calls push source, red, green,
// blue, order and direction and pass the position last in EDX; the position parameter first, second or sixth (the
// definition, the declaration in src/unknown_023540.cpp and both calls together) 547 each (same code, still EAX), so the
// arrival register follows the body (the index-scan bodies receive it in EDX), not the slot. The fallback as an early
// return 591; the override copies as an early return 584. All restored.
// Round 262 (m3 lane 3, fast with 0x24550, 0x22c30, 0x22750 and the caller 0x23690): the index body (entries read at each
// use, both distances through distance_sq3f, delta and offset through vector3d_from_points3d, dot3f, the ternary second,
// `fraction = 0; if (second)`) re-measured 576 (2094 B): scan 1 is retail's but `nearest` stays in memory (retail EDI,
// stored to +0x14 at the loop exits) and the limit takes EDI; on it the count == 1 copy as an if/else arm with no return
// 576 (same code); `opposite` declared with `nearest` before scan 1 564 (2089 B: the limit moves to retail's EBP, but
// red is then loaded into EDI at the entry and `nearest` stays in memory); that with scan 2's distance as
// length_sq3f(&offset) 660 (2020 B). On this body: `opposite` declared before scan 1 634 (2159 B); the count == 1 copy
// as an if/else arm 547 (same code).
// M3 lane 16 r139 (fast with 0x24550 and the caller 0x23690): the index body (`source->entries[i]` at each use, scan 1 and
// scan 2's distance through distance_sq3f(sample, position), delta and offset through vector3d_from_points3d, dot3f, the
// ternary `second`, `fraction = 0; if (second)`) re-measured 576 (2094 B, frame 0x20; 0x23690 330 -> 354). Its head is
// retail's but for the frame and the shared zero; scan 2 matches retail's shape except that delta.j and delta.k are spilled
// where retail spills only delta.k and gives the offset its own 12-byte home (offset.i stored each pass, never read): that
// home is the 0x2c frame's missing 12 bytes (retail: opposite +0, nearest +4, pass counter +8, count +0xc, samples +0x10,
// offset +0x14, delta +0x20 shared with the fallback vector; other_distance reuses the source argument's slot). On it: the
// offset through a pointer local to it with length_sq3f 654 (2033 B), with distance_sq3f 576 (same code: the pointer folds
// and the offset still has no home); the nearest index renamed so that it sorts after `opposite` 576 (same code: names do
// not move these slots); the fraction as a ternary on `second` 576 (same code); opposite and its distance declared ahead
// of `first` 576 (same code).
// M3 lane 14 r116 (fast with 0x22750, 0x22c30, 0x24550, 0x23690 and 0x22850; kept body confirmed plain): retail's index
// shape with scan 1 through distance_sq3f (0x24550), `nearest` from 0, delta through vector3d_from_points3d, scan 2 on
// `i != nearest` with offset through vector3d_from_points3d and dot3f, `second` as a ternary and the fraction under
// `if (second)`: with the distance as length_sq3f(&offset) 550 (2005 B; the dot then sums (k + i) + j, retail (k + j) + i);
// with the distance as distance_sq3f(sample, position) 459 (2070 B, not kept: it sums (k + i) + j where retail's scan 2 sums
// (k + j) + i, a rounding difference); the distance staged k, j, i on offset 550; the distance as length_sq3f of a second
// difference vector 455 (2070 B: the dot and the distance both sum (k + j) + i as retail, position in EDX as retail's
// caller passes it); then `inline` on 0x22750's definition 435 (2088 B; kept: both retail callers spill the fraction and
// reload XMM0 before every 0x22750 call, and 0x22850 also falls, 226 -> 209 at retail's 987 B). On that body: dot3f
// operands swapped 435 (same code); both difference vectors formed before the dot test 451; the dot written out i, j, k
// 439 (diagnostic); scan 2, first and second through `source->entries` 571 (retail's two-pointer scan 2 and pass counter
// at +0x18, but one shared zero register at the top and `nearest` in memory); scan 2 alone through `source->entries` 549
// (same, frame 0x20); that with `opposite` defined before scan 1 533; offset declared before scan 2 435 (same code);
// delta assigned k, i, j 457 (diagnostic); a named sample pointer in scan 2 456. The caller 0x23690 moves 330 -> 354
// because both its calls now set EDX as retail's do (the only change in its code).
// Self-review: the same body with the distance as length_sq3f(&offset) (no second vector) 531 (2023 B), but its projection
// then sums (k + i) + j where retail and the previous pointer body sum (k + j) + i, so the second vector stays.
// Coordinator review (11-19): the second difference vector (435) recomputes offset by hand, so scan 2's distance goes
// through distance_sq3f(&samples[i].position, position) like scan 1 instead: 439 in the fast and the full check (2088 B),
// 0x22850 209, nothing lost. Retail's scan 2 still sums (k + j) + i, as a vector's length_sq3f would.
// M3 lane 14 r118 (fast with 0x22750, 0x22c30, 0x24550; difflib-aligned counts in brackets, this body 439 [285]):
// scan 2's distance as dot3f(&offset, &offset) 531 [317] (2023 B, the same code as length_sq3f(&offset)); as
// length_sq3f(&offset) 531 (its distance sums (k + j) + i as retail's, its dot (k + i) + j); that with
// dot3f(&offset, &delta) 531 (same code); scan 2 through `source->entries` 554 [220] (2112 B: retail's two pointers,
// pass counter at +0x18 and loop latch, but `nearest` lives in memory in both scans and delta.j is spilled); that with
// length_sq3f(&offset) 635 [296]; that with `if (i == nearest) continue;` 554 (same code). Dead-local scan K = 1..12 on
// the length_sq3f form: 531 at every K, so its dot order is not a symbol-count tie.
// The frame remains 0x1c versus 0x2c, with two induction addresses and the offset spill still missing.
// First sample at the scan exit 439, after the second sample 418 (kept, plain), nearest position
// reference 439, late first record reference 439, candidate position reference/pointer 439/439,
// candidate reference with late first sample 460, with nearest position reference 439.
// Opposite-scan record cursor for projection/distance/both with a count bound
// 590/591/513, or an end bound 1234/1234/1187. A direct winning pointer with
// a count-bounded projection or distance cursor 605/605. NOT kept: all regress;
// the offset spill and independent induction addresses still differ.
// M3 lane 4 r228 (fast with 0x22750, 0x22c30, 0x24550, all MATCH; control 418 at 2094 B; masked aligned rows in brackets,
// control [276]): scan 2's distance as length_sq3f(&offset) 646 (2039 B) [310], with the dot named `projection` 646 [310],
// with a named sample pointer 646 [298]; the dot named on this body 418 (same code). Scan 2 reading source->entries for
// the offset and the distance: 549 at retail's 2122 B [149]: retail's two pointers, pass counter at +0x18 and latch, but
// nearest then leaves EDI in both scans (spilled at +0x10, opposite at +0x14, retail the reverse), delta takes y, z, x
// with y spilled (retail z, x, y with z spilled), no offset.x store, and the distance sums (k + i) + j (0x24550's order,
// retail (k + j) + i). On that body: the distance as length_sq3f(&offset) 628 (2057 B) [139], as dot3f(&offset, &offset)
// 628 [139], as length_sq3f of a second difference vector (diagnostic only, the reverted hand recompute) 544 (2122 B)
// [129]; on that last body opposite declared before scan 1 583 (2097 B) [137], the delta from source->entries with
// samples named after scan 2 561 [341], the delta declared before scan 1 544 (same code). Lead: the two-pointer scan 2
// needs source->entries, and nearest has to keep EDI across both scans.
// Nearest/opposite/both pre-tested while loops 418 each. Positive-domain do loops
// in the nearest/opposite/both scans 971/1092/1612. With the other scan pre-tested
// while, 971/1092. NOT kept: the post-test forms suppress four-wide expansion,
// shorten the body and leave the vector spill and extra induction address unresolved.
// Nearest, opposite and both cutoff scopes 418 each. Reset shared cutoff 418 or 419 before the delta.
// The reset cutoff or separate scopes with live table reads in scan two 549/549. One table snapshot
// across both scans 623. NOT kept: the live table forms reach retail length but retain a short frame
// and miss the candidate x store. The snapshot shortens the body, and the other forms tie or regress.
// M3 lane 3 r280 (fast with 0x22750, 0x22c30, 0x24550, all MATCH each time; control 418 at 2094 B, frame 0x1c; [n] =
// aligned differing rows, control [273]). Reading: retail computes the scan-2 offset y, z, x and stores only x to the
// offset's 12-byte home each pass, loads the delta k, i, j and stores only k (at +8 of the home it shares with the
// fallback vector), so both vectors keep their homes; the i product of retail's projection is `delta.i * offset.i`
// where the j and k products have the offset first. A difference helper returning its output pointer (a local
// one-caller copy of the difference helper, diagnostic), chained into dot3f, as a statement or through a pointer local:
// 418 each with the helper distance, 646 with length_sq3f (all the same code as the void helper: the pointer folds, no
// home). The offset's fields assigned directly in place of the difference helper (match-only, a helper body written
// out), with dot3f and the helper distance: i, j, k 433 [255]; j, i, k 435 [253]; j, k, i 435 [265]; i, k, j 427 [268]
// (all 2122 B, frame 0x28: the offset then has its home and its x is stored each pass whatever the field order, the
// delta loads k, i, j with only k stored as retail, the samples pointer keeps its home; still one induction pointer
// and the pass counter in ESI, which is the missing frame dword); k, j, i 469 [241] and k, i, j 469 [251] (2086 B).
// With the distance as length_sq3f(&offset) every order 649 (2039 B, the home is gone again). On the j, k, i form: the
// delta's fields assigned directly too 422 [222] (2094 B); the projection written `offset.k * delta.k + offset.j *
// delta.j + delta.i * offset.i` 428 [268]; the distance written k, j, i on the offset 649; both 649. On the helper
// offset: that written projection 575 (2135 B), with the written distance 646. Scan 2 through `source->entries[i]`:
// helper offset 549 [219] (2122 B, frame 0x20, listed; no samples local at all gives the same code), with length_sq3f
// 628 [286]; with the offset's fields assigned directly 848-859 (2357-2366 B: each field store makes the entries field
// be read again), with length_sq3f 572 (2109 B) in all six orders. Lead: the offset keeps its home when its fields are
// stored directly and the distance repeats the subtractions through the helper; no form here combines that with
// retail's two induction pointers.
// Later in r280 (a compile-only screen first, then the fast check): the induction pointers follow the base's kind, a
// named samples local gives one pointer in either scan, the entries field read gives two; the samples bound as
// `s_light_sample const *const &` to the entries field (same code as the field at each use, 549) with the offset's
// fields assigned directly and the helper distance reaches retail's frame 0x2c with two pointers, the pass counter
// and the offset's x stored each pass, but the distance then repeats its subtractions from the second pointer: 691
// (2228 B) for j, k, i and i, j, k, with dot3f's arguments swapped 691 (same code), with the written projection 696
// (2233 B); with length_sq3f(&offset) 650 (2038 B, frame 0x1c), the helper offset with length_sq3f 628 (2057 B).
// On the samples local, dot3f(&offset, &delta) with the j, k, i fields 435 (same code as the other order).
// Nearest or opposite distance scratch at scan scope 418/418, projection scratch at scan scope 418,
// offset declared before ordinal exclusion 418. Nearest cutoff, zero projection limit, or opposite cutoff
// on the left of its comparison 418/418/418. Both opposite-scan scratch values at scan scope 418.
// All tie. The 0x2c retail frame, second induction address and offset.i store remain unresolved.
// Reused offset norm as square accumulation or grouped sum 646/646. Candidate setter with either
// norm 649/649. Both setters with either norm 649/649. Anchor setter and accumulated norm 646.
// Candidate setter with offset-first dot and accumulated norm 649. NOT kept: 2039-byte extent
// and 0x1c frame, with the candidate-X store still missing.
// Conditional nearest/opposite state assignments, alone or paired, and named projection
// alone or with nearest 418 each. Named projection with opposite or both 1236 each.
// Filtered distance with the current cutoff for a rejected candidate 417. NOT kept:
// one-point gain, extent grows past retail, and the separate frame homes remain.
// Opposite scan with projection continue inside the ordinal arm, separate ordinal/projection guards,
// all rejecting guards, grouped rejection in the ordinal arm, or distance continue inside either
// set of admission arms: 418 each. Split ordinal ranges with positive or rejecting projection 600/600.
// NOT kept: the single scans retain the missing offset and pass-counter homes, and the two ranges regress.
// M3 lane 3 r284 (h2m fast screen with 0x22750, 0x22c30, 0x24550, all MATCH; [n] = aligned rows, control [205]): after
// scan 2 retail threads opposite == NONE straight to the first sample's copy (`cmp esi, -1; je 0x2330c`) and tests
// `second` on the add's flags (`add esi, ecx; mov ebx, esi; je`), where ours materialises the NULL (`xor ebx, ebx` ...
// `test ebx, ebx`). `second = NULL; if (opposite != NONE) second = ...;` 422 [208] (2092 B); `if (opposite != NONE) {
// second = ...; if (second) fraction = ...; }` 440 [205] (2088 B: retail's tail exactly, but a repeated test); `first`
// and `fraction` declared ahead of the ternary `second` 439 [208] (2088 B, plain fast check agrees: retail's threaded tail
// without the repeated test, but `opposite` then takes ESI through the remainder loop where retail keeps it at +0x10);
// `fraction` ahead of `first` 418 (same code); `second` as an if/else 418 (same code), with `first` and `fraction` ahead
// of it 439. The threaded tail needs nothing between `second` and its test.
// Search-role names for the vectors, winning ordinals and pass counters in all seven subsets,
// then all with both cutoffs renamed: 418 each. These names leave the frame and second induction unchanged.
// One direction workspace for anchor and fallback 419, with scan-scope offset or a shared candidate
// position 419/419. One candidate position with aggregate or shared setter offset 433/418. Shared
// direction and aggregate or setter offset 433/419, or with a candidate read from the live table 550.
// The aggregate recovers retail length and the candidate-X spill, with a 0x28 frame.
// The aggregate recovers the candidate-X spill and retail length. Candidate offset named before
// delta 433, reset distance limit 433, separate scan indices 433, named nearest/opposite ordinals
// 433, named sample count 433, opposite scan state declared before delta 417, loop-top ordinal
// skip 433, candidate point pointer 433. NOT kept: the best form gains one, keeps the 0x28 frame
// and lacks retail's second induction address.
// M3 lane 4 r240 (fast with 0x22750, 0x22c30, 0x24550; control 418): scan 2 with the samples bound as a const reference to
// the entries field and the distance taken from the offset: helper offset with length_sq3f 628 (2057 B); fields assigned
// directly i, j, k with dot3f and length_sq3f 650 (2038 B), with the projection and distance written in retail's k, j, i
// order 650 (same code), fields assigned j, k, i 650 each way; helper offset with the written sums 626; plain samples
// local with length_sq3f 646. Retail's distance `(k*k + j*j) + i*i` reuses the offset registers; distance_sq3f sums k, i, j.
// M3 lane 15 r123 (h2m fast screen with 0x22750, 0x22c30, 0x24550, all MATCH each time; control 418, [276] aligned rows).
// Reading: scan 1 of this body is retail's byte for byte. In the all-direct form (549, 2122 B [223]: retail's scan-2 pointer
// pair and pass counter home) `opposite` owns EDI outside scan 2, scan 1's unroll limit takes EDI (`lea edi, [ebx - 3]`,
// retail EBP) and `nearest` has no register in either scan; retail keeps `nearest` in EDI through both scans and spills it
// only around the scan-2 latch, as this body does. Diagnostic: with scan 2's skip test removed `nearest` gets EBP in scan 1
// (628), so its scan-2 use is what costs it the register there. On the all-direct form: every entries read through a
// file-local inline accessor 549 (same code); the accessor with `first` ahead of the delta, a named scan-2 sample pointer,
// dot3f(&offset, &delta) and length_sq3f(&offset) 635 (2051 B), the same without the accessor 635 (same code), the accessor
// with length_sq3f only 628 (2057 B); scan 1 in a file-local helper returning the index, `inline` 1018 (1649 B, stays a
// call), forceinline 549 (same code); scan 2 in a helper taking the nearest index by value, `inline` 1413 (1251 B),
// forceinline 584 (2086 B); both helpers, `inline` 1969 (726 B), forceinline 584, with length_sq3f(&offset) 628;
// `opposite` and its distance declared ahead of or right after `nearest` 583/583 (2097 B); `second` as NULL then
// `if (opposite != NONE)` 541 (2122 B [268]: the count moves to EDI, `nearest` still in memory); the skip as
// `if (i == nearest) continue;` 549 (same code); a second, redundant `i != nearest` in the update 549 (folded,
// diagnostic); `second` kept as a pointer set in the loop 563 (2122 B [339]); `first` ahead of the delta with the delta
// through it 554 (2112 B [216]); that with the skip as a pointer compare against `first` 593 (2066 B). Re-measured: the
// offset's fields assigned directly 433 (2122 B [259]), with a const reference to the entries field 691 (2228 B), with
// length_sq3f(&offset) 649/650. Body unchanged.
// Anchor record bound once 439, candidate position before ordinal test 593, both 439,
// or both with live entry-field indexing 554. Opposite scan unequal or preincrement
// latch 1236/418. Complete distance before the projection gate 668, or through the
// live candidate position with bound anchor 665. NOT kept: the last form reaches
// frame 0x2c but grows to 2218 bytes and regresses. The second induction and its
// offset.i spill remain unresolved at the opening extent.
// M3 lane 5 r226 (h2m fast screen with 0x22750, 0x22c30, 0x24550, all MATCH each time; control 418; "pass" = scan 2's
// first unrolled pass compared instruction by instruction with retail's 29, registers included; control 7 of 28).
// Reading: in retail's pass the y and z products have the offset first, the x product the delta first, both sums end
// on x, x is stored to the offset's home just ahead of the `jb` and never read back, and delta.j keeps XMM6: the
// offset's x behaves as a member stored directly into an address-taken vector, its y and z as values stored through
// a pointer. Diagnostic (a pointer to the whole local that the body writes through, not kept): `offset.i = ...`
// written directly with j and k through `vector3f *out = &offset`, dot3f(&delta, &offset) and the helper distance:
// 417 (2112 B, frame 0x28), pass 18 of 29: the whole dot block is retail's, registers included (offset XMM2/XMM3/
// XMM1, delta XMM5-7, temporaries XMM4/XMM0, the x store ahead of the `jb`); the 11 left are the distance, which sums
// (k + i) + j as 0x24550 does where retail sums (k + j) + i, and it still walks one pointer. The same with x stored
// last 559 (2122 B). With length_sq3f(&offset) both lose the home again (649, 2039 B, frame 0x1c), though both sums
// then run k, j, i with retail's operand sides: the store survives only while nothing after the `jb` reads the
// offset. With the delta's i direct as well 418 (the helper form's code: the store is gone). Forms that do not give
// the mix: all three members through the pointer local 418 (same code as the helper); the difference helper as a
// local copy that is plain, forceinline, takes a reference, returns its pointer (statement or chained into dot3f)
// 418 each, stores j, k, i 422, returns the vector by value 654 (2306 B, frame 0x30); `(&offset)->i` spellings 433
// and a `real n[3]` local indexed 0..2 433 (both the same code as direct members, pass 3 of 29); direct members in
// the orders ijk/ikj/jik/jki/kij/kji 433/427/435/435/469/469 (pass 3/3/3/2/3/0: all three products delta first).
// Reads do not matter: the dot and the length written out with `offset.i` direct and j, k through the pointer, on
// direct, mixed or helper stores, 649/649/646 (418 with the helper distance). On the entries-field scan (549, 2122 B,
// two pointers, pass 8 of 28): the mixed stores with the helper distance 760 (2320 B, frame 0x2c: each direct x
// store makes the entries field and the samples be read again), with length_sq3f(&offset) 572 (2109 B); direct
// members 859 (2366 B) / 572; three named differences then the members 859 / 572. Lead: retail's scan 2 needs a
// direct store of the offset's first member that does not make the entries field be read again, with j and k as
// plain values; no source form here gives both.
// Exclude nearest by sample or position address, winner bound before/after anchor
// delta, captured/indexed candidate forms 615/615/639/615 for either address.
// All shorten the body and reserve 0x14. The second induction address and offset.i
// spill remain unresolved.
// Candidate offset squared directly, accumulated, or as three named squares
// inside or before the projection test: 646 each with either square-sum order.
// NOT kept: the second scan shortens to 2039 bytes and still omits the offset
// spill and second induction address.
// Absolute four-record chunks for nearest/opposite/both 626/577/669, relative chunks 635/669/761.
// Advance the next ordinal before the nearest/opposite reduction 947/1190. NOT kept: the closest
// chunk frame is 0x28 at 2224 bytes, and advancing the ordinal suppresses unrolling.
// M3 lane 12 r170 (h2m fast with 0x24550 in the image; control 418): `vector3f offset` declared beside `delta` outside
// scan 2 with the distance as length_sq3f(&offset) 646 (2039 B), with the distance helper kept 418 (same code); scan 2
// inside a redundant `if (0 < count)` 585 (2100 B); on the entries-read scan 2 (549 re-measured) that guard 557; scan 2's
// skip test through a const reference to `nearest` 549 there and 418 here (same code each). Body unchanged.
// M3 lane 2 r133 (h2m fast with 0x24550, 0x22750, 0x22c30, all MATCH; control 418): the distance from a second
// difference vector (`vector3d_from_points3d(&samples[i].position, position, &difference)` then length_sq3f) 416
// (2094 B: the distance now sums (k + j) + i as retail, but the dot keeps delta.j/k spilled and its i product
// offset-first, and the offset.i store and second pointer are still missing); that with dot3f(&offset, &delta)
// 416 (same code); that with `first` declared ahead of the delta and the delta formed from it 435 (2088 B); the
// dot test as `if (dot3f(...) > 0.0f) continue;` 432 (2099 B); the outer tests as early returns (count <= 0
// fallback, then the flag copies, then the main path unnested) 606 (2082 B). Body unchanged.
// M3 lane 16 r174 (h2m fast with 0x24550, 0x22750, 0x22c30, all MATCH; [n] = aligned rows, control 418 [273]): scan 2's
// candidate read from the entries field (both reads) 549 [219] (2122 B: retail's two induction pointers and the pass
// counter at +0x18, frame 0x20, but `nearest` in memory and delta.j/k spilled); on it `first` formed ahead of the delta
// and the delta built from it 554 [216] (2112 B), the projection compare written `0.0f >= dot` 549 [219], the skip as
// `nearest != i` 548 [216]; on it the delta written out in all six component orders (diagnostic) 549 to 581 [219 to
// 233], none gives retail's delta.i/j in XMM5/XMM6 with only delta.k spilled. Body unchanged.
// M3 lane 12 r173 (h2m fast with 0x24550; control 418): the vector difference and dot as plain functions in another
// of this machine's files, so only link-time code generation inlines them (diagnostic, helpers removed) 418 (same code:
// the homes retail keeps do not come from link-time inlining); the offset read through a `vector3f const &` in the dot
// 418; the delta the same way 418; one `long i` shared by both scans 418; that with the delta declared beside it 418
// (all the same code). Retail's delta home is the fallback direction's slot (frame offset 0x20 in both paths). Body unchanged.
// Both strict reductions through one updater. Reference/pointer state outputs 419/419,
// accepted-result outputs 418/418, returned ordinal 608/608, returned cutoff 545/545.
// The 0x2c frame and separate candidate spill still differ.
// The second scan still needs its first-coordinate offset spill and second induction address. Horizontal
// pair written before or after the depth component 563/460, horizontal pair with aggregate or setter
// 433/437, depth pair before or after the first component 540/540, and depth pair with aggregate or setter
// 601/557. NOT kept: the 433 form reaches 2122 bytes and frame 0x24 but raises the score and still lacks
// eight frame bytes.
// M3 lane 8 r223 (h2m fast with 0x22750, 0x22c30, 0x24550, all MATCH; control 418): the entries field read at each
// use (549 re-measured, 2122 B: in scan 2 `nearest` sits at frame 0 and is reloaded into EBX for each skip test with EDI
// as the pass temporary, where retail keeps `nearest` in EDI with its home at frame 4 and `opposite` at frame 0); that
// with `opposite` renamed to sort ahead of `nearest` 549 (same code: names do not move these slots); that with the delta
// written out k, i, j (retail's load order) 624 (2182 B); the compare sides swapped (`0.0f >= dot`, `other_distance >
// distance`, both scans' `closest > distance`) 418/418/418 here and 549 on the entries-at-each-use form (same code).
// M3 lane 12 r176 (h2m fast with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, aligned 273): the fraction as one
// conditional expression 418; `first` declared ahead of `second` 439 (2088 B); the opposite scan's update as `other_distance >
// distance` 418; one scan index declared once for both scans 418 (same code each); `nearest` declared after the closest
// distance 416 (a declaration move under the bar, not kept). Retail's scan 2 runs four passes with a second index (i + 2)
// in a stack cell and two record pointers, and sums both the projection and the distance from the offset registers.
// Body unchanged.
// M3 lane 6 r231 (h2m fast with 0x24550, 0x22750 and 0x22c30, all MATCH; control 418, aligned 273): scan 2 reading the
// entries field at both uses 549 (2122 B, aligned 219); that with the distance as length_sq3f of a second difference vector
// 544 (2122 B, aligned 194, masked 103: retail's (k + j) + i sum and both induction pointers, frame 0x20 against 0x2c,
// `nearest` in memory); with length_sq3f(&offset) instead 628 (2057 B); the 544 form with `dot3f(&offset, &delta)` 544 and
// with the offset declared ahead of scan 2 544 (same code each). Body unchanged.
// M3 lane 7 r218 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418): scan 2's distance through a
// file-local inline helper that forms the difference vector and takes its length_sq3f (the form several other files use)
// 417 (2094 B; its distance then sums (k + j) + i as retail; a one-caller helper, one point, not kept); that with dot3f's
// arguments swapped 417 (same code); that with scan 2 reading `source->entries[i]` 545 (2122 B, two induction pointers,
// frame 0x20). Diagnostics, not keepable (a pointer to the whole offset written through): lane 5 r226's mixed store (x
// direct, y and z through the pointer) with that helper distance 417 (2112 B, frame 0x28: delta loads k, i, j and the
// distance's (k + j) + i are retail's, x stored before the `jb`; the projection sums (j + k) + i where retail sums
// (k + j) + i, one pointer, no pass counter); with the entries-field scan 761 (2320 B, frame 0x2c, every x store makes the
// entries field be read again). Body unchanged.
// M3 lane 6 r235 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, aligned 273, masked 262): scan 2's
// distance as distance_sq3f(position, sample) 541 (2122 B, frame 0x18, `nearest` then lives in memory); scan 1's the same
// way 448 (2094 B); both 553 (2122 B); scan 2's distance as length_sq3f(&offset) re-measured 646 (2039 B); scan 2's
// distance from `source->entries[i]` with the offset from the samples local 645 (2199 B, masked 191: two induction
// pointers as retail); the offset from `source->entries[i]` with the distance from the local 644 (2193 B). Body unchanged.
// M3 lane 3 r303 (h2m fast with 0x24550, 0x22750 and 0x22c30 in the image, all MATCH unless noted; control 418, aligned 273;
// a run that lists only this address leaves 0x24550 out of line and reads 1676 to 1794 B, so those are not counted): the
// shared 0x24550's sum written as one expression in four term orders (i, j, k; j, i, k; k, i, j; through length_sq3f) 461
// each (aligned 227: scan 2 then sums (k + j) + i as retail, scan 1 too where retail has (k + i) + j; 0x24550 itself 9 and
// matched 0x19ec40 11, so not kept: retail's scan 2 does not go through 0x24550); scan 2's distance through a plain
// squared-length function defined in another file, so only the link step inlines it, on the offset 646 (2039 B), with the
// entries field read per use 628 (2057 B) (the header helper's counts: no vector home either way). On the listed
// entries-per-use form with a second difference vector (544 re-measured at 2122 B, aligned 198; its scan 1 keeps
// `nearest` in memory with the loop limit in EDI): the skip as `nearest != i` 543 (aligned 195); `first` formed ahead of
// the delta and the delta built from it 549 (2112 B, aligned 191); no `samples` local 544 and the opposite pair declared
// ahead of the delta 544 (same code each); the delta read from `source->entries[nearest]` 619 (2180 B). Reading of retail's
// scan-2 latch: the pass counter and the count are reloaded into EDI and EBX for the step and `nearest` is then reloaded
// into EDI from its home ahead of the back jump, so `nearest` keeps EDI through both scans; on the 544 form our build
// loads it at the loop top into EBX instead and never gives it a register in scan 1. Body unchanged.
// M3 lane 16 r184 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, aligned 273). The caller passes
// the structure's sample block (count at +0, address at +4), so the entries field read at each use is the block's
// element read; that form (549 re-measured, 2122 B, aligned 223, masked 122) has retail's scan-2 loop skeleton. Reading
// of the register allocator's ranking on that form: the whole-range `nearest` candidate scores below zero once the
// scan-1 index has its register (its five scan-1 updates then count nothing, and it is charged for every block it only
// passes through), so it is dropped; the part of `opposite` that spans the remainder loop and the tail (29) then takes
// EDI ahead of the pass temporaries (26) and of the four scan-2 compares of `nearest` (18, EBX). In the kept body the
// same candidate scores 24 with its scan-1 updates counted and takes EDI. A forced ranking of `nearest` ahead of them
// (diagnostic, not source) brings that form to 346. The offset's x store: with the offset's members stored directly the
// 12-byte home and retail's frame order appear (opposite, nearest, count, samples, offset, delta; the squared limit in
// the source argument's cell), but any member store that stays in memory makes the entries field be read again, with or
// without the offset's address taken. Scan 2 on a const reference to the samples local: with member stores 433 (same
// code as the local), with the difference helper 418 (same code), with length_sq3f(&offset) 649. Scan 2 on the entries
// field at each use (diagnostics, helper bodies written out): member stores with the dot written out and the helper
// distance 859 (2366 B, frame 0x30), the same with a `real[3]` local 859, with dot3f on a cast of that array 859; with
// the length written out from the members as well 572 (2109 B) for the vector and for the array. Body unchanged.
// M3 lane 6 r240 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH each time; control 418; [n] = aligned rows by this
// round's count, control [208], the entries-per-use scan 2 (549) [193]). Reading: retail's dot block (the y and z products
// offset-first, the x product delta-first), the offset's x store ahead of the `jb`, frame 0x2c, both scan-2 pointers and the
// pass counter all follow from one ordering of the forwarded member values: the offset's first member has to be created ahead
// of the delta's first member, and the delta's j and k ahead of the offset's j and k. A helper store at offset 0 of a local is
// resolved one pass ahead of the stores at +4 and +8, so with both vectors through vector3d_from_points3d the order is
// delta.i, offset.i, delta.j, delta.k, offset.j, offset.k (all three products offset-first); members stored directly in this
// function are numbered by first reference and rank below every forwarded value (all three products delta-first).
// Diagnostic (a dead store, not keepable): `offset.i = 0.0f` ahead of the delta on the 549 form 564 (2130 B, frame 0x2c,
// [101]: scan 2 is retail's row for row but for `nearest` in memory, the opposite and nearest homes exchanged and the distance
// summing (k + i) + j); the same idea with both vectors by member stores on the samples local 417 (2112 B, frame 0x28).
// Lead (a new one-caller helper, so match-only; patch build/r240/lead-22cf0-401.patch): the delta through a file-local
// forceinline wrapper taking (sample, position, out) that calls vector3d_from_points3d, which delays the delta's first
// member by that one pass, with `first` formed ahead of the delta and passed to it, no samples local and scan 2 on
// `source->entries[i]`: 401 (2124 B, frame 0x2c, [102]). Around it: the wrapper without `first` ahead 564 (2130 B, [101], the
// dead-store code); the wrapper for both vectors 554 and no wrapper with `first` ahead 554 (2112 B, frame 0x20, same code).
// On the lead: `second` as NULL then `if (opposite != NONE)` 401 (same code); opposite declared with nearest 502; nearest
// after the closest distance 402; one index for both scans 401 (same code); the distance as length_sq3f of a second
// difference 550 (2112 B, frame 0x20, the home is gone). On the 564 form: length_sq3f(&offset) 628 (2057 B); a second
// difference through the wrapper or the helper 545/545 (frame 0x20); opposite declared with nearest 511; `second` as NULL
// then if 547 (2125 B). Still differing on the lead: the count test shares a zero register with the scan-1 start
// (`xor eax, eax; cmp ebx, eax`), `nearest` has no register in either scan (retail EDI with its home at +0x14; ours home
// +0x10 and EBX loaded per pass), the unroll limit takes EDI for EBP, and the distance sums (k + i) + j; 70 forced single
// register picks leave it at 395 or more.
// Also measured: both vectors by member stores with the offset declared first 418 (same code as the helpers); the offset by
// member stores declared at function scope 433 (as in the loop); the delta alone by member stores 418 (same code); on the 549
// form one vector for the fallback and the offset 593 (2146 B), scan 1 through that shared offset with length_sq3f in both
// scans 635 (2057 B), the offset declared at function scope 549 (same code), a dead `offset = delta` 549 (same code), the
// delta formed in the offset and copied 584 (2139 B); the projection as one expression of the point differences 788 (2311 B)
// on the entries field and 530 (2032 B) on the samples local; the candidate test in a forceinline helper with its own offset
// (members assigned, length_sq3f) 628, the projection alone in one 575 (2135 B); the delta formed inside scan 2 after the
// offset 571 (2198 B, not hoisted) on either base; offset and delta as one two-element array, offset first, 593 (2146 B) on
// the entries field and 433 on the samples local; the delta at +4 of a local aggregate 549 (diagnostic). Body unchanged.
// The 0x2c frame, second induction address and first-component spill are still missing. Reusing the candidate
// with the ordered dot product, nested/skip: shared difference 646/646, live entries 623/623,
// scan-scope offset 646/646, aggregate difference 649/649. The original second scan is restored.
// M3 lane GB r1 (fast with 0x22750, 0x22c30 and 0x24550, all MATCH each run; control 418, 2094/2122 B, frame 0x1c).
// Tried 15 forms: 3-pass component loops for delta on samples 766 (2286 B, 0x24), on source->entries with first ahead
// 807 (2317 B, 0x2c), with threaded second tail 807, with samples kept 789 (0x28), with reversed distance_sq3f 879;
// 3-pass component loop for offset on samples 1184 (1467 B, suppresses 4x unroll), on entries 1239, both 1126, delta
// loop + dvec loop on entries/samples 1148/1112; delta via existing vector3f_assign on samples 418 (2094 B, al 266/255),
// on entries with first ahead 554 (2112 B, 0x20, al 162/115), aggregate init on entries 554 (same code), assign + second
// vector3d_from_points3d + length_sq3f on entries 558 (al 196/116) and on samples 416 (2094 B, al 249/247; 2 pts, NOT
// kept). Body unchanged.
// The second scan still lacks its separate candidate-X store and alternating induction addresses.
// Shared offset with the two squared-length helpers 646/646; separate offset in opening sum order 418/418
// with either dot operand order; separate offset in k/j/i sum order 416/416, NOT kept: changed grouping and
// two-point gain. Squared-length helper for nearest alone or both scans 418/418.
// The 0x1c frame still lacks the candidate-X home and second induction address. Exact-count bounds
// suppress the unrolling of the affected scans. Branch and candidate-position forms shorten the body and
// raise the score. first exact count 924; second exact count 1236; both exact counts 1609; three-way
// sample branch 606; four-way sample branch 606; shared initial distance limit 418; winner-first
// comparisons 418; shared candidate position 593.
// M3 lane 2 r149 (h2m fast with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, aligned 197, registers and frame
// masked 134). Retail reads: the source parameter's stack cell is reused for the opposite distance after the samples
// pointer is loaded, `first` (the nearest sample's address) is formed ahead of scan 2 and held in EBP for the fraction
// call, the count has a home re-read at each unrolled step, and offset.i is stored to its home each pass and never read.
// Forms: the opposite distance declared ahead of the samples local and the delta 419 (masked 133); the delta declared at
// the top of the sample branch 418 (same code); the samples local declared ahead of scan 1 and used there too 623 (2071 B);
// scan 2's offset built through the existing vector3f_assign 418, the delta through it 418 (same code each). A forced-
// decision search (14 links) gives at best 409 (one pick), the frame staying 16 bytes short.
// M3 lane GA r7 (fast with 0x22750, 0x22c30, 0x24550, all MATCH; control 418 at 2094 B, fr 0x1c, al 137, masked 197; 22
// real trials). New structural findings: (1) nested `if (opposite != NONE) { second = &samples[opposite]; if (second)
// fraction = ...; }` matches retail's exact double-`je` tail block (eliminating `xor ebx, ebx; test ebx, ebx`): 440 (2088 B,
// al 137, masked 181), with `closest_distance` before `nearest` 438 (al 136, masked 179). (2) Existing `scaled_difference3f`
// (vector_products.h) for `delta` naturally delays delta.i without a one-caller wrapper, recovering retail's `delta` k/i/j
// loads, single delta.k spill, `other_distance` in the source arg slot, and `offset.i` store before `jb`: on `samples` 443
// (2111 B, fr 0x24, nearest stays in EDI), with `first` ahead + nested second 438 (2112 B, fr 0x28, masked 178, nearest in
// EDI), + second diff + length_sq3f 440 (2112 B, fr 0x28, masked 179: also retail's (k+j)+i sum with nearest in EDI), +
// `closest_distance` before `nearest` 442 (fr 0x24); on `source->entries[i]` + `first` ahead 582 (2140 B, +0x9, exact fr
// 0x2c with both scan-2 pointers and spilled pass counter, but nearest drops off EDI), with ternary second 582, with
// `samples` outside scan 2 582, with second diff + dot3f(&d,&d) 584 (fr 0x2c), mixed entries/samples 702/693. Also tried:
// `dot3f(&offset, &offset)` on samples 646, on entries 635, reversed dot + self-dot + nested second 655; `first` after
// `samples` 439, + delta k/i/j 463, + offset j/k/i 436 (2128 B, fr 0x28, masked 184); `opposite` before `samples` 596;
// `fraction` ternary 439; `sample` ptr in scan 2 439; `vector3f_add_scaled` for delta 418 (same code). Body unchanged.
// Opposite result using the count as sentinel 420/438, the nearest ordinal as
// sentinel 418/443, a one-based ordinal 281/407, or the distance improvement
// 427/426 (late/early anchor). NOT kept: the one-based form changes the retail
// NONE test and ordinal, and all forms retain frame 0x1c without the X spill.
// The second scan still needs the 0x2c frame and the candidate X store. Short-circuit projection and retained
// displacement norm 646, with the squared-length helper 646 or a second dot 646; cutoff on rejection 590;
// conditional winner and cutoff updates 646; named squares 646, named projection products 646; nearest skip
// and named projection 646. NOT kept: the reused displacement shortens the loop and drops the needed home.
// The second scan still lacks its candidate-X store and second induction address. Shared nearest and fallback
// direction with component stores, setter, entry default or initialized value 418/419/593/389. Earlier anchor
// with component stores or setter 439/439, also with a scoped opposite search 439/439. NOT kept: the 389
// form reaches retail length and frame 0x28, but adds a second fallback vector and copy absent in retail.
// The second scan still lacks the 0x2c frame and its distinct offset X spill.
// Anchor X before two- coordinate subtraction with the cached sample array 433, anchor X before two-
// coordinate subtraction with live entries in the opposite scan 555, anchor X after two-coordinate
// subtraction with the cached sample array 433, anchor X after two-coordinate subtraction with live
// entries in the opposite scan 555, complete anchor setter from X and pair with the cached sample array
// 579, complete anchor setter from X and pair with live entries in the opposite scan 578, complete
// anchor initializer from X and pair with the cached sample array 433, complete anchor initializer from
// X and pair with live entries in the opposite scan 555.
// NOT kept: The cached-entry forms retain 0x1c. Live entries reach 0x20 and one family reaches 2122
// bytes, with higher scores.
// M3 lane GA r11 (fast with 0x22750, 0x22c30, 0x24550, all MATCH; control 418 at 2094/2122 B, fr 0x1c; 14 trials):
// struct-copying delta (`delta_raw` -> `delta`) on samples 439 (2122 B, fr 0x24, mk 152; recovers retail's `offset.i`
// dead store before `jb` without a one-caller helper, though all 3 delta components spill), with first ahead + nested
// second + closest first 443 (fr 0x24, mk 143), on entries + first ahead (copy/assign) 572/572 (fr 0x28), member copy
// on samples/entries 439/554 (folds); `point3f first_pos = first->position` copy on samples/entries 417 (fr 0x28)/572
// (fr 0x2c, +0x9); `*position` copy for delta on samples/entries 418 (fr 0x28)/575 (fr 0x2c, +0x9); existing
// `vector3f_negate_ab` on samples/entries 400 (2122 B, mk 119, non-retail `0.0f -` ops)/569; existing
// `particle_direction_squared_2b` on offset 646, on dvec + delta_raw copy 442 (fr 0x24). Body restored at 418.
// M3 lane GA r13 (40 compiled trials across 3 batches, fast with 0x22750, 0x22c30, 0x24550, all MATCH; control 418 at
// 2094/2122 B, fr 0x1c, al=256, mk=255; body unchanged): short-circuit `if (!second || (fraction = ...) <= 0.0f)` 426 on
// base, 425 first ahead, 545 on entries (fr 0x20, 2p); `scaled_difference3f` for offset 1231/1203 (suppresses unroll), for
// delta on entries 582 (fr 0x2c); `s1_samples` in scan 1 + entries in scan 2 602 (fr 0x24, edi=1 & 2p=1, changes ABI to
// ret 0x14); `vector3f_scale(1.0f)` after delta 545; `basis_dot_xzy` 449. New lead (zero new helpers, NOT kept unmatched
// because `(vector3f const *)` casts with `-1.0f` only steer): existing `vector3f_add_scaled(position, &first->position,
// -1.0f, &delta)` delays `delta.i` by one pass (folding `-1.0f` to `subss`) and on `source->entries[i]` + `first` ahead
// matches retail's exact `0x2c` frame, `delta` k/i/j loads (`xmm7/5/6`, only `delta.k` spilled at +0x38), scan-2 dot
// operand sides, dead `offset.i` store at +0x24 before `jb`, dual induction pointers, and spilled pass counter: 401
// (2124/2122 B, +0x9, al=169, mk=75; nested/one-idx/frac-before-second/count1-else 401 each; short-circuit 530; closest
// first 402; first after 549; `count >= 1` 345 at mk=65; `length_sq3f(&offset)` 635), with second diff + `length_sq3f` 402
// (al=139, mk=60: entire scan 2 and tail match retail except `nearest` staying off EDI; `nearest != i` / `if (opposite !=
// NONE)` / while / `count1` else / `samples` for first+second / `if (count > 0)` / dot swap 402 each; late assign 403
// mk=58; mixed offset/diff 644/661); on `samples` + `first` ahead 406 (2108 B, fr 0x28, mk=65, edi=1, ox=1; nested 429;
// with second diff 399, mk=50; first after 418); `opp` before scan 1 502 (2122 B, fr 0x2c, edi=0 in unrolled scan 1).
// M3 lane 7 r230 (h2m fast with 0x22750, 0x22c30, 0x24550, all MATCH, and the caller 0x23690 at 354; control 418):
// scan 2 skipping the nearest sample with `continue` 418; scan 1's distance declared once ahead of the loop 418; the
// offset declared once with delta ahead of scan 2 418; both 418 (same code each); scan 1's distance as
// distance_sq3f(position, sample) 448. A forced-ranking search (diagnostic, 12 decisions) moved the aligned distance only
// 448 -> 433, so no single allocator decision gives retail's 0x2c frame. Body unchanged.
// M3 lane 16 r193 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, 2094/2122 B, frame 0x1c, aligned
// 273, masked 262): scan 2's dot as dot3f(&offset, &delta) 418 (same code); delta from source->entries[nearest] 418 (same
// code); scan 2's distance as distance_sq3f(position, sample) 541 (2122 B but frame 0x18); a per-sample pointer local
// in scan 2 for the offset and the distance 593 (2076 B); scan 1's distance as distance_sq3f(position, sample) 448.
// M3 lane 3 r314 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418, masked-aligned 262): reading
// `source->entries` at every use as scan 1 does (no `samples` local) 549 at retail's 2122 B (masked-aligned 122, scan 2
// gets retail's two induction pointers; frame 0x20 against retail's 0x2c, where offset and delta are two 12-byte homes
// with delta.k spilled and offset.i stored each pass, and `nearest` leaves EDI in scan 1); that with `first` formed ahead
// of scan 2 and the delta taken from it 554 (2112 B, masked 116); with `dot3f(&offset, &delta)` 549 and with `offset`
// declared beside `delta` 549 (both the same code); with the scan-2 distance as `length_sq3f(&offset)` 628 (2057 B). A
// forced-ranking search on the first form (30 single decisions) moved neither the frame nor the count. Body unchanged.
// M3 lane 4 r269 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418). Retail forms scan 2's x address
// from one induction pointer and the y/z addresses from the other, as scan 1 does; the planar idiom tried for that split:
// the offset's (x, y) through vector2d_from_points2d on the planar views with k written 563 (2095 B); x written with
// (j, k) through vector2d_from_points2d on the (y, z) pairs 540 (2125 B, frame 0x18), that with length_sq3f(&offset) 646
// (2039 B); on the entries-per-use scan 2 (re-measured 549): the planar (x, y) 458 (2122 B, frame 0x20, `nearest` loses
// its scan-1 register), x written with the (y, z) pair 856 (2367 B, frame 0x30). Body unchanged.
// M3 lane GB r35 (24 valid fast trials with 0x22750, 0x22c30, 0x24550, all MATCH; control 418, 2094/2122 B, +0x2, fr 0x1c,
// al 256, mk 255; body unchanged): non-const samples 418; first before samples 439; copy_light_sample plain inline 826
// (1869 B); direction lerp via vector3d_from_points3d 418; scan-2 `(k*k + j*j) + i*i` on offset on samples 646 (2039 B),
// on entries + first ahead 635 (2051 B, fr 0x20, mk 99); entries + first ahead + brace-init delta 554 (2112 B, fr 0x20,
// mk 115); count via s_light_sample_source pointer 418; reversed distance compares on samples 418, on entries + first ahead
// 554; nested `if (opposite != NONE)` on samples 440, on entries + first ahead 554, on entries + late first 554 (mk 114);
// scan-1 while on entries + first ahead 554; scan-1 entry pointer 418; const entries pointer in s_light_sample_source 418;
// samples for delta/first/second with source->entries in scan 2 549 (2122 B, fr 0x20, mk 121: entries in scan 2 still
// drops nearest off EDI); offset via vector3f_assign or real_point3d_set on entries + first ahead 781 each (2307 B, fr
// 0x30); copy_light_sample long order 418, const sample ref 418; `fraction > 0` / `< 1` nesting 416 (2111 B, al 418, -2
// under bar); fallback via vector3f_assign 419; local bsp pointer for count 418.
// M3 lane 12 r200 (H2M_FAST with 0x22750, 0x22c30 and 0x24550, all MATCH; control 418). Allocator reading: scan 2's
// three dot products are live together here (the i product formed before the first add), so only delta.i and
// delta.k get XMM registers and delta.j is read from memory; retail adds the j and k products before it forms the i
// product, keeps all three delta members in XMM5-7, and that pressure spills offset.i and the pass counter into its
// 0x2c frame. Forms: one best-distance variable for both scans 418 (same code); scan 1 as a do-while 971 (1724 B,
// not unrolled); the count == 1 copy as an if/else around the scans 418 (same code); scan 2 under a redundant
// `if (0 < count)` 585 (2100 B); `first` formed ahead of scan 2 on the samples local with the delta from it 439
// (2088 B). Body unchanged.
// @retail 0x22cf0
void __stdcall function_22cf0(s_light_sample_source const *source, real *red, real *green, real *blue, byte order,
    vector3f *direction, point3f const *position)
{
    long count = *(long *)((byte *)g_4e0344->bsp + 0x38);
    if (count > 0)
    {
        if (!g_5093f4)
        {
            if (count == 1)
            {
                copy_light_sample(source->entries, order, red, green, blue, direction);
                return;
            }
            long nearest = 0;
            real closest_distance = 1048576.0f;
            for (long i = 0; i < count; ++i)
            {
                real distance = distance_sq3f(&source->entries[i].position, position);
                if (distance < closest_distance)
                {
                    closest_distance = distance;
                    nearest = i;
                }
            }
            s_light_sample const *samples = source->entries;
            vector3f delta;
            vector3d_from_points3d(&samples[nearest].position, position, &delta);
            long opposite = NONE;
            real other_distance = 1048576.0f;
            for (long i = 0; i < count; ++i)
            {
                if (i != nearest)
                {
                    vector3f offset;
                    vector3d_from_points3d(&samples[i].position, position, &offset);
                    if (dot3f(&delta, &offset) <= 0.0f)
                    {
                        real distance = distance_sq3f(&samples[i].position, position);
                        if (distance < other_distance)
                        {
                            other_distance = distance;
                            opposite = i;
                        }
                    }
                }
            }
            s_light_sample const *second = opposite != NONE ? &samples[opposite] : NULL;
            s_light_sample const *first = &samples[nearest];
            real fraction = 0.0f;
            if (second)
                fraction = function_022c30(&first->position, position, &second->position);
            if (fraction <= 0.0f)
                copy_light_sample(first, order, red, green, blue, direction);
            else if (fraction >= 1.0f)
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
        else
        {
            memcpy(red, g_4b8940, sizeof(g_4b8940));
            memcpy(green, g_4b8964, sizeof(g_4b8964));
            memcpy(blue, g_4b8988, sizeof(g_4b8988));
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
	case 16: goto reset;
	case 17: goto reset;
	}
reset:
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
		dword *device = (dword *)D3D__pDevice;
		dword *push = (dword *)device[0];
		if (push >= (dword *)device[1])
			push = D3DDevice_MakeSpace();
		push[0] = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
		push[1] = 0;
		device[0] = (dword)(push + 2);
		dword flags = device[2];
		device[2] = flags & ~0x1800;
		if (flags & 0x1000)
			D3D::SetFence(1);
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
void function_1ed70(color4f const *color, s_1ed70_point const *points, short count)
{
    s_1ed70_point const *const *points_reference = &points;
    short const *count_reference = &count;
    short vertex_count = *count_reference;
    dword packed = pack_color4f(color);
    function_1ee60(2);
    dword *device = (dword *)D3D__pDevice;
    dword volatile *push = (dword *)device[0];
    if (push >= (dword *)device[1])
        push = D3DDevice_MakeSpace();
    dword swapped = (packed & 0xff00ff00) | ((packed & 0xff) << 16) | ((packed >> 16) & 0xff);
    push[0] = 0x41964;
    push[1] = swapped;
    device[0] = (dword)(push + 2);
    if (vertex_count > 0)
    {
        s_1ed70_point const *point = *points_reference;
        long remaining = (word)vertex_count;
        do
        {
            D3DDevice_SetVertexData2s(0, point->x, point->y);
            ++point;
        }
        while (--remaining);
    }
    function_1ef70();
}

// @retail 0x1ba00
void function_1ba00(byte *state)
{
    unsigned long output = 0;
    byte *definition = *(byte **)(state + 0x20);
    byte *blocks = *(byte **)(definition + 0x2c);
    dword *values = *(dword **)(definition + 0x34);
    byte *selection = *(byte **)(state + 0x24);
    *(dword *)(state + 0x14fc) = *(dword *)(selection + 0xde);
    *(dword *)(state + 0x1504) = *(dword *)(selection + 0xe6);
    *(dword *)(state + 0x1500) = *(dword *)(selection + 0xe2);
    bool final = false;
    for (long i = 0; i < (*(word *)(*(byte **)(state + 0x24) + 0xf6) >> 9); ++i)
    {
        short *entry = *(short **)(*(byte **)(state + 0x20) + 0x24) + ((*(word *)(*(byte **)(state + 0x24) + 0xf6) & 0x1ff) + i) * 2;
        state[0x1520 + i] = (byte)output;
        short source = entry[0];
        byte *block;
        if (source == NONE)
            block = blocks + ((word)entry[1] & 0x1ff) * 6;
        else
            block = blocks + (((word)entry[1] & 0x1ff) + *(long *)(state + 0x300 + source * 4)) * 6;
        dword *value = values + (*(word *)(block + 4) & 0x1ff) * 8;
        for (long j = 0; j < (*(word *)(block + 4) >> 9); value += 8, ++j)
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
    }
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
    sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
    sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
    sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_BORDER);
    sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    sampler_state_write(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
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
bool function_0bfe60(dword const *flags, long bit);

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
                    if (function_0bfe60(g_51f40c.bitsets[set], index))
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
                        dword *device = (dword *)D3D__pDevice;
                        dword volatile *push = (dword *)device[0];
                        if (push >= (dword *)device[1])
                        {
                            dword full_size = D3D::CDevice::m_PushSegmentSize;
                            push = D3D::MakeRequestedSpace(full_size >> 1, full_size);
                        }
                        push[0] = 0x817c8;
                        push[1] = 1;
                        push[2] = 1;
                        device[0] = (dword)(push + 3);
                        total = draw(slot, slot->data);
                        status = D3DDevice_EndVisibilityTest(test);
                    }
                    slot->k = PIN(total, 0, 65535);
                    if (status == S_OK)
                    {
                        dword *bitset = g_51f40c.bitsets[set];
                        bitset[index >> 5] |= 1 << (index & 31);
                    }
                }
                else
                    draw(slot, slot->data);
            }
        }
    }
}


long __stdcall function_3bf20(s_slot *slot, byte *data);
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

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
    if (index != NONE)
    {
        bool valid = false;
        bool available;
        if (g_51f408)
        {
            /* the frame counter is read before the slot is looked at */
            long counter = (long)g_485aa0;
            _ReadWriteBarrier();
            s_slot *slot = &g_51f40c.slots[index];
            if (slot->valid)
            {
                long frame = (counter + 1) % 3;
                key.amount = PIN(slot->f * g_5234c8, 0.0f, 1.0f);
                valid = true;
                available = (g_51f40c.bitsets[frame][index >> 5] & (1 << (index & 31))) == 0;
            }
            else
            {
                key.amount = 0.0f;
                available = false;
            }
        }
        else
        {
            key.amount = 0.0f;
            available = false;
        }
        function_20bb0(player, 0x20, &index, 1, true, function_3bf20);
        if (valid && !available && !(key.amount > 0.0f)) return false;
        return true;
    }
    return false;
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
void __stdcall function_34a90(long target, long blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real distortion, real scale,
    long count, bool full_surface, bool viewport_textures);

// @retail 0x22070
void __stdcall function_22070(real strength, real exponent)
{
    strength = strength > 0.0f ? PIN(pow(strength > 1.0f ? 1.0f : strength,
        exponent > 0.0f ? exponent : 0.0f), 0.0f, 1.0f) : 0.0f;
    if (strength > 0.0f)
    {
        function_14f60(0, 18);
        sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(0, D3DTSS_COLORSIGN, 0);
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
        sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
        sampler_state_write(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
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
            shl alpha, 24
        }
        g_484f68.PSFinalCombinerConstant0 = (dword)alpha;
        g_484f68.PSFinalCombinerInputsABCD = 0x8110000;
        g_484f68.PSFinalCombinerInputsEFG = 0x1100;
        function_1ccf0(&g_484f68);
        function_34a90(g_4858b8, 7, 0x1010101, false, false,
            1.0f, 0.0f, 1.0f, 1, false, false);
    }
}

typedef bool (__stdcall *t_211a0_vertex)(long, real const *, real const *, real *, long);
void function_34d90(long target, real const *weights, long blend, dword color_write,
    bool use_depth, bool depth_write, real depth, real low, real high,
    bool full_surface, t_211a0_vertex vertex);
void __stdcall function_351a0(long texture, long target);

PRIVATE __forceinline void configure_filter_textures(long const &source)
{
    for (long stage = 0; stage < 4; ++stage)
    {
        function_14f60(stage, (short)source);
        sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
        sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
        sampler_state_write(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
        sampler_state_write(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        sampler_state_write(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        sampler_state_write(stage, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    }
}

// score 4 (was 4): the source and pass-count incoming registers are reversed. Pass-count snapshot 4;
// const pass parameter 4; output snapshot 4; aggregate weights 4; pretested while 4; nested output tests 4;
// alternate/configure/radius order 33; scoped favor-size 139.
// @retail 0x21360
long function_21360(long count, long source, real radius, real low, real high, long target,
    long alternate, long output, bool configure, bool full_surface, real scale)
{
    real current_radius = radius * 0.01666f;
    if (alternate == NONE)
        alternate = source;
    if (configure)
        function_020f30(scale, 0.0f);
    for (long pass = 0; pass < count; ++pass)
    {
        real weights[4];
        weights[0] = current_radius;
        weights[1] = current_radius * 0.33f;
        weights[2] = current_radius * -0.33f;
        weights[3] = 0.0f - current_radius;
        configure_filter_textures(source);
        function_34d90(target, weights, 10, 0x1010101, false, false,
            1.0f, low, high, full_surface, NULL);
        source = target;
        target = alternate;
        alternate = source;
        current_radius *= 2.0f;
    }
    /* Copy the final pass to the requested output buffer. */
    if (output != NONE && output != source)
    {
        function_351a0(source, output);
        source = output;
    }
    return source;
}

// @retail 0x211a0
long function_211a0(long source, real passes, real distortion, real falloff,
    long target, long alternate, long output, long configure, long full_surface,
    real scale, real offset)
{
    (void)&output;
    real pass = 0.0f;
    if (alternate == NONE)
        alternate = source;
    if ((byte)configure)
        function_020f30(scale, offset);
    if (passes > 0.0f)
    {
        real growth = (1.0f - falloff) + falloff * 2.0f;
        do
        {
            real fraction = PIN(passes - pass, 0.0f, 1.0f);
            if (fraction > 0.0f)
            {
                configure_filter_textures(source);
                /* callers pass full_surface as a bool in its dword slot;
                   0x34a90 gets that bool as it is */
                function_34a90(target, 10, 0x1010101, false, false,
                    1.0f, fraction * distortion, 1.0f, 4, *(bool const *)&full_surface, false);
                source = target;
                target = alternate;
                alternate = source;
            }
            distortion *= growth;
            pass += 1.0f;
        } while (pass < passes);
    }
    /* Copy the final pass to the requested output buffer. */
    if (output != NONE && output != source)
    {
        function_351a0(source, output);
        source = output;
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
            sampler_state_write(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            sampler_state_write(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            sampler_state_write(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            sampler_state_write(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
            sampler_state_write(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
            sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
            sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
            sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
            sampler_state_write(0, D3DTSS_COLORSIGN, 0);
            sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
            sampler_state_write(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
            configure_filter_copy();
            function_34a90(5, 10, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
            long filtered = function_211a0(5, first, 1.0f, 0.0f, 6, NONE, NONE, 1, 0, 0.0f, 0.0f);
            real amount = PIN(pow(first > 1.0f ? 1.0f : first, 0.33f), 0.0f, 1.0f);
            function_14f60(0, (short)filtered);
            function_15780(0, 4);
#pragma inline_depth(0)
            if (amount < 1.0f)
            {
                function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
                function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTALPHA);
                function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVCONSTANTALPHA);
                function_0222d0(D3DRS_BLENDCOLOR, function_131fc0(amount));
                function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            }
            else function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
#pragma inline_depth(255)
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
            real amount = PIN(pow(second > 1.0f ? 1.0f : second, 0.33f), 0.0f, 1.0f);
            function_14f60(0, (short)filtered);
            function_15780(0, 4);
#pragma inline_depth(0)
            if (amount < 1.0f)
            {
                function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
                function_0222d0(D3DRS_SRCBLEND, D3DBLEND_CONSTANTALPHA);
                function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVCONSTANTALPHA);
                function_0222d0(D3DRS_BLENDCOLOR, function_131fc0(amount));
                function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            }
            else function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
#pragma inline_depth(255)
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
    real first_factor, second_factor, alpha;
    color4f color;
    first_factor = 1.0f;
    second_factor = 0.4f;
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
            if (state->value1c != -1.0f) first_factor = state->value1c;
            if (state->value20 != -1.0f) second_factor = state->value20;
        }
    }
    passes = passes > 0.0f ? passes : 0.0f;
    distortion = PIN(distortion, 0.0f, 1.0f);
    strength = PIN(strength, 0.0f, 1.0f);
    falloff = PIN(falloff, 0.0f, 1.0f);
    scale = PIN(scale, 0.0f, 1.0f);
    real first_input = first_factor;
    if (first_input < 0.0f) first_factor = 0.0f;
    else if (first_input > 1.0f) first_factor = 1.0f;
    second_factor = PIN(second_factor, 0.0f, 1.0f);
    if (strength > 0.0f || first_factor > 0.0f)
    {
        configure_filter_textures(g_4858b8);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSCombinerCount = 0x11004;
        g_484f68.PSAlphaInputs[0] = 0x18201920;
        g_484f68.PSAlphaOutputs[0] = 0x30c00;
        g_484f68.PSAlphaInputs[1] = 0x1a201b20;
        g_484f68.PSAlphaOutputs[1] = 0x30d00;
        g_484f68.PSAlphaInputs[2] = 0x1c201d20;
        g_484f68.PSAlphaOutputs[2] = 0x30c00;
        g_484f68.PSRGBInputs[0] = 0x8200920;
        g_484f68.PSRGBOutputs[0] = 0x30c00;
        g_484f68.PSRGBInputs[1] = 0xa200b20;
        g_484f68.PSRGBOutputs[1] = 0x30d00;
        g_484f68.PSRGBInputs[2] = 0xc200d20;
        g_484f68.PSRGBOutputs[2] = 0x30c00;
        color.alpha = 0.125f;
        color.red = distortion;
        color.green = distortion;
        color.blue = distortion;
        g_484f68.PSConstant0[3] = pack_color4f(&color);
        g_484f68.PSRGBInputs[3] = 0xc200140;
        g_484f68.PSRGBOutputs[3] = 0x20d00;
        g_484f68.PSAlphaInputs[3] = 0x1c110000;
        g_484f68.PSAlphaOutputs[3] = 0x10d00;
        // Retail clears the alpha conversion slot before rounding.
        alpha = 0.0f;
        real alpha_scale = 255.0f;
        long &packed = *(long *)&distortion;
        packed = 0;
        __asm
        {
            fld alpha
            fld alpha_scale
            fmulp st(1), st(0)
            fistp distortion
            shl distortion, 24
        }
        g_484f68.PSFinalCombinerConstant0 = (dword)packed;
        g_484f68.PSFinalCombinerInputsABCD = ((g_485a74 ? 0x1c : 0) << 24) | 0xf000d;
        g_484f68.PSFinalCombinerInputsEFG = ((g_485a74 ? 0x1d : 0) | 0xc1100) << 8;
        function_1ccf0(&g_484f68);
        function_34a90(7, 10, 0x1010101, false, false, 1.0f, 0.5f, 1.0f, 4, false, false);
        long filtered = function_211a0(7, passes, 1.0f, falloff, 8, NONE, NONE, 1, 0, scale, second_factor);
        function_14f60(0, (short)filtered);
        function_15780(0, 4);
#pragma inline_depth(0)
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_SRCBLEND, blend ? 0x307 : 1);
        function_0222d0(D3DRS_DESTBLEND, !preserve);
        function_0222d0(D3DRS_BLENDOP, 0x8006);
#pragma inline_depth(255)
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        color.alpha = first_factor;
        color.red = strength;
        color.green = strength;
        color.blue = strength;
        g_484f68.PSConstant0[0] = pack_color4f(&color);
        g_484f68.PSRGBOutputs[0] = 0x10c00;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        g_484f68.PSRGBInputs[0] = (g_485a74 ? 0x11 : 0) | 0x8011800;
        function_1ccf0(&g_484f68);
        function_34a90(g_4858b8, NONE, 0x1010101, false, false, 1.0f, 0.0f, 1.0f, 1, false, false);
        if (g_4858b4 != NONE)
            function_14bc0((short)g_4858b4, 0, true);
        else
            function_14bc0((short)g_4858b8, 0, true);
    }
}
