#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "object_queries.h"
#include <math.h>
#include <string.h>
#include <float.h>

// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "unknown_11cc90.h"

/* limits read by the range checks below (zero in the retail image; set at run time) */
real g_54e854;
real g_54e858;
real g_54e85c;
real g_54e860;

extern real g_4e9bd0;
short g_468d3c[6] = { 3, 3, 2, 1, 1, 3 };
short g_468d30[6] = { 3, 3, 2, 1, 1, 6 };
real const g_4449c0[6] = { 1500.0f, 1500.0f, 1500.0f, 100000.0f, 100000.0f, 100000.0f };

struct s_view_setup
{
	point3f position;
	s_location location;
	vector3f unknown14;
	vector3f forward;
	vector3f up;
	real field_of_view;
	real aspect;
	real scale_x;
	real scale_y;
	real deviation;
	real vertical_field_of_view;
	real ratio;
};

struct s_motion_channels_1701f0
{
	byte unknown00[8];
	dword flags;
	real target[16];
	vector3f translation;
	byte unknown58[0x94 - 0x58];
	byte channel_flags[6];
	byte unknown9a[2];
	real times[6];
	byte unknownb4[2];
	byte follow_matrix;
	byte unknownb7;
	s_view_setup view;
	real current[16];
	transform4x3f matrix;
	real first_derivative[13];
	real second_derivative[13];
	real fifth[13];
	real fourth[13];
	real third[13];
	real second[13];
	real first[13];
	real constant[13];
	real delta[13];
	byte unknown354[4];
};

// @retail 0x1701f0
void function_1701f0(long player_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real *out = state->second_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *time = state->times;
	for (long group = 0; group < 6; ++group)
	{
		real t = *time - g_4e9bd0;
		if (t > 0.0f)
		{
			real t2 = t * t;
			real t3 = t2 * t;
			for (short i = 0; i < g_468d3c[group]; ++i)
			{
				out[i] = fifth[i] * t3 * 20.0f + fourth[i] * t2 * 12.0f + third[i] * t * 6.0f + second[i] * 2.0f;
				if (out[i] > g_4449c0[group] || 0.0f - g_4449c0[group] > out[i])
				{
					for (short other = 0; other < 6; ++other)
						if (other != group && state->times[other] == *time)
							state->times[other] = 0.0f;
					*time = 0.0f;
				}
			}
		}
		else
			memset(out, 0, g_468d3c[group] * sizeof(real));
		out += g_468d3c[group];
		fifth += g_468d3c[group];
		fourth += g_468d3c[group];
		third += g_468d3c[group];
		second += g_468d3c[group];
		++time;
	}
}

// @retail 0x1703f0
void function_1703f0(long player_index)
{
	real inverse_dt = (real)(1.0 / g_4e9bd0);
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real *delta = state->delta;
	real *out = state->first_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *first = state->first;
	real *time = state->times;
	byte *channel = state->channel_flags;
	for (short group = 0; group < 6; ++group)
	{
		real t = *time - g_4e9bd0;
		if (t > 0.0f)
		{
			real t2 = t * t;
			real t3 = t2 * t;
			real t4 = t3 * t;
			for (short i = 0; i < g_468d3c[group]; ++i)
				out[i] = fifth[i] * t4 * 5.0f + fourth[i] * t3 * 4.0f + third[i] * t2 * 3.0f + second[i] * t * 2.0f + first[i];
		}
		else if ((state->flags & 1) && ((*channel & 2) || (state->flags & 8)))
			memset(out, 0, g_468d3c[group] * sizeof(real));
		else if (state->flags & 1)
		{
			for (short i = 0; i < g_468d3c[group]; ++i)
				out[i] = 0.0f - delta[i] * inverse_dt;
		}
		long count = g_468d3c[group];
		delta += count;
		out += count;
		fifth += count;
		fourth += count;
		third += count;
		second += count;
		first += count;
		++time;
		++channel;
	}
}

vector3f *matrix4x3_rotation_between(transform4x3f const *a, transform4x3f const *b, vector3f *out);

PRIVATE inline void cross_product_170c00(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

PRIVATE inline void point_set_170c00(point3f *point, real x, real y, real z)
{
	point->x = x;
	point->y = y;
	point->z = z;
}

PRIVATE inline void placement_basis_170c00(vector3f const *forward, vector3f const *up, transform4x3f *matrix)
{
	matrix->scale = 1.0f;
	matrix->forward = *forward;
	cross_product_170c00(up, forward, &matrix->left);
	matrix->up = *up;
	point_set_170c00(&matrix->position, 0.0f, 0.0f, 0.0f);
}

__declspec(noinline) vector3f *function_170c00(vector3f const *first_forward, vector3f const *first_up, vector3f const *second_forward, vector3f const *second_up, vector3f *out);

// @retail 0x170c00
vector3f *function_170c00(vector3f const *first_forward, vector3f const *first_up, vector3f const *second_forward, vector3f const *second_up, vector3f *out)
{
	transform4x3f first, second;
	placement_basis_170c00(first_forward, first_up, &first);
	placement_basis_170c00(second_forward, second_up, &second);
	return matrix4x3_rotation_between(&first, &second, out);
}

__declspec(noinline) void function_170bb0(real const *second, real const *first, real *out);

/* Retail's one caller (0x16fe90) forms the output address first, then first, then second (0x16feb0, 0x16feb6,
   0x16feb9): right to left over (second, first, out). The output is walked through its own cursor; with the
   parameter itself stepped, this order moves the three incoming registers (round 191). */
// @retail 0x170bb0
void function_170bb0(real const *second, real const *first, real *out)
{
	real *difference = out;
	long channel_index = 0;

	do
	{
		*difference++ = *first++ - *second++;
	} while (++channel_index < 10);
	for (; channel_index < 11; channel_index++)
	{
		/* the subtracted state's basis goes first */
		function_170c00((vector3f const *)second, (vector3f const *)(second + 3),
			(vector3f const *)first, (vector3f const *)(first + 3), (vector3f *)difference);
		second += 6;
		first += 6;
		difference += 3;
	}
}

PRIVATE inline void rotate_vector_170d70(vector3f *v, vector3f const *axis, real sine, real cosine)
{
	real cross_i = v->j * axis->k - v->k * axis->j;
	real projection = (v->i * axis->i + v->j * axis->j + v->k * axis->k) * (1.f - cosine);
	real cross_j = v->k * axis->i - v->i * axis->k;
	real cross_k = v->i * axis->j - v->j * axis->i;
	v->i = v->i * cosine + projection * axis->i - cross_i * sine;
	v->j = v->j * cosine + projection * axis->j - cross_j * sine;
	v->k = v->k * cosine + projection * axis->k - cross_k * sine;
}

// @retail 0x170d70
void function_170d70(vector3f const *rotation, vector3f *a, vector3f *b)
{
	vector3f axis = *rotation;
	real angle = normalize3d(&axis);

	if (angle != 0.f)
	{
		real s = (real)sin(angle);
		real c = (real)cos(angle);

		rotate_vector_170d70(a, &axis, s, c);
		rotate_vector_170d70(b, &axis, s, c);
	}
}

static inline bool real_is_finite(real value)
{
	return (*(long *)&value & 0x7f800000) != 0x7f800000;
}

static inline bool real_in_world_range(real value)
{
	return real_is_finite(value) && value >= -50000.0f && value <= 50000.0f;
}

PRIVATE inline bool motion_near_zero_170630(real value)
{
	return real_is_finite(value) && fabs(value) < 0.001f;
}

/* advances the player's motion channels by the elapsed time, then rebuilds the forward and up
   vectors at current[10] and current[13] as an orthonormal pair when they drift */
// score 64 (was 64): The 0x5c frame agrees. The deadline cursor and signed group ordinal spill in the opposite order, and the basis correction still differs.
// Earlier (64, was 72): the current pointer is declared before the values, and the new up's j is stored as soon as it
// is computed (the permuter found both: 70 and 66 alone). Before (72, was 81): the loop matches but for two slots
// (the time pointer the compiler makes and
// the group counter are swapped) and the order of the current and value pointer setup; after it
// the float sums and products come out in another order (the dot product as k, j, i where retail
// has k, i, j; the cross products' first loads) and 1.0f is loaded later. Our compiler reorders the
// operands of these sums and products whatever their order in the source (no change for any
// order of the dot product, dot3f, or the cross product's terms). The new up kept in locals and
// stored j, k, i 72 (stored directly 81). Tried: an explicit time pointer with the current pointer
// declared before the value pointer (the whole loop then matches, but the code after it changes:
// 124); declaration orders around it 120-140; a cross product helper for both products 122-140.
// Second logic walk (c2B, every block, retail 0x170630-0x170ba2): the six groups' tables, the time compare
// (> 0 / flags & 1 copy path), the t^5..t^1 polynomial and the 0 - velocity * step path, the < 5 add vs the
// 0x170d70 call, the 1.0 / 0.001 / 0.001 tests and both normalizes agree with retail, no behaviour difference.
// Also tried (all worse): explicit time pointer with the pointers declared in retail's lea order (target,
// fifth..first, velocity, constant, time, current, value): the loop is then identical, the prologue's lea order
// and the cross product change (141); `real one = 1.0f` for the length tests (72, same bytes); the dot
// product in a local (180) or the tests split into bool locals (123). Permuter (25 min on r622): best 63, from the
// two changes kept here plus spelling-only rewrites; the count as unsigned 72 alone.
// Round 575: time and group still trade stack slots. NOT kept: time cursor in the for step 143; time-range
// walk with derived ordinal 422; short channel stride 78; channel stride at each advance 127; exact group
// termination 65; group while loop 64; final rotation group first 341; shared basis dot product 151.
// The time cursor and group ordinal still spill in the opposite order. Their latch and the cross products also differ.
// Tried time-group index name 64, ordinal before channel setup 83, time cursor before current 141, time cursor before constant 129,
// indexed time address 88, shared copy width 329, self-dot length tests 64, accepted-orientation return 64.
// Deadline cursor at linear coefficients 131, indexed deadline base 103, deadline cursor beside target 139,
// scratch storage after coefficients 67, target-copy count snapshot 352, both strides before the advances 192,
// scalar side components 108, separate orthogonality deviation after length checks 124.
// The short group ordinal and deadline cursor still trade stack homes.
// The time cursor and group ordinal still trade spill homes. Writable time cursor 125, read-only cursor after
// ordinal 129, read-only channel cursors 125, cached coefficient width 475, separate coefficient-width cursor 329,
// target advanced first 78, shared signed stride with target first 157, sized time-array reference 103,
// interpolation arm first 64, inner interpolation scope 64, writable cursor with basis dot 125, with shared cross
// products 131 or with inline normalization 123. NOT kept: the explicit cursor recovers the channel loop, but the
// orientation tail grows the body to 1413 bytes.
// Deadline range with ordinal 375 or separate end 398, deadline advanced at read 290 or before
// the last stride 142, staged orthogonality sum 62, whole corrected up vector 234,
// staged left products 126, aggregate left cross product 64.
// NOT kept: the cursor forms grow the body. The staged sum gains two with the time and ordinal homes still swapped.
// Traversal scope 64, scoped deadline pointer 125, deadline beside interpolation values 141,
// elapsed time by subtraction assignment 134, linear cursor before the value or coefficient
// advances 88/74, paired corrected-up stores 70, rejecting orientation disjunction 64.
// NOT kept: the explicit cursor grows the orientation tail, and the in-place subtraction grows
// the body. The time and group stack slots still trade places in the opening form.
// Deadline in loop initializer 129; deadline before constant term 129; coefficient-width cell 88;
// copy-width cell 143; retained current-channel base 446; shared elapsed-time predicate 267;
// force-inlined motion test 64; nested orientation acceptance 64. The explicit deadline forms
// grow to 1413 bytes. The time and group homes and linear coefficient latch still differ.
// Separate finite-value guard in the motion predicate 64.
// Round 639 forms: target before current or scratch, derivative before target, and scratch cursor after target 64 each.
// Earlier or later group name 64/64, deadline array bound 65, derivative advance before scratch 78,
// named copy decision 142, existing placement cross for left 68 or with explicit deadline cursor 125.
// NOT kept: the explicit cursor restores the traversal at 1413 bytes, and the other forms leave the
// deadline and group homes exchanged.
// Complete forward/up/both snapshots for correction on the inferred walk
// 232/234/408, on the explicit walk 204/190/399. Both snapshots used during
// validation and correction 433/434. NOT kept: all shorten away from retail
// and regress. The two-vector forms also change the matched caller. The
// inferred deadline and signed group ordinal still exchange stack slots.
// Direct j/k/i up reconstruction 137, or with independent deadline 133.
// Six named cross products 126, or with independent deadline 125.
// Independent deadline with captured up components 134, one orientation
// acceptance result 125, shared copy width 120, or shared inner index 127.
// The independent deadline recovers the slot order in the traversal,
// but the retained correction schedule still differs and the body grows.
// Typed current-channel basis with vector pointers 64, direct fields 141, one basis reference 434,
// or the staged in-place up cross product 119. With an independent deadline cursor 125/123/435/124.
// NOT kept: the cursor can recover both loop homes, but grows the correction tail. The shared
// basis reference changes the frame and the matched caller.
// Separate deadline with shared basis base 124, basis references 125, post-tested six-group
// walk 125, coefficient-width cursor 348, copy-width cursor 341, staged orientation
// deviations 121, early basis addresses 121, or early signed ordinal 135. NOT kept:
// the shared basis base gives 1401 bytes and restores the traversal through +0x2b7.
// The orientation tail still differs. Separate width cursors grow the frame.
// Independent deadline with a shared side-vector setter 125, corrected up-vector setter 124, or both 124.
// Plain while traversal 125, player-state reference 125, rotation group first 376, deadline advance
// beside the target advance 158, or side-vector aggregate 125, all with the independent deadline.
// NOT kept: the 125 forms recover the first 0x2b8 bytes but grow the correction at 1413 bytes.
// M3 lane 12 r160 (fast; control 64): 0-7 initialised unused locals added at the top (a symbol-count phase scan,
// diagnostic) change the tail's product operand sides (22 of 33 retail's at none, 27 at six) and the length, but no
// phase gives retail's tail and the time/group slot swap never moves. Each form with its phase-0 score and its best
// over the eight phases: left through the existing cross product helper 68/68; the new up through it 79/79; that helper
// for left and its staged variant for up 120/116; the up pointer declared before forward 110 at 1395 bytes/88; a
// read-only time cursor advanced at the loop end 125 at 1413 bytes (the loop is then exact)/91 at 1399 bytes; that with
// the up pointer first 125/91.
// Linear, low-order, whole polynomial, or delta cursor names 64 each. Read-only target,
// derivative, or polynomial cursors 64 each. Shared unit-length checks 64. Separate target-copy
// cursors 326, with independent deadline 316. Independent deadline with polynomial names
// or shared unit-length checks 125 each. NOT kept: both 125 forms recover the traversal
// through +0x2b7 but change the basis correction and grow to 1413 bytes.
// Named deadline through a read-only cursor 94, or declared before the constant cursor 98.
// With the named deadline: shared basis dot 70, three named alignment products 94,
// nested alignment check or successful early return 94 each, shared side cross 94,
// staged up cross 98. The separate deadline recovers the two traversal slots, but
// none of these forms recovers the orientation schedule at retail length.
// Independent cursor with a named scheduled deadline 94, or both subtraction operands named 124; signed
// ordinal beside deadline 125; channel or shared-basis normalization range 220/309; shared basis with
// named length predicate 124; native current/target stride 120; corrected up accumulators 124. With the
// named deadline, staged orthogonality 92, direct j/k/i stores 117, shared basis 115, aggregate side
// vector 94. NOT kept: the named deadline restores the traversal through +0x34a at 1396 bytes, but the
// dot product and basis correction still raise the score.
// Linear coefficient advanced in the loop step, after the constant, or by capturing the
// next pointer at the latch or width read: indexed deadline 347/68/92/92, independent
// deadline 367/144/140/140. NOT kept: the spill order still differs and all scores rise.
// Separate corrected up vector through placement, shared, or staged cross with j/k/i stores
// 70/70/70, or whole vector publication 234. With independent deadline 130/130/130/201.
// NOT kept: The separate component stores leave the swapped deadline and group homes in the
// original loop. The independent form grows to 1413 bytes and differs in the basis correction.
// Named deadline through a read-only cursor 94, or declared before the constant cursor 98.
// With the named deadline: shared basis dot 70, three named alignment products 94,
// nested alignment check or successful early return 94 each, shared side cross 94,
// staged up cross 98. The separate deadline recovers the two traversal slots, but
// none of these forms recovers the orientation schedule at retail length.
// M3 lane 13 r131 (fast with 0x16fe90; base 64): the time pointer and the group counter trade homes because retail sets
// the time pointer ahead of the counter's zero; `real *time = state->times;` declared after `constant`, read as `*time`
// and stepped last in the body gives retail's whole loop (first difference +0x2b8, in the tail) but 125 by position
// (1413 B). The tail's product operand order is a numbering state: retail orders up.i < forward.k < computed values <
// forward.i, forward.j < up.j, up.k; the plain body has forward.* < computed < up.*, the explicit-pointer body computed
// < everything with forward.j/k above up.*. Diagnostic sweeps of 0..7 dead initialised function-scope locals (period
// 8; dead locals inside the correction block change nothing): plain body 64, 88, 112, 112, 112, 112, 86, 127 (with six
// the dot's sides and the second normalize are retail's); explicit-pointer body 125, 121, 121, 123, 125, 127, 91, 121.
// On the explicit-pointer body: the corrected up as named values declared in each of the six orders and stored j, k, i
// 124, 124, 124, 128, 128, 128; declared and stored at once in the six orders 131, 133, 133, 133, 132, 132; stored
// directly in the six orders 131, 133, 133, 133, 132, 132; through the cross helper with stores j, k, i or i, j, k 130
// / 130, as one vector assignment 201 (1363 B); the left rows in the five other orders 132, 128, 132, 132, 128 and
// through the cross helper 124. Seven tail spellings x 0..7 dead locals (direct stores, left through the helper, up
// through the helper, left as three reals, up as `forward + 1`, both vector pointers declared at the function top, the
// dot through dot3f): best 84 (1399 B, six dead locals with the named or helper forms); the first normalize stays one
// instruction long in every explicit-pointer state (the scale ranks below all three forward fields).
// Deadline range equality, ordered range or while range 380/380/380. Named
// deadline with for or while range 393/393, or range with alternate up normalization 393.
// Counted named deadline with alternate forward or up normalization 93/93.
// NOT kept: the counted forms recover traversal through +0x34a but worsen
// the score. Pointer-bounded traversal grows by 54 to 66 bytes against retail.
// Named indexed scheduled time 142, or from a local deadline-array base 161.
// Shared product difference for the side, corrected up, or both 64 each, or
// 142 each with the named scheduled time. NOT kept: the product forms tie
// and the named read lengthens the body without recovering the spill order.
// Named deadline with one signed copy-width load: current first with long or
// short coefficient width 129/145, target first 145/129. Short and long copy
// widths agree. NOT kept: all grow to 1413 bytes. The 129 forms recover the
// traversal through +0x2b7, but the basis correction remains farther from retail.
// The 0x5c frame agrees. The deadline cursor and signed group ordinal still exchange stack homes and
// the basis correction differs. Tried existing unit predicate for both basis axes 64, existing unit
// predicate with explicit deadline 125, explicit deadline and direct time-step read 255, explicit
// deadline and subtraction on elapsed time 218, unit predicates with direct time-step read 255, unit
// predicates with elapsed subtraction assignment 218, unit predicates with shared alignment dot 125,
// unit predicates with shared side cross 125. NOT kept: The explicit cursor recovers the traversal
// through +0x2b8 with the original time-step expression, but grows to 1413 bytes. The direct read or
// subtraction assignment loses the retail time-step scheduling.
// The deadline and signed ordinal still occupy exchanged slots. Named deadline with traversal scope 94,
// inclusive last group 96, exclusive sentinel 95, final-group distinction 352, rotation-first equality 357,
// scoped rotation-first equality 357, reversed time-test spelling 94, flag-first snap test 100.
// NOT kept: the first and seventh forms recover both traversal slots through +0x34a at 1396 bytes, but
// regress. The rotation tests grow the body to 1431 bytes and lose the opening schedule.
// With the named deadline: signed ordinal declared before cursor 94; early cursor advance 215;
// existing i-k-j basis dot 70, with successful early exit 70; both squared-length deviations
// computed before tests 131; alternate normalization of both axes 92; deadline array extent 95;
// staged side cross 94. NOT kept: the 94 and 92 forms recover the traversal through +0x34a
// at 1396 bytes. The dot forms shorten to 1389 bytes. The correction and normalization still differ.
// The indexed deadline and short ordinal still exchange slots. Indexed post-test 134/134, indexed deadline
// reference 88 or with post-test 143, independent deadline reference 121 or with post-test 127,
// independent deadline value with post-test 121, and reference with advance at read 289. NOT kept: the
// independent forms restore the traversal homes but grow the orientation tail.
// M3 lane 8 r225 (h2m fast, control 64): the whole check-and-repair tail as a file-local helper taking forward and up:
// `inline` stays a call 993 (631 B); `__forceinline` 110 at retail length 1395 B (both normalizes then load as
// retail, the cross section is one instruction long); with the explicit time cursor 125 (1413 B: the loop and both
// length tests are retail's, the dot and cross are not). Inside the forced helper: left through cross3f 110, left and
// up through cross3f 126, both through cross3f_staged 117, up alone staged 117 or through cross3f 126, the dot
// through dot3f either way 110; the corrected up computed in the six orders and stored j, k, i: i-first orders 117,
// k or j-k first orders 60 (1392 B); without the helper 70 (i first) / 119 (others). The helper taking one basis
// pointer 117 (kji 142), taking the channel array 446; up as `forward + 1` in the body 64. Dead locals inside the
// helper change nothing (diagnostic); caller-side phases 1..7 on the helper form 88/112/112/112/116/157/125,
// with the explicit cursor 121/123/123/123/96/91/121. Lead (match-only: a one-caller helper and statement orders
// picked by score): in the forced helper, the 36 compute x store orders of the corrected up give 56 (1392 B) for
// j-i-k, j-k-i or k-j-i computed and stored k first (k-i-j or k-j-i), 60 with j stored first, 117 with i computed
// first; on 56 the dot through dot3f or left through cross3f 60; with the explicit time cursor the six compute
// orders 124/124/124/128/128/128 (1413 B). On 56 the loop slots, the dot's k-i-j order, the cross section's
// schedule and the first normalize's j product still differ.
// M3 lane 5 r237 (H2M_FAST, control 64): `group` renamed channel, ordinal, a or zone 64 each (same code: the cursor and
// ordinal homes do not follow names); the loop's pointer advances with the six coefficient pointers ahead of value and
// velocity 228 (1415 B), all advances ahead of current/target 215 (1397 B), value moved after first (retail's EBP add
// position) 78, the coefficient advances reversed then velocity and value 241 (1415 B). Body unchanged.
// M3 lane 13 r152 (h2m fast with 0x16fe90, MATCH; control 64). Frame reading: the time step's compiler pointer and the group
// counter both have four references; equal counts go to the one referenced first, and the counter's zero comes ahead of
// the pointer's set-up, so the counter gets the lower home (retail sets the pointer first and gives it the lower home).
// On the explicit read-only time cursor stepped last (125 at 1413 B, aligned 35 against main's 65; loop exact): the up pointer
// declared ahead of forward 125; the time step read without the const cast 255; left through the file's cross product helper
// 125; left and the new up both through it 131; the counts read at each advance (no count local) 132 (1403 B); the new up
// stored directly i, j, k 131; only up.i named, j and k stored directly 125; all three named, stored i, j, k 130; the
// orthogonality test through dot3f(up, forward) 125, dot3f(forward, up) 125.
// Failed-test disjunction and staged up publication 70. On the named deadline walk, staged up and
// alternate normalizations 99; staged side and up 101; existing side cross and alternate normalizations
// 92; reversed shared dot inputs 70; failed-test disjunction and staged up 101; both axis references
// with staged up 101; signed copy and coefficient widths with staged up 148. NOT kept: The recovered
// traversal still diverges in the correction, and the shared dot shortens the body.
// Unsigned-word or byte group 66/234; next deadline derived from the ordinal 372;
// whole deadline-array pointer 103. With a named deadline: bulk target copy 367,
// all polynomial products named 89, named forward squared length 87, or adjacent
// basis addresses at the group update 94. NOT kept: the 89 form reaches retail
// length and changes the polynomial. Naming the squared length shortens the
// basis tail, and the adjacent addresses keep its first difference at +0x34b.
// Named deadline with exchanged multiplication reads in k, i, j, ki, kj, ij or kij scores 94 each.
// Staging the kij dot after both length tests scores 92. All reach +0x34b at 1396 bytes.
// M3 lane 16 r188 (H2M_FAST with 0x16fe90, MATCH; control 64). Frame reading: the group counter and the time step's
// compiler pointer both pack with four references, and the counter's references come first at the loop's head and at
// its latch, so it takes the lower cell. The orthogonality dot (retail: k term, then forward.i * up.i, then up.j *
// forward.j; ours k, j, then up.i * forward.i): the i term in parentheses 151 (1384 B), the j term in parentheses 151
// (1384 B), the k term in parentheses 64 (same code); `up->k * forward->k + up->i * forward->i + forward->j * up->j` 64,
// the j term written ahead of the i term 64 (same code each: the terms sort by rank). Body unchanged.
// Shared unit-length predicate on the indexed traversal 64. Named deadline with shared forward,
// up or both unit tests 94 each. With both tests plus ordered basis dot 70, staged left cross
// 94, both alternate normalizations 92, or ordered dot and staged cross 70.
// NOT kept: the indexed form ties and the independently walked forms raise the score.
// M3 lane 14 r156 (h2m fast with 0x16fe90 and 0x170d70, MATCH; control 64): the last group's call arm first on
// `group >= 5` 216 (1395 B); the time step as `t = times[group]` then `t -= step` 134 (1403 B); the time table in a
// `real const *` local ahead of the loop 103 (1395 B, first +0x18); the values array and its cursor declared after the
// coefficient cursors 67; the copy loop's index declared ahead of its loop 64 (same code). Body unchanged.
// @retail 0x170630
void function_170630(long player_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + player_index;
	real *current = state->current;
	real values[13];
	real *value = values;
	real *target = state->target;
	real *velocity = state->first_derivative;
	real *fifth = state->fifth;
	real *fourth = state->fourth;
	real *third = state->third;
	real *second = state->second;
	real *first = state->first;
	real *constant = state->constant;
	for (short group = 0; group < 6; ++group)
	{
		/* retail reads the time step from memory at each use; a const read keeps the compiler from hoisting it */
		real t = state->times[group] - *(real const *)&g_4e9bd0;
		if (!(t > 0.0f) && (state->flags & 1))
		{
			for (short i = 0; i < g_468d30[group]; ++i)
				current[i] = target[i];
		}
		else
		{
			if (t > 0.0f)
			{
				real t2 = t * t;
				real t3 = t2 * t;
				real t4 = t3 * t;
				real t5 = t4 * t;
				for (short i = 0; i < g_468d3c[group]; ++i)
					value[i] = fifth[i] * t5 + fourth[i] * t4 + third[i] * t3 + second[i] * t2 + first[i] * t + constant[i];
			}
			else
			{
				for (short i = 0; i < g_468d3c[group]; ++i)
					value[i] = 0.0f - velocity[i] * g_4e9bd0;
			}
			if (group < 5)
			{
				for (short i = 0; i < g_468d3c[group]; ++i)
					current[i] = value[i] + current[i];
			}
			else
				function_170d70((vector3f const *)value, (vector3f *)current, (vector3f *)(current + 3));
		}
		current += g_468d30[group];
		target += g_468d30[group];
		long count = g_468d3c[group];
		value += count;
		velocity += count;
		fifth += count;
		fourth += count;
		third += count;
		second += count;
		first += count;
		constant += count;
	}
	vector3f *forward = (vector3f *)(state->current + 10);
	vector3f *up = (vector3f *)(state->current + 13);
	if (!(motion_near_zero_170630(length_sq3f(forward) - 1.0f) &&
		motion_near_zero_170630(length_sq3f(up) - 1.0f) &&
		motion_near_zero_170630(up->k * forward->k + forward->i * up->i + up->j * forward->j)))
	{
		vector3f left;
		left.i = up->j * forward->k - up->k * forward->j;
		left.j = up->k * forward->i - forward->k * up->i;
		left.k = forward->j * up->i - up->j * forward->i;
		real up_i = forward->j * left.k - left.j * forward->k;
		real up_j = left.i * forward->k - forward->i * left.k;
		up->j = up_j;
		real up_k = forward->i * left.j - forward->j * left.i;
		up->k = up_k;
		up->i = up_i;
		normalize3d(forward);
		normalize3d(up);
	}
}

// @retail 0x172350
bool function_172350(point3f const *point)
{
	if (real_in_world_range(point->x) && real_in_world_range(point->y) && real_in_world_range(point->z))
		return true;
	return false;
}

// @retail 0x1723e0
bool function_1723e0(real value)
{
	if (real_is_finite(value) && value >= g_54e858 && value <= g_54e85c)
		return true;
	return false;
}

// @retail 0x172420
bool function_172420(real value)
{
	if (real_is_finite(value) && value >= g_45dbd8 && value <= g_54e860)
		return true;
	return false;
}

// @retail 0x172460
bool function_172460(real value)
{
	if (real_is_finite(value) && value >= g_45dbd8 && value <= 3600.0f)
		return true;
	return false;
}

bool function_a74c0(vector3f const *forward, vector3f const *up);
bool function_a7570(vector3f const *vector);

struct s_checked_placement_1724a0
{
	dword flags;
	point3f position;
	point3f target;
	byte unknown1c[8];
	real scale;
	real radius;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	byte unknown50[0x38];
	real duration;
};

// @retail 0x1724a0
bool function_1724a0(s_checked_placement_1724a0 const *placement)
{
	if (placement && (!(placement->flags & 1) ||
		(function_a74c0(&placement->forward, &placement->up) &&
		function_172350(&placement->position) && function_172350(&placement->target) &&
		function_a7570(&placement->velocity) && function_172420(placement->scale) &&
		function_1723e0(placement->radius) && function_172460(placement->duration))))
		return true;
	return false;
}

struct s_observer_command;
struct s_16f460;
void function_16f460(s_16f460 *state);

#define PLACEMENT_CLAMP_172520(value, minimum, maximum) \
	((value) = (value) < (minimum) ? (minimum) : (value) > (maximum) ? (maximum) : (value))

// @retail 0x172520
void function_172520(s_observer_command *command)
{
	s_checked_placement_1724a0 *placement = (s_checked_placement_1724a0 *)command;
	if (!function_1724a0(placement))
	{
		if (!function_a74c0(&placement->forward, &placement->up))
		{
			placement->forward = *g_4687a8;
			placement->up = *g_4687b0;
		}
		PLACEMENT_CLAMP_172520(placement->position.x, -50000.0f, 50000.0f);
		PLACEMENT_CLAMP_172520(placement->position.y, -50000.0f, 50000.0f);
		PLACEMENT_CLAMP_172520(placement->position.z, -50000.0f, 50000.0f);
		PLACEMENT_CLAMP_172520(placement->target.x, -50000.0f, 50000.0f);
		PLACEMENT_CLAMP_172520(placement->target.y, -50000.0f, 50000.0f);
		PLACEMENT_CLAMP_172520(placement->target.z, -50000.0f, 50000.0f);
		if (!function_a7570(&placement->velocity))
			placement->velocity = *g_4687a4;
		PLACEMENT_CLAMP_172520(placement->radius, g_54e858, g_54e85c);
		PLACEMENT_CLAMP_172520(placement->scale, 0.0f, g_54e860);
		if (!function_1724a0(placement))
			function_16f460((s_16f460 *)command);
	}
}

/* a table of 512 short lists: a bit per list that is in use, the first value
   of each list and the next value after each value */
struct s_short_list_table
{
	dword used[16];
	short first[512];
	short next[256];
};

PRIVATE __forceinline bool short_list_used_173b00(dword const *used, long index)
{
	return (used[index >> 5] & (1 << (index & 31))) != 0;
}

PRIVATE __forceinline void short_list_mark_used_173b00(dword *used, long index)
{
	used[index >> 5] |= 1 << (index & 31);
}

__declspec(noinline) void function_173b00(s_short_list_table *table, short list_index, short value);

// @retail 0x173b00
void function_173b00(s_short_list_table *table, short list_index, short value)
{
	if (!short_list_used_173b00(table->used, list_index))
	{
		table->next[value] = NONE;
		table->first[list_index] = value;
		short_list_mark_used_173b00(table->used, list_index);
	}
	else
	{
		table->next[value] = table->first[list_index];
		table->first[list_index] = value;
	}
}

struct s_ring_buffer
{
	long unknown00;
	long index;
	short values[256];
};

// @retail 0x173b60
void function_173b60(s_ring_buffer *buffer, short value)
{
	long index = buffer->index++;

	if (buffer->index == 256)
		buffer->index = 0;
	buffer->values[index] = value;
}

struct s_flag_holder
{
	byte unknown00[0x16];
	byte flag16_0 : 1;
	byte flag16_1 : 1;
	byte flag16_2 : 1;
};

// @retail 0x173b80
long function_173b80(s_flag_holder *holder)
{
	return holder->flag16_2;
}

/* ---- the local players ---- */

static inline long local_player_next(long index)
{
	long result = NONE;
	long start = index == NONE ? 0 : index + 1;

	for (long i = start; i < 4; i++)
	{
		if (g_4e8c20->entries[i] != NONE)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x172750
bool function_172750(long mode, point3f const *point, real radius)
{
	bool result = false;
	real limit = radius * radius;

	for (long i = local_player_next(NONE); i != NONE; i = local_player_next(i))
	{
		s_player_state *player = NULL;
		real distance_squared;

		if (i != NONE && g_4686c4 != NONE)
			player = &g_4e9bd4[i].state;
		vector3f delta;
		vector3d_from_points3d(point, &player->position, &delta);
		distance_squared = delta.j * delta.j + delta.k * delta.k + delta.i * delta.i;
		if (mode == 1)
			distance_squared = player->radius * player->radius * distance_squared;
		if (limit > distance_squared)
		{
			result = true;
			break;
		}
	}
	return result;
}

/* ---- the rectangle pool ---- */

struct s_rect4
{
	real x0, x1, y0, y1;
};

struct s_polygon_clip_173380
{
	s_rect4 bounds;
	short count;
	short unknown12;
	point2f *points;
};

short function_23a2b0(short count, point2f const *points, short clip_count,
	point2f const *clip, short capacity, point2f *output, real epsilon);

// @retail 0x173380
long function_173380(s_polygon_clip_173380 const *polygon, s_polygon_clip_173380 const *clip, s_polygon_clip_173380 *output)
{
	output->count = function_23a2b0(polygon->count, polygon->points, clip->count,
		clip->points, 64, output->points, 0.0001f);
	if (output->count == NONE)
	{
		output->count = clip->count;
		memcpy(output->points, clip->points, clip->count * sizeof(point2f));
	}
	return output->count >= 3;
}

struct s_pool_entry
{
	long key;
	short next;
	short unknown06;
	short value08;
	short unknown0a;
	s_rect4 rect;
	short count;
	short unknown1e;
	real *data;
	real unknown24;
	long unknown28;
	byte unknown2c[0x40];
};

struct s_pool
{
	long count;
	s_pool_entry entries[256];
	long data_used;
	real data[10240];
};

struct s_pool_source
{
	byte unknown00[0xa0];
	s_rect4 rect;
	byte unknown_b0[0x198 - 0xb0];
	long count;
	real data[1];
};

// @retail 0x172bb0
short function_172bb0(s_pool *pool, s_pool_source *source, short value, long key)
{
	long count = pool->count++;
	short index = (short)count;
	s_pool_entry *entry = &pool->entries[index];

	entry->key = key;
	entry->next = NONE;
	entry->unknown06 = NONE;
	entry->unknown0a = NONE;
	entry->unknown28 = NONE;
	entry->value08 = value;
	entry->rect = source->rect;

	real *data = &pool->data[pool->data_used * 2];
	pool->data_used += source->count;
	entry->data = data;
	memcpy(data, source->data, source->count * 8);
	entry->count = (short)source->count;
	entry->unknown24 = 0.0f;
	memset(entry->unknown2c, 0, 0x40);
	return index;
}

// @retail 0x173350
short function_173350(s_pool *pool, short index, short value)
{
	while (index != NONE)
	{
		s_pool_entry *entry = &pool->entries[index];

		if (entry->unknown06 == value)
			break;
		index = entry->next;
	}
	return index;
}

/* ---- merging the rectangles ---- */

/* lists of pool entries: bit i of the first 16 words says whether list i
   has a head (heads[i]); the entries are chained through next[] */
struct s_pool_lists
{
	dword valid[16];
	short heads[512];
	short next[256];
};

static inline short list_head(s_pool_lists const *lists, short list)
{
	return (lists->valid[list >> 5] & (1 << (list & 31))) ? lists->heads[list] : NONE;
}

static inline short list_next(s_pool_lists const *lists, short list, short index)
{
	short current = lists->heads[list];

	while (current != NONE && current != index)
		current = lists->next[current];
	return lists->next[index];
}

struct s_view_cluster_1733e0
{
	short index;
	short counts[6];
	short heads[6];
};

struct s_view_volume_1733e0
{
	long identifier;
	long cluster_index;
	long plane_index;
	byte unknown0c[0x20 - 0xc];
	real distance;
	byte unknown24[0x108 - 0x24];
};

struct s_view_collection_1733e0
{
	short active;
	byte unknown02[2];
	byte camera[0x1bc];
	byte unknown1c0[0xa6c - 0x1c0];
	short cluster_count;
	s_view_cluster_1733e0 clusters[128];
	byte unknown176e[2];
	long order[128];
	short volume_count;
	byte unknown1972[2];
	s_view_volume_1733e0 volumes[512];
};

struct s_frustum_1648d0;
struct s_camera_163db0;
struct s_16e150_bits;
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result);
void function_16de20(short index, s_16e150_bits const *table, dword *output);
bool __stdcall function_134950(long a, long b, void const *context);
void sort_4byte(void *unused, long *elements, unsigned long count, bool (__stdcall *compare)(long, long, void const *), void const *context);

// score 9 (was 11): the only differences are the register order of the loop bound reload (ours ecx then eax,
// retail eax then ecx) and the call to sort_4byte, where retail sets the register argument (`lea ecx,[ebx+0x1770]`)
// before the last stack push (`push edx` holding &camera) and ours after it. Tried: a local for the context, a local
// for the count, a local loop counter, none changed it. Permuter (25 min on r621): best 9 (full image 10), only with
// a volatile store to the cluster's first count; not kept.
// Round 98 (m3 lane 2, plain): parameters ordered (cluster_index, collection, camera) from the setup at retail's only call
// (0x13214b sets the collection's ebx before the cluster's eax; caller 0x1320f0 then matches): 9 (same code here).
// only the register order of the loop bound reload differs (ours ecx then eax, retail eax then ecx;
// retail's `mov eax, [abs]` is a byte shorter, hence 320/319). Earlier, the call to sort_4byte also differed, where
// retail sets the register argument (`lea ecx,[ebx+0x1770]`) before the last stack push (`push edx` holding &camera)
// and ours after it; that agrees since sort_4byte takes its scratch pointer first. Tried: a local for the context, a local
// M3 lane 8 r182 (fast), all 9 (same latch): the bound written `g_4e0348->list_count > i` (only the compare flips);
// the loop as `while` with `++i` at its foot; a local copy of g_4e0348 read by the bound (the global is still reloaded),
// or passed to 0x16de20; `cluster->counts[0]++` and `+= 1`; `collection->cluster_count++`; a `short` cluster index;
// the step `i++`.
// @retail 0x1733e0
void function_1733e0(long cluster_index, s_view_collection_1733e0 *collection, byte const *camera)
{
	memcpy(collection->camera, camera, sizeof(collection->camera));
	collection->active = 1;
	collection->cluster_count = 0;
	collection->volume_count = 0;
	if (cluster_index != NONE)
	{
		dword visible[16];
		s_match_globals const *bits = g_4e0348;
		function_16de20(cluster_index, (s_16e150_bits const *)bits, visible);
		for (long i = 0; i < bits->list_count; ++i)
		{
			if ((visible[i >> 5] & (1 << (i & 31))) && collection->cluster_count < 128)
			{
				long index = collection->cluster_count;
				s_view_cluster_1733e0 *cluster = &collection->clusters[index];
				collection->order[index] = index;
				cluster->index = (short)i;
				cluster->counts[0] = 0;
				cluster->heads[0] = NONE;
				++collection->cluster_count;
				short volume_index = collection->volume_count;
				if (volume_index < 512)
				{
					s_view_volume_1733e0 *volume = &collection->volumes[volume_index];
					collection->volume_count = volume_index + 1;
					function_163db0((s_camera_163db0 const *)camera, (box2f const *)(camera + 0xa0), 0, (s_frustum_1648d0 *)volume);
					++cluster->counts[0];
					if (cluster->heads[0] == NONE)
						cluster->heads[0] = volume_index;
				}
			}
		}
		sort_4byte(&camera, collection->order, collection->cluster_count, function_134950, collection);
	}
}

// score 105 (was 129): The one-word frame, cold creation path and signed volume index agree. The camera table is loaded late, the camera key is pushed early and the plane lookup uses other scratch registers.
// Earlier at 129 (was 129). Retail saves EBX for the cluster slot and defers EBP for the signed volume index. The build still places cluster creation ahead of traversal.
// The camera table loads late, the key and table registers are exchanged, and the plane
// lookup uses a different address schedule.
// Lead (P647, not kept): the signed camera offset by record extent as its own one-use local gives plain 11 at
// 434 bytes (camera table still late and exchanged with the key; the key push precedes the camera address
// addition; the distance transfer uses EAX instead of ECX). Folded into the subscript it is plain 105.
// The one-word frame, saved EBX and later BP volume index agree. The first difference at
// +0x10 is the map-miss branch span. At the admitted projection, retail loads the camera base into ECX before
// the volume count calculation, then holds the camera key in EDX for the multiply and push. The trials use the
// actual camera stride and test when the camera table base, record, or byte offset becomes live across volume
// reservation.
// Earlier at 129 (was 129): Retail saves EBX for the cluster slot and defers EBP for the signed volume index. The build still places cluster creation ahead of traversal.
// the creation block is now out of line and the slot (EBX) and volume index (BP) registers are retail's,
// at retail's 434 bytes. Left: the call setup (retail loads the camera array into ECX at the block top and the key into EDX,
// copies the key for the multiply and pushes it afterwards; ours loads the array into EDX after the count store and
// multiplies the key in EAX after pushing it) and, one row later, the plane lookup's operand order. Earlier (129, was 142):
// Retail saves EBX for the cluster slot and defers EBP for the signed volume index. The build still places cluster creation ahead of traversal.
// Earlier (129, was 142): Retail saves EBX for the cluster slot and defers EBP for the signed volume index. Cluster creation stays inline in the build.
// The save at entry and the signed volume index still exchange EBX and EBP, and cluster creation stays before the loop.
// Earlier score 129 (was 142): the logic walk found no behaviour difference (cluster map lookup/creation, the 512 volume
// limit, the 163db0 call, the volume/entry/cluster stores and the list walk all equal retail 0x172ee0-0x17308f).
// What still differs is layout and registers: retail keeps the new-cluster block out of line after the loop
// (je 0x172fad ... jmp 0x172f08) with cluster_slot in ebx and volume_index in ebp (pushed only before the loop);
// we keep it inline with the two registers swapped. Tried: creation inline with one trailing cluster pointer (156),
// `while ((slot = map[ci]) == NONE)` (134 but the first argument moves to eax and ret becomes 0x18), `for (;;)` with
// a break (138, same convention change), a forceinline helper returning the cluster (130, but needs a pointer
// difference to recover the slot), a helper returning the slot (159), volume_index declared before the slot
// (156), goto create/found (203). Permuter (25 min on r622): best 100, only with volatile locals and list_next's
// two index arguments swapped (a behaviour change); not kept.
// b15B (11-17) walk against retail, fast build on r848: all statements and jcc senses re-walked, no behaviour change.
// Forms kept, each retail's own order: the creation block is `if (count < 128) { ... } else return false;` (retail
// xor ecx,ecx 0x172fb1, jge 0x17300d, block out of line at 0x172fad-0x173008, jmp 0x172f08); the new cluster pointer
// is formed once and the stores go through it (0x172fdf, 0x172fe6, 0x172feb, 0x172ffd); plane_index is an if/else
// (retail's stores 0x172fa8 and 0x173016, NONE held in edx from 0x172f8a for the cmp at 0x172f8d); volume_count is
// stored (0x172f4d) before the volume pointer is formed (0x172f56), which puts collection in edi as in retail.
// Left: cluster_slot takes ebp and volume_index ebx (retail ebx and ebp, so retail pushes ebp only at 0x172f08 and ours
// pushes ebx there), and the cameras load comes after the count store where retail loads it at 0x172f3d.
// Tried: written-out clusters[slot] stores 124 (creation block then differs from retail's), swapped if/else 160, no
// `cluster` local 159/162, for(;;) 149, count as long 160, unsigned short or `cluster_count++` 142, volume_index/entry
// declared outside the loop, head pointer, next-index local (no change). A probe store using cluster_slot in the loop
// flips ebx/ebp (131), so the pick is a use-count tie; probe not kept.
// Eligible-volume scope 129. Admitted volume postincrement 129. Nullable cluster record 159.
// Capacity checked on map miss 159. Loop-break result 130. Typed camera stride 129.
// Portal sentinel value 129. Group count before head 129.
// The slot still uses EBP and the volume index EBX. The loop-break result grows to 442 bytes and changes
// the prologue. The matched caller and projection callee stay exact throughout.
// Separate mapped and selected slots 152; existing cluster address before slot assignment 151;
// order subscript from count 129; mapped value from count 129; chained map/order assignment 129;
// new cluster address before map/order stores 129; separate slots with chained stores 152;
// count advanced from reserved slot 122. NOT kept: seven-point gain, and reduced caller 2.
// The opening EBX save and later BP volume index are still exchanged.
// signed cluster list ordinal retained from loop entry 129; shared signed cluster list ordinal for creation
// and traversal 129; pool entry array retained through traversal 128; typed portal plane in existing branch
// 129; typed portal plane selected before publication 123; skip linked entries with traversal advance at the
// loop test 129; skip refused projections with traversal advance at the loop test 129; skip linked entries
// and refused projections at the loop test 129. NOT kept: the shared pool array gains one and the typed plane
// selection gains six. Both retain the 427-byte extent and the first register mismatch. Opening executable
// source restored.
// Tried early camera key 123; early key and camera 123; early camera with key at call 128; input entry reference
// 129; entry and volume references 129; entry reference with early key 123; selected cluster and allocated volume
// names 129; allocated cluster and candidate volume names 129.
// NOT kept: The early-key forms gain six at 427 bytes and leave the register exchange. The address-only form gains
// one.
// Separate entry cursor 133; immutable map result 129; reserved index from tested count 129;
// count read after entry admission 129; next-volume count 129; group head from published
// index 129; direct portal field 129; one resolved plane value 123. NOT kept: the resolved
// plane gains six and still exchanges the entry EBX save with the volume-loop BP save.
// The entry still saves EBP for the cluster slot and uses BX for the signed volume index.
// Capacity-controlled for with success return and while with checked-index result 148/136. Direct
// capacity loop with success return and capacity plus exhaustion controls 135/134. Capacity for
// and while with result values 134/141. Capacity result in the existing do loop 127. Signed
// successor tested before its widening 124. NOT kept: the last two gain two and five, respectively.
// Head array live through projection, paired array references after projection, or head array at both
// group accesses 128/128/128. Group element references or a reference to the carried cluster 129/129.
// Reselect cluster after projection by record, direct indexing or two array receivers 165/165/164.
// NOT kept: the first three gain one and retain the opening register exchange. Reselecting shortens
// the body and direct indexing changes the matched caller. The opening body is restored.
// New cluster counts/heads/both with fill 124/123/145 or fill_n 125/123/135. Complete initialization 187,
// record clear with index/head stores 128. NOT kept: best gains six and reaches retail length but grows to
// an eight-byte frame. The complete initializer changes the matched caller.
// Mapped slot reference 129; traversal cluster after creation join 159; cluster or volume
// count reference 129/129; cluster array capacity 129; shared volume array and capacity 131;
// group count postincrement 129; success default with one return 125. NOT kept: the last
// form restores the EBX/BP roles and the first 0xe bytes, but gains four and leaves creation inline.
// Round 639 forms: fixed cluster array 151, shared volume or cluster count field 129/129,
// signed volume array extent or fixed volume array 129/129, projected slot from selected cluster 132,
// both count field bindings 129, reserved volume name 129, named projection result 129.
// NOT kept: the cluster array changes extent, and the projected slot changes the prologue.
// The other forms leave the opening register exchange.
// With a shared capacity exit, skip linked entries, refused projections,
// or both before advancing at the test 125 each. Direct count increment,
// postincrement as reservation, and positive capacity arm 125 each. Signed
// successor with either increment form 122. NOT kept: the best gains seven
// at 431 bytes. Cluster creation still precedes the volume loop.
// Shared capacity result with a loop break 125, with joined cluster address 139,
// early camera key 118, camera base 125, common plane store 118, signed
// successor 122, widened volume slot 125, paired group cells 125,
// post-reservation camera key 125, and map miss first 132.
// NOT kept: the 118 forms leave cluster creation inline and move the camera
// key ahead of the count store or merge the separate plane stores.
// The signed-successor form gains seven.
// Native rectangle field 129, shared rectangle type 129, early rectangle address 129,
// and early camera key with direct, early, or post-reservation rectangle address 123/123/123,
// an early camera pointer 123, or the shared rectangle type 123. NOT kept: the six-point
// gains leave the register exchange and extent unchanged.
// Default incomplete with shared return 142, miss-first creation 144, local count
// postincrement 142, failed admission first 142, map-cell reference 142, wide count 150,
// fixed camera record extent 142, and one resolved plane store 142. NOT kept: these
// shared-return forms shorten the body and change the matched caller.
// Traversal pointer after index 129, between clears via record reference/pointer 129/129, through
// indexed count clear 123 or indexed index store 129. Between-clear bind with short record subscript,
// retained map cell, or preincremented signed count 129/129/129.
// NOT kept: the indexed count clear gains six. It still creates the cluster before traversal.
// M3 lane 12 r160 (fast; control 129): 0-7 initialised unused locals 129 at every count (the register tie is not a
// symbol-count effect); the slot read again from the map for the volume's cluster store 151 (410 bytes); the entry
// address as `pool->entries + entry_index` 129, the head cell through a short pointer 129 and `++collection->volume_count`
// 129 (all the same code).
// Slot/index lexical orders and immutable reservation/entry 129 each. Return the last
// signed capacity comparison after a break or a positive arm, with or without immutable
// entry/volume addresses: 126 each. NOT kept: the three-point gain leaves creation inline
// and shortens the body to 425 bytes.
// Early missing-cluster capacity guard 137, guarded miss first 159, guarded local postincrement
// 137. Immutable reservation counts, preincrement in count publication, separate count advance,
// immutable entry and volume addresses, or record selected by signed count 129 each.
// NOT kept: the guarded creation forms shorten away from retail. The register exchange
// and inline cluster creation persist.
// Success set at list exhaustion 140. Capacity failure clears completion and breaks, with named cluster
// admission 133, scoped creation receiver 133, explicit list-sentinel break 133, count before head 134,
// dword plane table 133, relative pool entry address 133, or early full-cluster guard 126. NOT kept:
// the guarded form gains three at 431 bytes and still exchanges the register saves. The other shared-
// result forms raise the score.
// Publish the traversal pointer after both clears through the created pointer, reference,
// checked-count indexing or checked-count pointer: 129/129/124/129. Shared capacity result:
// 125/125/118/125. NOT kept: the 118 form restores EBX and BP but puts the cluster creation
// before traversal and spills the traversal pointer after clearing the heads.
// Assignments at map hit, signed capacity test, unlinked-entry test, or projection: 129 each.
// With a shared capacity result: 125 each. NOT kept: The shared result restores EBX and BP
// and advances the first difference to +0xe. The gain is four and creation stays inline.
// Slot/index lexical orders and immutable reservation/entry 129 each. Return the last
// signed capacity comparison after a break or a positive arm, with or without immutable
// entry/volume addresses: 126 each. NOT kept: the three-point gain leaves creation inline
// and shortens the body to 425 bytes.
// M3 lane 13 r131 (fast with 0x1730a0, MATCH throughout; plain with the file's 33 functions; base 129): KEPT one result
// for both failures (`bool result = true;` cleared where the cluster table is full and where the volume table is full, the
// loop under `if (result)`, one `return result;`): 105 (434 B). The result known in every arm of the find-or-add is
// threaded past the later test, which is what moves the creation block out of line; with it the slot takes EBX and the
// volume index BP. Tried before it: creation under `if (slot == NONE)` with a created flag and the lookup under `if
// (!created)` 144 (418 B); a found flag from the map read 144; the mapped value kept and tested twice 144 (each lays the
// creation block first); the find-or-add as a forced-inline bool helper with out parameters, early returns or one
// result, 112 / 112 (434 B, creation out of line, slot still EBP); a have_cluster flag set in all three arms and tested
// after the if/else 112; the creation failure through a result tested by `if (!result) return false;` with the loop's
// own returns 112. On the kept form: the entry key named for both uses 105 (same code); the camera pointer named after
// the volume pointer 22 (the key takes ECX and the array EDX, retail the reverse; a one-use local, not kept), named
// between the count store and the volume pointer 13, spelled key * size + cameras, &cameras[key * size], as an integer
// sum, with an unsigned product, by indexing a 0x1bc-byte record array type, or through an inline accessor 22 each;
// started from cameras and advanced by the key 22; the camera byte offset named and added in the call 11 (the plane
// lookup is then retail's, the array still EDX); offset and pointer both named 22; with the key also named, or assigned
// inside the camera expression, 105 (forwarded back into the call); the call's key re-read through the pool array 21
// (diagnostic value: the array then takes ECX at the block top as in retail, the key is loaded twice). 1..7 dead locals
// on the 22, 13 and 11 forms: no change (not a numbering state). Diagnostics (restored): an extra store of the array
// ahead of the camera pointer 91, an extra store of the key 110, the count store after the volume pointer 87.
// Early camera advance 105, offset with count field 11, volume before commit 85, offset with
// volume array 11, offset with rectangle 11, offset with key field 11, portal displacement 105,
// offset with cluster join 11. NOT kept: the offset variants still push the key before completing
// the camera address, and rely on match-only bindings. The early volume address changes the branch layout.
// Shared camera key before volume selection 106; camera-record size as byte stride 105;
// key and offset before volume selection 106; table bound before the count with key read after it 106;
// fixed camera records and shared key 106; entry reference and shared key 106; volume
// reference with byte table and offset 105; bound volume count and shared key 106.
// NOT kept: the explicit key coalesces into the same multiply and push schedule.
// Camera byte base at reservation or entry selection 11/11. Native record base at those points 105/105, with
// one post-update key 106, or a record address before the volume 13. Byte offset with direct portal field 11 or
// before the volume 13. NOT kept: the 11-point forms still load the camera base late and exchange its register
// with the key. The one-use bindings and portal field reload require an exact match.
// Native portal record 105, with separate camera offset 11. Offset by native camera extent 11,
// byte subscript 11, output reference before count 84, direct count increment 11, selected plane
// with the saved or direct portal field 94/94. The signed camera offset by record extent as a
// one-use local gives plain 11 at 434 bytes; NOT kept at review: one-use locals need an exact
// match, and folded into the subscript it is plain 105 (the fast build gives 11 for both, so
// confirm these forms with the plain check). Retail multiplies the key before pushing it. NOT kept:
// the output reference changes the creation span, and the selected-plane forms change its two stores.
// M3 lane 3 r290 (H2M_FAST, control 105; the one-use camera offset local rebuilt, 11 at 434 B: left are the camera table
// in EDX loaded after the count store for retail's ECX at the block top, the key in ECX for retail's EDX, and the distance
// copied through EAX for retail's ECX): `&cameras[entry->key * 0x1bc]` in the call 105; the offset as `0x1bc * key` 11
// and typed `dword` 11 (both the offset lead's code); the key and the offset both named 105 (forwarded back); the portal
// index read from the entry at both uses (no local) 105; the distance through a local 105 (both the same code).
// Projection-typed output 11, output through identifier 11, volume reselected on success 13,
// early volume-array base with shared key 105, rectangle through leading coordinate 11, native
// count postincrement in the admitted arm 11, rectangle binding after displacement 11, and camera
// row before output selection 13. NOT kept: the gains rely on the one-use displacement or pointer
// and leave the camera table and key in exchanged registers.
// M3 lane 12 r172 (H2M_FAST, control 105): the camera address or the camera byte offset named ahead of the count store
// 13/13 (434 B; the key is then loaded before the store and again for the push, where retail loads it once after it);
// the volume address, then the camera address, then the count store 81 (collection leaves EDI); the key named after the
// volume address 105; the product as `entry->key * 0x1bc + cameras` 105; the camera table copied into a local, read once
// through its own address, or bound by const reference at the top of the admitted block 105 each (the copy folds and the
// table still loads into EDX after the count store).
// named projection result 105; projection result with separate camera displacement 11; selected camera
// record reference 22; byte camera record and named projection result 22; portal table in present arm
// 105; portal displacement in non-sentinel alternative 105; projection result with leading volume field
// 105; named projection result and portal table 105.
// NOT kept: The 11-point form requires the one-use camera displacement and still loads the base late,
// exchanges the base and key registers, pushes the key too early, and transfers distance through EAX.
// The 22-point forms require a one-use camera binding. The other forms reproduce 105.
// Native int camera displacement 11; camera row advanced from a shared key 105;
// entry address before capacity with separate displacement 127; reserved volume reference
// 11; positive capacity arm 11; pool array address with early displacement 13; const key
// field reference 11; native plane array width 11. NOT kept: the lower scores retain the
// one-use displacement and miss the early camera-table load and key/offset register schedule.
// Entry address before capacity also changes the caller.
// The camera base is still loaded late and the first push precedes the complete camera address. Indexed
// key 105, indexed key and bounds 106, read-only projection entry 105, camera row reference after or
// before the count store 13/13, leading record byte 105, count member reference 105, and count member with
// camera row reference 13. NOT kept: the 13 forms use a one-use camera binding and still exchange
// camera/key registers.
// Earlier at 105 (was 129).
// M3 lane 3 r299 (H2M_FAST with 0x1730a0, MATCH at every run; control 105, the one-use offset local rebuilt 11): the map
// parameter (retail's EDX) declared second, third or fourth among the stack parameters with the caller's call updated:
// 105 each, and 11 each on the offset form (the caller matches at every position, so the position does not order the
// camera sum). One to three dead initialised locals, or the key named, ahead of the call: 105 with the plane lookup
// unchanged. On the offset form: the camera table copied into a local ahead of the count store 11, bound by const
// reference there 11, copied and then bound 11 (all fold); bound by const reference with the product in the call 105.
// The key for the push or for the product read through pool->entries[entry_index] 13/13 (two loads). Reading: the
// scratch registers go round EAX, ECX, EDX in evaluation order; retail evaluates the table before the key (ECX, then
// EDX) and ours the key first, the same reversal as 0x172840's recount. The camera record through a forced-inline
// accessor in the call, parameters (table, key) or (key, table): 105 each (folds into the call's push order).
// M3 lane 7 r220 (H2M_FAST with 0x1730a0, MATCH at every run; control 105; leads re-measured: offset local 11, camera
// address local after the volume address 22, between the count store and the volume address 13). Reading: in retail the
// camera sum is complete before the key is pushed, with the table in ECX from the block top and the key in EDX; every form
// below leaves the key pushed ahead of the add. A forced-inline projection helper around the 0x163db0 call taking (cameras,
// entry, volume), the same with the volume address formed in its argument, taking (volume, entry, cameras) or taking
// (cameras, key, rectangle, volume) by value: 105 each (same code). The camera record through a 0x1bc-byte record type
// indexed in the call 105; the table viewed as a record holding a member array, indexed in the call 105, named 22. The
// camera address local declared at function scope 22; bound by `const &` and passed through the reference, as bytes or
// as the typed camera pointer, 22/22; that with the key named too 22; the table bound by `const &` 22; the rectangle
// address or the projection result named beside it 22/22 (all the 22 lead's code). The key and the camera address both
// named 105; the key named for both arguments 105. The volume address as `volumes[volume_count++]` 115, with the camera
// address named 87. Diagnostic (restored): a second use of the camera address (stored into the volume) 149 at 454 B, the
// caller 2.
// On the separated camera-offset form: successor in for update 151, linked guard 151, projection
// guard 151, both guards 151, result-controlled while 11, post-tested successor assignment 11.
// Native camera byte array 105, with successor and linked guard 147. NOT kept: the 11 forms
// require an exact match for the one-use offset and still exchange the camera base and key registers.
// The for forms shorten the function and break the matched caller.
// M3 lane 5 r243 (reduced image with 0x1730a0; control 105, the one-use offset local before the call re-measured 11).
// Reading of the allocator on the 11 form: the key read twice from the entry is a CSE register candidate coloured
// early into ECX, so the camera table becomes a late scratch temporary in EDX that cannot rise above the count store;
// retail has the table in ECX from the block top and the key in EDX. With the key in EDX (diagnostic) it is 13: the table
// load is still a late scratch temporary (now one step later in the EAX, ECX, EDX cycle), so both the key's register
// and the scratch cycle have to move. The camera sum's operands sort offset first, table second (the table parameter
// ranks lowest), the same roles as retail's. On the 11 form: the offset local declared bare at the function top 11, at
// the top of the loop body 11 (its rank follows where it is assigned, not declared), assigned right after the entry
// pointer 82, assigned ahead of the volume address 13; a named `key` for the product and the argument 105 (with the
// offset local or without it: the key then stops being a CSE temporary and the plane lookup shifts as on this body).
// Volume selected from the stored count, a signed end index, a named end pointer or the stored end pointer: 106 each.
// The same four with a separate camera displacement score 84. NOT kept: the displacement has one use, the EDI save moves after the map-miss test and the camera table is still loaded late.
// Direct pool index 106, array base 105, const entry reads with indexed store 106, and bind after admission 106.
// With a separate camera displacement those forms score 17/11/17/17. NOT kept: the one-use displacement
// requires an exact match. The camera base still loads late, and the key is pushed before the address add.
// M3 lane 3 r308 (H2M_FAST with 0x1730a0, MATCH throughout). The table load waits for the count store because the
// scratch cycle gives it EDX, the register of the `volume_index + 1` value the store reads; retail's table is in ECX
// and rises to the block top, and its key then takes EDX. 60 forced allocator decisions leave 105. On the offset lead
// (re-measured 11): the volume address ahead of the count store 84; `camera_offset + cameras` 11; `NONE != portal_index`
// 11. Diagnostic: a second use of the table (a probe store) 91 at 438 B, the table still a plain load.
// M3 lane 8 r238 (H2M_FAST with 0x1730a0, MATCH each run; control 105). The allocator's view: on this body the key and
// the camera product are one candidate (the key is pushed before the multiply, so the product reuses its register); once
// the camera address or offset is computed first the key is its own candidate and takes ECX as the first free register,
// which leaves the camera table a late EDX temporary (retail: key EDX, table ECX from the block top). The projection
// result in a named bool 105; the camera table copied into a local at the loop body's top 105 (folds); the count store
// as `(short)(volume_index + 1)` 105; the capacity test as `!(volume_index < 512)` 105; camera and rectangle pointers
// both named after the volume address 22.
// @retail 0x172ee0
bool function_172ee0(long *cluster_map, s_pool_lists const *lists, s_pool *pool, byte const *cameras,
	long group, s_view_collection_1733e0 *collection, long entry_index, long cluster_index)
{
	bool result = true;
	long cluster_slot = cluster_map[cluster_index];
	s_view_cluster_1733e0 *cluster;
	if (cluster_slot != NONE)
		cluster = &collection->clusters[cluster_slot];
	else
	{
		short count = collection->cluster_count;
		if (count < 128)
		{
			cluster_slot = count;
			collection->cluster_count = count + 1;
			collection->order[cluster_slot] = cluster_slot;
			cluster_map[cluster_index] = cluster_slot;
			cluster = &collection->clusters[cluster_slot];
			cluster->index = (short)cluster_index;
			memset(cluster->counts, 0, sizeof(cluster->counts));
			memset(cluster->heads, 0xff, sizeof(cluster->heads));
		}
		else
			result = false;
	}
	if (result)
	{
		do
		{
			short volume_index = collection->volume_count;
			if (volume_index >= 512)
			{
				result = false;
				break;
			}
			s_pool_entry *entry = &pool->entries[entry_index];
			if (entry->unknown28 == NONE)
			{
				collection->volume_count = volume_index + 1;
				s_view_volume_1733e0 *volume = &collection->volumes[volume_index];
				if (function_163db0((s_camera_163db0 const *)(cameras + entry->key * 0x1bc), (box2f const *)&entry->rect, entry->key, (s_frustum_1648d0 *)volume))
				{
					volume->cluster_index = cluster_slot;
					volume->distance = entry->unknown24;
					entry->unknown0a = volume_index;
					short portal_index = entry->unknown06;
					if (portal_index != NONE)
						volume->plane_index = *(long *)(*(byte **)((byte *)g_4e0348 + 0x60) + portal_index * 0x24 + 4);
					else
						volume->plane_index = NONE;
					if (cluster->heads[group] == NONE)
						cluster->heads[group] = volume_index;
					++cluster->counts[group];
				}
			}
			entry_index = list_next(lists, (short)cluster_index, (short)entry_index);
		} while (entry_index != NONE);
	}
	return result;
}

PRIVATE __forceinline long list_head_index_1730a0(s_pool_lists const *lists, short index)
{
	if (short_list_used_173b00(lists->valid, index))
		return lists->heads[index];
	return NONE;
}

// @retail 0x1730a0
void function_1730a0(s_pool_lists const *lists, s_pool *pool, long *cluster_map,
	byte const *cameras, long group, s_view_collection_1733e0 *collection)
{
	long count = pool->count;
	s_pool_entry *entry = pool->entries;
	for (long index = 0; index < count; ++index, ++entry)
	{
		long cluster_index = entry->value08;
		if (index == (short)list_head_index_1730a0(lists, (short)cluster_index))
		{
			if (!function_172ee0(cluster_map, lists, pool, cameras, group, collection, index, cluster_index))
				break;
		}
	}
}

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline void rect4_union(s_rect4 *rect, s_rect4 const *other)
{
	real value;

	value = MIN(rect->x0, other->x0);
	rect->x0 = value;
	value = MAX(rect->x1, other->x1);
	rect->x1 = value;
	value = MIN(rect->y0, other->y0);
	rect->y0 = value;
	value = MAX(rect->y1, other->y1);
	rect->y1 = value;
}

static inline real rect4_area(s_rect4 const *rect)
{
	return (rect->y1 - rect->y0) * (rect->x1 - rect->x0);
}

// @retail 0x172c70
void function_172c70(s_pool_lists *lists, s_pool *pool)
{
	for (long list = 0; list < g_4e0348->list_count; list++)
	{
		long index = list_head(lists, (short)list);

		while (index != NONE)
		{
			s_pool_entry *entry = &pool->entries[index];
			long next = list_next(lists, (short)list, (short)index);

			while (next != NONE)
			{
				s_pool_entry *other = &pool->entries[next];
				s_rect4 merged = other->rect;

				rect4_union(&merged, &entry->rect);
				if (rect4_area(&merged) < (rect4_area(&other->rect) + rect4_area(&entry->rect)) * 1.5f)
				{
					entry->unknown28 = next;
					other->unknown24 = MIN(entry->unknown24, other->unknown24);
					other->rect = merged;
					break;
				}
				next = list_next(lists, (short)list, (short)next);
			}
			index = list_next(lists, (short)list, (short)index);
		}
	}
}

/* ---- the view setup ---- */

struct s_rotation_matrix
{
	real m00, m01, m02;
	real m10, m11, m12;
	real m20, m21, m22;
};


static inline void rotation_matrix_from_axis_angle(s_rotation_matrix *m, vector3f const *axis, real angle)
{
	real s = (real)sin(angle);
	real c = (real)cos(angle);
	real i2 = axis->i * axis->i;
	real j2 = axis->j * axis->j;
	real k2 = axis->k * axis->k;
	real one_minus_c = 1.f - c;
	real tij = one_minus_c * axis->i * axis->j;
	real tik = one_minus_c * axis->k * axis->i;
	real tjk = one_minus_c * axis->k * axis->j;

	m->m00 = (1.f - i2) * c + i2;
	m->m01 = tij - s * axis->k;
	m->m10 = tij + s * axis->k;
	m->m11 = (1.f - j2) * c + j2;
	m->m20 = tik - s * axis->j;
	m->m02 = tik + s * axis->j;
	m->m22 = (1.f - k2) * c + k2;
	m->m12 = tjk - s * axis->i;
	m->m21 = tjk + s * axis->i;
}

static inline void rotation_matrix_transform(s_rotation_matrix const *m, vector3f const *in, vector3f *out)
{
	vector3f copy;

	if (in == out)
	{
		copy = *in;
		in = &copy;
	}
	out->i = in->i * m->m00 + m->m02 * in->k + m->m01 * in->j;
	out->j = in->k * m->m12 + m->m11 * in->j + in->i * m->m10;
	out->k = in->j * m->m21 + in->k * m->m22 + in->i * m->m20;
}

/* the rotation of `angle` about `axis` */
static __forceinline void rotation_matrix_from_axis_angle_171d90(s_rotation_matrix *m, vector3f const *axis, real angle)
{
	real i2 = axis->i * axis->i;
	real j2 = axis->j * axis->j;
	real k2 = axis->k * axis->k;
	real s = (real)sin(angle);
	real c = (real)cos(angle);
	real one_minus_c = 1.f - c;
	real tjk = (one_minus_c * axis->k) * axis->j;
	real tik = (one_minus_c * axis->k) * axis->i;
	real tij = (one_minus_c * axis->i) * axis->j;

	m->m00 = (1.f - i2) * c + i2;
	m->m01 = tij - s * axis->k;
	m->m10 = tij + s * axis->k;
	m->m11 = (1.f - j2) * c + j2;
	m->m20 = tik - s * axis->j;
	m->m02 = tik + s * axis->j;
	m->m22 = (1.f - k2) * c + k2;
	m->m12 = tjk - s * axis->i;
	m->m21 = tjk + s * axis->i;
}

/* `in` rotated by `m` (in may be out) */
static inline void rotation_matrix_transform_171d90(s_rotation_matrix const *m, vector3f const *in, vector3f *out)
{
	vector3f copy;

	if (in == out)
	{
		copy = *in;
		in = &copy;
	}
	out->i = (in->i * m->m00 + m->m02 * in->k) + m->m01 * in->j;
	out->j = (in->k * m->m12 + m->m11 * in->j) + in->i * m->m10;
	out->k = (in->j * m->m21 + in->k * m->m22) + in->i * m->m20;
}

// score 334 (was 334): The 0x50 frame and retail length agree. Unity still starts in XMM7 instead of XMM5, and two first-axis squares spill.
// Earlier (346, was 346): Logic walked block by block against retail: every call, constant and field offset agrees.
// The sums' grouping differs: retail adds the rotation terms in a different order in each of the four
// rotations (first-axis forward 0x171f17, first-axis up 0x171fb2, second-axis forward 0x17219e, second-axis up
// 0x172233). For i it uses ((i,k),j) / ((k,j),i) / ((k,j),i) / ((i,k),j), for j ((k,j),i) / ((i,k),j) / ((k,j),i) /
// ((i,k),j), and for k ((j,k),i) / ((i,j),k) / ((j,k),i) / ((i,j),k); the helper uses the first form everywhere.
// A helper that picks the grouping per call reproduces all four (checked in our output) but scores 361.
// What still differs is register choice and scheduling: retail holds the constant 1.0 in xmm5 for the whole
// function and computes axis i2/j2/k2 before the first fptan, ours spills them; retail multiplies s*axis.i on the
// x87 stack (fld/fmul/fstp 0x171e56-0x171e6d) and the other products in SSE; the deviation sum is fld/fmul-per-term
// in retail, ours loads all three first.
// Tried (no gain, all 356-383): sk/sj/si locals before or after the squares (357-383), ratio/original/vertical
// statements moved (360-361), deviation as float sum (367), per-term (double)(a*a) (372), double locals (419).
// The first unity load and original-axis squares keep their opening registers. The frame still agrees.
// Shared cross 346; staged cross 350; unconditional input vector 370; input scalars 413;
// staged output scalars 465; complete output vector 396; native three-axis matrix 346; reciprocal-bound ternary 346.
// NOT kept: the four alias-free transforms also demote the matched view initializer in preview.
// Unity remains in XMM3, while retail uses XMM5 and carries the squares through the tangent.
// Squared-axis vector 387; squared axis in matrix diagonal 361; shared weighted k 376;
// weighted-axis vector 343; retained original forward scalars 378; one horizontal field read 379;
// vertical field result reused for the half-angle 363; sin and cos passed to the matrix builder 350.
// NOT kept: the weighted-axis vector gains three at 1453 bytes and leaves the unity register different.
// Unity and the first axis squares still have different registers.
// Tried second angle after the first matrix 367, second angle after the first complete rotation 386,
// second angle evaluated first 339, both atan results used at their matrix calls 402, earlier scaled
// vertical tangent with deferred arctangent 352, shared axis-angle builder for the world axis 391, shared
// axis-angle builder for the rotated axis 331, shared axis-angle builder for both axes 347.
// NOT kept: The common builder for the rotated axis reaches retail length but combines two axis components
// before the cosine factor where retail scales one component first. Reversing the angle definitions gains
// seven and shortens the body.
// Matrix returned by value 471 (initializer loses its match); shared alias-safe transform 361; staged basis transform 368.
// Axis-square diagonal accumulators 346; staged diagonal 363; separate scaled cosine terms 363.
// Original direction copied by memcpy 346; separate transform source pointer 383. Unity still starts in XMM3.
// Forced alias-safe transform 346; descriptive axis-square names 346; pair-local sine products 376;
// retained rotated-forward address 350; staged dot 346; staged double squares or interleaved deltas
// and squares 349 each; double deviation vector 392. NOT kept: the first unity register remains
// different, and the new deviation forms shorten the function away from retail.
// Shared sine-axis scale after or before cosine 360/358. Cosine-complement scale 331.
// Cross terms staged per pair 366. Grouped common builder for the rotated axis 351,
// with shared sine contributions 516. Rescaled original forward 349, with shared point difference 395.
// NOT kept: cosine-complement scaling holds the axis components live where retail reads them again.
// The retail second rotation scales j before multiplying by i. Preserving that grouping still differs.
// The first-axis squares still spill and unity still starts in XMM3. Squares passed as a vector before
// vertical calculation 343, before the half-angle 333, between angles 330; scalar square inputs
// before the half-angle 392, between angles 389; double angles 348; shared double-factor
// sine-axis scale 374; double diagonals 397.
// NOT kept: the 330 vector form reaches 1464 bytes but uses frame 0x4c and still spills two
// squares. The double-angle form also demotes the matched initializer.
// Named square inputs together 381 or paired with products 379. Sine products before or after
// complement products 354/343. Row aggregate 492; symmetric coefficients then aggregate 469.
// Early horizontal axis 381; named double vertical tangent 368.
// NOT kept: the 343 form gains three at 1465 bytes with frame 0x54, and unity moves to XMM6.
// Both aggregate forms also demote the matched view initializer. The original square spills remain.
// Original forward captured after the ratio, vertical field, both angles or first matrix
// 345/345/345/358. Forward component capture split around the ratio after i or j 367/367.
// Ratio computed before and stored after the capture 349. Shared forward address through both rotations 365.
// NOT kept: the one-point gains leave unity in XMM3. Split captures and the shared forward address
// change the matched initializer. The opening body is restored.
// The two axis squares still spill before the tangent. Float sine/cosine/both
// 362/362/372; float horizontal tangent 372; float vertical arctangent 337;
// float half-angle tangent 371; float scale arctangents 346; float tangent
// and arctangent in both scale paths 336. NOT kept: the float paths admit
// different precision boundaries. The two lower scores still spill two
// squares, and the 337 form adds a move while using XMM7 for unity.
// Common real unity for the two matrix expansions and inverse 363, also for the vertical
// arctangent 333, or separate matrix-local unity 345. Shared unity with staged cosine
// complement 339, grouped diagonal complements 402, symmetric field initialization 370,
// diagonal field accumulation 370, or separate cosine conversion 361.
// NOT kept: 333 still spills two axis squares and uses XMM7 for unity. The extra unity local
// and helper argument change allocation only. The separate cosine conversion loses the initializer.
// Sine j/k before cosine and i after; double i sine intermediate; descriptive trig names
// Axis reference, matrix output reference, immutable original forward, symmetric matrix
// with signed corrections, and the builder returning its output pointer: 370/364/346/346/346/346/395/346.
// NOT kept: Two first-axis squares still spill across the half-angle tangent and unity starts in XMM3.
// Unity and the first-axis squares still have different registers. Indexed axis squares 414,
// with a component local 354; scalar squares with a diagonal loop 403; both loops 365;
// complement loop before or after cosine 370/373; three loops 382; in-place diagonal loop 406.
// NOT kept: the new forms all change the 0x50 frame and raise the score.
// One-argument vertical field arctangent 351. Explicit denominators at
// horizontal/vertical/both scaled angles 373/373/368. With the one-argument
// vertical field these score 377/377/372. Naming that vertical ratio on
// the combined form gives 339. NOT kept: seven points, with unity in XMM2
// at entry against XMM5 and FLD1 replacing the vertical unity memory load.
// Unity still starts in XMM3. Forward-field pointer through deviation or only the rotations 355/357.
// Up-field pointer at first rotation or entry 362/361. Both pointers early or up bound later 360/360.
// Both field references or both pointers at first rotation 361/359. NOT kept: all regress, and six
// forms also demote the matched initializer.
// Named horizontal half-angle 346, projected half-angle before doubling 337 reduced and 334 plain,
// named vertical field with a following field read 349, both fields before publication 363,
// published half-angle expression at each use 337, scaled tangent accumulators 345, referenced
// horizontal field 356, shared original-direction setter 380.
// NOT kept: the repeated half-angle is 1454 bytes and duplicates a computed value. The original-direction
// setter demotes the matched initializer.
// Saved forward, rotation workspace, second axis and unrotated-copy names 337 each.
// Horizontal/vertical angle names, deviation component names, late matrix declaration,
// and second axis declared beside matrix 337 each (reduced control 337).
// The square spills and early EDI save survive all eight forms.
// Scaled k with compound scaling 349, after the first diagonal 361, or at each mixed pair 355.
// Separate sine conversion 364 or delayed past cosine 365. Component-first sine products 337.
// Signed pair accumulators 349; sine products at diagonal boundaries 343.
// NOT kept: two axis squares still spill. The separate sine conversions also demote the view initializer.
// The ratio publication and projected slope still do not recover the unity register.
// Named tangent before/after ratio 385/385, named slope before/after ratio 363/363,
// compound aspect correction with ratio before tangent or between stages 385/385,
// ratio store after projected angle 337 reduced, and retained ratio through slope 392.
// NOT kept: the best ties the 337 reduced control at retail length, still spills
// two squares, and leaves unity in XMM7. The other forms shorten the body.
// Axis-square constants 337; component square accumulators 339; staged diagonal contributions 381;
// pair-local cross products after their diagonal 357; square as first diagonal summand 337;
// diagonal-first publication 384; cosine bound to diagonal scope 384; joint trig declaration 337.
// NOT kept: reduced control 337, full 334. Unity and two axis-square homes remain different.
// Saved forward, rotation workspace, second axis and unrotated-copy names 337 each.
// Horizontal/vertical angle names, deviation component names, late matrix declaration,
// and second axis declared beside matrix 337 each (reduced control 337).
// The square spills and early EDI save survive all eight forms.
// M3 lane 3 r282 (H2M_FAST, reduced control 337 against full 334): 1 to 7 dead longs at the top
// 337/370/353/365/356/358/357 (the length moves, unity and the square spills stay); sine and cosine through
// an inline out-pointer helper 337 (one row's operand order moves); the squares written at their uses with
// no named squares 364 (1449 B); the axis components read once into locals 377 (1427 B); the second axis
// through cross3f_staged 339 (1456 B). KEPT: the second axis through the shared cross3f in place of the
// file's private copy of it (same code, 337 reduced; the copy removed).
// Paired sine/cosine fields or aggregate 364/364, staged scalar assignments 337,
// trig values rounded together 355. A scoped half-angle leaves each score unchanged.
// NOT kept: reduced control 337, full 334. The pair shrinks the frame and delayed rounding
// changes the matched initializer. Unity and two square homes still differ.
// Angle pair by aggregate or fields 378/378, with shared tangent 361 or produced vertical field 390.
// Horizontal or vertical conversion at the matrix call 368/395, double half-angle 345,
// and doubled projection by addition 337. NOT kept: the addition ties the reduced control.
// Pair forms enlarge the frame and all other forms shorten the body.
// Shared diagonal calculation direct, with a named complement, with a named scaled
// complement, or with a complement accumulator 339/356/386/386. With a shared
// signed-pair store 380/402/367/367. NOT kept: every form shortens the matrix
// construction and leaves the unity register and square spill different.
// M3 lane 13 r139 (main re-measured 334 plain): the axis rotation rewritten in the shape of the code base's other axis
// rotation (sine and cosine first, then the squares, a sine-axis vector, each off-diagonal pair after its diagonal, the
// (1-c)*k product shared) 302 fast; with retail's (1-c)*i*j grouping 299 fast but 367 plain (1487 B, frame 0x4c, the EDI
// save scheduled before the first rotation); with the complement declared before the first diagonal 298 fast, 376 plain
// (1493 B); with the matrix stored as forward/left/up (retail keeps tij - s*k at +0xc) 291 fast. In the fast image unity
// then takes XMM5 and the first rotation's squares stay in XMM0/XMM2/XMM7 as retail; the second rotation and the deviation
// tail differ. Dead initialised locals on the 291 form 282/277/346/352/274/274/291 for 1..7 (diagnostic; uninitialised ones
// and their position change nothing). Fast, on the 291 form: `limit` folded 367, with the vertical-angle local 356, the
// vertical-angle local alone 355, `limit` and `half` 321, both angle intermediates 291; helper locals tk 317, tij 380,
// one_minus_c 309; sine products j, k, i 341; squares as a vector 339; sine-axis vector before the squares 337. On main's
// body: the deviation as a component vector squared from its fields 361 fast (1438 B; retail's interleaved store, fld,
// fmul tail but in i, j, k order), the float sum of the named differences 339 fast.
// Vector diagonal complements: separate or in-place scale 395/408, separate or in-place
// scale and sum 407/382, scaled-add 396, complements before trig 390, aggregate
// complements 369, retained scalar squares with vector scaling 415. NOT kept: all enlarge
// the 0x50 frame and regress from the 337 reduced control. Unity and square homes still differ.
// Pass rounded sine at the matrix call, named there or named at angle production
// 363/330/378 against the 337 reduced control. Cosine counterparts 367/358/351.
// Pass cosine and its complement at the call or angle producer 350/376.
// NOT kept: the 330 form shortens to 1445 bytes, loads unity into XMM3 and
// still spills two squares. Cosine inputs grow the frame to 0x54.
// Diagonal-field squares before/after trig 370/337. Diagonal-field complements or cosine products 370/383.
// Symmetric positive-field sine or cosine contributions 345/350. Square plus sine fields 358, diagonal and
// symmetric cosine fields 380 (reduced control 337). NOT kept: the tied form grows to 1473 bytes, and the other
// allocations score higher. Unity and the two square spills still differ.
// M3 lane 8 r225 (h2m fast, reduced control 337): retail squares each difference as fld/fmul from its own spill,
// interleaved with the SSE rows; ours converts each once and squares on the x87 stack. The differences held in a
// vector with stores k, j, i / i, j, k / k, i, j and the squares summed k, j, i, or summed i, j, k: 353 each (1444 B,
// the tail still loads all three first); both sides cast to double 337; the differences as doubles 396 (1417 B);
// the squares summed x, y, z 337; the rows computed x, y, z 337. The matrix stored transposed (tij - s*k at +0xc as
// retail) with the transform reading it transposed 358 (1453 B); a second matrix for the second rotation, declared
// there or at the top, 344 (1442 B).
// M3 lane 5 r237 (H2M_FAST, reduced control 337): the file's other axis-angle builder and transform (the unused inline
// pair above the view-setup helpers) for the first rotation only 386 (1425 B), both builders 360 (1455 B), the four
// transforms 368 (1441 B), builders and transforms 359 (1473 B), the second builder only 352 (1464 B). Body unchanged.
// Scalar square complements before sine or between sine and cosine 349/349.
// Cosine-seeded diagonal accumulators 385; cosine-first products 337; early
// complement accumulators 389; early complements in diagonal fields 354;
// square-seeded cosine accumulators 381; early axis-derived complements 388.
// NOT kept: cosine-first ties the reduced control. The square spills and unity
// register remain different, and early field staging changes the frame.
// M3 lane 5 r243 (reduced image, control 337): the residue is mostly code shape: ours evaluates three diagonal terms
// `(1 - square) * c + square` on the x87 stack from spilled squares (fld unity, fsub, fmul, fadd) where retail keeps the
// squares in XMM0/XMM2/XMM7 and uses subss/mulss/addss; the unity constant's register alone moves it to 325. In the
// axis-angle helper: sine and cosine ahead of the squares 364, ahead of the squares with the complement before them
// 364, the squares in k, j, i order 368, cosine ahead of sine 361 (each with a 4-byte larger frame); the diagonals as
// `c * (1 - square) + square` 337 (the same code: the product order is canonical).
// Builder inline or automatic 761/761, automatic transform 337, both automatic 761
// (reduced control 337). Symmetric pairs before diagonals 359, negative or positive
// partners first 368/373, late second and third squares 375. NOT kept: the builder
// needs forced expansion in this image, and the coefficient forms change the frame.
// Transform row accumulators 390; partial sums 346; terms formed one row at a time 356; all terms before output 453; partial and final terms before output 395.
// Bounded alignment 337; separate reciprocal arms 352; named squared deviation 337. Reduced control 337.
// NOT kept: the tied forms leave the opening unity register and two squares spilled. The staged row terms increase the frame.
// M3 lane 3 r308 (H2M_FAST with 0x1726d0 and 0x170fd0, MATCH throughout; control 337, aligned 250): 50 forced XMM
// rankings and picks give 333 at best. The file's generic builder at both calls 360 (1455 B), the generic transform at
// the four calls 368 (1441 B), both 359 (1473 B); this builder's rows in retail's order (diagonal, complement, each
// pair's product beside its rows) 357 (1448 B), with the sine products named right after the sine 337 (frame 0x54).
// M3 lane 8 r238 (H2M_FAST with 0x1726d0 and 0x170fd0, MATCH each run; reduced control 337): the builder's sine and
// cosine as `sinf`/`cosf` 339 (1450 B); the two scale angles as `atanf(tanf(half) * scale)` 380 (1429 B); the matrix
// declared after the second angle 337 (same code); the transform's in-place copy taken from `*out` 351; the builder's
// three sine products named after the complement products 339 (1466 B).
// The fixed-axis unity register and two square spills still differ. Existing general builder for the fixed axis 386;
// general forward/up/both transforms on the fixed axis 385/364/379, or general builder and both transforms 356;
// general forward/up/both transforms on the moving axis 360/346/348. NOT kept: every form shortens the rotation and regresses.
// M3 lane 14 r157 (fast screens with 0x1726d0, 0x170fd0, 0x172c70, 0x173910, 0x171d00 and 0x1701f0, all MATCH; reduced
// control 337, plain 334, difflib-aligned 378 plain). Forms: the closing dot through dot3f 342 (1453 B); the matrix as
// the existing matrix3x3 (forward, left, up) with the existing function_2477b1 transform 367 (1441 B), with
// vector3f_rotate_into_axes 377 (1426 B); the matrix as matrix3x3 with this file's transform and sums unchanged 337 fast,
// 334 plain (aligned 357 against 378: every matrix element then sits at retail's offset, tij - s * k at +0xc, but the
// code is otherwise the same; held as a patch); on it tij formed first 348
// (1455 B), with the shared (1 - c) * k named 357 (1431 B); tij first on main's matrix 348. Body unchanged.
// @retail 0x171d90
void function_171d90(s_view_setup *view)
{
	vector3f original = view->forward;
	s_rotation_matrix m;

	view->ratio = view->field_of_view / g_54e854;
	double projected_half = atan2((double)(tan(view->field_of_view * 0.5f) / view->aspect), 1.0);
	view->vertical_field_of_view = (real)(projected_half * 2.0);

	real half = view->vertical_field_of_view * 0.5f;
	real angle_a = (real)atan(tan(half) * view->scale_x);
	real angle_b = (real)atan(tan(half) * view->scale_y);

	rotation_matrix_from_axis_angle_171d90(&m, g_4687b0, angle_a);
	rotation_matrix_transform_171d90(&m, &view->forward, &view->forward);
	rotation_matrix_transform_171d90(&m, &view->up, &view->up);

	vector3f axis;
	cross3f(&view->forward, &view->up, &axis);
	rotation_matrix_from_axis_angle_171d90(&m, &axis, angle_b);
	rotation_matrix_transform_171d90(&m, &view->forward, &view->forward);
	rotation_matrix_transform_171d90(&m, &view->up, &view->up);

	real dot = view->forward.j * original.j + view->forward.k * original.k + view->forward.i * original.i;
	real limit = 0.0001f;
	if (!(dot > limit))
		dot = limit;
	real inv = 1.f / dot;
	real dz = original.k * inv - view->forward.k;
	real dy = original.j * inv - view->forward.j;
	real dx = inv * original.i - view->forward.i;
	view->deviation = (real)sqrt((double)dz * dz + (double)dy * dy + (double)dx * dx);
}

void function_11bed0(s_location *location, point3f const *point);

// @retail 0x1726d0
void function_1726d0(s_view_setup *view, point3f const *position, vector3f const *forward, vector3f const *up)
{
	memset(view, 0, sizeof(*view));
	view->position = *position;
	view->forward = *forward;
	view->up = *up;
	view->field_of_view = g_54e854;
	view->aspect = 1.3333334f;
	function_11bed0(&view->location, position);
	function_171d90(view);
}

struct s_polygon_173910
{
	long field_0;
	long plane_index;
	byte unknown08[0x14];
	long count;
	point3f const *points;
};

struct s_polygon_context_173910
{
	long field_0;
	struct s_planes
	{
		byte unknown00[0xc];
		plane3f const *planes;
	} *geometry;
	struct s_origin
	{
		byte unknown00[0x60];
		point3f position;
	} *origin;
};

short function_120850(vector3f const *v);
bool function_23a220(point2f const *points, short count, point2f const *point, real epsilon);

// Retail makes the side a 0/1 integer, tests its low byte at once and forms the table index from that test
// later (`mov eax, 1` / `xor eax, eax`, `test al, al`, `setne cl`): a one-byte 0/1 converted to bool.
// @retail 0x173910
bool function_173910(s_polygon_context_173910 const *context, s_polygon_173910 const *polygon)
{
	point2f points[128];
	plane3f const *plane = &context->geometry->planes[polygon->plane_index];
	short axis = function_120850(&plane->n);
	bool use_positive_side = (byte)(plane->n.n[axis] > 0.0f ? 1 : 0) != 0;
	s_polygon_context_173910::s_origin const *origin = context->origin;
	real distance = 0.0f - plane_distance_to_point(plane, &origin->position);
	point3f projected;
	projected.x = plane->n.i * distance + origin->position.x;
	projected.y = distance * plane->n.j + origin->position.y;
	projected.z = plane->n.k * distance + origin->position.z;
	point2f point;
	point.x = projected.n[g_440b94[axis * 2 + use_positive_side][0]];
	point.y = projected.n[g_440b94[axis * 2 + use_positive_side][1]];
	for (long i = 0; i < polygon->count; ++i)
	{
		point2f *destination = &points[i];
		point3f const *vertex = &polygon->points[i];
		destination->x = vertex->n[g_440b94[axis * 2 + use_positive_side][0]];
		destination->y = vertex->n[g_440b94[axis * 2 + use_positive_side][1]];
	}
	return function_23a220(points, (short)polygon->count, &point, 0.0001f) != false;
}

/* how the context's origin sees a polygon that keeps `state` points after clipping: 0 = nothing to show,
   1 = from behind its plane, 2 = from in front, 3 = from within 1/64 of the plane or with fewer than three
   points left; *inside is set when the origin is that close and lies in the polygon */
// @retail 0x173890
long function_173890(s_polygon_context_173910 const *context, s_polygon_173910 const *polygon, long state, bool *inside)
{
	plane3f const *plane = &context->geometry->planes[polygon->plane_index];
	s_polygon_context_173910::s_origin const *origin = context->origin;
	real distance = plane_distance_to_point(plane, &origin->position);
	long result;
	if (fabs(distance) > 0.015625)
	{
		*inside = false;
		if (state <= 0)
		{
			result = 0;
		}
		else if (state >= 3)
		{
			result = distance < 0.0f ? 1 : 2;
		}
		else
		{
			result = 3;
		}
	}
	else
	{
		*inside = function_173910(context, polygon);
		if (!*inside && state <= 0)
		{
			result = 0;
		}
		else
		{
			result = 3;
		}
	}
	return result;
}

struct s_projected_polygon_173520
{
	long field_0;
	byte state;
	byte unknown05;
	short field_6;
	short field_8;
	short unknown0a;
	real distance;
	box2f bounds;
	short count;
	short unknown22;
	point2f *points;
};

struct s_polygon_cache_173520
{
	struct s_geometry
	{
		byte unknown00[0x60];
		s_polygon_173910 *polygons;
	} *geometry;
	s_polygon_context_173910::s_planes *planes;
	byte *view;
	plane3f field_c_8;
	dword visited[16];
	s_projected_polygon_173520 polygons[512];
	long point_count;
	point2f points[5120];
};

box2f g_5476cc;
box2f *g_4687dc = &g_5476cc;
int __stdcall function_1429d0(transform4x3f const *matrix, long count, point3f const *source, point3f *destination);
long function_11fc80(plane3f const *plane, bool keep_inside, real tolerance, long point_count, point3f const *points, long maximum_count, point3f *out);

struct s_point_pool_173520
{
	long count;
	point2f points[5120];
};

// Matches (m3 lane 14 r154, on rounds 286 and 237's lead). Each part is needed; the count beside it is this body
// without it: the clipped vertices indexed by an ascending index beside the down-counter (with a walking source
// pointer its address is set up ahead of the destination stride's spill, 2); the reservation staged through
// `allocated` (retail commits the pool count and tests the state before it publishes the pointer; 4); the cap's
// compare read through a reference to the distance (retail reloads the winner; 15); the corners read through a
// second reference to the bounds (retail copies the inflated bounds back out of memory; 9); the recount's count
// widened through `long const &` (retail's points in ESI and count in ECX; 156).
// @retail 0x173520
s_projected_polygon_173520 *function_173520(long polygon_index, s_polygon_cache_173520 *cache)
{
	point3f transformed[64];
	point3f clipped[64];
	s_projected_polygon_173520 *result = &cache->polygons[polygon_index];
	dword bit = 1 << (polygon_index & 31);
	long word_index = polygon_index >> 5;
	if (!(cache->visited[word_index] & bit))
	{
		if (5120 - cache->point_count >= 64)
		{
			s_polygon_173910 const *polygon = &cache->geometry->polygons[polygon_index];
			function_1429d0((transform4x3f const *)(cache->view + 4), polygon->count, polygon->points, transformed);
			long count = function_11fc80(&cache->field_c_8, true, 0.0078125f, polygon->count, transformed, 64, clipped);
			bool inside;
			result->state = (byte)function_173890((s_polygon_context_173910 const *)cache, polygon, count, &inside);
			result->field_6 = ((short const *)polygon)[0];
			result->field_8 = ((short const *)polygon)[1];
			result->distance = FLT_MAX;
			box2f &bounds = result->bounds;
			bounds = *g_4687dc;
			s_point_pool_173520 *pool = (s_point_pool_173520 *)&cache->point_count;
			point2f *allocated = &pool->points[pool->count];
			pool->count += 64;
			result->points = allocated;
			result->count = (short)count;
			result->field_0 = *(long const *)((byte const *)polygon + 0x18);
			if (result->state)
			{
				long index = result->state == 1 ? count - 1 : 0;
				long step = result->state == 1 ? -1 : 1;
				long vertex_index = 0;
				long remaining = count;
				if (remaining > 0)
				for (; remaining != 0; --remaining)
				{
					point3f const *vertex = &clipped[vertex_index];
					point2f *point = result->points + index;
					real inverse = -1.0f / vertex->z;
					point->x = vertex->x * inverse;
					point->y = vertex->y * inverse;
					if (bounds.x0 > point->x)
						bounds.x0 = point->x;
					if (point->x > bounds.x1)
						bounds.x1 = point->x;
					if (bounds.y0 > point->y)
						bounds.y0 = point->y;
					if (point->y > bounds.y1)
						bounds.y1 = point->y;
					real distance = 0.0f - vertex->z;
					result->distance = result->distance > distance ? distance : result->distance;
					++vertex_index;
					index += step;
				}
				if (result->state == 3)
				{
					if (inside)
					{
						real const &distance = result->distance;
						result->distance = (real)(distance > 0.015625 ? 0.015625 : result->distance);
						result->bounds = *(box2f *)(cache->view + 0xa0);
						result->count = *(short *)(cache->view + 0x198);
						memcpy(result->points, cache->view + 0x19c, *(long *)(cache->view + 0x198) * sizeof(point2f));
					}
					else
					{
						bounds.x0 -= 0.00390625f;
						bounds.x1 += 0.00390625f;
						bounds.y0 -= 0.00390625f;
						bounds.y1 += 0.00390625f;
						box2f const &inflated = result->bounds;
						point2f *corners = result->points;
						corners[0].x = inflated.x0;
						corners[0].y = inflated.y0;
						corners[1].x = inflated.x1;
						corners[1].y = inflated.y0;
						corners[2].x = inflated.x1;
						corners[2].y = inflated.y1;
						corners[3].x = inflated.x0;
						corners[3].y = inflated.y1;
						result->count = 4;
					}
				}
				if (cache->view[0x84] && result->distance > *(real *)(cache->view + 0x88))
					result->state = 0;
				long const &used = result->count;
				pool->count = (result->points - pool->points) + used;
			}
			cache->visited[word_index] |= bit;
		}
		else
			return 0;
	}
	return result;
}


struct s_cluster_portals_172840
{
	byte unknown00[0x8c];
	long portal_count;
	short const *portals;
	byte unknown94[0xb0 - 0x94];
};

void function_11f5f0(box2f *bounds, point2f const *points, long count);

/* the pool's point storage as 0x172840 addresses it: the used count and the points after it */
struct s_point_storage_172840
{
	long used;
	point2f points[5120]; /* the pool's real data[10240] */
};

struct s_pool_view_172840
{
	long count;
	s_pool_entry entries[256];
	s_point_storage_172840 storage;
};

#define POINT_STORAGE_172840(pool) (&((s_pool_view_172840 *)(pool))->storage)

// Matches (m3 lane 14 r154, on round 160's store-order lead): the new entry's key, link, portal and cluster are
// stored in the record's field order (retail loads the key ahead of the link; in the order link, key, cluster,
// portal: 55 at 870 B), and the recount's count is widened through `long const &` as in 0x173520 (retail's points in
// EDX and count in ECX; read directly: 17, with the stores, the copy's set-up and the recount pair out of order).
// score 55 (was 134): The 0x30 frame and flag homes agree. The link and key scratch registers, signed cluster publication and sentinel encoding still differ.
// Earlier at 55 (was 134).
// Earlier at 134 (was 134). The result and capacity byte homes, signed cluster spill and sentinel publication still differ.
// The selected cluster and portal stores exchange scratch registers with the link and key, and
// the sentinel load adds two bytes to the publication block.
// The 0x30 frame and all four entry flag homes agree. The first difference at +0x24 is the
// empty-queue branch span. The successful clip publication uses CX for the predecessor where retail uses DX and
// leaves ECX for the key. The retail listing carries the destination rectangle address across clipping,
// template publication and bounds calculation. The trials test whether that address and the existing input
// rectangle bindings change the scratch-register allocation at the first stores.
// Earlier at 134 (was 134): The result and capacity byte homes, signed cluster spill and sentinel publication still differ.
// only the publication block differs (870 B against 868): its first scratch load takes CX where retail's
// takes DX (the link value; the key then ECX), so the stores reorder and the sentinel is a five-byte load. Earlier (134,
// was 205): The result and capacity byte homes, signed cluster spill and sentinel publication still differ.
// The 0x30 frame and early AL/ESI setup agree. The result and flag homes, signed cluster spill and false return
// still differ.
// Earlier comparison at 221.
// the block-by-block walk against retail 0x172840-0x172ba1 found no behaviour difference
// (ring-buffer pop and wrap, cluster/portal loop, the four skip tests, both rectangle tests, the pool/point limits,
// the 173380 clip, the stores into the new entry in retail's order, the 173b00/173b60 calls, the flag tail).
// Remaining: retail saves esi at entry (we push all four registers at the loop), so every [esp+N] differs; retail's
// spill slot for the other cluster (movsx'd, [esp+0x3c]) is the last one while ours is the second ([esp+0x18], with a
// zero-extending xor/mov); the g_4687dc rectangle copy is scheduled earlier in retail. Tried: other_cluster as long
// (236), as a ternary short/long (240/246), no variable with next->value08 reused for the 173b00 call (257/259),
// an extra short copy widened to long (246).
// b8A (11-17) walk against retail, fast build on r836: no logic difference found. Field offsets, the four skip tests,
// the four rectangle jb (0x1729a1-0x1729cd skip on below or unordered, as ours), the pool/point limits, the store
// order, the flag tail and the register arguments left from earlier instructions (173380: eax/ecx from 0x17299b and
// 0x17299e, ebx from 0x172a20; 173b60: esi from 0x172add) agree; there are no float constants or x87 rounding here.
// Not matched: the entry block (push esi at 0x172849 before the cmp, `mov al, 1` at 0x17284d feeding the result
// store, the zero-iteration je landing at 0x172b9d past the reload). Three forms together give every stack slot and
// the size (868) of retail: `if (polygon) { ... } else projection_full = true` (retail's block order 0x172b20,
// 0x172b39, 0x172b40); a `dword *` for next->unknown2c (retail reuses the lea at 0x172a9b as the base of the or at
// 0x172aae); `long other_cluster = field_6 == cluster ? field_8 : field_6` (widened once: movsx 0x172a62, spill
// 0x172a6d).
// Together 241 (opcode alignment 0.875, was 0.792); alone 248, 221 and 246. The row score drifts after retail's push
// esi, so they are not kept. An early `return true` before the flags: 139, frame 0x34 and a branchless tail (setne/
// dec/and) against retail's three test/je, not kept. result as byte/long, flags reordered, storage declared later: no
// change.
// single result from the three failure tests 205;
// Plain check: 205.
// Chained failure-byte initialization after the result 205.
// Chained failure-byte initialization before the result 205. Positive polygon path 143.
// Separate exclusion masks 205. State-specific cluster tests 205. Separate pool capacity guards 210.
// Positive pool capacity branch 205. Queue failure first 205.
// Positive polygon with signed cluster result (NOT kept, 873 bytes) 149.
// Positive polygon with shared bit-array base 134.
// Plain confirmation gives 134 at 868 bytes. The null-projection branch now reaches its own late failure
// store, and the copy and bit set reuse the same destination base as at retail 0x172a9b–0x172aae. All 36
// checked functions preserve their status, with no score rise elsewhere. The flag homes and false-return
// tail still differ.
// Independent pool/projection assignments with direct queue failure or success return 221/221;
// conditional return 226; pool/projection else-if with direct queue failure 215; carried result with
// queue predicate 227; combined pool/projection with direct queue failure 219; independent tests
// with projection before queue or first-write initialization 221/221; four flag names reordered 205.
// NOT kept: the return variants lose the early ESI save, and renaming leaves the flag homes in place.
// Independent failure stores with wide selected cluster 151/151, selection at publication 57,
// list insertion from the published field 150, wide-to-narrow sentinel read 49, selection
// after link/key 152, narrow selection with late extension 55, narrow-to-wide sentinel read 151.
// NOT kept: the 49 form adds a sentinel reload and five bytes. The 55 and 57 forms add two bytes.
// The independent failure stores recover the initial flag homes, but the publication schedule differs.
// Queue return first 246; boolean AND result 208; positive pool, projection or queue predicate 236 each.
// Three independent assignments 152; combined geometry failures 150; geometry within queue branch 148.
// Two geometry assignments and direct queue return 243. Widened cluster at publication 86 with the chain, 55 with independent tests.
// Chained sentinels 55/57; native portal width 239; portal ID local 55; widen at branch join 55.
// Native queue index, named point index or earlier bounds pointer 55 each.
// NOT kept: widening with independent tests restores the flag and spill homes at 870 bytes. Portal store first reaches
// 19 at 868 bytes, with the portal and cluster stores reversed from retail.
// The result and flag homes differ, and the widened cluster must stay live through the ordered entry stores. Short
// field plus separate extension 152, early predecessor store 60, key after both identifiers 31, rectangle memcpy
// 55, narrow predecessor local 55, widen after both identifiers 152, entry pointer before count 60, clipped count
// first 55, common bounds destination 55, key before predecessor 53. NOT kept: the 31 form publishes the cluster
// before the key. The 60 forms move the link or reservation sequence and the 53 and 55 forms grow to 870 bytes.
// Independent failure stores with widened publication and reused polygon, entry, or both clip bounds 55/55/55.
// Clipped end pointer 124. Shared clipped count 115. Shared clipped points 54. Parent bitset base 203.
// Signed allocated entry index 55. NOT kept: shared count removes a retail reload, while the shared point
// base and the 55 forms grow the body. The sentinel producer and publication schedule still differ.
// Completion or all capacity names 134/134. Count committed from the checked index 55,
// scoped storage receiver 55, native pool fields 254, rollback from allocation 249,
// shared point index 60, explicit cluster selection arms 55.
// NOT kept: the 55 forms recover the four flag homes but add two bytes and still
// schedule the cluster store and sentinel differently. The body stays at retail length.
// With independent failure stores, widen selected cluster after key or after link 55/55;
// narrow explicit arms 133; box-typed template copy 55; native predecessor or published index 55/55;
// reservation subscript postincrement 228; unsigned selection with signed publication 55.
// NOT kept: the 55 forms recover the flag homes at 870 bytes but still place the sign extension
// ahead of the retail link and key schedule and load the sentinel with a five-byte mov.
// With independent failures and signed publication, template reference after predecessor,
// key or both identifiers 55/55/55; direct buffer publication before reservation 140;
// named wrapped enqueue index 55; full queue arm first 59; arena reference 55;
// unsigned portal bit 55. NOT kept: the 55 forms recover the flag homes at 870 bytes,
// with the key and list-index schedule still different and a five-byte sentinel load.
// One state read 153, separate exclusions 153, branch admission 196, both sets separate 196, grouped per state 153.
// Selected exclusion mask 217, traversability conditional 207, one state read with independent limit results 249.
// Chained narrow publication then signed index 134, signed index first 86, with independent limits 243/241.
// NOT kept: the 86 form shrinks to 860 B and retains the displaced flag homes. Independent limits recover 868 B at 241.
// Pool/queue/projection failures as bytes 134/134/134, all unsigned or signed failure bytes 134/134,
// widened cluster producers and independent result tests 145, signed ring entry with original result
// tests 134, shared pool entry base 266. The widened form restores the result and flag byte homes, then
// grows to 873 bytes and changes the cluster select.
// Separate queue result store 146; separate pool result store 150; return the failure complement
// 241. With independent failure stores and signed list index: signed current cluster 145;
// select the cluster field before its signed read 128; published entry reference 145;
// separate head capture and increment 145; read-only projection 145. NOT kept: the field
// selection gains six, grows by one byte and loads through the selected address where retail
// selects the short value before extending it.
// flags before result 59, byte result 241, result before storage 55, conditional cluster selection 55,
// allocation pointer before reservation 140, single queue-wrap store 235, head shared with guard 251, wrap
// branch before increment 90.
// NOT kept: The 55 forms recover the flag homes at 870 bytes, with a different cluster store schedule and five-
// byte sentinel load.
// With independent failure stores and a signed destination: reserved index for success,
// rollback or both 142/135/255. Reserved address for success or both 149/260.
// Committed reservation end 260. Joined bitset rejection 55. Point-capacity upper bound 248.
// NOT kept: the 55 form grows to 870 bytes with the key scheduled late and a five-byte
// sentinel. Keeping the reservation index for rollback removes a retail pointer reload.
// M3 lane 12 r160 (fast; control 134): the result flag renamed to sort first or last (five names) 134 each. Re-measured:
// independent failure tests 152, the long ternary selection 86, both 55 (870 bytes; every home and the flag tail then
// agree, only the publication block's schedule differs). On the 55 form, all 60 orders of the selection and the next,
// key, cluster and portal stores (selection first among the cluster store's inputs): best 17 at 868 bytes (key, next,
// portal, cluster), retail's store order 55; the field order of the matched 0x172bb0 (key, next, 06, 0a, 28, cluster)
// with the selection at each position, best 19. On the 17 form: the bit-set copy without its pointer local 109, the
// pointer taken from memcpy's return value 17 (same code), the storage-used sum count-first or with casts 17; the
// permuter (600 s from the 17 form) found nothing lower. Left on 17: the key/next load order, the copy's esi/edi setup
// order and the data/count scratch pair in the storage-used sum. Nothing kept (width and store order match-only).
// With independent failure stores and signed publication, dequeued-entry, selected-cluster,
// projection, or both traversal references 55 each. Implicit cluster narrowing 55. Direct widened
// ring subscript 226, retained portal count 230, shared portal bit tests or tests and set 55 each.
// NOT kept: the 55 forms still grow to 870 bytes and keep the long sentinel load and
// displaced key store.
// With signed list publication: native short cluster store 152, queue failure or
// success return 241 each, queue result AND 250, pool then grouped projection/queue
// result 78, geometry chain then queue 72, positive capacity return 255, and
// direct pool-derived result 261.
// NOT kept: the 72 form recovers the flag homes, but shares a geometry false
// store that retail emits twice. The 78 form groups projection with queue handling.
// With independent failures and signed publication, failed clip first or rollback with continue 96/96;
// incoming cluster on the left of the comparison 56; captured short selection by equal or unequal arm
// 55/133; positive reservation block 55; continue on full queue 59; named publication key 55. NOT kept:
// the 96 forms reorder the clip arms and grow one byte; the 55–59 forms grow two bytes and retain the
// five-byte sentinel load.
// Independent failure stores with the link after the first cluster read and key after/before
// short selection, branch/conditional: 133/57 and 133/57. Link before the read: 60/60 and
// 129/60. NOT kept: the 57 forms grow to 870 bytes. The 60 forms recover the short sentinel
// at 868 bytes but publish the link and cluster ahead of their retail positions.
// Separate x/y overlaps, four independent edges, leading edge guard, or trailing edge guard:
// 134 each. With independent failure stores and signed cluster publication: 55 each.
// NOT kept: The 55 forms restore the flag and traversal homes but grow to 870 bytes,
// publish the key late and retain the five-byte sentinel load.
// M3 lane 13 r131 (fast, then plain with the file's 33 functions; base 134): KEPT the three failure tests as independent
// `if`s (retail's tail is three separate test/store sequences: pool, projection, queue) with the other cluster chosen by
// one ternary into a `long` (retail widens it once, spills the dword and reloads it for the list call): 55 (870 B); every
// row outside the publication block is then retail's but for jump targets. Alone: independent tests 152 (878 B); with a
// long set by an `if` 151; a short ternary 150; a short `if` selection widened right after it 55; widened after the key
// store with value08 from the long 55 (ternary or `if`), value08 from the short and widened after it 150/152; the key
// store written ahead of the link store 53 (leaves retail's store order); an inline other-cluster helper (ternary or `if`
// form, into a short or a long) 55/55/55; a second read of field_6 in the compare 55; the cluster on the left of the
// compare 56; a named bool then a ternary 55; inline by-value setters for link/key and cluster/portal 152 (short
// cluster) / 55 (long), one setter for all four 55 / 152; the key read into a local ahead of the link store 55 (both
// selection forms; propagated). Diagnostics (restored): one extra load/store statement between the selection and the
// link store gives retail's DX link, ECX key and `or eax, -1` sentinel (126, the stores then reorder); ahead of the
// selection it changes nothing (138). So retail has one more scratch temporary at the start of the publication block.
// Parent and reservation names 55, camera and destination names 55, selected short from shared
// side 150, ordered member receivers 55, signed parent and camera 55, bit copy return 55,
// early rectangle value 271, clipped-end recount 124. NOT kept: The 55 forms retain the
// two extra bytes and the publication schedule. The other forms grow the code.
// Widen selected short after rectangle/distance or bitset 150/150;
// narrow linkage sentinel after rectangle/distance 54. On the portal-before-cluster form:
// widen after rectangle/distance 150; shared source bitset 201; shared clipped points 41;
// shared clipped count 108. NOT kept: 41 reaches 868 bytes but reverses the retail cluster
// and portal stores and shares a point pointer retail reloads. The one-point gain leaves the
// five-byte sentinel.
// Default rectangle snapshot before linking or before the key store 272/271.
// NOT kept: both grow the frame and body.
// Carried rectangle pointer/reference 55/55, clip bounds receiver 55, clip bounds and point receiver 58, both
// carried input rectangles 55, entry or polygon input with the destination pointer 55/55, and both inputs with
// the clip output 55. NOT kept: the first publication scratches and five-byte sentinel stay different at 870
// bytes.
// Bounds template memcpy 55; byte-array bit map 55; complete clipped end index 55. On the
// short-sentinel form: template memcpy 17; byte-array map 17; scalar-pair recount 115;
// read-only point difference 17; named parent bitmap source 17. NOT kept: the 17 forms
// reach retail length with the short sentinel but retain the reordered publication,
// reversed copy setup and exchanged scratch registers in the recount.
// M3 lane 3 r290 (H2M_FAST, control 55; round 160's 17 form, key/next/portal/cluster store order, rebuilt 17 at 868 B with
// retail's key ECX, link DX, portal CX and `or eax, -1`; left: the stores' order, the copy's esi/edi set-up order and the
// recount's points ECX / count EDX for retail's EDX / ECX, the same swap as 0x173520's recount). Reading: the five-byte
// sentinel is `mov eax, -1` because EAX was last written by the 16-bit portal load; with the portal in CX the last write is
// the `movsx` and the short `or` follows. On the 17 form the recount as `pool->data_used = (next->data - pool->data) / 2 +
// next->count` 115 (878 B, cdq), with `>> 1` 42 (871 B: sar 3, but the base formed as `sub 0x6c08`); on this body the `/ 2`
// form 128 (880 B). On this body, read ahead of the link store and used at its place: the polygon distance 62 (874 B), the
// default rectangle pointer 55, the new entry's portal-bit pointer 55.
// Key and predecessor staged together 55/55, shared signed predecessor 55, bound predecessor bitmap
// array 203. On the reordered short-sentinel form: signed byte-offset recount 17, native arena origin
// 42, staged key and predecessor 17, earlier predecessor bitmap pointer 22. NOT kept: the 17 and 22
// forms reach retail length but keep the reordered publication and the exchanged recount registers.
// M3 lane 12 r172 (H2M_FAST, control 55; round 160's 17 form rebuilt, 17 at 868 B): the rectangle copy written ahead of
// the link and key stores 34 (868 B; the copy is then emitted first and the link store last); the two NONE stores first
// 34; the distance store first 49; with the copy first, all 24 orders of the link, key, cluster and portal stores, best
// 30 (868 B, same reading). The bit vector set through next->unknown2c directly 121 (on the 17 form 109); its pointer
// declared after the copy 55 (17 form 17, same code); the 64-byte vector copied as an aggregate 17 on the 17 form (same
// code); the copy through a forced-inline memcpy wrapper that sets up the source first (the form of the copy helper in
// src/unknown_1932c0.cpp) 55 / 17 (same code: `mov edi, eax` still precedes `add esi, 0x2c`). NOT kept: store orders
// are match-only.
// cluster field selection reused for list insertion 153; explicit cluster field cases 143; box-typed
// default copy 55; bound box used for default and update 55; clip output bounds member 55; recount by
// subtracting the reverse displacement 122; commit entry count from captured index 55; named clipped
// count 55.
// NOT kept: The bounds views, captured entry count and named clipped count reproduce 55. The signed-
// field selection changes the frame and removes the retail list argument lifetime. The reverse
// displacement grows to 872 bytes. The predecessor/key allocation and long sentinel encoding remain on
// the opening body.
// Read-only dequeued entry 55; allocated entry reference 55; narrow opposite endpoint update 55;
// const narrow endpoint with late widening 55; named clip result 55; native clip views for both
// inputs and output count/points 256; reservation counter postincrement after capacities 55;
// default bounds by native fields 56. NOT kept: the native clip views move the frame. The native
// bounds fields remove one byte and still load the link and key into exchanged scratch registers.
// The successful-clip publisher still widens the cluster before the key store and gives AX to the portal.
// Signed cluster from its field immediately after its store 19, after both IDs 71, or after both unset
// links 71; direct field selection 59, branch field selection 137, chained short 150, and signed branch
// join with a named predecessor or insertion short 151/151. NOT kept: the 19 form uses a one-use field
// binding and emits cluster first and key after portal, whereas retail publishes link, key, cluster,
// portal.
// M3 lane 3 r299 (H2M_FAST, control 55; round 160's key/next/portal/cluster store order rebuilt, 17). Reading of the
// publication block: the scratch registers go round EAX, ECX, EDX in evaluation order, so retail evaluates the key before
// the link, the portal before the widening, the copy's source before its destination and the entry count before the point
// difference; ours takes each pair the other way round. Two by-value forced-inline setters (link and key, cluster and
// portal) 55 (same code). 1-10, 12, 14, 16, 20, 24, 28, 32 and 40 dead initialised locals after the flags on the 17 form:
// 17 at every count; 4, 8, 12, 16 and 24 ahead of the storage pointer: 17 each (not a symbol-count state). The recount on
// the 17 form: storage through the macro at both uses 42 (871 B, the base folds to `sub 0x6c08`); the point difference
// named first 17; the count named first 17; `(data + count) - points` 106 (872 B, shl and lea); the difference stored and
// the count added by `+=` 105 (871 B); the count first with the difference cast 17; a byte-pointer difference shifted by
// 3 17; the difference or the count cast to unsigned 17/17/17; `data - (points - count)` 106; the difference, the whole
// recount (long count, short count or the entry as its argument) and the count or the count and points through
// forced-inline helpers 17 each (same code). Nothing kept.
// M3 lane 7 r220 (H2M_FAST, control 55; round 160's key/link/portal/cluster store order re-measured 17 at 868 B, with the
// two sentinel stores in field order 19). Reading of retail's block: the key is loaded (ECX) ahead of the link (DX); the
// link store goes out when EDX is taken for the default rectangle pointer and the key store when ECX is taken for the
// portal index; the selected cluster is widened only before its own store. A forced-inline by-value setter for the link and
// the key with parameters (entry, key, link) or (entry, link, key), the link passed as a short or as a long narrowed in
// the setter, the key parameter unsigned (a converting argument), or both by `const &`: 55 each (same code). A short copy
// of the dequeued value made where it is read, used for the link store, 55; the key parameter typed unsigned 55. The
// rectangle copy at each place among the six scalar stores (first to sixth; last is the order itself), for the orders
// (link, key, cluster, portal) 34 34 34 56 34 52; (key, link, portal, cluster) 34 33 32 32 28 29, copy last 17; (key,
// link, cluster, portal) 34 33 32 54 32 50, copy last 53; (link, key, portal, cluster) 34 34 34 34 30 31, copy last 19:
// none below the 17 lead (all 868 B but the 50 to 56 forms, 869 or 870 B).
// Native or positive clip status 56/56, short clip status 57; publish points through
// the clip output record 150, or use that record through rollback 256. Capture the
// bounds template before opposite-cluster selection or before clipping 55/55;
// snapshot the rectangle value before selection 152. NOT kept: the tying forms
// retain the 870-byte extent, exchanged scratch registers and long sentinel.
// M3 lane 5 r243 (reduced image, control 55; round 160's key/next/portal/cluster store order re-measured 17). Reading:
// in this block the stores follow the scratch registers' reuse (the key's ECX is reused for the portal, so the key store
// precedes the portal load); on the 17 form no single register or scheduling choice on its own goes below 17
// (diagnostic), so the store pair, the copy's ESI/EDI set-up and the recount's EDX/ECX pair are separate causes. On the 17 form: the cluster store ahead of the portal store 53 (870 B, five-byte sentinel); the
// bit-array pointer named ahead of the stores 33; the distance store ahead of the rectangle copy 53; the two sentinel
// stores swapped 19; the recount count-first through `&storage->points[0]` 17 (same code); the queue test written
// `== unknown00` with the arms swapped 22.
// Count-first recount and native successful or rollback point difference 55/55/55.
// Clipped base or count live across the bit copy 128/126, shared arena origin 275,
// immutable wide cluster 55, shared short list index 150. The key and predecessor
// allocation and long sentinel remain in the tying forms.
// Reserved pointer for recount 149, bounds 155, or both 148. Saved point start for commit 142 or both outcomes 255.
// Shared post-clip offset 235; recount through clip record 256; explicit inherited-bit assignment 55.
// NOT kept: retaining the reservation enlarges the frame, and the tied assignment still leaves the long sentinel.
// @retail 0x172840
bool __stdcall function_172840(long key, s_pool *pool, s_short_list_table *lists,
	s_ring_buffer *queue, s_polygon_cache_173520 *cache)
{
	s_point_storage_172840 *storage = POINT_STORAGE_172840(pool);
	bool result = true;
	bool pool_full = false;
	bool queue_full = false;
	bool projection_full = false;
	while (queue->unknown00 != queue->index)
	{
		long head = queue->unknown00++;
		if (queue->unknown00 == 256)
			queue->unknown00 = 0;
		long index = queue->values[head];
		s_pool_entry *entry = &pool->entries[(short)index];
		word cluster_index = entry->value08;
		s_cluster_portals_172840 const *cluster =
			&(*(s_cluster_portals_172840 **)((byte *)g_4e0348 + 0xa0))[(short)cluster_index];
		for (long portal = 0; portal < cluster->portal_count; ++portal)
		{
			long portal_index = cluster->portals[portal];
			s_projected_polygon_173520 *polygon = function_173520(portal_index, cache);
			if (polygon)
			{
				dword bit = 1 << (portal_index & 31);
				long word_index = portal_index >> 5;
				if (((dword *)entry->unknown2c)[word_index] & bit)
					continue;
				if (!(((dword *)g_4f93a4)[word_index] & bit))
					continue;
				if ((polygon->state == 1 && (polygon->field_0 & 2)) ||
					(polygon->state == 2 && (polygon->field_0 & 0x10)))
					continue;
				if (!((polygon->state == 1 && polygon->field_6 == (short)cluster_index) ||
					(polygon->state == 2 && polygon->field_8 == (short)cluster_index) ||
					(polygon->state == 3 && function_173350(pool, (short)index, (short)portal_index) == NONE)))
					continue;
				box2f const *polygon_bounds = &polygon->bounds;
				s_rect4 const *entry_rect = &entry->rect;
				if (!(polygon_bounds->x1 >= entry_rect->x0 && entry_rect->x1 >= polygon_bounds->x0 &&
					polygon_bounds->y1 >= entry_rect->y0 && entry_rect->y1 >= polygon_bounds->y0))
					continue;
				long new_index = pool->count;
				if (new_index == 256 || 5120 - storage->used < 64)
				{
					pool_full = true;
					continue;
				}
				++pool->count;
				s_pool_entry *next = &pool->entries[(short)new_index];
				point2f *allocated = &storage->points[storage->used];
				storage->used += 64;
				next->data = (real *)allocated;
				if ((byte)function_173380((s_polygon_clip_173380 *)&polygon->bounds,
					(s_polygon_clip_173380 *)&entry->rect, (s_polygon_clip_173380 *)&next->rect))
				{
					long other_cluster = polygon->field_6 == (short)cluster_index ? polygon->field_8 : polygon->field_6;
					next->key = key;
					next->next = (short)index;
					next->unknown06 = (short)portal_index;
					next->value08 = (short)other_cluster;
					next->unknown28 = NONE;
					next->unknown0a = NONE;
					next->rect = *(s_rect4 *)g_4687dc;
					next->unknown24 = polygon->distance;
					dword *portal_bits = (dword *)next->unknown2c;
					memcpy(portal_bits, entry->unknown2c, sizeof(next->unknown2c));
					portal_bits[word_index] |= bit;
					long const &count = next->count;
					storage->used = ((point2f *)next->data - storage->points) + count;
					function_11f5f0((box2f *)&next->rect, (point2f *)next->data, next->count);
					function_173b00(lists, (short)other_cluster, (short)new_index);
					if ((queue->index + 1) % 256 != queue->unknown00)
						function_173b60(queue, (short)new_index);
					else
						queue_full = true;
				}
				else
				{
					storage->used = (point2f *)next->data - storage->points;
					--pool->count;
				}
			}
			else
				projection_full = true;
		}
	}
	if (pool_full)
		result = false;
	if (projection_full)
		result = false;
	if (queue_full)
		result = false;
	return result;
}

// score 3 (was 3): the same sort_4byte call as function_1733e0: retail computes &scratch in edx, then `lea ecx` for
// the order array, then `push edx`; ours uses ecx for &scratch and pushes it first. Tried: locals for the order
// pointer, the count, the unused pointer, any combination (no change); dropping the block around `scratch` (42).
// Permuter (25 min on r621, about 1150 variants): nothing below 3.
// @retail 0x173130
void __stdcall function_173130(long camera_count, byte *cameras, long cluster_index,
	s_view_collection_1733e0 *collection)
{
	s_pool pool;
	s_polygon_cache_173520 cache;
	long cluster_map[512];
	s_short_list_table lists;
	s_ring_buffer queue;
	collection->active = (short)camera_count;
	if (cameras != collection->camera)
		memcpy(collection->camera, cameras, camera_count * 0x1bc);
	collection->cluster_count = 0;
	collection->volume_count = 0;
	if (cluster_index != NONE)
	{
		memset(cluster_map, 0xff, sizeof(cluster_map));
		byte *geometry = (byte *)g_4e0348;
		for (long group = 0; group < camera_count; ++group)
		{
			byte *camera = cameras + group * 0x1bc;
			cache.geometry = (s_polygon_cache_173520::s_geometry *)geometry;
			cache.planes = *(s_polygon_context_173910::s_planes **)(geometry + 0x18);
			cache.view = camera;
			memset(cache.visited, 0, ((*(long *)(geometry + 0x5c) + 31) >> 5) * sizeof(dword));
			memset(lists.used, 0, ((*(long *)(geometry + 0x9c) + 31) >> 5) * sizeof(dword));
			pool.count = 0;
			pool.data_used = 0;
			cache.point_count = 0;
			cache.field_c_8.i = 0.0f;
			cache.field_c_8.j = 0.0f;
			cache.field_c_8.k = -1.0f;
			cache.field_c_8.d = 0.0f;
			queue.unknown00 = 0;
			short index = function_172bb0(&pool, (s_pool_source *)camera, (short)cluster_index, group);
			function_173b00(&lists, (short)cluster_index, index);
			queue.index = 1;
			queue.values[0] = index;
			function_172840(group, &pool, &lists, &queue, &cache);
			function_172c70((s_pool_lists *)&lists, &pool);
			function_1730a0((s_pool_lists *)&lists, &pool, cluster_map, cameras, group, collection);
		}
	}
	{
		long scratch;
		sort_4byte(&scratch, collection->order, collection->cluster_count, function_134950, collection);
	}
}

struct s_segment_collision
{
    long kind;
    real fraction;
    point3f point;
    byte field_14[0x24 - 0x14];
    short material;
    byte field_26[0x5c - 0x26];
};
struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);

// Matches with the collision flags as an unsigned long declared before `result` (found by the permuter). The
// start then arrives in EAX, the end in ESI and `narrow` in DL, as retail's callers 0x171830 and 0x16ebf0 set them
// up (ret 4). Before (19), narrow stayed on the stack and the end came in EDX; `long narrow` gave 14, other spellings
// of the flag, the result and the direction 19-42.
// @retail 0x171d00
bool function_171d00(point3f const *start, point3f const *end, real *fraction, bool narrow)
{
    s_segment_collision collision;
    collision.material = NONE;
    unsigned long flags = 0x808c0f;
    bool result = false;
    if (narrow) flags = 0x808c0d;
    vector3f direction;
    direction.i = end->x - start->x;
    direction.j = end->y - start->y;
    direction.k = end->z - start->z;
    if (function_1697c0(flags, start, &direction, NONE, NONE,
        (s_collision_result_1697c0 *)&collision))
    {
        *fraction = collision.fraction;
        result = true;
    }
    return result;
}

struct s_camera_cluster_171830
{
	byte field_0[0x70];
	byte reference;
	byte field_71[0xb0 - 0x71];
};

struct s_camera_reference_171830
{
	short field_0;
	short index;
	plane3f plane;
	byte field_14[4];
};

// score 155 (was 155): The 0xb8 frame and incoming axes agree. The plane-lookup branch span, zero placement and center SSE schedule still differ.
// Earlier (270, was 284): The 0xbc frame and point in EBX differ from the 0xb8 frame and stack-passed point in
// retail. The center trace stays matched.
// The 0xb8 frame and entry convention agree. First +0x3c is the branch span
// to the zero load. The build clears XMM3 before the side branch; retail uses XMM1 at the plane test and lookup exits.
// Earlier 270: the 0xbc frame and point in EBX differed from retail. The center trace stayed matched.
// Earlier score 270 (was 284): the winning displacement spills before the side offsets are stored
// and reloads for refinement. The frame and point convention still differ.
// Earlier logic walk against retail: cluster/reference lookup, plane test (< 0), centre point, four
// samples (sign, offsets[sample & 1], 0x808c0f/0x808c0d), ten bisection steps (|found - upper_fraction| < 0.1),
// the final blend and *distance *= fraction all agree; no behaviour difference found. The remaining diff is the
// calling convention: retail takes up in eax and forward in ecx with point, distance and offset on the stack
// (ret 0xc; the prologue copies [esp+0xc4] into ebp); ours passes up in eax and point in ebx and leaves forward
// on the stack. The declared order of the five parameters changes nothing (6 orders tried, same score), and the
// old `(void)&point` lines were dead code (removed, same bytes). The 0x171d00 call is also set up differently
// until that function takes narrow in dl and end in esi.
// c2B: no body trigger found for the convention. Retail keeps position on the stack and copies it into ebp
// in the prologue (long-lived: loops and calls), and keeps forward and up, which die before the sample
// loop, in registers; ours keeps position in ebx and copies forward into ebp. The chain through 0x171d00
// matters (its convention changes this function's score: `long narrow` there gives 288), so this function
// is probably not fixable before 0x171d00 matches. 0x171d00 now matches (#987), which took this from 266 to 284.
// Permuter (25 min on r622): 26 below its own baseline, only with a float term truncated through an int and a
// volatile pointer (a behaviour change); not kept.
// Joint endpoint and direction 301. Shared preferred-side comparison 300. Boolean direction 281.
// Signed endpoint branch 324. Explicit lower-fraction fallback 284. Sample material after target 284.
// Refinement material after direction 286. Both material initializations at trace inputs 286.
// The joint endpoint, shared comparison and signed-endpoint forms recover the 0xb8 frame, but leave the
// point in EBX and shorten the body to 1197, 1204 and 1181 bytes. NOT kept: the Boolean direction gains
// three points and grows to 1224 bytes. All four reduced controls retain their opening scores.
// The 0xbc frame and point register still differ. Sample/retained-ray names 284; separately
// declared short while ordinal 284; sampled hit fraction read once 284; shared refinement
// point difference 286; signed cluster ordinal retained 284; shared rear-displacement scale
// 284; combined reference admission 284; no-displacement early exit 311; signed winning
// side 282. NOT kept: signed winning side gains two and shortens to 1213 bytes.
// Shared plane distance 284; clear-side-first separate success arms 252; plane-first split 284;
// plane-first helper 284; shared scale for up, side or both 284/284/283; shared sample endpoint
// setter 284; signed reference 281; named plane distance 285. NOT kept: the 252 form puts the
// clear-side store before the plane arm, away from retail's branch order. The frame and point
// convention remain different; the 281 form also moves away from retail length.
// The 0xbc frame and point in EBX remain. Location, rear-depth, offset preparation and
// location-plus-depth scopes 284/284/284/284. In-place side-cross offset 284, with
// location scope 284, with depth scope 284, and with both scopes 284.
// Fraction guard for the retained pointer 300, copied displacement 298, long/short/Boolean axis 291/302/292,
// winning sample and derived sign 296, indexed refinement 318, shared sampling and result fraction 292. NOT kept:
// all regress. The Boolean axis recovers frame 0xb8, but the point stays in EBX.
// Indexed refinement passes the point in EDI and lowers the caller to 298, with the larger 0xc0 frame in this
// function.
// Separate sign and axis loops 278, fixed four-sample latch test 281, shared segment query for samples 283,
// refinement 285 or both 270, input-point copy into center 289, best fraction at sample entry 286.
// NOT kept: the shared segment queries shorten to 1214 bytes with the same larger frame.
// The separate sign and axis loops recover frame 0xb8 but pass the point in EDI and grow to 1222 bytes.
// Kept winning displacement and sign declared at the center trace return. The displacement lives in
// a stack slot as in retail, four bytes above its retail home.
// Rear displacements in scalars 270, signed vector 307 or product vector 278. Staged center coordinates 270.
// Center fraction at the trace 269 or at depth setup 270. Toggled axis 295 or cyclic axis 300.
// NOT kept: the center and sample forms do not recover the point's stack ingress at retail length.
// Const point parameter reference 270, const basis parameter references 270, plane entry reference 270,
// sign before trace material 276, sign by sample-pair boundary 295, Boolean sign bit 304,
// conditional ray masks 302, distinct names for center and winning fractions 270.
// NOT kept: the pair boundary restores the frame at 1211 bytes. Conditional masks shorten
// the resolver to 1200 bytes and leave the point ingress unresolved.
// Point still arrives in EBX, where retail saves the sample ordinal. Post-tested samples 273;
// ascending refinement pass 270; displacement bounds 283; fraction bounds 275; nested sample
// admission 270; cluster record reference 270; unsigned short sample 270; narrow-by-default 296;
// separate plane result and clear-side assignment 298; indexed basis scales 363. NOT kept: fraction
// bounds recover the 0xb8 frame at 1215 bytes, with the point still in EBX. The original 1219-byte
// body is restored.
// Shared direction workspace 275, with shared collision output 275. Scaled-add helper for sample,
// refinement or both 270/270/270. Miss-first midpoint selection 291; endpoint decision after the
// hit test 270; midpoint point setter 270. NOT kept: sharing the direction recovers the frame at
// 1215 bytes with the point still in EBX. The other forms tie or regress.
// One collision result for sampling and refinement 270, Shared collision result declared at center trace return
// 270, Shared collision result declared in the entry phase 270, One direction workspace for both collision loops
// 275, Shared result and direction across the two loops 275, Unsigned ray masks in both phases 270, Shared
// collision result with unsigned ray masks 270, Both shared workspaces with unsigned ray masks 275. NOT kept: A
// shared direction workspace recovers frame 0xb8 but leaves the point in EBX, shortens the body to 1215 bytes and
// raises the score to 275. Shared collision storage with separate directions and unsigned masks tie at 270. Both
// checked controls retain their opening scores.
// Long sample ordinal 296. Unsigned word sample ordinal 270. Byte sample ordinal 287. Independent long
// displacement ordinal 269. Independent word displacement ordinal 270. Sample sign and axis from quotient and
// remainder 298. Unsigned sample with second-pair comparison 295. Short refinement budget 270. NOT kept: The
// long displacement ordinal gains one at retail length. The long and byte sample counts, quotient and
// remainder, and pair comparison recover the 0xb8 frame while preserving the EBX point ingress and moving
// away from retail length.
// File-local resolver 270, byte ray mode 306, separate ordinal/axis and ray mode names 270, center and
// winning sample names 270, near/far bracket names 270, split plane sides with zero on the left 270,
// short sample polarity 301, side cross before the center query 309. The 0xbc frame and point in EBX
// remain.
// Word axis index at selection 270; wrapping displacement cursor 288; integral sample sign 268;
// separate refinement outcomes 291; named fraction difference 270; winning fraction at center trace
// return 270; post-tested refinement count 273; clear the narrow-ray mask bit 270.
// NOT kept: integral sign gains two at retail length. The point still occupies EBX and the
// 0xbc frame still has the sample spill.
// complete midpoint fraction and guarded update 320; midpoint fraction field and guarded update 310;
// midpoint fraction field and latch 310; explicit midpoint hit and miss endpoints 289; sample offset by
// reference 270; separate sample acceptance guards 270; one unsigned sample bit view 270; weighted final
// contributions 270.
// NOT kept: midpoint forms shrink the frame to 0xb0 and raise the score. Three pass the point in EDI and
// the fourth retains EBX. The sample and blend forms tie. Retail still passes the point on the stack and
// reserves 0xb8.
// Shared inline zone test 267, with the location scoped 267. Typed cluster and zone tables with
// unsigned snapshot 270, signed snapshot 270 or global bases at use 270. Signed cluster field at each
// test 261. Winning fraction declared beside center fraction 270, refinement fraction read at each use
// 284.
// NOT kept: repeated zone reads shorten to 1215 bytes and keep the 0xbc frame and EBX point. Retail
// loads the cluster byte once. The shared zone test gains three and shortens to 1213 bytes.
// The point still arrives in EBX and the sample ordinal spills in the 0xbc frame.
// Sample and midpoint ray declaration order 270/270, sample endpoint scope 270,
// midpoint collision workspace first 270, point query home name 270, sample and
// midpoint ray home names 270/270, midpoint hit fraction home name 270.
// Stand-in image 270 -> 155 (dist 46): offsets[0] is copied before the cross product (this gives retail's register
// convention: forward ECX, up EAX, point on the stack), the cluster is a pointer whose reference byte is re-read,
// the blend is chosen in each arm, loop 2 builds its target point first, and the centre goes through a negated
// scaled forward vector. Left: the float zero's load placement and the depth * forward->i operand order.
// Reference byte captured once, with the flagged comparison assigned, separate
// success stores, a conditional result, or an index result refined on the flagged arm:
// unsigned 294/112/294/316, signed 296/292/296/322. Separate success stores
// with direct reference reads and named plane offset 152, zero on the left 152, and
// direct plane test in that split 155. NOT kept: the 112 form retains frame 0xb8
// but adds three bytes, changes the lookup registers and still clears zero before
// the plane expression. The agreeing 1219-byte entry is restored.
// Shared scale and in-place or separate negation 158/158; rear-vector setter 155;
// center-point setter 155; shared negation with center setter 158; paired setters 155;
// zero-initialized rear vector 155; scaled-vector setter with shared negation 158.
// M3 lane 13 r139 (fast with 0x170fd0, MATCH each run; control 155, plain 155): dead initialised locals 1..7 at the top
// 155/155/155/256/177/155/155 (diagnostic: only the sample loop's sites move); the side vector through cross3f(up, forward),
// cross_product_170c00(up, forward) or cross3f_staged 155 each (same code); every cross or depth product written the other
// way round 155 (same code); the plane test through the existing plane_distance_to_point 281 (1235 B, plain 281): its three
// products then load as retail (k*z, j*y, x loaded and multiplied by plane i) but the first sample loop gains a jump and
// padding; that with separate success stores 283, with retail's arm order (flagged arm first, `else narrow = true`) 281;
// dead locals 1..7 on the helper form at the top, before the depth or before the side vector 280 to 285 (the centre and
// cross sites never move); the side vector right after the 0x171d00 call 254, before the up copy 262.
// Packed low bits 155, packed unsigned flag 155, both packed fields 155, packed signed
// flag 155, signed reference byte 263, split signed result stores 263, unsigned reference
// threshold 264, and constrained comparison assignment 291. NOT kept: packed views
// leave the early zero unchanged, while signed and split forms lengthen the plane path.
// Center fused coordinate loop 371, separate rear and center loops 391, indexed rear alone 391, indexed center
// alone 386, indexed scale with shared negation 391, indexed update of copied center 384, copied forward with
// loop and negation 399, explicit plane admission plus fused loop 390. NOT kept: each grows beyond 1219 bytes
// and raises the score. Plane-exit zero and center scheduling remain.
// Plane products accumulated, subtotal named, indexed accumulation, or all
// products named with the plane arm first 281/287/281/284 or the unconditional
// arm first 283/287/283/282. NOT kept: all keep the lookup branch difference
// and grow the body. The zero and center SSE schedule remain displaced.
// M3 lane 3 r299 (H2M_FAST with 0x170fd0 and 0x171d00, both MATCH unless noted; control 155). Counts in brackets are
// difflib-aligned differing rows (build/r299/adiff.py; the positional 155 is mostly a one-row shift): control [36] = plane
// block 7 (zero loaded ahead of the flag branch, one shared flag store, the i product's sides), centre 18, cross product 10
// (five of its six products load the other operand), tail 1. KEPT (same code, 155): the refinement loop calls the existing
// 0x171d00 in place of its body written out (LTCG inlines it there); the sample loop through the same call 263 [60] and
// both loops 263 [57], because the refinement loop's target products then change sides, so the sample loop stays written out.
// LEAD, not kept (a one-use local; build/r299/lead-0x171830-named-distance.patch): the flagged arm first with the distance
// named, `if (reference & 0x80) { real plane_distance = plane_distance_to_point(&entry->plane, point); if (plane_distance <
// 0.0f) narrow = true; } else narrow = true;` 117 at 1222 B [25 with the kept call, 28 without]: the plane block (zero made
// after the products in XMM1, two flag stores) and all six cross products are then retail's; left are the centre's i product
// (retail copies the depth and multiplies by forward->i in memory, ours loads forward->i) and the refinement loop's target
// (retail i and j loaded, k multiplied in place with the midpoint in XMM0; ours j through a copy and the midpoint in XMM3).
// Other plane forms: the existing plane-distance helper in the `||` test 281 [36] (the i product right, a jump and padding
// ahead of the sample loop); the nested arms with the helper call in the test 281 [36]; not-flagged arm first 282/283
// [40/37]; a bool-returning forced-inline wrapper around the helper, the hand-written dot or with (point, plane) parameters
// 152 at 1222 B [45-47] (cross products right, zero still ahead of the branch, refinement target changed); the same wrapper
// taking the distance 152 [45]; a named bool 152 [45]; an identity wrapper on the distance 281 [36]; the whole cluster test
// as a bool-returning inline (one result, early returns, or returned expression) 281/301/289/289 [36-38]; the flag assigned
// from the bool expression 284 [36]; with the distance named: hand-written dot 152 [47], not-flagged arm first 288 [58],
// flag assigned from the compare 275 [30], assignment inside the `||` test 274 [31]. On the lead: 1-8 dead longs or floats
// at the top, after the cross product or in the refinement block 117-127 [28-36] (period 8, never the centre or the target);
// the 19 other parameter orders 117-119 [28-30] and 1-12 unread leading parameters 117-126 [28-36]; the centre as the
// existing scale and negate helpers 267 [56], unary minus 261 [43], products or sums written the other way round 117 [28]
// (same code), the centre through the existing vector sum 124 [34], the depth unnamed 119 [34], scale helper then subtract
// 266 [51], `point->x + (0.0f - back.i)` forms 92-93 at 1219 B [38-41] (the plane block and cross product then go back to the
// control's), negate then scaled sum 275 [62]; the 35 other orders of the rear-vector and centre rows 119-269 [30-61]; the
// rear vector and centre as aggregate initialisers or through the existing setters 117/124 [25-28]; the cross product
// through cross3f and either loop's target through vector3f_add_scaled, in all combinations with the calls, 117-120
// [25-31]; the refinement products' eight written operand orders 117 [28] (same code); one-use probes between the cross
// product and the refinement target (the sum, the sign, the distance, the axis index, the side scale through the helper)
// 117-121 [28-32]. On the control: 1-16 dead locals at the top 155 except 4 and 12 (256) and 5 (177), ahead of the cross
// product or the flag 155/257; the 19 other parameter orders 155-157 [34-40] (forward must stay ahead of up; with up first
// the caller is 2); the scale-and-negate centre 158 [40]. Diagnostics (reverted): with the plane test removed or cut to one
// product the cross products take retail's sides but the point then arrives in EBX; 0-7 unread parameters ahead of forward
// and 0-3 behind it [33-58].
// M3 lane 8 r225 (h2m fast, control 155): every product of the side cross loads the forward field first where
// retail follows the written order, and the center block keeps depth live one product longer. The nine parameter
// interleavings retail's call allows (ECX forward before EAX up; point, distance, offset on the stack) 155 each;
// forward, up, point or distance renamed 155 each; the narrow test as a flagged arm then `else narrow = true`
// 155 (zero on the left 155), as `if (!flag) ... else if (plane)` 282, as one assigned disjunction 283; the j
// product written forward first, the z sum back first, both 155, the back stores i, k, j 157; the side written
// straight into the second offset and scaled in place 155; both offsets through vector3f_scale 263, with the
// side through cross3f 263.
// M3 lane 7 r220 (H2M_FAST with 0x170fd0 and 0x171d00, MATCH at every run; control 155; counts in brackets are
// difflib-aligned differing rows, control [36]). Reading: retail's centre is three products first (depth copied times
// forward->i from memory, forward->j loaded times depth, depth in place times forward->k), then the three subtractions
// from zero, then the sums, which looked like vector-wide steps. Sweep of five plane forms by six centre forms by two side
// forms (60 builds). Centre forms: the existing scale, negate and sum helpers; scale and negate helpers with the sums
// written out; products, then `0.0f -` each, then the sums as three groups of statements; scale helper then written
// negations; scale into one vector and negate into a second: one code for all five. Side vector through cross3f(up,
// forward): same code as written out, at every form. Control plane test: centre written out 155 [36], the five grouped
// centres 158 [40]. Flagged arm first with the plane-distance helper in the test and `else narrow = true`: 281 at
// 1235 B [36], grouped centres 282 [44], [42]. Flagged arm first with the dot written out: the control's code (155 [36],
// 158 [40]). The named-distance lead: 117 at 1222 B [25], grouped centres 267 [55], [53]. The distance named with the dot
// written out: 152 at 1222 B [44], grouped centres 286 at 1238 B [59]. On the lead [25] the centre's i product and the
// refinement target are what is left (ours stores the midpoint after the k product and copies it for the j row; retail
// stores it ahead of the k product and multiplies in place). Stand-alone compiles: one product of two field loads comes
// out in one operand order whichever way it is written (the load with a field offset first, then the first-declared
// pointer's), and a three-term dot always sums k, j, i; two stand-alone side vectors that differ only in the order of the
// two pointer parameters come out with five of the six products the other way round, so the order here is set by the
// function around the products, not by their spelling.
// On the split plane path with the named distance: aggregate midpoint target 117, in-place center copy
// 348, staged scale 118, point setter 119, paired refinement branches 117, miss-first endpoint 165,
// initialized miss fraction 165, copied scale input 170. NOT kept: the 117 forms are three bytes longer
// than retail and still differ in the center transport. The center-copy form also changes the caller.
// Selected record before validity 275, selector resolved before record lookup 293, valid record carried into projection 112, carried plane pointer 276.
// The combined or separate sign and plane tests give the same result for each form. NOT kept: the 112 forms grow to 1222 bytes, exchange lookup scratch registers and still clear zero ahead of the plane expression.
// Full-ray result, as bool or byte: clear on the combined or split narrow paths 286/286, assign
// from the plane test 291, and clear on valid reference then restore after exclusion 281.
// NOT kept: every form grows to 1238 bytes and moves the first difference ahead of the lookup span.
// M3 lane 14 r154 (reduced image with 0x170fd0, 0x171d00 and 0x16ebf0, both callers' neighbours MATCH at every run;
// control 155, aligned 36). Reading of the operand orders: the two sides of a product are sorted by rank, the higher
// first, and on equal ranks the right-hand side goes first. A load through a parameter at offset 0 ranks by the
// parameter's number counted from the last one (offset 1, distance 2, up 3, forward 4, point 5), a load at +4 or +8 by
// two compiler temporaries two apart whose numbers move with every grouping pair around a float product, and a named
// local ranks below every such load but those at the lowest level.
// LEAD 9 at 1219 B (aligned 4; build/r154/lead-0x171830-9.patch; not kept: the named distance is a one-use local and
// the grouping pairs only steer, so both need a byte match): round 299's named-distance lead with the three sample-row
// products grouped, `target.x = (displacement->i * sign) + center.x;`; the three side-offset rows grouped instead 9
// (same code). On it the plane block, the cross product, both loops and the tail are retail's; left is the centre's
// first product, ours forward->i loaded and multiplied by the depth, retail the depth copied and multiplied by
// forward->i in memory. That row needs the depth to rank above forward->i while forward->i stays above up->k (the
// cross product's third row), and the depth as a named local ranks below every parameter's load; no order of the five
// parameters changes that (forward has to stay ahead of up for the caller's set-up). Diagnostic, not source: four
// unread parameters behind forward give the centre exactly (112; the plane's i product and one cross row then tie
// the other way).
// Forms (score [aligned rows]): the lead re-measured 117 [25]; grouping pairs on the refinement rows 118 [29]; on one
// or two sample rows 118 [29] / 118 [29]; the rear vector copied from forward and scaled in place 327 at 1250 B (326 on
// the control); the centre's products written forward first on the lead with three pairs 9 (same code). With 0, 1, 2
// and 3 sample-row pairs: the centre through the existing scale helper with the negation written out, with the
// existing negate helper in place, into a second vector, or with the existing sum helper as well, on the lead
// 267/268/268/159 each ([53] [57] [57] [32]; sum helper [55] [59] [59] [34]): the three loads then come first and the
// zero moves ahead of the flag branch again. The flagged arm with the helper call in the test (no named distance)
// 281/285/288/288 [36-61], with those four centres 282/286/289/289 each; the flagged arm with the dot written out
// 155/263/264/264 [36-61] (no pairs: the control's code), with those four centres 158/266/267/267 each.
// M3 lane 3 r308 (H2M_FAST with 0x170fd0 and 0x171d00, MATCH throughout; control 155 [32]). Round 154's lead
// re-measured 9 at 1219 B (first +0xd0, only the centre's first product left). On it: the rear-vector products in
// parentheses 267 (1223 B); `real depth = offset + *distance;` 9; the first rear product written `forward->i * depth` 9;
// `real const depth` 9; depth declared after the rear vector 9 (all four the lead's code).
// M3 lane 8 r238 (H2M_FAST with 0x170fd0 and 0x171d00, MATCH each run; round 154's lead rebuilt from its patch, 9): on
// the lead the depth sum written at each product (no local) 118 (1222 B); the rear products written forward-first 9;
// `real depth;` declared bare at the top 9; the forward components as `forward->n[0..2]` 9 (all three the lead's
// code); the centre without the rear vector 202 (1223 B); diagnostics: the depth accumulated in the offset parameter 9
// (same code), the rear vector through vector3f_scale with the depth sum as its argument, then negated, 159.
// The plane exit zero and first center product still differ. On the split plane test with grouped sample products:
// first rear compound scale 263, indexed compound scale 389, indexed rear/center/both 391/381/391, one center loop 371,
// indexed products with the shared negate 391, first rear product negated after the others 45 at 1219 bytes.
// On the opening plane path: all rear products then shared negate 158; three named rear products 264.
// NOT kept: the 45 form still differs and uses the former one-use plane distance and grouped sample products.
// M3 lane 14 r157 (fast screens with 0x170fd0, 0x171d00 and 0x16ebf0, both matched ones MATCH each run; round 154's lead
// rebuilt, 9 at 1219 B). Rank reading on the lead (the reassociation sort's ranks): a parameter load at offset 0 weighs
// 0x2000 per place counted from the last parameter plus 8 (point 0xa008, forward 0x8008, up 0x6008, distance 0x4008),
// the +4/+8 loads through forward weigh 0xc008 and 0x0008 (the count wraps at 0x10000), and the named depth weighs 0x200
// (0x20 per local). The centre's products then sort forward.i > depth, forward.j > depth, depth > forward.k; retail
// needs depth above forward.i only, which no local's weight reaches. On the lead: forward read through a pointer local
// in the three rear rows 154 (all three rear loads then come first, first difference back at +0x3c), with the pointer
// declared at the top 154, used in the cross product too 261, as a const reference 154; forward.i copied into a local
// ahead of the depth or after it 266 each (1222 B); the depth bound by const reference to the sum 263 (1223 B), or to
// a sum local 263. Diagnostic only (unread parameters are not source): four unread longs after forward 27 (first
// difference +0x92), at the end of the list 29. Body unchanged.
// @retail 0x171830
void function_171830(point3f const *point, vector3f const *forward,
	vector3f const *up, real *distance, real offset)
{
	real center_fraction = 1.0f;
	s_location location;
	function_11bed0(&location, point);
	bool narrow = false;
	if (location.cluster_index != NONE)
	{
		byte *geometry = (byte *)g_4e0348;
		s_camera_cluster_171830 *clusters = *(s_camera_cluster_171830 **)(geometry + 0xa0);
		s_camera_cluster_171830 *cluster = &clusters[location.cluster_index];
		if (cluster->reference != 0xff)
		{
			s_camera_reference_171830 *entry =
				&(*(s_camera_reference_171830 **)(geometry + 0x68))[cluster->reference & 0x7f];
			if (entry->index != NONE)
			{
				if (!(cluster->reference & 0x80) ||
					entry->plane.k * point->z + entry->plane.j * point->y + point->x * entry->plane.i - entry->plane.d < 0.0f)
					narrow = true;
			}
		}
	}
	/* the camera sits behind the point, along the view's forward vector
	   (retail: ecx, kept in edi, scales the distance at 0x171909-0x171911);
	   the samples beside it go along up (eax, kept in ebx) and up x forward.
	   With forward and up the other way round the ray went straight down from
	   a vehicle's camera marker into the vehicle, and the follow camera was
	   pulled in to the hood */
	real depth = *distance + offset;
	vector3f back;
	back.i = 0.0f - depth * forward->i;
	back.j = 0.0f - depth * forward->j;
	back.k = 0.0f - depth * forward->k;
	point3f center;
	center.x = point->x + back.i;
	center.y = point->y + back.j;
	center.z = point->z + back.k;
	function_171d00(point, &center, &center_fraction, narrow);
	vector3f const *best_offset = NULL;
	real best_sign;
	real best_fraction = center_fraction;
	real radius = *distance * 0.174f;
	vector3f offsets[2];
	offsets[0] = *up;
	vector3f side;
	side.i = up->j * forward->k - forward->j * up->k;
	side.j = forward->i * up->k - up->i * forward->k;
	side.k = forward->j * up->i - up->j * forward->i;
	offsets[0].i *= radius;
	offsets[0].j *= radius;
	offsets[0].k *= radius;
	offsets[1].i = side.i * radius;
	offsets[1].j = side.j * radius;
	offsets[1].k = side.k * radius;
	for (short sample = 0; sample < 4; ++sample)
	{
		s_segment_collision collision;
		collision.material = NONE;
		real sign = (real)((sample & 2) ? 1 : -1);
		vector3f const *displacement = &offsets[sample & 1];
		point3f target;
		target.x = displacement->i * sign + center.x;
		target.y = displacement->j * sign + center.y;
		target.z = displacement->k * sign + center.z;
		long flags = 0x808c0f;
		if (narrow) flags = 0x808c0d;
		vector3f direction;
		vector3d_from_points3d(point, &target, &direction);
		if (function_1697c0(flags, point, &direction, NONE, NONE,
			(s_collision_result_1697c0 *)&collision) && collision.fraction < best_fraction)
		{
			best_fraction = collision.fraction;
			best_offset = displacement;
			best_sign = sign;
		}
	}
	real fraction;
	if (best_offset)
	{
		real lower = 0.0f;
		real upper = best_sign;
		real lower_fraction = center_fraction;
		real upper_fraction = best_fraction;
		for (long remaining = 10; remaining; --remaining)
		{
			real middle = (upper + lower) * 0.5f;
			point3f target;
			target.x = best_offset->i * middle + center.x;
			target.y = best_offset->j * middle + center.y;
			target.z = best_offset->k * middle + center.z;
			real found_fraction;
			bool hit = function_171d00(point, &target, &found_fraction, narrow);
			if (hit)
			{
				if (fabs(found_fraction - upper_fraction) < 0.1f)
				{
					upper = middle;
					upper_fraction = found_fraction;
					continue;
				}
			}
			lower = middle;
			lower_fraction = hit ? found_fraction : 1.0f;
		}
		real direction;
		if (upper_fraction > lower_fraction)
			direction = lower;
		else
			direction = (real)(upper >= 0.0f);
		real blend;
		if (direction != 0.0f)
			blend = upper_fraction > lower_fraction ? lower : upper;
		else
			blend = 0.0f - (upper_fraction > lower_fraction ? lower : upper);
		fraction = (1.0f - blend) * best_fraction + blend * center_fraction;
	}
	else
	{
		fraction = center_fraction;
	}
	*distance *= fraction;
}

void function_3f500(long cluster_index);
double g_45e4c8 = -3.4028234663852886e+38;

/* the point (or vector) scaled by the matrix's scale, when it is not 1, then carried by its axes */
PRIVATE __forceinline void follow_matrix_point(transform4x3f const *matrix, point3f *point)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	point->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	point->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	point->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
}

PRIVATE __forceinline void follow_matrix_vector(transform4x3f const *matrix, vector3f *vector)
{
	real i = vector->i;
	real j = vector->j;
	real k = vector->k;
	if (matrix->scale != 1.0f)
	{
		i = matrix->scale * i;
		j = matrix->scale * j;
		k = matrix->scale * k;
	}
	vector->i = matrix->up.i * k + matrix->left.i * j + matrix->forward.i * i;
	vector->j = matrix->up.j * k + matrix->left.j * j + matrix->forward.j * i;
	vector->k = matrix->up.k * k + matrix->left.k * j + matrix->forward.k * i;
}

// Publishes one user's camera: clamps the motion channels, follows the attached matrix, keeps the view clear of
// geometry and updates the view's cluster.
// @retail 0x170fd0
void __stdcall function_170fd0(long user_index)
{
	s_motion_channels_1701f0 *state = (s_motion_channels_1701f0 *)g_4e9bd4 + user_index;
	point3f position = *(point3f *)&state->current[0];
	transform4x3f const *matrix = &state->matrix;
	real distance;
	if (0.0f > state->current[8]) distance = 0.0f;
	else if (state->current[8] > 3.4028235e38f) distance = 3.4028235e38f;
	else distance = state->current[8];
	vector3f *up = (vector3f *)&state->current[13];
	vector3f *forward = (vector3f *)&state->current[10];
	if (!function_a74c0(forward, up))
	{
		*forward = *g_4687a8;
		*up = *g_4687b0;
	}
	real field_of_view;
	if (g_54e858 > state->current[9]) field_of_view = g_54e858;
	else if (state->current[9] > g_54e85c) field_of_view = g_54e85c;
	else field_of_view = state->current[9];
	state->current[9] = field_of_view;
	if (-50000.0f > position.x) position.x = -50000.0f;
	else if (position.x > 50000.0f) position.x = 50000.0f;
	if (-50000.0f > position.y) position.y = -50000.0f;
	else if (position.y > 50000.0f) position.y = 50000.0f;
	if (-50000.0f > position.z) position.z = -50000.0f;
	else if (position.z > 50000.0f) position.z = 50000.0f;
	if (0.0f > distance) distance = 0.0f;
	else if (distance > g_54e860) distance = g_54e860;
	vector2f direction;
	direction.i = forward->i;
	direction.j = state->current[11];
	normalize2d_fast(&direction);
	position.x += state->current[4] * direction.j + state->current[3] * direction.i;
	position.y = state->current[3] * direction.j - state->current[4] * direction.i + position.y;
	position.z = state->current[5] + position.z;
	vector3f *view_forward = &state->view.forward;
	state->view.forward = *(vector3f *)&state->current[10];
	state->view.unknown14.i = 0.0f - state->first_derivative[0];
	state->view.unknown14.j = 0.0f - state->first_derivative[1];
	state->view.unknown14.k = 0.0f - state->first_derivative[2];
	vector3f *view_up = &state->view.up;
	state->view.up = *(vector3f *)&state->current[13];
	state->view.field_of_view = field_of_view;
	state->view.scale_x = state->current[6];
	state->view.scale_y = state->current[7];
	state->view.aspect = 1.33333337f;
	function_171d90(&state->view);
	if (state->follow_matrix)
	{
		follow_matrix_point(matrix, &position);
		follow_matrix_vector(&state->matrix, &state->view.unknown14);
		follow_matrix_vector(&state->matrix, view_forward);
		follow_matrix_vector(&state->matrix, view_up);
		vector3f const *translation = &state->translation;
		vector3f *view_motion = &state->view.unknown14;
		vector3f_add(view_motion, translation, &state->view.unknown14);
	}
	if (!(state->flags & 0x10) && distance != 0.0f)
		function_171830(&position, view_forward, view_up, &distance, 0.02f);
	state->view.position.x = position.x - state->view.forward.i * distance;
	state->view.position.y = position.y - state->view.forward.j * distance;
	state->view.position.z = position.z - state->view.forward.k * distance;
	/* the same ternary clamp as function_172520: retail keeps the value in a register and stores it once */
	PLACEMENT_CLAMP_172520(state->view.position.x, -50000.0f, 50000.0f);
	PLACEMENT_CLAMP_172520(state->view.position.y, -50000.0f, 50000.0f);
	PLACEMENT_CLAMP_172520(state->view.position.z, -50000.0f, 50000.0f);
	s_location location;
	function_11bed0(&location, &state->view.position);
	if (location.cluster_index != NONE)
	{
		if (location.cluster_index != state->view.location.cluster_index)
		{
			long new_cluster = location.cluster_index;
			if (new_cluster >= 0 && new_cluster < *(long *)((byte *)g_4e0348 + 0x9c))
				function_3f500(new_cluster);
		}
		state->view.location = location;
	}
	long player_index = NONE;
	if (user_index != NONE)
		player_index = g_4e8c20->entries[user_index];
	byte *player = g_4e8c24->data + (player_index & 0xffff) * 0x21c;
	if (*(long *)(player + 0x2c) == NONE && !(player[2] & 8) && *g_510c70 == g_4686c4)
	{
		long cluster_index = g_510c70[user_index + 1];
		if (cluster_index >= 0 && cluster_index < *(long *)((byte *)g_4e0348 + 0x9c))
			function_3f500(cluster_index);
	}
	/* the cast keeps 0.05f a float constant (retail: fld dword); the global holds -FLT_MAX, so this never fires */
	if ((real)fabs(g_45e4c8) < 0.05f)
		state->view.position.z -= 3.4028235e38f;
}
