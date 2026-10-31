// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FC2F0.CPP: a tracked point (an aim or look target) and its
   updates */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "slot_handler.h"
#include "unknown_1cec30.h"
#include "unknown_1946f0.h"
#include "object_default_placement.h"
#include <math.h>

void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
void havok_component_rigid_body_linear_velocity_set(long rigid_body_index, s_havok_component *component, vector3f const *velocity);

static inline void local_vector3d_from_havok(vector3f *vector, hkVector4 const *havok)
{
	vector->i = (*havok)(0);
	vector->j = (*havok)(1);
	vector->k = (*havok)(2);
}

static inline void local_scale3d(vector3f const *vector, real scale, vector3f *result)
{
	result->i = vector->i;
	result->j = vector->j;
	result->k = vector->k;

	result->i *= scale;
	result->j *= scale;
	result->k *= scale;
}

static inline void local_rigid_body_linear_velocity_get(long rigid_body_index, s_havok_component *component, vector3f *velocity)
{
	hkRigidBody *rigid_body = havok_component_rigid_body_get(rigid_body_index, component);

	if (!rigid_body->m_fixed)
	{
		local_vector3d_from_havok(velocity, &rigid_body->m_motion->m_linear_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

/* where a point lies: two indices that the tracking copies together */
struct s_tracking_location
{
	short unknown0;
	short unknown2;
};

struct s_tracked_point
{
	point3f point;
	vector3f offset;
	vector3f velocity;
	s_tracking_location location;
	bool unknown28;
	bool unknown29;
};

// @retail 0x1fc2f0
void function_1fc2f0(s_tracked_point *tracked, point3f const *point, bool unknown)
{
	tracked->offset = *g_4687a4;
	tracked->point = *point;
	tracked->unknown29 = false;
	tracked->velocity = *g_4687a4;
	*(volatile bool *)&tracked->unknown28 = unknown;
	*(volatile short *)&tracked->location.unknown0 = NONE;
	*(volatile short *)&tracked->location.unknown2 = NONE;
}

/* Updates an object's position from Havok state and adjusts its rigid-body linear velocity. */
// @retail 0x1fc350
void function_1fc350(long object_index, s_tracked_point *tracked)
{
	s_havok_object *havok_object = havok_object_get(object_index);
	s_havok_component *component = havok_component_get(havok_object->havok_component_index);

	transform4x3f matrix;
	function_1d4360(component, &matrix);

	s_slot_object_view *object = object_get(object_index);
	if (object->parent_index == NONE)
		function_b75a0(object_index, &matrix.position, NULL, NULL, NULL, false);

	vector3f *velocity = &tracked->velocity;
	vector3f linear_velocity;
	real magnitude = function_30bf0(velocity);

	if (magnitude > 1.0f)
	{
		local_rigid_body_linear_velocity_get(0, component, &linear_velocity);

		real linear_i = linear_velocity.i;
		real linear_j = linear_velocity.j;
		real linear_k = linear_velocity.k;

		real dot =
			((vector3f volatile *)velocity)->k * linear_velocity.k +
			((vector3f volatile *)velocity)->j * linear_velocity.j +
			((vector3f volatile *)velocity)->i * linear_velocity.i;

		if (dot > 1.0f)
		{
			magnitude -= 1.0f;
			if (dot > magnitude)
				dot = magnitude;

			vector3f correction;
			local_scale3d(velocity, dot, &correction);

			linear_velocity.i -= correction.i;
			linear_velocity.j -= correction.j;
			linear_velocity.k -= correction.k;

			havok_component_rigid_body_linear_velocity_set(0, component, &linear_velocity);
		}
	}
}

struct s_tracking_source
{
	byte unknown00[0x28];
	bool unknown28;
};

struct s_tracking_target
{
	byte unknown00[0xe4];
	real unknowne4;
};

struct s_tracking_result
{
	long unknown0;
	long direction;
};

// @retail 0x1fc620
void function_1fc620(s_tracking_result *result, s_tracking_source const *source, s_tracking_target const *target)
{
	if (source->unknown28)
	{
		if (target->unknowne4 < 0.0f)
			result->direction = 0xb;
		else if (target->unknowne4 > 0.0f)
			result->direction = 0xa;
		else
			result->direction = 1;
	}
}

struct s_tracking_state
{
	byte unknown00[0x2c];
	point3f unknown2c;
	point3f unknown38;
	vector3f velocity;
	s_tracking_location location;
	byte unknown54[0xe8 - 0x54];
	vector3f unknowne8;
};

struct s_tracking_output
{
	dword flags;
	byte unknown04[0xc - 0x4];
	vector3f velocity;
	byte unknown18[0x54 - 0x18];
	bool unknown54;
	byte unknown55[3];
	point3f point;
};

// @retail 0x1fc660
void function_1fc660(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	if (tracked->location.unknown0 == state->location.unknown0 && tracked->location.unknown2 == state->location.unknown2)
	{
		vector3f delta;

		vector3d_from_points3d(&state->unknown38, &state->unknown2c, &delta);
		if (length_sq3f(&delta) > 0.1f * 0.1f)
			output->flags |= 4;
	}
	if (tracked->unknown29)
	{
		output->point.x = state->unknowne8.i + tracked->offset.i;
		output->point.y = state->unknowne8.j + tracked->offset.j;
		output->point.z = state->unknowne8.k + tracked->offset.k;
		output->unknown54 = true;
		tracked->unknown29 = false;
	}
}

/* the tracked velocity, its change limited to half a unit per second */
// score 32 (was 43): the limit test is written the other way round, so the comparison matches
// retail's operand order and its branch, but the scheduler computes the components i, j, k where
// retail does j, i, k, so the tick conversion takes xmm4 where retail reuses xmm3, the deltas land
// in other registers and the limit is computed before the squared length. Converting the ticks
// after the rate products gives retail's first 0x5a bytes (j, i, k and xmm3), but then the tick
// products copy the scalar (movaps xmm4, xmm3; mulss xmm4, xmm0) where retail scales in place.
// Tried: rate local with long ticks 64; the components multiplied in one expression 96; the same
// without the tick scale local 64; the components loaded first and multiplied in place 44; the
// chains reordered to j, i, k 44 (same code); the chains as one expression each in that order 105;
// the chains as separate scalar locals in that order 93; the components as separate scalar locals
// 94 and without the velocity struct 106; the limit test the other way round 43 (kept); the tick
// products reordered to j, i, k 43 (same code); the three state loads reordered to j, i, k 47; the
// state velocity read into three locals with the chains in j, i, k 87; the chains as one expression
// each without the separate loads 105 (j, i, k) and 104 (i, j, k); the tick products into a second
// vector 43 (same code); local_scale3d for the tick stage 64; tick product and delta per component
// 64; ticks read first 64; the limit test as !(limit * limit > ...) 56; limit from the rate local
// 68; real ticks at the top 103; real ticks after the rate 104; the same without the parameter
// reference 80; local_scale3d for the rate and tick stages 99 (377 bytes), for the tick stage only
// 43 (same code), for the rate stage only 99; the velocity copied as a struct 132 (394 bytes); the
// rate read from the global at each use 99 (375 bytes); `(real)ticks` at each tick product 64; the
// conversion named after the rate products 64; local_scale3d with `(real)ticks` 64; the conversion
// read from the global after the chains 64; that with the velocity copied as a struct 137; all
// three rate products first, then the conversion, then the adds and subtracts (per component or
// grouped, the conversion inside the i chain) 115/115/115 (387-390 bytes).
// the limit test is written the other way round, so the comparison matches
// retail's operand order and its branch, but the scheduler still computes the components in the
// order i, j, k where retail does j, i, k, so the tick conversion takes xmm4 where retail reuses
// xmm3, the deltas land in other registers and the limit is computed before the squared length.
// rate local with long ticks 64; the components multiplied in one expression 96; the same without
// the tick scale local 64; the components loaded first and multiplied in place 44.
// The three chains reordered to j, i, k 44 (the compiler schedules them itself, the code is
// unchanged); the chains as one expression each in that order 105; the chains as separate scalar
// locals in that order 93; the components as separate scalar locals 94 and without the velocity
// struct 106; the limit test the other way round 43.
// This round: the tick products reordered to j, i, k 43 (the code is unchanged); the three state
// loads reordered to j, i, k 47 (the chain order follows the loads, but the registers move with
// them, so retail's xmm0 = i, xmm1 = j is lost); the state velocity read into three locals and
// the chains written in j, i, k 87; the chains as one expression each without the separate loads
// 105 in that order and 104 in i, j, k (the loads then fold into the products and the code loses
// 18 bytes); the tick products written into a second vector 43 (the code is unchanged). The chain
// order follows the order of the first definitions, so it moves with the loads and cannot be
// changed on its own.
// Earlier: forms that give retail's first 0x5a bytes (rate kept in xmm3, the three components
// loaded first) put the tick products in fresh registers and move the correction's last
// product late, one instruction long: rate local with long ticks 64;
// extends the word into EDI, while ours writes the word with a 16-bit store
// to DI and loads the float into xmm0.
// retail's first 0x5a bytes (rate kept in xmm3, the three components loaded
// first) but the tick products land in fresh registers and the correction's
// last product moves late, one instruction long: rate local with long ticks 64;
// with the tick scale through local_scale3d 64; tick product and delta per
// component 64; ticks read first 64; the limit test as !(limit * limit > ...) 56.
// `long ticks` and a `rate` local give retail's first 21
// instructions (rate in xmm3, movsx edi, components loaded first) but the tick
// products go to new registers where retail scales in place 64 (369 bytes);
// limit from the rate local 68; real ticks at the top 103; real ticks after the
// rate 104; the same without the parameter reference 80.
// Round 32 (lane 16): the state velocity copied as a struct 132 (also with the chains j, i, k);
// the tick conversion declared after the three chains, or natural per-component sums followed by
// `*= ticks` 64/64 (retail's j, i, k chain order and ticks in xmm3, but the tick products go into
// copies of the ticks register, 369 bytes); components loaded first then one expression per
// component times ticks, or each chain followed by its own `*= ticks`, 70/70. A compile-only screen
// shows the coupling: a conversion after the chains gives retail's first 14 SSE ops but copies,
// one before them scales in place but keeps i, j, k.
// Round 95: the existing local_scale3d for the rate and tick stages 99 (377 bytes), for the tick
// stage only 43 (code unchanged), for the rate stage only 99; the velocity copied as a struct 132
// (394 bytes); the rate read from the global at each use 99 (375 bytes).
// Round 37 (lane 16): the tick conversion after the chains gives retail's first 22 instructions
// (j, i, k chains, ticks in xmm3) but the tick products then take the ticks as their left operand
// (copies) 64; the same with the tick stage through local_scale3d, also with (real)ticks passed
// directly, 64/64; conversion at the top with local_scale3d for the tick stage 43 (same code);
// the conversion after each of the first nine chain statements 72 each; the chains written stage
// by stage (all rate products, then all adds, then all subtractions) with the conversion before
// the rate stage/after it/after the adds/after the subtractions 107/115/106/106.
// DL192: Common native velocity publication 61; scaled add 96; in-place scale/add 50; reused squared length
// 77; late tick conversion 45; with native input copy 137, squared length 54, squared length/scaled add 95.
// Late ticks move first difference to +0x5a but enlarge by four bytes; all restored.
// Round 21 (lane 10): the prediction through vector3f_add_scaled + vector3d_from_points3d 43 and the
// delta scaled in place 43 (both identical code); the correction through vector3f_add_scaled 97
// (frame); a named squared length before the limit 79 (lockstep shift: the limit, the early j
// square and the two spill stores then follow retail's schedule; chains still i, j, k and ticks in
// xmm4); that plus tick_scale defined after the chains 73 (ticks reuse xmm3 as retail, but the tick
// products become copies and the chains run k, j, i). No-op products (diagnostic) never move the j chain.
// Round 18 (lane 11): retail converts the ticks into the rate's XMM3 after the rate products, so
// the conversion follows them in the source, yet scales in place. The conversion after the rate
// chains gives retail's first 0x5a bytes (j, i, k chains, cvtsi2ss xmm3) but the tick products
// copy the tick register 64 (369 bytes); the same with inline conversions, local_scale3d,
// vector3f_scale, products written tick first, declared at the top and assigned late 64 each;
// tick products j, i, k 73; one `scale` local for rate then ticks 66. The subtraction fused into
// the tick product (`velocity.i = (velocity.i - p0.x) * tick_scale;`) scales in place in XMM3
// but loses the j-first schedule: 66; with chain/product/delta orders 48/54/71 (best at 366 bytes).
// DL192: Common native velocity publication 61; scaled add 96; in-place scale/add 50; reused squared length
// 77; late tick conversion 45; with native input copy 137, squared length 54, squared length/scaled add 95.
// Late ticks move first difference to +0x5a but enlarge by four bytes; all restored.
// Round 203: plain component loops for prediction/ticks/delta, their three
// pairs and all phases 100/43/80/115/102/80/107; all with a short ordinal 144.
// Tick-only loop is identical code; original body restored. Rate/tick register
// reuse, j-first prediction and limit/delta scheduling remain.
// Round 108 (lane 6): `delta` declared before `velocity` 43 (same code). The rate product and add
// per component, then the tick conversion, then subtract-and-scale per component (`v.c -= p;
// v.c *= tick_scale;`) gives ticks in XMM3 scaled in place 66 (369 bytes, limit still hoisted);
// with the squared length named before the limit 53 (366 bytes: limit after the length as
// retail); written j, i, k throughout 57 (retail's first 0x65 bytes, 369 bytes); j, i, k with the
// deltas assigned i, j, k 49 (the length then sums (k + j) + i as retail: the sum follows the
// order the delta fields are assigned); the chains j, i, k, subtract-and-scale i, j, k and deltas
// i, j, k 47 (366 bytes, first 0x5d bytes as retail; the two tick products then run i, j where
// retail runs j, i but still stores i first; the delta copies and the correction block differ).
// Subtracting all three then scaling all three: worse in every order (compile-only screen).
// Round 20 (lane 15, fast): the prediction chains written into their own `displacement` vector and `velocity`
// first assigned by the tick products: conversion after the chains 64 (retail's first 0x5a bytes, but the ticks
// stay the copied left operand), at the top 43 (same code as now), through local_scale3d 64, `(real)ticks` in
// each product 64; `short ticks` 43; the squared length summed j, k, i (diagnostic) 43, identical code.
// Round 98 (lane 8; LTCG env link with the ballast, equal to the full listing; env rows
// positional/aligned, base 39/46): tick products and deltas in all 6x6 orders with the conversion at
// the top or after the chains: none below 39/46, late conversions 58+ (369 bytes); the state velocity
// copied into its own vector and the chains written j,i,k / i,j,k / j,k,i into the result vector:
// the compiler still schedules i, j, k (identical code, best 39/46); correction added as
// `correction.c + state->velocity.c` 39/46; through the existing vector3f_scale + vector3f_add 39/46;
// local_scale3d + vector3f_add 39/46; delta scaled in place by vector3f_scale then vector3f_add 64/55.
// Round 24 (lane 14, fast), all on round 108's 47 form (components loaded first, rate stages j, i, k,
// subtract-and-scale i, j, k, named squared length): the correction through the existing vector3f_scale 47
// and the delta scaled in place with vector3f_add 47 (both the same code: the products and adds still
// interleave, retail does all three products first); `short ticks` 47 (same code); subtract, scale and
// delta per component i, j, k 48 (aligned 39 -> 33: delta i in XMM4 as retail, the tick products still
// i, j, the delta j copy in XMM5 where retail first moves velocity k into XMM3); the same per component
// j, i, k 58 (369 bytes: retail's j, i tick products, first difference +0x65, but velocity j is stored
// before i). Untried lead: j's subtract and scale, then i's with its delta, then j's delta, then k's.
// Round 23 (lane 15, fast): the correction through the existing vector3f_scale 43 (identical); per
// component `state->velocity.c + delta.c * scale` 97 (351 bytes); a predicted point (j, i, k) then
// `(predicted - p2c) * ticks` 73 (369 bytes: retail's j-first chains and in-place XMM3 ticks, but the
// loads follow j, i, k); round 108's order (rate stage j, i, k; conversion; per-component subtract and
// scale; deltas i, j, k) re-measured 47, with that stage j, i, k 49 (tick products j, i as retail; the
// velocity stores and deltas then run j, i where retail runs i, j), i, k, j 47, k, i, j 62, k, j, i 62,
// j, k, i 49; subtractions i, j, k grouped before the products j, i, k / i, j, k / j, k, i 96/93/97, the
// same with subtractions j, i, k 96/92/97 (375-378 bytes); one to three dead real locals 43 (identical).
// Round 42 (lane 1; minimal SSE compile-only harness, then fast): kept the squared length named once
// for the test and the root with the deltas assigned j, i, k: 32 (first difference +0x59; retail's limit
// and length schedule and its k, j, i sum). What still differs: the tick conversion takes XMM4 (retail
// reuses the rate's XMM3), so the velocity stores run j, i and the k product and delta copies differ;
// the correction products then interleave with their adds. Tried on it: deltas i, k, j 33 (harness
// k, i, j / k, j, i / j, k, i / i, j, k worse); the copy of the state velocity as one struct assignment
// 132 (394 bytes); the squared length named after the limit 46; the tick scale read straight from the
// global 32; the rate chains j, i, k 32 (both identical code). Harness only (no gain over the kept
// form): all chain and tick-product orders, the conversion late, inline or from the global, no rate
// local, `short ticks`, `limit` declared at its assignment, `delta` declared first, and the correction
// through vector3f_scale, vector3f_add, adds written correction-first (all equal), scaled in place or
// through vector3f_add_scaled (worse). Late conversions make the tick products copy the tick register.
// Round 181 (lane 3; ~4000 statement orders screened in a compile-only harness that reproduces this
// body's SSE code): with a named squared length, the rate stage j, k, i, the conversion, then k's
// subtraction, i's subtraction/scale/delta, j's subtraction/scale, k's scale and the j, k deltas gives
// retail's whole prediction block except k's tick product (ours multiplies into the ticks register,
// retail in place in XMM2 and then copies to XMM3): 46 (366 bytes). That order with the delta scaled in
// place j, k, i before the three sums 42 (all products ahead of the sums as retail, but the scale lands
// in XMM0 and the products in its copies). Round 24's untried lead (j subtract/scale, i
// subtract/scale/delta, j delta, k) 50; j and i subtractions first then i and j scales 86; the rate
// stage j, k, i with j's subtract/scale/delta first 52 (369 bytes). Screen only: every order that puts
// k's tick product ahead of j's keeps it in place but swaps the j/k delta registers; the conversion
// read straight from the global, a short or late tick count, a const scale, `v = v * s` and a dot
// product for the length change nothing; vector3f_scale + vector3f_add for the correction give
// retail's scale register (XMM1) and i product but interleave the sums.
// Round 140 (lane 4): the rate products as the components' first assignments, then adds/subtracts and a
// late conversion 105 (381 bytes, the loads fold into the products); the tick count and conversion named
// before the rate 33 (ticks take XMM3 and scale in place, the rate moves to XMM4); deltas assigned i, j, k
// 79 and k, j, i 79 (both screen-only before); a 600 s permuter run, 72 tries, none below 32; `short ticks`
// with the conversion after the chains 73 (369 bytes).
// Round 103 (lane 8): the conversion after the chains with the tick products written `tick_scale * v.c` 73
// (369 bytes); velocity declared after the tick scale 32 (identical); each rate chain as one expression
// `v.c = s.c * rate + p.c - q.c` 107 (345 bytes); the conversion between the velocity copies and the chains
// 72 (369 bytes); the delta through the existing vector3d_from_points3d 77.
// @retail 0x1fc4b0
void function_1fc4b0(s_tracked_point *tracked, s_tracking_state const *state, s_tracking_output *output)
{
	vector3f velocity;
	vector3f delta;
	real limit;

	s_tracked_point *const *tracked_reference = &tracked;
	function_1fc660(*tracked_reference, state, output);
	real rate = g_510c54->rate;
	long ticks = g_510c54->field_2_3;
	real tick_scale = (real)ticks;
	velocity.i = state->velocity.i;
	velocity.j = state->velocity.j;
	velocity.k = state->velocity.k;
	velocity.i *= rate;
	velocity.i += state->unknown38.x;
	velocity.i -= state->unknown2c.x;
	velocity.j *= rate;
	velocity.j += state->unknown38.y;
	velocity.j -= state->unknown2c.y;
	velocity.k *= rate;
	velocity.k += state->unknown38.z;
	velocity.k -= state->unknown2c.z;
	velocity.i *= tick_scale;
	velocity.j *= tick_scale;
	velocity.k *= tick_scale;
	delta.j = velocity.j - state->velocity.j;
	delta.i = velocity.i - state->velocity.i;
	delta.k = velocity.k - state->velocity.k;
	real length_squared = length_sq3f(&delta);
	limit = g_510c54->rate * 0.5f;
	if (!(limit * limit > length_squared) &&
		tracked->location.unknown0 == state->location.unknown0 &&
		tracked->location.unknown2 == state->location.unknown2)
	{
		real scale = limit / ((real)sqrt(length_squared) * g_510c54->rate);

		vector3f correction;
		local_scale3d(&delta, scale, &correction);
		output->velocity.i = state->velocity.i + correction.i;
		output->velocity.j = state->velocity.j + correction.j;
		output->velocity.k = state->velocity.k + correction.k;
	}
	else
	{
		output->velocity = velocity;
	}
	tracked->velocity = state->velocity;
	tracked->location = state->location;
}

struct s_actor_aim_unit
{
	byte unknown000[0x212];
	char field_212;
	byte unknown213[0x218 - 0x213];
	long weapons[4];
};

bool function_101b80(long weapon_index, short barrel_index, point3f *point);
void function_cafc0(long unit_index, point3f *position);

PRIVATE inline bool actor_weapon_position(long unit_index, point3f *position)
{
	s_actor_aim_unit *unit = (s_actor_aim_unit *)object_get(unit_index);
	short slot = unit->field_212;
	bool result = false;

	if (slot != NONE)
	{
		long weapon_index = unit->weapons[slot];
		if (weapon_index != NONE)
			result = function_101b80(weapon_index, 0, position);
	}
	return result;
}

/* Uses a weapon marker when available, otherwise the unit or actor origin. */
// @retail 0x1fc710
void function_1fc710(long actor_index, point3f *position)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown274 != NONE)
	{
		if (!actor_weapon_position(actor->unknown274, position))
			function_cafc0(actor->unknown018, position);
	}
	else if (!actor_weapon_position(actor->unknown018, position))
	{
		*position = *(point3f *)((byte *)actor + 0x22c);
	}
}
