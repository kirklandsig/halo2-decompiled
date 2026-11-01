// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10AF20.CPP: object changes the scripts make: attaching an object to
   another's marker, scaling an object and its children over time, and
   setting an object's velocity in its own frame */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "vector_products.h"

/* the object (a view of the object data) */
struct s_scripted_object
{
	long definition_index;
	byte unknown004[0xc - 4];
	long next_object_index;
	long first_child_index;
	long parent_index;
	byte unknown018[0x70 - 0x18];
	vector3f forward;
	vector3f up;
	byte unknown088[0xaa - 0x88];
	byte type;
	byte unknown0ab[0x12c - 0xab];
	dword flags_12c;
};

struct s_scripted_object_header
{
	byte unknown00[8];
	s_scripted_object *object;
};

#define SCRIPTED_OBJECT_GET(index) (((s_scripted_object_header *)g_4e0300->data)[(index) & 0xffff].object)

void __stdcall function_b8ee0(long parent_index, long marker_name, long object_index, long a);
void function_b7680(long object_index, real scale, real seconds);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity,
	bool unknown);
void function_1c4b00(long object_index, void *a, void *b, long c);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);

/* attaches an object that has no parent to a marker of another object */
// @retail 0x10af20
void function_10af20(long object_index, long marker_name, long other_object_index, long other_marker_name)
{
	if (object_index != NONE && other_object_index != NONE)
	{
		s_scripted_object *other = SCRIPTED_OBJECT_GET(other_object_index);

		if (other->parent_index == NONE && (!((1 << other->type) & 0x1c) || !(other->flags_12c & 1)))
			function_b8ee0(object_index, marker_name, other_object_index, other_marker_name);
	}
}

/* scales an object and its children over a number of ticks */
// @retail 0x10af80
void __stdcall function_10af80(long object_index, real scale, short ticks)
{
	if (object_index != NONE)
	{
		long child_index = SCRIPTED_OBJECT_GET(object_index)->first_child_index;

		function_b7680(object_index, scale, (real)ticks * (1.0f / 30.0f));
		while (child_index != NONE)
		{
			s_scripted_object *child = SCRIPTED_OBJECT_GET(child_index);

			function_10af80(child_index, scale, ticks);
			child_index = child->next_object_index;
		}
	}
}

PRIVATE __forceinline void scripted_cross_product(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

PRIVATE __forceinline void scripted_vector_scale(vector3f const *vector, real scale, vector3f *result)
{
	result->i = vector->i * scale;
	result->j = vector->j * scale;
	result->k = vector->k * scale;
}

/* sets an object's linear velocity from speeds along its forward, left and up
   axes */
// score 21 (was 30): frame 0xc, retail's spills of the left products into velocity and (with the
// forward_left_part local) the final adds in the forward registers agree; 378 bytes against 377. Left:
// the j row's second cross product loads forward.k where retail loads up.i, the left.k product is
// scheduled after the left.j spill, and the up.i term is loaded rather than copied from up_speed.
// Before round 19 (at 30): two cross-product terms took the other operand order, the k spill was
// scheduled after `push ebx`, and the up term was loaded rather than copied from up_speed (one byte),
// so the three final sums landed in the up registers where retail keeps them in the forward ones.
// Round 29: forward and up scaled into their own vectors with left scaled straight into velocity,
// then two vector3f_add, 76 -> 30 (kept). On it (fast): shared cross3f/cross3f_staged 30/30; forward
// and up pointer locals 135; forward scaled before the cross product 28 (forward_speed leaves xmm6,
// all three callers stop matching); left scaled first 82; up scaled before left 79; vector3f_scale
// for each part 30/30/30; operands of the last add swapped 30; first sum before the up scaling 85,
// with forward first 91; sums accumulated in forward_part 30. Earlier forms, on the old body:
// Round 81 (score 76): `forward`/`up` pointer locals for the cross product
// alone 74 (retail's product operand order); velocity written directly without the
// scaled copy 80 (retail's 0xc frame); left_part/up_part vectors 79-83; left scaled in
// place 83; velocity = left * left_speed first 85-87 (forward_speed leaves xmm6)
// Round 197 fast trials: Existing cross helper with up/forward/both scalar inputs, output
// last/first and actual vector return 84, 72, 95, 83, 72, 95, 156, 153.
// Forward scalar inputs with up by pointer: 72 with output first or last; four-point lead reverted.
// Round 77 (plain): the scale/add/add helper chain straight into velocity 84 (0xc frame, but each
// sum commutes: retail accumulates in the forward product's register); chain into the working
// vector with the last step writing velocity 86; per-component three-term sums 93 and through
// vector3f_assign 93 (both need the two helpers as plain definitions, which 0x1c1e70 accepts);
// forward and left summed per component, up through add-scaled 93. Retail stores nothing of
// velocity until all three sums are done and loads up.j early (xmm7), spilling two left products.
// Round 9 (fast): `forward`/`up` pointer locals for every read 75 (378 bytes); those with velocity
// written directly 84 (0xc frame and retail's cross-product operand order, but velocity is
// stored component by component as each sum finishes, where retail stores all three at the end);
// forward scaled into the working vector, left scaled into velocity and the two added with
// vector3f_add, then up added in place, with/without the pointers 84/83; `velocity = scaled;`
// 83, with the pointers 82.
// Round 35 screen (cross helper: file-local / shared cross3f / forward and up pointer locals /
// cross3f_staged, x velocity: current / scale-add-add chain / per-component sums / left scaled first
// then add-scaled forward and up / left scaled first then per-component sums): current velocity with
// the pointer locals or the staged cross 75 (378/379 bytes); every other velocity form drops retail's
// two spills (353-362 bytes) and scores 84-98.
// Round 102 (fast): scale(left) into velocity then add_scaled(forward) and add_scaled(up) 87 (the cross
// product orders, the 0xc frame and the magnitude tail match, but forward_speed moves to XMM4 and nothing
// spills); scale(forward) then add_scaled(left) and add_scaled(up) 84; the cross product straight into
// velocity, scaled in place, then forward and up 83; the j/k/i form straight into velocity 80; velocity
// declared before left with the left-first chain 87.
// Round 232: Completed-vector memcpy / native component loop crossed with original / staged cross and native / helper left addition
// 83, 76, 82, 75, 83, 78, 83, 78. Original executable source restored.
// Round 107 (fast; compile-only harness reproduces the product operand choice and final-add registers,
// and every text order of products, sums and scale spellings compiles the same): `vector3f const
// *forward = &object->forward;` passed only to the cross product 8 (377 bytes: retail's up-term copy and
// schedule; left: the j row's second product loads fwd.k where retail loads up.i, and the three final adds
// put the sum in the up registers); not kept, a one-use pointer local without a match. On it: an up
// pointer in the up scale 26, in the cross 26, declared first and both in the cross 34; forward pointer in
// the forward scale too 33, there only 28; up pointer alone in the cross 16 (that product right, the
// other five flip); up pointer in the cross and up scale 76 (366); first sum before the up scale 83;
// second add's operands swapped 8 (same code). Pointer locals for both scales only 26 (cross terms
// retail's, schedule not); on it up scaled first 28, up added through vector3f_add_scaled 83 (361).
// Also on the 8 form: up through vector3f_add_scaled 83 (361); first sum into forward_part, then
// forward_part + up_part into velocity 8; vector3f_scale for the up, forward or left part 8 each (same
// code); up scaled before left 77 (380). A second object lookup for the cross product's forward 30
// (CSE'd away). Harness: the cross helper's six row orders, plain or staged, keep the j row's operand
// order (orders starting with k fix it but compute k first); a shared `part` vector 33. Pointer
// placements (forward in cross/forward scale, up in cross/up scale): up scale only 28; forward scale and
// up cross 34; those plus the up scale 74 (366); forward in cross and both scales 32; forward cross, up
// cross and up scale 31; forward cross and scale with up cross 31.
// Round 4 (lane 18, fast): second products of the j and k cross terms with the operands swapped 30
// (same code); first add as velocity + forward_part 30 (same code); scale as scale * component 30
// (same code); scale storing j, k, i 46; cross storing k, j, i 40; velocity declared first 30; cross
// product straight into velocity, scaled in place 32. Restored.
// Round 21 (lane 14, fast): `forward`/`up` pointer locals for the cross product only, up declared first
// 34; forward declared first 26 (retail's cross-product operand order; one-use pointers, not kept);
// those pointers also used in the two scalings 42 (377 bytes and retail's k-spill schedule, but the
// forward j and up j products multiply from memory where retail preloads them); in the forward scaling
// only 31, in the up scaling only 32 (both 377 bytes); the first sum before the up scaling with the
// cross pointers 81 (362 bytes), with all pointers 86 (360). The last adds accumulate in the up
// registers in every form (retail in the forward ones).
// Round 93: forward-only cross pointer/reference 8; staged cross, unit scaled add, velocity-first,
// late or mutable pointer and forward-up declarations 8 each on that base; both sums forward with
// publication 84; up-alias cross 111; forward-alias 130; single accumulator and second-sum-forward
// 85 each (caller conventions changed). One-use aliases not kept; original restored.
// Round 19 (lane 17; compile-only harness agrees on the float block): the cross-product operand
// orders follow how many expressions are evaluated before the cross (diagnostic dead temporaries: one
// fixes the k row, two both rows); the final adds put the sum in the up registers whenever the last
// add reads velocity's stored value against up_part, in every scale order and accumulator tried; a
// fresh `forward_left_part` local (first assigned at the first add, after up_part) makes the forward registers the
// destination with retail's ((forward + left) + up) association: 21 (kept).
// Real (fast): left into velocity, `velocity + up_part` then `+ forward_part` 27; a left_part local
// with `left_part + up_part` into velocity then `+ forward_part` 25 (both add (left + up) + forward,
// retail (forward + left) + up, so not kept); the object through an inline scripted_object_get 30;
// up scaled first 32; up scaled before the cross product 82. On that form: the one-use forward
// pointer in the cross 19 (both cross rows right; left.k schedule and the up.i copy remain); pointer
// placements (forward ptr in cross/forward scale, up ptr in cross/up scale) 0001 19, 0010 29, 0011 77,
// 0100 19, 0101 19/19, 0110 25/27, 0111 73/75, 1001 19/21, 1010 19/27, 1011 26/75 (377 bytes: the
// left spill schedule right, but up.j not preloaded and the up_speed copy on up.j), 1100 26, 1101
// 26/26, 1110 26/83, 1111 36/94; inline scripted_object_get with the forward pointer 19, alone 21; a
// header local for the lookup 19 (one-use, not kept), with the forward pointer 21.
// @retail 0x10b010
void function_10b010(long object_index, real forward_speed, real left_speed, real up_speed)
{
	if (object_index != NONE)
	{
		s_scripted_object *object = SCRIPTED_OBJECT_GET(object_index);
		vector3f left;
		vector3f forward_part;
		vector3f up_part;
		vector3f forward_left_part;
		vector3f velocity;

		scripted_cross_product(&object->up, &object->forward, &left);
		scripted_vector_scale(&object->forward, forward_speed, &forward_part);
		scripted_vector_scale(&left, left_speed, &velocity);
		scripted_vector_scale(&object->up, up_speed, &up_part);
		vector3f_add(&forward_part, &velocity, &forward_left_part);
		vector3f_add(&forward_left_part, &up_part, &velocity);
		function_b7740(object_index, &velocity, NULL, false);
		function_1c4b00(object_index, &velocity, NULL, true);
		if (velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k > 0.0001f)
		{
			function_b9b90(object_index, false);
			function_b7360(object_index);
			function_bba20(object_index);
		}
	}
}
