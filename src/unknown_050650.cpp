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

/* A switch selects whether the vertical component is compared. */
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
		vector2f aim_horizontal = *(vector2f const *)aim;
		point2f reference_horizontal = *(point2f const *)reference;
		if (normalize2d_fast(&aim_horizontal) > 0.f &&
			normalize2d(&reference_horizontal) > 0.f)
		{
			real aim_y = aim_horizontal.j;
			if (reference_horizontal.x * aim_horizontal.i + reference_horizontal.y * aim_y > cosine)
				result = true;
		}
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

/* function_1697c0 fills 0x5c bytes; the shared result type covers the first
   0x4c (as unknown_1c20f0.cpp's collision buffer) */
struct s_collision_result_buffer_1697c0
{
	s_collision_result_1697c0 result;
	byte unknown4c[0x5c - 0x4c];
};
// MiniMax G (11-13): no new form, no lower score (303): the diff re-read this round shows the trig and magnitude temps four bytes below retail's (ours at +0x1c and +0x20, retail's at +0x20 and +0x24) while the saved arguments agree, and the frame is 12 bytes longer than retail's 1423, so the body stands.
// MiniMax H (11-13): no new form, no lower score (303).
// score 303 (was 303): frame 0xa8 agrees; ours is 12 bytes longer (1435 against 1423). Frame map (F = esp
// after `sub esp, 0xa8`): retail F+0 one transfer slot {magnitude, random integer, pitch sine, yaw cosine},
// F+4 candidate, F+0x10 attempt, F+0x14 axis, F+0x23 result, F+0x30 {pitch cosine, yaw sine}, F+0x38 yaw
// range, F+0x3c pitch range; ours F+0 candidate, F+0xc attempt, F+0x10 axis, F+0x1f result, F+0x20
// {magnitude, both sines}, F+0x30 {random integer, both cosines}, F+0x38 pitch range, F+0x3c yaw range.
// Round 176 (m3 lane 5, fast = full; 0x297660 stays matched): the wrapper's sine named `angle_sine` (the
// homes sorted the other way by name) 303; sine and cosine in one declaration 303; the counter
// post-incremented 303; yaw and pitch declared before the loop 303; the collision buffer declared before the
// ray 303 (all same code). Re-measured: cosine first in the wrapper 300 (0x297d30 23 -> 41, as round 85).
// MiniMax G (11-11): no new form, no lower score (303); the frame agrees at 0xa8, but retail keeps the first
// temporary (the length, the random integer and the sine) at the frame bottom while ours puts it above the result
// byte, so the axis and result homes sit four bytes lower here, and ours is twelve bytes longer than retail's
// (1435 against 1423); this round re-read the diff and the rotations' candidate spill and the temp's home are the
// two pairs that are left, so the body stands.
// MiniMax H (11-11): no new form, no lower score (303); the frame agrees at 0xa8, but retail keeps the firsttemporary (the length, the random integer and the sine) at the frame bottom while ours puts it above the result byte, so the axis and result homes sit four bytes lower here, and ours is twelve bytes longer than retail's (1435 against 1423).
// MiniMax H (11-11): no new form, no lower score (303); the frame agrees at 0xa8, but retail keeps the first temporary (the length, the random integer and the sine) at the frame bottom while ours puts it above the result byte, so the axis and result homes sit four bytes lower here, and ours is twelve bytes longer than retail's (1435 against 1423).
// MiniMax B (11-12): no new form, no lower score (303): the frame agrees at 0xa8, but retail keeps the first
// temporary (the length, the random integer and the sine) at the frame bottom where ours puts it above the result
// byte, so the axis and result homes sit four bytes lower here, and ours is twelve bytes longer than retail's
// (1435 against 1423); the two rotations still spill the candidate between them where retail keeps it in registers.
// Earlier 303 (was 303; 322 before that): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// Earlier 303 (was 303; 322): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// Earlier 303 (was 303; 322 before that): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// Earlier 303 (was 303; 322): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// integer and the sine) at the frame bottom where ours puts it above the result byte, so the axis/result
// homes sit 4 lower; the two rotations keep the candidate in registers in retail (ours spills it between them).
// Walk b4H (11-06, fast 303): statement-by-statement against retail 0x296e60-0x2973ec found no behaviour difference. Checked: pitch axis
// (-j, i, 0) and the 1e-4 magnitude test (0x296ec2) with the g_4687ac fallback (0x296f0c); hoisted yaw/pitch ranges (0x296f32-0x296f66); yaw drawn first, pitch
// second, kept unrounded on the x87 stack into fsin (0x297006); sine/cosine rounded (fstp dword 0x297047/0x29704d); pitch rotation about the axis, then yaw
// about g_4687b0 (0x2970f5-0x2971d0); ray = candidate * 3.0 (0x45dbd4), flags 0x10800001, ids NONE (0x2971e3-0x297206), word NONE at +0x24 (0x29720b); hit test
// `fraction > best` at result+4 (0x29723a); success normalizes the candidate (0x2972a2-0x297334); exit tail `!clear && best > 0.0` (0x297336-0x29734c).
// Plain-form tries: drop `clear`, tail `!result && best_fraction > 0.f` 361 (1446 B, demotes matched 0x297660 to 10); `else clear = true;` 372 (1455 B). Kept `clear = true`
// at the loop top: retail's `mov al, 1` (0x296f8f) sets it there on every attempt, and only this form matches 0x297660.
// MiniMax E (11-06): 303 (was 303): no new form lowered it. Retail puts the first 4-byte
// temporary (the length, the integer and the sine) at the frame bottom where ours puts it above the
// result byte, so the axis and result homes sit four lower, and ours is twelve bytes longer than
// retail (1435 against 1423).
// Earlier note (303, was 303; 322 before that): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// Earlier 303 (322 before that): frame 0xa8 agrees; first +0x19: retail puts one 4-byte temp (the length, the random
// integer and the sine) at the frame bottom where ours puts it above the result byte, so the axis/result
// homes sit 4 lower; the two rotations keep the candidate in registers in retail (ours spills it between them).
// Round 85 (m3 lane 12): `real versine = 1.f - cosine;` named in the rotate-in-place helper 293 (NOT kept (review):
// a one-use local, match only; retail forms the dot then 1 - cos, as the inline product; there the axis/result homes
// agree and first +0x31: retail keeps the length, random integer and sine in one frame-bottom temp; 0x297d30 stays 23); sine/cosine passed in the rotate call 303; cosine computed first 300, with versine 290 (0x297d30 23 -> 41);
// the normaliser's products as `j *= scale`/`scale * k`/all compound 293/293/293; versine and projection after the
// cross terms 286 (0x297d30 23 -> 158); the dot named before/after versine 293/293.
// Earlier at 322: first +0xa acquires pitch inputs before the default vector.
// Normalization scratch at +0x2c versus +0xc displaces axis/result homes; trig and rotation schedules differ.
// Earlier at 333: the frame remains 0x98 versus 0xa8; axis, random/trig and vector schedules differ.
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
// Lane 13 r48 (fast, then plain): on r44's structure (368) the rotation helper as dot3f(v, a) / dot3f(a, v) 344/344
// (0x297d30 23 -> 144), (1 - cosine) first 368, cosine-first rows 368, axis x vector added 357 (0x297d30 48), a named
// dot 349 (158), the projection after the crosses 349 (158), sine-scaled crosses 340 (153), copied-in components 360
// (243). Kept: the collision local as the 0x5c buffer function_1697c0 fills plus `clear = true` first in each pass
// 322 plain (1424 B, retail's frame and prologue; the initialiser then dead and dropped, same code). On 322: the
// single-exit normalize 341 (1449 B), the header normalize3d 340 (1447 B; both put the length temp at the frame
// bottom as retail), bool clear 344 (1435 B), single-exit with bool clear 368, normalize3d with bool clear 367.
// Round 242: Normalizer length/inverse-length and norm/reciprocal homes, four naming pairs
// and two entry reciprocal declarations 322 each; existing length_sq3f and dot3f producers
// 322/322. All original helpers restored; frame 0xa8 agrees, scratch/axis homes and
// initialization/rotation schedules remain. Matched caller and shared rotation user unchanged.
// Round 244: complete normalization outputs via shared scale with named/direct reciprocal,
// shared setter and complete coordinate products: 322/323/322/322; with one length-result exit
// 341/340/341/341. Original private normalizer and sampler restored; frame 0xa8 agrees,
// normalization scratch/axis homes and initialization/rotation schedules remain.
// Round 507: Normalized component publication ikj/jik/jki/kij/kji 321/320/319/324/324;
// complete product publication ikj/jki/kji 322/320/325. Three-point best rejected;
// original normalizer restored. Scratch, axis and rotation homes remain.
// Round 312: complete squared-norm terms 334; initial best setter 346, both 347;
// initial best memcpy 322, with squared terms 334; initial pitch setter 334, with squared
// terms 337; both startup setters with squared terms 342. Original helper and body restored;
// frame 0xa8 agrees, startup root acquisition, normalization scratch and axis homes remain.
// P265: Separate normalization result, zero-before/zero-after/complete-before/root-default,
// native sum 322/322/341/648; shared squared-length 322/322/341/648.
// Original result boundaries and all body/helper text restored; scratch and axis homes remain.
// Round 287: Joint native vector/scalar/normalizer lexical priorities, eight complete forms,
// 322/322/322/322/322/322/322/322. Original helper and sampler restored; scratch, pitch-axis and result-byte homes remain.
// Round 369: normalizer short-vector return/shared success exit/CRT output subsets
// 1/2/4/3/5/6/7: 334/363/322/385/334/363/385; all with Boolean clear 384.
// Exit forms use frame 0xa4 and demote the matched caller to 10; original helper, output copies,
// result flow and complete body restored. Scratch/axis/result homes remain.
// DH297: Test-produced success/failure-owned collision/immediate per-attempt admission,
// subsets 1-7, then fraction-first fallback: 348/322/348/322/348/322/348/360.
// Original Boolean result, collision graph, ten draws, helper bodies and complete sampler restored.
// P276: Existing marked normalizer at axis/candidate/fallback, seven subsets, then all
// with long sample ordinal: 418/417/448/511/543/562/651/652. Calls remain outlined and
// alter caller transport; original helper, declaration and complete sampler restored.
// DH300: Complete normalizer input snapshots, native/setter/library/aggregate/immutable,
// with float absolute, squared-norm or native output: eight 651s (outlined). Forced native/immutable/
// squared snapshots 441/441/452; forced complete scalar inputs 329. Original normalizer and body restored;
// frame-bottom scratch and pitch-axis/result homes remain.
// DH304: Private normalization-success bool/byte/short/long results, direct/common exits, 340/340/340/340/340/340/340/340.
// Original real normalization result, helper and sampler restored; frame-bottom scratch and axis/result homes remain.
// m3 lane 9 r76 (fast): pitch axis by member stores 322; pitch axis declared first 331; attempt at function scope
// 322; candidate at function scope 322; best_fraction before result and clear after the axis 322. Left: retail keeps
// the x87-to-SSE length temp and the draws' fild/fsin temps at the frame bottom (0xc/0x10) with everything above 4
// bytes higher; ours puts them at 0x2c/0x3c, and the two hoisted bound differences sit in swapped slots.
// DH310: Existing staged cross alone/dot/complement/full outputs/cosine/projection/cross-sine/all parts,
// 353/357/353/353/335/357/357/357; other rotation user 58/148/58/58/152/73/150/51 versus 23.
// NOT kept: every shared-helper user regression rejects its form. Original helpers, includes and body restored;
// magnitude scratch, axis/result homes and initialization schedule remain.
// Round 139 (m3 lane 6, fast, plain for the kept form; 0x297660 stays matched): the default best vector declared
// after result and best_fraction 316 (retail's entry order through +0x19); with that a bool clear 303 (kept,
// 1435 B; retail's `mov al, 1`); on 316 best_fraction first 316, clear first 316, the axis before the best
// vector 319; on 303 clear first 303, clear after result 303, the axis before the best vector 313,
// best_fraction first 303; the normalizer's length named length/z_/a_ 303 each; its products all vector * scale,
// all scale * vector, compound `*=`, the sum k, j, i 303 each (same code). Also on 322: the best vector after the
// axis normalization 343 (0x297660 4), the axis declared first 331, the candidate at function scope 322,
// `best_fraction < fraction` 322.
// M336: Returned success owns loop admission or tail termination, bool/byte/short/long,
// with failure-owned best-fraction updates: 351/346/363/361/346/353/363/362. All regress;
// original break, result and whole body restored. Native scratch/axis/result homes remain.
// N346: Complete squared-length node real/const-real/double/const-double, then allowed/short
// admission bool/const-bool: 303/303/350/350/303/303/307/307. Original private normalizer and sampler restored;
// NOT kept: unmatched intermediate/predicate ownership. The rejection Boolean recovers root/axis homes,
// but grows to 1460/1423; later random/sine scratch and rotation lifetime remain.
// Round 60 (m3 lane 13, fast; 0x297d30 tracked): `sinf`/`cosf` in the rotate helper 304 (0x297d30 145); the
// rotation rows with the projection term first 303 (same code); that with the single-exit normalizer 307 (1460 B:
// frame-bottom temp and every home through both random draws retail's; first difference +0x31, where retail loads
// the axis j into its own register and multiplies by the scale, ours multiplies a scale copy by memory); on it the
// normalizer products all `vector->x * scale` or compound `*=` 307/307 (same code).
// AC530: Plain native-array normalization publication, short/long/word/dword indices, explicit/compound
// scaling: 650/305/650/305/650/305/650/305 (fast control 303). Short/word forms outline; full-width forms
// retain 1435 bytes but regress. Original helper and body restored; scratch/axis/result homes remain.
// Y597: Actual normalized-output vector/readonly-vector/opaque/readonly-opaque pointer
// results, direct/shared exits: 357/357/357/357/357/357/357/357. NOT kept: no exact output
// contract or gain; original magnitude return, all consumers and complete sampler restored.
// M389: Root/absolute-value float-library boundaries, singly/together, 307/298/303;
// widened rounded-root/root-producer/reciprocal receivers 546/546/651;
// float root/absolute with double reciprocal 651/651. NOT kept: five-point best
// is below eight and length grows; double forms outline. Original helper/body restored.
// DL265: Complete rotation value/const-value/receiver pointer/const-pointer/reference/const-reference/
// output-pointer/output-reference protocols: 431/431/303/303/303/303/303/303. NOT kept: no exact native
// transport; original shared producer and body restored; root/axis/result scratch homes remain.
// Y597: Actual normalized-output vector/readonly-vector/opaque/readonly-opaque pointer
// results, direct/shared exits: 357/357/357/357/357/357/357/357. NOT kept: no exact output
// contract or gain; original magnitude return, all consumers and complete sampler restored.
// DL270: Complete normalized-vector returns with pointer/value inputs, optional
// real length/Boolean success outputs, ordinary/forced expansion:
// 634/551/642/573/458/458/475/475. NOT kept: all regress and move length away;
// four forms lose the matched caller (39/39/4/4). Original helper and sampler restored.
// F395: Collision-ray component loops, long/short/word/dword direct products 338/337/337/338;
// native candidate copy then in-place loops 376/362/362/376. NOT kept: all regress and
// enlarge the frame to 0xb0; copy forms also demote matched caller 0x297660 to 1.
// Original full body restored; native frame-bottom scratch and candidate transport remain.
// M398: Immutable rounded magnitude/reciprocal/paired angular results, subsets
// 1/2/3/4/5/6/7, then complete immutable rotation products: 303/303/303/303/303/303/303/303.
// NOT kept: no native scratch/home recovery; original producers and entire body restored.
// M402: Existing inline header normalization at axis/candidate/fallback, seven subsets,
// then complete graph with observed axis length: 307/303/299/307/305/299/305/410. NOT kept:
// four-point best is below eight; axis forms recover early homes but grow. Original body restored.
// N390: Explicit absolute-magnitude real/immutable-real producers, success/early-failure exits,
// 307/303/307/303; double/immutable-double producers 360/360/360/360. NOT kept:
// no exact lifetime recovery; original normalizer and sampler restored, native scratch/axis homes remain.
// Round 108 (m3 lane 9, fast with 0x297660/0x297d30; fast = full 303): `if (!actor_looking_normalize3d(&pitch_axis))` 307;
// the pitch draw passed straight to the rotate call 331 (0x297660 1); `__forceinline` on the rotate-in-place helper 303 (same
// code); the helper's dot named (`real dot = ...; real projection = dot * (1.f - cosine);`) 293 (first +0x31, the axis/result
// homes as retail, 0x297660 MATCH, 0x297d30 23; a one-use local, match only, as round 85's versine). On it (re-measures of
// listed forms): the dot summed k, j, i 293; the normalizer with an early `return 0.f;` 298 (masked aligned 192 -> 172);
// `(1.f - cosine) * dot` 293; on the body, `dot3f(vector, axis) * (1.f - cosine)` 307 (0x297d30 144). New on it:
// `real dot = dot3f(vector, axis);` 293 (same code). New on the body: the tail test as `best_fraction > 0.f && !clear` 303;
// the ray as `3.f * candidate.i` 303 (same code); the clear path normalizing `direction` after the copy 313; the best tail
// normalizing `direction` after the copy 338 (0x297660 4); `best_direction = candidate;` before `best_fraction = fraction;` 303.
// Also measured elsewhere (m3 lane 1 r85, fast): `if (!test_collision) continue;` 303 (same code); the NONE store before the
// ray 303; `result = true;` first on the clear path 303; the early-return normalizer 298 (1453 B); the fraction read at each
// use 307 (1460 B); on the early return, cosine first 308 (0x297d30 41), named versine 298, both 308.
// m3 lane 1 r85 (fast = full here; 0x297660 stays matched, 0x297d30 23): the loop body as `if (!test_collision) continue;`
// 303 (same code); the collision NONE store before the ray 303; `result = true;` first on the clear path 303; the
// normalizer's zero case as an early `return 0.f;` with the scaling after the test 298 at 1453 B (register-blind aligned
// 117 -> 54: the length temp moves to retail's frame bottom and the homes follow; the rotations still spill; gain 5,
// not kept), on it cosine before sine 308 (0x297d30 41), a named versine 298, both 308, the normalizer's products as
// `j *= scale`, all compound, `scale * j`, all `x * scale` 298 each (same code: j still becomes a scale copy times memory
// where retail loads j and multiplies); the hit fraction read at each use 307 (1460 B, the same frame-bottom layout).
// Round 178 (m3 lane 4, fast with 0x297660 and 0x297d30; aligned = difflib count with addresses masked, 175 here):
// the collision fraction read from the result at each of its two uses (no fraction local) 307 (aligned 137, first
// difference +0x31: retail's early homes and frame-bottom temps, but both rotations spill and the body grows to 1460
// bytes; the lead to pair with a rotation fix); the clear path's output normalized in place after the copy 313
// (aligned 292); the best-direction tail the same way 338 (1403 bytes, 0x297660 4); both 356 (0x297660 40); the
// no-collision case as an early `continue` 303 (same code).
// m3 lane 12 r106 (fast, 0x297660 matched and 0x297d30 23 throughout): `result = true` before the output store in the
// best-direction tail 303, in both success paths 303, before the candidate's normalize 303 (all the same code); the
// result byte renamed `success` 303 (same home); `return true` in the tail with the collision flag read once through
// its address 359 at 1442 B.
// m3 lane 14 r84 (fast with 0x297660 and 0x297d30; masked = aligned rows with registers and esp offsets masked, base 153):
// `clear = true` after both rotations 345 (1437 B). The lead: sine and cosine evaluated into this function's own named
// locals (not inside the angle wrapper, not as call arguments) put the x87 temps at retail's frame bottom: all four as
// locals before the rotations 263 (frame 0xb0, masked 68); in retail's order (pitch pair, rotation, yaw pair, rotation)
// 210 (frame 0xa8, 1423 B, masked 82: every home through the draws is retail's, the pitch pair's two slots swapped, the
// random integers in the other slot); one pair reused for both rotations 214 (1428 B, masked 68: the integers and the
// pitch pair in retail's slots, the yaw pair's swapped). All match-only (call results read once, and the angle wrapper's
// body written out). On the reused pair: the single-exit normalizer 208 (1423 B, masked 52), the early-return normalizer
// 208 (1424 B), the yaw cosine before its sine 207 (x87 order then not retail's), the best vector declared before
// best_fraction 205 (masked 50, retail's entry stores), with the NONE store before the ray 207 (masked 47, retail's
// collision block order), on that the header normalize3d 214, sinf/cosf 208, the rotation rows k, j, i 277 (0x297d30
// 172), each cross term next to its row 301 (0x297d30 167); axis-first products, an axis-first dot and projection-first
// rows compile to the same code; the four locals with the single-exit normalizer 234; the pair at function scope or
// ahead of the copy: same code. Same code as the base: the draws with __FILE__/__LINE__; the existing rotate_direction
// (unknown_1eb8a0.cpp) with sin/cos arguments (with the named pair, the pair's code); the wrapper merged into one helper
// (0x297d30 23 -> 146). Diagnostics: the named pair for one rotation only 286/223; the pitch pair named with the yaw
// sin/cos as arguments 309; four locals renamed, same code; 0x297d30 with sin/cos arguments 146, with named
// angle/sine/cosine 23. Left on the best lead: the pitch normalize's j preload, the first rotation's allocation (retail
// keeps the candidate's j in a register for the crosses), the exit's dl/ecx load order, the normalizer's product
// orientation (clear path j and k, tail i). Later on the 205 form: the pitch axis set member by member with j first 240
// (1423 B); the tail tested as `!result && best_fraction > 0.f` with `clear` kept in the loop 343 (0x297660 10); the hit
// and clear arms as one if/else (hit first) and the ray set member by member: same code.
// R149 (m3 lane 7, fast with 0x297660 (stays matched) and 0x297d30 (stays 23); register-and-stack-masked aligned counts in
// brackets, control 153): all four sines and cosines computed into named locals ahead of the two rotations, which then
// call actor_looking_rotate_in_place directly, 263 at 1423 B [69] (frame 0xb0 against 0xa8: four trig homes where retail
// rounds them through two shared temporaries; most registers and the frame-bottom temps then follow retail). Not kept: the
// yaw values are one-use locals evaluated ahead of the pitch rotation's stores (match-only). The yaw pair first 298 (1424 B);
// the four after the candidate copy 263 (same code); renamed pitch pair 263 (same code); only the yaw pair hoisted, pitch
// through actor_looking_rotate 305 (1426 B) [93]; the four values passed straight in the two calls 303 (same code as the
// body); each pair with its cosine first 265 [85]; the yaw pair hoisted, pitch values straight in its call 305 [93]. Only
// the pitch pair as named locals (sine, then cosine; before or after the candidate copy, same code), yaw straight in its
// call or through actor_looking_rotate: 286 at 1438 B [72] with retail's frame 0xa8, first difference +0x31. So the
// order and homes of the trig values set the frame: arguments are evaluated right to left (cosine first), the named
// pair gives retail's sine-first order. Also match-only (one-use call results; actor_looking_rotate's body by hand).
// MiniMax A (11-12): no new form, no lower score (303): retail's pitch axis sits four bytes higher in the frame and its
// magnitude temp is kept at the frame bottom, where ours stores it beside the axis, so every slot above them is out by
// four bytes and the body is 12 bytes longer than retail's 1423.
// m3 lane 15 r79 (fast with 0x297660 and 0x297d30; masked = aligned rows with registers and esp offsets masked, control
// 153): a `__forceinline` declaration on the normalize helper 303 (same code); the rotate helper's dot summed k, j, i
// 303 (same code); its three rows computed into named locals before the stores 338 (1440 B, frame 0xb0, masked 82: nearer
// retail's schedule, but 0x297d30 23 -> 153); its rows into a local vector and one aggregate copy back 432 (0x297660 1,
// 0x297d30 203); `clear` cleared only on a hit, the miss branch tested on the call directly, 388 (0x297660 10).
// P467: Direct blocked-state bool/byte/short/long at attempt entry
// 369/369/357/354, after both rotations 374/374/359/356. NOT kept:
// all regress and grow; original clear state, every draw and complete sampler restored.
// @retail 0x296e60
PRIVATE bool function_296e60(point3f const *origin, vector3f const *forward, bool test_collision,
	real yaw_lower, real yaw_upper, real pitch_lower, real pitch_upper, vector3f *direction)
{
	bool result = false;
	real best_fraction = 0.f;
	vector3f best_direction = *g_4687a8;
	bool clear;
	vector3f pitch_axis = { -forward->j, forward->i, 0.f };
	if (actor_looking_normalize3d(&pitch_axis) == 0.f)
		pitch_axis = *g_4687ac;
	for (short attempt = 0; attempt < 10; ++attempt)
	{
		clear = true;
		real yaw = function_259d0(&g_4e7408->unknown0, NULL, 0, yaw_lower, yaw_upper);
		real pitch = function_259d0(&g_4e7408->unknown0, NULL, 0, pitch_lower, pitch_upper);
		vector3f candidate = *forward;
		actor_looking_rotate(&candidate, &pitch_axis, pitch);
		actor_looking_rotate(&candidate, g_4687b0, yaw);
		if (test_collision)
		{
			vector3f ray = { candidate.i * 3.f, candidate.j * 3.f, candidate.k * 3.f };
			s_collision_result_buffer_1697c0 collision;
			collision.result.unknown24 = NONE;
			clear = !function_1697c0(0x10800001, origin, &ray, NONE, NONE, &collision.result);
			if (clear)
			{
				actor_looking_normalize3d(&candidate);
				*direction = candidate;
				result = true;
				break;
			}
			real fraction = *(real *)((byte *)&collision.result + 4);
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

// MiniMax G (11-13): no new form, no lower score (257): the diff re-read this round puts the first difference at the 0x1ffe00 call, which retail makes with both arguments on the stack where this build passes the first in a register; that callee is defined in src/unknown_1ff010.cpp, a file this lane may not edit, so the body stands.
// MiniMax H (11-13): no new form, no lower score (257).
// score 257 (was 257): frame 0x8c agrees; left: the result/point-valid byte homes and the direction chain's
// fall-through predecessor (retail: case 2's remembered 0x1e3b00 block; ours: the merged 0xcafc0 block).
// Round 176 (m3 lane 5, plain): case 1's object index local renamed 257 (same code: the merge is
// machine-level cross-jumping into whichever block is laid out before the chain); one case-2 object index for
// both arms, the aiming arm reassigning it through 0xbaf80, 298; `default: return result;` 298 (1464 B); case
// 6 reading specification->index at each use 257; case 2's prop reference in a named local used twice 257
// (both same code). Re-measured: const specification 257, const state and object views 257.
// MiniMax B (11-11): no new form, no lower score (257); the body is unchanged, and the grids listed above are exhausted.
// MiniMax G (11-11): no new form, no lower score (257); the two flags' byte homes are still swapped against
// retail's (the result byte and the presence byte), the three vector components still reach the stack homes in the
// order EAX/EDX/ECX where retail uses EAX/ECX/EDX, and our case 1 and case 2 blocks still cross-jump where retail
// keeps them apart; this round re-read the diff and the swapped homes follow from the order the two flags are
// written in, so the body stands.
// MiniMax H (11-11): no new form, no lower score (257); the two flags' byte homes are still swapped against retail's (the result byte and the presence byte), and our case 1 and case 2 blocks still cross-jump where retail keeps them apart, so the direction chain's layout predecessor stands.
// MiniMax B (11-12): no new form, no lower score (257): the two flags' byte homes are still swapped against
// retail's (the result byte and the presence byte), our case 1 and case 2 blocks still cross-jump where retail
// keeps them apart, and the direction chain's layout predecessor stands.
// Earlier 257 (was 257): frame 0x8c agrees; the result/point-valid byte homes, the direction chain's layout predecessor
// Earlier 257 (was 257): frame 0x8c agrees; the result/point-valid byte homes, the direction chain's layout predecessor
// (retail: case 2's remembered-prop 0x1e3b00 tail, which case 1's copy cross-jumps into) and the weapon path's registers differ.
// MiniMax D (11-10): no new form, no lower score (257).
// MiniMax E (11-06), at 257 (was 257): no new form lowered it. The result and point-valid byte
// homes are still swapped (ours writes the flag byte at esp+0x12, retail at esp+0x13), the three
// vector components reach the stack homes in the order eax/edx/ecx where retail uses eax/ecx/edx, and
// retail's class lookup block sits before our copy of the direction where ours follows it.
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
// Lane 13 r48 (fast; callers 0x297a50/0x297660/0x2980d0 stay matched): retail routes case 1/2's remembered NONE tests
// and case 1's `state < 1` to the exit that stores `result = 0` (ours: only the two normalize failures); `result =
// false;` before case 1's, case 2's or both trailing `goto done` 342 each (1404 B: the copies tail-merge, the
// 0x1ffe00 call loses its pushes); the remembered branch first (`if (state < 1 || state > 2) { if (unknown00 == NONE)
// goto done; ... }`) in case 2 341 (1404 B), in case 1 (`state < 1` to done first) 305 (1440 B), both 305; the
// object index read before 0x210850 in both remembered branches 309 (1444 B).
// Round 233: Both normalization producers with false-default positive assignment,
// bool/byte/short/long result: 267/263/380/372; provisional success with explicit unordered-safe
// failure correction: 260/266/287/287. Original result graph and complete body restored; narrow
// carriers retain frame 0x8c, wider carriers expand it to 0x90. Result/presence byte roles and
// shared case-tail predecessor remain.
// Round 243 (fast control 341/1404, full 257/1448): direct false admission returns for
// movement/pool/selected-reference/attention/object/default, actor admissions/all admissions
// 396/343/363/343/347/338/343/338. Three-point default forms rejected; all original exits,
// result producers and native point copies restored. Byte homes and direction-tail predecessor remain.
// Round 348: actual point presence true by default, cleared in direction-only cases,
// bool/byte/short/long 355/355/347/347; actual direction-only complement with
// false default 355/355/347/347. Calibrated fast control 258/1448 versus full
// 257/1448; all exact-only predicate-lifetime forms restored with original cases.
// Round 275 (fast control 341): Complementary failure result bool/byte/short/long,
// each with bool/byte point presence, 351/351/351/351/312/312/348/348; short failure
// plain 323 versus baseline 257. Original result, presence and full body restored.
// Round 469: Weapon-aim success-first/failure-first/two-use outcome/common failure join, bool/byte point presence: 270/264/265/258/270/264/265/258 (fast control 258). Original result graph and whole body restored; byte homes and case-tail predecessor remain.
// P264: complete direction/output library contracts, native+memcpy/native+memmove/
// memcpy+native/memcpy+memcpy/memcpy+memmove/memmove+native/memmove+memcpy/memmove+memmove
// 258/333/258/258/333/421/421/415 (fast control 258); original transfers and body restored.
// Round 137 (m3 lane 6, plain; --fast now scores the unchanged body 341 at 1404 bytes, so it cannot screen this
// one; 0x297a50/0x297660/0x2980d0 stay matched): retail's exit at 0x296d22 stores result = 0 and is reached from
// both remembered NONE tests, case 1's state < 1 and both normalize failures. A `failure: result = false; goto
// done;` label after the return taking all of them 257; only the two case exits 256; only the normalize failures
// 257; all of them with the normalize step as `if (!(normalize > 0)) goto failure; result = true;` 260;
// reference and state declared inside case 2's and case 1's own blocks 257.
// DH297: Eight complete direction/point/reference/object case-family orders
// 361/332/356/392/369/364/384/375 (fast control 341; full 257). The 0,4,3,5,2,1,6
// order is 360 plain at 1408/1444, rejected; original cases, predicates and full decoder restored.
// F336: Positive case-2/case-1 remembered-state nesting and normalized publication
// ownership, seven subsets, then positive result/presence with nested optional outputs:
// 341/341/341/341/341/341/341/341 (fast control 341; full 257). Complete original body restored.
// M325: Nullable actual point source, const/mutable pointers at entry/point/state/origin,
// 360 each (fast control 258, full 257); frame 0x90 and ECX specification lose three matched callers.
// NOT kept: unmatched pointer/presence form; original Boolean, frame, calls and whole body restored.
// Round 139 (m3 lane 6, plain): retail keeps case 2's 0xcafc0 block apart from case 1's (ours cross-jumps
// them) and case 1's remembered call jumps into case 2's after its third push. Case 1's object arm ending in
// `goto aim_at_point` like case 6 257 (identical code); point declared first 257; origin declared first 257;
// case 2's object index read from the reference at each use 257; the remembered `if (!aiming) { call; goto
// direction_from_actor; } goto local_254539;` in both cases 257.
// m3 lane 9 r76 (plain): case 2's remembered branch first (inverted state test) 257 (same code); with case 1 as
// `state < 1` exit then its remembered branch first 263 (1440 B); case 1's producers as case 6's form (one
// `goto aim_at_point` after the if/else) 257; case 2's non-aiming if/else swapped 257; `!aiming` first in cases 1
// and 6 257. Left: ours tail-merges case 1's and case 2's cafc0 blocks into the one that falls into the direction
// chain; retail keeps both inline (each `jmp` to the chain) and CSEs neither 0x1e3b00 `&point` (it recomputes `lea edi`).
// B386: Plain real-array direction-input/five-point-input/optional-output loops, subsets 1/2/4/3/5/6/7, then
// direct descending component index: 400/320/317/389/373/332/401/400 (fast control 258; full 257). Original
// record copies and body restored; byte homes and shared-tail predecessor remain.
// AC505: Existing remembered-point producer return at case 2/case 1/both
// consumers, separate then shared receiver lifetimes: 265/258/303/265/258/303;
// both case-owned receivers with byte/short presence 303/386 (fast control 258; full 257).
// NOT kept: ties/regressions; original point arguments, result graph and full body restored.
// Round 85 (m3 lane 12, plain): the weapon fallback as `vector3d_from_points3d(...); goto normalize_direction;` 283
// (1420 B); the output stores swapped 257; point_valid folded into the output test 257 (same code); point_valid set
// before the state lookup in cases 1 and 2 261; before the point copies in cases 3 and 5 257. Body unchanged.
// Round 60 (m3 lane 13, plain): the object index read from the reference at each use in both cases' object arms
// 257, case 2's player test as nested ifs with the `cafc0` call's own `goto direction_from_actor` 257, the
// remembered branches' `if (aiming)` jumping to `aim_at_point` with the weapon label dropped 257, case 2's aiming
// arm jumping straight to the weapon label 257 (all four byte-identical); the flags cleared in two statements 259.
// AC530: Reference lower-bound admission after the state query, byte/char/short/long
// 253/253/265/267; before the read-only query 279/279/296/296 (calibrated fast control 258).
// NOT kept: best gain five is below eight; predicate/observation forms need equality. Original
// reference observations and complete body restored; Boolean homes and shared-tail predecessor remain.
// P345: Movement/attention/object admission-produced return results, subsets
// 1/2/4/3/5/6/7 then separate false defaults: 378/335/335/369/369/323/358/356
// (reduced control 341; full 257). NOT kept: extra SETG/SETNE result stores absent from
// retail; original body restored; byte homes and direction-tail predecessor remain.
// M389: Both native normalization-return scalar receivers, local real/const-real/double/const-double
// 258/258/327/327; shared/test-owned real/const-real/double/const-double 258/258/327/327.
// NOT kept: one-use scalar/result residence did not match; double forms grow to 1480/1444.
// Original comparison producers, calls and complete body restored (fast control 258; full 257).
// DH374: Structured switch-failure breaks at case 0/2/1/5/6/default, both reference cases, then all failures:
// 258/258/258/258/258/258/258/258 (fast control 258; full 257).
// NOT kept: no byte match or eligible gain. Original single-exit decoder, case transfers,
// point/result/presence producers and full body restored; byte homes and remembered-tail predecessor remain.
// P345: Movement/attention/object admission-produced return results, subsets
// 1/2/4/3/5/6/7 then separate false defaults: 378/335/335/369/369/323/358/356
// (reduced control 341; full 257). NOT kept: extra SETG/SETNE result stores absent from
// retail; original body restored; byte homes and direction-tail predecessor remain.
// AC553: Existing actor/object getters forced/private-inline/external-inline, eight
// expansion/linkage pairs: 258 each (fast control 258; full 257). NOT kept: no
// native allocation change; original getter contracts and complete body restored.
// DL269: Shared post-dispatch origin selection and normalization, phase/entry/mutable
// origin pointer or native origin workspace, success-owned then guarded publication:
// 311/311/311/286/311/311/311/286 (fast control 258; full 257).
// NOT kept: every form regresses and shortens the body; original decoder restored.
// DH377: Successful dispatch breaks in cases 3/5/6/all three/1/2/both reference cases/all five:
// eight at 258 (fast control 258; full 257). NOT kept: identical code, no match or gain.
// Original success transfers and complete decoder restored; result/presence homes and shared-tail predecessor remain.
// DH383: Whole-decoder success default with false produced by every rejected dispatch,
// bool/byte/short/long, original comparison results 389/380/375/372; failure-only normalization
// 267/280/300/301. NOT kept: every full-graph lifetime regresses against calibrated 258 (full 257);
// original result/presence, all calls, cases and entire decoder restored.
// b5B (11-06) statement-by-statement walk against retail 0x2967b0-0x296d38, no source change: all seven
// cases, the jump-table order, every offset (0x5d0/0x5ec/0x26c/0x84/0x539/0x53a/0x50c+0x3c, 0x338/0x33c/0x340,
// 0x358/0x370, 0x18, 0x22c position), the signed short state tests (0x24: <1 / >2), the (1 << type) & 3 test on
// [object+0xaa] with the player_index [+0x13c] != NONE test, the 1ffbd0 argument list, comiss > 0.0 (jbe fails),
// the magnitude < 1.0 test (fld const; fcompi; jbe skips), and the optional outputs match retail; no float is
// rounded through fst/fstp, point - origin / point - position are subss in x, y, z order as in retail. No
// behaviour difference found. Only the point_valid/result stack homes (esp+0xa/0xb swapped) and the cross-jumped
// cafc0 blocks differ. Declaring point_valid first 341, separate initialisers 343/341 (fast control 341): dropped.
// Round 182 (m3 lane 4, plain; fast reads 341 at 1404 bytes; 0x297a50/0x297660/0x2980d0 stay matched): the two remembered
// blocks do not cross-jump because ours passes `&point` to 0x1e3b00 through one register (`mov edi, ecx` in case 2,
// `mov edi, eax` in case 1) where retail recomputes `lea edi` after the pushes, so case 1's copy can join case 2's
// after its differing push; the output argument as `(point3f *)point.n` 257 (diagnostic: same code, the address
// is one value). Also: the output test as two nested ifs 257; case 2's remembered branch after
// `if (state->unknown00 == NONE) goto done;` 257; the switch on a `short type` local 257; case 5 as a positive
// `if (attention_state > 0) { ... }` 257; case 2's marker declared with the function-scope locals 257 (all the same code).
// m3 lane 1 r85 (plain; 0x297a50/0x297660/0x2980d0 tracked): the remembered 0x1e3b00 output as `(point3f *)point.n`
// 257 (same code: ours still CSEs &point into EDI); cases 1 and 6's object point (cafc0/caf60/1caa40, one shape in
// retail) as one shared inline helper 398 (1296 B, the helper stays out of line: the inline budget is tight here), with
// a forceinline declaration 257 (same code); cases 3/5/6 ending in `break` with one `point_valid = true;` after the
// switch 368 (retail's byte homes, result low and point_valid high, but the specification moves to ECX and the three
// callers drop to 4/1/2), only cases 3 and 5 merged 354 (1404 B, same flip). Retail has the merged store and EAX.
// Lane 14 r81 (fast with 0x297a50/0x297660/0x2980d0 and every callee, 258 against the full 257; register-masked
// aligned count 110): the 0x1e3b00 reference argument cast to `point3f const *` 258 and the output as
// `(point3f *)&point.x` 258 (both same code: `&point` stays one value, `mov edi, ecx`); case 1's remembered test
// nested (`else if (state >= 1) { if (unknown00 == NONE) goto done; ...`) 258 (same code). Diagnostics, reverted:
// case 1's remembered branch removed 364 (1396 bytes); case 1's cafc0 call removed (caf60 only) 211 at 1408 bytes
// with the masked count 110 -> 24: the rest of the layout is then retail's (direction chain after case 2's
// remembered block) and only the result/point_valid homes and the weapon call's setup differ. So the merged cafc0
// tails of cases 1 and 2 set the layout. Matched 0x1b2770 calls 0x1e3b00 with the object load ahead of the pushes
// and a separate `lea edi` output (a different local). Here retail loads the object after the third push and
// recomputes `lea edi` for the same `&point`, which lets case 1's remembered copy join case 2's instead; ours loads
// the object first and copies the pushed address (`mov edi, ecx` / `mov edi, eax`).
// Round 111 (m3 lane 9, fast with 0x297a50/0x297660/0x2980d0 and every callee, control 258, full 257): `result = false;
// point_valid = false;` as two statements 260; same code: case 6's lookup with `NONE` for its second argument, the output
// test with `!= NULL`, the normalize tests as `0.f < function_30bf0(direction)`, the aim test as `if (aiming) goto ...; goto
// direction_from_actor;`; re-measured: case 1's remembered test nested (lane 14 r81) 258 (same code). The layout cause
// stays: retail's case 1 and case 2 0xcafc0 blocks are byte-identical (`lea eax, [esp + 0x14];
// mov ecx, ebx; call; jmp 0x296a18`) yet stay apart, and the direction chain follows case 2's 0x1e3b00 block, which case 1's
// copy joins after its third push because retail recomputes `lea edi` for the output and loads the object after the pushes;
// ours merges the two 0xcafc0 blocks into the one block that falls into the chain and passes &point to EDI by `mov edi, ecx`.
// MiniMax B (11-11): no new form, no lower score (257); retail's two byte homes are the result at +0x12 and the presence
// byte at +0x13 (the frame is 0x12 once the two pops are counted) and here they are the other way round, and both
// compiles reference them equally often, so the order follows the block layout.
// MiniMax A (11-12): no new form, no lower score: retail keeps case 2's block apart from case 1's where ours cross-jumps
// them, and the fast image builds the callee 0x1e1f20 differently, so the score it shows (341) is above the full build's.
// m3 lane 15 r79 (fast with 0x297a50/0x297660/0x2980d0 and every callee, control 258 = full 257; masked = aligned rows with
// registers and constants masked, 103): case 2's player arm as its own block with `goto direction_from_actor` 258; case 1's
// 1caa40 path first (`!((1 << type) & 3) || aiming`) 258; one function-scope object index for cases 1, 2 and 6 258;
// both remembered branches with the non-aiming 0x1e3b00 call first 258 (all same code, masked 103); the flags in one
// initialised declaration 260 (entry stores swap). Masked, what is left is the merged cafc0 blocks, the 0x1e3b00 setup
// (ours loads the object before the pushes and copies the output address, `mov edi, ecx`; retail loads it after the
// third push and recomputes `lea edi`, so case 1's copy can join case 2's) and the weapon call's actor reload.
// P467: Missing-point true default, false at each original point producer,
// bool/byte/char/short/word/int/long/dword: 343/343/343/382/382/382/382/382
// (fast control 341; full 257). NOT kept: no equality or reduced gain; original decoder restored.
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

// MiniMax H (11-13): no new form, no lower score (9).
// MiniMax A (11-12): no new form, no lower score (9): three fresh spellings of the lookup (the byte-offset
// product through the pool data, the offset computed before the pool read, and a byte base with an offset
// local) all compile to our code - MSVC keeps the scaled index in the pointer register and adds the base,
// where retail keeps the product in eax and forms the actor with a lea; the shared getter is used by sixteen
// matched functions of this file, so its spelling cannot change on its own account.
// MiniMax H (11-11): no new form, no lower score (9); retail still builds the actor offset in EAX and forms the actor with a lea where ours keeps the scaled index in the pointer register and adds the base, and the shared getter cannot be respelled because sixteen matched callers depend on it.
// MiniMax D (11-14): no new form, no lower score (9): only the prologue differs - retail loads the pool table into
// EDX, computes the actor offset in EAX (`mov eax, ecx`, `and eax, 0xffff`, `imul eax, eax, 0x888`) and forms the
// actor with `lea esi, [eax + edx]`, while this build keeps the scaled index in ESI and adds the base (`add esi, edx`);
// the getter is shared with sixteen matched functions of this file, so its spelling cannot change on its own account,
// and retail's only caller of this function, 0x1f38c0, has no body in this tree, so the call context that shapes the
// allocation is missing here.
// MiniMax B (11-11): no new form, no lower score (9): the getter is shared with sixteen matched callers, so no
// spelling of it can be changed on its own account, and none of the subscript forms reaches retail's prologue.
// MiniMax E (11-10, run 73): no new form, no lower score (9); retail builds the actor offset in EAX and forms the actor with lea esi,[eax+edx] where we build the offset in ESI and add the base, and the shared getter's spelling cannot change because sixteen matched functions depend on it.
// MiniMax E (11-10): no new form, no lower score (9): four more spellings of the actor address (a byte base
// local with an offset local, the offset local alone, the typed subscript and the base after the offset) all
// compile to the same code as the shared getter: ours keeps the scaled index in the pointer register and adds
// the base, retail keeps the product in eax and forms the actor with a lea.
// MiniMax E (11-09): no new form, no lower score (9): the masked slot in a local with the lookup written out
// and the slot multiplied by the element size both compile to the same code as the shared getter, and the
// mask written as a word truncation gives 38; the prologue still builds the actor offset in ESI and adds the
// base where retail keeps it in EAX and forms the actor with a lea.
// MiniMax C (11-08): forms tried, none lower: the byte offset in a local 9, the masked slot in a local 9, the getter written out 9, the base in a local 9; the permuter was killed at its time limit before it searched anything.
// MiniMax D (11-06): no new form, no lower score (9); five more spellings of the shared lookup (a data
// base and a masked slot local in either order, a byte offset local, the slot first, the current one)
// compile to the same code, so retail's offset in EAX and its `lea esi, [eax + edx]` are not
// steerable from the source; the reduced image also leaves our body one byte short.
// MiniMax E (11-07): score 9 (was 9): forms tried, none lower (the masked byte offset in a local, the
// data base in a local and the inline byte-pointer product with the literal size all compile to the same
// code); the prologue still builds the offset in ESI and adds the base where retail keeps it in EAX and
// forms the actor with a lea; retail's matched sibling 0x297a50 uses the same getter and compiles the
// same way as ours (the chain in ESI and an add), so what differs here is the allocation in this one
// function, not the spelling of the lookup.
// MiniMax E (11-06): score 9 (was 9): no new form tried this round; the prologue still builds the
// actor offset in ESI and adds the base where retail builds it in EAX and forms the actor with a lea,
// and ours ends a byte short with a trailing int3; two of the nine are reduced-image artifacts.
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
// MiniMax A (11-12): no new form, no lower score (9): the lookup written in this body - the plain inline expression, the
// byte-pointer product, the address of the array subscript, the masked index local and the offset local - compiles to our
// code in every case; MSVC keeps the scaled index in the pointer register, so retail's `imul` in eax and its
// `lea esi, [eax + edx]` are not reachable from the source. Original body restored.
// MiniMax C (11-09): score 9 (was 9): retail's prologue is one byte longer than ours (its `lea esi,
// [eax + edx]` against our `add esi, edx`), so our code is 113 bytes and retail's 114. Eight more
// getter spellings all compile to our code and score 9: an unsigned offset local, the byte-pointer
// product `(data + (index & 0xffff) * sizeof(view))`, an array-pointer subscript, the same product
// with the base added last, an unsigned index local, the array-address form, a signed literal
// product and a masked slot local. MSVC keeps the scaled index in the pointer register, so the
// `imul` never lands in EAX as retail has it.
// MiniMax C (11-09): score 9 (was 9): only the prologue differs: retail builds the actor offset
// in EAX (`mov eax, ecx`, `and`, `imul 0x888`) and forms the actor with `lea esi, [eax + edx]`,
// ours builds the offset in ESI and adds the base. A byte subscript, a typed subscript and an
// offset local all compile to the same code (9), so the register choice is not steerable here.
// MiniMax C (11-09): score 9 (was 9): the byte-pointer product spelled with a masked slot local
// (`(s_actor_looking_view *)(g_4f55f0->data + actor_slot * sizeof(s_actor_looking_view))`)
// compiles to the same code as the shared getter (9): MSVC keeps the scaled index in the pointer
// register, so the `imul` never lands in EAX as retail has it. Original body restored.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 9 (114 bytes, as retail's); the only difference is the prologue, where retail builds the actor offset in EAX (`mov eax, ecx`, `and`, `imul 0x888`) and forms the actor with `lea esi, [eax + edx]`, while ours builds the offset in ESI and adds the base (`add esi, edx`), one byte short; the shared getter is used by sixteen matched functions of this file, so its spelling cannot change without losing them.
// MiniMax R (11-10): no new form, no lower score (9); the body is unchanged, and the shared lookup keeps
// compiling to the same code.
// MiniMax A (11-11): no new form, no lower score (9); the shared getter written as a byte-pointer product, as a base plus an offset local, as a slot local and as a two-statement add compiles byte-identically in all seventeen functions of the file.
// MiniMax R (11-11): no new form, no lower score (9): only the prologue differs - retail builds the actor offset
// in eax (`mov eax, ecx`, `and`, `imul 0x888`) and forms the actor with `lea esi, [eax + edx]`, ours builds the
// offset in esi and adds the base (`add esi, edx`), one byte short; the call to 0x2967b0 is still unverified in
// the reduced image, so the reduced-image target address differs as well.
// MiniMax B (11-12): no new form, no lower score (9): three more ways of writing the actor lookup in this
// function's own body - a masked slot local added to the typed base, the byte-pointer product and the
// integer base plus the masked index - all compile to the same prologue (9), so MSVC keeps folding the
// address computation into ESI with the base load in EAX; retail keeps the base in EDX and the index in EAX.
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
// Round 73: all-component/axis-only/vector-only/score array views 23/23/23/23; both dot views 39;
// candidate reference 139; reused axis-j scalar 158; staged projection j,i,k/j,k,i/i,j,k/i,k,j
// 60/146/32/21; two-stage projection j,i/i,k/i,j/k,j/k,i/j,k 31/22/31/32/22/32;
// two-stage loop score 124. A 2400 s permuter startup timed out before its baseline. Restored.
// Round 58: forward's k cleared through an inline pointer helper with the score as written 27, written i,k,j / k,i,j / k,j,i 21
// (the selected-direction sum becomes retail's, the loop score j,k,i; one-caller helper, -2, not kept); a named forward.j at the score
// 69; score via dot3f 33; named direction i/j/k at the score 124/128/142 (928 bytes); pointers to forward's i/j/k at the score 23/25/33,
// to the direction's i/j/k 33/33/33. Compile-only rotation region (control 15): 432 statement and sum orders, 270 score/selected/helper
// combinations and 577 one-use named leaves: none below 15, dot stays k,j,i. Pointers to fields in the helper (per use, 5,250-step search) do
// move the dot to retail's j,i,k: compile-only 9, real 17 at best (pointers at all uses but the dot's j and k vector terms, the axis j of
// cross i, the axis k of cross j and the i and k result rows; mixed per use, not kept), its 24 one-use neighbours none lower; left there the
// axis j load ahead of the copy and the score's order; other searches real 25/25/135 (928 bytes). Pointers at every use of axis i+j 29, axis
// k+vector j+k 127, axis k+vector j+i 128, axis k+i+vector i 139, axis j+vector j 33, axis j+vector k 29, vector k+axis j 31, axis k 138,
// axis j 140, axis j+k 144, all of the axis 61 (928 bytes where a three-figure count). The score i,k,j never appeared in about 2,300 compile-only forms.
// Round 159 (fast; one plain control agreed): rotation with a separate result parameter, same pointer passed twice 68; copy inside the helper with separate input
// and output 478 (660 bytes); score `dot3f(&directions[i], &forward)` 33; named angle with sine and cosine passed to the in-place
// helper 146 (927 bytes); axis local in the loop body with the fraction before the copy 174 (910 bytes); `dot3f(axis, vector)` in
// the helper plus the dot3f score 142 (926 bytes).
// Round 86 (fast): k identity locals, k = 1..8, on the forward vector 33 at k = 1 and 23 otherwise; of the actor 141 (927 bytes); of the
// axis in the helper 61/61/61/173/173/173/83/61; of the helper's vector 23 each; on the call's axis argument 173/173/61/61/61/61/83/61; on its
// vector argument 23 each. Compile-only (register-mapped rotation region, control 15): real chains on cosine, sine and projection and
// axis chains for the dot or the cross only, none below 15; axis components in named reals, six orders, 46-81. Retail's axis j reads lead two
// products, an order the plain field reads have not given.
// Source forms (fast): fraction declared before the copy and set after the rotate 136 (927 bytes); `best_score < score` 23; the clamp as
// `score < 0.6f` 23; the angle named before the copy 33; the selected test as `dot < 0.7f` 23.
// Round 99 (m3 lane 12, fast): both 0x26c590 calls through its real signature instead of the header's
// compatibility overload 39 (first call only 39, loop call only 23, same code); the candidate copy as memcpy
// 82; sinf/cosf in the angle helper 145 (928 bytes). Diagnostics: the candidate not passed to 0x26c590 43, nor
// to 0x2105b0 74 (axis j still loaded after the copy, so the candidate's escape is not what orders it).
// Round 140 (m3 lane 7): the existing cross-file forceinline rotate_direction (0x1ecf50's file) with a named
// angle 33 (plain; the rotation block is byte-identical to ours, only the selected-direction test flips, so
// cross-module inlining does not set the ranks); the helper writing a result vector copied back 203; the
// vector's i, j, k named in the helper everywhere 243, everywhere but the dot 150; the axis's i, j, k named
// 233; both named 216. Diagnostics (fast, same code): forward and the candidate array renamed.
// Round 91 (m3 lane 11, fast): explicit parentheses are honoured. The loop score as `(i + k) + j` gives retail's
// score order and also the rotation dot's (j, i, k, axis j loaded ahead of the copy), but the pathfinding push sinks
// below the rotation and the cross k, cross i and k-row products load the other operand: 124 at 928 bytes (same code:
// `j + (i + k)`, `j + (k + i)`, fraction before the copy, the constant first in the angle, the best locals before the
// array; equal to round 73's two-stage score). On it: the selected test grouped `i + (j + k)` 143; cross terms
// written k, j, i 133 (j, k, i same code); cross terms first 140; rows k, j, i 49 (924 bytes, the cursor moves to j);
// dot3f(vector, axis) or (axis, vector) in the helper 35 (924 bytes); cross3f or cross3f_staged into a local 145;
// dot3f and cross3f 43; rotate_direction with sine/cosine locals 128; best score declared first 126; the score
// named `dot` before the clamp 124 (other code). Grid with the selected test: loop `(i + j) + k` 128, `(j + k) + i`
// 142; flat loop score with the selected test `(j + k) + i` 60; dot3f loop score with dot3f(best, &forward) 35.
// LEAD, not kept: the helper's dot parenthesized `(j + i) + k` on top gives 13 at 924 bytes, plain too (left: the dot's and
// the score's accumulator choice and a j-row register swap; inner written orders are canonicalised, 13 each), but
// it raises 0x296e60 303 -> 311, and retail's 0x296e60 sums its two dots (k + j) + i and (i + j) + k, so a fixed
// grouping in the shared helper is not retail's. The helper's dot parenthesized with the flat score 31.
// Kept: the selected test as dot3f(best, &forward), same code (the hand-written dot was dot3f's body).
// Round 111 (m3 lane 16, fast; control 23): forms ahead of the rotation, as perturbations of the operand ranks: the pathfinding
// lookup as a ternary initializer 207; without the structure local (the global cast at both uses) 143 (926 bytes); the three
// entry tests as nested ifs 23; `0.f < length` 23; the first trace test as `!= 0` 23; the best score declared before the best
// index 25; the best pair declared before the candidate array 23 (23 = same code); cosine computed before sine in the angle
// wrapper 41 (0x296e60 303 -> 300). Diagnostics (not source forms): one store through `specification` at entry keeps 924 bytes
// and loads every axis component ahead of the candidate copy (123: retail loads only axis j there); a read at offset 0 or +4 of
// `specification` at entry or ahead of 0x26c180 216-240 (943-948 bytes). The ranks move with what the function dereferences
// before the loop, not with the helper's spelling.
// Round 162 (m3 lane 8, fast with 0x296e60 0x297660 0x2980d0; control 23): pre-loop and tail forms as rank perturbations, all
// worse: the origin point named once for both trace calls 93; the output index named once for 0x210770 and 0x2105b0 197 (frame
// changes); the specification's vector stored before its type 31; `result = true` before the specification stores 25.
// Re-measured: the pathfinding pointer by if/else 207 (the listed ternary's code); the fraction declared after the rotate 136
// (927 bytes, the listed form's code).
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
						if (0.7f > dot3f(best, &forward))
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

// MiniMax B (11-14): no new form, no lower score (442 plain, 548 on the reduced image): the entry block still differs
// in its registers - retail loads the actor table into ECX, takes the row with `mov ebp, [ecx + A]`, keeps the actor
// index in ESI and spills the scaled offset to the stack, and reads the member word with `movsx edx` and `mov di`,
// where this build loads the table into EDX, keeps the index in EDI, scales it in ESI without a spill and reads the
// word with `movsx ecx` and `mov dx`; the frame stays 0xd0 against retail's 0xc8 and 0x1fdd90 is still inlined here
// where retail calls it, and that helper is defined in another file. The body stands.
// MiniMax G (11-13): no new form, no lower score (442 plain, 548 on the reduced image): the diff re-read this round keeps the frame 0xd0 against retail's 0xc8 and the cached animation words without retail's homes, and 0x1fdd90 is still inlined here where retail keeps it out of line, so the body stands.
// MiniMax H (11-13): no new form, no lower score (442 plain, 548 on the reduced image).
// score 442 (was 442): frame 0xcc versus retail's 0xc8. BLOCKED-BY 0x1fdd90: retail calls it from its only
// call site and ours inlines it (only /Ob1 on its file keeps it out of line: a build-flag lead).
// Round 176 (m3 lane 5, plain): the action state through 0xe70b0 held in a named by-value local 544; the turn
// block's planar vectors built from members (no `&actor->forward` cast) 450 (2032 B), that with the 0xe70b0
// call 528; the state-7 test nested under `if (valid)` 442; direction declared at function scope 442 (both same
// code). Re-measured: `function_e70b0(actor->unit_index).value` (the lookup's own body) 544 at 2000 B (retail's
// 0xc8 frame and offset spill, the state on a stack word without DI), with bool valid/looking_valid 561.
// MiniMax G (11-11): no new form, no lower score (442 plain, 548 on the reduced image); retail still calls 0x1fdd90
// out of line with the slot offset spilled to [esp+0x30] and the actor index in ESI where ours expands the call
// inline and keeps the index in EDI, and that helper is defined in another file, so its inline decision cannot be
// moved from here; this round re-read the diff and the entry block and the frame (0xcc against retail's 0xc8) are
// what that call leaves, so the body stands. BLOCKED-BY 0x1fdd90.
// MiniMax H (11-11): no new form, no lower score (442 plain, 548 on the reduced image); retail still calls 0x1fdd90 out of line with the slot offset spilled to [esp+0x30] and the actor index in ESI where ours expands the call inline and keeps the index in EDI, and that helper is defined in another file, so its inline decision cannot be moved from here. BLOCKED-BY 0x1fdd90.
// MiniMax C (11-11): no new form, no lower score (442); retail never inlines 0x1fdd90, so the block's difference is that helper's, and it lives in another file.
// MiniMax A (11-11): no new form, no lower score (442 plain, 548 on the reduced image); retail calls 0x1fdd90
// out of line with the slot offset spilled to [esp+0x30] and the actor index in ESI, ours expands the call inline
// and keeps the index in EDI, so the entry block and the frame (0xcc against retail's 0xc8) differ; the helper is
// defined in another file, so its inline decision cannot be moved from here. The statement walk the earlier rounds
// recorded agrees with the source on every block, condition, callee and constant, so what is left is that call and
// the register and frame layout. BLOCKED-BY 0x1fdd90.
// MiniMax D (11-10): no new form, no lower score (442): retail calls 0x1fdd90 out of line with the slot offset spilled to [esp+0x30] and the actor index in ESI where ours expands the call inline and keeps the index in EDI; 0x1fdd90 is defined in another file.
// b2J (11-06) statement walk against retail 0x298370..0x298b3e (align.py 548 fast, 442 full): every block, offset,
// condition direction, callee, argument order (0x2980d0 looking/bounds flags, 0x2967b0 type pointers +0x420/+0x434/+0x688 and the movement temp, 0x10f630 first=ESI,
// 0x11cc90 aiming/forward), constant (-1, 0.70710677, 0.34906584, 1.0, 2.5) and flag home agrees with the source; no behaviour difference
// found. Remaining differences are the inlined 0x1fdd90 and register/frame layout only.
// MiniMax E (11-06): 442 (was 442): no new form lowered it. The frame is 0xcc against retail's
// 0xc8 and ours is four bytes shorter (2016 against 2020); retail keeps the actor index in ESI and
// spills the slot offset to [esp+0x30] where ours keeps the index in EDI and the offset in ESI.
// MiniMax H (11-04): no new form, no lower score; retail calls 0x1fdd90 at +0x198 where our build expands it inline, and marking it noinline gives 590 and is not a permitted form; the frame stays 0xcc against retail's 0xc8 and the actor table is held in a register.
// MiniMax B (11-12): no new form, no lower score (442): the frame is 0xcc against retail's 0xc8 and ours is four
// bytes shorter (2016 against 2020); retail keeps the actor index in ESI and spills the slot offset to
// [esp+0x30] where ours keeps the index in EDI and the offset in ESI, and retail calls 0x1fdd90 at +0x198 where
// our build expands it inline.
// Earlier 442 (was 442): frame 0xcc versus retail 0xc8, early getter scheduling, vector homes and branch layout differ.
// Earlier 442 (was 442): frame 0xcc versus retail 0xc8, early getter scheduling, vector homes and branch layout differ.
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
// Lane 13 r48 (plain; --fast reads 548 for the unchanged body, with a CAVEAT): the entry copies chained
// (`field_x6f2625 = field_xe66477 = forward`) 548 fast (same as the fast control); the looking output published on
// each path (direction on success, forward on failure, field_xe66477 for case 6, retail's per-path copies) 637
// (2132 B); the output flags through `actor` with no re-get 590 (1956 B: the actor table is then no longer held in
// EBX); that with bool valid 585 (1960 B, frame 0xc4, valid in BL as retail); the re-get before the three output
// copies 461. The EBX hold comes from three users of the table (entry, the inlined 0x1fdd90, the re-get); retail
// has only the two, so BLOCKED-BY 0x1fdd90 stands.
// m3 lane 9 r76 (plain): the turn block reading actor->unit_index at each use 442; forced declared with the top
// flags 445; the aiming output as `valid ? direction : reference_vector` plus `aiming = valid != 0` 548 (2040 B);
// the target-point output stores swapped 457 (2008 B); the override decrement as nested ifs 442 (same code).
// Round 145 (m3 lane 5, plain): the override arm's `if (valid) forced = true;` as `forced = valid != 0;` 551 (1992 B);
// looking_valid declared with valid at the block top 442; the marker workspace at the block top 442; case 0's
// movement request at the block top 442; the turn request at the turn block's top 442 (all same code). Retail
// never inlines 0x1fdd90 anywhere (its byte pattern occurs only at 0x1fdd90), so BLOCKED-BY 0x1fdd90 stands.
// Y606: Real vector arrays, initial plain long/int/short/dword loops 564/564/568/564;
// CRT copy/move 564/527, components 541, both initial/final 564, final alone 548
// (fast control 548; full 442). NOT kept: CRT move rolls calls absent from retail
// and loses the input register; other forms regress or tie. Original type/body restored.
// Y608: Whole-lifecycle forward/aiming/looking member references, seven subsets,
// 548/548/540/548/540/540/540 fast; complete set with reference-vector member 526
// (control 548, full 442). NOT kept: no exact out-of-line predicate/forward-address
// residence; member-address lifetimes are unproven. Original complete body restored.
// DH379: Terminal attention presence/absence bool/byte/short/long, after the output getter:
// 548/548/548/548/548/548/548/548 fast, against its unchanged reduced control. NOT kept: no exact
// attention result or frame/pool recovery; original field test and complete body restored.
// m3 lane 1 r85 (plain; 0x1fdd90 tracked): the twice-written movement-aiming and override tests as shared inline helpers
// (also in 0x2982f0) 592 (1932 B: both helpers stay out of line here, inlined in 0x2982f0, and 0x1fdd90 stays inlined);
// with a temporary noinline on 0x1fdd90 (diagnostic) 616, the helpers still out of line: their sites are refused by
// call-site frequency, not by a budget; forceinline helpers 442 (same code); the state through the existing 0xe70b0
// (by-value return) 544 at 2000 B (re-measured; aligned 260 -> 229, retail's 0xc8 frame and copies), with 0x1fdd90 noinline
// (diagnostic) 591 at 1952 B (entry agrees through `push esi`; the index then takes EDI for ESI, the state stays in memory
// for DI); that with the aiming chain under `switch (animation_state)`'s default 542 (0x1fdd90 still inlined, so a colder
// site does not keep it out); the looking output per path 637 at 2132 B (re-measured); the side lookup (0x10f630 and the
// two side tests) in its own inline helper scope, as retail's shared slots suggest, 527 at 1980 B (the helper stays out of
// line); the two unused idle cosines in a block of their own around the 0x2973f0 query 442 (same code). Keeping 0x1fdd90
// out of line is at most necessary, not enough: /Ob1 on its file (lane 7 r99) 569 and the noinline diagnostics 591/616.
// MiniMax A (11-12): no new form, no lower score: our frame is 0xd0 against retail's 0xc8 and the cached animation words do
// not have retail's homes; the fast image shows 548 because the callees outside the reduced image build differently there.
// m3 lane 15 r79 (fast with all nineteen callees, which reads 442 = the full check; the 0x110ab0 calls push their
// argument as in retail, so that unmatched callee is not the blocker): 0x1fdd90's own body as one `&&` return 548
// (0x1fdd90 then 8 at 51 B, still inlined here); the animation-timer decrement as nested ifs 442; the three entry
// flags in one initialised declaration 442; the entry copies chained, re-measured on this image, 442; the target-point
// local renamed 442 (all same code). BLOCKED-BY 0x1fdd90 stands: retail keeps it out of line (its only caller) and
// our build inlines it; what keeps it out of line so far is only a build flag on its file (lane 7 r99's /Ob1, 569).
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
