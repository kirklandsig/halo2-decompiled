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
short *function_e70b0(long unit_index, short *value);
long function_1469f0(real seconds);

#pragma intrinsic(_ReadWriteBarrier)
// score 13 (was 13): the slot reference still loads into EDX after the actor's index
// arithmetic, where retail loads it into ECX before it; reading the reference into a named
// local before the actor's arithmetic (a bool or a copy) moves the load but costs the body.
// score 13 (was 13): retail reads the slot reference before actor arithmetic and the object table
// after index arithmetic; ours has the opposite order in each case.
// Slot data block 13; without the barrier 90; action helper result 13; actor/state/result 17;
// result/state/actor 13; state/result/actor 13; result/actor/state 17; actor/result/state 17.
// Packed reference field view/local names/entry declarations/actor-result scope/action lifetime 13;
// presence-first branch 91; branch-local actor lifetime 96; shared entry exit 13.
// Native field view and all scope forms restored; both remaining load regions still differ.
// Early reference predicate/default-first/wide-result/presence predicate 109/108/111/109;
// complete current/both reference values 100/104; native word equality logical/bitwise 222/130.
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
		short action;
		if (*function_e70b0(actor->unknown018, &action) == 5)
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

// score 6 (was 6): the reset's zero fields (0x4ac, 0x504) are still stored before the NONE fields
// (0x5ac, 0x5b0) and the seed store still precedes the delay's doubling (retail doubles first); the
// only form that reproduces retail's order makes the actor pointer volatile, which is not allowed.
// score 6 (was 9): the reset's seven stores are written here in retail's order rather than through
// the shared helper. Left: the zero fields (0x4ac, 0x504) are still stored before the NONE fields
// (0x5ac, 0x5b0), and the seed store still precedes the delay's doubling (retail doubles first).
// score 9 (was 17): the unset reference read into a local before it is stored keeps the time-globals
// pointer load and the x87 scale spill in retail's order. Left: the seed store still precedes the
// doubling (the draw helper is in include/unknown_0259d0.h) and the reset's store order is fixed in
// include/slot_handler.h.
// score 17 (was 19): the delay's doubling written as sample + sample (2.0f * sample gives 19) puts
// the second draw's scale and the time-globals load in retail's order. Left: the seed store still
// precedes the doubling, the reset's store order is fixed in include/slot_handler.h, and the
// time-globals pointer still loads above the reference store.
// score 19 (was 19): the delay's doubling and its +3 still sit one slot later than retail
// (after the seed store and the conversion store); both draws first, the swapped operands and
// the inline draws all cost more. The reset's store order is fixed in include/slot_handler.h.
// score 19 (was 19): the frame and the x87 code agree. Left: the first draw's doubling and +3 sit
// one slot later in retail (after the seed store and the conversion store); retail stores the reset's
// NONE fields after its first three zero stores and before 0x4ac/0x504 (ours after all five); and
// our build loads the time-globals pointer above the reset's NONE stores and the reference store
// (a test build with that global's address taken keeps it below, as retail does, but retail's image
// holds no address use of it), which also moves the second draw's scale.
// The two draws are a [0, 1] sample each with its own scale and offset (the range helper gave an
// 8-byte two-real record or the wrong x87 schedule). Range-helper forms: named reals 53, doubles 39,
// double/real 49, real/double 52; the header random-range helper 55 (57 in the pair); the reset
// without its volatile casts 43 in either store order; named reals through a seconds-to-ticks inline
// 53 (parameter scaled in place 53); the existing actor_reset_state 40, with named reals 55, doubles
// 41, double/real 51, real/double 54; named reals declared first/at the top/wait first 55; product
// operands swapped, an explicit (real) tick conversion or the seconds-to-ticks inline 55 each; the
// slot data block through slot->unknownc 80; the existing out-of-line reset called 115 (its file is
// /Ob1); named reals with the reset's plain stores in the out-of-line order 55, products seconds
// first 53; existing reset call with native payload and original/scalar delay storage, ordinary/
// forced reset declaration 121/126/121/126; whole slot 115/125/115/125; aggregate draws through a
// seconds-to-ticks inline 35; an in-file forceinline/inline range helper with named reals 53 (55 with
// actor_reset_state); raw-generator `(real)(3.0 + 2.0 * <random>)` forms 26 (qword constants; 24 with
// actor_reset_state), float constants 49, `(double)` on the random alone 26; the record's fields
// swapped 39; the time globals in a local used twice 35; products value first 35; the reference
// stored before the reset 71; the 0x5b0 store without its volatile cast 36, 0x5ac and 0x5b0 38.
// Round 20 (sample form): with the hand-written reset 14, with actor_reset_state 19 (kept); the
// samples inside the expressions 54; products value first 19; the state pointer set at the reset 19;
// the reference stored before the reset 60; the existing 0x1469f0 body for the conversions 19.
// Screened (one-file LTCG): every spelling of the scales and offsets gives the same code; both
// samples drawn first 19; the tick count in a local 43; the random step or the sample conversion
// through a local 87/23. Round 22: the second scale after the reference store 35; the second draw
// and its scale after the reset 75; the slot written through the parameter 19; the existing
// slot_random() for both draws 61; double delay/wait 45. Round 24: actor_reset_state's stores in
// retail's emitted order (50c, 5b4, 5b6, 5ac, 5b0, 4ac, 504) 19 (same code; 0x1b09b0 0 -> 4); the real
// out-of-line reset called with `inline` on its definition 19 (then inlined at every caller); the first
// draw inside its expression 46; both draws, wait before delay 53; the reference stored after the
// tick stores 56. Retail here, in 0x1b3ee0 and in 0x1c0430 keeps 0x4ac/0x504 after the NONE stores; our
// build sinks the NONE stores past them in all 240 statement orders screened (compile-only, 0x1b3ee0).
// @retail 0x1befd0
bool __stdcall function_1befd0(long actor_index, s_slot *slot)
{
	s_slot_0a *state = (s_slot_0a *)slot;
	real sample = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
	real delay = 3.0f + (sample + sample);
	real sample2 = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
	real wait = 0.5f * sample2;

	s_actor_view *actor = actor_get(actor_index);
	actor->unknown50c = false;
	actor->unknown5b4 = 0;
	actor->unknown5b6 = 0;
	actor->unknown5ac = NONE;
	actor->unknown5b0 = NONE;
	actor->unknown4ac = 0;
	actor->unknown504 = 0;
	s_reference unset_reference = g_470fa0;
	state->reference = unset_reference;

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
