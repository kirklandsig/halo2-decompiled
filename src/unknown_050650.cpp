// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_050650.CPP: actor direction selection and looking state. */

#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "props.h"
#include "unknown_0259a0.h"
#include "unknown_1e3920.h"
#include "unknown_11cc90.h"

/* A local view of the shared actor datum; other modules own the array. */
struct s_path_records_2967b0
{
	bool active;
	byte unknown01[0x2d - 1];
	char count;
	char index;
	byte unknown2f[0x3c - 0x2f];
	s_actor_path_point points[4];
};

typedef char assert_path_records_2967b0[(sizeof(s_path_records_2967b0) == 0xac) ? 1 : -1];

struct s_actor_looking_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x40 - 0x1c];
	bool attention_enabled;
	byte unknown041[0x54 - 0x41];
	long character_definition_index;
	byte unknown058[0x84 - 0x58];
	short field_xe428b1;
	short alert_state;
	byte unknown088[0x229 - 0x88];
	bool unknown229;
	byte unknown22a[2];
	point3f position;
	byte unknown238[0x264 - 0x238];
	bool direction_locked;
	byte unknown265;
	bool using_object;
	bool seated;
	byte unknown268[4];
	long object_index;
	short look_range_state;
	byte unknown272[0x27c - 0x272];
	s_location_view location;
	vector3f forward;
	byte unknown29c[0x328 - 0x29c];
	short unknown328;
	byte unknown32a[0x338 - 0x32a];
	long field_x50962e;
	long target_marker;
	bool target_marker_valid;
	byte unknown341[0x358 - 0x341];
	short attention_state;
	byte unknown35a[0x370 - 0x35a];
	point3f attention_point;
	byte unknown37c[0x41c - 0x37c];
	short aiming_mode;
	byte unknown41e[2];
	short aiming_direction_type;
	byte unknown422[2];
	long aiming_prop_index;
	byte unknown428[0x430 - 0x428];
	short looking_mode;
	byte unknown432[2];
	short looking_direction_type;
	byte unknown436[2];
	long looking_prop_index;
	byte unknown43c[0x48a - 0x43c];
	bool unknown48a;
	byte unknown48b[0x50c - 0x48b];
	union
	{
		s_path_records_2967b0 records_50c;
		struct
		{
			bool path_active;
			byte unknown50d[0x539 - 0x50d];
			char path_count;
			char path_index;
			byte unknown53b[0x548 - 0x53b];
			s_actor_path_point path[4];
		};
	};
	byte unknown5b8[0x5d0 - 0x5b8];
	bool movement_aiming;
	bool movement_aiming_valid;
	byte unknown5d2[0x5ec - 0x5d2];
	vector3f field_x6d9d38;
	vector3f movement_aiming_direction;
	byte unknown604[0x684 - 0x604];
	short override_priority;
	short override_timer;
	short override_direction_type;
	byte unknown68a[2];
	vector3f field_x013329;
	long idle_aiming_timer;
	long idle_looking_timer;
	short idle_aiming_direction_type;
	byte unknown6a2[2];
	vector3f idle_aiming_direction;
	short idle_looking_direction_type;
	byte unknown6b2[2];
	vector3f idle_looking_direction;
	byte unknown6c0[0x6ce - 0x6c0];
	short animation_timer;
	byte unknown6d0;
	bool unrestricted_looking;
	bool attention_selected;
	byte unknown6d3;
	vector3f reference_vector;
	vector3f field_xe66477;
	vector3f field_x6f2625;
	bool aiming;
	byte unknown6f9[0x758 - 0x6f9];
	vector3f combat_direction;
	byte unknown764[0x80c - 0x764];
	short output_mode;
	byte unknown80e[2];
	dword output_flags;
	byte unknown814[0x828 - 0x814];
	vector3f output_reference;
	vector3f output_aiming;
	vector3f output_looking;
	point3f output_point;
	byte unknown858[0x888 - 0x858];
};

static inline s_actor_looking_view *actor_looking_get(long actor_index)
{
	return (s_actor_looking_view *)g_4f55f0->data + (actor_index & 0xffff);
}

// @retail 0x298b60
bool function_298b60(long actor_index)
{
	bool result = false;
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (actor->aiming && actor->aiming_mode >= 2)
	{
		if (actor->aiming_direction_type == 2 ||
			(actor->aiming_direction_type == 1 && actor->aiming_prop_index == actor->field_x50962e))
			result = true;
	}
	return result;
}

// @retail 0x298bc0
bool function_298bc0(long actor_index)
{
	bool result = false;
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (actor->looking_mode == 0)
	{
		if (actor->alert_state >= 3)
			return function_298b60(actor_index);
	}
	else if (actor->looking_mode == 2)
	{
		if (actor->looking_direction_type == 2 ||
			(actor->looking_direction_type == 1 && actor->looking_prop_index == actor->field_x50962e))
			result = true;
	}
	return result;
}

/* Name inferred from the timer and direction initialization. */
// @retail 0x297560
PRIVATE void function_297560(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	real ticks = (real)g_510c54->field_2_3;
	long rounded_ticks;
	/* Retail rounds using the current x87 rounding mode. */
	__asm
	{
		fld ticks
		fistp rounded_ticks
	}
	actor->idle_aiming_timer = (short)rounded_ticks;
	actor->idle_looking_timer = (short)rounded_ticks;
	actor->idle_aiming_direction_type = 4;
	actor->idle_aiming_direction = actor->forward;
	actor->idle_looking_direction_type = 4;
	actor->idle_looking_direction = actor->forward;
}

// @retail 0x297600
PRIVATE void function_297600(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	/* Retail clamps after converting each decremented integer timer to real. */
	real aiming = (real)(actor->idle_aiming_timer - 1);
	actor->idle_aiming_timer = (long)(aiming > 0.f ? aiming : 0.f);
	real looking = (real)(actor->idle_looking_timer - 1);
	actor->idle_looking_timer = (long)(looking > 0.f ? looking : 0.f);
}

real normalize2d(point2f *vector);

static inline real actor_looking_normalize2d(point2f *vector)
{
	real x_square = vector->x * vector->x;
	real magnitude = (real)sqrt(x_square + vector->y * vector->y);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real scale = 1.f / magnitude;
		vector->x = vector->x * scale;
		vector->y = scale * vector->y;
		return magnitude;
	}
	return 0.f;
}

/* A switch selects whether the vertical component is compared. */
// score 2 (was 2): the x87 square addition is before the final reference-component store instead of after it; frame and final dot agree.
// Earlier: the horizontal dot now matches retail instruction for instruction; the last
// difference is the scheduler's placement of the x87 add: retail sums the two squares after the
// reference's second component is stored, ours sums them before that store.
// Vector parameters
// swapped, aggregate 2D copies and the 3D result assignment (kept). The aim renamed 40; declarations
// swapped 39; forceinline/shared first normalization 40/40; a product accumulation 20; a commuted sum 14;
// the final comparison assigned 25; nested comparison/single product local 6/6; product operands swapped 8;
// reversed terms and operands 8; helper terms/shared magnitude/named square 8/8/8; squared accumulation 10;
// dot3f 8; an early final return 83; shared normalization with reversed terms 8; indexed vector2f locals 8;
// comparison operands reversed 8; an explicit false arm 27; single-product operand swaps 8/8.
// Native copies with complete/original result graphs 4/6; complete projected aggregates 28; complete parallel product/comparison variants
// 66/66/65/66; both external normalizations 122; native copies with uniform operands/named parallel products 6/6/6/6; single-worker 100-trial
// search from native copies/result 4, no improvement. Four-difference forms restored under the retention gate.
// Complete single-exit dimensional result 4 (two-point gain, restored); horizontal-first dispatch 71; both workspaces
// at function scope 6; named native vector2f components 6; complete named dot 6; both named products 14; single-exit
// byte/integer results 57/62; direct horizontal success on that graph 62; full conjunction assignment 83;
// single exit plus y-first normalization stores 8; single exit plus native fast normalization 4 (restored).
// Single exit with the horizontal sum's term and operand orders 12/12/6/12, parenthesised terms 4, a
// named dot 68, named factors 47, pointers to the locals 38; none better than the plain single exit.
// Round 398: paired real squares, uniform scalar inputs, both, squares plus sum; each with original/conditional reference-projection lifetime
// 8/38/122/128/122/128/8/38; all restored.
// Round 113: unnamed helper square 6; helper with an else-zero single exit 2 (same code); sqrtf 2;
// brace-initialised reference copy 23; `>=` epsilon test 34; permuter (48 variants) none below 2.
// Lane 13 r20: the leaf order follows the first source mention of each field (only the final dot names them);
// a split x term gives x first but faddp first, a single tree gives the store first but the dot's order; retail
// needs reference x then aim y first, which one dot cannot spell. Both through normalize2d 122 (inline declaration
// 122; kept out of line even from a non-/Ob1 file 122); single tree with aim member copies y,x 10 / x,y 12; helper
// defined after the caller single/named 6/2; single tree with the dot's y term first 6, reference-first 4.
// Round 91: native fast normalization with horizontal term/operand orders 12/12/12/6/4/6/6;
// the existing magnitude helper in the private normalizer 6; all restored.
// Round 121: reference copy made after the aim test (nested ifs) 39; components read into named locals in the
// private normalizer 6; header normalize2d_fast + dot_product2d on vector2f copies 38 (258 bytes); dot with the y
// term first 8; one named squared magnitude 6. Compile-only: every 3D dot term/operand order gives identical code.
// @retail 0x296d60
PRIVATE bool function_296d60(real cosine, vector3f const *reference, vector3f const *aim, bool three_dimensional)
{
	bool result = false;
	if (three_dimensional)
	{
		result = reference->k * aim->k + reference->j * aim->j + reference->i * aim->i > cosine;
	}
	else
	{
		point2f aim_horizontal = *(point2f const *)aim;
		point2f reference_horizontal = *(point2f const *)reference;
		if (actor_looking_normalize2d(&aim_horizontal) > 0.f &&
			normalize2d(&reference_horizontal) > 0.f &&
			reference_horizontal.x * aim_horizontal.x + aim_horizontal.y * reference_horizontal.y > cosine)
			result = true;
	}
	return result;
}

// @retail 0x296600
PRIVATE real function_296600(long actor_index, long prop_ref_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_prop_datum *reference = prop_ref_get(prop_ref_index);
	s_type_5cfb45 *state = function_25d690(reference);
	s_type_f95cd3 *view = NULL;
	if (reference->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(reference->tracking_index);
		if (tracking)
			view = &tracking->view;
	}
	real interest = 0.f;
	if (reference->state >= 1 && reference->state <= 2)
	{
		if (state->unknown5e)
		{
			short ticks = *(short *)((byte *)state + 0x5c);
			interest = 7.f > (real)ticks * g_510c54->rate ? 1.8f : 0.4f;
		}
		else
			interest = prop_get(reference->prop_index)->unknown23 ? 2.f : 1.f;
	}
	else if (prop_get(reference->prop_index)->unknown23 && reference->state >= 3)
		interest = 1.5f;
	if (reference->object_index == actor->object_index || state->unknown3c == actor->object_index)
		interest = 0.f;
	real scale = state->unknown3c == NONE ? 1.f : 1.5f;
	if (view)
	{
		switch (*(char *)((byte *)view + 0x3a))
		{
		case 1: interest += scale * 0.5f; break;
		case 2: interest += scale; break;
		case 3: interest += scale * 2.f; break;
		}
	}
	if (state->unknown63)
		interest += scale * 2.f;
	if (view)
	{
		switch (*(char *)((byte *)view + 0x38))
		{
		case 1: return interest * 0.6f;
		case 3: return interest * 0.4f;
		case 4: break;
		default: return interest;
		}
	}
	return interest * 0.2f;
}

struct s_actor_looking_properties
{
	real aiming_yaw;
	real aiming_pitch;
	real looking_yaw;
	real looking_pitch;
	real aiming_cosine;
	byte unknown14[4];
	real looking_cosine;
	byte unknown1c[4];
	real idle_aiming_angle;
	real idle_looking_angle;
	real moving_aiming_angle;
	real moving_looking_angle;
	real_bounds idle_time;
	real_bounds alternate_idle_time;
	real_bounds alert_idle_time;
	real_bounds alternate_alert_idle_time;
};

/* The original character-property lookup has not been recovered yet. */
s_actor_looking_properties *function_1e5160(long character_definition_index);

struct s_actor_looking_object
{
	long definition_index;
	byte unknown04[0x14 - 4];
	long parent_index;
	byte unknown18[0xaa - 0x18];
	byte type;
	byte unknownab[0x13c - 0xab];
	long player_index;
	byte unknown140[0x1fc - 0x140];
	short seat_index;
	byte unknown1fe[0x346 - 0x1fe];
	short animation_offset;
	byte unknown348[0x3dc - 0x348];
	byte movement_state;
};

struct s_actor_looking_object_header
{
	byte unknown00[8];
	s_actor_looking_object *object;
};

static inline s_actor_looking_object *actor_looking_object_get(long index)
{
	return ((s_actor_looking_object_header *)g_4e0300->data)[index & 0xffff].object;
}

struct s_actor_looking_seat
{
	byte unknown00[0x88];
	real minimum_yaw;
	real maximum_yaw;
	byte unknown90[0xb0 - 0x90];
};

struct s_actor_looking_unit_definition
{
	byte unknown00[0x1cc];
	s_actor_looking_seat *seats;
};

// @retail 0x2973f0
PRIVATE bool function_2973f0(long actor_index, real *looking_cosine, real *aiming_cosine, real *idle_aiming_cosine, real *idle_looking_cosine)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (properties)
	{
		*aiming_cosine = properties->aiming_cosine;
		result = true;
		if (actor->using_object)
		{
			s_actor_looking_object *object = actor_looking_object_get(actor->object_index);
			s_tag_element *element = function_1e5450(actor_index, object->definition_index);
			if (element)
				*aiming_cosine = (real)cos(*(real *)((byte *)element + 0x70));
		}
		else if (actor->seated)
		{
			s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
			if (unit->seat_index != NONE)
			{
				s_actor_looking_object *parent = actor_looking_object_get(unit->parent_index);
				s_actor_looking_unit_definition *definition = (s_actor_looking_unit_definition *)g_4e3b44[parent->definition_index & 0xffff].bytes;
				s_actor_looking_seat *seat = &definition->seats[unit->seat_index];
				real yaw = 0.f - seat->minimum_yaw;
				*aiming_cosine = (real)cos(yaw > seat->maximum_yaw ? yaw : seat->maximum_yaw);
			}
		}
		*looking_cosine = properties->looking_cosine;
		if (actor->field_xe428b1 == 4)
		{
			*idle_aiming_cosine = (real)cos(properties->moving_aiming_angle);
			*idle_looking_cosine = (real)cos(properties->moving_looking_angle);
		}
		else
		{
			*idle_aiming_cosine = (real)cos(properties->idle_aiming_angle);
			*idle_looking_cosine = (real)cos(properties->idle_looking_angle);
		}
	}
	return result;
}

/* Returns ticks. */
// @retail 0x297c10
PRIVATE long function_297c10(long actor_index, bool alternate_range, bool extended)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	long result = g_510c54->field_2_3 * 3;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (properties)
	{
		real lower, upper;
		if (actor->alert_state >= 2)
		{
			if (alternate_range)
			{
				lower = properties->alternate_alert_idle_time.lo;
				upper = properties->alternate_alert_idle_time.hi;
			}
			else
			{
				lower = properties->alert_idle_time.lo;
				upper = properties->alert_idle_time.hi;
			}
		}
		else if (alternate_range)
		{
			lower = properties->alternate_idle_time.lo;
			upper = properties->alternate_idle_time.hi;
		}
		else
		{
			lower = properties->idle_time.lo;
			upper = properties->idle_time.hi;
		}
		if (extended)
		{
			lower *= 1.5f;
			upper *= 1.5f;
		}
		/* Keep the sample and seconds-to-ticks product in x87 precision until
		   retail's store to real immediately before integer rounding. */
		real random = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f);
		real ticks = (lower + (upper - lower) * random) * g_510c54->field_2_3;
		long rounded;
		__asm
		{
			fld ticks
			fistp rounded
		}
		result = rounded;
	}
	return result;
}

/* Name inferred from the conditions that suppress direction selection. */
// @retail 0x2982f0
PRIVATE bool function_2982f0(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = true;
	if (actor->direction_locked)
		result = false;
	else
	{
		s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
		if (unit->type == 0 && unit->movement_state == 5)
			result = false;
		else if (actor->field_xe428b1 >= 4 && actor->movement_aiming)
			result = false;
		else if (function_110ab0(actor->unit_index))
			result = false;
	}
	return result;
}

static inline real actor_looking_normalize3d(vector3f *vector)
{
	real magnitude = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real scale = 1.f / magnitude;
		vector->i = scale * vector->i;
		vector->j = vector->j * scale;
		vector->k = vector->k * scale;
		return magnitude;
	}
	return 0.f;
}

static inline void actor_looking_rotate_in_place(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real projection = (vector->i * axis->i + vector->j * axis->j + vector->k * axis->k) * (1.f - cosine);
	real cross_i = vector->j * axis->k - vector->k * axis->j;
	real cross_j = vector->k * axis->i - vector->i * axis->k;
	real cross_k = vector->i * axis->j - vector->j * axis->i;
	vector->i = vector->i * cosine + projection * axis->i - cross_i * sine;
	vector->j = vector->j * cosine + projection * axis->j - cross_j * sine;
	vector->k = vector->k * cosine + projection * axis->k - cross_k * sine;
}

static __forceinline void actor_looking_rotate(vector3f *vector, vector3f const *axis, real angle);

static inline void actor_looking_rotate(vector3f *vector, vector3f const *axis, real angle)
{
	real sine = (real)sin(angle);
	real cosine = (real)cos(angle);
	actor_looking_rotate_in_place(vector, axis, sine, cosine);
}

// score 333 (was 333): the frame remains 0x98 versus 0xa8; axis, random/trig and vector schedules differ.
// An earlier round: no new form; the frame is still 0x98 against retail's 0xa8 and the axis,
// random/trig and vector schedules still differ.
// Tried: direct collision Boolean 355; initial best vector before normalization 356; result declared first 354;
// long sample counter 355; axis/sample/range names 355; shared normalizer 330 (sum order changed, restored);
// shared random-range helper 360; one result return 377; paired span names 355; names plus one return 377;
// those plus initial best vector first 356; startup values before axis initialization 335 (kept).
// Early sample ordinal 358; distinct pitch/yaw vector results 383 (frame 0xb4);
// both 404; entry ray with complete vector setter 338 (retail length); entry
// collision result 335; split results with entry ray/collision 386/383; split
// results with one return 404. Restored.
// Unit-random producer with candidate after/before/between draws 335/336/341;
// named normalized fractions at those sites 340/341/335 (last retail length).
// Complete vector-copy setters with early/between candidate 363/363. Restored.
// Round 225: ray scale helper 339 (frame 0xa8, retail length); native long
// clear/result/both 320/324/335; scaled combinations 338/340/338; scaled native
// word states 324. Long clear retained at 320 (plain); ray helper and result restored.
// Original draws, transforms, producer values and calls preserved.
// Lane T round 28 (older base): Forced existing normalizer/angular/product boundaries, singly and in all
// combinations, 355 each; ordinary producers 1135. Original helpers restored.
// Direct collision parameter alone 355 on T's older base (main's 335 form kept).
// Round 171: Two-stage vectors, copy and scale subsets 387/408/362/382; consumed-stage ray reuse 391/407/343/362. Reuse recovers frame 0xa8, setters can recover length, but all regress. Original body and includes restored; frame and stack-input transport remain.
// Round 173: Shared fallback result 341; horizontal/vertical range names 320; long sample ordinal 319;
// result/range 341; result/ordinal 340; range/ordinal 319; all three 340; result with complete workspace
// names 341. One-point forms rejected. Original body restored; frame 0x9c versus 0xa8 and stack-input
// transport remain.
// DH round 239: Existing rotation dot/cross/complete-setter subsets 324/320/324/320/324/320/324;
// all three with parameter-only ranges after axis normalization 369. Frame remains
// 0x9c/0xa8; original helpers and complete sampler restored.
// Round 248: Complete forward observation, native/setter 354/353; later input 354/354;
// later best 355/354; later input and best 355/354. Frame remains 0x9c.
// Forward parameter at each other source position, definition and sole caller together: seven 320s.
// Original input reads, signature and caller restored; frame and stack-input transport remain.
// Round 95: the shared rotation helper in projection-first scalar form (for 0x297d30) 318; both ranges
// after the axis normalization as retail computes them 343 (kept: the only caller 0x297660 then passes
// the yaw bounds on the stack, 52 -> 19), one range only 313/313.
// Round 97: one exit with `result = true` for the best-direction tail 333 (kept: the collision flag now
// stays on the stack as in retail, which matched 0x297660); with `clear` set first in each pass as a bool 349.
// Round 433: Single-exit success stored as byte/char/short/long 370/370/382/383;
// without the dead entry collision-state value 370/370/382/383. All restored;
// native success widths do not recover the 0xa8 frame or initial CL zero store.
// Round 43: retail has two epilogues (`mov al, 1` after the best-direction tail, `mov al, dl` for the
// result): `return true;` in that tail 343 and with a bool clear 335 (aligned 323, retail's late ebx
// push), but both pass the collision flag in BL instead of on the stack, so 0x297660 stops matching;
// bool clear alone 346; long attempt counter 332; with bool clear 345.
// Lane 7 r99: draws as calls to the existing 0x259d0 333 (kept: LTCG inlines it with retail's x87 order
// x*C*range+lower and hoists both ranges); `lower + range * unit random` 333; `clear` set first in each pass,
// long 327 / bool 349 (bool gives retail's prologue, `mov al, 1` and DL result); long form plus `return true`
// after the best tail 336 at 1423 B (0x297660 4); `clear = true` between the draws 335. The frame gap is the
// collision result: function_1697c0 fills 0x5c bytes (items.cpp, projectiles.cpp; four matched users pad the
// 0x4c header type to 0x5c); the header type at 0x5c gives retail's 0xa8 frame, 331, with long clear first 322,
// but not kept (0x247950 356 -> 358, five padded users need their wrappers removed).
// Round 126 (m3 lane 6, fast with 0x297660, which stays matched): a long attempt counter 332; that with `clear = true`
// first in each pass 326 (1423 B; retail compares the counter as a word, `cmp dx, 0xa`, so the long type is not
// retail's), plus best_fraction or clear declared before result 326/326; a bool clear with the long counter 345, with
// clear first as well 348; on the short counter best_fraction before result 333, clear before result 333; the
// retail-faithful bool clear set first in each pass re-measured 349 (aligned difference 195 against the body's 233:
// retail's prologue and `sbb al, al`, but the rotated candidate is spilled between the two rotations, 1435 B). The
// frame needs the shared collision-result type at its real 0x5c size, which also changes users in other lanes'
// files this round (unknown_247fd0.cpp, unknown_2ba3e0.cpp, path_transitions.cpp).
// Round 109 (lane 8, fast; aligned = difflib count 234): the pitch axis set field by field 333; the collision
// result declared at function scope 333, the ray at function scope (assigned in the loop) 333, both 333 (all
// same code: VC packs the frame the same whatever the scopes); `clear = true` first in each pass with the
// function-scope collision result 327 (the listed long-clear-first form; 1424 bytes). Retail keeps a zero in
// ECX for the result byte and the attempt counter, and pushes EBX only at the loop (ours at entry, for the
// default best vector's k).
// Round 26 (m3 lane 2, fast): bool clear set at the top of each pass with `return true` after the best tail
// 327 (0x297660 4: the collision flag then passes in BL); that with one `return result` 349 at 1435 B (retail's
// late EBX push, EBX scratch, EBP/EDI best i/j, AL clear and DL result, the tail is retail's code, but the loop
// body is 16 bytes longer and the x87 temp sits above the pitch axis, retail's at the frame bottom); on it the
// candidate at function scope 349, the pitch axis member by member 349, the helper's cross terms j, k, i 349,
// the ray through the existing vector3f_scale 347 (frame 0xa0). Diagnostic, reverted: that form with the header's
// collision type at 0x5c 344 (retail's 0xa8 frame, first difference +0xa; the rotation stays 16 bytes longer).
// Lane 18 r14 (one-link LTCG env with the callee-closure objects, exact for this function: env count 326 = check
// 333): collision type at 0x5c 324; with byte clear 328; byte clear 330; byte clear set first in each pass 334;
// that with 0x5c 329 (late EBX push, byte clear in AL); yaw-axis pointer local before the loop 334, with 0x5c 333;
// rotation helper with the j/k cross terms first and i in its row: vector 328, scalars 326 (same code), 0x29c860
// spelling 328, k before j 329 (0x297d30 22/22/22/38, control 22); candidate copy before the draws 327, between
// them 343; fraction as `((real *)&collision)[1]` 326; best_fraction declared before result 326 (same code).
// Retail also holds g_4687b0 in ECX across the loop and keeps best.k in a stack slot.
// Lane 13 r44 (fast): the collision local as a 0x5c buffer (header type plus 0x10 bytes, as unknown_1c20f0.cpp)
// 331 (frame 0xa8); with it the header normalize3d for all three normalizations 364 (1451 B; the length temp
// moves to the frame bottom as in retail); the file's normalize helper single-exit (`else magnitude = 0.f;
// return magnitude;`) 366 (1453 B: temp at the frame bottom, both range and trig temps at retail's offsets);
// that with `bool clear` 380 (1462 B); with `clear = true` first in each pass 368 (1460 B: retail's prologue,
// late EBX push, ECX yaw axis, every frame offset, the collision block, latch and both tails; left are the two
// rotations, about 30 instructions longer with spills through the sine slot, and the pitch normalize's j
// product order); on that the rotation's cross_j/cross_k before cross_i 368 (same code).
// Lane 13 r46 (fast = full here): the rotation's dot product as k, j, i 333 (0x297d30 23, unchanged); long `clear = true`
// first in each pass 327 again (retail's prologue and late EBX push; the rest is the 0x10 frame gap, every stack
// offset shifted); on it a long attempt counter 326 at 1423 B (retail compares `cmp dx, 0xa`: a short counter), the
// candidate copy before the draws 334, the single-exit normalize 346 (1449 B), `clear` left uninitialised 327 (same
// code). In this body the rotations are about retail's length (fsin/fcos/branch at the same listing lines).
// @retail 0x296e60
PRIVATE bool function_296e60(point3f const *origin, vector3f const *forward, bool test_collision,
	real yaw_lower, real yaw_upper, real pitch_lower, real pitch_upper, vector3f *direction)
{
	vector3f best_direction = *g_4687a8;
	bool result = false;
	real best_fraction = 0.f;
	long clear = true;
	vector3f pitch_axis = { -forward->j, forward->i, 0.f };
	if (actor_looking_normalize3d(&pitch_axis) == 0.f)
		pitch_axis = *g_4687ac;
	for (short attempt = 0; attempt < 10; ++attempt)
	{
		real yaw = function_259d0(&g_4e7408->unknown0, NULL, 0, yaw_lower, yaw_upper);
		real pitch = function_259d0(&g_4e7408->unknown0, NULL, 0, pitch_lower, pitch_upper);
		vector3f candidate = *forward;
		actor_looking_rotate(&candidate, &pitch_axis, pitch);
		actor_looking_rotate(&candidate, g_4687b0, yaw);
		clear = true;
		if (test_collision)
		{
			vector3f ray = { candidate.i * 3.f, candidate.j * 3.f, candidate.k * 3.f };
			s_collision_result_1697c0 collision;
			collision.unknown24 = NONE;
			clear = !function_1697c0(0x10800001, origin, &ray, NONE, NONE, &collision);
			if (clear)
			{
				actor_looking_normalize3d(&candidate);
				*direction = candidate;
				result = true;
				break;
			}
			real fraction = *(real *)((byte *)&collision + 4);
			if (fraction > best_fraction)
			{
				best_fraction = fraction;
				best_direction = candidate;
			}
		}
	}
	if (!clear && best_fraction > 0.f)
	{
		actor_looking_normalize3d(&best_direction);
		*direction = best_direction;
		result = true;
	}
	/* Retail still draws ten samples when collision testing is disabled,
	   then returns false without writing the output vector. */
	return result;
}

/* The direction decoder has aiming and optional target-point outputs.
   A specification is a type followed by a 12-byte union. */
struct s_type_952051
{
	short type;
	short padding;
	union
	{
		long index;
		point3f point;
		vector3f vector;
	};
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
void function_cafc0(long unit_index, point3f *position);
void function_1caa40(long object_index, point3f *position);
long function_1e1f20(long actor_index);
real function_30bf0(vector3f *v);

/* Dependencies not yet recovered; signatures follow retail's call sites. */
void function_caf60(long unit_index, point3f *position);
void function_1e3b00(long object_index, long mode, point3f const *reference,
	void const *unknown0, void const *unknown1, point3f *position);
void function_1fc710(long actor_index, point3f *position);
bool function_1ffbd0(long actor_index, long weapon_index, short barrel_index,
	point3f const *origin, point3f const *target, bool flag, vector3f *direction,
	void *unknown0, void *unknown1, void *unknown2, void *unknown3);
bool __stdcall function_1ffe00(long actor_index, long object_index);

// score 257 (was 257): frame 0x8c agrees; the result/point-valid byte homes, the direction chain's layout predecessor
// (retail: case 2's remembered-prop 0x1e3b00 tail, which case 1's copy cross-jumps into) and the weapon path's registers differ.
// Earlier at 276 (was 276): frame 0x8c agrees; result/point-valid byte roles, native point copies and shared case tails differ.
// Earlier at 322 (was 322): Round 32: the jump-table base and every internal jump still differ (our body is 1424 bytes
// against retail's 1444); retail keeps the shared direction/normalise/output chain between case 2 and case 1 and holds
// two copies of the remembered-prop code (case 1's copy cross-jumps into case 2's) where ours shares one block.
// Retail copies the marker position and the case 3 and 5 points as three dwords where ours copies three floats, and
// retail calls 0x1ffe00 with both arguments on the stack where our build passes the first in EAX (that callee is
// declared in src/unknown_1ff010.cpp, not this lane's file).
// Tried: struct-assignment point copies 360 (1388 bytes); duplicated remembered blocks with a separate fail exit 361;
// the shared chain moved inside the switch between cases 2 and 1 322 (byte-identical: the layout follows the flow
// graph, not the text); the chain written inside case 2 333; the marker position as a struct assignment 333 (the
// same addresses, the copied dwords in another register order); case 1's own remembered copy jumping into case 2's
// 1e3b00 call 379 (the prologue's register choice flips); the Boolean homes made retail's (point_valid set once at
// the aim-at-point label, `point_valid = result = false`) 321, restored because the gain is under 8.
// Earlier at 276 (was 276): frame 0x8c agrees; Boolean homes, point/case-tail transport and aiming workspace remain.
// Earlier: frame 0x8c and movement-record addressing agree; Boolean role homes and point/case-tail transport differ.
// frame 0x8c and movement-record addressing agree; Boolean role homes and point/case-tail transport differ.
// Boolean role homes and case-specific point/reference transport differ.
// Tried (fast control 446): case 2 before 1 450; one exit 416; both 420; result first 420; Boolean names 420;
// default first 420; field-derived indices at each use 420; object case before point/vector cases 409;
// result assigned from both normalization tests 397 (kept); that with case order and flag name 401; flag name 416;
// test results plus flag name 397; Boolean declarations reversed 414; tests plus reversed declarations 395 (restored).
// Chained Boolean defaults 395 (+0x3a first); with case 2 first 399; entry marker
// workspace 397, with chained defaults 395; complete point setters 336 (kept,
// retail length); with chained defaults 334 (extra two-point gain restored);
// those plus case order 339, plus entry marker 339.
// Uniform five-point memcpy producers 397; case 2 first 401; chained defaults
// 395; both 399. Uniform memmove producers with those forms 444/442/442/440.
// All shorter than retail; original setters, cases and defaults restored.
// Round 224: Native external point fields and byte result/point-valid, seven subsets
// 336/345/345/336/336/345/345; native points plus byte result/word point-valid 356. All source and
// record declarations restored.
// Round 224 continuation: Result-owned point publication at the common return, crossed with case-2-first/shared defaults/
// long result, eight forms 336/341/334/339/350/346/351/347. Two-point shared-default form rejected;
// original result, epilogue, cases, setters and full body restored.
// Lane T round 28 (older base): All eight ordinary/inline/noinline state-query/origin-output declaration
// pairs 446 (unchanged fast control 446; full baseline 445). All provider and
// consumer declarations restored; saved-register timing and Boolean defaults remain.
// Round 171: Native point origins 381/377/349/377/385/389/355/393; path record and late pointer 330/330;
// shared defaults 328 fast, 330 plain (below threshold); case 2 first 301 fast, 290 plain kept. Named last
// index is neutral and omitted. Boolean-role names 301/303/303/303; native origins on typed graph
// 375/375/308/375/379/387/322/389. Frame and movement addressing agree; Boolean role homes and point/case-tail
// transport remain.
// Round 398 (fast 301, full 290): direction/output/both setters; aiming-query scope with neither/direction/output/both; all with late origin
// 371/318/387/301/371/318/387/387; all restored.
// Native byte aiming/read-only specification/read-only state contracts, all seven subsets,
// 301 each (unchanged fast control 301); existing point setter force-inline declaration 301.
// Original contracts and body restored; full baseline remains 290.
// Round 246: Per-case short/int/long state; shared short/int/long state; early per-case/shared short state: 301/310/310/301/310/310/307/307.
// Each state value serves two tests; all signed widths preserve the native short domain.
// All trial forms restored; no candidate qualifies.
// Round 248: Common actor/weapon origin workspace with native/setter producer
// 322/314; with early point 322/314, early origin 322/314, both 322/314.
// All restored; fast control 301, plain baseline 290. Boolean roles and point/case transport remain.
// DH round 257: Shared case handle/receiver/both 301/301/301; byte result on each 310/310/310; both with
// short/long result 330/322. All restored; reduced control 301, full baseline 290.
// Round 424: selected-type-derived point presence in short/word/long/dword at entry
// 384/384/387/387; consumed directly at successful normalization 392/392/394/394.
// All restored; native result/point-valid homes and case-specific point transport remain.
// Round 436 (fast control 301, full 290): point-presence assigned after each successful
// point producer, bool/byte/word/long 310/310/368/364; assigned at the two
// geometry consumers 372/372/393/393. All nonexact presence-lifetime forms restored.
// Round 43: retail copies every point as dwords (struct assignment), not through float setters:
// all five as `point = ...;` plain 306 (aligned 248 -> 131, but 1388 bytes: the copies tail-merge and the
// remembered-prop/actor-direction tail moves after the last case), the state+0x30 copy alone 296, each other
// one alone 375-379 (fast); point_valid given a longer name 290 (homes unchanged).
// Round 29 (lane 11, plain checks; --fast reads 301): both state points as struct assignments 276 (kept;
// retail's `add edi, 0x10/0x30` copies); the state+0x30 one alone 296; on 276 the marker position as well 277
// (1416 bytes), case 3's point 289, case 5's point 293; point_valid renamed to sort after result 290 (on the
// old base, homes unchanged); the two bool declarations reversed 276, `point_valid = result = false` 278 (homes
// unchanged). Left: retail keeps result at the lower byte (0xa) and point_valid at 0xb, and writes the
// remembered-prop code once per case (case 1's copy re-tests state < 1 and cross-jumps into case 2's at the
// 0x1e3b00 call), where ours shares one label after the switch.
// Lane 14 r22 (plain checks): case 1 with its own remembered-prop copy (`state >= 1 && state <= 2`
// arm, then `state < 1 || unknown00 == NONE` to done) 309 (1496 B, no cross-jump); both cases with
// their own copy and no shared label 309; `bool result = false; bool point_valid = false;` 278;
// point_valid set once at the aim-at-point label instead of in cases 3/5/6 281 (1424 B; the Boolean
// homes become retail's: retail stores it at 0x296c67, where cases 3/5/6 jump and case 6 falls in,
// and cases 1/2 enter after the store); with that, `point_valid = result = false` 279 (the entry
// then agrees through both flag stores). Left: retail lays the remembered-prop, actor-direction,
// normalize, output and return code right after case 2 (case 1 cross-jumps into it, case 0 and 4
// into its normalize step) and the aim/weapon code after case 6; its failure exit stores
// result = false before returning.
// Lane 7 r99 (plain checks): remembered-prop code written in each of cases 1 and 2 (case 1 `state < 1 ||`),
// no shared label 288; the direction/normalize/output labels placed in the text after case 2 288 (identical:
// label order never matters, the layout follows the flow graph); plus the aim section as
// `if (aiming) { weapon; if (weapon != NONE) {...} }` 302 (weapon body falls through as in retail, but the
// direction chain moves to the end behind case 2's 0xcafc0 block); in-case direction copies plus
// `goto normalize_direction` 288 (merged back); remembered `if (!aiming) 0x1e3b00(); goto aim_at_point;` 302;
// case 2's non-aiming paths to the aim test 302 (VC threads `aiming`). VC picks one jump predecessor to fall
// into the direction chain and cross-jumps the others' identical tails into it afterwards; retail's pick is
// case 2's remembered 0x1e3b00 block (case 1's copy cross-jumps into it; the 0xcafc0 blocks stay apart).
// N round 257: Actual marker output at function/case/valid-reference/aiming lifetime, original/two-use marker-key observations 322/322/322/322/322/322/322/322.
// Original marker lifetime, key reads and complete body restored; fast control 322, full 276.
// Round 26 (m3 lane 2, plain): the marker position member by member 276 (same code); marker, case 3 and case 5
// points as struct assignments together 306 (1388 B); that with point_valid set once at the aim label 370 (0x297660
// 1); the remembered-prop test as `result = state->unknown00 != NONE; if (!result) goto done;` 375 (retail's
// failure exit, shared by the normalize failures, both remembered NONE tests and case 1's state < 1, stores
// result = false); point_valid renamed to sort after result 276 (homes unchanged).
// Lane 18 r14 (one-link LTCG env with 333 objects, exact for this function: env count 233 = check 276; the
// 0x1ffe00 call now pushes both arguments as in retail): the first difference is point_valid's byte (ours below
// result, retail above); eleven renames of either flag (a_, zz_, valid_point, has_point, success, found ...)
// leave the homes unchanged; `bool result = false; bool point_valid = false;` 235, point first 233, `point_valid =
// result = false` 235, flags before the actor lookup 235 / point first 233. Case 2 with its own remembered-prop
// copy (retail keeps two) 228 at 1484 B: retail's order up to its 0x1e3b00 call, but case 1's copy does not
// cross-jump into it, so the direction/output tail still follows the shared copy; the tail written inside case 2
// is byte-identical (228).
// Lane 13 r44 (plain): each case's remembered-prop branch as its own `else if` (case 1 `else if (reference->state >= 1
// && state->unknown00 != NONE)`, retail's re-test of state < 1) 309 at 1496 B (case 1's copy is retail's code, but
// ours passes &point to 0x1e3b00 twice through one register, `mov edi, ecx`, where retail recomputes `lea edi`
// in the merged tail, so the copies never cross-jump); with point_valid set at a label before aim_at_point 314 at
// 1452 B (0x2967b0 then takes the specification in ECX and 0x297a50/0x297660/0x2980d0 stop matching); with the
// marker, case 3 and case 5 points as struct assignments 261 (kept: retail's dword copies); with the weapon path
// as `if (weapon_index != NONE) { ... }` 257 (kept, one goto fewer); on 257: `point_valid = result = false` 259,
// separate initialised declarations 259, case 1's body under `if (reference)` 257 (same code), the aim test as
// `if (aiming) { ... }` 257 (same code); the struct copies and then-block on the shared label 324 (1380 B).
// Lane 13 r46 (plain): case 0's route records written in full 331 (1444 B, but the index is no longer re-read after
// 0x24590 as retail does); `point_valid = result = false` 259 (homes unchanged); weapon_index/origin in the weapon
// block 257, case 2's player test inverted 257, `if (!result || !point_valid) goto done;` 257 (all the same code).
// A compile-only screen shows two spilled bools take their bytes by usage weight (the more-referenced one gets the
// lower byte), not by name or declaration order: retail's result is the heavier one, ours point_valid (five case
// stores before cross-jumping), which is why setting point_valid once at the aim label gave retail's homes.
// Round 189 (m3 lane 3, fast, control 322): the two flags declared after the other locals, point_valid first and
// result last, result first and point_valid last 322 each (homes unchanged); `point_valid = result = false` with
// point_valid declared first 324 (only the entry store order flips); point_valid set once at a new label before the
// aim test 351, that with the reversed chain 349.
// @retail 0x2967b0
PRIVATE bool function_2967b0(long actor_index, s_type_952051 *specification,
	bool aiming, vector3f *direction, bool *has_target_point, point3f *arg_0e6828)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result;
	bool point_valid;
	result = point_valid = false;
	point3f point;
	s_prop_datum *reference;
	s_type_5cfb45 *state;
	long weapon_index;
	point3f origin;

	switch (specification->type)
	{
	case 0:
	{
		if (!actor->movement_aiming)
			goto done;
		*direction = actor->field_x6d9d38;
		if (function_1f8660(actor_index) && actor->object_index == NONE &&
			actor->field_xe428b1 == 4 && actor->records_50c.index < actor->records_50c.count - 1 &&
			magnitude3d(&actor->field_x6d9d38) < 1.f)
		{
			s_path_records_2967b0 const *route = &actor->records_50c;
			function_210be0(&route->points[route->index].node,
				&route->points[route->index + 1].node, direction);
		}
		goto normalize_direction;
	}

	case 2:
		if (actor->field_x50962e == NONE)
			goto done;
		reference = prop_ref_get(actor->field_x50962e);
		state = function_25d690(reference);
		point_valid = true;
		if (reference->state >= 1 && reference->state <= 2)
		{
			if (aiming)
			{
				long object_index = function_baf80(reference->object_index);
				if (!actor->target_marker_valid)
				{
					function_1ffe00(actor_index, object_index);
					actor->target_marker_valid = true;
				}
				s_object_marker marker;
				if (actor->target_marker && function_b8d30(object_index, actor->target_marker, &marker, 1, false))
					point = marker.matrix.position;
				else
					point = *(point3f *)((byte *)state + 0x10);
				goto aim_at_point;
			}
			long object_index = reference->object_index;
			s_actor_looking_object *object = actor_looking_object_get(object_index);
			if (((1 << object->type) & 3) && object->player_index != NONE)
				function_cafc0(object_index, &point);
			else
				point = *(point3f *)((byte *)state + 0x30);
			goto direction_from_actor;
		}
		else if (state->unknown00 != NONE)
		{
			function_210850((s_type_c3b527 const *)&state->unknown48, &point);
			if (aiming)
				goto local_254539;
			function_1e3b00(reference->object_index, 1, &point, NULL, NULL, &point);
			goto direction_from_actor;
		}
		goto done;

	case 1:
		reference = (s_prop_datum *)record_pool_lookup(g_502418, specification->index);
		if (!reference)
			goto done;
		state = function_25d690(reference);
		point_valid = true;
		if (reference->state >= 1 && reference->state <= 2)
		{
			long object_index = reference->object_index;
			s_actor_looking_object *object = actor_looking_object_get(object_index);
			if (((1 << object->type) & 3) && !aiming)
			{
				if (object->player_index != NONE)
					function_cafc0(object_index, &point);
				else
					function_caf60(object_index, &point);
				goto direction_from_actor;
			}
			function_1caa40(object_index, &point);
			goto aim_at_point;
		}
		else if (reference->state >= 1 && state->unknown00 != NONE)
		{
			function_210850((s_type_c3b527 const *)&state->unknown48, &point);
			if (aiming)
				goto local_254539;
			function_1e3b00(reference->object_index, 1, &point, NULL, NULL, &point);
			goto direction_from_actor;
		}
		goto done;

	case 3:
		point = specification->point;
		point_valid = true;
		goto aim_at_point;

	case 4:
		*direction = specification->vector;
		goto normalize_direction;

	case 5:
		if (actor->attention_state <= 0)
			goto done;
		point = actor->attention_point;
		point_valid = true;
		goto aim_at_point;

	case 6:
		{
			long object_index = specification->index;
			if (object_index == actor->unit_index)
				goto done;
			s_actor_looking_object *object = (s_actor_looking_object *)function_badc0(object_index, 0xffffffff);
			if (!object)
				goto done;
			if (((1 << object->type) & 3) && !aiming)
			{
				if (object->player_index != NONE)
					function_cafc0(object_index, &point);
				else
					function_caf60(object_index, &point);
			}
			else
				function_1caa40(object_index, &point);
			point_valid = true;
			goto aim_at_point;
		}
	default:
		goto done;
	}

aim_at_point:
	if (!aiming)
		goto direction_from_actor;
local_254539:
	weapon_index = function_1e1f20(actor_index);
	if (weapon_index != NONE)
	{
		function_1fc710(actor_index, &origin);
		result = function_1ffbd0(actor_index, weapon_index, 0, &origin, &point, false, direction,
			NULL, NULL, NULL, NULL);
		if (!result)
		{
			vector3d_from_points3d(&origin, &point, direction);
			result = function_30bf0(direction) > 0.f;
			if (!result)
				goto done;
		}
		goto output_point;
	}

direction_from_actor:
	vector3d_from_points3d(&actor->position, &point, direction);
normalize_direction:
	result = function_30bf0(direction) > 0.f;
	if (!result)
		goto done;
	if (!point_valid)
		goto done;
output_point:
	if (has_target_point && arg_0e6828)
	{
		*has_target_point = true;
		*arg_0e6828 = point;
	}
done:
	return result;
}

// score 9 (was 9): this round: the full build scores 9 as well (the permuter's full-image baseline),
// so the reduced image does not distort this one; the lookup's spelling does not change the code
// (see below), and the prologue's register choice is not steerable from the source.
// score 9 (was 9): only the prologue differs: retail builds the actor offset in EAX (`mov eax, ecx`,
// `and`, `imul 0x888`) and forms the actor with `lea esi, [eax + edx]`; ours builds the offset in
// ESI and adds the base. This round: the `&((s_actor_looking_view *)g_4f55f0->data)[index & 0xffff]`
// subscript and an unsigned slot local with a byte-offset product compile to the same code (9), so
// MSVC canonicalises every spelling of the lookup; the body was restored.
// score 9 (was 9): only the prologue differs: retail builds the actor offset in EAX (`mov eax, ecx`,
// `and`, `imul 0x888`) and forms the actor with `lea esi, [eax + edx]`; ours builds the offset in
// ESI and adds the base. This round: every lookup form tried (byte offset local, the same with a
// base local, the masked slot first, the offset inline, typed and byte arithmetic, an unsigned or
// size_t slot, a subscript, a long-multiply getter, an unsigned getter parameter) compiles to the
// same code and scores 9, so the register choice is not steerable from the source; the reduced
// image leaves one global unresolved and flags the call target, so two of the nine are artifacts,
// and ours ends a byte short with a trailing int3.
// score 9 (was 9): this round: eight more forms of the shared getter (a data base and a masked
// slot local in either order, the base added with `+=`, a byte-pointer product with sizeof, a
// pool local, the slot masked in its own local) all compile to the same code (9); an
// `unsigned short` slot local gives 38 (`movzx esi, cx`) and was reverted. The prologue's
// register choice (the offset in EAX and `lea esi, [eax + edx]` against our offset in ESI and
// `add esi, edx`) is not steerable from the source.
// score 9 (was 9): only the prologue differs: retail builds the actor offset in EAX and forms the
// actor with `lea esi, [eax + edx]`, ours builds it in ESI and adds the base. This round: the
// shared getter rewritten as a byte-pointer product (`(byte *)data + (index & 0xffff) * sizeof(...)`)
// compiles byte-identically in all seventeen functions of the file, so MSVC canonicalizes it.
// score 9 (was 9): Round 30 (lane C): the prologue still builds the actor offset in ESI and folds
// the table base into it (`add esi, edx`); retail computes the offset in EAX and forms the actor
// with `lea esi, [eax + edx]`. New forms: a data-base local with a masked slot local, a byte
// offset local and the `&array[index]` subscript form (9 each); an `unsigned short` slot local
// gives 38 (`movzx esi, cx`) and was reverted.
// score 9 (was 9): only the prologue differs: retail builds the actor offset in EAX
// (`mov eax, ecx`) and moves the actor to ESI with `lea esi, [eax + edx]`; ours builds it in ESI.
// Tried: a specification pointer local 9; an if/else chain without the early return 9; the valid
// flag assigned from the `&&` 22; a valid local stored once 47; the getter as `data + index * size`
// 9; a temporary caller (its only retail caller 0x1f38c0 is disabled here) 9; a cdecl compile-only
// build gives ours too; shared actor getter or type 9 each; direct byte-base 9; offset local 9;
// getter forceinline 9; data-base or slot locals 9 each; nested actor getter 9; a mode pointer 9.
// Further forms: direct decoder result 42; low-mode rejection 38, with direct result 45;
// path-first reset 21, with direct result 45; nested independent guards 9; native mode
// snapshot 9; uniform getter observations 58. Original reset path and call values restored.
// @retail 0x296580
void function_296580(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (actor->aiming_direction_type == 0 && !actor->path_active)
	{
		actor->aiming_mode = 0;
		return;
	}
	if (actor->aiming_mode >= 3 &&
		function_2967b0(actor_index, (s_type_952051 *)&actor->aiming_direction_type,
			true, &actor->movement_aiming_direction, NULL, NULL))
		actor->movement_aiming_valid = true;
	else
		actor->movement_aiming_valid = false;
}

real function_11cc90(vector2f const *a, vector2f const *b);
/* Retail's clamped arccos helper; its implementation is outside this file. */
real function_50650(real cosine);

/* Name inferred from the yaw/pitch tests. Failure to decode or normalize
   leaves the direction accepted, as does the actor's bypass flag. */
// @retail 0x297a50
PRIVATE bool function_297a50(long actor_index, bool aiming, s_type_952051 *specification)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = true;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (actor->unrestricted_looking)
		return result;
	vector3f direction;
	if (properties && function_2967b0(actor_index, specification, aiming, &direction, NULL, NULL))
	{
		vector3f forward = actor->forward;
		forward.k = 0.f;
		vector3f horizontal = direction;
		horizontal.k = 0.f;
		if (function_30bf0(&horizontal) != 0.f && function_30bf0(&forward) != 0.f)
		{
			real cosine = dot3f(&horizontal, &direction);
			cosine = cosine < -1.f ? -1.f : (cosine > 1.f ? 1.f : cosine);
			real pitch = function_50650(cosine);
			if (direction.k < 0.f)
				pitch = 0.f - pitch;
			real yaw = function_11cc90((vector2f *)&forward, (vector2f *)&horizontal);
			if (aiming)
			{
				if (fabs(yaw) > properties->aiming_yaw + 0.01 || fabs(pitch) > properties->aiming_pitch + 0.01)
					result = false;
			}
			else if (fabs(yaw) > properties->looking_yaw + 0.01 || fabs(pitch) > properties->looking_pitch + 0.01)
				result = false;
		}
	}
	return result;
}

/* Values assigned by retail startup initializers 378620, 378680, 3786a0. */
real g_55e5bc = 0.17453292f;
real g_55e5c8 = 0.17453292f;
real g_55e5cc = 0.08726646f;

// @retail 0x297660
PRIVATE bool function_297660(long actor_index, bool use_aiming_direction, bool use_character_bounds,
	s_type_952051 *specification)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result = false;
	real yaw_lower = 0.f, yaw_upper = 0.f, pitch_lower = 0.f, pitch_upper = 0.f;
	real scale = 1.f;
	s_actor_looking_properties *properties = function_1e5160(actor->character_definition_index);
	if (properties)
	{
		vector3f base;
		if (actor->look_range_state > 1)
			scale = 0.6f;
		if (use_character_bounds)
		{
			base = actor->forward;
			base.k = 0.f;
			if (function_30bf0(&base) == 0.f)
				base = *g_4687a8;
			real yaw = properties->aiming_yaw * scale;
			yaw_upper = yaw;
			yaw_lower = 0.f - yaw;
			real minimum = 0.f - g_55e5bc;
			real pitch = properties->aiming_pitch * scale;
			real lower = 0.f - pitch;
			pitch_lower = minimum > lower ? minimum : lower;
			pitch_upper = g_55e5bc > pitch ? pitch : g_55e5bc;
		}
		else
		{
			if (use_aiming_direction)
				base = actor->field_xe66477;
			else if (!function_2967b0(actor_index,
				(s_type_952051 *)&actor->idle_aiming_direction_type, false, &base, NULL, NULL))
				base = actor->forward;
			vector3f horizontal = base;
			horizontal.k = 0.f;
			if (function_30bf0(&horizontal) != 0.f)
			{
				vector3f aiming_horizontal = actor->field_xe66477;
				aiming_horizontal.k = 0.f;
				if (function_30bf0(&aiming_horizontal) != 0.f)
				{
					real *angles = actor->field_xe428b1 == 4 ? &properties->moving_aiming_angle : &properties->idle_aiming_angle;
					if (use_aiming_direction)
					{
						yaw_lower = 0.f - angles[0];
						yaw_upper = angles[1];
						pitch_lower = -0.17453292f;
						pitch_upper = 0.17453292f;
					}
					else
					{
						real yaw = function_11cc90((vector2f *)&aiming_horizontal, (vector2f *)&horizontal);
						real cosine = dot3f(&horizontal, &base);
						cosine = cosine < -1.f ? -1.f : (cosine > 1.f ? 1.f : cosine);
						real pitch = function_50650(cosine);
						if (base.k < 0.f)
							pitch = 0.f - pitch;
						real yaw_minimum = yaw - g_55e5c8;
						real yaw_maximum = yaw + g_55e5c8;
						real pitch_minimum = pitch - g_55e5cc;
						real pitch_maximum = pitch + g_55e5cc;
						real yaw_limit = angles[0] * scale;
						real negative_yaw_limit = 0.f - yaw_limit;
						if (!(yaw_minimum > negative_yaw_limit))
							yaw_minimum = negative_yaw_limit;
						if (yaw_maximum > yaw_limit)
							yaw_maximum = yaw_limit;
						real pitch_lower_limit = scale * -0.17453292f;
						if (!(pitch_minimum > pitch_lower_limit))
							pitch_minimum = pitch_lower_limit;
						real pitch_upper_limit = scale * 0.17453292f;
						if (pitch_maximum > pitch_upper_limit)
							pitch_maximum = pitch_upper_limit;
						yaw_lower = yaw_minimum - yaw;
						yaw_upper = yaw_maximum - yaw;
						pitch_lower = pitch_minimum - pitch;
						pitch_upper = pitch_maximum - pitch;
					}
				}
			}
		}
		specification->type = 4;
		result = function_296e60(&actor->position, &base, true,
			yaw_lower, yaw_upper, pitch_lower, pitch_upper, &specification->vector);
	}
	if (!result)
	{
		specification->type = 4;
		specification->vector = actor->forward;
	}
	return result;
}

struct s_actor_looking_structure_view
{
	byte unknown000[0xc4];
	long pathfinding_count;
	s_pathfinding_data *pathfinding;
};

/* Name inferred from the path traces and the idle-direction caller. */
// score 23 (was 23): only rank-driven operand orders remain: the rotation dot (retail j,i,k with axis j
// loaded before the copy), the axis-j-first cross product and the score sum (retail i,k,j).
// counter homes, candidate cursor and expanded rotation scheduling differ.
// entry result order now agrees; loop homes, rotation scalar scheduling and return tail remain.
// Result first 139; existing angular wrapper 468; vector producer 261; both 466;
// native header/body induction with those producers 137/468/266/466.
// Separate forced angular declaration 78/265/85/270; earlier cursor step 88;
// real indexed candidate producer 80, kept at 924/924 bytes after plain confirmation.
// The 78 form retains the old comma-stepped loop and is not kept.
// Angle ordinal renamed, candidate ordinal renamed, both renamed and candidate declared first 80 each;
// candidate/trace scope, entry trace, entry horizontal, all entry workspaces and workspace names 80 each;
// single short ordinal 137; single long ordinal 198; declaration-time path pointer 214;
// int angle ordinal 80; two long ordinals 214; two int ordinals 214. Counter homes, rotation schedule and tail remain.
// Round 162: native trace record 80; full candidate workspace with native/original trace 226/226;
// native trace with direct array consumers 149; native trace with candidate/all vector copies 80/150;
// original trace with candidate/all intrinsic copies 80/150. Both ordinals and all original code restored.
// Existing sine/cosine rotation consumer: named angle 144; sine/cosine locals 80;
// cosine first 86; trig before candidate 80; fraction after rotation 154;
// angle ordinal stepped at last use 156; both shared dot products 144 (regrouped,
// exact-match-only); immediate successful publication return 224. Restored.
// Round 262: pointer-bounded candidate traversal (< / != end), both ordinals 164/164;
// sole angle ordinal on those traversals 191/191; full-width best index 153;
// post-query fraction producer 111; selected-index publication first 82;
// conditional lower-bound update then fraction product 91. Original body restored.
// Round 246: Actual candidate workspace/staged candidate dot/staged selected dot, seven combinations: 211/68/215/84/215/150/211/226.
// Eighth form adds native byte success storage to the complete candidate/score form.
// Kept t02, plain 68; original helper bodies and logical call inputs retained.
// Round 95: projection-first helper with scalar cross terms 51 (kept); score via dot3f 59
// (cursor +0x70 as retail); pointer/index mixes per use 28-141; single short counter with the
// pointer kept 139 (928 bytes); all uses indexed + single counter 33, with the score written out
// i,k,j/i,j,k/k,i,j 25 and j,i,k/j,k,i/k,j,i 23 (kept j,k,i, as the test below), statements 143,
// i+(k+j) 126; named sine/cosine = wrapper; helper parameter order, written operand orders and
// dot term order 23 each; dot3f in the helper 144; cross3f 58.
// Round 204: Rotation projection/cross producers: shared cross/staged cross 156/156; shared projection
// alone/with cross/with staged cross 142/149/149; same-order staged projection alone/with cross/with staged
// cross 139/82/82. Counter/cursor homes and scalar schedule remain; original helper, include set and body
// restored.
// Round 97: helper products written axis-j-first in the dot and cross k as retail computes them 23 (identical
// code); one extra float tuple at entry/at the loop top 48/48 (only the selected-direction sum rotates);
// the axis read once through a local before the loop 70; fraction default before the copy 23; horizontal
// forward as an initializer 199; forceinline in-place helper 23.
// Round 100: score as dot3f(&forward, &directions[i]) 33 (moves the rotation's axis loads ahead of the
// copy, as retail, but flips other product operands); the existing cross-file rotation helpers with
// sine/cosine arguments (cross-first scalars / dot3f with a cross vector) 146/140; the candidate array
// declared before forward 23; forward copied member-wise 199.
// Round 27: fraction as a ternary after the query 144; `1 - cosine` named in the helper 23; cross terms
// before the projection 158 (931 bytes); the 0.6 clamp as an if statement 141 (927 bytes); the dot in its
// own local beside the score 33.
// Round 121: score written forward-first (j,k,i) 23 (same code) / forward-first i,k,j 25; helper with cross i
// before the projection (0x170d70's order) 23; cross terms in a vector3f local 62; the dot named before the
// projection 23.
// @retail 0x297d30
PRIVATE bool function_297d30(long actor_index, s_type_952051 *specification)
{
	bool result = false;
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_actor_looking_structure_view *structure = (s_actor_looking_structure_view *)g_4e0348;
	s_pathfinding_data *pathfinding = NULL;
	if (structure->pathfinding_count > 0)
		pathfinding = structure->pathfinding;
	if (actor->object_index == NONE && !actor->unknown229 && !actor->direction_locked)
	{
		function_26c180(actor_index);
		if (actor->location.unknown10 != NONE)
		{
			vector3f forward = actor->forward;
			forward.k = 0.f;
			if (function_30bf0(&forward) > 0.f)
			{
				function_210770(actor->location.point.output_index, &forward, &forward);
				s_path_trace_result trace;
				function_26c590(actor->location.unknown10, NULL, &trace, pathfinding,
					&actor->location.point.point, NONE, &forward, 1.25f, 0);
				/* Retail tests the first byte, not the helper's return value. */
				if (*(byte *)&trace.unknown00)
				{
					vector3f directions[8];
					short best_index = NONE;
					real best_score = 0.f;
					for (short i = 0; i < 8; ++i)
					{
						directions[i] = actor->forward;
						real fraction = 1.f;
						actor_looking_rotate(&directions[i], g_4687b0, (real)i * 0.785398185f);
						function_26c590(actor->location.unknown10, NULL, &trace, pathfinding,
							&actor->location.point.point, NONE, &directions[i], 5.f, 0);
						if (*(byte *)&trace.unknown00)
							fraction = trace.distance * 0.2f;
						real score = directions[i].j * forward.j + directions[i].k * forward.k + directions[i].i * forward.i;
						score = (0.6f > score ? 0.6f : score) * fraction;
						if (score > best_score)
						{
							best_score = score;
							best_index = i;
						}
					}
					if (best_index != NONE)
					{
						vector3f *best = &directions[best_index];
						if (0.7f > best->j * forward.j + best->k * forward.k + best->i * forward.i)
						{
							function_2105b0(actor->location.point.output_index, best, best);
							specification->type = 4;
							specification->vector = *best;
							result = true;
						}
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x2980d0
PRIVATE bool function_2980d0(long actor_index, bool looking, bool use_character_bounds, vector3f *direction)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (use_character_bounds && actor->unrestricted_looking && actor->attention_enabled &&
		function_297d30(actor_index, ((s_type_952051 *)&actor->idle_aiming_direction_type)))
	{
		actor->idle_aiming_timer = function_297c10(actor_index, use_character_bounds, true);
		actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
		*((s_type_952051 *)&actor->idle_looking_direction_type) = *((s_type_952051 *)&actor->idle_aiming_direction_type);
		actor->attention_selected = true;
	}
	else
	{
		if (actor->idle_aiming_timer == 0 || !function_297a50(actor_index, use_character_bounds, ((s_type_952051 *)&actor->idle_aiming_direction_type)))
		{
			if (use_character_bounds && (random_next(&g_4e7408->unknown0) & 0x8000) > 0 &&
				actor->idle_looking_timer > g_510c54->field_2_3 &&
				function_297a50(actor_index, use_character_bounds, ((s_type_952051 *)&actor->idle_looking_direction_type)))
				*((s_type_952051 *)&actor->idle_aiming_direction_type) = *((s_type_952051 *)&actor->idle_looking_direction_type);
			else
				function_297660(actor_index, true, use_character_bounds, ((s_type_952051 *)&actor->idle_aiming_direction_type));
			actor->idle_aiming_timer = function_297c10(actor_index, use_character_bounds, true);
			actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
			*((s_type_952051 *)&actor->idle_looking_direction_type) = *((s_type_952051 *)&actor->idle_aiming_direction_type);
		}
		if (looking && (actor->idle_looking_timer == 0 ||
			!function_297a50(actor_index, use_character_bounds, ((s_type_952051 *)&actor->idle_looking_direction_type))))
		{
			function_297660(actor_index, false, use_character_bounds, ((s_type_952051 *)&actor->idle_looking_direction_type));
			actor->idle_looking_timer = function_297c10(actor_index, use_character_bounds, false);
		}
	}
	bool decoded;
	if (looking)
		decoded = function_2967b0(actor_index, ((s_type_952051 *)&actor->idle_looking_direction_type), use_character_bounds, direction, NULL, NULL);
	else
		decoded = function_2967b0(actor_index, ((s_type_952051 *)&actor->idle_aiming_direction_type), use_character_bounds, direction, NULL, NULL);
	if (!decoded)
		*direction = actor->forward;
	return true;
}

bool function_1fdd90(long actor_index);
bool function_10f630(long object_index, long *first, long *second);
void function_10e9f0(long object_index, short channel, real value, real time);
point3f *function_b9dd0(long object_index, point3f *result);

struct s_actor_looking_animation
{
	byte unknown00[0x36];
	short state;
};

/* A local view of unit request 0x2d; keep the shared request declaration. */
struct s_actor_looking_turn_request
{
	long type;
	short side;
	byte unknown06[2];
	point3f position;
	vector3f forward;
};

// score 442 (was 442): frame 0xcc versus retail 0xc8, early getter scheduling, vector homes and branch layout differ.
// Tried the out-of-line predicate declaration 546 fast; horizontal aggregate copies 562 fast; initial native setters 546 fast;
// explicit looking success/fallback stores 532 fast, 527 plain (three-point gain, restored); target point or selected vector
// renamed 546 fast each; validity at function scope 546 fast; aggregate inputs plus explicit stores 549 fast;
// out-of-line predicate plus explicit stores 532 fast, 527 plain (restored). Unchanged preview 546 fast.
// Separate noinline declarations in caller and callee units 570 fast, 590 plain at 1952/2020 (restored);
// this removes the expansion and recovers the 0xc8 frame, but later scheduling/layout regress.
// Distinct aiming/looking vector lifetimes, with explicit looking stores,
// nested aiming validation and case-set looking result 546, 534, 546, 534, 546, 534, 546, 534 fast.
// Disjoint outputs with explicit looking publication 525 plain at 2020/2020; restored.
// Uniform 17 aggregate memcpy producers on disjoint outputs, varied by actual
// predicate boundary and long animation carrier 534/540/562/563 fast; native
// memmove variants 624/630/670/671. Best memcpy form 525 plain at 2020/2020; restored.
// Round 224 continuation: Native long parallel validity/idle/both/turn-override/validity-turn/idle-turn/all, then all-word
// 528/539/525/535/529/532/540/553 fast (control 546). Parallel validity plus idle 506 plain,
// 2040/2020 bytes, kept; original producers, tests and call tuples retained. Full frame remains 0xd0.
// Lane T round 28 (older base): Inline predicate with long/unsigned-short/unsigned-long animation storage
// 534/530/534; noinline predicate with those carriers 590 each; ordinary predicate
// with long/unsigned-short 534/530. Original carrier and declarations restored; frame/validity homes remain.
// Round 171: Actual predicate call on retained long validity/idle graph, with independent
// forced/turn/animation carriers, scores 548/538/539/549/544/534/527/541 fast against unchanged 525. All
// regress. Original body, carriers and both predicate declarations restored; frame, early getter scheduling
// and result homes remain.
// Round 173: Cosine-home names, completed unit-query scope, and both 506 each. Entry validity defaults
// alone/with names/with unit scope/with both 528 each at 2020 bytes. All those forms plus entry flag
// defaults 528. Original full body restored; frame 0xd0 versus 0xc8 and early object/result ownership
// remain.
// DH round 239: Native marker/movement/turn-request record-name subsets 506 each;
// all three with the uninitialized marker workspace at entry also 506. Original
// names, workspace scope and whole body restored; frame 0xd0/0xc8 remains.
// DH round 252: One sequential aiming/looking validity result, native long/short/byte/bool
// with long idle, then each with short idle
// 506/549/522/522/540/547/524/524. Original complete executable body restored.
// Y423: High aiming case block first 536; looking case 2 first 525; both 536;
// those orders with sequential validity 536/525/536; high aiming with outer
// validity scope, alone/with looking order 536/536 fast (control 525). Restored.
// Existing validity defaults produced by marker/combat/aiming decoder success tests,
// subsets 1-7, 530/528/530/535/551/535/551; all plus looking decoder tests 526
// (fast control 525, full 506). All reserve 0xcc versus retail 0xc8; exact-match-only
// forms restored, preserving original result storage, scopes, queries and complete body.
// Round 45: late-query/horizontal/turn scalar home names, seven subsets,
// and all with uninitialized query outputs at entry, 525 each (fast control 525,
// full 506). Original names, scopes and complete body restored; frame remains 0xd0.
// Lane 7 r99 (plain): our build inlines 0x1fdd90 (its file has no /Ob1; retail calls it), which keeps the actor
// table in EBX so valid cannot use BL; a one-off /Ob1 on that file gave 569 at 1976 B and, with bool valid and
// idle, retail's 0xc8 frame at 583 (flags restored). Kept: horizontal vectors as aggregate copies 490 (from 506;
// retail copies dwords); the current side named before the test that uses it 446; bool idle 442 (frame 0xcc).
// Then: the two copies swapped 446; bool looking result 523 (on 446), 527 (on 442); bool valid 535 (on 446),
// 543 (on 442); first/second before the unit index 442; turn request declared with them 442; override
// result as `if (0x2967b0(...)) { valid = true; forced = true; }` 535. Retail also packs first/second into the
// two vectors' slots (as if they lived in an inlined helper's scope).
// Lane 18 r14 (one-link LTCG env with 351 objects, exact for this function: env count 411 = check 442; a plain
// noinline declaration of 0x1fdd90 here changes nothing): the side test as `if (first == A || first == B)
// { bool current_side = first == A; ...` 464 (retail's threaded side stores); bool valid 519; that with the side
// test 517; bool looking_valid 513; both bool 516; both with the side test 515.
// Lane 13 r44 (plain): the animation state through the existing 0xe70b0, which retail inlines here
// (`short animation_state; function_e70b0(actor->unit_index, &animation_state);`) 544 at 2000 B: retail's 0xc8
// frame, the actor offset at [esp+0x30] and retail's forward copies, but the state stays in memory (retail
// forwards it into DI) and 0x1fdd90 is still inlined, which keeps the offset in EDI; as
// `*function_e70b0(actor->unit_index, &state)` 544 (same code); with bool valid 552; the idle cosines in a
// block of their own around the query (retail packs them into direction's slots) 544 (same code); direction
// declared at function scope 544 (same code).
// Lane 13 r46 (plain): BLOCKED-BY 0x1fdd90: retail calls it from here only (one E8 call, no address taken in the image)
// and keeps it out of line; ours inlines it, keeping its pool in EBX, and the extra frame dword is `&actor->forward`
// held in a stack slot across the body where retail rematerialises `lea [ebp + 0x290]` at each use. Tried: the
// looking output as an if/else 579 (2080 B); the output flags through actor_looking_get at each use 441 (2020 B);
// a scoped pointer for the two entry forward copies 442; the turn block's forward point declared first 442; the
// request's forward copied before 0xb9dd0 445.
// Round 189 (m3 lane 3): BLOCKED-BY 0x1fdd90: retail calls it (its only retail caller is this function) and
// our build inlines it (27 calls against retail's 28). Fast (control 548): forced defined inside the 0x2973f0
// block 544; animation state long 550; forced with the top bools 548; the target-point pair declared after
// 0x297600 543; forced inside with valid before the direction 544.
// @retail 0x298370
void function_298370(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	s_actor_looking_object *unit = actor_looking_object_get(actor->unit_index);
	short animation_state = ((s_actor_looking_animation *)((byte *)unit + unit->animation_offset))->state;
	bool idle = false;
	bool allow_turn = false;
	bool has_target_point = false;
	point3f arg_0e6828;
	function_297600(actor_index);
	actor->field_xe66477 = actor->forward;
	actor->field_x6f2625 = actor->forward;
	real looking_cosine, aiming_cosine, idle_aiming_cosine, idle_looking_cosine;
	bool forced = false;
	if (function_2973f0(actor_index, &looking_cosine, &aiming_cosine, &idle_aiming_cosine, &idle_looking_cosine))
	{
		vector3f direction;
		long valid = false;
		if (animation_state == 6)
		{
			s_object_marker marker;
			if (function_b8d30(actor->unit_index, 0x4000095, &marker, 1, false) > 0)
			{
				direction = marker.matrix.forward;
				direction.k = 0.f;
				valid = function_30bf0(&direction) > 0.f;
				aiming_cosine = -1.f;
			}
		}
		else if (function_1fdd90(actor_index) && !actor->unknown48a)
		{
			direction = actor->combat_direction;
			valid = function_30bf0(&direction) > 0.f;
		}
		else if (actor->field_xe428b1 < 4 && actor->override_timer > 0 &&
			(actor->aiming_mode < 4 || actor->override_priority >= 1))
		{
			valid = function_2967b0(actor_index, (s_type_952051 *)&actor->override_direction_type,
				true, &direction, NULL, NULL);
			if (valid)
				forced = true;
		}
		else
		{
			switch (actor->aiming_mode)
			{
			case 0:
				if (actor->field_xe428b1 >= 4 && actor->movement_aiming)
				{
					s_type_952051 movement;
					movement.type = 0;
					if (function_2967b0(actor_index, &movement, true, &direction, NULL, NULL))
					{
						valid = true;
						forced = true;
					}
				}
				/* fall through */
			case 1:
				if (function_2982f0(actor_index) && function_2980d0(actor_index, false, true, &direction))
				{
					valid = true;
					idle = true;
				}
				break;
			case 2:
			case 3:
			case 4:
				valid = function_2967b0(actor_index, (s_type_952051 *)&actor->aiming_direction_type,
					true, &direction, NULL, NULL);
				if (valid && animation_state == 7)
				{
					allow_turn = true;
					aiming_cosine = 0.707106769f;
				}
				break;
			}
		}
		if (valid && !actor->unrestricted_looking && !forced &&
			!function_296d60(aiming_cosine, &actor->forward, &direction, actor->unknown229))
		{
			if (allow_turn)
			{
				if (!function_110ab0(actor->unit_index))
					function_e68c0(0x27, actor->unit_index);
			}
			else
				valid = false;
		}
		if (valid)
		{
			actor->field_xe66477 = direction;
			actor->aiming = true;
		}
		else
		{
			actor->field_xe66477 = actor->reference_vector;
			actor->aiming = false;
		}

		long looking_valid = false;
		if (animation_state == 6)
		{
			direction = actor->field_xe66477;
			looking_valid = true;
		}
		else if (actor->override_timer > 0 && (actor->aiming_mode < 4 || actor->override_priority >= 1))
			looking_valid = function_2967b0(actor_index, (s_type_952051 *)&actor->override_direction_type,
				false, &direction, &has_target_point, &arg_0e6828);
		else
		{
			switch (actor->looking_mode)
			{
			case 0:
			case 1:
				if (actor->aiming_mode < 4 && actor->unknown328 < 4 &&
					(actor->field_xe428b1 < 4 || !actor->movement_aiming) &&
					function_2982f0(actor_index) && (!idle || actor->alert_state < 3))
				{
					looking_valid = function_2980d0(actor_index, true, false, &direction);
					if (looking_valid)
						idle = true;
				}
				else
				{
					direction = actor->field_xe66477;
					looking_valid = true;
				}
				break;
			case 2:
				looking_valid = function_2967b0(actor_index, (s_type_952051 *)&actor->looking_direction_type,
					false, &direction, &has_target_point, &arg_0e6828);
				break;
			}
		}
		actor->field_x6f2625 = looking_valid ? direction : actor->forward;
		if (!idle)
			function_297560(actor_index);
		if (actor->unrestricted_looking)
		{
			actor->reference_vector = actor->field_xe66477;
			actor->unrestricted_looking = false;
		}
	}
	if (!actor->unknown229)
	{
		actor->reference_vector.k = 0.f;
		if (function_30bf0(&actor->reference_vector) == 0.f)
			actor->reference_vector = actor->forward;
	}
	if (actor->override_timer > 0 && --actor->override_timer <= 0)
		actor->override_priority = 0;
	if (actor->aiming && !function_110ab0(actor->unit_index) && !function_1f8660(actor_index) && animation_state == 5)
	{
		long unit_index = actor->unit_index;
		long first, second;
		if (function_10f630(unit_index, &first, &second))
		{
			bool current_side = first == 0xf000545;
			if (current_side || first == 0x10000546)
			{
				point2f aiming = *(point2f const *)&actor->field_xe66477;
				point2f forward = *(point2f const *)&actor->forward;
				if (normalize2d(&aiming) > 0.f && normalize2d(&forward) > 0.f)
				{
					real yaw = function_11cc90((vector2f *)&aiming, (vector2f *)&forward);
					bool desired_side = yaw > 0.f;
					if (desired_side != current_side && fabs(yaw) > 0.34906584f)
					{
						s_actor_looking_turn_request request;
						request.type = 0x2d;
						request.side = !desired_side;
						function_b9dd0(unit_index, &request.position);
						request.forward = actor->forward;
						function_e6900(unit_index, (s_unit_request *)&request);
					}
				}
			}
		}
	}
	if (actor->animation_timer > 0 && --actor->animation_timer <= 0)
		function_10e9f0(actor->unit_index, NONE, 1.f, 2.5f);
	actor->output_reference = actor->reference_vector;
	actor->output_aiming = actor->field_xe66477;
	actor->output_looking = actor->field_x6f2625;
	s_actor_looking_view *output = actor_looking_get(actor_index);
	if (actor->attention_selected)
		output->output_flags |= 8;
	else
		output->output_flags &= ~8;
	if (has_target_point)
	{
		actor->output_flags |= 0x8000;
		actor->output_point = arg_0e6828;
	}
	actor->output_mode = actor->alert_state < 2;
}
