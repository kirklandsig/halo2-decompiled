// @flags /O2 /arch:SSE /Gr
#include "reset_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include <xmmintrin.h>
#include <string.h>
#include <math.h>
#include "effects.h"
#include "sampler_state.h"

struct s_packed_shader_range
{
    word packed;
    __forceinline dword count() const { return (dword)packed >> 9; }
    __forceinline long index() const { return packed & 0x1ff; }
};

struct input_mapping_entry
{
	byte type;
	signed char key;
	byte unknown2[5];
	byte flag0 : 1;
	byte flag1 : 1;
	byte flag2 : 1;
	byte flag3 : 1;
	byte flag4 : 1;
	byte flags_pad : 3;
	byte extra;
	byte unknown9[3];
};

input_mapping_entry g_43e909[0x5e];

struct input_bit_vectors
{
	dword v58, v5c, v60, v64, v68;
	dword v6c[2], v74[2], v7c[2], v84[2], v8c[2], v94[2];
	dword v9c[2], va4[2], vac[2], vb4[2], vbc[2];
};

input_bit_vectors g_485058;

struct s_render_state_source;
struct s_01b050_shader_state;
struct s_18d70_state;
struct s_shader_constant_state;
void function_16f60(byte *state, word const *range);
void function_17000(byte *state, word const *range);
void __stdcall function_18900(s_render_state_source const *state, word const *range);
void function_189a0(byte const *state, word const *range);
void function_1ba00(byte *state);
void function_18a90(byte *state, word const *descriptor);
void function_18d70(s_18d70_state *state);
void function_18e80(s_shader_constant_state *state);
void __stdcall function_1b0a0(byte *state);
void function_1cf50(void);
void function_1b050(s_01b050_shader_state *state);
void function_17170(byte *state, word const *range);
void function_174d0(byte *state, long group, long stage, long pass, long entry);
void function_17420(byte *state, long stage, long group, long pass, long variant);
void function_1ae70(byte *state);
void __stdcall function_19be0(byte *state, word const *range);
void function_19ca0(byte *state, word const *range);
void __stdcall function_1a170(byte *state, s_packed_shader_range const *range);
void __stdcall function_1ab50(byte *state, s_packed_shader_range const *range);
void __stdcall function_18ee0(byte *state);

PRIVATE inline byte const *material_parameters(long tag)
{
    dword group = ((dword *)g_4e3b44)[(short)tag * 4];
    if (group == 0x5052544d || group == 0x70727433)
        return (byte const *)function_137bd0(tag)->function_x947334();
    return *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
}

// score 167 (was 167): frame/entry agree; logarithm width, reset and restore scheduling differ.
// Factor first 167; inline depth one 167; scoped size 483; dirty-mask order 167; packed-range view 167.
// logarithm width/reset schedule and restore/final pointer allocation;
// final record lifetime 167; common update 263; retained old tag 241; records-first test 169;
// promoted inverse 248; float consistency 306; coordinated texture inputs 167.
// @retail 0x16b90
void __stdcall function_16b90(byte *state, long tag, long first, long second,
    long third, long fourth, real scale)
{
    byte const *parameters = material_parameters(tag);
    byte *definition = g_4e3b44[*(long const *)parameters & 0xffff].bytes;
    byte *tables = *(byte **)(definition + 0x5c);
    byte *entry = *(byte **)(tables + 4) + first * 10;
    word packed = *(word *)entry;
    word next = ((word *)*(byte **)(tables + 0xc))[(packed & 0x1ff) + second];
    byte *pass = *(byte **)(tables + 0x14) + ((next & 0x1ff) + third) * 10;
    byte *pass_definition = g_4e3b44[*(long *)(pass + 4) & 0xffff].bytes;
    byte *records = *(byte **)(pass_definition + 0x20);
    byte *record = *(byte **)(records + 4) + fourth * 0x132;
    bool changed = record != *(byte **)(state + 0x24) || records != *(byte **)(state + 0x20);
    long previous = *(long *)state;
    bool different_tag = tag != previous;
    *(long *)(state + 4) = previous;
    *(long *)state = tag;
    real inverse;
    if (scale > 0.0f) inverse = 1.0f / scale;
    else inverse = 100000.0f;
    *(real *)(state + 0x28) = inverse;
    *(real *)(state + 0x2c) = (real)(log((double)inverse) * 0.6931471824645996f);
    *(dword *)(state + 0x58) = 0;
    state[0x1514] = false;
    *(dword *)(state + 0x1518) = 0;
    *(dword *)(state + 0x151c) = 0;
    if (changed || different_tag)
    {
        parameters = material_parameters(tag);
        *(byte const **)(state + 0xc) = parameters;
        definition = g_4e3b44[*(long const *)parameters & 0xffff].bytes;
        tables = *(byte **)(definition + 0x5c);
        *(byte **)(state + 0x10) = tables;
        entry = *(byte **)(*(byte **)(state + 0x10) + 4) + first * 10;
        *(byte **)(state + 0x14) = entry;
        packed = *(word *)entry;
        next = ((word *)*(byte **)(*(byte **)(state + 0x10) + 0xc))[(packed & 0x1ff) + second];
        pass = *(byte **)(*(byte **)(state + 0x10) + 0x14) + ((next & 0x1ff) + third) * 10;
        *(byte **)(state + 0x18) = pass;
        byte *range = *(byte **)(*(byte **)(state + 0x10) + 0x1c) + ((*(word *)(pass + 8) & 0x1ff) + fourth) * 6;
        *(byte **)(state + 0x1c) = range;
        *(dword *)(state + 0x30) |= g_485058.v6c[0];
        *(dword *)(state + 0x34) |= g_485058.v6c[1];
        *(dword *)(state + 0x38) |= g_485058.v9c[0];
        *(dword *)(state + 0x3c) |= g_485058.v9c[1];
        *(dword *)(state + 0x40) |= g_485058.v5c;
        function_16f60(state, (word const *)range);
        function_17000(state, (word const *)(range + 4));
        if (changed)
        {
            byte *old = *(byte **)(state + 0x24);
            if (old && *(byte **)(state + 0x20))
                function_18900((s_render_state_source *)state, (word const *)(old + 0x11c));
            *(byte **)(state + 0x24) = record;
            *(byte **)(state + 0x20) = records;
            *(dword *)(state + 0x44) = *(dword *)(record + 0x10c);
            *(dword *)(state + 0x48) = *(dword *)(record + 0x110);
            *(dword *)(state + 0x4c) = *(dword *)(record + 0x104);
            *(dword *)(state + 0x50) = *(dword *)(record + 0x108);
            *(dword *)(state + 0x54) = *(dword *)(record + 0x114);
            function_18900((s_render_state_source *)state, (word const *)(record + 2));
            function_189a0(state, (word const *)(record + 4));
        }
    }
    function_1ae70(state);
    if (changed || (*(dword *)(state + 0x58) & *(dword *)(state + 0x54)))
    {
        record = *(byte **)(state + 0x24);
        state[0x1514] = true;
        if (!(record[0xf7] & 0xfe)) memcpy(state + 0x1424, record + 6, 0xf0);
        else function_1ba00(state);
    }
    if (changed || different_tag || (*(dword *)(state + 0x58) & *(dword *)(state + 0x54)))
        function_17170(state, (word const *)(*(byte **)(state + 0x1c) + 2));
    function_174d0(state, first, second, third, fourth);
    record = *(byte **)(state + 0x24);
    function_18a90(state, (word const *)record);
    function_19be0(state, (word const *)(record + 0x124));
    function_19ca0(state, (word const *)(record + 0x122));
    function_1a170(state, (s_packed_shader_range const *)(record + 0x11e));
    function_1ab50(state, (s_packed_shader_range const *)(record + 0x120));
    function_17420(state, first, second, third, fourth);
    function_18d70((s_18d70_state *)state);
    function_18e80((s_shader_constant_state *)state);
    function_1b0a0(state);
    function_1cf50();
    if (state[0x1514]) function_1b050((s_01b050_shader_state *)state);
    else function_18ee0(state);
}

// @retail 0x167a0
void function_0167a0()
{
	dword *tables[4][5] = {
		{ 0, 0, 0, 0, 0 },
		{ g_485058.v6c, g_485058.v8c, g_485058.v84, g_485058.v7c, g_485058.v74 },
		{ g_485058.v9c, g_485058.vbc, g_485058.vb4, g_485058.vac, g_485058.va4 },
		{ &g_485058.v5c, &g_485058.v68, &g_485058.v64, &g_485058.v60, &g_485058.v58 },
	};

	memset(g_485058.vbc, 0, sizeof(g_485058.vbc));
	memset(g_485058.vb4, 0, sizeof(g_485058.vb4));
	memset(g_485058.vac, 0, sizeof(g_485058.vac));
	memset(g_485058.va4, 0, sizeof(g_485058.va4));
	memset(g_485058.v9c, 0, sizeof(g_485058.v9c));
	memset(g_485058.v94, 0, sizeof(g_485058.v94));
	memset(g_485058.v8c, 0, sizeof(g_485058.v8c));
	memset(g_485058.v84, 0, sizeof(g_485058.v84));
	memset(g_485058.v7c, 0, sizeof(g_485058.v7c));
	memset(g_485058.v74, 0, sizeof(g_485058.v74));
	memset(g_485058.v6c, 0, sizeof(g_485058.v6c));
	g_485058.v68 = 0;
	g_485058.v64 = 0;
	g_485058.v60 = 0;
	g_485058.v58 = 0;
	g_485058.v5c = 0;

	input_mapping_entry *e = g_43e909;
	for (long i = 0x5e; i > 0; i--, e++)
	{
		if (e->flag0)
			tables[e->type][0][e->key >> 5] |= 1 << (e->key & 0x1f);
		if (e->flag1)
			tables[e->type][1][e->key >> 5] |= 1 << (e->key & 0x1f);
		if (e->flag2)
			tables[e->type][2][e->key >> 5] |= 1 << (e->key & 0x1f);
		if (e->flag3)
			tables[e->type][3][e->key >> 5] |= 1 << (e->key & 0x1f);
		if (e->flag4)
			tables[e->type][4][e->key >> 5] |= 1 << (e->key & 0x1f);
		if (e->extra && e->type == 2)
			g_485058.v94[e->key >> 5] |= 1 << (e->key & 0x1f);
	}
}

word g_485ac0;
dword g_467014, g_467018;
__int64 g_485aa0;

__declspec(noinline) void function_0169f0();
// @retail 0x169f0
void function_0169f0()
{
	g_485aa0 = 1;
	*(dword *)&g_485ad4.lo = g_467014;
	*(dword *)&g_485ad4.hi = g_467018;
	if (g_485ac0 == 0)
		g_485ac0 = 60;
}

struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

// One object: the display setup's done flag, then the two screen rectangles.
struct s_unknown_485a88
{
	byte flag;
	short_rect_pair rects;
};
s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)

// @retail 0x17000
void function_17000(byte *context, word const *range)
{
	struct { dword *banks[2]; dword *masks[2]; } tables_r16 = {
        { (dword *)(context + 0x1530), (dword *)(context + 0x1630) },
        { (dword *)(context + 0x1730), (dword *)(context + 0x1734) }
    };
	
	byte *definition = *(byte **)(context + 0xc);
	byte *instance = *(byte **)(context + 0x10);
	dword *values = *(dword **)(definition + 0x18);
	byte *record = *(byte **)(instance + 0x24) + (*range & 0x1ff) * 4;
	for (long i = 0; i < (*range >> 9); record += 4, ++i)
	{
		word packed = *(word *)record;
		long bank = packed >> 4;
		long slot = packed & 15;
		dword *base = tables_r16.banks[bank & 1];
		dword *out = base + slot * 4;
		dword *source = values + record[3] * 4;
		dword *mask = tables_r16.masks[bank & 1];
		*mask |= 1 << slot;
		switch (((dword)*(word *)record >> 5) & 31)
		{
		case 0: case 5: out[0] = source[0]; break;
		case 1: case 6: out[1] = source[1]; break;
		case 2: case 7: out[2] = source[2]; break;
		case 3: case 8: out[3] = source[3]; break;
		case 4: case 16: case 17:
			out[0] = source[0]; out[1] = source[1]; out[2] = source[2]; break;
		case 9: case 11:
			out[0] = source[0]; out[1] = source[1]; break;
		case 10: case 12:
			out[2] = source[2]; out[3] = source[3]; break;
		case 13: case 18: case 19: case 20:
			out[0] = source[0]; out[1] = source[1]; out[2] = source[2]; out[3] = source[3]; break;
		case 14: case 15:
			out[0] = source[0]; out[1] = source[1]; out[3] = source[3]; break;
		}
	}
}

const real g_45dd7c = 480.0f, g_45dd80 = 640.0f;

struct s_2f6b0_point
{
	short x, y;
};

// @retail 0x2f6b0
void function_2f6b0(s_2f6b0_point const *position, s_2f6b0_point const *grid,
	s_2f6b0_point const *span, short_rect *outer, short_rect *inner)
{
	(void)&grid;
	(void)&span;
	(void)&outer;
	short_rect full = g_485a8a.a;
	short_rect bounds = g_485a8a.b;
	s_2f6b0_point step;
	step.x = (bounds.v3 - bounds.v1) / grid->x;
	step.y = (bounds.v2 - bounds.v0) / grid->y;
	long x = bounds.v1;
	short y = bounds.v0;
	bounds.v1 = x + (word)position->x * step.x;
	bounds.v3 = x + (word)(span->x + position->x) * step.x;
	bounds.v0 = y + (word)position->y * step.y;
	bounds.v2 = y + (word)(span->y + position->y) * step.y;
	*outer = bounds;
	*inner = bounds;
	if (position->x == 0)
		outer->v1 = full.v1;
	else
		inner->v1 += 4;
	if (position->x + span->x >= grid->x)
		outer->v3 = full.v3;
	else
		inner->v3 -= 4;
	if (position->y == 0)
		outer->v0 = full.v0;
	else
		inner->v0 += 4;
	if (position->y + span->y >= grid->y)
		outer->v2 = full.v2;
	else
		inner->v2 -= 4;
}

__declspec(noinline) void function_016a30(real scale, short y, short x);
// @retail 0x16a30
void function_016a30(real scale, short y, short x)
{
	g_485a8a.a.v1 = 0;
	g_485a8a.a.v0 = 0;
	g_485a8a.a.v3 = (short)(scale * g_45dd80);
	g_485a8a.a.v2 = (short)(scale * g_45dd7c);
	g_485a8a.b.v3 = g_485a8a.a.v3 - y;
	g_485a8a.b.v1 = y;
	g_485a8a.b.v0 = x;
	g_485a8a.b.v2 = g_485a8a.a.v2 - x;
}

byte g_485ac2;

// @retail 0x16a90
byte function_016a90()
{
	s_game_options_view *s = g_4e6948;
	if (s && s->flag && s->index != NONE && s->state == 3)
		return false;
	return g_485ac2;
}

long g_485898;

// @retail 0x16ac0
bool function_016ac0()
{
	return g_485898 >= 5 && g_485898 <= 7;
}

inline __declspec(noinline) long function_016ae0(real value);

// @retail 0x16ae0
long function_016ae0(real value)
{
	__asm {
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


// @retail 0x1bdf0
void function_1bdf0(void)
{
    g_485b78[0] |= g_485058.v74[0];
    g_485b78[1] |= g_485058.v74[1];
    g_485b78[2] |= g_485058.va4[0];
    g_485b78[3] |= g_485058.va4[1];
    g_485b78[4] |= g_485058.v58;
}

// @retail 0x1be50
void function_1be50(void)
{
    g_485b78[0] |= g_485058.v84[0];
    g_485b78[1] |= g_485058.v84[1];
    g_485b78[2] |= g_485058.vb4[0];
    g_485b78[3] |= g_485058.vb4[1];
    g_485b78[4] |= g_485058.v64;
}

struct s_33a0b_view;
extern s_33a0b_view *g_485a58;

// @retail 0x1bd50
void function_1bd50(void *state)
{
    if (g_485a58 != state || !state)
    {
        g_485a58 = (s_33a0b_view *)state;
        g_485b78[0] |= g_485058.v8c[0];
        g_485b78[1] |= g_485058.v8c[1];
        g_485b78[2] |= g_485058.vbc[0];
        g_485b78[3] |= g_485058.vbc[1];
        g_485b78[4] |= g_485058.v68;
        g_485b78[0] |= g_485058.v7c[0];
        g_485b78[1] |= g_485058.v7c[1];
        g_485b78[2] |= g_485058.vac[0];
        g_485b78[3] |= g_485058.vac[1];
        g_485b78[4] |= g_485058.v60;
    }
}


#include <math.h>

#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

struct s_1b230_callback
{
	void *context;
	real (__stdcall *evaluate)(void *, long);
};

struct s_1b230_function
{
	long size;
	byte *data;
};

extern double g_4858a0;
real function_13b390(void const *function, real input, real range);

// @retail 0x1b230
real function_1b230(s_1b230_function const *definition, long input_index, long range_index, real period)
{
	(void)&range_index;
	(void)&period;
	real input = 0.0f;
	real range = 0.0f;
	s_1b230_callback *state = (s_1b230_callback *)g_485a58;
	if (state && state->evaluate)
	{
		input = state->evaluate(state->context, input_index);
		range = ((s_1b230_callback *)g_485a58)->evaluate(((s_1b230_callback *)g_485a58)->context, range_index);
	}
	if (!input_index)
	{
		input = (real)(g_4858a0 / period);
		if (definition->data[0] != 3)
			input = (real)fmod((double)input, 1.0);
	}
	return function_13b390(definition, input, range);
}

// @retail 0x17550
void function_17550(byte *context, word const *range)
{
	word const *const *range_reference = &range;
	if (((dword)*range >> 9) > 0)
	{
		byte *definition = *(byte **)(context + 0xc);
		byte *entry = *(byte **)(definition + 0x58) + (*range & 0x1ff) * 4;
		for (long i = 0; i < (**range_reference >> 9); entry += 4, ++i)
		{
			definition = *(byte **)(context + 0xc);
			word const *subrange = *(word **)(definition + 0x50) + entry[3];
			short const *item = *(short **)(definition + 0x48) + (*subrange & 0x1ff) * 2;
			for (long j = 0; j < (*subrange >> 9); item += 2, ++j)
			{
				byte *function = *(byte **)(*(byte **)(context + 0xc) + 0x40) + item[0] * 20;
				s_1b230_function const *curve = (s_1b230_function *)(function + 12);
				real value = function_1b230(curve, *(long *)function, *(long *)(function + 4), *(real *)(function + 8));
				byte *data = curve->data;
				if (!(data[1] & 0xf0))
				{
					real lower = *(real *)(data + 4);
					real upper = *(real *)(data + 8);
					if (0.0f > value) value = 0.0f;
					else if (value > 1.0f) value = 1.0f;
					value = (upper - lower) * value + lower;
				}
				((real *)(context + 0x338))[entry[0] * 16 + item[1]] = value;
			}
		}
	}
}

// @retail 0x174d0
void function_174d0(byte *context, long group, long stage, long pass, long entry)
{
	byte *const *context_reference = &context;
	byte *definition = *(byte **)(*context_reference + 0xc);
	if (group < *(long *)(definition + 0x1c))
	{
		byte *record = *(byte **)(definition + 0x20) + group * 6;
		if (*(dword *)record & (1 << stage))
		{
			word range = *(word *)(record + 4);
			if (stage < (range >> 9))
			{
				// Keep each range address separate from its word load.
				word *stage_range = *(word **)(definition + 0x28) + (range & 0x1ff) + stage;
				word *const *stage_range_reference = &stage_range;
				range = **stage_range_reference;
				if (pass < (range >> 9))
				{
					word *pass_range = *(word **)(definition + 0x30) + (range & 0x1ff) + pass;
					word *const *pass_range_reference = &pass_range;
					range = **pass_range_reference;
					if (entry < (range >> 9))
					{
						word const *selected = (word *)(*(byte **)(definition + 0x38) + ((range & 0x1ff) + entry) * 10);
						function_17550(*context_reference, selected);
					}
				}
			}
		}
	}
}

// @retail 0x1b790
bool function_1b790(byte const *state, long index, real *out, long mode)
{
    real const *values = (real const *)(state + 0x320 + index * 0x40);
    real sx = values[7];
    real sy = values[8];
    transform4x3f matrix;
    real sine = 0.0f;
    real cosine = 1.0f;
    memset(&matrix, 0, sizeof(matrix));
    if (!(fabs(values[13]) < 0.0001f))
    {
        sine = sinf(values[13] * 6.2831854820251465f);
        cosine = cosf(values[13] * 6.2831854820251465f);
    }
    matrix.forward.j = 0.0f - sx * sine;
    matrix.position.x = (1.0f - cosine + sine) * sx * 0.5f + values[10];
    matrix.left.i = sy * sine;
    matrix.position.y = (1.0f - cosine - sine) * values[8] * 0.5f + values[11];
    matrix.forward.i = sx * cosine;
    matrix.left.j = sy * cosine;
    matrix.up.k = values[9];
    matrix.position.z = values[12];
    switch (mode)
    {
    case 5: case 6: case 7: case 8:
        out[mode - 5] = matrix.forward.i; break;
    case 9: out[0] = matrix.forward.i; out[1] = matrix.left.j; break;
    case 10: out[2] = matrix.forward.i; out[3] = matrix.left.j; break;
    case 11: out[0] = matrix.position.x; out[1] = matrix.position.y; break;
    case 12: out[2] = matrix.position.x; out[3] = matrix.position.y; break;
    case 13:
        out[0] = matrix.forward.i; out[1] = matrix.left.j;
        out[2] = matrix.position.x; out[3] = matrix.position.y; break;
    case 14:
        out[0] = matrix.forward.i; out[1] = matrix.forward.j;
        out[2] = 0.0f; out[3] = matrix.position.x; break;
    case 15:
        out[0] = matrix.left.i; out[1] = matrix.left.j;
        out[2] = 0.0f; out[3] = matrix.position.y; break;
    case 16:
        out[0] = matrix.forward.i; out[1] = matrix.left.j; out[2] = matrix.up.k; break;
    case 17:
        out[0] = matrix.position.x; out[1] = matrix.position.y; out[2] = matrix.position.z; break;
    case 18:
        out[0] = matrix.forward.i; out[1] = matrix.forward.j;
        out[2] = matrix.up.i; out[3] = matrix.position.x; break;
    case 19:
        out[0] = matrix.left.i; out[1] = matrix.left.j;
        out[2] = matrix.up.j; out[3] = matrix.position.y; break;
    case 20:
        out[0] = matrix.forward.k; out[1] = matrix.left.k;
        out[2] = matrix.up.k; out[3] = matrix.position.z; break;
    default: __assume(0);
    }
    return true;
}

struct s_tag_data;
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);

// @retail 0x1b580
bool function_1b580(byte const *definition, long component, long unused, long mode, real *out)
{
    long const *unused_reference = &unused;
    (void)unused_reference;
    s_1b230_function const *curve = (s_1b230_function const *)(definition + 0xc);
    real value = function_1b230(curve, *(long const *)definition,
        *(long const *)(definition + 4), *(real const *)(definition + 8));
    if (component != 4)
    {
        byte *data = curve->data;
        if (!(data[1] & 0xf0))
        {
            real low = *(real *)(data + 4);
            real high = *(real *)(data + 8);
            value = PIN(value, 0.0f, 1.0f);
            value = (high - low) * value + low;
        }
        switch (mode)
        {
        case 1: value = 1.0f - value; break;
        case 2:
            value = PIN((real)((cos(value * 6.2831854820251465f) + 1.0f) * 0.5f), 0.0f, 1.0f);
            break;
        case 3:
            value = PIN((real)(0.5f - cos(value * 6.2831854820251465f) * 0.5f), 0.0f, 1.0f);
            break;
        case 4:
            value = PIN((real)((sin(value * 6.2831854820251465f) + 1.0f) * 0.5f), 0.0f, 1.0f);
            break;
        case 5:
            value = PIN((real)(0.5f - sin(value * 6.2831854820251465f) * 0.5f), 0.0f, 1.0f);
            break;
        }
        out[component] = value;
    }
    else
    {
        color3f color;
        if (curve->data && curve->size > 0)
            unpack_color3f(function_13bc00((s_tag_data const *)curve, value), &color);
        else
            color = *(color3f const *)&g_4686cc->red;
        out[0] = color.red;
        out[1] = color.green;
        out[2] = color.blue;
    }
    return true;
}

/* one 4-byte entry of the value list that function_17960 walks */
struct s_shader_value_entry
{
    word output : 4;
    word group : 1;
    word mode : 5;
    word kind : 3;
    word unused : 3;
    byte unknown2;
    byte parameter;
};

// @retail 0x17960
void __stdcall function_17960(byte *state, word const *range)
{
    dword initial_range = *range;
    s_packed_shader_range const *range_view = (s_packed_shader_range const *)range;
    if (range_view->count() > 0)
    {
        byte const *definition = *(byte **)(state + 0xc);
        s_shader_value_entry const *entry = (s_shader_value_entry const *)(*(byte **)(definition + 0x58) + (initial_range & 0x1ff) * 4);
        real *outputs[2] = { (real *)(state + 0x1530), (real *)(state + 0x1630) };
        dword *changed[2] = { (dword *)(state + 0x1730), (dword *)(state + 0x1734) };
        for (long i = 0; i < (*range >> 9); ++i)
        {
            definition = *(byte **)(state + 0xc);
            long range_index = ((word const *)*(byte **)(definition + 0x50))[entry->parameter] & 0x1ff;
            long index = ((short const *)*(byte **)(definition + 0x48))[range_index * 2];
            byte const *parameter = *(byte **)(definition + 0x40) + index * 20;
            real *out = outputs[entry->group] + entry->output * 4;
            if (entry->mode < 5)
            {
                s_1b230_function const *curve = (s_1b230_function const *)(parameter + 12);
                long mode = entry->mode;
                real value = function_1b230(curve, *(long const *)parameter,
                    *(long const *)(parameter + 4), *(real const *)(parameter + 8));
                if (mode != 4)
                {
                    byte const *data = curve->data;
                    if (!(data[1] & 0xf0))
                    {
                        real low = *(real const *)(data + 4);
                        real high = *(real const *)(data + 8);
                        real clamped = 0.0f > value ? 0.0f : value > 1.0f ? 1.0f : value;
                        value = (high - low) * clamped + low;
                    }
                    out[mode] = value;
                }
                else
                {
                    color3f color;
                    if (curve->data && curve->size > 0)
                        unpack_color3f(function_13bc00((s_tag_data const *)curve, value), &color);
                    else
                        color = *(color3f const *)&g_4686cc->red;
                    out[0] = color.red;
                    out[1] = color.green;
                    out[2] = color.blue;
                }
            }
            else
                function_1b790(state, entry->kind, out, entry->mode);
            *changed[entry->group] |= 1 << entry->output;
            ++entry;
        }
    }
}

const dword g_467068[8] = { 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000,
    0x00ffffff, 0x00ff0000, 0x0000ff00, 0x000000ff };

PRIVATE __forceinline void store_shader_mask(byte *state, word packed, long index, dword mask, dword value)
{
    if ((packed & 0x3c0) != 0x200)
    {
        if (!(packed & 0x400))
        {
            *(dword *)(state + 0x144c + index * 4) = (*(dword *)(state + 0x144c + index * 4) & ~mask) | (value & mask);
            *(dword *)(state + 0x1518) |= 1 << index;
        }
        else
        {
            *(dword *)(state + 0x146c + index * 4) = (*(dword *)(state + 0x146c + index * 4) & ~mask) | (value & mask);
            *(dword *)(state + 0x151c) |= 1 << index;
        }
    }
    else if (!(packed & 0x400))
    {
        *(dword *)(state + 0x14d0) = (*(dword *)(state + 0x14d0) & ~mask) | (value & mask);
        *(dword *)(state + 0x1518) |= 0x100;
    }
    else
    {
        *(dword *)(state + 0x14d4) = (*(dword *)(state + 0x14d4) & ~mask) | (value & mask);
        *(dword *)(state + 0x151c) |= 0x100;
    }
}

// @retail 0x17170
void function_17170(byte *state, word const *range)
{
    dword const *values = *(dword **)(*(byte **)(state + 0xc) + 0x10);
    byte const *entry = *(byte **)(*(byte **)(state + 0x10) + 0x24) + (*range & 0x1ff) * 4;
    word remapping = *(word *)(*(byte **)(state + 0x24) + 0xf6);
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            word packed = *(word const *)entry;
            store_shader_mask(state, packed, (packed >> 6) & 15, g_467068[entry[2] & 7], values[entry[3]]);
        }
    }
    else
    {
        byte const *mapping = *(byte **)(*(byte **)(state + 0x20) + 0x24) + (remapping & 0x1ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i, entry += 4)
        {
            word packed = *(word const *)entry;
            long slot = packed & 7;
            short source = *(short const *)(mapping + slot * 4);
            if (source == NONE || ((packed >> 3) & 7) == *(long *)(state + 0x300 + source * 4))
            {
                long index = (packed >> 6) & 15;
                if ((packed & 0x3c0) != 0x200)
                    index += *(signed char *)(state + 0x1520 + slot);
                store_shader_mask(state, packed, index, g_467068[entry[2] & 7], values[entry[3]]);
            }
        }
    }
}

dword __cdecl pack_color4f(color4f const *color);

static inline real unit_clamp_176a0(real value)
{
    if (0.0f > value)
        value = 0.0f;
    else if (value > 1.0f)
        value = 1.0f;
    return value;
}

// @retail 0x176a0
void function_176a0(byte *state, s_packed_shader_range const *range)
{
    if (range->count() > 0)
    {
        byte const *entry = *(byte **)(*(byte **)(state + 0xc) + 0x58) + range->index() * 4;
        for (long i = 0; i < (long)range->count(); ++i, entry += 4)
        {
            byte const *definition = *(byte **)(state + 0xc);
            long range_index = ((word const *)*(byte **)(definition + 0x50))[entry[3]] & 0x1ff;
            long index = ((short const *)*(byte **)(definition + 0x48))[range_index * 2];
            byte const *parameter = *(byte **)(definition + 0x40) + index * 20;
            real values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            function_1b580(parameter, entry[2] & 7, (long)state, (entry[2] >> 3) & 7, values);
            color4f color;
            color.red = unit_clamp_176a0(values[0]);
            color.green = unit_clamp_176a0(values[1]);
            color.blue = unit_clamp_176a0(values[2]);
            color.alpha = unit_clamp_176a0(values[3]);
            dword value = pack_color4f(&color);
            word packed = *(word const *)entry;
            long slot = packed & 7;
            long remapping = *(word *)(*(byte **)(state + 0x24) + 0xf6);
            byte const *mapping = *(byte **)(*(byte **)(state + 0x20) + 0x24) + ((remapping & 0x1ff) + slot) * 4;
            short source = *(short const *)mapping;
            if (!(remapping & 0xfe00) || source == NONE || ((packed >> 3) & 7) == *(long *)(state + 0x300 + source * 4))
            {
                long output_index = (packed >> 6) & 15;
                if ((packed & 0x3c0) != 0x200 && (remapping & 0xfe00))
                    output_index += *(signed char *)(state + 0x1520 + slot);
                store_shader_mask(state, packed, output_index, g_467068[entry[2] & 7], value);
            }
        }
    }
}

// @retail 0x1b2c0
bool function_1b2c0(byte const *state, long index, long component, long texture, long mode, real *out)
{
    if (component < 4)
    {
        real value = ((real const *)(state + 0x5c))[index];
        switch (mode)
        {
        case 1: value = 1.0f - value; break;
        case 2:
            value = PIN((real)((cos(value * 6.2831854820251465f) + 1.0f) * 0.5f), 0.0f, 1.0f);
            break;
        case 3:
            value = PIN((real)(0.5f - cos(value * 6.2831854820251465f) * 0.5f), 0.0f, 1.0f);
            break;
        case 4:
            value = PIN((real)((sin(value * 6.2831854820251465f) + 1.0f) * 0.5f), 0.0f, 1.0f);
            break;
        case 5:
            value = PIN((real)(0.5f - sin(value * 6.2831854820251465f) * 0.5f), 0.0f, 1.0f);
            break;
        }
        out[component] = value;
        return true;
    }
    else if (component == 4)
        *(color3f *)out = ((color3f const *)(state + 0x108))[index];
    else
    {
        real width = 1.0f, height = 1.0f;
        if (texture)
        {
            width = (real)*(long const *)(state + 0x12e8 + texture * 64);
            height = (real)*(long const *)(state + 0x12ec + texture * 64);
        }
        switch (component)
        {
        case 5: out[component - 5] = width; break;
        case 6: out[component - 5] = width; break;
        case 7: out[component - 5] = width; break;
        case 8: out[component - 5] = width; break;
        case 9: out[0] = width; out[1] = height; break;
        case 10: out[2] = width; out[3] = height; break;
        case 11: out[0] = out[1] = 0.0f; break;
        case 12: out[2] = out[3] = 0.0f; break;
        case 13: out[0] = width; out[1] = height; out[2] = out[3] = 0.0f; break;
        case 14: out[0] = width; out[1] = out[2] = out[3] = 0.0f; break;
        case 15: out[1] = height; out[0] = out[2] = out[3] = 0.0f; break;
        case 16: out[0] = width; out[1] = height; out[2] = 1.0f; break;
        case 17: out[0] = out[1] = out[2] = 0.0f; break;
        case 18: out[0] = width; out[1] = out[2] = out[3] = 0.0f; break;
        case 19: out[1] = height; out[0] = out[2] = out[3] = 0.0f; break;
        case 20: out[0] = out[1] = out[2] = out[3] = 0.0f; break;
        default: __assume(0);
        }
    }
    return true;
}

#include <xtl.h>
struct s_texture_stage_parameter
{
    word state;
    byte index;
    byte value_type;
};
const s_texture_stage_parameter g_46703c[11] = {
    { 6, 0, 1 },
    { 30, 1, 0 },
    { 29, 2, 0 },
    { 29, 3, 0 },
    { 22, 4, 1 },
    { 23, 5, 1 },
    { 25, 6, 1 },
    { 24, 7, 1 },
    { 26, 8, 1 },
    { 27, 9, 1 },
    { 0, 0, 0 }
};
dword __cdecl pack_color3f(color3f const *color);

PRIVATE __forceinline long rounded_shader_integer(real value)
{
    long result;
    __asm { fld value }
    __asm { fistp result }
    return result;
}

PRIVATE __forceinline long rounded_shader_alpha(real value)
{
    real scale = 255.0f;
    long result = 0;
    __asm { fld value }
    __asm { fld scale }
    __asm { fmulp st(1), st(0) }
    __asm { fistp result }
    return result;
}

const s_texture_stage_parameter g_467020[7] = {
    { 75, 0, 0 }, { 75, 1, 0 }, { 61, 2, 2 }, { 77, 3, 1 },
    { 78, 4, 1 }, { 157, 5, 1 }, { 138, 6, 0 }
};

// @retail 0x1a170
void __stdcall function_1a170(byte *state, s_packed_shader_range const *range)
{
    if (range->count() <= 0) return;
    byte const *entry = *(byte **)(*(byte **)(state + 0x20) + 0x3c) + range->index() * 4;
    for (long i = 0; i < (long)range->count(); entry += 4, ++i)
    {
        s_texture_stage_parameter const *parameter = &g_467020[entry[0]];
        if (entry[1] == 1)
        {
            real value = ((real *)(state + 0x5c))[entry[3]];
            switch (parameter->value_type)
            {
            case 0:
                {
                    dword alpha = (dword)rounded_shader_alpha(value) << 24;
                    dword old = 0;
                    D3DDevice_GetRenderState((D3DRENDERSTATETYPE)parameter->state, &old);
                    D3DDevice_SetRenderState((D3DRENDERSTATETYPE)parameter->state, alpha ^ ((old ^ alpha) & 0xffffff));
                }
                break;
            case 1: D3DDevice_SetRenderState((D3DRENDERSTATETYPE)parameter->state, *(dword *)&value); break;
            case 2:
                {
                    dword integer_value = rounded_shader_integer(value);
                    D3DDevice_SetRenderState((D3DRENDERSTATETYPE)parameter->state, integer_value);
                }
                break;
            }
        }
        else
        {
            dword color = pack_color3f((color3f *)(state + 0x108) + entry[3]);
            dword old = 0;
            D3DDevice_GetRenderState((D3DRENDERSTATETYPE)parameter->state, &old);
            D3DDevice_SetRenderState((D3DRENDERSTATETYPE)parameter->state, (old & 0xff000000) | (color & 0xffffff));
        }
    }
}

// @retail 0x17b60
void __stdcall function_17b60(byte *context, s_packed_shader_range const *range)
{
    if (range->count() <= 0) return;
    byte *definition = *(byte **)(context + 0xc);
    byte const *entry = *(byte **)(definition + 0x58) + range->index() * 4;
    for (long i = 0; i < (long)range->count(); ++i)
    {
        definition = *(byte **)(context + 0xc);
        word selection = (*(word **)(definition + 0x50))[entry[3]];
        short index = (*(short **)(definition + 0x48))[(selection & 0x1ff) * 2];
        byte *parameter = *(byte **)(definition + 0x40) + index * 20;
        s_texture_stage_parameter const *setting = &g_467020[entry[0]];
        if (entry[1] == 1)
        {
            s_1b230_function const *curve = (s_1b230_function *)(parameter + 12);
            real value = function_1b230(curve, *(long *)parameter, *(long *)(parameter + 4), *(real *)(parameter + 8));
            byte *data = curve->data;
            if (!(data[1] & 0xf0))
            {
                real low = *(real *)(data + 4), high = *(real *)(data + 8);
                if (0.0f > value) value = 0.0f;
                else if (value > 1.0f) value = 1.0f;
                value = (high - low) * value + low;
            }
            switch (setting->value_type)
            {
            case 0:
                {
                    dword alpha = (dword)rounded_shader_alpha(value) << 24;
                    dword previous;
                    D3DDevice_GetRenderState((D3DRENDERSTATETYPE)setting->state, &previous);
                    D3DDevice_SetRenderState((D3DRENDERSTATETYPE)setting->state, alpha ^ ((previous ^ alpha) & 0xffffff));
                }
                break;
            case 1: D3DDevice_SetRenderState((D3DRENDERSTATETYPE)setting->state, *(dword *)&value); break;
            case 2:
                {
                    dword integer_value = rounded_shader_integer(value);
                    D3DDevice_SetRenderState((D3DRENDERSTATETYPE)setting->state, integer_value);
                }
                break;
            }
        }
        else
        {
            s_1b230_function const *curve = (s_1b230_function *)(parameter + 12);
            real value = function_1b230(curve, *(long *)parameter, *(long *)(parameter + 4), *(real *)(parameter + 8));
            dword color = function_13bc00((s_tag_data const *)curve, value);
            dword previous;
            D3DDevice_GetRenderState((D3DRENDERSTATETYPE)setting->state, &previous);
            D3DDevice_SetRenderState((D3DRENDERSTATETYPE)setting->state, (previous & 0xff000000) | (color & 0xffffff));
        }
        entry += 4;
    }
}

// score 51 (was 51): the switch on the procedure id is compiled to a different table: retail
// reads the entry with movzx ecx from a table at another base, ours uses
// esi and another table.
// @retail 0x1ab50
void __stdcall function_1ab50(byte *state, s_packed_shader_range const *range)
{
    if (range->count() <= 0)
        return;
    byte *entry = *(byte **)(*(byte **)(state + 0x20) + 0x3c) + range->index() * 4;
    for (long i = 0; i < (long)range->count(); ++i)
    {
        s_texture_stage_parameter const *parameter = &g_46703c[entry[1]];
        if (entry[2] == 1)
        {
            real value = ((real *)(state + 0x5c))[entry[3]];
            switch (parameter->value_type)
            {
            case 0:
                {
                    dword alpha = (dword)rounded_shader_alpha(value) << 24;
                    dword old;
                    D3DDevice_GetTextureStageState(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, &old);
                    sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, alpha ^ ((old ^ alpha) & 0xffffff));
                }
                break;
            case 1:
                sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, *(dword *)&value);
                break;
            case 2:
                {
                    dword integer_value = rounded_shader_integer(value);
                    sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, integer_value);
                }
                break;
            }
        }
        else
        {
            dword color = pack_color3f((color3f *)(state + 0x108) + entry[3]);
            dword old;
            D3DDevice_GetTextureStageState(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, &old);
            sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)parameter->state, (old & 0xff000000) | (color & 0xffffff));
        }
        entry += 4;
    }
}


// score 54 (was 54): the two arguments land in different registers (retail uses ebp for the
// second and ecx for the first) and the bit-field extraction order differs.
// @retail 0x19be0 standard
void __stdcall function_19be0(byte *state, word const *range)
{
    real *values[2] = { (real *)(state + 0x1530), (real *)(state + 0x1630) };
    dword *masks[2] = { (dword *)(state + 0x1730), (dword *)(state + 0x1734) };
    byte const *entry = *(byte **)(*(byte **)(state + 0x20) + 0x3c) + (*range & 0x1ff) * 4;
    for (unsigned long i = 0; i < (*range >> 9); ++i)
    {
        word packed = *(word const *)entry;
        function_1b2c0(state, entry[3], (packed >> 5) & 31, packed >> 13, 0,
            values[(packed >> 4) & 1] + (packed & 15) * 4);
        packed = *(word const *)entry;
        *masks[(packed >> 4) & 1] |= 1 << (packed & 15);
        entry += 4;
    }
}



// score 39 (was 39): the jump table's targets and the shifts around it differ: retail shifts
// the word at esp+0x18 and keeps the index in ecx, ours keeps it in esi
// and shifts later.
// @retail 0x18560
void __stdcall function_18560(byte *context, s_packed_shader_range const *range)
{
    if (range->count() <= 0) return;
    byte *definition = *(byte **)(context + 0xc);
    byte *entry = *(byte **)(definition + 0x58) + range->index() * 4;
    for (long i = 0; i < (long)range->count(); ++i)
    {
        definition = *(byte **)(context + 0xc);
        word selection = (*(word **)(definition + 0x50))[entry[3]];
        short index = (*(short **)(definition + 0x48))[(selection & 0x1ff) * 2];
        byte *parameter = *(byte **)(definition + 0x40) + index * 20;
        s_texture_stage_parameter const *setting = &g_46703c[entry[1]];
        s_1b230_function const *curve = (s_1b230_function *)(parameter + 12);
        if (entry[2] == 1)
        {
            real value = function_1b230(curve, *(long *)parameter, *(long *)(parameter + 4), *(real *)(parameter + 8));
            byte *data = curve->data;
            if (!(data[1] & 0xf0))
            {
                real low = *(real *)(data + 4), high = *(real *)(data + 8);
                if (0.0f > value) value = 0.0f;
                else if (value > 1.0f) value = 1.0f;
                value = (high - low) * value + low;
            }
            switch (setting->value_type)
            {
            case 0:
                {
                    dword alpha = (dword)rounded_shader_alpha(value) << 24;
                    dword previous;
                    D3DDevice_GetTextureStageState(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, &previous);
                    sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, alpha ^ ((previous ^ alpha) & 0xffffff));
                }
                break;
            case 1: sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, *(dword *)&value); break;
            case 2:
                {
                    dword integer_value = rounded_shader_integer(value);
                    sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, integer_value);
                }
                break;
            }
        }
        else
        {
            real value = function_1b230(curve, *(long *)parameter, *(long *)(parameter + 4), *(real *)(parameter + 8));
            dword color = function_13bc00((s_tag_data const *)curve, value);
            dword previous;
            D3DDevice_GetTextureStageState(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, &previous);
            sampler_state_write(entry[0], (D3DTEXTURESTAGESTATETYPE)setting->state, (previous & 0xff000000) | (color & 0xffffff));
        }
        entry += 4;
    }
}



// score 246 (was 246): sampler convention and captured descriptor versus dirty-store rereads;
// short counter 246; byte counter 282; source index long 248; selector byte 246;
// pack result at store 344; existing clamp calls 318.
// @retail 0x19ca0
void function_19ca0(byte *state, word const *range)
{
    byte const *definition = *(byte **)(state + 0x20);
    byte const *entry = *(byte **)(definition + 0x3c) + (*range & 0x1ff) * 4;
    word remapping = *(word *)(*(byte **)(state + 0x24) + 0xf6);
    real values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); ++i)
        {
            long component = entry[2] & 7;
            function_1b2c0(state, entry[3], entry[2] & 7, 0, (entry[2] >> 3) & 7, values);
            if (*(word const *)entry & 0x800)
                for (long j = 0; j < 4; ++j) values[j] = (values[j] + 1.0f) * 0.5f;
            values[0] = values[0] < 0.0f ? 0.0f : values[0] > 1.0f ? 1.0f : values[0];
            values[1] = values[1] < 0.0f ? 0.0f : values[1] > 1.0f ? 1.0f : values[1];
            values[2] = values[2] < 0.0f ? 0.0f : values[2] > 1.0f ? 1.0f : values[2];
            values[3] = values[3] < 0.0f ? 0.0f : values[3] > 1.0f ? 1.0f : values[3];
            color4f color;
            color.red = values[0]; color.green = values[1]; color.blue = values[2]; color.alpha = values[3];
            dword value = pack_color4f(&color);
            dword mask = g_467068[component];
            dword packed = *(word const *)entry;
            store_shader_mask(state, packed, (packed >> 6) & 15, mask, value);
            entry += 4;
        }
    }
    else
    {
        byte const *mapping = *(byte **)(definition + 0x24) + (remapping & 0x1ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i)
        {
            dword packed = *(word const *)entry;
            long slot = packed & 7;
            short source = *(short const *)(mapping + slot * 4);
            if (source != NONE && ((packed >> 3) & 7) != *(long *)(state + 0x300 + source * 4))
            {
                entry += 4;
                continue;
            }
            long component = entry[2] & 7;
            function_1b2c0(state, entry[3], entry[2] & 7, 0, (entry[2] >> 3) & 7, values);
            if (*(word const *)entry & 0x800)
                for (long j = 0; j < 4; ++j) values[j] = (values[j] + 1.0f) * 0.5f;
            values[0] = values[0] < 0.0f ? 0.0f : values[0] > 1.0f ? 1.0f : values[0];
            values[1] = values[1] < 0.0f ? 0.0f : values[1] > 1.0f ? 1.0f : values[1];
            values[2] = values[2] < 0.0f ? 0.0f : values[2] > 1.0f ? 1.0f : values[2];
            values[3] = values[3] < 0.0f ? 0.0f : values[3] > 1.0f ? 1.0f : values[3];
            color4f color;
            color.red = values[0]; color.green = values[1]; color.blue = values[2]; color.alpha = values[3];
            dword value = pack_color4f(&color);
            dword mask = g_467068[component];
            packed = *(word const *)entry;
            long index = (packed >> 6) & 15;
            if ((packed & 0x3c0) != 0x200) index += *(signed char *)(state + 0x1520 + (packed & 7));
            store_shader_mask(state, packed, index, mask, value);
            entry += 4;
        }
    }
}
// @retail 0x18ee0
void __stdcall function_18ee0(byte *state)
{
    bool first_constant = (*(dword *)(state + 0x1518) & 0x100) != 0;
    bool second_constant = (*(dword *)(state + 0x151c) & 0x100) != 0;
    *(dword *)(state + 0x1518) &= ~0x100;
    *(dword *)(state + 0x151c) &= ~0x100;
    while (*(dword *)(state + 0x1518))
    {
        dword remaining = *(dword *)(state + 0x1518);
        long index;
        __asm { bsf ecx, remaining }
        __asm { mov index, ecx }
        *(dword *)(state + 0x1518) &= ~(1 << index);
        D3DDevice_SetRenderState((D3DRENDERSTATETYPE)(index + 10), *(dword *)(state + 0x144c + index * 4));
    }
    while (*(dword *)(state + 0x151c))
    {
        dword remaining = *(dword *)(state + 0x151c);
        long index;
        __asm { bsf ecx, remaining }
        __asm { mov index, ecx }
        *(dword *)(state + 0x151c) &= ~(1 << index);
        D3DDevice_SetRenderState((D3DRENDERSTATETYPE)(index + 18), *(dword *)(state + 0x146c + index * 4));
    }
    if (first_constant)
        D3DDevice_SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT0, *(dword *)(state + 0x14d0));
    if (second_constant)
        D3DDevice_SetRenderState(D3DRS_PSFINALCOMBINERCONSTANT1, *(dword *)(state + 0x14d4));
}

// @retail 0x17420
void function_17420(byte *state, long stage, long group, long pass, long variant)
{
    byte const *definition = *(byte **)(state + 0xc);
    if (stage < *(long *)(definition + 0x1c))
    {
        byte const *stage_entry = *(byte **)(definition + 0x20) + stage * 6;
        if ((*(dword const *)stage_entry & (1 << group)) &&
            group < (*(word const *)(stage_entry + 4) >> 9))
        {
            s_packed_shader_range const *group_range =
                (s_packed_shader_range const *)*(byte **)(definition + 0x28) +
                (*(word const *)(stage_entry + 4) & 0x1ff) + group;
            if (pass < (long)group_range->count())
            {
                s_packed_shader_range const *pass_range =
                    (s_packed_shader_range const *)*(byte **)(definition + 0x30) + group_range->index() + pass;
                if (variant < (long)pass_range->count())
                {
                    word const *entry = (word const *)(*(byte **)(definition + 0x38) +
                        (pass_range->index() + variant) * 10);
                    function_176a0(state, (s_packed_shader_range const *)(entry + 3));
                    function_17960(state, entry + 4);
                    function_17b60(state, (s_packed_shader_range const *)(entry + 1));
                    function_18560(state, (s_packed_shader_range const *)(entry + 2));
                }
            }
        }
    }
}

real function_336f0(long selector);
void function_33980(long selector, vector3f *out);
long function_34060(long mode, dword flags);

// @retail 0x1ae70
void function_1ae70(byte *state)
{
    *(dword *)(state + 0x58) = 0;
    dword scalar_pending[2];
    dword vector_pending[2];
    dword last_pending;
    for (long mask = 0; mask < 2; ++mask)
        scalar_pending[mask] = ((dword *)(state + 0x30))[mask] & ((dword *)(state + 0x44))[mask];
    for (long mask = 0; mask < 2; ++mask)
        vector_pending[mask] = ((dword *)(state + 0x38))[mask] & ((dword *)(state + 0x4c))[mask];
    last_pending = *(dword *)(state + 0x40) & *(dword *)(state + 0x54);
    for (long mask = 0; mask < 2; ++mask)
    {
        dword remaining = scalar_pending[mask];
        while (remaining)
        {
            long bit;
            __asm { bsf ecx, remaining }
            __asm { mov bit, ecx }
            long index = mask * 32 + bit;
            ((real *)(state + 0x5c))[index] = function_336f0(index);
            remaining &= ~(1 << bit);
            ((dword *)(state + 0x30))[index >> 5] &= ~(1 << (index & 31));
        }
    }
    for (long mask = 0; mask < 2; ++mask)
    {
        dword remaining = vector_pending[mask];
        while (remaining)
        {
            long bit;
            __asm { bsf ecx, remaining }
            __asm { mov bit, ecx }
            long index = mask * 32 + bit;
            function_33980(index, (vector3f *)(state + 0x108 + index * 12));
            remaining &= ~(1 << bit);
            ((dword *)(state + 0x38))[index >> 5] &= ~(1 << (index & 31));
        }
    }
    dword remaining = last_pending;
    while (remaining)
    {
        long bit;
        __asm { bsf ecx, remaining }
        __asm { mov bit, ecx }
        ((long *)(state + 0x300))[bit] = function_34060(bit, *(dword *)(*(byte **)(state + 0x14) + 2));
        remaining &= ~(1 << bit);
        ((dword *)(state + 0x40))[bit >> 5] &= ~(1 << (bit & 31));
        ((dword *)(state + 0x58))[bit >> 5] |= 1 << (bit & 31);
    }
}
