// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1AEB60.CPP: the slot handlers of types 0xe and 0xf
   (0x47de20..0x47dec0) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_1fb7e0.h"
#include "unknown_26b230.h"
#include "unknown_11c010.h"
#include "props.h"

/* the state of a slot of type 0xe */
struct s_slot_0e
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d[0x40 - 0xd];
};

/* the state of a slot of type 0xf */
struct s_slot_0f
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short ticks;
	short timer;
	bool unknown12;
	bool unknown13;
	bool unknown14;
	bool unknown15;
	bool unknown16;
	bool unknown17;
	s_type_c3b527 point;
	vector3f facing;
	char unknown34;
	byte unknown35[0x40 - 0x35];
};

struct s_prop_datum_0f
{
	byte unknown00[0x24];
	short unknown24;
	byte unknown26;
	char unknown27;
	byte unknown28[0x3c - 0x28];
};

bool function_1f8660(long index);
void function_1f86a0(long index);
bool function_1f4f40(long actor_index, vector3f const *facing, short unknown, s_type_c3b527 const *point, bool face_prop);
short __stdcall function_1aec30(long actor_index, short level, bool active);
bool __stdcall function_1af810(long actor_index, s_slot *slot);
void __stdcall function_1afb30(long actor_index, s_slot *slot);
void __stdcall function_1afcf0(long actor_index, s_slot *slot);

/* ---- slot type 0xe ---- */

// @retail 0x1aeb60
short __stdcall function_1aeb60(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->prop_index != NONE && prop_node_get(actor->prop_index)->type == 1)
		result = 3;
	return result;
}

// @retail 0x1aebb0
short __stdcall function_1aebb0(long actor_index, s_slot *slot, bool active)
{
	return function_1aeb60(actor_index) > 0 ? g_46fbe8 : g_46fbe4;
}

// @retail 0x1aebd0
bool __stdcall function_1aebd0(long actor_index, s_slot *slot)
{
	s_slot_0e *state = (s_slot_0e *)slot;
	long i = 0;

	actor_get(actor_index)->unknown084 = 4;
	s_actor_view *actor = actor_get(actor_index);
	actor->unknown3fe = 3;
	do
	{
		actor->unknown400[i].reference = g_470fa0;
		i++;
	}
	while (i < 4);
	state->unknown0c = false;
	return true;
}

short __stdcall function_1a79e0(long actor_index, short level, bool active);
short actor_behavior_find_level(long owner_index, short type);
real function_30bf0(vector3f *vector);
struct s_clump_object_view;
s_clump_object_view *function_26bdd0(s_iterator *iterator);

// @retail 0x1aec30
short __stdcall function_1aec30(long actor_index, short level, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = function_1a79e0(actor_index, level, active);
	long prop_index = actor->prop_index;
	if (prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(prop_index);
		if (result != g_46fbe4 && node->unknown24 >= 1 && node->unknown24 <= 2)
			*(short *)((byte *)actor + 0x306) = 0;
	}
	long weapon_index = function_1e1f20(actor_index);
	if (weapon_index != NONE)
	{
		byte *entry = (byte *)function_1e5280(actor_index, ai_object_get(weapon_index)->definition_index);
		if (entry && (*entry & 2))
			actor->unknown449 = true;
	}
	bool *state = &((s_slot_0e *)((byte *)actor + 0x90 + level * sizeof(s_slot)))->unknown0c;
	if (!actor->unknown5d0 && actor->unknown07c != NONE && !actor->unknown229 && actor->unknown26c == NONE && prop_index != NONE)
	{
		if (actor->unknown040)
		{
			s_prop_state_view *prop = prop_node_state(prop_node_get(prop_index));
			real minimum = *state ? 2.0f : 2.5f;
			vector3f direction;
			vector3d_from_points3d(&actor->position, &prop->position, &direction);
			if (function_30bf0(&direction) > minimum)
			{
				real furthest = -50.0f;
				bool found = false;
				s_iterator iterator;
				function_26bda0(actor->unknown07c, &iterator);
				s_actor_view *other = (s_actor_view *)function_26bdd0(&iterator);
				while (other)
				{
					if (other != actor && other->unknown26c == NONE && actor_behavior_find_level(iterator.index, 0xe) != NONE)
					{
						vector3f offset;
						vector3d_from_points3d(&actor->position, &other->position, &offset);
						if (9.0f > length_sq3f(&offset))
						{
							vector3f facing;
							vector3d_from_points3d(&actor->position, &other->position, &facing);
							real distance = dot3f(&direction, &facing);
							found = true;
							if (distance > furthest)
								furthest = distance;
						}
					}
					other = (s_actor_view *)function_26bdd0(&iterator);
				}
				if (found)
				{
					if (*state)
					{
						if (furthest >= 2.0f)
							*state = true;
					}
					else if (-0.25f >= furthest)
						*state = true;
				}
			}
			else
				*state = false;
		}
	}
	else
		*state = false;
	actor->unknown449 = actor->unknown449 | (*(bool *)actor->unknown3dc | *state);
	return result;
}

/* ---- slot type 0xf ---- */

// @retail 0x1aef40
short __stdcall function_1aef40(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (function_1e2030(actor_index) && actor->prop_index != NONE)
	{
		s_prop_datum_0f *prop = (s_prop_datum_0f *)prop_node_get(actor->prop_index);
		if (prop->unknown24 >= 1 && prop->unknown24 <= 2 && prop->unknown27 >= 1)
			result = 3;
	}
	return result;
}

PRIVATE __forceinline void function_1aefc1(real arg_0, short *arg_1)
{
    long local_0;
    __asm
    {
        fld arg_0
        fistp local_0
    }
    *arg_1 = (short)local_0;
}

// @retail 0x1aefc0
bool __stdcall function_1aefc0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0f *state = (s_slot_0f *)slot;
	dword *seed = &g_4e7408->unknown0;

	*seed = 1664525 * *seed + 1013904223;
	real local_0 = (real)(*seed >> 16) * (1.f / 65535.f);
	real seconds = 3.0f + 7.0f * local_0;
	real timer = (real)g_510c54->field_2_3 * 0.2f;
	function_1aefc1(timer, &state->timer);
	real ticks = g_510c54->field_2_3 * seconds;
	function_1aefc1(ticks, &state->ticks);
	state->unknown12 = false;
	state->unknown13 = false;
	state->unknown34 = false;
	state->unknown16 = false;
	state->unknown17 = false;
	state->unknown14 = false;
	state->unknown15 = !actor->unknown5d4;
	return true;
}

// @retail 0x1af0a0
short __stdcall function_1af0a0(long actor_index, s_slot *slot, s_slot *arg_2)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0f *state = (s_slot_0f *)slot;
	s_slot_0f *local_0 = (s_slot_0f *)arg_2;
	short finished = g_46fbe4;
	short result;

	if (actor->prop_index != NONE)
	{
		s_prop_datum_0f *prop = (s_prop_datum_0f *)prop_node_get(actor->prop_index);
		result = g_46fbe8;
		if (state->unknown12)
		{
			local_0->timer = 3;
			local_0->ticks = 2;
			local_0->unknown0d = true;
			local_0->unknown12 = true;
			local_0->unknown0c = true;
			result = 5;
		}
		else if (prop->unknown27 < 2 &&
			(REFERENCE_EQUAL(actor->unknown418, g_470fa0) || !function_1f8660(actor_index) || state->unknown13))
		{
			result = finished;
		}
	}
	else
		result = finished;
	return result;
}

/* the actor's weapon entry (function_1e5240) as 0x1af530 reads it */
struct s_weapon_entry_0f
{
	byte unknown00[0x14];
	long unknown14;
	long unknown18;
	byte unknown1c[0x24 - 0x1c];
	long unknown24;
	long unknown28;
	long unknown2c;
	long unknown30;
};

/* the squad's variant (function_272a00) */
struct s_squad_variant_0f
{
	byte unknown00[0x24];
	short unknown24;
	byte unknown26[0x31 - 0x26];
	char unknown31;
};

real function_1f8940(long actor_index);
void *function_272a00(long actor_index);

// @retail 0x1af530
bool function_1af530(long actor_index, long *b, long *a)
{
	s_weapon_entry_0f *entry = (s_weapon_entry_0f *)function_1e5240(actor_index);
	bool result = false;

	if (entry)
	{
		s_squad_variant_0f *variant = (s_squad_variant_0f *)function_272a00(actor_index);

		*a = entry->unknown18;
		*b = entry->unknown14;
		if (variant)
		{
			short index = variant->unknown31 <= 0 ? variant->unknown24 : variant->unknown31 - 1;

			switch (index)
			{
			case 1:
				*a = entry->unknown28;
				*b = entry->unknown24;
				break;
			case 2:
				*a = entry->unknown30;
				*b = entry->unknown2c;
				break;
			}
		}
		result = true;
	}
	return result;
}

static __forceinline long real_to_long(real value);
short *function_e70b0(long unit_index, short *value);

// score 49 (was 49): frame agrees; ECX and EDX are swapped through the whole body (retail: index EDX,
// actor ECX), down to the final call's argument setup. Existing state query
// ordinary/inline/forced 138 each; query with long timer 143; timer/mode 143; mode 137; readonly actor
// 138; readonly records 138.
// Earlier:
// retail also keeps the random scale and the range as two fmuls. Slot view first 79; one actor
// pointer for the last test 138; marker local in the last block 80; `--timer == 0` 79; prop index
// local 79.
// Native countdown/mode/both 134/80/138; shared range 49; its native countdown/mode/both 127/64/129; unit
// reference 49; short/early short countdown 49/123; actor reference and read-only node/unit/actor/all
// views 49 each; final guard 49; direct shared product/conversion 49/49; shared low-half index with
// pointer/reference 127/127. Timer test combined as in 0x1b7c70 49; `--state->ticks <= 0` 49;
// slot cast declared first 49; constant first in the time test 49; timer record by pointer
// addition 49 (the same code each time).
// Nested prop/clock/tick names singly, in all pairs and together 49 each; early uninitialized tick declaration 49.
// Actor getter boundary 49; getter observations at timer/prop/both 117/115/140;
// unit/timer-unit/prop-unit/all phases 120/117/117/132; all computed receiver forms rejected.
// Slot data block through slot->unknownc 49; existing unit action-state helper call 49 and existing prop
// getter 49 (both kept: they replace hand-written copies, same code); nested final actor-state test 49;
// named random delay 49.
// Round 404: Native slot data block, then countdown/mode/elapsed combinations and entry cast order, all
// eight 49 at 443/443. Data block canonicalizes unchanged; complete original body and type restored.
// Actor/index and final-call allocation remain.
// Round 7 (lane 15): random scale times the tick rate 49; `(real)` tick rate 49; action local at function
// scope 49; slot cast before the actor 49 (identical code each time); a permuter run ended at its
// 2400 s timeout with no result (its full-image baseline takes most of that).
// Round 114: final mode test as a call to the existing 0x1f86f0 138 (kept out of line); through the
// existing actor pointer 132; nested timer test 49; nested final test 49; named action value 49;
// a 26-try permuter run found nothing below 49.
// Round 38 (lane 9): the final mode test as a call to the existing 0x1f86f0 with an `inline` or
// `__forceinline` declaration here 138/138 (LTCG still keeps it out of line; its file was off limits);
// `now` read before the node/prop lookups 97; `action` declared at the top 49; `10.0f <` elapsed test 49.
// Round 100: actor declared first and assigned after the slot cast 49; const actor pointer 49; a named
// char for the byte flag tested and passed to 0x1f4f40 53. Diagnostics: without the 0x1f86a0 call or
// without actor_index in 0x1fb7e0's ECX argument the index still takes ECX (135/128), so neither use
// decides the tie.
// Round 16: every instruction is retail's except the ECX/EDX roles (the actor pointer and its
// reloads, and so every temporary beside it); dropping the index from the 0x1f86a0, 0x1fb7e0 or
// 0x1f4f40 call, or the final mode read through `actor`, leaves the index in ECX. The final mode
// read as a raw indexed field 49; the action query through a fresh actor lookup 120; the timer
// test as `== 0` 49; the ticks test as `< 1` 108; the timer guard as `>= 1` 126.
// Round 10 (lane 13): the final test as a call to the existing 0x1f86f0 with `inline` on its definition
// (inlined as in retail, its other callers unchanged) 49 (the same code); with the slot cast first 49;
// with the action state read after the call 49; the time read before the lookups 97; the node getter
// at both uses 54. Retail's coloring looks as if ECX were taken at entry (index EDX, data ECX).
// Round 99 (lane 6): the tick store through the existing 0x1aefc0 helper (fistp into a short) 49
// (identical code); `rate * elapsed` 49; slot cast first with the actor assigned separately 49; a
// unit-index local for the action-state read 49; the prop stamp before the event call 133.
// Round 3 (lane 17): the actor looked up at each use in the timer test 152 (465 bytes); a named elapsed
// tick count in the prop test 49; the elapsed seconds named 49; a named object index for the event
// call 49; node and prop declared at function scope 49.
// Round 7 (m3 lane 2): matched 0x1aeb60/0x1af0a0 here give the index ECX and the data EDX as ours does;
// the index copied into a local 49; the prop index named for its test and lookup 49; the index read once
// through its address 49; `2 == ...` for the final mode test 49; `state->timer == 0` 49.
// Round 11 (m3 lane 2): the two final tests and the action query as one condition (action declared
// first) 49; the prop time product in a named elapsed value 49; the timer's zero test nested over
// the actor flag test 49; `if (--state->ticks < 1)` 108 (445 bytes); the final mode read through a
// second named actor lookup 49. Retail: 8 matched functions load an actor index into EDX with the
// data base in ECX (0x1a80e0, 0x1b5aa0, 0x258660, ...), 15 into ECX (0x1aeb60, 0x1a93c0, ...); no
// frame or prologue feature separates the two groups.
// Round 132 (an LTCG env link with 0xe70b0's object reproduces it): with 0xe70b0 left out of line the
// first 97 instructions are retail's (index EDX, actor data ECX), so the swap follows the inlined
// action-state read. At the call: its result in a pointer local 110 (437 bytes); called alone and the
// local compared 49; `7 !=` 49; the unit index in a local 49; `state->unknown34 != 0` 49.
// Round 110 (fast): the final block as a guard return 49; the action query's out value tested in place of
// the returned pointer 49; `7 != ...` 49; `0 < state->timer` 49 (the same code each time). Diagnostic, not
// kept: the actor address written offset first (`(index & 0xffff) * size + data`) 49, so the operand order
// of the address expression does not decide the ECX/EDX tie either.
// Round 98 (lane 8; LTCG env link with the ballast and 0xe70b0's object, equal to the full listing):
// the swap comes from the LTCG-inlined 0xe70b0 action query. Linked without 0xe70b0's object (the
// query stays an external fastcall call) the same source gives retail's roles (index EDX, actor ECX,
// temporaries EDX) for the first 97 rows; 0xe70b0 written with a unit local, or the query spelled out
// in place (diagnostics), keep the swap. Trials: `(short)state->unknown34` 49; `short action = 0;`
// 49; the timer block as one `> 0 && !--timer && ...` condition 49 (identical code each time); the
// query result in a named `short const *` 105 aligned 49 (436 bytes); re-measured in the env: the
// query called, then `action != 7` 49, slot cast before the actor 49 (identical code).
// Round 24 (lane 14): every differing row is ours with the scratch registers rotated (ECX/EDX at the
// entry, EAX/EDX and ECX/EAX in the later blocks), as if retail's allocation started one register
// further on. The definition moved after 0x1afcf0 in this file 49 (fast and plain; other functions
// unchanged); the existing typed actor_datum_get for the first lookup 49, for both lookups 49; the
// delay product written random times ticks 49 (the same code each time).
// Round 101 (lane 8; exact LTCG harness, 42 = 49): BLOCKED-BY 0xe70b0's inlined shape (its file is
// another lane's this round). 0xe70b0 bodies tried in the harness only: an actions-pointer local 49, a
// result-pointer local 105 (436 bytes), a header-pointer local 54; here `if (--state->ticks <= 0)` 49
// (same code), a named object index for the event call 49.
// Round 181 (lane 3): the rotation reaches the final call: retail keeps the 0x34 byte in DL and
// loads ECX (the facing argument) before pushing it, ours keeps it in CL and pushes it first, so the
// byte's colour and the actor's (ECX in retail) are one decision. The definition moved to the top of
// the file 49; the action state read into a short value local 49; the random range given file and
// line as 0x1bf0f0 does 49; a `__forceinline` declaration of 0xe70b0 here 49; an explicit short
// conversion of the byte argument 49 (identical code each time).
// Round 30 (lane 14, fast): `if (--state->ticks <= 0)` 49; the game time read between the node and prop
// lookups 96; `if (!--state->timer && ...)` inside the timer guard 49; the action query tested before the
// mode test (diagnostic: the index still takes ECX) 118; the prop looked up at each use 153 (468 bytes).
// Round 55 (fast): `now` declared at function scope 49; `long const now` with a named `real const elapsed`
// and `10.0f < elapsed` 49; the final mode read through the entry actor pointer 132 (423 bytes);
// `state->ticks -= 1;` 49; the unit index through a `long const &` 49.
// @retail 0x1afb30
void __stdcall function_1afb30(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0f *state = (s_slot_0f *)slot;

	if (state->timer > 0)
	{
		state->timer--;
		if (!state->timer && actor->unknown227 && !actor->unknown229)
			function_1f86a0(actor_index);
	}
	state->ticks--;
	if (state->ticks <= 0)
	{
		if (actor->prop_index != NONE)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);
			s_type_76cf92 *prop = prop_get(node->unknown08);
			long now = g_510c54->game_time;
			if ((real)(now - prop->unknown38) * g_510c54->rate > 10.0f)
			{
				function_1fb7e0(0x96, actor_index, NULL, node->object_index, NONE);
				prop->unknown38 = now;
			}
		}
		state->ticks = (short)real_to_long(g_510c54->field_2_3 * function_259d0(&g_4e7408->unknown0, NULL, 0, 3.0f, 10.0f));
	}
	if (state->unknown16 && !state->unknown17 && state->unknown34 && actor_get(actor_index)->unknown504 == 2)
	{
		short action;
		if (*function_e70b0(actor->unknown018, &action) != 7)
		{
			function_1f4f40(actor_index, &state->facing, state->unknown34, &state->point, true);
			state->unknown17 = true;
		}
	}
}

// @retail 0x1afcf0
void __stdcall function_1afcf0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0f *state = (s_slot_0f *)slot;

	if (actor->prop_index != NONE)
	{
		s_prop_datum_0f *prop = (s_prop_datum_0f *)prop_node_get(actor->prop_index);

		actor->unknown41c = 2;
		actor->unknown420 = 2;
		actor->unknown4a2 = true;
		if (prop->unknown27 >= 1)
		{
			actor->unknown488 = true;
			actor->unknown41c = 4;
			actor->unknown420 = 2;
		}
		else
		{
			if (!function_1f8660(actor_index) || 2.0f > function_1f8940(actor_index))
			{
				actor->unknown41c = 4;
				actor->unknown420 = 2;
			}
			if (actor->unknown229)
				actor->unknown482 = true;
		}
		if (actor->unknown5d0)
			state->unknown15 = false;
		if (state->unknown15)
			actor->unknown449 = false;
	}
}

/* ---- the handlers ---- */

s_slot_child g_46f440[27] =
{
	{0x2e, 1, NONE, {0}, 0.0f, 0, 0},
	{0x30, 1, NONE, {0}, 0.0f, 0, 0},
	{0x55, 1, NONE, {0}, 0.0f, 0, 0},
	{0x56, 1, NONE, {0}, 0.0f, 0, 0},
	{0x57, 1, NONE, {0}, 0.0f, 0, 0},
	{0x35, 1, NONE, {0}, 0.0f, 0, 0},
	{0x34, 1, NONE, {0}, 0.0f, 0, 0},
	{0x15, 1, NONE, {0}, 0.0f, 0, 0},
	{0x50, 1, 13, {0}, 1.0f, 0, 0},
	{0x33, 1, NONE, {0}, 0.0f, 0, 0},
	{0x13, 1, NONE, {0}, 0.0f, 0, 0},
	{0x14, 1, NONE, {0}, 0.0f, 0, 0},
	{0x12, 1, NONE, {0}, 0.0f, 0, 0},
	{0x4f, 1, NONE, {0}, 0.0f, 0, 0},
	{0x11, 1, NONE, {0}, 0.0f, 0, 0},
	{0x10, 1, NONE, {0}, 0.0f, 0, 0},
	{0x31, 1, NONE, {0}, 0.0f, 0, 0},
	{0xf, 1, NONE, {0}, 0.0f, 0, 0},
	{0x58, 1, NONE, {0}, 0.0f, 0, 0},
	{0x52, 1, NONE, {0}, 0.0f, 0, 0},
	{0x51, 1, NONE, {0}, 0.0f, 0, 0},
	{0x1b, 1, NONE, {0}, 0.0f, 0, 0},
	{0x22, 1, NONE, {0}, 0.0f, 0, 0},
	{0x16, 1, NONE, {0}, 0.0f, 0, 0},
	{0x5f, 0, NONE, {0}, 0.0f, 0, 0},
	{0x60, 1, NONE, {0}, 0.0f, 0, 0},
	{0x5, 1, NONE, {0}, 0.0f, 0, 0},
};

s_slot_handler_1 g_47de20 =
{
	{
		0xe, 1, 0, -2, 0,
		function_1aeb60, function_1aebb0, function_1aebd0, 0, 1, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	function_1aec30, 27, g_46f440
};

s_slot_handler_2 g_47de70 =
{
	{
		0xf, 2, 0, -2, 0,
		function_1aef40, (t_slot_evaluate)function_1af0a0, function_1aefc0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1af810, function_1afb30, function_1afcf0
};

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *arg_0, long arg_2, point3f *arg_1);
long function_baf40(long arg_0);
struct s_object_motion_view;
void function_2640c0(long arg_0, s_object_motion_view *arg_1);
void function_1e3b00(long arg_0, long arg_1, point3f const *arg_2, void const *arg_3, void const *arg_4, point3f *arg_5);
struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long arg_0, point3f const *arg_1, vector3f const *arg_2, long arg_3, long arg_4, s_collision_result_1697c0 *arg_5);

struct s_1af5c0
{
	byte field_0[4];
	real field_4;
	byte field_8[0x24 - 8];
	short field_24;
	byte field_26[0x5c - 0x26];
};

struct s_1af5c1
{
	byte field_0[0x2c];
	short field_2c;
	byte field_2e[2];
	point3f field_30;
};

struct s_1af5c2
{
	byte field_0[3];
	byte field_3;
	byte field_4[4];
	s_ai_object *field_8;
};

struct s_1af5c3
{
	byte field_0[0x14];
	long field_14;
};

struct s_1af5c4
{
	byte field_0[0x30];
	byte *field_30;
};

struct s_1af5c6
{
	long field_0;
	point3f field_4;
	point3f field_10;
};

// score 20 (was 20): the index is masked in EAX where retail masks it in ECX; the argument is
// passed in the other register.
// Also: retail loads the 0x1697c0 object argument before the first ray difference, keeps the distance in
// the slot at +0x14 (ours +0x10) and loads it first in the hit product.
// Round 15: the ray vector built in the call's argument list (the point-difference helper returning its
// output) 20; the existing distance3d for the distance 31; the second object index in its own local 20;
// the second header read inside the test 20; the hit fraction read into a local after the test 19.
// Round 11 (lane 18): the tail from the 0x11c010 test on is 0x1c8df0 with constant arguments (no side
// probes, the 0x15808c2f mask, the 0x20 bit cleared for an actor with +0x26c set, no out-pointer):
// a call to 0x1c8df0 330 (359 bytes; LTCG keeps it out of line, also with a __forceinline
// declaration here 330, or with auto-inlining allowed 330, or with `inline` on 0x1c8df0's definition 321); the distance through the existing
// vector3d_from_points3d + magnitude3d 20 (kept: it replaces the hand-written difference and square
// root), with the hit fraction read into a local after the test 19; the ignore index copied before the
// ray vector 20; the ignore index chosen by a plain if/else 140 (fast); the copied point and the 2640c0
// buffer in one block and the probe's vector and result in another 44 (frame stays 0x80, but the point
// copy goes below the NONE/source pair), the same with the point copy renamed 44; one category result
// in 0x1c8df0's if/else-if form 20.
// @retail 0x1af5c0
long function_1af5c0(long arg_0, long arg_1, point3f const *arg_2, long arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_node_view *local_1 = prop_node_get(arg_1);
	s_1af5c1 *local_2 = (s_1af5c1 *)function_25d690((s_prop_datum *)local_1);
	long local_19 = local_1->object_index;
	s_1af5c2 *local_3 = &((s_1af5c2 *)g_4e0300->data)[local_19 & 0xffff];
	s_ai_object *local_4 = local_3->field_8;
	s_1af5c0 local_7;
	long local_8;
	s_1af5c6 local_9;
	point3f &local_5 = local_9.field_10;

	local_9.field_0 = NONE;
	local_5 = *arg_2;

	function_2640c0(local_19, (s_object_motion_view *)&local_7);
	local_19 = local_1->object_index;
	local_3 = &((s_1af5c2 *)g_4e0300->data)[local_19 & 0xffff];
	if (local_3->field_3 != 0 || (local_8 = ((s_1af5c3 *)local_4)->field_14) == NONE)
		local_8 = local_19;
	local_8 = function_baf40(local_8);
	if ((short)arg_3 == 0)
	{
		local_9.field_4 = local_5;
		local_9.field_4.z += 0.05f;
	}
	else
		function_1e3b00(local_0->unknown018, arg_3, &local_5, NULL, NULL, &local_9.field_4);
	long local_10 = function_14a280(g_4e033c, 0, &local_9.field_4);
	if (local_10 != NONE)
		local_10 = *(short *)(((s_1af5c4 *)g_4e0348)->field_30 + local_10 * 8);
	else
		local_10 = local_9.field_0;
	bool local_11 = local_0->unknown26c != NONE;
	short local_15 = local_2->field_2c;
	if (!clusters_visible_11c010(local_10, local_15))
		return 4;
	local_7.field_24 = NONE;
	long local_12 = 0x15808c2f;
	if (local_11)
		local_12 = 0x15808c0f;
	vector3f *local_13 = (vector3f *)&local_5;
	vector3d_from_points3d(&local_9.field_4, &local_2->field_30, local_13);
	if (!function_1697c0(local_12, &local_9.field_4, local_13, local_8, NONE, (s_collision_result_1697c0 *)&local_7))
		return 0;
	vector3f local_20;
	vector3d_from_points3d(&local_9.field_4, &local_2->field_30, &local_20);
	real local_14 = magnitude3d(&local_20);
	if (1.0f > local_14)
		return 4;
	return 1.0f > local_14 * local_7.field_4 ? 2 : 4;
}
