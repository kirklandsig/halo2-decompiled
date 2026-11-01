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

// score 152 (was 152): Frame 0x350 agrees; result/range/projection homes and inverse delta/motion SIMD groups remain.
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
// @retail 0x263810
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range)
{
	volatile long result = 0;
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
				motion.i = endpoint->x - point->x;
				motion.j = endpoint->y - point->y;
				motion.k = endpoint->z - point->z;
				if (motion.k * motion.k + motion.j * motion.j + motion.i * motion.i > 0.002500000176951289f)
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
							delta.i = motion.i * amount + point->x - origin->x;
							delta.j = motion.j * amount + point->y - origin->y;
							delta.k = motion.k * amount + point->z - origin->z;
						}
					}
				}
			}
			real distance_squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
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
					if (!actor->unknown007 && use_facing)
					{
						real facing_secondary, facing_primary;
						function_263740(actor_index, &delta, (vector3f *)((byte *)actor + 0x2a8),
							(vector3f *)((byte *)actor + 0x2b4), (vector3f *)((byte *)actor + 0x2c0),
							range, scale, &facing_secondary, &facing_primary);
						long object_index = *(long *)((byte *)actor + 0x26c);
						if (object_index != NONE)
						{
							s_object_seat seats[64];
							short count = function_c8b80(object_index, seats, 64);
							s_object_seat *seat = seats;
							for (short i = 0; i < count; ++i)
							{
								long occupant = function_c8f60(seat->object_index, seat->seat_index);
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
								++seat;
							}
						}
						secondary = facing_secondary;
						primary = facing_primary;
					}
					else
					{
						primary = scaled_range;
						secondary = scaled_range * 0.7f;
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

// @retail 0x267a80
short function_267a80(real *distance, point3f const *point, vector3f const *direction, point3f const *position, vector3f *unknown)
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

// score 6 (was 6): the tag lookup: retail keeps the index in ecx and the
// table base in eax, ours keeps the index in eax and the base in ecx;
// thirteen source rewrites all scored 6. Retail reloads the pool's data for
// the held object with no store or call in between; every form without the
// volatile pool view shares the first load (28): the held index through
// function_cbd50 28 (6 with the view kept), the held object through
// object_get 28 or object_header_get 28. Tried (r27b): the comparison through a
// real local 12; the tag index through a long local 12; `if (...) result = true`
// 14. 0xee8a0 (matched) needs the same volatile view for its weapon lookup.
// Round 14, no volatile: the existing function_cbd50 for the held index 28 (result
// declared first 40); the existing unit_get_current_weapon 28; the fields at each use
// 27; the object pointer for the held slot with the slot read through a second
// lookup 4 (retail's reload; only the entry's data register and the slot's sign
// extension differ). Two held lookups reload it data-first (15, 142 bytes).
// Round 38, no volatile: the 0x267370 form (held index through function_cbd50, the
// definition read inside the compare) 31; a pool pointer local for both lookups 27; the
// threshold entries selected (NULL when empty) before the held lookup 30 (142 bytes); the
// entry object through object_header_get's header 28; an unused header lookup in the entry
// block (diagnostic) gives retail's entry block but a data-first held reload 15 (142 bytes).
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
		s_equipped_object_view *held = (s_equipped_object_view *)((s_object_header_view *)((s_record_pool volatile *)g_4e0300)->data)[held_index & 0xffff].object;
		byte *definition = g_4e3b44[held->tag_index & 0xffff].bytes;
		result = thresholds->object_threshold <= *(real *)(definition + 0x238);
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

// score 14 (was 14): the only difference left is the order of three 12-byte stack homes: retail stacks
// movement, offset, ahead, target_direction and direction upward from esp+0x34, ours movement, offset,
// direction, ahead and target_direction, and the lea/xor order at the three 0x267a80 calls follows it.
// score 14 (was 14): the direction and ahead homes are swapped: retail stacks movement, offset,
// ahead, target_direction and direction, ours movement, offset, direction, ahead and
// target_direction. Declaration order, scope, type, initialiser and extra source references all
// leave the homes as they are.
// score 14 (was 14): only three vector homes differ: retail stacks movement, offset, ahead,
// target_direction, direction upward from esp+0x34; ours movement, offset, direction, ahead,
// target_direction (12-byte slots; names and declaration places do not move them).
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
// @retail 0x267c50
void function_267c50(long actor_index)
{
	long const *actor_reference = &actor_index;
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
			function_26be00(*actor_reference, &iterator);
			s_prop_datum *node;
			while ((node = (s_prop_datum *)function_26be30(&iterator)) != NULL)
			{
				s_type_5cfb45 *state = function_25d690(node);
				if (!prop_get(node->prop_index)->unknown23 && !function_25d690(node)->unknown5e &&
					!prop_get(node->prop_index)->unknown22 &&
					(prop_get(node->prop_index)->unknown25 || state->unknown3c == NONE))
				{
					vector3f direction;
					if (function_267840(*actor_reference, iterator.index, &direction))
					{
						bool active = false;
						vector3f offset = {0.0f, 0.0f, 0.0f};
						short proximity = function_267a80(NULL, &state->position, &direction,
							(point3f *)(data + 0x238), &offset);
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
									short projected = function_267a80(NULL, &state->position, &direction, &ahead, 0);
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
					if (check_target && function_267a80(NULL, (point3f *)(data + 0x238), &target_direction, &state->position, 0) >= 2)
						*(bool *)(data + 0x3df) = true;
				}
			}
		}
		if (moving_toward)
		{
			function_262800(*actor_reference, *(s_reference *)(data + 0x418), true);
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

// score 262 (was 502): frame 0x80 and the forward/relative blocks now agree; the established and
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
// @retail 0x266640
void function_266640(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long strongest_node = NONE, best_node = NONE;
	byte *data = (byte *)actor;
	memset(data + 0x324, 0, 0x10);
	short best_level = 0;
	real strongest = 0.0f;
	s_actor_prop_iterator iterator;
	iterator.next = actor_get(actor_index)->first_prop_index;
	s_prop_node_view *entry;
	while ((entry = actor_next_prop(&iterator)) != NULL)
	{
		s_prop_datum *node = (s_prop_datum *)entry;
		long index = iterator.index;
		s_type_f95cd3 *view = NULL;
		if (node->tracking_index != NONE)
		{
			s_type_e5ff81 *tracking = tracking_get(node->tracking_index);
			if (tracking) view = &tracking->view;
		}
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
				if (distance < 15.0f)
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
					positive = (established || visible && !(distance > 3.5f && approach < 0.5f)) && score > 0.0f;
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
