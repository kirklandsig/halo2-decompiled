// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2626b0.h"
#include "unknown_1f4460.h"
#include "units.h"
#include "data_array.h"
#include "unknown_0d0690.h"
#include "unknown_2605d0.h"
#include "unknown_0259a0.h"

bool function_1b8d80(long actor_index, long object_index, short seat_index, bool ignore_reserved);

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool function_e68c0(long type, long unit_index);
real function_11e000(point3f const *b, point3f const *a, vector3f const *d);

/* the slot tests 0x5b and 0x5c, and the slot types 0x5a, 0x59 and 0x51 */

struct s_slot_5a
{
	s_slot_header header;
	long unknown0c;
	byte unknown10[0x40 - 0x10];
};

void function_1f86a0(long index);

static __forceinline long real_to_long(real value);

short __stdcall function_1bbf40(long actor_index, s_slot *slot);
short __stdcall function_1bc2a0(long actor_index, s_slot *slot);
short __stdcall function_1bc6d0(long actor_index);
short __stdcall function_1bc850(long actor_index, s_slot *slot, s_slot *next);
bool __stdcall function_1bc810(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
void __stdcall actor_behavior_release_prop(long actor_index, s_slot *slot, long index);
bool __stdcall function_1bc980(long actor_index, s_slot *slot);
bool __stdcall function_1bcab0(long actor_index, s_slot *slot);
void __stdcall function_1bcc10(long actor_index, s_slot *slot);

// @retail 0x1bc2a0
short __stdcall function_1bc2a0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown858 != NONE)
		goto done;

	{
		long vehicle_index = actor->unknown26c;
		if (vehicle_index == NONE || actor->unknown269)
			goto done;

		s_slot_object_view *vehicle = object_get(vehicle_index);
		if (*(long *)((byte *)vehicle + 0x248) == NONE && vehicle->actor_index == NONE)
		{
			if ((actor->unknown358 == 2 || actor->unknown358 == 4) && prop_node_get(actor->unknown368)->unknown24 >= 1)
			{
				vector3f direction;
				point3f const *end = (point3f *)actor->unknown388;
				point3f const *local_0 = &actor->unknown370;
				direction.i = end->x - local_0->x;
				direction.j = end->y - local_0->y;
				direction.k = end->z - local_0->z;
				real radius = actor->unknown36c;
				if (function_11e000(&actor->position, local_0, &direction) < radius * radius)
					goto trigger;
			}
			if (vehicle->unknownec < 0.1f)
				goto trigger;
		}
		if (actor->unknown358 == 2)
		{
			s_slot_object_view *object = (s_slot_object_view *)function_badc0(actor->unknown360, NONE);
			if (object && (object->parent_index == actor->unknown018 || object->parent_index == vehicle_index))
				goto trigger;
		}
	}
	goto done;
trigger:
	function_e68c0(0x1d, actor->unknown018);
done:
	return g_46fbe4;
}

/* the state of slot type 0x59 */
struct s_slot_59
{
	s_slot_header header;
	real unknown0c;
	long unknown10;
	bool unknown14;
	bool unknown15;
	bool unknown16;
	bool unknown17;
	byte unknown18[0x40 - 0x18];
};

inline real distance3d_fast(point3f const *a, point3f const *b)
{
	real i = a->x - b->x;
	real j = a->y - b->y;
	real k = a->z - b->z;

	return (real)sqrt(j * j + (i * i + k * k));
}

void function_26c180(long actor_index);

// @retail 0x1bc420
short __stdcall function_1bc420(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown024 == 1 || !team_is_enemy(actor->unknown024, 1))
	{
		if (actor->unknown31c != NONE)
		{
			real seconds = g_510c54->field_2_3 * 2.0f;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			if (actor->unknown31e > ticks)
				result = 3;
		}
	}
	return result;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1bc4f0
bool __stdcall function_1bc4f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	_ReadWriteBarrier();
	short team = actor->unknown024;
	bool result = false;

	if (team == 1 || !team_is_enemy(team, 1))
	{
		function_1f86a0(actor_index);
		result = true;
	}
	return result;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1bc580
short __stdcall function_1bc580(long actor_index, s_slot *slot, bool active)
{
	s_slot_5a *state = (s_slot_5a *)slot;
	short result = g_46fbe8;

	if (state->unknown0c == NONE || actor_get(actor_index)->unknown31c == NONE)
		result = g_46fbe4;
	return result;
}

/* the nearest actor of the actor's group not in a vehicle */
// @retail 0x1bc5d0
long function_1bc5d0(long actor_index, long ignore_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;

	if (actor->unknown07c != NONE)
	{
		real best_distance = 3.4028235e38f;
		s_element_502420_member_iterator iterator;
		iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
		s_actor_view *other;

		while ((other = element_502420_member_next(&iterator)) != NULL)
		{
			long other_index = iterator.index;
			if (other_index != actor_index && other_index != ignore_index && other->unknown26c == NONE)
			{
				real distance = distance3d_fast(&actor->position, &other->position);

				if (best_distance > distance)
				{
					best_distance = distance;
					result = other_index;
				}
			}
		}
	}
	return result;
}

/* whether the clump's vehicle (+0x26c) still has seats free that the actor
   cannot take while none of those it wants is taken */
// @retail 0x1bc6d0
short __stdcall function_1bc6d0(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown07c == NONE)
		return 0;

	if (element_502420_get(actor->unknown07c)->member_count > 1)
	{
		s_object_seat seats[0x40];
		short occupied = 0;
		short refused = 0;
		short seat_count;
		{
			short seats_listed = 0;

			function_c8a40(actor->unknown26c, seats, &seats_listed, 0x40);
			seat_count = seats_listed;
		}
		for (short i = 0; i < seat_count; i++)
		{
			s_unit_seat_definition *definition = seats[i].definition;

			if (!TEST_FIELD_BIT(definition->flags.bit11))
			{
				bool wanted = TEST_FIELD_BIT(definition->flags.bit3);

				if (function_c8f60(seats[i].object_index, seats[i].seat_index) != NONE)
				{
					if (wanted)
						occupied++;
				}
				else if (!function_1b8d80(actor_index, seats[i].object_index, seats[i].seat_index, false) && wanted)
				{
					refused++;
				}
			}
		}
		if (occupied == 0 && refused > 0)
			result = 3;
	}
	return result;
}
// @retail 0x1bc810
bool __stdcall function_1bc810(long actor_index, s_slot *slot)
{
	s_slot_59 *state = (s_slot_59 *)slot;
	bool result = true;

	if (!state->unknown14)
	{
		state->unknown15 = true;
		state->unknown0c = 0.0f;
		state->unknown10 = function_1bc5d0(actor_index, NONE);
		state->unknown16 = true;
		state->unknown14 = true;
		result = state->unknown10 != NONE;
	}
	state->unknown17 = false;
	return result;
}

// @retail 0x1bc980
bool __stdcall function_1bc980(long actor_index, s_slot *slot)
{
	s_slot_59 *state = (s_slot_59 *)slot;
	bool result = true;

	if (state->unknown10 == NONE)
		return false;
	if (actor_get(actor_index)->unknown040)
	{
		s_actor_view *other = actor_get(state->unknown10);

		state->unknown17 = true;
		function_26c180(state->unknown10);
		if (other->unknown27c.unknown10 != NONE)
		{
			result = function_1f4460(actor_index, &other->unknown27c.point, other->unknown27c.unknown10, NONE, false);
			if (!result && state->unknown15)
			{
				state->unknown10 = function_1bc5d0(actor_index, state->unknown10);
				state->unknown17 = false;
				result = state->unknown10 != NONE;
			}
		}
		else
			result = false;
	}
	return result;
}

// @retail 0x1bca20
void __stdcall function_1bca20(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown50c)
	{
		actor->unknown41c = 2;
		actor->unknown420 = 0;
	}
}

// @retail 0x1bca60
short __stdcall function_1bca60(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->unknown26 >= 2)
		result = 3;
	return result;
}

// @retail 0x1bcc10
void __stdcall function_1bcc10(long actor_index, s_slot *slot)
{
	bool moving = actor_get(actor_index)->unknown504 == 2;
	s_actor_view *actor = actor_get(actor_index);

	if (moving)
		function_262800(actor_index, actor->unknown418, false);
}

bool function_1f8660(long actor_index);
real function_1f8940(long actor_index);
real function_1e3920(long actor_index);

// @retail 0x1bcab0
bool __stdcall function_1bcab0(long actor_index, s_slot *slot)
{
	bool result = true;
	s_actor_view *actor = actor_get(actor_index);
	if (actor->unknown040)
	{
		s_2605d0_request request;
		memset(&request, 0, sizeof(request));
		request.type = 0;
		if (actor->unknown270 == 4 || actor->unknown270 == 5)
			request.unknown05b = true;
		if (!REFERENCE_EQUAL(actor->unknown418, g_470fa0) &&
			(!function_1f8660(actor_index) || function_1f8940(actor_index) < function_1e3920(actor_index) * 2.0f))
		{
			function_262800(actor_index, actor->unknown418, false);
		}
		else
		{
			*((bool *)&request + 0x692) = true;
			*(s_reference *)((byte *)&request + 0x694) = actor->unknown418;
		}
		byte *scratch = ai_scratch_buffer_get();
		long other_index;
		bool unknown;
		s_reference reference = function_261280((s_prop_search *)&request, actor_index, NULL, &other_index, scratch, &unknown);
		if (REFERENCE_EQUAL(reference, g_470fa0))
		{
			result = false;
		}
		else
		{
			s_reference found = reference;
			reference = function_2626b0(actor_index, found, other_index, scratch, unknown, true);
			if (REFERENCE_EQUAL(reference, g_470fa0))
				function_262800(actor_index, found, false);
		}
		if (!result)
			actor_get(actor_index)->unknown040 = false;
		ai_scratch_buffer_release(scratch);
	}
	return result;
}

// @retail 0x1bcc50
void __stdcall function_1bcc50(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 3;
	actor->unknown420 = 2;
	actor->unknown4ae = true;
	actor->unknown488 = true;
}

s_slot_handler_0 g_47ea3c =
{
	0x5b, 0, 0, -2, 0, function_1bbf40
};

s_slot_handler_0 g_47ea50 =
{
	0x5c, 0, 0, -2, 0, function_1bc2a0
};

s_slot_handler_2 g_47ea68 =
{
	{
		0x5a, 2, 0xbff, -2, 0,
		function_1bc420, function_1bc580, function_1bc4f0, slot_proc_nothing, NONE, {0},
		0, actor_behavior_release_prop, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, slot_proc_nothing
};

s_slot_handler_2 g_47eab8 =
{
	{
		0x59, 2, 0xbff, -2, 0,
		function_1bc6d0, (t_slot_evaluate)function_1bc850, function_1bc810, 0, NONE, {0},
		function_1c1520, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bc980, 0, function_1bca20
};

s_slot_handler_2 g_47eb08 =
{
	{
		0x51, 2, 0, -2, 0,
		function_1bca60, function_1bced0, 0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1bcab0, function_1bcc10, function_1bcc50
};

s_ai_player *ai_player_get(long player_index);
short ai_player_index_get(long player_index);
long function_1b8d40(long object_index);
short function_1b8cc0(long object_index, long *seat_object_index);
bool function_1b9200(long object_index, long prop_index);
point3f *function_b9dd0(long object_index, point3f *result);

// score 47 (was 47): NONE is still kept in EBP where retail keeps it in EBX and the special
// flag still travels in BL where retail uses CL (the special path does `mov cl, 1; jmp
// tail` and the scan stores 0 at its start); this round's trials in this file found no
// lower form of that carrier choice
// Earlier: NONE is still kept in EBP where retail keeps it in EBX and the special
// flag still travels in BL where retail uses CL; this round's trials in this file found
// no lower form of that carrier choice
// Earlier: NONE in EBP (retail EBX) and the special flag in BL (retail CL: the special
// path does `mov cl, 1; jmp tail`, the scan stores 0 at its start) remain; the tail and the
// proximity block now match. Round 78: the tail's prop node as a named pointer 85 -> 51 and the
// deltas x, y, z 51 -> 47 (kept); on that, `&&` flag 200; flag set in if/else arms 200;
// function-scope false flag with the scan in the else 223; bitwise operands swapped 47 (same
// code); conditional-expression flag 194; bitwise flag declared first at function scope 47;
// false flag set to the field on the type-6 path 99 (NONE in EBX, flag memory-only).
// Earlier: retail keeps the special-vehicle flag in CL at the shared tail (the
// special path does `mov cl, 1; jmp tail`, the scan's exits reload it from the dead actor
// argument slot) and lays the tail out after the passenger scan; ours reloads the flag from
// memory there and places the tail right after the special test, so the lockstep compare is
// shifted through the scan. Round 70: the rounding inlined through a forceinline declaration
// of real_to_long (as unknown_1bba20.cpp does) 189 -> 166. With it: bool flag 198 (every
// stack slot as retail, aligned 61); flag set in an if/else around the scan 198; flag set in
// an if 200; flag at function scope 198; proximity deltas declared x, y, z (retail squares z
// first) 166 (198 with the bool); sum written x + y + z 198 (with the bool).
// Round 173: Point-buffer and iterator declarations at the seat phase, and completed player-query scope,
// all seven subsets 166 each; all workspaces with Boolean special carrier 198. Original body restored; CL
// predicate transport and the shared tail remain.
// Round 270: reused vehicle identity 202; guarded reused prop identity 166; both 202;
// native player-seat fields 166; vehicle/prop/both with those fields 202/166/202;
// all native owners with bool special 221 at 861 bytes. Original restored.
// Round 252: native actor/timer/both 166/167/167; normalized conditional 208/208,
// with actor/timer/short both 208/208/206; actual bool field 166, conditional 174,
// bool carrier with conjunction/conditional 198/175. Raw-byte conditional 146/146/147
// changed the integer carrier and was excluded. Complete body and field restored.
// Round 403: returned vehicle/unit/both point receivers 205/162/201;
// with the shared separation producer 205/162/201; both with native
// separation/Boolean special 201/196. Original body restored; CL/tail remains.
// Round 273: complete separation from the existing header/native producers, each with
// reverse sum/shared square/shared dot/all squared parts, 166/166/166/166 and
// 200/200/200/200. Native producer grows the frame to 0x40; original graph restored.
// Round 270 M: state-first conjunction with long/bool/byte/short special 213/213/213/215;
// canonical bitwise conjunction with long/bool/byte/short special 152/85/85/201.
// Boolean bitwise conjunction kept, 85/861 plain; the post-scan tail is shared.
// Entry NONE EBP/EBX, predicate setup and separation SSE homes still differ.
// Round 83: `&&` flag 200 (aligned 23: flag memory-only, the unknown268 block laid out after the
// special path); with returns for the scan exits 226; with an inline timer-minimum helper used twice
// 286; const `&&` flag 200; the player loop as a forceinline helper, `&&`/bitwise flag 235/207.
// Round 85: `&&` flag with the scan as the then-arm and `special = true` in the else 202; flag set in
// both arms (`special = false` in the scan arm) 200; `&&` with a return for the proximity exit 226;
// `&&` with the datum test nesting the loop body 200; `&&` with the proximity minimum as an early
// `goto done` 200. A compile-only screen shows what moves the `&&` layout: the proximity exit inside
// the loop (without it, or without the loop, the special path jumps over the scan as in retail).
// Round 18 (lane 11): `&&` flag, if/else arms, false then if, scan in the else with
// the flag set there or at its declaration: 189/189/191/189/191 at 872 bytes (NONE in
// EBX, entry matches, but the flag stays in memory and the tail follows the special
// path); the tail as a forced-inline helper taking the flag by value 47 (same code),
// with the `&&` flag 189.
// Round 39 (m3 lane 12): the bitwise flag is already spilled around the scan to the dead
// actor-index slot as retail, but its byte def before the branch takes BL (so NONE takes EBP);
// every branchy def gives NONE in EBX but threads the known-true flag by copying the tail's
// first block onto the special path (flag memory-only, 872 bytes): `&&` 200, false then if
// 202, `&& != 0` 200, function-scope `&&` 200; const bitwise flag 47. All restored.
// Round 433: Complementary ordinary-case predicate, bitwise/logical OR, original/complementary
// permission, entry/eligible permission scope; fast 48/74/53/75/200/222/201/222.
// Original body and existing helper restored.
// Round 44 (lane 16, fast counts; the unchanged control gives 47): `special = type == 6;
// special &= byte != 0` 47 (same code); `(type == 6) & actor->unknown268` (raw byte) 193;
// `special = false; if (type == 6) special = actor->unknown268 != 0;` 99 (aligned 11: the entry
// is retail's, NONE in EBX, the special test branches as retail, but the flag is stored
// at both defs and read from memory at the tail, so the tail's movsx takes ECX); that with
// `else special = false` instead of the initializer 187 (877 bytes); that with special
// declared first in the function 135 (frame 0x30).
// Round 18 (lane 14): bitwise flag with the bool field unconverted 193; flag built as `== 6` then
// `&= field != 0` 47 (same code); flag computed without the one-use tag local 47; `&&` flag with a
// return at the proximity exit only 226 (872 bytes; compile-only screen: the special path still falls
// into the unknown268 block); bitwise flag held in a long 172.
// Round 32 (lane 10, fast counts): the tail condition as a named `release` bool 47 (same code), with the `&&`
// flag 200; `special = type == 6; if (special) special = byte != 0;` 210 (877 bytes); the tail as nested
// `if (special) if (prop_index != NONE)` 47 (same code), with the `&&` flag 200; the flag declared next to
// allow_exit and assigned the `&&` value in the block 200; `special = false; if (type == 6 && byte)
// special = true; else { scan }` in the block 202 (872 bytes: both constants stored, read from memory at
// the tail), with the false flag at function scope 223.
// Round 109 (lane 5, fast): the entry tests as one `goto done` 47 (same code). Cause found: NONE
// loses EBX whenever the special def needs a third byte register (EDX keeps the vehicle index for
// 0x1b8d40), so the `!= 0` bitwise def takes BL; `(type == 6) & actor->unknown268` (two byte
// registers) gives NONE in EBX and retail's entry. An explicit `special = false` at the scan's start
// gives retail's split (0 stored to the argument slot, reloaded at the scan's exit, a scratch
// register in the tail): bitwise def 68 (flag AL, NONE EBP); raw-bool def 193 (aligned 18: only the
// setcc def against retail's branchy `mov cl, 1; jmp`, and the tail flag in AL, not CL), operands
// swapped 193 (same); `&&` def 200 (flag memory-only, tail after the special path); the F11 def 97
// (memory-only). Every all-constant (branchy) def keeps the flag in memory.
// Round 15 (m3 lane 2, LTCG env harness = file + depth-4 closure; positional/aligned, base 42/41 = full
// 47): `&&` flag 180/43 (special path stores 1 and the tail is laid out after it); `&& != 0` 180/43; `&&` with
// a return at the proximity exit 210/57; `type == 6 ? byte != 0 : false` 154/20 (876 bytes: NONE in EBX and
// the body retail's, but the def is `setne al` stored to the argument slot and the tail reads memory);
// `type != 6 ? false : byte != 0` 148/19 (862 bytes, same two spots); raw-bool conditional 154/20; false
// initializer with `if (cond) special = true; else { scan }` 184/45; declared bare with `special = false`
// at the scan start 180/43.
// Round 131 (lane 4, aligned counts against 39): seat_object = NONE declared at the outer block's
// top 47 (aligned 40); allow_exit declared after the actor lookup 52 (44); `!!actor->unknown268`
// in the bitwise def 47 (same code); the iterator declared at the outer block's top 47 (same code);
// the seat index in a long 196.
// Round 38 (m3 lane 10, fast): the type-6 false-then-field form (99) is aligned 15: entry, NONE in EBX and
// the scan as retail; left: the true path stores setne to the argument slot (retail `mov cl, 1; jmp`
// to the join), no reload into CL at the join, the tail tests the slot, and the >= 3 block's float
// temporaries move to [esp + 0x18]/[esp + 0x14] because the slot stays live. Compile-only screens
// thread every branchy def (the join placed after the special path) unless the proximity exit leaves
// the loop by `break`. Trials: `return g_46fbe4;` for the proximity exit 76; that with the if/else
// branchy def 226 (872 bytes); with the false-then-field def 133; the negated flag (`scan = true;
// if (type == 6) scan = !field;`) 100 (aligned 18); the false-then-field flag as a long 210 (877).
// Round 55 (lane 12, fast): `special = true;` preset, then `if (!(type == 6 && byte)) { special = false; scan }`
// 187 (aligned 11: the only form found whose special path jumps over the scan to the tail as retail's does, with
// NONE in EBX; left: the preset stores 1 to the slot before the test, and the flag stays in memory at the tail,
// retail splits it, CL outside the scan and the slot inside, so retail's 10.0f temp reuses the slot); that with
// the flag a long 192 (aligned 11), declared at function scope 211; `special = type == 6 ? byte != 0 : false`
// with `special = false` at the scan's start 97 (aligned 11); `special = byte != 0; if (type != 6) special =
// false;` with the scan's false 126 (862 bytes). Harness (this file plus its callees' files; same layouts as the
// build): `&&`, if/else arms, the inverted if/else with `else special = true`, a single-exit inline flag getter
// all lay the tail's first block after the special path; only a preset-true flag keeps the scan between.
// Round 135 (lane 4, fast): the type test named (`bool turret = ... == 6;`) 47 (same code); `if (type == 6 &&
// byte) special = true; else { special = false; scan }` 200 (872 bytes, flag memory-only); the vehicle type read
// into a short local 47 (same code); `& (actor->unknown268 == true)` 47 (same code); `special = false; if (type
// == 6 && byte) special = true; else { scan }` 202.
// Round 117 (fast): `special = false; if (type == 6 && byte) special = true; if (!special) { scan }` 202, with
// `special = false` at the scan's start 200; the bitwise def with that false 68; `special = byte != 0; if (type
// != 6) special = false;` with it 126; `special = false; if (type == 6) special = byte != 0;` with it 97 (861
// bytes, aligned 14: NONE in EBX and the scan as retail; left the setne def stored to the slot on the special
// path, the tail reading the slot, and the 10.0 temp's homes).
// Round 31 (lane 13, fast): the special path as the else of `if (type != 6 || !byte) { special = false;
// scan }` 200 (872 bytes); preset true with that `||` test 187; `if (special == false)` 47 (same code); the
// bitwise operands field first 47 (same code); `special = false; if (type == 6 && byte) special = true;`
// then `if (!special) { scan }` 202 (872 bytes). A compile-only build of this file alone puts NONE in EDI
// (not predictive for this function).
// Round 185 (lane 3, fast): the if/else flag (`mov cl, 1` arm, false at the scan's start) with a
// compiled-out `do {} while (false);` at the tail 200 (872 bytes); one inline delay-minimum helper used
// by both minimum blocks, on the bitwise flag 218 (762 bytes: the copies merge); the bitwise flag as a
// byte 47; the proximity exit as `break` with a `nearby` flag tested after the loop (diagnostic) 200;
// the if/else flag with `special = false` at the end of the scan arm 190 (861 bytes: retail's entry,
// NONE in EBX, `mov cl, 1; jmp` over the scan, the scan laid out next and the flag in CL at the tail;
// left: retail stores the 0 at the scan's start and reloads it at the exit, and its frame is 0x34 where
// ours is 0x30, the seat object taking the dead actor slot retail gives the flag); the false after
// `allow_exit = true` (live across the team test and the loop) 200 (872 bytes).
// Round 44 (lane 1, fast): if/else arms with `special = actor->unknown268 != 0` in the special arm and `special =
// false` opening the scan 200 (872 bytes: the field is re-read at the tail, but the arm stores 1 to the slot and
// the tail's first block is laid out right after it); with the bitwise flag: the proximity exit as `return
// g_46fbe4;` 76, every scan exit as a return 76, the 0x1b8d40 exit as a nested if 47 (same CFG), the
// datum-index `continue` as a nested if 47 (same code).
// Round 108 (m3 lane 7, fast): `special = false; if (type == 6) { if (byte) special = true; }` 202 (872 bytes);
// `if (type == 6 && byte) special = true; else if (0x1b8d40 != NONE) goto done; else { special = false; ... }`
// 200 (872); `special = false; if (type == 6 && byte) special = true; else { scan }` 202 (872); the raw-byte
// bitwise def with `special = false` opening the scan and seat_object declared (NONE) before it 193 (862 bytes,
// aligned 13: only the setcc def and the AL/CL tail register left), with the false after the 0x1b8d40 test 193
// (aligned 14); on that seat placement, `type == 6 ? byte : false` 211 (877), `special = false; if (type ==
// 6) special = byte;` 97 (861 bytes, aligned 12: entry exact to +0x87; left the setne def stored to the slot,
// the tail reading memory and the 10.0 temporaries' homes). Every constant (branchy) def keeps the flag in
// memory at the tail; retail's `mov cl, 1` with the split spill is still unreproduced.
// Round 141 (lane 4, fast): the special test as a multi-exit inline helper (`return false` twice,
// `return true`) 200 (872 bytes); `byte special = 0; if (type == 6) special = actor->unknown268;` 99;
// the proximity exit as `close = true; break;` with the minimum after the loop under `if (close)`:
// `&&` 200, bitwise 47, false-then-field 99 (each the same as its goto form: the break is threaded
// back); that with the preset-true `||` form 187; the whole test in an inline helper taking the
// actor index 268 (813 bytes).
// Round 60 (m3 lane 9, fast): on the if/else flag with `special = false` closing the scan arm (190, frame
// 0x30: the unspilled flag leaves the dead actor slot to the seat object), the seat object declared bare
// at the function top or the outer block top and assigned NONE before the call 190/190 (same code); it
// declared with its NONE at the function top 209 (it moves to the frame, the seat short then takes the
// actor slot, frame still 0x30); `special = false` also opening the scan arm (diagnostic) 190 (dead, dropped);
// the `&&` flag with allow_exit declared after the actor lookup 201 (872 bytes); the if/else flag with
// `special = false` right after the 0x1b8d40 exit (live across the rest of the scan) 200 (872 bytes).
// Round 65 (lane 12, fast): round 185's if/else flag (`special = true` arm, `special = false` ending the
// scan arm) rebuilt 190 (frame 0x30: the flag needs no memory, so the seat object takes the dead actor
// slot); on it: `long seat_object = NONE;` declared before the actor lookup 209 (the NONE store moves to
// the entry); seat_object declared bare at the outer block's top and assigned in place 190 (frame still
// 0x30); the special arm assigning `actor->unknown268 != 0` 190 (folded); the flag as a byte 190; the
// false moved to just before the iterator loop 200 (872 bytes, memory-only, and unset on the team path).
// Diagnostic (listed form): false at the scan's start makes the flag memory-only and lays the tail after
// the special path (872 bytes), while the bitwise base already splits the flag (BL, the actor slot
// across the scan, BL again) as retail does with CL.
// Round 58 (lane 16, fast): the if/else flag with `special = false` at the end of the scan arm 190 again
// (861 bytes; frame 0x30, the seat object in the dead actor slot); that with a second `special = false` at
// the scan's start (diagnostic) 190 (the start def is dropped as dead); the if/else flag as a byte (1, and
// 0 at the scan's start) 200 (872 bytes); the false def after the 0x1b8d40 exit test 200 (872), after the
// seat call before `allow_exit = true` 200 (872). Any false def live across a call copies the tail's first
// block onto the special path; only the def at the scan's end keeps retail's `mov cl, 1; jmp`. On that
// end-def form: `long seat_object = NONE` at the outer block's top 192, the iterator there 190 (same code).
// Round 27 (m3 lane 2, fast): if/else def with `special = false` after the 0x1b8d40 test 200 (872 bytes,
// memory-only), just before `allow_exit = true` 200; the false at the end of the scan arm 190 (as round 185),
// with `seat_object` declared and set to NONE at function scope 209 (frame still 0x30), declared there and
// assigned in place 190 (same code), with a byte flag 190 (same code); the `&&` flag renamed to sort first or
// last 200/200 (same code); a forceinline tail helper taking the flag and allow_exit by value with the `&&` def
// 199 (865 bytes), with the if/else def and the false at the scan's start 199.
// Lane 12 r72 (fast): the bitwise flag as a long 172; declared bare at the function top and assigned in
// place 47 (same code); the tag pointer folded into the type read 47 (same code); `& actor->unknown268`
// without the `!= 0` 193; `if (type == 6 && byte) { special = true; goto tail; } special = false;` before
// the scan, the label opening the tail, 200 (872 bytes: the special path stores 1 to the argument slot and
// the tail is laid out right after it, so the tail-after-the-scan layout is not a threading copy).
// Round 110 (m3 lane 7, fast): retail homes the flag in the dead actor_index parameter slot. `bool special = type
// == 6; special &= actor->unknown268 != 0;` 47 (same code); the if/else def with `special = false` opening the
// scan and the tail's prop test nested under `if (special)` 200 (872 bytes); the if/else def with `special`
// declared before `allow_exit` 200 (872 bytes); the bitwise def with `(bool)actor->unknown268` 47 (same code);
// `special` declared bare before the tag lookup 47 (same code); the permuter (anneal, 600 s, 55 variants) found
// nothing below 47.
// Round 63 (m3 lane 16, fast): preset-true flag with the tail test written `prop_index != NONE && special`
// 187 (same code as unswapped); the if/else flag (false at the scan's start) with that tail order 200 (872
// bytes); the preset-true flag as a byte 187; preset true with the false def after the 0x1b8d40 exit 187.
// Lead: on the if/else flag with the false at the scan's start, `allow_exit = true` moved from after the seat
// call to the end of the scan arm (diagnostic) gives retail's exact flag code in the real build (`mov cl, 1;
// jmp` over the scan, 0 stored to the dead actor slot at the scan's start, the slot reloaded into CL at the
// scan's exit, the scan laid out before the tail) but 233 (839 bytes: allow_exit then lives in BL, so NONE
// takes EBP and the frame is 0x30). Without a statement at the end of the scan arm the empty join is removed,
// the tail is laid out after the special arm and the flag stays in memory; retail's IR had something there.
// @retail 0x1bbf40
short __stdcall function_1bbf40(long actor_index, s_slot *slot)
{
    bool allow_exit = false;
    s_actor_view *actor = actor_get(actor_index);
    if (actor->unknown858 == NONE && actor->unknown26c != NONE && !actor->unknown269)
    {
        byte *tag = g_4e3b44[object_get(actor->unknown26c)->tag_index & 0xffff].bytes;
        bool special = (*(short *)(tag + 0x1f0) == 6) & (actor->unknown268 != 0);
        if (!special)
        {
            if (function_1b8d40(actor->unknown26c) != NONE)
                goto done;
            long seat_object = NONE;
            short seat = function_1b8cc0(actor->unknown26c, &seat_object);
            allow_exit = true;
            if (!team_is_enemy(actor->unknown024, 1))
            {
                s_data_datum_iterator iterator;
                iterator.data = g_4e8c24;
                iterator.index = NONE;
                iterator.datum_index = NONE;
                while (data_datum_iterator_next(&iterator))
                {
                    if (iterator.datum_index == NONE)
                        continue;
                    s_ai_player *player = ai_player_get(iterator.datum_index);
                    short player_index = ai_player_index_get(iterator.datum_index);
                    if (player)
                    {
                        if (player->unit_index == seat_object && *(short *)((byte *)player + 8) == seat &&
                            *(short *)((byte *)player + 0xa) > 0)
                            goto done;
                        if (actor->unknown31c == player_index)
                            goto done;
                    }
                    long unit_index = *(long *)((byte *)iterator.datum + 0x2c);
                    if (unit_index != NONE)
                    {
                        point3f vehicle_point;
                        point3f unit_point;
                        function_b9dd0(actor->unknown26c, &vehicle_point);
                        function_b9dd0(unit_index, &unit_point);
                        real x = unit_point.x - vehicle_point.x;
                        real y = unit_point.y - vehicle_point.y;
                        real z = unit_point.z - vehicle_point.z;
                        if (x * x + y * y + z * z < 6.25f)
                        {
                            short minimum = (short)real_to_long((real)g_510c54->field_2_3 * 3.0f);
                            if (*(short *)actor->unknown2f4 < minimum)
                                *(short *)actor->unknown2f4 = minimum;
                            goto done;
                        }
                    }
                }
            }
        }
        if (actor->unknown268)
        {
            if (actor->unknown086 >= 3)
            {
                short minimum = (short)real_to_long((real)g_510c54->field_2_3 * 10.0f);
                if (*(short *)actor->unknown2f4 < minimum)
                    *(short *)actor->unknown2f4 = minimum;
                allow_exit = false;
            }
            else
                allow_exit = true;
        }
        if (special && actor->prop_index != NONE)
        {
            s_prop_node_view *prop = prop_node_get(actor->prop_index);
            if (prop->unknown24 >= 1 && prop->unknown24 <= 2 && !function_1b9200(actor->unknown26c, actor->prop_index))
            {
                *(short *)actor->unknown2f4 = 0;
                allow_exit = true;
            }
        }
        if (allow_exit && *(short *)actor->unknown2f4 <= 0)
            function_e68c0(0x1d, actor->unknown018);
    }
done:
    return g_46fbe4;
}
