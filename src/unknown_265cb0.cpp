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
	byte unknown00[8];
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

// MiniMax H (11-12): no new form, no lower score (84): the result long still lands at [esp+0xc] where retail
// keeps it at [esp+8], the clip's amount out-parameter sits at [esp+0x10] against retail's [esp+0x1c] and
// retail's range is at [esp+0x30] against our [esp+0x18]; the seat loop's two cross products keep our
// operand order, and the declaration orders the earlier rounds tried do not move these homes.
// MiniMax B (11-11): no new form, no lower score (84); the vector homes are still one slot off from retail's, the range's
// first load and the two products still differ, and the range is still loaded into XMM2 for XMM0; the body is 1310 bytes
// against retail's 1308.
// score 84 (was 84): retail's block layout and registers but for the range's first load and two products, 1310 bytes against 1308, masked 20. Left: stack homes
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
// @retail 0x263810
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range)
{
	long result = 0;
	if (mode == 0 || mode == 1)
	{
		s_actor_view *actor = actor_get(actor_index);
		byte *settings = (byte *)actor_get_character_situation_entry(actor_index);
		if (settings)
		{
			real range = *(real *)(settings + 4);
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
				if (!(*settings & 1))
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

// MiniMax H (11-12): no new form, no lower score (168): the first difference is still the inlined 0x26be00
// lookup's address operand (ours [eax+ecx+0x58], retail [ecx+eax+0x58]), a base/index order the compiler
// picks when the helper is expanded, and that helper's body is in src/unknown_26bda0.cpp, which is not mine;
// the permuter refuses this function because the reduced image and the full image disagree.
// MiniMax B (11-11): no new form, no lower score (168); the first difference is still the table load at +0x5c, ours
// [eax+ecx+0x58] against retail [ecx+eax+0x58], a base/index order the compiler picks; the frame is still 0x7c against
// retail's 0x80 and the normalized direction is still folded instead of being held in XMM1..XMM3 across the dots.
// score 168 (was 168): retail's length (2880). Left, in order: the players iterator's table pointer loads into ECX where retail
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
// Earlier first line: score 194 (was 194): frame 0x80 and the forward/relative blocks now agree; the established and
// visible flags take each other's stack bytes, the facing/scoring block keeps the direction in memory
// where retail keeps it in xmm1-3, and the entry's second actor lookup indexes [eax+ecx] for [ecx+eax].
// Earlier first line: score 502 (was 502): Frame 0x84/0x80 and selection/iterator/scoring homes remain.
// Earlier score 502 (was 502): frame 0x84 versus 0x80, selection/iterator and transform/scoring homes differ.
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
					long other_index = object_get(node->object_index)->actor_index;
					s_actor_view *other = other_index == NONE ? NULL : actor_get(other_index);
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
		for (short i = 0; i < 2; i++)
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
				if (distance < 15.0f && (visible || established))
				{
					if (visible)
					{
						facing = direction.k * forward.k + direction.j * forward.j + direction.i * forward.i;
						if (facing > 0.8660253882408142f) score = (facing - 0.8660253882408142f) * 7.4641008377075195f;
						else if (distance < 1.0f) score = 1.0f;
					}
					if (established)
					{
						if (distance < 2.5f) score += 1.0f;
						else if (distance <= 3.5f)
						{
							real decrease = distance - 2.5f;
							if (decrease < 0.0f) decrease = 0.0f;
							score += 1.0f - decrease;
						}
						if (approach > 0.5f) score += 1.0f;
					}
					if (established || !(distance > 3.5f && approach < 0.5f)) positive = score > 0.0f;
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
