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

// score 47 (was 47): BLOCKED-BY 0x18a90 (src/unknown_01cf50.cpp, contributor file: retail passes the state
// in EBX, ours pushes it; that tail is about 42 of the 47 lines and the extra byte, the later calls shifting
// by one instruction) and the restore block's schedule: retail reloads the table field before the entry
// store and loads `second` after the two table loads, ours loads `second` right after the entry `lea`.
// Frame, entry and logarithm agree. Forms (the history ran 167 -> 162 -> 153 -> 132 -> 95 -> 47):
// restore entry-load order (retail reloads the table field before the entry store) and
// BLOCKED-BY 0x18a90 (src/unknown_01cf50.cpp, contributor file: retail passes the state in ebx, ours
// pushes it; that tail is about 42 of the 47 lines and the extra byte).
// BLOCKED-BY 0x18a90 (retail passes the state in EBX, ours pushes it; the later calls
// then shift by one instruction) and the `second` load of the restore block, scheduled after the table
// loads in retail and right after the entry `lea` here; frame, entry and logarithm agree.
// Direct entry publication with staged chain index/subscript/pointer/table-reload/accessor/parameter-address
// forms 47 each; original reference with staged index 47; separate restored-entry local 183. Original body restored.
// Earlier 95 from 132: frame four, the logarithm and the tail agree; left are the restore block (retail reloads
// the range field before each call; ours keeps it in a register) and BLOCKED-BY 0x18a90, which retail passes
// the state in ebx (ours pushes it).
// Earlier: Frame and numeric/reset sequence agree; logarithm width and restore allocation remain.
// Earlier 167 from 167: logarithm width/reset schedule and restore/final pointer allocation;
// logarithm factor width and restore allocation remain; frame and numeric/reset sequence agree.
// logarithm width/reset schedule and restore/final pointer allocation;
// frame and numeric/reset sequence agree; logarithm factor width and restore allocation differ.
// final record lifetime 167; common update 263; retained old tag 241; records-first test 169;
// promoted inverse 248; float consistency 306; coordinated texture inputs 167.
// Typed numeric state fields 167; typed pointer fields 181; full state view 181;
// force-inline material lookup 167; global-first mask unions on full view 181.
// Factor first 167; inline depth one 167; scoped size 483; dirty-mask order 167; packed-range view 167.
// Rounded logarithmic value before stores 167; late tag publication 295; both published restore ranges 279;
// record reload after render call 192; raw global-first unions 167.
// Pending-mask snapshot 188; late record predicate 297; late tag predicate 287; direct old
// record 167; conditional reciprocal 167 (fast).
// early dirty mask, update byte or late counter pair each 296; first published range 167;
// second published range 187.
// Final consumers read the actual published record field: entry-only read 169; pass-only read 167;
// early saved-tag predicate 298; published-record field reference 162 (plain); equality arms 167.
// Record-field pointer 162; signed entry/pass/both carriers 163/163/164;
// negated equalities 162; saved tag at both tests 246; dirty-mask cursor 161; both cursors 161 (one-point gains dropped).
// Dirty-word library clear 153 (recovers the numeric/reset scheduling). Native entry/chain/both descriptors,
// const record and signed change forms 162; update/counter/all/reset-last clears 287/258/307/157.
// Actual entry offset, chain index, pass index, range index, offset/chain, offset/pass,
// chain/pass/range and all four named computations each 153.
// Native entry/pass/range/all word pointers 153/153/153/153;
// readonly entry words 153; chained counter resets 155;
// unqualified float-input logarithm 152; reciprocal default first 157.
// Float-input logarithm with native descriptors/first range/second range 152/152/184;
// with first mask pair/mask cursor/both 151/151/151 (two-point gains excluded).
// First/second/both native mask loops 152/180/180; published table/record/both field lifetimes
// 289/283/307; native signed packed carriers 153; signed carriers with table field 289.
// Early changed/tag/both declaration homes 153/153/153; separate record publication 153;
// early record-table/selected-record/both pointer homes 153/153/153; complete native declaration block 153.
// Readonly definition/pass/both 153/153/153; first/second/both native record copies
// 153/153/153; range publication at consumer 153; copies with late publication 153.
// Entry/pass/both/range/record/entry-pass-range/all base-first pointers 153 each; all bases with
// ordinary log input 152 (one-point gain dropped).
// Actual table header / five-word entry / three-word range / table-entry /
// table-range / entry-range / all metadata records / all readonly records 153 each.
// Distinct restore word/input/entry/all carrier lifetimes 153 each; initial/restore/both
// metadata graph scopes 153 each; full restore graph with separate upload record 153.
// Existing lookup single-result homes/uninitialized/group-first/default-first 153/153/153/153;
// same graph with group fields at every test 153/153/153; direct conditional lookup 153. Original helper restored.
// Round 226: selected-record prefix 153; record-block header 153; pass-input prefix 153; record
// and block 153; record and pass 153; block and pass 153; all three metadata objects 153; all
// readonly metadata objects 153.
// Final change/tag/mask test orders 155/158/159/158/159; reversed early test 155,
// reversed middle test 175, both reversed 177.
// entry at use 153; chain at use 153; previous tag at use 153; entry chain at use 153; entry tag at use
// 153; chain tag at use 153; all field observations 153; fields old record at use 153.
// Published reciprocal log input with direct/scalar publication 284/153;
// with paired-counter library clear 305/258, chained counters 284/155,
// and long changed result 280/195. Exact original logarithm and reciprocal retained.
// Eager record/restore/final predicates and their pairs/all 303/219/168/291/296/206/276; all plus eager copy gate 293. Original double expression, observations and calls restored.
// Published entry-field lvalue 132; pass field 153; range field 294; entry/pass 132;
// entry/range and all fields 290; pass/range 294; published table-field lvalue 153.
// Minimal entry-field form kept after ordinary confirmation at retail length 972.
// Existing updater inline declarations 16f60/17000/189a0, their three pairs/all,
// and all three noinline declarations 132 each. Original declarations and body restored.
// DL159: coherent updater inline definition/declaration 132; on that graph, global-first complete mask groups 001/010/011/100/101/110/111 all 132. Original provider specifications, exact logarithm and body restored.
// Round 104: logarithm product in float 131 (retail's dword factor); with the record read from its field at
// every use after it is stored (retail reloads it after 0x18900) 95, kept. On it: both range calls reading the range field 177 (978 B); with the entry stored from its local 191;
// entry, pass and range written and read through their fields 177; plus global-first unions 177.
// Round 231: range-field reference 164; with dirty-word cursor 164; first/second/both native mask
// loops 156/47/47; cursor plus first/second/both loops 156/47/47; second loop alone 181. Minimal
// reference plus second loop retained.
// Round 233, on the recovered range-reference/native-mask graph: published table reference 47;
// native word range/entry field references 47/47; published pass/table-plus-pass references 47/47;
// packed entry accessor 47; entry-reference consumption scope 47; unsigned native mask index 47.
// Round 60: entry reference after its store 293; range reference after its store 61;
// direct sampler record 47; separate restored packed/next/both words 47/47/47;
// direct first updater 149; direct second updater 47; entry word at chain 47;
// direct published entry 294. Original body restored.
// Round 235: published table snapshot after entry/before entry/after packed 151/231/151; after-entry with distinct packed/chain/both/pass/all restore receivers 151/151/151/151/151.
// Round 76: entry stored directly into its field, no reference, 47 (the table reload now hoists as in
// retail; first difference +0x19e -> +0x1a7; restored, same count); entry through the existing entry local 183; packed word read
// inside the chain index 47; tables re-read once into its local 47; named chain/pass bases 47.
// Round 155: entry table from the local 151; entry stored to its field directly 47 (table reload in place,
// the second index load hoists instead); plus range stored directly 163; entry local reused 183;
// direct entry store with swapped index sum 47.
// Round 34: table field through a reference for all three reads 47; entry word read inside the index 47;
// direct entry store 47 (table reload as retail; the second-index load then hoists above the store);
// direct store with `second + (packed & 0x1ff)` 47; direct store read back through a const reference 293.
// Round 118 (all keep the second-index load above the entry store): direct store with the entry read back
// from its field 47; that with `second + (packed & 0x1ff)` 47; next folded into the pass expression 47; that
// with the entry read from its field inside it 47; the reference form with the swapped sum 47; `second`
// read through its address (diagnostic) 47, so the order is not an aliasing effect.
// Round 83, all on the direct entry store (aligned 3: only `second` loads before the entry store, retail
// after both table bases): chain lookup folded into the pass expression 47; tables re-read into its
// local after the store 47; `second` read through its address 47; named chain index 47.
// Round 115 (after 0x17170 matched; the tail is still BLOCKED-BY 0x18a90): chain table read into a pointer
// before the entry word 47; `second + index` and `third + index` sums 47; entry base from the `tables` local 151;
// `first * 10 +` base 47; pass table read before the entry store 119. Original body restored.
// MiniMax R (10-31): no new form, no lower score.
// Round 54 (lane 10, fast): restore table pointer published in a chained assignment 47; range reference
// declared beside the entry reference 47; definition derived from the just-published parameters field 47;
// the entry byte offset named once for both walks 47; parameters published after the definition lookup 193;
// the pending masks ORed in right after the tables are published 191 (978 bytes); the pass published after
// the range is computed 118 (974 bytes).
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
    *(real *)(state + 0x2c) = (real)log((double)inverse) * 0.6931471824645996f;
    memset(state + 0x58, 0, sizeof(dword));
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
        byte *&restored_entry = *(byte **)(state + 0x14);
        restored_entry = *(byte **)(*(byte **)(state + 0x10) + 4) + first * 10;
        packed = *(word *)restored_entry;
        next = ((word *)*(byte **)(*(byte **)(state + 0x10) + 0xc))[(packed & 0x1ff) + second];
        pass = *(byte **)(*(byte **)(state + 0x10) + 0x14) + ((next & 0x1ff) + third) * 10;
        *(byte **)(state + 0x18) = pass;
        byte *&range = *(byte **)(state + 0x1c);
        range = *(byte **)(*(byte **)(state + 0x10) + 0x1c) + ((*(word *)(pass + 8) & 0x1ff) + fourth) * 6;
        *(dword *)(state + 0x30) |= g_485058.v6c[0];
        *(dword *)(state + 0x34) |= g_485058.v6c[1];
        for (long mask_index = 0; mask_index < 2; ++mask_index)
            ((dword *)(state + 0x38))[mask_index] |= g_485058.v9c[mask_index];
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
            *(dword *)(state + 0x44) = *(dword *)(*(byte **)(state + 0x24) + 0x10c);
            *(dword *)(state + 0x48) = *(dword *)(*(byte **)(state + 0x24) + 0x110);
            *(dword *)(state + 0x4c) = *(dword *)(*(byte **)(state + 0x24) + 0x104);
            *(dword *)(state + 0x50) = *(dword *)(*(byte **)(state + 0x24) + 0x108);
            *(dword *)(state + 0x54) = *(dword *)(*(byte **)(state + 0x24) + 0x114);
            function_18900((s_render_state_source *)state, (word const *)(*(byte **)(state + 0x24) + 2));
            function_189a0(state, (word const *)(*(byte **)(state + 0x24) + 4));
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
    byte *const &shader_record = *(byte **)(state + 0x24);
    function_18a90(state, (word const *)shader_record);
    function_19be0(state, (word const *)(shader_record + 0x124));
    function_19ca0(state, (word const *)(shader_record + 0x122));
    function_1a170(state, (s_packed_shader_range const *)(shader_record + 0x11e));
    function_1ab50(state, (s_packed_shader_range const *)(shader_record + 0x120));
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
	short_rect full = g_485a8a.a;
	short_rect bounds = g_485a8a.b;
	s_2f6b0_point step;
	step.x = (bounds.v3 - bounds.v1) / grid->x;
	step.y = (bounds.v2 - bounds.v0) / grid->y;
	short_rect cell;
	cell.v1 = bounds.v1 + position->x * step.x;
	cell.v3 = bounds.v1 + (span->x + position->x) * step.x;
	cell.v0 = bounds.v0 + position->y * step.y;
	cell.v2 = bounds.v0 + (span->y + position->y) * step.y;
	*outer = cell;
	*inner = cell;
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
    real sine = 0.0f;
    real cosine = 1.0f;
    real sx = values[7];
    real sy = values[8];
    transform4x3f matrix;
    memset(&matrix, 0, sizeof(matrix));
    if (!(fabs(values[13]) < 0.0001f))
    {
        sine = sinf(values[13] * 6.2831854820251465f);
        cosine = cosf(values[13] * 6.2831854820251465f);
    }
    matrix.forward.i = sx * cosine;
    matrix.forward.j = 0.0f - sx * sine;
    matrix.position.x = (1.0f - cosine + sine) * sx * 0.5f + values[10];
    matrix.left.i = sy * sine;
    matrix.left.j = sy * cosine;
    matrix.position.y = (1.0f - cosine - sine) * sy * 0.5f + values[11];
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

/* a shader curve's value mapped into the curve's [low, high] range, unless the
   curve's flags say it is already in range */
PRIVATE __forceinline real shader_curve_remap(s_1b230_function const *curve, real value)
{
    byte const *data = curve->data;
    if (!(data[1] & 0xf0))
    {
        real low = *(real const *)(data + 4);
        real high = *(real const *)(data + 8);
        value = PIN(value, 0.0f, 1.0f);
        return (high - low) * value + low;
    }
    return value;
}

// @retail 0x1b580
bool function_1b580(long unused, byte const *definition, long component, long mode, real *out)
{
    long const *unused_reference = &unused;
    (void)unused_reference;
    s_1b230_function const *curve = (s_1b230_function const *)(definition + 0xc);
    real sample = function_1b230(curve, *(long const *)definition,
        *(long const *)(definition + 4), *(real const *)(definition + 8));
    if (component != 4)
    {
        real value = shader_curve_remap(curve, sample);
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
            unpack_color3f(function_13bc00((s_tag_data const *)curve, sample), &color);
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
                    out[mode] = shader_curve_remap(curve, value);
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

/* the shader's output remapping, a packed range of its remapping table */
static inline s_packed_shader_range const *shader_remapping(byte const *state)
{
    return (s_packed_shader_range const *)(*(byte **)(state + 0x24) + 0xf6);
}

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

/* the same store, reading the output index from the descriptor word at each use */
PRIVATE __forceinline void store_descriptor_mask(byte *state, word const &packed, dword mask, dword value)
{
    if ((packed & 0x3c0) != 0x200)
    {
        if (!(packed & 0x400))
        {
            *(dword *)(state + 0x144c + ((packed >> 6) & 15) * 4) = (*(dword *)(state + 0x144c + ((packed >> 6) & 15) * 4) & ~mask) | (value & mask);
            *(dword *)(state + 0x1518) |= 1 << ((packed >> 6) & 15);
        }
        else
        {
            *(dword *)(state + 0x146c + ((packed >> 6) & 15) * 4) = (*(dword *)(state + 0x146c + ((packed >> 6) & 15) * 4) & ~mask) | (value & mask);
            *(dword *)(state + 0x151c) |= 1 << ((packed >> 6) & 15);
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
    word remapping = shader_remapping(state)->packed;
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); entry += 4, ++i)
        {
            dword mask = g_467068[entry[2] & 7];
            dword value = values[entry[3]];
            store_descriptor_mask(state, *(word const *)entry, mask, value);
        }
    }
    else
    {
        byte const *mapping = *(byte **)(*(byte **)(state + 0x20) + 0x24) + (remapping & 0x1ff) * 4;
        for (long i = 0; i < (*range >> 9); entry += 4, ++i)
        {
            word packed = *(word const *)entry;
            short const *source = (short const *)(mapping + (packed & 7) * 4);
            if (*source == NONE || ((packed >> 3) & 7) == *(long *)(state + 0x300 + *source * 4))
            {
                dword mask = g_467068[entry[2] & 7];
                dword value = values[entry[3]];
                if ((packed & 0x3c0) != 0x200)
                {
                    long index = *(signed char *)(state + 0x1520 + (packed & 7)) + ((packed >> 6) & 15);
                    store_shader_mask(state, packed, index, mask, value);
                }
                else
                {
                    store_shader_mask(state, packed, (packed >> 6) & 15, mask, value);
                }
            }
        }
    }
}

dword __cdecl pack_color4f(color4f const *color);

static inline void unit_clamp_176a0(real &value)
{
    if (0.0f > value)
        value = 0.0f;
    else if (value > 1.0f)
        value = 1.0f;
}

// @retail 0x176a0
void function_176a0(byte *state, s_packed_shader_range const *range)
{
    if (range->count() > 0)
    {
        byte const *entry = *(byte **)(*(byte **)(state + 0xc) + 0x58) + range->index() * 4;
        for (long i = 0; i < (long)range->count(); entry += 4, ++i)
        {
            byte const *definition = *(byte **)(state + 0xc);
            long range_index = ((word const *)*(byte **)(definition + 0x50))[entry[3]] & 0x1ff;
            long index = ((short const *)*(byte **)(definition + 0x48))[range_index * 2];
            byte const *parameter = *(byte **)(definition + 0x40) + index * 20;
            real values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            function_1b580((long)state, parameter, entry[2] & 7, (entry[2] >> 3) & 7, values);
            unit_clamp_176a0(values[0]);
            unit_clamp_176a0(values[1]);
            unit_clamp_176a0(values[2]);
            unit_clamp_176a0(values[3]);
            color4f color;
            color.red = values[0];
            color.green = values[1];
            color.blue = values[2];
            color.alpha = values[3];
            dword value = pack_color4f(&color);
            word packed = *(word const *)entry;
            long slot = packed & 7;
            short const *source = (short const *)(*(byte **)(*(byte **)(state + 0x20) + 0x24) + (shader_remapping(state)->index() + slot) * 4);
            if (shader_remapping(state)->count() == 0 || *source == NONE || ((packed >> 3) & 7) == *(long *)(state + 0x300 + *source * 4))
            {
                dword mask = g_467068[entry[2] & 7];
                if ((packed & 0x3c0) != 0x200)
                {
                    long output_index = (packed >> 6) & 15;
                    if (shader_remapping(state)->count() > 0)
                        output_index += *(signed char *)(state + 0x1520 + slot);
                    store_shader_mask(state, packed, output_index, mask, value);
                }
                else
                {
                    store_shader_mask(state, packed, (packed >> 6) & 15, mask, value);
                }
            }
        }
    }
}

// @retail 0x1b2c0
bool function_1b2c0(byte const *state, real *out, long index, long component, long texture, long mode)
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
    {
        out[0] = ((color3f const *)(state + 0x108))[index].red;
        out[1] = ((color3f const *)(state + 0x108))[index].green;
        out[2] = ((color3f const *)(state + 0x108))[index].blue;
    }
    else
    {
        real width = 1.0f;
        real height = 1.0f;
        if (texture)
        {
            width = (real)*(long const *)(state + 0x12e8 + texture * 64);
            height = (real)*(long const *)(state + 0x12ec + texture * 64);
        }
        long output_component = component - 5;
        switch (component)
        {
        case 5: out[output_component] = width; break;
        case 6: out[output_component] = width; break;
        case 7: out[output_component] = width; break;
        case 8: out[output_component] = width; break;
        case 9: out[0] = width; out[1] = height; break;
        case 10: out[2] = width; out[3] = height; break;
        case 11: out[0] = out[1] = 0.0f; break;
        case 12: out[3] = out[2] = 0.0f; break;
        case 13: out[0] = width; out[1] = height; out[3] = out[2] = 0.0f; break;
        case 14: {
            real *zero = out + 1;
            out[0] = width;
            zero[0] = 0.0f;
            zero[2] = zero[1] = 0.0f;
            break;
        }
        case 15: out[1] = height; out[0] = out[3] = out[2] = 0.0f; break;
        case 16: out[0] = width; out[1] = height; out[2] = 1.0f; break;
        case 17: out[0] = out[1] = out[2] = 0.0f; break;
        case 18: {
            real *zero = out + 1;
            out[0] = width;
            zero[0] = 0.0f;
            zero[2] = zero[1] = 0.0f;
            break;
        }
        case 19: out[1] = height; out[0] = out[3] = out[2] = 0.0f; break;
        case 20: out[0] = out[1] = out[3] = out[2] = 0.0f; break;
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

/* value * 255 rounded as the x87 does, moved into the alpha byte of a color */
PRIVATE __forceinline long packed_shader_alpha(real value)
{
    real scale = 255.0f;
    long result = 0;
    __asm { fld value }
    __asm { fld scale }
    __asm { fmulp st(1), st(0) }
    __asm { fistp result }
    __asm { shl result, 24 }
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
                    dword alpha = packed_shader_alpha(value);
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
                    dword alpha = packed_shader_alpha(value);
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
                    dword alpha = packed_shader_alpha(value);
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


// @retail 0x19be0
void __stdcall function_19be0(byte *state, word const *range)
{
    real *values[2] = { (real *)(state + 0x1530), (real *)(state + 0x1630) };
    dword *masks[2] = { (dword *)(state + 0x1730), (dword *)(state + 0x1734) };
    byte const *entry = *(byte **)(*(byte **)(state + 0x20) + 0x3c) + (*range & 0x1ff) * 4;
    for (long i = 0; i < (*range >> 9); ++i)
    {
        word packed = *(word const *)entry;
        function_1b2c0(state, values[(packed >> 4) & 1] + (packed & 15) * 4, entry[3],
            (packed >> 5) & 31, packed >> 13, 0);
        packed = *(word const *)entry;
        *masks[(packed >> 4) & 1] |= 1 << (packed & 15);
        entry += 4;
    }
}



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
                    dword alpha = packed_shader_alpha(value);
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

// score 130 (was 130): BLOCKED-BY 0x1b2c0 (round 32 below). Only the mapped loop's rescale test after 0x1b2c0 differs: retail reloads the word
// (`mov ax,[ebx]; test ah,8`), ours tests memory (`test byte [ebx+1],8`). Earlier: frame 0x28 and sampler agree;
// deferred index and post-store descriptor rereads remain.
// Earlier: frame and sampler agree; deferred index and post-store descriptor rereads remain.
// Frame and sampler agree; deferred index and post-store descriptor rereads remain.
// Direct descriptor pointer/reference 267/267; mapped pre-rescale reload 267; mapped descriptor across pack 263 (not kept: a gain under 8);
// validation descriptor across pack 288; direct pointer on that graph 263; separate mapped output word 268;
// short mapping base 263 (no added gain, restored).
// Frame and sampler agree; captured descriptor/index and post-store rereads remain.
// Earlier: Frame and sampler carriers agree; captured descriptors and dirty-store scheduling remain.
// Earlier: Frame and sampler carriers agree; captured descriptors and store scheduling remain.
// Frame 0x28 and sampler agree; captured descriptors and
// destination/dirty-store scheduling remain. Unsigned/signed writer packed formals 291/291;
// aggregate colors both/direct/mapped 273/273/269; aggregates with unsigned/signed formal 291/291;
// aggregates with long source 283.
// Frame and sampler carriers agree; captured descriptors and store scheduling remain.
// short counter 246; byte counter 282; source index long 248; selector byte 246;
// pack result at store 344; existing clamp calls 318. Frame pointer 365;
// zero inline depth 652; global optimization off 1687; size 517; native
// ternary clamp loops 359.
// Unsigned counters 325; rescale pointer traversal 354; remapping-source address 268; separate
// clamp passes 358; shared native color workspace 246 (fast).
// Existing packed-range accessors 325; post-clamp mask capture 275; positive filter
// with full-width descriptors 269; alpha-first color 250; alpha-first clamps 270 (fast).
// sample-array name order 246; branch-local sample arrays 250; mapped-field reference 268;
// shared sampler selector 246; descending component rescale 262 (fast).
// Recovered sampler with early component mask 269 (plain); interleaved color fields 332;
// combined component processing 268 (one-point addition dropped); conditional stores/shared counter 269/269.
// Direct descriptor fields 265 (plain 265, 1198/1225); mapped fields 284; both fields 279;
// native descriptors 265; separate output descriptors 269; fields/native/separate RGB copies
// 309/312/315; direct fields plus native mapped descriptors 265 (1197/1225).
// Actual four-byte entry fields, input bitfields, rescale bit and native signed mapping records:
// eight isolated/combined source forms 265 each. Shared writer and sampler unchanged.
// Round 197: complete definition/remapping/source observations and their four combinations
// 267 each; all observations with a common genuine color workspace 267. Original writer/body restored.
// Round 204: sibling reference clamps 267; common color/direct component/both 267/267/267;
// bounded short counter with clamps 266; with common color/direct component/both 266/266/266.
// One-point forms restored; descriptor/index capture and post-store rereads remain.
// Round 161: Complete paired clamp results both/direct/mapped 382/356/291;
// all results before sample publication both/direct/mapped 380/356/291; native color
// results before sample publication both/direct 375/356. Original sample writes,
// shared writer, sampler and executable body restored.
// Round 185 indexed-first reversed constant/indexed/both bank arms 267/267/267; constant-first four
// bank orders 265 each; bank-first 242 plain (dropped for shared-writer regression). On bank-first:
// XOR 245; common masked value 290; masked XOR 290; dword packed 258; dword XOR 263; dword
// masked/both 320/320; signed packed 258 (fast). Original helper and body restored; descriptor
// capture and dirty-store scheduling remain.
// Shared writer reference 267; masked value 302; split stores 267; reference/mask 302;
// reference/split 267; mask/split and all three 302; named destination values 267.
// Native external indexed banks/constants/dirty words and their pairs/all 267 each; all fields with packed reference 267.
// Round 351: native component/mask/packed-color scalar home names in all seven combinations
// 267/267/267/267/267/267/267; all with descriptor/input/output ordinal names 267. Original names restored.
// Round 148: indexed destination pointers 267; constant destination pointers 296; all
// destination pointers 296; pointers/packed reference 296; pointers/dword packed 320;
// pointers/mask complement 274; pointers/XOR replacement 274; pointers/reference/complement 278.
// All executable forms restored.
// Round 353: short writer index, category/bank-first 266/255; shared complement
// 286/269; shared masked input 298/270; both mask parts 317/305.
// Every short-index form loses a matched sibling; original shared writer restored.
// Round 369: common traversal 845; bounded short counter 844; repeated component/shared color/both on common traversal 845 each, with short counter 844 each. All restored.
// Round 167: complete mask/color/remap domains 267/267/267/268/268/268/268/268;
// descriptor-aware direct/mapped/both writers 154/276/153; dword/reference/signed/short contracts 163/153/163/153/159;
// native word, source field, rescale capture and combined contracts 152/129/153/130/130;
// caller-masked contracts 184/254/192; actual input records/rescale bitfields 130/130/135/130;
// boolean/word rescale helpers 135/130/135/135; word accessors 130/130/130/130/130/130/130/130;
// shared writer macro with word/source domains 183/139/166/136 (other writer worsened, restored);
// redundant flag predicates 161/162/130/130; runtime descriptor/mask predicates 135/130/130/135/130/130;
// descriptor-aware writers/accessors all restored: new single-caller helpers did not match.
// The 130 form differs by the mapped rescale word load; closest diagnostic 129 was not retained.
// Round 242: Existing writer mask/color/index native references, seven nonempty
// combinations and all with descriptor reference, 267 each. Original shared
// writer restored; deferred index and descriptor rereads remain.
// Round 222: External destination references, indexed pair/constant-zero/constant-one
// subsets 267/287/287/287/287/296/296; all with reused preserved mask 274. Constant-one
// forms lose a matched writer sibling; complete original helper and body restored.
// Mapped source/index/descriptor-slot uninitialized declarations at branch entry,
// seven subsets, then all at function entry, 267 each. Original producer points,
// scopes, shared writer and complete body restored; descriptor/index schedule remains.
// Round 293: Shared writer common output/dirty completion, category/bank branch-or-conditional trees with bit/ordinal results: 444/471/438/447/465/475/456/457. Exact-only pointer forms all regress and lose a matched writer sibling; complete original helper and body restored.
// Round 428: actual mapping rows as signed-word pairs, long/short slot and mapped ordinal,
// rejection-first 267/267/266/266; positive admission 267/267/266/266. One-point
// bounded-ordinal forms rejected; original byte cursor, source, guard, writer and body restored.
// Round 42 (lane 16): the direct loop's store is the structural cause. A forceinline store taking the
// descriptor as `word const &` and reading the output index from it at each use gives retail's direct loop
// exactly (re-read after the store, lea address) 154 (1228 bytes); with the mapped loop's `packed` a word,
// the store called in both arms of the category test with index `remap + field` 152 (1225 bytes, mapped
// store exact); plus the mapping element through a `short const *source` read twice 130 (1225 bytes): only
// retail's rescale test after 0x1b2c0 (`mov ax,[ebx]; test ah,8`, ours `test byte [ebx+1],8`) differs.
// That needs a word value tested for two different bits (as in matched 0x12c770 and 0x1ab770); not found.
// On the 130 form: `packed` re-read for the rescale test 130; loop-top `packed` for the test only/for test
// and store 193/239 (spilled); re-read moved from after the pack call to after 0x1b2c0 126; rescale test
// inside the component loop 150. Same mapped forms without the direct-loop store 272; mapped index
// unconditional 163; mapped arms with a dword `packed` 227. The store as a function-like macro (index
// re-evaluated) gives the same 130 but 0x17170 291 (198 with mask/value locals); evaluating mask/value
// once in it 254 and loses 0x176a0. Not kept: the reference store has one caller, 0x17170 rises with it.
// Round 445: Native short/word/byte/signed-char four-channel rescale ordinals with
// long outer ordinals 356 each, with bounded short outer ordinals 355 each.
// Bodies shrink to 1166/1167 versus 1196/1225; original complete body and writer restored.
// Round 115: the direct loop through store_descriptor_mask (index read from the descriptor at each use; also
// 0x17170's direct loop) and the mapped loop in 0x176a0's shape (word `packed`, `short const *source`, store in
// both category arms, index `remap + field`) 130, kept. On it: `slot`/`component` locals dropped 130; the
// re-read moved from after the pack call to after 0x1b2c0 126 (one word kept in EDI; retail reloads it twice);
// loop-top `packed` for the test only 193 (spilled; retail reloads it from the entry, as if 0x1b2c0 kept memory)
// and for the test and store 239; that as a dword 305; `word const &packed` to the entry 254. Compile-only
// (not counted): word/short/bitfield/struct-copy locals, `!= 0`, `(x >> 8) & 8` and casts all fold to the
// memory test.
// Review: the remapped arm's one-use `index` local written at the call instead, 130 (same).
// Round 138 (lane 4; a one-file LTCG harness of this file plus unknown_131e50.cpp reproduces it): the rescale
// test through an inline rescale helper taking the descriptor 130; through an inline descriptor getter 130; a
// named masked word `word rescale = *(word const *)entry & 0x800;` 130; the word read before the 0x1b2c0 call
// and tested after it 193 (spilled); a bool read before the call 247. Harness only: `!= 0`/`== 0x800` tests,
// a short read, a word cast, the call result kept 130 each. Every live-across-call form spills to the stack;
// retail's unfolded `mov ax` needs a second use of the reloaded word between the two calls (not found).
// Round 27 (m3 lane 15, plain; `--fast` fails on this file; 130 since round 115, 267 before): retail's
// AX is dead after its test and its store reloads the word after the pack call, so the test's load must
// have a second use the optimizer drops (retail's entry shows the same unfolded form for `remapping`, which has two real uses).
// `(bool)((word >> 11) & 1)` 135; `bool rescale = (word & 0x800) != 0` 135; the word read once after
// 0x1b2c0 into a local used for the test and as the store's packed 126 (`test edi, 0x800`, kept in EDI
// across the pack call where retail reloads); the test in the loop condition, flag first/count first
// 170/171 (rolled loop, 1203/1202 bytes). Compile-only: short/indexed spellings, a word reference at the
// loop top or after the call, four rescale statements and a do/while all fold to the memory test; the
// test inside the component loop gives `mov ax, [..]; and ax, 0x800` and one test per component.
// Round 32 (m3 lane 14): rescale test inside the component loop 150 (retail's `mov ax, [ebx]` load, but
// `and ax, 0x800` and four tests); four per-component tests of a word local 151 (not threaded);
// `(word & 0x800) == 0x800` 130; the entry word read at each use with no loop-top local 130 (same code).
// Cause: retail's test load is the loop-top word CSE'd across the 0x1b2c0 call and reloaded from [ebx]
// (not spilled), which LTCG allows only when 0x1b2c0 writes through its `out` parameter alone. Our
// 0x1b2c0 writes cases 14/18 through a pointer copy (`real *zero = out + 1`), which makes callers assume
// it writes any memory. 0x1b2c0 forms (0x19ca0/0x1b2c0): cases 14/18 stored through `out` directly
// MATCH/6 (plain; VC then sinks the trailing out[2]/out[3] stores of cases 10, 12/13 and 14 and flips all
// three pairs); best direct spellings (`out[1] = out[2] = out[3] = 0.0f`, or cases 12/13 written out[3]
// first) MATCH/2; `real *const zero` 130/MATCH; a pointer to `&out[1]` 130/MATCH; existing
// vector3f_assign/real_point3d_set on `(vector3f *)(out + 1)` 130/MATCH each; a plain `for (long i = 1;
// i < 4; i++) out[i] = 0.0f;` MATCH/78 (any loop in 0x1b2c0 moves its first switch's shared clamp tail
// from case 2 to case 4). Compile-only (reproduces 0x1b2c0 exactly): ~400 direct spellings of cases
// 10-20, merged 14/18 labels, `return true` in the case, fall-through into case 12, `(out + 1)[k]`, cast
// fields and `out++` all floor at 2 or worse. Next: a 0x1b2c0 form that hides case 14's stores from the
// sinking without a pointer copy or a loop.
// MiniMax S (10-31): confirmed the cause and its price. Writing cases 14/18 of 0x1b2c0 straight through `out`
// (no `real *zero = out + 1` copy) makes LTCG lose the knowledge that the call leaves the entry table alone, so
// 0x19ca0 matches retail byte for byte (MATCH, 0). The price: VC then emits case 14/18's three zero stores as
// out[1], out[3], out[2] where retail has out[1], out[2], out[3], so 0x1b2c0 goes from MATCH to 2. Every direct
// spelling tried (~60: all six store orders, chains, `out` and `out + 1` pointer copies, dependency chains, the
// width store in each of the four positions, the zeros before the width) floors at 2; the pointer copy keeps
// 0x1b2c0 exact but leaves 0x19ca0 at 130, so the direct form is not kept (it raises another function's score here).
// Round 279: Existing native four-vector persistent sample output, indexed/field consumers
// with separate/common color workspace: native zero 130 each, CRT zero 170 each. All eight
// exact-only storage forms restored, including the temporary include; sampler and writers
// unchanged. The mapped rescale word load remains.
// @retail 0x19ca0
void function_19ca0(byte *state, word const *range)
{
    byte const *definition = *(byte **)(state + 0x20);
    byte const *entry = *(byte **)(definition + 0x3c) + (*range & 0x1ff) * 4;
    word remapping = shader_remapping(state)->packed;
    real values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (!(remapping & 0xfe00))
    {
        for (long i = 0; i < (*range >> 9); ++i)
        {
            long component = entry[2] & 7;
            dword mask = g_467068[component];
            function_1b2c0(state, values, entry[3], entry[2] & 7, 0, (entry[2] >> 3) & 7);
            if (*(word const *)entry & 0x800)
                for (long j = 0; j < 4; ++j) values[j] = (values[j] + 1.0f) * 0.5f;
            values[0] = values[0] < 0.0f ? 0.0f : values[0] > 1.0f ? 1.0f : values[0];
            values[1] = values[1] < 0.0f ? 0.0f : values[1] > 1.0f ? 1.0f : values[1];
            values[2] = values[2] < 0.0f ? 0.0f : values[2] > 1.0f ? 1.0f : values[2];
            values[3] = values[3] < 0.0f ? 0.0f : values[3] > 1.0f ? 1.0f : values[3];
            color4f color;
            color.red = values[0]; color.green = values[1]; color.blue = values[2]; color.alpha = values[3];
            dword value = pack_color4f(&color);
            store_descriptor_mask(state, *(word const *)entry, mask, value);
            entry += 4;
        }
    }
    else
    {
        byte const *mapping = *(byte **)(definition + 0x24) + (remapping & 0x1ff) * 4;
        for (long i = 0; i < (*range >> 9); ++i)
        {
            word packed = *(word const *)entry;
            long slot = packed & 7;
            short const *source = (short const *)(mapping + slot * 4);
            if (*source != NONE && ((packed >> 3) & 7) != *(long *)(state + 0x300 + *source * 4))
            {
                entry += 4;
                continue;
            }
            long component = entry[2] & 7;
            dword mask = g_467068[component];
            function_1b2c0(state, values, entry[3], entry[2] & 7, 0, (entry[2] >> 3) & 7);
            if (*(word const *)entry & 0x800)
                for (long j = 0; j < 4; ++j) values[j] = (values[j] + 1.0f) * 0.5f;
            values[0] = values[0] < 0.0f ? 0.0f : values[0] > 1.0f ? 1.0f : values[0];
            values[1] = values[1] < 0.0f ? 0.0f : values[1] > 1.0f ? 1.0f : values[1];
            values[2] = values[2] < 0.0f ? 0.0f : values[2] > 1.0f ? 1.0f : values[2];
            values[3] = values[3] < 0.0f ? 0.0f : values[3] > 1.0f ? 1.0f : values[3];
            color4f color;
            color.red = values[0]; color.green = values[1]; color.blue = values[2]; color.alpha = values[3];
            dword value = pack_color4f(&color);
            packed = *(word const *)entry;
            if ((packed & 0x3c0) != 0x200)
            {
                store_shader_mask(state, packed, *(signed char *)(state + 0x1520 + (packed & 7)) + ((packed >> 6) & 15), mask, value);
            }
            else
            {
                store_shader_mask(state, packed, (packed >> 6) & 15, mask, value);
            }
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
