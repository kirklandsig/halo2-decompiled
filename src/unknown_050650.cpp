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
#include "normalize3d.h"

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
	real magnitude = magnitude3d(vector);
	if (!(fabs(magnitude) < 0.0001f))
	{
		vector3f_scale(vector, 1.f / magnitude, vector);
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
// score 270 (was 239): Frame 0xa8 agrees. The startup zero register, axis j preload and later magnitude/random/trig scratch
// differ. The 239 form (r157, below) is restored to main's body: on the tree merged with main it lost 0x1c4590 and 0x24cf66.
// MiniMax G (11-20): no new form, no lower score (270): the rotation dot's term order (k, j, i) regresses this sampler to 324
// and the other orders compile to the same code, so the dot stays as it is; the frame agrees at 0xa8 and the first difference is
// still the zero's register (XMM1 against retail's XMM2) at +0x31, and the rest (the trigonometric transfer slots, the candidate
// spilled between the two rotations and the collision argument schedule) needs the rotation written out at the call site, so the
// body stands.
// MiniMax E (11-20): no new form, no lower score (270): the frame agrees (0xa8) and the body is 1431 bytes against retail's 1423; the zero still lands in XMM1 where retail has XMM2, and this build stores it before the three component stores where retail stores it after them - swapping the two declarations measures 267, below the keep bar, and the rest (the trigonometric transfer and the collision argument scheduling) needs the shared helper, which would raise a matched caller's score.
// Frame 0xa8 agrees. The pitch-axis, result-byte and initial magnitude homes
// now agree. First +0x31 selects XMM1 for zero instead of XMM2. Random-integer/trigonometric
// transfer and collision argument scheduling still differ. 1431 bytes against retail's 1423.
// Earlier at 299: 1433 bytes, first +0x19. Frame map before the ray scale change (F = esp
// after `sub esp, 0xa8`): retail F+0 one transfer slot {magnitude, random integer, pitch sine, yaw cosine},
// F+4 candidate, F+0x10 attempt, F+0x14 axis, F+0x23 result, F+0x30 {pitch cosine, yaw sine}, F+0x38 yaw
// range, F+0x3c pitch range; ours F+0 candidate, F+0xc attempt, F+0x10 axis, F+0x1f result, F+0x20
// {magnitude, both sines}, F+0x30 {random integer, both cosines}, F+0x38 pitch range, F+0x3c yaw range.
// MiniMax E (11-15): no new form, no lower score (303): the frame agrees at 0xa8 and the diff is still the temp's
// home (retail keeps the length, the random integer and the sine in one frame-bottom slot, ours keeps the length
// above the result byte) and the candidate, which retail holds in registers across the two rotations while ours
// spills it between them; recovering either needs the rotation written out at the call site or the normaliser's
// length inlined, and its magnitude3d sits in another file, so the body stands.
// MiniMax S (11-14): no new form, no lower score (303): the frame agrees at 0xa8 and the diff is the temp home (retail
// keeps the length, the random integer and the sine in one frame-bottom slot, ours keeps the length above the result
// byte) and the candidate, which retail holds in registers across the two rotations while ours spills it between them
// because the rotation helper writes through a pointer. Recovering either needs the rotation written out at the call
// site or the normaliser's length inlined, and its magnitude3d (0x24590) sits in another file, so the body stands.
// MiniMax G (11-13): no new form, no lower score (303): the diff re-read this round shows the trig and magnitude temps four bytes below retail's (ours at +0x1c and +0x20, retail's at +0x20 and +0x24) while the saved arguments agree, and the frame is 12 bytes longer than retail's 1423, so the body stands.
// MiniMax H (11-13): no new form, no lower score (303).
// MiniMax C (11-16): no kept form: an early return in the file-local normaliser moves its magnitude into retail's
// frame-bottom slot (296) but leaves the pitch axis four bytes low, the sine and the random draw in their own homes
// and the rotation block 28 bytes longer than retail's, so the original body stands.
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
// m3 lane 5 r181 (fast with 0x297660 and 0x297d30; control 303, aligned 175, masked 85): kept (same code, plain check):
// the normalizer's length through the existing magnitude3d (0x24590), which replaces its hand-written body. New: the
// normalizer's test as `fabs(magnitude) >= 0.0001f` 343 (1439 B); the rotate wrapper's angle as a double 369 (1451 B,
// 0x297d30 23 -> 120); both draws through the existing slot_random_range 365 (1449 B, 0x297660 1). Every other
// rotation, normalizer, draw, collision and exit form this round considered is already listed above, so no untried
// natural form is left; the open lead stays lane 14 r84's named sine/cosine pairs (match-only).
// P467: Direct blocked-state bool/byte/short/long at attempt entry
// 369/369/357/354, after both rotations 374/374/359/356. NOT kept:
// all regress and grow; original clear state, every draw and complete sampler restored.
// m3 lane 15 r84 (fast with 0x297660 MATCH and 0x297d30 23 throughout; control 303): `__assume(forward != NULL)` at the top
// 298 (1453 B; `__assume(direction != NULL)` gives the same code), `__assume(origin != NULL)` 307 (1460 B), `__assume(attempt <
// 10)` at the loop top 334 (1453 B); the pitch axis through the existing vector3f_assign 303 (1447 B); the ray through it 297
// (1453 B, the assume form's code family; under the bar); both 306 (1452 B). The lower counts are one code family (any extra
// statement ahead of the loop gives 1453 B and first difference +0x31), not a local change toward retail. Body unchanged.
// P518: Existing fallback normalizer at pitch/clear/best, masks 1/2/4/3/5/6/7,
// then pitch fallback library copy: 438/303/303/438/438/303/438/303. NOT kept: no exact
// scratch/rotation repair; original helpers, all observations and complete body restored.
// Round 247 (m3 lane 3; fast, kept change confirmed plain): kept: the normalizer scales through the existing vector3f_scale
// (include/normalize3d.h) in place of its three hand-written products, 303 -> 299 at 1433 B (0x297660 MATCH, 0x297d30 23,
// the file's other functions unchanged). On it: the helper as `return normalize3d(vector);` 300 (1455 B); single exit with `else
// magnitude = 0.f;` 305 (1458 B); an early `return 0.f;` for the short case 296 (1451 B, not kept: -3); a result local set in the
// scaling arm 299; the rotate helper's dot as k, j, i / j, k, i / j, i, k 299 each (same code). Before it (base 303): the
// rotate helper caching the vector's k in a local (0x2551f0's helper spelling) 293, but 0x297d30 23 -> 245; that with the
// sines and cosines passed straight (no angle wrapper) 303; the vector's i, j and k all cached 300 (0x297d30 243); the best
// vector declared before best_fraction 300.
// P548: Complete rotation-axis native/immutable vector, scalar/immutable scalar,
// aggregate/immutable aggregate, existing setter and library captures:
// 406/406/335/335/335/335/346/406. NOT kept: all regress and every form raises
// the other rotation user (23 -> 210/233/234); three also lose the exact caller.
// Original shared helper and entire sampler restored; native scratch/axis homes remain.
// P550: Complete best-hit promotion in bool/byte/short/long 299/299/299/299; backward transfers
// at pitch fallback/candidate/winner/both final outputs 350/348/352/359. NOT kept: no qualifying
// native scratch/home repair; original full body, helper graph and headers restored.
// Frame-bottom magnitude/random/sine scratch, axis/result homes and rotations remain.
// P552: Standard forward and uninitialized component transfers at pitch fallback, sample acquisition,
// best-hit retention and both final publications.
// Fast scores 349/350/352/348/349/350/352/348. NOT kept: no permitted confirmed gain; original body restored.
// P554: Existing rotation scalar receivers, reference/pointer-axis, reference/reference-axis,
// value/pointer-axis/vector-output, reference/value-axis, value/component-output,
// vector/component-output, reference/reference-axis-components, value/reference-axis-components:
// 305/305/339/335/339/293/295/304. NOT kept: scalar contracts lack native call evidence and every sibling score rises
// (23 -> 142/142/243/232/243/33/34/231); original shared helper and complete sampler restored.
// P556: Actual ray/result/candidate/axis/best-vector/final-output/fraction/best-fraction
// lvalue receivers: 299/305/339/305/307/352/306/299. NOT kept: no native scratch/home equality;
// original complete sampler and every shared helper retained.
// P558: Existing normalizer with separate input/output pointer/pointer, reference/pointer,
// pointer/reference and reference/reference contracts, both formal orders:
// 295/295/295/295/295/295/295/295. NOT kept: no native scratch/home equality; original private helper,
// in-place calls and entire sampler restored.
// P560: Existing private normalizer with explicit scalar magnitude output, vector/scalar
// pointer/reference domains and both formal orders: eight 295 at 1448/1423 bytes. NOT kept:
// four-point reduction is below eight and length grows away; root/axis homes recover but zero/scale
// SIMD, trig transfer and rotation allocation still differ.
// Original magnitude return, all three consumers and complete sampler restored.
// P581: Rounded random-yaw/random-pitch/best-fraction promoted receiver storage, seven
// subsets and immutable draw results: 332/362/363/335/379/366/371/371. NOT kept: no exact frame-bottom scratch,
// axis/result-home and candidate-rotation recovery; original complete sampler and helpers restored.
// P590: Original incoming origin/forward/collision/yaw bounds/pitch bounds/output
// fixed bindings: eight 299 at 1433/1423. NOT kept: no exact frame-bottom
// magnitude/random/sine or axis/result recovery; original full body restored.
// P592: Single in-place normalizer reference, direct/single/early exits, reciprocal and
// immutable-magnitude variants: 299/305/296/301/299/307/298/301. NOT kept: native scratch/axis homes
// unresolved; three-point best lengthens to 1451/1423. Original helper and full body restored.
// Round 192 (m3 lane 5, fast with 0x297660 (stays matched) and 0x297d30 (stays 23); masked = aligned rows with registers and
// esp offsets masked; control 299, masked 154): lane 14 r84's match-only lead rebuilt on today's body (one sine/cosine pair
// reused for both rotations through actor_looking_rotate_in_place, the best vector before best_fraction) 214 (1426 B,
// masked 67). On it: the rotate helper's dot named 214 (same code); the candidate copied at the top of the pass, before the
// draws, 251 (masked 129); the up axis through a `vector3f const *` local in the pass 214 (masked 68). On the body: that up
// pointer 308 (1446 B); each rotation's sine/cosine pair as named locals in its own block 217 at 1423 B (masked 86), with
// the best vector before best_fraction 214 (1423 B, masked 84). All match-only (the wrapper's body written out or a one-use
// pointer); body unchanged.
// Earlier 299 (was 299): MiniMax D (11-17): no new form, no lower score (299): the frame-bottom homes still differ -
// MiniMax D (11-17): no new form, no lower score (299): the frame-bottom homes still differ -
// retail's three locals sit at [esp+0x1c]/[esp+0x20]/[esp+0x24] against this build's [esp+0x18]/[esp+0x1c]/[esp+0x20],
// retail's flag byte is at +0x2f against this build's +0x2b, and retail keeps the magnitude result at [esp+0xc] where this
// build keeps it at [esp+0x2c]; the masked count is 154, so what is left is the homes rather than the statements, and
// lane 14 r84's reused sine/cosine pair is a match-only form (its body written out), which is not a permitted shape.
// Shared normalizer at pitch/clear/best 305/299/299; pitch-clear/pitch-best/clear-best 305/305/299;
// private in-place normalization by reference 299, with shared pitch normalization 305.
// NOT kept: the pitch forms recover the early axis and magnitude homes but add 25 bytes.
// P608: Paired random draws through one field pointer, object pointer, field reference or
// object reference, at loop or draw scope: eight 296 at 1451/1423, first +0x31.
// NOT kept: three-point gain, longer body and different generator loads despite native entry slots.
// Earlier score 303.
// Round 416: Existing in-place rotation with named pitch/yaw/both trig pairs
// 307/226/217, and direct pitch/yaw/both 305/296/299. Named pitch with direct
// yaw 286, direct pitch with named yaw 228. The existing angle helper using the
// shared rotation 289. NOT kept: the 1423-byte forms at 217/226/228 require
// one-use trig results. The helper form grows and raises its other user's score
// from 23 to 245. The 217 form recovers the axis, result byte and magnitude homes.
// Ray lifetime ending at the call, hit-record lifetime ending at the fraction, both,
// obstruction-first branch, its scoped ray, its fraction storage and its continue: seven 299s.
// The existing scale helper for the collision ray scores 270 alone and with the obstruction-first branch.
// The scale-only form is retained after the plain check.
// Deferred clear publication 374; one normalized candidate 437, selected vector
// pointer 474, separate selection arms 516; deferred store through candidate 340
// or best direction 348; normalize best direction after loop 446; deferred clear
// store with separate fallback 358. NOT kept: all raise the score and change the
// matched caller; the frame-bottom magnitude/draw/trig home remains.
// The initial pitch-axis slots and the shared magnitude, random and sine transfer word still differ.
// Planar magnitude before/after axis initialization 328/328; fallback with planar length 339;
// direct scaled-axis setter 309; planar normalization with field stores/setter 341/357;
// fallback-first planar normalization 407; planar length from initialized axis 302. NOT kept:
// the last form restores two pitch-axis component slots, moves the result byte and lengthens the body.
// The startup zero register and random/trig transfer word still differ.
// Attempt- or function-scope candidate with the two draws and turns in one block, inner pitch block, pitch-only block, and pitch-only block with immutable draws all score 270 (eight forms).
// Best fraction at the pitch-axis declaration or after fallback 267/292; result at those
// two points 327/319; attempt initialized at the axis 302. Implicit trailing axis zero 384;
// zero-initialized axis with component stores 404, also with later best fraction 404.
// NOT kept: the 267 form gains three at the opening length and the 1423-byte result form
// changes the matched caller. Zero register and later transfer still differ.
// Ray before the draws or across attempts 270/270; query record at attempt entry or across
// attempts 270/270; query default before scale 270; scale return passed to query 270; ray copied
// and scaled in place 354; default direction before fraction 267.
// NOT kept: the three-point gain retains the wrong zero register and transfer word. The ray copy
// also raises the caller's score.
// Round 264 (m3 lane 3, fast with 0x2967b0, 0x297660 (matched) and 0x297d30 (23); a single-address fast build reads
// 412 at 1353 B, so always measure with the same set; control 270, aligned 198, register-masked 70): 0-8 dead
// initialised long locals at the top 270 each (same code: no symbol-count window); the pitch axis set field by field,
// j first 306, i first 270 (same code); the collision ray written out component by component (i, j, k or j, i, k)
// 299 each; the wrapper's trig through sinf/cosf 273 (0x297d30 145); yaw and pitch declared at function scope and
// assigned in the loop 270 (same code). Reading: register-masked, what is left is the rotations' operand ranks
// (retail's dot adds the i term last, ours the j term), the clear-path magnitude order (retail k, j, i; ours k, i, j)
// and the two transfer slots holding {magnitude, draws, first sine, second cosine} in retail, the cosine set in ours.
// The pitch-axis j product still takes a reciprocal copy instead of preloading j.
// Paired horizontal components 270, with the axis setter 296; copied-then-negated lateral component 361.
// Magnitude supplied to the scaling stage directly, named for the axis, named for both outputs,
// all named, or immutable 333/333/335/335/333. NOT kept: the split magnitude enlarges the frame
// and leaves the initial zero register and later random/trigonometric transfer different.
// Fraction initialized from axis zero 271; axis zero initialized from fraction 271; fraction initialized
// between normalization and fallback 381; default direction setter 365; pitch fallback setter 331;
// both initial vector setters 345; best-hit direction setter 350; both best-direction setters 356.
// NOT kept: the setter forms at the best-direction producers also change the matched sampler caller.
// M3 lane 7 r194 (fast with 0x2967b0, 0x297660 (MATCH) and 0x297d30 (23); control 270; a one-file link with the files of
// 0x259d0 and 0x24590 reproduces 1431 B, aligned 198): `if (!test_collision) continue;` 270 (same code); the clear tail
// copying the candidate out and normalizing the output in place 288 (1426 B); the best tail that way 348 (1400 B, 0x297660
// 4); both 376 (1388 B, 0x297660 40). One-file link diagnostics: the random state read as a global aggregate member
// (aligned 368, 1426 B); the axis globals read that way (aligned 198, same code).
// M3 lane 10 r131 (fast with 0x2967b0, 0x297660 (MATCH), 0x297d30 (23), 0x297a50, 0x2980d0, 0x298370 and the callees
// 0x1ffe00/0x1e1f20; control 270). Reading: retail's two transfer slots hold {magnitude, both draws, first sine, second
// cosine} and {first cosine, second sine}, i.e. the four trig values take slots in the order they are produced (sine
// first in the pitch rotation, cosine first in the yaw rotation); ours homes the wrapper's two named locals per name in
// both expansions. Retail's normalizer j product loads j first (`j * scale` in xmm1, the zero then in xmm2). Forms: the
// normalizer's length read through `real const &length = magnitude` 332 (1435 B); the wrapper's sine and cosine each
// through a const reference 274 (1429 B; 0x297d30 23 -> 60); the pitch-axis normalize and fallback in a block of their own
// 270 and each rotation in a block of its own 270 (both same code); the pitch rotation through the wrapper and the yaw
// rotation through the in-place helper with `(real)sin(yaw), (real)cos(yaw)` as arguments 331 (1434 B); the pitch-axis
// length bound to `real const &` and tested afterwards 282 (1437 B).
// Direct clear, hit-first, hit with continue and disabled-collision skip without a separate
// clear carrier, each with unpublished result or positive fraction first at fallback 344/335 each.
// NOT kept: all raise the score at retail length and change the matched caller. The 344 forms
// also shrink the 0xa8 frame.
// Round 498: zero-first comparison, named axis length, scoped axis length, separate initial best-vector assignment, scaler result in the trace call, named obstruction, sampling names, immutable hit fraction.
// Fast scores in order 270/270/270/270/270/270/270/270.
// NOT kept: All eight trials tie at 270 and 1431 bytes. Every control keeps its opening score.
// The frame stays 0xa8 on the opening body. The zero register, pitch-axis j preload and shared random/trig transfer word still differ.
// M3 lane 12 r158 (h2m fast with 0x297660 (stays matched) and 0x297d30 (stays 23 unless noted); control 270): the
// header's shared normalize3d in place of the file-local normalizer at the three uses 282 (1437 B); the file-local normalizer
// in that helper's single-return shape (zero set in an else arm) 282 (1437 B); the rotate helper's dot as `dot3f(axis,
// vector)` 282 (1437 B; 0x297d30 144); its cross terms with the axis as the left operand of each product 270 (same code);
// its output rows with the cosine product written `cosine * vector->i` 270 (same code).
// Separate normalizer source and destination for axis 340, clear sample 333, best hit 342,
// axis and clear 338, axis and best 331, clear and best 343, all three 349. Normalizer owns
// both the scaled and unchanged direction writes 554.
// NOT kept: the separate forms grow the frame to 0xb0 and the best-output forms change its
// matched caller. The complete output form outlines the math and reserves 0xac.
// Optional fallback in the normalizer: alternate arm / success exit / magnitude zero test / short-axis exit
// 456/456/459/461. Shared double-factor scale with the real reciprocal on those forms 476/476/479/481.
// NOT kept: the normalizer stays out of line and every form grows the frame to 0xac while shortening the sampler.
// Independent fallback components with aggregate output or the shared setter, by entry placement: original
// 377/377, before fraction 377/377, after pitch-axis construction 380/380, there with fraction 380/380. NOT
// kept: the 0xa4 frame, longer body and changed matched caller depart from retail.
// M3 lane 5 r224 (lead, not kept): the shared in-place rotation with each cross term scaled by the sine where it is
// computed (rows subtract the scaled term) 270 -> 199 (1433 B, plain agreed), but it raises 0x297d30 23 -> 153.
// Yaw-axis reload after hit processing, before the fraction test, or at trace return
// 370/317/314. Axis pointer before the draws, after them, or between the turns
// 282/282/283. Axis reference before candidate initialization or after the pitch turn
// 282/283. NOT kept: the carried pointer changes the matched caller and the per-attempt
// bindings lengthen the sampler. The startup zero and trigonometric spills still differ.
// Axis height initialized from fraction, fraction from axis height, and chained zeros 271 each.
// Setter with chained zero 292. Ascending short with tail less-than or terminal inequality 570 each.
// Direct clear trace with collision-first or fraction-first fallback 339/340.
// NOT kept: startup zero remains in XMM1 and the tail count changes the matched caller.
// M3 lane 2 r131 (H2M_FAST with 0x297660 (MATCH), 0x297d30 (23), 0x2967b0 and 0x24590; control 270, aligned 198, masked
// 71): the fallback test as `0.f == normalize(...)` 270 (same code); the rotate helper's rows with the sine term ahead of the
// projection term 287 (1438 B; 0x297d30 31); the rotate helper with the vector's k read into a local and the crosses scaled
// by the sine 317 (0x297d30 244); best_fraction initialised after the pitch axis 267 (under the bar); lane 5 r224's
// sine-scaled crosses re-measured 199 (0x297d30 153). Body unchanged.
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): actor_looking_rotate's in-place step through include/axis_rotation.h's helper 416 (1347 B; 0x297d30
// 245). Restored.
// M3 lane 5 r238 (h2m fast with 0x297d30 (23), 0x297660, 0x2967b0, 0x24590, 0x2980d0, 0x297a50; control 270, aligned 198
// exact / 71 register-masked). Reading: retail's x87-to-SSE transfer slots hold {magnitude, both draws, first sine,
// second cosine} and {first cosine, second sine}; this build's hold {magnitude, both cosines} and {draws, both sines},
// and each rotation's pair written out in its own block gets the same slots as the wrapper (331 at 1434 B, frame 0xb0),
// so variable identity does not set them. An automated search over the shared in-place rotation's spelling, every
// form screened on both users (63 forms: the dot's six term orders and product sides, the projection's operand
// order, six declaration orders and four product-side forms of the cross terms, the sine scaled in the cross or in the
// row, four row term orders and each row product's sides): 44 forms compile to main's code (270 / 23), the dot with k
// ahead of j 324 / 23, the cross terms with k ahead of j 268 / 39, sine-scaled crosses 199 / 153 (with those cross
// orders 196 / 155), rows `cos - sin + proj` 287 / 31 and `proj - sin + cos` 340 / 44: no form lowers this body
// without raising 0x297d30. The wrapper's cosine computed first 274 (0x297d30 41); the candidate copied between the
// two draws 330 at 1423 B (aligned 383: worse); the up axis through a pointer local 283 (1438 B). Body unchanged.
// M3 lane 15 r139 (h2m fast with 0x297660, 0x2980d0, 0x297d30 (23) and 0x24590; control 270): the normalizer's length local
// renamed (slot-name order) 270 (same code); the pitch axis passed through a const pointer local 328 (frame 0xb0); the
// candidate through a pointer local for both turns 334; both draws held as double 341 (1425 B, frame 0xb0, aligned 329
// against 198); the candidate declared ahead of the draws and assigned after them 270 (same code). Body unchanged.
// M3 lane 6 r241 (h2m fast with 0x2967b0, 0x297660 (MATCH), 0x297d30 (23), 0x24590, 0x2980d0, 0x297a50; control 270):
// the pitch axis set member by member with k first 282 (1437 B); the candidate copied member by member i, j, k 265 at
// 1423 B (aligned 275 against 128), j, k, i 265 (same code), k, j, i 294; `clear` initialised true once before the loop
// with no per-pass reset (the same behaviour) 328. Forcing single allocator decisions (69 of them) never matches (best
// 266) and leaves the 22 operand-order differences (both rotations' products and the normalizer's j product, which
// retail forms as j * scale) untouched, so the remaining work is operand rank, not register choice. Body unchanged.
// M3 lane 11 r166 (h2m fast with 0x2967b0, 0x297660 (MATCH), 0x297d30 (23 unless noted), 0x24590, 0x2980d0, 0x297a50; control
// 270, aligned 198, with registers, XMM numbers and stack offsets masked 71). Rank reading: with the brace initialiser the
// pitch axis's members are early symbols (below the reciprocal), so the normalizer forms scale * j; retail's j * scale and
// its first dot order (k + j) + i need the pitch axis as a memory object handed to helpers by address (first member early,
// j and k numbered late). The pitch axis through the existing vector3f_assign: 296 (1440 B, frame 0xb0, aligned 246, masked
// 67): the normalizer's three products and every product and sum of the pitch rotation then have retail's operand order,
// with two extra spills in that rotation and the trig transfer cells exchanged; LEAD, build/r166/lead-0x296e60-axis-
// setter.patch. On it: through real_point3d_set, the ray declared ahead of the draws, or best_fraction declared after
// best_direction, the same ranks (not screened further); best_fraction after the pitch axis 293 (masked 65); the candidate
// copied member by member 295 (1431 B, masked 105), through vector3f_assign 290 at 1423 B (masked 91); the fallback through
// vector3f_assign as well 346 (1457 B, masked 49: the yaw rotation then lines up, its axis loads at levels 3/2/0 for
// i/j/k; build/r166/lead-0x296e60-axis-and-fallback-setter.patch); the brace initialiser with the candidate through
// vector3f_assign 320 (1423 B, masked 100); 1-8 dead longs on the lead 296 each. The yaw dot (retail: axis i * v.i +
// axis j * v.j + v.k * axis k) needs the axis levels i >= 3, j = 2, k = 0 with the candidate numbered as on the lead; the
// lead has 3/0/6. Redundant pairs in the shared rotation step on the lead (diagnostic; 0x297d30 128-173): around the i
// row's first product 304 (levels 3/4/2; around the k row's sine product the same levels), around the dot's first product
// 300 (3/6/4), both 301 (masked 59), around all three dot products 296 (the lead's code; 0x297d30 143). Live scratch
// stores ahead of the loop (diagnostic) 371/392. Body unchanged.
// Scaled vector published via assignment, setter, memcpy or memmove 666/337/666/648;
// early rejection with the same four transfers 666/337/666/648.
// The startup zero register, axis j preload and transfer-word reuse remain. NOT kept: all staged normalizations change the frame and increase the score.
// Nested-sum inline normalizer at axis/clear sample/best hit 292/335/335, axis-clear/
// axis-best/clear-best 338/338/340, all 340, axis through setter 311.
// NOT kept: all lengthen and regress. The axis-only form retains frame 0xa8,
// and the others grow it to 0xb0. The original zero, j product and transfer homes remain.
// M3 lane GB r4 (fast with 0x2967b0, 0x297660 (MATCH), 0x297d30 (23 unless noted), 0x24590, 0x2980d0, 0x297a50; control
// 270, al=185, mask=114; 40 trials): early-return actor_looking_normalize3d (`if (fabs(magnitude) < 0.0001f) return 0.f;`)
// on the 270 base 271 (fr=0xa8, 1430 B, al=103, mask=72), + best_direction before best_fraction 271 (al=99, mask=67), +
// collision.unknown24=NONE before vector3f_scale ray 273 (al=97, mask=61), + inline fraction 267 (mask=103, -3 not kept),
// + pitch_axis fields i,j,k 271 / k,j,i 317 / candidate fields 294 / candidate vector3f_assign 315 / proj-last rows 275
// (0x297d30 31). Lead (saved build/gb4/296e60-vassign-norm-early-best-none.cpp): early-return normalizer +
// vector3f_assign(&pitch_axis, ...) + best_direction before best_fraction + collision.unknown24=NONE before ray: 292
// (fr=0xb0, 1441 B, mask=46 vs baseline 114, 0x297660 MATCH, 0x297d30 23; no match-only forms): matches 299/345 insns in
// masked opcode/operand order (0..112 prologue/pitch-normalize j*scale/draws/pitch-dot start, 169..280 full yaw rotation/
// collision/loop tail/clear k,j,i magnitude, 283..345 best normalize and exits), leaving only 2 extra pitch-rotation spills
// ([esp+0x40]/[esp+0x54], +8 B frame because candidate.j stays live past fstp instead of dying into v.j*axis.i) and 1 clear
// i-scale operand flip. On that lead: without NONE-before-ray 292 (mask=52), without best-before-frac 295 (mask=57, same with
// inline perpendicular2d), header normalize3d 300 (mask=82), single-exit normalizer 343, fallback through vector3f_assign 350
// (without early return 343, mask=61) or fields 334 (fr=0xa8, al=121), candidate through vector3f_assign 285 (1423 B, mask=67),
// clear/axis/candidate declaration moves 292 (same code), inline fraction 293 (mask=84), rotate_in_place (cosine, sine) param
// order 292 (270 on base), axis-first crosses / cross j,k,i / proj-first rows / named versine 292 (mask=46–52), dot k,j,i 293.
// Also on base: cross3f local_up 425, cross3f in rotate_in_place 329 (312 on lead; 0x297d30 58), norm fabs>=0.0001f 328,
// dropping __forceinline on actor_looking_rotate 270 (0x297d30 468). Body unchanged.
// M3 lane GB r15 (fast with 0x2967b0 (336), 0x297660 (MATCH), 0x297d30 (23 unless noted), 0x24590 (MATCH), 0x2980d0 (MATCH),
// 0x297a50 (MATCH); control 270, al=220, mk=115; 78 trials across 5 fast batches + 3 full-link ripple checks; body unchanged):
// LEAD (saved build/gb15/lead-0x296e60-direct-sc-vassign.patch): actor_looking_rotate passing `(real)sin(angle),
// (real)cos(angle)` directly into actor_looking_rotate_in_place (dropping the two one-use trig locals) +
// `vector3f_assign(&pitch_axis, -forward->j, forward->i, 0.f)` drops 0x296e60 270 -> 239 plain (-31, 1427/1423 B, fr=0xa8,
// al=198, mk=79, 0x297660 MATCH, 0x297d30 23, all 17 file functions intact), recovering pitch-axis j*scale, pitch-rotation
// dot/cross order, and byte-identical clear-path and best-direction normalize tails; NOT kept because adding the first call
// to `vector3f_assign` in this TU shifts whole-program /GL symbol numbering and triggers 2-pt tie flips in unrelated matched
// 0x0c2d00 (unknown_0bffa0.cpp +0x12e) and 0x185be0 (unknown_185be0.cpp +0x21c) (confirmed on vector3f_assign alone, and
// persisted with inline/__forceinline/dot3f variants; direct `(sin, cos)` alone 269 keeps both matched). On the 239 lead:
// best_direction declared before best_fraction 236 plain (al=195, mk=74; + NONE-before-ray or ray-scale return passed to
// 0x1697c0 236, mk=68), early-return actor_looking_normalize3d 244 plain (1428 B, fr=0xa8, al=157, mk=33; + best_direction
// first 244, mk=28; + NONE-before-ray 244, mk=22: 325/347 insns match masked incl. full yaw rotation, shifts result/+4 B),
// single-exit normalizer or header normalize3d 265 (1433 B), dot3f in rotate_in_place 249 (0x297d30 144), cross j/k/i,
// proj-first rows, (1-cos)-first, clear after pitch_axis, dot3f/length_sq3f normalizer, one/both __forceinline, best_fraction
// after axis, `!test_collision` continue 236 each (same code), manual `*=` normalizer 293 (fr=0xb0), fallback/candidate
// vector3f_assign 342/290 (fr=0xb0), candidate fields 295, pitch_axis fields 266, best before result 266, axis first 261,
// inline fraction 265 (252 with early return). Other trials: direct `(cos, sin)` on base/B3_02/B3_03/oldn-B3_03
// 273/275/256/251 (fcos first, 0x297d30 41); merged `sine, dot3f, cosine` rotate on base/B3_02/oldn-B3_02/B3_03/oldn-B3_03
// 273/274/270/290/292 (0x297d30 23 -> 6); merged inline-cross/staged/jki/out-vec 361/317/329/322/290/438 (0x297d30 129-203);
// in-place staged/out-vec 329/304/434 (0x297d30 153-203, 0x297660 1); `0.f - forward->j` on B3_02 273 (mk=62); member-wise
// ray `* 3.f` / `3.f *` on B3_03 303 (fr=0xa8, 1451 B); header normalize_inline 340 (fr=0xb0).
// M3 lane 3 r311 (fast screen with 0x297d30 (23), 0x297660 (MATCH), 0x2967b0, 0x24590, 0x2980d0 and 0x297a50; control
// 270). Rank reading: the normalizer's three products sort the reciprocal (a temp) above the pitch axis's i and j, which
// are member symbols of the brace-initialised local; retail's `j * scale` needs j ranked above the reciprocal, which only a
// load at an offset of a memory object gives (the setter form re-measured 296: normalizer products and registers' roles
// then as retail, zero still in XMM1, frame 0xb0); 1-7 unused locals at the top move nothing (270 each, diagnostic).
// Forms: the attempt counter as long 269 (1430 B; retail compares it as a word, so short is right); the collision step as
// `if (!test_collision) continue;` 270 (same code); the success exit copying the candidate out and normalizing the output
// in place 288 (1426 B); the same at the trailing exit 348 (1400 B, 0x297660 4); at both 376 (1388 B, 0x297660 40).
// M3 lane 12 r193 (H2M_FAST with 0x297660, 0x297a50, 0x2980d0 (MATCH) and 0x297d30 (23); control 270): lane GB r15's lead
// (the rotation helper passing the sine and cosine calls straight in, and the pitch axis through the existing
// vector3f_assign) re-measured 239 at 1427 B; its full check now loses matched 0x1c4590 (operand order of one add) and
// 0x24cf66 (two tied loads swapped), while 0xc2d00 and 0x185be0 stay matched: not kept, held as a patch. The setter alone
// 296 (1440 B); the sine/cosine change alone 269 (1430 B); the lead with the yaw axis copied into a local 431 (1506 B);
// the lead with the pitch axis's zero test written `!normalize(...)` 249 (1433 B). Body unchanged.
// M3 lane 2 r157 (h2m fast with 0x297660, 0x297a50, 0x2980d0 (MATCH), 0x297d30 (23) and 0x24590; control 270). KEPT 270 ->
// 239 (check.py --fast and the full check; nothing lost, 0x1c4590, 0x24cf66, 0x185be0 and 0xc2d00 all still match): lane
// GB r15's held form, the rotation helper passing the sine and cosine calls straight in (its two one-use locals dropped)
// and the pitch axis set through the existing vector3f_assign; the sine/cosine change alone 269 (1430 B). On 239: the
// rotation helper reading the vector's three components into locals first 306 (1450 B; 0x297d30 243); the rotation's
// dot product named ahead of the versine product 295 (1439 B); the fallback test on `!result` in place of `!clear` 344
// (1423 B, but 0x297660 10); early returns in place of the result flag 341 (0x297660 13); `clear` initialised true at
// its declaration as well 239 (same code).
// M3 lane 2 r158: the r157 form (sine and cosine passed straight in, the pitch axis through vector3f_assign), 239, lost
// 0x1c4590 (5) and 0x24cf66 (2) in the full check of the tree merged with main (as lane 12 r193 had seen); restored to
// main's 270 form, held as a patch.
// M3 lane 6 r253 (reduced-image screen with 0x297660, 0x297a50, 0x2980d0 and 0x24590 MATCH, 0x297d30 23; control 270;
// whole-image checks listing 0x1c4590, 0x24cf66, 0xc2d00, 0x185be0 and 0x41640). The r157 form re-measured on today's
// tree: 239, still losing 0x1c4590 (5) and 0x24cf66 (2). 270 -> 239, HELD at review: the same form with the pitch axis set through
// the shared point setter, `real_point3d_set((point3f *)&pitch_axis, ...)` (the spelling src/unknown_1f03e0.cpp and
// src/unknown_205be0.cpp already use for vector storage): the same code for this function (1427 B, the normalizer's
// j * scale and the pitch rotation's operand order as retail), and 0x1c4590, 0x24cf66, 0xc2d00, 0x185be0 and 0x41640
// all stay matched, since that setter is not a new per-file inline. Held because the typed vector setter gives the same
// code and differs only in the count of per-file inline instances, so the point setter on a vector (through a cast) was
// chosen for the link layout, not for the code; main's body is back. On the held form: the setter alone (trig locals
// kept) 296 (1440 B); the fallback axis through the setter as well 345 (1456 B); the candidate through the setter 290
// (1423 B, frame 0xb0); the collision index stored ahead of the ray scale 239 (same code). Rank reading on the held
// form: the pitch axis i and j live in registers from the normalizer into the loop (442 and 458) and are coloured
// after ten of the loop's own temporaries (476 to 1196), so j finds only XMM6 and XMM7 free; retail has j in XMM1.
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
			vector3f ray;
			vector3f_scale(&candidate, 3.f, &ray);
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

// score 72 (was 72): retail stores false into the returned flag at the two trailing case exits (cases 1 and 2,
// sharing the normalise-failure tail), its two flag homes are the other way round, and its weapon block keeps the
// direction in EDI and reloads the actor index; the forms tried follow.
// MiniMax G (11-18), at 175 (was 257): the player test of cases 1 and 6 is written with the == NONE arm
// first, which restores retail's case-1 block layout; the body is 1424 B against retail's 1444. Left: the two
// flags' byte homes are still swapped against retail's (the returned flag at +0x0a, the presence byte at +0x13),
// the two 0xcafc0 calls still sit side by side where retail keeps them apart, and the jump table sits 20 B early.
// Earlier 72 (was 257): every point-producing path now ends at aim_at_point, whose `if (aiming)` weapon block falls
// into the direction-from-actor code (round 264, below), which gives retail's block layout and its two separate
// 0xcafc0 blocks. Left: the result/point-valid byte homes are swapped, cases 1 and 2's trailing failure exits take the
// plain exit where retail stores result = false first, and the weapon block keeps the direction pointer in EBX where
// retail uses EDI and reloads the actor index for 0x1ffbd0.
// Earlier at 257: retail keeps case 1's and case 2's identical 0xcafc0 blocks inline where this build
// tail-merges them (that moves case 2's block and the direction chain's fall-through predecessor); retail's failure
// exit stores result = false (F+2, point-valid at F+3); the weapon block keeps the direction pointer in EBX where
// retail uses EDI and reloads the actor index. Round 209 (m3 lane 4) at the end of this note.
// MiniMax R (11-15), not kept (the merge measured 341 -> 340 fast, under the bar): reported 175 (was 257): case 1's and case 6's player test inverted, so the NONE arm comes
// first and retail's case-1 block layout returns (the body is 1424 B against retail's 1444). Left: retail keeps
// the non-NONE arm inline in both cases where this build keeps the NONE arm inline, this build's case 1 and
// case 6 copies of the 0xcafc0 call sit side by side where retail keeps them apart, the result/presence byte
// homes are still swapped (retail keeps the returned flag at F+2 and the presence byte at F+3), and the 0x1ffbd0
// call still holds the actor index in EDI where retail reloads it from the stack.
// MiniMax A (11-14, second pass): score 168 (was 177): the two flags are cleared in the order retail uses (the
// presence byte first), and the two trailing case exits clear both flags before leaving, which gives retail's
// byte homes (the returned flag at +0xa, the presence byte at +0x13). Left: retail keeps case 1's and case 2's
// byte-identical 0xcafc0 blocks apart where this build tail-merges them into one, so the body is 16 bytes short,
// and the 0x1e3b00 call of the direction chain still loads the object before its pushes and copies the output
// address, where retail loads the object after the third push and recomputes the address.
// MiniMax A (11-14): score 177 (was 257): case 1's player test inverted, so the NONE arm comes first, restores
// retail's case-1 block layout; the frame is 0x8c as retail's, the byte homes still differ (retail's returned
// flag at +0xa and its presence byte at +0x13, ours the other way round) and the two 0xcafc0 blocks are still
// merged into one. Forms tried and rejected this round: declaring the two bools in the other order and
// initialising them in the other order both leave the byte homes as they were (the compiler picks the homes
// itself), and the polarity retail uses (the non-NONE arm first, which keeps the two 0xcafc0 blocks apart)
// scores 257 at 1448 bytes, so the merged form stays.
// MiniMax G (11-13): no new form, no lower score (257): the diff re-read this round puts the first difference at the 0x1ffe00 call, which retail makes with both arguments on the stack where this build passes the first in a register; that callee is defined in src/unknown_1ff010.cpp, a file this lane may not edit, so the body stands.
// MiniMax H (11-13): no new form, no lower score (257).
// MiniMax E (11-16): 166 (was 257): the player test of cases 1 and 6 is written with retail's polarity (the
// == NONE test first, its je branch calling 0xcaf60, as retail's bytes show at 0x296b25 and 0x296c39), which
// keeps the two 0xcafc0 blocks apart as in retail; the two flags are then cleared in retail's order (the
// returned flag first, `point_valid = result = false`, so the prologue stores F+2 before F+3) and the two
// trailing case exits clear the returned flag before leaving, which gives retail's byte homes (the returned
// flag at F+2, the point-valid byte at F+3) and brings the body to 1428 bytes against retail's 1444. Left:
// retail keeps case 1's and case 2's byte-identical 0xcafc0 calls apart where this build tail-merges them
// into one, so the jump table sits earlier and every branch target differs. The forms this round that were
// not kept: case 6's player test with the other polarity (177), the point-valid store moved to the aim label
// (367), the shared point-valid store for cases 3/5/6 with cases 1 and 2 skipping it (366, and it cost the
// matched callers 0x297660, 0x297a50 and 0x2980d0 their match).
// MiniMax C (11-16): no new form, no lower score (257 plain, 341 on the reduced image): the frame agrees (0x8c) and
// what is left is the byte homes of the result and the point-valid flag plus the direction chain's fall-through
// predecessor, where retail enters case 2's remembered 0x1e3b00 block and ours falls into the merged 0xcafc0 block;
// the reduced image scores higher because the helper it calls is laid out differently in that build.
// Earlier at 257: frame 0x8c agrees; left: the result/point-valid byte homes and the direction chain's
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
// m3 lane 5 r181 (plain checks; 0x297a50, 0x297660 and 0x2980d0 MATCH in every form; base 257, aligned 68, masked 55).
// Lead (match-only, not kept): case 1's player test inverted (`if (object->player_index == NONE) caf60; else cafc0;`)
// 177 at 1424 B (aligned 27, masked 11): the direction chain then follows case 2's remembered 0x1e3b00 block and case 2
// keeps its own 0xcafc0 block as in retail, but case 1's 0xcafc0 block cross-jumps into case 2's (retail keeps it as case
// 1's fall-through, `je` to caf60); on it `result = false;` before the two trailing case `goto done`s 170 (aligned 18: the
// result/point_valid homes are then retail's), with `point_valid = result = false` 168 (1428 B, aligned 17, masked 11;
// diff in build/r181/lead-2967b0.patch of that round). Left on 168: case 1's merged 0xcafc0 block and the weapon block
// (retail loads the direction into EDI and reloads the actor index from its home for 0x1ffbd0; ours uses EBX and keeps
// the actor index in EDI). On 177/168: case 1's object arm ending in `goto aim_at_point`, its inner if/else as two ifs
// with their own gotos, case 2's player arm as its own block, case 2's player test inverted: all the same code; case 6's
// player test inverted 175; case 1's or case 2's or both object arms with their own vector3d_from_points3d and `goto
// normalize_direction` 168 (same code); the weapon calls through a forceinline helper taking a copy of the actor index
// (diagnostic) 168 (same code). Without the inversion (case 1 original): the result stores with `point_valid = result =
// false` 254; on that, case 1's 0x1caa40 path first, the own-direction arms and case 1's locals renamed 254 each (all the
// same code): only that test's polarity moves the layout. Case 6's player test inverted alone 257. Retail's two 0xcafc0 blocks are
// byte-identical (8d 44 24 14 8b cb e8), so the merge is decided by the block layout, not by the bytes.
// Later on the 168 form: case 6's player test inverted 166 (aligned 22: case 6's 0xcafc0 block moves out of line, a positional
// drop); the weapon block as `if (weapon_index == NONE) goto direction_from_actor;` 234 (1444 B, the chain moves back to the
// end); case 1 written before case 2 333; case 1 as a `state < 1` exit, then its remembered branch, then the object arm 194;
// the flags cleared in two statements 170; case 1's remembered test with its operands swapped 172. Same code as 168: case 1's
// 0x1caa40 path first, the aim test as `if (aiming) goto local_254539;`, a `failure:` label for the two result stores, the
// origin declared in the weapon block, case 2's remembered branch first, case 1's object test as nested ifs.
// P467: Missing-point true default, false at each original point producer,
// bool/byte/char/short/word/int/long/dword: 343/343/343/382/382/382/382/382
// (fast control 341; full 257). NOT kept: no equality or reduced gain; original decoder restored.
// P501: Standard whole-point transfer at five producers, state pair, all producers and optional publication:
// 346/341/306/345/341/379/332/338. NOT kept: Reduced control 341 differs from authoritative 257. Best plausible
// single-producer t03 scores 306 reduced but 299 plain at 1404/1444, so it regresses and shortens away from
// retail; every form restored. Original complete executable body restored.
// MiniMax A (11-14), NOT kept: 335 (was 341), a 6-point gain, under the 8-point bar for a function above 30.
// Round 105 (m3 lane 1, fast with 0x297a50/0x297660/0x2980d0 and every callee, control 258 = full 257): the remembered
// branch of cases 1 and 2 as one shared forced-inline helper (0x210850, then 0x1e3b00 unless aiming) 258 (same code: ours
// still loads the object before the pushes and copies &point into EDI); that helper returning false when aiming 258 (same
// code); point_valid set once at a label ahead of aim_at_point for cases 3/5/6 368 (the three callers drop to 4/1/2, as
// round 85 found); case 6's NONE compare with its operands swapped 258; case 2's body nested under a positive
// `field_x50962e != NONE` test 258 (all same code but the label form).
// MiniMax R (11-15), NOT kept: 340 (was 341), a 1-point gain, under the 8-point bar for a function above 30.
// MiniMax D (11-15): no new form, no lower score (257): the switch's branch into the table is laid out at a different offset from retail's and the table itself sits four bytes further along in our text.
// MiniMax E (11-16), NOT kept: 335 (was 341), a 6-point gain, under the 8-point bar for a function above 30.
// MiniMax D (11-16), NOT kept: 340 (was 341), a 1-point gain, under the 8-point bar for a function above 30.
// P590: Original six incoming parameter fixed bindings, complete bindings and
// parameter role identities: eight 258 (fast control 258; full 257). NOT kept:
// no exact result/presence homes or native case-two tail; original body restored.
// MiniMax S (11-16): no new form, no lower score (257): the instructions match one for one and only the branch
// targets differ, because this build lays the case blocks out in a far cold section where retail keeps them next
// to the function.
// M3 lane 4 r209 (fast with 0x297660/0x297a50/0x2980d0 and the callees 0x1ffe00/0x1e1f20; without the two callees the
// fast build reads 341 at 1404 bytes, with them 258 at 1448 as the full build): MiniMax E's three forms together (flags
// cleared `point_valid = result = false`, `result = false; goto done;` at the two trailing case exits, NONE-first player
// tests in cases 1 and 6) 166 (1428 bytes, aligned 28); with case 6's player test as on main 168 (aligned 19: only case
// 1's merged 0xcafc0 block and the weapon block's EBX/EDI left). On that 168 form: case 1 reading
// `reference->object_index` at each use 168, the weapon locals scoped in the weapon block 168, case 1 with its own prop
// pointer 168 (all same code: the merge is not operand identity), `point_valid = true;` ahead of case 1's
// `goto aim_at_point` (case 6's tail) 164 (aligned 25), the case 1/2 failure tails as `return false;` 188 (its own
// xor-al exit). Case 1's object arm in case 6's shape (all three calls then `goto aim_at_point`) on main's player test
// 255, on the 166 form 166 (same code); cases 1 and 6 through a shared object-point inline helper: plain inline 392
// (1300 bytes, kept out of line), forceinline 255, forceinline with the NONE-first test 166. None kept: each gain rests
// on forms kept only with a match.
// Shared recalled projection 311; shared direct-point cases 397; typed state point reads 258.
// Common prop lookup 416; explicit reference first 416; looking arm first 424;
// with shared direct-point cases 408; with typed state point reads 416 (reduced control 258).
// The common lookup forms shorten the body and change the specification argument's register.
// Round 415. case 2 state-admitted presence 278, case 1 state-admitted presence 265, both state-admitted 278, case
// 2 point-write presence 375, case 1 point-write presence 102, both point-written 390, case 1 point-written with
// case 2 state-admitted 213, case 1 state-admitted with case 2 point-written 348, case 1 reference-state snapshot
// 263, both reference-state snapshots 263. NOT kept: the two lower previews publish presence after the point calls
// instead of at the state test. The 102 form also falls short of retail length.
// Explicit live-state selection in cases 2, 1 and both 303/249/310.
// Early case-one lower bound 263. Unit-kind range in cases 2, 1, 6 and all
// 293/268/395/376. NOT kept: the 249 form reaches retail length but replaces
// the native signed range branches with equality branches and leaves the
// player-point blocks merged. Original complete body restored.
// Case 2 state local 258, case 1 state local 258, separate case states 258, separate
// case references 258, case 2 reference and state local 258, case 1 reference and state
// local 258, both case-local pairs 258, both pairs with read-only state 258.
// NOT kept: all tie the reduced control at 258. The player-point blocks still merge.
// MiniMax G (11-18), NOT kept: 340 (was 341), a 1-point gain, under the 8-point bar for a function above 30.
// Round 264 (m3 lane 3; fast with 0x297660, 0x297a50, 0x2980d0 and the callees 0x1ffe00, 0x1e1f20, which reads the
// full build's count; control 258): the weapon block moved after the return (label order only) 258 (same code); the
// remembered branches as `if (!aiming) 0x1e3b00(...); goto aim_at_point;` (no weapon label) 258 (same code); on it
// case 2's non-aiming object arm `goto aim_at_point`, case 1's object arm as if/else ending `goto aim_at_point` (case
// 6's shape) and the aim test as `if (aiming) { weapon block }` falling into the direction code 73 fast, 72 plain
// (1436 bytes; kept: it removes the direction and weapon labels and six gotos). On 73: `point_valid = result = false`
// 75; `result = false;` before cases 1 and 2's trailing `goto done` 38 (1440 bytes); both 36 fast, 33 in the full
// image (aligned 9: only the weapon block's EBX/EDI is left). On 36, same code each: the weapon index and origin
// scoped in the weapon block; the 0x1ffbd0 result assigned inside its test; `aiming && (weapon_index = ...) != NONE`;
// the fallback as `if (result) goto output_point;`; the fallback's test after its block; the actor index for 0x1ffbd0
// read once through its address; a local copy of the actor index for 0x1e1f20/0x1fc710; those two calls through a
// forceinline helper returning the weapon index. The permuter cannot search this function (its reduced image reads
// 310 on the 36 form).
// Separate remembered input in cases 2/1/both: 309/308/343. Case-local in-place
// remembered point in cases 2/1/both: 338/337/399. Case-local player point in
// cases 2/1: 313/203 (preview control 258). NOT kept: the 203 form adds a one-use
// point copy, enlarges the frame, and grows to 1464 bytes against retail's 1444.
// The adjacent Boolean homes and shared direction tail still differ.
// M3 lane 16 r142 (fast with 0x297660/0x297a50/0x2980d0 and the callees 0x1ffe00/0x1e1f20, all MATCH; control 73, full 72):
// diagnostic, round 264's redundant `result = false;` before the two trailing case `goto done`s re-measured 38 (1440 B: the
// flag homes become retail's, only the init store order and the weapon block's EBX/EDI are left); the remembered-point test
// held in `result` (`result = state->unknown00 != NONE; if (result)`, case 1 with its `state >= 1 &&`) 259 (1456 B); every
// `goto done` as `return result;` with no label 383 (1428 B); only the two trailing case exits as `return result;` 166 (1448
// B); the two flags initialised at their declarations 75. In retail's weapon block the actor index rides in EDI for 0x1e1f20
// and 0x1fc710 only, then EDI takes the direction and the actor index is reloaded from its home between the pushes for
// 0x1ffbd0 (EBX unused there); ours keeps the actor index in EDI and gives the direction EBX.
// Direct failure and success returns 394; common switch exit 73; shared weapon normalization 107;
// positive normalize branch 91; named normalized length 73; shared weapon normalization with separate
// flag defaults 109; nested point-publication guards 77; common switch exit and combined normalize guards 73.
// Reduced control 73; the swapped Boolean homes and aiming registers persist.
// M3 lane 10 r131 (fast with 0x296e60, 0x297660, 0x297a50, 0x297d30, 0x2980d0, 0x298370 and the callees 0x1ffe00 and
// 0x1e1f20, the three callers MATCH in every form; control 73). Round 264's lead rebuilt 36 (1440 B). Reading of its
// leftover: retail loads the actor index into EDI for the 0x1e1f20 and 0x1fc710 calls only, gives EDI to the direction
// right after 0x1fc710 and reads the actor index for 0x1ffbd0 from its stack home (`mov eax, [esp + 0xbc]`, hoisted
// between the pushes to where EAX is free); ours serves all three calls from EDI and gives the direction EBX. Retail's
// other two 0x1ffbd0 calls (in 0x1fc7e0) set the register argument up last, so the parameter order is right.
// Diagnostic, reverted at once (a forbidden form): on the lead, a volatile read of the parameter at the 0x1ffbd0 call is
// a byte match in the fast and in the plain check, so the third read kept apart from the first two is all that is left.
// Forms on the lead that compile to the lead's code (36): the first two calls through a forceinline helper with an
// unsigned parameter, or one taking `long const *`; 0x1ffbd0 through a forceinline wrapper taking the index by value, or
// taking a one-member aggregate built from it or cast from the parameter's address; the first two calls, or all three,
// through `long const &` or `long const *` bound to the parameter; one read through `long const *actor_reference =
// &actor_index` at the actor lookup, at 0x1e1f20, at 0x1fc710 or at 0x1ffbd0 (forwarded each time); the third argument
// as `*(long const *)(void const *)&actor_index` or `*(dword const *)&actor_index`; a local copy of the direction for
// the weapon block; the fallback's difference written component by component; `if (weapon_index != NONE)` repeated
// around the 0x1ffbd0 call; that call inside `do { } while (false)` or a one-pass barrel loop. Other forms on the lead:
// the flag argument as `!aiming` 69 (1452 B); the barrel as `aiming ? 0 : 1` 66 (1452 B); the third argument as
// `aiming ? actor_index : NONE` 84 (1456 B); the third argument rebuilt from its two halves 397 (1468 B). Diagnostics on
// the lead, reverted: the third use removed 30 (the direction then takes EDI, so the allocator reuses EDI once the copy
// is dead); `if (aiming)` repeated around the 0x1ffbd0 call 65 (a real block boundary does not end the copy); the
// parameter's address passed to a call on the default path 427 (1500 B: every use then reads the home, the first two
// and case 0's as well, so retail's parameter is not address-escaped); one more direction use in the fallback 48 or
// after the aim call 58 (the direction stays in EBX: not a use-count tie).
// MiniMax A (11-19): score 36 (was 72): retail has two exit blocks, the plain one at 0x296aaa that returns the
// result flag and the failure one at 0x296d22 that stores 0 to the result flag before returning it, and the
// failure block is reached from the two remembered-branch failures (0x2969e0 and 0x296ae3) and from the two
// normalize failures (0x296a6a and 0x296d16), while this source has one `done` label, so the failure paths
// never stored the flag and the two Boolean homes came out the wrong way round (this build's result flag
// landed above the presence byte). Storing the flag on the four failure exits as retail does - `result =
// false;` before the trailing `goto done` of cases 1 and 2 and before the two normalize failures - gives
// retail's homes, and `point_valid = result = false` at the entry gives retail's store order (the result
// flag first). Round 264's diagnostic measured the same idea at 38 and called it a diagnostic; it is retail's
// own failure block, so it is kept here. The matched neighbours 0x297a50, 0x297660 and 0x2980d0 stay matched
// and every other function of the queue keeps its score. What is left is one instruction: the body matches
// retail's instruction for instruction up to the weapon block, where retail loads the direction into EDI and
// reloads the actor index from its home (`mov eax, dword ptr [esp + 0xbc]`, 7 bytes) for the 0x1ffbd0
// argument, while this build keeps the actor index in EDI (`mov eax, edi`, 2 bytes) and gives the direction
// EBX; that five-byte difference makes the body 1440 bytes against retail's 1444, so the jump table sits four
// bytes earlier and the branches to the failure block name a different address. The forms tried for it (the
// weapon locals scoped in the block, the 0x1ffbd0 result assigned inside its test, `aiming && (weapon_index =
// ...) != NONE`, the fallback as `if (result) goto output_point;`, the actor index read through its address, a
// local copy of the actor index, the two calls through a forceinline helper, a local for the direction pointer)
// all compile to the same code, so this is the register allocator's choice and not the source's. A full-image
// permuter run on this body was stopped at its time limit without scoring a variant (baseline 33 differing
// instructions in the full image; the reduced image cannot score this function at all, as it builds it 36
// bytes shorter there). The two 0xcafc0 blocks now match again.
// MiniMax A (11-19), NOT kept: 340 (was 341), a 1-point gain, under the 8-point bar for a function above 30.
// M3 lane 10 r131 (coordinator, 11-20): MATCH. Retail's failure-block stores (`result = false;` before the trailing
// `goto done` of cases 2 and 1, `point_valid = result = false` at entry) plus one read of the actor_index parameter
// through its address for the 0x1ffbd0 argument, which retail reloads from its home there.
// @retail 0x2967b0
PRIVATE bool function_2967b0(long actor_index, s_type_952051 *specification,
	bool aiming, vector3f *direction, bool *has_target_point, point3f *arg_0e6828)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	bool result;
	bool point_valid;
	point_valid = result = false;
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
			goto aim_at_point;
		}
		else if (state->unknown00 != NONE)
		{
			function_210850((s_type_c3b527 const *)&state->unknown48, &point);
			if (!aiming)
				function_1e3b00(reference->object_index, 1, &point, NULL, NULL, &point);
			goto aim_at_point;
		}
		result = false;
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
			}
			else
				function_1caa40(object_index, &point);
			goto aim_at_point;
		}
		else if (reference->state >= 1 && state->unknown00 != NONE)
		{
			function_210850((s_type_c3b527 const *)&state->unknown48, &point);
			if (!aiming)
				function_1e3b00(reference->object_index, 1, &point, NULL, NULL, &point);
			goto aim_at_point;
		}
		result = false;
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
	if (aiming)
	{
		weapon_index = function_1e1f20(actor_index);
		if (weapon_index != NONE)
		{
			function_1fc710(actor_index, &origin);
			// One volatile read of the parameter: retail reloads actor_index from its stack home for this argument.
			result = function_1ffbd0(*(long const volatile *)&actor_index, weapon_index, 0, &origin, &point, false, direction,
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
	}
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

// @retail 0x296580
void function_296580(long actor_index)
{
	s_actor_looking_view *actor = actor_looking_get(actor_index);
	if (actor->aiming_direction_type == 0 && !actor_looking_get(actor_index)->path_active)
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
// M3 lane 13 r93 (fast; control 23): dead-local phase scans (`long dead_k = k;`, n = 0..7, removed): at the function top, in
// the loop body and ahead of the loop 23/33/23/23/23/23/23/23 each (only n = 1 moves the selected-direction dot, never the
// rotation or score rows); in the rotate helper 23 at every n (0x296e60 303 -> 293 at 1437 B for n >= 1); so the operand
// order here is not a symbol-count phase. The round-91 lead rebuilt (helper dot `(j + i) + k`, score `(i + k) + j`) 13, with
// 0x296e60 303 -> 311 (1460 B), so the shared helper change is not keepable; left at 13: the dot's accumulator (XMM2 for
// XMM0), the j row's XMM5/XMM6 and the score's inner pair k, i (retail i, k); scans on it 13 (top n = 1 23). The trace's
// first byte as the named bool of the sector-trace record needs a cast (the real signature takes the path record).
// Round 201 (m3 lane 4, fast with 0x2980d0, 0x296e60, 0x297660, 0x26c590 and the trace/normalise callees): a named
// pointer to the actor's facing for both copies 144 (928 B); the selected direction by index at its three uses 39; the
// structure/pathfinding lookup ahead of the actor 224; the normalised length named before its test 144 (928 B);
// re-measured the up-axis pointer local 70 and the candidate pointer 139; permuter (600 s, seed 201, 62 tries) none
// below 23. Retail's dot starts `movss xmm0, [axis + 4]` hoisted above the candidate copy; ours starts v.k * axis.k.
// Round 203 (m3 lane 4, fast = full 23): a forceinline helper holding 1-7 dead locals before the loop, at the loop top or
// between the copy and the rotation (21 builds) 23 each, identical code; `directions + i` at the rotation and the trace 23
// (same code); the angle named between the copy and the rotation 33; the copy inside a forceinline helper taking (source,
// axis, angle, out) 60. Diagnostic: a file-local copy of the rotate pair with the dot `((j + i) + k)`, used only here, plus
// the score `(i + k) + j` in explicit parentheses 13 with 0x296e60 unchanged (299), so lane 11's 13 needs no change to the
// shared helper; the copy alone 31. Left at 13: the dot's inner pair takes XMM2 for the j product (retail XMM0), the j row's
// XMM5/XMM6, and the score's inner pair k, i (retail i, k; parentheses stop reassociation but not the operand swap). On 13
// own dead locals n = 1..7 23/13/13/13/13/13/13; dead helper locals 1-7 between the copy and the rotation, or ahead of
// the score, 13 each. Not kept (a one-caller twin of the shared helper).
// M3 lane 14 r101 (fast with 0x296e60/0x2980d0/0x297660; control 23): a separate trace record for the loop's trace 144 (928 B);
// the direction loop as `while (i < 8)` with `i++` last 23 (same code); the best-index test and the selected dot as one `&&`
// test 144 (928 B); the counter declared beside the best pair 23 (same code); the clamp as `if (0.6f > score) score = 0.6f;
// score *= fraction;` 141 (927 B).
// Round 91 (m3 lane 2, fast; control 23): the clamp product written fraction-first 23; best_index stored before
// best_score 25; `real score;` declared at the loop-body top and assigned at the sum 23; the fraction as
// `0.2f * trace.distance` 23; a const actor pointer 23 (the 23s the same code). Restored.
// M3 lane 2 r95 (fast with 0x297660/0x2980d0, both MATCH; control 23): the loop score accumulated i, k, j through a
// `real &sum` bound to it 65; the helper's projection accumulated j, i, k through a bound reference, then `*= 1 - cos`,
// 153 (928 B; 0x296e60 308 fast); the helper's rows accumulated axis term, cosine term, cross term (0x23d260's row
// spelling) 140 (928 B; 0x296e60 293 fast, where fast reads 391 for its 299); sine and cosine of the angle passed
// straight to the in-place helper 147 (930 B). Same code (23): the helper's dot written j, i, k with the axis j first;
// `fraction` declared beside the best pair and set at the loop top. Restored.
// M3 lane 15 r100 (fast with 0x296e60/0x2980d0/0x297660; control 23, 0x296e60 270): the wrapper calling include/
// axis_rotation.h's shared in-place rotation (the vector's k read into a local) 245 (893 B; 0x296e60 339); the in-place
// helper taking the vector and the axis by reference 23 (same code); the axis by value 210 (0x297660 fast 4); the helper
// returning the vector pointer 23 (same code); each cross term scaled by the sine where it is computed 153 (925 B), which
// lowers 0x296e60 to 199 fast (a lead for that function; not kept, it raises this one).
// M3 lane 5 r203 (fast with 0x296e60/0x2980d0/0x297660; control 23, 0x296e60 270): the shared helper's dot grouped
// `(j + i) + k` with the score `(i + k) + j` re-measured 13, but 0x296e60 270 -> 339 (1442 B): not kept; forward built by
// the aggregate initializer `{ forward.i, forward.j, 0 }` 199 (927 B); the direction array declared at the function top
// 23; the counter declared ahead of its loop 23; the angle as `0.785398185f * i` 23; forward declared, then assigned 23
// (the 23s the same code).
// M3 lane 12 r143 (fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x2967b0; control 23): round 91's lead re-measured
// on today's 0x296e60 (helper dot `((j + i) + k)`, score `(i + k) + j`) 13 with 0x296e60 270 -> 339; the score alone
// parenthesized `(i + k) + j` 124 (928 B); the score accumulated i, k, j in three statements 143 (928 B); the helper's
// projection staged (`projection = dot; projection *= 1 - cos;`) 141 (927 B; 0x296e60 282); the helper's product written
// `(1 - cos) * dot` 23 (same code); the fraction as a ternary after the trace 144 (re-measure of round 27's form).
// M3 lane 11 r126 (fast, this function alone; control 23): redundant parens (no grouping change) move the inlined
// rotation's operand orders, since its nodes follow every node of this body: one pair around the angle argument or the
// first score product gives the dot retail's order and orientation (axis j first, j, i, k) but moves its accumulator,
// the cross and row orientations and the 0x26c590 pushes (132/128, 928 B); 1/2/3 nested pairs on the angle 132/150/89, on
// the score products 128/144/62, on the clamp product 60/144/141; around plain leaves no effect. The 4 x 4 x 4 grid of
// angle/score/clamp pair counts: best 21 (two score products and the clamp twice; the score's middle term then retail's),
// then 23, 29, 31, 56, 60, 62. Under the four clamp shifts: the vector argument as `directions + i` the same code as
// `&directions[i]` (23/60/144/141); the up axis through a pointer local 70/146/146/135. The angle with the conversion
// implicit 23 (same code); the score as (i + k) then += j, or (k + i) then += j, 124/124 (928 B).
// M3 lane 12 r152 (fast with 0x296e60 270, 0x2980d0, 0x297660; control 23): the score as two statements on its local, `(j + k)`
// then `+= i` 142 (928 B), `(i + j)` then `+= k` 128 (928 B), `(i + k)` then `+= j` 124 (928 B, lane 11 r126's two-stage form);
// three statements i, k, j 143 (928 B). Redundant pairs shift every float product by their count mod 4 wherever they sit
// (around the clamp product, the fraction product or a helper product alike; around the parenthesized ternary they count
// nothing): `(i + k)` then `+= j` at shifts 0-3 124/143/59/9. LEAD at 9 (924 B, steering parens, not kept): the dot and
// the score are retail's; left are three rotation products: cross k's first loads v.i where retail loads axis j, cross i's
// second loads axis j where retail loads v.k, and the j row accumulates axis j * projection where retail accumulates
// v.j * cosine. Other cycles at shifts 0-3: `(j + k) += i` 142/143/58/46; `(i + j) += k` 128/130/62/31; parens inside
// the score's products, best 23/31/33; the axis through a const pointer local 169/67/168/144; a named `direction`
// pointer for the copy, rotation and trace 143/68/19/128 (with the score through it 145/68/40/145); the helper's cross i
// second product parenthesized (odd counts, 0x296e60 270 -> 282) best 35, with the score's k product too best 22.
// Natural spellings on the `(i + k)` then `+= j` form (clamp compare reversed, fraction-first clamp product, `0.2f *
// distance`, constant-first angle, fraction declared before the copy, `best_score < score`) 124 each (same code).
// M3 lane 5 r213 (fast with 0x296e60 at 270, 0x2980d0 and 0x297660 matched throughout; control 23): the pathfinding lookup
// through the existing out-of-line getter 0x1fa7f0 (as matched 0x2551f0 calls it) 82; the angle constant as a constant
// expression (pi / 4, 2 pi / 8, pi * 0.25, (2 pi) / 8) 23 each (same code, folded first); the in-place helper spelled as
// matched 0x1055b0's (`(1 - cos) * dot3f(vector, axis)`, rows axis-first) 144 (925 B; 0x296e60 282); as matched
// 0x177d10's (axis k named, cross terms j, k, i, rows axis-first) 99 (0x296e60 362). Dead initialised longs at the top,
// K = 8..40: states 23/33/39/42/48/74 only. Form x K grid (K = 0..7): the score through dot3f in either argument order
// 35/32/48/35/45/35/35/35; the 0x1055b0 helper 144 at every K, with the dot3f score 142-147; the 0x177d10 helper 99-108.
// M3 lane 3 r277 (fast with 0x24590 listed: without it the reduced image drops magnitude3d's expansion and 0x296e60 reads
// 412 for its 270; plain agreed for the lead): LEAD 6 at 924 B, not kept: the two rotate helpers merged into one taking the
// angle, with `real dot = dot3f(vector, axis);` ahead of the sine and cosine and `projection = dot * (1 - cos)` (first
// difference +0x20c: the rotation dot and cross k are retail's; left are cross i's second product, axis j loaded first
// where retail loads v.k, and the score's i, j, k for retail's i, k, j). It is a statement order picked by score and it
// raises 0x296e60 270 -> 282 (1437 B; every other function of the file unchanged), so match-only. The merged helper with
// sine and cosine first 23 (0x296e60 282: the merge alone costs 0x296e60 the 12); without the forceinline declaration 468.
// On the lead: the dot hand-written or dot3f(axis, vector) 6; `(1 - cos) * dot` 6; rows cosine-first 6; `sine * cross` 6;
// products written axis-first 6; cosine before sine 24; cross k computed first 33; crosses before the trig 158 (931 B);
// crosses between the trig and the projection 162 (0x296e60 276); projection named from dot3f then scaled 144; the vector's
// k cached first (axis_rotation.h's spelling) 244 (894 B); the score written ijk/ikj/jik/kij/kji 8/8/6/8/6 (written order
// only swaps i and j, k stays last); grouped (i + k) + j, (k + i) + j, j + (i + k) 53 each; two statements (i + k) then
// += j, (k + i) then += j 53/53; through dot3f either order 145 (928 B); a `direction` pointer for copy, rotation, trace
// and score 141, without the score 127; FLOOR macro clamps 39 (sum argument, old helper) and 142 (dot3f argument); `0.f <
// length`, fraction before the copy, `best_score < score`, `score < 0.6f`, the best pair first, implicit angle conversion,
// `!= 0` trace test 6 each (same code). Diagnostics: dead longs at the top n = 1..7 16/6/6/6/6/6/6; redundant parens
// on the clamp product 1/2/3 127/129/62, on the angle 88/135/149; permuter (900 s, 93 tries) none below 6. Also on
// the lead: the axis j read once into a named real 146 (925 B; 0x296e60 339); the axis copied into a local vector 209
// (0x296e60 368).
// M3 lane 5 r224 (h2m fast with 0x296e60, 0x297660, 0x2980d0 and 0x24590; control 23, 0x296e60 270): the shared in-place
// rotation with each cross term scaled by the sine where it is computed 153 (925 B) with 0x296e60 199 (plain agreed:
// 153 / 199; retail here reloads the sine late and scales the finished cross term, so a lead for 0x296e60 only); the
// angle named at the loop-body top ahead of the copy 33; lane 3 r277's merged helper re-measured 6 (0x296e60 282), with
// sine-scaled crosses 74 (0x296e60 198); the dot3f projection with sine-scaled crosses 139 (270); the staged dot with
// sine-scaled crosses 153 (199); the score written forward-first 23 (same code); the clamp as a ternary then
// `score *= fraction;` 141.
// M3 lane 5 r226 (h2m fast with 0x296e60, 0x297660, 0x2980d0 and 0x24590; control 23, 0x296e60 270): the in-place
// helper's rows stored i, k, j / j, i, k / k, i, j / j, k, i / k, j, i 151 (927 B) / 57 / 169 / 66 / 172 (910 B), with
// 0x296e60 333/300/326/300/280 (the store order moves the candidate cursor, not the score's order). Every product in
// its own parentheses (a common hand style), by site: score only 62; rows only 22 (0x296e60 282, a one-point change,
// not kept); rows + score 25; crosses 144 (928 B); crosses + score 128; crosses + rows 144 (925 B); crosses, rows
// and score 147; dot only 143 (0x296e60 333); dot + score 140; dot + rows 25 (0x296e60 340); dot + rows + score 37;
// dot + crosses 62; dot + crosses + score 25; dot + crosses + rows 128; all four 166. The same styles on lane 3 r277's
// merged-helper lead (6; 0x296e60 282): score 141 (927 B); rows 142 (926 B; 0x296e60 284); rows + score 8; crosses
// 129; crosses + score 60; crosses + rows 54; all three 124; `(1 - cos) * dot` the same code in each. The pair count
// acts mod 4 here too (0: 6/8, 1: 142/60, 2: 129/124, 3: 141/54), with the site deciding inside a phase.
// M3 lane 4 r245 (h2m fast with 0x296e60/0x297660/0x2980d0/0x24590; control 23, 0x296e60 270): lane 12 r152's lead
// rebuilt (score `(i + k)` then `+= j`, three redundant pairs on the clamp product) 9; its whole residual is one leaf, the
// axis j taking the other operand side in cross k's first product, cross i's second and the j row's accumulator. On the
// `(i + k) += j` score, 0-3 pairs spread over the angle, the fraction product, the three score products and the clamp
// product (70 forms): count 0 124; count 1 143-148; count 2 59 at every site but the angle (83); count 3 with an angle
// pair 44-76, without one 9 (12 forms), 23, 29, 31, 33: the count sets the products, no site mix flips the axis j. On the
// lead: the fraction as a ternary initialiser after the trace 100; one `direction` pointer for the copy, rotation and
// trace 128 (928 B); the score read through a `vector3f const &` to the candidate 146 (928 B). All restored.
// M3 lane 7 r212 (h2m fast with 0x296e60/0x297660/0x2980d0/0x24590; control 23, 0x296e60 270): the loop score grouped
// explicitly as (i + k) + j 124 (928 B), as (k + i) + j 124 (928 B); the shared rotation helper's dot grouped (j + i) + k
// 31 (0x296e60 339, 1442 B), that with the j product written axis-first 31 (0x296e60 339), the dot written k + (j + i) 31
// (0x296e60 283). Restored.
// M3 lane 14 r143 (h2m fast with 0x296e60 at 270 throughout, 0x297660, 0x2980d0 and 0x24590 matched; control 23): the
// trace result typed as the header's named sector-trace view, `trace.blocked` for the first-byte read (as matched
// 0x1b2340) 23 (same code); that with both traces through the eight-argument declaration 66; a `point3f const *` origin
// for both trace calls 69; the actor's sector index named for its test and both calls 182; the best direction indexed at
// each use without the `best` pointer 39. All restored.
// M3 lane 2 r136 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched throughout; control 23): the
// fraction set by if/else after the trace (no initialiser) 144 (the ternary's code); the trace record declared at function
// scope 23; the candidate array declared ahead of the first trace 23; forward's i and j copied as a planar pair before
// the k clear 217 (906 B); the direction loop as a do/while on `++i < 8` 23 (the 23s the same code). Restored.
// M3 lane 8 r224 (h2m fast with 0x296e60, 0x2980d0, 0x297660 and 0x24590; 0x296e60 270 and the rest matched in every build;
// control 23): the loop score as `dot3f(&forward, &directions[i])`, either argument order, with 0-3 redundant pairs on the clamp
// product 35/144/11/128 (928 B at 3). LEAD 11 at 924 B (steering parens, not kept; the shared helper untouched): the rotation dot
// and cross k are retail's; left are the j row (v.j * cos and axis j * projection in each other's registers) and the score's
// order (k, j, i for retail's i, k, j). The hand-written score in all 12 written orders x 0-3 clamp pairs: 23/25 at 0, 60, 144,
// 141. On the dot3f score: the clamp as an if then `score *= fraction` 128 (928 B); as a ternary statement then `*=` 128;
// fraction-first clamp product 35; constant-first angle and `0.2f * distance` the same cycle (35/144/11/128); a `direction`
// pointer for copy, rotation, trace and score 139/143/143/126; the selected or best test reversed 35/35; the fraction as a
// ternary initialiser 114/130/103/129; the angle named 32/145/29/128; the dot3f call itself in 1-3 pairs: total count mod 4.
// No natural form gives the 2-pair shift.
// M3 lane 6 r232 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23): the shared rotate
// helper's trig as `sinf`/`cosf` 145 (928 B; 0x296e60 273); the loop step `i++` 23; the best score declared ahead of the best
// index 25; the candidate array at function scope 23; the clamp as an if then `score *= fraction` 141 (re-measured); the
// fraction initialised ahead of the candidate copy 23. Restored.
// M3 lane 11 r155 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590, MATCH throughout; control 23): a grid of
// argument spelling x score form x 0-3 redundant pairs on the clamp product (score forms: main's; `i + k` then `+= j`;
// dot3f(&forward, &directions[i]); dot3f(&directions[i], &forward)). Plain arguments reproduce the listed cycles (23/60/
// 144/141, 124/143/59/9, 35/144/11/128, 35/144/11/128). A `vector3f *direction = &directions[i]` passed to the rotation and
// the trace (no reads or writes through it) 33/69/144/141, 124/143/68/19, 32/145/29/128, 32/145/29/128; the same pointer
// passed to only the rotation, or only the trace, gives exactly those numbers (its existence sets the cycle, not its uses).
// A `vector3f const *up = g_4687b0` for the rotation 70/144/143/141, 134/65/143/143, 144/146/138/143, 144/146/138/143;
// both pointers 61/145/143/140, 135/172 (911 B)/142/141, 144/145/139/21, 144/145/139/21. The angle wrapper computing
// cosine before sine (0x296e60 270 -> 274) with main's, the two-stage and the dot3f score: 41/73/145/142, 125/144/70/25,
// 51/145/31/129. Nothing below round 152's 9.
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): actor_looking_rotate's in-place step through include/axis_rotation.h's helper 245 (893 B; 0x296e60
// 416). Restored.
// M3 lane 3 r300 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590, MATCH each run; control 23): the candidate
// array declared after the best pair 23 (same code); the trace fraction as `trace.distance / 5.f` 23 (same code: it folds to
// the 0.2 product); the best index stored before the best score 25; the selected direction as `directions[best_index]` at
// each use (no pointer) 39; a per-candidate trace record inside the loop 144 (928 B). Restored.
// M3 lane 15 r140 (h2m fast with 0x296e60/0x297660/0x2980d0/0x24590; control 23, 0x296e60 270): lane 3 r277's merged
// helper (the dot ahead of the sine and cosine) re-measured 6 (0x296e60 282; left: cross i's second product and the
// score's order); on it the score in two statements (`i*i + k*k`, then `+= j*j`) with 0-3 redundant pairs on the clamp
// product 53/143 (928 B)/141 (927 B)/33; the score as `dot3f(&forward, &directions[i])` 145 (928 B), as
// `dot3f(&directions[i], &forward)` 145 (928 B). Retail 0x296e60 runs each of its rotations as fsin, dot, fcos too.
// M3 lane 13 r152 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23): the selected test as
// `best_index >= 0` 34; the selected direction as `directions + best_index` 23 (same code); a `vector3f &direction` bound to
// the candidate for the copy, rotation, trace and score 139 (928 B); no result local, `return true` at the success and
// `return false` at the end 221 (939 B); 0x2105b0 writing straight into the specification's vector (the in-place call and
// the copy dropped) 154 (909 B).
// M3 lane 13 r153 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23). Operand-rank reading
// (the compiler sorts the operands of each product and each sum by a rank, descending): every read of the candidate goes
// through the loop's one walking pointer and carries one rank level (6 here, +2 per redundant paren pair, mod 8); the axis
// reads carry 1 (i), 2 (j) and 0 (k), j and k +2 per pair; forward's members rank by first mention (k at its clear, then j
// and i in the score). Retail's rotation needs the candidate's k above the axis j, and the axis j at or above the
// candidate's i and j, so no single level for the candidate gives it; the leads at 6, 9 and 11 are tie cases of it. A
// two-statement score splits only the candidate's i from its j and k (`(j + k)` then `+= i` levels 2/4/4, `(i + k)` then
// `+= j` 6/4/4, `(i + j)` then `+= k` 4/4/4); a named candidate pointer gives the candidate the axis's per-member levels
// (6/4/2 against 1/4/2). Tried: the score through a `vector3f const *` to the candidate declared at the score 43; that
// pointer handed to dot3f 32; the rotation run on a local copy that is stored to the array afterwards 195; the score
// computed ahead of the fraction test 34; lane 3 r277's merged helper with the dot between the sine and the cosine
// (retail's instruction order) 6 (0x296e60 273; the lead's code here, match-only).
// M3 lane 12 r189 (reduced-image screen with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590; control 23). Rank levels
// read per form: main's body has the candidate at 6/6/6 and the axis at i 1, j 2, k 0. One redundant pair on the angle
// 132 (candidate 4, axis 1/4/2); the in-place helper's projection as `dot3f(axis, vector) * (1 - cosine)` 144, that with
// the angle pair 66: there the candidate's k sits on its own level (2, beside the axis j) with i and j on 6, the first form
// where the three members split, though to the wrong side. Lane 3 r277's merged helper rebuilt 6 (0x296e60 282): the
// candidate (2/2/2) ties exactly with the axis j (2), which gives retail's dot order and puts the axis j first in all
// three products that pair it with the candidate, where retail has the candidate's k first in cross i's second term; the
// score's i, j, k comes from forward's members being numbered k (at its clear), then j and i (at the score). On that
// lead: `dot3f(axis, vector)` 6 (same code: the helper's argument order does not settle the tie); 1/2/3 pairs on the
// angle 88/135/149, the same with the axis-first dot; forward's k cleared through a pointer in an inline helper
// (diagnostic) 47, k still numbered ahead of j and i; the score as `dot3f(&directions[i], &forward)` or
// `dot3f(&forward, &directions[i])` with 0-3 pairs on the clamp product 145/139/88/145 each. Left: a form where the
// candidate's k alone ranks above the axis j.
// M3 lane 13 r158 (h2m fast with 0x296e60, 0x297660, 0x2980d0 and 0x24590; control 23, 0x296e60 270 at 1431 B): lane 3 r277's
// merged helper (sine, dot, cosine) re-measured 6 with 0x296e60 273 (1430 B; aligned 128 -> 135 there, a register swap of
// its two scale products plus row orders), with the dot hand-written, `dot3f(axis, vector)` or `(1 - cos) * dot` the same
// 6/273, dot first 6/282, versine named 6/273. On that lead: the angle named at the loop-body top 16; the up axis through a
// `vector3f const *` local 175 (909 B); the cross terms in a vector3f local 49 (0x296e60 337); the projection after the
// cross terms 162 (934 B; 0x296e60 276). Restored.
// M3 lane 14 r156 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23): the loop's rotation
// through include/axis_rotation.h's in-place helper with sine and cosine locals in the loop body 246 (892 B), with a named
// angle and the trig as arguments 245 (893 B); the direction counter as `long` 219 (898 B); the selected test as
// `best_index > NONE` 24; the score read through a `vector3f const &` bound to the candidate at the score 43. Restored.
// M3 lane 3 r312 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23): forward declared
// and then assigned from the actor 23; the length test written `0.f < function_30bf0(&forward)` 23; the clamp's arms
// swapped (`score > 0.6f ? score : 0.6f`) 25; the angle with an implicit conversion (`i * 0.785398185f`) 23; the best
// test as `best_score < score` 23 (the 23s the same code). Restored.
// M3 lane 12 r194 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23). Reading: the
// score's product order follows the numbering of forward's members (k at its clear, then j and i at the score), and
// retail's i, k, j needs j numbered ahead of k. Forms: forward built member by member (i, j, then the k clear) 199
// (927 B); forward from a three-value initialiser list 199 (927 B); forward through the existing three-value setter
// 199 (926 B); the score's terms written i, j, k 25; the candidate copied through that setter 87 (930 B).
// M3 lane 13 r165 (h2m fast with 0x296e60 at 270, 0x297660, 0x2980d0 and 0x24590 matched; control 23): the in-place
// helper's parameters as (axis, vector, sine, cosine), (vector, axis, cosine, sine) or (sine, cosine, vector, axis) 23
// each (same code: an inline helper's parameter positions do not reach these ranks); the in-place helper
// `__forceinline` 23; the angle wrapper without its forceinline declaration 468 (670 B: it stays out of line); the
// pathfinding pointer as a conditional initialiser 207. Restored.
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

// score 442 (was 442): Frame 0xcc versus retail 0xc8. The inlined combat test and actor-slot and animation-state homes differ.
// MiniMax A (11-20): no new form, no lower score (442): the frame is still 0xcc against retail's 0xc8 and the first difference is at the entry, where retail loads the actor table into ECX, keeps the actor index in ESI and the scaled index in EAX (stored to a home at +0x30) and holds the animation state in DI, while this build loads the table into EDX, reloads it into EAX, keeps the actor index in EDI and the animation state in DX; that is the allocator's choice for the values live across the 0x297600 and 0x2973f0 calls, and 0x1fdd90 is still expanded here where retail calls it out of line.
// Earlier score 442 (was 442): frame 0xcc versus retail's 0xc8; 0x1fdd90 is inlined here (retail calls it), and any test ahead
// MiniMax A (11-20): no new form, no lower score (442): the frame is 0xd0 against retail's 0xc8
// score 442 (was 442): frame 0xcc versus retail's 0xc8; 0x1fdd90 is inlined here (retail calls it), and any test ahead
// of the call keeps it out of line (r131, below), which still leaves the valid flag's type and the per-path publications.
// MiniMax B (11-19): no new form tried this round, no lower score (442): the frame is still 0xcc against retail's
// 0xc8 and the local layout differs with it - this build keeps the animation state at [esp+0x28] and the four cosines
// below it, where retail keeps the state at [esp+0x20] and the cosines above it, and the turn request's fields four
// bytes higher - so every stack home differs; 0x1fdd90 is still expanded here where retail calls it out of line:
// BLOCKED-BY 0x1fdd90.
// MiniMax B (11-18, round 2): no new form, no lower score (442): the frame is still 0xcc against retail's 0xc8 and the
// local layout differs with it - this build keeps the animation state at [esp+0x28] and the four cosines below it,
// where retail keeps the state at [esp+0x20] and the cosines above it, and the turn request's fields four bytes
// higher - so every stack home differs; the declaration order that would put the locals where retail has them is not
// kept, and 0x1fdd90 is still expanded here where retail calls it out of line: BLOCKED-BY 0x1fdd90.
// MiniMax B (11-18): no new form, no lower score (442): the entry block's register roles still differ - retail keeps
// the actor index in ESI, the slot offset in EAX, adds it to the base and spills it at esp+0x30, where this build
// colours the registers differently - and 0x1fdd90 is still expanded here where retail calls it out of line (its
// definition is in another file): BLOCKED-BY 0x1fdd90.
// MiniMax E (11-15): no new form, no lower score (442): the call boundaries agree with retail's except that 0x1fdd90 is expanded here where retail calls it out of line, and its definition is in another file. BLOCKED-BY 0x1fdd90.
// MiniMax C (11-15): no new form, no lower score (442 plain): the frame is still 0xd0 against retail's 0xc8 and the
// cached animation words still have other homes, and this link still inlines 0x1fdd90 where retail calls it, which
// keeps the actor table in a register; the forms for the frame and the carriers are in the notes below and none of them
// lowers the score, and 0x1fdd90's own file is not this lane's: BLOCKED-BY 0x1fdd90.
// MiniMax B (11-14): no new form, no lower score (442 plain, 548 on the reduced image): the entry block still differs
// in its registers - retail loads the actor table into ECX, takes the row with `mov ebp, [ecx + A]`, keeps the actor
// index in ESI and spills the scaled offset to the stack, and reads the member word with `movsx edx` and `mov di`,
// where this build loads the table into EDX, keeps the index in EDI, scales it in ESI without a spill and reads the
// word with `movsx ecx` and `mov dx`; the frame stays 0xd0 against retail's 0xc8 and 0x1fdd90 is still inlined here
// where retail calls it, and that helper is defined in another file. The body stands.
// MiniMax G (11-13): no new form, no lower score (442 plain, 548 on the reduced image): the diff re-read this round keeps the frame 0xd0 against retail's 0xc8 and the cached animation words without retail's homes, and 0x1fdd90 is still inlined here where retail keeps it out of line, so the body stands.
// MiniMax H (11-13): no new form, no lower score (442 plain, 548 on the reduced image).
// MiniMax S (11-14): no new form, no lower score (442): the two getters that hand back the view and the object
// are shared with many other functions of this file, so their spelling - which decides whether the base and the
// scaled index stay apart - was left alone and the body was re-measured unchanged.
// MiniMax C (11-16): no new form, no lower score (442 plain, 548 on the reduced image): the frame is 0xcc against
// retail's 0xc8 and the blocker is unchanged - retail calls 0x1fdd90 from its only call site while this build inlines
// it, and only /Ob1 on that helper's own file keeps it out of line, which is a build-flag change this lane may not
// make.
// MiniMax E (11-16): no new form, no lower score (442): the frame is still 0xcc against retail's 0xc8 and the
// blocker is unchanged - retail calls 0x1fdd90 from its only call site while this build inlines it, and only
// /Ob1 on that helper's own file keeps it out of line, which is a build-flag change this lane may not make.
// MiniMax C (11-17): no new form, no lower score (442, was 442): the frame is still 0xcc against retail's 0xc8, and 0x1fdd90 is still inlined here where retail calls it from its only call site (only /Ob1 on its own file keeps it out of line, a build-flag lead): BLOCKED-BY 0x1fdd90.
// Earlier 442: frame 0xcc versus retail's 0xc8. BLOCKED-BY 0x1fdd90: retail calls it from its only
// MiniMax E (11-20): no new form, no lower score (442): the frame is still 0xcc against retail's 0xc8 and the body keeps its own length; the entry's argument order follows 0x1fdd90's convention, which does not match yet, so the call setup cannot be had from this file: BLOCKED-BY 0x1fdd90.
// Earlier score 442 (was 442): frame 0xcc versus retail's 0xc8. BLOCKED-BY 0x1fdd90: retail calls it from its only
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
// MiniMax D (11-16): no new form, no lower score (442).
// MiniMax S (11-16): no new form, no lower score (442): the same cold block placement as in 0x2967b0, this
// build puts the default and the case blocks in a far section where retail keeps them in the function.
// MiniMax A (11-18): no new form, no lower score (442): the entry block still differs in register roles and in when the
// record address is formed - retail keeps the actor index in esi, builds the slot offset in eax and adds it to the base,
// where this build keeps the actor index in edi and the offset in esi - and the shared getter cannot change because
// sixteen matched functions in this file use it; retail also calls 0x1fdd90 out of line where this build expands it
// inline, and that callee's file is not this lane's: BLOCKED-BY 0x1fdd90.
// Earlier 442: MiniMax D (11-17): no new form, no lower score (442 plain, 548 on the reduced image): the entry
// block still differs in register roles and in when the record address is formed - retail keeps the actor index in esi,
// builds the slot offset in eax and adds it to the base (`and eax, 0xffff`, `imul eax, eax, 0x888`, `add ebp, eax`) and
// spills the offset to [esp+0x30], and then reads the record through ebp, where this build keeps the actor index in edi,
// the offset in esi, reads the record through `mov eax, [ebp + esi + 0x18]` and only then adds the offset to the base;
// the shared getter cannot change (sixteen matched functions in this file use it), so the spelling has to be this
// function's own. BLOCKED-BY 0x1fdd90: retail calls it out of line and this build expands it inline.
// M3 lane 10 r131 (fast with 0x2967b0, 0x296e60, 0x297660, 0x297d30, 0x297a50, 0x2980d0, 0x1fdd90, 0x2982f0, 0x297600,
// 0x2973f0, 0x296d60, 0x297560, 0xe70b0 and the callees 0x1ffe00/0x1e1f20; control 442, aligned 356). New reading: a
// test ahead of the 0x1fdd90 call keeps it out of line with no build flag: the combat test as `!actor->unknown48a &&
// function_1fdd90(actor_index)` 590 at 1952 B (aligned 325), and an implied `animation_state != 6 &&` ahead of the call
// (diagnostic, folds) 591 at 1952 B, the same code as a noinline pin (diagnostic, reverted) 591. So the blocker is a
// form question, but out of line the body is still 68 bytes short of retail's 2020: retail's valid flag is a bool (BL
// only in the marker arm, threaded elsewhere), retail publishes the aiming direction a second time after the turn
// request (0x2986fb) and publishes the looking output per path, with the identical tails cross-jumped (one forward
// copy, one aiming-output copy shared by animation state 6 and the looking modes' else arm, two direction copies).
// On the out-of-line form (implied test or pin, diagnostics): bool valid 599 (aligned 314), with bool looking_valid
// 606 (310), the looking output as an if/else 589/601/632 (aligned 305/296/298 with the bools and the existing 0xe70b0),
// the state through the existing 0xe70b0 620/628/635 (aligned 334/326). Forms on the body: the 0x1fdd90 declaration
// at block scope in the caller 442 (same code); the combat test as `function_1fdd90(...) ? !unknown48a : false` 531
// (2024 B); the turn arm publishing the direction itself, with the not-turn arm and the plain arms publishing their own
// (three publication arms) 537 (2008 B); the looking output written per path with a forward default (no looking_valid)
// 637 (2132 B: the two 0x2967b0 calls no longer share their tail); both 667 (2136 B); both with bool valid 585 (2068 B);
// the looking output as an if/else 579 (2080 B, re-measured); with bool valid and looking_valid 539 (2004 B). Diagnostic:
// a second caller of 0x1fdd90 (in its own file or in this one) leaves this site inlined (442).
// MiniMax G (11-20): no new form, no lower score (442): the frame is still 0xd0 against retail's 0xc8, because
// this build inlines the matched predicate 0x1fdd90 where retail calls it at +0x198, and the later signed-angle
// call still takes the horizontal inputs in the opposite order (our comiss xmm1,xmm0 against retail's comiss
// xmm0,xmm1, with esi and edi swapped): BLOCKED-BY 0x1fdd90's own file.
// Share the masked long actor slot through the looking, movement, or prop lookup 534/534/534.
// Word slot with those lookups 523/523/523. Shared byte displacement, direct or named bases 442/442.
// NOT kept: both slot types recover frame 0xc8 and the displacement home but shorten and raise the score.
// M3 lane 5 r238 (h2m fast with its callees and 0x1fdd90; control 442, aligned 356 exact / 179 register-masked; 27
// calls for retail's 28): 0x1fdd90's own body with its second test nested (a heavier body) is still expanded here
// (548) and no longer matches itself (7); the looking output chosen through a `vector3f const *` with one copy at
// the end 536 (2012 B; aligned 365 / 156), with `bool valid` 614 (1932 B; 341 / 165); `bool valid` and `bool
// looking_valid` re-measured 547 (1996 B; 366 / 175). Reading: the actor's pool offset retail spills at entry is read
// once, by the output getter at the end, which this body already re-reads there. BLOCKED-BY 0x1fdd90 stands.
// M3 lane 6 r241 (h2m fast with its fifteen listed callees and neighbours; control 442): the target point stored before
// the 0x8000 flag 457 (2008 B); output_mode stored before the target-point block 451; the attention flag as one ternary
// store 442 (same code); the turn test with the angle test first 449; the three flags declared ahead of the animation
// state 442 (same code). Retail calls 0x1fdd90 first and tests the +0x48a byte after it, so a test-first order is not
// retail's. Body unchanged.
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
