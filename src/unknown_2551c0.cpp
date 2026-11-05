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
	real vector_k = vector->k;
	real projection = (vector->i * axis->i + vector->j * axis->j + vector_k * axis->k) * (1.f - cosine);
	real cross_i = vector->j * axis->k - vector_k * axis->j;
	real cross_j = vector_k * axis->i - vector->i * axis->k;
	real cross_k = vector->i * axis->j - vector->j * axis->i;
	vector->i = vector->i * cosine + projection * axis->i - cross_i * sine;
	vector->j = vector->j * cosine + projection * axis->j - cross_j * sine;
	vector->k = vector_k * cosine + projection * axis->k - cross_k * sine;
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

/* how good a turned direction is: how close it stays to forward, scaled by
   how far the trace along it reached */
static inline real direction_score(vector3f const *forward_direction, vector3f const *turned_direction, real distance)
{
	vector3f const *forward = forward_direction;
	vector3f const *direction = turned_direction;
	real alignment = dot3f(forward, direction) + 1.f;
	real reach = distance * 0.2f;
	return alignment * reach;
}

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

					real score = direction_score(&forward, &direction, trace.distance);
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

// score 89 (was 89): the object-plus-radius sum is written at both of its uses, the timing
// default is a ternary, and the heading vector is declared ahead of the object index; the frame is
// 0x54 and the length matches retail's. Still differs in the 0x110ab0 argument register (ECX where
// retail uses EAX) and the scratch-register rotation after it, the sum's operand order (retail
// adds the field first), the length and projection stack slots (retail's are four bytes higher),
// the object lookup against the vector copies, and the timing call's argument loads and the
// request stores.
// Earlier score 141 (was 141): frame 0x58 versus 0x54, first-length/radius homes and unit-call scratch allocation remain.
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
// Round 31 (lane D): score 89 (was 90): the request stores are ordered as retail orders them (the
// object index into the arguments, then the direction, then the request type), which is the only
// change; the frame is 0x54 and the length is retail's 963. Still differs in the 0x110ab0 argument
// register (ECX where retail uses EAX) and the scratch-register rotation after it, the object lookup
// against the vector copies, the sum's operand order and its stack slot (retail's is four bytes
// higher), the length's stack slot, the timing call's argument loads and the projection's stack slot.
// Measured this round on this body: the request stores as short, long, type 89, long, type, short
// 89, type, short, long 90, short, type, long 90, type, long, short 90; the sum's operands in the
// other order 89 (MSVC keeps the radius first either way); the sum named inside the projection test
// 222 and named in the outer scope 221; the timing locals hoisted out of their block 89; the object
// lookup before the vector copies 94 (964 bytes). All restored.
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
// Round 267: Timing-output declaration orders 19/20/21/22, 20/22/19/21, 21/22/19/20,
// 20/19/22/21, 19/22/21/20, 21/20/19/22, 22/19/20/21, 20/21/22/19: 89 each.
// Original output declarations restored; unit-argument and first-length/projection homes remain.
// Round 27 (lane 17, fast): the lead. The guard split with the unit read into a named long before the
// 0x110ab0 test, as matched 0x1ae440/0x1a9ec0 write it (a one-use local, so match-only) 42: the argument
// takes EAX and the scratch rotation disappears; with it the target lookup before the vector copies 22,
// the request stores short, long, type 19 (type, short, long and short, type, long 19; long, type, short and
// type, long, short 22), the object size in a named local used by both sums 17 (the sum then loads the field
// first as retail). Left at 17: the first length, the sum, the adjusted projection and the 0x26bfa0/timing
// outputs sit in other slots (retail 0x14 sum, 0x18 length and projection, 0x1c index and distance; ours
// 0x10, 0x14, 0x1c, 0x18), and the lateral reload and the side maximum swap XMM2/XMM3. No gain on it: the
// radius declared before the target 19; a named maximum, reversed compare and fallback sum spellings 19 each;
// the sum as `length + (size + radius)`, `projection < ...`, radius first in one or both sums, 0.6f first 19
// each; renames of the length, projection and side outputs 19 each; side outputs reordered 19 (13/14 swapped
// 21), declared at the heading or 0.95 block top 19; the side outputs as an array 66; the adjusted projection
// as a copy or as a new variable from the last subtraction 17; projection, index and timing declarations moved
// 17; a named reach with a repeated sum 25, used in both 196; a named lateral copy 17; the unit also passed
// to later uses 227/234; the unit radius in a variable 167; the side defaulted to zero first 117. Without the
// named unit: target lookup first 94 (964 bytes), store order 89, both 94.
// Round 263: Uniform radius through a multi-use properties receiver with shared projection/alignment dot subsets 220/220/220/220;
// pure getter at both radius uses with those subsets 153/153/153/153. Original snapshot, products and body restored.
// Round 327: Complete entry readiness in bool/byte/short/long storage, positive and
// complementary graphs, 87 each at 963 bytes. Unit argument and scalar homes remain; two-point
// gains rejected.
// Round 232: isolated unit-test blocked/ready result at its short-circuit boundary,
// bool/byte/short/long storage, 89/89/89/89 and 89/89/89/89. No native result
// width recovers the initial unit-argument register or later scalar/query homes; restored.
// Round 235: native signed/unsigned word/long exit-result producers at final observation
// and entry-declared with original-site assignment: 89 each. Original final global
// observation and complete body restored; initial unit-call scratch rotation remains.
// Round 263: Uniform radius through a multi-use properties receiver with shared projection/alignment dot subsets 220/220/220/220;
// pure getter at both radius uses with those subsets 153/153/153/153. Original snapshot, products and body restored.
// Round 353: Second-probe-only reverse producer, existing setter, scoped second output
// and probe-output role names, 135/135/191/191/135/135/191/191. Original probe workspace
// and complete body restored; unit-call scratch register and length/projection/query homes remain.
// Round 507: Uniform three-use object radius after properties/delta/projection/positive
// admission 221/221/216/215; opposite sum operands give the same four scores. Original
// field observations and body restored; unit-argument and scalar homes remain.
// Round 300: exact-only recovered entry/radius/request graph with paired final
// clearance producers, both declaration orders, mutable/const and conditional/
// complete result arms: 17 each (eight forms); reuse of distance for its
// normalized-length and adjusted-projection phases 17. Restored: the entry
// graph still needs a one-use unit snapshot; sum/length/projection homes differ.
// Round 137 (on the match-only 17 form above, aligned counts equal the score): the adjusted projection stored
// in the length variable (one symbol) 17: the slot groups become retail's (length+adjusted, sum+side, index+
// timing) with +0x14 and +0x18 swapped, plus the lateral/maximum XMM2/XMM3 swap; a separate adjusted variable
// 17 (slots unchanged); radius variable reused for lateral 80; on the shared form: renames, top or own-statement
// declaration 17; sides swapped 18; sum spellings 17/17/17/17/162/185/19; size declared earlier 193/183; named
// reach 25/197/19/197/197; side maximum named, reversed, inline, lateral named or if/else 17 each. Guard shapes
// without the unit local (early return, nested, all in one) 92 at 964 bytes: the argument stays in ECX (one
// scratch rotation step behind retail). Body unchanged at 89.
// Round 125 (lane 8, fast, on the rebuilt 17 form): length, projection, index and timing locals renamed so they
// sort first or last 17/17 (names do not move the slots); the side vector as an initializer 17; the existing
// dot helper for the 0.95 test 17 (same code), for the lateral dot 29; the two length guards joined (no
// heading_length) 17; the type radius named 17; the object size read before the dot 194 (975 bytes); the size
// local in the adjusted subtraction too 185 (970 bytes: it takes its own slot). Body unchanged at 89.
// P286: Native Boolean result owners at first probe/second probe/timing, seven subsets
// and all with entry-only declarations, 89 each. NOT kept: one-use call-result and embedded
// assignment forms need a match. Original body restored; unit scratch and scalar homes remain.
// Round 80 (m3 lane 9, fast), on the match-only 17 form rebuilt (the object size named just before the first sum; named
// at the radius read instead 194, 975 bytes): the adjusted projection in the length variable 17 (aligned 29); a named
// reach used by both sums 196 (951 bytes: the radius call sinks to its use); with the length variable too 197; renamed
// a_reach or z_reach 197 each (same code). Slots there: sum and side at one slot, length and adjusted at another, in
// the opposite order to retail; lateral XMM3, maximum XMM2 (retail XMM2, XMM3). Body unchanged at 89.
// Round 47 (lane 15, fast, on the match-only 17 form): adjusted projection in the length variable 17 (slot groups retail's, 0x14/0x18
// swapped); with a named reach 197, reach alone 196 (951 bytes); one final subtraction into it, `= length - x`, `x = x - y` 17/17/17;
// length and second side renamed either way, six forms 17 each (names do not reorder these slots). XMM2/XMM3 and the swap remain.
// F364: Native vector-owned projection, instance/static and ordinary/forced
// boundaries, original/recovered entry graphs: 89/89/89/89/25/25/17/17. NOT kept: recovered
// graph needs a one-use unit snapshot and mixed radius reads without a match. Original body restored.
// Round 40 (m3 lane 18, fast), on lane 17's match-only 17 form rebuilt (named unit before 0x110ab0, target lookup first,
// request stores short/long/type, named size in both sums): the location outputs declared before the side outputs 17;
// the timing outputs declared real first 17; the timing outputs declared before the side block 17; the timing default as
// an if/else into a local 17; the first test as `length + (size + radius)` 17. Slots there: ours sum+side0+timing0 at
// 0x10, length+side1+timing3 at 0x14, index+timing2 at 0x18, adjusted alone at 0x1c; retail side0+timing0 at 0x10,
// sum+side1+timing3 at 0x14, length+adjusted at 0x18, index+timing2 at 0x1c. Body restored at 89.
// B418: Complete product/planar-sum/final-projection stages in the existing
// projection helper, seven subsets and immutable complete stages: 87/87/87/87/87/87/87/87.
// NOT kept: at most two fewer differences; original helper and full body restored.
// B420: Complete lateral-clearance and heading admissions, positive/negative
// bool/byte/short/long values at both original tests: 89/89/89/89/89/89/89/89.
// NOT kept: no exact match; original complete body restored.
// B424: Standard two-probe maximum, direct/real/double conditional then default
// side result, direct/named conditional expression: 120/120/152/155/155/162/120/120.
// NOT kept: no exact match; original maximum, result ownership, includes and body restored.
// B427: Actual external unit/target/radius scalar references, subsets 1/2/4/3/5/6/7,
// then complete references with readonly actor: 89/89/87/89/123/123/95/95. NOT kept: radius-only
// gains two, below threshold; original field observations, receiver and complete body restored.
// Unit-call scratch and first-length/projection/query homes remain.
// B431: Plain native component traversals for heading/motion/reverse producer subsets
// 1/2/4/3/5/6/7, then all as plain while loops: 215/218/89/223/215/218/223/223.
// NOT kept: no gain; original aggregate inputs, reverse products and full body restored.
// Unit-call scratch and first-length/projection/query homes remain.
// N386: Complete CRT motion/heading/reverse-probe transports, subsets 1..7:
// 184/198/220/203/218/225/252; native construction at all three boundaries 201.
// NOT kept: no equality or gain; original copies, reverse products and complete body restored.
// F391: Native motion/heading inputs through standard forward/backward range capture,
// each alone then all four paired directions: 184/184/201/201/197/197/197/197.
// NOT kept: every form regresses; heading captures grow frame 0x54 to 0x60 and all lengths grow.
// Original full vector inputs, zero-k stores, target acquisition, normalizations and whole body restored;
// unit-call scratch register and first-length/projection/query homes remain.
// Round 150 (m3 lane 5, fast, on the match-only 17 form rebuilt, masked aligned 0): adjusted as a new variable, as a const ref, size as a const ref 17 each (homes
// unchanged); reach, length, radius as const refs 196 (951), 162 (967), 61 (957); radius bound through a cast temporary 61, with a named reach 196, no size 192; adjusted in the
// length variable 17 (groups right, 0x14/0x18 swapped); on that, embedded length assignment, joined guards, copy then subtract, whole adjustment in it, four timing declarations 17
// each, sides declared swapped 18; named maximum, named sums, negated compare, ternary max helper, lateral copy or const ref in the block, second side first in the max 17 each
// (XMM2/XMM3 unchanged); max as a second ternary 51, if-form helper 19, lateral const ref at its definition 25, `||` of the sides 90; side outputs declared at top, heading, dot,
// sum, fabs or 0.95 block, each side, shared and unshared, 17 each (scope does not move them). Diagnostics, reverted: one more read of the sum moves it to 0x10 (166), of the length
// or second side no move (187/96), of a timing output no move (63/70/79); one or two more lateral uses keep XMM3 (92/103). Body unchanged at 89.
// T58: Complete readiness partitions 1/2/4/3/5/6/7 and sole-exit entry rejection without a unit snapshot:
// 89/89/89/89/89/89/89/89. NOT kept: no eligible match or gain; complete original source restored.
// Unit-call scratch register and first-length/projection/query homes remain.
// @retail 0x255740
short __stdcall function_255740(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->unknown5ac != NONE && local_0->unknown5b0 == 2 &&
		!function_110ab0(local_0->unknown018) && local_0->unknown5d0)
	{
		vector3f local_3 = local_0->unknown5ec;
		long local_1;
		local_1 = local_0->unknown5ac;
		s_slot_object_view *local_2 = object_get(local_1);
		local_3.k = 0.f;
		vector3f local_4 = local_0->unknown290;
		local_4.k = 0.f;
		real local_5 = function_30bf0(&local_3);
		real heading_length = 0.f;
		if (local_5 > 0.f)
			heading_length = function_30bf0(&local_4);
		if (heading_length > 0.f)
		{
			real local_6 = *(real *)((byte *)function_1e4a50(local_0->unknown054) + 4);
			vector3f local_7;
			vector3d_from_points3d(&local_0->position, &local_2->unknown030, &local_7);
			real local_8 = function_255741(&local_7, &local_3);
			if (local_8 > 0.f)
			{
				if (local_2->unknown03c + local_6 + local_5 > local_8)
				{
					vector3f local_10;
					local_10.i = 0.f - local_3.j;
					local_10.j = local_3.i;
					local_10.k = 0.f;
					real local_11 = local_7.j * local_10.j + local_7.i * local_10.i + local_7.k * local_10.k;
					if (fabs(local_11) < (local_2->unknown03c + local_6) * 0.6f)
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
							if ((unit_animation_get_timing(local_0->unknown018, 0x7000101, local_18, &local_19, &local_20, &local_21, &local_22) ? local_20 * 1.33f : 0.4f) > local_8)
							{
								s_unit_request local_24;
								*(long *)(local_24.arguments + 4) = local_0->unknown5ac;
								*(short *)local_24.arguments = local_17;
								local_24.type = 0x29;
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
