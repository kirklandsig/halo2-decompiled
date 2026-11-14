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
#include "vector_products.h"
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
// MiniMax B (11-13): no new form, no lower score (32): retail's tick products run j, i, k while its velocity stores run i, j, k, and the inlined scaled-difference helper keeps the two together (the deltas written i, j, k give retail's stores and its (k + j) + i sum but move the products to i, j, k, 79); the deltas in the order k, i, j measure 30 and the copy-then-subtract form 29, but both are statement orders picked by score and were reverted by the coordinator on 11-09, and the permuter (fast, 7 tries in 390 s) found nothing below the 32 differing instructions of the reduced image, so the body is the original one.
// score 32 (was 32): frame 0x10 and the prediction prefix agree; first difference at +0x65: the velocity stores
// and the deltas run j, i where retail runs i, j, and the correction products and adds differ.
// MiniMax G (11-12): no new form, no lower score (32); the deltas written i, j, k give retail's velocity stores (i, j, k)
// and its (k + j) + i sum, but the scaled difference then runs i, j, k with them (79), so the helper's component order and
// the store order stay coupled; retail's tick products run j, i, k with its stores i, j, k.
// MiniMax G (11-11): no new form, no lower score (32); retail takes the scaled difference in the order j, i, k but its
// deltas in the order i, j, k, and one source order cannot give both: writing the deltas i, j, k (79) moves the scaled
// difference to i, j, k with them, so the difference stays inside the inlined helper and its component order.
// MiniMax D (11-09): no new form, no lower score (32); the permuter run was killed with no candidate.
// MiniMax R (11-09): the squares written out by hand in the three orders give 29 with the same code, and the store order whose
// operations match retail's still shuffles the registers against it, so the difference stays inside the inlined length helper.
// MiniMax R (11-08): the deltas assigned i, j, k 79, i, k, j 55, j, k, i 52, k, j, i 72 and k, i, j 30; the delta
// copied i, k, j and then subtracted k, i, j 29 (366 bytes; not kept: an order picked by score, reverted by the coordinator 11-09), the other thirty-five copy and subtract orders 32 to
// 79; the tick stage written out per component with the deltas i, j, k 55, and the prediction written out per component
// in j, i, k 63 (369 bytes). What still differs: retail stores the velocity i, j, k, takes the squares j, k, i and sums
// them as (k + j) + i, while here the stores and the squares run j, i, k and the sum is (j + k) + i.
// MiniMax H (11-06): no new form, no lower score; the first difference is at +0x59, retail converting the ticks
// into xmm3 (the register the rate arrived in) where ours converts them into xmm4, and retail's tick products
// run j, i, k while its deltas run i, j, k; here the chain order follows the first delta's component and the
// delta order follows the tick products' order, so that combination has no source form: the deltas i, j, k 79,
// the chain statements j, i, k with the deltas i, j, k 73, the same with the tick products j, i, k 77, a
// separate scaled vector 83 and 77 with the chains j, i, k, the tick products j, i, k alone 32 (same code), the
// tick conversion after the chains 73, the tick conversion and the deltas interleaved 79, and the conversion
// written at each product 73. Putting the conversion after the chains does put the ticks in xmm3 as retail has
// them, but the products then copy that register instead of scaling in place, and the conversion at the top of
// the body scales in place but takes xmm4.
// MiniMax D (run 46): the rate chains j, i, k with the tick products and the deltas interleaved
// i, j, k 73 (chains, stores, deltas and the k, j, i sum are then retail's, but the tick products
// run i, j and the tick stays in XMM4); the products grouped 77; the conversion after the chains 73
// (XMM3 as retail, but the chains run k, j, i and the products copy the tick register); the velocity
// copied before the subtractions 34; the deltas i, j, k 79.
// MiniMax D (11-07): the prediction runs through the existing vector3f_add_scaled into a predicted point and
// the ticks through the existing scaled_difference3f (the same arithmetic as the hand-written chains): 32, with
// the tick conversion in XMM3 as retail's. Not kept: the deltas reordered to k, i, j for 30 (an order picked
// by score; i, j, k with the helpers 79).
// Earlier: the limit test is written the other way round, so the comparison matches
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
// Lane 18 round 17 (an LTCG link of this file's generated unit with its callee closure, equal to the full listing,
// 33 there for 32): new, all identical code: `correction` declared with the other locals 33; a named `limit_squared`
// 33; the root named before the scale 33. Re-measured listed forms (same results as listed): the delta scaled in
// place then summed 33; tick products written `tick_scale * v` with the conversion at the top 33; dot3f for the
// length 33; deltas k, j, i 79 (359 bytes) and i, k, j 34. No untried natural form is left here.
// Round 105 (lane 7, fast): the rate read from the global at each chain 97 (375 bytes); the conversion after the
// chains (ticks read at the top) 73 (369 bytes); the conversion read straight from the global 32 (identical); the
// rate products and adds j, i, k, then the conversion, the subtractions, the tick products and the deltas as
// i, j, k / i, j, k / j, i, k 93 (378 bytes) and k, j, i / i, j, k / i, j, k 76 (369 bytes).
// Round 46 (m3 lane 10, fast): the prediction through the existing vector3f_add_scaled into a predicted
// point, then the existing scaled_difference3f(p2c, predicted, (real)ticks, velocity) 32 (the tick conversion
// then reuses the rate's XMM3 and scales in place as retail, first difference +0x65; the velocity stores and
// deltas still run j, i where retail runs i, j); on it the deltas assigned i, j, k 79 (the stores, deltas and
// the (k + j) + i sum are then retail's; only the rate chains and tick products run i, j where retail runs
// j, i), i, k, j 55, k, i, j 30, j, k, i 52, k, j, i 72; the delta through scaled_difference3f with 1.0f 83;
// vector3d_from_points3d then vector3f_scale for the tick stage 73 (369 bytes). On the k, i, j form: a 600 s
// permuter run (77 tries) none below 30; predicted declared at the top 30; the limit before the squared
// length 38; the delta through vector3d_from_points3d 80; the correction through vector3f_scale +
// vector3f_add 58 (369 bytes), through local_scale3d + vector3f_add 70, through vector3f_add_scaled 87 (348
// bytes); a vector3f predicted 30; the tick count passed without the (real) cast 30. The squared length sums
// (c + b) + a for deltas assigned a, b, c, so retail's (k + j) + i needs i, j, k, which moves the rate chains
// off retail's j, i, k.
// Round 59 (lane 16, fast): deltas assigned k, i, j 34; the delta as an aggregate initialiser 79; point
// pointer locals for the two state points 35; `real tick_scale = g_510c54->field_2_3;` 32 (identical); a
// predicted point (copied first, `*= rate`, `+= p38`) then the existing scaled_difference3f(p2c, predicted,
// ticks, velocity) 57 (369 bytes: the tick conversion then takes XMM3 and the j/i tick products scale in
// place as retail, first difference +0x65, but the i/j stores, the k product and the correction differ);
// that with the delta through scaled_difference3f(state velocity, velocity, 1.0f, delta) 59 (366 bytes; the
// correction then forms all three products before the load-first adds as retail), with `(real)ticks` 56.
// Round 48 (m3 lane 11, fast): every stage through the existing helpers (vector3f_add_scaled into a predicted
// point, vector3d_from_points3d, vector3f_scale by the ticks, vector3d_from_points3d for the delta, the delta
// scaled in place then vector3f_add) 57: retail's whole store/delta/sum shape, colours rotated as with the
// 1.0f scaled_difference3f delta (ticks and k XMM4, i XMM2, j XMM3 where retail has XMM3/XMM4/XMM2); that
// with the 1.0f scaled_difference3f delta 57; the delta copied per component then subtracted in place, k, i, j,
// 34; the predicted point with a `long ticks` local and deltas k, i, j 30; that with the delta copied i, k, j
// then subtracted k, i, j 29 (366 bytes). Screen only: local names, declaration order, ticks
// or rate locals, dot3f for the length, limit first, a separate correction vector, no parameter reference, and
// (diagnostics) no else arm, no location test or no tail never move the helper form's colours (ticks stay in
// XMM4); `delta = velocity;` then in-place subtractions 82-90.
// Round 61 (lane 16, fast): on round 59's scaled_difference3f form (ticks in XMM3 scaled in place, 57) the
// deltas assigned i, j, k 53 (the velocity stores i then j and the k product placed as retail, but the chains
// then run i, j, k), i, k, j 57, j, k, i 52, k, i, j 58, k, j, i 72, through vector3d_from_points3d 80: the chain
// order follows the delta order, while retail has j-first chains with an i, j, k delta (its k, j, i sum). A
// compile-only screen of ~1150 forms (rate stage per component/staged/copied first/as a struct in all six
// orders, the conversion inline/at the top/before the call, a separate predicted point, three declaration
// orders, dot3f, seven correction forms) found none closer than 3 aligned below the kept form; `sqrtf` for
// the root changes nothing.
// Round 128 (m3 lane 6, fast; compile-only harness positional counts track the build at +4): the deltas through
// the existing scaled_difference3f(state velocity, velocity, 1.0f, delta) on this body 57 (the stores, deltas and
// (k + j) + i sum are then retail's instruction for instruction, only rotated: conversion/k delta XMM4, i delta XMM2,
// j delta XMM3 where retail has XMM3/XMM4/XMM2; the correction then multiplies all three in place and is one
// instruction short, so the listing shifts); that with the correction through vector3f_scale + vector3f_add 54;
// the f46 prediction (vector3f_add_scaled, scaled_difference3f with the ticks) with deltas k, i, j and the
// correction adds j, i, k 30 (two points, not kept), with deltas j, i, k 32; all helpers (that prediction,
// vector3d_from_points3d delta, vector3f_scale + vector3f_add) 80. Harness only: renamed locals, one to three dead
// locals, declaration orders, short/inline/early conversion on the scaled_difference3f delta all identical; rate
// products staged before the conversion with that delta 51+ positional; ~450 random prediction/delta/correction
// combinations none below the round 46 form.
// Round 24 (lane 17, fast): diagnostic dead temps 1-7 on this body 32 each (identical code: the temp count that
// sets SSE operand orders elsewhere does not reach this body). Round 181's 46 form rebuilt (rate stage j, k, i,
// conversion, k subtraction, i subtract/scale/delta, j subtract/scale, k scale, deltas j, k): 46 at +0x78, and its
// whole difference is k's tick product (ours multiplies into the dying tick register, retail multiplies v.k in place
// and copies it to XMM3) plus the correction block (retail: scale in XMM1, j/k scaled in place, all three products
// before the load-first adds); on it dead temps 0-7 46 each; k's product written `tick_scale * velocity.k` 46
// (canonicalised); the conversion after k's subtraction 64 (369 bytes); the correction through the existing
// vector3f_scale + vector3f_add 70.
// Round 48 (m3 lane 11, fast): every stage through the existing helpers (vector3f_add_scaled into a predicted
// point, vector3d_from_points3d, vector3f_scale by the ticks, vector3d_from_points3d for the delta, the delta
// scaled in place then vector3f_add) 57: retail's whole store/delta/sum shape, colours rotated as with the
// 1.0f scaled_difference3f delta (ticks and k XMM4, i XMM2, j XMM3 where retail has XMM3/XMM4/XMM2); that
// with the 1.0f scaled_difference3f delta 57; the delta copied per component then subtracted in place, k, i, j,
// 34; the predicted point with a `long ticks` local and deltas k, i, j 30; that with the delta copied i, k, j
// then subtracted k, i, j 29 (366 bytes). Screen only: local names, declaration order, ticks
// or rate locals, dot3f for the length, limit first, a separate correction vector, no parameter reference, and
// (diagnostics) no else arm, no location test or no tail never move the helper form's colours (ticks stay in
// XMM4); `delta = velocity;` then in-place subtractions 82-90.
// Round 151 (lane 4, fast): the squared length written out `k*k + j*j + i*i` 32 (same code); the correction
// products written `scale * d.i`, `d.j * scale`, `d.k * scale` (retail's copy-then-in-place pattern) 32 (same
// code: the products still sink into the sums); that with the delta through the existing scaled_difference3f
// (state velocity, velocity, 1.0f) 57 (the i product stays in place, folded into its sum); the tick products
// written `tick_scale * v.c` with the conversion at the top 32 (same code); each component's state copy placed
// at the head of its own chain 46 (first difference +0x20); retail's tick and delta order (tick j, tick i,
// delta i, delta j, tick k, delta k) after the chains 83 (first difference +0x2f).
// Round 61 (lane 16, fast): on round 59's scaled_difference3f form (ticks in XMM3 scaled in place, 57) the
// deltas assigned i, j, k 53 (the velocity stores i then j and the k product placed as retail, but the chains
// then run i, j, k), i, k, j 57, j, k, i 52, k, i, j 58, k, j, i 72, through vector3d_from_points3d 80: the chain
// order follows the delta order, while retail has j-first chains with an i, j, k delta (its k, j, i sum). A
// compile-only screen of ~1150 forms (rate stage per component/staged/copied first/as a struct in all six
// orders, the conversion inline/at the top/before the call, a separate predicted point, three declaration
// orders, dot3f, seven correction forms) found none closer than 3 aligned below the kept form; `sqrtf` for
// the root changes nothing.
// Round 247: Existing shared scaler native/scalar input capture crossed with complete scalar/native
// output publication: 146/32/32/145/146/145/189/32; original body and helpers restored.
// Round 249: Native current/previous/both source-position copies 127/127/162; coordinate
// initializers current/previous 97/93, mixed 147/147, both 114. Original source observations and
// body restored; tick/rate register reuse remains.
// DH269: On the named-length graph, native output/local/common base accumulations
// 146/137/110; the corresponding shared-add forms 130/137/110; complete native result
// and common-expression publication 50/50. All regress; original body, includes,
// parameter storage, length lifetime, delta order and correction producer restored.
// Round 461: Final tracked velocity/location native/memcpy/memmove pairs, excluding both
// native, with direct tracked parameter: 99/102/99/99/102/100/100/109.
// Original complete transfers, prediction/correction graph and parameter storage restored.
// Round 508: Existing complete vector setter at delta/tick/input producers, seven subsets,
// then all with correction publication, without the own-parameter address read:
// 104/99/104/99/104/99/104/102. Original body restored; tick/delta/correction homes remain.
// Round 513: Complete overlap-safe transfers at initial velocity, fallback output and
// history velocity, seven subsets, then all with location commit, without the own-parameter
// address read: 162/107/100/158/157/105/153/170. Original body and includes restored.
// Round 309: Without the own-parameter address read, indexed correction accumulation,
// location-first admission and word-order subsets 1..7: 99/100/100/99/99/100/100;
// all with short ordinal 121. Original prediction, parameter storage, helper and complete body restored; tick/delta/correction allocation remains.
// Round 74 (lane 9, fast): rate product and add i, j, k, then the conversion, then `v = (v - p) * ticks` i, j,
// k, deltas k, j, i 72 (first +0x65); as two statements, scale j, i, k, the same deltas 72; chains and scale
// j, i, k, deltas i, j, k 49 (ticks in XMM3 scaled in place; left: the stores run j, i against retail's i, j,
// then the delta homes and the correction); that with the correction scaled in place 49; written in the
// outputs 99 (348 bytes); chains j, i, k, scale i, k, j, deltas k, j, i 72; chains j, i, k, scale i, j, k,
// deltas j, i, k, ticks at the top, in place 32 (same code); fused i, j, k, deltas j, i, k, ticks at the top
// 32 (same code). Compile-only, about 2,800 random orders of loads, chains, conversion, products, deltas,
// scalar or struct chains and five correction shapes: none closer than the current form on the whole SSE
// sequence.
// P271: Consistent prediction/limit/normalization rate observation on the named-length
// graph, existing rate/tick/delta producer masks 0..7, direct update parameter:
// 100/105/100/105/100/97/100/97. Original body and helpers restored; tick/delta/correction homes remain.
// Round 541: Existing scaler pointer/reference result contracts, discarded/scalar/shared-add/native-local
// consumers, with direct update argument: 99/99/99/148/99/99/99/148. Original helper and
// body restored. Two alias-sensitive output-first forms 150/150 excluded; no retained change.
// Round 62 (m3 lane 10): correction adds `correction.c + state->velocity.c` 32; limit before the deltas
// 46; the limit test inverted with its arms swapped 32; tick scale straight from the short field 32;
// correction products inline in the output sums 101 (348 bytes). Original body restored.
// Round 124 (m3 lane 8, fast): a game-time globals pointer local for the rate, ticks, limit and root 32
// (same code); a pointer local for the state velocity at the copies and deltas 32; delta declared before
// velocity 32; limit declared at its assignment 32; the correction written `s.c + d.c * scale` per row 101
// (348 bytes), with retail's i row product-first 101.
// m3 lane 11 r62 (fast): `real const` tick scale 32 and `real const` rate 32 (same code); the root's
// divisor written rate first 32 (same code); the fallback publication per component 89 (352 bytes); the
// if/else inverted with the fallback arm first 32 (same code).
// Lane 18 round 36 (fast): tick products `velocity.c *= ticks` with the long count converted implicitly
// (no tick_scale local) 73 (369 bytes); the three state-velocity copies before the rate and tick reads 94 (370
// bytes); tick_scale declared bare and assigned after the count read 32 (same code); the call through `tracked`
// without the parameter reference 99 (+0x2); the rate stage as `v.c = rate * v.c + p38.c; v.c -= p2c.c` 32 (same code).
// m3 lane 14 r56 (fast): deltas assigned i, j, k 79 (first difference +0x2f: the chains follow); tick products i, j,
// deltas i, j, then the k product and delta 79; the rate read from the global at each chain use 97 (375 bytes);
// the delta scaled in place for the correction 32 (same code); the tick conversion placed after the chains 73
// (369 bytes). Retail converts the ticks into the dead rate register (XMM3), ours into XMM4.
// m3 lane 11 r62 (fast): `real const` tick scale 32 and `real const` rate 32 (same code); the root's
// divisor written rate first 32 (same code); the fallback publication per component 89 (352 bytes); the
// if/else inverted with the fallback arm first 32 (same code).
// Round 85 (m3 lane 9, fast): chains j, i, k, scale j, i, k, deltas i, j, k 77; tick products in named values stored i, j 73; chains j, i, k then scale
// and delta row by row i, j, k 73 (stores i, j and sum k, j, i as retail, ticks still XMM4); scale j, i, deltas i, j, then the k row 77; the correction
// as `correction.i = delta.i * scale; delta.j *= scale; delta.k *= scale` 84 (368 bytes), on the row form 47 (delta.i spills, frame 0x1c). Rate and add,
// the conversion, then rows (sub, scale, delta) i, j, k: rate i, j, k 54; j, k, i 49; j, i, k 48 (ticks in XMM3 as retail, first +0x5d). Rate i, j, k, the
// conversion, subs, scales, deltas i, j, k 99 (378 bytes); rate j, i, k, subs j, i, k, rows 92 (378 bytes); the round-181 order 46, with the correction
// scaled in the delta 46; rate i, j, k rows with that correction 54.
// DL253: one plain tick/delta component loop with long/dword counters 59/59;
// short/word counters 140/140 (403 bytes); full prediction/tick/delta loop
// with long/dword counters 98/98 (374 bytes), short/word 96/96 (366 bytes).
// NOT kept: all regress; counter-width selections require equality. Tick reuse still differs.
// DL255: Ordinary inline update-provider metadata, complete rate/tick/delta producer
// masks 0..7: 102/117/102/117/94/105/94/105. Tracked-pointer result with original/direct
// argument storage: 32/99. NOT kept: metadata changes ingress and regresses; result
// contract ties or regresses. Original complete body, providers and caller declarations restored.
// DL257: Exact widened rate/tick/rounded-length carriers (rate, ticks, length, rate+ticks,
// rate+length, ticks+length, all), then all plus rounded correction scale:
// 98/127/91/106/109/121/108/108. NOT kept: no gain; width/precision selections require native
// shape or equality. Original complete body, real carriers, helper and parameter storage restored;
// dying-rate/tick register reuse remains.
// P309: Existing scaler native-copy/library-copy/two-loop/fused-loop, ordinary/forced, with
// direct update argument: 149/149/99/99/149/149/99/99. Copy/fused loops also lose its matched
// user. NOT kept: larger frame/length and no exact target; original helper and full body restored.
// M347: Limit-admission outcome, direct bool/byte/short/long then
// test-produced bool/byte/short/long: 32/32/32/32/32/32/32/32.
// NOT kept: one-use/test-produced comparison outcomes do not match; original complete body restored.
// DH333: Standard native-array prediction/tick/delta transforms, seven subsets, then
// complete graph plus correction addition, with direct tracked argument: 200/178/238/136/234/192/243/225.
// NOT kept: regressing frame/code growth; original prediction, parameter storage and full body restored.
// P327: Complete scalar prediction on the named-length graph, six declaration orders,
// then producer definitions with original/late ticks, direct update argument: 117/117/117/117/117/117/117/117.
// NOT kept: all regress and lose native frame/extent; original body and parameter storage restored.
// Round 87 (m3 lane 16, fast): tick_scale declared after the chains (ticks in XMM3 as retail, but the products copy it: `movaps xmm4, xmm3; mulss
// xmm4, xmm0`), tick products i, j, k 73 (369 bytes); products j, i, k 72 (369); products j, i, k with deltas i, j, k 72 (369); products and deltas i, j, k 73
// (369); fused product and delta rows i, j, k 73 (369), rows j, i, k 72 (369); `v = v * t` 73 (369); `v = t * v` 73 (369). Original body restored.
// Lane 13 r62 (fast; rate rows into a `predicted` point, then scaled_difference3f with `(real)ticks`, which gives retail's XMM3
// and in-place products): predicted through vector3f_add_scaled, deltas i, j, k 79 (re-measured), deltas j, i, k 32 (re-measured,
// first +0x65, chains and products agree with retail, the stores and deltas run j, i); predicted by statements j, i, k with deltas
// i, j, k 63 (369 bytes), k, j, i with deltas i, j, k 78, j, i, k with deltas k, i, j 78. Rate rows follow the delta order in every
// form, so retail's chains j, i, k with stores and deltas i, j, k stay unreached.
// M3 lane 9 r88 (fast): rate rows j,i,k or i,j,k crossed with tick+delta rows i,j,k or j,i,k, as statements or one expression, conversion after the rate rows: 74 74 78 78 73 73 72 72 (369 bytes: direct field
// references give the copied tick product). Prediction through a pointer helper with rows j,i,k or i,j,k, then scaled_difference3f with `(real)ticks`, deltas i,j,k or j,i,k: 61 81 79 32 (366, in-place tick products);
// j,i,k helper + i,j,k deltas swaps XMM0/XMM1 (loads follow the helper's rows). Length as dot3f, k-j-i, j-k-i, accumulated k first, accumulated j first: 79 79 79 79 55 on i,j,k, 61 61 61 61 82 on j,i,k.
// Retail loads i, j, k into XMM0-2 but prints the j chain first with stores and deltas i, j, k; nothing here separates the chain order from the delta order.
// B452: Existing add-scaled/in-place scaled-difference prediction, with delta/correction/
// displacement-storage masks 0-7: 32/90/60/80/88/101/64/84. NOT kept: tick reuse and
// correction scheduling remain; original parameter storage, helpers and entire body restored.
// Round 137 (m3 lane 8, fast): rate chains and tick products j, i, k with deltas i, j, k and the squared length written
// `k*k + j*j + i*i`, conversion at the top 77, after the chains 72 (369 bytes); the round 46 prediction (vector3f_add_scaled,
// then scaled_difference3f with `(real)ticks`) with deltas i, j, k and that written-out length 79 (first +0x2f), with deltas
// k, i, j 30 and j, i, k 32 (both the same code as length_sq3f): MSVC orders the summed squares by the deltas' definition
// order whatever the expression's order, so retail's (k + j) + i sum implies deltas defined i, j, k.
// Round 76 (m3 lane 10, fast with 0x1fc660): on the round 46 prediction form (vector3f_add_scaled into a predicted
// point, scaled_difference3f with the ticks; XMM3 in place, first +0x65; re-measured 32): the length as dot3f 32, a
// `long ticks` local converted at the call 32, `delta` declared before `velocity` 32 (all the same code: the sum is
// reassociated to (k + i) + j for deltas j, i, k). On this body: the tick products written `tick_scale * v` 32 (same
// code); the else arm's copy per component 89 (352 bytes).
// Round 57 (m3 lane 2, fast with 0x1fc660): the fallback copy as memcpy 32 (same code); on round 46's helper prediction
// (vector3f_add_scaled into a predicted point, scaled_difference3f with the ticks, deltas k, i, j; re-measured 30): the
// prediction computed in place in velocity (no predicted point) 58 (369 bytes); the correction sums written correction
// first 30 (same code); rate and tick scale named before the helpers 61 (the ticks leave XMM3 again); the predicted point
// copied from +0x38 and scale-added in place 126 (395 bytes). Diagnostic, reverted at once: the limit's rate read through a
// volatile field cast 32 (ticks still XMM4: the rate local's live range is not what holds XMM3; the conversion's place is).
// Round 140 (m3 lane 7, fast with 0x1fc660): new: the delta through vector3d_from_points3d together with the
// correction through vector3f_add_scaled 78 (352 bytes). Re-measured (listed): the delta through vector3d_from_points3d
// 80 (stores, deltas and the (k + j) + i sum as retail, but the rate chains then run i first and the ticks leave XMM3);
// the delta as a plain component loop 80 (round 203); the correction through vector3f_add_scaled 97 (348 bytes); the
// velocity as vector3d_from_points3d then an in-place scale by the ticks (local_scale3d), deltas j, i, k or i, j, k
// 73/73 (369 bytes; listed with vector3f_scale).
// Round 100 (m3 lane 12, fast with 0x1fc660): the squared length through the existing k, j, i statement-sum helper of reciprocal_normalize.h 78
// (first +0x2f); that with the deltas through scaled_difference3f at 1.0f 82, with deltas i, j, k and the existing in-place scale helper plus
// vector3f_add for the correction 77, with both helpers and the 1.0f delta 80; the in-place scale helper and vector3f_add alone 57 (369 bytes).
// All restored.
// Round 78 (m3 lane 13): the compile-only /Gr harness reproduces this float block exactly (only the parameter registers
// differ). Scored against retail's 54-instruction block from the tick conversion to the correction stores (aligned misses,
// base 36): deltas i,j,k 33, i,k,j 33, j,k,i 35, k,i,j 32, k,j,i 36; delta copied then subtracted i,j,k / j,i,k 46/46;
// vector3d_from_points3d delta 36. All are listed families (none new); the chain order still follows the delta order.
// Idiom scan of every matched retail function for retail's `mulss a, t; mulss b, t; movss [m], b; subss b, ...` (the
// second product stored and subtracted first): no hit outside this function, so no matched source shows the form.
// Harness, with normalize3d.h added: the correction through vector3f_scale + vector3f_add with deltas i,j,k / j,i,k
// 36/37, vector3d_from_points3d delta with them 36. Fast, on this body (those forms were listed on the 39/43/47 bodies
// only): correction through the existing vector3f_scale with the three adds 32 (same code); vector3f_scale +
// vector3f_add 57 (369 bytes); local_scale3d + vector3f_add 57; the delta scaled in place by vector3f_scale then
// vector3f_add 57; the adds written correction first 32 (same code).
// Round 165 (m3 lane 6, fast with 0x1fc660): the output rows written j, i, k 32 (masked 13 -> 18) and k, j, i 32 (masked
// 21); the correction added through vector3f_add(&state->velocity, &correction, ...) 57 (369 bytes, listed) and with the
// operands swapped 57 (same code); the delta scaled straight into the output, then `+= state->velocity` per component
// 101 (384 bytes); the scale's divisor through the rate local 82 (372 bytes). Retail's deltas end in XMM4/XMM2/XMM3
// (i/j/k) where ours end in XMM4/XMM3/XMM2: the j and k homes are swapped.
// M3 lane 10 r88 (fast with 0x1fc660, 0x1fc350, 0x1fc2f0; base 32, aligned 29, masked 13): the squared length written out
// `j*j + k*k + i*i` on these deltas 32 (same code); `i*i + (j*j + k*k)` 78 (first +0x2f: the grouping moves the chain order); deltas
// defined j, k, i with `i*i + j*j + k*k` 52 (+0x20), k, j, i 72; deltas i, j, k (re-measured 79) with `j*j + k*k + i*i`, `(k*k + j*j) + i*i`
// or `i*i + (k*k + j*j)` 79 each (same code). Retail squares j first, then k, adds, then i, with the chains j, i, k and the
// stores i, j, k.
// Lane 12 r104 (fast with the file's functions, control 32): the deltas through scaled_difference3f at 1.0f with the
// correction through the existing vector3f_add_scaled 78 (352 bytes); the deltas i, j, k with that correction 99 (348);
// the call given `tracked` with the one read through the parameter's address moved to the location test 32 (same code);
// the tick count read after the prediction helper 32 (same code); the root named before the scale 46 on this body
// (listed as 33 on an earlier one). Re-measured as listed: the delta as an aggregate initialiser 79; limit declared
// at its assignment 32. The squared length through reciprocal_normalize.h's k, i, j statement-sum helper 32 (same code).
// Round 169 (m3 lane 6, fast with the file's five matched functions; masked = aligned count with registers and stack
// offsets masked, base 11): a 600 s permuter run (60 tries) reached 29 only through a volatile read of the rate (not used).
// The squared length accumulated k, j, i on the j, i, k deltas 78 (the stores, deltas and (k + j) + i sum as retail, but
// the prediction chains then run i, j, k); that with deltas i, j, k 79, j, k, i 54 (masked 4), k, j, i 54, i, k, j 79,
// k, i, j 76; `k*k + j*j` then `+= i*i` 78, `j*j + k*k` then `+= i*i` 78, `k*k` then `+= j*j + i*i` 32; accumulated
// j, k, i on the j, i, k deltas 30 (chains as retail, stores j, i), with `s = k*k + s` for the second term 30, with deltas
// k, i, j 29, i, j, k 55, i, k, j 55, j, k, i 55 (369 bytes), k, j, i 55 (369 bytes). Retail's chain order and its store
// order come from different sum forms here; no form gives both. On the 29 form: the first output row written
// `correction.i + state->velocity.i` 29, `sqrt((real)length_squared)` 29, `0.5f * rate` for the limit 29, `delta`
// declared before `velocity` 29 (all the same code), the output through vector3f_add 57 (369 bytes).
// M3 lane 1 r90 (fast with 0x1fc660, 0x1fc350, 0x1fc2f0, 0x1fc710, 0x1fc620; aligned/register-masked control 32 [29/13]):
// a 600 s permuter run on this body (61 tries, one confirming full build) found nothing below 29, and its 29 needs the
// rate read through a volatile cast: that cast alone gives 29 [26/15] (diagnostic, reverted at once; the limit's rate
// is then reloaded), so the variant's other parts (the first correction add written correction first, the limit
// declared ahead of delta, the tick count assigned after its declaration, a `(real)` on the squared length) add
// nothing; the first add's operands swapped alone 32 (same code). No untried natural form is left (as lane 18 r17
// found): the store/delta order and the rate-chain order stay coupled through the delta statements.
// M3 lane 12 r113 (fast with the file's five other functions, all kept): re-measured on this body, the call without the
// parameter reference 99 (frame 0x14; tracked no longer arrives in EAX), so it stays; the deltas assigned i, j, k 79 (stores,
// early dj square and (k + j) + i sum then retail's; left the tick products i, j for retail's j, i and the delta homes
// di/dj/dk XMM3/XMM4/XMM2 for retail's XMM4/XMM2/XMM3). No unlisted form; body unchanged.
// M3 lane 12 r115 (fast with 0x1fc660): the velocity rows spelled k, j, i or j, k, i with the deltas through the existing
// vector3d_from_points3d 53/53 (row-order variants of the listed per-component tick stage); the squared length
// accumulated j, k, i 30 (-2, a written-out length_sq3f), k, j, i 78; velocity and delta declared at their first use 32
// (same code); the prediction straight into velocity with the scaled difference in place 57 (369 bytes); the limit
// computed before the squared length 46. A compile-only screen of 700 prediction, delta, length
// and correction spellings (a copy of this file reproduces this listing) found nothing closer; the correction is
// retail's only through local_scale3d (vector3f_add_scaled or spelled rows lose the copy-then-scale).
// M3 lane 4 r190 (fast with 0x1fc660): the squared length through the existing dot3f(&delta, &delta) 32 (same code);
// the deltas through the existing vector3d_from_points3d 80 (the listed i, j, k deltas); both 80; both plus the correction
// through the existing vector3f_add 87; dot3f with the correction through vector3f_add 57 (369 bytes, the listed vector3f_add
// form). New forms: the limit as `rate / 2.0f` 32, `real const limit` declared where computed 32, the root's product
// rate-first 32 (all three the same code); the location tests state-first 36. Body unchanged.
// M3 lane 2 r77 (fast with 0x1fc660, control 32): no unlisted natural form found for the coupled delta/store order: delta
// and length orders, the prediction and tick stages through the existing helpers or written per component, in-place forms,
// declaration places and the limit spellings are all listed above. Body unchanged.
// M3 lane 7 r156 (fast with the file's five matched functions and 0x1e55d0): the squared length summed from the raw velocity
// differences with the delta formed in the then-branch only 83 (378 B); the deltas j, i, k kept for the correction with the
// length summed from raw differences i, j, k 77 (371 B; the sum is then not reassociated), the same k, j, i 77; the correction
// written back into `velocity` with one output copy after the if 50 (366 B); the root through the existing magnitude3d(&delta)
// 46 (366 B: inlined, its sum CSE'd with the squared length; chains i, j, k with stores i, j, k), with the deltas i, j, k 64,
// i, k, j 64, k, i, j 38, j, k, i 59, k, j, i 54; the squared length through the existing out-of-line distance_sq3f
// (0x24550, inlined) of state->velocity and velocity with the deltas j, i, k kept 103 (386 B: the differences are formed
// twice), the deltas formed in the then-branch only 83 (374 B), the deltas i, j, k 98 (382 B). Body unchanged.
// M3 lane 5 r179 (fast with 0x1fc660/0x1fc350/0x1fc2f0/0x1fc710/0x1fc620): re-measured round 508's vector setter for the
// delta, now with the parameter-address read kept, 90 (352 bytes). No untried natural form (lane 1 r90, lane 2 r77,
// lane 12 r113). Body unchanged.
// M3 lane 16 r116 (fast with 0x1fc660): the existing vector3f_assign (unknown_0259d0.h) for the delta 90 (352 bytes; the
// listed vector setter), for the correction output 32 (same code), both 94 (352 bytes); the timing globals through one
// named pointer local for the rate, ticks, limit and root 46 (first difference +0x20); the squared length through
// length_sq3f at both uses (no local, diagnostic) 46. Body unchanged.
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
	vector3f predicted;
	vector3f_add_scaled((vector3f const *)&state->unknown38, &state->velocity, rate, &predicted);
	scaled_difference3f(state->unknown2c.n, predicted.n, (real)ticks, velocity.n);
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
