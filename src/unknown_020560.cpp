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
// Round 71: the entry body in a helper keeping (state, entry, index) 415 (90 bytes, out of line), (state, index)
// 431, the part after the NONE test 409; the same three forceinline 4/4/4; forceinline bitmap getter 114 (379
// bytes), bias getter 114 (379), level helper 4, bitmap and bias getters 114, all three 114.
// Inline call-and-store helper with evaluated arguments 4; mask-without-bit helper 4; single-bit helper 4; both 4.
// Round 85: scan index initialized 53; masked shift index 18; const entry pointer 4; count declared outside
// its arm 4; adjusted value declared outside its arm 8; implicit floor conversion 4; split level initialization 4;
// named truncated level 4; ordinary inline floor helper 166. Opening body and helper declaration restored.
// Round 138: the bit-scan asm through EAX or EDX 6/6 (clear still EDX); index declared before the loop 4;
// `while (remaining != 0)` 4; remaining declared, then assigned 4.
// Round 67: bitmap declared at loop-body scope ahead of the NONE test 14, with the bias too 18, both at the top
// of the body 21; a loop-scope copy of a pending mask 38; a for loop clearing the bit in its increment with
// the NONE test as `continue` 4. Diagnostic: one to eight unused locals at entry leave EDX (symbol count not it).
// Round 132 (m3 lane 7, fast): the call's short arguments without casts 4; the level truncated in the
// clamp statement 4; an explicit return after the loop 4; the level test as `level < entry level` 4;
// bitmap's zero as the else arm of the magnitude test 90 (399 bytes). Lead: lane 8 r114 found VC's
// scratch choice a round robin (one extra early temp rotates every later block, 0x5c720); the EDX here
// may be one hidden allocation off retail rather than a property of the clear.
// Round 155 (m3 lane 5, fast): the floor's asm split after cvttss2si 4; floor parameter `real const` 4; one-line asm
// statements 4; adjusted bias as a ternary 65 (399 bytes). Permuter (600 s, 41 variants) found nothing below 4.
// Round 141 (m3 lane 8, fast with 0x143c0 and caller 0x16b90): the call cast to void 4; the remainder in a named
// local 4; the bias arm as one combined test without `adjusted` 65 (400 bytes); the entry as `&state[0x1320] +
// index * 0x40` 4; the frame value named for the magnitude test and the remainder 108 (406 bytes); a pointer to
// the entry's level used for the test and the store 4. Caution: the compile-only harness is not faithful here
// (non-LTCG /Gr passes the bitmap to 0x143c0 in EDX, so EDX is natural there; LTCG passes it in ECX), so its
// screens of the bitmap block say nothing; retail 0x143c0 itself clobbers EDX.
// Round 79 (m3 lane 11, fast): the NONE test as an early clear plus `continue` 107; the global bias at +0x2c read
// into a local before the loop 126 (414 bytes); 0x143c0's two size stores through a short pointer (callee form;
// 0x143c0 still matches) 4; the bit computed at the loop top and cleared at the end 108 (398 bytes); bitmap as dword 4. Diagnostic
// (reverted): a probe load stored to a global in the latch takes ECX and the clear keeps EDX, so EAX is held off
// at the latch rather than one rotation step behind. A one-file LTCG link with 0x143c0's file reproduces EDX but
// not 0x143c0's register convention.
// Round 165 (m3 lane 5, fast with 0x143c0 and 0x16b90): `fabsf` for the magnitude test 4; `fmodf(frame, (real)count)`
// for the frame 4 (same code). Diagnostics on the LTCG image (reverted): the clear keeps EDX with the bitmap block
// removed, the second floor removed, the call removed, a zero bitmap in the call, a void callee in place of 0x143c0,
// and with the loop body cut to one 0x143c0 call (the cut-down bodies also changed this function's frame or homes:
// `state` moved into a register, `remaining` into EBP, so they do not isolate the latch); matched 0x18e80's latch takes EAX after a call that leaves EAX
// unused. No untried natural form of the latch or the frame block is left in this note.
// Round 189 (m3 lane 4, fast with 0x16b90 and 0x16b10): the shift count as `(byte)index` 111; the render reset state
// typed (slot fields, +0x2c bias, +0x1420 slot mask) with per-use slot indexing 51 (419 bytes), with a slot pointer 4;
// an inline pin function for both clamps 4; a short stage local at the loop top 4; the floor result named as an
// integral before the clamp 4. Diagnostics: a cut-down loop (bsf, NONE test, a call, the clear) keeps EDX with an LTCG
// or XDK callee, called conditionally or not; a second use of the mask (0x18e80's state clear) gives EAX, `mov ecx` first.
// Round 78 (m3 lane 15, fast with 0x143c0, 0x16b90 and 0x16b10): `__assume(state != NULL)` at the top 4; `__assume(index
// >= 0 && index < 4)` after the bit scan 4; `__assume(entry != NULL)` ahead of the NONE test 4 (all the same code);
// `__assume(count > 0)` after the count read 89 (405 bytes: the count goes through a stack home); `__assume(bitmap >= 0)`
// ahead of the call 106 (413 bytes: the bitmap gets a stack home). Reading with round 79's latch probe: the clear's
// temporary is a next-fit choice (the probe took ECX, the clear then EDX), so ours enters the latch one step after
// EAX (the tag argument's load) and skips the shift's ECX, where retail enters it at EAX. Diagnostic (reverted): 0x143c0's
// stage parameter as `long` (definition, the six declarations and the three `(short)` call casts; every caller passes a
// long) 4, with 0x143c0 and its matched callers unchanged: the stage conversion is not the extra step. Probes (reverted):
// one or two copies between globals ahead of the 0x143c0 call leave the clear in EDX; one after the call takes ECX and
// the clear EDX; two after the call take ECX and EDX and the clear takes EAX. So the rotation restarts at the call (its
// result's EAX) and retail has two more steps than ours between the call and the clear, with no code left for them.
// Round 240 (m3 lane 3, fast with 0x143c0 and 0x16b90): the entry as `state + (index << 6) + 0x1320` 4; the clear as a
// plain assignment 4, with a `1L` shift 4; `level` declared at loop scope after the entry 4; the mask declared in a
// `for` header with an empty step 4.
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

// MiniMax D (11-05): no new form, no lower score; the argument slots agree with retail's (the source
// pointer in the first stack slot, the order byte in the fifth), so the whole difference is the frame,
// 0x18 against retail's 0x2c.
// MiniMax E (11-06): no lower score; retail takes the position pointer as its third parameter (EDX)
// and pops six stack arguments, so retail's declaration has nine parameters against our seven; the
// call sites are in another file and the parameter order may not change, so this cannot be reached here.
// score 547 (was 547): Frame 0x18/0x2c; query input, native vector/ordinal and fraction homes remain.
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
// @retail 0x22cf0
void __stdcall function_22cf0(s_light_sample_source const *source, real *red, real *green, real *blue, byte order,
    vector3f *direction, point3f const *position)
{
    long count = *(long *)((byte *)g_4e0344->bsp + 0x38);
    if (count > 0)
    {
        if (!g_5093f4)
        {
            s_light_sample const *samples = source->entries;
            if (count == 1)
            {
                copy_light_sample(samples, order, red, green, blue, direction);
                return;
            }
            s_light_sample const *first = samples;
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
                    first = nearest_sample;
                }
                ++nearest_sample;
            }
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
            if (fraction <= 0.0f)
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
