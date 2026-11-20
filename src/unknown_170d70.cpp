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
// score 64 (was 72): The 0x5c frame agrees. The deadline cursor and signed group ordinal spill in the opposite order, and the basis correction still differs.
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

// score 129 (was 142): Retail saves EBX for the cluster slot and defers EBP for the signed volume index. The build still places cluster creation ahead of traversal.
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
// @retail 0x172ee0
bool function_172ee0(long *cluster_map, s_pool_lists const *lists, s_pool *pool, byte const *cameras,
	long group, s_view_collection_1733e0 *collection, long entry_index, long cluster_index)
{
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
			return false;
	}
	do
	{
		short volume_index = collection->volume_count;
		if (volume_index >= 512)
			return false;
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
	return true;
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

static inline void view_cross_product(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - b->j * a->k;
	result->j = a->k * b->i - b->k * a->i;
	result->k = b->j * a->i - a->j * b->i;
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

// score 334 (was 346): The 0x50 frame and retail length agree. Unity still starts in XMM7 instead of XMM5, and two first-axis squares spill.
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
	view_cross_product(&view->forward, &view->up, &axis);
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

// score 3 (was 69): the side test uses EAX instead of AL, and the final call sets up the two
// argument addresses in reverse order.
// Earlier at 69: logic equals retail (plane from the polygon's plane index, axis = function_120850, projection
// of the context origin onto the plane, the two axis indices from g_440b94[axis * 2 + (plane[axis] > 0)], the 2d copy
// of every vertex, point-in-polygon with 0.0001f); no behaviour difference found. Retail makes the 0/1 an int
// (mov 1 / xor), tests it at once (test al, al) and builds the index with setne later; that and the early
// `mov edi, [polygon+0x1c]` (count) and the origin pointer (retail addresses [eax+0x60] directly, we form
// eax+0x60) still differ. Tried: long positive = cond ? 1L : 0L with an `axes` pointer (126, 129 direct), bool positive
// (127 / 130), bool positive with `positive != 0` or byte/char positive (60 but the compiler then passes both
// parameters on the stack, `ret 8`: not usable), long positive with `!= 0` (77), short positive (69, this form),
// all 48 operand/sum orders of the origin dot product (135 each: the order does not change the code), origin
// as a local pointer vs context->origin->position.x directly (126 vs 135).
// Permuter (25 min on r622): best 58, only with a (char) cast of the count passed to 0x23a220; retail reads it as
// a zero-extended word (0x173ac5) and (char) would turn a count of 128 negative, so it isn't kept. A (word) cast and
// the plane declared first are 69.
// Round 575: the axis and side still occupy the wrong registers across projection. NOT kept:
// projection index before origin 128 or after distance 104; side as row bit 117; default-zero side 121;
// shared point projection 136; vertex base before the loop 156; query point initializer 69;
// three-coordinate projection loop 69.
// Tried comparison widened directly 69, unsigned-short side 69, normalized side local 120, early copy bound 122,
// byte side with early copy bound 137, native postprojection indices 127, native predicate return 70, named axis component 122,
// byte side normalized early 118, normalized side with origin record 7, normalized byte with origin record 40,
// normalized long with origin record 3, normalized signed byte with origin record 40, normalized bool with origin record 40.
// Tried side ternary 3, side branches 135, side-bit mask 46, side equality 8, byte conversion 4,
// query before projected workspace 3, named predicate 5, named call count 5, normalization after distance 126.
// NOT kept: the byte conversion recovers the test but reverses the x product operands.
// The AL side test and the final call address order remain. Low-byte mask 13, complementary side predicate 123,
// signed-byte normalization 4, byte side with scalar-first x product 4, staged normal-first x product 4,
// origin-first x sum 4, complete point initializer 4, selected axis-table row 92, shared query address 3,
// staged distance-first x product 4.
// NOT kept: byte normalization recovers the AL test but exchanges the x product inputs.
// @retail 0x173910
bool function_173910(s_polygon_context_173910 const *context, s_polygon_173910 const *polygon)
{
	point2f points[128];
	plane3f const *plane = &context->geometry->planes[polygon->plane_index];
	short axis = function_120850(&plane->n);
	long positive = plane->n.n[axis] > 0.0f ? 1 : 0;
	bool use_positive_side = positive != 0;
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

// score 96 (was 123): the pool count and state test precede pointer publication in retail. The stride spill, x87 clamp reload and bounds expansion still differ.
// Earlier: logic walked block by block against retail 0x173520-0x17387e: pool/visited bit tests, the 1429d0 and
// 11fc80 argument lists, the 173890 call (cache as context, count as state), FLT_MAX (0x44aed4), the g_4687dc bounds
// copy, pool count update, the index/step setup, the 1/z projection loop with its min/max tests and the distance
// minimum, the state 3 inside clamp (0.015625 as a double) and the memcpy of the view's points, the four 1/256 inflate
// steps and the corner order, the view[0x84] cull and the pool recount agree; no behaviour difference found. What
// still differs: retail stores pool->count and tests result->state before it stores result->points, the count and
// field_0 (ours stores points first); `mov [esp+0x1c], ecx` (step * 8) comes after the `lea edx` in ours; the clamp
// (retail: fld const, fld distance, compare, pop both, reload the winner; ours keeps a copy: one extra fld), which
// shifts everything after it by one line; the inflate (retail adds/subs the constant in xmm0 one box field at a time,
// ours interleaves them) and the register of the final count recompute. Tried: `++pointer` before `index += step`
// (123 -> 96, kept); every order of the four stores points/pool count/count/field_0 (124-131); points through a
// local pointer after the pool count store (128); `distance <= c ? distance : c` (124), `c < distance` (same code),
// if-statement clamp (111), float constant (114), (double) cast on the false arm (96); `index = step + index` (123).
// Permuter (25 min on r622): best 58, from temporaries plus a memcpy size read through a char (a behaviour change);
// not kept.
// Round 575: pool publication and state-test scheduling still differ. NOT kept: state predicate before
// metadata 101; shared reverse predicate 121; forward source index 988; signed byte direction 98;
// double clamp result 96; direct bounds member 161; typed inside view 117; point-value corners 212.
// The pool increment and state-test schedule, stride reload, x87 clamp, and bounds expansion still differ.
// NOT kept: capturing the state moves the projection branches. The explicit float clamp grows the body.
// Tried state snapshot after reservation 172, returned byte for traversal 180, scoped bounds alias 96, named point stride 96,
// signed remaining-count loop 934, explicit float clamp outcomes 117, explicit minimum selection 96, library box copy 96,
// explicit double clamp outcomes 96, direct cache pool members 188.
// Tried clipped vertex reference 96; shared depth 109; whole projected point 184; destination element reference
// 96; distance published after advance 96; late distance with destination reference 96; late distance with whole
// projected point 184; early signed distance 125.
// NOT kept: The references and delayed publication fold to the opening code. Whole-point assignment grows the
// frame, and the depth forms raise the score.
// Tried negated ordered clamp 96, named clamp test 96, float clamp snapshot 83, clipped pointer range 197,
// start from direction 119, clip-local buffer 96, population-local buffers 96, count-first recount 96,
// traversal scoped to the positive count 111. NOT kept: the float snapshot grows to 882 bytes and spills.
// Cache entry by index 296, cache entry reference 96, point arena bound at population entry 96,
// reservation from the advanced count 115, pool commit before the state predicate 101, countdown
// scoped to positive counts 96, named first slot in the recount 96, bounds pointer 96.
// The advanced-count form reaches retail length but changes the reservation arithmetic and raises the score.
// State field reference 99; typed source cluster pair, flags, or both 96 each;
// shared ordered inside cap or per-vertex minimum 96 each; clipped-count for loop 96;
// clipped-count while loop 932. NOT kept: the while loop unrolls, and the other forms
// leave the pool commit, stride spill and x87 cap sequence unresolved.
// Visited-word reference 245, array base 96, explicit bit read/update 96, visited coordinates
// before entry lookup 147, arena reference 115, real cap result 117, named expanded edges 96,
// edge values shared with corners 103. NOT kept: the word reference shrinks the frame and the
// other forms leave or worsen the reservation, cap reload and edge-store gaps.
// Actual pool subobject 96; pool object reference 96; retained count field 96; shared visited-bit
// accessors 96; projected point setter 96; corner setter 109; nested cull guard 96; named capacity 96.
// The pool and cull forms keep the opening code. The corner setter grows to 878 bytes. The pool
// commit/state order, stride spill and x87 compare-and-reload still differ.
// Round 639 forms: native count field 96, direct increment after pointer publication 177, source flags
// before bounds 120, clipped count with classification 117, source and clip metadata before reservation
// 120, bounds before source metadata 130, stride in loop update 96, explicit direction assignment 96.
// NOT kept: the best forms tie and the pool publication, stride spill and x87 clamp remain.
// Capacity rejection inside the miss path 233, last reservable index 232, count from the
// reserved pointer 200, reserved pointer through all point writes and recount 196, default
// forward index 172, nonnegative count 184, guarded postdecrement 96, ascending counter with
// separate source cursor 968. NOT kept: the postdecrement ties, the ascending counter unrolls,
// and the reservation forms do not recover retail's count and state-test order.
// score 45 in a full build with stand-in SDK headers (was 96): the four corners read result->bounds by name instead
// of through the `bounds` reference the inflate writes through, so the inflated bounds are copied back out of memory
// as retail does (0x173796 on).
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
			long used = pool->count;
			result->points = pool->points + used;
			pool->count = used + 64;
			result->count = (short)count;
			result->field_0 = *(long const *)((byte const *)polygon + 0x18);
			if (result->state)
			{
				long index = result->state == 1 ? count - 1 : 0;
				long step = result->state == 1 ? -1 : 1;
				point3f const *local_cb4759 = clipped;
				long remaining = count;
				if (remaining > 0)
				for (; remaining != 0; --remaining)
				{
					point2f *point = result->points + index;
					real inverse = -1.0f / local_cb4759->z;
					point->x = local_cb4759->x * inverse;
					point->y = local_cb4759->y * inverse;
					if (bounds.x0 > point->x)
						bounds.x0 = point->x;
					if (point->x > bounds.x1)
						bounds.x1 = point->x;
					if (bounds.y0 > point->y)
						bounds.y0 = point->y;
					if (point->y > bounds.y1)
						bounds.y1 = point->y;
					real distance = 0.0f - local_cb4759->z;
					result->distance = result->distance > distance ? distance : result->distance;
					++local_cb4759;
					index += step;
				}
				if (result->state == 3)
				{
					if (inside)
					{
						result->distance = (real)(result->distance > 0.015625 ? 0.015625 : result->distance);
						bounds = *(box2f *)(cache->view + 0xa0);
						result->count = *(short *)(cache->view + 0x198);
						memcpy(result->points, cache->view + 0x19c, *(long *)(cache->view + 0x198) * sizeof(point2f));
					}
					else
					{
						bounds.x0 -= 0.00390625f;
						bounds.x1 += 0.00390625f;
						bounds.y0 -= 0.00390625f;
						bounds.y1 += 0.00390625f;
						point2f *corners = result->points;
						corners[0].x = result->bounds.x0;
						corners[0].y = result->bounds.y0;
						corners[1].x = result->bounds.x1;
						corners[1].y = result->bounds.y0;
						corners[2].x = result->bounds.x1;
						corners[2].y = result->bounds.y1;
						corners[3].x = result->bounds.x0;
						corners[3].y = result->bounds.y1;
						result->count = 4;
					}
				}
				if (cache->view[0x84] && result->distance > *(real *)(cache->view + 0x88))
					result->state = 0;
				pool->count = (result->points - pool->points) + result->count;
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

// score 134 (was 205): The result and capacity byte homes, signed cluster spill and sentinel publication still differ.
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
					short other_cluster = polygon->field_6;
					if (other_cluster == (short)cluster_index)
						other_cluster = polygon->field_8;
					next->next = (short)index;
					next->key = key;
					next->value08 = other_cluster;
					next->unknown06 = (short)portal_index;
					next->unknown28 = NONE;
					next->unknown0a = NONE;
					next->rect = *(s_rect4 *)g_4687dc;
					next->unknown24 = polygon->distance;
					dword *portal_bits = (dword *)next->unknown2c;
					memcpy(portal_bits, entry->unknown2c, sizeof(next->unknown2c));
					portal_bits[word_index] |= bit;
					storage->used = ((point2f *)next->data - storage->points) + next->count;
					function_11f5f0((box2f *)&next->rect, (point2f *)next->data, next->count);
					function_173b00(lists, other_cluster, (short)new_index);
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
	else if (projection_full)
		result = false;
	else if (queue_full)
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

// score 270 (was 284): The 0xbc frame and point in EBX differ from the 0xb8 frame and stack-passed point in
// retail. The center trace stays matched.
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
			bool hit = false;
			s_segment_collision collision;
			collision.material = NONE;
			point3f target;
			target.x = best_offset->i * middle + center.x;
			target.y = best_offset->j * middle + center.y;
			target.z = best_offset->k * middle + center.z;
			long flags = 0x808c0f;
			if (narrow) flags = 0x808c0d;
			vector3f direction;
			vector3d_from_points3d(point, &target, &direction);
			real found_fraction;
			if (function_1697c0(flags, point, &direction, NONE, NONE,
				(s_collision_result_1697c0 *)&collision))
			{
				found_fraction = collision.fraction;
				hit = true;
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

// score 295 (was 325): same behaviour as retail. Walked statement by statement against retail
// (0x170fd0-0x17182b): constants, every field offset, every branch direction, the call arguments and the stack
// order of 0x171830 (position, &distance, 0.02f) agree; no behaviour difference was found.
// The horizontal direction is a vector2f normalized with the existing normalize2d_fast (the magnitude is not used
// after it, and the helper's threshold and scale are the same): retail keeps x at [esp+0x18] and y at [esp+0x1c],
// and with the vector the frame is retail's 0x20 (the loose real locals gave 0x24) at retail's 2142 bytes. The same
// pair as a `real direction[2]` with the normalize written out gives 320 and every slot as retail, but that only
// groups two locals, so it is not kept.
// What still differs:
// (1) the call to function_171830. Retail passes forward in ecx, up in eax and &position, &distance, 0.02f on
//     the stack (0x171684-0x171694); our function_171830 takes position in ebx and forward on the stack, so the
//     call setup differs and ebx is busy: the state pointer lands in ebp (retail ebx) and the view.up pointer in
//     ebx (retail ebp). Needs function_171830 to match first (its own convention comes from its body).
// (2) the scheduling of the position sums (0x171186-0x1711d9: retail loads current[4] and current[3] into
//     registers first and multiplies in place; ours folds them as memory operands), a few folded operands in the
//     matrix block (`movaps; mulss [mem]` where retail loads the matrix field first), the translation add
//     (retail `offset + translation`), and the branch not taken: retail loads x and z again there
//     (0x17165c), ours keeps them in registers. Term order and folding move with every upstream change.
// Kept (each made it closer): the direction vector (above); struct copies for the default forward/up and the view
// forward/up; `up` declared before `forward` (retail's lea order); pointer locals for view.forward/view.up; the y of
// the direction read as state->current[11] (retail loads [ebx+0x138], x through the forward pointer); the field of
// view tested on the memory operand; the ternary clamp macro for view.position; `player_index = NONE` then an if;
// (real)g_45e4c8 so 0.05f is a dword constant; the point helper declares z, y, x in that order.
// Tried, no help: `a = a + b` for the translation, the other five declaration orders of the point helper (x,y,z
// 341, z,x,y 334, others 347-357) and of the vector helper (i,j,k best; 322-327), a `long cluster_index` local in
// the cluster test (365), __fastcall or reordered parameters on function_171830, 24 spellings of the three
// position updates (all the same), the updates in z,y,x order (411 with vector2f), `s_location` declared first,
// real direction_x/y locals (346), the direction array (320, above).
// Permuter, 30 minutes (11-06, on the direction_x/y source before the vector2f form): 323 from 341, only with casts and
// stores moved (view.aspect before the position sums, view_up declared earlier). Not kept.
// The point transform now reads its scale once. The state and collision-point registers still differ at entry.
// Ordered distance initializer 336; field-of-view readback 441; shared horizontal channel reads 415;
// named horizontal corrections 409; shared vector addition 336; cached point-transform scale 325 (kept);
// cached vector-transform scale 397; signed-word location cluster 336.
// The 0x20 frame agrees. The collision callee still takes the point in EBX, displacing the state to EBP.
// in-place point helper 709; three in-place vector helpers 1110; direct transform fields 325; direct
// collision fields 325; local-player lookup helper 325; camera point rename 325; direct initial basis
// copies 402; horizontal vector initializer 325. NOT kept: The existing transform calls stay out of line
// and shorten the body. The direct initial view copies also shorten it. The other forms tie at the retail
// extent.
// The camera state remains in EBP and the collision point argument still uses EBX.
// Input point after distance clamp 352; coordinate aggregate 468; local distance clamp 362;
// initial placement clamps on all three coordinates 336; projected-point aggregate 429;
// in-place projection then point copy 422; point setter 389; local projection clamped before copy 378.
// Collision resolver stack slots before/after the basis parameters 325/325;
// later up pointer 325; direct up/forward through all phases 374/375;
// nested collision gate 325; current up value before publication 385;
// conditional entry distance 413. The collision point still occupies EBX.
// The state remains in EBP with the point argument in EBX.
// Current-channel address, rate-channel address, follow translation reference, current plus rate, current plus translation, rate plus translation, all three, and all with a translated view-vector address 464/325/325/464/464/325/464/464.
// The state remains in EBP and the collision point still arrives in EBX.
// Output point address before calibration, after follow, at first store, or by reference 382 each;
// calibrated view through location 459; horizontal subvector 337; follow matrix reference 325;
// distinct focus point 459. NOT kept: the address forms keep the collision register conflict,
// and the distinct focus point shortens the body by 93 bytes.
// Input point memcpy 325. Shared point setter 518. Followed-point copy 399.
// Sweep/projection snapshot 406. Shared sweep address 325. View-axis memcpy 325.
// Sign-extended cluster after the change check 295 (kept). Separate count-load guard 295.
// The state still uses EBP and the collision point still uses EBX.
// View up/forward/both by reference 295/295/295; horizontal direction scoped to displacement 295;
// indexed state address 295; basis pointers bound after both copies 405; initial/final coordinate
// clamp loops 434/456. NOT kept: the late basis pointers shorten the body and the clamp loops
// grow it. The state base remains in EBP and collision point in EBX.
// The collision point in EBX still displaces the state from its retail register.
// Point/vector/both row accumulators 382/297/382. Full point/vector/both setters 295/465/463.
// Row accumulators with a single point/vector setter 382/461. The point setter alone ties at retail length.
// Horizontal position after both view copies, after all derivatives or between derivative y and z 440/410/440.
// X and Z before view copies, Y after both 295. Vertical offset ahead of both horizontal sums 439.
// X and Z before the forward copy, Y before up 295. X between copies and Y after both 410.
// Horizontal sums after derivatives with aspect last 410. NOT kept: no gain, or frame shrinks to 0x1c.
// The collision point still occupies EBX.
// Guard-scoped adjusted distance 374, guarded input point copy 419, both 447,
// immutable input point until clamping 473. Projection X through the retained
// forward axis 357, with direct up fields 369; separate displacement vector
// 304; three projected coordinates before their stores 295. NOT kept: the
// guarded copies enlarge the frame or body, and the point still uses EBX.
// Two horizontal offset channels by value with component expressions 385.
// Two horizontal offset channels by value with shared dot product 385.
// Two horizontal offset channels by value with shared cross product 385.
// Two horizontal offset channels by value with both shared products 385.
// Two horizontal offset channels by view with component expressions 364.
// Two horizontal offset channels by view with shared dot product 364.
// Two horizontal offset channels by view with shared cross product 364.
// Two horizontal offset channels by view with both shared products 364.
// NOT kept: the horizontal offset vector forms increase scores and lengths. The four read-only
// views keep the 0x20 frame at 2146 bytes, but the collision point in EBX still displaces the state.
// Addressed view for camera publication 415. Addressed view in the follow branch 330. Addressed view for
// projection and clamps 314. Addressed view for projection and cluster lookup 379. View reference across
// calibration and output 529. Indexed state by reference 295. Projection through the retained forward axis
// 388. Addressed view for the cluster lookup only 357. NOT kept: The view reference restores the opening EBX
// state base and delays the first difference to +0x2c, but shortens the body by 96 bytes and raises the
// score. The indexed state reference ties. The other receiver scopes regress.
// Declared array element 295, follow-distance and output-axis names 295, separate input/output axis
// names 295, view receiver for projection 384, view receiver for clamps and location 371, same receiver
// through final depth 461, follow-distance clamp macro 295, sampled field-of-view clamp 434. The state
// stays in EBP and the collision point in EBX.
// Up copy between derivative stores 295; aspect after both scales 295; typed player read 295;
// user-cluster table read once 295; scoped location publication 295; derivative vector setter 295;
// view receiver for calibration and derivative transform 313; shared clamp destination 370.
// The collision point stays in EBX. Keeping the view receiver shortens the body and sharing
// the clamp destination lengthens it.
// horizontal normalizer through shared planar scaler 295; paired horizontal offset products 366; y
// correction live to field-of-view publication 296; horizontal outputs published together after field of
// view 321; shared point setter for channel displacement 388; named point-transform products 383; named
// vector-transform products 295; named point and vector transform products 387.
// NOT kept: the alternate planar scaler and named vector-transform products tie. The later horizontal sum
// forms preserve length and increase the score. Point-transform product names shorten the body. The
// collision point still arrives in EBX and displaces the state base.
// Published location receiver 295, combined changed-cluster guard 325, nested user-cluster bounds 295,
// typed cluster count 295. Direct input-up address 363, later normalization-through-copy forward
// pointer 379, output-axis pointers at normalization 446, adjusted projection depth snapshot 295.
// The point still uses EBX with the 0x20 frame.
// The sweep point in EBX still displaces the state to EBP. Separate sweep distance from explicit
// bounds, a conditional, a mutable request copy or an upper selection 385/385/391/385.
// With separate distance, const starting point view 385, starting point assignment 385, distance
// before all coordinate clamps 420 or before the vertical clamp 413. Each stays at 2142 bytes.
// Stand-in image 295 -> 6: products with the matrix field first (follow_matrix_point's locals in x, y, z order, the
// vector calls passing &state->matrix), the translation added with vector3f_add, both struct copies written relative
// to state, and function_171830's retail convention (see its note). Left: one operand order at 0x17161b, pop esi.
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
		vector3f_add(&state->view.unknown14, translation, &state->view.unknown14);
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
	if (fabs((real)g_45e4c8) < 0.05f)
		state->view.position.z -= 3.4028235e38f;
}
