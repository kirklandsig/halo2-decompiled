// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26e370.h"
#include "unknown_11cc90.h"
#include "units.h"
#include "unknown_26b230.h"
#include "unknown_0259a0.h"
#include "unknown_1f9240.h"
#include <string.h>
#include "unknown_0d0690.h"
#include <float.h>

static __forceinline long real_to_long(real value);

/* the slot tests 0x5f, 0x60, 0x5e, 0x4d, 0x4e and 0x4f, and slot type 0x4c */

struct s_slot_4c
{
	s_slot_header header;
	byte unknown0c[4];
	long element_index;
	byte unknown14[0x1c - 0x14];
	long unknown1c;
	short seat_index;
	byte flags;
	byte unknown23;
	short unknown24;
	byte unknown26[2];
	real unknown28;
	real unknown2c;
	byte unknown30[0x34 - 0x30];
	point3f point;
};

short __stdcall function_1b94c0(long actor_index, s_slot *slot);
short __stdcall function_1b9500(long actor_index, s_slot *slot);
short __stdcall function_1b9540(long actor_index, s_slot *slot);
short __stdcall function_1b9640(long actor_index, s_slot *slot);
short __stdcall function_1b9890(long actor_index, s_slot *slot);
short __stdcall function_1b99d0(long actor_index, s_slot *slot);
short __stdcall function_1b9fc0(long actor_index);
short __stdcall function_1ba4e0(long actor_index, s_slot *slot, bool active);
long __stdcall function_1ba090(long actor_index, s_slot *slot);
void __stdcall function_1ba3f0(long actor_index, s_slot *slot, s_slot_target_list *list);
bool __stdcall function_1ba5c0(long actor_index, s_slot *slot, long index);
short __stdcall function_1bb3a0(long actor_index, long joint_index, long a, long b);
short __stdcall function_1b2ff0(long actor_index);
void *function_1e5240(long actor_index);
short __stdcall function_c8b80(long object_index, s_object_seat *seats, short maximum_count);

/* how the actor's character uses its weapon (unknown_1e1f20.h) */
struct s_character_weapon_1b9
{
	byte unknown00[0x10];
	real unknown10;
};
short __stdcall function_1bcc90(long actor_index);

/* a seat of a vehicle's tag (0xb0 bytes) */
struct s_vehicle_seat_definition
{
	s_seat_definition_flags flags;
	byte unknown04[0x88 - 0x4];
	real unknown88;
	real unknown8c;
	byte unknown90[0xb0 - 0x90];
};

/* the vehicle tag's flags at +0x1ec */
struct s_vehicle_tag_flags
{
	dword unknown0 : 11;
	dword bit11 : 1;
	dword unknown12 : 20;
};

struct s_vehicle_tag_view
{
	byte unknown000[0xbc];
	byte flagsbc;
	byte unknownbd[0x1c8 - 0xbd];
	long seat_count;
	s_vehicle_seat_definition *seats;
	byte unknown1d0[0x1ec - 0x1d0];
	s_vehicle_tag_flags flags1ec;
	short unknown1f0;
};


struct s_player_view
{
	byte unknown000[0xc0];
	char team;
	byte unknown0c1[0x21c - 0xc1];
};

point3f *function_b9dd0(long object_index, point3f *result);
real function_30bf0(vector3f *v);
real function_11cc90(vector2f const *a, vector2f const *b);

/* an element of g_502424 as slot type 0x4c sees it */
struct s_4c_element
{
	byte unknown00[0x7c];
	short unknown7c;
	byte unknown7e[2];
	long object_index;
	bool unknown84;
	byte unknown85;
	bool unknown86;
};

inline void object_seat_unreserve(s_slot_object_view *object, long seat_index)
{
	if (seat_index >= 0 && seat_index < 32)
		object->unknown3b4 &= ~(1 << seat_index);
}

/* the outermost vehicle carrying the object */
// @retail 0x1b8c80
long function_1b8c80(long object_index)
{
	long result = NONE;

	while (object_index != NONE)
	{
		s_slot_object_view *object = object_get(object_index);

		if (object->type != 1)
			break;
		result = object_index;
		object_index = object->parent_index;
	}
	return result;
}

// @retail 0x1b8cc0
short function_1b8cc0(long object_index, long *seat_object_index)
{
	s_object_seat seats[0x40];
	short result = NONE;
	long seat_object = NONE;
	short count = 0;

	function_c8a40(function_1b8c80(object_index), seats, &count, 0x40);
	for (short i = 0; i < count; i++)
	{
		if (TEST_FIELD_BIT(seats[i].definition->flags.bit2))
		{
			result = i;
			seat_object = seats[i].object_index;
			break;
		}
	}
	if (seat_object_index)
		*seat_object_index = seat_object;
	return result;
}

// @retail 0x1b8d40
long function_1b8d40(long object_index)
{
	long result = NONE;
	long seat_object_index = NONE;
	long vehicle_index = function_1b8c80(object_index);
	short seat_index = function_1b8cc0(vehicle_index, &seat_object_index);

	if (seat_object_index != NONE && seat_index != NONE)
		result = function_c8f60(seat_object_index, seat_index);
	return result;
}

// @retail 0x1b8d80
bool function_1b8d80(long actor_index, long object_index, short seat_index, bool ignore_reserved)
{
	s_actor_view *actor = actor_get(actor_index);
	s_object_header_view *header = object_header_get(object_index);

	if (header->type == 1)
	{
		s_slot_object_view *object = (s_slot_object_view *)header->object;

		if (seat_index >= 0 && seat_index < 32)
		{
			dword mask = 1 << seat_index;

			if (object->unknown3b0 & mask)
				return true;
			if (!ignore_reserved && (object->unknown3b4 & mask))
				return true;
		}
	}
	if (actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1))
	{
		for (short i = 0; i < MAXIMUM_AI_PLAYERS; i++)
		{
			s_ai_player *entry = &g_4f55cc[i];

			if (entry->player_index != NONE && entry->unit_index == object_index && entry->unknown08 == seat_index &&
				entry->unknown0a > 0)
			{
				return true;
			}
		}
	}
	if (actor->unknown2f2 > 0 && actor->unknown2e8 == object_index && actor->unknown2ec == seat_index)
		return true;
	return false;
}

// @retail 0x1b8eb0
bool function_1b8eb0(long actor_index, long object_index, short seat_index, bool ignore_reserved)
{
	bool result = false;

	if (function_c8f60(object_index, seat_index) == NONE &&
		!function_1b8d80(actor_index, object_index, seat_index, ignore_reserved))
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE && function_c8200(actor->unknown018, object_index, seat_index))
			result = true;
	}
	return result;
}

// score 3 (was 3): at the 0xc8200 call (now in retail's (unit, object, seat) order, which loads
// the unit last as retail) ours reloads the seat pointer into ECX and reads the object at once;
// retail reloads it into EAX and loads the seat index in between. Before the reorder ours loaded
// the unit index first. Round 23: the other parameter orders 3/4/4/2 (the last, (object, unit,
// seat), loses 0x11b0c0's match); flag tested through seats[i] 82; actor lookup at the call 135;
// seat pointer 100; nested 0xc8200 test 3; actor after result 3; named actor team 3; seat array
// declared first 3; plain bitfield test 69. Round 70: one result with `break` for
// the two rejections, bool result and the seat count from 0xc8b80 (retail inlines it) gave the
// shrink-wrapped ebp/esi saves (109 -> 3); 0xc8b80 alone 109, bool alone 117, without 0xc8b80 7
// (harness), long result 48 (harness); every 0xc8200 parameter order 4-27 (harness); nested
// c8200 test 3; actor declared after result 3; unit index local 122; named seat index 78; seat
// pointer 71; named seat object 71 and occupant test inverted 46 (harness).
// Round 270: complete seat snapshot 121; reused object/seat/both snapshots 75/104/137;
// named hostile/reservation/both decisions 3/3/3; native tag receiver 3. Restored.
// Round 252: eligibility accumulated with logical OR 43, bitwise OR 31, conditional 3;
// named eligibility 3; actor at every use 116 (matched caller lost); read-only actor,
// read-only flags, both 3/3/3. Original complete body restored.
// Round 42: unit/seat/object/paired field-address locals 3/3/3/3; const actor pointer 3;
// read-only full definition 3; separate occupied flag 62; read-only count 3. Restored.
// Round 56 (lane 9, fast): the definition pointer itself kept and its flags read at the test 3; the
// 0xc8200 test nested under `!function_1b8d80(...)` 3; `result |= function_c8200(...)` there 31; the
// c8200 arguments read through `(&seats[i])->` 3; a block-scoped seat pointer for the c8200 arguments 73
// (380 bytes). A one-file LTCG harness is not exact here (349-361 bytes).
// Round 54 (m3 lane 11, fast; 0xc8200 now matches): the count from a direct 0xc8a40 call 3; `if (bit11)
// continue;` with the occupant code unnested 3; the definition-flags pointer declared after the object block 84;
// the loop index at function scope 3; last_object_index declared after the seat array 3 (a repeat); a `long`
// count 121 (390 bytes); the occupant's object in a named pointer for the team read 3.
// Round 124: actor declared after the last-object local/seat array/count 3/3/116; object and team
// rejections inverted 3/3; object and team tests nested 3/3; the else-if split into an else 3. Restored.
// Round 34 (lane 17, fast): occupant declared at the loop top 3; team compare written actor-first 4;
// occupant test as `== NONE` with the team arm in the else 49; last_object_index stored before the
// object test 8; result declared after the count 52 (366 bytes). Restored.
// Round 169 (lane 4, fast): occupant renamed 3; `continue` closing the occupant arm with the reservation
// test unnested 3; the reservation result in a named bool 3; `continue` when reserved, c8200 test after 3;
// the bit11 skip as `continue` (a repeat) 3; the loop as `while` with the increment at its end 3; a 600 s
// fast annealing permuter run: 25 tries, its reduced image scores the original 112, none below 3. Restored.
// Round 85 (fast, 0xc8200 matched): 0xc8200's parameter orders again, at its definition, declaration and
// all five calls: (unit, seat, object) 3 (0x11ade0, 0x1b8eb0 lost), (object, unit, seat) 2 (0x11b0c0 lost;
// the unit load then moves ahead of the object load), (object, seat, unit) 3 (0x11b0c0 lost), (seat, unit, object) 4,
// (seat, object, unit) 4; last_object_index as the 0xc8200 object argument 69, as all three calls' 128
// (358 bytes); the definition read at the test without its local 84; object or occupant at function scope 3/3.
// Round 172 (m3 lane 4, fast): (object, seat, unit) again (definition, declaration, five calls): the
// seat-pointer reload then takes EAX as retail but the unit load moves from last to second, 3 (0x11b0c0
// lost, its unit/object moves swapped); `long i` 127 (361 bytes); a fresh actor lookup at the 0xc8200 call
// 135 (399 bytes); `last_object_index != seats[i].object_index` 4; the seat's object index named at the
// loop top for the object test, the store, 0xc8f60 and 0x1b8d80 3 (same code); `dword last_object_index` 3;
// `object->unknown3b4 != 0` 3. Restored.
// Round 156 (m3 lane 6): the seat index named in the unoccupied arm for both calls 78 (fast); the seat's object index
// named there 74 (fast). LTCG one-file harness (this file, units.cpp, unit_object_type.cpp, unknown_1c9830.cpp and the
// ballast reproduce 3): 45 combinations of declaration order, flags-pointer placement, `long i`, `(&seats[i])->`
// arguments, a unit local and else-if/nested/else forms best 3 (declaration-order shuffles 5-107); 0xc8200's result in
// a named bool 3; the seat's object index named at the loop top for every use including 0xc8200 69. Probes: a constant
// unit or seat argument still reloads the seat pointer into ECX.
// Round 157 (m3 lane 6, harness): our reload of the spilled `&seats[i]` returns to its home register ECX; retail's goes to
// EAX with the seat-index load between. A seat pointer at the loop top for the 0xc8200 arguments 69 (fast 73 at 380 bytes; the
// pointer takes EBP, frame 0x310), for every use 84; a reference for every use 84; the pointer for both unoccupied-arm calls 82; the
// 0xc8200 test nested under the 0x1b8d80 test 3 (fast 3); a short seat index local for both unoccupied-arm calls 70.
// Round 224 (m3 lane 3, fast): a seat pointer declared at the loop top and used only for the 0xc8200 object
// argument (the seat index still through seats[i]) 3 (same code); a `long *` to the seat's object index used only
// there 3 (same code); the loop-top seat pointer for every object-index read and the definition 25; the 0xc8200
// result as `result = c8200(...) ? true : result` under the 0x1b8d80 test 3; the loop test as `count > i` 5; the
// occupant looked up again for the team read instead of a local 67 (387 bytes).
// Round 156 (m3 lane 8, fast): 1-8 unused initialised locals at the top or at the loop-body top all 3 (no numbering
// tie); the seat index named at the top of the bit11 block for all three calls 78, the seat's object index there 74
// (both repeats of round 156 lane 6), both 137 (349 bytes); the 0xc8200 test ahead of the 0x1b8d80 test 40 (383 bytes);
// 0xc8200 nested under the 0x1b8d80 test with its own `result = true` 3 (same code); a walking seat pointer advanced
// at the body's end for every seat read 130 (360 bytes); the two rejections as direct `return false` 118 (365 bytes);
// `result = false;` assigned just before the loop 52 (366 bytes, round 34's code).
// Round 175 (m3 lane 5, fast with 0xc8200 0xc8b80 0xc8f60 0x1b8d80 0x1df560 0x11b0c0 0x11ade0 0x1b8eb0): retail's setup is
// the plain last-to-first order of (unit, object, seat) with the reload lifted above the seat load: seat into DX, the saved
// seat address into the first free register (EAX) and through it, the unit into ECX. Ours reads the object argument first,
// through ECX, in every form: an unsigned conversion on the object argument 3 (same code); the object index in a local
// just before the call 3 (same code); the seat index in a short local just before the call 33 (381 bytes: the seat is then
// zero-extended and loaded last, the object still first through ECX); both locals 33; a function-scope seat pointer set at
// the loop top for the object argument 3 (same code); the seat pointer kept in a local record ahead of the seat list, read
// back for the object argument, 17 (the store moves ahead of the object read at the loop top, the reload still ECX).
// tools/permute.py with the eight callers and callees as `--context` (its reduced image then scores 3, not round 169's
// 112): 78 tries in 900 s, 17 at 3, none below.
// Round 176 (m3 lane 6, fast, base fast 3): a loop-top `&seats[i]` pointer used only for the 0xc8200 seat argument 73
// (380 bytes), only for the 0x1b8d80 arguments 74, only for the 0xc8f60 arguments 94, only for the object test, lookup
// and store 102 (392 bytes), for the object arguments of both unoccupied-arm calls 94 (382 bytes). Also: 0xc8200 nested
// under the 0x1b8d80 test with the unit read into a block-scoped local just before the call 3 (the seat-pointer reload now
// takes EAX and the seat loads between it and the object read, as retail, but the unit load moves first: `mov ecx,
// [edi + 0x18]` ahead of the seat); the occupied-object rejection alone as `return false` 102 (391 bytes); the hostile-team
// rejection alone as `return false` 118 (391 bytes). Restored.
// @retail 0x1b8f30
bool function_1b8f30(long actor_index, long object_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	long last_object_index = NONE;
	s_object_seat seats[0x40];
	short count = function_c8b80(object_index, seats, 0x40);

	for (short i = 0; i < count; i++)
	{
		s_seat_definition_flags *definition = &seats[i].definition->flags;

		if (seats[i].object_index != last_object_index)
		{
			s_slot_object_view *object = object_get(seats[i].object_index);

			if (object->type == 1 && object->unknown3b4)
			{
				result = false;
				break;
			}
			last_object_index = seats[i].object_index;
		}
		if (!TEST_FIELD_BIT(definition->bit11))
		{
			long occupant = function_c8f60(seats[i].object_index, seats[i].seat_index);

			if (occupant != NONE)
			{
				short team = object_get(occupant)->team;

				if (team != actor->unknown024 && function_1df560(team, actor->unknown024))
				{
					result = false;
					break;
				}
			}
			else if (!function_1b8d80(actor_index, seats[i].object_index, seats[i].seat_index, false) &&
				function_c8200(actor->unknown018, seats[i].object_index, seats[i].seat_index))
			{
				result = true;
			}
		}
	}
	return result;
}

/* false when an enemy of the actor rides the object */
// @retail 0x1b90b0
bool function_1b90b0(long actor_index, long object_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = true;

	for (long child_index = object_get(object_index)->first_child_index; child_index != NONE; )
	{
		s_slot_object_view *child = object_get(child_index);

		if (child->type == 0 && child->unknown1fc != NONE)
		{
			short team = 0;

			if (child->actor_index != NONE)
				team = actor_get(child->actor_index)->unknown024;
			else if (child->player_index != NONE)
				team = ((s_player_view *)g_4e8c24->data)[child->player_index & 0xffff].team;
			result = !function_1df560(team, actor->unknown024);
			if (!result)
				break;
		}
		child_index = child->next_object_index;
	}
	return result;
}

/* the player riding the object */
// @retail 0x1b9190
long function_1b9190(long object_index, long *rider_index)
{
	long result = NONE;
	s_slot_object_view *object = object_get(object_index);

	if (rider_index)
		*rider_index = NONE;
	for (long child_index = object->first_child_index; child_index != NONE; )
	{
		s_slot_object_view *child = object_get(child_index);

		if (child->type == 0 && child->player_index != NONE)
		{
			result = child->player_index;
			if (rider_index)
				*rider_index = child_index;
			break;
		}
		child_index = child->next_object_index;
	}
	return result;
}

PRIVATE __forceinline void function_1b9201(long arg_0, vector3f *arg_1)
{
	s_slot_object_view *local_0 = object_get(arg_0);

	if (local_0->parent_index == NONE)
		*arg_1 = local_0->forward;
	else
		matrix_transform_vector(object_node_matrix(object_get(local_0->parent_index), local_0->parent_node), &local_0->forward, arg_1);
}

// score 8 (was 8; 11 earlier): native 0x2c frame and 541-byte length; first +0x79.
// Count and index now occupy ESI/EBP as retail; the inclusive last-index bound
// decrements the count and uses js/jle instead of native cmp/jle and cmp/jl.
// The selected pointer's NULL store still uses a separate EAX zero.
// MiniMax C (11-11): 6 (was 8), NOT kept (coordinator 11-11: a guard the loop test already implies is match-only; body at 8): the seat count test around the scan keeps the count in ESI and the index in EBP as retail does and brings the native cmp/jl with it; the exclusive bound without that test still gives 11 with the count in EBP and the index in ESI, and the seat pointer's zero still lands in EAX where retail reuses the index's zero. Swapping the two declarations compiles byte-identically, the index declared in the for statement gives 11 with the count in EBP and the index in ESI, and writing the test the other way round (index against count) gives the same 11 with the count in EBP. This round also tried: the seat pointer held as a dword and as an int with the address cast at the use, the seat pointer initialised from the index, the count test written against the literal zero, the scan written as a while loop with the increment at the bottom, and the NULL store split from the declaration with the index declared first — every one of them gives the same six differences, with the count test still against a second zero in EAX; the index declared inside the count test moves the seat pointer's stack slot and scores 135. A permuter run with annealing over the function (146 tries) found nothing below the baseline.
// MiniMax H (11-11): no new form, no lower score (8); the body is unchanged, because a seat-count test with the
// index declared before it reaches 6 by putting the count in ESI and the index in EBP, but that test repeats the
// loop's own bound and is not kept, and the exclusive bound (11), the count in a local (11), the index declared
// before the seat pointer (8) and a guarded do-while (11) all either move the count to EBP and the index to ESI
// or keep the decremented count and js/jle where retail has cmp/jle and cmp/jl.
// MiniMax C (11-10): no lower score (8); a seat-count test around a scan whose index is declared before that test puts the count in ESI and the index in EBP and keeps the native cmp/jl, but the test compares against a second zero in EAX and the selected pointer's NULL store takes that zero too (6); the same test written with the index folds into the loop (11), the reversed spelling of it keeps a constant test (6), a count read before the flags test hoists the load (14), a last-seat local is 108 with the inclusive bound and 118 with a strict one, and the inclusive bound with the index declared before the loop is byte-identical to the kept body.
// MiniMax C (11-10): no new form, no lower score (8); the exclusive bound gives 11 with the count in EBP and the index in ESI (the reversed comparison 12), a seats local with the exclusive bound 23 and with the inclusive bound 27, an explicit count guard 135 and a while loop with the index declared first 23 — every exclusive form moves the count to EBP and the index to ESI.
// MiniMax I (11-08): no new form, no lower score (8); the exclusive bound with the count in a local, the index declared before the seat pointer and the reversed comparison all give 11 or 12 with the count in EBP and the index in ESI.
// MiniMax R (11-06): no lower score (8); the exclusive bound 11, the same with the count in a local 11, the index declared before the seat pointer 11, both together 11 and the inclusive bound with a local count 26 all move the count to EBP and the index to ESI; a long count local with the index declared before the seat pointer is 11 as well, and a short count makes the compiler compare in 16 bits and rewrites the loop.
// MiniMax E (11-06): no new form, no lower score.
// Exclusive bound with the index declared before the seat pointer 11, the same
// with the count in a local 11, a widened int copy of the index 11, an
// exclusive while loop 11 and a neq bound 12: every exclusive bound moves the
// count to EBP and the index to ESI, so retail's ESI count and EBP index stay
// out of reach while the inclusive bound keeps them and loses the cmp/jl.
// Earlier forms: a seat-count-minus-one local 8, not kept (one-use local);
// explicit positive-count guard 6, not kept (redundant test), and 135;
// index declared before the loop 8/11; reversed comparison 9; seat pointer
// after the count 108; enclosing-block index initialization 14; count local
// 11/14; function-scope seat pointer 67; unchanged recompile 11.
// A 179-trial permuter run stayed at 8; three other runs hit their time limit.
// Shared forward helper 302; state after/between vector declarations 11;
// split/function-scope state 11; const seat/state 11; result between lookups 11;
// split seat initialization 11; index-first plus state after vectors 11.
// P323: direct exclusive bound 11; inclusive conditional seat publication 8;
// exclusive conditional/failure-first publication 11/11; exclusive bound with
// seat_index/selected_seat/target_state names 11/11/11; complete direct tag
// reads at every use 27. All at 541 bytes; original complete body restored.
// MiniMax A (11-11, second pass): no new form, no lower score (8); the seat scan's preamble is still the whole difference: retail keeps
// the count in ESI and the index in EBP, tests the count against the index's own zero register (cmp esi,ebp) and stores the selected
// pointer NULL from that register, while our build materialises a second zero in EAX for both the test and the store. The exclusive
// bound (11), the count compared with the index (12), the count in a local (11), the while form (11), a 32-bit index (144), a seat-count
// test around the scan (135) and the count read before the guard (67, which also moves the whole prologue) all fail to give retail's
// registers; only a test that repeats the loop's own bound puts the count in ESI and the index in EBP, and that one is not kept.
// MiniMax E (11-04, not kept, m3 review: the 8 -> 6 rests on a seat-count test
// that repeats the loop's own bound, a redundant compare kept only with a byte
// match; main's loop restored): with that test the count and the index occupy
// ESI/EBP as retail and the loop test is cmp/jl; retail keeps the index's zero in
// EBP for that test and for the selected pointer's NULL store, ours materialises
// a second zero in EAX for both. The index declared before the test 6; the same
// with the count in a local 11, the test as count != 0 6, a
// while loop 6, a seats local 25, a pointer loop 139, the index inside the
// loop 135, the test as i < count 11.
// MiniMax G (11-10): no new form, no lower score (8); ten loop spellings (the bound exclusive or inclusive, the count in a local or read again, the seats array in a local, a while form, the index widened or narrowed) all give 11, 23 or 27, because every exclusive bound moves the count to EBP and the index to ESI where retail keeps the count in ESI and the index in EBP, and the inclusive bound keeps them but needs the decremented count and js/jle instead of retail's cmp/jle and cmp/jl.
// MiniMax R (11-10): no new form, no lower score (8); the body is unchanged, and the seat scan's inclusive
// bound still needs the decremented count and the separate zero that retail does not.
// MiniMax A (11-11): no new form, no lower score (8); the seat scan preamble is the whole difference: retail keeps the count in ESI and the index in EBP, tests the count against the index own zero register (cmp esi,ebp) and stores the selected pointers NULL from that register, while our build materialises a second zero in EAX for both the test and the store; every exclusive spelling (for, while, the count in a local, the index declared first, the count compared with the index) moves the count to EBP and the index to ESI (11 or 12), and a seat-count test around the scan keeps retail registers and the native cmp/jl but our build still materialises the EAX zero (6, not kept: the test repeats the loops own bound).
// P453: index-plus-one bound 39; position/forward/direction output objects
// at function scope 8/8/8, all three 8; earlier angle declaration 8;
// direction aggregate initialization 8; position/forward declarations
// exchanged 8. NOT kept: no gain; original scan and callee inputs retained.
// Round 240 (m3 lane 3, fast): the exclusive bound with 1..8 unused locals at the top 11 each (no numbering phase);
// a per-iteration element pointer with the exclusive/inclusive bound 11/8; the seat block typed as the tag array
// (`s_tag_array seats`, the element read from its address, 0x1ba090/0x1bb570 updated) exclusive/inclusive 11/8;
// the bit test as `if (!bit3) continue;` inclusive/exclusive 8/11. Every exclusive form keeps the count in EBP,
// the index in ESI and the zero web (guard, NULL store, first index) in EAX.
// @retail 0x1b9200
bool function_1b9200(long object_index, long prop_index)
{
	s_slot_object_view *object = object_get(object_index);
	s_vehicle_tag_view *tag = (s_vehicle_tag_view *)g_4e3b44[object->tag_index & 0xffff].bytes;
	bool result = false;

	if (prop_index != NONE && tag->unknown1f0 == 6 && object->parent_index == NONE)
	{
		result = true;
		if (!(tag->flagsbc & 1))
		{
			s_vehicle_seat_definition *seat = NULL;
			for (short i = 0; i <= tag->seat_count - 1; i++)
			{
				if (TEST_FIELD_BIT(tag->seats[i].flags.bit3))
					seat = &tag->seats[i];
			}
			if (seat)
			{
				s_prop_state_view *state = prop_node_state(prop_node_get(prop_index));
				point3f position;
				vector3f forward;

				function_b9dd0(object_index, &position);
				function_1b9201(object_index, &forward);
				forward.k = 0.0f;
				if (function_30bf0(&forward) > 0.0f)
				{
					vector3f direction;

					direction.i = state->position.x - position.x;
					direction.j = state->position.y - position.y;
					direction.k = 0.0f;
					if (function_30bf0(&direction) > g_45dbd8)
					{
						real angle = function_11cc90((vector2f *)&forward, (vector2f *)&direction);

						if (angle <= seat->unknown88 || angle > seat->unknown8c)
							result = false;
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1b9420
bool function_1b9420(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool volatile result = false;

	if (actor->unknown086 >= 3)
		return true;

	s_object_child_iterator iterator;

	function_d0620(&iterator, actor->unknown26c);
	if (function_d0690(&iterator))
	{
		do
		{
			if (object_get(iterator.child_index)->player_index != NONE)
				return true;
		} while (function_d0690(&iterator));
		return result;
	}
	return result;
}

// @retail 0x1b94c0
short __stdcall function_1b94c0(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (function_1b9420(actor_index) && function_1b2ff0(actor_index) > 0)
		result = 0x6d;
	return result;
}

// @retail 0x1b9500
short __stdcall function_1b9500(long actor_index, s_slot *slot)
{
	short result = g_46fbe4;

	if (function_1b9420(actor_index) && function_1bcc90(actor_index) > 0)
		result = 0x6b;
	return result;
}

/* the vehicle's entry in the actor's character tag says when the actor
   wants to be in it */
// @retail 0x1b9540
short __stdcall function_1b9540(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_object_view *vehicle = object_get(actor->unknown26c);
	s_tag_element *element = function_1e5450(actor_index, vehicle->tag_index);
	bool wanted = actor->unknown5d4;

	if (element && actor->prop_index != NONE)
	{
		if (wanted)
		{
			real elapsed = (real)(g_510c54->game_time - actor->unknown2f8) * g_510c54->rate;

			if (element->unknowna8 > elapsed)
			{
				wanted = true;
			}
			else if (elapsed > element->unknownac)
			{
				wanted = false;
			}
			else
			{
				s_character_weapon_1b9 *weapon = (s_character_weapon_1b9 *)function_1e5240(actor_index);

				if (weapon && prop_node_get(actor->prop_index)->unknown28 > weapon->unknown10)
					wanted = false;
			}
		}
		else if (vehicle->unknown100 > element->unknowna4)
		{
			wanted = true;
			actor->unknown2f8 = g_510c54->game_time;
		}
	}
	else
	{
		wanted = false;
	}
	actor->unknown44a = wanted;
	actor->unknown449 = wanted;
	return g_46fbe4;
}

// @retail 0x1b9fc0
short __stdcall function_1b9fc0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown858 == NONE)
	{
		s_slot_entry_iterator iterator;

		iterator.actor_index = actor_index;
		iterator.reference.unknown2 = 0x4c;
		iterator.reference.unknown0 = NONE;
		while (function_26f0c0(&iterator))
		{
			s_4c_element *element = (s_4c_element *)element_502424_get(actor->memory[iterator.reference.unknown0].unknown4);

			if (!element->unknown86)
			{
				if (actor->unknown26c != NONE)
				{
					if (actor->unknown266 && actor->unknown26c == element->object_index)
						result = 3;
				}
				else if (actor->unknown328 < 10)
				{
					result = 3;
				}
			}
		}
	}
	return result;
}

// @retail 0x1ba3f0
void __stdcall function_1ba3f0(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_slot_4c *state = (s_slot_4c *)slot;

	if (state->unknown1c != NONE && state->seat_index != NONE)
		object_seat_unreserve(object_get(state->unknown1c), state->seat_index);

	s_4c_element *element = (s_4c_element *)list;

	if (element->object_index != NONE && element->unknown7c == 1)
	{
		short count = 0;
		s_object_seat seats[0x40];

		function_c8a40(element->object_index, seats, &count, 0x40);
		if (count > 0)
		{
			s_object_seat *seat = seats;
			long local_0 = (word)count;
			do
			{
				s_object_header_view *header = object_header_get(seat->object_index);

				if (header->type == 1)
					object_seat_unreserve((s_slot_object_view *)header->object, seat->seat_index);
				seat++;
			}
			while (--local_0);
		}
	}
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 6 (was 6): retail loads the slot pointer into ECX before the actor
// index is masked, ours loads it after the actor lookup and after the branch
// that tests the actor, so the two builds are one instruction out of step.
// This round: slot data block 29, without barrier 50, actor-first 31;
// const actor 6; const state without barrier 44; data block/const actor
// without barrier 50; const data state 29; actor object reference 6.
// Round 23: the slot read once through its address before or after the
// actor lookup 6/6, the same without the barrier 44/44; the barrier before
// the actor lookup 44, at the head of the NONE block 43, first 44.
// Round 56 (lane 9, one-file LTCG harness, exact here): actor, barrier, then the state 6; no state local
// (the slot cast at each use) 6, without the barrier 41; the point copied through a pointer to it 6;
// the state declared `register` (as 0x1b8070's) 6, without the barrier 41, actor first without it 41.
// Without the barrier the only causes are the slot load (retail: entry block) and, from it, our register
// compare of +0x26c where retail compares memory (ECX is busy there).
// Round 54 (m3 lane 11, fast): without the barrier, `byte flags` read at the top 44, `point3f const *point` at the
// top 44, an early return for a busy actor 46 (212 bytes), the NONE test in a named bool 44; the state after the
// actor with the barrier after both 6. Matched retail callbacks load a parameter in the entry block only when two
// sibling paths use it (0x1b22e0, 0x29f370, 0xa9120).
// Round 125: pointer names reversed 6; split state initialization 6; split actor initialization 6;
// actor declared first 6; outer comparison reversed 6; threshold written > 6 gives 8; memcpy point 6;
// const slot parameter 6; final store before the outer test 52 (189 bytes).
// Round 34 (lane 17, fast): an early return for a busy actor with the barrier 14 (209 bytes); the
// state declared inside the NONE block 6; the flags block's stores 0x41c first 6; flags read into a
// byte before the barrier 38; the busy field in a named long 6. Without the barrier (re-measured) 44.
// Round 169 (lane 4, fast): without the barrier the only change is the 0x26c test loaded into ECX ahead of the
// add (208 bytes), so the slot load is the one cause. The flags test inverted with its arms swapped 38; the slot
// read through its address, then the barrier, then the actor 44; the flags through a seat-selection pointer at
// slot+0x1c formed at the top (0x1ba5c0's view), no barrier, 44; the 0x86 arm with its own final store and
// return 14; the 0x86 test inverted (`< 7` arm first) 22; a 600 s fast annealing permuter run (its reduced image
// scores the original 6): 53 tries, none below 6.
// Round 85 (fast; a compile-only screen reproduces this function exactly): slot used in both successors of the
// NONE test (diagnostic, an extra store in an else) 52 (240 bytes), the load is still not hoisted, unlike
// 0x1b22e0 where the use sits in a join block; the existing seat-selection view for the flags test 6; the point
// copied field by field 33; the flags byte read into a local inside the NONE block 6; the 0x450 store written in
// every arm 6; the early return for a busy actor 14 (repeat). Screened only: an `idle` bool tested in each arm,
// the NONE test repeated in an && chain, flags tested before the NONE test: none loads the slot at entry.
// Round 94 (lane 16, fast): `__assume(state != NULL)` after the state with the barrier 6, without it 44; `__assume(actor
// != NULL)` in place of the barrier 44; the state after the actor with `__assume(state != NULL)` in place of the barrier
// 44; a point pointer at the top with the barrier 6. Diagnostics: a real slot read in the join block after the outer if
// loads the slot in the entry block, but after the 0x26c compare (just before the jne), with or without the barrier
// (retail loads it before the mask); an extra read inside the flags arm does not; an `__assume` at the join does not.
// Round 60 (m3 lane 2, fast): `s_slot_4c *const state` 6; the slot parameter `s_slot *const` 6, without the barrier
// 44; the flags test as `(state->flags & 4) != 0` 6; the point copied through a const reference in the arm 6 (all 6s
// the same code). `register` on the slot parameter does not build (the generated cast rejects it).
// Round 145 (m3 lane 7, fast; a compile-only build of the file reproduces this function exactly): the state as
// `slot ? (s_slot_4c *)slot : NULL` 6; the point through a const reference at the top 6; the flags test as
// `(bool)((state->flags >> 2) & 1)` 6; the body in an inline helper called with (state, actor) and the barrier
// before the call 44 (208 bytes); with (actor, state) parameters and the barrier at the helper's top 6. Compile-only
// diagnostics: a second definition of the state (in an else) loads the slot at entry; a read in the join block
// loads it after the compare; helper forms without the barrier compare in ECX like the body without it.
// Round 172 (m3 lane 6, fast; diagnostics only, reverted: a volatile cast is not keepable): the slot read once through
// a volatile view of the parameter first, without the barrier 50 (210 bytes), with it 29; after the actor lookup
// without the barrier 49 (210), then the barrier 31 (207); the barrier, then the read 29. Each loads the slot in the
// entry block, but the data base then takes ESI (with a `push esi` at entry) where retail keeps it in EDX until the
// constant 4. No untried non-volatile form that touches the slot's load or the entry block is left in this note.
// Round 94 (m3 lane 11, fast): the state as the data block address minus the header size (zero offset through a
// member), no barrier 44 (208 bytes); a reference-typed state, no barrier 44; `__assume` on a slot field at the top,
// no barrier 44, with it 6; the state through a view wrapper with an inline converting constructor 83 (237 bytes),
// with the barrier 80 (234); the outer test inverted with an empty then-arm 6 (same code). Re-measured: data block
// with the actor first and the barrier 31 (207), without 49 (210); state first with the barrier 29. Standalone
// compile-only probes of the same shape: a local aggregate or constructor-built context holding both pointers, a
// derived-type or first-member cast, a dead local store, an empty inline call taking the slot, `__noop`, an empty asm
// block: none loads the parameter before its first use or stops the 0x26c load being formed from base+index ahead of
// the add; only a member at a non-zero offset (the data block) or the barrier does. Readings: retail's 0x26c compare
// stays in memory because ECX already holds the slot and EDX the base when the add is made; the data-block forms
// instead keep the early load and move the base to ESI (pushed at entry), so the cause is a real entry-block use of
// the slot ahead of the compare, not the block pointer.
// Round 240 (m3 lane 3, fast): the parameter typed as the 0x4c slot view in the definition (the table entry cast) 6,
// the same without the barrier 44; the 0x86/0x50c tests nested in an else block 6; the flags arm's point copy before
// the 0x420/0x41c stores 40, its 0x44d store before the copy 14. Diagnostics (reverted): without the barrier, one or
// three more slot reads in the flags arm 44/111, the slot is still loaded after the branch.
// @retail 0x1ba8c0
void __stdcall function_1ba8c0(long actor_index, s_slot *slot, s_slot_target_list *list)
{
	s_slot_4c *state = (s_slot_4c *)slot;
	s_actor_view *actor = actor_get(actor_index);
	_ReadWriteBarrier();

	if (actor->unknown26c == NONE)
	{
		if (state->flags & 4)
		{
			actor->unknown420 = 4;
			actor->unknown41c = 3;
			actor->unknown424.point = state->point;
			actor->unknown44d = true;
		}
		else if (actor->unknown086 >= 7)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 2;
			actor->unknown488 = true;
		}
		else if (actor->unknown50c)
		{
			actor->unknown41c = 2;
			actor->unknown420 = 0;
		}
	}
	actor->unknown450 = 0x6000086;
}
#pragma function(_ReadWriteBarrier)

/* the unit fields function_1ba990 reads */
struct s_unit_1ba990
{
	byte unknown000[0x84];
	real unknown084;
	vector3f velocity;
	byte unknown094[0xec - 0x94];
	real unknownec;
	byte unknownf0[0x10a - 0xf0];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word unknown10a_3 : 13;
};

/* unknown_0259d0's distance_sq3f (0x24550), inlined */
static inline real distance_squared3d_1ba990(point3f const *a, point3f const *b)
{
	vector3f v;
	v.i = b->x - a->x;
	v.j = b->y - a->y;
	v.k = b->z - a->z;
	real sum = v.k * v.k;
	sum += v.i * v.i;
	sum += v.j * v.j;
	return sum;
}

/* whether the unit is close enough to the actor (and slow enough) for it */
// @retail 0x1ba990
bool function_1ba990(long actor_index, long unit_index, bool force, real near_radius, real far_radius, bool use_near_radius)
{
	s_actor_view *actor = actor_get(actor_index);
	s_unit_1ba990 *unit = (s_unit_1ba990 *)object_get(unit_index);
	bool result = false;

	if (TEST_FIELD_BIT(unit->flag10a_2))
	{
		result = false;
	}
	else if (force)
	{
		result = true;
	}
	else if (0.1f > unit->unknownec)
	{
		result = false;
	}
	else
	{
		real radius = use_near_radius ? near_radius : far_radius;
		point3f position;

		function_b9dd0(unit_index, &position);
		if (distance_squared3d_1ba990(&actor->position, &position) < radius * radius)
		{
			result = true;
			if (!use_near_radius && length_sq3f(&unit->velocity) > 0.25f)
				result = false;
		}
	}
	if (0.5f > unit->unknown084)
		return false;
	return result;
}

__declspec(noinline) bool function_f5dc0(long object_index);
long function_25d810(long object_index, long actor_index, bool create);
bool __stdcall function_25c230(long actor_index, long prop_ref_index, short unknown);

/* a player as slot test 0x4d sees it */
struct s_player_unit_view
{
	byte unknown00[0x2c];
	long unit_index;
};

short actor_behavior_find_level(long owner_index, short type);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* slot test 0x4d: the actor boards the vehicle of a friendly player near
   enough to it */
struct s_1b99d0_unit_flags
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word remaining : 13;
};

struct s_1b99d0_tag_flags
{
	dword lower : 18;
	dword bit18 : 1;
	dword upper : 13;
};

struct s_node_view;
void function_26be00(long actor_index, s_iterator *iterator);
s_node_view *function_26be30(s_iterator *iterator);
bool function_26c120(long actor_index);
long function_1e4a50(long character_index);
void __stdcall function_26c2d0(long prop_index);
void function_270590(s_path_source *source, s_type_c3b527 const *point, long node_index);

// @retail 0x1b99d0
short __stdcall function_1b99d0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;
	if (!actor->unknown224 && !actor->unknown225 && actor->unknown040 && actor->unknown858 == NONE &&
		function_26c120(actor_index) && actor->prop_index != NONE)
	{
		long selected = NONE;
		real best_score = 10.0f;
		s_prop_node_view *base = prop_node_get(actor->prop_index);
		real range_low;
		real range_high;
		byte *properties = (byte *)function_1e4a50(actor->unknown054);
		s_iterator iterator;
		function_26be00(actor_index, &iterator);
		function_26c180(actor_index);
		if (properties && actor->unknown27c.unknown10 != NONE && actor->unknown278[0])
		{
			byte *buffer = ai_scratch_buffer_get();
			bool prepared = false;
			s_prop_node_view *node = (s_prop_node_view *)function_26be30(&iterator);
			while (node)
			{
				if (node->type == 4 && node->unknown24 >= 1 && node->unknown24 <= 2)
				{
					s_slot_object_view *vehicle = object_get(node->object_index);
					s_vehicle_tag_view *tag = (s_vehicle_tag_view *)g_4e3b44[vehicle->tag_index & 0xffff].bytes;
					if (!TEST_FIELD_BIT(((s_1b99d0_unit_flags *)((byte *)vehicle + 0x10a))->bit2) &&
						!TEST_FIELD_BIT(((s_1b99d0_tag_flags *)((byte *)tag + 0x1ec))->bit18) &&
						vehicle->unknown3a0 != NONE && function_1b8f30(actor_index, node->object_index))
					{
						if (tag->unknown1f0 != 6 || function_1b9200(node->object_index, actor->prop_index))
						{
							real ownership_weight = vehicle->unknown3a0 == actor->unknown030 ? 0.75f : 1.2f;
							real type_weight = tag->unknown1f0 == 6 ? 1.5f : 1.0f;
							real state_weight = 1.0f;
							if (actor->unknown086 >= 8)
								state_weight = 1.5f;
							else if (actor->unknown086 < 6)
								state_weight = 0.75f;
							real score = node->unknown28 * state_weight * type_weight * ownership_weight;
							if (score < best_score)
							{
								s_prop_state_view *state = prop_node_state(node);
								function_26c2d0(iterator.index);
								if (*(long *)((byte *)state + 0x44) != NONE)
								{
									if (!prepared)
									{
										s_path_source source;
										s_path_settings settings;
										memset(&source, 0, sizeof(source));
										source.radius = *(real *)(properties + 4);
										source.unknown04 = 1;
										source.object_index = NONE;
										source.unknown0c = NONE;
										source.unknown4c = 0.0f;
										function_270590(&source, &actor->unknown27c.point, actor->unknown27c.unknown10);
										source.unknown45 = true;
										source.unknown48 = 15.0f;
										source.unknown28[0] = 1;
										s_prop_state_view *base_state = prop_node_state(base);
										*(point3f *)((byte *)&source + 0x2c) = base_state->position;
										*(real *)((byte *)&source + 0x3c) = 1.0f;
										*(long *)((byte *)&source + 0x38) = base->object_index;
										*(real *)((byte *)&source + 0x40) = 0.0f;
										source.unknown44 = false;
										function_1f9240(actor_index, &settings);
										function_271300((s_type_f17a25 *)buffer, NULL, &settings, &source, 0);
										function_2715a0(buffer);
										prepared = true;
									}
									real closest_distance;
									real path_distance;
									function_270750(buffer, *(long *)((byte *)state + 0x44), &state->unknown48,
										&path_distance, (long)&closest_distance, 0);
									if (path_distance < 15.0f && closest_distance > base->unknown28 * 0.5f)
									{
										best_score = score;
										selected = node->object_index;
										range_low = ownership_weight * 13.0f;
										range_high = ownership_weight * 16.0f;
									}
								}
							}
						}
					}
				}
				node = (s_prop_node_view *)function_26be30(&iterator);
			}
			if (selected != NONE)
			{
				if (function_f5dc0(selected))
				{
					byte *output = (byte *)slot;
					*(word *)(output + 0x1c) |= 1;
					*(long *)(output + 0x20) = selected;
					result = 0x5d;
				}
				else
				{
					s_slot_4c *output = (s_slot_4c *)slot;
					output->unknown28 = range_low;
					output->seat_index = NONE;
					output->unknown2c = range_high;
					output->unknown1c = selected;
					output->flags |= 0x29;
					result = 0x4c;
				}
			}
			ai_scratch_buffer_release(buffer);
		}
	}
	return result;
}


// @retail 0x1b9640
short __stdcall function_1b9640(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (!actor->unknown224 && !actor->unknown225 && actor->unknown858 == NONE)
	{
		long vehicle_index = NONE;
		real near_radius = FLT_MAX;
		real far_radius = FLT_MAX;
		s_data_datum_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while (data_datum_iterator_next(&iterator))
		{
			s_player_unit_view *player = (s_player_unit_view *)iterator.datum;

			if (player->unit_index != NONE)
			{
				s_slot_object_view *unit = object_get(player->unit_index);

				if (unit->unknown1fc != NONE && !function_1df560(unit->team, actor->unknown024))
				{
					long outermost_index = function_1b8c80(unit->parent_index);
					s_slot_object_view *vehicle = (s_slot_object_view *)function_badc0(outermost_index, 2);

					if (vehicle && function_1b8f30(actor_index, outermost_index))
					{
						real scale = vehicle->unknown3a0 == actor->unknown030 ? 2.0f : 1.0f;
						real near_distance = 10.0f;
						real far_distance = 12.0f;

						if (function_1ba990(actor_index, outermost_index, false, scale * near_distance, scale * far_distance, false))
						{
							vehicle_index = outermost_index;
							near_radius = near_distance;
							far_radius = far_distance;
						}
					}
				}
			}
		}

		if (vehicle_index != NONE)
		{
			long prop_index = function_25d810(vehicle_index, actor_index, true);
			short slot_index;

			if (prop_index != NONE && prop_node_get(prop_index)->unknown24 < 1)
				function_25c230(actor_index, prop_index, 3);
			slot_index = actor_behavior_find_level(actor_index, 0x4c);
			bool already_assigned = slot_index != NONE && function_1b8c80(((s_slot_4c *)&actor->slots[slot_index])->unknown1c) == vehicle_index;
			if (!already_assigned)
			{
				s_slot_4c *state = (s_slot_4c *)slot;

				state->unknown1c = vehicle_index;
				state->unknown28 = near_radius;
				state->seat_index = NONE;
				state->unknown2c = far_radius;
				state->flags |= 0x29;
				result = 0x4c;
			}
		}
	}
	return result;
}

/* slot test 0x4e: the actor goes for the vehicle it was told to use */
// @retail 0x1b9890
short __stdcall function_1b9890(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (!actor->unknown224 && !actor->unknown225 && actor->unknown858 == NONE)
	{
		long vehicle_index = actor->unknown2e8;

		if (vehicle_index != NONE && actor->unknown2f0 && !function_f5dc0(vehicle_index) &&
			object_get(vehicle_index)->unknown34f > 0 &&
			function_1ba990(actor_index, vehicle_index, false, 20.0f, 24.0f, false))
		{
			s_slot_4c *state = (s_slot_4c *)slot;
			long prop_index = function_25d810(actor->unknown2e8, actor_index, true);

			if (prop_index != NONE && prop_node_get(prop_index)->unknown24 < 1)
				function_25c230(actor_index, prop_index, 3);
			long updated_vehicle = actor->unknown2e8;
			state->unknown28 = 20.0f;
			state->seat_index = NONE;
			state->unknown2c = 24.0f;
			state->unknown1c = updated_vehicle;
			state->flags |= 0x29;
			result = 0x4c;
		}
	}
	return result;
}

/* invites the clump members to the joint, the nearest first, when the
   object has more than one seat to take */
// @retail 0x1bb3a0
short __stdcall function_1bb3a0(long actor_index, long joint_index, long a, long b)
{
	long count = 0;
	short free_count = 0;
	s_actor_view *actor = actor_get(actor_index);
	s_4c_element *element = (s_4c_element *)element_502424_get(joint_index);
	point3f position;

	{
		short seat_count = 0;
		s_object_seat seats[0x40];

		function_c8a40(element->object_index, seats, &seat_count, 0x40);
		for (short i = 0; i < seat_count; i++)
		{
			if (!TEST_FIELD_BIT(seats[i].definition->flags.bit11))
				free_count++;
		}
	}
	function_b9dd0(element->object_index, &position);
	if (free_count > 1 && actor->unknown07c != NONE)
	{
		s_element_502420_member_iterator iterator;
		s_actor_view *other;

		iterator.next_index = element_502420_get(actor->unknown07c)->first_actor_index;
		iterator.index = NONE;
		while ((other = element_502420_member_next(&iterator)) != 0)
		{
			long other_index = iterator.index;

			if (actor != other &&
				(other->unknown26c == NONE || other->unknown26c == element->object_index && other->unknown266))
			{
				vector3f delta;

				vector3d_from_points3d(&position, &other->position, &delta);
				if (function_26eae0(joint_index, other_index, 3, (real)(1.0 / (length_sq3f(&delta) + 0.1f))))
					count++;
			}
		}
	}
	return (short)count;
}

// @retail 0x1bb530
void __stdcall function_1bb530(long actor_index, s_slot *slot, long index)
{
	s_slot_4c *state = (s_slot_4c *)slot;
	s_502424_element *element = element_502424_get(state->element_index);

	if (state->unknown1c == index || element->target.unknown0 == index)
	{
		state->unknown1c = NONE;
		element->target.unknown0 = NONE;
	}
}

s_slot_handler_0 g_47e954 =
{
	0x5f, 0, 0xbff, -2, 0, function_1b94c0
};

s_slot_handler_0 g_47e968 =
{
	0x60, 0, 0xbff, -2, 0, function_1b9500
};

s_slot_handler_0 g_47e97c =
{
	0x5e, 0, 0, -2, 0, function_1b9540
};

s_slot_handler_0 g_47e990 =
{
	0x4d, 0, 0, -2, 0, function_1b9640
};

s_slot_handler_0 g_47e9a4 =
{
	0x4e, 0, 0, -2, 0, function_1b9890
};

s_slot_handler_0 g_47e9b8 =
{
	0x4f, 0, 0, -2, 0, function_1b99d0
};

s_slot_handler_2x g_47e9d0 =
{
	{
		{
			0x4c, 2, 0, -2, 0,
			function_1b9fc0, function_1ba4e0, joint_initiate, joint_leave, 1, {0},
			0, 0, function_1bb530, 0, 0, 0, 1
		},
		(t_slot_proc)joint_update, joint_activate, joint_deactivate
	},
	(t_slot_proc)function_1ba090, (t_slot_release)function_1ba3f0, (t_slot_release)function_1ba5c0, slot_release_nothing, function_1ba8c0, (t_slot_proc4)function_1bb3a0,
	1, 10, 1.5f, 0x5b
};

/* slot type 0x4c's evaluate: fails when the element or its target is gone or ten seconds
   have passed; an actor in a vehicle finishes, unless the element has more than one member
   and the actor's +0x266 flag is set */
// @retail 0x1ba4e0
short __stdcall function_1ba4e0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);
	s_slot_4c *state = (s_slot_4c *)slot;
	s_4c_element *element = (s_4c_element *)element_502424_get(state->element_index);

	state->unknown24++;
	if (element->object_index == NONE || state->unknown1c == NONE ||
		state->unknown24 > real_to_long(g_510c54->field_2_3 * 10.0f))
	{
		result = g_46fbe4;
	}
	else if (actor->unknown26c != NONE)
	{
		result = g_46fbe4;
		if (element->unknown7c > 1)
		{
			if (actor->unknown266)
				result = g_46fbe8;
			if (!element->unknown84)
				function_1fb7e0(0x39, actor_index, NULL, NONE, NONE);
		}
	}
	return result;
}

long __stdcall function_c7160(long unit_index, short seat_index, long vehicle_index, long marker_position, long position);
bool function_cc750(long unit_index, short seat_index);
bool function_cc7b0(long unit_index, short seat_index);
real normalize2d(point2f *vector);

/* the distance between two points on the ground plane */
static inline real distance2d(point2f const *a, point2f const *b)
{
	point2f offset;

	vector2d_from_points2d(b, a, &offset);
	return magnitude2d((vector2f const *)&offset);
}

struct s_seat_approach_result
{
	point3f point;
	vector3f direction;
	real score;
	bool close;
	bool facing;
	bool approaching;
};

/* whether the actor can take the seat, with the seat's entry point, the
   approach direction and a score for choosing between seats */
// @retail 0x1baae0
bool function_1baae0(long actor_index, long object_index, short seat_index, bool ignore_bonus,
	bool ignore_reserved, bool vertical, s_seat_approach_result *out)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	if (function_1b8eb0(actor_index, object_index, seat_index, ignore_reserved))
	{
		point3f marker;
		point3f position;
		if (function_c7160(actor->unknown018, seat_index, object_index, (long)&marker, (long)&position) != NONE)
		{
			real bonus = 0.0f;
			vector3f direction;
			direction.i = marker.x - position.x;
			direction.j = marker.y - position.y;
			direction.k = 0.0f;
			if (normalize2d((point2f *)&direction) == 0.0f)
				direction = actor->unknown290;
			real distance;
			if (vertical)
			{
				vector3f offset;
				vector3d_from_points3d(&actor->position, &position, &offset);
				distance = sqrtf(length_sq3f(&offset));
			}
			else
			{
				distance = distance2d((point2f const *)&position, (point2f const *)&actor->position) >
					distance2d((point2f const *)&marker, (point2f const *)&actor->position) ?
					distance2d((point2f const *)&marker, (point2f const *)&actor->position) :
					distance2d((point2f const *)&position, (point2f const *)&actor->position);
			}
			s_slot_object_view *object = object_get(object_index);
			if (object->type == 1 && !ignore_bonus && function_cc750(object_index, seat_index) &&
					TEST_FIELD_BIT(((s_vehicle_tag_view *)g_4e3b44[object->tag_index & 0xffff].bytes)->flags1ec.bit11))
				bonus = 50.0f;
			real dot = actor->unknown290.i * direction.i + actor->unknown290.j * direction.j;
			bool close = distance < 0.4f;
			bool facing = dot > 0.6f;
			bool approaching = distance < 1.1f && dot > 0.0f;
			real score = 10.0f / (distance + 1.0f) + bonus;
			if (function_cc7b0(object_index, seat_index))
				score += 200.0f;
			if (out)
			{
				out->point = position;
				out->direction = direction;
				out->score = score;
				out->close = close;
				out->facing = facing;
				out->approaching = approaching;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x1bb1d0
short function_1bb1d0(long actor_index, long object_index, s_object_seat const *seats,
    short count, bool reject_single, long *chosen_object, s_seat_approach_result *out)
{
    s_actor_view *actor = actor_get(actor_index);
    short result = NONE;
    if (actor->unknown26c != NONE)
    {
        if (actor->unknown26c == object_index && actor->unknown018 != NONE)
        {
            s_slot_object_view *unit = object_get(actor->unknown018);
            if (chosen_object)
                *chosen_object = unit->parent_index;
            result = unit->unknown1fc;
            if (out)
            {
                out->facing = true;
                out->approaching = true;
                out->direction = actor->unknown290;
                out->point = actor->position;
                out->score = 1.0f;
                out->close = true;
            }
        }
    }
    else
    {
        long best_object = NONE;
        real best_score = 0.0f;
        short eligible = 0;
        s_seat_approach_result best;
        for (short i = 0; i < count; i++)
        {
            s_object_seat const *seat = &seats[i];
            if (!TEST_FIELD_BIT(seat->definition->flags.bit11))
            {
                s_seat_approach_result approach;
                short seat_index = seat->seat_index;
                if (function_1baae0(actor_index, seat->object_index, seat_index, false, false, false, &approach))
                {
                    eligible++;
                    if (approach.score > best_score)
                    {
                        best_object = seat->object_index;
                        result = (short)seat_index;
                        best = approach;
                        best_score = approach.score;
                    }
                }
            }
        }
        if (reject_single && eligible == 1)
            result = NONE;
        else if (result != NONE)
        {
            if (chosen_object)
                *chosen_object = best_object;
            if (out)
                *out = best;
        }
    }
    return result;
}

struct s_seat_selection
{
    long object_index;
    short seat_index;
    byte flags;
    byte unknown7;
    short unknown8;
    byte unknowna[2];
    real near_radius;
    real far_radius;
};

bool function_2676d0(long actor_index);
bool function_1e2030(long actor_index);
short __stdcall function_c8b80(long object_index, s_object_seat *seats, short maximum_count);

// @retail 0x1b9e70
bool function_1b9e70(long actor_index, s_seat_selection *selection, bool reserved,
    s_4c_element const *element, s_seat_approach_result *out)
{
    bool result = false;
    bool search = false;
    if (selection->seat_index != NONE)
    {
        if (function_1baae0(actor_index, selection->object_index, selection->seat_index, false, !reserved, false, out))
            result = true;
        else
        {
            long seat_index = selection->seat_index;
            object_seat_unreserve(object_get(selection->object_index), seat_index);
            search = ((selection->flags >> 3) & 1) != 0;
            selection->seat_index = NONE;
            selection->object_index = NONE;
        }
    }
    else
        search = true;
    if (selection->seat_index == NONE && search)
    {
        s_object_seat seats[0x40];
        short count = function_c8b80(element->object_index, seats, 0x40);
        long object_index;
        bool reject_single = element->unknown85 && (!function_2676d0(actor_index) || !function_1e2030(actor_index));
        selection->seat_index = function_1bb1d0(actor_index, element->object_index, seats, count,
            reject_single, &object_index, out);
        if (selection->seat_index != NONE && object_index != NONE)
        {
            selection->object_index = object_index;
            return true;
        }
    }
    return result;
}


long function_baf40(long object_index);
bool __stdcall function_f5d10(long vehicle_index, long bit, bool set);
void function_26ee40(s_joint_behavior_state *behavior);
void function_1f86a0(long actor_index);

// MiniMax A (11-13): no new form, no lower score (134); what still differs: the frame is 0x10 here and 0x14 in
// retail (retail spills the 0x20 mask byte to the stack where this build keeps it in bl), the actor index stays in
// ecx where retail keeps it in edx, and retail keeps the iterator result in ebx and pushes it where this build keeps
// it in eax; the memory summary of 0xbaf40 (src/unknown_0bad50.cpp) decides the register split around the two calls
// in the loop: BLOCKED-BY 0xbaf40. The permuter's only lower form (122) swaps joint_decline's two arguments, and
// the callee reads its first parameter as the actor index (retail puts the actor index in EAX at that call), so the
// swap is not kept.
// score 134 (was 134): 861 bytes as retail; frame 0x10 (retail 0x14): retail spills the 0x20 mask byte to
// F+3 and keeps the loop entry in EBX (home F+4, reloaded after the root calls), ours keeps the mask in BL
// and the entry in memory (one more reload before the joint index shifts the tail); retail also takes the
// object-header data in the prologue and reads the flags once above the slot-flag test.
// MiniMax C (11-10), at 132 (was 132): no lower score this round. The frame is still 0x10 against
// retail's 0x14, and the mask's home is the whole difference: retail keeps the 0x20 mask byte in the
// stack at F-0x13 and the loop entry in EBX, this build keeps the mask in BL and spills the entry, so
// every stack offset after it shifts by four. Fifteen forms of the mask and of the comparison were
// tried and none of them puts the mask in memory: the else arm re-reading the flags 244, the mask as
// a bool 241, the comparison as a ternary 228, the comparison inlined without the mask 250, a second
// mask assignment under an if 255, the mask as a dword 248, as an int 248, as a word 253, the
// comparison with an extra and 241, and the mask as an unsigned char, the comparison with a
// not-equal test, the comparison as a byte, a separate copy of the flags byte and the mask computed in
// two steps at 132 each. Four more shapes of the same value were measured since: the same value under
// an if 240, the same value as a ternary 228, the decline test as a separate boolean local 132 and the
// same value under an if with braces 240; the frame stays 0x10 and the mask stays in BL in all of them.
// MiniMax C (11-10), at 132 (was 133): the joint index is held in a dword and the second test against NONE
// compares it as a signed value, which is the pair that lowers the score (the dword alone and the cast alone
// each score 133); the frame is still 0x10 against retail's 0x14, retail still spills the 0x20 mask byte to
// F+3 and keeps the loop entry in EBX where ours keeps the mask in BL and the entry in memory, and retail
// still takes the object table base in the prologue, into ESI, where ours loads it into EAX after the actor
// index math.
// MiniMax C (11-10), at 133 (was 134): the element index is stored before the slot flag, which pairs the
// two stores the way retail orders them; the frame is still 0x10 against retail's 0x14, retail still spills
// the 0x20 mask byte to F+3 and keeps the loop entry in EBX where ours keeps the mask in BL and the entry in
// memory, and retail still takes the object table base in the prologue, into ESI, where ours loads it into
// EAX after the actor index math.
// MiniMax C (11-10), at 134 (was 134): the iterator loop is the for form of the matched sibling
// 0x1b4680, which gives the same score and the exact length of 861 bytes, with the first difference at
// +0x2 (the frame) instead of +0x4; retail still spills the 0x20 mask byte to F+3 and keeps the loop entry
// in EBX where ours keeps the mask in BL and the entry in memory.
// MiniMax I (11-08): no new form, no lower score (134).
// MiniMax C (11-06): no new form, no lower score; the frame is still 0x10 against retail's 0x14,
// retail still spills the 0x20 mask byte to F+3 and keeps the loop entry in EBX (home F+4, reloaded
// after the root calls) where ours keeps the mask in BL and the entry in memory, and retail still
// takes the object-header data in the prologue and reads the flags once above the slot-flag test;
// the mask read once before the loop scores 136, so the mask is read per iteration as it was.
// Earlier first line (round 103 unchanged): 861 bytes as retail; frame 0x10 (retail 0x14): retail spills the 0x20 mask byte to
// Earlier at 134 (was 134; round 103 unchanged): 861 bytes as retail; frame 0x10 (retail 0x14): retail spills the 0x20 mask byte to
// F+3 and keeps the loop entry in EBX (home F+4, reloaded after the root calls), ours keeps the mask in BL
// and the entry in memory (one more reload before the joint index shifts the tail); retail also takes the
// object-header data in the prologue and reads the flags once above the slot-flag test.
// MiniMax G (11-05): the element index, the flags and the same-object result read as wider or narrower types give 134/134/244/248/253, and the mask as a bool 241; retail's EBX entry and spilled mask byte against our BL mask and memory entry, and the frame 0x10 against 0x14, stay unmatched; retail loads the object-table global at +0x06 where ours loads it at +0x0f, and its decline block falls through into the loop latch where ours jumps to a latch at the end of the body, but the header lookup written out is the same code, a nested actor_get gives 230 and an explicit continue in the decline path is the same code; no new form, no lower score.
// Before round 62: the frame 0x14 agrees; retail takes the object-header data before scaling
// the actor (actor index in EDX, ours ECX), keeps the loop entry in EBX (spilled at F+4), spills the
// 0x20 mask byte to F+3 and re-reads the flags for the 0x40 test; ours keeps the entry in memory
// (iterator struct member) and the flags or the mask in BL. Earlier headers, merged: the frame 0x14
// still agrees; the object-header data is still acquired after the actor is scaled where retail
// takes it before, and the eligibility mask, the entry and the result keep their homes; unchanged
// that round (198); this round's trials in this file found no lower form of that order (198);
// frame 0x14 agrees; early object-header acquisition, incoming actor, mask/entry/result carriers
// and reusable homes remain (198).
// Earlier: frame 0x14 agrees; incoming actor ECX/EDX, early object-header
// acquisition, selection-mask storage and iterator/result homes differ.
// Earlier slot-data form 198; raw-mask and independent iterator variants 202/207;
// Result-first 190; explicit same-object result 230; cleanup result 258.
// Complete byte flags with/without slot data 249/249; raw word/dword mask 257/252;
// branch-assigned decline with byte/word/dword 249/257/252, with slot data 249.
// Original body restored.
// frame 0x14 agrees; early object-header acquisition,
// selection-mask, entry/result and reusable frame homes still differ.
// Round 172: data-member with header/actor/both observations 198/263/263;
// actor/header observations alone 263/198; single cleanup-result join 258;
// full-width same result 201; full-width specified with data/actor 288.
// Original body, iterator record and every type restored.
// the 0x14 frame agrees; retail acquires object-header data before
// scaling the actor, ours loads it later. The eligibility mask, entry/result
// carriers and reusable stack homes still differ.
// Typed slot payload 198; payload/native parent field 198; payload/raw mask 207, with parent 207;
// payload/separate entry and iterator 202, with parent 202; separate entry/raw mask 212, all 212.
// Round 270: positive admission with bool/long/byte carriers 194/194/194;
// slot data member plus positive admission 194; long request code 198;
// default-owned request code 215; positive admission with long code 194;
// positive admission, data member and default-owned code 211. Original restored.
// Round 272: native header getter and parent field with payload/iterator/result
// lifetime subsets 198/198/202/202/198/198/202/202. Original body and types restored.
// Round 187: consistent selection-field observations with member payload, result/iterator names
// and combinations, eight at 249 (frame 0x18, 874 bytes); actual iterator aggregate construction
// with member payload, ordinary returned-entry for loop, byte bit fields or all three, five at 260
// (885 bytes). Indexed member-data form tried first on both new graphs; original body/types restored.
// Round 3 (lane 18): the two radii stored and read through the selection (retail's [ebp+0xc]/[ebp+0x10];
// 0x1ba5c0 reads them at selection - 0x1c + 0x28) give retail's tail: joint index in EBX, the slot reread
// from its argument, `mov eax, ebx` at the exit, 861 bytes, but frame 0x10: 239. With a separate entry and
// byte mask 238, the mask at function scope 238; positive admission (retail's decline-first block order)
// 240, with a found-entry pointer after the loop 240, entry or mask declared first 240/240; the 0x20 test
// written at both uses with no local 241 (frame 0x14 and the masked byte spilled to [esp+0x13] as retail,
// but the flags byte then stays in BL for the 0x40 test), 0x40 as a shifted bit 241, 0x40 through the slot
// view 229, both 0x20 tests through the slot view 238. Separate entry and byte mask alone 212. Open: the
// mask spilled while the entry pointer takes EBX (ours keeps the mask or flags in BL and the entry in memory).
// Round 203: one/two named root results with native/original payload and
// normalized/raw selection mask 235/235/235/235/234/234/234/234. The indexed
// payload form was first on each graph; controls survive. All are exact-match-
// only call-result/assigned-flag forms; original body and layouts restored.
// Round 22 (lane 13; scores positional, "al" = a difflib-aligned count, baseline al 91): the radii
// read/stored through the selection (struct grown to +0x14), a plain entry local and the 0x20 test
// at both uses 237 (861 bytes, al 82, retail's F+3/F+4/F+8 homes); plus the 0x40 test through the
// slot view 225 (frame 0x10, mask in BL); bit 6 as a bitfield 237; for-loop iterator 237; the
// fallback lookup in its own local then assigned (0x1bead0's form) 239 (al 75, retail's double NONE
// test); a bool specified in the found block 246; a byte flags local there 237 (al 73); the 0x29
// set as bitfield stores 239; the default object index stored through the slot view 239 (al 72:
// flags re-read after it and hoisted above the slot-flag branch, as retail); positive admission 237
// (861, al 64: retail's decline-after-0x40 layout); one 0x1fb7e0 call per seat type 240 (al 58: no
// sbb select, retail's tail-merged call); the 0x29 set after the radii 240 (al 56); a byte mask
// local 230 (al 77: 0x40 test from memory as retail, but the mask then takes BL); that local before
// the element 242, an assigned-if `same` 245, the negative decline test 230; element or iterator
// declared earlier 240/240; the original loop with the new tail 244. The al-56 form (all of the
// above but the byte local) leaves the loop's entry in memory and the prologue order.
// Round 37 (lane 1, fast; "al" = difflib-aligned count, 121 for the original on this metric): the
// selection grown to 0x14 with the radii at +0xc/+0x10, plain iterator and entry locals and the 0x20
// test at both uses 237 (861 bytes, al 109); plus positive admission 241 (al 95); plus 0x1bead0's
// fallback local assigned after its test 238 (al 80, retail's double NONE test); plus the default arm's
// stores in retail's order (radius, seat, radius, flags) 239 (al 74); plus the default object index
// stored through the slot state 237 (al 68); plus one 0x1fb7e0 call per seat type 240 (865 bytes, al
// 61); plus a byte mask local 230 (frame 0x10, mask in BL, al 85), with a named element index 230; the
// sibling's `for` loop 240 (al 61); the 0x40 test as `(flags >> 6) & 1` 240. Open: retail keeps the
// loop entry in EBX (home F+4) and re-reads the flags for the 0x40 test; ours keeps the flags (or the
// byte mask) in BL and the entry in memory, entering the loop by a jump past a reload, which shifts
// every later instruction by two.
// Round 109 (lane 6, fast): the matched 0x1bead0's loop (a plain slot-entry iterator and a for loop over
// the returned entry) 202; the joint index declared first 198 (same); the slot/selection pointers before
// the actor 201; the radii through the selection 239 (861 bytes, frame 0x10), with that loop 239, plus
// `byte specified = flags & 0x20` (retail's masked byte, spilled to F+3) 238.
// Round 45 (lane 10, fast): 1..16 unused locals at the top 198 each (no numbering ties); the unit test through
// `unit->parent_index` 198 (same code), with object_get written in the test and no unit local 198 (same code);
// the slot/selection pointers after the early return 261 (877 bytes), with the inline test 261; the slot/selection
// pointers before the actor with the inline test 201. Retail re-reads g_4e0300 at the later lookups, so a named
// header-table local for all three lookups would differ there; the FORMS-slot-data-block form is the
// "typed slot payload 198" above (no change).
// Round 217: Mirrored read-only root equality with member/original payload and bool/byte/word/long
// comparison-result domains: 198/198/198/198/202/202/201/201. Original equality, payload and carriers restored.
// Round 62 (fast, confirmed plain): plain iterator and entry locals, a byte mask, 0x1bead0's fallback
// element local assigned after its test and the radii through the selection (s_seat_selection grown to
// +0x14, as 0x1ba5c0 also reads them) 156 (kept); one 0x1fb7e0 call per seat type 135 (kept, retail's
// branchy constants into one call, no sbb); the radii stored in the default arm before/after the flags
// 135/134 (original order kept). On that: positive admission 135 (aligned 93); default stores in retail's
// order 133; default object index through the slot state 229; both of the first two 133; no mask local
// 245 (868 bytes), with positive admission 241 (864 bytes, frame 0x14 but the flags byte in BL); `same`
// as if/else 141/223; the mask or entry declared at the top, entry declared in the while 135 each;
// `!same && specified` 228; a char mask 135; mask before the element 238; for loop 135. Diagnostic: the
// root calls to an unresolved stand-in leave the mask in BL (135), so 0xbaf40's summary is not the cause.
// Round 116 (lane 8, fast): the found entry's index read after the loop 134 (same code); the mask declared before
// the element 240 (862 bytes); `const byte` mask 134; first entry fetched before the loop with the step at its bottom
// 134; diagnostic `long` mask 248 (873 bytes: the mask leaves BL but the CSE'd flags byte takes it; the entry stays in
// memory). The entry loses EBX to every byte candidate.
// Round 51 (lane 10, fast; control 134): the decline decided in an if/else into a named bool 134; the decline
// test negated with the join attempt first and the decline in the else 134; the decline arm ending in `continue`
// 134; entry/element/specified renamed (slot_entry, joint, has_object: same-size slots sort by name) 134; the
// joint index declared first 134 (all the same code). Retail's frame is mask byte F+3, entry home F+4, actor F+8,
// iterator F+0xc; ours has entry F+0, actor F+4, iterator F+8 (same order) with the mask in BL, so the open
// difference is the callee-saved choice (retail gives EBX to the entry), not slot order.
// Round 20 (lane 17, fast): the element index read once into a local and reused for the joint index at
// the break 244; a plain break with `if (entry != NULL) joint_index = entry->unknown4;` after the loop
// 134 (same code); the iterator's actor index stored before the unit test 139; the iterator declared
// first 134 (same code); diagnostic (reverted): the unit's lookup written in place with the header table
// as the subscript base 134 (same code, so the head's early header load is not evaluation order).
// Round 39 (m3 lane 14, fast): the FORMS-slot-data-block form (the state read through `slot->unknownc` as a data
// struct with offsets 0xc lower, its bool and the +0x24 short through it) 134 (same code), declared between the
// actor and the unit 134; the iterator declared at the top 134; joint_index and entry declared at the top 134; the
// decline test as an if/else-assigned bool 134 (same code); the fallback assigned straight to joint_index (no
// element local) 233. In retail EDI keeps the actor through the loop
// untouched and EBX takes the entry (spilled at F+4, briefly the first 0xbaf40 result); ours gives BL the mask and
// uses EDI for that result, reloading the actor.
// Round 20 (lane 17, fast): the element index read once into a local and reused for the joint index at
// the break 244; a plain break with `if (entry != NULL) joint_index = entry->unknown4;` after the loop
// 134 (same code); the iterator's actor index stored before the unit test 139; the iterator declared
// first 134 (same code); diagnostic (reverted): the unit's lookup written in place with the header table
// as the subscript base 134 (same code, so the head's early header load is not evaluation order).
// Round 54 (m3 lane 11, fast; "al" = difflib-aligned, base al 109): `same` from `(selection->flags & 0x20)` with
// the byte mask declared after it 244 (868 bytes, al 90: frame 0x14, retail's F+3 mask spill and F+4/F+8/F+0xc
// homes, but the CSE'd flags byte takes BL for the 0x40 test); the mask first but unused in `same` 244 (al 90);
// the decline test reading the flags 244 (al 90); the first with the 0x40 test through `state->flags` 234 (al 93,
// mask back in BL), as `(selection->flags >> 6) & 1` 244 (al 90); no mask local, 0x40 through the slot 234.
// Round 56 (m3 lane 10, fast): `bool specified = (flags & 0x20) != 0` 241 (863 bytes); `long` or `dword` specified 248
// (873 bytes); `same` testing the field with the mask local only in the decline 244 (868 bytes), the reverse 244; `byte
// const` or `unsigned char` specified 134 (same code); no mask local (0x20 at both uses) 244 (868 bytes: frame 0x14 and
// the masked byte at [esp+0x13] as retail, but the flags byte is CSE'd into BL for the 0x40 test and the entry stays in
// memory), with the 0x40 test through a bit-6 bitfield 244, through the slot view 234 (865 bytes); the mask local with
// the 0x40 test through the slot view 235 (866 bytes), through a bit-6 bitfield 134 (same code).
// Round 513: Complete per-iteration flags captured as byte/word/dword, loop/function
// carriers, with typed member payload first and on the outer dword graph:
// 134/134/235/233/134/235/233/233. Original body and data declarations restored.
// Round 462: fallback-failure guard, absent-selection guard, both, both with common
// cleanup-result exit, each with native/original slot payload: 232/232/138/138/232/232/237/237.
// Original payload and exits restored; frame, mask spill and entry/root carriers remain.
// Round 318: One actual element receiver shared by search and completion, crossed with outer
// same/raw-mask declarations: native payload first, complete graph, element, same,
// element+same, element+mask, same+mask, complete graph at entry, eight at 134.
// Original observations and complete body restored; mask spill and entry/root carriers still
// differ.
// Round 465: admission field captured before the root calls, bool/byte/short/long,
// each with native/original payload: 225/225/225/225/214/214/225/225.
// Original field observation restored; frame, mask spill and entry/root homes remain.
// Round 237: Complete compatibility/incompatibility bool, native/original payload, 247/247/236/236;
// compatible per-use mask bool/long, native/original payload, 260/260/265/265. Per-use forms
// recover frame 0x14 and mask +0x13, but the whole flags byte occupies BL and entry stays stacked.
// Native member payload paired with each graph; original payload, masks, tests and full body restored.
// AC468: Complemented specified/root predicate in bool/byte/short/long, each with native/original
// slot-member payload first: 136/136/136/136/139/139/138/138. Original predicate and payload restored;
// frame 0x10 versus 0x14, raw-mask spill and entry/root carriers remain.
// Y538: Saved-mask complement alone in bool/byte/short/long, each with native/original
// member payload, 245/245/245/245/265/265/254/254. All restored; frame, raw-mask spill
// and entry/root carriers remain. The actual 0x34-byte member payload is neutral in every pair.
// AC489: Separate distance/seat validation guards and negative special admission, seven
// subsets, plus native slot-member payload on the complete graph: eight at 134. Original
// body restored; frame, mask spill and entry/root carriers remain.
// Round 142 (fast; aligned count in brackets, control 134 [109]): retail's decline arm sits above the shared next call and the accept arm
// below it: the predicate inverted or written positively in an if/else gives that layout, 134 [97] (not kept: the count does not move while
// the frame and EBX differ); positive with `continue` 134 [97]; `continue` on decline, a decline bool, one goto form, and the for-loop on the
// original predicate 134 [109] each. On the positive form: unit read directly or a parent local, entry declared first 134 [97] each; same in
// if/else 245 [104]; bool mask 240 [94]; mask declared first 242 [95]; mask at function scope, char mask 134 [97]; long mask 248 at 873 [81]
// (frame 0x14, but the whole flags byte takes BL); element index in a two-use local 252 at 845 [90]. EBX goes to the mask where retail gives it
// to the entry, which leaves the entry on the stack and the mask out of its +0x13 slot.
// The slot data-block form (selection through slot->unknownc) 134 [109]; a whole data struct 140 [109], with the positive predicate 141 [97]:
// retail folds +0xc into its reads through the slot here, so that form does not apply.
// Round 132: stable current-entry pointer/reference/field pointer 134 each; nested actor lookup
// for the unit 230; existing object-header getter 134; same-root byte/short/long results
// 134/140/139 (864/863 bytes for the wider two). Original body restored.
// Round 47 (lane 15, fast, base 134): no mask local, flags temp at F+3 as retail but the flags byte in BL, 244 (868 bytes); with the
// found index read after the loop 244; 0x40 through a second selection cast 234 (865); 0x40 as a shift 244; both bits as `& 0x60` 245
// (871); `same` as false default plus if 239 (865). 0xbaf40 is pure, so the flags load stays common across its calls.
// Round 132: stable current-entry pointer/reference/field pointer 134 each; nested actor lookup
// for the unit 230; existing object-header getter 134; same-root byte/short/long results
// 134/140/139 (864/863 bytes for the wider two). Original body restored.
// AC506: Concrete-slot/untyped/byte/header callback pointer formals, each paired
// with indexed native member payload first and original payload: eight at 134.
// NOT kept: alias contracts do not recover the mask spill or EBX entry; original
// callback definition/declaration, payload and complete body restored.
// Round 130 (m3 lane 8, fast): retail duplicates the loop's next-entry call into the decline path and tests the
// 0x40 bit with a fall-through decline. The decline test as `(u && !bit40) || (!same && (u || specified))` 234
// (862 B), its two halves swapped 237, the ternary negated with arms swapped 231 (862 B); a flags byte local
// read before the slot-bool test for both arms' 0x20 tests 141; the entry declared in the while condition 134
// (same code).
// Y561: Read-only actor/unit/loop-candidate/all receiver contracts,
// each through member/original payload, eight at 134. Original mutable
// receivers and payload restored; mask spill and EBX entry remain.
// AC506: Concrete-slot/untyped/byte/header callback pointer formals, each paired
// with indexed native member payload first and original payload: eight at 134.
// NOT kept: alias contracts do not recover the mask spill or EBX entry; original
// callback definition/declaration, payload and complete body restored.
// AC511: Actual search-only/search-and-fallback iterator scopes, original/readonly
// current-entry receivers, native/original slot-member payload first in each pair:
// eight at 134. NOT kept: frame 0x10, raw-mask spill and EBX entry remain; original
// scopes, receivers, payload and complete body restored.
// AC513: Actual nested selection member, native payload/full slot 134/134;
// direct nested-member observations full/payload 189/238; native member with positive
// admission 134, early declaration 137, late full/payload receiver 233/233.
// NOT kept: no exact match or qualifying gain; original types, payload and body restored.
// AC516: Complete raw matching identity mask across root calls, byte/word/dword/long,
// native slot-member payload first in each pair: 139/139/251/251/248/248/248/248.
// NOT kept: assigned mask does not match; original Boolean, payload and complete body restored.
// AC518: Actual join-call result bool/byte/short/long, each with native member
// payload first and whole-slot payload second: 158/134/158/134/158/134/158/134.
// NOT kept: one-use admission result unmatched; original body and types restored.
// AC523: Progressive same-root result initialized from canonical specified,
// then replaced by the gated equality, bool/byte/short/long with member/full
// payload pairs: 225/225/225/225/296/296/295/295. NOT kept: result residence
// unmatched; original mask, equality, payload types and complete body restored.
// AC525: Post-validation transport-admission bool/byte/short/long results,
// native slot-member/full-slot pairs, eight at 134. NOT kept: result residence
// unmatched; original payload, masks, roots and full body restored; mask/entry homes remain.
// AC527: Post-search reservation observation as bool/byte/short/long, native member/full-slot
// payload pairs, eight at 134. NOT kept: one-use result residence unmatched; original types,
// observation, raw mask, root short circuit and complete body restored; mask/entry homes remain.
// AC547: Existing root/iterator/accept-decline/all ordinary-inline declaration
// policies, native member payload first in each pair: eight at 134. NOT kept:
// no frame/mask/entry recovery; original declarations, payload and body restored.
// AC549: Selected handle absent throughout search, conditional/exhaustive/default
// post-search producers, then conditional with direct fallback; native member/full-slot pairs:
// 134/134/134/134/235/235/233/233. NOT kept: no frame/mask/entry recovery;
// original handle lifetime, fallback, payload and complete body restored.
// M3 lane 4 r164 (fast; aligned count in brackets, control 134 [84]): EBX holds the joint index after the loop in both builds, and in
// the loop the byte mask (only BL can hold it across the calls) takes it where retail gives it to the entry. Positive predicate 134
// [83]; the element looked up again through the entry in the predicate 254 (874 B), in the root compare 243 (862 B), in both 254; the
// predicate re-lookup with the positive predicate 250 (874 B), with both 250. Volatile mask (diagnostic, reverted) 254 (879 B): frame
// 0x14 and the mask at +0x13, but the flags read is then shared and BL holds it, so retail's spilled mask is not a volatile's shape.
// Round 150 (m3 lane 5, fast; m = masked aligned, control m29): mask as byte, bool, long const ref 134, 241 (863), 248 (873); positive predicate 134 m24; on it same as const ref 134,
// element or index const ref 134, same inside a do-while(false) 246 (872) m20 (frame 0x14, same at +0x13), nested if or named roots 245 m29, roots through a ternary 250 m37, for-form
// iterator 134 m24; flags local ahead of the slot test 141, with positive 142 m25. No mask local, positive: 240 (864) m18 (masked byte at +0x13 as retail, but the flags byte takes BL
// for `test bl, 0x40`); the 0x40 test through a word, dword, short read 138, 135, 138, a slot-offset byte 230 m21 (the masked value takes BL), a shift or bool conversion 240.
// Diagnostics, reverted: one more read of the entry or the mask 263, 254 (mask keeps BL); a compiler barrier after the two root calls on the mask form 134, no mask 230 m26, volatile
// mask 247 m17: that last gives retail's EBX entry with `mov ebx, eax` for the first root, so the difference is which byte value holds BL. 0xbaf40's memory summary as a cause is
// untested, its file was held this round.
// T58: Actual iterator aggregate/reference-tuple/value/library construction, native member/full-slot payload pairs:
// 134/134/259/259/256/256/256/256. NOT kept: no eligible match or gain; complete original source restored.
// Mask spill, EBX entry and actor/root homes remain.
// N398: Actual slot/state/header/byte callback reference formals, each with indexed member/full-slot payload 134/134/134/134/134/134/134/134.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// Round 92 (m3 lane 16, fast): `same` as a ternary with a false arm 223; the entry as a pointer to const 134; the mask
// as a byte cast of the masked flags 134; the decline ternary negated with its arms swapped 231 (862 B); `specified != 0`
// in `same` 134. Register-blind aligned difference 49 of 247: the prologue object-table load, the loop (decline path with
// its own next-entry call, mask spill, EBX entry) and the default arm (flags re-read after the object-index store and the
// 0x20 load hoisted above the slot-bool test) are three separate blocks.
// Lane 11 round 80 (fast; al/m = difflib-aligned and register-masked aligned counts, this body al 109 m 49; the positive
// predicate 134 al 97 m 36). On the positive predicate: bit 5 through a byte bitfield at both uses with no mask local and a
// plain 0x40 mask 240 (861 bytes, frame 0x10, the bit value in BL, the 0x40 test re-read from memory as retail); bits 5 and 6
// both through the bitfield 240; the flags through a `byte const *` field pointer with no mask local 240 (864 bytes, frame
// 0x14, m 28), with the byte mask local 134 (same code); the mask local set from the bitfield as `? 0x20 : 0` 134 (same
// code); `same` from the bitfield with the plain mask in the decline 136, the reverse 136; no mask local with plain masks
// 240 (864 bytes, m 28). Predicates with one shared same-object test, with / without the mask local: `u && !bit40 || (u ||
// specified) && !same` 234 (862) / 244 (870); two decline ifs ending in `continue` 255 (880) / 269 (888); an if / else-if
// chain 255 / 269 (same code); positive `(!u || bit40) && (!(u || specified) || same)` 165 (883) / 265 (891); `u && !bit40
// || !same && (specified || u)` 244 (870); a nested ternary `(u ? !bit40 : !specified) ? u : !same` 268 (886) / 251 (873);
// m 44-59 for all of these. On the no-mask-local positive form: a header-table local for the unit lookup 240 (same code),
// the unit lookup nested in the actor lookup 245 (m 41), no unit local 240 (same code); a flags byte local above the slot
// bool test in the found block 238 (862 bytes, m 27), with the slot bool named too 238, a bool `specified` there 247 (m 33);
// with the mask local, that flags local 142, the header-table local 134. All restored.
// Round 103 (m3 lane 16, fast with 0x1b8c80/0x1b8d40/0x1ba990/0x1b9e70, all matched): `same` started false and set by an
// `if (specified)` 241; the element as a pointer to const 134 and the selection as a const pointer 134 (both the same code).
// Diagnostics (reverted): the two root calls through the matched pure reader 0x1b8c80 136 (aligned 86, register-masked 50,
// against this body's 84/49), through 0x1b8d40 380 (973 bytes, it inlines). 0xbaf40's own memory summary stays untested:
// its file is held this round. The slot data-block form (state through `slot->unknownc`) is round 39's 134 (same code).
// Also: `joint_index` declared at the top of the function 134 and the entry declared in the while condition 134 (both the same code).
// Round 103 (m3 lane 9, fast): no unit pointer (the field test on object_get directly) 134 (same code); the mask before the element
// lookup 240 (862 bytes); the positive ternary `unknown86 ? (flags & 0x40) && same : !specified || same` with the accept call
// inside and the decline in the else 134 (aligned 97), with `continue` after a failed accept 134; `joint_index` declared before
// the iterator 134 (same code). Retail's loop never touches EDI (the actor stays there) and gives root 1 EBX, reloading the entry
// from F+4 after the root calls; ours gives root 1 EDI and reloads the actor. The 0xbaf40 summary lead needs a body change in
// src/unknown_0bad50.cpp, on this round's avoid list. Also: the state and selection pointers declared before the actor 137; the
// decline written `unknown86 ? !same || !(flags & 0x40) : !same && specified` 231 (862 bytes); the actor fetched again after
// the loop (diagnostic, frees EDI in the loop) 261 (878 bytes).
// Round 69 (m3 lane 15, fast): the iterator's invitation index in a short local passed to both the decline and the
// join call (0x1b6120's idiom) 134 (same code); the default arm's `|= 0x29` through the slot state's flags 202 (873 B);
// positive admission with the default object index through the slot state and the default stores in retail's order
// (object, near radius, seat, far radius, flags) 230 (861 B, loop unchanged); 0x1b6120's whole-reference copy
// (`*(long *)&iterator.reference` in a local) for both calls 134 (same code). No other untried natural form is left for
// the EBX choice (entry in retail, mask here).
// MiniMax H (11-11): no new form, no lower score (134); the body is unchanged, because retail still gives EBX to
// the loop entry and spills the mask byte where ours keeps the mask in BL and the frame is 0x10 against retail's
// 0x14, and the remaining lever is 0xbaf40's memory summary in src/unknown_0bad50.cpp, which this lane may not
// edit.
// MiniMax R (11-10): no new form, no lower score (134): the frame is 0x10 against retail's 0x14, and retail
// spills the mask byte and keeps the loop entry in EBX.
// M3 lane 9 r110: no new form. The FORMS slot data-block form is round 39's 134 (same code); `unit->parent_index` is
// listed (same code). 0xbaf40's memory summary is still untested (src/unknown_0bad50.cpp is on this round's avoid list
// again), and the opaque-callee diagnostic above (root calls to an unresolved stand-in, 135, mask still in BL) speaks
// against it. The open cause is the callee-saved choice: retail gives EBX to the loop entry and spills the mask; the
// note lists the mask types, mask placements, entry/index locals, loop spellings, decline spellings and names.
// M3 lane 3 r231 (fast with 0x1b8c80/0x1b8d40/0x1ba990/0x1b9e70, all matched). Retail also lays the loop out differently: the
// decline block follows the unknown86 test and falls into the next-entry call, with the unknown86-clear tests, the accept call
// and the success block after it; ours puts the unknown86-clear tests first and the decline block jumps to the next-entry call,
// which follows the accept call. Re-measured listed forms only: the plain break with `if (entry)` after the loop 134 (same code)
// and the element index named for the lookup and the joint index 244 (round 20's), the decline condition negated with the
// accept test as the then-arm 134 (same code, lane 9 r103's positive ternary), the fallback assigned straight to the joint
// index 233 (round 39's), the loop as a `for` 134 (MiniMax C's). The slot data-block form is round 39's 134 (same code). No
// untried natural form is left in this body: the remaining lever is 0xbaf40's memory summary, and its file
// (src/unknown_0bad50.cpp) is on this round's avoid list again.
// Round 170 (m3 lane 6, fast with 0x1b8c80/0x1b8d40/0x1ba990/0x1b9e70, all matched; masked = register-masked aligned count,
// 75 for this body): retail has one next-entry call in the loop (0x1ba17a); its decline block falls into it and the failed
// accept jumps back to it (round 130's "duplicated" call is block placement). Forms: the mask read through the slot view
// (`state->flags & 0x20`) 244 (866 B); the seat index after the 0x26c test read through the top's unit pointer 249 (frame 0x14,
// masked 66, but the early return gets its own epilogue and the mask stays in BL); the 0x26c compare's operands swapped 134
// (two lines differ); the iterator's reference fields stored unknown0 first 134 (two lines differ); the found block's bit-6
// store ahead of the object store 229. Same code as this body: the unknown86 arm as `!((flags & 0x40) && same)`, the entry
// initialised to NULL, `specified != 0` in `same`, the 0xc byte as a typed `bool` member of the slot state, bit 6 as
// `(flags & 0x40) != 0` at the store and the 0x1ba990 argument, the 0x2676d0/0x1e2030 test negated with its arms swapped.
// Re-measured: the slot/selection pointers after the early return 233 (round 45's 261 on an older body).
// MiniMax A (11-11, second pass): no new form, no lower score (134); the frame is still 0x10 against retail's 0x14, and the difference
// is still the registers: ours keeps the incoming actor index in ECX and spills it to its own slot, keeps the seat flags byte in BL and
// stores the iterator's result to the stack, where retail keeps the actor index in EDX and writes it straight into the iterator record,
// pushes EBX before the call and keeps the result there, and spills the flags byte to the stack; the actor array base, the scaled index
// and the tag table global therefore land in different registers (retail EBP, EDI and ESI, ours ECX, EBP and EDX).
// MiniMax A (11-11): no new form, no lower score (134); our build keeps the incoming actor index in EBX for the whole body and saves it in the prologue, where retail keeps it in EDX only until the iterator record is filled and reloads it from its stack slot for each later call, so the actor array base, the scaled index and the tag table global land in different registers (retail EBP, EDI and ESI, ours ECX, EBP and EDX) and the frame slots of the stored actor pointer differ.
// m3 lane 10 r99 (fast with 0x1b8c80/0x1b8d40/0x1ba990/0x1b9e70, all matched): the decline's 0x20 test re-read from the
// flags with the mask local used only by `same` 244 (868 B); the element index stored before the slot flag together with a
// dword joint index compared as signed (MiniMax C's 133 and 132 forms combined) 132, two points and both steering, not kept;
// `joint_index == NONE &&` in the while condition with no break 256 (874 B); the tail without gotos (one success bool, the
// cleanup and `return NONE` in an `if (!success)`) 201; the root compare's operands swapped (selection's root first) 134.
// Round 240 (m3 lane 3, fast): the seat block typed as the tag array (with 0x1b9200) 134; the body after the parent test
// wrapped in `if (parent == NONE) { ... return joint_index; } return NONE;` with the joint index declared first 134; the
// iterator's actor index stored after both reference fields 134, the three stores fully reversed 134, the actor index
// between the reference fields 134. Diagnostics (reverted): 1..8 unused locals at the top 134 each (no numbering phase);
// the two root calls to an undefined stand-in: with the mask local 134 (mask still in BL), with no mask local 229 (the
// entry then takes EBX with its F+4 home and the reload after the roots as retail, but the flags byte is re-read for
// both tests and nothing lands at F+3), against 244 for no mask local with the real roots.
// @retail 0x1ba090
long __stdcall function_1ba090(long actor_index, s_slot *slot)
{
    s_actor_view *actor = actor_get(actor_index);
    s_slot_object_view *unit = object_get(actor->unknown018);
    s_slot_4c *state = (s_slot_4c *)slot;
    s_seat_selection *selection = (s_seat_selection *)&state->unknown1c;
    if (*(long *)((byte *)unit + 0x14) != NONE)
        return NONE;
    s_slot_entry_iterator iterator;
    iterator.actor_index = actor_index;
    iterator.reference.unknown2 = 0x4c;
    iterator.reference.unknown0 = NONE;
    long joint_index = NONE;
    s_slot_memory_entry *entry;
    while ((entry = function_26f0c0(&iterator)) != NULL)
    {
        s_4c_element *element = (s_4c_element *)element_502424_get(entry->unknown4);
        byte specified = selection->flags & 0x20;
        bool same = specified && function_baf40(element->object_index) == function_baf40(selection->object_index);
        if ((element->unknown86 ? !(selection->flags & 0x40) || !same : specified && !same))
            joint_decline(actor_index, iterator.reference.unknown0);
        else if (function_26ecc0(actor_index, iterator.reference.unknown0, (s_joint_behavior_state *)slot))
        {
            joint_index = entry->unknown4;
            break;
        }
    }
    if (joint_index == NONE)
    {
        long element_index = function_26e940(actor_index);
        if (element_index != NONE)
        {
            *((bool *)slot + 0xc) = true;
            state->element_index = element_index;
        }
        joint_index = element_index;
    }
    if (joint_index != NONE)
    {
        s_4c_element *element = (s_4c_element *)element_502424_get(joint_index);
        if (*((bool *)slot + 0xc))
        {
            if (selection->flags & 0x20)
            {
                element->object_index = function_1b8c80(selection->object_index);
                element->unknown86 = ((selection->flags >> 6) & 1) != 0;
                if (actor->unknown26c == selection->object_index)
                    selection->seat_index = object_get(actor->unknown018)->unknown1fc;
            }
        }
        else if (!(selection->flags & 0x20))
        {
            selection->object_index = element->object_index;
            selection->seat_index = NONE;
            selection->flags |= 0x29;
            selection->near_radius = 11.0f;
            selection->far_radius = 14.0f;
        }
        if (!(selection->flags & 0x20) || selection->object_index == NONE || element->object_index == NONE ||
            !function_1ba990(actor_index, selection->object_index, ((selection->flags >> 6) & 1) != 0, selection->near_radius, selection->far_radius, false))
            goto failed;
        if (function_2676d0(actor_index) && function_1e2030(actor_index))
        {
            selection->seat_index = NONE;
            element->unknown85 = true;
        }
        else
        {
            if (!function_1b9e70(actor_index, selection, true, element, 0) || selection->object_index == NONE || selection->seat_index == NONE)
                goto failed;
            s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[object_get(selection->object_index)->tag_index & 0xffff].bytes;
            s_vehicle_seat_definition *seat = &definition->seats[selection->seat_index];
            if (actor->unknown26c == NONE)
            {
                if (TEST_FIELD_BIT(seat->flags.bit2)) function_1fb7e0(0x65, actor_index, 0, selection->object_index, NONE);
                else if (TEST_FIELD_BIT(seat->flags.bit3)) function_1fb7e0(0x67, actor_index, 0, selection->object_index, NONE);
                else function_1fb7e0(0x68, actor_index, 0, selection->object_index, NONE);
                function_f5d10(selection->object_index, selection->seat_index, true);
            }
        }
        function_1f86a0(actor_index);
        state->unknown24 = 0;
    }
    return joint_index;
failed:
    if (joint_index != NONE)
        function_26ee40((s_joint_behavior_state *)slot);
    return NONE;
}


#include "unit_requests.h"
struct s_unit_child_iterator
{
    long object_index;
    long unit_index;
    short seat_index;
    long next_index;
};
struct s_damage_object;
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);
void function_1e3400(long actor_index, short squad_index);
void function_201ad0(long squad_index, long object_index);

// @retail 0x1bb570
void __stdcall function_1bb570(long vehicle_index, long actor_index)
{
    if (g_4f55d0->active)
    {
        s_actor_view *actor = actor_get(actor_index);
        bool driver = *(long *)((byte *)object_get(vehicle_index) + 0x248) == actor->unknown018;
        if (*((bool *)actor + 0x3c))
            *((bool *)actor + 0x3c) = false;
        else if (actor->unknown030 != NONE)
            *(long *)((byte *)actor + 0x28) = actor->unknown030;
        long root_vehicle = vehicle_index;
        long root_index = function_baf40(root_vehicle);
        if (g_4e0300->data[(root_index & 0xffff) * 12 + 3] == 1)
            root_vehicle = root_index;
        byte *vehicle = (byte *)object_get(root_vehicle);
        if (actor->unknown030 != *(long *)(vehicle + 0x3a0))
        {
            if (!driver && *(long *)(vehicle + 0x248) == NONE)
                goto event;
            byte *squad = NULL;
            if (*(long *)(vehicle + 0x3a0) != NONE)
                squad = g_51e9d8->data + (*(long *)(vehicle + 0x3a0) & 0xffff) * 0x98;
            if (squad && !function_1df560(*(signed char *)(squad + 0x76), actor->unknown024))
                function_1e3400(actor_index, *(word *)(vehicle + 0x3a0));
            else if (driver)
            {
                if (actor->unknown030 == NONE && *(long *)((byte *)actor + 0x28) != NONE)
                    function_1e3400(actor_index, *(word *)((byte *)actor + 0x28));
                if (actor->unknown030 != NONE)
                    function_201ad0(actor->unknown030, root_vehicle);
            }
            else
            {
                byte *local_11eac6 = (byte *)object_get(*(long *)(vehicle + 0x248));
                if (*(long *)(local_11eac6 + 0x13c) != NONE && !team_is_enemy(actor->unknown024, 1))
                    goto event;
                if (!squad)
                    function_1e3400(actor_index, NONE);
                else
                {
                    s_slot_object_view *unit = object_get(actor->unknown018);
                    s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[object_get(*(long *)((byte *)unit + 0x14))->tag_index & 0xffff].bytes;
                    if (!TEST_FIELD_BIT(definition->seats[unit->unknown1fc].flags.bit11))
                    {
                        function_e68c0(0x1d, actor->unknown018);
                        goto done;
                    }
                }
                goto event;
            }
        }
        if (driver)
        {
            s_object_seat seats[0x40];
            long previous = NONE;
            short seat_count = function_c8b80(root_vehicle, seats, 0x40);
            for (short i = 0; i < seat_count; ++i)
            {
                if (seats[i].object_index != previous)
                {
                    previous = seats[i].object_index;
                    s_unit_child_iterator iterator;
                    iterator.object_index = previous;
                    iterator.unit_index = NONE;
                    iterator.seat_index = NONE;
                    iterator.next_index = *(long *)((byte *)object_get(previous) + 0x10);
                    byte *unit;
                    while ((unit = (byte *)function_d05c0(&iterator)) != NULL)
                    {
                        if (*(long *)(unit + 0x12c) != NONE && *(long *)(unit + 0x12c) != actor_index && *(short *)(unit + 0x1fc) != NONE)
                        {
                            s_actor_view *other = actor_get(*(long *)(unit + 0x12c));
                            if (function_1df560(other->unknown024, actor->unknown024))
                                function_e68c0(0x1d, iterator.unit_index);
                            else if (other->unknown030 != actor->unknown030)
                                function_1e3400(*(long *)(unit + 0x12c), (word)actor->unknown030);
                        }
                    }
                }
            }
        }
event:
        {
            s_slot_object_view *unit = object_get(actor->unknown018);
            if (unit->unknown1fc != NONE)
            {
                s_vehicle_tag_view *definition = (s_vehicle_tag_view *)g_4e3b44[*(long *)vehicle & 0xffff].bytes;
                if (!TEST_FIELD_BIT(definition->seats[unit->unknown1fc].flags.bit11))
                    function_1fb7e0(0x66, actor_index, 0, root_vehicle, NONE);
            }
        }
done:
        actor->unknown2e8 = NONE;
        actor->unknown2ec = NONE;
        *(short *)actor->unknown2ee = NONE;
        actor->unknown2f0 = false;
        *(short *)((byte *)actor + 0x2f4) = (short)real_to_long((real)g_510c54->field_2_3 * 10.0f);
        actor = actor_get(actor_index);
        *((bool *)actor + 0x5d4) = false;
        *(dword *)((byte *)actor + 0x810) &= ~1;
    }
}

struct joint_state;
long joint_count_invited_participants(joint_state const *arg_0);
bool __stdcall function_1badc0(long arg_0, long arg_1, point3f const *arg_2,
    s_type_c3b527 *arg_3, long *arg_4, bool arg_5, bool *arg_6);
real function_1f8940(long arg_0);
bool function_1f8660(long arg_0);

// @retail 0x1ba5c0
bool __stdcall function_1ba5c0(long arg_0, s_slot *arg_1, long arg_2)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_seat_selection *local_2 = (s_seat_selection *)((byte *)arg_1 + 0x1c);
    s_4c_element *local_3 = (s_4c_element *)arg_2;
    volatile bool local_4 = true;
    if (local_2->object_index == NONE || local_3->object_index == NONE)
    {
        local_4 = false;
        return local_4;
    }
    if (local_0->unknown26c != NONE || (local_2->flags & 2))
        return local_4;
    if (*(short *)((byte *)local_3 + 2) <= 0 &&
        (short)joint_count_invited_participants((joint_state *)local_3) != 0)
        return local_4;
    if (*((byte *)local_0 + 0x5d8) || !function_1b90b0(arg_0, local_3->object_index) ||
        !function_1ba990(arg_0, local_3->object_index, !(local_2->flags & 1),
            ((s_slot_4c *)((byte *)local_2 - 0x1c))->unknown28, ((s_slot_4c *)((byte *)local_2 - 0x1c))->unknown2c, true))
    {
        local_4 = false;
        return local_4;
    }
    s_seat_approach_result local_5;
    if (!function_1b9e70(arg_0, local_2, false, local_3, &local_5))
    {
        local_4 = false;
        return local_4;
    }
    function_f5d10(local_2->object_index, local_2->seat_index, true);
    ((s_slot_4c *)((byte *)local_2 - 0x1c))->point = *(point3f *)&local_5.direction;
    if (local_5.approaching)
    {
        ++*(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30);
        if ((real)*(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30) * g_510c54->rate >= 1.0f)
            goto submit_request;
    }
    else
        *(short *)((byte *)((s_slot_4c *)((byte *)local_2 - 0x1c)) + 0x30) = 0;
    if (local_5.close && local_5.facing)
    {
submit_request:
        s_unit_request local_7;
        local_7.type = 0x1c;
        local_7.type1c.object_index = local_2->object_index;
        local_7.type1c.seat_index = local_2->seat_index;
        local_7.type1c.unknowna = false;
        local_7.type1c.unknownb = false;
        local_4 = function_e6900(local_0->unknown018, &local_7);
    }
    else if (local_5.close)
        function_1f86a0(arg_0);
    else if (local_0->unknown040)
    {
        s_type_c3b527 local_8;
        long local_9;
        bool local_10;
        bool local_14;
        if (!function_1badc0(arg_0, local_2->object_index, &local_5.point,
            &local_8, &local_9, false, &local_10))
            goto reset_attempts;
        local_14 = function_1f4460(arg_0, &local_8, local_9,
            local_10 ? NONE : local_2->object_index, false);
        local_4 = local_14;
        if (local_14)
        {
reset_attempts:
            local_2->unknown7 = 0;
        }
        else
        {
            ++local_2->unknown7;
            short local_11 = (local_2->flags & 1) ? 5 : 50;
            if ((short)local_2->unknown7 > local_11)
            {
                local_4 = false;
                actor_get(arg_0)->unknown040 = false;
            }
            else
                local_4 = true;
        }
    }
    bool local_12;
    if (function_1f8660(arg_0))
    {
        local_12 = 1.0f > function_1f8940(arg_0);
        if (local_12)
            local_2->flags |= 4;
        else
            local_2->flags &= ~4;
        return local_4;
    }
    else
    {
        vector3f local_13;
        vector3d_from_points3d(&local_0->position, &local_5.point, &local_13);
        real square = local_13.k * local_13.k;
        square += local_13.i * local_13.i;
        square += local_13.j * local_13.j;
        local_12 = 1.0f > square;
    }
    if (local_12)
        local_2->flags |= 4;
    else
        local_2->flags &= ~4;
    return local_4;
}
