// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2551C0.CPP: slot handler 0xc (handler at 0x47f790) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"
#include "unknown_0259a0.h"
#include "unknown_1dacb0.h"

/* the slot state of handler 0xc */
struct s_slot_0c_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[2];
	point3f point;
	real unknown1c;
};

/* the unit's offset at +0x346 */
struct s_unit_0c_view
{
	byte unknown000[0x346];
	short unknown346;
};

/* turns a vector about a unit axis by the angle whose sine and cosine are
   given */
static inline void function_x84d9e8(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real projection = (vector->i * axis->i + vector->j * axis->j + vector->k * axis->k) * (1.f - cosine);
	real cross_i = vector->j * axis->k - vector->k * axis->j;
	real cross_j = vector->k * axis->i - vector->i * axis->k;
	real cross_k = vector->i * axis->j - vector->j * axis->i;
	vector->i = vector->i * cosine + projection * axis->i - cross_i * sine;
	vector->j = vector->j * cosine + projection * axis->j - cross_j * sine;
	vector->k = vector->k * cosine + projection * axis->k - cross_k * sine;
}

real function_30bf0(vector3f *v);
bool function_26bf10(long object_index);
long function_1fa7f0(void);
void function_cfec0(long unit_index);
bool function_e68c0(long type, long unit_index);

short g_470b4c = -1;
short g_470b50 = -2;

short __stdcall function_2551c0(long actor_index);
short __stdcall function_255570(long actor_index, s_slot *slot, bool active);
bool __stdcall function_2551f0(long actor_index, s_slot *slot);
void __stdcall function_255520(long actor_index, s_slot *slot);
void __stdcall function_255660(long actor_index, s_slot *slot);

s_slot_handler_2 g_47f790 =
{
	{
		0xc, 2, NONE, -2, 0,
		function_2551c0, function_255570, function_2551f0, function_255520, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, function_255660
};

// @retail 0x2551c0
short __stdcall function_2551c0(long actor_index)
{
	short local_0 = 0;
	s_actor_view *local_1 = actor_get(actor_index);
	local_0 = local_1->unknown018 == NONE ? local_0 : 3;
	return local_0;
}

// score 43 (was 43): cross-product schedule and score-dot registers still differ.
// Round 102 (lane 7, fast): the rotation outputs assigned k, j, i 59 (the score dot keeps k, i, j);
// the direction copied field by field (diagnostic) 167 (792 bytes); the trace result declared before
// the direction copy 43; the score as distance part times dot part 43; sinf/cosf for the angle 72
// (820 bytes). Retail's score dot j, k, i is the reverse of the copy's store order (i, k, j), ours
// k, i, j the reverse of the rotation's output stores (j, i, k); without the rotation (diagnostic)
// the dot is hoisted before the call, so that lead is unconfirmed.
// Round 37 (lane 16): the rotation's dot as a named local before the projection 56 (gives
// retail's early cross_j product a.k*v.i, but the dot then sums i, k); after the cross terms 70;
// the named dot's terms in the 11 other orders 56 each; the named dot at each position among the
// cross terms with all six cross orders 56-125; the score's dot through the file's accumulating
// dot (0x255741's helper, either operand order) 72/72; the projection's dot through it 74/74.
// Round 78: score as (1 + dot) 43; best_score < score 43; 0.2f * distance 43; angle as
// 0.7853982f * i 43; distance * 0.2f * (dot + 1) without the grouping 118. A diagnostic no-op
// product (not kept) reorders the projection dot and fixes the length (814) without touching the
// score dot: both orders follow VC's tuple numbering, not statement position.
// Earlier allocation: retail computes the second products of the j and k cross terms before
// the call's pushes (ours the first j product: 2 bytes longer, j/k cross registers swapped) and
// sums the score's dot product j, k, i (ours k, i, j). Kept: the rotation as projection first, then
// three named cross terms, each output `v * cos + projection * axis - cross * sin` (frame now
// retail's). Tried: cross before projection 109, any other statement order 49-112; product operand
// orders 43; the original output forms 72; i/j/k locals 116; cross as a struct or cross3f 154/159;
// dot3f for the projection 72; output statements reordered 59-124; one_minus_cosine local 56;
// forceinline 43; the score's dot written out in any order 72-146 or swapped 43; loop-local
// declaration orders 43.
// Earlier: our frame was four bytes larger than retail's and the
// inlined rotation helper ran on a different SSE schedule.
// Round 132: shared cosine/axial/both/transverse/cosine+transverse/axial+transverse/all scaling
// [143, 203, 195, 189, 140, 197, 165]; native helper references 43. Early cross products and
// score-dot schedule remain.
// Named second j/k products 109/109; both second products 65; both first products 65;
// named j/k cosine products 120/109; both cosine products 61;
// second products plus cosine parts 139.
// Output base sums i/j/k 109/109/109; all base sums 68; success word/long 106/76;
// signed/unsigned byte direction counters 43/43.
// Round 138: axis value 233; input value 213; projection i/j/k parts 109/109/124; transverse i/j/k parts 109/109/109.
// Round 85: about 90 helper forms screened (cross order, product operand order, output form,
// projection form): VC canonicalizes the product and sum forms, so only the cross statement
// order and the output term order change code. Cross i/k/j 49; projection (1 - cosine) first
// 72; dot3f(axis, vector) 72; outputs v*cos - cross*sin + projection*axis 133 (814 bytes);
// score as the distance part times the dot part 43.
// Round 16 (lane 10): the score's dot as a named local 109 (it also reshuffles the rotation schedule);
// the helper returning the vector 43; sine and cosine named before the call 43; the rotated vector
// renamed to sort after forward 43; the helper's axis parameter first 43; cosine before sine 72.
// Round 181: native slot data block 89 (820 bytes, larger frame); restored slot view with rotation
// inputs axis/vector/sine/cosine or axis/sine/cosine/vector or cosine/sine/axis/vector 43 each;
// vector/sine/cosine/axis, sine/cosine/axis/vector, axis/cosine/sine/vector and
// sine/cosine/vector/axis 72 each. Early cross products and score-dot carriers remain.
// Round 18 (lane 10): the vector by value with the rotated vector returned 377 (619 bytes); outputs through
// a local vector copied back 200; the helper taking the angle (sine/cosine inside) 43; a long loop
// counter 167; the axis components as helper locals 197.
// Round 93: axis global through a local pointer 203 (835 bytes); direction declared beside forward
// 43; helper taking the angle (sine/cosine inside) 43; helper copying the source vector itself 43;
// score dot grouped i + (j + k) 146 (814 bytes, frame slots and rotation schedule reshuffled).
// Lane 14 r9: one or two extra no-op products (diagnostic) leave the score dot's k, i, j order; angle
// as constant times index 43; `1.f + dot` 43; `0.2f * distance` 43; direction copy before the angle 43;
// found declared before the best score 45.
// score dot grouped i + (j + k) 146 (814 bytes, frame slots and rotation schedule reshuffled). Round
// 37: the unit's action state read through the existing 0xe70b0 call (tail and frame unchanged, but
// the rotation's dot then sums j, k, i and the score's k, j, i) 70, with its local declared at the top
// or before the length test 70/70, with the score's dot operands swapped 70; the early exit as a
// nested if 43 (70 with the call); forward/direction renamed (three spellings) 43 each; the state
// point copied as a point 43. Round 101: the rotation as Rodrigues with `cross3f(axis, vector)`
// added (`+ cross * sine`) 159 (820 bytes); the same with three scalar axis-by-vector cross terms
// 139 (814 bytes; the early products become retail's pair but the whole rotation reschedules);
// found declared before best_score 45; found set before the best score and direction 48; the
// direction loop as a do/while 43 (same code).
// Round 3 (lane 17): the rotation helper's locals declared first in four orders, then assigned in the
// kept order 43 each (VC numbers by assignment); the helper reading the axis global itself (no axis
// parameter) 75 (820 bytes); the trace result declared outside the loop 43.
// direction loop as a do/while 43 (same code). Round 19 (lane 13), on the existing 0xe70b0 call (70, 820
// bytes; its inlined body perturbs the early cross products and both dots): its state local declared at
// the function top 70; found before the best score 72; forward and best direction declared first 70; trace
// first in the loop 70; direction first in the loop 70.
// Round 20 (lane 15, fast): `0.f < length` 43 (same code); the best update as direction, found, score 48 and as
// direction, score, found 47; the distance part named (`reach`) 56 (815 bytes); the loop as `i != 8` 109 (814 bytes).
// Round 26 (lane 15, fast): the extra two bytes are two pre-call products loaded from the axis where retail keeps
// the vector's component in a register and folds the axis (cross_j's v.i * a.k and cross_k's v.j * a.i). The real
// 0x26c590 overload in place of the header's argument-order wrapper 109 (814 bytes); one parenthesised product in
// the rotation helper, each of its 18 in turn: the projection's j product 49 (815 bytes), its i product 78 (820
// bytes), its k product 116, the six cross products 109 each, the outputs' cosine products 109-138, projection and
// sine products 109-123; source permuter (600 s, 43 tries) no gain.
// Round 36 (lane 10, fast), the rotation helper in the forms of other files' rotations: matched 0x1ecf50's
// forceinline helper (named dot3f dot, then the projection) 56 (815 bytes), the same as plain inline 56; the
// projection dot j, k, i with the j/k outputs projection-first 43; the dot i + (j + k) with crosses j, k, i
// and axis-first projection terms 116 (814 bytes); matched 0x11d3b0's form (dot k, j, i) 43. Compile-only
// (reproduces the pre-call SSE schedule): 216 forms (6 dot orders x 6 cross orders x 3 output forms, named
// dot or not) never give retail's early a.k*v.i / v.j*a.i products (aligned 11-34 against 14).
// @retail 0x2551f0
bool __stdcall function_2551f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;
	bool result = false;

	if (actor->unknown018 == NONE)
	{
		goto done;
	}

	state->timer = g_510c54->field_2_3 * 10;
	*(vector3f *)&state->point = actor->unknown290;
	state->point.z = 0.f;
	if (function_30bf0((vector3f *)&state->point) > 0.f)
	{
		if (function_26bf10(actor->unknown018))
		{
			function_26c180(actor_index);
			if (actor->unknown27c.unknown10 != NONE)
			{
				s_pathfinding_data *pathfinding = (s_pathfinding_data *)function_1fa7f0();
				real best_score = 0.f;
				bool found = false;
				vector3f forward;
				vector3f best_direction;

				function_210770(actor->unknown27c.point.output_index, &actor->unknown290, &forward);
				for (short i = 0; i < 8; i++)
				{
					real angle = (real)i * 0.7853982f;
					vector3f direction = forward;
					s_path_trace_result trace;

					function_x84d9e8(&direction, g_4687b0, (real)sin(angle), (real)cos(angle));
					function_26c590(actor->unknown27c.unknown10, &actor->unknown27c.point.point, &trace, pathfinding,
						&actor->unknown27c.point.point, NONE, &direction, 5.f, 0);

					real score = (dot3f(&forward, &direction) + 1.f) * (trace.distance * 0.2f);
					if (score > best_score)
					{
						best_score = score;
						best_direction = direction;
						found = true;
					}
				}

				if (found)
				{
					function_210770(actor->unknown27c.point.output_index, &best_direction, (vector3f *)&state->point);
				}
			}
		}

		function_cfec0(actor->unknown018);
		s_unit_0c_view *unit = (s_unit_0c_view *)object_get(actor->unknown018);
		if (*(short *)((byte *)unit + unit->unknown346 + 0x36) != 0)
		{
			function_e68c0(0x27, actor->unknown018);
		}
		result = true;
	}
done:
	return result;
}

// @retail 0x255520
void __stdcall function_255520(long actor_index, s_slot *slot)
{
	dword *flags = &handler_object_get(actor_get(actor_index)->unknown018)->flags134;
	*flags &= ~0x200000;
}

// @retail 0x255570
short __stdcall function_255570(long actor_index, s_slot *slot, bool active)
{
	s_slot_0c_state *state = (s_slot_0c_state *)slot;
	short result = g_470b50;
	real ticks;
	long rounded;

	state->timer--;
	if (state->timer <= 0)
	{
		return g_470b4c;
	}

	ticks = g_510c54->field_2_3 * 0.5f;
	__asm
	{
		fld ticks
		fistp rounded
	}

	if (state->timer > rounded)
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE)
		{
			long child_index = handler_object_get(actor->unknown018)->first_child_index;

			while (child_index != NONE)
			{
				s_handler_object_view *child = handler_object_get(child_index);
				if (child->type == 5)
				{
					return result;
				}
				child_index = child->next_object_index;
			}

			long timer;

			ticks = g_510c54->field_2_3 * 0.5f;
			__asm
			{
				fld ticks
				fistp timer
			}
			state->timer = (short)timer;
		}
	}

	return result;
}

// @retail 0x255660
void __stdcall function_255660(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;

	actor->unknown458 = *(vector3f *)&state->point;
	actor->unknown450 = 0x4000089;
	actor->unknown456 = true;
}

struct s_2556c0
{
	byte field_0[4];
	word field_4;
	byte field_6[10];
};

// @retail 0x2556b0
bool function_2556b0(point3f const *arg_0, vector3f const *arg_1, long arg_2, real *arg_3)
{
	s_pathfinding_data *local_0 = (s_pathfinding_data *)function_1fa7f0();
	s_sector_trace_result local_2;
	function_26c590(local_0, arg_0, arg_2, NONE, arg_1, 1.5f, NULL, (s_path_trace_result *)&local_2);
	if (local_2.blocked)
	{
		long local_5 = local_2.edge_index;
		if (local_5 >= 0 && local_5 < *(long *)local_0->unknown08)
		{
			s_pathfinding_edge const *local_6 = local_0->edges + local_5;
			byte const *local_7 = (byte const *)local_6;
			if (local_7[5] & 4)
			{
				local_2.distance = 1.5f;
				local_2.blocked = false;
			}
		}
	}
	if (arg_3)
	{
		*arg_3 = local_2.distance;
	}
	return !local_2.blocked;
}

struct s_255d60
{
	byte field_0[4];
	point3f field_4;
	short field_10;
	short field_12;
	long field_14;
	long field_18;
};

point3f *function_b9dd0(long object_index, point3f *result);

/* walks the clumps of g_502420, only while the ai is active */
struct s_clump_datum_walk
{
	s_data_datum_iterator iterator;
	long clump_index;
};

static inline void clump_datum_walk_new(s_clump_datum_walk *iterator)
{
	if (g_4f55d0->active)
	{
		iterator->iterator.data = g_502420;
		iterator->iterator.index = NONE;
		iterator->iterator.datum_index = NONE;
	}
}

/* the next clump, or NULL; retail inlines it with the index scan, where LTCG
   keeps a plain inline out of line */
PRIVATE __forceinline s_255d60 *clump_datum_walk_next(s_clump_datum_walk *iterator)
{
	s_255d60 *clump = NULL;

	if (g_4f55d0->active)
	{
		s_record_pool *data = iterator->iterator.data;
		long index = data_next_absolute_index_inlined(data, iterator->iterator.index + 1);
		byte *datum;

		if (index != NONE)
		{
			datum = data->data + data->size * index;
			iterator->iterator.index = index;
			iterator->iterator.datum_index = (*(short *)datum << 16) | index;
		}
		else
		{
			iterator->iterator.index = data->maximum_count;
			iterator->iterator.datum_index = NONE;
			datum = 0;
		}
		iterator->iterator.datum = datum;
		if (datum)
			clump = (s_255d60 *)datum;
		iterator->clump_index = iterator->iterator.datum_index;
	}
	return clump;
}

// @retail 0x255d60
bool function_255d60(long arg_0, point3f const *arg_1, real arg_2, real arg_3, short *arg_4)
{
	real local_20 = arg_2 > arg_3 ? arg_2 : arg_3;
	s_actor_view *local_0 = actor_get(arg_0);
	bool local_3 = false;
	long local_4 = 0;
	real local_1 = local_20 * local_20;
	real local_2 = arg_2 * arg_2;
	s_clump_datum_walk local_5;
	clump_datum_walk_new(&local_5);
	for (;;)
	{
		s_255d60 *local_6 = clump_datum_walk_next(&local_5);
		if (!local_6)
		{
			break;
		}
		bool local_8 = function_1df560(local_0->unknown024, local_6->field_12);
		vector3f local_9;
		vector3d_from_points3d(&local_6->field_4, arg_1, &local_9);
		if (sqrt(length_sq3f(&local_9)) < local_20 + 6.0)
		{
			s_element_502420_member_iterator local_10;
			s_actor_view *local_11;
			local_10.next_index = local_6->field_18;
			local_10.index = NONE;
			while ((local_11 = element_502420_member_next(&local_10)) != NULL)
			{
				if (local_11->unknown018 != NONE)
				{
					point3f local_12;
					vector3f local_13;
					function_b9dd0(local_11->unknown018, &local_12);
					vector3d_from_points3d(arg_1, &local_12, &local_13);
					real local_14 = length_sq3f(&local_13);
					if (local_8)
					{
						if (local_2 > local_14)
						{
							local_4++;
						}
					}
					else if (local_1 > local_14)
					{
						local_3 = true;
						break;
					}
				}
			}
		}
	}
	if (!local_3 && !team_is_enemy(local_0->unknown024, 1))
	{
		s_record_pool_iterator local_15;
		local_15.data = g_4e8c24;
		local_15.index = NONE;
		for (;;)
		{
			byte *local_17 = data_iterator_next_calling(&local_15);
			if (!local_17)
			{
				break;
			}
			long local_18 = *(long *)(local_17 + 0x2c);
			if (local_18 != NONE)
			{
				vector3f local_19;
				vector3d_from_points3d(arg_1, &object_get(local_18)->unknown030, &local_19);
				if (local_1 > length_sq3f(&local_19))
				{
					local_3 = true;
					break;
				}
			}
		}
	}
	if (arg_4)
	{
		*arg_4 = (short)local_4;
	}
	return local_3;
}

struct s_255b10
{
	byte field_0[0x68];
	long field_68;
	byte field_6c[4];
	long field_70;
	long field_74;
	long field_78;
};

struct s_255ce0
{
	byte field_0[0x7cc];
	bool field_7cc;
	byte field_7cd[3];
	s_type_c3b527 field_7d0;
	long field_7e0;
};

real normalize2d(point2f *v);
bool function_10f340(long unit_index, long mode, long set);
void *function_1e5380(long actor_index);
bool function_1ff6c0(long actor_index, long prop_index, point3f const *point, long *target);
bool __stdcall function_1ffa30(s_type_c3b527 const *arg_0, long arg_4, long arg_1, long arg_2, bool arg_3);

// @retail 0x255b10
bool function_255b10(long arg_0, s_type_c3b527 const *arg_1, long arg_2, bool arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	bool result = false;
	long local_2 = NONE;
	point3f local_3;
	if (arg_1->output_index == NONE || !function_2104b0(arg_1->output_index, &arg_1->point, &local_3))
	{
		local_3 = arg_1->point;
	}
	long local_4 = local_0->unknown018;
	byte *local_5 = (byte *)object_get(local_4);
	s_255b10 *local_6 = (s_255b10 *)(local_5 + *(short *)(local_5 + 0x12a));
	long local_7 = local_6->field_70;
	if (local_7 == NONE)
	{
		local_7 = 0x6000086;
	}
	c_type_709360 local_8 = ((s_graph_tag *)g_4e3b44[local_6->field_68 & 0xffff].bytes)->overlay_get(
		local_7, local_6->field_74, local_6->field_78, 0xd000021, NULL, NULL, NULL);
	if ((local_8.index != NONE || function_10f340(local_4, 0x7000101, 0xd000021)) && local_0->unknown07c != NONE)
	{
		point2f local_9;
		local_9.x = local_3.x - local_0->position.x;
		local_9.y = local_3.y - local_0->position.y;
		if (normalize2d(&local_9) > 0.f &&
			local_0->unknown290.i * local_9.x + local_0->unknown290.j * local_9.y >= 0.f)
		{
			byte *local_10 = (byte *)function_1e5380(arg_0);
			if (local_10 && function_1ff6c0(arg_0, arg_2, &local_3, &local_2))
			{
				short local_11;
				if (!function_255d60(arg_0, &local_3, *(real *)(local_10 + 0xc), *(real *)(local_10 + 0x20), &local_11) &&
					function_1ffa30(arg_1, arg_0, arg_2, local_2, arg_3))
				{
					result = true;
				}
			}
		}
	}
	return result;
}

// @retail 0x255ce0
bool function_255ce0(long arg_0)
{
	bool local_2 = false;
	s_actor_view *local_0 = actor_get(arg_0);
	byte *local_1 = (byte *)object_get(local_0->unknown018);
	if (!function_110ab0(local_0->unknown018) && *(real *)(local_1 + 0xf8) == 0.f)
	{
		s_255ce0 *local_3 = (s_255ce0 *)local_0;
		local_2 = function_255b10(arg_0, &local_3->field_7d0, local_3->field_7e0, local_3->field_7cc);
	}
	return local_2;
}

long function_1e4a50(long arg_0);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
bool unit_animation_get_timing(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance);
short __stdcall function_255740(long arg_0, s_slot *arg_1);

s_slot_handler_0 g_47f7dc = {8, 0, 0, -2, 0, function_255740};

PRIVATE __forceinline real function_255741(vector3f const *arg_0, vector3f const *arg_1)
{
	real local_0 = arg_0->i * arg_1->i;
	local_0 += arg_0->j * arg_1->j;
	local_0 += arg_0->k * arg_1->k;
	return local_0;
}

// score 141 (was 141): frame 0x58 versus 0x54, first-length/radius homes and unit-call scratch allocation remain.
// Earlier: frame 0x58 versus 0x54 and first-length/radius homes remain; retail keeps
// the radius in the argument home and adds the object size only after the projection test.
// Fallback timing initialized before query 200; later heading vector 219; later object lookup 223;
// shared reverse scale 223; entry vector homes 205; exhaustive timing branches 205; trace distances
// after location 213; first length in guard 200; completed direction reused 200; timing and trace
// lifetimes 206; earlier interval lifetime 151; earlier request 226; earlier side 200; nested
// normalizations 200; later radius on fallback body 203; early-declared exhaustive interval 205;
// entry first length 151; trace/location names 151; pre-normalization interval 149; byte side 220;
// actor/target/both references 151/151/151; unit handle/radius values 245/232; actor/target point
// values 240/249; heading-length result/reduced guard 143/143, latter retained; interval role name 151;
// entry interval lifetime 168; reversed aggregate 151; first/second planar memcpy 151/151;
// normalized direction/heading/both 243/243/269; immutable first length 143; total reach 145;
// combined range guard 143; separate original/adjusted projection 151; with normalized direction 245;
// interval shares first timing output early-initialized/declared/conditional/late 142/205/142/205;
// second output 205/205/205/205; direction/heading/side/reverse vector names 143 each;
// location/request names 143/143; combined vector/all workspace names 143/143.
// Default duration in an if/else or ternary after the query 205/205 (frame 0x54, 954 bytes);
// with the radius sum after the projection test 211 (the 0x1e4a50 call then sinks too);
// 0x2556b0 returning bool, as retail's tests of al show 141 (kept); with the if/else 205.
// Round 72: a radius read used once is forwarded into its one use, so the 0x1e4a50 call moves
// with it (sum named after the dot 210). The default in an else arm plus the object-plus-radius
// sum written at both of its uses (first test and the 0.6 test) gives 95 at 964 bytes: frame
// 0x54, call before the dot, sum after the test, as retail. Not kept (a repeated expression
// counts only with a match). It still differs in the 0x110ab0 argument register and the
// rotated scratch registers after it, the sum's operand order (field first in retail; either
// spelling gives radius first), the first-length/projection slot packing (names do not move
// it), the timing call's unit load and the request store order. Without the else arm 220.
// Direct trace-test side result 141; immutable side at trace scope 141; direct
// animation value 168; direct side plus animation 168; unsigned word side 142;
// immutable side plus animation 168; retained-fallback conditional 141; complete
// side/animation/timing-result graph 168. Original executable body retained.
// conditional heading-result initialization 141; heading result rejection 162; positive projection
// rejection 162; reach rejection 162; lateral clearance rejection 162; heading alignment rejection 162; all
// five geometric rejection exits 162; named complete forward-range predicate 143.
// Fallback interval after radius input/sum/difference/projection 141/137/138/136;
// inside projection/reach 136/136; after lateral/adjusted distance 147/152;
// projection/reach interval with later radius addition 139/141. Subthreshold forms restored.
// Entry heading result 141; both normalization outputs 141; entry-declared heading 141; entry projection 141; both outputs with projection-boundary interval 136; sequential alignment helper 141; heading with entry displacement 141; heading with entry timing outputs 141. Restored.
// Round 37 (lane 16): the round-72 95 form rebuilt (95, 964 bytes); on it the request stores
// written short, long, type 94 (the scheduler keeps its own order; the 5ac load takes ECX where
// retail reuses EAX); request store order alone 141; the else-arm default alone 205 (954 bytes).
// What the 95 form still lacks is mostly scratch rotation from the 0x110ab0 argument (ECX for
// retail's EAX), the sum's operand order, the length/projection slots and the timing call's pushes.
// The guard split into two nested ifs 141 (on the 95 form 95); the target read before the guard 233.
// Round 44: late fallback with the post-projection radius expression repeated 95 at 964 bytes (diagnostic);
// named post-projection sum in either operand order 211 each; request stores in disassembly order on the
// repeated form 94; direction-length name 94; forward/remaining distance names 94 each; timing outputs
// declared before geometry 94; first-length local reused for adjusted distance 95. Opening body restored.
// Both native copies before planar zeros 141; opposite independent copy order 142;
// existing in-place planar setter 196 (963 bytes), direct planar setter inputs 224;
// existing side/reverse/both-with-planar setters 157/141/224; late zeros plus side/reverse setters 157.
// Native producer contracts do not remove the extra fallback home; original vector producers restored.
// Round 40 (on the 95 form): 0x110ab0 swapped for another stdcall bool(long) callee 96 (same rotation, so it
// is the caller's); guard as an early return 95, inner guard part as an early return 95; the 0x1e4a50 element
// kept as a byte pointer with its +4 radius read in both sums 208.
// Round 19 (lane 13): the radius read early and accumulated (`radius += size` or `radius = size + radius`)
// after the projection test, with the else-arm default 211/211 (the read is still forwarded and the
// 0x1e4a50 call sinks below the test), with the early default 139; the else-arm default with the
// reduced length guard 205; the else-arm default with the sum named between the delta and the dot 209.
// The else-arm default alone is 205 positional but aligned 92 (registers masked 30, against 126/81).
// Round 40 (on the 95 form): 0x110ab0 swapped for another stdcall bool(long) callee 96 (same rotation, so it
// is the caller's); guard as an early return 95, inner guard part as an early return 95; the 0x1e4a50 element
// kept as a byte pointer with its +4 radius read in both sums 208.
// Round 42 (lane 16): why the late sum moves the call: our build forward-substitutes a radius used once
// into its use and the pure 0x1e4a50 call goes with it, to whatever point first needs it (retail keeps
// the call first in the block; 0x1e4a50 is LTCG in retail too, XMM4 stays live across it). Entry-declared
// radius 141; with the sum after the projection test 139 (call sunk after the dot); sum between the dot
// and the test 139 (call placed inside the vector math); radius read before the heading test, sum after
// the projection test 139 (call sunk into the block). The round-72 form (radius used twice through the
// repeated sum, default in an else arm) rebuilt 95: it keeps the call in place; the first difference is
// then the 0x110ab0 argument register (ECX; retail EAX) and the rotation after it.
// Round 450: live target identity 141; target object resolution 239; both 240; actor
// resolution alone/with identity/with object/all 277/277/308/313 (record expansions exact-only);
// all lateral projection products named 143. Original observations and body restored.
// @retail 0x255740
short __stdcall function_255740(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->unknown5ac != NONE && local_0->unknown5b0 == 2 &&
		!function_110ab0(local_0->unknown018) && local_0->unknown5d0)
	{
		long local_1 = local_0->unknown5ac;
		s_slot_object_view *local_2 = object_get(local_1);
		vector3f local_3 = local_0->unknown5ec;
		local_3.k = 0.f;
		vector3f local_4 = local_0->unknown290;
		local_4.k = 0.f;
		real local_5 = function_30bf0(&local_3);
		real heading_length = 0.f;
		if (local_5 > 0.f)
			heading_length = function_30bf0(&local_4);
		if (heading_length > 0.f)
		{
			real local_23 = 0.4f;
			real local_6 = *(real *)((byte *)function_1e4a50(local_0->unknown054) + 4);
			real local_9 = local_2->unknown03c + local_6;
			vector3f local_7;
			vector3d_from_points3d(&local_0->position, &local_2->unknown030, &local_7);
			real local_8 = function_255741(&local_7, &local_3);
			if (local_8 > 0.f)
			{
				if (local_9 + local_5 > local_8)
				{
					vector3f local_10;
					local_10.i = 0.f - local_3.j;
					local_10.j = local_3.i;
					local_10.k = 0.f;
					real local_11 = local_7.j * local_10.j + local_7.i * local_10.i + local_7.k * local_10.k;
					if (fabs(local_11) < local_9 * 0.6f)
					{
						s_slot_object_view *local_12 = object_get(local_0->unknown018);
						if (local_12->type == 0)
							local_8 -= *(real *)(g_4e3b44[local_12->tag_index & 0xffff].bytes + 0x270);
						local_8 -= local_2->unknown03c;
						if (local_4.i * local_3.i + local_4.j * local_3.j + local_4.k * local_3.k > 0.95f)
						{
							short local_17;
							{
								real local_13 = 0.f;
								real local_14 = 0.f;
								long local_15;
								s_location_view local_16;
								function_26bfa0(local_1, &local_15, &local_16);
								vector3f local_25;
								local_25.i = local_10.i * -1.f;
								local_25.j = local_10.j * -1.f;
								local_25.k = local_10.k * -1.f;
								if (function_2556b0(&local_16.point.point, &local_10, local_15, &local_13) && local_11 > 0.f)
									local_17 = 1;
								else if (function_2556b0(&local_16.point.point, &local_25, local_15, &local_14) && local_11 < 0.f)
									local_17 = 2;
								else if ((local_13 > local_14 ? local_13 : local_14) > 0.5f)
									local_17 = local_13 + local_11 > local_14 - local_11 ? 1 : 2;
								else
									local_17 = 0;
							}
							long local_18;
							switch (local_17)
							{
							case 0:
							case 1:
								local_18 = 0xa00022d;
								break;
							case 2:
								local_18 = 0xb00022e;
								break;
							}
							short local_19, local_21;
							real local_20, local_22;
							if (unit_animation_get_timing(local_0->unknown018, 0x7000101, local_18, &local_19, &local_20, &local_21, &local_22))
								local_23 = local_20 * 1.33f;
							if (local_23 > local_8)
							{
								s_unit_request local_24;
								local_24.type = 0x29;
								*(short *)local_24.arguments = local_17;
								*(long *)(local_24.arguments + 4) = local_0->unknown5ac;
								function_e6900(local_0->unknown018, &local_24);
							}
						}
					}
				}
			}
		}
	}
	return g_470b4c;
}
