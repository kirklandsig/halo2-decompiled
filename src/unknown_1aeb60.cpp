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
	if (local_3->field_3 == 0)
	{
		local_8 = ((s_1af5c3 *)local_4)->field_14;
		if (local_8 == NONE)
			local_8 = local_19;
	}
	else
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
