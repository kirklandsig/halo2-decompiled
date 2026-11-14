#include <math.h>
#include <new.h>
#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "bit_vector.h"
#include "unknown_19ec40.h"
#include "unknown_1efac0.h"
#include "unknown_2420a0.h"
#include "unknown_13927e.h"
#include "unknown_1523c0.h"
#include "game_engine_events.h"
#include "engine_object_definition.h"

// @flags /O2 /arch:SSE /Gr
#include "view_context_storage.h"

/* ---- globals ---- */
s_slot_table *g_51ec80;

/* the two colors (alpha, then red, green and blue) of the markers drawn by
   function_243ed0 */
color4f g_468c80[2] =
{
	{ 1.0f, 0.4588235318660736f, 0.729411780834198f, 1.0f },
	{ 1.0f, 0.8078431487083435f, 0.5607843399047852f, 0.8705882430076599f },
};

/* ---- views of other data ---- */
struct s_slot_object
{
	long definition_index;
	byte unknown04[0xaa - 0x04];
	byte object_type;
	byte unknownab[0xc2 - 0xab];
	short sc2;
	long lc4;
	long lc8;
	byte unknowncc[0x13c - 0xcc];
	long player_index;
	byte unknown140[0x17e - 0x140];
	short slot;
	byte unknown180[0x1fc - 0x180];
	short s1fc;
	byte unknown1fe[0x212 - 0x1fe];
	char field_x11c898;
};

struct s_slot_object_header
{
	byte unknown00[8];
	s_slot_object *object;
};

struct s_player_view
{
	byte unknown00[0x2c];
	long unit_index;
	byte unknown30[0x88 - 0x30];
	byte type;
	byte unknown89[0xc0 - 0x89];
	char team;
	byte unknownc1[0x21c - 0xc1];
};

/* the options of the engine (the game options, g_4e6948) */
struct s_ctf_options
{
	byte unknown00[0xc];
	char mode;
	byte unknown0d[0x180 - 0xd];
	long engine_type;
	byte unknown184[0x22c - 0x184];
	union
	{
		dword flags;
		struct
		{
			dword bit0 : 1;
			dword bit1 : 1;
			dword bit2 : 1;
			dword bit3 : 1;
			dword bit4 : 1;
			dword bit5 : 1;
			dword bit6 : 1;
			dword bit7 : 1;
			dword unknown : 24;
		} bits;
	} flags22c;
	short s230;
	short s232;
	long carrier_speed;
	long l238;
	byte unknown23c[4];
	long team_mode;
	short scale_a;
	short scale_b;
	byte unknown248[0x1128 - 0x248];
	bool b1128;
};

static inline s_ctf_options *ctf_options()
{
	return (s_ctf_options *)g_4e6948;
}

static inline s_slot_object *slot_object_get(long object_index)
{
	return ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

static inline s_player_view *ctf_player_get(long player_index)
{
	return (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_player_view));
}

void function_b58c0(long index, dword mask);
long function_146650(void);

/* the engine's globals changed: tell the clients */
static inline void ctf_globals_changed(dword mask)
{
	if (g_55e4d0[g_4e9ae8->engine_index] && g_4e9ae8->value24 != NONE)
		function_b58c0(g_4e9ae8->value24, mask);
}

/* rounds as the x87 does */
static __forceinline long ctf_real_to_long(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

struct s_team_entry
{
	long object_index;
	short team;
	byte unknown06[2];
	long l8;
	long lc;
};

long player_find_unit_item_of_type(long, long);
bool player_iterator_next(long *iterator);
void function_1967d0(long a, long c, long b, long delta);
void function_1970a0(long b, long a, long delta);
void function_197210(long player_index, long value14, long value10);
s_event *game_engine_event_initialize(long type, long subtype, s_event *event);
void game_engine_event_set_cause_player(long player_index, s_event *event);
point3f *function_b9dd0(long object_index, point3f *result);
s_team_entry *function_15e410(short team);
bool function_161e10(long team);
long function_161eb0(long team);
bool function_15b2f0();
short function_101280(long object_index);
long function_cbd50(long object_index, short index);

/* callees not decompiled yet (stubs in src/stubs/lane_o.cpp) */
short function_158990(long team);
void __stdcall function_a7810(dword mask);
void __stdcall function_a7870(long object_index);
struct s_effect_owner;
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
long __stdcall function_b7b40(void *data);
void function_15e050(long object_index, short value);
void function_15e130(long object_index);
void __stdcall function_b8540(long a);
void function_157670();
void function_15e4d0();
color3f function_7f720(short team_index);
bool function_bacc0(long object_index, long index, point3f const *point);
bool function_138860();
bool function_138880();
void function_15fe70(long player_index);
void function_15b3a0(long team, bool a);

/* ---- the game engine class whose vtable is at 0x459d18 ---- */
struct s_marker_update;

struct s_ctf_query
{
	byte unknown00[0xc];
	dword flags;
};

class c_game_engine_markers
{
public:
	virtual long v0();
	virtual bool v1();
	virtual void v2() {}
	virtual void v3() {}
	virtual bool v4(long index);
	virtual void v5() {}
	virtual void v6(long player_index);
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual void v10() {}
	virtual void v11() {}
	virtual void v12();
	virtual void v13(long arg_0);
	virtual void v14(long arg_0);
	virtual void v15(long player_index) {}
	virtual bool v16(long player_index, long object_index);
	virtual void v17(long player_index, long object_index) {}
	virtual void v18();
	virtual real v19(long player_index);
	virtual long v20(long object_index);
	virtual void v21(long object_index);
	virtual void v22() {}
	virtual void v23(long object_index, long unit_index);
	virtual void v24(long object_index, long unit_index);
	virtual long v25(long ticks, bool a, bool b);
	virtual long v26();
	virtual void v27() {}
	virtual void v28() {}
	virtual void v29() {}
	virtual void v30(long killer, long victim, bool suicide, long);
	virtual long v31(long killer, long victim, bool suicide);
	virtual void v32() {}
	virtual bool v33(long player_index, s_ctf_query const *query);
	virtual bool v34(long value);
	virtual bool v35(long player_index, long type);
	virtual void v36(long value);
	virtual long v37(long player_index, byte *flag);
	virtual void v38() {}
	virtual void v39() {}
	virtual bool v40();
	virtual void v41(s_marker_update *update) {}
	virtual void v42(dword mask, dword *changed, s_marker_update *update) {}
	virtual byte v43(dword mask, s_marker_update *update) { return 0; }
	virtual void v44(long, s_marker_update *update);
	virtual void v45(dword *flags, long, s_marker_update *update);
	virtual bool v46(dword flags, long, s_marker_update *update);
	virtual void v47() {}
	virtual void v48() {}
	virtual void v49() {}
	virtual void v50() {}
};

/* the copy of the slot table a client keeps */
struct s_marker_update
{
	byte unknown00[0x24];
	long l24;
	long l28;
	short a[9];
	short b[9];
	byte c[9];
	byte unknown59[3];
	s_long_triple triples[3];
	byte b80;
	byte unknown81[3];
};

// @retail 0x2420a0
bool c_game_engine_markers::v4(long index)
{
	long none = NONE;

	if (g_51ec80->a[index] == none && g_51ec80->b[index] == none)
		return false;
	return true;
}

// @retail 0x2420d0
long c_game_engine_markers::v37(long player_index, byte *flag)
{
	s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);
	long result = NONE;

	*flag = 1;
	if (*((byte *)player + 0xc0) != 0xff)
	{
		if (player_find_unit_item_of_type(player_index, 1) != NONE)
			result = (g_4e6948->mode_180 == 9) * 8 + 0xb;
	}

	return result;
}

// @retail 0x242140
real function_242140(long object_index)
{
	s_slot_object *object = ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	real result = 0.0f;
	long slot = object->slot;
	short scale = g_4e6948->scale_a;
	real divisor = (real)(scale == 0 ? 1 : scale);

	if (slot != NONE)
	{
		if (g_51ec80->flags[slot] & 8)
		{
			result = 1.0f;
		}
		else
		{
			short count = g_51ec80->e[slot];
			long n = count > 0 ? count : 0;

			result = g_510c54->rate / divisor * (real)n;
		}
	}

	return result;
}

/* the slot (0..8) whose first or second identifier is the value, or 9 */
PRIVATE long slot_find(long value)
{
	long i;

	for (i = 0; i < 9; i++)
	{
		if (g_51ec80->b[i] == value || g_51ec80->c[i] == value)
			break;
	}

	return i;
}

PRIVATE long slot_name(long slot)
{
	if (slot < 9)
		return (short)g_4e9ae8->name.c[slot];
	return NONE;
}

// @retail 0x242b60
long function_242b60(long value)
{
	return slot_name(slot_find(value));
}

// @retail 0x243400
void c_game_engine_markers::v45(dword *flags, long, s_marker_update *update)
{
	dword changed = 0;
	dword mask = *flags & 0x1f;
	s_slot_table *g;
	long i;

	if (mask)
		v42(mask, &changed, update);

	g = g_51ec80;
	if (*flags & 0x20)
	{
		long value = g->l1f4;

		if (update->l24 != value)
		{
			update->l24 = value;
			changed |= 0x20;
		}
	}

	if ((*flags >> 7) & 1)
	{
		for (i = 0; i < 9; i++)
		{
			short value = g->d[i];

			if (update->a[i] != value)
			{
				update->a[i] = value;
				changed |= 0x80;
			}
		}
	}

	if (*flags & 0x100)
	{
		for (i = 0; i < 9; i++)
		{
			short value = g->e[i];

			if (update->b[i] != value)
			{
				update->b[i] = value;
				changed |= 0x100;
			}
		}
	}

	if (*flags & 0x200)
	{
		for (i = 0; i < 9; i++)
		{
			byte value = g->flags[i];

			if (update->c[i] != value)
			{
				update->c[i] = value;
				changed |= 0x200;
			}
		}
	}

	if (*flags & 0x400)
	{
		i = 0;
		do
		{
			long a = g->triples[i].a == NONE ? NONE : g->triples[i].a & 0xffff;

			if (update->triples[i].a != a)
			{
				update->triples[i].a = a;
				changed |= 0x400;
			}
			long b = g->triples[i].b == NONE ? NONE : g->triples[i].b & 0xffff;
			if (update->triples[i].b != b)
			{
				update->triples[i].b = b;
				changed |= 0x400;
			}
			long c = g->triples[i].c == NONE ? NONE : g->triples[i].c & 0xffff;
			if (update->triples[i].c != c)
			{
				update->triples[i].c = c;
				changed |= 0x400;
			}
			i++;
		}
		while (i < 3);
	}

	*flags = changed;
}

/* true when a marker with these flags applies in the current team mode */
#define MARKER_APPLIES(flags, options) \
	(((flags) & 1) && (options)->team_mode != 1 && (options)->team_mode != 2 || \
	 ((flags) & 2) && (options)->team_mode == 1 || \
	 ((flags) & 4) && (options)->team_mode == 2)

// @retail 0x243b10
long function_243b10(long key_b, bool team_only, long *second)
{
	s_game_options_view *options = g_4e6948;
	long results[8];
	long key_a;

	if (team_only)
		key_a = options->mode_180 != 9 ? 0 : 2;
	else
		key_a = (options->mode_180 == 9) * 2 + 1;

	long count = function_19ec40(0, 0.0f, (short)key_a, (short)key_b, 0, 8, results, 0.0f);
	long result = NONE;
	long i = 0;

	if (count > 0)
	{
		do
		{
			long index = results[i];
			s_marker_entry *entry = g_4e0350->marker_entries + index;

			if (MARKER_APPLIES(entry->flags, options))
			{
				if (result == NONE)
				{
					result = index;
				}
				else
				{
					if (second)
						*second = results[i];
					break;
				}
			}
			i++;
		}
		while (i < count);
	}

	return result;
}

PRIVATE __forceinline void function_2417a1(s_marker_entry const *arg_0, long arg_1, point3f *arg_2);

// score 2 (was 173): 711 bytes as retail; only the other position's z copy is stored one instruction
// later (after `fld st(1)`, filling the load latency); retail stores it first.
// Earlier (173): frame 0x40 against retail's 0x44. Retail keeps the slot table in EBP through
// the inner loop (re-reading g_51ec80 when EBP is borrowed, and in the marker select's else arm) and
// keeps the inner index on the stack; ours re-reads the table at its use and keeps the index in EAX.
// Retail's x87 distance squares dx first and pops it there; ours squares dy first.
// Round 77 (plain): no table local 200; the select's else arm through the global 200; the result index
// named first 173; the marker position through function_2417a1 (moved above, retail's y-first copy
// shape) 172; the index declared in the for 173; the count named result_count 173.
// Round 8 (plain, on the function_2417a1 copy): the distance through magnitude2d 172; the other
// position read at each use 223 (666 B); the table local at the loop top 169 / 180 (the slot moves to
// the stack, caller 0x240160 lost); the marker select as if/else 188; the index declared first 172.
// Round 87 (plain): the entry as `marker_entries + results[j]` 173, the sum as dy * dy + dx * dx 173,
// a flags local for the three clauses 173, rejected markers `continue` 173 (all four identical code);
// the squared distance accumulated (`d = dx * dx; d += dy * dy;` as 0x1b0cd0's file does) then sqrt
// 173: that gives retail's exact x87 sequence, so keep it in any later fix. Left: retail spills the
// inner index (the frame's extra dword) and keeps the table in EBP through the inner loop; ours keeps
// the index in EAX and folds the entry flags load to `[ecx + ebp + 0x16]`, borrowing EBP.
// Round 89 (plain, each with the accumulated distance): 0x243b10's `if (count > 0) do ... while`
// loop 186; an options local as 0x243b10 has 171 (clause code identical, only the key's registers
// move); the select through the global with the table local after it 169 at 717 B (the slot moves
// to the stack, caller 0x240160 lost); the index renamed 171 (identical); ctf_options() in the key
// and the marker test 173; the position through marker_position() 173; the table at each use 200.
// Retail loads the team mode into EBP while EAX still holds the options pointer (ours coalesces both
// in EBP), so EAX looks busy in retail's clause 1 too; the index spill is the cause to find.
// Round 71 (plain, each on the accumulated distance, which alone stays 173): the select's else arm
// through the global 200; both select arms through the global 200; the inner index and count
// declared at function scope 173; the table local assigned at the outer loop top 169 (the slot
// parameter moves to the stack and caller 0x240160 is lost). Retail also keeps count in memory
// from its definition (stored straight from EAX) and stores j = 0 at once.
// Round 111 (fast; a fast image of this function alone builds 0x19ec40 differently, so include
// 0x1e5bb0/0x1e2a90/0x29b00): accumulated distance through a distance2d helper 173; ten placements
// of the j/key/count/point declarations 173 (193-195 with the point read before the query); named
// result index 173; table local inside the `marker_index != NONE` block 169 at 717 B, but matched
// caller 0x240160 drops to 46. Original restored.
// Round 176 (fast, each on the accumulated distance, which alone stays 173): an options local per inner
// iteration 173 (identical); a short marker index 180; the array selected by k, then indexed, 204
// (caller 0x240160 lost); the y-first copy helper function_2417a1 172 (the copy is then retail's; the
// count stays in ECX and the index in EAX where retail keeps the count in EAX and spills the index);
// sqrtf 173. The table local at the loop top or in the marker block (169, 717 B) hoists the table to
// EDI and moves the slot to EBP from the stack, so it is not retail's allocation either.
// Round 45 (lane 11, plain; a fast image builds 0x19ec40 differently here, 203 for this body): the distance's
// squares named (`dx_squared`, `dy_squared`) 173; the delta as a vector2f through dot_product2d 173; a named
// squared distance 173 (all identical code); the x maximum written bound-first like y and z 152 (701 bytes, a
// positional gain only: the x compare then loads the bound into a register where retail compares the distance
// with the memory bound, so not kept), with the squares reversed 152, with the accumulated distance 152.
// Round 126 (m3 lane 6, fast with 0x1e5bb0/0x1e2a90/0x29b00/0x19ec40 in the image; alone it builds 203 at 690 B),
// all on the accumulated distance (aligned difference 58 -> 53, still 173): the inner index declared in its for 173,
// at the marker block's top 173; with the y-first copy helper function_2417a1 for the marker position 172, plus the
// index in the for 172 or at the block top 172; the table local at the loop top with the select's else arm through
// the global 169 (717 B, caller 0x240160 46). Retail's count stays in EAX and is stored straight to its slot; the
// spilled inner index and the EBP table remain.
// Round 45 (lane 11, plain; a fast image builds 0x19ec40 differently here, 203 for this body): the distance's
// squares named (`dx_squared`, `dy_squared`) 173; the delta as a vector2f through dot_product2d 173; a named
// squared distance 173 (all identical code); the x maximum written bound-first like y and z 152 (701 bytes, a
// positional gain only: the x compare then loads the bound into a register where retail compares the distance
// with the memory bound, so not kept), with the squares reversed 152, with the accumulated distance 152.
// Round 186 (fast, with 0x1e5bb0/0x1e2a90/0x29b00 in the image): a loop-invariant marker-entries pointer
// read before the inner loop 170 (702 bytes; frame 0x44 as retail and the first difference moves to +0x20,
// but the pointer takes the spilled slot retail gives the inner index, and the index stays in EAX), with the
// accumulated distance 170, with an options local for the clauses 170, with both 170; the table read through
// the global at each use with the accumulated distance 200 (688 bytes). Retail keeps the table in EBP across
// the inner loop and reuses it after the 0x19ec40 call (LTCG knows the call leaves g_51ec80 alone); ours
// reloads the table for the x updates and borrows EBP for the entries base, so the index keeps a register.
// Round 115 (fast with 0x19ec40 and its three callees; LCS-aligned distance in brackets, base 173 [58]): the
// accumulated distance 173 [53]; the select's else arm through the global 200 [64], with the accumulated
// distance 200 [59]; `if (marker_index == NONE) continue;` 173 [58], with the accumulated distance 173 [53];
// count and the inner index declared before the query 173 [53]; no table local, the global at each use, with
// the accumulated distance 200 [59] (688 bytes).
// Round 20 (lane 17, fast with 0x19ec40's callees in the image): the existing y-first copy helper
// function_2417a1 for the marker position with the accumulated distance 172 (copy and x87 code
// retail's; the count still moves to ECX after the call where retail keeps it in EAX and spills j); on it:
// the result index named 172, the count declared apart and compared as `count > j` 172, the results
// array at function scope 172, an options local at function scope for the key and the marker test (as
// 0x243b10) 169; on 169: the named result index 169, the options local per marker block 172, the inner
// loop as `while` with `j++` 169, the results array at function scope 169, the marker select as if/else 188.
// Round 22 (lane 17, fast with 0x19ec40 and its callees 0x1e5bb0/0x1e2a90/0x29b00 in the image, which agrees with
// the full count; without them the image builds 203): the accumulated distance 173; the table local assigned at
// the top of each k iteration 169 (717 bytes; the slot parameter moves to the stack, the table is hoisted to
// EDI and caller 0x240160 drops to 46), with the accumulated distance 169; `long j` declared in the for 173;
// count declared before the results array and assigned from the query 173.
// Round 55 (lane 11, fast with 0x19ec40's callees, each on the accumulated distance): the table assigned per
// select arm, read through the table 184, through the global 184, the read before the assignment 184 (717 B each,
// the table hoisted to EDI, the slot on the stack, caller 0x240160 46); the select's else arm through an inline
// getter with its own table local 200 (688 B); the marker test through an inline bool helper on the entry 173,
// forced inline 173, on the flags with an options local 173, with an if/return body 173 (same code); the marker
// position through an entry pointer local 172, with const entry and other 172, entry local with the fields copied
// y first 190, x first 190. Diagnostic, reverted at once: the else arm through a const cast of the global's
// address 199 (692 B; frame 0x44 and retail's else-arm reload appear, caller 0x240160 7).
// Round 118 (fast with 0x19ec40 and its three callees; the one-file harness builds 203 here, so it is no guide): the
// select with `k != 0` and the arms swapped plus the accumulated distance 175; no `slots` local outside the loop, the
// select through the global and `slots` at the marker block's top 169 (717 B, caller 0x240160 46), with the accumulated
// distance 169 (aligned 83 against 54); `slots` read after the 0x19ec40 call 169, with the accumulated distance 169.
// Lead: `test; mov ebp, [g]; jne` is the same first instruction of both select arms merged upward, so retail's else arm
// loads the table twice; every form that reads the global outside `slots` hoists it out of the loop.
// Round 140 (m3 lane 5, fast with 0x19ec40 and its three callees): one `point3f *bounds` picked by `k == 0 ?
// &bounds[0][slot] : &bounds[1][slot]` as matched 0x244680 does, with the accumulated distance, 388 (499 B: retail
// really repeats the k test at each use); the accumulated distance computed inside each k arm 197 (743 B, VC does
// not merge the two copies upward); the table through the global at each use 200 (688 B; both select arms then load
// it into their own scratch register and nothing stays in EBP); `short key` 173 (same code); a function-scope pointer
// to g_4e0350 for the position and the inner loop 178.
// Round 61 (m3 lane 10, fast with 0x19ec40/0x1e5bb0/0x1e2a90/0x29b00/0x240160): retail reloads the table from
// g_51ec80 itself after borrowing EBP (`mov ebp, [0x51ec80]` in the clause code) where ours reloads the `slots`
// local's spill; the global at every use re-measured 200 (688 bytes, as rounds 89/115; both select arms then load
// it into EAX/ECX and EBP stays free); on it the accumulated distance 200 (as round 115), `if (marker_index == NONE)
// continue;` 200, `k != 0` with the arms swapped 200, the array selected by k and then indexed 191 (691 bytes, caller
// 0x240160 46). On the current body: the key as `mode_180 == 9 ? 3 : 1` 173 (same code); dy computed before dx 174;
// the bound-update arms as `if (k != 0)` with the arms swapped 173.
// Round 65 (m3 lane 10, fast with 0x19ec40/0x1e5bb0/0x1e2a90/0x29b00/0x240160): the select as `!k ? ...` 173; on the
// accumulated distance: the clause through ctf_options() 173, a const marker point 173, the other position as a const
// reference 219 (666 B), the inner loop as `while` with `j++` at the body end 173.
// Round 79 (m3 lane 9; fast with 0x19ec40/0x1e5bb0/0x1e2a90/0x29b00/0x240160, plain confirmed): kept: no table
// local (g_51ec80 at each use), the bounds init stores before the query and round 87's accumulated distance 146
// (711 bytes, retail's length; the else arm's global reload is now retail's; k is spilled and the count held in EBX
// where retail spills j and count and keeps k in EBX and the table in EBP). With the slots local, the init stores
// before the query 185 (711 bytes). On 146: the init stores between the query and the point copy (retail's order)
// 142 (caller 0x240160 46: not kept); the marker select as if/else 184; k declared in its for 146; results at
// function scope 146; plain sqrt(dx * dx + dy * dy) 150; the key before the init stores 186 (caller lost).
// NOT kept (review): the 146 combines a re-read of g_51ec80 at each use where retail holds the table in EBP,
// the init stores placed before the query where retail stores them after it, and a two-statement distance
// sum: a combination picked by score; body restored at 173.
// Round 208 (m3 lane 3, fast with 0x19ec40/0x1e5bb0/0x1e2a90/0x29b00/0x240160, plain confirmed): kept g_51ec80 read
// at each use (the init stores stay after the point copy, retail's order), the existing function_2417a1 for both
// positions and the squared distance accumulated in its own named local: 2 at 711 bytes (retail's EBP table with
// its reloads, frame 0x44, spilled count and index, x87 squares). Grid of those four (table/marker copy/other copy/
// accumulated): 171-200 with any one missing except 29/23 without the other copy and 8 with the summed distance.
// The other element pointer then had one use and was dropped (2). On 2: dy before dx 16; named squares 8; a
// vector2f through magnitude2d 8; sqrtf 2; a double cast on the sqrt argument 11; distance_squared declared first 2;
// other_point at the loop top 2; `other` const 2; a 600 s permuter run, 66 tries, none below 2 (its reduced image
// scores the base 178, so it cannot guide this function).
// Round 67 (m3 lane 1, plain with 0x240160/0x19ec40; fast differs here: fast 203 for the plain 173): the accumulated
// distance `d = dx * dx; d += dy * dy;` 173; the `slots` local declared at the top of the k loop 169 (717 bytes, but
// the slot argument moves to the stack and caller 0x240160 drops to 46: not kept); `sqrt(dy * dy + dx * dx)` 173
// (same code); `if (marker_index == NONE) continue;` 173; the init stores through the existing real_point3d_set 200.
// Round 43 (lane 17, fast with 0x19ec40 and its callees, which agrees with the plain 2): the result index named once
// for the marker test and the copy 193 (690 bytes); the squared distance in a double 13 (frame 0x48); the marker point
// declared first in its block 2; dx and dy declared before the copy 2; `continue` on a failed marker test 2 (all
// three identical). The compile-only harness schedules this block differently (fld before the z load), so it is no
// guide here. Original body restored.
// Round 108: both squares named before accumulation 8; only the x square named 2; only the y square named,
// or assigned in the accumulation expression, 8; a two-use other-z local 23; the inner copy through memcpy 2;
// the inner getter/structure copy 2; the existing three-component setter 176 (716 bytes); reversed second-sum
// operands 2; getter/structure copies for both points 2; both z differences named after the distance 108 (694 bytes).
// The named y square moves the z store to retail's position but changes the x87 operations. All forms restored.
// @retail 0x243c00
void function_243c00(long slot)
{
	long k;

	for (k = 0; k < 2; k++)
	{
		long marker_index = k == 0 ? g_51ec80->b[slot] : g_51ec80->c[slot];

		if (marker_index != NONE)
		{
			long results[8];
			long key = (g_4e6948->mode_180 == 9) * 2 + 1;
			long count = function_19ec40(0, 0.0f, (short)key, (short)slot, (short)(k + 1), 8, results, 0.0f);
			point3f point;
			function_2417a1(g_4e0350->marker_entries, marker_index, &point);
			long j;

			if (k == 0)
			{
				g_51ec80->bounds[0][slot].x = 0.5f;
				g_51ec80->bounds[0][slot].y = 0.0f;
				g_51ec80->bounds[0][slot].z = 0.0f;
			}
			else
			{
				g_51ec80->bounds[1][slot].x = 0.5f;
				g_51ec80->bounds[1][slot].y = 0.0f;
				g_51ec80->bounds[1][slot].z = 0.0f;
			}

			for (j = 0; j < count; j++)
			{
				if (MARKER_APPLIES(g_4e0350->marker_entries[results[j]].flags, g_4e6948))
				{
					point3f other_point;
					function_2417a1(g_4e0350->marker_entries, results[j], &other_point);
					real dx = point.x - other_point.x;
					real dy = point.y - other_point.y;
					real distance_squared = dx * dx;
					distance_squared += dy * dy;
					real distance = (real)sqrt(distance_squared);

					if (k == 0)
					{
						g_51ec80->bounds[0][slot].x = distance > g_51ec80->bounds[0][slot].x ? distance : g_51ec80->bounds[0][slot].x;
						g_51ec80->bounds[0][slot].y = g_51ec80->bounds[0][slot].y > point.z - other_point.z ? g_51ec80->bounds[0][slot].y : point.z - other_point.z;
						g_51ec80->bounds[0][slot].z = g_51ec80->bounds[0][slot].z > other_point.z - point.z ? g_51ec80->bounds[0][slot].z : other_point.z - point.z;
					}
					else
					{
						g_51ec80->bounds[1][slot].x = distance > g_51ec80->bounds[1][slot].x ? distance : g_51ec80->bounds[1][slot].x;
						g_51ec80->bounds[1][slot].y = g_51ec80->bounds[1][slot].y > point.z - other_point.z ? g_51ec80->bounds[1][slot].y : point.z - other_point.z;
						g_51ec80->bounds[1][slot].z = g_51ec80->bounds[1][slot].z > other_point.z - point.z ? g_51ec80->bounds[1][slot].z : other_point.z - point.z;
					}
				}
			}

			if (k == 0)
			{
				g_51ec80->bounds[0][slot].y += 0.1f;
				g_51ec80->bounds[0][slot].z += 0.9f;
			}
			else
			{
				g_51ec80->bounds[1][slot].y += 0.1f;
				g_51ec80->bounds[1][slot].z += 0.9f;
			}
		}
	}
}

/* ---- the marker list ---- */
PRIVATE s_color_bits *marker_color()
{
	color4f *color = &g_468c80[0];

	if (g_4b9ed8 != NONE)
	{
		long player_index = g_4e8c20->entries[g_4b9ed8];

		if (player_index != NONE)
		{
			s_player_view *player = (s_player_view *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c);

			if (player->type == 1 || player->type == 3)
				color = &g_468c80[1];
		}
	}

	return (s_color_bits *)&color->red;
}

PRIVATE __forceinline s_color_bits *function_243ed1()
{
    s_color_bits *local_0 = (s_color_bits *)((byte *)g_468c80 + 4);
    if (g_4b9ed8 != NONE)
    {
        long local_1 = g_4e8c20->entries[g_4b9ed8];
        if (local_1 != NONE)
        {
            s_player_view *local_2 = (s_player_view *)(g_4e8c24->data + (local_1 & 0xffff) * 0x21c);
            if (local_2->type == 1 || local_2->type == 3)
                local_0 = (s_color_bits *)((byte *)(g_468c80 + 1) + 4);
        }
    }
    return local_0;
}

#pragma inline_depth(1)
// @retail 0x243ed0
bool function_243ed0(s_marker_list *list, point3f const *position)
{
	list->b0 = 1;
	list->b1 = 0;
	list->l4 = 1;
	list->position = *position;
	list->r14 = 0.0f;
	list->r18 = 0.1f;
	list->r1c = 0.0f;
	list->l20 = NONE;
	list->color24 = *function_243ed1();
	list->color30 = *function_243ed1();
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 1;
	list->items[0].a = *function_243ed1();
	list->items[0].b = *function_243ed1();
	list->items[0].kind = 0;
	list->items[0].r = 1.0f;
	list->items[0].index = NONE;
	return true;
}
#pragma inline_depth(255)

// @retail 0x2440a0
bool function_2440a0(s_marker_list *list, long object_index, point3f const *point)
{
	s_slot_object *object = ((s_slot_object_header *)g_4e0300->data)[object_index & 0xffff].object;

	if (list->count < 2 && object->slot != NONE)
	{
		s_game_options_view *options = g_4e6948;
		long divisor = options->divisor == 0 ? 1 : options->divisor;
		long scale = options->scale_b == 0 ? 5 : options->scale_b;
		long value = NONE;

		list->items[list->count].a = *(s_color_bits *)point;
		list->items[list->count].b = *(s_color_bits *)point;
		list->items[list->count].kind = (options->mode_180 == 9) + 1;
		list->items[list->count].index = NONE;

		short slot = object->slot;
		if (slot >= 0 && slot < 9)
			value = g_51ec80->d[slot];

		if (options->mode_180 == 9 && (g_51ec80->flags[slot] & 2))
			list->items[list->count].r = value == NONE ? 0.0f : (real)value / (real)scale;
		else
			list->items[list->count].r = value == NONE ? 0.0f : (real)value / (real)divisor;

		list->b0 = 1;
		list->count++;
	}

	return true;
}

/* ---- the object whose destructor is at 0x244930 ---- */
#include "unknown_244980.h"

// @retail 0x244930 deleting c_marker_object

// @retail 0x244bb0
c_marker_object *c_marker_object::create(dword value, void *memory)
{
	return new (memory) c_marker_object(value, value0c);
}

/* ---- the list of lines drawn by a marker ---- */
struct s_line_a
{
	long a;
	long b;
	long c;
	byte d;
	byte e;
	short f;
	point3f position;
	real r1c;
};

struct s_line_b
{
	long a;
	long b;
	long c;
	byte d;
	byte e;
	short f;
	point3f position;
	real r1c;
	real r20;
	real r24;
	real r28;
};

struct s_line_list
{
	short count_a;
	short count_b;
	byte unknown04[4];
	s_line_a lines_a[0x100];
	s_line_b lines_b[0x100];
};

PRIVATE inline void function_244d51(point3f *arg_0, real arg_1, real arg_2, real arg_3)
{
	arg_0->x = arg_1;
	arg_0->y = arg_2;
	arg_0->z = arg_3;
}

// @retail 0x244ca0
void function_244ca0(real height, real radius, s_line_list *list, long a, long b, long c, byte d, byte e, point3f const *position, short f)
{
	if (list->count_a < 0x100)
	{
		s_line_a *line = &list->lines_a[list->count_a++];

		line->a = a;
		line->b = b;
		line->d = d;
		line->e = e;
		line->c = c;
		line->f = f;
		line->position = *position;
		line->r1c = radius;
	}

	if (height > 0.0f)
	{
		real z = position->z - height;

		if (list->count_a < 0x100)
		{
			s_line_a *line = &list->lines_a[list->count_a++];

			line->a = a;
			line->b = b;
			line->c = c;
			line->d = d;
			line->e = e;
			line->f = f;
			function_244d51(&line->position, position->x, position->y, z);
			line->r1c = radius;
		}

		if (list->count_b < 0x100)
		{
			s_line_b *line = &list->lines_b[list->count_b++];

			line->a = a;
			line->b = b;
			line->c = c;
			line->d = d;
			line->e = e;
			line->f = f;
			function_244d51(&line->position, position->x, position->y, z);
			line->r1c = 0.0f;
			line->r20 = 0.0f;
			line->r24 = height;
			line->r28 = radius;
		}
	}
}

/* ---- the engine's handlers (lane O) ---- */

/* the team table of the multiplayer globals */
struct s_ctf_team_view
{
	byte unknown00[0x10];
	short slot_teams[9];
	byte unknown22[0xc18 - 0x22];
	long team_count;
	s_team_entry teams[1];
};

/* the object placement data of 0xb7930/0xb7b40 */
struct s_object_placement
{
	byte unknown00[0x1c];
	point3f position;
	byte unknown28[0xc4 - 0x28];
};

/* the iterator over the players of 0x19f300 */
struct s_player_iterator
{
	s_player_view *player;
	s_record_pool *data;
	long index;
	long absolute_index;
};

bool player_iterator_next_with_unit(long *iterator);

static inline real distance_sq3f(point3f const *a, point3f const *b)
{
	vector3f v;

	vector3d_from_points3d(a, b, &v);
	return length_sq3f(&v);
}

/* copies of function_15eaf0 and function_161e10, which retail inlines in
   function_242ef0 */
static inline bool ctf_game_has_teams()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = g_4e6948->flags184.bit0;

	return result;
}

static inline bool ctf_team_is_active(long team)
{
	bool result = false;

	if (ctf_game_has_teams() && team >= 0 && team < 8)
		result = (g_4e9ae8->wc & (1 << team)) != 0;

	return result;
}

static inline point3f *marker_position(long marker_index)
{
	s_marker_entry *entry = &g_4e0350->marker_entries[marker_index];
	return &entry->position;
}

long function_2421d0(long team);
long function_242210(point3f const *point);
void function_2422d0(long slot);
bool function_2423f0();
long function_242510(long object_index);
struct s_242ef1 { point3f const *field_0; };
long function_242ef0(s_242ef1 arg_0, long team);
void function_2430a0(long player_index, bool flag);
void function_2431a0();
void function_243250(long player_index);
void function_2432e0(long team);
void function_243a20(long object_index);
bool function_243a80(long slot);
bool function_244300(long object_index, long *player_index);
bool function_244680(long team, long unit_index);
bool function_2447f0();

// @retail 0x240140
long c_game_engine_markers::v0()
{
	return ctf_options()->engine_type == 9 ? 9 : 1;
}

// @retail 0x240160
bool c_game_engine_markers::v1()
{
	s_slot_table *table = (s_slot_table *)&g_4e9ae8->stats;
	long i;

	memset(table, 0, sizeof(s_slot_table));
	g_51ec80 = table;

	for (i = 0; i < 9; i++)
		table->c[i] = NONE;

	for (i = 0; i < 9; i++)
	{
		long second = NONE;

		table->a[i] = (short)function_243b10(i, true, NULL);
		table->b[i] = (short)function_243b10(i, false, &second);
		table->c[i] = (short)second;
		function_243c00(i);
	}

	table->l38 = function_243b10(NONE, true, NULL);
	table->initialized = true;

	for (i = 0; i < 9; i++)
		table->d[i] = NONE;

	for (i = 0; i < 9; i++)
		table->objects[i] = NONE;

	for (i = 0; i < 3; i++)
	{
		table->triples[i].a = NONE;
		table->triples[i].b = NONE;
		table->triples[i].c = NONE;
	}

	return true;
}

// @retail 0x240290
void c_game_engine_markers::v12()
{
	if (ctf_options()->mode != 4)
	{
		if (ctf_options()->team_mode == 1)
		{
			g_51ec80->l1f4 = function_161eb0(NONE);
			ctf_globals_changed(0x20);
		}

		for (long i = 0; i < 9; i++)
		{
			if (function_243a80(i))
				function_2422d0(i);
		}
	}
}

#pragma inline_depth(1)
// @retail 0x240300
void c_game_engine_markers::v6(long player_index)
{
	s_player_view *player = ctf_player_get(player_index);

	if (ctf_options()->mode != 4 && *(volatile char const *)&player->team != NONE)
	{
		s_event event;

		game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 0);
		event.a = player_index;
		game_engine_event_send_inline(&event);

		if (ctf_options()->team_mode == 1)
			function_243250(player_index);
	}
}
#pragma inline_depth(255)


// @retail 0x240e30
real c_game_engine_markers::v19(long player_index)
{
	real result = 1.0f;

	if (player_find_unit_item_of_type(player_index, 1) != NONE)
	{
		switch (ctf_options()->carrier_speed)
		{
		case 1:
			result = 1.0f;
			break;
		case 2:
			result = 1.25f;
			break;
		default:
			result = 0.75f;
			break;
		}
	}

	return result;
}

// @retail 0x240e80
long c_game_engine_markers::v20(long object_index)
{
	long result = NONE;
	s_slot_object *object = slot_object_get(object_index);

	if ((1 << object->object_type) & 4)
	{
		short slot = object->slot;

		if (slot >= 0 && slot < 9)
			result = g_51ec80->objects[slot];
	}

	return result;
}

// @retail 0x240ee0
void c_game_engine_markers::v30(long killer, long victim, bool suicide, long)
{
	if (killer != NONE && victim != NONE && !suicide && player_find_unit_item_of_type(victim, 1) != NONE)
	{
		long absolute_index = killer & 0xffff;
		s_player_view const *local_0 = ctf_player_get(killer);

		if (ctf_options()->engine_type == 9)
		{
			long team = local_0->team;
			function_1967d0(absolute_index, team, 0x14, 1);
			function_1970a0(absolute_index, 0x16, 1);
		}
		else
		{
			long team = local_0->team;
			function_1967d0(absolute_index, team, 0x10, 1);
			function_1970a0(absolute_index, 0x13, 1);
		}
	}
}

// @retail 0x240f90
long c_game_engine_markers::v31(long killer, long victim, bool suicide)
{
	long result = NONE;

	if (killer != result && victim != result && !suicide && player_find_unit_item_of_type(victim, 1) != result)
		result = ctf_options()->engine_type == 9 ? 0x28 : 0x27;

	return result;
}

// @retail 0x240fe0
void c_game_engine_markers::v21(long object_index)
{
	function_243a20(object_index);
}

// @retail 0x240ff0
void c_game_engine_markers::v23(long object_index, long unit_index)
{
	if (ctf_options()->mode != 4)
	{
		if (function_101280(object_index) == 1)
		{
			s_record_pool *objects = g_4e0300;
			s_slot_object *unit = ((s_slot_object_header *)objects->data)[unit_index & 0xffff].object;
			long slot = ((s_slot_object_header *)objects->data)[object_index & 0xffff].object->slot;

			if (slot != NONE)
			{
				long player_index = unit->player_index;

				if (player_index != NONE)
				{
					long absolute_index = player_index & 0xffff;
					s_player_view *player = ctf_player_get(player_index);

					if (!g_51ec80->flag_bits[slot].bit0)
					{
						if (ctf_options()->engine_type != 9)
						{
							function_1967d0(absolute_index, player->team, 0xf, 1);
							function_1970a0(absolute_index, 0x12, 1);
						}
						else
						{
							function_1967d0(absolute_index, player->team, 0x15, 1);
						}
					}
				}

				byte_flags_set(&g_51ec80->flags[slot], 0, true);
				byte_flags_set(&g_51ec80->flags[slot], 1, false);
				function_a7810(0x200);

				if (slot >= 0 && slot < 9)
				{
					g_51ec80->d[slot] = NONE;
					g_51ec80->times[slot] = 0;
					function_a7810(0x80);
				}

				player_index = unit->player_index;
				if (player_index != NONE && slot >= 0 && slot < 9)
				{
					s_event event;

					game_engine_event_set_cause_player(player_index,
						game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 1, &event));
					event.effect_team = (short)((short)slot == 8 ? NONE : (short)slot);
					function_19eb90(&event);
				}
			}
		}
	}
}

PRIVATE __forceinline short function_2411b1(short arg_0)
{
	short local_0 = NONE;
	if (arg_0 != 8)
		local_0 = arg_0;
	return local_0;
}

// @retail 0x2411b0
void c_game_engine_markers::v24(long object_index, long unit_index)
{
	if (ctf_options()->mode != 4)
	{
		s_slot_object *object = slot_object_get(object_index);

		if (function_101280(object_index) == 1)
		{
			long slot = object->slot;

			if (slot >= 0 && slot < 9)
			{
				s_slot_object *unit = slot_object_get(unit_index);
				s_slot_table *g = g_51ec80;
				long player_index;

				if (g->flags[slot] & 1)
				{
					g->d[slot] = ctf_options()->s230;
					g->times[slot] = g_510c54->field_2_3;
				}
				else
				{
					g->d[slot] = NONE;
					g->times[slot] = 0;
				}
				function_a7810(0x80);

				if (ctf_options()->engine_type == 9)
				{
					if (!(g->flags[slot] & 1))
						g->e[slot] = NONE;
					else if (!(ctf_options()->flags22c.flags & 0x20))
						g->e[slot] = NONE;
					function_a7810(0x100);
				}

				player_index = unit->player_index;
				if (player_index != NONE && !(g->w204 & 1))
				{
					s_event event;

					game_engine_event_set_cause_player(player_index,
						game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 2, &event));
					event.effect_team = (short)((short)slot == 8 ? NONE : (short)slot);
					function_19eb90(&event);
				}
			}
		}
	}
}

// @retail 0x241320
long c_game_engine_markers::v25(long ticks, bool a, bool b)
{
	if ((real)ticks * g_510c54->rate <= 5.0f)
	{
		s_slot_table *g;

		switch (!a)
		{
		case false:
		{
			g = g_51ec80;
			if (b && (ctf_options()->flags22c.flags & 2))
			{
				if (g->b1fc)
					g->l200 = ctf_real_to_long((real)g_510c54->field_2_3 * 5.0f);
				else
					g->l200--;
			}
			else
			{
				g->l200 = 0;
			}
			break;
		}
		default:
			g = g_51ec80;
			break;
		}
		if (ticks <= g->l200)
			ticks = g->l200;
	}

	return ticks;
}

// @retail 0x2413c0
long c_game_engine_markers::v26()
{
	if (g_4e0350->flags & 8)
		return function_161eb0(NONE);

	return g_51ec80->l1f4;
}

// @retail 0x2413f0
bool c_game_engine_markers::v33(long player_index, s_ctf_query const *query)
{
	bool result = true;
	dword flags = query->flags;

	if ((flags & 1) || (flags & 2) && ctf_options()->engine_type != 9)
	{
		long team = ctf_options()->team_mode == 1 ? g_51ec80->l1f4 : ctf_player_get(player_index)->team;

		if (function_15e410((short)team))
		{
			bool away = !g_51ec80->flag_bits[team].bit0;

			if ((flags & 1) && away || (flags & 2) && !away)
				result = false;
		}
	}
	else if ((flags & 4) || (flags & 8) && ctf_options()->engine_type == 9)
	{
		long team = ctf_options()->team_mode == 1 ? g_51ec80->l1f4 : ctf_player_get(player_index)->team;

		if (function_15e410((short)team))
		{
			bool away = !g_51ec80->flag_bits[team].bit0;

			if ((flags & 4) && away || (flags & 8) && !away)
				result = false;
		}
	}

	return result;
}

/* a copy of function_15b2f0, which retail inlines in v16 */
struct s_ctf_engine_state
{
	byte unknown00[0x6c];
	short w6c;
	byte unknown6e[0xc04 - 0x6e];
	long lc04;
	byte unknownc08[0xc14 - 0xc08];
	long engine_index;
};

static inline bool ctf_engine_running()
{
	s_ctf_engine_state *g = (s_ctf_engine_state *)g_4e9ae8;
	bool result = false;

	if (g_55e4d0[g->engine_index] && g->w6c == 1 && (g_4e6948->mode == 4 || g->lc04 == 1))
		result = true;

	return result;
}

// @retail 0x241520
bool c_game_engine_markers::v16(long player_index, long object_index)
{
	bool result = true;

	if (!ctf_engine_running())
		result = false;
	else if (player_index != NONE && object_index != NONE)
	{
		s_player_view *player = ctf_player_get(player_index);
		long slot = slot_object_get(object_index)->slot;

		if (function_101280(object_index) == 1 && slot != NONE)
		{
			if (ctf_options()->engine_type == 9 && (g_51ec80->flags[slot] & 2))
				result = false;
			else if (slot != 8 && (ctf_options()->engine_type != 9 && slot == player->team ||
					ctf_options()->engine_type == 9 && slot != player->team))
				result = false;
		}
	}

	return result;
}

// @retail 0x241fc0
void c_game_engine_markers::v36(long value)
{
	if (value == 1)
	{
		long team = g_51ec80->l1f4;
		long next = function_161eb0(team);

		if (team != NONE && next == NONE)
			next = team;
		function_2432e0(next);
	}
}

// @retail 0x242000
bool c_game_engine_markers::v34(long value)
{
	bool result = false;

	if (!value)
		result = true;

	return result;
}

// @retail 0x242010
bool c_game_engine_markers::v35(long player_index, long type)
{
	bool result = false;

	if (player_find_unit_item_of_type(player_index, 1) != NONE)
	{
		if (type == 2)
			result = ctf_options()->l238 == 0;
		else if (type == 3)
			result = ctf_options()->flags22c.bits.bit6;
		else if (type == 1)
			result = ctf_options()->flags22c.bits.bit7;
	}
	else
	{
		result = ((c_game_engine *)this)->c_game_engine::v5(player_index, type);
	}

	return result;
}

// @retail 0x2421d0
long function_2421d0(long team)
{
	long result = NONE;

	if (team == 8)
	{
		result = g_51ec80->l38;
	}
	else if (team >= 0 && team < 8)
	{
		long slot = function_158990(team);

		if (slot != NONE)
			result = g_51ec80->a[slot];
	}

	return result;
}

// @retail 0x242210
long function_242210(point3f const *point)
{
	s_object_placement data;
	long object_index;
	long definition_index;

	if (ctf_options()->engine_type == 9)
		definition_index = engine_object_definition(_engine_object_bomb);
	else
		definition_index = engine_object_definition(_engine_object_flag);

	function_b7930(&data, definition_index, NONE, 0);
	data.position = *point;
	object_index = function_b7b40(&data);
	if (object_index != NONE)
		function_a7870(object_index);

	return object_index;
}

// @retail 0x2422d0
void function_2422d0(long slot)
{
	s_slot_table *g = g_51ec80;
	long marker_index;

	g->e[slot] = NONE;
	ctf_globals_changed(0x100);
	g->d[slot] = NONE;
	g->times[slot] = 0;
	ctf_globals_changed(0x80);
	g->flags[slot] = 0;
	ctf_globals_changed(0x200);

	marker_index = function_2421d0(slot);
	if (marker_index != NONE)
	{
		point3f point;
		point3f const *local_0 = marker_position(marker_index);
		long local_1 = ((long const *)local_0)[1];
		((long *)&point)[0] = ((long const *)local_0)[0];
		((long *)&point)[1] = local_1;
		((long *)&point)[2] = ((long const *)local_0)[2];
		long object_index = function_242210(&point);

		if (object_index != NONE)
			function_15e050(object_index, slot);
	}
}

// score 7 (was 7): the object index is kept in EDI and the object in EBP, retail keeps them the other
// way round. Native index/eligibility/object reference 7; team-index/team-object/
// other-count names 7; second-loop element address/nested accept 7; native slot table/
// const object pointer/table-entry address 7; slot-field address 60; the object getter at each
// use 86 (reloads after the call); index or object local renamed to sort first or last 7 each;
// the reject arm first 7; the rejected count advanced in its own statement 7.
// Round 53: function-scope object index reused for the second loop's element 7; permuter (58
// variants) 7; lookup written without the inline getter 7 (diagnostic); a `long i` per loop 7;
// `dword` object index 7; the eligibility result in a named bool 79.
// Lane 15 round 42 (fast): object and slot declared before the index 7; no slot local 79 (index EDI and object EBP
// stay); `short slot` 7; nested tests through a table-entry pointer with `continue` 7; one && test with `continue` 7;
// a named team entry pointer 7; the table value named in the test 7; the flag cleared after the reject store 9
// (with a named eligibility bool 9); `++i` 7.
// This round: team-entry field reference 104 (267 bytes); register index/object/both 7 each;
// accepted field reread 54; rejected field reread 50; both field rereads 104; nested two-use
// table-entry pointer 7; force-inline object getter 7. Original body restored.
// Round 31 (lane 18, fast): the object pointer defined from the team entry before the index local 7; the
// accepted store through the slot local 31; the call on `object->slot` with the table test on the slot
// local 31; the second loop's element in a named local 7; the rejected count copied to a loop bound 7.
// Original body restored.
// Round 61: diagnostics (reverted): one more real read of the object flips both registers to retail's (80, 300 bytes),
// one read fewer of the index does too (50/54), so retail ranks the object at or above the index with the same final
// reads. `__assume` on the object 7, on the index 7, on the slot 7, on a team-entry pointer 7, on a table-entry pointer in
// the accepted arm 7, on one before the test 79 (268 bytes); on a header pointer 64. A dead object read 7; the slot read
// twice 7; a late-folding ternary or switch around the slot read 7/7/7; separate reads of the team entry for the index and
// the getter, either order, 7/7, through an entry pointer 7; inline list-append, table-set or claim helpers 7/7/7; the
// object declared at function scope 7; an inline slot accessor at every read 79, with the slot local 7.
// A copy of the index for both stores 7, across a pointer `__assume` 7; a second read of the team entry for the stores
// across one 7; a pointer `__assume` between the getter and the slot read 7.
// Round 75 (lane 1, fast): a compiled-out `do { } while (false);` at the loop top, after the index, object or slot, at
// either end of each arm, at the loop end, or before or after the first loop 7 each (11 places); the index through an
// inline getter 7; the index declared at function top 7; object then index declared bare at the loop top 7; the getter
// taking the index by const reference 7, or filling an out parameter 7; a NULL object seed before the getter, in the
// loop or at function top, 7/7; an accepted flag then a reject test 7; a named element pointer with `continue` in the
// accepted arm 7.
// Round 172 (m3 lane 4, fast): an accepted flag with the reject arm under `if (!accepted)` 7; the function-scope
// declarations as result, arrays, count, i 7; count before result 9; i before result and count 7; the scalars
// before the arrays 7; the table filled by `memset(..., 0xff, ...)` 7. Diagnostics: either second-loop call removed
// 63/67 (the whole allocation moves).
// Round 156 (m3 lane 6, fast): a per-iteration pool local (`objects = g_4e0300`) with the direct lookup 7; a named
// masked index for the lookup 7; 30 random combinations of declaration order, function-scope index, getter/direct/pool
// lookup, short slot, the three reject-arm orders and the inverted test 7 (12), 9 (14), 11 (4); the table initialised
// by an initializer list 94; separate loop variables for the first and third loops and a named element in the second,
// all seven combinations 7. 0xb8540 is still a stub here (retail calls the real function).
// Round 80 (m3 lane 1, fast with 0x2447f0): a const object target 7; a function-scope team base pointer for the
// index reads 82; the table test written `NONE == ...` 7; a named second-loop element with the rejected store
// before the result store 9; a function-scope slot local 7.
// Round 100 (m3 lane 9, fast): the stubbed 0xb8540 replaced by its disabled draft (diagnostic) 7, so the stub is
// not the cause; one store of the index through a target pointer chosen per arm 100 (300 bytes); a `long const`
// index 7; the second slot read named and used for the test and the store, with `continue` 7; index and object
// at function scope with the index reused for the second loop's element 7.
// Round 156 (m3 lane 8, fast): a function-scope team walker advanced in the for increment 82, advanced at the body's
// end 81; the 0x243a80 rejection and the taken-slot rejection as separate arms 79; the accept nested under the
// 0x243a80 test with `continue` and the reject code after it 7; a team entry pointer at the loop top 7 (same code);
// the two arrays declared the other way round 7; the third loop's test as a `continue` 7; the first loop as `while`
// with the increment at the body's end 7 (all the same code).
// Round 86 (m3 lane 14, fast): loop 2 over a stepped `long *` element pointer 70; loop 3's test with the table read
// before the 0x243a80 call 12; the team count through an inline reader 7 and the team entry through an inline
// accessor 7 (both the same code); loop 2 as `while` with the increment at the body's end 7 (same code); no slot
// local re-measured 79 (a repeat of round 42's form). Diagnostics: without loop 3 the swap stays (71); without loop 2
// the whole allocation moves (108).
// Round 174 (m3 lane 6, fast; the one-file harness does not reproduce this function): the getter given a second
// read of the team entry through a `dword const` view, the index local before or after it 7/7 (merged, same
// code); the object looked up by the entry's low word through a `word const` view 47 (`movzx eax, bx`: the object
// takes EDI as in retail, but the index takes EBX and the team walker EBP); a header pointer local between the
// index and the object 7 (same code); the absolute index as a named `word` local 81; the entry held as an
// index/salt pair with the pair's index for the lookup 81.
// Round 195 (m3 lane 4, fast): no index local, the team entry read for the lookup and both stores 104 (267 bytes, the
// stores reload the entry after the call); a const object pointer and a const slot 7; the index assigned inside the
// getter's argument 7; loop 3's result store ahead of the 0x2422d0 call 9; a short slot with `NONE == ...` 7.
// @retail 0x2423f0
bool function_2423f0()
{
	long others[8];
	long slot_objects[9];
	bool result = true;
	long count = 0;
	long i;

	memset(slot_objects, NONE, sizeof(slot_objects));

	for (i = 0; i < ((s_ctf_team_view *)g_4e9ae8)->team_count; i++)
	{
		long object_index = ((s_ctf_team_view *)g_4e9ae8)->teams[i].object_index;
		s_slot_object *object = slot_object_get(object_index);
		long slot = object->slot;

		/* the table test reads the slot again after the call */
		if (function_243a80(slot) && slot_objects[object->slot] == NONE)
		{
			slot_objects[object->slot] = object_index;
		}
		else
		{
			result = false;
			others[count++] = object_index;
		}
	}

	for (i = 0; i < count; i++)
	{
		function_15e130(others[i]);
		function_b8540(others[i]);
	}

	for (i = 0; i < 9; i++)
	{
		if (function_243a80(i) && slot_objects[i] == NONE)
		{
			function_2422d0(i);
			result = false;
		}
	}

	return result;
}

// @retail 0x242510
long function_242510(long object_index)
{
	long result = NONE;
	long slot = slot_object_get(object_index)->slot;

	if (slot != NONE)
	{
		s_slot_table *g = g_51ec80;

		if (g->flags[slot] & 2)
		{
			point3f position;

			function_b9dd0(object_index, &position);
			for (long team = 0; team < 8; team++)
			{
				if (function_161e10(team))
				{
					long team_slot = function_158990(team);

					for (long k = 0; k < 2; k++)
					{
						if (team_slot != NONE)
						{
							long marker_index = k ? g->c[team_slot] : g->b[team_slot];

							if (marker_index != NONE)
							{
								point3f point;
								point3f const *local_0 = marker_position(marker_index);
								*(long volatile *)&point.x = ((long const *)local_0)[0];
								long local_1 = ((long const *)local_0)[2];
								((long *)&point)[1] = ((long const *)local_0)[1];
								((long *)&point)[2] = local_1;

								if (distance_sq3f(&position, &point) < 0.09f)
								{
									result = team;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return result;
}

extern vector3f *g_4687bc;

struct s_location
{
	long leaf_index;
	short cluster_index;
	short bsp_index;
};

struct s_damage_owner
{
	long field_0;
	long field_4;
	short field_8;
};

struct s_effect_owner;

struct s_effect_marker
{
	point3f position;
	vector3f forward;
	dword name;
};

struct s_type_1e6529
{
	long definition_index;
	dword flags;
	s_damage_owner owner;
	long field_14;
	long field_18;
	s_location field_1c;
	point3f position;
	point3f origin;
	vector3f direction;
	byte field_48[0x54 - 0x48];
	real field_54;
	byte field_58[0x7c - 0x58];
	short field_7c;
	byte field_7e[0x84 - 0x7e];
	byte field_84;
	byte field_85[3];
};

struct s_242661
{
	byte field_0[0xe0];
	long field_e0;
	long field_e4;
	long field_e8;
	long field_ec;
	long field_f0;
};

void function_176b60(s_effect_owner const *owner, long marker_count, s_effect_marker *markers, long mode, long tag_index);
void function_a8040(long index, long value, point3f const *position);
void function_d6660(s_type_1e6529 *data, long definition_index);
void function_bc190(long object_index, s_damage_owner *owner);
s_location *object_get_root_location(long object_index, s_location *location);
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index);
void function_15e360(point3f const *point, long object_index);
byte *record_pool_lookup(s_record_pool *data, long datum_index);
void function_242e00(long player_index, long team);

// @retail 0x242660
void function_242660(long object_index, bool arg_1)
{
	s_slot_object *object = slot_object_get(object_index);

	if (ctf_options()->engine_type == 9 && !ctf_options()->b1128)
	{
		long slot = object->slot;
		s_242661 const *constants = (s_242661 const *)mp_globals_block()->constants;
		long damage_index = NONE;

		if (slot != NONE)
		{
			long effect_index;
			long value;

			if (!arg_1 && (g_51ec80->flags[slot] & 2))
			{
				long team = function_242510(object_index);
				long carrier = g_51ec80->carriers[slot];

				if (team != NONE && carrier != NONE && record_pool_lookup(g_4e8c24, carrier))
				{
					function_242e00(carrier, team);
					g_51ec80->carriers[object->slot] = NONE;
					function_a7810(0x400);
				}
				effect_index = constants->field_e0;
				damage_index = constants->field_e8;
				value = 0;
			}
			else
			{
				effect_index = constants->field_f0;
				value = 1;
			}

			if (effect_index != NONE)
			{
				vector3f local_0 = *g_4687b0;
				point3f position;
				s_effect_marker markers[5];
				s_damage_owner owner;

				function_b9dd0(object_index, &position);
				function_bc190(object_index, &owner);
				markers[4].position = position;
				markers[3].position = position;
				markers[2].position = position;
				markers[1].position = position;
				markers[0].position = position;
				markers[0].forward = *g_4687b0;
				markers[1].forward = *g_4687bc;
				markers[2].forward = *g_4687b0;
				markers[3].forward.i = 0.0f - local_0.i;
				markers[3].forward.j = 0.0f - local_0.j;
				markers[3].forward.k = 0.0f - local_0.k;
				markers[4].forward = local_0;
				markers[0].name = 0x700054c;
				markers[1].name = 0x70000c0;
				markers[2].name = 0x20000ca;
				markers[3].name = 0x8000550;
				markers[4].name = 0x400054f;
				function_176b60((s_effect_owner const *)&owner, 5, markers, 1, effect_index);
				function_a8040(object_index, value, &position);
			}

			if (damage_index != NONE)
			{
				s_type_1e6529 data;
				point3f position;

				data.field_7c = NONE;
				function_b9dd0(object_index, &position);
				function_d6660(&data, damage_index);
				function_bc190(object_index, &data.owner);
				data.field_54 = 1.0f;
				object_get_root_location(object_index, &data.field_1c);
				data.origin = position;
				data.position = position;
				data.direction = *g_4687b0;
				data.field_84 = 0x19;
				function_d6c80(&data, NONE);
			}
		}
	}

	if (object->slot != NONE)
	{
		/* retail calls the marker lookup here */
#pragma inline_depth(0)
		long marker_index = function_2421d0(object->slot);
#pragma inline_depth()

		g_51ec80->d[object->slot] = NONE;
		g_51ec80->times[object->slot] = g_510c54->field_2_3;
		ctf_globals_changed(0x80);
		if (ctf_options()->engine_type == 9)
		{
			g_51ec80->e[object->slot] = NONE;
			ctf_globals_changed(0x100);
		}
		if (marker_index != NONE)
		{
			point3f point;

			g_51ec80->flags[object->slot] &= ~1;
			g_51ec80->flags[object->slot] &= ~2;
			g_51ec80->flags[object->slot] &= ~8;
			ctf_globals_changed(0x200);
			g_51ec80->w204 |= 1;
			point = *marker_position(marker_index);
			function_15e360(&point, object_index);
			g_51ec80->w204 &= ~1;
		}
	}
}

PRIVATE __forceinline long function_242ef4(s_slot_table *g, s_242ef1 const &arg_0, long team)
{
	long result = NONE;

	for (long i = 0; i < 8; i++)
	{
		if (ctf_team_is_active(i))
		{
			long slot = function_158990(i);

			if (slot != NONE)
			{
				for (long k = 0; k < 2; k++)
				{
					long marker_index = k == 0 ? g->b[slot] : g->c[slot];
					bool valid;

					if (ctf_options()->team_mode == 1 && ctf_options()->engine_type == 9)
						valid = i != team && i == g->l1f4;
					else
						valid = ctf_options()->engine_type == 9 ? i != team : i == team;

					if (valid && marker_index != NONE)
					{
						point3f marker;
						point3f const *local_0 = marker_position(marker_index);
						((long *)&marker)[0] = ((long const *)local_0)[0];
						long local_1 = ((long const *)local_0)[2];
						((long *)&marker)[1] = ((long const *)local_0)[1];
						((long *)&marker)[2] = local_1;

						g = g_51ec80;
						point3f const *point = *(point3f const *const volatile *)&arg_0.field_0;
						if (distance_sq3f(point, &marker) < 1.0f)
						{
							result = marker_index;
							break;
						}
					}
				}
			}
		}
	}

	return result;
}

// @retail 0x242ef0
long function_242ef0(s_242ef1 arg_0, long team)
{
	return function_242ef4(g_51ec80, arg_0, team);
}

// @retail 0x2430a0
void function_2430a0(long player_index, bool flag)
{
	long absolute_index = player_index & 0xffff;

	if (ctf_options()->mode != 4 && absolute_index >= 0 && absolute_index < 16)
	{
		if (function_146650() > g_51ec80->player_times[absolute_index])
		{
			s_event event;
			long subtype = flag ? 13 : 7;

			__assume(ctf_options() != NULL);
			game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, subtype);
			event.cause_player_index = player_index;
			event.cause_team = event_player_get(player_index)->team;
			function_19eb90(&event);
			g_51ec80->player_times[absolute_index] = function_146650() + ctf_real_to_long((real)g_510c54->field_2_3 * 4.0f);
		}
	}
}

// @retail 0x2431a0
void function_2431a0()
{
	s_ctf_options *options = ctf_options();

	g_51ec80->b1fc = false;
	if (options->flags22c.flags & 2)
	{
		for (long i = 0; i < 9; i++)
		{
			s_team_entry *entry = function_15e410((short)i);

			if (entry)
			{
				bool carried = entry->lc != NONE;
				s_slot_flags flags = g_51ec80->flag_bits[i];
				bool dropped = flags.bit2;
				bool away = !flags.bit0;
				bool armed = options->engine_type == 9 && flags.bit1;

				if (carried || dropped || armed || away && function_244300(entry->object_index, NULL))
				{
					g_51ec80->b1fc = true;
					return;
				}
			}
		}
	}
}

#pragma inline_depth(1)
// @retail 0x243250
void function_243250(long player_index)
{
	if (ctf_options()->mode != 4 && ctf_options()->team_mode == 1)
	{
		s_event event;

		game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 6);
		event.cause_team = g_51ec80->l1f4;
		event.a = player_index;
		game_engine_event_send_inline(&event);
	}
}
#pragma inline_depth(255)


// @retail 0x2432e0
void function_2432e0(long team)
{
	if (ctf_options()->mode != 4)
		function_15e4d0();

	g_51ec80->l1f4 = team;
	ctf_globals_changed(0x20);

	if (ctf_options()->mode != 4)
	{
		function_157670();
		function_243250(NONE);
		for (long i = 0; i < 9; i++)
		{
			if (function_243a80(i))
				function_2422d0(i);
		}
	}

	g_51ec80->l200 = 0;
}

// @retail 0x243380
bool c_game_engine_markers::v40()
{
	return function_2447f0();
}

// @retail 0x243390
void c_game_engine_markers::v44(long, s_marker_update *update)
{
	memset(update, 0, sizeof(s_marker_update));
	v41(update);
	update->l24 = function_161eb0(NONE);
	update->l28 = 0;
	memset(update->a, 0xff, sizeof(update->a));
	memset(update->triples, 0xff, sizeof(update->triples));
	update->b80 = 0;
}

// @retail 0x243a20
void function_243a20(long object_index)
{
	s_slot_object *object = slot_object_get(object_index);

	if (object->slot != NONE)
	{
		long team = object->slot;
		color3f color = function_7f720((short)team);

		function_bacc0(object_index, 0, (point3f const *)&color);
	}
}

// @retail 0x243a80
bool function_243a80(long slot)
{
	bool result = false;

	if (slot >= 0 && slot < 8)
	{
		long team_mode = ctf_options()->team_mode;

		if (team_mode != 2 && function_161e10(slot))
		{
			if (team_mode == 1)
			{
				if (ctf_options()->engine_type == 9)
					result = slot != g_51ec80->l1f4;
				else
					result = slot == g_51ec80->l1f4;
			}
			else
			{
				result = true;
			}
		}
	}
	else if (slot == 8)
	{
		result = ctf_options()->team_mode == 2;
	}

	return result;
}

PRIVATE __forceinline short function_244301(short arg_0)
{
	return arg_0 == 8 ? NONE : arg_0;
}

// @retail 0x244300
bool function_244300(long object_index, long *player_index)
{
	bool result = false;
	s_ctf_constants *constants = mp_globals_block()->constants;
	real radius_squared = constants->pickup_radius * constants->pickup_radius;
	long slot = slot_object_get(object_index)->slot;
	long team = function_244301((short)slot);
	point3f position;

	function_b9dd0(object_index, &position);
	if (slot != NONE)
	{
		s_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.absolute_index = NONE;
		iterator.index = NONE;
		while (player_iterator_next_with_unit((long *)&iterator))
		{
			s_player_view *player = iterator.player;
			long player_team = player->team;
			long unit_index = player->unit_index;

			if ((ctf_options()->engine_type != 9 && player_team != team) || (ctf_options()->engine_type == 9 && player_team == team) || ctf_options()->team_mode == 2)
			{
				point3f unit_position;
				vector3f delta;

				function_b9dd0(unit_index, &unit_position);
				vector3d_from_points3d(&unit_position, &position, &delta);
				if (length_sq3f(&delta) < radius_squared)
				{
					result = true;
					if (player_index)
						*player_index = iterator.index;
					break;
				}
			}
		}
	}

	return result;
}

PRIVATE __forceinline real function_244681(long arg_0, s_palette_source_globals const *arg_1, point3f *arg_2, real arg_3)
{
	point3f const *local_0 = &arg_1->marker_entries[arg_0].position;
	long local_1 = ((long const volatile *)local_0)[0];
	((long *)arg_2)[0] = local_1;
	real local_2 = arg_3 - arg_2->x;
	((long *)arg_2)[1] = ((long const volatile *)local_0)[1];
	((long *)arg_2)[2] = ((long const volatile *)local_0)[2];
	return local_2;
}

// @retail 0x244680
bool function_244680(long team, long unit_index)
{
	short own_slot = function_158990(team);
	bool result = false;
	point3f position;
	s_slot_table *g;

	function_b9dd0(unit_index, &position);
	g = g_51ec80;
	for (long i = 0; i < 9; i++)
	{
		short slot_team = ((s_ctf_team_view *)g_4e9ae8)->slot_teams[i];

		if (i != own_slot && slot_team != NONE && function_161e10(slot_team))
		{
			for (long k = 0; k < 2; k++)
			{
				long marker_index = k == 0 ? g->b[i] : g->c[i];

				if (marker_index != NONE)
				{
					point3f const *bounds = k == 0 ? &g->bounds[0][i] : &g->bounds[1][i];
					point3f marker;
					real dx = function_244681(marker_index, g_4e0350, &marker, position.x);
					real dy = position.y - marker.y;

					if (dx * dx + dy * dy < bounds->x * bounds->x &&
						bounds->z > position.z - marker.z &&
						bounds->y > marker.z - position.z)
					{
						result = true;
						break;
					}
				}
			}
		}
	}

	return result;
}

PRIVATE __forceinline void function_2447f2(long arg_0, s_palette_source_globals const *arg_1, point3f *arg_2)
{
	point3f const *local_0 = &arg_1->marker_entries[arg_0].position;
	long const *local_1 = (long const *)local_0;
	long local_2 = local_1[2];
	long local_3 = local_1[1];
	long local_4 = local_1[0];
	((long *)arg_2)[0] = local_4;
	((long *)arg_2)[1] = local_3;
	((long *)arg_2)[2] = local_2;
}

// @retail 0x2447f0
bool function_2447f0()
{
	bool result = function_2423f0();

	for (long i = 0; i < 9; i++)
	{
		s_team_entry *entry = function_15e410((short)i);

		if (entry && entry->object_index != NONE && !(g_51ec80->flags[i] & 1))
		{
			if (entry->l8 == NONE)
			{
				long marker_index = function_2421d0(i);
				point3f position;
				point3f marker;

				if (marker_index == NONE)
					continue;
				function_b9dd0(entry->object_index, &position);
				function_2447f2(marker_index, g_4e0350, &marker);
				real delta_x = marker.x - position.x;
				real delta_y = marker.y - position.y;
				real delta_z = marker.z - position.z;
				real distance_squared = delta_z * delta_z + delta_y * delta_y + delta_x * delta_x;
				if (!(distance_squared > 0.2f * 0.2f))
					continue;
			}

			g_51ec80->flags[i] |= 1;
			ctf_globals_changed(0x200);
			result = false;
		}
	}

	return result;
}

/* the scores of the players (0x1c bytes each at +0x304 of the multiplayer
   globals) */
struct s_ctf_player_score
{
	byte unknown00[8];
	short score;
	byte unknown0a[0x1c - 0x0a];
};

static inline s_ctf_player_score *ctf_player_scores()
{
	s_ctf_player_score *result = NULL;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = (s_ctf_player_score *)((byte *)g_4e9ae8 + 0x304);

	return result;
}

long function_baf80(long object_index);
bool function_15b7c0(long delta, long player_index);
void function_10da60(long object_index, point3f *position);

PRIVATE __forceinline long function_240741(long arg_0, s_mp_globals const *arg_1)
{
	long local_0 = 0;
	if (g_55e4d0[arg_1->engine_index])
	{
		s_ctf_player_score const *local_1 = (s_ctf_player_score const *)((byte const *)arg_1 + 0x304);
		if (local_1)
			local_0 = local_1[arg_0 & 0xffff].score;
	}
	return local_0;
}

// @retail 0x240740
void function_240740(long object_index)
{
	long slot = slot_object_get(object_index)->slot;
	long best_player_index = NONE;
	long best_score = 0x80000000;
	s_player_iterator iterator;

	iterator.data = g_4e8c24;
	iterator.absolute_index = NONE;
	iterator.index = NONE;
	while (player_iterator_next((long *)&iterator) != 0)
	{
		if (slot == 8 || iterator.player->team == slot)
		{
			long score = function_240741(iterator.index, g_4e9ae8);
			if (score > best_score)
			{
				best_player_index = iterator.index;
				best_score = score;
			}
		}
	}

	if (slot >= 0 && slot < 9)
		g_51ec80->objects[slot] = best_player_index;
}

void function_1972a0(long player_index, long value14, long value10);
bool function_162b90(long item_index);
long players_first_active_local_player(void);
long function_14de10(long index);
void function_2007b3(long arg_0, long arg_1, long arg_2, long arg_3);
void function_242660(long object_index, bool arg_1);

struct s_240821
{
	dword field_0;
	dword field_4 : 8;
	dword field_c : 1;
	dword field_d : 23;
};

PRIVATE __forceinline long function_240822(long arg_0)
{
	long local_0 = NONE;
	if (arg_0 == NONE)
		arg_0 = 0;
	else
		arg_0++;
	for (long local_1 = arg_0; local_1 < 4; local_1++)
	{
		if (g_4e8c20->entries[local_1] != NONE)
		{
			local_0 = local_1;
			break;
		}
	}
	return local_0;
}

// @retail 0x240820
void __stdcall function_240820(long arg_0)
{
	s_slot_object *local_0 = slot_object_get(arg_0);
	dword local_1 = *(dword const *)((byte const *)ctf_options() + 0x230);
	long local_2 = local_0->slot;
	s_event local_3;

	if (local_2 == NONE)
		return;
	s_team_entry *local_4 = function_15e410((short)local_2);
	if (local_1 <= 5)
		local_1 = 5;

	if (ctf_options()->mode != 4 && local_2 >= 0 && local_2 < 9 && local_4)
	{
		bool local_8 = local_4->lc != NONE;
		bool local_9 = !(g_51ec80->flags[local_2] & 1);
		bool local_7 = false;
		long local_20 = ctf_options()->engine_type;
		bool local_6 = local_20 == 9 && (g_51ec80->flags[local_2] & 8) != 0;
		long local_10 = NONE;

		if (local_8)
		{
			function_1972a0(local_4->lc, (local_20 == 9) + 1, local_4->team);
		}
		if ((local_8 || local_9) && g_51ec80->d[local_2] != NONE)
		{
			g_51ec80->d[local_2] = NONE;
			function_a7810(0x80);
		}

		if (!(ctf_options()->engine_type == 9 && local_9) && function_244300(arg_0, &local_10))
		{
			if (!(g_51ec80->flags[local_2] & 4) && ((short *)g_51ec80->unknown138)[local_2] > 4 * g_510c54->field_2_3)
			{
				game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, ctf_options()->engine_type == 9 ? 0x10 : 0xc, &local_3);
				local_3.effect_team = function_244301((short)local_2);
				game_engine_event_set_cause_player(local_10, &local_3);
				function_19eb90(&local_3);
			}
			g_51ec80->flags[local_2] |= 4;
			local_7 = true;
			((short *)g_51ec80->unknown138)[local_2] = 0;
		}
		else
		{
			if (!local_9 && !local_8)
				((short *)g_51ec80->unknown138)[local_2]++;
			else
				((short *)g_51ec80->unknown138)[local_2] = 0;
			g_51ec80->flags[local_2] &= ~4;
		}
		function_a7810(0x200);

		short local_11 = g_51ec80->d[local_2];
		if (local_11 != NONE && ((ctf_options()->engine_type == 9 && local_6) || !local_7))
		{
			long local_12 = g_51ec80->times[local_2] - 1;
			s_game_time_globals *local_23 = g_510c54;
			if (local_12 < 0)
				local_12 = 0;
			else if (local_12 > local_23->field_2_3)
				local_12 = local_23->field_2_3;
			g_51ec80->times[local_2] = (short)local_12;
			if ((short)local_12 == 0)
			{
				g_51ec80->d[local_2] = (short)(local_11 - 1 < 0 ? 0 : ((dword)(local_11 - 1) > local_1 ? local_1 : local_11 - 1));
				function_a7810(0x80);

				if (g_51ec80->d[local_2] == 0)
				{
					if (!local_8 && TEST_FIELD_BIT(((s_240821 const *)local_0)->field_c))
					{
						if (!(ctf_options()->engine_type == 9 && (g_51ec80->flags[local_2] & 2)) && (g_51ec80->flags[local_2] & 1))
						{
							game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 4, &local_3);
							local_3.effect_team = function_244301((short)local_2);
							function_19eb90(&local_3);
						}
						function_242660(arg_0, false);
					}
					else
					{
						g_51ec80->times[local_2] = local_23->field_2_3;
					}
				}
				else
				{
					g_51ec80->times[local_2] = local_23->field_2_3;
				}
			}
		}

		if (function_162b90(arg_0))
		{
			game_engine_event_initialize(ctf_options()->engine_type == 9 ? 10 : 3, 4, &local_3);
			local_3.effect_team = function_244301((short)local_2);
			function_19eb90(&local_3);
			function_242660(arg_0, false);
		}
		function_240740(arg_0);
	}

	if (local_2 != NONE)
	{
		if (ctf_options()->engine_type != 9)
		{
			bool local_14 = local_4->lc != NONE;
			bool local_24 = !local_14 && (g_51ec80->flags[local_2] & 1);
			for (long local_15 = players_first_active_local_player(); local_15 != NONE; local_15 = function_240822(local_15))
			{
				long local_16 = local_15 == NONE ? NONE : g_4e8c20->entries[local_15];
				if (local_16 != NONE && ctf_player_get(local_16)->team == local_2)
				{
					if (local_14)
						function_2007b3(local_15, 6, 1, 5);
					if (local_24)
						function_2007b3(local_15, 5, 1, 6);
				}
			}
		}
		else if (g_51ec80->flags[local_2] & 2)
		{
			long local_17 = function_242510(arg_0);
			if (local_17 != NONE)
			{
				for (long local_18 = players_first_active_local_player(); local_18 != NONE; local_18 = function_14de10(local_18))
				{
					long local_19 = local_18 == NONE ? NONE : g_4e8c20->entries[local_18];
					if (local_19 != NONE && ctf_player_get(local_19)->team == local_17)
						function_2007b3(local_18, 8, 1, NONE);
				}
			}
		}
	}
}


// @retail 0x240d70
void c_game_engine_markers::v18()
{
	s_ctf_engine_state *state = (s_ctf_engine_state *)g_4e9ae8;
	if (g_55e4d0[state->engine_index] && state->w6c == 1)
	{
		long const &first_mode = (long)ctf_options()->mode;
		if (first_mode == 4 || state->lc04 == 1)
		{
			long const &mode = (long)ctf_options()->mode;
			if (mode != 4)
				function_2447f0();
			for (long i = 0; i < 9; i++)
			{
				s_team_entry *entry = function_15e410((short)i);

				if (entry)
					function_240820(entry->object_index);
			}
			if (ctf_options()->mode != 4)
			{
				function_2431a0();
				if (ctf_options()->mode != 4 && ctf_options()->team_mode == 1)
				{
					long team = g_51ec80->l1f4;

					if (team >= 0 && team < 8 && !function_161e10(team))
						function_15b3a0(NONE, false);
				}
			}
		}
	}
}

// @retail 0x241900
bool function_241900(long player_index)
{
	s_player_view *player = ctf_player_get(player_index);
	long unit_index = player->unit_index;
	bool result = false;

	if (unit_index != NONE)
	{
		long vehicle_index = function_baf80(unit_index);

		if (vehicle_index != unit_index)
		{
			s_player_iterator iterator;

			iterator.data = g_4e8c24;
			iterator.absolute_index = NONE;
			iterator.index = NONE;
			while (player_iterator_next_with_unit((long *)&iterator))
			{
				if (iterator.index != player_index &&
					iterator.player->team == player->team &&
					player_find_unit_item_of_type(iterator.index, 1) != NONE &&
					iterator.player->unit_index != NONE &&
					function_baf80(iterator.player->unit_index) == vehicle_index)
				{
					result = true;
				}
			}
		}
	}

	return result;
}

#pragma inline_depth(1)
// score 13 (was 13): the cause's team is loaded into EAX where retail loads it into EDX, and the
// fields of the event are written in the other order.
// Round 8: early/top team declarations and player-team name 13 each; early event player 54;
// early players with direct score-call arguments 54; branched initializer/engine-first 37;
// selected type 54; default then override 41; direct array with branched initializer 37;
// defaulted type with if, ternary reassignment or +=7 33 each; Boolean/arithmetic type 54 each.
// The defaulted type reaches retail's early ECX data load but keeps its type branch and late stores.
// Original body restored.
// @retail 0x242e00
void function_242e00(long player_index, long team)
{
	s_event event;

	if (function_15b7c0(1, player_index))
	{
		long local_0 = ctf_options()->engine_type == 9 ? 0x12 : 0xe;
		long local_1 = player_index & 0xffff;
		long local_2 = ctf_player_get(player_index)->team;
		function_1967d0(local_1, local_2, local_0, 1);
	}

	game_engine_event_initialize_inline(&event, ctf_options()->engine_type == 9 ? 10 : 3, 5);
	event.cause_player_index = player_index;
	long cause_team = ctf_player_get(player_index)->team;
	event.effect_team = team;
	event.cause_team = cause_team;
	game_engine_event_send_inline(&event);
}
#pragma inline_depth(255)


// @retail 0x244240
bool function_244240(s_marker_list *list, long object_index, point3f const *point)
{
	list->b0 = 1;
	list->b1 = 0;
	list->l4 = 1;
	function_10da60(object_index, &list->position);
	if (ctf_options()->engine_type == 9)
	{
		list->r14 = 0.1f;
		list->r18 = 0.1f;
	}
	else
	{
		list->r14 = 0.8f;
		list->r18 = 0.1f;
	}
	list->r1c = 0.0f;
	list->l20 = object_index;
	list->color24 = *(s_color_bits const *)point;
	list->color30 = *(s_color_bits const *)point;
	list->r3c = 1.0f;
	list->r40 = 1.0f;
	list->count = 0;
	function_2440a0(list, object_index, point);
	return true;
}

/* the player index of an absolute index (NONE when that player is free) */
/* copies of datum_get_absolute and index_to_datum_index (unknown_16b570.cpp),
   which retail inlines here */
static inline byte *ctf_datum_get_absolute(s_record_pool *data, long index)
{
	byte *result = 0;

	if (index != NONE && index >= 0 && index < data->high_water_index)
	{
		byte *datum = data->data + data->size * index;

		if (*(short *)datum != 0)
			result = datum;
	}

	return result;
}

static inline long ctf_index_to_datum_index(s_record_pool *data, long index)
{
	long result = NONE;

	if (index != NONE)
		result = (*(short *)(data->data + data->size * index) << 16) | index;

	return result;
}

static inline long ctf_player_index_from_absolute(long absolute_index)
{
	s_record_pool *players = g_4e8c24;
	byte *datum = ctf_datum_get_absolute(players, absolute_index);
	long result = NONE;

	if (datum)
		result = ctf_index_to_datum_index(players, absolute_index);

	return result;
}

// @retail 0x2437a0
bool c_game_engine_markers::v46(dword flags, long, s_marker_update *update)
{
	bool result = true;
	dword mask = flags & 0x1f;
	s_slot_table *g;
	long i;

	if (mask)
		result = v43(mask, update) != 0;

	g = g_51ec80;
	if ((flags & 0x20) && g->l1f4 != update->l24)
	{
		function_2432e0(update->l24);
		g = g_51ec80;
	}

	if (flags & 0x80)
	{
		for (i = 0; i < 9; i++)
			g->d[i] = update->a[i];
	}

	if (flags & 0x100)
	{
		for (i = 0; i < 9; i++)
			g->e[i] = update->b[i];
	}

	if (flags & 0x200)
	{
		for (i = 0; i < 9; i++)
			g->flags[i] = update->c[i];
	}

	if (flags & 0x400)
	{
		for (i = 0; i < 3; i++)
		{
			g->triples[i].a = ctf_player_index_from_absolute(update->triples[i].a);
			g->triples[i].b = ctf_player_index_from_absolute(update->triples[i].b);
			g->triples[i].c = ctf_player_index_from_absolute(update->triples[i].c);
		}
	}

	return result;
}
void function_15e360(point3f const *point, long object_index);

// score 12 (was 141): only the players' data pointer is loaded after the event's initializing stores
// (retail loads it into EDX before them). The marker position goes through the existing y-first copy
// helper function_2417a1 (retail's registers and argument convention throughout, the stored scale read
// back for its zero test). Before: retail kept the player in EBP and the marker in EDI and spilled the
// slot to the player's argument home; ours kept the slot in EBP, the marker in ESI and the player in
// EDI. Tried: the stored scale read back 140; a named scale 140; that test without the player reference
// 145; without the masked-index local 140; the team through the masked index 135 (603 B); the masked
// index at entry 136; read through the player reference 140; the player-index reference removed 141.
// An element pointer for the marker entry gave 32, 12 with the stored scale. Tried on 12: the players'
// data pointer read before the initializer 13 (data/team registers swap EAX/EDX; also declared at the
// block top, or const, 13), the cause player's pointer before it 149, the cause team before it 158,
// the team stored first 144, the effect team first 32/42, the event declared at its use 12, the cause
// team read through the event's index 12, the initializer and cause helpers called 178..212, the
// players' data or the cause pointer read before the last change call 63/122 (before the flag store
// 63/140), a named cause team stored after the effect team 14, the cause team before the cause index
// 145, a word-cast index 156. Permuter (600 s, 15 tries): nothing below 12.
// The existing cause setter / initializer / sender forced inline 178/181/107; the player getter forced
// inline 12; the file-local player accessor 12; an event pointer 12; post-initializer player pointers
// through either accessor 12/12.
// 145, a word-cast index 156. Permuter (600 s, 15 tries): nothing below 12. Also tried on 12: an inline
// cause-player setter 12, the existing out-of-line setter called 178 (565 B, not inlined), a chained
// inline initializer returning the event with the inline setter 12, the event at function scope 12,
// typed player-array indexing for the cause team 12.
// @retail 0x242ba0
void function_242ba0(long marker_index, long object_index, long player_index)
{
	long slot = slot_object_get(object_index)->slot;

	if (slot != NONE)
	{
		long marker_team = function_242b60(marker_index);

		if (ctf_options()->engine_type == 9)
		{
			long absolute_index = player_index & 0xffff;

			function_1967d0(absolute_index, ctf_player_get(player_index)->team, 0x13, 1);
			function_1970a0(absolute_index, 0x15, 1);
		}

		if (marker_index != NONE)
		{
			point3f point;
			s_slot_table *g;
			s_event event;
			s_slot_object *object;

			function_2417a1(g_4e0350->marker_entries, marker_index, &point);
			g_51ec80->w204 |= 1;
			function_15e360(&point, object_index);
			g = g_51ec80;
			g->w204 &= ~1;
			g->carriers[slot] = player_index;
			ctf_globals_changed(0x400);
			g->d[slot] = ctf_options()->scale_b;
			if (!g->d[slot])
				g->d[slot] = 10;
			g->times[slot] = g_510c54->field_2_3;
			ctf_globals_changed(0x80);
			g->flags[slot] |= 2;
			ctf_globals_changed(0x200);

			game_engine_event_initialize_inline(&event, 10, 13);
			event.cause_player_index = player_index;
			event.cause_team = event_player_get(player_index)->team;
			event.effect_team = marker_team;
			game_engine_event_send_inline(&event);

			object = slot_object_get(object_index);
			object->lc8 = NONE;
			object->lc4 = NONE;
			object->sc2 = NONE;
		}
	}
}

#include "flexible_surface_calls.h"

void function_19f680(long tag, long group, long pass, long variant, void *context,
	point2f const *vertices, long count, point3f const *center, real radius,
	real perimeter, point3f const *color, real height);
struct s_sort_record;
typedef bool (__stdcall *t_record_fill)(long, void *, long, long, long, void *, s_sort_record *);
void function_41490(short group, long tag, short kind, real distance, t_record_fill fill,
	dword value, void (__stdcall *callback)(void *), void *context, point3f const *position);

/* draws the circle of a slot's marker (a negative slot names the second set): 32 points around the
   marker's position at the slot's radius, eight per quadrant */
// @retail 0x244470
void __stdcall function_244470(long arg_0, long arg_1, long arg_2, long arg_3,
	long arg_4, long arg_5, void *arg_6)
{
	s_marker_entry *entry;
	point3f local_3;
	real local_2;
	if (arg_5 >= 0)
	{
		entry = &g_4e0350->marker_entries[g_51ec80->b[arg_5]];
		local_3 = entry->position;
		local_2 = g_51ec80->bounds[0][arg_5].x;
	}
	else
	{
		long index = -1 - arg_5;

		entry = &g_4e0350->marker_entries[g_51ec80->c[index]];
		local_3 = entry->position;
		local_2 = g_51ec80->bounds[1][index].x;
	}
	point2f local_4[32];
	for (long local_5 = 0; local_5 < 8; ++local_5)
	{
		real local_6 = (real)local_5 * 0.19634954631328583f;
		real sine = (real)sin(local_6);
		real cosine = (real)cos(local_6);
		point2f *first = &local_4[local_5];
		point2f *second = &local_4[local_5 + 8];
		point2f *third = &local_4[local_5 + 16];
		point2f *fourth = &local_4[local_5 + 24];
		first->x = local_3.x + sine * local_2;
		first->y = local_3.y + cosine * local_2;
		second->x = local_3.x + cosine * local_2;
		second->y = local_3.y - sine * local_2;
		third->x = local_3.x - sine * local_2;
		third->y = local_3.y - cosine * local_2;
		fourth->x = local_3.x - cosine * local_2;
		fourth->y = local_3.y + sine * local_2;
	}
	real local_9 = local_4[1].x - local_4[0].x;
	real local_10 = local_4[1].y - local_4[0].y;
	function_19f680(arg_0, arg_2, arg_4, arg_3, arg_6, local_4, 32, &local_3,
		local_2, (real)sqrt(local_9 * local_9 + local_10 * local_10) * 32.f, g_468710, 0.4f);
}

// @retail 0x244610
void __stdcall function_244610(void *arg_0)
{
	function_40f60(arg_0, function_d4bc0, function_244470);
}

// @retail 0x244630
void function_244630(long arg_1, point3f const *arg_0)
{
	byte *local_0 = (byte *)g_4e3b44[g_4e034c->index & 0xffff].data;
	byte *local_1 = *(byte **)(local_0 + 0xc);
	byte *local_2 = *(byte **)(local_1 + 0x534);
	function_41490(0, *(long *)(local_2 + 0xc4), NONE, 640.f,
		(t_record_fill)function_d4bc0, (dword)function_244470, function_244610, (void *)arg_1, arg_0);
}

PRIVATE __forceinline void function_2417a1(s_marker_entry const *arg_0, long arg_1, point3f *arg_2)
{
    point3f const *local_1 = &arg_0[arg_1].position;
    long local_0 = ((long const *)local_1)[1];
    ((long *)arg_2)[0] = ((long const *)local_1)[0];
    ((long *)arg_2)[1] = local_0;
    ((long *)arg_2)[2] = ((long const *)local_1)[2];
}

// @retail 0x2417a0
void c_game_engine_markers::v13(long arg_0)
{
	point3f local_5;
	if (ctf_options()->engine_type == 9)
	{
		s_player_view *local_0 = ctf_player_get(arg_0);
		if (ctf_options()->team_mode != 2)
		{
			if ((byte)local_0->team == 0xff)
				return;
			s_team_entry *local_1 = function_15e410(local_0->team);
			if (!local_1 || local_1->object_index == NONE)
				return;
		}
		for (long local_2 = 0; local_2 < 9; ++local_2)
		{
			short local_3 = ((s_ctf_team_view *)g_4e9ae8)->slot_teams[local_2];
			if (local_3 != NONE && local_3 != local_0->team && function_161e10(local_3))
			{
				short local_4 = g_51ec80->b[local_2];
				if (local_4 != NONE)
				{
					function_2417a1(g_4e0350->marker_entries, local_4, &local_5);
					function_244630(local_2, &local_5);
				}
				local_4 = g_51ec80->c[local_2];
				if (local_4 != NONE)
				{
					function_2417a1(g_4e0350->marker_entries, local_4, &local_5);
					function_244630(-1 - local_2, &local_5);
				}
			}
		}
	}
}

#include "engine_peer.h"

PRIVATE inline bool function_2419c1(short arg_0, short arg_1)
{
	c_engine_peer *local_0 = g_55e4d0[g_4e9ae8->engine_index];
	return local_0 && local_0->p27(arg_0, arg_1);
}
extern color3f *g_468714;
long function_14de70(long local_player_index);

// @retail 0x2419c0
void c_game_engine_markers::v14(long arg_0)
{
	s_marker_list local_14;
	s_marker_list local_31;
	long local_0 = function_14de70(arg_0);
	s_player_view *local_1 = ctf_player_get(local_0);
	bool local_2 = false;
	bool local_4 = false;
	bool local_3 = false;
	if (ctf_options()->engine_type == 9)
	{
		if (player_find_unit_item_of_type(local_0, 1) != NONE || function_241900(local_0))
		{
			if (ctf_options()->team_mode == 1)
				local_3 = true;
			else
				local_4 = true;
		}
	}
	else if (player_find_unit_item_of_type(local_0, 1) != NONE || function_241900(local_0))
		local_2 = true;
	if (ctf_options()->team_mode == 2)
	{
		s_team_entry *local_5 = function_15e410(8);
		if (local_5)
		{
			bool local_6 = !(bool)g_51ec80->flag_bits[8].bit0;
			bool local_7 = local_5->lc != NONE;
			bool local_8 = !local_6 && !local_7;
			bool local_10 = false;
			bool local_11 = false;
			s_color_bits local_9 = *(s_color_bits *)g_468714;
			if (local_7 && ctf_player_get(local_5->lc)->team == local_1->team)
				local_10 = true;
			if (ctf_options()->engine_type == 9 && g_51ec80->flag_bits[8].bit3)
				local_11 = true;
			bool local_12 = false;
			if (!local_7 || !local_10)
			{
				if (ctf_options()->engine_type == 9)
				{
					if (local_6 || local_8)
						local_12 = *(long *)((byte *)g_4e6948 + 0x23c) != 3;
					else if (local_7 && !local_10)
					{
						switch (*(long *)((byte *)g_4e6948 + 0x23c))
						{
						case 0: local_12 = true; break;
						case 2: local_12 = local_11; break;
						}
					}
				}
				else if (local_6 || local_8)
					local_12 = *(long *)((byte *)g_4e6948 + 0x23c) != 3;
				else if (local_7 && !local_10)
				{
					switch (*(long *)((byte *)g_4e6948 + 0x23c))
					{
					case 1: local_12 = true; break;
					case 2: local_12 = true; break;
					}
				}
			}
			if (local_12)
			{
				local_5 = function_15e410(8);
				if (local_5 && (local_5->lc == NONE || ctf_player_get(local_5->lc)->team != local_1->team))
				{
					if (function_244240(&local_14, local_5->object_index, (point3f *)&local_9))
						function_24e59f(&local_14);
				}
			}
		}
	}
	for (long local_15 = 0; local_15 < 8; ++local_15)
	{
		if (ctf_team_is_active(local_15))
		{
			s_team_entry *local_16 = function_15e410((short)local_15);
			if ((local_15 == local_1->team && local_2) || (local_15 != local_1->team && local_4) || (local_15 == g_51ec80->l1f4 && local_3))
			{
				long local_17 = function_158990(local_15);
				if (local_17 != NONE)
				{
					for (long local_18 = 0; local_18 < 2; ++local_18)
					{
						long local_19 = local_18 == 0 ? g_51ec80->b[local_17] : g_51ec80->c[local_17];
						if (local_19 != NONE)
						{
							point3f local_20;
							function_2417a1(g_4e0350->marker_entries, local_19, &local_20);
							if (function_243ed0(&local_14, &local_20))
								function_24e59f(&local_14);
						}
					}
				}
			}
			if (local_16)
			{
				bool local_22 = !(bool)g_51ec80->flag_bits[local_15].bit0;
				bool local_23 = local_16->lc != NONE;
				bool local_24 = !local_22 && !local_23;
				bool local_25 = false;
				bool local_26 = false;
				color3f local_27 = function_7f720((short)local_15);
				s_color_bits local_28 = *(s_color_bits *)&local_27;
				if (local_23 && ctf_player_get(local_16->lc)->team == local_1->team)
					local_25 = true;
				if (ctf_options()->engine_type == 9 && g_51ec80->flag_bits[local_15].bit3)
					local_26 = true;
				bool local_29 = false;
				if (!local_23 || !local_25)
				{
					if (ctf_options()->engine_type == 9)
					{
						if (local_15 == local_1->team)
							local_29 = true;
						else
						{
							switch (*(long *)((byte *)g_4e6948 + 0x23c))
							{
							case 0: local_29 = true; break;
							case 1: local_29 = local_24; break;
							case 2: local_29 = local_26; break;
							}
						}
					}
					else if (local_15 != local_1->team)
						local_29 = true;
					else
					{
						switch (*(long *)((byte *)g_4e6948 + 0x23c))
						{
						case 0: if (local_24) local_29 = true; break;
						case 1: local_29 = true; break;
						case 2: local_29 = !local_22; break;
						}
					}
				}
				if (local_29)
				{
					if (function_244240(&local_31, local_16->object_index, (point3f *)&local_28))
						function_24e59f(&local_31);
				}
			}
		}
	}
	if (local_0 != NONE)
	{
		s_player_view *player = ctf_player_get(local_0);

		if (player->unit_index != NONE)
		{
			s_player_iterator local_32;
			local_32.data = g_4e8c24;
			local_32.absolute_index = NONE;
			local_32.index = NONE;
			while (player_iterator_next((long *)&local_32))
			{
				if (local_32.index != local_0)
				{
					if (!function_2419c1(local_32.player->team, player->team))
					{
						if (function_162550(local_32.index, &local_14))
						{
							long local_35 = player_find_unit_item_of_type(local_32.index, 1);
							if (local_35 != NONE)
							{
								long local_36 = slot_object_get(local_35)->slot;
								if (local_36 != NONE)
								{
									s_color_bits local_38;
									if (local_36 == 8)
										local_38 = *(s_color_bits *)g_468714;
									else
									{
										color3f local_37 = function_7f720((short)local_36);
										local_38 = *(s_color_bits *)&local_37;
									}
									function_2440a0(&local_14, local_35, (point3f *)&local_38);
								}
							}
							function_24e59f(&local_14);
						}
					}
				}
			}
		}
	}
}
