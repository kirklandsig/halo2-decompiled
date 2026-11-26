// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_265CB0.CPP: the props an actor knows (0x265cb0..) */

#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "props.h"
#include <math.h>
#include "unknown_26b230.h"
#include "object_markers.h"
#include "unknown_1fb7e0.h"
#include "object_queries.h"
#include "unknown_249e20.h"
#include "units.h"
#include "unknown_2729b0.h"
#include "data_array.h"
#include "unknown_25d020.h"
#include "vector_products.h"
#include <string.h>


point3f *function_b9dd0(long object_index, point3f *result);
long function_1469f0(real seconds);
long function_1e4a90(long index);
long function_25d810(long object_index, long actor_index, bool create);
void function_118e10(long object_index, point3f *point);
real function_20e190(long object_index);


struct s_object_motion_view
{
	point3f point;
	point3f center;
	byte unknown18[0xc];
	long location[2];
	vector3f velocity;
};

// @retail 0x2640c0
void function_2640c0(long object_index, s_object_motion_view *result)
{
	long const *object_reference = &object_index;
	s_slot_object_view *object = object_get(object_index);
	long root_index = NONE;
	for (long index = object_index; index != NONE; index = object_get(index)->parent_index)
		root_index = index;
	s_slot_object_view *root = object_get(root_index);
	function_b9dd0(object_index, &result->center);
	result->velocity = root->velocity;
	result->location[0] = *(long *)((byte *)root + 0x28);
	result->location[1] = *(long *)((byte *)root + 0x2c);
	if ((1 << object->type) & 3)
	{
		s_object_marker marker;
		function_b8d30(*object_reference, 0x4000095, &marker, 1, false);
		result->point = *(point3f *)((byte *)&marker + 0x60);
	}
	else if (object->type == 12)
		function_118e10(*object_reference, &result->point);
	else
		result->point = result->center;
}

// @retail 0x266540
void function_266540(long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	long index = *(long *)((byte *)actor + 0x338);
	if (index != NONE)
	{
		long object_index = actor->unknown018;
		if (object_index != NONE && *(char *)((byte *)actor + 0x324) >= 2)
		{
			s_prop_node *node = (s_prop_node *)prop_ref_get(index);
			s_type_76cf92 *prop = prop_get(node->prop_index);
			if (!prop->unknown35 && prop->unknown23 && *(real *)((byte *)prop + 0x2c) >= 2.0f)
			{
				s_type_f95cd3 *view = function_25d740(node);
				if (view->unknown14 != NONE && *(short *)view < 9 &&
					(real)(g_510c54->game_time - view->unknown14) > g_510c54->rate * 8.0f &&
					function_20e190(object_index) > 0.0f)
				{
					function_1fb7e0(0xb7, *actor_reference, NULL, node->object_index, NONE);
					prop->unknown35 = 1;
				}
			}
		}
	}
}

bool function_25d9b0(long prop_index);
bool function_1e2030(long actor_index);
void *function_1e4f90(long actor_index);
void *function_1e5240(long actor_index);
long function_1b9190(long object_index, long *rider_index);

// @retail 0x265d30
real __stdcall function_265d30(long actor_index, long prop_index)
{
	long const *actor_reference = &actor_index;
	long const *prop_reference = &prop_index;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_datum *node = prop_ref_get(prop_index);
	s_type_76cf92 *prop = prop_get(node->prop_index);
	s_type_5cfb45 *state = function_25d690(node);
	s_type_f95cd3 *view = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking) view = &tracking->view;
	}
	if (*(bool *)((byte *)prop + 0x26) || !prop->unknown23 || node->state <= 0 ||
		(state->unknown5e && *(short *)((byte *)state + 0x5c) * g_510c54->rate >= 5.0f) ||
		object_get(node->object_index)->type == 15 || (node->type != 1 && node->type != 6))
		return 0.0f;
	short state_score = 0;
	short range_score = 0;
	real current_bonus = 0.0f;
	short special_score = 0;
	real view_bonus = 0.0f;
	real direction_bonus = 0.0f;
	if (actor->unknown007)
		range_score = 0;
	else if (function_25d9b0(*prop_reference))
		range_score = 0;
	else if (*(bool *)((byte *)actor + 0x269))
		range_score = *(long *)((byte *)actor + 0x26c) == state->unknown3c ? 10 : 0;
	else if (!function_1e2030(*actor_reference))
	{
		real minimum = 0.0f;
		byte *settings = (byte *)function_1e4f90(actor_index);
		if (settings) minimum = *(real *)(settings + 4);
		if (node->unknown28 < 1.0f && !(node->state == 3 && view && view->unknown88))
			range_score = 10;
		else if (state->unknown3c != NONE)
			range_score = 0;
		else if (state->unknown69 != *(bool *)((byte *)actor + 0x265))
			range_score = 10;
		else if (minimum > node->unknown28)
			range_score = 10;
	}
	else
	{
		byte *settings = (byte *)function_1e5240(actor_index);
		if (node->unknown28 < 1.0f && !(node->state == 3 && view && view->unknown88))
			range_score = 7;
		else if (settings)
		{
			real minimum = *(real *)(settings + 0x18);
			s_character_variant *variant = (s_character_variant *)function_272a00(actor_index);
			if (variant)
			{
				switch (function_272ad0(variant))
				{
				case 1: minimum = *(real *)(settings + 0x28); break;
				case 2: minimum = *(real *)(settings + 0x30); break;
				}
			}
			if (minimum > node->unknown28) range_score = 5;
			else if (*(real *)(settings + 0xc) > node->unknown28) range_score = 3;
		}
	}
	if (state->unknown5e) state_score = 1;
	else if (view && !actor->unknown007 && view->unknown2a) state_score = 6;
	else if (node->state >= 1)
	{
		if (actor->unknown007) state_score = 2;
		else if (view && (view->unknown06 == 0 || view->unknown06 == 1))
			state_score = state->unknown63 && view->unknown39 <= 1 ? 5 : 4;
		else if (node->state >= 3)
			state_score = !view || view->unknown88 ? 1 : 2;
		else state_score = 3;
	}
	if (*prop_reference == *(long *)((byte *)actor + 0x338) && actor->unknown086 >= 5 && !state->unknown5e)
		current_bonus = *(bool *)((byte *)actor + 0x354) ? 0.2f : 2.0f;
	if (state->unknown67) special_score = 10;
	if (view)
	{
		view_bonus = (real)(view->unknown60 * 0.3);
		if (view_bonus < 0.0f) view_bonus = 0.0f;
		else if (view_bonus > 3.0f) view_bonus = 3.0f;
	}
	real distance_bonus = 5.0f / (node->unknown28 * 0.1f + 1.0f);
	if (*(long *)((byte *)actor + 0x26c) != NONE && actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1))
	{
		long rider_index;
		function_1b9190(*(long *)((byte *)actor + 0x26c), &rider_index);
		if (rider_index != NONE)
		{
			s_slot_object_view *rider = object_get(rider_index);
			vector3f delta;
			if (view) delta = *(vector3f *)((byte *)view + 0x2c);
			else
			{
				vector3d_from_points3d((point3f const *)((byte *)actor + 0x238), &state->position, &delta);
			}
			direction_bonus = 0.0f > dot3f((vector3f const *)((byte *)rider + 0x150), &delta) * 6.0f ? 0.0f : dot3f((vector3f const *)((byte *)rider + 0x150), &delta) * 6.0f;
		}
	}
	return ((real)(range_score + state_score) + (real)special_score + direction_bonus) + distance_bonus + view_bonus + current_bonus;
}

void *actor_get_character_situation_entry(long actor_index);
bool function_11bf30(s_location const *location);

struct s_perception_origin_view
{
	point3f point;
	byte unknown0c[0xc];
	vector3f direction;
	s_location location;
};

// @retail 0x263ed0
short function_263ed0(long actor_index, s_perception_origin_view const *origin, point3f const *point,
	s_location const *location, short type, short mode)
{
	s_location const *const *location_reference = &location;
	short const *type_reference = &type;
	short const *mode_reference = &mode;
	s_actor_view *actor = actor_get(actor_index);
	byte *settings = (byte *)actor_get_character_situation_entry(actor_index);
	short result = 0;
	if (settings && type && origin->location.cluster_index != NONE && location->cluster_index != NONE)
	{
		vector3f delta;
		vector3d_from_points3d(&origin->point, point, &delta);
		real range = *(real *)(settings + 0x18);
		real distance_squared = length_sq3f(&delta);
		if (dot3f(&origin->direction, &delta) < 0.0f)
			range *= 0.8f;
		if (actor->unknown084 == 3)
			range *= 0.7f;
		else if (actor->unknown084 == 1)
			range *= 0.4f;
		if (*type_reference == 4)
			range *= 0.2f;
		else if (*type_reference == 1)
			range *= 0.45f;
		else if (*type_reference == 3)
			range *= 0.7f;
		if (function_11bf30(&origin->location) || function_11bf30(*location_reference))
			range *= 0.25f;
		if (*mode_reference != 0 && *mode_reference != 1)
			range *= 0.7f;
		if (range * range > distance_squared)
		{
			long cluster_a = origin->location.cluster_index;
			long cluster_b = location->cluster_index;
			s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
			if (!function_249c20(bsp, cluster_b, cluster_a))
			{
				real distance = function_249d60(bsp, cluster_b, cluster_a) * 1.3333333730697632f;
				real direct_distance = (real)sqrt(distance_squared);
				distance = distance > direct_distance ? distance : direct_distance;
				if (range > distance)
					result = 2 + (*type_reference >= 3);
			}
		}
	}
	return result;
}

struct s_distance_rates_view
{
	byte flags;
	byte unknown01[3];
	real distance_limit;
	real near_distance;
	real far_distance;
	real maximum_distance;
	real far_rate;
};

// @retail 0x263d70
void function_263d70(long actor_index, real distance, real rate, real scale, real *secondary, real *primary)
{
	s_distance_rates_view *settings = (s_distance_rates_view *)actor_get_character_situation_entry(actor_index);
	real *const *primary_reference = &primary;
	real *const *secondary_reference = &secondary;
	real a = 0.0f;
	real b = 0.0f;
	if (settings)
	{
		if (distance > settings->maximum_distance)
		{
			a = 0.0f;
			b = 0.0f;
		}
		else
		{
			real near_rate = rate * scale;
			real far_rate = settings->far_rate * scale;
			real near_secondary = near_rate * 0.7f;
			real far_secondary = far_rate * 0.7f;
			if (far_secondary > 3.5f)
				far_secondary = 3.5f;
			if (distance > settings->far_distance)
			{
				a = far_rate;
				b = far_secondary;
			}
			else
			{
				real near_distance = settings->near_distance;
				real secondary_distance = near_distance * 0.8f;
				if (distance < near_distance)
					a = near_rate;
				else
				{
					real range = settings->far_distance - near_distance;
					if (range <= 0.0f)
						a = far_rate;
					else
					{
						real fraction = (distance - near_distance) / range;
						a = (1.0f - fraction) * near_rate + fraction * far_rate;
					}
				}
				if (distance < secondary_distance)
					b = near_secondary;
				else
				{
					real range = settings->far_distance - secondary_distance;
					if (range <= 0.0f)
						b = far_secondary;
					else
					{
						real fraction = (distance - secondary_distance) / range;
						b = (1.0f - fraction) * near_secondary + fraction * far_secondary;
					}
				}
			}
		}
	}
	**primary_reference = a;
	**secondary_reference = b;
}

PRIVATE inline real basis_dot3f(vector3f const *a, vector3f const *b)
{
	return a->i * b->i + a->j * b->j + a->k * b->k;
}

// @retail 0x263740
void function_263740(long actor_index, vector3f const *direction, vector3f const *axis_a,
	vector3f const *axis_b, vector3f const *axis_c, real rate, real scale, real *secondary, real *primary)
{
	vector3f components;
	components.i = basis_dot3f(axis_a, direction);
	components.j = basis_dot3f(axis_b, direction);
	components.k = basis_dot3f(axis_c, direction);
	real vertical = (real)atan2(components.k, sqrt(components.i * components.i + components.j * components.j));
	if (vertical > 0.5235987901687622f || vertical < -0.7853981852531433f)
	{
		*secondary = 0.0f;
		*primary = 0.0f;
	}
	else
		function_263d70(actor_index, (real)fabs(atan2(components.j, components.i)), rate, scale, secondary, primary);
}

short __stdcall function_c8b80(long object_index, s_object_seat *seats, short maximum_count);
bool function_26d370(point3f const *point, vector3f const *direction, plane3f const *plane, real *distance);
real function_30bf0(vector3f *vector);

// score 84 (was 84): m3 lane 16 r179 and lane 5 r233 unchanged (their lines just above the marker); r216 unchanged (r200 unchanged); retail's block layout and registers but for the range's first load and two products, 1310 bytes against 1308, masked 20. Left: stack homes
// M3 lane 6 r223 (h2m fast with 0x263740/0x26d370, both MATCH; control 84): local renames, to test name order among
// same-size homes: `a`/`b` as occupant_secondary/occupant_primary, the facing pair as secondary_reach/primary_reach,
// the squared distance and clip amount as distance_sq/clip_fraction, range and scale as perception_range/range_scale,
// and (diagnostic) `result` under two names sorting first and last: 84 each (same code, homes unchanged). The frame map
// is as round 67 found: ours shares the clip amount's slot with the seat loop's down-counter, retail with the squared
// distance.
// MiniMax E (11-20): no new form and no lower score (84): retail's homes run result 0x8, facing maxima 0x14/0x18, the
// clip amount sharing 0x1c with the squared distance, scale 0x20, range 0x30, loop count 0x34, seat cursor 0x48 and the
// delta at 0x4c/0x50/0x54, while this build puts the result at 0xc, the clip amount at 0x10, the range at 0x18 and the
// delta at 0x48/0x4c/0x50, so the whole set of homes sits one cell low. The seat loop's two cross products also load
// their operands the other way round, and both come from a helper in another file's header, so the forms that would
// move them are not this lane's to write; the body stands.
// MiniMax H (11-20): no new form, no lower score (84): six source forms measured this round (the clip amount's scope, the facing pair's declaration order, the distance's scope, the delta as a point3f, and the scale and the facing maxima initialized at the top) leave the homes where they are, except the two top-init forms that rise to 90 and 208/212; retail's homes run result 0x8, facing maxima 0x14/0x18, clip amount 0x1c, scale 0x20, range 0x30, loop count 0x34, seat cursor 0x48 and delta 0x4c/0x50/0x54, and ours run result 0xc, clip amount 0x10, range 0x18, scale 0x1c, facing maxima 0x20/0x30, seat cursor 0x34 and delta 0x48/0x4c/0x50.
// The 0x350 frame agrees. The initial result is four bytes above retail, and the clip output shares the later seat-count slot.
// Earlier measurement 84 (was 84): r200 unchanged; retail's block layout and registers but for the range's first load and two products, 1310 bytes against 1308, masked 20. Left: stack homes
// MiniMax R (11-17): no new form, no lower score (84): the seat loop's two cross products load their operands in the
// other order (retail multiplies a term with the vector's component in the register and this build with the memory
// operand), and the shared header helper's spelling fixes that order, so the products cannot be moved from here; the
// seat loop's result still lands four bytes above retail's and the range still loads into xmm2 for xmm0.
// MiniMax E (11-16): no new form, no lower score (84): the permuter (900 s, one job) left the baseline at 84; the result's first store still lands four bytes above retail's, the range is still loaded into XMM2 for XMM0, and retail multiplies the seat loop's third cross term the other way round (forward.j * up.i), which the shared header helper cannot produce.
// M3 lane 8 r189 (fast with 0x263740/0x25c9d0/0x26d370, all MATCH; control 84): names do not order these homes (result,
// range or the facing maxima renamed to sort first or last, diagnostic, 84 each); the facing copies as secondary then
// primary 84; distance_squared declared at the top of the settings block 84; the clip amount declared in the motion
// block 84; the loop index as long 173 (repeats an earlier form); the b maximum tested before the a maximum 83; the
// scaled range as `range * scale` 84; the posture switch as an if/else-if chain 131 (1308 B). Retail's homes, by esp
// offset: result 0x8, facing maxima 0x14/0x18, clip amount sharing 0x1c with distance_squared, scale 0x20, range 0x30,
// loop count 0x34, seat cursor 0x48, delta 0x4c; ours: result 0xc, clip amount sharing 0x10 with the loop count, range
// 0x18, scale 0x1c, facing maxima 0x30/0x20, seat cursor 0x34, delta 0x48, distance_squared 0x5c.
// Previous stack measurements are in the lane 8 round 166 entry below.
// MiniMax H (11-12): no new form, no lower score (84): the result long still lands at [esp+0xc] where retail
// keeps it at [esp+8], the clip's amount out-parameter sits at [esp+0x10] against retail's [esp+0x1c] and
// retail's range is at [esp+0x30] against our [esp+0x18]; the seat loop's two cross products keep our
// operand order, and the declaration orders the earlier rounds tried do not move these homes.
// MiniMax B (11-11): no new form, no lower score (84); the vector homes are still one slot off from retail's, the range's
// first load and the two products still differ, and the range is still loaded into XMM2 for XMM0; the body is 1310 bytes
// against retail's 1308.
// MiniMax R (11-15): no new form, no lower score (84): re-measured at 1310 bytes against retail's 1308, the
// remaining differences are the stack homes (ours at frame+0x10, +0x18 and +0x1c where retail keeps them at
// +0x1c, +0x28 and +0x30) and the two seat-loop cross products, whose operand order differs because both
// products are computed by a helper that lives in another file's header and is therefore not this lane's to
// rewrite; every form in the note block below has already been measured, so the body stands.
// MiniMax G (11-04): the motion declaration hoisted beside the delta gives identical code (152);
// no new form, no lower score.
// MiniMax S (11-10): no new form, no lower score (84); the vector homes are still one slot off from retail's, the range's first load
// and the two products still differ, and the range is still loaded into XMM2 for XMM0.
// retail's block layout and registers but for the range's first load and two products, 1310 bytes against 1308, masked 20. Left: stack homes
// (result +4 for retail's +0; amount shares with the count where retail shares it with the squared distance; range, scale, the cursor and the
// two facing values one slot off), x-operand forms in the two seat-loop cross products, the range loaded into XMM2 for XMM0. Round 65 (m3
// lane 14, fast): a named facing test read again after the arms, for retail's arm past the first exit (retail's jmp back to the join after a
// barrier): alone 153 (1312); seats indexed alone 142; both 84 [20], kept, with primary copied first in the re-test; the same test written
// out at both places 342 (1347); a per-iteration seat element pointer 90 at 1308; long index 173; range read after delta 87; delta through
// vector3d_from_points3d 142 at 1308; amount or plane declared wider 84 each; forward cast at each use 147; side/up and a/b declaration
// orders, ternary maxima, `continue` on no occupant 84 each; a world-up pointer local before the loop 149, in the body 90; non-const forward 84.
// MiniMax G (11-04): the motion declaration hoisted beside the delta gives identical code (152);
// no new form, no lower score.
// Earlier first line: score 104 (was 152): retail's registers throughout (the SIMD groups are now right), 1310 bytes against 1308. Left: stack homes
// (result +4 for retail's +0; amount shares with the count where retail shares it with the squared distance; range, scale, the
// cursor and the two facing values one slot off), the not-facing arm sits inline where retail places it after the first return, and
// x-operand forms in the two seat-loop cross products. Round 64, fast, masked aligned in brackets: the projected delta through a clipped
// point and vector3d_from_points3d 262 [38]; with vector3d_from_points3d for motion and length_sq3f for both squared lengths 104
// [30], kept; on that: the projected delta written out again 110 [38] at 1308 bytes; `seats[i]` directly 142 [25] (1313 bytes), a per-iteration seat pointer 148 [34]; vector3d_from_points3d for the
// first delta too 145 [38]; a named clipped fraction 235 [40]; `short result`, the ternary 3/2, `a`/`b` at the block top 104 (same
// code); negated facing test with the arms swapped 106 [32] (the arm stays inline); amount initialised 260 [42].
// Earlier first line: score 152 (was 152): Frame 0x350 agrees; result/range/projection homes and inverse delta/motion SIMD groups remain.
// Earlier: result/range homes and inverse SIMD groups remain; seat thresholds now stay in memory.
// result/range homes, inverse SIMD groups and seat-threshold reloads remain.
// Round 57: retail's result stores are a spill of a long result (dword store, `mov ax, word` reload at
// each tail-duplicated exit). Plain long result 241 (result home +0xc); short return type with every
// declaration 186 (1311 B), with a plain long result 245; volatile short result 179 (aligned listing count
// worse: 92 against 86), with the short return 186; else-arm secondary before primary 195; the two
// out-of-range stores as else-if arms 184.
// the register allocation differs from the first block
// the register allocation differs from the first block
// on (retail keeps delta in xmm5-7 and the motion in xmm2-4, ours the other way
// round), and our seat loop keeps secondary and primary in xmm1/xmm2 across
// iterations where retail compares them in memory and reloads them after the
// loop, which also moves retail's not-facing block after the result-3 exit.
// Frame: retail has result, secondary, primary at +0/+4/+8 and range at +0x20;
// ours has secondary, primary, range at +0/+4/+8 and result at +0x20.
// Tried: if/else for the 3-or-2 result, retail's separate exits (262: the
// frame shifts), result/secondary/primary as one struct (270), secondary and
// primary at function scope (262), volatile long result (262), range declared
// after the motion block (291), range re-read from settings (275), range read
// through a pointer (229), the facing block as an inlined helper with pointer
// outputs (281, inlines c8b80), the clip as a clipped point (280),
// vector3d_from_points3d for delta and motion (200), a and b at function scope
// and `secondary < a` compares (no change).
// Tried at 226 (lane): qualified long result 226; qualified byte result 232; long seat count 240;
// long seat index 226; shared seat-element pointer 226.
// Frame/result and inverse SIMD homes remain. Ordinary short result 250; ordinary long result 250; delta initializer 221; motion initializer 221; plane scope 221 (fast, each with the seat increment in the body).
// Frame/result and inverse SIMD homes remain. Fraction reuse 211; plane distance first 304; cursor displacement 271; threshold defaults 265; reverse delta assignments 233 (fast; seat increments in body).
// Frame 0x350 agrees; result/range homes, inverse SIMD groups and threshold reloads remain. Early range square 266; scaled-range square 221; threshold maxima 221; bounded seat end 245; plane initializer 221 (fast; seat increments in body).
// Direct secondary/primary/both query outputs 230/229/214; previous-value order 215;
// previous values before basis 230; loop-scoped previous values 214;
// up after normalization 221; direct outputs with later up 214. All with body cursor step.
// Earlier trial set: Frame/result and inverse SIMD homes remain. Ordinary short result 250; ordinary long result 250; delta initializer 221; motion initializer 221; plane scope 221 (fast, each with the seat increment in the body).
// Earlier trial set: Frame/result and inverse SIMD homes remain. Fraction reuse 211; plane distance first 304; cursor displacement 271; threshold defaults 265; reverse delta assignments 233 (fast; seat increments in body).
// Earlier trial set: Frame 0x350 agrees; result/range homes, inverse SIMD groups and threshold reloads remain. Early range square 266; scaled-range square 221; threshold maxima 221; bounded seat end 245; plane initializer 221 (fast; seat increments in body).
// Round 138: clipped vector/in-place/receiver 276/276/276; read-only actor/occupant 195/195; facing value 258; both values 311; motion square 229.
// Native external settings with byte mask/bitfield 221/221; per-use range 225/225;
// those four records with both existing vector difference helpers 221/221/245/245 (body cursor step).
// Shared scaled differences for delta/motion/both 234/233/225; shared magnitudes 221;
// with delta/motion/both shared producers 234/265/257; staged native inputs with both helpers 366.
// Seat cursor advanced in body for each trial; all original producers, grouping and cursor restored.
// Round 166: Corresponding squared components named at both motion/delta magnitudes:
// k/j/i 226/226/226; kj/ki/ji/all 221 each; all with complete plane-product names 221.
// Each trial advances the seat cursor in the body. Original expressions and cursor restored.
// Round 71: renaming result/secondary/primary/range and a long result compile identically (slot order is not
// name-driven here); separate `result = K; return result;` exits 269 (1333 bytes), with a long result 269;
// if/else 3-or-2 with a long result 259; separate exits with a plain short result 328.
// Round 167: Consistent long/dword return contracts with short/word result storage 200 each;
// long/dword storage 195 each at retail length. On the complete long return/result graph,
// separate 3/2 stores 255; projection fraction clamped in place 184 (plain, kept); both 241;
// direct threshold accumulators 230. Keep long return/result, one projection fraction and
// seat cursor stepped in the body; both external declarations agree, exact caller preserved.
// Round 145: Vector role names/vector plus plane-fraction names 184/184; mode failure at
// one result exit with existing/plain result 184/241; settings failure likewise 184/241;
// plain mode/settings early-result returns 340/350. Original body restored.
// Round 243: Native point/origin/both/endpoint values 351/363/396/327; point-endpoint
// 366, origin-endpoint 378; all inputs in either declaration order 432/432. Original body
// restored.
// Round 205: Plain component loops for delta/motion/both 429/407/431; projected output
// 221; projected with delta/motion/both 432/411/425; all with short ordinals 438.
// Original producers restored; result/range homes and inverse SIMD groups remain.
// Posture cases reversed 184; exhaustive scale switch, ordered arms and
// complete conditional scale 202/253/253; each of the four with per-use range
// reads 273/311/306/306. Original scale, range observations and body restored.
// Round 249: coordinated projection inline/noinline 294/184; threshold inline/noinline
// 184/184; both inline, projection-inline/threshold-noinline, projection-noinline/threshold-inline,
// both noinline 294/294/184/184. Original declarations and bodies restored.
// Round 212: outer/scaled/both range-failure returns with inherited result 216/219/253;
// both plus missing-settings return 253; the four with ordinary result 329/314/273/275.
// Frame 0x350 persists, entry roles move; original admission, result and observations restored.
// Round 252: Final primary-test/complete-conditional/primary-first/distance-first results
// 249/283/235/230; ordinary result storage 281/281/220/221. Original result graph and inherited storage
// restored; result/range homes and SIMD allocation remain.
// Round 196: Normalizer inline/noinline/combined/forceinline declarations, each with original
// and post-normalization up lifetime, 184 each. Original declarations and body restored.
// Round 28 (m3 lane 12): retail returns a short (`mov ax, word ptr [esp + 8]`) through separate result
// exits; the 3/2 choice as if/else stores gives those exits and moves result toward retail's +0 slot
// (241, 1327 bytes); with a short return 245 (needs 0x25c9d0's declaration changed too). Motion declared
// before delta 184; result before the references 184; maxima as conditionals 184; a/b beside the maxima 184.
// Ours also keeps secondary/primary in xmm registers across the seat loop; retail compares them in memory.
// Lane 9 r25: retail returns a short (`mov ax, word ptr [esp + 8]` at each exit) from a long result
// slot; the prototype in src/unknown_25d690.cpp (another lane's file this round) keeps the return
// long. Result renamed level 184 and secondary/primary renamed 184 (identical code: names do not
// move these slots); range read after delta 184; primary declared before secondary 184.
// Round 30 (m3 lane 12): retail's frame swaps result and range (result at frame 0x0, range at 0x20 above the
// motion vector; ours the other way), and range comes in xmm0 with delta in xmm5-7 (ours delta xmm2-4, range
// xmm5). Range renamed to sort after result 184 (identical); range declared at function scope before/after
// result 184/184; range read after the delta 184; delta declared before range 184.
// Round 84: retail's `mov ax, word ptr` exits need a short return, and 0x264b50's declaration sits in
// a file another lane holds this round; in the seat loop retail compares and stores secondary/primary
// in memory (`comiss xmm0, [mem]`) where ours copies them through xmm1/xmm2. a/b at the seat-loop top
// 184; secondary/primary at function scope 184; delta at function scope 184; indexed seats instead of
// the cursor 211; seat array at the facing-block scope 184.
// Lane 4 r106 (fast): the six parameter reference locals had no effect (removed: 184, one product's
// operands swap); without the `volatile` result 241 (the result still lives in memory as a spilled long,
// so retail's `mov ax, word ptr` exits need only the short return: BLOCKED-BY the 0x25c9d0 declaration in
// src/unknown_25d690.cpp); the clipped delta as point + motion * amount through a point helper and
// vector3d_from_points3d 254, with the first delta and motion through vector3d_from_points3d too 255.
// Round 28 (m3 KIMI2): short return/early exits 255; ordinary result 325; facing locals with early exits 180;
// reversed publication 180; short/facing/single select 197; short/facing/explicit 3-or-2 stores 152 (plain, kept);
// mode/settings failure exits 188/202; publication order, ordinary result, distance declaration, projected order,
// facing and outer declaration orders 152 each; direct count-down 197; direct accumulators 245.
// Round 166: existing complete vector assignment for delta/motion/both/projected
// 164/154/166/152; projected with delta/motion/both, then all plus plane normal
// 164/154/165/293. Original component producers restored.
// Round 224: Live actor/handle/occupant and actor-handle/actor-occupant/handle-occupant/all/all-forward
// 311/146/218/324/328/218/322/363; live-handle count/index short-word/short-long/long-word/long-long
// 229/185/232/185. Six-point 146 rejected below margin; original receivers and signed traversal restored.
// Round 228: Immutable range/distance/scaled-range producers, all seven combinations,
// and all three with the seat workspace at entry 152 each. Original mutable declarations
// and seat lifetime restored; result/range homes and inverse SIMD groups remain.
// DH round 230: Live handles with amount/plane/both/vectors/all projection declarations at entry
// 146 each; fallback thresholds before facing 238; direct facing outputs 245; conditional final result 228.
// Six-point forms rejected; original body restored. Result/range/projection homes and SIMD groups remain.
// Round 189: paired range squares/final squares/both, matched two-term magnitude partials,
// range-partials/final-partials/all/all with live handle: 193/172/183/193/152/183/168/168.
// Original products and handle observation restored; result/range/projection homes and SIMD groups remain.
// Round 394: empty/non-player/both seat continue exits, each with native/seat-scope
// query outputs, 205 each; both with later up 205; both with long index 209.
// Original eligibility, cursor and output scopes restored; result/range/SIMD homes remain.
// DH round 232: Unsigned full-width result 152; stable actor direction gate 296; both 296;
// signed admission interval 162, with unsigned result 162, stable gate 301, both 301;
// unsigned-word result on the full graph 301. Native result/range/projection homes remain; restored.
// DH round 239: Corresponding projected products/positions/both in every row
// 191/191/195; each with consistent live seat handles 187/187/195; products or
// positions with both squared-range expressions 195/195. Original expressions,
// handles and complete body restored; result/range/amount and SIMD homes remain.
// DH round 252: Existing staged cross at side/up/both; both with later up, seat-scope outputs,
// both scopes, corresponding basis/output names, both scopes and long ordinal
// 152/152/152/152/152/152/152/183. Original complete executable body restored.
// Round 214: existing dot/basis-dot plane producers, both operand orders: copied normal 150 each; native normal 290 each.
// Exact-only sum forms restored; no gain meeting the margin, result/range/amount homes and inverse SIMD groups remain.
// Earlier: the volatile result anchor and the pointer-to-whole-local side
// operand are gone, so the result, range and delta stack homes and the delta/motion SIMD
// register blocks differ from retail again.
// Earlier: the facing branch is entered on retail's negated condition and the cross
// product's multiply operands now follow retail's order; the result, range and delta stack homes
// and the delta/motion SIMD register blocks still differ.
// Coordinator review: the facing branch on the negated condition with the volatile result kept scores 150 (was 152);
// with int result as well 150. Neither meets the 8-point bar, so main's code stays; the 144 above also had the cross
// product's operand order, which is not in this file.
// Round 240: reusable mode test/all-axis origin scalars/all-axis point scalars and their pairs/all
// 323/346/319/364/342/380/395; all with ordinary result 395. Original input observations and
// complete body restored; mode caching grows the frame to 0x354.
// Round 38 (m3 lane 11, fast): seat element pointer per iteration of an indexed loop 190 (1310 B);
// clip as `delta.i += motion.i * amount` 232 (1296 B); not-facing secondary from primary 152;
// occupant forward cast at each of its three uses (no pointer local) 195 (1315 B); one out-of-range
// store after both range tests behind an in-range flag 196 (result slot then at retail's +0, first
// difference +0x27). Original body restored.
// Round 246: native actor basis fields in all seven nonempty subsets, then all
// with the native occupant basis, 152 each. Original padding, receiver expressions
// and complete body restored; result/range/projection homes remain.
// Round 263: Library projection/paired-maxima/plane-copy masks 1/2/4/3/5/6/7 score
// 307/203/152/316/307/203/316; complete graph with ordinary result 316. Original selectors, plane copy and
// inherited result restored; result/range/amount homes and inverse SIMD groups remain.
// Round 74 (m3 lane 12, fast): the posture switch as if/else-if 188; distance squared through the existing
// length_sq3f 152, the motion test through it 152, both 152 (same code); `use_facing && !actor->unknown007` 154.
// Round 295: isolated complete plane products in yzx/xyz/xzy/zyx declaration orders,
// mutable/const, each 191 (1310 bytes). Original plane expression and native copy restored.
// Round 300: positive projection before upper clamp, combined/nested admission and
// in-place/named clipped fraction 269/269/287/287; with live object handle
// 263/263/269/269. Original admission, fraction and handle observations restored.
// Round 322: Distance-owned outer/scaled/final paired comparisons, all seven subsets, then
// corresponding motion-minimum comparison, 152 each. Original comparisons and complete body
// restored; result/range/amount homes and inverse SIMD groups remain.
// DH299: Native four-state result enum crossed with range-after-delta, entry
// distance scratch and entry projection scratch, all eight subsets: 152 each.
// Original result type and scratch lifetimes restored; result/range/amount homes and inverse SIMD groups remain.
// DH309: Complete result/range, projection and addressed facing/seat-output lexical role
// families, seven subsets and complete distance-role names: 152/152/152/152/152/152/152/152.
// Native scalar homes and SIMD groups unchanged; original complete body restored.
// Round 54 (m3 lane 14, fast): amount declared at the settings-block top 152; not-facing arm with secondary
// stored first 161; facing arm copying primary first 152; seat array at function scope 152; occupant forward
// pointer set before the player test 160. Retail's seat-loop cross product loads the up axis first in its
// first product (ours the forward component); retail also lays the not-facing arm after the result exits.
// M336: Pre-collection seat ordinal and facing-entry basis/query-output workspaces,
// seven subsets: 152/152/152/152/152/152/152; all with cursor advanced after lookup 205.
// Original scopes/cursor restored; result/range/amount homes and inverse SIMD groups remain.
// Round 48 (m3 lane 2, fast with 0x263740): the `volatile` dropped from the result 152 (same score, kept: the
// plain long result is still spilled to +4); on that: occupant and player tests merged 152; facing branch on
// the negated condition 150; short result 152; amount renamed to sort after result 152 (amount/count keep +0);
// clamp into a named clipped fraction by ternary 263.
// B414: Basis, first-query-output and plane lifetimes end at their last consumers, seven subsets;
// complete graph with negative projection outcome: 152/152/191/152/191/191/191/191. NOT kept: ties or regressions.
// Original complete body restored; result/range/amount homes and inverse delta/motion SIMD groups remain.
// DH341: Complete sole-result defaults on mode/settings/post-projection/distance/outer-radius/
// inner-radius/post-facing/leaf outcomes 321/328/363/363/363/372/327/325.
// NOT kept: native result home not recovered; frames or lengths regress. Original body restored.
// DH344: Existing complete setters at initial delta/motion/projected delta and their
// pairs/all, then all with staged basis crosses: 164/154/152/166/164/154/165/165.
// NOT kept: native result/range/projection homes and SIMD groups remain; original body restored.
// DH346: Projected displacement in motion via native stores/setter, independent vector, complete
// mutable/immutable scalar results via stores/setter, then scalar-to-motion transfer: 316/316/316/150/150/150/150/316.
// NOT kept: 150 gains only two; native result/range/projection homes remain. Original body restored.
// P331: Geometry admission results at endpoint/length/query, masks 1/2/4/3/5/6/7 then complementary
// all: 188/189/189/197/197/197/190/230. NOT kept: no equality or gain; original complete body
// restored.
// DH371: Actual outer/scaled/secondary/primary/near/secondary-improvement/primary-improvement/
// player-eligibility Boolean test results: 188/188/152/152/152/152/152/191.
// NOT kept: no equality or gain; original complete body restored. Result/range/projection
// homes and inverse delta/motion SIMD groups remain.
// P331: Geometry admission results at endpoint/length/query, masks 1/2/4/3/5/6/7 then complementary
// all: 188/189/189/197/197/197/190/230. NOT kept: no equality or gain; original complete body
// restored.
// M404: indexed-seat byte/signed-char/word/int ordinals, mutable count:
// 186/185/193/173; immutable count: 186/185/193/173.
// NOT kept: no exact native allocation; original signed-short traversal and body restored.
// MiniMax B (11-06) round 2: no new form, no lower score (84); the amount read into a local at
// the function's scope, declared before the result or after it, gives the same code (84), so the
// low frame slots cannot be moved that way: ours keeps the result at +0x0c and the amount at
// +0x08 where retail has the result at +0x08 and the amount at +0x14; the two cross products go
// through the shared `cross3f` and `cross3f_staged` (both of them, either one alone: 84, the
// same code) and the second one's arguments exchanged score 96; in the first cross product retail
// loads the up component into the register and the forward component as the memory operand for
// the `up.i * forward.k` term while ours does the opposite, the values agree, and only the
// shared helper's product order can produce retail's split, so the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (84); the frame slots are still permuted (ours
// keeps the result at +0x0c, the range at +0x18, the delta at +0x38..0x40 and the seat cursor
// at +0x34, retail the result at +0x08, the range at +0x30, the delta at +0x3c..0x44 and the
// cursor at +0x48), the range still loads into XMM2 where retail uses XMM0, and the two
// cross-product products still take their operands in the exchanged order; the body is unchanged.
// P360: Complete outer/scaled radius rejection results in bool/byte/short/long: 152/152/152/152/152/152/152/152.
// NOT kept: no exact native source recovery; original complete body restored; result/range/projection homes and two basis product schedules remain.
// T63: Shared cross-product complete products, scoped/unscoped mutable/const,
// component results mutable/const, successive subtraction and uniform positive products:
// 154/154/154/154/84/84/150/150. NOT kept: ties/regressions; shared header and full body restored.
// T65: On the current ordinary-result/indexed-seat graph, semantic result/range/fraction
// home identities, seven subsets and complete squared-distance identity: eight 84.
// NOT kept: identical code; original body restored. Native scalar homes, range ingress
// and the two cross-product operand schedules remain.
// P364: Escaping plane-distance field/scalar accumulation boundaries, preserving
// the original y+z+x tree: 96/154/150/86/154/96/84/84. NOT kept: native scalar homes/producer
// equality absent; original complete body restored, shared helpers unchanged.
// P366: Resolved 3/2/1 result-leaf exit subsets 1/2/3/4/5/6/7 score 149/143/180/136/180/174/356; all leaves
// plus radius-failure exits 356. NOT kept: all regress and several shorten away from native; original long
// result, call tuples and complete body restored. Result/range/projection homes and both cross-product
// schedules remain.
// P369: Complete native delta/clipped-point aggregate and existing plane-normal CRT transfer boundaries, masks
// 1/2/4/3/5/6/7, then immutable clipped point: eight 84. NOT kept: no scalar-home, range-ingress or product repair;
// original motion helper, native copies and full body restored.
// P372: Indexed-seat occupant/player/both eligibility continues: 84/84/84; ordinary
// forward while with step after final index use, no/occupant/player/both continues:
// four 161; both continues with separately declared short ordinal 84. NOT kept:
// ties/regressions; original full body restored. Scalar homes and basis products remain.
// P375: Existing safe staged cross with first/second/both inputs as complete twice-used scalar
// captures, mutable/const, then both native/aggregate vector snapshots: 166/162/165/166/162/165/387/165. NOT kept: no native
// scalar-home/product equality; shared header and complete original body restored.
// T69: Actual range-entry byte-mask/byte-bitfield receiver, mutable/readonly settings,
// each with mutable/readonly actor input: eight 84 on the native SIMD/indexed-seat graph.
// NOT kept: no native range ingress or scalar-home recovery; original types and full body restored.
// Round 138 (m3 lane 7, fast with 0x263740/0x25c9d0 = plain 84): retail loads the range into XMM0 at entry and
// reloads it at the join after the endpoint block on every path (all its failure jumps land on the reload);
// ours loads it into XMM2 and the endpoint-NULL/unknown007 exits jump past the reload (+0x1d7 for +0x1d1). The
// 2 extra bytes are the two seat-loop cross products (ours `movss` from memory then `mulss` by the register).
// Frame map, ours/retail: amount+count 0/c (retail amount+squared distance), result 4/0, range 8/20, scale c/10,
// facing primary 10/8, facing secondary 20/4, cursor 24/38, delta 38/3c, a/b 44,48/48,4c, squared distance 4c/c.
// Diagnostics: `(void)&range` 84 (same code); the join's range read through its address 150 (1312 B, entry still
// XMM2). Forms: the endpoint test as nested ifs 84 (same code); the squared distance through dot3f 84 (same
// code); scale declared with the range at the settings-block top 212 (1312 B). A compile-only /FAs screen does
// not give our LTCG frame here (fastcall entry), so it cannot rank these homes.
// Round 145 (m3 lane 8, fast with 0x263740/0x25c9d0; masked = register-masked aligned count, base 20): one function-scope
// real for the clip amount and then the squared distance (retail keeps both in one slot) 81 (1310 bytes, masked 18: the
// delta and a/b homes become retail's and the count gets its own slot; a gain of 3 on a variable holding two unlike
// values, not kept). Homes then, ours/retail: range 10/30, scale 14/20, result 18/10, facing primary 1c/18, amount 20/1c,
// facing secondary 24/14, motion 28/24, cursor 34/48, plane 38/38, count 48/34. On that form: the two facing locals'
// names exchanged, renamed (four ways) or declared the other way round, the amount renamed, the range renamed 81 each
// (same code: names and declaration order move no slot here); the scaled range written out at its four uses 147; the
// not-facing secondary from primary 81; the facing copy secondary first 81; the b maximum before the a maximum 83; an
// explicit seat cursor 154; the endpoint block inside `do { } while (false)` 417 (masked 79); `real const` range 81;
// the range read after the endpoint block 299 (masked 34); a second named copy of the range at the join 83; a
// per-iteration seat element pointer 89 (masked 28). The two seat-loop cross products: retail forms five of the first
// one's six products from the world-up component in the register (ours three), and takes the side component as the
// copied register operand in the second one's `forward.k * side.j` and `forward.k * side.i` (ours loads forward.k).
// Round 82 (m3 lane 1, fast with 0x263740/0x25c9d0 = plain 84): the thresholds written straight into secondary/primary with
// no facing copies and no second facing test 199 (1331 B: ours then carries both in XMM1/XMM2 round the seat loop and skips
// retail's reload after it; retail compares and stores them in memory); the defaults assigned first and the facing block
// overwriting them from its copies, no else and no second test 197 at retail's 1308 bytes (the two default lines sit
// inline ahead of the test, retail has them out of line after the first exit); the copies at the end of the facing block
// with the else kept 142 (1313 B); one scalar for the clip amount and then the squared distance 81 (amount and squared
// distance share a home as in retail, result moves to +0x10; the name does not fit both values, not kept); the clamp as an
// in-place ternary 84; facing_ pair declared first and secondary copied first 84. Diagnostics, reverted: 0x263d70 storing
// both output pointers to scratch globals leaves the first form at 199, and so does this function defined ahead of
// 0x263d70 and 0x263740: neither an escaping pointer nor definition order makes our link keep the thresholds in memory.
// M3 lane 10 r90 (fast with 0x263740/0x25c9d0, both MATCH): no gain. The plane distance through dot3f 138 (1308 bytes, masked 28); the
// intersection test as `amount >= 0.0f` 236; the vehicle index read at each use instead of the `object_index` local 84, `range * scale`
// 84, the three threshold compares written distance-first 84 (all same code). The permuter (`--fast --anneal`, 600 s after a 1041 s
// scratch build) tried 65 variants and found none below 82 instructions.
// Round 86 (m3 lane 1, fast with 0x263740/0x25c9d0; aligned/masked in brackets, base 84 [19/17]): the squared
// distance declared uninitialised at the settings-block top 84 (same code: declaration position moves no slot); the
// occupant's forward as a vector copy 195 (1312 B [77/47]); the plane distance through dot3f 138 at 1308 B [34/24];
// the actor's three basis vectors through one `vector3f const *basis` pointer (plane normal and the 0x263740 call) 243
// [57/39]; the actor's +7 flag read once into a bool used by both tests 85 [20/18]. Re-measured on this body: a long seat
// count 193 [64/41]. In the first seat-loop cross product ours loads the forward component in the up.i * forward.k and
// up.j * forward.i products where retail loads the world-up component.
// Round 78 (m3 lane 14, fast, masked aligned in brackets, base 84 [19]): the actor looked up after the settings 310
// at retail's 1308 bytes [34]; the vehicle index read ahead of the first 0x263740 call 146 [24] (origin then in EBX);
// both 315 (1308) [37]; the endpoint test with explicit `!= NULL` and `== 0` 84 (same code); the outer range test
// inverted with the out-of-range arm first 152 (1312) [37]; re-measure of the listed long seat count on this base 193
// (1318) [51]. A volatile float local read ahead of the range load (one --fast diagnostic in src, reverted at once)
// leaves the range in XMM2, so its register is not a rotation step; the compile-only /Gr screen gives another
// allocation here.
// Round 67 (m3 lane 15, fast with 0x263740/0x25c9d0): both builds turn the seat loop into a cursor and a down-counter;
// the down-counter (ours) or the squared distance (retail) takes the clip amount's dead home, and the clamped amount
// itself stays in XMM0 after the call. The seat count renamed `seat_count` 84 (identical code); the squared distance
// unnamed, `length_sq3f(&delta)` at its five uses 84 (identical code: it is a spill temp either way); the clipped point
// through a copy of path_transitions.cpp's point_along_vector (diagnostic) 90; the 3/2 result as one ternary 84; `long
// count` 193 (1318 bytes); the occupant's forward as a value copy 195 (1312 bytes).
// Round 185 (m3 lane 4, fast with 0x25c9d0/0x263740/0x26d370): the up cross and the side cross through the existing
// cross3f_staged 84 each (same code: the seat-loop cross products' operand forms do not follow the helper's statement
// order), both 84 (a combination of same-code forms, a re-measure); re-measured: the squared distance as
// `dot3f(&delta, &delta)` 84 (same code, as round 138 found); the not-facing defaults with secondary assigned first 101
// (aligned 71); the object index as `actor->unknown26c` 84 (same code); primary and secondary as `facing ?
// facing_primary : scaled_range` / `facing ? facing_secondary : scaled_range * 0.7f` with no else and no re-test 101.
// No untried natural form is left for the stack homes.
// m3 lane 5 r169 (fast with 0x263740/0x25c9d0, base 84 [aligned 55, masked 17]): the outer test as `distance_squared <
// range * range` 84, the scaled test the same way 84, `range * scale` 84, the vehicle index read from actor->unknown26c at
// both uses (no local) 84, the not-facing secondary as `0.7f * scaled_range` 84 (all the same code); re-measured: `use_facing
// && !actor->unknown007` 86 [58/20].
// MiniMax G (11-04): the motion declaration hoisted beside the delta gives identical code (152);
// no new form, no lower score.
// MiniMax S (11-10): no new form, no lower score (84); the vector homes are still one slot off from retail's, the range's first load
// and the two products still differ, and the range is still loaded into XMM2 for XMM0.
// M3 lane 2 r73 (plain with the file's 32 functions, 0x25c9d0, 0x264b50 and 0x1e34e0): the clipped point through
// vector3f_add_scaled 90 (aligned 69, masked 26). Census of the other natural forms, all listed above with scores: the
// first delta through vector3d_from_points3d 142, the plane distance through dot3f 138, the clamp as a ternary 84, the
// range read per use 225 or again from the settings 275, the posture switch as if/else 188, a long seat count 193, a long
// index 173, seats indexed 142, a seat pointer per iteration 90, the maxima as ternaries 84, the squared distance at each
// use 84, the vehicle index at each use 84, the forward cast at each use 147, a world-up pointer 149/90. What is left is
// stack-slot packing (amount with the loop's down-counter here, with the squared distance in retail) and two operand orders.
// m3 lane 1 r92 (fast with 0x263740/0x25c9d0, control 84; aligned 57, registers masked 54, registers and stack offsets
// masked 20): no trial run. Frame from `sub esp, 0x350` (ours/retail): amount+seat down-counter 0/-, result 4/0, range 8/20,
// scale c/10, facing secondary 10/8, motion and side 14/14, facing primary 20/4, cursor 24/38, plane and up 28/28, delta
// 38/3c, a/b 48,44/4c,48, squared distance 4c/c (retail: amount + squared distance share c, counter alone at 24). So the
// spill slots take the clip amount's dead slot in another order (ours the counter, retail the squared distance). With
// registers and offsets masked, the rest is the two seat-loop cross products' operand sides. Every natural form found that
// changes those reference counts (range or facing reads, seat pointer or index, maxima, facing copies, result forms, one
// amount/distance scalar) is listed above.
// m3 lane 8 r158 (fast with 0x263740, 0x25c9d0, 0x26d370; register-masked aligned 17): the occupant object looked up at each
// use (no `object` local) 173 (1319 B); actor_get(actor_index) at every use (no `actor` local) 286 (1344 B). Left as before:
// the result's home (+4 for +0), the first seat-loop cross product's load order in `a->i * b->k` and `a->j * b->i` (ours
// loads the forward component, retail the world-up one) and the second's `forward.k` products (ours loads forward.k, retail
// copies side.j/side.k and multiplies forward.k from memory). The other files' cross helpers are file-private, so using one
// here would be a copy of cross3f's body.
// M3 lane 7 r156 (fast with the file, 0x25c9d0, 0x26d370, 0x263740; all matched stay matched): the posture scale as
// `if (*settings & 1) scale = 1.0f; else switch` with a `default: scale = 1.0f;` arm 214; the clipped delta written straight
// into `delta` as `(motion.c * amount + point.c) - origin.c` 146 (1313 B); the scaled-range test inverted with its
// out-of-range arm first 152 (1312 B); the secondary and primary tests as `distance_squared < x * x` 84 (same code); the
// `> 1` clamp moved inside the `amount > 0.0f` test 252 (1308 B); `amount > 0.0f` in the clip test for `!(0.0f > amount)`
// 259 (1294 B). Body unchanged.
// m3 lane 14 r88 (fast with 0x263740, 0x25c9d0, 0x26d370, all MATCH): `side` and `up` declared at the facing arm's top 84 (same
// code); the loop maxima with the primary update first 83 (a statement order by score, not kept: a and b swap homes, so the
// EBX output address agrees and the EDI one moves further off; retail has b below a, one slot above ours). A grep of this
// note for the result, range, facing, seat, maxima, cross, plane, clip and out-of-range forms finds every other natural
// form listed, so no untried one is left.
// MiniMax B (11-11): no new form, no lower score (84); the vector homes are still one slot off from retail's, the range's
// first load and the two products still differ, and the range is still loaded into XMM2 for XMM0; the body is 1310 bytes
// against retail's 1308.
// Round 81 (m3 lane 15, fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH throughout; base 84, aligned 55, masked 19/17):
// this function has an operand-order phase. Unused locals `long dead_n = actor_index + n;` for n = 1..7 (placeholders, not kept):
// 84, 86, 138, 138, 82, 82, 82. At n = 0 the first seat-loop cross has two wrong products (up.i * forward.k and up.j * forward.i
// load the forward component); n = 3..7 gives retail's load for up.j * forward.i only; n = 3 and 4 also reorder the second cross
// (1308 bytes, not retail's order); up.i * forward.k and the second cross's two forward.k products (ours loads forward.k, retail
// copies the side register) stay wrong at every n. Unused locals inside a forced-inline probe ahead of either cross (1..7 each) 84
// (no effect); cross3f_staged for the first cross, the second or both gives cross3f's code at every n (84, 84, 86, 138, 138, 82,
// 82, 82 each). With n = 0..7: a per-iteration seat pointer 90, 90, 146, 148, 88, 90, 90, 90 (1308 bytes each); a world-up
// pointer local in the body 90, 90, 90, 148, 150, 90, 90, 90 (1308); the same local ahead of the loop 149, 149, 144, 146, 147,
// 151, 149, 149; both pointer locals 90, 90, 148, 150, 90, 90, 90, 90. Natural changes in the number of named locals at n = 0: the
// scaled range written out 146 (1315 bytes); the vehicle index read at both uses 84 (same code); the facing test written at both
// uses 342 (1347); the squared distance at each use 84 (same code); scaled range and vehicle index 150 (1312); those and the
// facing test 347; scaled range, vehicle index and squared distance 150; vehicle index and squared distance 90 (1308). Frame:
// retail's 4-byte homes sit in descending order of their references (result 8; facing secondary, facing primary, the clip
// amount's slot, scale and range 4 each; count and cursor 3; a and b 2), and the clip amount's slot has 4 only as one variable
// with the squared distance, which is the merged form rounds 145 and 82 measured at 81.
// P458: Complete initial mode admission/rejection outcomes, bool/byte/short/long:
// 150/150/150/150/150/150/150/150. NOT kept: native result/range/projection homes and basis products not recovered; original complete body restored.
// P474: Standard delta/motion/clipped coordinate producers, typed normal copy,
// paired seeded magnitudes, y-seeded plane reduction, complete graph and
// explicit reductions. Result/range/projection homes and basis schedules remain.
// Scores 268/206/405/310/209/341/390/400. NOT kept: ties/regressions;
// original executable body and helper contracts restored.
// P482: Standard scalar range/facing-copy/conditional-improvement transfers, subsets
// 1..7, then all with nonfacing primary: 198/168/203/148/198/178/203/252. NOT kept:
// all regress; range+improvements reaches 1308 bytes and the native result slot but uses
// a nonnative GPR range load. Original producers, observations, helpers and body restored.
// P484: Complete optional range-failure publication, rejection/success outcomes
// bool/byte/short/long, on the native indexed-seat graph: eight 239 at 1270/1308 bytes.
// NOT kept: tested outcome/result storage misses equality and moves frame/extent away;
// original range branches, scalar homes, shared products and complete body restored.
// MiniMax H (11-14): no new form, no lower score (84): the seat loop's two cross products are still computed by a helper that is private to another file, so their operand order is not this lane's to change.
// M3 lane 13 r91 (fast with 0x263740, 0x25c9d0 and 0x26d370, all MATCH; control 84): the occupant's forward pointer declared
// right after the object lookup, ahead of the player test, 96 at retail's 1308 bytes (the forward base is formed ahead of the
// test and the first cross then reads fwd.k/fwd.j through it, not through the object); side/up and a/b declared at the seat
// loop body's top 84 (same code); both together 96 (1308 B). Diagnostics, reverted: the seat loop never entered 189 (1314 B);
// the forward-early form with the clip amount and the squared distance as one scalar 93 (1308 B). Body unchanged.
// m3 lane 8 r166 (fast with the file and its neighbours, control 84; aligned 57, registers masked 54, registers and stack
// offsets masked 18): no trial run. The masked residue is the result's home and the two seat-loop cross products' operand
// loads (2 bytes); a grep of this note for each lead (world-up copy or pointer, typed forward, seat pointer, count width,
// facing flag, range, plane, clip, out-of-range, mode tests, maxima) finds every natural form already listed.
// M3 lane 14 r95 (fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH): scope trials for the clip amount's
// shared home (ours shares it with the seat loop's down-counter, retail with the squared distance; the amount itself has two
// memory references, the lea for the call and one load): the seat count declared in the settings block 84, the clip amount
// declared at the endpoint block's top 84, the seats array declared at the facing block's top 84 (all same code), so
// lexical scope does not pick the partner. Also same code: the settings pointer typed `byte const *` 84, the squared
// distance `real const` 84.
// P503: Existing cross/difference/squared-length force-inline declarations in seven sets, then all plus basis dot:
// 84/84/84/84/84/84/84/84. NOT kept: All eight tie 84 at 1310 bytes, every calibrated file/provider/caller
// measurement is unchanged, and no native scalar home, range ingress or cross-product schedule is recovered. All
// experimental declarations removed. Original complete executable body restored.
// m3 lane 5 r185 (fast with 0x263740/0x25c9d0/0x26d370, all MATCH; control 84): the posture scale and scaled range
// computed ahead of one `range * range > d && scaled_range * scaled_range > d` test with a single out-of-range arm 259
// (1270 B); the occupant's player index in a local for its test 84 (same code); `vector3f up, side;` 84 and `real b, a;`
// 84 (same code: the homes do not follow declaration order); the merged test with the player-index local 259 (1270 B).
// M3 lane 5 r187 (fast with 0x263740/0x25c9d0/0x26d370, all MATCH; control 84): the facing test held in a `long` 84; the
// squared distance through dot3f(&delta, &delta) 84; the two out-of-range arms as `else if (out_of_range)` 84; the clip's
// upper clamp as `amount = amount > 1.0f ? 1.0f : amount` 84; the endpoint and actor tests as nested ifs 84 (all the same
// homes: the clip amount still shares the counter's slot at +0, the result sits at +4).
// Round 112 (m3 lane 10, fast): the facing outputs declared at function scope after the result 84 (same code); the
// not-facing secondary computed from primary 84 (same code); the world-up axis copied into a local vector for the first
// seat-loop cross 220 (1326 bytes); the occupant's forward pointer formed ahead of the player test 96 (1308 bytes, but
// `lea esi` then precedes the first cross); the first cross given the object-relative cast for the forward with the
// pointer kept for the second cross and 0x263740 152 (1312 bytes). Retail treats every forward component as the
// memory operand in both crosses (loading the world-up and side components), ours loads the forward component in
// three of them. All restored.
// P592: Original incoming origin/actor/point/endpoint/posture/mode/facing/output fixed
// bindings: eight 84 at 1310/1308. NOT kept: no native result/range/projection-home
// or cross-product operand recovery; original complete body restored.
// M3 lane 1 r114 (fast with 0x263740/0x25c9d0/0x26d370, all MATCH): retail's 4-byte homes are result, facing primary,
// facing secondary, the clip amount, scale and range (frame 0, 4, 8, c, 10, 20); declaring facing primary, facing secondary
// and scale ahead of range at the settings block's top 84 (same code), `range` given a longer name 84 and `result` another
// name 84 (same code: neither names nor declaration order move these homes); the facing pair as one
// two-element array (primary [0], secondary [1]) 103; the seat loop counter as long 173 (1308 B).
// b12K (11-17) walk against retail, fast build on r846: every statement, field offset (+7, +0x26c,
// +0x2a8/+0x2b4/+0x2c0, seat object +0x13c/+0x18c..+0x194, settings +0/+4), argument and constant checked
// (0.0025f, 0.0f as a memory operand at 0x26399c, 1.0f, 0.3f, 0.7f shared with the scaled secondary at 0x263cc6,
// 36.0f) and every jcc sense: jbe at 0x263911, 0x26398e, 0x26399c, 0x263a14, 0x263a66, 0x263c9a, 0x263ca7,
// 0x263cf0 skip NaN and ja at 0x263981 lets it through, as ours does; the shared out-of-range tail 0x263d0c, the
// `test ax,ax; jle` seat guard at 0x263adb and the dword result with `mov ax, word` reloads agree. No behaviour
// difference found, so the body is unchanged. New measurements (fast, with 0x263740/0x25c9d0/0x26d370): both seat
// crosses written out as component expressions 148, the second alone 84 (same code as cross3f), the first alone
// 148, and ten of the twelve products' source operand order flipped one at a time 148 each: the emitted operand
// side does not follow source order. The emission still differs from retail in the first cross's world-up.i *
// forward.k and world-up.j * forward.i (retail loads the up component first, 0x263b57 and 0x263b7d) and in the
// second's side.j * forward.k and side.i * forward.k (retail copies the register first, 0x263bd2 and 0x263bf7);
// the second cross's two are the 2 extra bytes. Best match-only recipe: none (84).
// Select original or clipped point by pointer/value, or select the displacement
// by pointer/value: 345/322/301/283; with the shared point setter 337/331/301/282.
// NOT kept: none recovers the native clip/distance home. The closest score grows
// to 1334 bytes and keeps the first result-store mismatch.
// The clip output still shares the lowest slot with the seat counter, and the result stays one word above retail's.
// Two-sided clip if/else 97, upper-first nested select 192, inclusive endpoint saturation 189. Signed seat
// countdown with postincremented input 187, indexed count with postincremented input 185. Primary fallback
// first with selected 3/2 tier 84, four-way result select 153, explicit zero in the primary fallback 153.
// NOT kept: the two-sided clip and cursor forms reach 1308 bytes but keep the wrong result home and raise the score.
// M3 lane 7 r177 (fast with 0x263740/0x25c9d0/0x26d370/0x263d70, all MATCH; control 84, aligned 57): a redundant
// `if (0 < count)` guard around the seat loop 148 (1315 B); the seat element copied into a local for the occupant lookup
// 150 (the frame moves); the seat element bound as a const reference 90 (1308 B, aligned 70); the occupant's forward
// bound as `vector3f const &` 84 (same code); the second probe's outputs and maxima in their own block 84 (same code).
// Seat record reference 90, value copy 150, named vehicle/seat/both 84/84/86;
// record reference, value copy and paired fields with a scan-wide world-up pointer 149/203/144.
// NOT kept: the result and projection/count homes remain unresolved.
// DH398: Positive signed seat admission with a word or dword countdown, pointer stepped
// before the call, after it or at body end. Word or dword admitted count with indexed walk 184/163/150/139/176/180, 188/184.
// NOT kept: Four countdown forms reach retail length at 1308 bytes, but all eight score higher and keep the result four bytes above the retail home. The original indexed seat walk is restored.
// MiniMax G (11-18): no new form, no lower score (84): the frame agrees at 0x350; the result, the range and the three
// delta components sit in homes four bytes away from retail's (the range at +0x18 against retail's +0x30, the deltas at
// +0x48..+0x50 against retail's +0x4c..+0x54), and the range is loaded into xmm2 where retail loads it into xmm0; the
// home offsets follow the declaration order, so moving them is frame shaping. The clip and count homes the earlier
// rounds record stay unresolved.
// M3 lane 3 r259 (fast with 0x263740/0x25c9d0/0x26d370, all MATCH; control 84): the clip amount declared at function
// scope ahead of the result 84 (same code); the seat basis (both crosses and the normalise) through a new forced-inline
// helper with a local copy of the world-up pointer 88 at 1308 B (the first cross's i product flips the wrong way), with no
// local 86 (1310 B), with a forward-pointer local 146 (1315 B), world-up and forward locals 92 (1308 B), a side-vector
// local for the second cross 92 (1308 B), world-up and side locals 88 (1308 B); as plain `inline` it stays out of line
// (408, diagnostic). None moves the result/clip/range homes.
// M3 lane 3 r261 (fast with 0x263740, 0x25c9d0 and 0x26d370, all MATCH; control 84): the first delta through the
// file's vector3d_from_points3d (as the clip arm already does) 142 at retail's 1308 B (the homes stay; the operand
// sides elsewhere move); the range read after the delta 87; both 145 (1308 B); the clipped point through the shared
// vector3f_add_scaled 90; the facing results copied at the end of the facing block in place of the second `if (facing)`
// 101. The range still loads into XMM2 (retail XMM0) and every 4-byte home but the delta's is where round 81 found it.
// M3 lane 11 r121 (fast with 0x263740, MATCH each time; control 84): the range as a `real const &` bound to the settings field
// 215 (1308 B: read at each use); the occupant test as `if (occupant == NONE) continue;` 84 (same code); unused locals inside the
// facing arm, n = 1, 2, 3, 5: 84, 86, 138, 82 (diagnostic, the same phase round 81 found at the top); the endpoint test written
// `!actor->unknown007 && endpoint` 88; the seat index declared ahead of the count 84 (same code).
// Use the eventual up vector for the world-up input to the side cross, then overwrite it with
// corrected up. Copy initialization, direct/staged crosses: 219 each with frame 0x34c.
// Component setter with the same four cross pairs: 167 each at 1308 bytes. The result and
// clip homes still differ.
// M3 lane 5 r200 (fast with 0x263740, 0x25c9d0 and 0x26d370, all MATCH; control 84, frame-blind 9): the endpoint clip block
// through a new forced-inline helper 417 (1392 B); the facing pair declared first 84 and distance_squared declared before
// the clip block 84 (same code); the posture switch as an if/else-if chain 131 (1308 B); `use_facing && !actor->unknown007`
// 86. Diagnostic, reverted: 1..7 dead initialised locals 84/86/138/138/82/82/82. Reading: retail shares one slot between the
// clip amount and distance_squared ([esp+0x1c]) with the result lowest; ours gives the clip amount the lowest slot.
// M3 lane 8 r194 (fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH; control 84): both seat-loop crosses through a
// file-private cross product in the textbook spelling (`a->j * b->k - a->k * b->j`, as the other files' private copies) 84
// (byte-identical: the products' operand sides do not follow the spelling); `real const range` 84; the vehicle index read as
// `actor->unknown26c` 84; the not-facing secondary as `primary * 0.7f` 84 (all the same code); the seat count held in a long
// 193 (1318 B). Body unchanged.
// M3 lane 6 r200 (fast; control 84): distance_squared and everything after it in its own `{}` block 84; `vector3f delta`
// declared ahead of the range 84; the endpoint test as two nested `if`s 84; the posture switch with a `default: break;`
// 84; the range `real const` 84 (each the same score; homes and the two seat-loop products unchanged).
// M3 lane 7 r186 (fast with 0x263740, 0x25c9d0, 0x26d370, all MATCH; control 84, aligned/masked 57/22): the occupant and
// player tests combined in one `&&` test 90 (1308 B; 70/36); `i++` for `++i` 84 (same code); the squared distance as
// dot3f(&delta, &delta) 84 (same code); the facing query writing `secondary`/`primary` directly (no facing copies, no
// re-test) 196 (1328 B, on this base); the vehicle index as the typed `actor->unknown26c` 84 (same code). Diagnostic, not
// kept: 8/9/10/11 dead initialised locals 84/84/86/138 (period 8; no count moves the homes). Reading: the aggregates sit in
// retail's homes; only the 4-byte cells are permuted: retail packs the clip amount with distance_squared and gives result,
// the two facing outputs the lowest cells, ours packs the amount with the seat count and the pair lands lowest.
// M3 lane 4 r226 (fast with 0x263740/0x25c9d0/0x26d370; control 84): the actor's byte flag at +7 read once into a named
// bool for the clip and facing tests 97; the motion threshold as `0.05f * 0.05f` 84 and the close-range one as `6.0f * 6.0f`
// 84 (same code); the settings through the file's typed situation view with flags and range at +0/+4 84 (same code); the
// endpoint and flag tests as nested ifs 84 (same code). All restored.
// M3 lane 5 r210 (fast with 0x263740, 0x25c9d0, 0x26d370 and five neighbours; control 84, register-masked 46): both
// seat-loop crosses through a trial copy of the textbook cross spelling (`a->j * b->k - a->k * b->j`, as several files keep
// it locally) 84, the first cross only 84, the second only 84 (all the same code: the product operand order does not
// follow the spelling); the seat array declared at function scope 84, beside the facing outputs 84 (same code).
// Y643: sampled point with direct delta 320; sampled point with shared delta 332; selected point pointer with direct
// delta 337; selected point pointer with shared delta 345; sampled point with direct delta and late range 326;
// sampled point with shared delta and late range 339; selected point pointer with direct delta and late range 332;
// selected point pointer with shared delta and late range 344.
// NOT kept: Calculating delta after clip selection shortens the function below retail length and raises the score on
// all eight forms. The earlier range reads also change the matched caller 0x25c9d0 in the reduced image.
// Empty seat list when the actor has no vehicle. Conditional then default count, each with indexed
// then pointer traversal, scores 186/204/102/204 with short counts and 202/206/199/206 with long counts.
// Empty-or-half-open seat range 207 or with step at lookup 202, signed admission with
// equality latch 152, nonnegative indexed bound 188, indexed advance after lookup 161,
// seat array extent 84, separate clipped fraction 245, scalar names 84, inclusive last
// seat 204. NOT kept: the extent and names tie. The indexed advance and separate clip
// fraction reach 1308 bytes but keep the result and clip homes displaced.
// M3 lane 16 r152 (fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH; control 84; body unchanged). Retail packs
// the clip amount and distance_squared into one cell ([esp+0x1c]); ours packs the amount with the seat count and gives
// distance_squared the top cell (0x5c), which is what puts delta and the probe outputs four bytes low. One real for
// both (the clip amount's variable reused for the squared distance; a reused variable, so match-only) 81 at 1310 B:
// delta and the probe outputs then sit in retail's homes, but motion moves up four and result, range and scale stay
// off. On that form, 1 to 7 dead initialised locals (diagnostic) 83/83/85/139/139/81/81 (1308 B at four and five):
// no symbol count completes the layout. distance_squared written through a pointer to itself 84 (same code: the
// pointer folds, the cell does not change class). One, two or three redundant parenthesis pairs around `scale * range`
// 150/154/148 (1312/1312/1310 B): the pairs do move the float operand sides, but not to retail's.
// Clip output initialized to zero or unity 263/268. Separate intersection status 84,
// plane scope ending at the query 84, nested success and lower bound 84, named positive
// fraction test 153, named motion squared 150, condition-local intersection status 84.
// NOT kept: the narrowed scopes tie and the initialized output grows the body.
// The result slot and the clip output’s spill partner still differ.
// M3 lane 13 r126 (fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH; control 84): the clip amount, the squared
// distance, the delta or the seat count declared at the top of the function (or of the facing block for the count) 84
// each (same code: scope does not move these homes); the settings lookup ahead of the actor pointer 310 (1308 B).
// M3 lane 14 r131 (fast with 0x263740, 0x25c9d0 and 0x26d370, all MATCH; control 84; body unchanged): the two facing
// maxima declared at the top of the settings block 84 (same code); the seat count held as `long` 193 (1318 B); the range
// read from the settings field at each use, no range local, 268 (1308 B); the not-facing arm storing the secondary limit
// first 101; the vehicle index through the actor view's typed unknown26c field 84 (same code).
// M3 lane 11 r134 (fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH; control 84, aligned 57, frame-blind 20):
// the two `if (facing)` tests merged (the else arm filling the facing pair, both outputs copied after) 199 (1331 B); the
// clipped point through the existing vector3f_add_scaled 90 (aligned 69); distance_squared `real const` 84 (same code);
// the plane distance through dot3f 138 (1308 B, aligned 73); the delta through vector3d_from_points3d 142 (1308 B, aligned 79).
// MiniMax A (11-20): no new form, no lower score (84); the clip amount and the squared distance still take separate cells where retail packs them into one, so delta and the probe outputs sit four bytes low and the motion cell sits four high; the earlier rounds' reused-variable and dead-local forms measure 81 or more.
// Point setter return 148, with outer clip fraction/plane/both 148/148/148,
// separate intersection and lower tests 148, capped fraction 213, positive
// intersection before cap 276, outer workspaces and capped fraction 213.
// NOT kept: the native-length forms leave the displaced result and clip homes
// and raise the score.
// M3 lane 6 r216 (fast with 0x263740/0x25c9d0, all MATCH; control 84): the defaults in an else arm of the second facing
// test (copies first) 101; `result = 2;` then `if (distance_squared < 36.0f) result = 3;` 136 (1292 B); the posture scale
// through a new forced-inline helper 84 (same code); the 3/2/1 level through a new forced-inline helper 234 (1268 B); the
// vehicle seat scan through a new forced-inline helper with pointer maxima 196 (1318 B). A two-second LTCG frame lab (this
// file's generated unit linked with the full build's caller and callee objects, frame homes read by name) reproduces this
// frame exactly: 25 natural respellings (facing copies, defaults and negated arm, maxima order, result-chain spellings,
// out-of-range arms, posture if/else, seat pointer or reference, world-up pointer, early forward pointer, delta through
// vector3d_from_points3d, plane through dot3f, mode as a switch, the helpers above) and 131 pairs of them all keep the
// clip amount sharing the seat counter's cell; a redundant `result = 0;` anywhere after entry emits no code and moves
// result to retail's cell 0 only (match-only); only extra blocks after the seat loop (probes, not source forms) give the
// squared distance the clip amount's cell as retail.
// M3 lane 11 r140 (H2M_FAST with 0x263740/0x26d370, both MATCH; control 84): the squared distance through dot3f of the delta
// with itself 84 (same code); declared, then assigned 84 (same code); read through a `real const &` at its five tests 150
// (1312 B); the clip test `amount >= 0.0f` for `!(0.0f > amount)` 236. Diagnostic (a file-local copy of the cross helper
// with the subtracted products written a first, not keepable): 84, the same code, so the seat-loop cross products' operand
// sides do not follow the helper's spelling.
// Common radius failure before the sole exit 239, late failure block 239; squared limits selected before classification 129.
// Primary fallback before the secondary classification 84; clipped point setter 148, inner positive gate 184, positive arm clamp 252, point initializer 84.
// NOT kept: the two ties keep the first result store and the clip slot. The remaining forms regress.
// Unscaled-radius or scaled-radius guard to the sole exit 152/152; both guards 193.
// NOT kept: these forms retain the displaced result store and raise the score.
// Complete range level expression, primary fallback on a zero result, split close/intermediate
// arms with that fallback, and explicit primary-or-zero 153/90/90/153; named squared tests
// with chained, conditional, explicit-tier or fallback result selection 84/153/84/153.
// NOT kept: Two zero-result fallback forms reach 1308 bytes at 90. The named tests tie or regress.
// The result and clip slots remain displaced.
// Direct default-level returns at mode, settings, unscaled radius or scaled radius failure 315/320/254/238.
// Paired entry guards, paired radius guards, mode with unscaled radius, and all four guards 325/222/360/317.
// NOT kept. The exits raise the score and leave the result/clip allocation unresolved.
// Default primary threshold carried across the scaled test, with fallback block, selected secondary,
// selected pair or final fallback 90/90/90/104. Conditional threshold initializers in both orders
// 101/90, carried primary with one joined pair 90, separate default primary declaration 90.
// NOT kept: the threshold lifetime variants raise the score. Result, clip and range homes still differ.
// M3 lane 5 r229 (h2m fast with 0x263740/0x26d370, both MATCH; control 84): compare operand sides, one at a time: the
// two maxima tests as `facing_secondary < a` / `facing_primary < b` 84; the motion threshold with the constant on the left
// 84; `36.0f > distance_squared` 84; `distance_squared < secondary * secondary` 84; `distance_squared < primary * primary`
// 84; `NONE != object->player_index` 84 (all the same code). Reference counts per home (normalised to the body's esp)
// sort retail's 4-byte homes and aggregates by references per byte (result 8 refs lowest, then the 1.0 group: facing
// pair, clip amount sharing with the squared distance, scale, motion, range; then counter, plane, cursor, delta, a, b);
// ours follows the same order except its first slot (clip amount shared with the seat counter, 5 refs) sits below the
// result, and the squared distance (2 refs) gets its own slot at the top.
// M3 lane 16 r174 (h2m fast with 0x263740, 0x25c9d0, 0x26d370, all MATCH; control 84, aligned 57): the facing limits
// copied at the end of the facing arm (no second facing test) 101; the scaled range as `range * scale` 84 (same code);
// the scale chosen by an if/else with a defaulted posture switch (no 1.0 initialiser) 214; the range test as
// `distance_squared < range * range` 84; the clip test as `0.0f < amount` 84 (all same code). Retail's seat-loop
// subtrahends load the up global's member first in all three side components (ours only in i). Body unchanged.
// M3 lane 12 r173 (h2m fast with 0x263740 and 0x26d370, both MATCH; control 84). Where the two bytes come from: our
// first range load goes to XMM2 and stays there on the early path, so the no-endpoint and unknown007 jumps land six bytes
// past the join's range reload while the clip path's jumps land on it; retail loads into XMM0 and every path reloads.
// Forms: the facing copies moved into the facing arm (no second `if (facing)`) 101; the seat maxima as ternaries 84 (same
// code); the seat array declared at the top of the settings block 84 (same code); the plane distance through dot3f 138
// (1308 B: the second seat cross product reschedules, the range load does not move); the plane sum with its first two
// terms parenthesised 150 (1312 B), each product parenthesised 154 (1312 B). Body unchanged.
// Optional endpoint via condition declaration, named selection, complete branches or clear
// 152/152/152/152, range after the initial delta 155/155/155/155. NOT kept:
// the result stays at +0xc and the clip fraction still shares with the seat count.
// M3 lane 5 r233 (h2m fast with 0x263740, 0x25c9d0, 0x26d370 and 0x263d70, all MATCH; control 84, aligned 55; body
// unchanged). A link-time lab with named homes (this file's unit and the nine objects of its reduced link, 1.3 s a
// variant, the full build's frame and code exactly; build/r233/lab.py and p263.py) gives ours as clip amount + seat
// down-counter, result, range, scale, facing primary, motion + side, facing secondary, cursor, plane + up, delta, a, b,
// squared distance, seats, against retail's result, facing secondary, facing primary, clip amount + squared distance,
// scale, motion + side, range, counter, plane + up, cursor, delta, a, b, seats. Both are in descending references per
// byte (2.0, 1.25, the 1.0 group, 0.75, 0.67, 0.5); what differs is the clip amount's partner and the order inside
// each equal group. Measured in the lab: one to five unused locals leave every home where it is (they only move the
// seat-loop products); the squared distance declared first in the settings block (so that it is the older symbol) the
// same frame; the clip amount pairs with the seat loop's counter in every form, the compiler's down-counter or a named
// index held in memory (a long count, a guarded do-while), and the squared distance is always placed last; the facing
// pair's order follows the order of the two copies after the seat loop (secondary copied first puts the secondary
// home below the primary one, retail's order) and a and b follow the order of the two maxima tests; a restated
// `result = 0;` moves the result first and also exchanges range and scale. Scored: the seat loop as a do-while under
// `if (count > 0)` 183 (1308 B); secondary copied first 84 (same score); one scalar for the clip amount and the squared
// distance re-measured 81, with secondary copied first 81 (the homes then run range, scale, result, facing secondary,
// amount, facing primary, motion + side, cursor, plane + up, counter, delta, a, b), with a restated result and
// secondary copied first 101 (1311 B). No form here gives the squared distance the clip amount's cell without naming
// them as one variable, and on that form range and scale still sort ahead of the facing pair where retail has them
// behind it.
// M3 lane 16 r179 (H2M_FAST with 0x263740 and 0x26d370, both MATCH; control 84): the occupant pointer declared const 84;
// the occupant's forward vector bound by const reference and passed by address to both crosses and the facing query 84;
// the clipped point declared beside `motion` 84; `bool const facing` 84 (all the same code); the scaled-range test
// through a named squared scaled range 150 (1312 B). Body unchanged.
// M3 lane 14 r149 (H2M_FAST with 0x263740 and 0x26d370, both MATCH; control 84). Diagnostic phase scan, 1..7 unused
// longs at the top: 84, 86, 138 (1308 B), 138, 82, 82, 82; the phase moves only the first seat cross's subtrahend rows (at
// 5..7 the k row is retail's, the j row never) and never the second cross's register copies or the homes. Forms: the seat
// count typed long 193 (1318 B); `vector3f side, up;` at the top of the seat-loop body 84 (same code); the first delta
// through vector3d_from_points3d 142 (1308 B); the clipped point through vector3f_add_scaled 90; the facing maxima
// initialised to 0.0f 201 (1315 B). Body unchanged.
// M3 lane 12 r182 (link-time lab with named homes, exact for the base: 0 rows against our column; control aligned 57,
// masked 12; body unchanged). Homes: every local declared at the function top in retail's slot order, the reverse
// order, or subsets (facing pair; facing pair, scale, range; amount and the squared distance either way) leave every
// home where it is (57 each, same code), so the tie order is not the declaration order. Probes (diagnostic): the
// range read after the endpoint block moves its home behind motion as retail has it (58), an extra scale store
// moves scale first (84), facing maxima initialised at the top 74: the order follows each symbol's references.
// Range: a `volatile` range (diagnostic, reverted) gives retail's entry copy through XMM0 but reads memory twice at
// the join (208, 1322 B); the range squared through an inline helper taking a const reference 57 (same code), all
// four squares through it 57; the join's range through a pointer or a reference local 76. Cross products: dead-local
// phase scan K = 0..16 has period 8 with masked 12, 12, 14, 14, 14, 10, 10, 10: the first cross's j subtrahend
// (retail the up global's i first) and the second cross's two forward.k products (retail copies the side register
// and multiplies by memory) are wrong at every K. Single forms (aligned/masked): delta through the difference
// helper 73/14, plane distance through dot3f 67/14, clipped point through the add-scaled helper 69/18, both 63/12,
// forward written at each use 85/30, the plane normal through a pointer 86/29, the two facing tests written out
// 156/26, the b maximum first 56/12, secondary copied first, squared distance or motion length through dot3f, the
// vehicle index inline, the 3/2 result as a conditional, either cross through the staged helper 57/12 each (same
// code). A random search over 150 combinations of those forms (with and without K = 5) reached 54/10 at best
// (delta helper + b maximum first + both dot3f forms). No form here lowers the count.
// M3 lane 16 r184 (H2M_FAST with 0x263740 and 0x26d370, both MATCH; control 84, aligned 57, masked 20). Tracer reading:
// the range's first piece is one register candidate from the settings read to the facing call (blocks 1 to 16, 4
// references, priority 29), coloured after the clip and loop values that hold XMM0/XMM1 in that span, so it loads into
// XMM2 where retail loads it into XMM0 (retail's piece ends at the store). The frame packer sorts these cells by
// references per byte (result, range, scale and the counter/amount cell all 1250; the facing maxima 1000), so retail's
// order (facing maxima below scale, range above the motion/side cell) needs the facing maxima denser and the range
// thinner than here. Tried: the delta as a brace initialiser 84; `real const` scaled range 84; `long const` vehicle
// index 84; `short const` seat count 84; `real const` range alone, re-measured, 84 (an earlier round listed 81) and with
// the scaled range const 84 (all the same code). A forced-ranking search ran 10 of 45 decisions in 20 minutes on this
// loaded machine before it was stopped (none matched; the closest, the first delta row's register moved, 102). Body unchanged.
// M3 lane 6 r240 (H2M_FAST with 0x263740, 0x26d370, 0x25c9d0 and 0x263d70, all MATCH; control 84, aligned 55). Reading of the
// operand ranks: in the second seat cross retail multiplies the stored side members by forward.k from memory, so there the
// side members' value numbers rank above forward.k's load (0x14008 here) and below forward.i's (0x1a008); this body's side
// values are numbered 0x117c0 and 0x12c00. Diagnostic (reverted): 336 unused initialised longs at the function top raise
// them to 0x145c0 / 0x14600 and give retail's second cross and retail's 1308 B, 53 (aligned 51); what is left then is the
// range's first register, the first cross's two subtrahends and the homes. The numbers belong to this function alone:
// its definition moved to the end of the file 84 (same code), 400 extra file-scope longs defined or only declared 84 (same
// code), 336 unused uninitialised longs 84 (same code). So retail's body creates about 323 to 875 more local symbols
// ahead of the seat loop than this one (the 0x266640 note in this file reads 254 to 1278 for its own head). First cross:
// its six loads rank up (0, 4, 6) and forward (5, 0, 2) in eighths; retail's two subtrahends need the up axis's first
// member at or above forward.k and its second at or above forward.i. Forms: the up axis through a pointer local in the
// occupant block 90 (1308 B: up becomes (6, 0, 6), the i subtrahend flips too), the same pointer ahead of the seat loop
// 156; `forward` declared after `side, up` 84 (same code). Body unchanged.
// The initial result remains four bytes high and the clip output still shares the seat-count slot.
// Setting the secondary tier then promoting it scores 136 with long, short, unsigned long and int.
// Starting with the near tier then demoting it scores 138 with all four. The original tier test is restored.
// Common radius rejection write: nested positive branches and nested negative comparison/branches
// 225/229/225; joined positive comparison/branches and joined negative comparison/branches
// 225/225/229/225; three radius states 229. NOT kept: all shrink the frame to 0x34c and raise the score.
// The first result store and clip-fraction spill still occupy different slots.
// M3 lane 1 r167 (h2m fast with 0x263740, 0x26d370 and 0x263d70, all MATCH; control 84, aligned 55, masked 17). Kept:
// the settings read through this file's `s_distance_rates_view` (its flags byte and the range at +4 named) 84 (same
// code). Tried on it: the posture switch with `default: break;` 84; the clipped point brace-initialised 84; `side, up`
// declared ahead of the seat loop 84; `a, b` declared ahead of the seat loop 84 (all the same code).
// The result still starts four bytes high and the clip fraction shares the seat count slot. Use final
// threshold storage for actor secondary or primary 200/200; for seat secondary, primary or both 84/84/84;
// for both queries' secondary, primary or both 188/188/215. The actor-only forms move the frame to 0x354.
// The seat-only forms leave the frame at 0x350 and reproduce the opening code.
// M3 lane GA r6 (19 trials, fast with 0x263740/0x26d370/0x25c9d0/0x263d70, all MATCH; control 84 at 1310 B, aligned 19 / masked 42; body unchanged):
// `real range` declared outside `if (settings)` 84; `short result = 0` 84; `vector3f_assign` for initial delta 154 (1308 B);
// `real_point3d_set` for clipped point 148 (1308 B); `mode >= 0 && mode <= 1` 100; early `return 0` out-of-range guards 222 (1296 B);
// `vector3f const &world_up = *g_4687b0` 90 (1308 B); second `if (!facing) { defaults } else { copies }` 84; plain `while (i < count)` seat loop 84;
// shared `actor_up` pointer for `plane.n` and `0x263740` 132 (1308 B); `if (result == 0 && primary*primary > distance_squared)` 90 (1308 B),
// with nested `if (mode == 0)` 90 (1308 B); duplicating tier check into both `if (facing)` arms 164 (1343 B, `result` at `[esp+8]`).
// Lead: splitting `mode == 0` so `else if (primary*primary > distance_squared) result = 1;` appears in both arms (tail-merged by MSVC) gives 86 (1310 B, +0x27, aligned 11 / masked 34), moving `result` to retail's `[esp+8]` across all five sites and placing `facing_secondary`/`facing_primary` adjacent in retail's order (+0x20/+0x24), though `amount` still pairs with `seat_counter` (+0x14) shifting `motion` off +0x24; on it `switch (mode)` 140, `else if (mode == 1)` 151, secondary copied first 86, b-max first 85, `vector3f_add_scaled` clipped point 92 (masked 31).
// GB6 (24 trials, fast with 0x263740/0x26d370/0x25c9d0/0x263d70, all MATCH, control 84): `amount = PIN(amount, 0.0f, 1.0f);
// if (amount > 0.0f)` (`amount < 0.0f ? 0.0f : (amount > 1.0f ? 1.0f : amount)`) fixes retail's `range` load into `xmm0`,
// both `je`/`jne` jumps to the `range` reload, exact `1308/1308` B and exact clamp block, scoring 97 (lead held by the
// coordinator; first cross flips two products by symbol phase and `amount` still pairs with seat counter);
// `PIN` hi-first 192 (1296 B), local `fraction = PIN(...)` 246, `PIN` + `vector3f_add_scaled` 105 (1308 B), `within_range`
// flag 239 (0x34c), `within_range` + `PIN` 231, `delta` k/j/i 104, `clipped` k/j/i 100, `plane.d` x/y/z or z/y/x 84 each,
// typed actor basis struct 131 (1308 B), `mode >= 0 && mode <= 1` + seat ref 98. On `PIN`: `vector3d_from_points3d` delta
// 156, `basis_dot3f` plane 152, secondary copied first 97, b-max first 96, both secondary-first 116, `amount` or `amount`+
// `distance_squared` hoisted 97 each, `cross3f_staged` 97, seat ptr 91 (1310 B), `range` after `delta` 101 (with sec-first
// 101); diagnostic (not keepable, reused variable): `PIN` + one scalar for `amount`/`distance_squared` 69 (1308 B, exact
// retail offsets for `range` +0x30, `delta` +0x4c, `motion` +0x24, `plane` +0x38, `a`/`b` +0x58/+0x5c, `facing_primary`
// +0x18). Body unchanged.
// GB9 (43 trials, fast with 0x263740/0x26d370/0x25c9d0/0x263d70, all MATCH, control 84 at 1310/1308 B, al=109, espm=22, mk=59;
// body unchanged): combining `PIN(amount, 0.0f, 1.0f)` (97 alone) + split `mode == 0` (86 alone) reaches 76 (1308/1308 B, +0x27,
// `result` at +8, `range` in xmm0 and at retail's +0x30, `motion+side` at +0x24; with b-max first 75; `NOT kept`: both parts alone
// regress and `al=111, espm=25, mk=68`, `amount` still pairs with seat counter at +0x14). On `PIN`+split: sec-first 76, single
// `if (facing)` 140, ternary 3:2 76, `vector3f_add_scaled` 84, no `clipped` 140, `plane.d` `dot3f` 139, `delta` helper 143,
// `long i` 166 (on base 173), scoped `facing_secondary`/`facing_primary` 140 (with `bool facing` 95, init before `if` 193, else-first
// 140), `range` after `delta` 81, else sec-first 96. Passing `&secondary, &primary` directly to 0x263740 (dropping `facing_secondary`/
// `facing_primary`) cleanly pairs `amount` with `distance_squared` at retail's exact `+0x1c` (`T09` 190 at 1334 B, `B2_10` with `bool
// facing` 187 at 1331 B, `al=92`; on base 199, on split 195; init-before 203, else-first/ternary/b-max/else-sec/range-after 190-210,
// `real_bounds` 192-243, `real_bounds` for `facing_limits` 100/108), matching 11 of 13 stack slots and 14/16 first-cross insns, but
// emits +23..26 B of extra `xmm1`/`xmm2` reloads of `secondary`/`primary` inside the seat loop.
// M3 lane GA r13 (30 compiled trials across 2 batches, fast with 0x263740, 0x26d370, 0x25c9d0, 0x263d70, all MATCH;
// control 84 at 1310/1308 B, al=109, mk=20; body unchanged): on `PIN(amount, 0.0f, 1.0f)` (97): `seat_end = seats + count`
// pointer walk 171 (on base 177, on `PIN`+split 171: end-pointer still pairs with `amount`), countdown `rem = count` 133,
// `word` loop index 184, hoisted `distance_squared`/`amount` 97/97, `dot3f(&delta,&delta)` 97, `dot3f` plane 152, `if/else`
// posture 126, `short result` 97, early-return mode guard 315, no `bool facing` 152. Combining `PIN` (97) + split `mode == 0`
// (86) + two-use `s_object_seat const *seat = &seats[i]` (90 alone, 92 with split, 91 with `PIN`) reaches 70 (1310/1308 B,
// +0x27, al=98, mk=12; + b-max first 69, al=97, mk=12; `cross3f_staged` 1st/2nd or `dot3f` motion 70 each; `PIN`+split+sec-first
// +b-max 75 at 1308 B, mk=20): NOT kept (every component regresses alone and `amount` still pairs with `seat_counter` at
// +0x14). On `T06` (70): direct `&secondary, &primary` 184 (1328 B, pairs `amount`+`distance_squared` at +0x1c), direct
// secondary-only 188 / primary-only 187 (both revert `amount` to +0x14), scoped `facing_secondary/primary` inside `if (facing)`
// 89 (1308 B), defaults-first before `if` 190, inlined `scale * range` 141, shared two-use `actor_up` pointer 144.
// GB12 (44 fast trials with 0x263740/0x26d370/0x25c9d0/0x263d70, all MATCH, control 84 at 1310/1308 B, al=63, mk=20;
// body unchanged): two-use `s_object_seat const *seat = &seats[i]` alone 90 (1308 B); on `PIN`+split `mode == 0` (76 at
// 1308 B, al=65, mk=23) adding `seat` drops score to 70 (1310 B, al=52, mk=12, fixing 7 insns in the first seat cross;
// with b-max first 69, al=51, mk=12; `NOT kept`: parts alone regress and `amount` still pairs with seat counter at +0x14).
// Also tried on `PIN`+split (or `B12`): ternary MAX `a > f ? a : f` 76 / rev 134 (on base 84/141), 2-caller inline tier
// helper 278/285 (on base 271), inline/forceinline facing helper 818/198 (770/270), direct `settings->distance_limit`
// 268 (on base 268), `distance_squared` declared at top 76, single `if` facing 140, `while` seat loop 76, `posture` if/else
// 110, `short result` 76, `flag7` local 312, `plane` helper 84/247, `switch (mode)` 266, `amount >= 0.0f` 236; on `B12`:
// `cross3f_staged` 70, `matrix3x3` basis ptr 117 (1308 B), hoisted `world_up` 141, `exceeded` flag 72 (on base 239),
// `result == 0` guard/nested 97/97 (on base 90), `range` after delta 75, sec-first copy 70, `if (count > 0)` 145.
// M3 lane 7 r230 (h2m fast with 0x263740, 0x26d370, 0x25c9d0 and 0x263d70, all MATCH; control 84): the frame packer
// counts scale and range at 5 references each where retail's layout needs 4 (both final codes have 4: ours counts one
// store per posture case and two range loads ahead of the join). Forms aimed at one posture-scale store: both cases
// sharing one `posture == 0 ? 0.3f : 0.7f` assignment 214 (1312 B); the posture tested by an `if` with that conditional
// 181 (1311 B); the whole scale as one nested conditional at its declaration 219 (1308 B); an explicit `default: break;`
// 84 (same code); the clip clamp as a conditional assignment 84 (same code). Body unchanged.
// M3 lane 12 r193 (H2M_FAST with 0x263740, 0x26d370, 0x25c9d0 and 0x263d70, all MATCH; control 84). Frame packer reading:
// cells are packed by size, then references, and a joining value tries the newest cells first, so the clip amount (2
// references) meets the seat loop's two compiler temporaries (3 references each, packed before it) and joins the newer
// one; retail's pairing with the squared distance needs those temporaries to interfere with it or to pack after it.
// Forms (all 84, the same code): the seat buffer declared at the top of the facing block; `side` and `up` declared
// apart, up first; the occupant pointer to const; the two facing maxima declared ahead of the crosses; the facing flag
// set by an `if`. Body unchanged.
// GB22 (26 fast trials with 0x263740/0x26d370/0x25c9d0/0x263d70 MATCH, control 84 at 1310/1308 B; body unchanged):
// nested 0x26d370/amount if 84 (on PIN+split with extra guard 247); inline pin_real helper 97 (1308 B, range->xmm0);
// !(distance_squared >= limit) compares 89; parameter address reads (like file peers) actor_ref 152 (143 on PIN+split),
// mode_ref 164, origin_ref 170 (breaks peer); while (i < count) seat=&seats[i++] 182 (1308 B; 174 on PIN+split);
// on PIN+split (76): inline posture_scale helper 76 (+ seat ptr 70, al 12, mk 19), plane.d z+y+x 76; direct &secondary/&primary
// with MAX ternary, real* pointers, reversed if, or real limits[2] array all cleanly pair amount+distance_squared (+0x1c/+0x18)
// with result=+8 and range=xmm0 but keep seat-loop xmm1/xmm2 reloads (190/190/190/195 at 1331-1334 B).
// @retail 0x263810
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range)
{
	long result = 0;
	if (mode == 0 || mode == 1)
	{
		s_actor_view *actor = actor_get(actor_index);
		s_distance_rates_view *settings = (s_distance_rates_view *)actor_get_character_situation_entry(actor_index);
		if (settings)
		{
			real range = settings->distance_limit;
			vector3f delta;
			delta.i = point->x - origin->x;
			delta.j = point->y - origin->y;
			delta.k = point->z - origin->z;
			if (endpoint && !actor->unknown007)
			{
				vector3f motion;
				vector3d_from_points3d(point, endpoint, &motion);
				if (length_sq3f(&motion) > 0.002500000176951289f)
				{
					plane3f plane;
					plane.n = *(vector3f *)((byte *)actor + 0x2c0);
					plane.d = origin->y * plane.n.j + origin->z * plane.n.k + origin->x * plane.n.i;
					real amount;
					if (function_26d370(point, &motion, &plane, &amount) && !(0.0f > amount))
					{
						if (amount > 1.0f) amount = 1.0f;
						if (amount > 0.0f)
						{
							point3f clipped;
							clipped.x = motion.i * amount + point->x;
							clipped.y = motion.j * amount + point->y;
							clipped.z = motion.k * amount + point->z;
							vector3d_from_points3d(origin, &clipped, &delta);
						}
					}
				}
			}
			real distance_squared = length_sq3f(&delta);
			if (range * range > distance_squared)
			{
				real scale = 1.0f;
				if (!(settings->flags & 1))
				{
					switch (posture)
					{
					case 0: scale = 0.3f; break;
					case 1: scale = 0.7f; break;
					}
				}
				real scaled_range = scale * range;
				if (scaled_range * scaled_range > distance_squared)
				{
					real secondary, primary;
					real facing_secondary, facing_primary;
					bool facing = !actor->unknown007 && use_facing;
					if (facing)
					{
						function_263740(actor_index, &delta, (vector3f *)((byte *)actor + 0x2a8),
							(vector3f *)((byte *)actor + 0x2b4), (vector3f *)((byte *)actor + 0x2c0),
							range, scale, &facing_secondary, &facing_primary);
						long object_index = *(long *)((byte *)actor + 0x26c);
						if (object_index != NONE)
						{
							s_object_seat seats[64];
							short count = function_c8b80(object_index, seats, 64);
							for (short i = 0; i < count; ++i)
							{
								long occupant = function_c8f60(seats[i].object_index, seats[i].seat_index);
								if (occupant != NONE)
								{
									s_slot_object_view *object = object_get(occupant);
									if (object->player_index != NONE)
									{
										vector3f const *forward = (vector3f *)((byte *)object + 0x18c);
										vector3f side, up;
										cross3f(g_4687b0, forward, &side);
										function_30bf0(&side);
										cross3f(forward, &side, &up);
										real a, b;
										function_263740(actor_index, &delta, forward, &side, &up, range, scale, &a, &b);
										if (a > facing_secondary) facing_secondary = a;
										if (b > facing_primary) facing_primary = b;
									}
								}
							}
						}
					}
					else
					{
						primary = scaled_range;
						secondary = scaled_range * 0.7f;
					}
					if (facing)
					{
						primary = facing_primary;
						secondary = facing_secondary;
					}
					if (mode == 0 && secondary * secondary > distance_squared)
					{
						if (distance_squared < 36.0f)
							result = 3;
						else
							result = 2;
					}
					else if (primary * primary > distance_squared)
						result = 1;
				}
				else
				{
					if (out_of_range) *out_of_range = true;
				}
			}
			else
			{
				if (out_of_range) *out_of_range = true;
			}
		}
	}
	return result;
}

short const g_44ae3c[13] = { 1, 1, 2, 3, 4, 6, 7, 8, 8, 9, 9, 9, 9 };
short g_470fa4[4] = { 0, 0, 2, 3 };
short g_470fe0[3] = { 1, 2, 3 };

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void *function_272a00(long actor_index);
bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);

// @retail 0x2662f0
void function_2662f0(long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(*actor_reference);
	short desired;
	long task_index = actor->unknown858;
	if (task_index != NONE)
	{
		byte *task = g_502408->data + (task_index & 0xffff) * 0xd4;
		if (*(short *)(task + 0x84) > 0)
		{
			desired = *(short *)(task + 0x84) - 1;
			*(long *)actor->unknown088 = 0;
			goto update;
		}
	}
	desired = g_44ae3c[actor->unknown328];
	{
		short clump_minimum = 1;
		short recent_minimum = 0;
		if (actor->unknown07c != NONE)
		{
			s_clump *group = (s_clump *)(g_502420->data + (actor->unknown07c & 0xffff) * 0x50);
			clump_minimum = g_470fa4[group->state];
		}
		if (actor->unknown26c == NONE && desired == 1 && actor->unknown086 == 0)
			desired = 0;
		if (actor->unknown358 > 0)
			recent_minimum = 2;
		desired = MAX(desired, MAX(clump_minimum, recent_minimum));
	}
	if (desired < 3)
	{
		if (actor->unknown086 > 3)
		{
			actor->unknown086 = 3;
			*(long *)actor->unknown088 = 0;
		}
		{
			byte *definition = NULL;
			if (actor->unknown030 != NONE)
			{
				byte *squad = g_51e9d8->data + (actor->unknown030 & 0xffff) * 0x98;
				definition = *(short *)(squad + 0x2a) != NONE ?
					*(byte **)((byte *)g_4e0350 + 0x244) + *(short *)(squad + 0x2a) * 0x7c : NULL;
			}
			if (definition && desired < *(short *)(definition + 0x28) - 1)
				desired = *(short *)(definition + 0x28) - 1;
			else
			{
				byte *variant = (byte *)function_272a00(*actor_reference);
				if (variant)
				{
					short level = g_470fe0[*(short *)(variant + 0x20)];
					if (actor->unknown086 >= level && desired < level)
						desired = level;
				}
			}
		}
		if (desired < actor->unknown086)
		{
			short timer = *(short *)actor->unknown088;
			desired = actor->unknown086;
			switch (actor->unknown086)
			{
			case 3:
				if (timer * g_510c54->rate >= 0.0f)
					desired = 2;
				else
					break;
			case 2:
				if (timer * g_510c54->rate >= 15.0f)
					desired = 1;
				break;
			}
		}
		else
			*(long *)actor->unknown088 = 0;
	}
	else
		*(long *)actor->unknown088 = 0;
update:
	if (actor->unknown086 != desired)
	{
		if (desired >= 3 && actor->unknown086 < 2)
			function_1a8220(*actor_reference, 7, 2, 3, 1, 42, 3);
		*(long *)actor->unknown088 = 0;
		actor->unknown086 = desired;
	}
	else
		++*(long *)actor->unknown088;
	if (actor->unknown086 >= 7)
		*(bool *)((byte *)actor + 0x8c) = true;
}

// @retail 0x263d30
long function_263d30(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result;
	if (actor->unknown086 >= 3)
		result = 2;
	else
		result = actor->unknown084 >= 4;
	return result;
}

PRIVATE __forceinline s_type_f95cd3 *actor_tracked_prop_view(long prop_index)
{
	s_prop_node_view *node = prop_node_get(prop_index);
	s_type_f95cd3 *result = NULL;
	if (node->view_index != NONE)
	{
		byte *base = g_502414->data + (node->view_index & 0xffff) * sizeof(s_type_e5ff81);
		if (base)
			result = (s_type_f95cd3 *)(base + 0x70);
	}
	return result;
}

// @retail 0x265c30
void function_265c30(long prop_index, long actor_index, bool active)
{
	long const volatile *actor_reference = &actor_index;
	bool const *active_reference = &active;
	s_type_f95cd3 *view = actor_tracked_prop_view(prop_index);
	if (view)
	{
		if (*active_reference)
			view->unknown50 = g_510c54->game_time;
		else
			view->unknown50 = NONE;
		*(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, prop_index);
	}
}

// @retail 0x267770
void function_267770(long prop_index, long actor_index)
{
	long const *actor_reference = &actor_index;
	s_actor_view *actor = actor_get(actor_index);
	long previous = *(long *)((byte *)actor + 0x338);
	*(long *)((byte *)actor + 0x338) = prop_index;
	if (previous != NONE)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(previous);
		if (view) *(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, previous);
	}
	if (prop_index != NONE)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(prop_index);
		if (view) *(real *)((byte *)view + 0x3c) = function_265d30(*actor_reference, *(long *)((byte *)actor + 0x338));
	}
}

// @retail 0x267180
short function_267180(s_prop_datum *node)
{
	s_type_f95cd3 *view = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	long result;
	short state = node->state;
	if (!state)
		result = 0;
	else if (node->type != 1 && node->type != 6)
		result = 0;
	else if (function_25d690(node)->unknown5e)
		result = 1;
	else if (view->unknown2a)
		result = 12;
	else if (state >= 1 && state <= 2)
	{
		result = 6;
		if (node->unknown27 >= 1)
		{
			if (node->unknown28 < 1.0f)
				result = 11;
			else
			{
				if (node->unknown28 < 12.0f)
					result = 7;
				char level = view->unknown39;
				if (level <= 2)
				{
					result = 8;
					if (function_25d690(node)->unknown63)
					{
						result = 9;
						if (level <= 1)
							result = 10;
					}
				}
			}
		}
	}
	else if (state >= 3)
	{
		if (view->unknown69)
			result = 2;
		else if (view->unknown88)
			result = 3;
		else if ((g_510c54->game_time - view->unknown10) * g_510c54->rate < 1.0f && *(short *)view >= 5)
			result = 5;
		else
			result = 4;
	}
	if (view)
		*(short *)view = (short)result;
	return (short)result;
}

struct s_actor_prop_iterator
{
	long index;
	long next;
};

PRIVATE __forceinline s_prop_node_view *actor_next_prop(s_actor_prop_iterator *iterator)
{
	s_prop_node_view *node = NULL;
	long index = iterator->next;
	if (index != NONE)
	{
		node = prop_node_get(index);
		iterator->index = index;
		iterator->next = node->next_index;
	}
	return node;
}

// @retail 0x265cb0
void function_265cb0(long actor_index)
{
	s_actor_prop_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = actor_next_prop(&iterator)) != NULL)
	{
		s_prop_view_fields *view = prop_node_view(node);
		if (view)
			view->unknown3c = function_265d30(actor_index, iterator.index);
	}
}

// @retail 0x265bb0
void function_265bb0(long actor_index)
{
	s_actor_prop_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *node;
	while ((node = actor_next_prop(&iterator)) != NULL)
	{
		s_type_f95cd3 *view = actor_tracked_prop_view(iterator.index);
		if (view)
		{
			view->unknown2a = false;
			*(short *)((byte *)view + 0x28) = NONE;
		}
	}
}

struct s_prop_thresholds
{
	byte unknown000[0x158];
	real actor_threshold;
	real object_threshold;
};

struct s_prop_threshold_table
{
	byte unknown000[0xc8];
	long count;
	s_prop_thresholds *entries;
};

long function_1e4990(long index);

long function_1e1f20(long actor_index);
long function_cbd50(long unit_index, short weapon_index);

struct s_object_activity_flags
{
	byte unknown00[0x10a];
	word flag0 : 1;
	word flag1 : 1;
	word blocked : 1;
	word unused : 13;
};

/* the object's summed value: its actor's base (or, for a player, the settings' one), its seat's
   value, the tag values of the two objects looked up for it and, when the actor's flag 0x225 is
   set, the settings' second value */
// @retail 0x267370
real function_267370(long object_index)
{
	real result = 0.0f;
	if (object_index != NONE)
	{
		s_slot_object_view *object = object_get(object_index);
		byte *settings = NULL;
		if (((s_prop_threshold_table *)g_4e034c)->count > 0)
			settings = (byte *)((s_prop_threshold_table *)g_4e034c)->entries;
		if (!((bool)((s_object_activity_flags *)object)->blocked) && ((1 << object->type) & 3))
		{
			s_actor_view *actor = NULL;
			long actor_index = object->actor_index;
			if (actor_index != NONE)
				actor = actor_get(actor_index);
			long first;
			if (actor)
			{
				result = *(real *)((byte *)function_1e4990(actor->unknown054) + 8);
				first = function_1e1f20(actor_index);
			}
			else
			{
				first = function_cbd50(object_index, *(char *)((byte *)object + 0x212));
				if (object->player_index != NONE && settings)
					result = *(real *)(settings + 0x160);
			}
			long second = function_cbd50(object_index, *(char *)((byte *)object + 0x213));
			if (object->parent_index != NONE)
			{
				s_slot_object_view *parent = object_get(object->parent_index);
				if (parent->type == 1 && object->unknown1fc != NONE)
				{
					byte *tag = g_4e3b44[parent->tag_index & 0xffff].bytes;
					byte *seats = *(byte **)(tag + 0x1cc);
					result = *(real *)(seats + object->unknown1fc * 0xb0 + 0x38) + result;
				}
			}
			if (first != NONE)
				result = *(real *)(g_4e3b44[object_get(first)->tag_index & 0xffff].bytes + 0x238) + result;
			if (second != NONE)
				result = *(real *)(g_4e3b44[object_get(second)->tag_index & 0xffff].bytes + 0x238) + result;
			if (actor && actor->unknown225 && settings)
				result = *(real *)(settings + 0x164) + result;
		}
	}
	return result;
}

// @retail 0x267700
void function_267700(long actor_index, s_prop_datum *node, s_type_76cf92 *prop, s_type_f95cd3 *view)
{
	s_prop_datum *const *node_reference = &node;
	s_actor_view *actor = actor_get(actor_index);
	real value;
	if (prop->actor_index != NONE)
		value = *(real *)((byte *)actor_get(prop->actor_index) + 0x2cc);
	else
		value = function_267370((*node_reference)->object_index);
	value -= *(real *)((byte *)actor + 0x2cc);
	if (value < 0.0f)
		view->unknown60 = 0.0f;
	else
		view->unknown60 = value;
}

// Parameter order (point, direction, position, distance, unknown) is retail's: the callers' argument
// setup in 0x267c50 sets the NULL distance (ecx) before the address leas only with distance fourth,
// and with it 0x267c50 matches while this function still matches (found by MiniMax S and E, 11-10).
// @retail 0x267a80
short function_267a80(point3f const *point, vector3f const *direction, point3f const *position, real *distance, vector3f *unknown)
{
	long result = 0;
	vector3f *offset = (vector3f *)unknown;
	if (distance)
		*distance = 0.0f;
	if (offset)
		*offset = *g_4687a4;
	point2f planar = *(point2f const *)direction;
	real magnitude = (real)sqrt(planar.x * planar.x + planar.y * planar.y);
	if (fabs(magnitude) < 0.0001f)
		magnitude = 0.0f;
	else
	{
		real scale = 1.0f / magnitude;
		planar.x *= scale;
		planar.y = ((point2f volatile *)&planar)->y * scale;
	}
	if (magnitude > 0.0f)
	{
		vector3f delta;
		delta.i = position->x - point->x;
		delta.j = position->y - point->y;
		delta.k = position->z - point->z;
		real length = (real)sqrt(delta.i * delta.i + delta.j * delta.j);
		if (distance)
			*distance = length;
		real projection = delta.i * planar.x + delta.j * planar.y;
		if (projection > length * 0.8660253882408142f)
		{
			real scale = 0.0f - projection;
			delta.i = direction->i * scale + delta.i;
			delta.j = direction->j * scale + delta.j;
			delta.k = direction->k * scale + delta.k;
			if (offset)
			{
				offset->i = 0.0f - delta.i;
				offset->j = 0.0f - delta.j;
				offset->k = 0.0f - delta.k;
			}
			if (delta.k > -0.5f && delta.k < 0.9f)
				result = 2;
			else if (delta.k > -0.8f && delta.k < 1.2f)
				result = 1;
			else
				goto done;
			real squared = delta.i * delta.i + delta.j * delta.j;
			if (squared < 0.36000001430511475f)
				goto done;
			if (squared < 1.2100000381469727f)
			{
				result = 1;
				goto done;
			}
		}
	}
	result = 0;
done:
	return (short)result;
}

s_ai_player *ai_player_get(long player_index);
void function_cb7e0(long unit_index, vector3f *vector);
bool function_1e1e50(long actor_index, vector3f *direction);
void function_26be00(long actor_index, s_iterator *iterator);
real function_30bf0(vector3f *vector);

struct s_node_view;
s_node_view *function_26be30(s_iterator *iterator);

// @retail 0x267840
bool function_267840(long actor_index, long prop_index, vector3f *direction)
{
	s_actor_view *actor = actor_get(actor_index);
	s_prop_datum *node = prop_ref_get(prop_index);
	s_type_76cf92 *prop = prop_get(node->prop_index);
	s_type_5cfb45 *state = function_25d690(node);
	volatile bool result = false;
	if (!prop->unknown22)
	{
		if (prop->unknown25)
		{
			s_slot_object_view *object = object_get(*(long *)((byte *)prop + 8));
			s_ai_player *player;
			if (object->player_index != NONE && (player = ai_player_get(object->player_index)) != NULL)
			{
				if (state->unknown63)
					result = true;
				else
				{
					long now = g_510c54->game_time;
					real ticks = (real)g_510c54->field_2_3 * 2.5f;
					long rounded;
					__asm { fld ticks }
					__asm { fistp rounded }
					long elapsed = now - player->unknown0c;
					result = false;
					if (elapsed < rounded) result = true;
				}
				function_cb7e0(node->object_index, direction);
				if (!result && *(char *)((byte *)actor + 0x324) > 0)
				{
					s_iterator iterator;
					function_26be00(actor_index, &iterator);
					s_prop_datum *other;
					while ((other = (s_prop_datum *)function_26be30(&iterator)) != NULL)
					{
						if (other->state >= 1 && prop_get(other->prop_index)->unknown23)
						{
							s_type_5cfb45 *other_state = function_25d690(other);
							vector3f delta;
							vector3d_from_points3d(&state->position, &other_state->position, &delta);
							if (function_30bf0(&delta) > 0.0f &&
								direction->i * delta.i + direction->j * delta.j + direction->k * delta.k > 0.5f)
							{
								result = true;
								break;
							}
						}
					}
				}
			}
		}
		else if (prop->actor_index != NONE)
			result = function_1e1e50(prop->actor_index, direction);
	}
	else
		result = false;
	return result;
}

// @retail 0x267550
bool function_267550(long actor_index)
{
	byte *definition = (byte *)function_1e4990(actor_get(actor_index)->unknown054);
	bool result = false;
	if (definition && ((s_prop_threshold_table *)g_4e034c)->count > 0)
		result = *(real *)(definition + 8) >= ((s_prop_threshold_table *)g_4e034c)->entries->actor_threshold;
	return result;
}

struct s_equipped_object_view
{
	long tag_index;
	byte unknown004[0x212 - 4];
	char selected;
	byte unknown213[5];
	long objects[4];
};

/* The held-object lookup rereads the pool's data field. */

// @retail 0x2675f0
bool function_2675f0(long object_index)
{
	s_equipped_object_view *object = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[object_index & 0xffff].object;
	long held_index = NONE;
	short selected = object->selected;
	if (selected != NONE)
		held_index = object->objects[selected];
	bool result = false;
	if (held_index != NONE && ((s_prop_threshold_table *)g_4e034c)->count > 0)
	{
		s_prop_thresholds *thresholds = ((s_prop_threshold_table *)g_4e034c)->entries;
		real held_value = *(real *)(g_4e3b44[((s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[held_index & 0xffff].object)->tag_index & 0xffff].bytes + 0x238);
		result = thresholds->object_threshold <= held_value;
	}
	return result;
}

// Keep the prop lookup out of line in this accessor.
#pragma inline_depth(0)
// @retail 0x2684f0
short function_2684f0(s_actor_view *actor)
{
	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_view_fields_get(actor->prop_index);
		if (view)
			return view->unknown00;
	}
	return 0;
}
#pragma inline_depth(255)

long function_1e4a10(long index);
short actor_behavior_find_level(long owner_index, short type);
struct s_node_view;
s_node_view *function_26be30(s_iterator *iterator);
void function_1f86a0(long actor_index);
void function_262800(long actor_index, s_reference reference, bool unknown);

PRIVATE __forceinline long round_tick_count(real ticks)
{
	long result;
	__asm { fld ticks }
	__asm { fistp result }
	return result;
}

long function_146650(void);

// MiniMax S (11-10): retail's 0x267a80 takes (point, direction, position, distance, unknown); with that order this function
// matches and 0x267a80 still matches (its body matches under both orders, only the callers differ), but the change needs
// src/unknown_25fc30.cpp's declaration (line 73) and its two call sites (lines 1221, 1244), which is outside this lane - reported, not applied.
// MiniMax E (11-10): the parameter layout is retail's, confirmed from the compiled prologue of 0x267c50,
// which matches retail opcode for opcode (`mov ebx, [esp + 0x68]`, i.e. the single stack argument
// at [E+4], and the same `and ebp, 0xffff` / `imul ebp, 0x888` index math), so the one argument here is the
// same in both and nothing is blocked by the declaration.
// MiniMax E (11-10): no new form, no lower score (7). The NULL distance argument written as 0 instead
// of NULL compiles to the same code (7). The one remaining difference is where the `xor ecx, ecx` for
// that NULL distance is emitted: retail sets ECX before the local-address leas of EDI (and of EDX in the
// projected call), ours sets it last. Retail's order is the reverse of the argument order with the
// distance argument fourth, i.e. it matches 0x267a80 declared as (point, direction, position, distance,
// unknown); that is a parameter-order change, which is a banned form, so it is not applied here.
// score 7 (was 7): the five 12-byte homes now agree; only the argument setup at the three 0x267a80 calls
// differs: retail sets ECX (the NULL distance, `xor ecx, ecx`) before the local-address lea of EDI (and of EDX
// in the projected call), ours sets ECX last. Earlier note, from before the homes agreed: retail stacks
// movement, offset, ahead, target_direction and direction upward from esp+0x34, ours movement, offset,
// direction, ahead and target_direction, and the lea/xor order at the three 0x267a80 calls follows it.
// The direction and ahead homes are swapped: retail stacks movement, offset,
// ahead, target_direction and direction, ours movement, offset, direction, ahead and
// target_direction. Declaration order, scope, type, initialiser and extra source references all
// leave the homes as they are.
// Only three vector homes differ: retail stacks movement, offset, ahead,
// target_direction, direction upward from esp+0x34; ours movement, offset, direction, ahead,
// target_direction (12-byte slots; names and declaration places do not move them).
// Round 108 (a compile-only /FAs screen of this file gives exactly our homes): renaming direction
// five ways and moving the ahead, target_direction, direction, movement and offset declarations
// (seven places) leave the homes as they are, so local names do not order these slots here. A new
// one-use vector lands between offset and direction wherever its use is, and last when declared
// innermost; one more `&target_direction` anywhere puts it lowest. Real (fast): the 0x267840 result
// in a named bool 14; the target query nested under `if (check_target)` 14; the larger query result
// clamped by an if 314 (1934 bytes); the position through a named point pointer for ahead 14; the
// first query's arguments on one line 14; direction declared with target_direction 14.
// Tried, round 22 (fast, plain 14): `long ticks` with the decay after the if 99, with `real
// ticks` 384 (SSE conversion), `double ticks` 394 (qword constant); the volatile decay dropped
// 415; the shared real_to_long 170; the decay computed in each arm (kept: 26, the rounding is
// then filded straight from its slot); the game-time read through the existing getter (kept:
// 16, retail reads the time before the rounding); `>= 1 ? 3 : 1` for the last state (kept: 14),
// both states as one ternary 39, the outer ternary 36; vector names (direction, target, ahead
// renamed) 14 each; direction declared outside the loop, at the loop body's top, beside or
// before target_direction 14 each; ahead declared with movement 14; ahead through
// vector3f_add_scaled or real_point3d_set 16/16 (before the tail fix).
// Earlier (score 99, the old ticks form): ordinary pooled/clock reads with memset zero 178;
// projected-point loop 297; wide query results 318; named maximum 146; target-copy loop 141;
// ordinary-access common tick conversion 400; native point setter 146; activity lifetime 344;
// iterator rejection continue 146; elapsed interval 293; per-use prop lookup 119; shared
// dot/length/both 146/146/146; query output initialization omitted 347; prop read first 326;
// native fields 146; false-byte array memset 206; wide event results 371; wide target result
// 374; state per use 433; state eligibility snapshot 338; typed offset output 99; offset outer
// direction 99; offset blend before delta 99; offset outer query homes 99; branch decay result
// 398; delta before conversion 100; smoothing value samples 345; sampled early delta 389;
// native rounded count 99; stored width rounded count 396; branch decay late delta 394; sampled
// branch decay 402; ordinary conversion inline 373; branch exponent result 416; smoothed field
// 99; desired field 99; both fields 99; native position 393; native origin 383; both geometry
// inputs 431; prop record 99; field and geometry 431; interpolation product inputs 99; direct
// delta 99; direct blend 99; delta-first addition 99; direct pair 414; direct delta/addition 99;
// direct blend/addition 99; direct pair/addition 414; ticks ternary 99; rounding of a ternary
// 170; 0.25 first 99; negated test 103; a seconds-to-ticks helper for both or one arm 99; vector
// local names 99; vector declarations moved 99; native smoothing fields/cached clock/native byte
// flags, seven combinations 99/425/425/99/99/425/425/425; native target assignment 133; offset
// setter 101; both 135; projected-point aggregate 99; target/aggregate 133; offset/aggregate
// 101; all producers 135; read-only projected point 135; uninitialized difference/decay/blend
// entry declarations, all seven subsets, and with the rounded count at entry 99 each.
// Round 427 (old ticks form): exact quarter/player-window/hold clock products using double literals, seven
// subsets 423/256/425/133/424/257/426; all with the rounded-count name 426. Every signed-short
// product remains exactly representable. Original literals, count name and full body restored;
// hoisted clock conversion, joined count copy and geometry homes remain.
// Round 21 (lane 13): ahead as a copy of the position plus `+=` steps 209 (1974 bytes);
// real_point3d_set 14; vector3f_add_scaled 14; length_sq3f and dot3f for the two sums 14;
// offset zeroed by a chained assignment 18. A compile-only model of this file gives our exact
// homes: names, declaration order/scope and symbol count never move them; the slot order follows
// how many references each vector has (two fewer `&direction` uses give retail's order).
// Round 43 (per-arm decay form): target/movement/both library copies 14/14/14;
// target/movement/both vector setters 77/186/224; library target plus movement setter 186;
// target setter plus library movement 77. The setters disturb the scalar/geometry allocation;
// original producers restored. The three 12-byte home rotation and query setup remain.
// Round 60: a direction pointer at all three calls 14; a direction reference 14; a post-writer
// direction pointer at both queries 14; a pointer only at the first query 14; a target pointer 14;
// a movement pointer 14; an ahead pointer 14; an offset pointer 14. Every listing is identical;
// original executable body restored.
// Round 121 (lane 8): a compile-only frame screen ranks the 12-byte homes by references that survive to code: one more
// materialized target_direction read puts it above ahead, two above direction; two more ahead reads put it above
// direction; forwarded pointers, inline pointer helpers and reads a common subexpression removes do not count. Retail's
// order needs target and ahead each above direction. Real (fast): ahead from a position copy, then its fields, 197 (1972
// bytes; ahead's home right, the copy is not removed); target zeroed at its declaration 342 (1977); both 372 (1999);
// the ahead copy with a library copy of the target 197 (1972); an ahead pointer for the fields and the query 14.
// Round 65 (m3 lane 10, fast): target_direction copied field by field from the view 77 (1961 B, SSE copy); ahead as an
// aggregate initializer 14 (same code); offset zeroed by a chained assignment after its declaration 18; a state_position
// pointer for the three queries 125; an origin pointer for the actor position at all four uses 36. Permuter (600 s):
// its reduced image scores 248 against the full 14, nothing better in 49 tries.
// Round 63 (m3 lane 14, fast): the prop test as nested ifs 14; as an inverted test with `continue` 14; the movement test as
// nested ifs 14; the target query's result named in a nested if 14; the query's point/direction/position parameters not
// const 14; ahead a const aggregate 14. Compile-only frame screen, references dropped with liveness kept: without the
// first query's direction, ahead > direction > target; without the second's, direction > ahead > target (no change);
// without both, retail's ahead > target > direction. So the two query references do not weigh the same, and an unused
// or input-only vector shares target_direction's slot, so a new vector does not test a reference's weight. The direction
// output and first query references are what retail's order lacks.
// Round 144: actor-reference reads at entry/all behavior queries/final callback, then direct actor_index at the
// direction producer/iterator setup/movement callback, and split proximity/projected declarations: 14 each,
// identical listings. NOT kept: no allocation change; the original parameter observations and declarations restored.
// Round 139 (m3 lane 8; compile-only frame screen, exact for these homes, then fast): diagnostics with a dummy global in
// place of each direction reference: without the 0x267840 output direction is never written and shares
// target_direction's slot; without the first query ahead > direction > target; without the second no change; without
// both retail's ahead > target > direction. Extra target reads count the same outside the loop, at the loop top or
// deep inside (one passes ahead, two pass direction), so nesting does not weigh them; `(void)&x` counts nothing.
// Real (fast): `active = state->unknown63; if (!active)` (code differs) not built; ahead as a vector3f 14; the
// player lookup as nested ifs 14; offset declared before `active` 18; movement declared ahead of its test 14;
// target_direction declared at the function's top 14.
// Round 99 (m3 lane 12): the homes were weighted by the direction pointer escaping: 0x267840 passes it to
// 0x1e1e50, which read its own direction parameter through its address; with 0x1e1e50 writing `*direction`
// directly (still matched) the homes are retail's, 14 -> 7 (plain). A compile-only screen showed it: the
// homes follow whether 0x267840's two outgoing uses expose the pointer. Then (fast): 0x267a80's parameters
// reordered everywhere to (direction, position, distance, point, unknown) 167 (0x267a80 loses its match);
// actor_index read directly instead of through its address 7 (same code, kept); diagnostics only: the NULL
// distance through a local 7, direction as one-element storage behind a pointer 7, 0x267a80 defined after
// this function 7 (no change in the ECX order). Retail's other caller 0x25f290 sets ECX last, as ours does.
// Lane 7 r138 (11-09): MATCH, held: 0x267a80's parameters reordered to (point, direction, position, distance,
// unknown) everywhere (retail's second call sets up last-to-first under that order; 0x267a80 still matches).
// Held, not committed: it also moves the arguments on two call lines in 0x25f290 (contributor-finished,
// src/unknown_25fc30.cpp; its bytes unchanged, score 253), which needs that owner's OK.
// P395: Activity default after offset/after query/before direction/before state 11/52/16/46;
// corrected-home offset memset/chained zero/query-only output 71/11/275; activity default at state test 29.
// NOT kept: no exact setup repair; original complete body restored.
// Round 145 (m3 lane 8, fast with 0x267a80, which stays matched): the three calls through a forceinline wrapper that
// supplies the NULL distance, its parameters (point, direction, position, offset) 7; the wrapper taking the distance as
// well, in the orders (point, direction, position, offset, distance), (point, direction, position, distance, offset),
// (point, direction, distance, position, offset) and (direction, distance, point, position, offset) 7 each: all five
// the same code, so the setup follows 0x267a80's own declared order, not an inlined caller's. HELD as before
// (lane 7 r138's reorder matches; BLOCKED-BY the two call lines in the contributor's 0x25f290, which m3_owner.py
// still reports as contributor code).
// Round 82 (m3 lane 1, fast = plain 7): the NULL arguments as typed zero casts 7; a file-level const null pointer for the
// distance 7; 0x267a80's parameters all top-level const 7; 0x267a80 writing through `unknown` with no offset alias local 7
// (0x267a80 still matches); the actor position as the typed field at the two calls and the three ahead sums 7. Same code
// each time: only the held parameter reorder (lane 7 r138) moves the ECX setup.
// @retail 0x267c50
void function_267c50(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	byte *data = (byte *)actor;
	*(real *)(data + 0x3d4) = 0.0f;
	short level = *(short *)(data + 0x328);
	s_prop_threshold_table *table = (s_prop_threshold_table *)g_4e034c;
	if (table && table->count > 0)
	{
		byte *values = (byte *)table->entries;
		if (level >= 12)
		{
			real threshold_a = 0.25f, threshold_b = 0.25f;
			byte *settings = (byte *)function_1e4a10(actor->unknown054);
			if (settings)
			{
				if (*(real *)(settings + 0x60) > 0.0f) threshold_a = *(real *)(settings + 0x60);
				if (*(real *)(settings + 0x5c) > 0.0f) threshold_b = *(real *)(settings + 0x5c);
			}
			if (*(real *)(data + 0x2d8) > threshold_a) *(real *)(data + 0x3d4) = *(real *)(values + 0x2c);
			else if (*(real *)(data + 0x2d8) > 0.0f) *(real *)(data + 0x3d4) = *(real *)(values + 0x28);
			else if (*(real *)(data + 0x2dc) > threshold_b) *(real *)(data + 0x3d4) = *(real *)(values + 0x24);
			else *(real *)(data + 0x3d4) = *(real *)(values + 0x20);
		}
		else if (level >= 11) *(real *)(data + 0x3d4) = *(real *)(values + 0x18);
		else if (level >= 10) *(real *)(data + 0x3d4) = *(real *)(values + 0x10);
		else if (level >= 9) *(real *)(data + 0x3d4) = *(real *)(values + 8);
		else if (level >= 8) *(real *)(data + 0x3d4) = *(real *)values;
		else *(real *)(data + 0x3d4) = 0.0f;
	}
	real decay;
	if (*(real *)(data + 0x3d4) > *(real *)(data + 0x3d8))
		decay = expf(-0.6931471824645996f / round_tick_count((real)g_510c54->field_2_3 * 0.25f));
	else
		decay = expf(-0.6931471824645996f / round_tick_count((real)g_510c54->field_2_3));
	real difference = *(real *)(data + 0x3d4) - *(real *)(data + 0x3d8);
	real blend = 1.0f - decay;
	*(real *)(data + 0x3d8) += difference * blend;
	if (actor->unknown024 != NONE && !team_is_enemy(actor->unknown024, 1) &&
		*(long *)(data + 0x26c) == NONE && !*(bool *)(data + 0x225) && actor->unknown858 == NONE &&
		actor_behavior_find_level(actor_index, 0x38) == NONE && actor_behavior_find_level(actor_index, 0x10) == NONE &&
		actor_behavior_find_level(actor_index, 0x11) == NONE)
	{
		bool moving_toward = false, nearby_player = false;
		bool check_target = function_2684f0(actor) > 6;
		vector3f target_direction;
		if (check_target)
		{
			s_type_f95cd3 *view = function_25d700(*(long *)(data + 0x338));
			if (view) target_direction = *(vector3f *)((byte *)view + 0x2c);
		}
		if (actor->unknown086 > 5)
		{
			s_iterator iterator;
			function_26be00(actor_index, &iterator);
			s_prop_datum *node;
			while ((node = (s_prop_datum *)function_26be30(&iterator)) != NULL)
			{
				s_type_5cfb45 *state = function_25d690(node);
				if (!prop_get(node->prop_index)->unknown23 && !function_25d690(node)->unknown5e &&
					!prop_get(node->prop_index)->unknown22 &&
					(prop_get(node->prop_index)->unknown25 || state->unknown3c == NONE))
				{
					vector3f direction;
					if (function_267840(actor_index, iterator.index, &direction))
					{
						bool active = false;
						vector3f offset = {0.0f, 0.0f, 0.0f};
						short proximity = function_267a80(&state->position, &direction,
							(point3f *)(data + 0x238), NULL, &offset);
						if (proximity >= 1)
						{
							*(bool *)(data + 0x3de) = true;
							if (prop_get(node->prop_index)->unknown25)
							{
								*(bool *)(data + 0x3dc) = true;
								nearby_player = true;
							}
						}
						if (state->unknown63) active = true;
						else
						{
							long player_index = object_get(node->object_index)->player_index;
							s_ai_player *player;
							if (player_index != NONE && (player = ai_player_get(player_index)) != NULL)
								active = function_146650() - player->unknown0c < round_tick_count((real)g_510c54->field_2_3 * 2.5f);
						}
						if (prop_get(node->prop_index)->unknown25 && active)
						{
							real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
							if (distance_squared < 1.0f && *(bool *)(data + 0x5d0))
							{
								vector3f movement = *(vector3f *)(data + 0x5ec);
								if (function_30bf0(&movement) > 0.0f)
								{
									point3f ahead;
									ahead.x = movement.i * 0.4f + *(real *)(data + 0x238);
									ahead.y = movement.j * 0.4f + *(real *)(data + 0x23c);
									ahead.z = movement.k * 0.4f + *(real *)(data + 0x240);
									short projected = function_267a80(&state->position, &direction, &ahead, NULL, 0);
									if ((projected > proximity ? projected : proximity) >= 1)
									{
										real dot = movement.k * offset.k + movement.j * offset.j + movement.i * offset.i;
										if (dot > (distance_squared < 0.25f ? 0.0f : 0.8660253882408142f))
										{
											*(bool *)(data + 0x3dd) = true;
											moving_toward = true;
										}
									}
								}
							}
						}
					}
					if (check_target && function_267a80((point3f *)(data + 0x238), &target_direction, &state->position, NULL, 0) >= 2)
						*(bool *)(data + 0x3df) = true;
				}
			}
		}
		if (moving_toward)
		{
			function_262800(actor_index, *(s_reference *)(data + 0x418), true);
			function_1f86a0(actor_index);
			*(short *)(data + 0x3e2) = (short)round_tick_count((real)g_510c54->field_2_3);
		}
		else if (*(short *)(data + 0x3e2) > 0)
			--*(short *)(data + 0x3e2);
		else
			*(bool *)(data + 0x3dd) = false;
		if (nearby_player || moving_toward)
			*(short *)(data + 0x3e0) = (short)round_tick_count((real)g_510c54->field_2_3 * 1.5f);
		else if (*(short *)(data + 0x3e0) > 0)
			--*(short *)(data + 0x3e0);
		else
		{
			*(bool *)(data + 0x3dc) = false;
			*(bool *)(data + 0x3dd) = false;
			*(bool *)(data + 0x3de) = false;
			*(bool *)(data + 0x3df) = false;
		}
	}
	else
	{
		*(bool *)(data + 0x3de) = false;
		*(bool *)(data + 0x3dc) = false;
		*(bool *)(data + 0x3df) = false;
		*(bool *)(data + 0x3dd) = false;
		*(short *)(data + 0x3e2) = 0;
		*(short *)(data + 0x3e0) = 0;
	}
	if (actor->unknown086 >= 2) actor->unknown084 = 4;
	else actor->unknown084 = actor->unknown086 >= 1 ? 3 : 1;
}

// @retail 0x2675b0
bool function_2675b0(long node_index)
{
	s_prop_node_view *node = prop_node_get(node_index);
	s_type_76cf92 *prop = prop_get(node->unknown08);
	bool result = false;
	if (prop->actor_index != NONE)
		result = function_267550(prop->actor_index);
	return result;
}

/* These wrappers read the handle after resolving the complete record address. */
// @retail 0x267680
bool function_267680(long node_index)
{
	s_prop_node_view volatile *node = prop_node_get(node_index);
	long object_index = node->object_index;
	long type = ((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].type;
	bool result = false;
	if ((1 << type) & 3)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2676d0
bool function_2676d0(long actor_index)
{
	s_actor_view volatile *actor = actor_get(actor_index);
	long object_index = actor->unknown018;
	bool result = false;
	if (object_index != NONE)
		result = function_2675f0(object_index);
	return result;
}

// @retail 0x2672e0
void function_2672e0(s_type_f95cd3 *view, s_prop_node_view const *node)
{
	s_prop_node_view const *const *node_reference = &node;
	if (view->unknown54 > 1.0f)
		view->unknown54 = 1.0f;
	real value = 1.0f;
	if (!view->unknown2a)
	{
		if ((*node_reference)->unknown27 < 2 || view->unknown39 == 4)
			value = 0.0f;
		else if (view->unknown39 == 3)
			value = 0.3f;
		else if (view->unknown39 == 2)
			value = 0.6f;
		else
			value = 0.8f;
	}
	if (!(value >= view->unknown54))
		value = value * (1.0f - 0.995f) + view->unknown54 * 0.995f;
	view->unknown54 = value;
}


PRIVATE inline s_type_f95cd3 *countdown_prop_view(s_prop_datum *node)
{
	s_type_f95cd3 *result = NULL;
	if (node->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
		if (tracking)
			result = &tracking->view;
	}
	return result;
}

// @retail 0x263670
void function_263670(s_prop_datum *node)
{
	short *countdown = (short *)node->unknown1e;
	if (*countdown > 0)
	{
		--*countdown;
		if (*countdown == 0)
			node->unknown1c = NONE;
	}
	s_type_5cfb45 *state = function_25d690(node);
	s_type_f95cd3 *view = countdown_prop_view(node);
	if (state->unknown5e)
		++*(short *)((byte *)state + 0x5c);
	else
		*(short *)((byte *)state + 0x5c) = 0;
	if (view)
	{
		short *age = (short *)((byte *)view + 0x28);
		if (*age != NONE)
		{
			++*age;
			if ((real)*age * g_510c54->rate >= 1.5f)
			{
				view->unknown2a = false;
				*age = NONE;
			}
		}
		short *remaining = (short *)view->unknowna0;
		if (*remaining > 0)
			--*remaining;
		short *duration = (short *)view->unknown08;
		if (node->unknown27 >= 1)
		{
			if (*duration < 0x7fff)
				++*duration;
		}
		else
			*duration = 0;
	}
}


struct s_prop_object_links_view
{
	byte unknown00[0x10a];
	word unknown10a_0 : 2;
	word has_links : 1;
	word unknown10a_3 : 13;
	byte unknown10c[0x120 - 0x10c];
	short size;
	short offset;
};

struct s_prop_object_link
{
	byte unknown00[4];
	union
	{
		word packed_index;
		struct
		{
			word unknown04_0 : 3;
			word link_index : 13;
		};
	};
	byte unknown06[2];
};

/* The optional output pointer stays on the stack until the scan is complete. */
// @retail 0x2651e0
bool function_2651e0(long object_index, short *volatile output_index)
{
	s_prop_object_links_view *object = (s_prop_object_links_view *)((s_object_header_view *)g_4e0300->data)[object_index & 0xffff].object;
	bool result = false;
	short index = NONE;
	if (TEST_FIELD_BIT(object->has_links))
	{
		long count = (dword)(long)object->size >> 3;
		s_prop_object_link *links = (s_prop_object_link *)((byte *)object + object->offset);
		index = 0x7fff;
		for (short i = 0; i < count; i++)
		{
			s_prop_object_link *link = &links[i];
			if ((link->packed_index & 0xfff8) > 0)
			{
				if (link->link_index < (word)index)
				{
					index = link->packed_index >> 3;
					result = true;
				}
				break;
			}
		}
	}
	short *output = output_index;
	if (output)
	{
		if (result)
			*output = index;
		else
			*output = NONE;
	}
	return result;
}


// @retail 0x264260
long function_264260(vector3f const *direction, vector3f const *facing, real distance)
{
	real cosine = 0.0f - (direction->k * facing->k + direction->j * facing->j + direction->i * facing->i);
	real lateral;
	if (cosine <= 0.0f)
		lateral = 3.402823466e+38F;
	else if (cosine >= 1.0f)
		lateral = 0.0f;
	else
		lateral = (real)(sqrt(1.0f - cosine * cosine) * distance);

	if (cosine > 0.9925f || lateral < 0.5f)
		return 0;
	if (cosine > 0.9063f || lateral < 1.5f)
		return 1;
	if (cosine > 0.5f)
		return 2;
	if (cosine > 0.0f)
		return 3;
	return 4;
}


void *actor_get_character_situation_entry(long actor_index);

short const g_44ae58[4][4] =
{
	{0, 0, 1, 3},
	{0, 1, 2, 3},
	{0, 2, 3, 4},
	{0, 3, 4, 4}
};

struct s_prop_rate_definition
{
	byte unknown00[0x24];
	real durations[3];
};

// @retail 0x2683f0
real function_2683f0(long actor_index, long node_index, short type)
{
	real result = 0.0f;
	s_prop_rate_definition *definition = (s_prop_rate_definition *)actor_get_character_situation_entry(actor_index);
	if (definition)
	{
		s_prop_datum *node = prop_ref_get(node_index);
		switch (node->type)
		{
		case 2:
			result = 1.0f;
			break;
		case 3:
			result = 1.0f;
			break;
		case 4:
			result = 1.0f;
			break;
		case 8:
			result = 1.0f;
			break;
		case 1:
			if (function_25d690(node)->unknown3c != NONE)
			{
				result = 1.0f;
				break;
			}
		case 5:
		case 6:
		case 7:
			switch (g_44ae58[(short)function_263d30(actor_index)][type])
			{
			case 0:
				break;
			case 1:
				result = definition->durations[0] > 0.0f ? g_510c54->rate / definition->durations[0] : 1.0f;
				break;
			case 2:
				result = definition->durations[1] > 0.0f ? g_510c54->rate / definition->durations[1] : 1.0f;
				break;
			case 3:
				result = definition->durations[2] > 0.0f ? g_510c54->rate / definition->durations[2] : 1.0f;
				break;
			case 4:
				result = 1.0f;
				break;
			default:
				__assume(0);
			}
			break;
		}
	}
	return result;
}

// score 158 (was 158; 168 before lane 2 r119): The 0x80 frame agrees. The contact head reverses the base and index in its address.
// Earlier stacked line, merged: 158 (was 158; 168 earlier): The 0x80 frame agrees. The contact head reverses the base and index in its address.
// M3 lane 6 r223 (h2m fast with 0x26be00/0x26be30/0x25d690/0x25d740/0x25d810, all MATCH; control 158): 0x26be00 is now
// this machine's and its file is open, so its lookup was respelled there: `(index & 0xffff) * size + data` 158, the
// multiply by a signed 0x888 158 (both the same code here and 0x26be00 still matches), `(word)index * size` 697 (0x26be00
// 7); diagnostic, restored at once: the actor lookup here written with its operands swapped 158 (same code). The head's
// `[eax + ecx + 0x58]` follows neither spelling (retail's own 0x26be00 body and 0x263210 use `[offset + data]` too).
// The positive arm with the NONE case first 164 (the NONE arm then precedes the slot test; retail keeps it after the
// arm's closing jump and cross-jumps it back to the count store); the count read into a `short` used for the test and
// the decrement 158 (same code). Reading: the velocity dot after the velocity normalize folds the direction in both
// builds; only after the relative normalize does retail load it into XMM1-XMM3, which earlier diagnostics reproduced
// only with the direction escaped and that call opaque, so the lead is 0x30bf0's whole-program summary (BLOCKED-BY 0x30bf0).
// Earlier stacked line, merged: 158 (was 158; 168 before lane 2 r119): The 0x80 frame agrees. The contact head reverses the base and index in its address.
// MiniMax I (11-19): no new form, no lower score (168): the first difference is still the inlined 0x26be00's head load, whose
// base and index are chosen inside that callee's own source in a file this lane may not edit; the object table's pointer still
// takes ECX where retail keeps EAX, and the player search further down still keeps its counters in other registers.
// MiniMax H (11-18): no new form, no lower score (168): the 0x80 frame agrees, but the first iterator load still
// encodes its address with the base and the index the other way round, the object table's pointer still takes ECX where
// retail keeps EAX, and the player search further down still reads another global and keeps its counters in other
// registers.
// Earlier measurement 158 (was 158): the 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding;
// Earlier measurement 158 (was 158; 168 before lane 2 r119): the 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding;
// Earlier measurement 158 (was 168): the 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding;
// the players iterator loads its table into ECX (retail EAX) and the direction is not held in XMM1..XMM3 across the two dots.
// Earlier first lines: 158 (was 168); 168 (was 168): r200 unchanged.
// MiniMax A (11-20): no new form, no lower score (168): the 0x80 frame agrees and the first difference is still the contact head's reversed base and index in the address encoding (ours `[eax + ecx + 0x58]`, retail `[ecx + eax + 0x58]`), which comes from the inlined 0x26be00's `object->value58` read of the clump table - that helper lives in src/unknown_26bda0.cpp, so the roles cannot be changed from this file; the rest of the gap is the players iterator's table pointer, which retail loads into EAX and stores at once (a five-byte load against this build's six-byte one into ECX, which moves the loop pad and every row after it), and the per-case scratch rotations; the two NONE stores in the other order measure 170 and the two player searches merged into one 829, so the two separate searches and the store order are retail's.
// Earlier at 168 (was 168): r200 unchanged. The 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding.
// Earlier first line (merged into the note above): 168 (was 168): r200 unchanged. The 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding.
// Earlier stacked line, merged here: 168 (was 168): r200 unchanged. The 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding.
// Earlier measurement 168 (was 168): r200 unchanged. The 0x80 frame agrees. The contact head still has the base and index reversed in its address encoding.
// M3 lane 8 r189 (fast with 0x26be00/0x26be30/0x25d690/0x25d740, all MATCH; control 168): the first difference is the
// inlined 0x26be00, whose head load retail addresses as [table + slot offset + 0x58] and ours as [offset + table + 0x58]
// (the slot offset is shared with the actor lookup). Forms: the byte alias `data` dropped for `(byte *)actor` 168; the
// alias taken from actor_get with the actor cast from it 168; the clear through `(byte *)actor + 0x324` 168; the level
// and strongest declared after the clear 171; the iterator declared before the actor lookup 168; the actor lookup after
// the other locals' initialisers 180 (first difference +0x7).
// One decoded actor slot for both lookups, long/word with the iterator helper 168/695 and with the typed actor accessor 168/695.
// Named clear destination, with the typed head read, and with typed summary fields 168/168/168. Chained selections 170.
// Earlier 168 (was 168): retail's 2880-byte extent. The first iterator load reverses the address operands.
// The players iterator's table pointer loads into ECX where retail
// uses EAX and stores it at once (one extra pad line after it); the search shape; the facing and relative dots keep the
// direction in memory where retail holds j, k, i in xmm2, xmm1, xmm3 across both dots after the last normalize; the scoring
// block's established/visible layout. Round 64, fast, masked aligned in brackets (start 194 [66]): the block entered on
// `visible || established` with one shared score test 171 [66], kept; the zeros before the clear 168 [64], kept; the prop
// iterator through 0x26be00/0x26be30 and the view through 0x25d740, same code, kept. Not kept: rejection flag + zeros +
// player NULL first + long second counter 377 [24] at 2882 bytes (the flag is match-only; every structural block is then
// retail's and the score stays up only through the iterator's extra pad line); same without the flag, score test in both
// arms 391 [32], with `!(distance > 3.5f)` 386 [29]; established-first block 320 [52]; `visible || established` after the
// facing block 383 [45]; `established || (visible && ...)` 258-336 [63-72]; slot store in both positive arms 390 [24]; arms
// swapped 377 [25]; vehicle index or definition pointer named 377 [24]; long vehicle type 383; reset stores reordered or
// chained 377-379; iterator stores reordered 378/379; pool local 377; a NULL datum store 236 at 2888 bytes (extra store, not
// retail's). Diagnostics, reverted: a dead read beside the vehicle lookup puts the table pointer in EAX at 2880 bytes (247
// [43]); two beside the iterator also do; one gives EDX: the iterator's register is a rotation step, not its statements.
// MiniMax S (11-10): no new form, no lower score (168); the players iterator's table pointer still loads into ECX where retail
// uses EAX, and the entry loop's index compare and the iterator's setup still differ.
// Earlier first line: 194, was 194: frame 0x80 and the forward/relative blocks now agree; the established and
// visible flags take each other's stack bytes, the facing/scoring block keeps the direction in memory
// where retail keeps it in xmm1-3, and the entry's second actor lookup indexes [eax+ecx] for [ecx+eax].
// Earlier first line: 502, was 502: Frame 0x84/0x80 and selection/iterator/scoring homes remain.
// Earlier status 502, was 502: frame 0x84 versus 0x80, selection/iterator and transform/scoring homes differ.
// the prop walk now uses the file's prop iterator (retail's unrotated walk with
// the cursor at 0x3c and the current index at 0x38) and reads the actor index from its slot; the
// frame is 0x84 against 0x80 since the visible flag became a stack byte, and the result/level slots,
// the forward transform (retail if/else straight from the unit) and the scoring block differ.
// Round 152: iterator 669 fast; NONE results before the actor lookup 666 fast, right after it 656
// fast (673 plain); short best level 604; matching-object arm as a branch 584; forward if/else read
// from the unit 775 (frame 0x74); retain set before the distance test and visible defaulted true
// and cleared 564 (kept); visible alone 572.
// Earlier: Frame 0x7c/0x80, sentinel/link homes and conversion expansion remain.
// Earlier 684 (was 684): frame 0x7c versus 0x80; sentinel homes, loop header and tick calls differ.
// frame 0x7c versus 0x80; saved links and conversion expansion remain.
// frame 0x7c versus 0x80; sentinel homes, loop header and tick calls differ.
// Frame 0x7c versus 0x80; sentinel/link homes and expanded tick conversions
// remain. Pointer player-table walk 685; forward library copy 684; velocity library copy 684;
// parallel matrix pair sums 684; native actor fields 684 (calibrated fast).
// Earlier: frame 0x7c versus 0x80; sentinel homes, loop header and tick calls differ.
// frame 0x7c versus 0x80; saved link/predicate homes and expanded conversion calls remain.
// Conditional facing 731; wide best level 701; direct parameter 701; shared difference 705;
// wide retention 704; live relative inputs 695; native velocity copy 695; early defaults 755;
// phase defaults 752.
// Round 156: depth zero 937; depth one 695; shared player lookup 702;
// retained actor lookup 723; separate missing-view guard 696 (plain scores).
// word threshold 712; byte conditions 712; both 712; geometry names 712 (reduced);
// direct transformed output 700 reduced, 715 plain; entry cursor advance 708 reduced, 726 plain.
// j93: Frame 0x7c/0x80, saved-link homes and conversion expansion remain; helper declaration restored.
// shared tracking view 695; shared player lookup 724; very close result 719; native player search indices 702;
// notification counter view 695; level helper inline 717.
// Frame, saved-link homes and expanded tick conversions remain.
// selection names 695; cached state test 707; post-clear results 684;
// direct score result 695; direct retention result 705 (plain).
// Frame 0x7c/0x80, saved links and expanded conversions remain. Single player search 706; point phase 684; relative initializer 684; selection scope 684; long vehicle kind 684 (plain; ordinary actor reads in all).
// Frame/saved links and conversion expansion remain. Native iterator 702; live elapsed field 684; long contact result 694 (frame 0x80); direction library copy 684; shared reset 699; native best level 707 (plain).
// Word level with long established/plus long player slot/long visible/long retain/
// long established and visible 696/724/706/706/699; recovered-frame early selection
// defaults 684, early successor cursor 712, scalar-first strongest comparison 696 (calibrated fast).
// Matrix component loop 721; direction component loop 711; relative component loop 696; traversal-scope contact handle 702; traversal-scope tick threshold 702 (fast control 702); relative loop 714 plain, rejected.
// Earlier 684 (was 695): Frame, saved-link homes and expanded tick conversions remain.
// Earlier 684 (was 684): Frame 0x7c/0x80, saved links and expanded conversions remain. Single player search 706; point phase 684; relative initializer 684; selection scope 684; long vehicle kind 684 (plain; ordinary actor reads in all).
// Earlier 684 (was 684): Frame/saved links and conversion expansion remain. Native iterator 702; live elapsed field 684; long contact result 694 (frame 0x80); direction library copy 684; shared reset 699; native best level 707 (plain).
// Round 112: Long visible 683 (one-point gain dropped); long positive 710; long close 727; long
// very-close 699; combined state/view guard 705; ordinary actor input in all.
// Independent pre-clear best/strongest/both/strength/best-strength/strongest-strength/all-three/level results 695/685/685/684/695/685/685/695 (calibrated fast).
// Original result lifetimes restored; frame 0x7c/0x80, saved links and expanded conversions remain.
// Round 150: wide maximum 739; long queried level 684; both 739; sentinel declaration order 684; shared relative difference 692; nested maximum guards 684; direct proximity result 711; wide maximum with sentinel order 739 (calibrated fast).
// Round 155: native field references (alias-free), then notification fields at uses: selection 684; elapsed 684; selection elapsed 684; grace 684; selection grace 684; elapsed grace 684; selection elapsed grace 684; notification fields at uses 682. All restored.
// Native cursor/selection/all datum handles 684/684/684; existing prop receiver 707;
// single strength observation 685; all handles with prop/strength/both 707/685/732.
// Original complete body and all inherited input/helper declarations restored.
// Round 212: Common notice dispatch with short/word/long/dword selected event 734 each;
// player admission shared before selection 711 each. Frame 0x7c remains; original calls restored.
// Round 376: Native transform pointer/value 684/774; all nine row products with
// raw/pointer/value receivers 684/684/774; direct output with native pointer/value/value-and-
// products 717/724/724. Calibrated fast; original body restored.
// Round 140: Sequential absolute/relative velocity workspace with parent observation/shared player
// ordinal/native established interactions 701/701/701/701/683/683/683/683; native visible/positive/retain
// combinations and full-width player slot on that graph 682/704/682/707/685/707/710/706. Best 682 gains only
// two, frame 0x74 versus 0x80; all original vectors, observations, result widths, ordinal lifetimes and input
// access restored.
// Round 354: shared dot at absolute/relative/facing/absolute-relative/absolute-facing/relative-facing/all
// scoring sites 684 each; all with native signed-word maximum 707. Original complete body restored.
// Native-word established/visible, positive, both, retain, pair-retain, positive-retain,
// all scoring, all six predicates 699/711/728/684/703/715/732/751 (calibrated fast).
// Word established/visible separately 694/685; pair with short best level/early
// sentinels 700/700. Established word recovers frame 0x80, but all gains fail; restored.
// Round 166: late visibility/derived retention/both/later established 577/572/567/564;
// visibility+established/retention+established/all/all with long visibility 577/572/563/560.
// Combined predicates recover frame 0x80, first +0x3b. On that graph: reversed results 559,
// wide level 647, live iterator index 560, reversed+live 559, reversed+wide 646,
// wide+live 647, all 646, all with early constants 655. Early constants alone/reversed
// 568/568. Best five-point gain is below the gate; original body and parameter access restored.
// Round 228: Independent first-search table ordinal, NONE/bounded induction, long/short
// result and conditional/guarded pointer selection 552/549/552/548/536/538/555/551.
// Guarded long ordinal with ordinary actor input also 536, plain confirmed at retail
// 2880 bytes; retained without the parameter-address alias. First frame remains 0x84/0x80,
// selection/iterator homes, forward transform and scoring allocation still differ.
// DH round 230: On the independent-ordinal graph, long established/visible/retain/pair-established-visible/
// pair-established-retain/pair-visible-retain/all predicates 577/579/550/583/567/579/603;
// all with reversed selection declarations 604. Restored; frame 0x84/0x80 and native result homes remain.
// Round 399: independent-search graph, word first/second/both counters 533/530/530;
// long second result 577; word first result with long second 575, with word second
// counter 528 (kept); word counters/first result with long/byte second result 580/639.
// Calibrated preview; first result stays in -1..1 and each bounded counter in 0..2.
// Frame remains 0x84/0x80; selection/iterator, forward and scoring homes differ.
// DH round 232: On the retained ordinal graph, unsigned ordinal, native word threshold,
// both, per-use iterator index, index/ordinal, index/threshold, all, and all with the first-pass
// workspace scoped each score 536 in the calibrated image. Frame 0x84/0x80 and selection/
// iterator/transform/scoring homes remain; complete original executable body restored.
// DH round 239: Contact/player iterator and saved contact-state name subsets each
// 528; all three with reversed independent selection declarations 529. Calibrated
// image; original iterator/state names and complete body restored, frame still 0x84/0x80.
// DH round 252: Independent repeated player handle/datum observations and direct current-contact result,
// all seven subsets, then all with completed selection scope
// 528/528/528/528/528/528/528/528. Original complete executable body restored.
// Existing transform/relative/both complete setters 528/528/528; safe in-place transform
// 705 (frame 0x74), with relative/point/both 705/712/708; native transform initializer with
// relative and point producers 528. All vector producers restored; frame and saved homes remain.
// Round 426: complete transform rows/scoring dots/both accumulated 529/528/529; shared
// final positive-score producer alone/with rows/with dots/with both 502/503/502/503;
// all with native byte positive 503. Keep only the shared positive producer, plain
// confirmed at 502, 2856/2880 bytes; original products, grouping, iterators and calls remain.
// Round 430: Complete visibility result with original/complete close/complete very-close/both
// proximity producers: bool 560/636/512/628; byte 560/636/512/628. Complete close recovers frame 0x80
// but regresses; original body restored.
// Existing transform/relative/both complete setters 528/528/528; safe in-place transform
// 705 (frame 0x74), with relative/point/both 705/712/708; native transform initializer with
// relative and point producers 528. All vector producers restored; frame and saved homes remain.
// Round 240: complementary established/visibility/both/retention/established-retention/visibility-retention/
// all decisions 502/560/562/505/505/560/562; byte complementary direct decisions 562.
// Original independent searches, shared positive producer and all body values restored; frame 0x84/0x80 remains.
// Round 455: Late retention constant 558; with early scoring zeros 573; with late visibility constant 565;
// both 569; complete predicate role names on each graph 558/573/565/569. Late visibility recovers
// frame 0x80, but all regress. Original observations, calls, defaults and body restored.
// Round 38 (m3 lane 11, plain): forward copied or transformed straight from the unit 745 (2807 B);
// long second search counter 565; facing/score zeros at the unit block top 575; existing
// object_get_forward 713 (frame 0x74, forward block exact); with score zeroed at the iteration top and
// approach/facing at the unit block top 436 (frame 0x80); then strongest_node assigned before strongest
// 278; strongest_node declared before best_node 434, both 276; score before the threshold 270 (kept);
// with the long second counter 271, and player NULL before the first search 396; relative velocity
// through vector3d_from_points3d 262 (kept, relative block exact); visible or retain set first
// 500/497; C-style flag declarations in either order 262/262; NONE reset first in the positive branch 262.
// On the 262 body: best_level (and strongest) declared before the memset 259/259 (retail's immediate
// zero store and loop entry; a 3-point gain, not kept); with a shared iterator-start helper for the
// file's three prop walks 259 (the walks still match); with the long second counter 260; visibility
// as one expression or an if/else 450/450 (2841 B).
// Round 45 (lane 11; fast image 199 at 2880 bytes for this body, 0x25d810 builds differently there): the
// established/visible flags renamed so their names sort the other way 199/199/199 (identical code: the
// byte slots do not follow names here); the slot search with a long counter (retail's pointer-walking
// loop, as the first search) 199 fast, 263 plain (2848 bytes); the player cleared before the first search
// with it 375, alone 384 (the not-found exit then threads past the index test, which retail keeps). The
// existing ai_player_get/ai_player_index_get calls: ai_player_get stays out of line (its file is /Ob1;
// retail inlines it here) 867, with a forceinline declaration 877; ai_player_index_get for both searches
// (the second search is then CSE'd away) 822/797/818 for a long index with the player cleared first, a
// short index, and a short index with the player cleared first.
// Round 36 (lane 13; plain checks, and a one-file LTCG link of this file with its callee closure and the
// ballast, equal to the full build; --fast is wrong here, its reduced image builds 0x25d810 differently):
// the view through the existing 0x25d740 262 (same code); the walk through the existing 0x26be00/0x26be30
// 262 (same code); the player through the existing ai_player_get/ai_player_index_get 2616 B (ai.cpp is /Ob1:
// they stay calls, where retail inlines 0x1c8180 here); the slot search alone through ai_player_index_get
// 2659 B. Retail's search is ai_player_get's shape: player NULL first, a long copy of the short index for
// its NONE test, a long second counter; written so 408 (2850 B, the whole search then equals retail's, the
// rest shifts one instruction). `if (established || !(far && slow)) { if (established) {...} positive = score
// > 0; }` 262 (masked aligned 75 -> 60), with that search 407 (masked 51), with best_level/strongest
// declared before the memset too (immediate zero store as retail) 364 in the link harness; the same with
// `visible &&` kept in the outer test 281 (695/695 instructions); no `visible &&` in the old expression 269.
// Left: retail keeps direction i/j/k in xmm3/xmm2/xmm1 from the relative dot to the facing dot (ours folds
// each read; dot3f spellings and reordered terms compile the same), established/visible slot order (follows
// retail's extra established re-test on the hidden path) and the players loop's alignment jump.
// Round 33 (lane 14, plain; --fast builds this function differently, 2880 B and 199, with a callee caveat):
// the second player search with a long counter (retail's `cmp eax, 2` pointer loop) 263 (2848 B, aligned
// 116 -> 114); that plus the player pointer set to NULL before the first search 403; plus best_level and
// strongest declared before the memset 260 (aligned 112); plus the direction through vector3d_from_points3d
// 266; plus established/visible renamed 260 (same code: spill slots). The length gap is the scoring block:
// retail loads direction j/k before the relative test and i in both arms, keeps them for the facing dot,
// stores visible = false straight to its byte and threads the established test into the not-visible path.
// Round 30 (lane 2, plain): sequence-aligned counts (the check scores positionally) are 116 for this body.
// The search in ai_player_get's shape (player NULL first, long copy of the short index for its NONE test, long
// second counter) 408 (aligned 112); with best_level/strongest declared before the memset 405 (aligned 110:
// everything up to the players loop then equals retail; the only shift is the iterator's table pointer, loaded
// into ECX (6-byte load) where retail uses EAX (5 bytes) and reuses it for NONE, so our loop-top pad becomes
// jmp + lea ecx,[ecx] for retail's single lea ebx,[ebx] and every later instruction is one off). On that form
// the iterator's NONE fields chained 405, assigned before the table pointer 407 (the load stays in ECX), the
// iterator declared at the function's top 405. The same three iterator statements give retail's EAX form in
// the matched 0x138fa0 and 0x192b20, so the ECX choice comes from this loop's register colouring (ours uses
// ECX/EDX where retail uses EAX/ECX through the whole players loop).
// Round 74 (m3 lane 12; --fast listed with 0x25d810 equals plain): `score > 0` first in the positive expression 260;
// `if (established || ...) positive = score > 0.0f;` 258; `far_and_slow` named 251 (2880 bytes); positive
// assigned at the end of the established block with the rest as its else 194 (kept, plain; the difflib-aligned
// count rises 112 -> 142, the gain is positional); on that: the else as `else if (...) positive = ...` 257, with
// `far_and_slow` named 249, `else if (visible)` 194, score tested before the distance pair 195, the pair negated
// into `<=`/`>=` 257, best_level/strongest before the memset 191, a long second search counter 194, both 191.
// Compile-only (no LTCG) keeps direction i/j/k in xmm3/xmm2/xmm1 across the relative and facing dots as retail
// (visible in BL there); our LTCG build keeps visible in AL and folds every direction read. Calling 0x30bf0
// through a function pointer for the relative vector (diagnostic) did not change that (487).
// Round 348: native player/contact/both iterator aggregate initialization
// 394/194/394; with early constant results 391/191/391; both with native second
// search counter 402, plus early results 399. The 191 form gains only three and
// is restored; byte homes, direction preloads and entry operands remain.
// Round 295: all-axis normalized direction scalars before contact or velocity normalization,
// mutable/const and forward/reverse declaration orders, each 194. Original consumers restored.
// M297: Complete product terms named uniformly in all three scoring dots, six
// declaration orders kji/kij/jki/jik/ikj/ijk, then const kji/ijk [194, 194, 194, 194, 194, 194, 194, 194]. Original expressions restored.
// DH round 279: Player-visit native forward/point/direction scratch, seven declaration subsets then
// complete role names [194, 194, 194, 194, 194, 194, 194, 194]. Original executable body restored.
// Round 61 (lane 1, fast): second search with a long counter 194; player NULL first 399; with the index through a long local
// 404 (search equals retail; aligned 99); `if (distance < 15 && (visible || established))` with no visible test in the
// non-established arm 381 at retail's 2880 bytes (visible stays in memory and the test threads, aligned 97), the same
// without the search form 242; declarations before the memset 191 (a gain of 3, not kept), with the rest 378-239. Named
// direction scalars after the last normalize 381 (same code). Diagnostics, reverted: direction's address stored to a
// global 497; all three normalizes through a function pointer with the vectors' addresses stored 555; only the relative
// normalize so, address not stored/stored 509/529: i/j/k load into registers after that call only with both, so retail's
// callers treat the normalize as killing an escaped direction. /Ow on the file 194 (same code; one flag diagnostic).
// Round 520: conditional approach maximum/complete absolute producer/native established
// byte subsets, then all with native visibility: 194/371/371/194/194/371/371/371.
// Maximum selection is neutral; complete absolute production regresses. Body restored.
// AC481: consume the actual point-provider result through the existing difference
// helper or a thrice-used position pointer; original/copied relative input and
// pure pre-clear result defaults, masks 0..7: 521/521/449/446/518/518/446/443.
// Complete original body restored; entry operands, byte homes and direction residence remain.
// DH297: Late score/facing/approach zero lifetimes, subsets 1-7, then all with
// native byte positive: 745/715/749/365/688/704/749/749. Score/facing release shrinks
// the frame below 0x80; original early zeros, predicates, observations and whole body restored.
// Round 535: independent first/second/both real-table pointer walks, then native long
// first/second/both ordinal combinations: 387/369/433/389/372/433/456/456. Original scans and body
// restored; entry operand order, established/visible bytes and direction residence remain.
// Round 537: acceptance-result false lifetime at player/admitted/geometry/distance
// boundaries, bool 194/194/194/194; byte 194/194/194/194. Original complete body restored;
// established/visible homes and normalized direction residence remain.
// Round 54 (m3 lane 14, fast listed with 0x25d810 = plain 194): `if (established || (visible && !(far &&
// slow))) { if (established) {...} positive = score > 0; }` (retail's one shared score test) 249 (2880 B;
// ours tests established before visible, retail threads the visible test into both arms); the positive arm
// with the NONE-slot case first 200; strongest declared before best_level 194; one positive expression after
// the established block 262; facing dot as forward*direction 194. Original body restored.
// N340: Whole actor/vehicle admission regions, original/failure-first/success-first
// trees excluding the original pair: 194/194/194/194/194/194/194/194. All restored;
// lookup operands, established/visible homes and direction residence remain.
// P313: Existing shared dots at absolute/relative/facing consumers on the recovered-frame graph,
// seven subsets, then all with the point-difference helper: 194/194/194/194/194/194/194/246. NOT kept:
// no exact arithmetic-association result. Original body and shared helper definitions restored.
// Round 208 (m3 lane 3, fast with 0x25d810): strongest's zero after the prop-iterator start 194, best_level's 195,
// both 195; the iterator start from the first actor lookup 729 (2843 bytes); `(short)p` in the first search 194;
// the players iterator declared at the top 194.
// J251: On frame 0x80, real direction/velocity/all-dot array traversals 636/194/298;
// direction+velocity, direction+dots, velocity+dots and all 636/643/298/643;
// all with descending input axes 743. NOT kept: loops retain runtime traversal or
// worsen homes/length; copy alone canonicalizes. Original body restored at 194;
// first +0x5c, direction SIMD residence and established/visible byte homes remain.
// DH343: Existing native component-array identity throughout direction/velocity/relative/forward,
// direction+velocity/direction+relative/direction+forward/all: 194/194/194/194/194/194/194/194.
// Original body restored; entry operand order, Boolean homes and direction SIMD residence remain.
// N356: Signed-octet established/visible/retain storage, seven subsets and complete plain-char family:
// 194/194/194/194/194/194/194/194. NOT kept: unmatched storage/result-lifetime forms.
// Original executable source restored; Entry index operands, exchanged Boolean homes and direction SIMD residence remain.
// P331: Post-search player/unit, slot-conflict and unseated-unit admission results, masks
// 1/2/4/3/5/6/7 then complementary all: 194/790/194/194/194/790/194/194. NOT kept: no
// equality/gain; original body restored.
// DH351: Explicit distance/absolute/relative normalization outcomes, all seven
// subsets, then both speed-result locals: 194 each. NOT kept: no gain; original
// tests and full body restored; entry operands, Boolean homes and direction SIMD residence remain.
// DH353: Slot-owned established/sentinel-owned visibility/complete retention arms,
// subsets 1/2/4/3/5/6/7: 195/710/710/710/701/718/707; failure-first full graph 707.
// NOT kept: no gain; original decisions, frame, observations and query calls restored.
// DH356: Complete facing/distance/speed score-contribution producers, seven subsets,
// then all zero lifetimes at the score region: 687/283/261/744/736/281/738/731.
// NOT kept: one-use result forms regress; original full body restored; query remains exact.
// Round 65 (m3 lane 14, fast, masked in brackets; base 168 [61]): the two searches are retail's existing helpers (same bytes in 0x1c8180):
// `ai_player_get` + `ai_player_index_get`, as matched 0x1bbf40 writes them: 849 (2642 bytes, both calls, the forward getter drops out of
// line too); with `inline` on 0x1c8180's definition in src/ai.cpp 795 (2682; its callers all still match); plus a `__forceinline`
// declaration of object_get_forward here 383 [52] at 2880, every search line retail's. Not kept (score rises). On that: scoring block with
// a `rejected` flag (`if (visible) {...; if (!established && far && slow) rejected = true;} else if (!established) rejected = true; if
// (!rejected) {...}`) 305 [27] at 2886, retail's block order; score test in both established arms 378 [54]; flag in the else only 383 [52];
// second search alone as the helper 769 (2680). Left on 305: the iterator's table pointer in ECX for EAX, one byte longer, so the head's
// 6-byte pad becomes `jmp` + 3 bytes and later lines sit one off; direction held in XMM1-3 across the dots. Same code on it: iterator set
// through an inline helper 383, stores reordered or chained 385, vehicle definition named 383, eligibility bool, inline eligibility
// helper, inline type getter, type read at each test, loop nested in the test 305 each. Next: that one scratch step, then this recipe.
// F404: Complete normalized-direction value at distance/query/absolute-speed/relative-speed
// boundaries, mutable then const: 429/429/429/439/429/429/429/439. NOT kept: no exact value-ownership form.
// Original normalizers, vectors, predicates and complete body restored; iterator/byte homes and SIMD residence remain.
// MiniMax B (11-06) round 2: no new form, no lower score (168); the first difference is still the
// table load at +0x5c, ours [eax + ecx + 0x58] against retail [ecx + eax + 0x58], and that load
// is the inlined 0x26be00, whose source sits in a file this lane may not edit, so the register
// roles cannot be swapped from here; the frame is retail's 0x80 and the normalized direction is
// still folded instead of being kept in XMM1..XMM3; the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (168); the first difference is still the
// table-pointer load at +0x5c, where ours addresses it as [eax+ecx+0x58] and retail as
// [ecx+eax+0x58], the frame is still 0x7c against retail's 0x80, the iterator's table pointer
// still loads into ECX where retail uses EAX (which makes our loop head one byte longer and
// puts every later line one off), and the normalized direction is still folded instead of
// being kept in XMM1..XMM3 across the three dots; the body is unchanged.
// T63: Existing iterator start/advance/state-query inline contracts, seven subsets
// then all with target inline metadata: eight 168; exact query stays matched.
// NOT kept: no equality or gain; original body and all declaration metadata restored.
// P365: Three direction scalars captured after the absolute dot and before relative normalization,
// six declaration orders plus two immutable orders: eight 168. NOT kept: no native SIMD
// producer equality or gain; complete original body and helpers restored.
// P367: Independent live approach/facing/acceptance scalar-home identities, seven
// subsets and complete threshold identity: eight 168s. NOT kept: no native SIMD
// or player-iterator home equality; complete original body and names restored.
// P370: Uniform k/j-pair, i-term and complete accumulation boundaries in all three dots,
// masks 1..7 and immutable complete parts: 168/174/168/168/168/168/168/168. NOT kept:
// no SIMD/iterator recovery or byte match; original expressions and complete body restored.
// P373: Actual facing-excess/closeness/retention-span arithmetic parts, seven subsets,
// complete immutable parts: eight 168s, 2880/2880 bytes, first +0x5c. NOT kept:
// no SIMD/iterator/policy equality; original complete body and helpers restored.
// P377: Actual unit-velocity/actor-position/view-strength field receivers, seven
// subsets then complete immutable pointers: 168/168/168/168/168/168/168/168. NOT kept: no native iterator/pool/SIMD
// equality; original complete body, field observations and helpers restored.
// P380: First-walk result as native node/typed datum, walk-only or bare entry lifetime, while-return or
// for-cursor advancement on the retained shared-iterator and early-zero graph.
// Scores 168/753/168/753/168/753/168/753. NOT kept: while forms tie 168/2880; for forms regress to
// 753/2923, versus native 2880; early index, iterator and SIMD homes stay different.
// Original complete body restored.
// Round 138 (m3 lane 7, fast with 0x25d810, every 0x26be00 and ai_player_get user listed): 0x26be00's
// file is open to machine 3 (its owner prints ours). Retail's inlined iterator start puts the reloaded
// table base first (`[ecx + eax + 0x58]`, as in 0x1e23c0) and 0x25d770 the offset first, both matched here,
// so the order follows the context: 0x26be00's sum written offset-first 168 (same code, its eight matched
// inliners unchanged); the memset straight after the actor lookup 186 (first +0x31); the other actor as
// `other = NULL; if (...) other = actor_get(...)` 382. Round 65's recipe re-measured: both searches through
// ai_player_get/ai_player_index_get + `inline` on 0x1c8180 + a __forceinline object_get_forward declaration
// 383 at 2880 (all 16 users of those helpers still match; every search line retail's, one line late behind
// the iterator's table pointer in ECX, a 1-byte longer load, so the loop-top pad is `jmp` + `lea ecx, [ecx]`
// for retail's `lea ebx, [ebx]`); the same with a __forceinline ai_player_get declaration here in place of
// the ai.cpp `inline` 643 (2839 B, the call stays). All restored.
// Round 145 (m3 lane 8, fast; masked aligned in brackets, base 168 [64]): the game time through the existing 0x146650
// getter 168 [64] (same code); the two constant tick conversions as calls of the existing 0x1469f0 699 at 2826 bytes
// (it stays out of line; its body is inline assembly and its own file declares it noinline, which of the two keeps it out was not isolated); the players iterator as an aggregate initialiser 380 [62] at 2883
// bytes (a zero store for the datum, table pointer still ECX); its table pointer stored last 170 [64]; the table pointer
// through a forceinline getter with a result local 168 [64] (same code); the table pointer read into a local ahead of
// the vehicle lookup 396 [69]. Retail's two searches are the bodies of 0x1c8180 and 0x1c8150 (long counter, the slot
// stored as a dword), as lane 14 r65's recipe has them; that recipe still waits on the iterator's register. After the
// last normalize retail loads direction.j and .k into XMM2/XMM1 ahead of the branch and reuses them for the relative
// and the facing dot; ours reads the direction from memory at each product.
// Round 82 (m3 lane 1, fast with 0x25d810 = plain 168): 0x26be00's lookup written as an indexed element of the table
// 168 (same code, 0x26be00 and its users unchanged); the scoring test as a `consider` flag set in the visible arm
// (`established || !(far && slow)`) and from `established` otherwise, with one score test under it 239 (2886 B); a short
// threshold 168 (same code); threshold declared before score 174; the unit index assigned inside the player test and
// the slot stored without its cast 168 (same code). Diagnostics on the direction residence, reverted: an inline copy
// of the normalize body defined in this file 168 (same code); `direction`'s address stored to a scratch global with
// the relative normalize called through a global function pointer 545 (2923 B): the three direction loads then sit
// ahead of the test and both dots multiply by registers, close to retail (retail sinks the i load into both arms);
// 0x30bf0's magnitude through a one-line inline helper 168 (same code, 0x30bf0 still matches); 0x30bf0 reading its
// parameter through its address 513 (0x30bf0 21, its convention changes). Retail's shape needs the normalize call to
// be able to write `direction`; no form of 0x30bf0 that keeps its bytes makes our link think so.
// M3 lane 10 r90 (fast with 0x25d810/0x26be00/0x25d740, all MATCH): no gain. The iterator's datum index stored before its index 170; visible
// as one direct bool (`reference == NONE || ... >= 1`) 428 (frame changes); the byte alias of the actor dropped 168 (same code). Re-measured:
// player NULL before the first search 373; the elapsed counter read at each use 168; the NONE fields chained 170. Every other natural form
// screened this round (prop/state getters, threshold and decrease arithmetic, the close flags) is already listed above.
// Round 86 (m3 lane 1, fast with 0x25d810; aligned/masked in brackets, base 168 [80/38]): the first scratch-rotation
// difference is the other actor's lookup (retail reuses EAX for the object pointer, ours takes ECX, and the actor lookup
// after it is one step on). LEAD: the object read into a pointer local and its actor index read at each use
// (`s_slot_object_view *object = object_get(node->object_index); ... object->actor_index == NONE ? NULL :
// actor_get(object->actor_index)`) 163 [75/38], that lookup then exactly retail's (a gain of 5, under the bar). On it:
// lane 14 r65's search recipe (ai_player_get/ai_player_index_get, `inline` on 0x1c8180, a __forceinline object_get_forward
// declaration) 378 [74/36], all helper users matched: the search lines are retail's, the registers on both sides of them
// one rotation step on, as at the iterator, so retail makes one scratch allocation more than ours before the iterator;
// on that, 0x1c8150 with an early return 367 (0x1c8150 and five callers then differ; reverted at once), the vehicle index
// read once into a local 204 [79/40] (ours keeps it in ESI where retail reloads it; the loop-head pad realigns, not a
// real gain), the vehicle object fetched again after the type test 378 (same code); on 163 the vehicle index local 425.
// Round 78 (m3 lane 14, fast unless marked; masked aligned in brackets, base 168 [59]): the other actor's object as a
// named pointer used for both reads (`other_object = object_get(node->object_index)`) 163 [59], plain 163 (aligned 80
// -> 75: that block's EAX/ECX/EDX homes become retail's; a gain of 5, not kept); the same lookup written out at both
// uses with no index local 168; the existing 0x1df560 called in place of team_is_enemy 679 (2797 bytes, stays a
// call); the player pointer as a ternary 372. On 163: score declared at the loop top 374 (2884 bytes); a long player
// slot 385 (2899, a re-measure of the listed long player slot); a short first-search counter 171; the vehicle read
// through object_get() at both uses 622 (2902); the player pointer declared at the loop top 368; the index named again
// beside the object pointer 168; re-measure of round 33's long second counter 164. Diagnostics, each in src for one
// --fast check and reverted at once: one or two volatile reads through the address of the player table global, or of
// an actor field, ahead of the players iterator move the first difference to +0x13, so they do not isolate the table
// pointer's register in this function.
// Round 67 (m3 lane 15, fast with 0x25d810): retail inlines 0x1df560 here (its 0xbfe60/0x15e020 calls), as the file's
// team_is_enemy copy already does; the copy as one single-exit block (diagnostic, header restored) 168 (identical code); the
// two player-datum unit reads through game_engine_events.h's inline event_player_get 168 (identical); the vehicle lookup
// written out in place of object_get 168 (identical: inline calls add no scratch step here); the second search's counter
// as long 169 (that loop then takes retail's stepped-pointer shape, `cmp eax, 2`, registers aside); with the player
// pointer also built from a long copy of the found index 375 (both searches retail's shape but for registers and where
// `xor edi, edi` sits; the table pointer still ECX). The table pointer's ECX is the players loop's colouring (ECX/EDX
// for retail's EAX/ECX throughout); `mov eax, [abs]` is one byte shorter, which moves the loop-top pad. The unknown23
// test through the `prop` local instead of a fresh prop_get 655 (2845 bytes: retail reads it afresh).
// Round 185 (m3 lane 4, fast with 0x25d810/0x26be00/0x26be30/0x25d740; fast base 168 = plain): the prop iterator
// started before the memset 690 (first +0x13); visible as one direct value (`reference == NONE ||
// prop_ref_get(reference)->unknown27 >= 1`) 428 (713 lines: the flag leaves its byte home); the node index read from the
// iterator at both uses with no index local 168 (same code); the prop index as `actor->prop_index` at its three uses
// 168 (same code); the best level stored as `actor->unknown328` 168 (same code). Re-measured on the 168 base: the
// unknown5e test through the `state` local (the cached state test, 707 earlier) 430; positive as
// `(established || !(distance > 3.5f && approach < 0.5f)) && score > 0.0f` (round 74's `score > 0` first, mirrored) 239.
// m3 lane 5 r169 (fast with 0x25d810; aligned/masked in brackets, base 168 [80/38]): the scoring block with the
// established part written in both arms (`if (visible) {...; if (established) {...} else if (...) ...} else if
// (established) {...}`, for retail's cross-jumped entry stub at 0x266f93) 392 at 3005 B [121/60]: the copies are not
// merged. Re-measured round 78's other-object pointer 163 [75/38]; on it: the iterator's NONE fields chained 165, the
// player pointer declared before the first search 368, `visible` initialised directly from the reference test 423,
// `(established || visible)` in the outer test 234 (2882 B), the prop record looked up inside the unknown23 arm 499
// (2859 B). Retail stores visible = true ahead of the 0x25d810 call
// and treats 0x30bf0 as killing an escaped direction (round 61's diagnostics); our compile-only build does so too,
// so that residence follows how the link sees 0x30bf0 (its file and flags), not this body.
// MiniMax S (11-10): no new form, no lower score (168); the players iterator's table pointer still loads into ECX where retail
// uses EAX, and the entry loop's index compare and the iterator's setup still differ.
// M3 lane 2 r73 (plain with the file's 32 functions, 0x25c9d0, 0x264b50 and 0x1e34e0; aligned 107, masked 64 at 168): the
// one-use state record read at its use 361 (first difference +0x35); the direction through vector3d_from_points3d alone 174
// (masked 64). Re-measured on this base (earlier at 500-560 and 194): very_close as one direct bool 619 (frame 0x7c; also
// with its declaration left in place, 619), close as one direct bool 620 (frame 0x7c), positive as one test after the
// scoring block (score stays 0 when the block is skipped) 265 (2898 B). Retail keeps both close flags as stack bytes.
// m3 lane 1 r92 (fast with 0x25d810/0x26be00/0x25d740, control 168; aligned 107, registers masked 67): the first difference
// is the inlined 0x26be00 lookup's address operands (`[eax + ecx + 0x58]` for retail's `[ecx + eax + 0x58]`), then the
// listed rotation steps (other object, other actor, players iterator), the first search's NULL player (retail `xor edi,
// edi` before the scan), the second scan's stepped pointer, the direction residence and the scoring block. New finding: in
// the positive arm retail keeps the decay arm's `[0x320] = ax; [0x31c] = slot` tail and the NONE-slot reset jumps back
// into it, where ours keeps the reset arm's copy and the decay arm jumps forward (same block order, other merge direction).
// A `short cooldown` local for the decay test and store 168 (same code); re-measured on this base: the reset chained as
// `actor->unknown31e = *(short *)(data + 0x320) = 0;` 168 (same code); diagnostic: the two reset stores in the other order
// 168 (same code). No other untried natural form found. Body unchanged.
// M3 lane 7 r156 (fast with the file, 0x25c9d0, 0x26d370, 0x263740, 0x25d810, 0x26be00, 0x25d740; base masked 64): `retain`
// assigned directly in the not-positive arm 211 (2882 B, masked 62); the positive arm's decay through a `short cooldown` local
// with an unconditional store (`if (cooldown > 0) cooldown -= 2;` then the store, retail's `jle` onto its store) 169 (both arms
// share the store tail, but ours keeps the reset arm's copy and emits `sub eax, 2` for retail's `add eax, -2`); the decay as a
// ternary store 173; that local form with the reset arm first 174; the established decrease test as `!(distance > 3.5f)`
// (retail's `ja` sense) 239. Body unchanged.
// m3 lane 14 r88 (fast with 0x25d810, 0x26be00, 0x25d740, all MATCH; 0x263210's fast DIFF 292 is a reduced-image artefact,
// it shows on the unchanged body too): 0x26be00's own lookup written as an array index (it stays matched) 168 (same code: the
// inlined table load's base/index order does not follow that spelling); the positive arm's slot store written in both arms
// 481 (2896 B); re-measured on this base, the NONE reset first 174 (the merge then goes retail's way, the reset arm jumping
// into the decay arm's 0x320/0x31c stores, but the reset arm is then laid out first). Body unchanged.
// MiniMax B (11-11): no new form, no lower score (168); the first difference is still the table load at +0x5c, ours
// [eax+ecx+0x58] against retail [ecx+eax+0x58], a base/index order the compiler picks; the frame is still 0x7c against
// retail's 0x80 and the normalized direction is still folded instead of being held in XMM1..XMM3 across the dots.
// T101: Complete conditional byte-counter additions for active/close/very-close,
// sets 1..7: 621/419/621/338/632/607/649; complete conditional publications 652.
// NOT kept: every form regresses; arithmetic/extra-store shapes do not match.
// Original complete body restored; iterator, player-loop and SIMD residence remain.
// P474: Standard k-seeded reverse-coordinate reductions at absolute, relative
// and facing dots, seven subsets then explicit operation contracts.
// Native iterator homes and normalized-direction SIMD residence remain.
// Scores 506/494/593/559/645/639/696/775. NOT kept: ties/regressions;
// original executable body and helper contracts restored.
// P481: Actual point/absolute/relative scratch final-consumer scope ends, seven
// subsets, then all with read-only unit receiver: eight 168s at 2880/2880.
// NOT kept: no native iterator/search/SIMD equality or gain; original scopes,
// producers, helpers and complete executable body restored.
// P484: One complete actor/vehicle eligibility result bool/byte/short/long,
// positive then rejection-owned: 434/434/434/434/434/434/434/434. NOT kept:
// shared failure/result storage misses equality and shortens extent to 2839/2880;
// original guards, resets, iterators, searches, geometry and entire body restored.
// MiniMax H (11-14): no new form, no lower score (168): the first difference is still the inlined 0x26be00 and the table load at +0x5c, whose helper is defined in another file.
// M3 lane 13 r91 (fast with 0x25d810, control 168; difflib-aligned counts with addresses masked in brackets, base [80]): no
// `positive` flag, the far-and-slow case written `else if (distance > 3.5f && approach < 0.5f) score = 0.0f;` under the
// established block and the arms chosen by `if (score > 0.0f)` 168 [80] (same code); the far-and-slow zeroing moved into the
// visible arm (`if (!established && distance > 3.5f && approach < 0.5f) score = 0.0f;`) with the outer test reduced to
// `distance < 15.0f` 243 [68] at 2890 B: the scoring block then takes retail's block order (visible arm first, the far-and-slow
// exit inline, the not-visible arm entering the established block) with no flag; the same keeping `(visible || established)`
// in the outer test 242 [79]; that 243 form with round 65's recipe (other-object pointer, ai_player_get/ai_player_index_get,
// `inline` on 0x1c8180, a __forceinline object_get_forward declaration; all 15 helper users matched) 300 [59] at 2886 B, 695
// instructions as retail: what is left there is the iterator's table pointer register, the direction residence across the
// two dots and the positive arm's merge direction. Re-measured: player NULL first 373 [92], long slot counter 169 [84], both
// 376 [87]. Diagnostics, reverted: n = 0..7 unused long locals at the top 168 each (the base/index order and the table
// register do not follow the symbol count); the recipe with the scoring block removed still loads the table pointer into
// ECX with NONE in EAX, so that choice is local to the iterator start. Body unchanged.
// m3 lane 8 r166 (fast with the file, 0x25c9d0, 0x26d370, 0x25d810, 0x26be00/0x26be30, 0x25d740 and the 15 users of 0x1c8150/
// 0x1c8180): re-measured round 65's recipe (ai_player_get/ai_player_index_get, `inline` on 0x1c8180, a __forceinline
// object_get_forward declaration) 383 and with round 86's other-object pointer 378 (both at 2880 B, every other listed
// function matched): on 378 every line up to the players iterator start is retail's but the inlined 0x26be00's `[eax + ecx +
// 0x58]` and the jump targets the one-byte shift moves; at the iterator start ours loads the table pointer into ECX and
// schedules `or eax, -1` and `lea esi` ahead of its store, retail loads it into EAX, stores it and then reuses EAX for NONE.
// Diagnostic (reverted at once): on 378 the relative normalize called through a global function pointer with direction's
// address stored 525 (2921 B): the iterator still takes ECX, so that register does not follow the direction residence.
// Also on 378 (same code, 378): the vehicle test as `!(vehicle_type == 0 || vehicle_type == 1 || vehicle_type == 4)`; the
// players iterator declared ahead of the vehicle lookup. Every free register choice up to the vehicle test's `xor ecx, ecx` is
// retail's; the first one after it (the iterator's table pointer) and those inside the players loop are one rotation step on.
// M3 lane 14 r95 (fast with the file's 33 matched functions and 0x26be00's users 0x1b99d0, 0x1c1d50, 0x1e23c0, 0x25b440,
// 0x25d770, 0x25d810, 0x267840, 0x267c50, all MATCH): 0x26be00 (ours now, inlined at the first difference) written as one
// statement without its one-use object pointer 168 (same code: `[eax + ecx + 0x58]` stays); the actor's `data` alias
// dropped for `((byte *)actor)` at its 30 uses 168 (same code); the other actor as `other = NULL; if (index != NONE) other =
// actor_get(index);` 382 (block layout moves); the prop flag through the `prop` local instead of the second prop_get 655
// (2845 B: retail fetches it again); the players threshold typed short without the two casts 168 (same code).
// P503: Actual actor/vehicle/unit provider references in seven sets, then readonly vehicle/unit providers:
// 168/168/168/168/168/168/168/168. NOT kept: All eight tie 168 at 2880 bytes; iterator base/index encoding, player
// table register, direction SIMD retention and scoring merge are unchanged. Reference-only allocation forms lack
// byte equality. Original complete executable body restored.
// M3 lane 2 r86 (fast with 0x25d810; 0x26be00's eight users MATCH throughout): 0x26be00's lookup spelled index-first
// (`(index & 0xffff) * size + data`) 168 (same code: the inlined `[eax + ecx + 0x58]` stays); the players iterator's datum
// index in a named local for both searches 168 (same code); the positive arm as `if (slot == NONE) reset; else if (slot
// != player_slot) continue; else if (cooldown > 0) decay;` 174, with the middle test positive and `else continue;` 174;
// the first search's index widened into a long local for the NONE test and the address 372 (that block becomes
// retail's `movsx eax, si; cmp eax, -1`, but the players loop's registers rotate). Re-measured: 0x1c8150 for the slot
// search alone, and for both searches, 769 each (2680 B, a call); 1 to 7 unused locals 168 each.
// MiniMax R (11-16): no new form, no lower score (168); retail keeps the direction's three components in XMM1/XMM2/XMM3 across the relative and facing dot products where this build reads them from the frame again, and the two player searches still differ (retail's first search compares the index in 32 bits with the table base in EDX and the result in ESI, ours in 16 bits with the base in ESI and the result in EDX, and retail's second search steps a pointer where ours scales the index).
// MiniMax H (11-12): no new form, no lower score (168): the first difference is still the inlined 0x26be00
// lookup's address operand (ours [eax+ecx+0x58], retail [ecx+eax+0x58]), a base/index order the compiler
// picks when the helper is expanded, and that helper's body is in src/unknown_26bda0.cpp, which is not mine;
// the permuter refuses this function because the reduced image and the full image disagree.
// MiniMax B (11-11): no new form, no lower score (168); the first difference is still the table load at +0x5c, ours
// [eax+ecx+0x58] against retail [ecx+eax+0x58], a base/index order the compiler picks; the frame is still 0x7c against
// retail's 0x80 and the normalized direction is still folded instead of being held in XMM1..XMM3 across the dots.
// MiniMax I (11-15): no new form, no lower score (168): retail's second player search keeps a dword result at +0x1c and a
// 32-bit counter with a stepped pointer, and the result is read back as a word, so typing that result long with a long counter
// measures 365 (2893 bytes against 2880) and is not kept; the first search has retail's shape already (a 16-bit result in SI, a
// 32-bit counter) and differs only in the register that holds the table base (EDI here, EDX in retail); in the positive arm
// retail zeroes +0x31e and jumps back to the one store of the +0x320 count, while this build stores both in the else branch and
// falls through to the +0x31c store.
// M3 lane 14 r100 (fast with 0x25d810, 0x26be00, 0x25d740, all MATCH; base 168): the iterator start written as the file's
// sibling idiom `iterator.next = actor_get(actor_index)->first_prop_index;` 168 (same code: the reloaded table's
// `[eax + ecx + 0x58]` stays; retail's own 0x26be00 body has that order too, `[eax + edx + 0x58]`); the same as an array
// index on the table (diagnostic) 168 (same code); the start through the `actor` local 559 (retail reloads the table); the
// relative and facing dots through dot3f (`dot3f(&relative, &direction)`, `dot3f(&direction, &forward)`) 168 (same code:
// the direction stays in memory); `direction` declared at the players-loop top 168 (same code).
// M3 lane 1 r112 (fast with 0x25d810 listed, equal to plain): the slot search with a long counter 169 (2880 B: the loop
// is then retail's exactly, masked 35 -> 32; plain 169 too); with `player = NULL` declared ahead of the first search
// 376 (the not-found exit threads past the index test); with a long copy of the short index for its NONE test and
// subscript (`long table_index = player_table_index;`) 375 (masked 30: the search is retail's but for where `xor edi,
// edi` sits); with the player as one ternary 184; the facing dot's products written forward-first 168 (same code).
// b8E (11-17) walk against retail, fast build on r836 (168, 2880 B, as before): no logic, offset, field, callee or
// constant error found. Walked: the prop walk 0x2666b0-0x2668c8 (`!prop` skips the strongest test at 0x266743, a failed
// unknown5e test does not, 0x266755; 0x2672e0 returns its view in EAX, 0x26679f `mov edi, eax` feeds 0x267700, ours
// passes `view`); the flags 0x2667c7-0x266899; the tail 0x2668cd-0x26690a; the 0xa8/0xa9 events 0x26690f-0x266a5b; the
// gates 0x266a5b-0x266b5d; the players loop 0x266b70-0x267147 (every constant read; every jcc sense equals ours, NaN
// included, except one). That one: the decrease test `else if (distance <= 3.5f)` compiles `comiss 3.5, d; jb` (NaN
// skips) where retail 0x266fcc is `comiss d, 3.5; ja` (NaN enters); NaN cannot reach it (0x266db3 `jbe 0x26713c` drops
// it), so no behaviour differs. `!(distance > 3.5f)` there gives retail's sense and measures 239 (fast, r836), 701
// instructions against 695: not kept. Facing lives only in xmm1 (0x266f39); the stack zero at 0x266c51 is what the
// not-visible arms reload (0x266f9f, 0x26705f), as ours `facing = 0.0f`. The far-and-slow exit 0x266f8e skips the score
// test; ours reads `score > 0` on the same paths. Body unchanged.
// M3 lane 2 r95 (fast with 0x25d810/0x26be00/0x25d740, all MATCH; control 168): retail's positive arm keeps the decay arm's
// `[0x320] = ax; [0x31c] = slot` tail and the reset arm (laid out after the join) jumps back into it, so its unmerged
// layout had the reset arm first. The reset arm written first (`if (slot == NONE) reset; else { mismatch continue;
// decay; }`) 174 (2880 B: the merge direction is then retail's, reset jumping to the decay's 0x320 store, but the reset
// block stays ahead of the decay where retail places it after the join); with the slot search's counter long as well 175;
// the slot store written in both arms 481 (2896 B, first difference +0x35, the duplicate store kept), the same with the
// reset arm first 481; the approach maximum as `approach = value > approach ? value : approach;` 168 (same code).
// M3 lane 3 r256 (fast with 0x25d810, 0x26be00, 0x25d740, all MATCH; control 168): the players iterator started through a
// file-local inline setter of its three fields (data, then NONE index and datum index) 168 (same code: the table pointer
// still takes ECX); the first normalize through the header's inline normalize3d 405 (2954 B), the relative one 507 (2967 B);
// approach and facing in two declarations 168 (same code); the player pointer as `if (index == NONE) player = NULL; else
// player = &...;` 374.
// Round 620: the inlined iterator load still exchanges base and index. Typed proximity fields 168;
// typed winning level and view influence 168; one view influence read 409; typed proximity with retained
// other object 163 (NOT kept: five-point gain above 30), plus one view influence read 404;
// absent view rejected first 589; direct proximity predicates 621, with retained other object 616.
// Named primary/other/both reference handles 168/168/168, proximity assigned from both 587.
// Observed object and both handles 163. NOT kept: the five-point gain at 163 is below the eight-point cutoff.
// Actor lookup delayed 595, both lookups delayed 592, shared separation 379. Iterator SIB, player searches
// and direction residence still differ.
// MiniMax S (11-17): fast build on r852: the permuter's 2 tries and the source order of the inlined 0x26be00 both stay at 168;
// the prologue and the frame (0x80 in both) match instruction for instruction and the first difference is the inlined 0x26be00
// reading [eax+ecx+0x58] where retail reads [ecx+eax+0x58] (the same address, the two registers swapped), and the player loop
// further down allocates the table base and the index to different registers.
// M3 lane 7 r177 (fast with 0x25d810/0x26be00/0x25d740/0x26be30/0x2662f0/0x266540, all MATCH; control 168, aligned
// 107): the slot search's counter long 169; 0x26be00 in the array-index form 168 (same code); the score through a
// reference-bound local 168 (same code); the decay read into a named short before its store 168 (same code); the
// relative vector ahead of the velocity copy 266 (695 instructions, aligned 122).
// DH398: Normalized direction snapshot after its call, distance guard, contact lookup,
// or relative construction, as a vector or three scalars 429/429/429/429, 168/168/168/168.
// NOT kept: Vector copies enlarge the frame to 0x8c. The scalar snapshots reproduce the baseline, including the iterator SIB and the direction reloads. The original direction consumers are restored.
// The first difference reverses the base and index in the inlined iterator load. Both builds have a 0x80
// frame. The player scans retain different registers even when they share the table and datum index in
// source. shared datum index for both searches 168; shared player table for both searches 168; shared
// player table and datum index 168; first scan selects player pointer 378; player selected from first
// scan bound 375; slot selected from second scan bound 370; both searches select from scan bound 189;
// player pointer selected after first scan 374. NOT kept: the retention threshold was not met.
// M3 lane 3 r261 (fast with 0x25d810, 0x26be00, 0x25d740, 0x26be30, 0x2662f0, 0x266540, 0x267700 and 0x2672e0, all
// MATCH; control 168): the other actor's lookup as `s_slot_object_view *object = object_get(...); other =
// object->actor_index == NONE ? NULL : actor_get(object->actor_index);` 163 (the type-2 arm is then retail's line for
// line: the object pointer reuses EAX and the actor table takes ECX, as round 620's retained other object found; a
// five-point gain, under the bar); that lookup as an if/else on `other_index` 168; as `other_index != NONE ?
// actor_get(other_index) : NULL` 173; on the 163 form: the slot search's counter long 164, with the player as a ternary
// on the found index 179, the direction built by vector3d_from_points3d 169. Left on 163: the first iterator load's
// base/index order, the players loop (retail's table pointer in EDX, the datum index in EBP, the found index in ESI, the
// player in EDI, and no jump over the loop-top padding), and the direction's components held in XMM1..XMM3 across the
// relative and facing dots (retail loads two of them ahead of the relative-length test).
// M3 lane 11 r121 (fast with 0x25d810, MATCH each time; control 168): the players iterator started through a file-local
// inline helper returning the iterator by value 383 (2883 B: the whole iterator is copied, the table pointer still misses EAX;
// diagnostic for a scheduler boundary, not kept); the two NONE stores chained 170; the players iterator declared at function
// scope 168 (same code); the actor looked up as the byte pointer first with the typed actor derived from it 168 (same code: the
// iterator start keeps `[eax + ecx + 0x58]`); the byte alias dropped, the raw offsets through `(byte *)actor` 168 (same code).
// Shared actor displacement with pool pointer, pool reference, refreshed data or field reads:
// 167 each, first +0x1a7. Typed indexing across the same reads: 168 each.
// NOT kept: the one-point SIB gain is below the retention threshold.
// M3 lane 5 r200 (fast with the eight callees, all MATCH; control 168; on lane 3 r261's 163 form unless noted): `visible` as
// `reference == NONE || prop_ref_get(reference)->unknown27 >= 1` 423 (the frame changes); the iterator's NONE fields chained
// 165; the player as a ternary on the found index 367; both searches in ai_player_get's shape (player NULL first, a long copy
// of the short index for its NONE test, a long second counter) 378 (register-masked aligned 41 -> 39: both searches are then
// retail's line for line, one line late because the iterator's table load takes ECX where retail's takes EAX); on that, the
// NONE fields first 380, the iterator set up before the vehicle lookup 401 (2889 B), the NONE fields chained 380.
// M3 lane 8 r194 (fast with the nine callees, all MATCH; control 168): the players iterator's NONE fields in one chained
// assignment 170; the established decay as `score += 1.0f - MAX(distance - 2.5f, 0.0f);` 168 (same code); the positive arm's
// decay through a `short &` binding to the 0x320 field 168 (same code: the reset arm still holds the shared 0x320 store); the
// zero-distance `continue` ahead of the established and retain flags 182; the established flag computed after the visibility
// lookup 618. Body unchanged.
// M3 lane 6 r200 (fast; control 168): re-measured round 65's search recipe (both searches as the existing 0x1c8180 and
// 0x1c8150 helpers 849 at 2642 B; `inline` on 0x1c8180's definition 795 at 2682 B, all its callers still matched, the
// forward getter then drops out of line; plus a `__forceinline` declaration of that getter 383 at 2880 B with every search
// line retail's); a dead field read beside the vehicle lookup on it 383 (eliminated). On the base: the other actor as `other =
// NULL; if (other_index != NONE) other = actor_get(...)` 382; very_close declared ahead of close 406; `facing, approach`
// declared in that order 170; `close` as one direct `||` expression 620 (first difference +0x0); the other-actor ternary
// with the test negated and the arms swapped 173. Still left on the recipe: the players iterator's table pointer in ECX for
// retail's EAX (`mov eax, [abs]` is a byte shorter), which shifts the loop pad and everything after it.
// M3 lane 7 r186 (fast with 0x26be00, 0x26be30, 0x25d690, 0x25d740, 0x25d810, all MATCH; control 168, aligned/masked
// 107/88): `player` cleared before the first search and the slot search on a long counter 376 (702 rows; the slot search
// becomes retail's pointer walk, the NONE test threads); the player through the existing ai_player_get with the long slot
// search 849 (2642 B: ai.cpp's /Ob1 keeps it a call, re-measured); ai_player_get's shape written out (player NULL first,
// short index copied to a long for its NONE test, long slot counter) 383 (107/83: both searches are retail's instruction for
// instruction, but the players table then loads into ECX, a 6-byte load against retail's 5-byte EAX load, so the loop-top
// pad becomes `jmp` plus `lea ecx, [ecx]` and every later row is one off); plus the decrease test as `!(distance > 3.5f)`
// 317 (108/84); plus the established arm nested in `if (established || !(far && slow)) { ...; positive = score > 0; }`
// 377 (2882 B, 99/75: retail's block order for the far-and-slow test). Left: the players table's register at the iterator
// setup (the matched siblings' identical three statements load it into EAX) and the direction components, which retail
// keeps in XMM1-3 from the relative dot to the facing dot.
// M3 lane 5 r210 (fast with 0x26be00, 0x26be30, 0x25d690, 0x25d740 and four neighbours; control 168, register-masked 41):
// the actor lookup written out (diagnostic for the iterator head's operand order) 168 (same code); the contact iterator
// declared first 168 (same code); best level and strongest declared after the iterator start 172; the players
// iterator's index and datum index stored before its table 170; the players iterator declared at function scope 168
// (same code).
// Contact step results declared in the while condition or advanced in the for loop score 168/168
// and 753/753 as a node view and prop datum, respectively. Keeping the other object across both
// actor-index reads scores 163/163 and 753/753. NOT kept: five-point gain at 163.
// Post-increment loss/age/both 168/168/168; separate increment publication
// loss/age/both 224/206/223; post-increment loss or age with shared actor displacement
// and other-object field reads 162/162 at 2880 bytes. NOT kept: the six-point gain
// leaves a score above 30. Player iterator and scoring registers still differ.
// M3 lane 16 r152 (fast with 0x25d810, MATCH each time; control 168; body unchanged): the vehicle-type test through a
// file-local inline predicate taking the type by value 769 (2687 B: one more inline function here moves the link's
// inline choices for the whole body); this file's own inline definition of 0x30bf0 (same name and body, in place of
// the declaration) 168 (same code, 0x30bf0 still matches: the direction's residence does not follow where the call
// binds); `direction` as an aggregate initializer 168 (same code); direction k and j named inside the relative arm
// 168 (same code: the copies fold back into memory operands). Re-measured: the two NONE stores ahead of the table
// store 170. Diagnostics (behaviour changes, reverted at once): the vehicle-type exit without its two reset stores
// 398, and the players iterator started with no NONE stores at all 385: in both the iterator's table pointer still
// loads into ECX, so that register follows neither the zero register of the reset stores nor the NONE constant.
// Reading of retail's relative arm: direction j and k load ahead of the relative-length branch and i in both arms
// (the skip arm at 0x266ed0, the join at 0x266ed6), all three then feeding the facing dot from registers, where the
// first dot (after the velocity normalize) reads them from memory in both builds.
// Other actor-index field reference or pointer 384/384, contact object handle 168,
// shared threshold distance 379, nested very-close tests 350, explicit qualified outcomes 168,
// complete view-state selection 619, named shared-target object handles 168.
// NOT kept: the field alias inserts an address addition. The explicit outcomes and handles tie.
// The contact head SIB and player iterator registers still differ.
// M3 lane 2 r119 (fast, plain agrees; 0x26be00/0x26be30/0x25d690/0x25d740/0x25d810 MATCH each run; control 168; aligned = rows
// outside a minimal diff against retail, addresses, stack offsets and registers masked, control 66). Kept, 158 at 2880 B
// (aligned 50), four parts that each need the others (leave one out: 235/163/224/163): (1) the outer test reduced to
// `distance < 15.0f` with `visible || established` tested after the facing block (the visible arm is then laid out first,
// as retail); (2) the decrease test as `!(distance > 3.5f)` (retail's `comiss d, 3.5; ja` sense, the b8E walk's one
// differing jcc); (3) the slot search on a long counter (retail's 32-bit pointer walk); (4) the other actor through the
// object pointer (`object->actor_index == NONE ? NULL : actor_get(object->actor_index)`, lane 3 r261's type-2 form).
// Steps: (1) alone 241 (2887 B, aligned 54); (1)+(2) 168 (2883 B, 695 instructions as retail, aligned 52); (1)+(2)+(3) 163.
// On (1): the established arm setting positive with the far-and-slow test as its else 247 (2890 B); the not-established arm
// first with positive in each arm 248 (2895 B, aligned 41). On the 163 form: player NULL first with a long copy of the short
// index for its NONE test 377 (the players loop then takes retail's registers, one row late behind the ECX table load), the
// long copy alone 369, NULL first alone 370. On 158: the far-and-slow test joined to the outer test ahead of the established
// arm, `if ((visible || established) && (established || !(far && slow))) { if (established) {...} positive = ...; }` 224
// (2880 B, aligned 34: the scoring block's layout is then retail's, the direction residence and the iterator remain; a
// nested test repeating its enclosing one, match-only); the iterator's NONE fields chained 160; the NONE reset arm first
// 164; the decay through a short local with one unconditional store 159; the players iterator's two NONE fields stored
// ahead of its table pointer 160. Diagnostic: 1 to 7 unused long locals at the top
// 168 each (the +0x5c base/index order does not follow the symbol count).
// Early player iterator index/datum/both before the vehicle lookup 398/398/410,
// before actor admission 423/423/421, with shared actor displacement and both
// defaults before the vehicle lookup or actor admission 409/420. NOT kept: the
// shared displacement fixes the head address encoding, but the other forms
// leave the pool register and player search differences and raise the score.
// M3 lane 2 r123 (fast with 0x25d810/0x26be00/0x26be30/0x25d690/0x25d740, all MATCH; control 158, difflib-aligned 96,
// register-masked 50; check.py --fast agrees on the lead). LEAD (match-only: an if/else order picked by score): the
// positive arm's skip test hoisted ahead of its dispatch with the reset arm first (`if (31c != NONE && 31c != slot)
// continue; if (31c == NONE) { reset } else if (cooldown > 0) decay; 31c = slot;`) 149 (2880 B, aligned 90, masked 44):
// the arm is then retail's row for row (the NONE test threads into the skip test, the reset arm sits after the join and
// jumps back to the decay's 0x320 store); the same with the decay arm first 158 (same code), so the gain is the arm order
// alone. Also: the slot store at the end of both positive arms 239 (2896 B: the then-arm's store reuses the slot loaded
// for its compare, CX, so the two copies differ and do not merge); the hoisted skip test with three plain statements
// (reset, decay, slot) 224 (2884 B). Re-measured on 158: the reset-first else-if chain 164; the reset arm first 164;
// player NULL first with a long copy of the short index 372 (the search lines are retail's, everything after them one row
// and one scratch step late behind the iterator's ECX load). Scoring block with no `visible || established` wrapper: the
// established arm setting positive with `else if (visible && !(far && slow))` for the rest 186 (2853 B; with the lead
// 177); the established block followed by one `established || (visible && !(far && slow))` test 255 (2848 B; with the
// lead 255). Diagnostic: one or two unused locals read from actor fields beside the iterator 158 (dropped before
// allocation; the iterator's ECX does not move).
// M3 lane 3 r283 (fast, control 158; body unchanged): the scoring block as `if (established) { ...; positive = score > 0; }
// else if (visible && !(far && slow)) positive = score > 0;` with no outer `visible || established` test 186 (2853 B); the
// vehicle-type test as `!(type == 0 || type == 1 || type == 4)` 158 (same code); the vehicle definition named and its type
// read at each of the three compares 363; `retain` assigned directly in the decay arm (`retain = !(count > limit || age <
// threshold);`) 201 (2882 B); the established block's approach bonus tested ahead of the distance bonuses 232. Reading:
// every instruction ahead of the players loop is retail's, yet the loop's table pointer is one scratch-rotation step off
// (ECX for retail's EAX); lane 2 r119's 377 form already gives the loop retail's registers behind that one-byte-longer load.
// Prop-view head 158, with the sibling cursor 158; iterator seeded before the clear 690, typed head before the clear 690.
// Winning handles read at publication 158; level stored after its handle 420; cached signed alert slot 499; unsigned slot check 526.
// NOT kept: the typed iterator forms tie and the remaining forms regress. The player iterator and direction homes still differ.
// MiniMax H (11-20): no new form, no lower score (158): the relative dot still reads the direction's components from
// memory where retail keeps them in registers (it loads the direction's j and k before the branch), the players
// iterator still takes the table pointer in eax where retail takes it in ecx, and neither the operand order in the
// three dots nor copying the direction's components into scalars changes anything; a permuter run found no variant.
// state and view gate 158, view and state gate 585, one state-first eligibility test 158, one view-first
// eligibility test 585, view-owned updates with nested state 585, view-owned updates with combined state
// 585, nested state and one view influence read 605, combined state and one view influence read 605.
// NOT kept: The state-first forms tie. The other forms shorten the body and raise the score. The one-read forms also disagree with retail’s two influence loads.
// The 0x80 frame agrees. The contact-head address reverses base and index, and the player iterator and direction registers still differ.
// Player-pointer and slot searches with bounded header tests, independently and together 363/353/179.
// Bounded slot with failure default 164, bounded pointer initialized by selection 362, shared table
// across separate scans 158, shared table with bounded scans 179, block-scoped bounded cursors 364.
// NOT kept: the shared table ties and the other forms regress at retail length. Contact-head address
// encoding and the player iterator's table register still differ.
// Shared admission and reset with positive/complement/presence-first/explicit-default actor tests,
// each with supported-type assignment or unsupported-type clear 401/401, 401/401, 459/459, 401/401.
// The contact-head address remains the first difference and the shared reset shortens the body.
// M3 lane 14 r140 (h2m fast with 0x26be00/0x25d810/0x25d740, all MATCH; control 158): the not-established far-and-slow
// decision inside the visible arm (`if (!established && !(far && slow)) positive = score > 0.0f;`) with the established
// arm after it setting positive itself, no `visible || established` block, 160 (2885 B: the scoring block then has
// retail's order, the established test jumping out to the bonus arm and the unseen path testing established before it, but
// the two score tests stay separate where retail shares one, and the zero/approach XMM roles and two slot pairs swap);
// that form with the far-and-slow test written first 233, with the visible decision as one direct boolean expression 227;
// one `established || (visible && !(far && slow))` block holding the bonuses and the score test 247 (2856 B); the
// established arm with `else if (!visible || (far && slow)) score = 0.0f;` and one score test 255 (2848 B).
// M3 lane 16 r174 (h2m fast with 0x25d810, 0x26be00, 0x26be30, 0x25d740, 0x25d690, all MATCH; control 158, aligned 93):
// the relative maximum as `approach = value > approach ? value : approach;` 158; approach and facing declared apart 158;
// the velocity dot through dot3f 158 (all same code); `facing` declared just ahead of the distance test (zeroed there)
// 706 (2854 B); the relative vector written out from the two velocities (diagnostic) 172 (2883 B). Retail loads the
// normalized direction's k and j into XMM1/XMM2 after the relative normalize and before its length test, and i into XMM3
// on both edges, then reads all three from registers in the facing dot. Body unchanged.
// M3 lane 4 r253 (h2m fast with 0x26be00, 0x26be30, 0x25d690, 0x25d740 and 0x25d810, all MATCH; control 158): the stacked
// first lines merged into one note. Diagnostics for the head's `[eax + ecx + 0x58]`: 8, 16, 24 and 40 initialised dead long
// locals at the top 158/158/164/158 (the order never flips, so it does not follow the symbol count); the count block cleared by
// four long stores in place of the memset 688 (still `[eax + ecx]`). Forms: the head read as `actor_get(actor_index)`'s +0x58
// field with no 0x26be00 call 158, as `g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x58` 158, product first 158 (all the
// same code: matched 0x1e23c0 forms the same CSE'd product and gets retail's data-first order, so the order is this body's
// context, not a spelling); strongest/best declared ahead of the actor 170; all three dots through dot3f 158, the relative and
// facing dots alone 158 (same code: the sum order is canonical and the direction still reads from memory). Body unchanged.
// M3 lane 16 r184 (H2M_FAST with 0x26be00, 0x26be30, 0x25d690, 0x25d740 and 0x25d810, all MATCH; control 158, aligned 96,
// masked 50). Reading of the head's `[eax + ecx + 0x58]` (tracer rank view): the add is ordered by rank, highest first;
// the product is the common subexpression shared with the actor lookup, and a compiler temporary ranks by its symbol
// number (here 0x702, rank 0x1e040), above the reloaded table field (0x18008), so the product becomes the base. Retail's
// data-first order needs that temporary's number, modulo 0x800, at or below 0x400: about 254 to 1278 more symbols ahead
// of it than this body creates. Diagnostic (reverted): 256 and 300 unused initialised longs at the top flip the head to
// retail's order, 157, first difference +0x428. The calls are already retail's, so retail's extra symbols would come
// from helpers expanded inline where this body writes their code out (the two player searches are 0x1c8150/0x1c8180's
// bodies, which stay calls from here). Tried: the distance test ahead of the established and retain flags 172; retain
// declared after the distance test 163; the relative maximum as a ternary 158; the reference test as `!(... >= 1)` 158
// (same code each); the players iterator's NONE fields stored before its table 160, datum index first 160, chained 160.
// Body unchanged.
// Typed alert recipient, typed iterated unit, both, named player record
// pointer/reference, one guarded unit-index read, two nested guards and
// one guarded read with typed alerts all 158. The contact-head address
// and player table register still differ.
// M3 lane 16 r186 (H2M_FAST with 0x26be00, 0x26be30, 0x25d690, 0x25d740 and 0x25d810, all MATCH; control 158, aligned 96).
// Reading of the direction registers (reassociation ranks, own words): each product's operands sort by rank, highest
// first, and a local's field ranks by when the source first names that field. Here direction's fields are named first
// (its three stores), then velocity's, relative's and last forward's (in the facing dot), so forward outranks direction
// and becomes the register operand of the facing dot; direction is then a foldable memory operand at both later dots and
// is never worth a register. Retail has direction as the register operand of the facing dot (`mulss xmm1, [esp + 0x48]`),
// so its forward fields rank below direction's: they were named before direction's stores.
// Diagnostic, reverted (an unused local only steers): `real probe = forward.k + forward.j + forward.i;` right after the
// forward lookup emits no code and makes the whole dot block retail's (direction's j and k loaded before the relative
// length test, i on both edges, the facing dot in place in XMM1-XMM3): 180 positional, aligned 53 (what is left: the
// head's address order, the two player searches, the far-and-slow block order and the count stores' order).
// Forms tried for a natural early naming of forward's fields: direction through vector3d_from_points3d with all three
// dots through dot3f 196 (aligned 71: direction does sit in XMM1-XMM3, but each dot then sums i, k, j where retail sums
// k, j, i; the helper's own term order, all six, gives the same code); that with the dot arguments swapped 196; direction
// through the points helper alone 164 (aligned 103); with the facing dot through dot3f 164; the points helper, dot3f at
// the velocity and relative dots and the facing dot direct with forward first 200 (aligned 74), direction first 164
// (aligned 105); the facing dot computed right after the direction's normalize 350 (2863 B), just ahead of the distance
// test 711 (2847 B). Symbol numbering is per function: 300 unused externs ahead of the function 158, a 300-local
// function ahead of it 158 (same code, the head stays `[eax + ecx + 0x58]`). Body unchanged.
// M3 lane 1 r167 (h2m fast with 0x26be00, 0x26be30, 0x25d690, 0x25d740, 0x25d810, all MATCH; control 158, aligned 71,
// registers masked 29). Retail tests `established` with a `jne` to the bonus block and lays the far-and-slow test out
// as the fall-through, both reaching one shared `score > 0` test. Forms: inside the outer test `if (!established) { if
// (!(far && slow)) positive = score > 0; } else { bonus; positive = score > 0; }` 153 at 2885 B (masked 19: retail's
// block order, but the far path keeps its own copy of the `score > 0` test, the 5 extra bytes; the gain is partly that
// copy realigning rows behind the dot block, which is two rows short); the same with no outer test (`visible &&` in the
// far condition) 167 (2864 B); the far case zeroing the score with one `positive = score > 0` after the if/else 224
// (2880 B, masked 20); the first form with the reset arm first (`if (slot == NONE) {...} else {...}`) 159; `if
// (established) {bonus; positive = ...} else if (!(far && slow)) positive = ...` 219 (2880 B). Re-measured the note's
// joined form, `if ((visible || established) && (established || !(far && slow)))`, 224 (masked 20). Body unchanged.
// @retail 0x266640
void function_266640(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long strongest_node = NONE, best_node = NONE;
	byte *data = (byte *)actor;
	short best_level = 0;
	real strongest = 0.0f;
	memset(data + 0x324, 0, 0x10);
	s_iterator iterator;
	function_26be00(actor_index, &iterator);
	s_node_view *entry;
	while ((entry = function_26be30(&iterator)) != NULL)
	{
		s_prop_datum *node = (s_prop_datum *)entry;
		long index = iterator.index;
		s_type_f95cd3 *view = function_25d740((s_prop_node *)node);
		s_type_5cfb45 *state = function_25d690(node);
		s_type_76cf92 *prop = prop_get(node->prop_index);
		if (node->state >= 1)
		{
			if (!view) continue;
			if (!function_25d690(node)->unknown5e)
			{
				short level = function_267180(node);
				if (level > best_level)
				{
					best_level = level;
					best_node = index;
				}
				if (prop_get(node->prop_index)->unknown23)
				{
					function_2672e0(view, (s_prop_node_view *)node);
					function_267700(actor_index, node, prop, view);
					if (node->unknown27 >= 1) ++*(char *)(data + 0x325);
					++*(char *)(data + 0x324);
				}
				else if (node->type == 2)
				{
					s_slot_object_view *object = object_get(node->object_index);
					s_actor_view *other = object->actor_index == NONE ? NULL : actor_get(object->actor_index);
					bool close = false;
					bool very_close = false;
					if (node->unknown28 < 8.0f) close = true;
					else if (*(bool *)((byte *)state + 0x62) && other &&
						*(long *)(data + 0x338) != NONE && *(long *)((byte *)other + 0x338) != NONE &&
						prop_ref_get(*(long *)(data + 0x338))->object_index == prop_ref_get(*(long *)((byte *)other + 0x338))->object_index)
						close = true;
					if (view->unknown06 == 0 || view->unknown06 == 1)
						very_close = node->unknown28 < 3.0f;
					if (close) ++*(char *)(data + 0x326);
					if (very_close) ++*(char *)(data + 0x327);
				}
			}
		}
		if (view && *(real *)((byte *)view + 0x3c) > strongest)
		{
			strongest_node = index;
			strongest = *(real *)((byte *)view + 0x3c);
		}
	}
	*(short *)(data + 0x328) = best_level;
	*(long *)(data + 0x32c) = best_node;
	if (strongest_node != *(long *)(data + 0x338)) function_267770(strongest_node, actor_index);
	function_266540(actor_index);
	*(bool *)(data + 0x340) = false;
	function_2662f0(actor_index);
	if (*(bool *)(data + 0x30c) && *(short *)(data + 0x310) >= 0 &&
		*(short *)(data + 0x310) < 2 && actor->unknown086 < 8)
	{
		s_ai_player *player = &g_4f55cc[*(short *)(data + 0x310)];
		if (player->unknown0c <= 0 || g_510c54->game_time - player->unknown0c > g_510c54->field_2_3 * 3)
		{
			short first = g_510c54->field_2_3 * 3;
			short second = g_510c54->field_2_3 * 15;
			real *settings = (real *)function_1e4a90(*(long *)(data + 0x54));
			if (settings)
			{
				first = (short)function_1469f0(settings[0]);
				second = (short)function_1469f0(settings[1]);
			}
			short elapsed = ++*(short *)(data + 0x30e);
			if (elapsed == first)
			{
				if (player->player_index != NONE)
					function_1fb7e0(0xa8, actor_index, NULL, *(long *)(g_4e8c24->data + (g_4f55cc[*(short *)(data + 0x310)].player_index & 0xffff) * 0x21c + 0x2c), NONE);
			}
			else if (elapsed == second && player->player_index != NONE)
				function_1fb7e0(0xa9, actor_index, NULL, *(long *)(g_4e8c24->data + (g_4f55cc[*(short *)(data + 0x310)].player_index & 0xffff) * 0x21c + 0x2c), NONE);
		}
		else *(short *)(data + 0x30e) = 0;
	}
	else *(short *)(data + 0x30e) = 0;
	if (actor->unknown26c == NONE || actor->unknown018 == NONE ||
		(actor->unknown024 != 1 && team_is_enemy(actor->unknown024, 1)))
	{
		actor->unknown31e = 0;
		*(short *)(data + 0x320) = 0;
		actor->unknown31c = NONE;
		return;
	}
	s_slot_object_view *vehicle = object_get(actor->unknown26c);
	short vehicle_type = *(short *)(g_4e3b44[vehicle->tag_index & 0xffff].bytes + 0x1f0);
	if (vehicle_type != 0 && vehicle_type != 1 && vehicle_type != 4)
	{
		actor->unknown31e = 0;
		*(short *)(data + 0x320) = 0;
		actor->unknown31c = NONE;
		return;
	}
	s_data_datum_iterator players;
	players.data = g_4e8c24;
	players.index = NONE;
	players.datum_index = NONE;
	while (data_datum_iterator_next(&players))
	{
		short player_table_index = NONE;
		for (long p = 0; p < 2; ++p)
			if (g_4f55cc[p].player_index == players.datum_index) { player_table_index = p; break; }
		s_ai_player *player = NULL;
		if (player_table_index != NONE) player = &g_4f55cc[player_table_index];
		short player_slot = NONE;
		for (long i = 0; i < 2; i++)
			if (g_4f55cc[i].player_index == players.datum_index) { player_slot = (short)i; break; }
		real score = 0.0f;
		long threshold = round_tick_count((real)g_510c54->field_2_3 * 2.0f);
		if (player && *(long *)(players.datum + 0x2c) != NONE)
		{
			long unit_index = *(long *)(players.datum + 0x2c);
			if (actor->unknown31c != NONE && actor->unknown31c != player_slot) continue;
			s_slot_object_view *unit = object_get(unit_index);
			if (*(short *)((byte *)unit + 0x1fc) == NONE)
			{
				real approach = 0.0f, facing = 0.0f;
				vector3f forward;
				object_get_forward(unit_index, &forward);
				point3f point;
				function_b9dd0(unit_index, &point);
				vector3f direction;
				direction.i = actor->position.x - point.x;
				direction.j = actor->position.y - point.y;
				direction.k = actor->position.z - point.z;
				real distance = function_30bf0(&direction);
				bool established = actor->unknown31c == player_slot && actor->unknown31e > (short)threshold;
				bool retain = true;
				if (!(distance > 0.0f)) continue;
				bool visible = true;
				long reference = function_25d810(unit_index, actor_index, false);
				vector3f velocity = unit->velocity;
				vector3f relative;
				vector3d_from_points3d((point3f const *)&vehicle->velocity, (point3f const *)&unit->velocity, &relative);
				if (function_30bf0(&velocity) > 0.0f)
					approach = velocity.k * direction.k + velocity.j * direction.j + velocity.i * direction.i;
				if (function_30bf0(&relative) > 0.0f)
				{
					real value = relative.k * direction.k + relative.j * direction.j + relative.i * direction.i;
					if (value > approach) approach = value;
				}
				if (reference != NONE && prop_ref_get(reference)->unknown27 < 1)
					visible = false;
				bool positive = false;
				if (distance < 15.0f)
				{
					if (visible)
					{
						facing = direction.k * forward.k + direction.j * forward.j + direction.i * forward.i;
						if (facing > 0.8660253882408142f) score = (facing - 0.8660253882408142f) * 7.4641008377075195f;
						else if (distance < 1.0f) score = 1.0f;
					}
					if (visible || established)
					{
						if (established)
						{
							if (distance < 2.5f) score += 1.0f;
							else if (!(distance > 3.5f))
							{
								real decrease = distance - 2.5f;
								if (decrease < 0.0f) decrease = 0.0f;
								score += 1.0f - decrease;
							}
							if (approach > 0.5f) score += 1.0f;
						}
						if (established || !(distance > 3.5f && approach < 0.5f)) positive = score > 0.0f;
					}
				}
				if (positive)
				{
					if (actor->unknown31c != NONE)
					{
						if (actor->unknown31c != player_slot) continue;
						if (*(short *)(data + 0x320) > 0) *(short *)(data + 0x320) -= 2;
					}
					else { actor->unknown31e = 0; *(short *)(data + 0x320) = 0; }
					actor->unknown31c = player_slot;
				}
				else
				{
					if (actor->unknown31c != player_slot) continue;
					++*(short *)(data + 0x320);
					if (approach < 0.0f) ++*(short *)(data + 0x320);
					if (*(short *)(data + 0x320) > round_tick_count((real)g_510c54->field_2_3 * 4.0f) || actor->unknown31e < (short)threshold) retain = false;
				}
				if (actor->unknown31c == player_slot)
				{
					++actor->unknown31e;
					if (facing > 0.8660253882408142f && distance < 2.5f) ++actor->unknown31e;
				}
				if (retain) continue;
			}
		}
		if (actor->unknown31c == player_slot)
		{
			actor->unknown31c = NONE;
			actor->unknown31e = 0;
			*(short *)(data + 0x320) = 0;
		}
	}
}

struct s_actor_object_sample;
bool function_28fa60(long perception_index, point3f const *point, s_actor_object_sample *sample);

__declspec(noinline) bool __stdcall function_2641c0(long actor_index, s_2641c0 *sample, point3f const *point);

// @retail 0x2641c0
bool __stdcall function_2641c0(long actor_index, s_2641c0 *sample, point3f const *point)
{
    point3f const *const *point_reference = &point;
    s_actor_view *actor = actor_get(actor_index);
    if (*((byte *)actor + 7) && *(long *)((byte *)actor + 0x1c) != NONE)
        return function_28fa60(*(long *)((byte *)actor + 0x1c), *point_reference, (s_actor_object_sample *)sample);
    memcpy(sample, (byte *)actor + 0x22c, sizeof(*sample));
    return true;
}

// @retail 0x264210
void function_264210(long actor_index, long prop_ref_index)
{
    s_prop_datum *reference = (s_prop_datum *)(g_502418->data + (prop_ref_index & 0xffff) * 0x3c);
    s_2641c0 context;
    if (function_2641c0(actor_index, &context, &function_25d690(reference)->position))
        function_264330(actor_index, prop_ref_index, &context, 0, true);
}
