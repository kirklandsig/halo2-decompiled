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
bool function_1f4f40(long actor_index, short unknown, s_type_c3b527 const *point, bool face_prop, vector3f const *facing);
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
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);

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
		if (function_e70b0(actor->unknown018).value != 7)
		{
			function_1f4f40(actor_index, state->unknown34, &state->point, true, &state->facing);
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

short __stdcall function_1c8df0(short arg_2, short arg_3, point3f const *arg_4, long arg_0,
 point3f const *arg_1, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9);


// MiniMax H (11-19): no new form, no lower score (5): the hit product still loads the hit fraction first where retail loads the distance, and the ignore index still loads after the first ray difference where retail loads it before; the product's order needs a one-use field copy and the early load needs the two-point probe inlined, so neither is kept.
// MiniMax I (11-19): no lower score (5, as before): retail's hit product loads the magnitude out of its slot into
// XMM0 and multiplies by the hit distance in its own slot, while ours loads the distance first. Reading the hit
// distance into a local of its own before the magnitude is computed does give retail's order and measures 3, but
// that is a one-use field copy, which is only kept with an exact match, so it is not kept; the 0x1697c0
// ignore-index reload still lands after the first ray difference in ours.
// score 5 (was 20): retail loads the 0x1697c0 ignore-index argument (`mov ecx, [esp + 0x14]`) ahead of the first ray
// difference, ours after it, and retail's hit product loads the distance first (ours the hit fraction). Round 137
// fixed the other sites (see its entry at the end of this note).
// Before round 137, at 20: the index is masked in EAX where retail masks it in ECX; the argument is
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
// Round 101 (plain): NONE-initialised resolved index 20; repeated actor lookup 174; typed object
// pointer 20; word second subscript 132; second lookup as pointer arithmetic 20; unsigned datum index 20;
// early distance-vector declaration 20; direct actor predicate 115 (568 bytes); sqrt(length_sq3f) 20.
// Round 50 (lane 13, plain; --fast misbuilds this function, 71 at 598 bytes): hit product operands swapped 20 (same code);
// distance renamed a_distance, z_distance 20; distance declared apart from its assignment 20; the mask selection below the ray
// vector 37; the result field store below the ray vector 40; the flag test named before the or 20.
// Round 121 (plain): retail shares two stack homes: the point copy with the ray vector (+0x28) and the 2640c0 buffer with the
// hit result (+0x34). A private inline point-pair collision helper with its own vector (as 0xd6f90's) fixes the object argument's
// load order, 28 (frame 0x8c); the point copy and both buffers in scoped blocks on it, source pair struct trimmed to 16 bytes 44
// (frame 0x80, but the 12-byte shared home sorts below the 16-byte pair; retail has the pair lower), the same renamed either way
// 44, the pair declared first 44, the distance vector scoped 44, distance3d 50; 16 extra pair reads leave that order
// (compile-only with the magnitude inlined reproduces it); the helper taking the vector as an out pointer on the original frame
// 24, with the fraction local 24, with the distance vector scoped 24. A separate NONE long plus 12-byte point sorts as retail
// compile-only, but the long folds away, also through a pointer local 159 (565 bytes); retail keeps it in memory.
// Round 86 (plain): k identity locals on the input point, k = 1..8, 20 each (the remaining sites do not follow symbol numbering); the second
// header read written in the test 20; the header flag in a byte local 20; the resolved index as if/else 20; a named hit product with the distance
// first 20; the ray vector in the call's argument list 20; the hit fraction in a local after the test, or declared at the top, 19; with one category
// result as well 106 (601 bytes); the distance compared as `local_14 < 1.0f`, as in 0x1c8df0, 20; the distance declared before the second vector 20.
// Round 216 (m3 lane 3, plain): the existing ai_object_get/object_header_get for the two header lookups 20 (same code);
// the ignore index declared first 20 (same code); the ray in a fresh scoped vector, as the inlined query's own local,
// 28 (frame changes, first difference +0x2); the distance declared with the other locals 20; the object index read
// from the prop node at each use after 0x2640c0 20. Left: retail's tail is 0x1c8df0 inlined with constant arguments
// (only this call site; LTCG keeps ours out of line), so the mask temp (EAX, retail ECX), the early ignore-index load
// and the distance's slot follow that inlined body's temporaries.
// Round 71 (lane 14, plain): the ignore index chosen without the assignment inside the condition, as a nested if/else
// (kept: identical code), as NONE then two ifs, or as a ternary with a NONE fallback test, 20 each; the object index first
// with the actor field taken when the header flag is clear and the field is set 29; the header flag tested as a bool 20.
// Round 73 (lane 14, plain): 0x1c8df0's noinline dropped with a __forceinline declaration here and the tail as the call
// (diagnostic) 82 (589 bytes: inlined here, but matched 0x1b2770 then inlines it too, 656); the ignore index named before
// the tail 20 (same code); the mask as a ternary on the flag 40; the miss and distance tests as 0x1c8df0's short category
// with one exit 109 (603 bytes); the first element read straight into local_4 and the second lookup in its own pointer 20.
// Round 64 (m3 lane 2, plain): the hit fraction read into a local after the test, with the distance and its vector in a
// nested block, 19 (the product loads the distance first as retail; not kept: a one-use field copy); on it: the actor local
// renamed to sort last 19 (same code: names do not move the distance's slot); the mask's 0x20 bit cleared with `&= ~0x20`
// 19 (same code); the object index overwritten by the +0x14 field and passed to 0xbaf40, as matched 0x1b0400 does, 28; a
// copy of the ignore index made before the cluster test, where the inlined call's argument copy sits, 19 (same code, the
// copy propagates); the whole tail in one nested block 19 (same code). Left as before: the second header mask in EAX
// (retail ECX), the ignore index loaded after the first subtraction (retail first), the distance at +0x10 (retail +0x14).
// Round 95 (m3 lane 10, plain): the ignore index's default copied before the second header lookup 29 (the mask temp moves
// to ECX as retail's, but the default then holds EAX and the flag byte goes to BL); the flag held in a named bool 20; the
// second lookup through a word-converted index 132 (movzx); through byte-offset element arithmetic 20; the flag test
// negated with the arms swapped 20. Our `and eax, 0xffff` is one byte shorter than retail's ECX form, so everything after
// +0x8c sits one byte early: the flagged call, global and constant lines after it are that shift, not separate causes.
// Round 86 (m3 lane 13, plain): the hit fraction read into a local after the test, re-measured, 19 (the product then
// loads the distance first; a one-use field copy, not kept); on it: the 0xbaf40 result in its own variable 19 (same
// code); the distance declared with the other locals and its vector in a nested block 19 (same code: the distance
// stays at +0x10); the second difference written into the ray vector's storage again 58 (593 bytes: the stores come
// back); the actor's +0x26c test written where the mask is chosen, no early bool, 115 (568 bytes). The three sites
// left (second header mask in EAX, ignore index loaded after the first subtraction, distance at +0x10) all sit in the
// part retail gets from inlining 0x1c8df0, which LTCG keeps out of line here; a file-local reduced copy of it is ruled
// out (no inline twin of an existing function).
// Round 157 (m3 lane 7, plain): the cluster test passing `local_2->field_2c` directly 20 (same code); written as 0x1c8df0
// writes it (`local_10 != NONE && local_15 != NONE && !function_11c010(...)`) 22. A register-masked aligned diff leaves only
// the ignore index's load (retail ahead of the first ray subtraction) and the 1-byte `and eax, 0xffff` (retail ECX).
// Stopped there: rounds 86 and 216 place the remaining sites in 0x1c8df0's inlined body, and lane 14 r73 shows matched
// 0x1b2770 lost once 0x1c8df0 inlines, so its noinline stays (BLOCKED-BY 0x1c8df0's call-site inlining).
// Round 101 (m3 lane 11, fast, which now builds this function as the full check does, with the file's functions,
// 0x1af810, 0x1b2770 and 0x1c8df0): the query's miss flag and fraction local in 0x1c8df0's form (`missed = true` / `missed =
// false; fraction = field_4`, then `local_14 < 1.0f` and `local_14 * fraction < 1.0f`) 19 (the product then loads the
// distance first; a flag set in a test plus a one-use field copy, not kept); the product test alone as `local_14 * field_4
// < 1.0f` 20 (same code); the hit arm first with the miss return after it 54; the actor flag read after the cluster test 59
// (573 bytes); the cluster index as a `short` with the query result in its own long 21 (the table read becomes `mov ax`).
// Diagnostic (not kept): the ignore index's address taken 136 (frame 0x84). Left as before: the mask temp (EAX, retail ECX),
// the ignore index's load (retail at the head of the block after the mask, as an IR load the scheduler hoists; ours right
// before its push) and the distance's home (+0x10, retail +0x14).
// Round 183 (m3 lane 6, fast with 0x1af810, 0x1b2770 and 0x1c8df0, which reads 20 as the full build): a 600 s
// permuter run with those three as context (40 tries) best 20; the raised source z as `local_5.z + 0.05f` 22; the
// cluster index chosen by one conditional expression 20 (same code); the prop state pointer const 20 (same code); the
// mask's 0x20 bit cleared with `&= ~0x20` on this form 20 (same code). Still BLOCKED-BY 0x1c8df0's call-site inlining.
// Round 90 (m3 lane 15, fast with the file's functions, 0x1af810, 0x1b2770 and 0x1c8df0; control 20): the actor's +0x26c bool
// read right after the 0xbaf40 call 131; the object index read from the prop node for the first lookup and the 0x2640c0
// argument, with `local_19` declared at the reload, 39; the ignore index defaulted to the object index and replaced by one
// `!flag && field != NONE` test 29 (the mask temp moves to ECX as retail's, but the flag byte goes to BL, as round 95's
// copy); the product test as an if with returns 20 (same code); the leaf index named and the cluster chosen by a ternary
// 20 (same code). Diagnostic (listed form): the distance renamed `distance` 20. Still BLOCKED-BY 0x1c8df0's call-site inlining.
// Round 188 (m3 lane 5, fast with the file's functions, 0x1af810, 0x1b2770 and 0x1c8df0; control 20): the second lookup indexed
// by the prop node's field with the object index assigned after it 20 (same code); the ignore index chosen by one combined
// if/else (`!flag && field != NONE`) 123 (585 bytes); the 0x1e3b00 arm first (`(short)arg_3 != 0`) 39; the source record's
// NONE store after the point copy 23; 0x2640c0 given the prop node's field directly 39. Still BLOCKED-BY 0x1c8df0's call-site
// inlining (src/unknown_1c8df0.cpp is another lane's file this round).
// Round 93 (m3 lane 15, fast with the file's functions, 0x1af810, 0x1b2770 and 0x1c8df0; control 20): the probe's source and
// target points named once as const pointers for both differences and the probe 24 (only the magnitude's x87 order moves);
// the tail as one short category started at 4 and set to 2 by a combined test 110 (604 bytes); the probe result in a named bool
// 20 (same code); the mask local as `dword` 20 (same code); the two distance tests joined with `||` before `return 2` 35.
// Re-measured by mistake: the second lookup indexed by `(word)` 132 (round 101). Still BLOCKED-BY 0x1c8df0's call-site
// inlining (here this file is /Ob1; the files of the five callers retail calls it from out of line are /O2).
// Round 103 (m3 lane 13, fast with 0x1af810, 0x1b2770 and 0x1c8df0, which read as the full build; control 20): the actor
// pointer const 20 (same code); the actor's +0x26c bool read right after the actor lookup at the top 116 (first +0x2); the
// result field's NONE store ahead of the cluster test 28; the prop node and derived lookups ahead of the actor lookup 34
// (first +0x6); the result field's NONE store between the mask and the ray vector 27. Still BLOCKED-BY 0x1c8df0's
// call-site inlining.
// M3 lane 15 r96 (fast with 0x1af810, 0x1b0400, 0x1b2770, 0x1be120, 0x25f7b0 and 0x1c8df0; diagnostics on the inlining
// cause, all restored): 0x1c8df0's noinline dropped with the tail as a plain call (no inline or forceinline declaration
// anywhere) 69 (589 bytes): LTCG then inlines it here and into 0x1b2770 (656, lost), the two callers passing a constant
// zero mode, and nowhere else; with this body unchanged it still inlines into 0x1b2770, so that decision does not depend on
// this file. On the inlined call: this function returning `short` (as 0x1af810's `(short)` casts show; its declaration
// there changed with it, 0x1af810 stays matched) 32 at 584 bytes, with retail's early ignore-index load and the
// distance-first product, left the frame (0x8c, retail 0x80), the mask temp (EAX, retail ECX), the magnitude's sum order and
// the distance home; the point copy and the motion buffer in a block 43 (frame 0x80, but the 12-byte copy sorts below the
// 16-byte NONE/point pair); the NONE cell and source point as separate locals 159 (565 bytes, the NONE folds). 0x1b2770
// given the existing magnitude3d for its hand-written length still inlines 0x1c8df0 (656). On the kept hand-written body a
// `short` return type 56 (601 bytes). Still BLOCKED-BY 0x1c8df0's call-site inlining (0x1b2770 must keep it out of line).
// Round 137 (m3 lane 9, fast with 0x1af810, 0x1b2770 and 0x1c8df0, then plain with the file's functions; control 20): the
// 0xbaf40 call written in each arm of the header-type test (`type == 0 && field != NONE` passes the field, else the object
// index) 5, kept: the second header mask moves to ECX and the distance to the +0x14 home, both as retail. On it: the hit
// fraction read into a local after the test 3 (only the ignore index's load is then late; a one-use field copy, not kept
// without a match); the product operands swapped 5; `<` compares 5 either operand order; the last test as an if with
// returns 5; the distance declared at the top 5 (all the same code); the ray vector built in the call's argument list
// through a helper returning its output 5 (same code), with the fraction local 3; a named copy of the ignore index ahead of
// the ray vector 5 (the copy propagates); the mask chosen below the ray vector 22; the result's NONE store below the ray
// vector 25; a two-point collision helper taking the vector's storage and the ignore index by value 9, with the fraction
// local 7 (the magnitude's x87 order changes). On the fraction-local form (3): the mask as a ternary 26; no vector pointer
// local 3; the NONE store ahead of the cluster test 12; the mask ahead of the NONE store 10 (the ignore index then loads at
// the block's top, but the pushes run ahead of the first difference); the mask cleared with `&= ~0x20` 3; the ray vector
// ahead of the mask 20, ahead of the NONE store 21. On the old body (20): the ignore index from an inline helper with
// literal returns 123 (585 bytes), an inline header-type getter 20, the 0xbaf40 result in its own variable 20 (same code).
// Reading: retail's early load is the by-value ignore-index parameter of the two-point probe 0x16a040, which retail
// inlines inside the inlined 0x1c8df0 (0x16a040 and 0x1c8df0 are both kept out of line on main).
// Diagnostics on that reading (a file-local inline copy of the two-point probe with its own vector, not keepable: an
// inline twin of 0x16a040; all with the fraction local): on the kept frame 44 (frame 0x8c; aligned with registers and
// stack offsets masked 3: only the frame size differs, the early load and the product order are retail's); with the
// source cell and point as a 16-byte pair and the point copy at function scope 37 (frame 0x8c, the pair below the copy
// as retail); with the copy in a block that ends before the cluster lookup 27 (584 bytes, frame 0x80, masked 0: every
// instruction is retail's, but the 12-byte copy/vector home sorts below the 16-byte pair, retail has the pair lower);
// that with the motion buffer in the block and the result in a block around the tail 27, the copy and the pair renamed
// either way 27 / 27, the pair declared first in the function 27, the buffer declared last 27, the cell's NONE store
// inside the block after the copy 27 (masked 2) or ahead of a bare copy 27; the cell as its own long or one-member
// aggregate 158 (566 bytes: it folds, so retail's cell and point are one object); the first half as an inline helper
// owning the copy 312 (439 bytes: it stays out of line).
// M3 lane 12 r139 (fast with 0x1af810, 0x1b2770, 0x1c8df0 and 0x1c8df0's other callers; diagnostics with 0x1c8df0's separate
// noinline declaration dropped, restored): whether an inline-level boundary at 0x1b2770's call keeps 0x1c8df0 out of line
// there: 0x1b2770's visibility query in a static helper 957 bytes (the helper stays out of line with 0x1c8df0
// inlined into it), the same helper forceinline 1425 bytes (0x1c8df0 inlined through it), the whole unknown040 block as a static
// helper 148 bytes (out of line, 0x1c8df0 inside); LTCG inlines 0x1c8df0 at that call site at any nesting level. On this body:
// the object index re-read through a fresh prop node lookup 178 (594 bytes); the ignore index bound by `long &` for the probe 20
// (same code). Still BLOCKED-BY 0x1c8df0's call-site inlining.
// Shared component setter for the ray 16, for the measured vector 5, for both 16; shared point setter
// on the ray workspace 16. Set the mask bit from a cleared default 8, with the ray setter 19, with the
// measured-vector setter 8, with both 19. The ray setters load the ignore index at retail's offset but
// stage the vector components in another order. The hit product still loads the fraction first.
// N421 previews: existing two-point collision call 207; typed collision result receiver retained from ray
// setup 5; distance gate and hit product set a shared classification 97; successful collision owns the
// distance and classification block 42; successful collision assigns a return value and both paths share
// the return 81; explicit ordered hit-distance classification with a shared default 5; ray vector allocated
// independently of the input point copy 45; existing two-point collision call with the shared
// classification result 189.
// NOT kept: the existing two-point collision call remains out of line, the dedicated ray grows the frame,
// and the classification variants tie or regress. Opening body restored.
// Matched in a full build with stand-in SDK headers (was 5): retail inlines function_1c8df0 here and nowhere else,
// so this is a plain call of it and function_1c8df0 loses its noinline prototype; the point copy lives in its own
// block so that its slot is shared with the inlined body's vector (frame 0x80).
// @retail 0x1af5c0
short function_1af5c0(long arg_0, long arg_1, point3f const *arg_2, long arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_node_view *local_1 = prop_node_get(arg_1);
	s_1af5c1 *local_2 = (s_1af5c1 *)function_25d690((s_prop_datum *)local_1);
	long local_19 = local_1->object_index;
	s_1af5c2 *local_3 = &((s_1af5c2 *)g_4e0300->data)[local_19 & 0xffff];
	s_ai_object *local_4 = local_3->field_8;
	s_1af5c0 local_7;
	long local_8;
	point3f position;
	long cluster_index = NONE;
	{
	point3f local_5 = *arg_2;

	function_2640c0(local_19, (s_object_motion_view *)&local_7);
	local_19 = local_1->object_index;
	local_3 = &((s_1af5c2 *)g_4e0300->data)[local_19 & 0xffff];
	if (local_3->field_3 == 0 && ((s_1af5c3 *)local_4)->field_14 != NONE)
		local_8 = function_baf40(((s_1af5c3 *)local_4)->field_14);
	else
		local_8 = function_baf40(local_19);
	if ((short)arg_3 == 0)
	{
		position = local_5;
		position.z += 0.05f;
	}
	else
		function_1e3b00(local_0->unknown018, arg_3, &local_5, NULL, NULL, &position);
	}
	long leaf_index = function_14a280(g_4e033c, 0, &position);
	if (leaf_index != NONE)
		cluster_index = *(short *)(((s_1af5c4 *)g_4e0348)->field_30 + leaf_index * 8);
	return function_1c8df0((short)cluster_index, local_2->field_2c, &local_2->field_30, local_8, &position, 0, false,
		local_0->unknown26c != NONE, true, NULL);
}
