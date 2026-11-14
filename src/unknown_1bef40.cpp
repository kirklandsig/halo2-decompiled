// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_0259d0.h"
#include "unknown_1fb7e0.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "unknown_20fe20.h"
#include "unknown_1e1f20.h"

extern "C" void _ReadWriteBarrier(void);

/* slot types 0xa and 0xb */

struct s_slot_0a
{
	s_slot_header header;
	s_reference reference;
	short unknown10;
	short unknown12;
	byte unknown14[0x40 - 0x14];
};

struct s_slot_0a_data
{
	s_reference reference;
	short unknown04;
	short unknown06;
	byte unknown08[0x34 - 0x08];
};

struct s_slot_0b
{
	s_slot_header header;
	word unknown0c;
	byte unknown0e[2];
	long unknown10;
	byte unknown14[0x40 - 0x14];
};

short actor_behavior_find_level(long owner_index, short type);
short __stdcall function_1b7920(long actor_index);

short __stdcall function_1bef40(long actor_index);
short __stdcall function_1bf0f0(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1bf230(long actor_index, s_slot *slot);
bool __stdcall function_1bf4e0(long actor_index, s_slot *slot);
void __stdcall function_1c1520(long actor_index, s_slot *slot, long index);
void __stdcall function_1bf5c0(long actor_index, s_slot *slot);

/* the block of the actor's character tag function_1e4d10 returns */
struct s_character_d10
{
	byte unknown00[0x2c];
	real unknown2c;
};

void *function_1e4d10(long actor_index);
void function_1f86a0(long index);

short function_2684f0(s_actor_view *actor);
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);
long function_1469f0(real seconds);

#pragma intrinsic(_ReadWriteBarrier)
// score 13 (was 13): retail loads the slot's reference into ECX ahead of the actor's index arithmetic and compares
// it with the global as a memory operand; ours (with the barrier) loads it into EDX after the arithmetic, and the
// inlined action lookup scales the index into ECX where retail scales it into EDX and reads the unit table late.
// Earlier notes, merged into this one:
// MiniMax A (11-12): no new form, no lower score (13); the body is unchanged, because the slot's
// reference still loads into EDX after the actor's index arithmetic where retail loads it into ECX
// before it, and the inlined action lookup still scales the index into ECX where retail scales it
// into EDX; this round the reference copied into a named s_reference local measures 100, and the
// order inside the action lookup comes from the macro in a file this lane may not edit.
// MiniMax G (11-10): no new form, no lower score (13); the two load regions are unchanged - the slot's reference is still loaded into EDX after the actor's index arithmetic where retail loads it into ECX before it, and the inlined action lookup still scales the index into ECX where retail scales it into EDX and reads the unit table late; that order comes from the macro in a file this lane may not edit, so the function cannot match from here.
// MiniMax C (11-11): no new form, no lower score (13); the slot's reference is still loaded into EDX after the actor's index arithmetic where retail loads it into ECX before it, and the inlined action lookup still scales the index into ECX where retail scales it into EDX; that order comes from the action-lookup macro in src/unknown_0a76b0.cpp, a file this lane may not edit.
// MiniMax D (11-10): no new form, no lower score (13); the slot's reference is still loaded into EDX after the
// actor's index arithmetic where retail loads it into ECX before it, and the inlined action lookup still scales
// the index into ECX where retail scales it into EDX and reads the unit table late - that order comes from the
// macro in a file this lane may not edit, so the function cannot match from here.
// MiniMax I (11-08): no new form, no lower score (13); the reference copied into a named local gives 16 with the table base in ESI instead of EDX, the barrier at five positions 94, and the action helper's result unused 13 (the same code).
// MiniMax C (11-06): no new form, no lower score; the slot reference still loads into EDX after the actor's index arithmetic where retail loads it into ECX before it, and the inlined action lookup still scales the index in ECX where retail scales it into EDX.
// MiniMax I (11-06): no new form, no lower score; the slot reference still loads into EDX after the actor's index arithmetic where retail loads it into ECX before it, and the inlined action lookup still scales the index in ECX where retail scales it into EDX; this round the reference copied into a named local after the actor's lookup (99) and before it (100) both move the load but grow the body, and the permuter (420 s) found no candidate.
// MiniMax G (11-05): no new form, no lower score; copying the slot reference into a long or into a
// named local before the actor's arithmetic still scores 100 and moves the table's data into EBX,
// and the inlined action lookup still scales the index in ECX where retail scales it into EDX.
// MiniMax G (11-04): no new form, no lower score; the slot reference still loads into EDX after
// the actor's index arithmetic where retail loads it into ECX before it, and the inlined action
// lookup still scales the index in ECX where retail scales it into EDX.
// MiniMax E (11-04): no new form, no lower score; the barrier at five positions scores 94, 94,
// 94, 94 and 16, the actor first 17, and the reference compared through the slot 13 (the same
// code), against the unchanged 13.
// MiniMax S (11-09): the unit index carried into the action helper in a named long local 13 (the same code) and
// in a named short local 29; the slot reference and the unit table's two load regions are unchanged.
// Earlier: the slot reference is still loaded late into EDX where retail loads
// it early into ECX, and the inlined action lookup still scales the index in ECX where
// retail scales it into EDX and reads the unit table late; reading the reference into a
// named local before the actor's arithmetic moves the load but spills the actor pointer;
// moving the barrier down to the reference's second use gives 89 and still loads it late
// MiniMax D (11-01): the barrier at the top 94, in the else arm 90, dropped 90, the reference copied
// into a named local 99/100/104 (before the actor's arithmetic, after it, or with a hand-written
// actor lookup), the actor lookup moved into the else arm 96 or after the barrier 94, the compare's
// operands swapped 13: all worse or equal. The copy does put the reference load where retail has it
// (ECX, ahead of the actor's arithmetic) and keeps the global a memory operand, but the table's
// data then moves to EBX with an extra push, where retail keeps it in EDX.
// Earlier: the slot reference still loads into EDX after the actor's index
// arithmetic, where retail loads it into ECX before it; reading the reference into a named
// local before the actor's arithmetic (a bool or a copy) moves the load but costs the body.
// Earlier: retail reads the slot reference before actor arithmetic and the object table
// Earlier measurement 13 (was 13): the slot reference still loads into EDX after the actor's index
// Earlier measurement 13 (was 13): retail reads the slot reference before actor arithmetic and the object table
// after index arithmetic; ours has the opposite order in each case.
// Slot data block 13; without the barrier 90; action helper result 13; actor/state/result 17;
// result/state/actor 13; state/result/actor 13; result/actor/state 17; actor/result/state 17.
// Packed reference field view/local names/entry declarations/actor-result scope/action lifetime 13;
// presence-first branch 91; branch-local actor lifetime 96; shared entry exit 13.
// Native field view and all scope forms restored; both remaining load regions still differ.
// Early reference predicate/default-first/wide-result/presence predicate 109/108/111/109;
// complete current/both reference values 100/104; native word equality logical/bitwise 222/130.
// MiniMax H (11-11): no new form, no lower score (13); the body is unchanged, because the barrier still
// decides where the slot's reference load sits: moving it above the actor's index arithmetic puts the
// global loads after the barrier (94) instead of retail's early ECX load, and copying the reference into
// a named local moves the load but sends the actor table's data to EBX with an extra push.
// MiniMax E (11-10): no new form, no lower score: the slot's reference is still loaded into edx after
// the actor's index arithmetic where retail loads it into ecx before it, and the inlined action
// lookup's own order comes from the macro's order in another file, which this lane may not edit.
// MiniMax E (11-09): no new form, no lower score (13); the barrier decides where the slot's reference load
// sits: with the barrier before the compare the load sinks to the compare and the global stays folded in
// it, with the barrier moved away the global is materialised into a register instead (94 at the top of
// the function, 90 before the write at its end), and with the reference copied into a local ahead of the
// actor's lookup the load is early but the actor table's data moves to EBX with an extra push (100, 104
// with the barrier moved); the inlined action lookup's own order (the index scaled into EDX and the unit
// table read late) comes from the macro's order in src/unknown_0a76b0.cpp, which this lane may not edit.
// MiniMax R (11-09): no new form, no lower score (13): the slot's reference is still loaded into
// edx after the actor's index arithmetic where retail loads it into ecx before it, and the action
// lookup still scales the index into ecx and reads the unit table early where retail scales into edx
// and reads the table late; the reference copied into a named local, the barrier at five positions,
// the scope forms of the three locals and the action helper's unused result were all measured and
// all score 13 or more, so the body stands as it is.
// Round 97 (m3 lane 1, fast): the existing 0x1f86f0 predicate in place of the field test 107 (326 bytes; it stays a
// call). No barrier 90 (the global is loaded into ECX at the top); no barrier with the compare's operands swapped 85
// (322 bytes: the reference then loads into ECX ahead of the arithmetic as retail, but the global is loaded into EDX
// for a register compare; 8 aligned differences against this body's 13). With the function first in the file that
// swap turns matched 0x1bf230 into 8, so the rest was measured with the function at its address place (after
// 0x1befd0), where the unchanged body scores 13 and every neighbour matches (the move alone gains nothing, not kept).
// There: swapped with the barrier 13; swapped with the barrier at the function top 94, ahead of the result read 16,
// in the first arm 85; the reference read into a long local after the result read and then the barrier 99 (323
// bytes: retail's whole sequence, `cmp ecx, [global]` and retail's action-lookup order, but the table base moves to
// EBX with a push at entry), with the compare swapped 99, without the barrier 90, swapped without it 85; that local
// first or between the actor and the result 100/99; an aggregate-typed or const local 99/99; the result read first,
// barrier-free, swapped/unswapped 85/90; a subtraction test 90; a named bool from the swapped compare 85. The state
// typed as the whole slot (as 0x1befd0 and 0x1bf230 type it): plain 18, with the local and the barrier 16 (320
// bytes: the table base goes through ESI and the two wait slots swap), barrier-free 90, swapped 85. Diagnostics
// (reverted): a forced read through the actor ahead of the barrier 135; the later store taking another global 34
// (the table base stays out of EDX either way).
// P419: Callback inline, getter __forceinline, outer scalar lifetimes,
// immediate conversion results and direct action-failure arm 13 each;
// low-readiness arm first 62; separate countdown update/test 13; double
// random-result locals 116. All forms restored; both load regions remain.
// Round 115 (m3 lane 16, fast with 0x1bf230 and 0x1befd0; control 13): both load regions are ours hoisting a global
// load where retail does not: without the barrier the NONE reference global is loaded at the top into ECX (90), and in
// the inlined action lookup the unit table base is loaded ahead of the index arithmetic (retail's matched 0x1afb30 has
// the index first only because no third register is free there). Diagnostics, reverted: the NONE reference global
// defined in a /GL- object instead of src/globals.cpp, barrier-free, 90 (so it is not the link-time view of the global);
// barrier-free with the branch's final store of the global removed 90 (not a common value with that store); 0-7 dead
// initialised locals 13 each (no phase effect). Forms: the state test through a fresh actor lookup 106; the unit index
// read through a fresh actor lookup 106 (328 bytes each).
// Also: the reference read through a pointer to the field 13 (same code); barrier-free with the reference compared
// through the slot's data bytes 90; barrier-free with a const copy of the NONE reference compared 103 (325 bytes).
// @retail 0x1bf0f0
short __stdcall function_1bf0f0(long actor_index, s_slot *slot, bool active)
{
	s_slot_0a_data *state = (s_slot_0a_data *)slot->unknownc;
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe8;
	_ReadWriteBarrier();
	if (REFERENCE_EQUAL(state->reference, g_470fa0))
	{
		function_1f86a0(actor_index);
	}
	else if (actor->unknown504 == 2)
	{
		if (function_e70b0(actor->unknown018).value == 5)
		{
			if (state->unknown06 > 0)
				state->unknown06--;
			else if (function_2684f0(actor) >= 10)
			{
				real delay = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 3.0f, 5.0f);
				real wait = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.0f, 0.5f);
				state->unknown04 = (short)function_1469f0(delay);
				state->unknown06 = (short)function_1469f0(wait);
				function_262800(actor_index, state->reference, false);
				function_1f86a0(actor_index);
				state->reference = g_470fa0;
			}
			else if (--state->unknown04 <= 0)
			{
				g_46eeb8[0x2b]->unknown8 = g_46f348;
				g_46eeb8[0xa]->unknown8 = g_46f348;
				result = g_46fbe4;
			}
		}
		else
		{
			result = g_46fbe4;
		}
	}
	return result;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x1bef40
short __stdcall function_1bef40(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->prop_index != NONE)
	{
		s_character_d10 *character = (s_character_d10 *)function_1e4d10(actor_index);

		if (character)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);

			if (node->unknown24 >= 1 && node->unknown24 <= 2 && character->unknown2c * 2.0f > node->unknown28)
				result = 3;
		}
	}
	return result;
}

static __forceinline long real_to_long(real value);

/* resets the actor, then waits a random 3 to 5 seconds and up to half a second */
// @retail 0x1befd0
bool __stdcall function_1befd0(long actor_index, s_slot *slot)
{
	s_slot_0a *state = (s_slot_0a *)slot;
	real sample = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
	real delay = (real)(3.0f + 2.0f * sample);
	real sample2 = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
	real wait = 0.5f * sample2;

	actor_reset_state(actor_index);
	state->reference = g_470fa0;

	state->unknown10 = (short)real_to_long(g_510c54->field_2_3 * delay);
	state->unknown12 = (short)real_to_long(g_510c54->field_2_3 * wait);
	return true;
}

// @retail 0x1bf230
bool __stdcall function_1bf230(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0a *state = (s_slot_0a *)slot;
	bool result = true;

	if (!REFERENCE_EQUAL(state->reference, g_470fa0) && actor->unknown5b4 > 0)
		state->reference = g_470fa0;
	if (actor->unknown040)
	{
		long other_index = NONE;

		result = false;
		if (REFERENCE_EQUAL(state->reference, g_470fa0))
		{
			byte *scratch = ai_scratch_buffer_get();
			s_2605d0_request request;
			bool unknown;

			memset(&request, 0, sizeof(request));
			unknown = false;
			request.type = 7;
			request.unknown008 = 5.0f;
			request.unknown00c = 5.0f;
			request.unknown056 = true;
			request.unknown057 = true;
			request.unknown010 = 10.0f;
			state->reference = function_2605d0(actor_index, &request, 0, (long)&other_index, scratch, &unknown);
			ai_scratch_buffer_release(scratch);
			if (REFERENCE_EQUAL(state->reference, g_470fa0))
				goto done;
		}
		state->reference = function_2626b0(actor_index, state->reference, other_index, NULL, false, true);
		result = !REFERENCE_EQUAL(state->reference, g_470fa0);
	}
done:
	return result;
}

// @retail 0x1bf360
void __stdcall function_1bf360(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (node->unknown24 >= 1 && node->unknown24 <= 2)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 2;
			actor->unknown488 = true;
		}
	}
}

// @retail 0x1bf3d0
void __stdcall function_1bf3d0(long actor_index, s_slot *slot, bool active)
{
	s_slot_0a *state = (s_slot_0a *)slot;

	if (!active || (state->reference.unknown2 & 0x8000))
		state->reference = g_470fa0;
}

// @retail 0x1bf3f0
bool function_1bf3f0(long actor_index, long other_index, short type)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (other_index != NONE)
	{
		s_actor_view *other = actor_get(other_index);

		if (other->prop_index != NONE && actor->prop_index != NONE &&
			prop_node_get(actor->prop_index)->unknown08 == prop_node_get(other->prop_index)->unknown08 &&
			actor_behavior_find_level(other_index, type) != NONE)
		{
			result = true;
		}
	}
	return result;
}

// @retail 0x1bf480
short __stdcall function_1bf480(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (function_1b7920(actor_index) && function_1bf3f0(actor_index, actor->unknown3cc, actor->unknown3d0))
		result = 3;
	return result;
}

// @retail 0x1bf4e0
bool __stdcall function_1bf4e0(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);

	if (function_1bf3f0(actor_index, actor->unknown3cc, actor->unknown3d0))
	{
		s_slot_0b *state = (s_slot_0b *)slot;
		short type;

		state->unknown10 = actor->unknown3cc;
		state->unknown0c = actor->unknown3d0;
		type = actor->unknown3d2;
		actor->unknown3cc = NONE;
		if (type != NONE)
			function_1fb7e0(type, actor_index, NULL, actor_get(state->unknown10)->unknown018, NONE);
		function_1f86a0(actor_index);
		result = true;
	}
	return result;
}

// @retail 0x1bf590
short __stdcall function_1bf590(long actor_index, s_slot *slot, bool active)
{
	s_slot_0b *state = (s_slot_0b *)slot;
	short result = g_46fbe4;

	if (function_1bf3f0(actor_index, state->unknown10, state->unknown0c))
		result = g_46fbe8;
	return result;
}

/* how the actor's character uses its weapon, by difficulty */
struct s_weapon_difficulty_entry_0b
{
	long unknown0;
	long unknown4;
	long unknown8;
};

struct s_character_weapon_0b
{
	byte unknown00[0x98];
	s_weapon_difficulty_entry_0b difficulty[3];
};

/* aims the actor at its prop: held props are only approached */
// @retail 0x1bf5c0
void __stdcall function_1bf5c0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor->prop_index);
	s_prop_state_view *s_type_5cfb45 = prop_node_state(node);
	s_prop_view_fields *view = prop_node_view(node);
	long weapon_index = function_1e1f20(actor_index);

	if (node->unknown24 >= 1 && node->unknown24 <= 2)
	{
		actor->unknown488 = true;
		actor->unknown41c = 4;
		actor->unknown420 = 2;
	}
	else if (view)
	{
		s_type_c3b527 point = view->unknown18;
		point3f position;

		point.point.z += s_type_5cfb45->unknown38 - s_type_5cfb45->position.z;
		function_210850(&point, &position);
		actor->unknown41c = 3;
		actor->unknown420 = 3;
		actor->unknown424.point = position;
		actor->unknown430 = 2;
		actor->unknown434 = 3;
		actor->unknown438.point = position;
		actor->unknown444 = NONE;
		actor->unknown488 = true;
		actor->unknown4a0 = true;
		actor->unknown48c = true;
		actor->unknown490_point = point;
		if (weapon_index != NONE)
		{
			s_character_weapon_0b *weapon = (s_character_weapon_0b *)function_1e5280(actor_index, object_get(weapon_index)->tag_index);

			if (weapon)
			{
				s_weapon_difficulty_entry_0b *entry;
				short local_0 = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;

				switch (local_0)
				{
				case 2:
					entry = &weapon->difficulty[1];
					break;
				case 3:
					entry = &weapon->difficulty[2];
					break;
				default:
					entry = &weapon->difficulty[0];
					break;
				}
				actor->unknown710 = entry->unknown4;
			}
		}
	}
}

s_slot_handler_2 g_47ee18 =
{
	{
		0xa, 2, 0x7ff, -2, 0,
		function_1bef40, function_1bf0f0, function_1befd0, slot_proc_nothing, NONE, {0},
		0, 0, 0, 0, function_1bf3d0, 0, 0
	},
	(t_slot_proc)function_1bf230, 0, function_1bf360
};

s_slot_handler_2 g_47ee68 =
{
	{
		0xb, 2, 0, -2, 0,
		function_1bf480, function_1bf590, function_1bf4e0, 0, NONE, {0},
		function_1c1520, 0, 0, 0, 0, 0, 0
	},
	0, 0, function_1bf5c0
};
