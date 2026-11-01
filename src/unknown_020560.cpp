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

// score 4 (was 4): Frame agrees; the final slot-bit clear uses EDX instead of EAX. Tried unsigned slot bit 4;
// signed traversal mask 4; unsigned slot index 4; toggle selected bit 14; subtract selected bit 16; assign
// remaining mask 4; pending slots name 4; for mask traversal 4; named slot bit 4; bias declaration first 4;
// slot index name 4; texture frame name 4; lowest-bit clear 19;
// early clear 111; entry lifetime 4; scan-index lifetime 4; level lifetime 4; integral
// level clamp 86; short bitmap 4; typed texture entry 4; guarded do continuation 4; reversed mask
// assignment 4; complement by -1 xor 4; masked xor clear 4; short stage 4; entry tag local 4;
// existing bit setter 132; callee inline declarations 4; ternary bitmap 104; one-expression level
// 202; one conditional bias 115; bitmap set on both arms 90; named frame time 108. Compile-only
// screens: only removing the whole bitmap block gives EAX; ~450 forms of that block keep EDX.
// Round 206: update noinline contract 4; level clamp arms 4; adjusted clamp arms 4; both clamps 4;
// real remainder 4; double remainder 4; integral bias floor 4; both floor boundaries 4.
// Explicit floor result 85; native SSE floor helper 124. Original helper restored.
// Round 69: bitmap/bias at function scope 4/4; reversed level compare 4; `>=` magnitude test 90;
// tag local for the NONE test 4. Harness: EAX only when the bitmap is a constant zero; a constant
// bitmap in the arm, an unconditional bitmap or a call without it keep EDX.
// Round 123: bit scan in an inline index helper 4; bit scan split into two asm statements 4;
// bias assigned in the arm without its zero default 65; ignored call result named 4; bitmap
// narrowed to short at the floor 16. Harness: the second floor's asm decides it (an asm copy
// that clobbers EDX in place of ECX gives EAX here).
// Round 10: integer level index before its float 4; const floor parameter 4; named bit before the
// clear 4; element address as &state[...] 4; definition count read inline in the floor call 4.
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
            level = PIN(level, 0.0f, 2.0f);
            level = (real)(long)level;
            if (*(real *)(entry + 0x10) > level)
            {
                function_143c0(*(long *)entry, (short)bitmap, (short)index, bias);
                *(real *)(entry + 0x10) = level;
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

PRIVATE __forceinline void copy_light_sample(s_light_sample const *sample, byte order, real *red, real *green, real *blue, vector3f *direction)
{
    long bytes = order * order * sizeof(real);
    memcpy(red, sample->red, bytes);
    memcpy(green, sample->green, bytes);
    memcpy(blue, sample->blue, bytes);
    *direction = sample->direction;
}

// score 589 (was 589): Frame 0x18/0x2c, input/selection homes, override layout and fraction retention differ.
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
        s_light_sample const *samples = source->entries;
        if (count == 1)
        {
            copy_light_sample(samples, order, red, green, blue, direction);
            return;
        }
        long nearest = 0;
        real closest_distance = 1048576.0f;
        s_light_sample const *nearest_sample = samples;
        for (long i = 0; i < count; ++i)
        {
            real x = position->x - nearest_sample->position.x;
            real y = position->y - nearest_sample->position.y;
            real z = position->z - nearest_sample->position.z;
            real distance = z * z + x * x + y * y;
            if (distance < closest_distance)
            {
                closest_distance = distance;
                nearest = i;
            }
            ++nearest_sample;
        }
        s_light_sample const *first = samples + nearest;
        vector3f delta;
        delta.i = position->x - first->position.x;
        delta.j = position->y - first->position.y;
        delta.k = position->z - first->position.z;
        long opposite = NONE;
        real other_distance = 1048576.0f;
        for (s_light_sample const *opposite_sample = samples;
             opposite_sample < samples + count; ++opposite_sample)
        {
            if (opposite_sample != first)
            {
                real x = position->x - opposite_sample->position.x;
                real y = position->y - opposite_sample->position.y;
                real z = position->z - opposite_sample->position.z;
                real projection = delta.k * z + delta.j * y + delta.i * x;
                if (projection <= 0.0f)
                {
                    real distance = z * z + y * y + x * x;
                    if (distance < other_distance)
                    {
                        other_distance = distance;
                        opposite = opposite_sample - samples;
                    }
                }
            }
        }
        real fraction = opposite != NONE
            ? function_022c30(&first->position, position, &samples[opposite].position) : 0.0f;
        if (fraction <= 0.0f || opposite == NONE)
            copy_light_sample(first, order, red, green, blue, direction);
        else
        {
            s_light_sample const *second = samples + opposite;
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

// score 52 (was 52): the zeroed register is EDI in ours and EBP in retail, and the block the
// test jumps to is laid out differently.
// The code is retail's apart from the colours (loop counter and total
// EDI where retail has EBP, the test index EBP where retail has EDI) and ADD for retail's LEA
// of the bitset row; push pointer without volatile 56; device as a field pointer 56 (52 with
// volatile); total zeroed in an else arm 139; one combined query-result test 52;
// &bitsets[set][0] 52; slots + index 52; the push expansion in a forced-inline helper 52.
// Round 271: plain completed-pointer/per-store packet advancement crossed with
// long/short remainder and lower/upper-first saturation: 56/103/137/131/92/117/128/140.
// All restored; loop/count and test-index colours plus bitset row address remain.
// Lane 8 r79: non-query draw first 140; bitset row declared between test and status 54; device
// global at each use 93; bitset row global at each use 122; `(1 << slot->c) & mask` 52.
// Lane 13 r14: retail gives the test index EDI and the device pointer EBX (status's register),
// leaving EBP to the loop counter and the total; ours puts the counter, device and total in EDI and
// the test index in EBP. The total declared at the top of the query block 139 (441 bytes); status
// declared before the test index 53; `set + index * 3` 52; the test index written at both calls 142;
// the clamped total stored in the arm with an else storing 0 132 (443 bytes).
// Round 23 (lane 10): the SDK's begin-visibility-test call for the push expansion 158 (stays out
// of line); the index from an if instead of the ternary 55; the result locals at the query block
// 52; the clamp as if/else 52; a const set 52. Without LTCG every spelling of the bitset row
// (row pointer, &row[0], flat index, pointer arithmetic, const, a separate global or struct row)
// gives ADD; retail's LEA costs the one byte that shifts the exits.
// Round 49 (lane 12), screened in an LTCG mini-link with the D3D objects (it reproduces ours but for
// 0xbfe60's bitset argument, so only differences count): loop counter, index or set declared at the
// top no change; bitset row declared before the test index +17; the index from an if +3; status as
// long no change; a while loop +5.
// Round 7 (m3 lane 2): retail's LEA comes back when the row is first computed as 0xbfe60's argument
// (`function_0bfe60(g_51f40c.bitsets[set], index)`) and the store goes through a row pointer named at
// the store (CSE'd and spilled as retail): 38, only the colours left (a mixed read, kept only on a
// match). With that form: status declared first 39, `set + index * 3` 38, index from an if 41; the row
// at both uses 122 and through the existing bit_vector_set 122 (the store folds, no spill); the row
// declared before the test index 119; the SDK begin-test call 158 (stays out of line).
// Round 27 (lane 11): status set S_OK in an else arm 139 (441 bytes); the loop counter declared
// before the loop 52; the push pointer read as `*(dword volatile **)device` 52; the test index written
// at both calls 142 (as lane 13); status declared before the test index 53; `slot->b == player` 52;
// `player == g_5234c4` 53.
// Round 108 (lane 6), on round 7's 38 form (the row passed straight to 0xbfe60, named at the store;
// only the colours left: counter/total EDI where retail has EBP, test index and player EBP/EBP
// where retail has EDI/EBX, the device EDI where retail reuses status's EBX): the loop counter
// declared before the loop 38; `slot->b == player` 38; the index set to the counter and then
// overwritten from the list 41; the test index computed before the query branch 49; the push
// pointer without volatile 42; a count local (`indices ? count : g_51f40c.count`) in place of
// reassigning the parameter 47. Retail inlines
// the SDK's begin-visibility-test body here, which our link keeps out of line.
// Round 31 (lane 10): on the round-7 row form (38) the rest is one register permutation (retail
// counter/total EBP, player EBX, slot fields and test index EDI, device EBX; ours EDI, EBP, EBX,
// EDI). The total as short or word 125/125 (the clamp re-extends); the row named at the loop top 85;
// round-7 form with a short total 135; with the mask tested before the player 135; with the slot
// tests as a `continue` filter 38. A one-file harness with the D3D objects linked is not exact (60:
// it cannot reproduce MakeRequestedSpace's register argument).
// Round 28 (lane 14), `--fast`: round 7's row form rebuilt 38 (aligned 39; the pure permutation: counter/total EDI,
// test index and player EBP, slot-field temps EBX, device EDI where retail has EBP, EDI, EBX/EDI and EBX); on it:
// the test index as dword 38, the visibility-result outputs declared at the query block top 38, a const device
// pointer 38, the test index, status, total, test+status, status+test and all three declared (uninitialized) at
// function scope 38 each (symbol order does not move the colours), the test index declared after status 39, the
// total declared at the query block top 155 (437 bytes), the store through a (word) cast 38. On the kept form: a
// const row pointer 52 (same code). The XDK's D3DDevice_BeginVisibilityTest is a library function (no header
// inline); no other file writes the 0x817c8 push. The draw result in its own `drawn` local clamped into the total
// 96 (463 bytes), on the row form 136, with status declared first 136.
// Round 56 (lane 12): the SDK's begin-test call in the plain full build 158 (still out of line;
// retail holds the push header only here); the global test as an early-return guard 52; the count
// reassigned before the set 66; a pointer local to the whole visibility table for count, slots and
// row 52; the frame counter read at each use 125.
// Round 185 (lane 3, fast): on round 7's row form (38, only the colours left): `test`/`total` named
// `test_index`/`drawn` 38; `long const set` 38; the push expansion in a forced-inline helper 38; the
// three slot tests as nested ifs 38 (all the same code: the counter EDI, test index EBP, device EDI
// where retail has EBP, EDI, EBX). Round 7's form restored to the baseline body (a mixed read).
// Round 139, on the kept form: the global test split into two nested ifs 52; the visibility-result
// outputs declared before the bitset test 52; the push pointer advanced before its store 52; the slot
// tests as a `continue` filter 52; the index set to the counter and overwritten from the list 55.
// Round 141 (lane 4): retail holds the begin-test push header only here (the SDK function inlined
// by its link; the push constant appears once in the whole image). The set computed in the query
// block 108; status = S_OK at the loop body top 138; the device declared before the status test 52;
// the segment size read at both MakeRequestedSpace arguments 52; `!indices ? i : indices[i]` 118.
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
                    dword *bitset = g_51f40c.bitsets[set];
                    if (function_0bfe60(bitset, index))
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
                        dword volatile *push = (dword volatile *)device[0];
                        if (push >= (dword volatile *)device[1])
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
                        total = PIN(total, 0, 65535);
                    }
                    slot->k = total;
                    if (status == S_OK)
                        bitset[index >> 5] |= 1 << (index & 31);
                }
                else
                    draw(slot, slot->data);
            }
        }
    }
}


long __stdcall function_3bf20(s_slot *slot, byte *data);

// score 6 (was 6): only the frame counter load differs: retail loads it after the global test
// and before the slot pointer, ours after the slot's valid test (our build moves a load to its
// only use). Retail reads the counter as if it were volatile: the read through a volatile pointer
// at the top of the global block matches byte for byte (forbidden form, diagnostic only). Retail keeps the clamped amount in the dead key's second field with a store in each
// clamp arm, which a direct field store gives (the union member) and a reference to the field
// does not (it merges the stores). Tried: a plain amount 108 (frame 0x1c, own slot); the key
// in its own block 108; that and the rest under `index != NONE` 101; the positive guard alone 92;
// with a slot pointer 91; the global and valid tests nested, both else arms written out 87; the
// frame computed before the slot pointer 99; the frame read before it 93; one clamped
// assignment from a named product 87; the key built in an inline helper 108.
// Round 271: independent amount with cleared/nonzero-only or full aggregate request
// initialization, early/trailing lookup failure and direct/shared bit test:
// 119/133/120/129/135/140/131/139. All retain frame 0x1c; original body restored.
// Lane 8 r79: key and query as inline helpers 96-117; plain amount local 86 (frame 0x1c); with
// this body: the frame computed before the slot pointer 75, the counter read first 72, read as an
// __int64 copy, after the slot pointer, reassigned, 64-bit add or `+ 1` first: 6 each; permuter 6.
// Round 20: the counter read at the storage block top with the set at the arm top 6 (the load
// sinks), read before the storage test 6; written inside the bitset test 72 (the load stays, the
// divide moves after the clamp); the bit test through function_0bfe60 90; the bit test before
// the clamp 79.
// Round 23 (lane 10): the existing bit-test helper (stays out of line) 90, with the counter read
// at the top of the global block 88; the slot pointer before the global test 22; a const counter
// copy before the slot pointer 6; the whole frame expression there 75; an inline query helper
// taking the counter 106.
// J197: validity from admission, ordinary/before-slot/early-row/after-slot: 78/78/74/78;
// byte/long/short validity 78/103/106; default-false availability 79. Shared inline bit
// test on the direct-key graph 6; with early counter capture 6, row at helper argument 73,
// capture before global admission 6. Counter-load scheduling remains; opening body restored.
// J199: direct slot fields with early/use-point counter 71/71; original graph
// with default availability/amount/both 42/82/86; direct final disjunction 12;
// default-true/default-false final results 40/40. Counter load still sinks; restored.
// Round 20: the counter read at the storage block top with the set at the arm top 6 (the load
// sinks), read before the storage test 6; written inside the bitset test 72 (the load stays, the
// divide moves after the clamp); the bit test through function_0bfe60 90; the bit test before
// the clamp 79.
// Round 88: availability before validity, direct/shared bit test 77/77; blocked polarity
// and row pointer 68/68; availability before the slot 90; full counter there 90;
// inline target 6; const slot 6. Early dependencies move the divide and bit work; restored.
// Round 16: the counter's low dword read through its address at the global block top 6 (still
// moves to its use); the counter read first with the amount and availability defaults stored
// before the slot test 84 (it still moves); the counter read first with the slot test inverted 6;
// the counter read right after the lookup call 6; the global and slot tests joined by `&&` with
// one else 22 (the slot pointer is then computed before the global test).
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
            s_slot *slot = &g_51f40c.slots[index];
            if (slot->valid)
            {
                long frame = ((long)g_485aa0 + 1) % 3;
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
