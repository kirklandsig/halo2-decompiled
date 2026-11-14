// @flags /O2 /Ob1 /arch:SSE /Gr
/* VEHICLES.CPP: vehicles

The vehicle object type's callbacks (its definition at 0x467b40) and the
helpers only they call. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "bit_vector.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "unknown_1428b0.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_1cec30.h"
#include "effects.h"
#include "bit_vector.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "unknown_1eb550.h"
#include "vehicle_ray.h"
#include "object_tag_definitions.h"
#include <math.h>
#include <string.h>

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif

#ifndef FLAG
#define FLAG(bit) (1 << (bit))
#endif

#ifndef SET_FLAG
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))
#endif

/* the vehicle (the unit fields, then the vehicle's own from +0x334; the
   fields read here) */
struct s_vehicle
{
	long definition_index;
	dword object_flags;
	byte unknown008[0xc];
	long parent_object_index;
	byte unknown018[0x30 - 0x18];
	point3f center;
	byte unknown03c[0x70 - 0x3c];
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
	byte unknown0a0[0xaa - 0xa0];
	byte object_type;
	byte unknown0ab[0xb4 - 0xab];
	long havok_component_index;
	byte unknown0b8[0xbc - 0xb8];
	long unknown0bc;
	word flags_c0;
	byte unknown0c2[0x10a - 0xc2];
	byte flags_10a;
	byte unknown10b[0x118 - 0x10b];
	short unknown118;
	short unknown11a;
	byte unknown11c[0x120 - 0x11c];
	short unknown120;
	short unknown122;
	byte unknown124[0x12a - 0x124];
	short animation_state_offset;
	byte unknown12c[0x134 - 0x12c];
	dword flags_134;
	byte unknown138[0x148 - 0x138];
	dword control_flags;
	byte unknown14c[4];
	vector3f local_velocity;
	byte unknown15c[0x1b0 - 0x15c];
	real throttle;
	real steering;
	real unknown1b8;
	byte unknown1bc[0x248 - 0x1bc];
	long unknown248;
	byte unknown24c[0x25c - 0x24c];
	real unknown25c;
	real unknown260;
	byte unknown264[0x288 - 0x264];
	real unknown288[3];
	byte unknown294[0x2c4 - 0x294];
	real boost;
	byte unknown2c8[0x334 - 0x2c8];
	real unknown334;
	real unknown338;
	byte unknown33c[0x348 - 0x33c];
	byte flags348;
	byte flags349;
	byte unknown34a;
	byte unknown34b;
	byte unknown34c;
	byte unknown34d;
	byte unknown34e;
	byte unknown34f;
	byte unknown350;
	byte unknown351;
	word unknown352;
	real speed;
	real turn;
	real steering_angle;
	real unknown360;
	real unknown364;
	real unknown368;
	real unknown36c;
	real unknown370;
	real unknown374;
	real unknown378;
	real unknown37c;
	real unknown380;
	byte unknown384;
	byte unknown385;
	byte unknown386;
	byte unknown387;
	byte unknown388;
	byte unknown389[2];
	char gear;
	char unknown38c;
	byte unknown38d;
	short unknown38e;
	long unknown390[2];
	long unknown398[2];
	long unknown3a0;
	long unknown3a4;
	long unknown3a8;
	long unknown3ac;
	long unknown3b0;
	long unknown3b4;
	long unknown3b8;
	long unknown3bc;
	long unknown3c0;
	long unknown3c4;
	long unknown3c8;
	long unknown3cc;
	point3f unknown3d0;
	real unknown3dc;
	real unknown3e0;
	point3f unknown3e4;
	vector3f unknown3f0;
	real unknown3fc;
	long unknown400;
	short unknown404[12];
	short unknown41c[3][2];
};

struct s_vehicle_header
{
	byte unknown00[2];
	byte flags;
	byte type;
	byte unknown04[4];
	s_vehicle *vehicle;
};

#define VEHICLE_GET(index) (((s_vehicle_header *)g_4e0300->data)[(index) & 0xffff].vehicle)
#define VEHICLE_HEADER_GET(index) (&((s_vehicle_header *)g_4e0300->data)[(index) & 0xffff])
#define VEHICLE_OVERLAY(vehicle, index) (*(c_type_709360 *)&(vehicle)->unknown404[(index) * 2])
#define VEHICLE_OVERLAY41C(vehicle, index) (*(c_type_709360 *)(vehicle)->unknown41c[index])
#define VEHICLE_DEFINITION_GET(vehicle) (g_4e3b44[(vehicle)->definition_index & 0xffff].bytes)

/* a flags word read as bits */
struct s_vehicle_word_flag_bits
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

/* a flags dword read as bits */
struct s_vehicle_flag_bits
{
	dword bit0 : 1;
	dword bit1 : 1;
	dword bit2 : 1;
	dword bit3 : 1;
	dword bit4 : 1;
	dword bit5 : 1;
	dword bit6 : 1;
	dword bit7 : 1;
	dword bit8 : 1;
	dword bit9 : 1;
	dword bit10 : 1;
	dword bit11 : 1;
	dword bit12 : 1;
	dword bit13 : 1;
	dword bit14 : 1;
	dword bit15 : 1;
	dword bit16 : 1;
	dword bit17 : 1;
	dword bit18 : 1;
	dword bit19 : 1;
	dword bit20 : 1;
	dword bit21 : 1;
	dword bit22 : 1;
	dword bit23 : 1;
	dword bit24 : 1;
	dword bit25 : 1;
	dword bit26 : 1;
	dword bit27 : 1;
	dword bit28 : 1;
	dword bit29 : 1;
	dword bit30 : 1;
	dword bit31 : 1;
};

#define VEHICLE_DEFINITION_FLAGS(definition) ((s_vehicle_flag_bits const *)((definition) + 0x1ec))
#define VEHICLE_WORD_BITS(field) ((s_vehicle_word_flag_bits const *)&(field))
#define VEHICLE_DWORD_BITS(field) ((s_vehicle_flag_bits const *)&(field))

void __stdcall function_c42e0(long unit_index, void const *placement);
bool __stdcall function_10f430(long unit_index, long field_7c, long state_name, long weapon_name, long action_name,
	real blend, bool flags, long mode);
void __stdcall function_bd020(long object_index);
point3f *function_b9ef0(long object_index, point3f *result);
void __stdcall function_b9b90(long object_index, bool disable);
real function_d1210(long object_index);
// @retail 0x2053c0
void function_2053c0(real *value, real const *rates, real direction, real dt)
{
	(void)&direction;
	(void)&dt;
	if (direction != 0.0f)
	{
		real acceleration = (real)(fabs(direction) * rates[2] * dt);
		real deceleration = (real)(fabs(direction) * rates[3] * dt);
		if (direction > 0.0f)
		{
			if (*value <= -deceleration)
				*value += deceleration;
			else if (*value >= 0.0f)
				*value += acceleration;
			else
				*value = (*value / deceleration + 1.0f) * acceleration;
			*value = *value > rates[0] ? rates[0] : *value;
		}
		else
		{
			if (*value >= deceleration)
				*value -= deceleration;
			else if (*value <= 0.0f)
				*value -= acceleration;
			else
				*value = (*value / deceleration - 1.0f) * acceleration;
			*value = *value > -rates[1] ? *value : -rates[1];
		}
	}
}

// @retail 0x2054b0
bool function_2054b0(real *value, real const *rates, real direction, real dt, real target)
{
	bool result = true;
	if (*value > target)
	{
		function_2053c0(value, rates, -direction, dt);
		if (*value <= target)
			*value = target;
		else
			result = false;
	}
	else if (*value < target)
	{
		function_2053c0(value, rates, direction, dt);
		if (*value >= target)
			*value = target;
		else
			result = false;
	}
	return result;
}

/* the vehicle's animation state */
PRIVATE inline s_animation_state *vehicle_animation_state_get(s_vehicle *vehicle)
{
	return (s_animation_state *)((byte *)vehicle + vehicle->animation_state_offset);
}

/* places a vehicle from the scenario: the unit placement, and (when the
   placement asks) its parked animation */
// @retail 0xee910
void __stdcall vehicle_place(long vehicle_index, byte const *placement)
{
	function_c42e0(vehicle_index, placement + 0x4c);
	if (placement[0x50] & 2)
	{
		if (function_10f430(vehicle_index, 0x7000001, 0x7000101, 0x7000101, 0x700005d, 0.0f, 0, 0))
		{
			vehicle_animation_state_get(VEHICLE_GET(vehicle_index))->channels_finish();
			function_bd020(vehicle_index);
		}
		else
		{
			function_10f430(vehicle_index, 0x7000001, 0x7000101, 0x7000101, 0x400000c, 0.0f, 0, 0);
			function_bd020(vehicle_index);
		}
	}
}

/* clears a vehicle's driving state */
// @retail 0xee9b0
void __stdcall function_ee9b0(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	c_type_709360 *overlay;
	long i;

	vehicle->unknown34a = 0;
	*(word *)&vehicle->flags348 = 0;
	vehicle->unknown34b = 0;
	vehicle->unknown34c = 0;
	vehicle->unknown34d = 0;
	vehicle->unknown34e = 0;
	vehicle->unknown34f = 0;
	vehicle->unknown350 = 0;
	vehicle->unknown384 = 0;
	vehicle->unknown385 = 0;
	vehicle->unknown386 = 0;
	vehicle->unknown387 = 0;
	vehicle->unknown388 = 0;
	vehicle->unknown38e = 0;
	vehicle->unknown351 = (byte)NONE;
	vehicle->speed = 0.0f;
	vehicle->turn = 0.0f;
	vehicle->steering_angle = 0.0f;
	vehicle->unknown334 = 0.0f;
	vehicle->unknown338 = 0.0f;
	vehicle->unknown360 = 0.0f;
	vehicle->unknown364 = 0.0f;
	vehicle->unknown368 = 0.0f;
	vehicle->unknown36c = 0.0f;
	vehicle->unknown374 = 0.0f;
	vehicle->unknown370 = 0.0f;
	vehicle->unknown378 = 0.0f;
	vehicle->unknown37c = 0.0f;
	vehicle->unknown38c = (char)0xfe;
	vehicle->unknown380 = 0.0f;
	vehicle->gear = NONE;
	memset(vehicle->unknown398, 0, sizeof(vehicle->unknown398));
	memset(vehicle->unknown390, 0, sizeof(vehicle->unknown390));
	vehicle->unknown3ac = 0;
	word *flags = (word *)&vehicle->flags348;
	*flags |= FLAG(8);
	function_b9ef0(vehicle_index, &vehicle->unknown3d0);
	vehicle->unknown3b8 = NONE;
	vehicle->unknown3bc = NONE;
	vehicle->unknown3c8 = NONE;
	vehicle->unknown3c0 = NONE;
	vehicle->unknown3cc = NONE;
	vehicle->unknown400 = NONE;
	VEHICLE_OVERLAY(vehicle, 0).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 0).index = NONE;
	VEHICLE_OVERLAY(vehicle, 1).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 1).index = NONE;
	VEHICLE_OVERLAY(vehicle, 2).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 2).index = NONE;
	VEHICLE_OVERLAY(vehicle, 3).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 3).index = NONE;
	VEHICLE_OVERLAY(vehicle, 4).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 4).index = NONE;
	VEHICLE_OVERLAY(vehicle, 5).graph_index = NONE;
	VEHICLE_OVERLAY(vehicle, 5).index = NONE;
	overlay = &VEHICLE_OVERLAY41C(vehicle, 0);
	for (i = 0; i < 3; i++)
	{
		overlay->graph_index = NONE;
		overlay->index = NONE;
		overlay++;
	}
}

/* a new vehicle: its state reset, its physics, and its parked animation */
// @retail 0xeeb80
bool __stdcall function_eeb80(long vehicle_index, void const *data, long unused)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool result = false;

	if (*(long *)(g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes + 0x24) != NONE)
	{
		vehicle->unknown3a0 = NONE;
		vehicle->unknown3a4 = NONE;
		vehicle->unknown3a8 = 0;
		vehicle->unknown3b0 = 0;
		vehicle->unknown3b4 = 0;
		function_ee9b0(vehicle_index);
		if (definition[0x2ac] & 1)
			function_b9b90(vehicle_index, true);
		if (VEHICLE_GET(vehicle_index)->animation_state_offset != NONE)
		{
			if (function_10f430(vehicle_index, 0x7000001, 0x7000101, 0x7000101, 0x700005c, 0.0f, 0, 0))
			{
				vehicle_animation_state_get(vehicle)->channels_finish();
				return true;
			}
			function_10f430(vehicle_index, 0x7000001, 0x7000101, 0x7000101, 0x400000c, 0.0f, 0, 0);
		}
		result = true;
	}
	return result;
}

bool function_f42f0(long vehicle_index);

PRIVATE inline void vehicle_approach(real *value, real const *rates, real bound, real dt);

/* a vehicle's steering: the angle to its velocity, eased by the
   definition's steering curve, and its throttle and turn rates */
// @retail 0xeec90
void function_eec90(long vehicle_index, real *steering)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	vector3f const *forward = &vehicle->forward;
	vector3f left;

	cross3f(&vehicle->up, forward, &left);
	real angle = (real)atan2(left.k * vehicle->local_velocity.k + left.j * vehicle->local_velocity.j +
		left.i * vehicle->local_velocity.i, dot3f(&vehicle->local_velocity, forward));

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit3) &&
		!function_f42f0(vehicle_index) && !(function_d1210(vehicle_index) > 0.0f))
	{
		function_2054b0(&vehicle->speed, (real const *)(definition + 0x1f4), 1.0f, dt, 0.0f);
		function_2054b0(&vehicle->turn, (real const *)(definition + 0x22c), 1.0f, dt, 0.0f);
	}
	else
	{
		real throttle = vehicle->throttle;
		real const *rates = (real const *)(definition + 0x1f4);
		real bound = (0.0f > throttle ? rates[1] : rates[0]) * throttle;
		vehicle_approach(&vehicle->speed, rates, bound, dt);
		real turn_input = vehicle->steering;
		rates = (real const *)(definition + 0x22c);
		bound = (0.0f > turn_input ? rates[1] : rates[0]) * turn_input;
		vehicle_approach(&vehicle->turn, rates, bound, dt);
	}

	real maximum_angle = (*(real *)(definition + 0x250) == 0.0f ? 1.0 : *(real *)(definition + 0x250)) *
		0.017453292384743690;
	real exponent = *(real *)(definition + 0x254) == 0.0f ? 1.0 : *(real *)(definition + 0x254);

	if (angle < -3.1415927f)
		angle = -3.1415927f;
	else if (angle > 3.1415927f)
		angle = 3.1415927f;

	real ratio = (real)fabs(angle / maximum_angle);
	real sign = angle >= 0.0f ? 1.0f : -1.0f;
	real eased = (real)pow(ratio, exponent) * sign * maximum_angle;

	if (eased < -3.1415927f)
		eased = -3.1415927f;
	else if (eased > 3.1415927f)
		eased = 3.1415927f;

	real target = !vehicle->gear ? 0.0f - eased : eased;
	real rate = *(real *)(definition + 0x210) * 0.017453292f;
	/* the steering limits in degrees: +0x204 the largest angle, +0x208 the smallest */
	real_bounds angles = *(real_bounds const *)(definition + 0x204);
	real minimum = angles.hi * 0.017453292f;
	real maximum = angles.lo * 0.017453292f;
	real difference = target - vehicle->steering_angle;

	if (difference != 0.0f)
	{
		real direction = difference > 0.0f ? 1.0f : -1.0f;

		vehicle->steering_angle = direction * rate * dt + vehicle->steering_angle;
		if (minimum > vehicle->steering_angle)
			vehicle->steering_angle = minimum;
		else if (vehicle->steering_angle > maximum)
			vehicle->steering_angle = maximum;

		real remaining = target - vehicle->steering_angle;

		if (remaining != 0.0f)
			remaining = remaining > 0.0f ? 1.0f : -1.0f;
		if (direction != remaining)
			vehicle->steering_angle = target;
	}
	*steering = eased;
}

/* a pointer to the 2d forward default (0x468778, to 0x440af4) */
point2f *g_468778;

/* a 2d vector made unit length, or the default when it's too short */
PRIVATE inline void vehicle_normalize2d(point2f *v, point2f const *fallback)
{
	real length = (real)sqrt(v->y * v->y + v->x * v->x);

	if (!(0.0001f > (real)fabs(length)))
	{
		real inverse = 1.0f / length;

		v->x = inverse * v->x;
		v->y = v->y * inverse;
		if (length != 0.0f)
			return;
	}
	*v = *fallback;
}

/* direct turn control: the input turned into the frame of the velocity,
   the angle it asks the vehicle to turn, and whether it's reversing */
// MiniMax B (11-13): no new form, no lower score (126): the velocity copy still takes x in ECX and y in EAX where retail takes
// x in EAX and y in ECX, the entry magnitude's schedule (retail adds i*i + j*j before squaring k, so its 1.0 lands in XMM3) and
// the reversing atan2's operand order remain; the known leads (the planar velocity copy with y-first squares 122, the blend rows
// written x first) are statement orders or under the eight-point bar.
// MiniMax G (11-12): no new form, no lower score (126): nothing new this round; the SSE register colouring of the 1.0 constant
// and the remaining homes remain.
// MiniMax G (11-12): no new form, no lower score (126): the frame, the length and the layout are retail's; left are
// the SSE register colouring (the 1.0 constant) and the remaining homes.
// MiniMax H (11-11): no new form, no lower score (126); the velocity copied as a planar aggregate with the speed's
// squares written y then x 122 (four points, under the bar) and the y-first squares alone 126 (same code), so the
// original body is kept; the 1.0 constant still lands in xmm2 instead of xmm3. Later in the same round: the velocity
// copied x first 130 and the input magnitude computed after the speed 166 (the magnitude has to come first), so the
// body's order stays.
// score 126 (was 126): frame, length and layout now retail's; left: SSE register colouring (the 1.0 constant in
// xmm2 against retail's xmm3, the normalized forward/heading in other registers), the velocity copy's eax/ecx, two
// x87 operand orders in the atan2 sums and the reversing compare's operand order (`fxch`, jb).
// MiniMax C (11-10): the score is 126 (was 126); the entry magnitude is added after the velocity copy where retail adds the two squares before the square root, so the 1.0 constant lands in xmm2 instead of xmm3 and the x87 operand order of the sums differs.
// MiniMax R (11-09): the input rotation before the forward copy with the entry magnitude after the velocity copy is 122 (four points, under the bar) and the permuter's thirty tries found no variant, so the original body is kept at 126.
// MiniMax R (11-06): no new form, no lower score (126); the frame agrees and only the SSE colouring and x87 operand orders of the note below are left.
// MiniMax R (11-09): the input rotation before the forward copy with the entry magnitude after the velocity copy is 122 (four points, under the bar) and the permuter's thirty tries found no variant, so the original body is kept at 126.
// MiniMax R (11-06): no new form, no lower score (126); the frame agrees and only the SSE colouring and x87 operand orders of the note below are left.
// Earlier: frame 0x20 versus 0x28, initial magnitude and vector homes differ.
// Entry magnitude/direction/shared-normalizer subsets 306/337/324/327/309/339/303
// restored; early magnitude observation needs a match. Existing turn approach 330.
// Shared byte writer 306, retained after plain confirmation; with turn approach/product terms 310/310.
// Round 77: the reversing bit written as bit 9 of the flags word (word_flags_set) 314: the writer is
// then retail's (`or/and byte ptr [esi+0x349]`, one exit each; ours split it into load/op/store), and
// only its position against the longer retail body moves the count. Restored on that count.
// Round 425: one velocity/direction workspace 321; one heading/blend workspace 307; both 322;
// complete 2D role names alone/with either/both 306/321/307/322; both with long reverse result 319.
// All original source restored; the 0x28 frame and entry magnitude remain unresolved.
// Lane 13 round 25: retail keeps the three-axis input magnitude squared from the entry to the end (its slot is
// read after the turn approach: the reversing bit needs it above 0.09, not dt) and recomputes it for the gear
// (length_sq3f at both, the flag test fixed) 312 at 1171 bytes; forward's 2D left vector as a point2f `left`, the
// angle's y the input's dot with it (retail's 0x28 frame and slot order) 312 (aligned 160 -> 119); then the shared
// normalizer taking its fallback (blend falls back to forward), the existing vehicle_approach with a rates pointer
// and the bit-9 flag writer (round 77's form, repeated because it only pays once the body has retail's length) 187
// at 1145; the magnitude before the velocity copy 216 (1144); with blend's left vector as a point2f `blend_left`
// 132; with the velocity copied y first 128; the fallback direction stored y first 130; both 126 (kept). On it:
// `fabs(...) >= 1.9198622f` (retail's fxch/jb) 218 at 1145; the atan2 terms or operands swapped 126 each;
// compile-only screens, none better: statement orders of the forward copy/input/normalize/left/heading (144),
// the normalizer's products as `*=`, either operand order or y first, magnitude term orders/groupings/dot3f.
// Also on 126: the velocity as one point2f copy 130; the entry magnitude written out 128.
// Lane 13 round 28: blend.x first with `velocity * inverse` and the fallback x first 128; the magnitude after the
// velocity copy with k's x term first and blend.x first 128; the normalizer as `*=` 126; the velocity and fallback
// copied x first 132 (300 compile-only statement-order combinations screened; their best three 128/128/130).
// Round 182 (the 1.0 constant still lands in xmm2 after the input magnitude's z term, retail's xmm3 before it):
// the normalizer returning the old length with each caller copying its fallback on 0 126 (identical); the
// normalizer forced inline 126 (identical); the input magnitude after the speed root 278; left/blend/blend_left
// declared at the top 126 (identical); one controls-vector pointer for both magnitudes 243.
// Round 103 (first difference: the velocity copy loads x into ECX, retail EAX; then the 1.0 constant): the
// velocity as an aggregate initializer `{ i, j }` 130; the short-speed fallback as `direction = velocity` 275
// (1159 bytes); forward, heading or the input vector as aggregate initializers 126 each (identical).
// Round 44 (lane 11): k written in both blend rows 126, k declared after blend 126, heading declared before forward
// 126 (all identical code); the input rotation before the forward copy 122; forward normalized before the input
// rotation 270 (1146 bytes). Retail loads the 1.0 constant between the entry magnitude's i/j add and its k
// product (XMM3; ours after the whole sum, XMM2), and writes blend.y before blend.x.
// Round 13 (lane 17), at 126: the velocity copied x first alone 130 (VC then loads y first and squares it first; the
// y-first copy loads x first, as retail, but into ECX); the short-speed arm written first (`if (0.0001f > ...)`)
// 134; `speed` declared with the top locals 126; the normalizer's y product written `inverse * v->y` 126; `left`
// written y first 126. Retail's normalizers keep the source operand order (x: inverse loaded, y: v->y loaded) where
// ours loads inverse for both.
// Round 253: Complete input capture with normalized-output staging, initial shared normalization and
// full-width reversing truth subsets 0-7: 507/507/576/576/508/508/575/575. All helper and
// target source restored; frame/velocity-copy, early 1.0 residence and later SSE colouring
// remain.
// F320: initial existing fallback normalizer, complete 2D role names and repeated steering-product
// name subsets 349/126/349/126/349/126/349; all with component direction copy 310. The normalizer
// forms grow to 1189/1170 bytes; neutral forms retain 1144. Original body/helper restored; initial
// velocity receivers and 1.0 SSE residence remain.
// M314: Existing squared-length helper with named squares/running sum/native input value/scalar
// inputs 126/144/331/138; each with library velocity copy 130/148/331/142. Complete original
// helper and target restored; early 1.0 residence and velocity-copy receivers remain.
// Round 370: Initial/helper reciprocal lifecycles and complete helper-output staging,
// subsets 1/2/4/3/5/6/7, then all with enclosing speed: 278/126/507/278/502/507/502/502.
// Complete staged graph with existing normalizer forced inline 348; original helper/body restored.
// Velocity receivers, early 1.0 residence and later SSE/x87 colouring remain.
// M329: Complete positive angular-admission channel, bool/byte/short/long, direct
// producer 189/189/188/190; complete if/else 189/189/188/190. Every consumer was
// complemented consistently. All regress; original reversing channel, helpers and body restored.
// M336: Existing 2D normalizer success states, bool/byte/short/long, admission-owned
// or initial-failure-owned: 126/126/126/126/126/126/126/126. Original helper restored;
// velocity-copy receivers, early 1.0 residence and later SIMD/x87 homes remain.
// P287: Complete input-product/angular-projection boundaries, subsets 1/2/4/3/5/6/7,
// then all with steering-time definition observation: 225/126/126/225/225/126/225/297.
// Original body restored; velocity receivers, early 1.0 and x87/SSE ranks remain.
// M338: Test-owned angular offset/gear/signed rate outcomes, subsets 1/2/4/3/5/6/7 and final flag arms:
// 211/195/126/224/211/195/224/234. Original body restored; velocity receivers and early 1.0/SSE/x87 homes
// remain. NOT kept: no gain; non-retail gear-store/length forms restored.
// F353: Existing shared 2D speed/helper magnitudes and paired angular dots, subsets 1-7; all with real
// vector workspaces 225/126/225/126/225/126/225/225.
// NOT kept: no gain; original full body/helpers restored.
// Y560: Complete weighted-forward blend products, row/staged scalar then row/staged
// vector, y/x then x/y publication: 126/126/131/131/210/126/211/131. NOT kept: no byte match;
// original coefficient/products and whole body restored; velocity/unity and SIMD/x87 homes remain.
// F355: Complete perpendicular and heading-blend point initializers, seven subsets, then the complete fallback copy:
// 126/126/126/126/126/126/126/277. NOT kept: no gain; original body restored.
// F358: Existing native then inline 2D normalizers at forward/heading/blend, each seven subsets plus explicit blend length:
// 340/325/344/393/403/388/456/456/126/126/126/126/126/126/126/126. NOT kept: no exact native expansion; original provider calls, fallback graph and full body restored.
// P301: Existing 2D normalizer output/fallback/both reference contracts, original
// and forced inline, then both with bulk fallback copy or float root: 126 each.
// Original helper, callers and complete body restored; velocity-copy receivers,
// early unity SIMD residence and later angular x87/SSE homes remain.
// M378: Complete forward/heading/both memcpy, velocity with forward/heading/both,
// then all and forward/heading memmove: 128/132/134/132/136/138/404/407. NOT kept: all regress;
// original complete scalar copies, helpers and body restored; entry velocity/unity homes remain.
// AC528: Initial speed acceptance/rejection in bool/byte/short/long
// 225/225/225/225/225/225/225/225. NOT kept: no exact match or eligible gain; original complete body restored.
// Velocity-copy receivers, early unity and SIMD/x87 homes remain. No retained candidate.
// P329: Reused speed/heading-weight/angle declarations, six orders then reciprocal scope/role names:
// 126/126/126/126/126/126/126/126. NOT kept: no byte match; original complete body restored.
// Velocity-copy receivers and unity SIMD/x87 homes remain.
// Y591: Scalar initial velocity/direction/paired perpendicular vectors, seven subsets,
// then all with velocity-role names: 153/126/240/233/249/240/249/249. NOT kept: no exact allocation
// repair; original complete vector workspaces, helper contracts and body restored.
// M382: Actual initial/helper x87 magnitude result storage and consistent float-root API, masks
// 1/2/4/3/5/6/7 then all without redundant root casts: 282/507/126/511/282/507/513/513.
// NOT kept: no precision match or early unity/home repair; original helper and complete body restored.
// M384: Actual reverse-angle/absolute-result/steering-angle return storage, real/double
// at producer or entry-declared: 126/126/126/126/126/126/126/126. NOT kept: no equality for
// result materialization; original full body restored; entry velocity/unity and angular homes remain.
// M388: Squared-norm inputs to sqrt, initial/helper real and double carriers:
// 225/128/126/126/225/225/128/309. NOT kept: no gain or precision equality; last form
// grows to 1150/1144. Original helper/body restored; velocity/unity and angular homes remain.
// Round 92 (lane 9), at 126: dead pointer locals 1-8 (diagnostic) 126 each (aligned 74: not numbering; the cause is
// the entry magnitude's schedule, retail adds i*i + j*j before squaring k and loads 1.0 into the freed XMM3); the
// velocity as a vector2f through magnitude2d 225; the input rotation before the forward copy 122; both 120 (under the
// gain bar); a local_velocity pointer for its four reads 126, with the rotation moved 122; on 120: the entry magnitude
// after the velocity copy 122, the local_velocity pointer 122.
// Round 68 (lane 13), at 126: the velocity copied x first with the speed's squares written y then x 130 (VC
// still loads y first, into ECX); the y-first copy with y then x squares 126; dot3f(controls, controls) at both
// magnitude sites 287 (1131 bytes: the two sites CSE); the blend rows as heading + forward * k 126; the blend
// weight's dot written forward x first 126 (identical). The 1.0 constant is held for the whole function (retail
// XMM3, ours XMM2); its register follows the entry schedule (retail adds i*i + j*j before squaring k).
// Round 95 (lane 9), at 126: a tick-length getter for dt 126 (identical); the input magnitude written out at both
// sites in 0xf1cd0's by-name order 321 (1102 bytes: VC CSEs the two sites), at the entry only 128, grouped
// t + (s + k) 313, k + (t + s) 303, (t + k) + s 321.
// P391: Absolute speed/helper magnitude results in real/double at their original roots,
// speed/helper/both then immutable both: 225/129/126/127/225/309/225/309. NOT kept:
// one-use results do not match; paired doubles grow to 1150/1144. Original helper and full
// body restored; entry velocity registers, early unity and later SSE/x87 homes remain.
// P400: Reciprocal arithmetic domains, initial/helper/both double receivers of real
// division 289/507/513; double numerator with real receivers 284/507/530; both double
// divisions/receivers 511; complete component division 126. NOT kept: no exact
// precision or unity-allocation repair; original helper and complete body restored.
// T78: Final retained-magnitude admission at its post-approach producer,
// bool/byte/short/long positive then complementary: 156/156/161/160/156/156/161/160.
// NOT kept: every one-use result regresses and grows to 1153..1157/1144. Original
// complete body restored; entry velocity/unity and later SSE/x87 homes remain.
// Round 101 (m3 lane 9, fast), at 126 (aligned 87, masked 23): the velocity as a planar aggregate copy
// (`velocity = *(point2f const *)&vehicle->local_velocity`) 130; that with the speed's squares written y then x 122
// (the copy is then retail's: x into EAX, y into ECX, x stored and squared first; four points, under the bar, not
// kept). On it: the entry magnitude after the speed 274 (1147 bytes), after the velocity copy 124; the magnitude as
// two statements (i and j squares, then `+=` the third), adjacent 128, around the copy and the speed 278/278 (the
// 1.0 load only moves ahead of both adds). The reversing test as `fabs(atan2(...)) >= 1.9198622f` gives retail's
// `fxch; fcompi; jb` (ours `!(... < c)` is `fcompi; ja`, which also differs from retail for a NaN angle) but is two
// bytes longer until the SSE homes agree: alone 218 at 1145 (masked 20), with the velocity form 214 at 1145
// (aligned 80, masked 20; a score rise, not kept). On that 214 form: the normalize helper's products as `*=` for
// y / for both / y through a named local / y statement first / both value-first / both inverse-first 214, 214,
// 214, 213, 214, 214 (same code: VC picks the product's destination itself); the direction as `velocity / speed`
// 214 (same code). Left: retail holds 1.0 in XMM3 (ours XMM2; it loads between the entry sum's two adds), keeps the
// normalized forward in XMM1/XMM0 where it was computed and multiplies it in place by the blend weight (ours moves
// it to XMM4/XMM7 and multiplies a copy of the weight), and loads each normalize's y into a register ahead of the
// divide.
// Round 180 (m3 lane 4, fast; aligned/masked = difflib counts with addresses / also stack offsets masked, 74/61 at
// 126; only 23 instructions differ once registers and offsets are masked too): the planar velocity copy with the
// speed's squares y first re-measured 122 (aligned 70, masked 57); with heading as a planar copy of the local
// velocity 128, forward as a planar copy of the vehicle's forward 124, both 130; with the reversing test as
// `fabs(...) >= 1.9198622f` 214 (1145 bytes, masked 55), that with the forward copy 222. Left: the entry magnitude's
// add order (retail adds i*i + j*j before squaring k, so 1.0 lands in XMM3), each normalize's y loaded ahead of the
// divide, and the blend rows stored y first. Also: both perpendicular x terms as a unary minus 126 (same code).
// Round 184 (m3 lane 4, fast; aligned/masked as difflib counts with addresses / also registers masked, 74/26 at 126):
// the controls read through a `vector3f controls` member over throttle/steering/unknown1b8 (no cast at either
// length_sq3f site) 126 (same code); re-measured: the planar velocity copy with y-first squares 122. A compile-only
// screen of vehicles.cpp (/O2 /Ob1 /arch:SSE /Gr, no /GL) reproduces our ef070 exactly but the entry convention, so 84
// statement-order/planar/compare/blend combinations ran there first: the x87 operand order of the angle's atan2 sums
// follows the order in which input and forward are first written, not the products' spelling, and retail's order
// comes from input.x, forward.x, input.y, forward.y written interleaved: in the build 120 (masked 22), with the planar
// copy 116 (both statement orders picked by score, not kept); that with `fabs(...) >= 1.9198622f` and blend.x
// written first 216 at 1145 bytes (aligned 59, masked 15: the best structure, LEAD); blend.x first alone 126
// (aligned 69), with the planar copy 122 (aligned 65); the interleave, planar copy and blend.x first 116 (aligned 61,
// masked 17). The normalizer's `*=`/operand spellings, the input products' operand order and the entry magnitude's
// placement give identical code in the screen. Left on the lead: the entry magnitude's schedule (retail adds i*i +
// j*j before squaring k, so 1.0 lands in XMM3), input.y's products (the interleave reorders them), each normalize's y
// loaded ahead of the divide, and the reversing atan2's first product (retail loads input.y first).
// Then on the interleave + planar copy (+ blend.x first, e6 116; + `>=` compare, e3 216): heading as a planar copy on
// e3 222 (1145 bytes); the reversing sum written from blend directly (`input.y * blend.x - blend.y * input.x`) with
// `>=` on e6 216 (1144 bytes, aligned 64); blend_left.y written first on e6 116 (same code); the entry magnitude
// after the direction block on e6 154; left computed just before the angle on e3 237 (1132 bytes).
// Round 154 (m3 lane 8, fast; compile-only screen of vehicles.cpp for the rest, positional/aligned/register-masked,
// base 117/75/14): the throttle and steering read once into named locals for the input rotation 277 (1136 bytes;
// screen aligned 85, masked 20); same code in the screen: the three normalizes as the existing normalize2d_fast with
// the caller's `== 0.0f` fallback test (0x1b0cd0's idiom with the same g_468778 default), dt passed straight to
// vehicle_approach. The entry magnitude written out changes only the order of its loads (screen aligned 77). The
// masked residue is still the entry schedule (1.0 in xmm2), each normalize's y loaded ahead of the divide in retail,
// and the reversing compare (retail's `>=` NaN sense). Later the same round: round 184's e6 (interleave, planar copy
// with y-first squares, blend.x first) with the normalizer's y product written first 115 (aligned 61, masked 13; a
// lead made of statement orders, not kept), with the `>=` reversing test too 215 (1145 bytes, masked 11). A byte
// walk of that 1145-byte form (reduced image against retail) puts the extra byte in the heading normalize (ours
// copies the inverse into another register; retail loads y first), not in an addressing fold. Compile-only: the
// normalizer reading x and y once into locals changes the whole function (936 bytes).
// M3 lane 1 r100 (fast, 0xef4f0 still matched): 1..8 unused long locals at entry 126 each (same code); the normalizer with
// separate source and destination pointers 126 (same code), with its y row written before its x row 125 (the forward rows swap
// registers, nothing else), both together 125; its components through a real array 126 (same code); both rows value-first 126,
// the length as x*x + y*y 126, both rows as divisions by the length 126 (VC makes the reciprocal itself), the y component read
// into a local ahead of the reciprocal 126 (same code each). Reading: integer scratch registers are handed out in turn (EAX,
// ECX, EDX skipping busy ones); retail's velocity copy is one step off ours and agrees again at the forward copy, which is the
// entry magnitude's pointer evaluated after the copy (the 124 order above) rather than before it. On the SSE side retail colours
// the vector components first (forward and blend in XMM0/XMM1), then the two-use scalars (reciprocal and blend weight XMM2,
// 1.0 XMM3); ours colours the scalars first (reciprocal XMM0, 1.0 XMM2, forward XMM4/XMM7, heading XMM3/XMM5). The 1.0 load's
// place, each normalize's y loaded ahead of the divide and `forward * k` against `k * forward` all follow from that order.
// @retail 0xef070
void function_ef070(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	point2f velocity;
	point2f direction;
	point2f forward;
	point2f heading;
	point2f input;

	real input_magnitude_squared = length_sq3f((vector3f const *)&vehicle->throttle);

	velocity.y = vehicle->local_velocity.j;
	velocity.x = vehicle->local_velocity.i;

	real speed = (real)sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

	if (!(0.0001f > (real)fabs(speed)))
	{
		real inverse = 1.0f / speed;

		direction.x = inverse * velocity.x;
		direction.y = inverse * velocity.y;
	}
	else
	{
		direction.y = velocity.y;
		direction.x = velocity.x;
	}
	forward.x = vehicle->forward.i;
	forward.y = vehicle->forward.j;
	input.x = vehicle->throttle * direction.x - vehicle->steering * direction.y;
	input.y = vehicle->steering * direction.x + vehicle->throttle * direction.y;
	vehicle_normalize2d(&forward, g_468778);

	point2f left;

	left.x = 0.0f - forward.y;
	left.y = forward.x;

	heading.x = vehicle->local_velocity.i;
	heading.y = vehicle->local_velocity.j;
	vehicle_normalize2d(&heading, g_468778);

	real k = 1.0f - (heading.y * forward.y + heading.x * forward.x);
	point2f blend;

	blend.y = forward.y * k + heading.y;
	blend.x = forward.x * k + heading.x;

	vehicle_normalize2d(&blend, &forward);

	point2f blend_left;

	blend_left.x = 0.0f - blend.y;
	blend_left.y = blend.x;

	bool reversing = !((real)fabs(atan2(input.y * blend_left.y + blend_left.x * input.x,
		blend.y * input.y + blend.x * input.x)) < 1.9198622f);
	real angle = (real)atan2(input.y * left.y + left.x * input.x, forward.y * input.y + forward.x * input.x);
	real offset;

	if (reversing)
		offset = angle > 0.0f ? -3.1415927f : 3.1415927f;
	else
		offset = 0.0f;
	angle = offset + angle;
	vehicle->gear = length_sq3f((vector3f const *)&vehicle->throttle) != 0.0f ? !reversing : vehicle->gear;

	real steer = PIN(*(real *)(definition + 0x220) * angle, -0.78539819f, 0.78539819f) * 1.2732395f;
	real const *rates = (real const *)(definition + 0x22c);
	real bound = (0.0f > steer ? rates[1] : rates[0]) * steer;
	real dt = g_510c54->rate;

	vehicle_approach(&vehicle->turn, rates, bound, dt);
	word_flags_set((word *)&vehicle->flags348, 9, reversing && input_magnitude_squared > 0.09f);
}

/* the vehicle tuning values (0x4678b8..0x4678d8) */
real g_4678b8 = 1.5f;
real g_4678bc = 1.5f;
real g_4678c0 = 1.5f;
real g_4678c4 = 1.5f;
real g_4678c8 = 0.5f;
real g_4678cc = 7.5f;
real g_4678d0 = 0.1f;
real g_4678d4 = 0.4f;
real g_4678d8 = 0.4f;

/* a pointer to a default vector (0x4687b8) */
vector3f *g_4687b8;

/* moves a value toward a bound by the definition's rates, without
   passing it */
PRIVATE inline void vehicle_approach(real *value, real const *rates, real bound, real dt)
{
	if (*value > bound)
	{
		function_2053c0(value, rates, -1.0f, dt);
		if (bound >= *value)
			*value = bound;
	}
	else if (bound > *value)
	{
		function_2053c0(value, rates, 1.0f, dt);
		if (*value >= bound)
			*value = bound;
	}
}

/* a vehicle's control: its drift and boost flags, braking, the steering of
   its control type, and its root node's movement */
// @retail 0xef4f0
void __stdcall function_ef4f0(long vehicle_index, real *steering)
{
	s_vehicle_header *header = VEHICLE_HEADER_GET(vehicle_index);
	s_vehicle *vehicle = header->vehicle;
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool special = function_f42f0(vehicle_index);
	bool braking = special || (vehicle->control_flags & 0x800);

	if (*(short *)(definition + 0x1f0) == 5)
	{
		real throttle = PIN(vehicle->throttle, 0.0f, g_4678d4);

		if (throttle == vehicle->throttle)
			vehicle->throttle = g_4678d4;
	}
	*steering = 0.0f;
	word_flags_set((word *)&vehicle->flags348, 3, (vehicle->control_flags & 2) ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit4) &&
		(vehicle->throttle > 0.0f && 0.0f > vehicle->speed || 0.0f > vehicle->throttle && vehicle->speed > 0.0f));
	if (TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit17))
		word_flags_set((word *)&vehicle->flags348, 5,
			TEST_FIELD_BIT(((s_vehicle_flag_bits const *)&vehicle->control_flags)->bit11));
	*(word *)&vehicle->flags348 &= ~FLAG(4);
	if (TEST_FIELD_BIT(((s_vehicle_flag_bits const *)(definition + 0xbc))->bit28) &&
		(braking || vehicle->unknown334 > 0.0f))
	{
		word_flags_set((word *)&vehicle->flags348, 4, vehicle->unknown334 > vehicle->unknown338);
		if (braking)
		{
			vector3f *input = (vector3f *)&vehicle->throttle;

			*input = *g_4687a8;
			if (special)
			{
				if (vehicle->unknown350 == 1)
				{
					real rate;

					*input = *g_4687ac;
					if (*(real *)(definition + 0x22c) >= 0.0f)
						rate = *(real *)(definition + 0x22c);
					else
						rate = 0.0f - *(real *)(definition + 0x22c);
					vehicle->turn = rate;
				}
				else if (vehicle->unknown350 == 2)
				{
					real rate;

					*input = *g_4687b8;
					if (*(real *)(definition + 0x230) >= 0.0f)
						rate = *(real *)(definition + 0x230);
					else
						rate = 0.0f - *(real *)(definition + 0x230);
					vehicle->turn = 0.0f - rate;
				}
			}
		}
	}

	switch (*(short *)(definition + 0x1f2))
	{
	case 0:
		function_eec90(vehicle_index, steering);
		break;
	case 2:
		function_ef070(vehicle_index);
		break;
	}

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit7))
	{
		s_vehicle *current = VEHICLE_GET(vehicle_index);
		point3f const *root = (point3f const *)((byte *)current + *(short *)((byte *)current + 0x116) + 0x28);
		real dt = g_510c54->rate;
		real root_input = vehicle->unknown3e0;
		real const *rates = (real const *)(definition + 0x1f4);
		real bound = (0.0f > root_input ? rates[1] : rates[0]) *
			root_input;

		vehicle_approach(&vehicle->unknown3dc, rates, bound, dt);
		vector3d_from_points3d(&vehicle->unknown3e4, root, &vehicle->unknown3f0);
		vehicle->unknown3e4 = *root;
	}
}

void function_ba1d0(long object_index, vector3f *linear_velocity, vector3f *angular_velocity);
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity);

/* a float rounded to an integer (fld, fistp) */
__forceinline long vehicle_round(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* flips a vehicle back over: a spin about its forward (or its side) for up
   to two seconds while it's upside down */
// MiniMax I (11-14): the side cross through the shared staged cross helper (cross3f_staged) 6 (was 8);
// cross3f_staged at the first cross as well 10; the two count variables, the compound sum and the
// swapped operands of 0xf6060's marker sum all leave that sum in EAX.
// score 6 (was 8): retail's length and tail now; three products load the other operand (ours loads up.j
// for the first cross's k term and the side cross's i term, and up.i for the side cross's j term;
// retail loads forward each time). Kept: one `bool result` exit, the
// for the first cross's k term and the side cross's i term, up.i for the side cross's j term, and the
// velocity for the dot's i term; retail loads forward each time). Kept: one `bool result` exit, the
// failure arm first (`if (!(...)) { clears } else { ...; result = true; }`) and word_flags_set for the
// flag clear (the clear stays a fused `and byte` last, as retail).
// Round 130: result + else-arm stores with `flags348 &= ~0x40` 30 (759 B, clear split), with a word
// clear or a bit-field clear 15 (clear moves first), SET_FLAG 15, word_flags_set/byte_flags_set 12/12
// (tail exact, six products flip), both helpers with the two returns 12/12 (753 B), the failure arm
// first 8 (kept); on it: an up pointer local 94, a named dot result 10, `-(up.k / rate * 2)` 159,
// De Morgan tests 11, byte_flags_set 8, result declared first 178. A compile-only screen (products
// match the full build) found none under four flips in 576 combinations of declaration orders,
// result placements, cross3f_staged for either cross, the up pointer, named dot and rate forms.
// Before round 130 (score 12): three products with up.j keep it in a register (retail folds it as the
// memory operand: the first cross's k term and the side cross's i and k terms), and the fail path
// re-zeroes AL before returning false (retail returns the entry's zero), two bytes over retail.
// Double literals 98; shared crosses 140; both 79 (kept); angular/both scales 83/83;
// scaled sum 83; single result/vector names 79/79; seven scalar name/scope forms 79;
// coefficient at use 93, with shared sum 95.
// Round 90: `down` computed after the side cross 93 (rate then stays in xmm7, no spill); dot terms
// i,k,j 80, k,j,i 79; dot3f for the dot 79; dot3f inside vector3f_scale for the projection 79.
// Round 77: `strength *= rate` 76 (strength in the rounding value's slot, as retail); with `down`
// after the cross 93, with dot3f in vector3f_scale 76; side-block product/sum operand orders, statement
// orders, cross3f_staged, vector3f_add_scaled / vector3f_scale + vector3f_add with a `down` local and
// every placement of the side/down declarations 76 (down assigned before the cross) or 91 (after);
// vector3f_add_scaled with `0.0f - forward.k` as its argument 16; the angular velocity through
// vector3f_scale 12 (all kept); then cross3f_staged for either cross 16/18/25; `rate *= strength` 20;
// the hand projection 20; a result per arm 30, an else arm 12, chained zero stores 14, the flag
// cleared first 27; the condition in a bool returned at one/two exits 32/20.
// Round 31: one `bool result` exit (false, success sets true, the stores in an else) declared
// first 188, after the lookups 30 (no re-zero, but the flag clear splits into load/and/store), between
// them 62, set before the 34e increment 30; with the flag cleared first 15 (751/751, exit as retail but
// the flag store first). A compile-only screen reproduces the product operands exactly: every result
// form flips three more products; the helper's component order (all six, staged or not), operand text
// order, inline/forceinline, local declaration order, the if/else inverted and the rate computed first
// never give the first cross's f.i load, which moves only when the side block is removed.
// Round 425: native rate clamp 12; strength if/else 140; writable flag bit 15; shared byte clear 12;
// clamp/strength 140, clamp/bit 15, strength/bit and all 140. All original source restored.
// Round 48: shared up at both crosses 141; shared forward 204; both 209; forward-default axis 169;
// angular copy/in-place scale 266/91; full forward snapshot 226; up snapshot with rate 146;
// branch-scoped up at the first/side cross 150/109. Original body restored.
// Round 132: the product choice follows the order VC first meets each field in dominating code (an
// earlier read of one operand makes it the memory operand), so it moves with the condition and the
// rounding (with the rounding out of line 3 flips, with a cast in its place 5). Five real trials,
// all on the kept form: linear/angular/axis declared before strength with the side cross through
// cross3f_staged 6; that with `result = true` before the increment 6; axis/angular/linear before
// strength, staged side cross and De Morgan tests 9; linear/axis/angular before strength, staged side
// cross, De Morgan tests and the axis if/else inverted 9; angular/linear/axis after strength,
// `result = true` first and the staged side cross 6. A compile-only screen of 1500 random combinations
// (declaration orders, flag helper, result placement, staged crosses, side scale+add, De Morgan,
// inverted axis branch, side and rate declared early, named dot) found none under three flips; the
// condition's spellings (casts, operand sides, `!= 0`) and local names give identical code.
// Lane 13 round 28: dot3f(&velocity, &forward) 8 (same code); an up pointer used only at the first cross 4 (first
// cross and the dot's i term then retail's); with cross3f_staged for the side cross 2 (only the side cross's
// `a->k * b->j` still loads up.j; a one-use pointer, so kept only on a match); cross3f_staged for the side alone 6;
// the up pointer at the first cross and the rate with the staged side 4 (the side's i/j pairs flip instead; the
// side cross still reads vehicle->up). Compile-only screens: the pointer at both crosses 5 flips, a forward
// pointer at any use 2-15, 256 helper/pointer combinations and 400 random ones (scopes, scale forms, named
// dot, strength if/else, `rate *= strength`, declaration orders) at best 1 flip; the pointer's name changes nothing.
// J220: complete mode byte/short/long 172/158/158; result byte/short/long 174/180/178;
// angular output as axis workspace 30, with byte mode 174. Post-reader mode
// byte/staged-side/short/long 10/19/38/31 (partial observations, exact-only). One common cross
// direct/staged before rate clamp 194/194, after clamp 223/223.
// Round 80: side cross from the axis copy 113 (731 bytes); scale coefficient from axis.k 51;
// the two-flip form with axis.k 57, with the side cross from axis 116 (730), with both 124 (728);
// projection through axis then copied back 73 (775), on the two-flip form 72 (775), with the
// velocity call selecting its output 157 (795). Restored.
// Round 134 (lane 6), at 8: a pointer-returning cross helper at both crosses 8; the file's own vehicle_cross at both
// crosses 8; the positive test with the clears in an else 12; a two-level nested test with the clears under
// `if (!result)` 12; the pointer-returning cross with the angular scale after the projection 33. Compile-only screen
// (exact for these products): the pointer-returning or file cross at either cross alone, the angular scale after the
// projection, `v * s` scale helpers and `up.k <= 0.9f` keep all four flips.
// Round 92 (lane 9), at 8: dead pointer locals (diagnostic) only change which forward products flip (1-8 give
// 10/10/12/15/15/6/8/8, never 0). Grid of an up pointer at the first cross (U), cross3f_staged for the side cross (S),
// a named dt (D), a named dot (N) and a forward pointer used throughout (F), 32 combinations: S 6, U+S 6, U+N 6, U 8,
// N 10, D 14, S+N 19, S+D 23, every form with F 164; with dead locals on S/U+S/U+N the best is U+N 4 (left: the side
// cross's forward.k * up.j and the dot's i term). All are one-use forms, kept only on a match.
// Round 78 (lane 10, fast): the vehicle through the existing inline getter `(s_vehicle *)havok_object_get(...)`
// 8 (same code); an up pointer for the condition, the rate and both crosses 98 (735 bytes); the up pointer
// after the velocity call at both crosses 96, with the rate too 96 (735 bytes each); cross3f_staged for the
// first cross alone 10.
// Round 95 (lane 9), at 8: 9-16 unused locals at the top 12/10/12/15/15/6/8/8 (period 8, never 0); the crosses
// and the dot written out (diagnostic) 24/83/93/94, the dot alone 10; vehicle_cross at the side cross 8;
// vehicle_cross first with cross3f_staged at the side 6; cross3f_staged first with vehicle_cross at the side 10;
// cross3f_staged at both 10; the strength test as `mode == 4 || mode == 2` 10; a tick-length getter for the rate 12.
// Round 84 (lane 10, fast), at 8: an up pointer at the first cross with cross3f_staged at the side cross 6 (current
// body; one-use pointer, needs a match); axis set to forward before the mode test with the cross overwriting it 173;
// the side scale named before the side cross 82; the three vectors declared at function scope 11; the velocity call
// before the strength value 146; cross3f_staged at the side with the dot as dot3f(&linear_velocity, &forward) 8.
// m3 lane 1 r95 (fast), at 8: re-measured lane 10 r84's up pointer at the first cross with cross3f_staged at the side 6
// (the first cross's k row and the dot's i term become retail's, the first cross's two up.i products flip to load forward,
// and the side cross's forward.k * up.j still loads up.j; lane 13 r28 reported 2 for a like form), lane 10 r78's pointer
// at both crosses 96 (735 B) and round 92's plain-side pointer form 8. New: the pointer declared before the mode test with
// the staged side 6 (same three flips); that with the dot as dot3f(&linear_velocity, &vehicle->forward) 8 (the dot's i term
// flips back; with the plain side cross it flips back too, so the staged side cross sets it); an up pointer declared with
// the lookups and used at the condition and the first cross only 10; a forward pointer for the dot and the projection
// scale only 50; the axis if/else inverted (`!= 4 && != 3` with the copy first) 8 (same code).
// Round 192 (m3 lane 4, Fable then Opus): MATCH (8 -> 0, plain and full check) with three new one-caller
// PRIVATE __forceinline helpers (vehicle_flip_strength, vehicle_flip_rate, vehicle_forward_speed: each declares
// its own local, which moves retail's product operand loads). NOT kept (coordinator review 11-13): invented
// one-caller helpers are reverted even with a match. Lead: the locals those helpers declare set the operand
// order; look for an existing helper or inline (the vector math or vehicle headers) that would declare them.
// P443: Entry-bit result, coefficient/axis/correction predicates, bounds receiver, projection predicate,
// double-factor angular scale and vector-setter axis copy: 8/18/42/14/10/10/138/199. A 300 s permuter, 36 trials,
// no lower than 8. NOT kept: no exact operand repair; original body and all shared helpers restored.
// P445: Const definition receiver, separate final angular coefficient, exhaustive bounded-rate result,
// consumed scale return, widened completed dot transport, angular-output side workspace, duration and
// elapsed-tick producers: 8/14/10/10/87/8/191/26. NOT kept: no exact product-load repair; whole body restored.
// P461: Success-default outcome bool/byte/short/long 8/201/206/204; complementary
// rejection outcome bool/byte/short/long 210/210/215/213. NOT kept: no exact
// product-load repair; original failure-first result graph and complete body restored.
// MiniMax A (11-13): no new form tried this round, no lower score (8): all four differences are still the
// mulss operand order - this build loads the [edi+..] operand into the destination register first in both cross
// products and in both speed products, retail loads the other one first; retail's own two crosses use opposite
// orders, so the order is register pressure and not the source, and round 192's match needed three invented
// one-caller helpers that the coordinator rejected; the lead is an existing helper or inline that declares the
// same locals.
// @retail 0xef870
bool function_ef870(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool result = false;

	if (!(TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit6) && vehicle->unknown34d &&
		(long)vehicle->unknown34e < vehicle_round((real)g_510c54->field_2_3 * 2.0f) &&
		0.9f >= vehicle->up.k))
	{
		vehicle->unknown34e = 0;
		vehicle->unknown34d = 0;
		word_flags_set((word *)&vehicle->flags348, 6, false);
	}
	else
	{
		real strength = vehicle->unknown34d == 2 || vehicle->unknown34d == 4 ? 0.3 : -0.3;
		vector3f linear_velocity;
		vector3f angular_velocity;
		vector3f axis;

		function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
		if (vehicle->unknown34d == 4 || vehicle->unknown34d == 3)
		{
			cross3f(&vehicle->forward, &vehicle->up, &axis);
		}
		else
		{
			axis = vehicle->forward;
		}

		real rate = vehicle->up.k / g_510c54->rate * -2.0f;

		if (*(real *)(definition + 0x23c) > rate)
			rate = *(real *)(definition + 0x23c);
		else if (rate > *(real *)(definition + 0x240))
			rate = *(real *)(definition + 0x240);
		strength *= rate;
		if (vehicle->unknown34d == 2 || vehicle->unknown34d == 1)
		{
			vector3f side;

			cross3f_staged(&vehicle->forward, &vehicle->up, &side);
			vector3f_add_scaled(&axis, &side, 0.0f - vehicle->forward.k, &axis);
		}
		vector3f_scale(&axis, strength, &angular_velocity);
		if (*(short *)(definition + 0x1f0) == 0)
		{
			vector3f_scale(&vehicle->forward, dot3f(&vehicle->forward, &linear_velocity), &linear_velocity);
		}
		function_b77d0(vehicle_index, &linear_velocity, &angular_velocity);
		vehicle->unknown34e++;
		result = true;
	}
	return result;
}


/* whether a vehicle's controls are doing anything its definition reacts
   to */
// @retail 0xefb60
bool function_efb60(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);

	return TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit0) && (vehicle->speed != 0.0f || vehicle->gear != NONE) ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit1) && vehicle->steering_angle != 0.0f ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit2) && vehicle->unknown25c != 0.0f ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit3) && vehicle->unknown260 != 0.0f ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit5) && vehicle->turn != 0.0f;
}

/* a control value decayed toward zero, or zeroed when it's small */
PRIVATE inline void vehicle_decay(real *value, real scale)
{
	real magnitude = *value >= 0.0f ? *value : 0.0f - *value;

	if (0.05f > magnitude)
		*value = 0.0f;
	else
		*value = *value * scale;
}

/* lets a vehicle's controls go: each decays toward zero */
// @retail 0xefc30
void function_efc30(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	real scale = g_510c54->rate * 10.0f;

	/* the gear through its own lookup: retail stores it ahead of the flag */
	VEHICLE_GET(vehicle_index)->gear = NONE;
	((s_vehicle_word_flag_bits *)&vehicle->flags348)->bit5 = false;
	vehicle_decay(&vehicle->unknown378, scale);
	vehicle_decay(&vehicle->speed, scale);
	vehicle_decay(&vehicle->steering_angle, scale);
	vehicle_decay(&vehicle->turn, scale);
	vehicle_decay(&vehicle->unknown370, scale);
	vehicle_decay(&vehicle->unknown374, scale);
}

/* the physics state a vehicle's update fills and its vehicle type's
   physics function updates (0x1890 bytes on the stack) */
struct s_vehicle_physics_point
{
	byte unknown00[0x80];
	bool unknown80;
	byte unknown81[0x9c - 0x81];
	real unknown9c;
	real unknowna0;
	real value;
};

/* a wheel or other contact of the physics state (0xd8 bytes) */
struct s_vehicle_contact
{
	transform4x3f matrix;
	byte unknown34[0x70 - 0x34];
	byte contact_type;
	byte unknown71[3];
	vector3f contact_velocity;
	bool unknown80;
	byte unknown81[2];
	bool unknown83;
	real compression;
	byte unknown88[4];
	real torque;
	byte unknown90[0xa0 - 0x90];
	vector3f contact_normal;
	long contact_handle;
	short unknownb0;
	byte unknownb2[0xd8 - 0xb2];
};

/* a contact of the physics definition (0x4c bytes) */
struct s_vehicle_contact_definition
{
	byte unknown00[4];
	dword flags;
	byte unknown08[4];
	real distance;
	byte unknown10[0x4c - 0x10];
};

/* the vehicle's Havok physics definition as its state points to it */
struct s_vehicle_physics_definition
{
	byte unknown00[0x3c];
	long point_count;
	byte unknown40[4];
	long contact_count;
	s_vehicle_contact_definition *contacts;
};

struct s_vehicle_physics_state
{
	byte unknown000[4];
	s_vehicle_physics_definition *definition;
	transform4x3f matrix;
	real unknown3c;
	byte unknown040[0x4c - 0x40];
	real unknown4c;
	byte unknown050[0x5c - 0x50];
	real unknown5c;
	byte unknown060[0x6c - 0x60];
	real unknown6c;
	real unknown70;
	byte unknown074[0x8c - 0x74];
	real unknown8c;
	s_vehicle_physics_point points[16];
	s_vehicle_contact contacts[16];
};

struct s_vehicle_contact_result
{
	bool valid;
	byte unknown01[3];
	real distance;
	short material;
	byte unknown0a[0x18 - 0xa];
	vector3f normal;
	byte unknown24[8];
	byte type;
	byte unknown2d[3];
	vector3f velocity;
};

extern short g_54e898;

PRIVATE __forceinline void reset_vehicle_contact(s_vehicle_contact *contact)
{
	contact->contact_normal = *g_4687b0;
	contact->unknownb0 = g_54e898;
	contact->compression = 0.0f;
}

// @retail 0x207ab0
void function_207ab0(long index, s_vehicle_physics_state *state, s_vehicle_contact_result const *result)
{
	s_vehicle_contact *contact = &state->contacts[index];
	s_vehicle_contact_definition *definition = &state->definition->contacts[index];
	reset_vehicle_contact(contact);
	if (result->valid)
	{
		contact->unknownb0 = result->material;
		contact->contact_normal = result->normal;
		contact->compression = PIN(definition->distance - result->distance, 0.001f, definition->distance);
		contact->contact_handle = NONE;
		contact->contact_type = result->type;
		contact->contact_velocity = result->velocity;
	}
}

struct s_type_1e6529
{
	long definition_index;
	byte unknown04[0x7c - 0x4];
	short material_index;
	byte unknown7e[0x88 - 0x7e];
};

bool function_f1320(long vehicle_index);
void __stdcall function_1d24a0(s_havok_component *component, float position);
void function_1d2460(s_havok_component *component);
bool function_205510(s_havok_component *component, void const *definition_physics, void *buffer);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_1cfb90(s_havok_component *component, transform4x3f const *matrix, void const *buffer);
bool __stdcall function_f6670(long vehicle_index, bool update);
void function_ba3d0(long object_index);
s_vehicle_physics_state *function_f06c0(s_vehicle_physics_state *state);
bool __stdcall function_205be0(s_vehicle_physics_state *state, long vehicle_index);
void __stdcall function_f1cd0(long vehicle_index, s_vehicle_physics_state *state);
void __stdcall function_f1f80(long vehicle_index, s_vehicle_physics_state *state);
void __stdcall function_f2360(long vehicle_index, s_vehicle_physics_state *state);
void __stdcall function_f3010(long vehicle_index, s_vehicle_physics_state *state, real steering);
void __stdcall function_f4800(long vehicle_index, s_vehicle_physics_state *state);
void __stdcall function_2056e0(long vehicle_index, s_vehicle_physics_state *state, real braking,
	vector3f const *force, vector3f const *torque);
void function_f6010(long vehicle_index);
void __stdcall function_f6bd0(long vehicle_index, s_vehicle_physics_state *state);
bool function_cc380(long object_index);
void function_d6660(s_type_1e6529 *data, long definition_index);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
void __stdcall function_ba7f0(long object_index, long a, long b, long mask);
bool function_113e40(long unit_index);
void function_bba20(long object_index);
bool __stdcall function_e6830(long unit_index);
bool __stdcall function_111650(long unit_index, long *names);
long function_10f5f0(long object_index);

/* the vehicle object type's update: its Havok bodies, controls, its
   vehicle type's physics, its kill volume, boost and lights, and the unit
   update */
// MiniMax B (11-13): no new form, no lower score (561): our frame is 0x1990 where retail's is 0x1984, so every stack
// reference sits a dword higher, and the point-sum loop's unrolling and the flag stores keep retail's different shape; this
// needs a rewrite of the body's frame, not a tweak.
// MiniMax G (11-12): no new form, no lower score (561): the lit flags read as a word once with the byte read again in the false
// arm (retail's shape) gives 576 at the same length, so the flag reads stay as they are.
// MiniMax D (11-11): no new form, no lower score (561): the damage record cut to retail's 0x80 bytes (the frame 0x1990 to 0x1988) and the buffer and the names moved to the function's top scope change nothing, so the frame is still four bytes above retail's 0x1984 and every later slot shifts; the word at +0xc0 is still read into EAX where retail reads it into ECX.
// MiniMax D (11-10): no new form, no lower score (561): the frame is 0x1990 where retail's is 0x1984, so
// the physics state's layout is twelve bytes too big here and every later slot shifts with it; the word
// at +0xc0 is read into EAX where retail reads it into ECX.
// score 561 (was 561): earlier 562; frame 0x1990 versus retail 0x1984, native flag-result byte stores (the six dead [esp+0x13]
// earlier at 561 (was 562). Frame 0x1990 versus retail 0x1984, native flag-result byte stores (the six dead [esp+0x13]
// stores), sum-loop unrolling, the flags348 write/read forms and scalar/SSE homes remain.
// Statement walk (b5L, 2026-11-06, retail 0xefde0-0xf0699, block by block against disasm and align.py): every field
// offset, condition direction, callee/argument order, switch case, constant (0.3 at 0x45e610, 6.0, 1.0, 0.0) and
// float operation order agrees, except the child damage record: retail stores the word at +0x7c (0xffff) BEFORE
// 0xd6660 (0xf03f1, call 0xf03fb), and 0xd6660 itself overwrites that word with g_47d8e0 (0xd6681), so it is a dead
// store; ours stored it after the call, which left it NONE (fixed, as 0xdbfb0 does it). The 0x205510 buffer is six
// reals (the callee copies real values[6]; retail's home is 0x18 bytes): `real buffer[6]` replaces `byte[0x28]`
// (frame 0x19ac -> 0x1990, same score). SET_FLAG for the +0x134 bit 23 and MAX for the open/powered select compile
// the same as the written-out forms (561). Retail re-reads flags348 bit 5 after 0xba7f0 (0xf057a); ours keeps the
// value from before the call (a fresh read there 570); 0xba7f0 does not obviously write it. Tried: MIN/MAX for the
// boost clamps 576 (2252 bytes); the file's bit views (TEST_FIELD_BIT of VEHICLE_WORD_BITS/DEFINITION_FLAGS) for
// the definition flag 575, flags348 bit 2 571, bit 5 576, flags_c0 bits 12/10 580/575 (dword view for flags_134 561);
// bit-view writes of flags348 bits 0/1 571 (word read-modify-write; retail uses byte or/and); a named
// `fast != lit` bool 561. Retail evaluates `fast != lit` with setne/test (0xf051b).
// MiniMax R (11-06): forms tried, none lower (562; buffer 0x30/0x28/0x18 gives frames 0x19b4/0x19ac/0x1990, a function-scope buffer and split velocity blocks change nothing); retail's 0x18-byte buffer and the velocity share one home at esp+0x20, ours sits apart at esp+0x48.
// Earlier score 565 (was 565): Frame 0x19ac versus retail 0x1984, native flag-result byte stores, sum-loop unrolling
// and scalar/SSE homes remain.
// Earlier at 597 (was 597): frame 0x19ac versus retail's 0x1984; flag and scalar homes differ. 715 before round 114.
// Round 114 (lane 6): 715 -> 639 with no change here, from 0x2056e0's cap helper no longer naming a
// double (LTCG had pushed its 8-byte alignment need onto 0x2056e0's callers).
// Physics-state branch scope, separate velocity outputs and both 715 each; final default-array
// initialization alone or with either/both workspace changes 723 each; all with native flag tests 753.
// Original scopes, scalar observations, default stores and existing local qualifier retained.
// Round 426: names/damage/buffer lexical-home subsets and all with the matrix renamed 715 in all eight forms; original body restored, aligned frame unchanged.
// Round 45 (lane 1): the idle test through the existing 0x10f5f0 instead of its body written out 597 (kept; LTCG
// now calls 0x10f5f0 and inlines 0x113e40, retail the reverse; a forceinline declaration 597, ignored). The six
// dead [esp+0x13] byte stores are an inlined helper's bool result kept in memory: bipeds' field_bit_test 673 (no
// stores); stores appear with an `__assume` in the function (`__assume(vehicle != NULL)` 666, type switch
// `default: __assume(0)` 672, which drops retail's range check) or a two-definition result (`if (bit < 32)`
// helper 667): no natural form yet. The 0x205510 buffer is six reals: `real buffer[6]` gives frame 0x1990 (639);
// with linear_velocity per block 639/597; the compile-only probe then gives retail's exact 0x1984 frame once
// names also has its own block, but the real build keeps 0x1990 (LTCG groups the 12-byte velocity first, so the
// buffer sits apart and the kill-volume scalars start 12 bytes low). names `= { 0 }` 629, memset 634, both in a
// block 629.
// Round 469: Sixteen-point short/word/signed-byte/byte ordinals with three native axis bounds
// 612 each; those four ordinals with the original buffer 612 each (2220/2264 bytes).
// Frame 0x1990 still exceeds 0x1984; original complete body and local qualifier restored.
// J222: Native position field 597; existing offset-vector producer and field/producer 595/595; complete
// position observation alone and with field/producer/both 637 each; best field/producer with actual six-real
// buffer 595. Two-point forms rejected; every original field view, buffer, body and inherited qualifier
// restored. No calibrated loss.
// M283: Ascending point cursor with indexed/postincrement/countdown/pointer-distance bounds 588/588/616/578;
// with six-real output 588/588/616/578. Keep the indexed cursor, plain full confirmed 597 -> 588;
// pointer-distance bounds introduce signed division per iteration, absent in retail, so are diagnostic only. Original buffer retained; frame 0x19ac/0x1984 and flag-result stores remain.
// Round 251: Two-vector buffer, velocity name, state name, buffer/velocity, buffer/state, both names,
// buffer/both names and six-real buffer/both names: 588 each. Original complete body and inherited qualifier
// restored; frame and native flag-result stores remain.
// Round 236: Native owning-word bit readers, initial/active/physics and pair subsets
// 600/595/612/600/644/597; full readers with actual six-real buffer 644
// (frame 0x1990); final Boolean result completion 595. All original record layout,
// buffer, observations, qualifiers and complete body restored; frame 0x19ac versus
// 0x1984, native flag-result stores and scalar/SSE homes remain.
// M309: Actual returned matrix input, original/returned physics-state receiver,
// with original/six-real output extent: 590/622/590/622; completed final result
// 597/622/597/622. All restored; frame 0x19ac and native flag-result homes remain.
// P268: Ordinary changed result retained, 588 -> 565 plain at retail length. With per-use
// alternate/existing byte setters/both: 571/649/653; reversed relational operands give
// 565/571/649/653. On ordinary changed, six-real buffer with names/velocity/both phase
// scopes: 565 each (ties, restored). Frame 0x19ac and native flag-result stores remain.
// M311: Native tag-field layout with actual physics-case orders
// 013456/016543/654310/601345/345601/456013/560134/103456: 588/590/592/592/590/590/590/590; restored.
// M311 continued: Native indexed light records, plain/Boolean bit-4 test 595/589; restored.
// AC481: direct initial changed truth / branch-owned lit truth / logical final result,
// subsets 1/2/4/3/5/6/7: 569/567/587/570/639/581/623; all with a signed
// countdown point cursor 642. Original body restored; frame and flag-result homes remain.
// F334: Coherent per-use definition, long stopped result and six-real extent subsets
// 1/2/3/5/6/7: 761/583/761/761/583/761; all with short stopped 759.
// Native short stopped alone 571; full-width reused final result 570.
// Extent-only control 565 repeated an earlier form and is excluded; all executable
// source restored. Frame, native flag-result bytes and sum-loop scheduling remain.
// AC491: Entry-owned bounds/time/reduction scalar declarations, masks 1/2/4/3/5/6/7, then coherent
// scalar identities: 565/565/565/565/565/565/565/565. Original complete body and producers restored;
// native frame and homes remain.
// AC494: Existing word-bit query at initial/active/physics families, masks 1/2/4/3/5/6/7,
// then all with six-real output: 620/597/633/656/673/663/696/696. Calls remain outlined
// where retail expands the tests; original complete body and declarations restored.
// NOT kept: the shared query adds calls absent from the native flag tests.
// P283: Direct idle admission with original/six-real output and original/zero-filled
// name array: 565/565/571/572; completed logical idle value with the same variants:
// 565/565/571/572. All tie or regress; original body, buffer and array stores restored.
// Idle-call expansion, frame/state homes, flag-result stores and sum scheduling remain.
// N338: Complete axial/radial response products, squared-radius and radial-scale
// accumulator subsets, then running response accumulators: 566/565/565/565/565/566/565/565.
// Original complete body restored; frame 0x19ac/0x1984, flag-result and reduction homes remain.
// N340: Branch-owned initial powered/open/maximum production, seven subsets,
// then complete logical numeric selection: 543/565/565/543/543/565/543/587. All restored;
// native frame, predicate-result byte stores and reduction homes remain.
// NOT kept: 543 shares the initial unity load where retail loads it twice.
// P286: One ordinary Boolean predicate receiver at original snapshot/activity/physics tests,
// seven nonempty subsets 575/565/565/575/575/565/575; all with six-real buffer 575.
// NOT kept: embedded predicate assignments require a match; original entire body restored.
// B394: Boolean result/changed tail accumulation and the two conditional completions;
// at original/entry call-result lifetime: 573/573/568/567/573/573/568/567.
// NOT kept: no exact match; original complete executable source restored.
// J244: Existing squared-length/dot producers with original/shared point difference
// and original/six-real buffer: 565/565/561/561/565/565/561/561.
// NOT kept: four-point lead is below threshold; six-real frame 0x1990 still
// exceeds 0x1984. Original complete body and arithmetic restored; flag stores and reduction remain.
// P295: Direct Boolean snapshot/activity/physics tests on the ordinary result graph,
// masks 1/2/4/3/5/6/7, then six-real output: 575/599/565/617/575/599/617/617.
// Word-load/shift prefix partly recovered; predicate stores and native frame still miss.
// Original tests, load widths, API buffer and entire body restored.
// Y562: Existing six-real API record with long/int count and counter, indexed/remaining
// traversal: 565/596/565/596/565/596/565/596. Original byte buffer, signed reduction and entire body restored.
// NOT kept: native record leaves frame 0x1990/0x1984; countdown grows to 2292/2264 bytes.
// Y564: Actual three-entry request-array initializer at original/entry/post-stop/
// pre-damage phases 565/629/614/587; entry declaration with late stores 565;
// post-first-result initializer 565; original/entry initializers with six-real API
// output 565/629. NOT kept: no gain; original full body and array values restored.
// Y566: Damping real-input power overload/float power/exact float base/shared timestep:
// 565/566/565/515; each with the six-real API extent 565/566/565/515.
// NOT kept: timestep sharing grows to 2276/2264 bytes; native extents still leave
// frame 0x1990 versus 0x1984. Original complete body and observations restored.
// P298: Control-call accumulation by test/logical/conditional/named-result forms
// 533/621/533/533; request-call accumulation 614/619/614/614. Original body restored.
// NOT kept: 533 grows to 2300/2264 bytes; frame and native predicate stores still differ.
// F376: Initial numeric powered/open bit extraction and unsigned snapshot width, subsets
// 1/2/4/3/5/6/7, then explicit maximum production: 598/565/565/599/602/601/624/624.
// Original complete body restored; frame 0x19ac/0x1984 and native predicate stores remain.
// P328: Saved attached complement and complete attachment admission in bool/byte/short/long:
// 565/565/575/575/563/563/574/574. NOT kept: best gain is below eight and shortens away from retail.
// Original body restored; frame 0x19ac/0x1984, predicate-result and reduction homes remain.
// AC545: Complete speed-result comparison arms, bool/byte/short/long at producer
// 565/565/571/581; entry declarations 565/565/571/581. NOT kept: no equality;
// wide carriers shorten away from native length. Original full body restored;
// frame, initial predicate-byte stores and rolled reduction remain.
// J254: Existing bounds/state/component/reset providers inline/forced:
// 565/628/565/2131/565/644/565/614. NOT kept: forced expansion grows away from retail; bounds
// form loses matched 0x1d56f0. Original complete body and every declaration restored; native
// frame and predicate stores remain.
// F386: Complete velocity/offset/position component-array observations, seven subsets,
// then all with six-real output: 565 each. Original entire body restored; native frame,
// predicate-byte stores and reduction homes remain. NOT kept: no gain.
// F389: Completed point-average branch/default/result/running-result publications,
// original then six-real API output: 573/566/565/565 twice. NOT kept: ties/regressions;
// original average, buffer and complete body restored; native frame/flag-result homes remain.
// B441: Complementary reused moving predicate in bool/byte/short/long, at producer/entry
// declaration boundaries: 512/512/580/586/512/512/580/586. NOT kept: complement instructions are absent
// from retail and extents grow away from 2264; original complete body restored.
// Frame 0x19ac/0x1984, native predicate stores and rolled reduction remain.
// P330: Plain raw light cursor/indexed scan, native Boolean bit-four test and
// branch-owned mask: 565/566/571/565/565/565/571/571. NOT kept: no gain; frame 0x19ac
// and predicate/reduction homes remain. Original full body, buffer and array values restored.
// M393: Point-sum/damping/radial rate-distance-scale double receivers, subsets 1/2/4/3/5/6/7 then immutable nonaccumulators:
// 565/582/635/576/637/635/639/639. NOT kept: nonexact precision receivers; native frame, predicate-byte stores and reduction homes remain.
// Original complete body restored.
// P345: Real point-array endpoints, less-than/equality and native/double totals,
// with original/six-real API output: 668/648/600/609/668/648/600/609. NOT kept: all regress;
// native frame, predicate bytes and rolled reduction remain; original complete body restored.
// Round 68 (lane 13), at 562: retail's bottom home (0x18 bytes) holds the 0x205510 buffer, all three velocities
// and the name array; ours keeps them apart. `real buffer[6]` 562 at frame 0x1990 (the names then share the
// buffer); with the velocity declared in the state branch and in the falling-damage block 562 (0x1990); with the
// names in their own block 562 (0x1990: the names then share the velocity and the buffer stays apart; a home's
// size seems fixed by its first member, and the velocity is placed first); the velocity/names blocks with the
// 0x28 buffer 562 (0x19ac); the state-branch velocity declared just before its 0xba1d0 call 562 (0x1990).
// Round 147 (m3 lane 7, fast): `bool fast = fabs(speed) >= definition+0x214` 565 (gives retail's fxch/fcompi/jb with fast
// in BL; masked aligned 189 -> 186); lit with a field read per arm, `alternate ? (flags348 >> 1) & 1 : flags348 & 1`, 568;
// that with bit 5 read again after 0xba7f0 583 (2244 B, masked aligned 164); both with `>=` 573 (162); `>=` with the fresh
// read and the original lit 572 (163), with alternate through the word bit view too 570 (163); the fresh read through
// the word bit view 576 (retail tests it with shr/test and writes bits 1/0 with memory or/and), with alternate through
// it too 577; flags348 declared as one word (flags349 its high byte) 575 at 2276 B and matched 0xf0fd0 drops to 56.
// Retail's lit reads the word once (alternate and the true arm) and the byte again in the false arm, with `& 1` after
// the join; its `fast != lit` is setne/test. 0x10f5f0 marked `inline` was not measured: four matched callers in other
// files (0xe9830, 0xc49b0, 0x265680, 0x155810) call it out of line.
// Round 76 (m3 lane 14, fast): the six stores have the shape of an inline bool helper whose result has two definitions
// (matched 0x15b7c0's score_teams_enabled() leaves the same `and cl, 1; mov [esp+0x13], cl` store); a file-local helper
// `result = false; if (bit < 32) result = TEST_FIELD_BIT((flags >> bit) & 1);` on the six tests 574 (2312 bytes: the stores
// appear, the first block is retail's apart from the result and `changed` slots exchanged), the one-definition form
// (bipeds' field_bit_test shape) 577 (no stores); `(bool)` on the bit-12 test 580 (retail's shr/and, no store);
// TEST_FIELD_BIT on every shift-and-mask test 602 (2296 bytes, aligned 266 -> 253: retail extracts every bit with shr
// and tests it); a `__forceinline` declaration of 0x10f5f0 here 561 (ignored: retail inlines 0x10f5f0 only here and
// calls it from four matched functions, so `inline` on its definition is out). Retail does not unroll the point-sum
// loop (in-place addss, countdown, 0xa8 stride); ours unrolls it by four. Of these, `(bool)`/bit views, the one- and
// two-definition helpers and the 0x10f5f0 declaration re-measure listed forms. New: the flags348 bit writes through the
// existing byte_flags_set (`fast` as the value) 645 (2184 bytes; the writes collapse instead of retail's separate in-memory
// or/and); the point-loop bound and the average read from the definition field at each use 561 (same code). No further
// untried natural form found for the stores or the unrolled loop.
// Round 154 (m3 lane 8, fast): `inline` on 0x10f5f0's definition (diagnostic of the inline swap) 644 at 2328 bytes:
// this function then inlines 0x10f5f0 as retail does but still inlines 0x113e40, which retail calls here and from
// 0xdd360 (ours calls it from 0xdd360 too), and 0xc49b0, 0xe9830, 0x155810, 0x265680 keep their calls; so the budget
// is not what keeps 0x113e40 out of line in retail (LEAD: retail's file settings for 0x113e40/0x10f5f0, a build-flag
// question). Re-measured: flags348 bits 1/0 written through a byte bit view declared in s_vehicle (overlaying the
// byte) 561 (same code: VC combines bitfield writes in a register too); with the second bit-5 test a fresh read of
// it 570 (aligned 266 -> 229; the listed fresh read). Compile-only screen of the point-sum loop: an indexed, while,
// postincrement, countdown, double-total or unsigned-bound spelling all unroll by four; only `i != count` (a `je`
// guard where retail's is `jle`, so it differs for a negative count) or a short index keep it rolled.
// Also round 154: both flags_10a tests (kill-volume bit 2, child bit 7; retail extracts each with `shr; test 1`)
// through the file's TEST_FIELD_BIT(VEHICLE_WORD_BITS(...flags_10a)->bitN) idiom 589 at 2292 bytes (both tests then
// retail's shape); the child bit 7 alone 577 (2284 bytes); the kill-volume bit 2 alone 575 (2276 bytes); the
// kill-volume test's flags_c0 bit 6 through the word bit view 571 (2276 bytes).
// MiniMax A (11-13): no new form tried this round, no lower score (561): our frame is 0x1990 where retail's is
// 0x1984, so every stack reference sits a dword higher, and the point-sum loop's unrolling and the flag stores
// keep retail's different shape; the frame needs a different set of locals in the body.
// MiniMax C (11-14): no new form, no lower score (561); retail still materialises the word flag's bit 12
// as a byte at [esp+0x13], written and never read, where this build tests it in place.
// @retail 0xefde0
bool __stdcall function_efde0(long vehicle_index)
{
	s_vehicle_physics_state state;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool stopped = function_f1320(vehicle_index);
	bool changed = false;
	word flags_c0 = vehicle->flags_c0;
	vector3f linear_velocity;

	if (((flags_c0 >> 12) & 1) && vehicle->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(vehicle->havok_component_index);
		real powered = (vehicle->control_flags & 0x2000) ? 1.0f : 0.0f;
		real open = ((flags_c0 >> 10) & 1) ? 1.0f : 0.0f;
		real position = MAX(powered, open);

		function_1d24a0(component, position);
		function_1d2460(component);
		if (position > 0.0f)
			changed = true;
	}
	if (!(*((byte *)vehicle + 0xc1) & 1))
	{
		vehicle->unknown0bc = g_510c54->game_time;
		changed = true;
	}
	if (stopped)
		function_efc30(vehicle_index);

	if (*(short *)(definition + 0x1f0) != 6 && !stopped && ((*(byte *)&vehicle->flags_c0 >> 6) & 1))
	{
		if (vehicle->parent_object_index != NONE)
		{
			function_b9b90(vehicle_index, true);
		}
		else if (!(definition[0x2ac] & 1))
		{
			real steering = 0.0f;

			function_ef4f0(vehicle_index, &steering);
			changed |= function_ef870(vehicle_index);
			if ((*((byte *)vehicle + 0xc1) & 1) && !function_efb60(vehicle_index))
			{
				if ((vehicle->flags349 & 1) && vehicle->havok_component_index != NONE)
				{
					s_havok_component *component = havok_component_get(vehicle->havok_component_index);
					real buffer[6];

					if (function_205510(component, definition + 0x2ac, buffer))
					{
						transform4x3f matrix;

						function_ba160(vehicle_index, &matrix);
						function_1cfb90(component, &matrix, buffer);
						function_f6670(vehicle_index, false);
						function_ba3d0(vehicle_index);
					}
				}
			}
			else
			{
				function_f06c0(&state);
				changed = true;
				function_ba1d0(vehicle_index, &linear_velocity, NULL);
				if (function_205be0(&state, vehicle_index))
				{
					if (!((vehicle->flags348 >> 2) & 1))
					{
						switch (*(short *)(definition + 0x1f0))
						{
						case 0:
							function_f1cd0(vehicle_index, &state);
							break;
						case 1:
							function_f1f80(vehicle_index, &state);
							break;
						case 3:
							function_f2360(vehicle_index, &state);
							break;
						case 4:
							function_f3010(vehicle_index, &state, steering);
							break;
						case 5:
							function_f4800(vehicle_index, &state);
							break;
						case 6:
							function_2056e0(vehicle_index, &state, 0.0f, NULL, NULL);
							break;
						}
					}
					function_f6010(vehicle_index);
					function_f6670(vehicle_index, true);
					function_f6bd0(vehicle_index, &state);

					long count = *(long *)(definition + 0x2e8);
					real total = 0.0f;
					long i;

					s_vehicle_physics_point const *point = state.points;
					for (i = 0; i < count; i++)
					{
						total += point->value;
						++point;
					}
					vehicle->unknown3fc = count ? total / (real)count : 0.0f;

					if (((*(byte *)&vehicle->flags_c0 >> 6) & 1) && ((1 << definition[0x1f0]) & 0x28) &&
						vehicle->unknown25c > 0.0f && !function_cc380(vehicle_index) && !((vehicle->flags_10a >> 2) & 1))
					{
						byte *bsp = (byte *)g_4e0348;
						real lower = *(real *)(bsp + 0x1c);
						real upper = *(real *)(bsp + 0x20);
						real radius = *(real *)(bsp + 0x1c0);
						real rate = g_510c54->rate * 6.0f;
						real damping = (real)pow(0.30000001192092896, (double)g_510c54->rate);
						point3f const *position = (point3f const *)((byte *)vehicle + 0x64);

						function_ba1d0(vehicle_index, &linear_velocity, NULL);
						if (lower != 0.0f && lower > position->z)
							linear_velocity.k = (lower - position->z) * rate + linear_velocity.k * damping;
						if (upper != 0.0f && position->z > upper)
							linear_velocity.k = (upper - position->z) * rate + linear_velocity.k * damping;
						if (radius != 0.0f)
						{
							vector3f offset;

							offset.i = *(real *)(bsp + 0x1c4) - position->x;
							offset.j = *(real *)(bsp + 0x1c8) - position->y;
							offset.k = *(real *)(bsp + 0x1cc) - position->z;

							real distance_squared = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;

							if (distance_squared > radius * radius)
							{
								real distance = (real)sqrt(distance_squared);
								real scale = (distance - radius) / distance * rate;

								linear_velocity.i += offset.i * scale;
								linear_velocity.j += offset.j * scale;
								linear_velocity.k = offset.k * scale + linear_velocity.k * damping;
							}
						}
						function_b77d0(vehicle_index, &linear_velocity, NULL);
					}
				}
			}
		}
	}

	if (g_4e6948->mode != 4 && ((*(dword *)(definition + 0x1ec) >> 6) & 1))
	{
		byte *globals = *(byte **)((byte *)g_4e034c + 0x144);

		function_ba1d0(vehicle_index, &linear_velocity, NULL);
		if (0.0f - *(real *)(globals + 0x5c) > linear_velocity.k)
		{
			for (long child_index = *(long *)((byte *)vehicle + 0x10); child_index != NONE; )
			{
				s_vehicle_header *header = VEHICLE_HEADER_GET(child_index);
				s_vehicle *child = header->vehicle;

				if (((1 << child->object_type) & 1) && !((child->flags_10a >> 7) & 1))
				{
					s_type_1e6529 damage;

					damage.material_index = NONE;
					function_d6660(&damage, *(long *)(globals + 0x28));
					function_d7b80(&damage, child_index, NONE, NONE, NONE, NULL);
				}
				child_index = *(long *)((byte *)child + 0xc);
			}
		}
	}

	SET_FLAG(vehicle->flags_134, 23, vehicle->control_flags & 1);
	if ((vehicle->flags_134 >> 23) & 1)
	{
		if (1.0f > vehicle->boost)
		{
			vehicle->boost = *(real *)(definition + 0x25c) * g_510c54->rate + vehicle->boost;
			if (vehicle->boost > 1.0f)
				vehicle->boost = 1.0f;
			changed = true;
		}
	}
	else if (vehicle->boost > 0.0f)
	{
		vehicle->boost = vehicle->boost - *(real *)(definition + 0x25c) * g_510c54->rate;
		if (!(vehicle->boost > 0.0f))
			vehicle->boost = 0.0f;
		changed = true;
	}

	bool fast = !((real)fabs(vehicle->speed) < *(real *)(definition + 0x214));
	bool alternate = (vehicle->flags348 >> 5) & 1;
	bool lit = (alternate ? vehicle->flags348 >> 1 : vehicle->flags348) & 1;

	if (fast != lit)
	{
		dword mask = fast ? 0xffffffff : 0;

		if (alternate)
		{
			long count = *(long *)(definition + 0x2f0);
			byte *light = *(byte **)(definition + 0x2f4);

			for (long i = 0; i < count; i++, light += 0x4c)
			{
				if (((*(dword *)(light + 4) >> 4) & 1) && *(long *)(light + 0x48) != NONE)
					mask &= ~(1 << *(long *)(light + 0x48));
			}
		}
		function_ba7f0(vehicle_index, NONE, 0, mask);
		if (alternate)
		{
			if (fast)
			{
				vehicle->flags348 |= 2;
				vehicle->flags348 &= ~1;
			}
			else
			{
				vehicle->flags348 &= ~2;
				vehicle->flags348 &= ~1;
			}
		}
		else
		{
			vehicle->flags348 &= ~2;
			if (fast)
				vehicle->flags348 |= 1;
			else
				vehicle->flags348 &= ~1;
		}
	}

	bool unattached = vehicle->parent_object_index == NONE &&
		(vehicle->havok_component_index == NONE || !(*((byte *)vehicle + 0xc1) & 1));
	bool idle = false;

	if (function_113e40(vehicle_index))
	{
		if (function_10f5f0(vehicle_index) == 0x400000c)
		{
			idle = true;
		}
	}
	else
	{
		idle = true;
	}
	if (!idle || unattached)
	{
		changed = true;
		function_bba20(vehicle_index);
	}

	long names[3];

	names[0] = 0x7000101;
	names[1] = 0x7000101;
	names[2] = 0;

	bool result = function_e6830(vehicle_index);

	result |= function_111650(vehicle_index, names);
	vehicle->unknown3bc = NONE;
	vehicle->unknown3c4 = NONE;
	vehicle->unknown3c8 = NONE;
	vehicle->unknown3cc = NONE;
	function_b9ef0(vehicle_index, &vehicle->unknown3d0);
	return result | changed;
}

/* clears the sixteen entries of the physics state's block at +0xbc0;
   returns the state (retail leaves it in eax) */
// @retail 0xf06c0
s_vehicle_physics_state *function_f06c0(s_vehicle_physics_state *state)
{
	s_vehicle_contact *contact = state->contacts;
	short i;

	for (i = 16; i; i--)
	{
		contact->unknownb0 = NONE;
		contact++;
	}
	return state;
}

/* what the vehicle's animation values sample into */
struct s_vehicle_animation_sample
{
	long node_count;
	real_quaternion_transform *transforms;
	dword const *node_mask;
};

typedef void (__stdcall *vehicle_animation_value_callback)(long vehicle_index, s_graph_tag *graph,
	c_animation_channel *channel, real ratio, real pitch, real weight, void *user);

/* samples one of the vehicle's value-driven animations: an aiming screen
   by its yaw, or else at the value's frame ratio */
// @retail 0xf06e0
void __stdcall function_f06e0(long vehicle_index, s_graph_tag *graph, c_animation_channel *channel, real ratio,
	real pitch, real weight, void *user)
{
	s_vehicle_animation_sample *sample = (s_vehicle_animation_sample *)user;
	s_animation *animation = NULL;

	if (channel->animation_id.index != NONE)
	{
		animation = function_1daea0(graph_tag_get(channel->graph_tag_index), channel->animation_id);
	}
	if (animation->blend_screen != NONE)
	{
		channel->sample_aiming(ratio, pitch, weight, sample->node_mask, sample->node_count, sample->transforms);
	}
	else
	{
		channel->sample_ratio(ratio, weight, sample->node_count, sample->transforms, sample->node_mask);
	}
}

bool function_1dceb0(s_graph_iterator3c *iterator, s_graph_tag *graph);


/* a speed as a fraction of the definition's maximum forward or reverse
   speed, 0.5 at rest */
PRIVATE __forceinline real vehicle_speed_fraction(real speed, byte const *definition)
{
	real reverse_limit = *(real *)(definition + 0x1f8);
	real forward_limit = *(real *)(definition + 0x1f4);
	real result = 0.5f;

	if (speed > 0.0f)
	{
		if (forward_limit > 0.0f)
		{
			result = speed / forward_limit * 0.5f;
			result += 0.5f;
		}
	}
	else if (reverse_limit > 0.0f)
	{
		result = speed / reverse_limit * 0.5f;
		result += 0.5f;
	}
	return PIN(result, 0.0f, 1.0f);
}

/* hands each of the vehicle's value-driven animations (its steering,
   sideways drift, speed, forward speed and two engine overlays, the
   graph's function animations and the three aiming overlays) to the
   callback with its value */
// score 9 (was 9): the cross is now retail's; left: the two VEHICLE_GET lookups' SIB order (2), the three
// reloads after the overlay-kind loop (3) and the forward-speed sum's first two products (4).
// MiniMax R (11-06): no new form, no lower score (9); the flat sum keeps (i + j) + k here for every term order and
// operand orientation and for the array-member spelling too, while a compile-only probe of the same expression
// gives retail's (k + j) + i order when the three products have fresh component nodes.
// Earlier at 13: Graph-loop scope fixes constructor/load scheduling; header SIB forms, join reload
// order and cross/dot operand ranks remain.
// Earlier at 40 (was 44): the overlay lookups now call 0x1cb9d0 directly (each gets its own result home: frame 0x9c as
// retail) and the spin fractions are one ternary each (kept in xmm0 to the push). Left: retail inlines the state's
// channel-play routine (0x1cb490) at every set, so its id is loaded before the graph test and NONE stays in EDX
// (ours: EAX), which moves the iterator/channel init, the kind-loop value/counter homes and the cross block; and VC
// folds `x * 0.5f + 0.5f` to `(x + 1) * 0.5` where retail multiplies first.
// Round 182: the forwarding wrapper replaced by the direct call 383 (kept); the ternary spin fractions 371 (kept);
// overlay fields typed as animation ids 383 (identical); up/forward pointer locals for the cross 370 (one-use);
// `(real)(x * 0.5f) + 0.5f` 393; each overlay read into a local first 476; the function channel declared before the
// iterator 371; the kind value at loop-body scope 371. Diagnostic: the channel-play routine called at each set and
// forced inline in its header 327 (first block then exact), but 0x1101e0/0x16760c lose their out-of-line calls; this
// file is /Ob1, so a plain call stays out of line (flags lead).
// Earlier at 409: frame 0x88 versus 0x9c, returned-id temporaries and channel/count homes differ.
// Existing speed helper forced inline 468; with cross helper 470; channel role names 468/470;
// single final speed clamp 472, with cross 450, role names 472/450. Both directional limits
// retained at entry: original/clamp 446/424; with cross 409 (kept)/412.
// Round 426: separate returned-id values for each of the six overlays 408 each; all six 408; all with the loop id 434. Original body restored; frame remains 0x88.
// Round 103 (kept, 371 -> 40 at retail's 2027 bytes): each set through the existing channel_play, with
// channel_play and channel_start marked `inline` at their definitions (this /Ob1 file can then expand them as
// retail does; their other callers, 0x1101e0 and 0x16760c included, keep their out-of-line calls) 313 alone;
// the speed fraction's half product named (VC turns `x * 0.5f + 0.5f` into `(x + 1) * 0.5f`) 393 alone, with
// its clamp once at the return, the reverse limit read first and the kind loop clamping its named value in
// place 414 alone (2011 bytes) and 364 with channel_play; the function channel declared after the iterator
// setup 365; the steering dot as `left.k * v.k + v.j * left.j + left.i * v.i` 40 (written i, j, k with the
// velocity first 365; through dot3f either way 372). Left: the two VEHICLE_GET lookups' SIB order, the reloads
// after the overlay lookup (retail state, graph, vehicle index; ours vehicle index first), two of the cross
// product's six terms load the other operand first, and the iterator/channel setup before the function loop
// (retail stores the graph index early and the iterator's NONE fields before the channel's). Tried at 40: the
// forward-speed i term's operands swapped 40 (identical); dot3f for the forward speed 42; cross3f_staged on
// the earlier 42 form 42 (identical); the iterator's 0x28 word before next_index 40 (its two stores then in
// retail's order); the function channel declared before the iterator 44, right after it (before the setup)
// 44; the iterator's NONE fields before its floats 40.
// Coordinator review (lane 8 r103): the lane's 40 named each half product in a one-use local (`real half`),
// kept only with a match; without them 356. Writing the half product into the result (and the loop's value)
// first and adding 0.5f in a second statement keeps VC from refolding it, 44 with no new local (kept).
// Round 44 (lane 11): the iterator initialized by a constructor of s_graph_iterator3c (mem-init list in member
// order, the animation id's own constructor between index and the 0x28 word, floats typed real) 40 (kept, shared
// with 0xf6670: the iterator's stores are then retail's 0x6c/0x8c/0x90/0x92/0x94/0x96 order); on it: the function
// channel declared before the iterator 44 (twice, before and after the real-typed fields); linear_velocity
// declared first 40, `i` declared first 40, the cross result declared at the block top 40 (all identical code).
// Left: retail schedules the graph index load and the iterator's NONE stores before the channel's NONE stores.
// Round 13 (lane 17), at 44 (left: the two VEHICLE_GET SIB orders 2, the three reloads after the overlay block 3,
// the cross 8 and forward-speed dot 4 operand orders, the iterator/channel setup 27): the iterator given a
// constructor in its header (mem-initializers in member order with the 0x28 field as a short, floats in the body;
// 0xf6670 drops its copy of the init, stays 286; 0x1dceb0 still matches) 40: the iterator's NONE stores then come in
// retail's member order and the cross's two up.i products load retail's operand, but every channel store still
// precedes the graph-index load (retail loads it right after the channel's three zero words) and the iterator stores
// follow it; on that: one forward pointer for the cross and the forward-speed dot 48, `state` declared at function
// scope (assigned before the channel) 40, `left` declared at block scope 40, the forward speed passed straight to the
// fraction (no `speed` local) 40. Diagnostics: `iterator.animation_id = c_type_709360()` after the index 149 (a
// temporary copy); the function channel declared before the iterator 44; the sum's source order is ignored (VC
// always forms (i + j) + k here, retail keeps k, j, i).
// J222: Complete up/forward/both cross inputs 404/420/442; twice-read header address 525, with up/forward/both
// inputs 533/532/554; next graph produced before constructors and after each iteration 142 (exact-only). All
// original executable source restored; no calibrated loss.
// M283: Frame 0x9c and extent agree; first +0x2d is header SIB ownership, with join reloads and constructor/graph scheduling still different.
// Native header ordinal long with normal/early iterator/early channel/both constructors 114/518/552/530;
// word ordinal with the same lifetimes 205/527/590/561. All eight bodies restored; original 40 retained.
// Round 250: native value construction for channel/iterator/function-channel, seven subsets 40 each;
// graph-only iterator/channel scope 13 plain, 2027 bytes; remaining SIB/reload/cross/dot ranks.
// Round 253: First/second header access and failed-admission-return subsets 1/2/4/3/5/6/7:
// 17/13/13/17/17/13/17; staged cross on graph-only workspace 13. All bodies restored;
// frame/constructors agree, header SIB, join reload and cross/dot ranks remain.
// P255: Existing object getter at both/first/second headers 17/13/17; paired dot helpers
// 361, with both/first/second getters 365/361/365; reversed paired dots with both getters
// 365. Original body restored; header SIB, join reloads and cross/dot ranks remain.
// J239: actual table-base capture 521; separate refresh/publication ordinals 17; bounded short ordinal 384; base with separate ordinals 521; base with short ordinal 506; separate short ordinals 384; all three 506; sequential animation-ID output cursor 17; coordinated kind-getter inline declarations 13. Original body and declarations restored.
// Round 92: raw byte-addressed first/second/both headers 13/13/13; refresh scope 13; const vehicle/graph/state
// pointers 13; value-initialized cross output 13; three-statement forward sum 11, with reversed final i product 11;
// k+j initializer plus i 15. NOT kept: the two-point staged gains are below the retention threshold. Original body
// restored; header SIB, join reloads, shared cross loads and the final forward i load remain.
// Round 43 (lane 17, fast): the overlay-kind loop with its own counter 17 (the cross's up.i products then load
// forward: the product operands follow numbering); both small loops with for-scoped counters 17; the overlay
// fields declared as animation-id arrays in s_vehicle 13 (identical); the overlay channel declared before `state`
// 32; the forward speed as dot3f(velocity, forward) 15. Original body restored.
// Round 92 (lane 9): the two definition-period arms' one-use `value` locals written in their calls 9 (kept: the
// cross's six products then load retail's operands); `model` written into the graph lookup 13, `speed` written into
// the call 13, any two of model/speed/the arm values 9 (the cross follows the count of named locals: dead pointer
// locals, diagnostic, give retail's cross for three counts in eight). The forward speed as three statements (`speed =
// k term; speed += j term; speed += i term`) 7, only its i term's operands left (not kept: a += split of a float sum);
// all 48 one-expression term orders and operand orientations 9. On the 7 form: the overlay update block as an inline
// or forceinline helper 7, the kind loop with its own counter 11, through a pointer cursor 451, as do/while 508,
// through a named kind value 52, the graph index named for the test and the store 11, the i term named/written
// `speed = speed + ...`/`... + speed`/`*=` staged 7 each. Diagnostics: the SIB order flips only when two whole
// overlay arms go; a test block before, inside or after the kind loop never gives retail's reload order; extra
// float/int statements in the arms or the kind loop break the length. channel_play as `&&`, two returns or a
// ternary unmatches 0x1cb490 itself (16/20/20); channel_start without its graph-index local unmatches 0x1cb410 (7).
// Round 68 (lane 13), at 9: the permuter (600 s, 66 tries) found nothing better; the callbacks given a local copy
// of the vehicle index 13 (the cross products flip, the reloads stay); the forward-speed local renamed 9;
// VEHICLE_GET as pointer arithmetic (base + index) 9; the second / first lookup through
// VEHICLE_HEADER_GET(...)->vehicle 9/9 (all identical code). Retail reloads state, graph, then the vehicle index
// after the kind loop (the reverse of the order the update block clobbers EDI, EBX, ESI); ours the index first.
// Round 78 (lane 10, fast): dot3f(&vehicle->forward, &linear_velocity) for the forward speed 11 (the sum then
// runs (j + k) + i and the i term still loads forward.i), dot3f(&linear_velocity, &vehicle->forward) 11 (same
// code); a forward pointer for the speed terms 15 (one-use; the cross products flip too); the graph through
// graph_tag_get(*(long *)(model + 4)) 9 (same code); both dots through dot3f 365 (2011 bytes).
// Round 95 (lane 9), at 9: 9-16 unused locals at the top (9/12/16 at the end) never move the SIB pair, the
// reloads or the forward-speed sum (only the cross, period 8); the forward speed grouped k+(j+i) 9, k+(i+j) 9,
// i+(k+j) 11, (k+i)+j 11, (j+i)+k 9, j+(k+i) 11; a forward pointer for its three reads 15 (the sum then (j+k)+i,
// the i term retail's at +1 local, 13), eight term orders on it 13 each; one forward pointer for the cross and the
// speed 17; a named header pointer for the first lookup (0xef4f0's form) 13, for both 13 (SIB unchanged). Matched
// 0xeeb80 gets retail's data-first SIB from the same two lookup lines.
// Round 84 (lane 10, fast), at 9: the graph/state guard as an early return 9 (same code); the forward speed computed
// once before the guard 508 (2043 bytes: VC computes it eagerly, no sinking into the arm); both lookups indexed by
// `(word)vehicle_index` 178 (2011 bytes); vehicle_speed_fraction plain `inline` 581 (1803 bytes, out of line under
// /Ob1); the update test written `vehicle->unknown400 != state->graph_tag_index` 10.
// Round 173 (m3 lane 5, fast), at 9: by-value copies for the forward speed (the copies are materialised, not
// scalar-replaced): the forward vector 317 (2055 bytes), the velocity 321 (2059), both 357 (2087), the forward copy with
// i, j, k terms 317; one named absolute index for both lookups 13 (SIB unchanged, the cross flips); a velocity pointer for
// the forward-speed terms 13 (one-use; only the cross flips); the arm-1 dot through
// dot3f re-measured 376 (2007). Diagnostic: a vehicle-index copy declared after `state` for every callback 13 (the reload
// order stays, so it does not follow symbol order; the cross flips).
// Round 117 (m3 lane 12, fast), at 9: the model tag read through the typed object definition 9 (same code, kept);
// the forward speed grouped `(k + j) + i` explicitly 11 (the sum then (j + k) + i, the i term unchanged); `(real)`
// casts on its three products 9; the vehicle index parameter as `dword` 9; `speed` declared at the overlay block's top
// and assigned in the arm 9 (all three the same code).
// Round 192 (m3 lane 4, fast), at 9: the forward speed through the new vehicle_forward_speed helper (a forward pointer and a
// speed local; 0xef870 uses it) 11 (the sum becomes (j + k) + i and its i product flips); the graph and animation-state
// tests as nested ifs 9; `state` declared first at function scope 9; `graph` declared first at function scope 9; the file's
// vehicle_cross for the left vector 9 (all four the same code). Diagnostics: 1/2/3 dead inline-helper locals at the top or
// before the state lookup 13/13/17 (only the cross and forward-speed products move; the SIB pair and the three reloads
// never do, so they are not a numbering phase).
// @retail 0xf0760
void __stdcall function_f0760(long vehicle_index, vehicle_animation_value_callback callback, void *user)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	byte *model = g_4e3b44[((s_object_definition const *)definition)->model.tag_index & 0xffff].bytes;
	s_graph_tag *graph = (s_graph_tag *)g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
	vector3f linear_velocity;

	function_ba1d0(vehicle_index, &linear_velocity, NULL);
	if (graph && VEHICLE_GET(vehicle_index)->animation_state_offset != NONE)
	{
		s_animation_state *state = vehicle_animation_state_get(vehicle);
		c_animation_channel channel;
		long i;

		if (state->graph_tag_index != vehicle->unknown400 && state->unknown74 != NONE && state->unknown78 != NONE)
		{
			vehicle->unknown400 = state->graph_tag_index;
			VEHICLE_OVERLAY(vehicle, 0) = state->overlay_find_for_class(0x7000101, 0x7000101, 0x8000078);
			VEHICLE_OVERLAY(vehicle, 1) = state->overlay_find_for_class(0x7000101, 0x7000101, 0x4000079);
			VEHICLE_OVERLAY(vehicle, 2) = state->overlay_find_for_class(0x7000101, 0x7000101, 0x800007a);
			VEHICLE_OVERLAY(vehicle, 3) = state->overlay_find_for_class(0x7000101, 0x7000101, 0x800007b);
			VEHICLE_OVERLAY(vehicle, 4) = state->overlay_find_for_class(0x7000101, 0x7000101, 0xc00007d);
			VEHICLE_OVERLAY(vehicle, 5) = state->overlay_find_for_class(0x7000101, 0x7000101, 0x110005c9);
			for (i = 0; i < 3; i++)
			{
				VEHICLE_OVERLAY41C(vehicle, i) = state->overlay_kind_get(i);
			}
		}
		if (VEHICLE_OVERLAY(vehicle, 0).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 0), 0))
		{
			callback(vehicle_index, graph, &channel, vehicle->steering_angle, 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 1).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 1), 0))
		{
			vector3f left;

			cross3f(&vehicle->up, &vehicle->forward, &left);
			callback(vehicle_index, graph, &channel,
				((left.k * linear_velocity.k + linear_velocity.j * left.j + left.i * linear_velocity.i) /
					*(real *)(definition + 0x1f4) + 1.0f) * 0.5f,
				0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 2).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 2), 0))
		{
			callback(vehicle_index, graph, &channel, vehicle_speed_fraction(vehicle->speed, definition), 0.0f, 1.0f,
				user);
		}
		if (VEHICLE_OVERLAY(vehicle, 3).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 3), 0))
		{
			real speed = vehicle->forward.k * linear_velocity.k + vehicle->forward.j * linear_velocity.j +
				linear_velocity.i * vehicle->forward.i;

			callback(vehicle_index, graph, &channel, vehicle_speed_fraction(speed, definition), 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 4).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 4), 0))
		{
			callback(vehicle_index, graph, &channel, *(real *)(definition + 0x20c) > 0.0f ?
				vehicle->unknown360 / *(real *)(definition + 0x20c) : 0.0f, 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 5).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 5), 0))
		{
			callback(vehicle_index, graph, &channel, *(real *)(definition + 0x20c) > 0.0f ?
				vehicle->unknown364 / *(real *)(definition + 0x20c) : 0.0f, 0.0f, 1.0f, user);
		}

		{
			s_graph_iterator3c iterator;
			c_animation_channel function_channel;

			while (function_1dceb0(&iterator, graph_tag_get(state->graph_tag_index)))
			{
				if (state->channel_play(&function_channel, iterator.animation_id, 0))
				{
					byte value = ((byte *)vehicle->unknown398)[iterator.index];

					callback(vehicle_index, graph, &function_channel, value == 0xff ? 1.0f : value * (1.0f / 255.0f), 0.0f,
						1.0f, user);
				}
			}
		}
		for (i = 0; i < 3; i++)
		{
			if (VEHICLE_OVERLAY41C(vehicle, i).index != NONE)
			{
				real value = vehicle->unknown288[i] * 0.5f;

				value += 0.5f;
				value = PIN(value, 0.0f, 1.0f);

				if (state->channel_play(&channel, VEHICLE_OVERLAY41C(vehicle, i), 0))
				{
					callback(vehicle_index, graph, &channel, value, 0.0f, 1.0f, user);
				}
			}
		}
	}
}

/* the vehicle type's callback (+0x6c) that samples its value-driven
   animations into the node transforms */
// @retail 0xf0f50
void __stdcall function_f0f50(long vehicle_index, dword const *node_mask, long node_count,
	real_quaternion_transform *transforms)
{
	s_vehicle_animation_sample sample;

	sample.node_count = node_count;
	sample.transforms = transforms;
	sample.node_mask = node_mask;
	function_f0760(vehicle_index, function_f06e0, &sample);
}

/* whether the vehicle's definition sets flag 7 */
// @retail 0xf0f90
bool function_f0f90(long vehicle_index)
{
	bool result = (*(dword *)(VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index)) + 0x1ec) >> 7) & 1;
	return result;
}

void function_1d1260(s_havok_component *component);
void __stdcall function_1d01c0(s_havok_component *component);
void function_1cf120(long component_index);
void function_1d1540(s_havok_component *component);

/* sets the vehicle's flag 2, rebuilding its Havok component when it changes */
// @retail 0xf0fd0
void function_f0fd0(long vehicle_index, bool set)
{
	if (vehicle_index != NONE)
	{
		s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
		bool was_set = (vehicle->flags348 >> 2) & 1;

		if (set)
		{
			((s_vehicle_word_flag_bits *)&vehicle->flags348)->bit2 = true;
		}
		else
		{
			((s_vehicle_word_flag_bits *)&vehicle->flags348)->bit2 = false;
		}
		if (was_set != set && vehicle->havok_component_index != NONE)
		{
			s_havok_component *component = havok_component_get(vehicle->havok_component_index);
			bool active = TEST_FIELD_BIT(component->flag5);

			if (active)
			{
				function_1d1260(component);
			}
			function_1d01c0(component);
			function_1cf120(vehicle->havok_component_index);
			if (active)
			{
				function_1d1540(component);
			}
		}
	}
}

/* the collision result as 0xf1070 reads it (0x5c bytes) */
struct s_vehicle_ground_collision
{
	byte unknown00[8];
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x3c - 0x26];
	long unknown3c;
	long unknown40;
	byte unknown44[4];
	long unknown48;
	byte unknown4c[4];
	long unknown50;
	byte unknown54[0x5c - 0x54];
};

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
point3f *function_b9dd0(long object_index, point3f *result);
long function_1fa3a0(long a, long b, long object_index, long c, point3f const *point);
extern vector3f *g_4687bc;

/* the ground under the vehicle, found once a tick (for the vehicle types
   that drive on it) by a ray straight down from its center */
// @retail 0xf1070
void function_f1070(long vehicle_index, long *location, long *unknown3c0, point3f *point, long *unknown3c8,
	long *unknown3cc)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);

	switch (*(short *)(definition + 0x1f0))
	{
	case 0:
	case 1:
	case 4:
	case 6:
		if (g_510c54->game_time > vehicle->unknown3b8)
		{
			s_vehicle_ground_collision collision;
			point3f origin;
			vector3f vector;

			vehicle->unknown3b8 = g_510c54->game_time;
			collision.unknown24 = NONE;
			function_b9dd0(vehicle_index, &origin);
			origin.x += g_4687b0->i * 0.4f;
			origin.y += g_4687b0->j * 0.4f;
			origin.z += g_4687b0->k * 0.4f;
			vector.i = g_4687bc->i * 2.0f;
			vector.j = g_4687bc->j * 2.0f;
			vector.k = g_4687bc->k * 2.0f;
			if (!function_1697c0(0x84000d, &origin, &vector, NONE, NONE, (s_collision_result_1697c0 *)&collision))
			{
				break;
			}
			vehicle->unknown3d0 = collision.point;
			vehicle->unknown3bc = collision.unknown50;
			vehicle->unknown3c8 = collision.unknown40;
			vehicle->unknown3cc = collision.unknown48;
			if (collision.unknown50 != NONE)
			{
				vehicle->unknown3c0 = function_1fa3a0(collision.unknown3c, collision.unknown50, collision.unknown40, collision.unknown48,
					&collision.point);
			}
			else
			{
				vehicle->unknown3c0 = NONE;
			}
		}
		if (point)
		{
			*point = vehicle->unknown3d0;
		}
		if (location)
		{
			*location = vehicle->unknown3bc;
		}
		if (unknown3c8)
		{
			*unknown3c8 = vehicle->unknown3c8;
		}
		if (unknown3cc)
		{
			*unknown3cc = vehicle->unknown3cc;
		}
		if (unknown3c0)
		{
			*unknown3c0 = vehicle->unknown3c0;
		}
		return;
	}
	if (point)
	{
		function_b9dd0(vehicle_index, point);
	}
	if (location)
	{
		*location = NONE;
	}
	if (unknown3c8)
	{
		*unknown3c8 = NONE;
	}
	if (unknown3c0)
	{
		*unknown3c0 = NONE;
	}
	if (unknown3cc)
	{
		*unknown3cc = NONE;
	}
}

/* whether the vehicle is not a type 6 vehicle */
// @retail 0xf12e0
bool function_f12e0(long vehicle_index)
{
	bool result = false;

	if (*(short *)(VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index)) + 0x1f0) != 6)
	{
		result = true;
	}
	return result;
}

/* whether the vehicle, with its flag 2 at +0x10a set, is a type 5 vehicle
   or a type 4 one of kind 1 */
// score 1 (was 1): ours reuses the kind comparison's flags with a jmp to the
// shared jne; retail re-tests the type. Nested conditions, a named kind local,
// a switch and a 300 s permuter run all scored 1.
// @retail 0xf1320
bool function_f1320(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	bool flagged = TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags_10a)->bit2);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool result = false;

	if (flagged)
	{
		short type = *(short *)(definition + 0x1f0);

		if (type == 4 && *(short *)(definition + 0x218) == 1)
		{
			result = true;
		}
		else if (type == 5)
		{
			result = true;
		}
	}
	return result;
}

/* a gear's torque curve over the engine's angular velocity */
struct s_vehicle_torque_curve
{
	real min_torque;
	real max_torque;
	real peak_torque_scale;
	real past_peak_torque_exponent;
	real torque_at_max_angular_velocity;
	real torque_at_2x_max_angular_velocity;
};

/* a gear of the vehicle definition (its block at +0x268, 0x44 bytes) */
struct s_vehicle_gear
{
	s_vehicle_torque_curve loaded;
	s_vehicle_torque_curve cruising;
	real min_time_to_upshift;
	real engine_upshift_scale;
	real gear_ratio;
	real min_time_to_downshift;
	real engine_downshift_scale;
};

#define VEHICLE_MAX_ANGULAR_VELOCITY(definition) (*(real *)((definition) + 0x264))
#define VEHICLE_GEAR_COUNT(definition) (*(long *)((definition) + 0x268))
#define VEHICLE_GEARS(definition) (*(s_vehicle_gear **)((definition) + 0x26c))

/* the velocity from one point to another over a time, its length capped by
   a limit that blends from the second toward the first as the velocity
   lines up with the first point */
// @retail 0xf1380
void __stdcall function_f1380(vector3f *velocity, vector3f const *a, vector3f const *b, real aligned_limit,
	real limit, real value)
{
	real scale;
	real dot;
	real length_squared;

	velocity->i = a->i - b->i;
	velocity->j = a->j - b->j;
	velocity->k = a->k - b->k;
	scale = 1.0f / value;
	velocity->i *= scale;
	value = velocity->i;
	velocity->j *= scale;
	velocity->k *= scale;
	dot = a->j * velocity->j + a->k * velocity->k + a->i * velocity->i;
	if (dot > 0.0001f)
	{
		aligned_limit = dot * dot / (velocity->k * velocity->k + velocity->j * velocity->j + velocity->i * velocity->i) /
			(a->j * a->j + a->k * a->k + a->i * a->i) * (aligned_limit - limit) + limit;
	}
	else
	{
		aligned_limit = limit;
	}
	length_squared = velocity->k * velocity->k + velocity->j * velocity->j + velocity->i * velocity->i;
	if (length_squared > aligned_limit * aligned_limit)
	{
		real factor = aligned_limit / (real)sqrt(length_squared);

		velocity->i = value * factor;
		velocity->j *= factor;
		velocity->k *= factor;
	}
}

/* the ratio of one of the vehicle's gears, 0 for none */
// @retail 0xf14f0
real function_f14f0(short gear, long vehicle_index)
{
	long index = gear;

	if (index != NONE)
	{
		return VEHICLE_GEARS(VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index)))[index].gear_ratio;
	}
	return 0.0f;
}

/* a torque curve's torque at an engine angular velocity */
// @retail 0xf15d0
real function_f15d0(long vehicle_index, s_vehicle_torque_curve const *curve, real angular_velocity)
{
	byte *definition = VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index));
	real peak_scale = 0.001f > curve->peak_torque_scale ? 0.001f : curve->peak_torque_scale;
	real peak = VEHICLE_MAX_ANGULAR_VELOCITY(definition) * peak_scale;

	if (peak > angular_velocity)
	{
		if (peak > 0.001f)
		{
			return (curve->max_torque - curve->min_torque) / peak * angular_velocity + curve->min_torque;
		}
		return 0.0f;
	}
	if (angular_velocity >= VEHICLE_MAX_ANGULAR_VELOCITY(definition))
	{
		real slope = (curve->torque_at_2x_max_angular_velocity - curve->torque_at_max_angular_velocity) /
			VEHICLE_MAX_ANGULAR_VELOCITY(definition);

		return curve->torque_at_max_angular_velocity - VEHICLE_MAX_ANGULAR_VELOCITY(definition) * slope +
			slope * angular_velocity;
	}
	else
	{
		real result;
		real range = VEHICLE_MAX_ANGULAR_VELOCITY(definition) - peak;

		if ((range >= 0.0f ? range : 0.0f - range) > 0.0001f)
		{
			real slope = (curve->torque_at_max_angular_velocity - curve->max_torque) / range;
			real intercept = curve->max_torque - slope * peak;

			if ((curve->max_torque >= 0.0f ? curve->max_torque : 0.0f - curve->max_torque) > 0.001f)
			{
				real torque = curve->max_torque;
				real fraction = (slope * angular_velocity + intercept) / torque;

				if (PIN(fraction, 0.0f, 1.0f) == fraction)
				{
					fraction = (real)pow(fraction, curve->past_peak_torque_exponent);
				}
				result = fraction * torque;
			}
			else result = 0.0f;
		}
		else result = 0.0f;
		return result;
	}
	return 0.0f;
}

/* a gear's torque at an engine angular velocity, blended between its
   cruising and loaded curves by the engine's load */
// @retail 0xf1540
real __stdcall function_f1540(long vehicle_index, short gear, real angular_velocity)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	s_vehicle_gear *definition_gear = &VEHICLE_GEARS(VEHICLE_DEFINITION_GET(vehicle))[gear];
	real cruising = function_f15d0(vehicle_index, &definition_gear->cruising, angular_velocity) *
		(1.0f - vehicle->unknown380);

	return cruising + function_f15d0(vehicle_index, &definition_gear->loaded, angular_velocity) * vehicle->unknown380;
}

/* the gear the vehicle shifts to: neutral (NONE) when it coasts slowly, the
   reverse gear (0) or first gear when the throttle changes direction, and
   up or down a gear at its engine's shift points */
// @retail 0xf1740
short __stdcall function_f1740(long vehicle_index, real throttle_input)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	s_game_time_globals *time = g_510c54;
	volatile bool braking;

	if (vehicle->unknown384 && (0.5f > vehicle->unknown384 * time->rate || (vehicle->control_flags & 2)))
	{
		braking = true;
	}
	else
	{
		braking = false;
	}
	char current = vehicle->gear;
	short gear = current;

	if (VEHICLE_MAX_ANGULAR_VELOCITY(definition) >= 0.001f)
	{
		real throttle = braking ? 0.0f : throttle_input;
		real angular_velocity = vehicle->unknown378;
		real ratio = PIN(angular_velocity / VEHICLE_MAX_ANGULAR_VELOCITY(definition), 0.0f, 1.0f);

		if (throttle == 0.0f)
		{
			if (VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 0.25f > angular_velocity ||
				(vehicle->unknown248 == NONE && vehicle->unknown386 * time->rate > 1.0f))
			{
				gear = NONE;
				goto done;
			}
		}
		else if (*(short *)(definition + 0x1f0) == 1)
		{
			if (0.0f > throttle)
			{
				if (!braking)
				{
					gear = 0;
					goto done;
				}
			}
			else if (current < 1)
			{
				if (!braking)
				{
					gear = 1;
					goto done;
				}
			}
			else
			{
				s_vehicle_gear *definition_gear = &VEHICLE_GEARS(definition)[current];

				if (vehicle->unknown387 * time->rate > definition_gear->min_time_to_upshift && throttle >= ratio &&
					ratio > definition_gear->engine_upshift_scale)
				{
					long higher = current + 1;
					if (higher < 1)
					{
						higher = 1;
					}
					else if (higher > VEHICLE_GEAR_COUNT(definition) - 1)
					{
						higher = VEHICLE_GEAR_COUNT(definition) - 1;
					}
					gear = (short)higher;
				}
				if (vehicle->unknown388 * time->rate > definition_gear->min_time_to_downshift &&
					definition_gear->engine_downshift_scale > ratio)
				{
					if (gear <= 0)
					{
						gear = 1;
						goto done;
					}
					long lower = current - 1;

					gear = (short)PIN(lower, 1, VEHICLE_GEAR_COUNT(definition) - 1);
				}
			}
		}
	}
done:
	return gear;
}

bool function_f5d70(long vehicle_index);

/* runs the vehicle's engine for a tick: its load follows the throttle, a
   new gear engages after a delay (at once when it changes direction), the
   shift timers count, and the gear's torque spins the engine up */
// @retail 0xf1950
void __stdcall function_f1950(long vehicle_index, real throttle_input, short new_gear)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);

	if (VEHICLE_MAX_ANGULAR_VELOCITY(definition) > 0.001f)
	{
		real throttle = !vehicle->unknown385 && !function_f5d70(vehicle_index) ? throttle_input : 0.0f;

		real gear_ratio = function_f14f0(vehicle->gear, vehicle_index);
		real engine;

		if (VEHICLE_MAX_ANGULAR_VELOCITY(definition) * -0.5f > vehicle->unknown378)
		{
			engine = VEHICLE_MAX_ANGULAR_VELOCITY(definition) * -0.5f;
		}
		else if (vehicle->unknown378 > VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 1.5f)
		{
			engine = VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 1.5f;
		}
		else
		{
			engine = vehicle->unknown378;
		}
		engine /= VEHICLE_MAX_ANGULAR_VELOCITY(definition);

		bool reversed = new_gear != -2 && (new_gear >= 1) != (vehicle->gear >= 1);

		vehicle->unknown380 += (throttle - engine) * (g_510c54->rate * 45.454544f);
		vehicle->unknown380 = PIN(vehicle->unknown380, 0.0f, 1.0f);
		if (reversed || (vehicle->gear == NONE && new_gear != vehicle->gear))
		{
			vehicle->gear = (char)new_gear;
			vehicle->unknown38c = (char)0xfe;
			vehicle->unknown386 = 0;
			vehicle->unknown387 = 0;
			vehicle->unknown385 = 0;
			vehicle->unknown388 = 0;
		}
		else if (vehicle->unknown38c == (char)0xfe && new_gear == vehicle->gear)
		{
			vehicle->unknown385 = 0;
			vehicle->unknown386 = (byte)MIN(vehicle->unknown386 + 1, 0xff);
			if (vehicle->gear >= 0)
			{
				s_vehicle_gear *definition_gear = &VEHICLE_GEARS(definition)[vehicle->gear];

				if (engine > definition_gear->engine_upshift_scale)
				{
					vehicle->unknown387 = (byte)MIN(vehicle->unknown387 + 1, 0xff);
				}
				else
				{
					vehicle->unknown387 = 0;
				}
				if (definition_gear->engine_downshift_scale > engine)
				{
					vehicle->unknown388 = (byte)MIN(vehicle->unknown388 + 1, 0xff);
				}
				else
				{
					vehicle->unknown388 = 0;
				}
			}
		}
		else
		{
			long delay = real_truncate(g_510c54->field_2_3 * 0.05f);

			if (delay <= 1)
			{
				delay = 1;
			}
			if (!vehicle->unknown385)
			{
				vehicle->unknown38c = (char)new_gear;
				vehicle->unknown385 = 1;
			}
			else if (vehicle->unknown385 == delay)
			{
				vehicle->gear = vehicle->unknown38c;
				vehicle->unknown38c = (char)0xfe;
				vehicle->unknown386 = 0;
				vehicle->unknown387 = 0;
				vehicle->unknown385 = 0;
				vehicle->unknown388 = 0;
			}
			else
			{
				vehicle->unknown385++;
			}
		}
		if (vehicle->gear >= 0)
		{
			real torque = function_f1540(vehicle_index, vehicle->gear, vehicle->unknown378);
			real angular_velocity = vehicle->unknown378 +
				(vehicle->unknown37c * gear_ratio + torque) / *(real *)(definition + 0x260) * g_510c54->rate;

			vehicle->unknown378 = angular_velocity;
			vehicle->unknown378 = PIN(angular_velocity, VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 0.05f,
				VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 1.3f);
		}
		else
		{
			vehicle->unknown378 *= 0.95f;
		}
	}
}

/* takes the engine's load torque from the physics state, and stops the
   upshift timer while no contact touches the ground */
// @retail 0xf1c80
void function_f1c80(long vehicle_index, s_vehicle_physics_state *state)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	long count;
	long i;

	vehicle->unknown37c = state->unknown70;
	count = state->definition->contact_count;
	for (i = 0; i < count; i++)
	{
		if (state->contacts[i].unknown80)
		{
			return;
		}
	}
	vehicle->unknown387 = 0;
}

real function_f5e00(long vehicle_index);
void function_f5ee0(long vehicle_index);

/* the physics of type 0 vehicles (the differential-steered ones): the
   engine drives each side's contacts, the turn trading torque between
   the sides */
// @retail 0xf1cd0
void __stdcall function_f1cd0(long vehicle_index, s_vehicle_physics_state *state)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real magnitude = (real)sqrt(vehicle->throttle * vehicle->throttle + vehicle->steering * vehicle->steering +
		vehicle->unknown1b8 * vehicle->unknown1b8);
	real maximum_turn = 0.001f > *(real *)(definition + 0x22c) ? 0.001f : *(real *)(definition + 0x22c);
	real turn = vehicle->turn / maximum_turn;
	real gear_throttle = MIN(magnitude, 1.0f);
	real engine_throttle = MIN(magnitude, 1.0f);
	real torque;
	real torque_magnitude;
	real left;
	real right;
	long i;

	function_f1950(vehicle_index, engine_throttle, function_f1740(vehicle_index, gear_throttle));
	short gear = vehicle->gear;
	if (gear == NONE)
		torque = 0.0f;
	else
		torque = function_f14f0(gear, vehicle_index) * VEHICLE_GET(vehicle_index)->unknown378;
	torque_magnitude = torque >= 0.0f ? torque : 0.0f - torque;

	left = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque - torque_magnitude * turn;
	right = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque + torque_magnitude * turn;
	for (i = 0; i < state->definition->contact_count; i++)
	{
		state->contacts[i].torque = (i & 1) ? right : left;
	}
	/* each side's wheel spin advanced, then wrapped into the definition's period */
	vehicle->unknown368 += left * dt;
	vehicle->unknown368 = *(real *)(definition + 0x20c) > 0.001f ?
		(real)fmod(vehicle->unknown368 > 0.0f ? vehicle->unknown368 : vehicle->unknown368 + *(real *)(definition + 0x20c),
			*(real *)(definition + 0x20c)) : 0.0f;
	vehicle->unknown36c += right * dt;
	vehicle->unknown36c = *(real *)(definition + 0x20c) > 0.001f ?
		(real)fmod(vehicle->unknown36c > 0.0f ? vehicle->unknown36c : vehicle->unknown36c + *(real *)(definition + 0x20c),
			*(real *)(definition + 0x20c)) : 0.0f;
	function_2056e0(vehicle_index, state, function_f5e00(vehicle_index), NULL, NULL);
	function_f1c80(vehicle_index, state);
	function_f5ee0(vehicle_index);
}

void function_f5bf0(long vehicle_index);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

/* a wheel's spin advanced by its angular velocity, wrapped into the
   definition's period */
PRIVATE inline void vehicle_wheel_spin_wrap(real *spin, real delta, real const *period)
{
	*spin += delta;
	*spin = *period > 0.001f ? (real)fmod(*spin, *period) : 0.0f;
	if (0.0f > *spin)
	{
		*spin += *period;
	}
}

/* the contact turned about its up axis by half the steering angle, in the
   state's frame */
PRIVATE inline void vehicle_contact_steer(s_vehicle_physics_state *state, s_vehicle_contact *contact, real sine,
	real cosine, quaternionf *rotation)
{
	rotation->i = 0.0f;
	rotation->j = 0.0f;
	rotation->k = sine;
	rotation->w = cosine;
	function_141e10(&contact->matrix.rotation, rotation);
	contact->matrix.position.x = 0.0f;
	contact->matrix.position.y = 0.0f;
	contact->matrix.position.z = 0.0f;
	contact->matrix.scale = 1.0f;
	function_142a60(&state->matrix, &contact->matrix, &contact->matrix);
}

/* the physics of type 1 vehicles (the wheel-steered ones): the engine's
   torque to every contact, the steered contacts turned, and the wheels'
   spin */
// score 10 (was 10): frame 0x30, 991-byte length and ret 8 agree; torque and loop-offset homes
// remain swapped, plus the first fmod advance reload/zero order.
// Before the frame repair (29): the spin wrap helper now takes the field's address and the advance (it adds,
// wraps and re-reads the period through its pointer), which gives retail's spin tail. Left: frame
// 0x34 versus 0x30 (one never-referenced dword at the frame top) with the torque and loop-offset
// slots swapped; every other differing line is that 4-byte offset. Round 182: the helper in place
// through pointers without the advance 89 (976 bytes, the fmod argument re-reads the field); the
// advance inside the helper 29 (kept); braking through the existing 0xf5d70 call 321 (out of line
// under /Ob1; forced inline 29, identical to the written-out test, but 0xf1950/0xf5ee0/0xf6d10 then
// lose their out-of-line calls); torque renamed for slot order 29 (identical). Diagnostics: the extra
// dword stays without the throttle/0xf1740 call, the spin tail, the steering calls or the gear
// call, and with a constant braking argument; it goes (frame 0x30) only without the 0x2056e0 call.
// It is not 0xefde0's aligned frame passed down (with 0xefde0 not calling it, still 0x34). Also 29
// each (identical): the rotation at function scope, torque/i/braking declared first, braking as one
// boolean expression.
// Earlier at 71 (was 189): the contact definition as an element pointer, both steering
// the frame is now retail's 0x30 (round 114); before that, frame 0x34 versus
// 0x30; native gear and incoming roles agree;
// the contact definition as an element pointer, both steering
// bits read early into named locals, and the spin wrap helper changed to take the
// spin value and return the wrapped one; the contact loop now matches retail
// instruction for instruction. Left: frame 0x34 versus 0x30 (one never-referenced
// dword at the frame top) with the torque and loop-offset slots swapped, and the
// spin tail spilling the period instead of re-reading it from the definition.
// Earlier at 189: frame 0x34 versus 0x30; native gear and incoming roles agree;
// torque/contact homes, flag extraction and spin allocation remain. Shared workspace/in-place spin
// and original bits 304/257/201/272/216; native short gear on the two legal graphs 187/202.
// Early one-use bit forms 334/92/121 and their short-gear/return-value forms
// 155/172/101/161/238/263 restored. Bit expressions at their original use point,
// after contact stores, value/pointer period and original/short gear 201/187/216/198; t09 retained after plain confirmation.
// DL180: entry-zero torque/dword flags 274; selection-zero torque/dword flags 301; entry-zero
// torque/word flags 273; selection-zero torque/word flags 300; entry-zero torque/byte flags 284;
// selection-zero torque/byte flags 311; entry-zero torque/use-point bits 274; selection-zero
// torque/use-point bits 301; spin forceinline 189; steer forceinline 189; both forceinline 189;
// spin noinline 425; steer noinline 350; both noinline 545. Original executable body restored.
// J198: torque name/throttle scope/quaternion loop-entry/function-entry declarations 189 each;
// native contact cursor with original/loop/entry/combined lifetimes 245 each.
// Value-input spin result, value/field periods and short/byte gear 182/261/197/262;
// separate exhaustive/default-zero results with value/field periods 189/261/208/258.
// Seven-point value-input gain below margin; frame, torque/contact and period homes remain.
// Round 49: torque/counter declaration swap 189; steering helper with a matrix argument 189;
// both 189; const spin 189; late torque/late counter/both 189 each; behavioural counter name 189.
// Original target and steering helper restored.
// Round 50: the contact definition as an element pointer read twice (legal bit tests) 187 (below
// the margin). On the early-bit graph (named bits after the flags read, plain if/else-if gear) 100
// at 991 bytes; with the short gear 156; plus the element pointer (retail's dead `add eax, edi`) 62:
// the loop then equals retail but for slots. Spin wrap there: period through a pointer 89 (976
// bytes); value-in/value-out helper 241 (`ret 4`: vehicle_index moves to a register); value copy
// inside the helper 93 (still `fld` of the field); the wrap with direct field names 29 at 991 bytes
// (spin code then retail's: value temp, period re-read, field stored before the sign test; not
// kept: early bits and a hand-written helper need a match). Left at 29: frame 0x34 against 0x30
// (an extra never-referenced dword at the top) and torque/loop-offset slots swapped (retail shares
// the throttle slot with torque; compile-only does too, LTCG pairs it with the offset). i before
// torque, scoped throttle with late torque, a named gear-select result, the throttle magnitude
// passed inline: 29 each. Original body restored.
// Round 114 (lane 6): the 0x34 frame was 8-byte alignment that LTCG pushed up from 0x2056e0, whose
// inline cap helper named a `double scale`; passing the scale straight to scale_vector_by_double
// (0x2056e0 still matches) gives retail's 0x30 frame here (189 -> 187 on this body). On round 50's D9
// form (early named bits, the element pointer, the wrap written on the fields; 29 before) that is 10,
// and with `torque * dt` written in both adds (no spin local) 8 at 991 bytes: only the torque and
// loop-offset slots stay swapped (torque 0x14 / offset 0x18 sharing the throttle's, retail the other
// way round). None of those forms is kept without a byte match. On the 8 form: i or torque declared
// first 8/8, torque declared late 8, throttle in a block 8, throttle passed inline 8, a while loop 8,
// plain else-if gear 177, torque preset to 0 270, brake store before the torque store 19, torque
// stored first 32, contact pointer before the definition pointer 9, memcpy for the rotation copy 71,
// the existing VEHICLE_DWORD_BITS view for the bits (early 8, at use 180), torque from an inline
// helper returning from each arm 8 / single exit 71, spin named before the calls 81. Wrap helper
// forms: period through a pointer 84, by reference 84, forceinline 84; the helper also doing the add
// with a named spin 10 (the D9 spin code), with `torque * dt` per call 70, with torque and dt passed
// 70; on this body (period pointer, add inside) 183. Diagnostics: extra torque uses in or after the
// loop, throttle and torque as one variable (10), and removing any single call never move torque
// above the offset. Source permuter on the 8 form: 6 tries in 616 s, nothing lower. Original body
// restored.
// Round 103 (the frame is now 0x30; left: torque at 0x14 and the loop offset at 0x18 sharing the throttle's
// slot, retail the other way round (8), and after the first fmod the period zero `xorps xmm2` comes before
// the advance reload, retail after (2)): the gear arm as one ternary 10 (identical); the second steering
// bit read from the element expression 242 (frame 0x34); torque assigned once,
// `braking || vehicle->gear == NONE ? 0 : ...` 192 and with the short gear read first 193 (both give
// torque retail's slot, shared with the throttle, but merge the two zero paths and put the computed path
// first); a nested ternary with the gear assigned inside it 10 (identical). Diagnostics: without the
// in-loop torque store torque takes retail's slot; without its use after the loop it does not. So torque
// weighs more than the loop offset here (in-loop use and its three definitions); retail's must weigh less.
// J220: non-braking producer first 23, active gear first 18, both 23;
// advance before contact loop 168, with non-braking first 181. Definition cursor 168,
// both real cursors 210, definition cursor/non-braking first 181.
// Existing throttle temporary named before/after torque 10/10.
// Round 13 (lane 17), at 10 (the same 8 slot lines and the 2-line zero/reload order after the first fmod): torque
// renamed 10 (identical); the period as one named pointer for both wraps 65; the contact and
// contact-definition pointers declared at function scope 10; throttle declared with the other locals 10; the
// rotation declared in each steering branch 186 (frame 0x40).
// Round 81: contact-definition/contact/both names 10 each; `axle_torque` 10; `const delta` and
// `real *const spin` 10 each; a local advance name 10; torque/braking declarations swapped 10;
// definition/contact/both pointer arithmetic 10 each; a spin reference and helper advance/spin names 10 each;
// throttle/torque joint declarations in either order 10 each; the wrap helper defined after its caller 10.
// Opening body restored.
// Round 134 (lane 6), at 10: the wrap helper's ternary as if/else 54 (993 bytes); the loop counter declared in
// the for statement 10; the gear arm as `torque = 0.0f; if (gear != NONE) torque = ...;` (0xf1cd0's shape) 198
// (975 bytes); the contact's torque and brake stores through a two-statement inline helper 10; the wrap's sign
// test as `*spin < 0.0f` 10. A compile-only (no /GL) listing shares torque with the throttle as retail does and
// homes the loop offset in a parameter slot, so it does not reproduce the LTCG frame.
// Round 43 (lane 17, fast), at 10: torque named to sort after the compiler's `tv` temporaries (`wheel_torque`) 10
// (identical: names do not order this frame); both steering bits read from the element expression (no pointer)
// 242 (frame 0x34); a const contact-definition pointer 10; the gear read at each use in an else-if chain 176; the
// braking test reading the vehicle lookup at each use (no `current`) 10. Original body restored.
// Round 92 (lane 9), at 10: both wraps written on their fields (as 0xf1cd0 now matches) with the named spin 10
// (identical), with `torque * dt` in both adds 8 (only the eight torque/offset slot lines; the spin local written out
// needs a match); a wrap-only helper `(real *spin, real const *period)` with the caller doing `+=` 80 (976 bytes), with
// `torque * dt` 84; on the 8 form: dead pointer locals (diagnostic) 1-8 give 8/10/10/10/10/10/10/10 (the slots never
// move), vehicle_contact_steer written out at both sites 10.
// Round 78 (lane 10, fast): the throttle magnitude held in `torque` itself (diagnostic) 12, the joint variable
// still at 0x14 and the loop offset at 0x18; the throttle through the file's vehicle_magnitude 128; the contact
// flags read once into a dword local 160; vehicle_wheel_spin_wrap or vehicle_contact_steer declared
// __forceinline 10/10 (same code); a bare block around the second lookup and the braking test 10 (same code).
// Round 95 (lane 9), at 10: `state->contacts[i].torque = torque;` in the loop (0xf1cd0's spelling) 8 (the 2-line
// zero/reload order becomes retail's, the slots stay; mixed access); `if (!braking)` with the zero arm second 23;
// the gear test `!= NONE` with the computed arm first 18; the wrap helper called with `torque * dt` 70, with the
// contacts store too 71; torque, throttle or i renamed to sort first or last 10 each (spill slots ignore names); a
// tick-length getter for dt 10, for dt and the braking test 65.
// Round 84 (lane 10, fast), at 10: every contact access written `state->contacts[i].x` (no contact pointer) 46; the
// half angle written into both trig calls 65; braking preset false and set by the test 222; the spin as `dt * torque`
// 10 (same code); dt read after the vehicle and definition lookups 192.
// m3 lane 1 r95 (fast), at 10: re-measured round 95's `state->contacts[i].torque = torque;` 8 (only the eight torque/offset
// slot lines left; a mixed spelling, kept only on a match); on it: new, the braking store through the array too 22; the
// permuter (926 s, 23 tries) best 8; re-measured round 114's brake store before the torque store 19 and spin named before
// the calls 81 (torque still at 0x14, the offset at 0x18). New on the kept body: the gear product written field first
// (`unknown378 * function_f14f0(...)`) 10; diagnostics that change torque's use count (behaviour kept, extra code): the
// contact store as `braking ? 0.0f : torque` 166 (997 B), the spin as `braking ? 0.0f : torque * dt` 233 (994 B), a dead
// `= 0.0f` initialiser on torque 10; in all three torque keeps 0x14 and the offset 0x18.
// Round 192 (m3 lane 4, fast), at 10: braking assigned straight from its test (no if/else) 10; the quaternion declared at
// function scope 10; cosf/sinf for the half-angle trig 10; the quaternion as a local of vehicle_contact_steer (no rotation
// parameter) 183 (934 bytes); fmodf in the wrap helper 10. Diagnostics: 1 or 2 dead inline-helper locals at the top, before
// the contact loop or before the spin 65/24 at each (the torque and loop-offset slots never move; only the spin tail's add
// operands do), so the slot swap is not a numbering phase.
// @retail 0xf1f80
void __stdcall function_f1f80(long vehicle_index, s_vehicle_physics_state *state)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real half_angle = vehicle->steering_angle * 0.5f;
	real cosine = (real)cos(half_angle);
	real sine = (real)sin(half_angle);
	bool braking;
	real torque;
	long i;

	function_f5bf0(vehicle_index);

	s_vehicle *current = VEHICLE_GET(vehicle_index);

	if (current->unknown384 && (0.5f > current->unknown384 * g_510c54->rate || (current->control_flags & 2)))
	{
		braking = true;
	}
	else
	{
		braking = false;
	}

	real throttle = vehicle->throttle >= 0.0f ? vehicle->throttle : 0.0f - vehicle->throttle;

	function_f1950(vehicle_index, throttle, function_f1740(vehicle_index, vehicle->throttle));
	if (braking)
	{
		torque = 0.0f;
	}
	else
	{
		short gear = vehicle->gear;

		if (gear == NONE)
		{
			torque = 0.0f;
		}
		else
		{
			torque = function_f14f0(gear, vehicle_index) * VEHICLE_GET(vehicle_index)->unknown378;
		}
	}
	for (i = 0; i < state->definition->contact_count; i++)
	{
		s_vehicle_contact_definition *contact_definition = &state->definition->contacts[i];
		s_vehicle_contact *contact = &state->contacts[i];
		quaternionf rotation;
		bool turn_left = TEST_FIELD_BIT((contact_definition->flags >> 2) & 1);
		bool turn_right = TEST_FIELD_BIT((contact_definition->flags >> 3) & 1);

		contact->torque = torque;
		if (braking)
		{
			contact->unknown83 = true;
		}
		if (turn_left)
		{
			vehicle_contact_steer(state, contact, sine, cosine, &rotation);
		}
		else if (turn_right)
		{
			vehicle_contact_steer(state, contact, 0.0f - sine, cosine, &rotation);
		}
		else
		{
			contact->matrix.rotation = state->matrix.rotation;
		}
	}
	function_2056e0(vehicle_index, state, function_f5e00(vehicle_index), NULL, NULL);
	function_f1c80(vehicle_index, state);
	function_f5ee0(vehicle_index);

	real spin = torque * dt;

	vehicle_wheel_spin_wrap(&vehicle->unknown360, spin, (real const *)(definition + 0x20c));
	if (!(TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit5)))
	{
		vehicle_wheel_spin_wrap(&vehicle->unknown364, spin, (real const *)(definition + 0x20c));
	}
}

void function_141590(transform4x3f const *in, transform4x3f *out);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void function_11d790(quaternionf const *q, real *angle, vector3f *axis);

/* a real's magnitude by comparison (retail's comiss form, not fabs) */
PRIVATE inline real vehicle_magnitude(real value)
{
	return value >= 0.0f ? value : 0.0f - value;
}

/* a damping force against a speed, at most what stops it within a tick */
PRIVATE __forceinline real vehicle_friction(real speed, real friction, real scale, real ticks)
{
	real direction = speed > 0.0f ? 1.0f : -1.0f;
	real magnitude = speed >= 0.0f ? speed : 0.0f - speed;
	real force = scale * (friction >= 0.0f ? friction : 0.0f - friction) * direction;
	real limit = magnitude * ticks;

	return PIN(0.0f - force, 0.0f - limit, limit);
}

/* the turn rate multiplier while control flag 0x1000 is set (writable data in retail) */
real g_47ff30 = 2.5f;

/* the physics of type 3 vehicles (the hovering ones): forces toward the
   desired speeds along the facing, its left and its up, friction where a
   control is idle, and a torque that turns the vehicle to face its
   facing, banked into its turns */
// MiniMax B (11-13): no new form, no lower score (682): our frame is 0x100 where retail's is 0xfc (our mass local takes
// the extra slot); the lead that reads the mass field at each use reaches retail's frame but only scores 676, under the
// eight-point bar, and the entry XMM numbering, the maximum speed product's operand form and the force/torque scheduling
// remain.
// MiniMax G (11-12): no new form, no lower score (682); the mass field read at each use (no mass local) gives retail's
// frame 0xfc at 676 and 3248 bytes, and the reciprocal declared after the first clamp reaches 674 only by moving that
// declaration; left: the frame, the acceleration scale, linear and angular velocity and maximum speed homes, retail's
// 1.0 in XMM7 and the maximum speed product's operand order.
// MiniMax B (11-11): no new form, no lower score (682): the frame is still the first difference and every slot under
// it is displaced with it; I found no spelling of the projected-speed sums that moves the frame.
// MiniMax R (11-06): no new form, no lower score (730); the permuter (600 s, 40 tries, seed 5501) gave best 709 differing
// instructions against the 711 of the body, and its variants swap call arguments, so the file was restored; the column
// alignment puts the first differences at the frame (0x100 against 0xfc), the top slot
// (esp+0x108 against esp+0x104) and the first float home (esp+0x20 against esp+0x34).
// score 682 (was 682): earlier 699; Complete projected-speed running sums recover retail length 3244;
// Earlier at 682 (was 699). Complete projected-speed running sums recover retail length 3244;
// frame 0x100 versus retail 0xfc; scalar/vector/transform homes and force scheduling remain.
// Earlier at 699 (was 713): complete angular error in the torque output, then the existing in-place scale;
// 3229/3244 bytes, frame 0x100 versus retail 0xfc.
// Earlier at 713 (was 730): Native mutable turn multiplier restored; frame 0x100 versus retail 0xfc;
// scalar/vector homes and force/trig scheduling still differ; 3223 bytes (retail 3244).
// Earlier (730, was 676): frame 0x100, 3219/3244 bytes.
// Walked against retail 0xf2360-0xf3009 statement by statement (v1D, 2026-11-06): the arithmetic, offsets, conditions, callees
// and call arguments all agree with retail (constants checked by value in the XBE). Structure fixes: the damping-tick static is
// initialized once before the first friction test (retail 0xf27ba-0xf2818 tests and sets the guard before the speed test, not inside
// each branch: 811 for that alone); retail evaluates |speed fraction| - |turn| twice (0xf2b81-0xf2bc9), which MAX of the two
// vehicle_magnitude calls gives (same bytes as the written-out ternaries);
// the banked up vector stays in `up` and is copied into desired.up after desired.left is crossed (0xf2e2d-0xf2e79 copy [esp+0x20]);
// 0x142a60 writes a third transform (0xf2ed2: output at [esp+0xd8], inputs at 0xa4/0x70), so the relative transform is its own
// local. The friction clamp stays PIN: retail's 0xf28a4-0xf28b6 is the two compares PIN expands to; the same clamp written out as
// an if/else on the negated force gives 664 at 3246 bytes, but it is PIN's body by hand, so it is not kept. The native multiplication of
// 2.5 by the turn rate (0xf2afa) is recovered in T64. Still different: native copies of facing and velocity (i, j)
// before the slip product (0xf2b22-0xf2b36); frame 0xfc (one slot fewer).
// Tried on top (v1D): friction scale applied after the direction (0xf288d) 811 (the compiler reassociates, same bytes); turn limits
// through an inline helper with pointer outputs 771 (folds the same); facing/velocity (i, j) as point2f copies 744 (was 715 at the time); ba1d0's second
// output written into `force` (same bytes); no s_ticks hoist with the other fixes 820.
// Earlier at 692 (was 692; earlier 706): Frame 0xd0 versus retail 0xfc; scalar/vector/transform homes and remaining force/trig scheduling differ.
// Earlier at 728 (was 773): frame 0xd0 versus 0xfc; controls and transform/scalar homes remain.
// Shared cross products 830 kept; shared friction/bank sums 879; both 867; shared
// normalize/mass scales 861; cross/scales 836; sums/scales 865; all 846; all with
// both point setters 846. Plain confirms four existing cross3f calls, unchanged call roles.
// Round 426: facing-at-use/boost-default/mass subsets 895/835/905/794/874/803/882, matrix homes 882; facing after boost/velocity 806/846, with default boost 833/860; each with mass 773 (kept)/821/803. Frame 0x10c remains; plain confirms 773.
// DL196: current-output/response/normalization subsets 773/775/744/759/728/759/744;
// desired output 773. Complete facing before/after velocity 856/856; complete
// force products 728, with late facing 856. Keep only current-output ownership
// and the normalization result; original facing reads and response observations retained.
// Round 45 (lane 1): `real climb = vehicle->unknown1b8;` for the elevation's compare and product 721 (retail loads
// the field once; a 7-point gain, under the 8-point bar, so not kept); a separate relative transform for
// 0x142a60's result (retail has three transforms at 0x70/0xa4/0xd8) 736 (frame 0x104, the transforms 8 bytes
// high), with climb 729; with climb, `boost * maximum`
// and `boost * vehicle->speed` 721 each (identical code), boost preset to 1.0f then the conditional 747, boost as
// one ternary 721 (identical).
// J221: Friction APIs 726; force producer 755; both 753; with distinct relative output friction/force/both 734/763/760; all with climb 753, velocity role names 760. Original transform plus friction APIs and the actual twice-used climb observation 719 fast/plain, kept; same fields, values, float groups and game-call tuples. All other forms restored.
// J224: Banked result in the existing up vector 725; directly in desired.up 706 (plain, kept).
// Each with distinct relative output 733/733; rotation-output role names with the first two
// 725/706; distinct output plus role names 707 for desired.up (the in-place-up comparison failed).
// Current matrix before banked products with late sine 710; with late cosine/sine pair 709.
// The distinct-output forms retain frame 0x104; selected desired.up uses 0xd0, versus retail 0xfc.
// Round 251: Angular-output/banking/conversion lifetime subsets 1/2/4/3/5/6/7 with a distinct relative
// transform 707 each, 3245/3244 bytes, frame 0x104; all three on the original in-place graph 706. Original
// complete body restored; retail frame 0xfc and scalar/transform homes remain.
// Round 319: Complete bounded forward/left/up speeds before reciprocal with distinct relative output, six producer orders 699/699/699/699/699/699; immutable 699; original output 692, kept and plain-confirmed. All fraction trees and original observations preserved.
// Round 322: Complete interleaved bound/fraction producers with reciprocal after first bound, six
// orders on distinct relative output 718/723/731/730/735/734; immutable retail order 718; original
// output 710. Original complete body restored; frame/scalar homes and friction/trig scheduling
// remain.
// F327: Distinct inverse output with the relative result in current/desired/inverse
// 701/700/703; inverse declared at entry 701/700/703; complete matrix role names
// with current/desired result 701/700. Original complete body restored; frame and scalar/transform homes remain.
// P267: coherent per-use mass/climb/acceleration fields on the distinct-output graph,
// subsets 1/2/4/3/5/6/7: 735/717/741/742/756/740/747; all on the original output 739.
// Mass-only reaches frame 0x100; acceleration-only and climb/acceleration reach retail 0xfc
// at 741/740. Every form regresses; original observations and complete body restored.
// AC481: desired.forward owns all facing observations; distinct relative output /
// existing staged torque APIs / both position setters, masks 0..7:
// 795/798/792/797/793/800/792/797. Original body restored; frame and scalar homes remain.
// F334: Banked desired.up via existing scale/add-scaled producers, separate versus
// consumed scale return, crossed with distinct relative output and shared force/torque
// mass scales: 728/728/738/738/728/728/738/738. Frames remain 0xd0/0x104;
// original full body restored. Retail frame 0xfc and scalar/vector homes remain.
// Round 544: Complete projected-up/force/torque setters, subsets 1/2/4/3/5/6/7,
// then all with independent relative output: 689/691/692/690/689/691/690/698.
// Best gains three; NOT kept under the gain gate. Original whole body restored.
// M327: Named squared magnitude / accumulated magnitude / completed short-vector fallback / existing normalization scale:
// 692/740/708/692 on original output, 699/748/710/699 with independent relative output.
// Original full body restored; frames remain 0xd0/0x104 versus retail 0xfc.
// M331: Existing projected-speed dot3f boundaries, subsets 1/2/4/3/5/6/7 then all with independent relative
// output: 717/692/715/717/704/715/720/728. NOT kept: no qualifying gain or byte match. Original complete
// body restored; retail frame 0xfc and scalar/transform scheduling remain.
// B385: Plain vector-array normalization/force/torque loops, subsets 1/2/4/3/5/6/7,
// 690/739/692/692/691/739/687; all with final mass-scaling loops 687.
// NOT kept: best gain five is below eight; original complete body restored.
// M333: Actual bound/reciprocal, directional-mass and projected-speed/force lexical
// homes, seven subsets: 692 each; all with independent relative output 699. Frames
// remain 0xd0/0x104 versus 0xfc; original full body and all producers restored.
// Round 555: Existing squared-length/dot/inline-normalizer/called-normalizer
// producers, original then independent relative output: 692/692/692/739/699/699/699/731.
// NOT kept: no byte match; changed normalization grouping/precision is exact-only.
// Original whole body, normalization and output graph restored.
// F350: Native definition control/linear/angular field domains, seven subsets and complete
// immutable receiver, 692 each. Existing staged cross products for side/bank/current/desired
// axes, side-bank/matrix pairs/all/all with independent relative output:
// 691/692/742/734/691/755/755/763. NOT kept: best gain one; frame remains 0xd0
// or 0x104 versus retail 0xfc. Original body, record view and all helpers restored.
// P297: Complete forward/left/up clamp-result arms, seven subsets, then all with
// independent relative output: 692/692/692/692/692/692/730/722. Original macro bounds/body restored; frame and homes remain.
// F354: Float CRT bank sine/cosine/both 689/689/692; distinct relative output
// 699/699/700; both with distinct inverse/current or desired result 702/699.
// NOT kept: best gain three is below eight; frames 0xd0/0x104 versus 0xfc; original body restored.
// P297: Complete forward/left/up clamp-result arms, seven subsets, then all with
// independent relative output: 692/692/692/692/692/692/730/722. Original macro bounds/body restored; frame and homes remain.
// F359: Full CRT cosine/sine/both returns with real conversion at bank products,
// original/separate relative output, then immutable pairs: 692/697/698/710/717/717/698/717.
// NOT kept: one ties and the others regress/lengthen; no exact storage repair. Original full body restored.
// P299: Planar facing/velocity/paired copies and paired existing 2D product, original
// then independent relative output: 737/737/764/764/745/745/769/769. NOT kept: native
// planar copy staging does not recover frame 0xfc; every form regresses. Original full body restored.
// F379: Exhaustive turn-rate policy results, default/branch bank ownership,
// special/normal arm priority and full/conditional control arms: 718/718/718/718/689/689/689/689.
// NOT kept: best gain three is below the gate; original body restored, frame 0xd0 versus 0xfc remains.
// Y579: Actual quaternion return nested/received, original/relative/inverse-current/
// inverse-desired output graphs: 690/689/698/698/700/700/699/699.
// NOT kept: best gain three; native 0xfc frame remains missing. Original full body restored.
// M376: Existing friction complete tested result, initialized result, repeated negative
// bound/result and entry-result declaration; original then distinct relative output:
// 692/722/741/692/700/730/749/700. NOT kept: no gain; frames stay 0xd0/0x104 versus native 0xfc.
// Original helper and complete body restored.
// Y594: Current/desired/both matrix unit-scale/zero-position initializers and complete current aggregate,
// original/distinct relative output: 677/721/709/739/685/725/700/749. NOT kept: nonnative aggregate clears;
// 677 reaches native length but inserts rep stosd. Current axes and three position-zero stores before
// left cross, original/distinct output 676/682; original output kept at 3244 bytes, plain confirmed.
// Y598: Actual velocity/basis/applied-force lexical identities, separate and complete,
// original then distinct relative output: 676/676/676/676/682/682/682/682. NOT kept:
// no exact slot repair; frames remain 0xd0/0x104 versus 0xfc. Original complete body restored.
// P336: Normalized up/lateral/bank-axis in actual matrix rows, seven separate-output
// graphs then all with in-place output: 720/738/776/754/797/772/782/774. Bank-axis alone
// recovers frame 0xfc but grows to 3311/3244 and regresses; all equality-only forms restored.
// M381: Actual projected-up/force/torque aggregate construction, subsets 1..7, then
// all with independent relative output: 692/692/692/692/692/692/692/699. NOT kept: ties or regression;
// original vector producers, inverse/relative ownership and complete body restored.
// N378: Existing complete basis constructors current/desired/both/distinct output, inline then force-inline:
// 742/734/755/763/742/734/755/763. NOT kept: no byte equality or qualifying native-shape gain;
// original executable source restored; other constructor users regress; native frame 0xfc and scalar/transform homes remain.
// P347: Complete branch-owned turn/speed-fraction/slip-scale results, seven subsets then all with independent relative output:
// 676/676/676/676/676/676/676/682. NOT kept: Seven ties and one regression; frames remain 0xd0/0x104 versus native 0xfc. Original complete body restored.
// J259: Separate force multiplier with time-factor ownership, independent relative output and uniform mass reads: 711/811/708/803/705/784/711/774; three precomputed friction results with mass snapshots/reads and existing final-vector scaling: 683/711/690/711.
// NOT kept: no eligible native repair; opening executable source restored. Native frame 0xfc and first difference +0x66 recovered at 774; scalar homes and force scheduling still differ. Association, initialization and early one-use results require exact equality; all restored.
// Permuter, 30 minutes (11-06, on the form before PIN came back): 651 from 662, only with a volatile definition pointer and
// vehicle_friction's arguments swapped. Not kept.
// T64: Native mutable 2.5 turn multiplier alone 730 -> 713, plain confirmed (3223/3244).
// Current-matrix identity/declaration/both 713/713/713; complete force/torque/up setters
// 712/713/731; force+torque 712. NOT kept: one-point leads or nonnative storage;
// only the actual data-object read remains, with every original call and math tree.
// P361: Angular rate part / lower bound / both 713/720/711; complete error alone / with rate /
// with bound / with both 713/713/720/713. NOT kept: the two-point combined lead is below the gain gate.
// Complete error before common scale: extra vector / direct torque products / existing scale /
// scale-expression argument 699/699/699/699. Kept only the last, with the actual torque output
// and the existing in-place scale, no extra vector or one-use scale local. Retail 0xf2f4c..0xf2f83
// finishes all three errors before applying their common gain; source trees and game calls agree.
// Plain whole-program check confirms 699; all other measurements and baseline matches survive.
// P364: Complete directional energy/mass producers, mutable/const immediate/phased
// squares and four accumulator boundaries: 697/697/697/697/699/721/698/699. NOT kept: no native
// frame/producer equality; best gains two. Original full body restored, including the retained torque scale.
// P367: Discarded angular-output workspace reused for up/left/bank axis/rotation
// axis/force/torque/desired-left/current-left: 744/699/699/684/699/693/696/696.
// NOT kept: 684 reaches native length but grows frame to 0x10c versus 0xfc and
// shares native-distinct query/axis homes; other leads miss the gain gate.
// Actual query-scope ending on this graph 699. Original three transforms,
// independent outputs, common torque scale and complete body restored.
// P369: Coherent mass-field reads at every use 696, native frame 0xfc and 3244 bytes, first +0x66; NOT kept: gain
// three. Existing full force/torque/normalization scale boundaries, masks 1/2/4/3/5/6/7 and nested torque output:
// 699/699/699/699/699/699/699/701. Coherent mass with force/torque/both scale calls 709/709/719; response/query/all
// axis-output identities each 696. All restored; original 699, frame 0x100 and 3229 bytes retained.
// P381: Complete selected-bank branch/conditional-arm/expression/slip-default results,
// original magnitudes then existing magnitude calls: 722/722/722/690/722/722/722/690.
// NOT kept: 690 reaches native length but keeps frame 0x100/0xfc and adds a slip
// publication before the selection, unlike retail's carried control bank. Original complete body restored.
// T73: Complete projection product parts, mutable/const immediate/phased 700/700/700/700;
// existing-result running sums 682 plain, kept; parts plus sums 702; bare and phased sums 682/682.
// Only the minimal complete running sums remain: native up-speed has successive additions and stores;
// original operands, sum trees, observations and calls fixed. Exact length 3244, original frame 0x100.
// Round 147 (m3 lane 7, fast): retail reads the mass field once for the three directional masses and once
// each for the force and torque scales, and gives mass no home (ours spills it: frame 0x100). The field read at
// each use (re-measure of P369) 676 at 3248 bytes, frame 0xfc, first +0x66 (gain 6, not kept); on it: bounds
// interleaved with the reciprocal (re-measure of round 322) 682 (3249 B, masked aligned 450 -> 444);
// vector3f_scale of force and torque by the field (re-measure of P369's scale calls) 689 (3228 B; the end then
// loads the field once per vector, as retail), with the torque scale argument as `state->unknown6c *
// angular_scale` 689 (same code); a named pointer for definition+0x22c (approach rates, then the turn limit)
// 732; a block-local mass for the three directional masses, the field at the end, 711; maximum_speed declared
// at its assignment 676 and the two velocities declared just before 0xba1d0 676 (same code). Left: retail keeps
// state in EBX and the rates pointer in EDX (ours the reverse), its force home sits at esp+0x14 below up (ours:
// up at 0x14, the acceleration scale at 0x20, force above), and its products with boost load the field first;
// on the field-per-use form, both boost products written boost first (re-measure of round 45) 676 (same code).
// Round 76 (m3 lane 14, fast): the mass field read at each use (no mass local; P369's form re-measured on the current
// body) 676, frame 0xfc and first difference +0x66
// (3248 bytes; retail reads the field once for the three mass products, once for the force scale and once for the
// torque scale); on that, the force and torque scaled through the existing vector3f_scale with the field as its scale
// 689 (3228 bytes; the tail then reads the field once per vector as retail, the torque products copy), the torque scale
// as `state->unknown6c * angular_scale` 676 (same code), the reciprocal of the maximum speed declared before the three
// clamps 693 (aligned closer), after the first clamp (retail computes it there) 674 at 3244 bytes (not kept: a moved
// declaration, needs a match); with the mass local, both scales through vector3f_scale 682 (same code). Still differs:
// retail keeps 1.0 in XMM7 across the 0xba1d0 call (ours XMM5/XMM6) and loads the definition field first for the
// maximum speed product (ours copies the boost). On the 676 form (re-measures of earlier boost-block spellings, now on
// the 0xfc frame): the maximum speed staged as the field then `*= boost` 722 (3224 bytes, same product shape), boost
// as one ternary 676 (same code), boost preset to 1.0f then the conditional 723, the boost product's operands swapped
// 676 (same code), `boost * vehicle->speed` 676 (same code), `boost * field` for the maximum speed 676 (same code).
// Retail's frame order (angular velocity, acceleration scale, maximum speed, linear velocity from the bottom) differs
// from ours (acceleration scale, angular, linear, maximum speed).
// Round 105 (m3 lane 16, fast), on the mass-field-per-use form (P369's, re-measured 676 at 3248; aligned 500,
// register-masked 327, slot-masked 245): vehicle_friction taking the friction field as `real const *` and reading it
// at each use 686 at 3248 (aligned 493, masked 284, slot-masked 200: retail's friction magnitude re-reads the field in
// its negation, and the approach rates then sit in EDX with state in EBX, as retail); that with the reciprocal
// declared after the first clamp 692 at 3244 (masked 275, slot-masked 192). Both are exact-only (a helper parameter
// type, a moved declaration). Also: the elevation product written `(select) * climb` as 0xeec90 does 676 (same code);
// a named `rates` pointer for definition+0x1f4 (maximum speed and the elevation bound), with or without its reuse for
// the approach rates, 732 at 3268; normalize3d(&up) for the up normalization 738 at 3275; the acceleration field read
// at each use 769 at 3206; vehicle_magnitude() inside vehicle_friction for either or both ternaries 676 (same code);
// the speed staged as `speed = vehicle->speed; speed *= boost;` 715 at 3220.
// Lead: the pointer friction form; left on it are the entry XMM numbering (retail keeps 1.0 in XMM7), the maximum
// speed product's operand form, the reciprocal's placement and the force/torque scheduling.
// Round 425: complete/negative-default/positive-default friction direction
// 682/688/688; complete/default-first speed magnitude 682/720; coefficient
// expression/complete arms 682/682; full force accumulator 707.
// NOT kept: no native-frame gain; original helper and body restored.
// P441: Complete parallel error/impulse/acceleration producers, mutable/const, then immediate/phased force accumulators: 699/699/700/700/700/700/682/682; native grouped mass reloads 714.
// NOT kept: Native frame 0xfc and acceleration/velocity homes remain; no eligible gain. Original complete body restored.
// P445: Signed-climb result, shared backward bound, initial normalization admission,
// native-order position setters, fallback setter, complete elevation arms, selected-bank magnitude provider
// and immutable time/response/maximum/mass coefficients:
// 716/765/699/682/682/721/687/682. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P447: Uniform friction admissions; boost/descent/ascent/mode/turn-boost/fallback admissions; immutable late angular coefficient:
// 682/682/682/734/682/684/699/682. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// MiniMax A (11-13): no new form tried this round, no lower score (682): the frame is 0x100 here and 0xfc in
// retail, so every local sits four bytes higher, and this build loads two constants at the top where retail
// loads one.
// MiniMax C (11-14): no new form, no lower score (682); the frame still reserves 0x100 against retail's
// 0xfc, and the boost scale still lives in xmm5/xmm6 where retail uses xmm6/xmm7.
// @retail 0xf2360
void __stdcall function_f2360(long vehicle_index, s_vehicle_physics_state *state)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real acceleration_scale = *(real *)(definition + 0x280);
	real boost;
	real maximum_speed;
	vector3f linear_velocity;
	vector3f angular_velocity;

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit4))
	{
		boost = function_d1210(vehicle_index) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
	}
	else
	{
		boost = 1.0f;
	}
	maximum_speed = *(real *)(definition + 0x1f4) * boost;
	vector3f const *facing = &vehicle->local_velocity;
	function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
	if (TEST_FIELD_BIT(VEHICLE_DWORD_BITS(vehicle->flags_134)->bit23))
	{
		vehicle->unknown1b8 = -1.0f;
	}
	else if (vehicle->control_flags & 2)
	{
		vehicle->unknown1b8 = 1.0f;
	}

	real climb = vehicle->unknown1b8;
	real elevation = climb *
		(0.0f > climb ? *(real *)(definition + 0x1f8) : *(real *)(definition + 0x1f4));

	vehicle_approach(&vehicle->unknown370, (real const *)(definition + 0x22c), elevation, dt);

	real speed = vehicle->speed * boost;
	real bounded_forward_speed = PIN(speed, 0.0f - maximum_speed, maximum_speed);
	real bounded_left_speed = PIN(vehicle->turn, 0.0f - maximum_speed, maximum_speed);
	real bounded_up_speed = PIN(vehicle->unknown370, 0.0f - maximum_speed, maximum_speed);
	real inverse_maximum = 1.0f / maximum_speed;
	real forward_fraction = bounded_forward_speed * inverse_maximum;
	real left_fraction = bounded_left_speed * inverse_maximum;
	real up_fraction = bounded_up_speed * inverse_maximum;
	vector3f up;

	up.i = -(facing->k * facing->i);
	up.j = -(facing->k * facing->j);
	up.k = 1.0f - facing->k * facing->k;

	real mass = vehicle->unknown25c;

	real forward_mass = mass * (forward_fraction * forward_fraction);
	real up_mass = mass * (up_fraction * up_fraction);
	real left_mass = mass * (left_fraction * left_fraction);
	real length = (real)sqrt(up.k * up.k + up.j * up.j + up.i * up.i);

	if (0.0001f > (real)fabs(length))
	{
		length = 0.0f;
	}
	else
	{
		real inverse = 1.0f / length;

		up.i = inverse * up.i;
		up.j = inverse * up.j;
		up.k = inverse * up.k;
	}
	if (length == 0.0f)
	{
		up.i = 1.0f;
		up.j = 0.0f;
		up.k = 0.0f;
	}

	vector3f left;

	cross3f(&up, facing, &left);

	real forward_speed = linear_velocity.j * facing->j;
	forward_speed += linear_velocity.i * facing->i;
	forward_speed += facing->k * linear_velocity.k;
	real up_speed = linear_velocity.k * up.k;
	up_speed += linear_velocity.j * up.j;
	up_speed += linear_velocity.i * up.i;
	real left_speed = linear_velocity.k * left.k;
	left_speed += linear_velocity.j * left.j;
	left_speed += linear_velocity.i * left.i;
	real left_force = (vehicle->turn - left_speed) * left_mass * acceleration_scale * state->unknown6c;
	real up_force = (vehicle->unknown370 - up_speed) * up_mass * acceleration_scale * state->unknown6c;
	real forward_force = (speed - forward_speed) * forward_mass * acceleration_scale * state->unknown6c;
	vector3f force;

	force.i = facing->i * forward_force + up.i * up_force + left.i * left_force;
	force.j = facing->j * forward_force + up.j * up_force + left.j * left_force;
	force.k = facing->k * forward_force + up.k * up_force + left.k * left_force;
	// Set once, before the first friction test (retail 0xf27ba-0xf2818).
	static real s_ticks = g_510c54->field_2_3 * 0.1f;

	if (vehicle->speed == 0.0f)
	{
		real friction = vehicle_friction(forward_speed, *(real *)(definition + 0x200),
			*(real *)(definition + 0x27c) * acceleration_scale, s_ticks) * state->unknown6c;

		vector3f_add_scaled(&force, facing, friction, &force);
	}
	if (vehicle->turn == 0.0f)
	{
		real friction = vehicle_friction(left_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c), s_ticks) *
			state->unknown6c;

		vector3f_add_scaled(&force, &left, friction, &force);
	}
	if (vehicle->unknown370 == 0.0f)
	{
		real friction = vehicle_friction(up_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c), s_ticks) *
			state->unknown6c;

		vector3f_add_scaled(&force, &up, friction, &force);
	}

	real angular_scale = *(real *)(definition + 0x270) * 15.0f;
	real maximum_turn_rate = 2.0943952f;
	real maximum_bank = 1.5707964f;

	if (*(short *)(definition + 0x218) == 4)
	{
		if (vehicle->control_flags & 0x1000)
		{
			maximum_turn_rate = g_47ff30 * maximum_turn_rate;
		}
		maximum_bank = 0.78539819f;
	}

	real turn = 0.001f > *(real *)(definition + 0x22c) ? 0.0f :
		vehicle->turn / (real)fabs(*(real *)(definition + 0x22c));
	real speed_fraction = 0.001f > maximum_speed ? 0.0f : inverse_maximum * speed;
	// Retail takes both magnitudes twice (0xf2b81-0xf2bc9), so they are not shared locals.
	real slip_scale = MAX(0.0f, vehicle_magnitude(speed_fraction) - vehicle_magnitude(turn));
	real slip = (linear_velocity.j * facing->i - facing->j * linear_velocity.i) / (real)fabs(maximum_speed) *
		slip_scale * maximum_bank;
	real bank = turn * maximum_bank * -0.25f;

	if ((bank >= 0.0f ? bank : 0.0f - bank) > maximum_bank * 0.075f)
	{
		if (0.0f > bank)
		{
			if (bank > slip)
			{
				bank = slip;
			}
		}
		else if (!(bank > slip))
		{
			bank = slip;
		}
	}
	else if ((slip >= 0.0f ? slip : 0.0f - slip) > (bank >= 0.0f ? bank : 0.0f - bank))
	{
		bank = slip;
	}
	bank = PIN(bank, 0.0f - maximum_bank, maximum_bank);

	real cosine = (real)cos(bank);
	real sine = (real)sin(bank);
	vector3f axis;
	transform4x3f desired;

	cross3f(facing, &up, &axis);
	up.i = cosine * up.i + axis.i * sine;
	up.j = cosine * up.j + axis.j * sine;
	up.k = cosine * up.k + axis.k * sine;

	transform4x3f transform;
	transform4x3f relative;
	quaternionf rotation;
	vector3f rotation_axis;
	real rotation_angle;

	transform.scale = 1.0f;
	transform.forward = vehicle->forward;
	transform.up = vehicle->up;
	transform.position.x = 0.0f;
	transform.position.y = 0.0f;
	transform.position.z = 0.0f;
	cross3f(&vehicle->up, &vehicle->forward, &transform.left);
	desired.scale = 1.0f;
	desired.forward = *facing;
	cross3f(&up, facing, &desired.left);
	desired.up = up;
	desired.position.x = 0.0f;
	desired.position.y = 0.0f;
	desired.position.z = 0.0f;
	function_141590(&transform, &transform);
	function_142a60(&desired, &transform, &relative);
	function_141f60(&relative.rotation, &rotation);
	function_11d790(&rotation, &rotation_angle, &rotation_axis);

	real rate = PIN(rotation_angle * maximum_turn_rate * 0.95492965f, 0.0f - maximum_turn_rate, maximum_turn_rate);
	vector3f torque;

	torque.i = rotation_axis.i * rate - vehicle->angular_velocity.i;
	torque.j = rotation_axis.j * rate - vehicle->angular_velocity.j;
	torque.k = rotation_axis.k * rate - vehicle->angular_velocity.k;
	vector3f_scale(&torque, angular_scale * state->unknown6c, &torque);
	force.i *= mass;
	force.j *= mass;
	force.k *= mass;
	torque.i *= mass;
	torque.j *= mass;
	torque.k *= mass;
	function_2056e0(vehicle_index, state, 0.0f, &force, &torque);
}

/* a pointer to a default 2d vector (0x468774) */
point2f *g_468774;

real normalize2d(point2f *v);
real function_30bf0(vector3f *v);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);
bool __stdcall function_a75d0(vector3f *vector, real maximum);
extern vector3f *g_4687a4;

/* a real's sign as an integer */
PRIVATE inline long vehicle_sign(real value)
{
	return value == 0.0f ? 0 : (0.0f > value ? -1 : 1);
}

/* how far an angle from straight ahead turns toward the side: 1 at 0, 0 at
   a right angle, and from there to -1 behind */
PRIVATE inline real vehicle_angle_fraction(real angle)
{
	return (1.5707964f - (real)fabs(angle)) * 0.63661975f;
}

/* the physics of type 4 vehicles (the walking and skating ones): forces
   toward the controls' speed in the facing's frame, a yaw torque toward the
   steering, torques that hold the vehicle upright while it is off the
   ground, and its boost */
// MiniMax B (11-13): no new form, no lower score (1000): our frame is a dword bigger than retail's 0x74 and the velocity
// and force blocks build differently from the head; the scaled values' homes stay blocked by 0x1427f0/0x142640 (the
// header's matrix_transform_vector has no scale step).
// MiniMax G (11-12): no new form, no lower score (1000); the frame is 0x80 against retail's 0x74 and the velocity and force
// homes are not one uniform shift; retail inlines 0x142640 and 0x1427f0 while this file calls them, and those helpers are
// in a file that is not mine, so the inlining cannot be reached from here.
// MiniMax B (11-11): no new form, no lower score (1000): the frame gap of twelve bytes and the mismatched homes stay;
// the velocity and the force are not the same pair of homes, so no single local accounts for it.
// MiniMax C (11-10): no new form tried, no lower score (1000); the frame is 0x80 against retail's 0x74 and the velocity and force homes between them are not one uniform shift, and the forms in the note below (the velocity as scalars, the acceleration producers, the contact results and the two helpers' inlining) all land above 1000.
// score 1000 (was 1052): Statement-by-statement walk against retail (2026-11-06, v1B) found no behaviour difference
// MiniMax R (11-06): no new form, no lower score (1000); the frame is 0x78 against retail's 0x74 and the homes
// between them are not one uniform shift (the parameter reloads sit 4 apart, the pair at esp+0x64 0x10 apart and the
// first float home 8 the other way), so it is not a single missing local; the column alignment of this round
// finds no earlier split to work from.
// score 1000 (was 1000): Earlier at 1000 (was 1052). Statement-by-statement walk against retail (2026-11-06, v1B) found no behaviour difference
// (field offsets, the 29 constants, condition directions, callee arguments, loop shapes and local types agree).
// Retail takes |pitch_slope|, |roll_slope|, the yaw |arg| and the |target| / |steering| pair in the yaw test by
// comparison (comiss; 0 - x), not x87 fabs: vehicle_magnitude() gives that (1052 -> 1009, 1092 instructions each);
// the yaw test as `|steering| < |target| * dt * 2` 1008; the ground-contact count loop outside its own `count > 0`
// with a second guard on the division, as retail tests count twice, 1000 (tail now agrees). Still differs: frame 0x78
// versus 0x74, which shifts every stack offset; retail calls 0xa75d0 (ours inlines it, also without its `inline`) and
// inlines 0x142640 (ours calls it, then spills acceleration to a copy); velocity and force homes.
// Tried this round: velocity as three scalars 1059, plus a matrix-scale local 1060; sign(steering) multiplied first in
// the yaw target (retail evaluates it first) 1024.
// MiniMax R (11-06): no new form, no lower score (1000); the frame is 0x78 against retail's 0x74 and the homes
// between them are not one uniform shift (the parameter reloads sit 4 apart, the pair at esp+0x64 0x10 apart and the
// first float home 8 the other way), so it is not a single missing local; the column alignment of this round
// finds no earlier split to work from.
// Existing inverse transform 1137; forward transform and accumulation 1053 kept;
// both 1238; shared cross products 1130; inverse/cross 1134; forward/cross 1056;
// all three 1222; all with mass scales 1254. Plain confirms the shared in-place
// forward transform and vector accumulation; original incoming-velocity calculation kept.
// Round 304: scalar incoming velocity/shared yaw/shared airborne producers in seven
// nonempty combinations 1094/1072/1064/1066/1062/1053/1089; all with force/velocity role names 1089.
// Original-graph persistent/planar/later/all native frame-object name families 1053/1053/1053/1053.
// Original body restored; the frame remains 0x80 versus 0x74.
// Round 44 (lane 11): the permutation count/pointer through the existing 0xba690 1052 (kept: removes the helper's
// body written by hand); the velocity transform through the existing 0x1427f0 1238 at 4511 bytes (out of line:
// its /Ob1 file; retail inlines it here, x, y and z in registers with only the forward and left rows stored);
// 0x1427f0 marked inline at its definition 1084 (frame 0x84) but matched 0x210770 then breaks (48); a
// __forceinline redeclaration here 1238 (ignored). The frame stays 0x80 against 0x74 (a flags lead).
// Round 469: Angular-output/scaled-velocity/heading-pair lifetime scopes in seven nonempty
// combinations, then all with input declared at consumption: 1053/1053/1053/1053/1053/1053/1053/1053.
// Every initializer, expression and call restored; frame 0x80 versus 0x74 remains.
// J224: Live zero-vector receiver 1095; existing setters for both initial outputs 1092;
// both 1106; incoming-velocity setter 1060; zero/incoming 1113; output/incoming 1089;
// all 1126; all with one real velocity scratch buffer 1127. Live-zero forms use frame
// 0x7c versus retail 0x74 but regress. Original complete executable body restored;
// two incomplete-image comparisons excluded and completed on exact archived sources.
// P252: Low-byte flag payload/full-width driven/full-width paired direction truths, subsets 1/2/4/3/5/6/7:
// 1053/1052/1095/1099/1080/1110/1080; all native byte carriers 1053. Subsets 4/3/5/6/7 shrink the frame to
// 0x7c but regress; original complete body and 0x80 frame restored.
// J238: Queried-velocity ownership 1056; complete native scale producers 1061; actual point-output
// cursor 1058; query/scales 1066; query/point 1046 at retail length (six-point gain rejected);
// scales/point 1066; all 1054; all plus contact-count cursor 1054. Coordinated forward-transform inline
// declaration 1174 loses matched 0x16c2f0 (107) and is restored. Original complete body and
// declarations restored; frame 0x80 versus 0x74 and velocity/force homes remain.
// B358: Named repeated pitch/roll scales / one stable active-phase admission /
// separate native point-loop indices, subsets 1/2/4/3/5/6/7:
// 1067/1102/1052/1091/1067/1102/1091; all with paired shared projections 1091.
// Frame remains 0x80 versus 0x74. Complete original body and all evaluations restored.
// F329: Shared normalized forward, shared lateral-axis storage and uniform definition
// fields, seven subsets: 1053/1053/1176/1053/1188/1212/1197; all with shared-role
// names 1197. Forward storage reaches retail length but uses frame 0x84. All restored;
// original frame 0x80 versus 0x74 and force/velocity homes remain.
// N322: Scalar control/upright-target/upright-error storage, subsets 1/2/4/3/5/6/7:
// 1095/1076/1076/1061/1053/1069/1061; all plus scalar lateral storage 1077.
// Control/error storage reaches retail length with frame 0x84 but regresses; original body restored.
// F344: Completed speed/acceleration-policy/yaw-policy scopes, seven nonempty
// subsets, and completed airborne target/axis/error scope: 1052 each. Frame stays
// 0x80 versus 0x74; original complete body restored. Extra scope-only probe 1052
// excluded from eight-trial credit because it closes no additional local lifetime.
// DH308: Existing integer sign helper zero-result/positive-result/nested-result/flat-result/early-return/
// nonzero-result/negative-first correction/negative-first conditional: 1057/1049/1052/1052/1052/1057/1051/1052.
// NOT kept: best gain is three points; all retain frame 0x80 versus 0x74. Original helper and body restored.
// DL251: Plain yaw/airborne torque component loops with long/none, none/long, long/long,
// unsigned/none, none/unsigned, unsigned/unsigned, long/unsigned and unsigned/long ordinals; frame
// remains 0x80 versus 0x74: 1096/1080/1113/1096/1080/1113/1113/1113.
// Original complete source restored; no eligible gain.
// N335: Native inverse-scale/paired damping/final mass loops, subsets 1/2/4/3/5/6/7:
// 1080/1068/1091/1080/1086/1102/1088; complete unsigned loops 1088. All regress with frame 0x80.
// Original body restored; velocity and force homes remain.
// Round 555: Existing setters for acceleration, both lateral axes and boost
// direction, seven subsets; all plus yaw update: 1052/1060/1053/1060/1053/1060/1060/1060.
// Original complete body restored; frame/force/velocity ownership still differs.
// P297: Existing ground/yaw/boost dot3f producers, seven subsets, then all with the
// angle producer forced inline: 1051/1071/1068/1071/1068/1071/1071/1071. NOT kept: changed float trees require equality; original body/helpers restored.
// F354: Live default-target receiver/repeated steering direction/fraction numerator,
// singles and pairs/all/all forced inline: 1077/1047/1053/1089/1060/1053/1105/1105.
// NOT kept: best gain five is below eight; frame stays 0x80 versus 0x74; original body/helper restored.
// DH323: Repeated clamp inputs at yaw/paired upright controls/final ground
// fraction, subsets 1/2/4/3/5/6/7 1068/1064/1052/1068/1068/1064/1068; all plus boost fade 1068.
// NOT kept: ties or regressions, frame remains 0x80 versus 0x74. Original
// complete expressions, helper contracts and body restored.
// P297: Existing ground/yaw/boost dot3f producers, seven subsets, then all with the
// angle producer forced inline: 1051/1071/1068/1071/1068/1071/1071/1071. NOT kept: changed float trees require equality; original body/helpers restored.
// M350: Existing angle producer magnitude mutable/const 1057/1057; complete
// result initialized/assigned/const 1052/1052/1052; magnitude plus result,
// result-first and both const 1053/1053/1053. Frame stays 0x80; original
// helper/body restored, with force/velocity homes still differing.
// DH325: Native force/torque/working-velocity memcpy subsets 1/2/4/3/5/6/7:
// 1052/1052/1052/1052/1052/1052/1052; complete memmove 1093. All restored;
// frame 0x80 versus 0x74 and original force/velocity homes remain.
// F379: Direct two-use driving/braking outcomes from the original control test,
// bool/byte/short/long and complements: 1057/1057/1056/1057/1057/1057/1056/1057.
// NOT kept: regressions; original whole body restored, frame 0x80 versus 0x74 and force/velocity homes remain.
// Y579: Conditional working-velocity value/branch receiver/conditional receiver/
// setter producers, original/query-owned output: 1095/1072/1055/1042/1116/1111/1111/1072.
// NOT kept: 1042 shortens to 4639/4666, away from retail; all retain frame 0x80. Original full body restored.
// Y579: Existing limiter inline summary removed: 1052, other user 433 -> 514.
// NOT kept: shared-user regression; original declaration and complete body restored.
// M376: Existing float CRT initial-angle/paired-heading/body-magnitude producers,
// Seven subsets, then angle-helper magnitude: 1053/1053/1056/1057/1053/1068/1050/1053.
// NOT kept: best 1050 gains only two; frame stays 0x80 versus 0x74.
// Original APIs, private helper and complete body restored.
// Y589: Complete branch-owned scalar velocity, nonunit/unit/captured/conditional
// producers, original or projection-end scope: 1036/1064/1058/1091 twice.
// NOT kept: frame remains 0x80; 1036 shortens to 4639/4666, farther from retail.
// Original full body, velocity values, helpers and call inputs restored.
// Y591: Initialized facing/forward/paired airborne planar objects, seven subsets,
// then all plus initialized control point: 1052 each. NOT kept: no gain; frame
// remains 0x80 versus 0x74. Original full body and helper contracts restored.
// Y594: Two-entry planar direction table, for/while long/dword/short/word
// normalization traversals: 1125/1125/1114/1114/1125/1125/1114/1114. NOT kept:
// frames 0x84/0x88 and lengths 4706/4694 move away from native 0x74/4666.
// Original complete body restored; incoming velocity and force homes remain.
// Y596: Complete planar projection pair/array, assigned/entry/initialized/readonly outputs:
// 1057/1057/1057/1057/1057/1057/1057/1057. NOT kept: no equality; frame stays 0x80 versus 0x74; original full body restored.
// Y598: Persistent default outputs, aggregate/scalar force/torque/both, then force/both assignments:
// 1074/1095/1078/1074/1095/1078/1052/1052. NOT kept: no exact constructor result.
// Original complete body restored; frame 0x80 versus 0x74 and velocity/force homes remain.
// P334: Native inverse/damping/paired-mass component-transform contracts,
// seven subsets then queried-velocity range copy: 1090/1105/1078/1074/1093/1138/1118/1138.
// NOT kept: no eligible native frame/velocity/force-home recovery; original complete body restored.
// Permuter, 30 minutes (11-06): 913 from 976 (fast), only with both atan2 calls' arguments swapped (a behaviour change) and
// a volatile heading_side. Not kept.
// P386: Final contact results seeded / complete / conditional, both test orders, then twice-used gaps:
// 1012/1000/1000/1019/1005/1005/1000/1010. NOT kept: ties or regressions; joined/reordered nonmatching stores rejected.
// Native frame 0x74 and velocity/output homes remain; original complete body restored.
// P396: Independent forward/side acceleration selections, complete conditional/braking-first
// single/paired/mixed producers: 1001/1000/1001/1093/1093/1001/1093/1093. NOT kept: ties or regressions;
// native frame/velocity/force homes remain. Original entire body restored.
// Round 165 (m3 lane 6, fast): retail calls 0xa75d0 and inlines 0x142640 (and 0x1427f0); ours the reverse. `inline` on
// 0x142640's definition 1233 at 4794 bytes (frame 0x7c: both helpers then inline here) and matched 0x16c2f0 inlines it too
// (107); 0xa75d0's definition without `inline` 1000 (still inlined here: its file auto-inlines; 0x1f0870 347 -> 492, both fast),
// so both decisions follow the helpers' file flags (a flags lead for the user, not a source form). The two acceleration
// scalings through vector3f_scale 1000 (same code, kept: an existing helper for its body written out); the inverse
// scale through vector3f_scale 1058 (4682 bytes); the transformed velocity as a vector3f local (all three rows) 1064
// (4682 bytes, frame 0x7c); velocity as the scalars x, y, z in 0x1427f0's order 1059 (the listed scalar form: retail's
// three loads, but the scaled values copy before the products), with `x *= inverse` 1059 (same code).
// Round 76 (m3 lane 14, fast): this file is /Ob1, so only functions marked inline are inlined here: 0xa75d0 is marked
// inline at its definition (its other callers need that), 0x142640 and 0x1427f0 are not (their file is held this round).
// Tried: a `__forceinline` declaration of 0x142640 in this file 1000 (ignored: still called); an `inline` declaration
// 1000 (ignored); the velocity read into three scalars as 0x1427f0 reads them and the two speeds written into a
// vector3f (retail keeps them in two adjacent slots, the helper's output with its third row dropped) 1079 (4634 bytes;
// the head then loads scale, 1.0 and x, y, z as retail, but the scaled values get copies at the join), with `x *= inverse`
// 1079; the existing 0x1427f0 called with that output 1174 (out of line; a re-measure of round 44's form). The header's
// matrix_transform_vector has no scale step, so it is not 0x142640. BLOCKED-BY 0x1427f0/0x142640 inlining.
// MiniMax A (11-13): no new form tried this round, no lower score (1000): the frame is 0x78 here and 0x74 in
// retail, and retail keeps the matrix products in the XMM registers where this build spills them to the stack
// and reads the parity flags with lahf, so the two bodies have different shapes from the first difference on.
// MiniMax C (11-14): no new form, no lower score (1000); the frame still reserves 0x78 against retail's
// 0x74, so every stack home sits four bytes higher.
// @retail 0xf3010
void __stdcall function_f3010(long vehicle_index, s_vehicle_physics_state *state, real steering)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real mass = vehicle->unknown25c;
	vector3f force = *g_4687a4;
	vector3f torque = *g_4687a4;
	vector3f linear_velocity;
	vector3f angular_velocity;
	long i;

	function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
	if (vehicle->up.k > -0.2f)
	{
		vector3f velocity = linear_velocity;
		point2f facing;
		point2f forward;
		point2f input;

		if (state->matrix.scale != 1.0f)
		{
			real inverse = 1.0f / state->matrix.scale;

			velocity.i *= inverse;
			velocity.j *= inverse;
			velocity.k *= inverse;
		}

		real forward_speed = state->matrix.forward.k * velocity.k + state->matrix.forward.j * velocity.j +
			state->matrix.forward.i * velocity.i;
		real left_speed = state->matrix.left.k * velocity.k + state->matrix.left.j * velocity.j +
			state->matrix.left.i * velocity.i;

		facing.x = vehicle->local_velocity.i;
		facing.y = vehicle->local_velocity.j;
		normalize2d(&facing);
		forward.x = vehicle->forward.i;
		forward.y = vehicle->forward.j;
		normalize2d(&forward);

		real cosine = forward.x * facing.x + forward.y * facing.y;
		real sine = facing.y * forward.x - forward.y * facing.x;

		input.y = cosine * vehicle->steering + sine * vehicle->throttle;
		input.x = cosine * vehicle->throttle - sine * vehicle->steering;
		if (vehicle->unknown370 > 0.0f)
		{
			word flags = *(word *)&vehicle->flags348;
			real boost = 1.0f;

			if (TEST_FIELD_BIT((flags >> 4) & 1))
			{
				boost = function_d1210(vehicle_index) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
			}

			real angle = (real)atan2(input.y, input.x);
			real forward_fraction = vehicle_angle_fraction(angle);
			real forward_weight = (real)fabs(forward_fraction);
			real side_fraction = (1.0f - forward_weight) * (angle > 0.0f ? 1.0f : -1.0f);
			real side_weight = (real)fabs(side_fraction);
			bool forwards = forward_fraction > 0.0f;
			bool leftwards = side_fraction > 0.0f;
			real forward_speed_maximum = forwards ? *(real *)(definition + 0x1f4) : *(real *)(definition + 0x1f8);
			real side_speed_maximum = leftwards ? *(real *)(definition + 0x22c) : *(real *)(definition + 0x230);

			if (*(short *)(definition + 0x218) == 1)
			{
				real pitch = (real)atan2(state->matrix.forward.k, sqrt(state->matrix.forward.i * state->matrix.forward.i +
					state->matrix.forward.j * state->matrix.forward.j));
				real roll = (real)atan2(state->matrix.left.k, sqrt(state->matrix.left.i * state->matrix.left.i +
					state->matrix.left.j * state->matrix.left.j));
				real pitch_slope = pitch * 2.3149807f * (pitch * 2.3149807f);
				real roll_slope = roll * 2.3149807f * (roll * 2.3149807f);

				pitch_slope = pitch_slope > 1.0f ? 1.0f : pitch_slope;
				roll_slope = roll_slope > 1.0f ? 1.0f : roll_slope;
				forward_speed_maximum -= (forwards == (pitch > 0.0f) ? 1.0f : -1.0f) * vehicle_magnitude(pitch_slope) *
					forward_speed_maximum * 0.33f;
				side_speed_maximum -= (leftwards == (roll > 0.0f) ? 1.0f : -1.0f) * vehicle_magnitude(roll_slope) *
					side_speed_maximum * 0.33f;
			}

			real target_speed = side_speed_maximum * side_weight + forward_speed_maximum * forward_weight;

			if (TEST_FIELD_BIT((flags >> 3) & 1))
			{
				target_speed *= 0.8f;
			}
			target_speed *= boost;

			real target_forward = target_speed * input.x;
			real target_left = target_speed * input.y;
			real inverse_dt = 1.0f / dt;
			vector3f acceleration;

			acceleration.i = inverse_dt * (target_forward - forward_speed);
			acceleration.j = inverse_dt * (target_left - left_speed);
			acceleration.k = 0.0f;

			bool driven;
			real heading;

			if (vehicle->throttle == 0.0f && vehicle->steering == 0.0f)
			{
				driven = false;
				heading = (real)atan2(-acceleration.j, -acceleration.i);
			}
			else
			{
				driven = true;
				heading = (real)atan2(target_left, target_forward);
			}

			real heading_forward = (real)fabs(vehicle_angle_fraction(heading));
			real heading_side = (real)fabs((1.0f - heading_forward) * (heading > 0.0f ? 1.0f : -1.0f));
			real forward_acceleration;
			real side_acceleration;

			if (driven)
			{
				forward_acceleration = *(real *)(definition + 0x1fc);
				side_acceleration = *(real *)(definition + 0x234);
			}
			else
			{
				forward_acceleration = *(real *)(definition + 0x200);
				side_acceleration = *(real *)(definition + 0x238);
			}

			real maximum_acceleration = side_acceleration * heading_side + forward_acceleration * heading_forward;

			if (vehicle->unknown34f > 0 && (real)fabs(steering) > 0.78539819f)
			{
				real slowdown = vehicle->unknown34f * 0.05f;

				if (slowdown > 0.98f)
				{
					slowdown = 0.98f;
				}
				maximum_acceleration = (1.0f - slowdown) * maximum_acceleration;
			}
			function_a75d0(&acceleration, maximum_acceleration * boost);
			if (vehicle->unknown352 & 0x20)
			{
				vector3f_scale(&acceleration, 0.5f, &acceleration);
			}
			else if (vehicle->unknown352 & 0x10)
			{
				vector3f_scale(&acceleration, 0.9f, &acceleration);
			}
			function_142640(&state->matrix, &acceleration, &acceleration);

			real scale = state->unknown6c * vehicle->unknown370;

			vector3f_add_scaled(&force, &acceleration, scale, &force);
		}
		if (vehicle->unknown370 > 0.0f)
		{
			short kind = *(short *)(definition + 0x218);
			real arg_3097c5;
			real maximum_yaw_acceleration;

			if (kind != 1 && kind != 2 && kind != 3)
			{
				arg_3097c5 = vehicle->angular_velocity.k * vehicle->up.k + vehicle->angular_velocity.j * vehicle->up.j +
					vehicle->angular_velocity.i * vehicle->up.i;
				maximum_yaw_acceleration = 3.1415927f;
			}
			else
			{
				arg_3097c5 = (vehicle->up.k * vehicle->angular_velocity.k + vehicle->up.j * vehicle->angular_velocity.j +
					vehicle->angular_velocity.i * vehicle->up.i + vehicle->angular_velocity.k) * 0.5f;
				maximum_yaw_acceleration = 4.712389f;
				if ((steering > 0.0f) != (arg_3097c5 > 0.0f))
				{
					real magnitude = PIN(vehicle_magnitude(arg_3097c5), 0.78539819f, 3.1415927f);

					maximum_yaw_acceleration = (magnitude - 0.78539819f) * 14.0f + maximum_yaw_acceleration;
				}
			}

			real target = (real)sqrt((real)fabs(steering) * maximum_yaw_acceleration * 2.0f) * vehicle_sign(steering);

			if (vehicle_sign(steering) == vehicle_sign(target) &&
				vehicle_magnitude(steering) < vehicle_magnitude(target) * dt * 2.0f)
			{
				target = steering / dt * 0.5f;
			}

			real yaw_acceleration = PIN((target - arg_3097c5) / dt, 0.0f - maximum_yaw_acceleration,
				maximum_yaw_acceleration);
			real yaw_torque = state->unknown5c * yaw_acceleration * vehicle->unknown370;

			torque.i += vehicle->up.i * yaw_torque;
			torque.j += vehicle->up.j * yaw_torque;
			torque.k += vehicle->up.k * yaw_torque;
		}
		if (1.0f > vehicle->unknown370)
		{
			vector3f left;
			point2f target = *g_468774;
			point2f forward2d;
			point2f left2d;
			real pitch_target;
			real roll_target;

			left.i = vehicle->forward.k * vehicle->up.j - vehicle->forward.j * vehicle->up.k;
			left.j = vehicle->forward.i * vehicle->up.k - vehicle->up.i * vehicle->forward.k;
			left.k = vehicle->forward.j * vehicle->up.i - vehicle->forward.i * vehicle->up.j;
			forward2d.x = vehicle->forward.i;
			forward2d.y = vehicle->forward.j;
			left2d.x = left.i;
			left2d.y = left.j;
			normalize2d(&forward2d);
			normalize2d(&left2d);
			if (vehicle->up.k > 0.0f)
			{
				point2f error;

				error.x = target.x - (vehicle->up.j * forward2d.y + vehicle->up.i * forward2d.x) -
					(vehicle->angular_velocity.j * left2d.y + left2d.x * vehicle->angular_velocity.i) * 0.5f;
				error.y = target.y - (vehicle->up.j * left2d.y + vehicle->up.i * left2d.x) -
					(0.0f - (vehicle->angular_velocity.j * forward2d.y + forward2d.x * vehicle->angular_velocity.i)) *
					0.5f;

				real pitch_error = (real)fabs(error.x) * vehicle_sign(error.x * input.x);
				real roll_error = (real)fabs(error.y) * vehicle_sign(error.y * input.y);
				real damping = (1.0f - vehicle->up.k) * 3.4906585f;

				pitch_target = PIN(pitch_error + 1.0f, 0.3f, 2.5f) * input.x * 1.3962634f + target.x;
				roll_target = PIN(roll_error + 1.0f, 0.3f, 2.5f) * input.y * 1.3962634f + target.y;
				pitch_target = error.x * damping + pitch_target;
				roll_target = error.y * damping + roll_target;
			}
			else
			{
				pitch_target = input.x * 1.3962634f + target.x;
				roll_target = input.y * 1.3962634f + target.y;
			}

			vector3f zero = *g_4687a4;
			real pitch_torque = state->unknown4c * pitch_target;
			real roll_torque = 0.0f - state->unknown3c * roll_target;
			real airborne = 1.0f - vehicle->unknown370;

			torque.i += (left.i * pitch_torque + zero.i + vehicle->forward.i * roll_torque) * airborne;
			torque.j += (left.j * pitch_torque + zero.j + vehicle->forward.j * roll_torque) * airborne;
			torque.k += (left.k * pitch_torque + zero.k + vehicle->forward.k * roll_torque) * airborne;
		}
		if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit3))
		{
			real speed = (vehicle->forward.j * linear_velocity.j + vehicle->forward.k * linear_velocity.k +
				linear_velocity.i * vehicle->forward.i) / *(real *)(definition + 0x1f4);
			vector3f left;
			vector3f const *up = g_4687b0;

			speed = PIN(speed, 0.0f, 1.0f);
			left.i = vehicle->forward.k * vehicle->up.j - vehicle->forward.j * vehicle->up.k;
			left.j = vehicle->forward.i * vehicle->up.k - vehicle->forward.k * vehicle->up.i;
			left.k = vehicle->up.i * vehicle->forward.j - vehicle->forward.i * vehicle->up.j;
			if (speed > 0.0f)
			{
				real pitch = state->unknown4c * speed * vehicle->unknown370 * -5.2359877f;
				real lift = state->unknown6c * speed * vehicle->unknown370 * 3.6000001f;

				torque.i += pitch * left.i;
				torque.j += left.j * pitch;
				torque.k += left.k * pitch;
				force.i += lift * up->i;
				force.j += up->j * lift;
				force.k += up->k * lift;
			}
			if (vehicle->unknown34c > 0)
			{
				vector3f direction;

				direction.i = left.j * up->k - left.k * up->j;
				direction.j = left.k * up->i - left.i * up->k;
				direction.k = up->j * left.i - left.j * up->i;
				if (function_30bf0(&direction) > 0.0f)
				{
					real fade = PIN(1.0f - vehicle->unknown34c * dt, 0.0f, 1.0f);
					real push = (1.0f - vehicle->unknown370) * state->unknown6c * fade;
					real forward_push = push * 1.8000001f;
					real up_push = push * 0.90000004f;

					force.i = up_push * up->i + (direction.i * forward_push + force.i);
					force.j = up->j * up_push + (direction.j * forward_push + force.j);
					force.k = up_push * up->k + (direction.k * forward_push + force.k);
				}
			}
		}
		force.i *= mass;
		force.j *= mass;
		force.k *= mass;
		torque.i *= mass;
		torque.j *= mass;
		torque.k *= mass;
	}

	long permutation_count;
	long permutations;

	function_ba690(vehicle_index, &permutation_count, NULL, NULL, &permutations);

	for (i = 0; i < *(long *)(definition + 0x2e8); i++)
	{
		byte *point_definition = *(byte **)(definition + 0x2ec) + i * 0x4c;
		short permutation;

		state->points[i].unknown9c = mass;
		if ((point_definition[4] & 1) && (permutation = *(short *)(point_definition + 0x32)) >= 0 &&
			permutation < permutation_count)
		{
			state->points[i].unknowna0 =
				*(real *)(point_definition + 0x38 + (char)((byte const *)permutations)[permutation * 8 + 1] * 4);
		}
		else
		{
			state->points[i].unknowna0 = 0.0f;
		}
	}
	state->unknown8c = steering;
	function_2056e0(vehicle_index, state, 0.0f, &force, &torque);

	real upright = 0.4f > vehicle->up.k ? 0.4f : vehicle->up.k;
	long count = *(long *)(definition + 0x2e8);
	long touching = 0;
	real grounded = 0.0f;

	for (i = 0; i < count; i++)
	{
		if (state->points[i].unknown80)
		{
			touching++;
		}
	}
	if (count > 0)
	{
		grounded = (real)touching / (real)count;
	}
	grounded = PIN(grounded * upright, 0.0f, 1.0f);
	if (grounded - vehicle->unknown370 > 0.1f)
	{
		vehicle->unknown370 += 0.1f;
	}
	else if (-0.1f > grounded - vehicle->unknown370)
	{
		vehicle->unknown370 -= 0.1f;
	}
	else
	{
		vehicle->unknown370 = grounded;
	}
}

/* the duration of the vehicle's current flip (+0x350: 1 and 2 to either
   side, 3 end over end) */
PRIVATE inline real vehicle_flip_duration(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);

	if (vehicle->unknown350 > 0)
	{
		if (vehicle->unknown350 > 2)
		{
			if (vehicle->unknown350 == 3)
			{
				return g_4678bc;
			}
		}
		else
		{
			return g_4678c0;
		}
	}
	return 0.0f;
}

/* how far through its flip the vehicle is, eased in for a side flip and out
   for one end over end */
// @retail 0xf4250
real function_f4250(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	real elapsed = vehicle->unknown351 * g_510c54->rate;
	real duration = vehicle_flip_duration(vehicle_index);
	real result = 0.0f;

	if (duration > elapsed)
	{
		if (vehicle->unknown350 == 1 || vehicle->unknown350 == 2)
		{
			real remaining = 1.0f - elapsed / duration;

			result = 1.0f - remaining * remaining;
		}
		else
		{
			result = elapsed / duration;
			result *= result;
		}
	}
	return result;
}

/* whether the vehicle is flipping */
// @retail 0xf42f0
bool function_f42f0(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	bool result = false;

	if (vehicle->unknown350)
	{
		real elapsed = vehicle->unknown351 * g_510c54->rate;

		result = vehicle_flip_duration(vehicle_index) > elapsed;
	}
	return result;
}

/* the flip a boosting vehicle's controls ask for: none (0), to either side
   (1 or 2) or end over end (3), by the direction they push */
// @retail 0xf4360
long function_f4360(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	long result = 0;

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit3) && !(vehicle->control_flags & 0x800))
	{
		real throttle = vehicle->throttle >= 0.0f ? vehicle->throttle : 0.0f - vehicle->throttle;
		real steering = vehicle->steering >= 0.0f ? vehicle->steering : 0.0f - vehicle->steering;

		if (throttle > 0.75f || steering > 0.7f)
		{
			real angle = (real)atan2(vehicle->steering, vehicle->throttle);
			real direction = fmod(angle + 12.566371f, 6.2831855f);

			if (PIN(direction, 2.5132742f, 3.7699113f) == direction)
			{
				result = 3;
			}
			else if (!(0.08726646f > direction) && !(direction > 6.195919f))
			{
				if (PIN(direction, 0.0f, 2.5132742f) == direction)
				{
					result = 1;
				}
				else
				{
					result = 2;
				}
			}
		}
	}
	return result;
}

/* the cross product of two vectors */
PRIVATE inline void vehicle_cross(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

/* the orientation the vehicle's physics aims for: its facing held level,
   turned through its flip while it flips (returns whether it flips), else
   the state's own */

// MiniMax B (11-13): no new form, no lower score (118): the dot product's first term is still commuted (ours multiplies
// forward.i by up.i where retail multiplies up.i by forward.i) and the first cross's operand order still differs; both follow
// from the helper's argument order, which cannot be swapped for score.
// MiniMax G (11-12): no new form, no lower score (118): nothing new this round; the projection/cross input ranks and the
// angle/trig homes remain.
// MiniMax G (11-12): no new form, no lower score (118): the projection/cross input ranks and the angle/trig homes
// remain; retail keeps the angle on the x87 stack in all three arms, which only the listed `double angle` spelling
// gives.
// MiniMax D (11-11): 117 (was 136): the flip truth is a byte-wide flag (retail keeps it in a byte at
// [esp+0x1c] and hands it back in AL), the fallback up copy is one aggregate move, the angle stays on the
// x87 stack as a double and the default case's axis is computed before the trigonometry; what still differs
// is the dot's i-term operand rank, the two cross products' operand ranks, the trigonometry's spill slots
// (the sine at [esp+0x18] and the cosine at [esp+0x1c], retail the other way) and the shared up rotation's
// register order. The permuter's variant on the older body reaches 110 differing instructions, but its whole
// gain is one call that computes the up from the left and the forward where retail computes the left from the
// up and the forward, so it changes the result and is dropped (without that call the variant is 117); a fresh
// run on this body found nothing lower in 46 tries. Retail returns the flag byte as it stands (`mov al, dl`)
// while the bool return normalises it (`test`/`setne` on both exits); both spellings of retail's form (a bool
// local, an unsigned char return) score 131 and change the declaration, so the byte-wide flag stays.
// score 117 (was 136): projection/cross input ranks and angle/trig homes remain;
// MiniMax C (11-10): the score is 136 (was 136); the dot product multiplies the forward by the up where retail multiplies the up by the forward, the fallback copy moves one word and two floats where retail moves three words, and the trig block spills the sine and cosine where retail keeps the angle on the x87 stack.
// MiniMax E (11-08): no new form, no lower score (136); the dot's i term still multiplies the forward by the up where retail multiplies the up by the forward, the fallback copy still moves one word and two floats where retail moves three words, and the trig block still spills the sine and cosine through the stack where retail keeps the angle on the x87 stack.
// MiniMax E (11-07): no new form, no lower score (136); the projection and cross operand ranks and the
// x87 angle and trig homes are unchanged from the earlier rounds' forms.
// MiniMax R (11-06): no new form, no lower score (136); the entry and the frame agree, and the body differences of the note below are unchanged.
// Earlier (136, was 136): Fallback up setter with staged cross, native up receiver and rotation setters kept;
// projection/cross operand ranks and trig homes remain.
// Shared cross 204; external cross 326; native forward/up/both 263/269/298;
// shared cross with forward/both 263/298. Wide angle 177 rejected without exact equality;
// coordinated external force-inline 326, with wide angle 359; all original contracts restored.
// J198: staged cross/projection/both/dot/products/combinations 188/205/190/211/206/207/207/207.
// Up/forward/both receivers and case/trig combinations 177/204/178/188/188/178/178/178.
// On native receiver, complete projection/dot/both/sine-first 171/171/169/169; staged cross, native up receiver and all symmetric products kept plain.
// Double angle/cases/trig/both 147/147/147/147; shared dot/scale/both/trig 154/148/149/149.
// All precision/grouping forms exact-only and restored. Projection-load order, cross operands and trig homes remain.
// Round 431: full forward value 264; staged rotated forward/up/both rows 169/169/169;
// forward value with rotated forward/up/both 264/264/262; complete staging with tested flipping result 262;
// native CRT forward/up/matrix copy subsets 169 each (seven). Original graph restored; projection preloads and trig residence remain.
// Round 43 (lane 16): retail keeps the angle on the x87 stack (one fld of the fraction above the switch,
// fmul/fsub per case) and has `cosine` in XMM4 and cos*up.i in XMM5 in both predecessors of the shared up
// rotation. `double angle` 147 at 787 bytes (x87 flow then retail's; exact-only, restored); the up rotation
// and left cross written in both paths (default returning) 238; the projection as one reversed sum and
// three `-=` statements 177; the same through facing->up with no pointer 188; dot3f(up, forward) /
// dot3f(forward, up) 171/171; dot grouped (j + k) + i / (k + j) + i 170/170 (j term retail's); grouped with
// the k and i operands swapped 170 (identical code: product operands are canonicalised).
// Lane 13 round 25: the forward pointer declared after up (forward through it everywhere) 165; forward copied
// before the up pointer 169; the forward pointer declared first 169; that with dot3f(up, forward) 178; the
// projections subtracted in place without the named products 169. Retail's 30 shorter bytes are the angle kept
// on x87 (fraction loaded above the switch); only `double angle` gives that (exact-only, not repeated).
// Lane 13 round 28: the up copy written to facing->up 168 (forward loads then hoist above it, as retail's); every
// use through facing->up 207; with dot3f and the projection through vector3f_scale 164; with the dot read through
// facing->up 154; the up pointer declared after the dot (copy and dot through facing->up, everything after
// through the pointer) 154; all uses through facing->up 171, all through the pointer 169; the dot declared at its
// assignment 154; in the cases 1/2 path cos, sin then the cross 154, the cross first 154; dot3f passed straight to
// vector3f_scale (no dot local) with the pointer declared after it 154 (kept). Left: the dot's i term
// and two left-cross products load forward where retail loads up.
// Round 182: dot3f(&facing->forward, &facing->up) 154 (identical); the left cross through facing->up 151 (833
// bytes; its first product then retail's, three others flip); that with the dot's arguments swapped 151; the
// projection removed in place through a `dot` local 174 (compile-only it gives the dot's i term); the up pointer
// declared first with the dot through it 164. A compile-only harness (reproduces our products exactly) screened 144
// dot/projection/cross/normalize/fallback spellings and 30 by-name copy orders: none gives retail's cross operand
// orders (it needs the j components ranked before i and i before k for both vectors); best 3 of 9 products off.
// Round 45 (lane 1): `double angle` 130 at 786 bytes (exact-only, not kept); with the up rotation and left cross
// written in both paths (default returning, no goto) 121 at 797 bytes (retail's default path also preloads cosine
// and cos * up.i, as a tail merge of two written-out rotations would); the same with `real angle` 339 at 983
// bytes; on the 121 form the x87 forward rows' products swapped 121 and sine declared first 121 (identical code:
// sine keeps the state slot where retail's takes the result slot).
// Round 44 (lane 11): sine declared before cosine 154; the axis declared first 154; plain cross3f for the flip axis
// 154 (all identical code); `double angle` 130 at 786 bytes with either sine/cosine order (exact-only, not kept).
// Round 226: complete rotated forward/up through vector3f_assign 145 (827 bytes), plain confirmed;
// input setters 230, both setter groups 215, enclosing axis 154, axis with inputs/rotations/both
// 230/145/215, all vector setters 220. Original grouping, precision and all row expressions retained.
// Round 250: existing setters independently at initial up/fallback up/negative axis and combinations
// 186/136/207/144/186/138/204; all with forward setter 220. Fallback up setter alone kept plain,
// 136 at 828/806; original values/precision retained. Projection/cross ranks and trig homes remain.
// Projection-only workspace scope on the fallback-setter graph 136 (tie, restored).
// Round 253: Vector-difference/cosine-up/sine-axis parts subsets 1/2/4/3/5/6/7:
// 134/144/157/144/155/134/168; all with byte flipping truth 176. The two-point forms
// rejected; original complete body restored. Projection input ranks, trig homes and angle
// residence remain.
// P255: Common matrix-source copy/late pure flip test/complete scale-add up rotation
// subsets 1-7 224/220/222/167/228/211/219; all with separate scaled-axis/add 219.
// Original real-angle and complete body restored; projection/cross ranks and trig homes remain.
// Round 361: Complete CRT input/fallback/result copy boundaries, subsets 1-7:
// 193/145/158/136/193/145/158; all with enclosing axis workspace 158. Original
// native copies, fallback setter and complete body restored; projection/cross
// input ranks and angle/trigonometric homes remain.
// Round 470: Plain real-array component loops for projection removal/negative axis/
// up rotation, subsets 1/2/4/3/5/6/7, then all with forward rotation: 168/170/174/145/177/180/176/176.
// Original body restored; projection i-product, cross ranks and angle/trig homes remain.
// Round 236: Full projection-only/both/implicit producers 223/262/262; forward scale/add 235;
// exact-valued promoted angular coefficient case/default/both 165/160/166, exact-only;
// complete forward product parts 131; with shared subtraction/native fallback copy/both
// 133/149/152. Five-point producer form rejected. Original complete body restored;
// projection i-product, cross ranks and angle/trig homes remain.
// Round 325: Common real-angle/trig phase, switch/conditional dispatch, cosine/sine first
// and case/default axis branch order: 184/160/184/159/184/160/184/159. Original complete body restored;
// projection/cross ranks and x87 angle/trig homes remain.
// P268: Rotated output forward/up/both field receivers after/before the matrix copy:
// 141/193/184/141/193/184; both with complete forward/forward-and-up product parts
// 194/195. Original body restored; projection/cross ranks and angle/trig homes remain.
// Round 370: Complete forward parts with cosine-up/sine-axis/outer-trig declaration
// subsets 1/2/4/3/5/6/7, then all with native fallback copy: 145/156/131/155/145/156/155/159.
// Five-point best rejected; original body restored. Projection/cross ranks and angle/trig homes remain.
// F344: Native final-left fields as projection/flip-axis workspaces, direct
// projection/axis/both 220/207/288; field receivers 215/219/291; mixed receivers
// 288/287. NOT kept: output-storage graph adds nonretail stores and length.
// Original complete body, actual helper contracts and real precision restored.
// DH308: X87 angle with complete forward/cosine-up/sine-axis part subsets 1/2/4/3/5/6/7,
// then all with native fallback copy: 172/176/175/178/176/176/171/140. All regress at 776..786 bytes versus 806.
// Exact-only precision forms restored; original real angle, setters, helpers and body retained.
// P287: Final branch-owned return truth, five producer graphs 222/222/222/214/214;
// first three with a native normalization-result receiver 231/231/231. All restored;
// projection input ranks and angle/trig homes remain.
// Round 546: Existing sequential cross producers at facing/axis/final-left sites,
// subsets 1/2/4/3/5/6/7: 138/136/165/166/167/165/167; all with X87 angle 171.
// NOT kept: no gain; precision form requires a match. Original complete body restored.
// M336: Staged-cross first/second scalar input captures and complete product parts,
// seven subsets then immutable complete inputs: 178/177/165/225/200/193/234/234.
// All regress and grow; shared helper restored. Projection ranks and trig homes remain.
// M338: Entry flip truth in bool/byte/short/long, positive/complementary channels:
// 136/159/240/237/169/169/244/241. Original body restored; projection/cross ranks and angle/trig homes
// remain. NOT kept: no gain; unsupported truth widths/polarities restored.
// F353: Consistent float CRT trig APIs and actual trig-result name/declaration homes
// 173/173/159/159/173/173/159/159.
// NOT kept: no gain; original full body/helpers restored.
// AC507: X87 fraction carrier with real/double angle/cosine/sine domains, masks
// 0..7, 198/203/237/237/230/230/237/222. NOT kept: all regress, change the
// frame and lack a precision match; original real fraction/trig graph restored.
// F355: Wide fraction with real/wide angle, original/entry trig declarations and original/late flip predicate:
// 199/202/199/202/227/235/227/235. NOT kept: no gain; original body restored.
// F358: Existing dot/scale/staged-cross forced expansion declarations, seven subsets, then existing setter expansion:
// 136/136/136/136/136/136/136/136. NOT kept: no exact projection/cross/trig repair; all original declarations and helper contracts restored.
// P301: Projection workspace consumes complete up/forward input, aggregate/library
// transfers and original/shared subtraction: 271/271/271/271/257/252/257/252.
// NOT kept: all regress, grow the native one-word frame to 0x10/0x14 and lengthen
// code. Original workspace, shared helpers, precision and complete body restored.
// P329: Projection/axis/up receiver identities, seven subsets then both vector declarations:
// 136/136/136/136/136/136/136/136. NOT kept: no byte match; original complete body restored.
// Projection/cross operand ranks and angle/trig homes remain.
// M374: Actual physics/orientation/index parameter-home identities, seven role graphs,
// then all with input basis identity: 136/136/136/136/136/136/136/136. NOT kept: no native
// projection/trig home repair; original complete body and all contracts restored.
// J257: Existing double-factor scale at projection/up/axis and native default-forward product boundaries, with x87 angle/fraction diagnostics:
// 205/218/229/232/251/241/255/208/200/204/208/159/228. NOT kept: all thirteen regress; prologue/storage gaps remain; existing helper already inlines and precision forms lack equality.
// Original executable source restored; allocation gaps remain.
// P344: Isolated normalizer return with original entry flip truth, mutable/immutable real/double at
// producer, then real/double declarations before fraction/basis: 159/159/217/217/159/217/159/217. NOT
// kept: no byte match; original whole body restored. Projection/cross ranks and angle/trig homes
// remain.
// M378: Complete default placements 1/D/2, 2/D/1, D/1/2, D/2/1; paired real cases
// in both label/default orders: 136/136/136/136/165/165/165/165. NOT kept: no native projection/
// trig repair or byte equality; original cases, precision and complete body restored.
// M382: Every angular sweep product named at its producer, case/default real-real/double-real/real-double/double-double,
// arm-local 133/172/169/162; enclosing 161/169/167/162. Real arm-local fixes the projection prefix but gains only three.
// NOT kept: below keep gate, no byte equality for materialization/precision; original complete body restored.
// M384: Initialized affine sweep, real/double before/after basis copy, switch/conditional
// dispatch: 136/136/174/174/138/138/171/171. NOT kept: no byte equality for
// precision/lifetime forms; original complete body restored; fallback/cross and x87 trig homes remain.
// M386: One sequential projection/flip-axis vector, native then entry angle/cosine/sine
// declaration subsets 0..7: 179/179/179/179/179/179/179/179. NOT kept: no byte equality for
// reused-workspace/scalar lifetimes; original separate vectors and complete body restored.
// M388: Existing return-valued coordinate setters at forward/up/fallback, masks
// 1/2/4/3/5/6/7, then initial up: 136/136/136/136/136/136/136/207. NOT kept:
// no projection/cross/trig equality; original complete body and all helpers restored.
// P344: Isolated normalizer return with original entry flip truth, mutable/immutable real/double at
// producer, then real/double declarations before fraction/basis: 159/159/217/217/159/217/159/217. NOT
// kept: no byte match; original whole body restored. Projection/cross ranks and angle/trig homes
// remain.
// Round 68 (lane 13), re-measured at 136: `double angle` 174 at 778 bytes (it also moves caller 0xf4800's frame
// to 0xfc: an 8-byte alignment need); double angle with the up rotation and left cross written in both paths
// (default returns) 232 at 881 bytes; the same with real angle 290 at 934 bytes. New: the fallback as
// `*up = *g_4687a8` (retail's integer aggregate copy) 145; the up pointer declared before the projection with
// dot3f(up, forward) and that aggregate fallback 155.
// Round 95 (lane 9), at 136: `*up = *g_4687a8;` 145 (aligned 62 against 59; that block then retail's); with
// `double angle` 134 at 784 bytes (aligned 58; `double angle` alone is aligned 49 at 778, the x87 angle and the
// default case's x87 forward rotation then retail's; match-only); that with the default case's own up rotation and
// left cross 253; the aggregate copy with dot3f(&facing->forward, &facing->up) 145; the aggregate copy with the
// default case's own rotation (real angle) 320.
// P388: Actual forward/up output accumulation through existing scale/add producers,
// cosine-first/sine-first and consumed-axis graphs: 235/181/204/249/273/246/261/284.
// NOT kept: all regress and add nonnative materialization; original whole body,
// real precision and shared helpers restored. Projection/cross/trig homes remain.
// P398: Complete dot first/second and scale scalar-input ownership, seven subsets,
// then immutable complete inputs: 179/179/175/169/169/179/165/165.
// NOT kept: all regress and lengthen; unmatched input copies require equality.
// Original shared projection providers and entire body restored; projection/cross/trig homes remain.
// T80: Canonical fallback comparison, degenerate/normalized bool/byte/short/long:
// 159/159/159/159/159/159/159/159, all 831/806 bytes. Projection/cross input ranks and angle/trig
// homes remain.
// NOT kept: no byte equality or qualifying gain; original complete body restored.
// Round 101 (m3 lane 9, fast), at 136 (828 bytes, aligned 81, masked 54). Retail's flip block: the fraction is
// loaded on the x87 stack once ahead of the switch, each case scales it there (`fmul; fsub` / `fmul; fsubr` /
// `fmul`) and the angle never goes to memory (`fld st(0); fcos; fstp; ...; fsin; fstp`), so the angle is a double.
// Retail keeps cosine at +0x18 and sine at +0x1c (the two dead stack-parameter homes; ours the other way round),
// loads cosine into XMM4 in each arm, forms `cosine * up.i` in each arm with its own operand form (a copy of the
// cosine times memory after the default arm's forward rows; the loaded up.i times XMM4 in the other arm), and joins
// at the sine load; it stores each rotated up component before loading the next. Tried: `double angle`, re-measured,
// 174 at 778 (aligned 68, masked 41; caller 0xf4800 unchanged in fast, 1087); that with `*up = *g_4687a8` 134 at
// 784; with the rotated up as three member assignments 132 at 784; with the default arm's forward rows as member
// assignments too 130 at 786 (aligned 68, masked 42; six points, under the bar, not kept). On that 130 form: sine
// declared before cosine 130 (same code); either one renamed (diagnostic) 130 (same code: names move no slot); the
// default arm's cosine computed before its sine 136; the up rows and the left cross written in both paths with the
// default arm returning 121 at 797 (aligned 106: VC merges the whole second copy back into the first; plain 121 with
// caller 0xf4800 unchanged at 1087; not kept: it needs vector3f_assign written out as member assignments, which is
// 232 at 881 with the helper, round 68), sine first 121; both rotations inside the switch (cases 1 and 2 sharing a ternary angle, default first/last) 230/230 at 893,
// with an if/else angle 146/146 at 788; the up rows as an inline helper called in both arms, (axis, sine, cosine)
// 276 at 939, (axis, cosine, sine) 276, with the roll arm's sine and cosine evaluated in the call 272 at 935.
// Left on the 130 form: the two slots, the default arm's first forward row (ours `sine * up.i` first), the
// per-arm cosine product, and the first cross's and the dot's operand forms.
// Round 165 (m3 lane 6, fast with 0xf4250/0xf4800): the up rows as three in-place member statements i, j, k (each stored
// before the next is loaded, as retail) 172 at 841 bytes (aligned 81 -> 45: retail's sine and cosine homes, the final cross
// reloading the rotated up with retail's operand order, retail's epilogue); that with `double angle` 163 at 792 (aligned 33,
// masked 29; left: the dot's i term, the fallback setter's float copy, the first cross's operand order, both predecessors'
// early cosine load and `cosine * up.i` before the join, and the flip path's pops); row i written like rows j and k 163
// (same code). On the 163 form: the fallback as `*up = *g_4687a8` or `facing->up = *g_4687a8` 132 at 784 (the listed 132
// form; aligned 74: the aggregate copy flips the flip block's operand order and the two homes back); that with real angle
// 151 at 833; that with the up pointer declared first and dot3f(up, &facing->forward) 142 at 784; the up rows and left cross
// written in both paths with the default arm returning 148 at 806 (aligned 90: VC keeps the default arm's copy and the
// case path jumps back into it, where retail keeps the case path's tail and the default arm jumps forward to it after its
// own `cosine * up.i`). Lead: the 163 form; retail's early `cosine * up.i` in both arms looks like two cross-jumped copies
// scheduled before the merge.
// Round 105 (m3 lane 16, fast), on the listed 163 form (in-place up rows + `double angle`, re-measured 163 at 792,
// aligned 23): the first cross through `&facing->up` 165 at 792 (aligned 19, masked 14: then only the dot's i term,
// the fallback setter's float copy, one product of the first cross and both arms' early `cosine * up.i` differ); that
// with `*up = *g_4687a8` or `facing->up = *g_4687a8` 130 at 786 (aligned 49); vehicle_cross at the roll axis and the
// final cross 162, at the first cross 165; `cosine * up.i` named in each arm before the join 122 at 808 (aligned 50:
// the default arm then computes it on x87). On the 148 form (both paths, re-measured 148 at 806): the `default:` label
// first 148 (same code); the roll through bipeds.cpp's rotate body (v, axis, sine, cosine) 274 at 913 and the default
// arm in 0xf4800's pitch spelling (member forward rows, vector3f_scale + vector3f_add_scaled) 246 at 891 (neither
// tail-merges); per-arm sine/cosine/axis locals 286 at 924. Real angle: three complete switch arms, each with its own
// angle, trig, cross and up rows, 185 at 843 (the fraction load and the x87 angle are then retail's, but the cases
// interleave the cross with the angle, so retail's case angle does cross a join). Lead: the facing->up cross on the
// 163 form; retail's early `cosine * up.i` needs the up rows scheduled inside both arms.
// Round 112 (m3 lane 9, fast, base 136): the default arm's sin/cos on the unnamed product `(real)sin(fraction *
// 6.2831855f)` / cos (no angle store there) 133 at 827 bytes (one product order changes in the final cross; three
// points, under the bar); that with `*up = *g_4687a8` 152 at 833; the default arm's axis negation before its sin/cos
// (retail interleaves them) 174 at 836 (the zero register moves to xmm3). No further untried natural form: retail
// keeps the angle on the x87 stack in all three arms (one fld of the fraction above the switch), which only the listed
// `double angle` gives; the rest is product ranks and the sine/cosine homes. Also: the default angle as
// `6.2831855f * fraction` 136 (same code); the not-flipping path as an early `return false` 170 at 827;
// `!function_30bf0(up)` 136 (same code).
// MiniMax I (11-14): no new form, no lower score (118): retail's default arm keeps the angle on the
// x87 stack, takes its cosine from [esp+0x18] and its sine from [esp+0x1c] and rotates the forward
// with SSE, while ours takes the sine from [esp+0x18], the cosine from [esp+0x1c] and rotates it on
// the x87 stack; the dot's first term and the first cross's operand order also stay as the shared
// helpers order them.
// MiniMax A (11-13): no new form tried this round, no lower score (118): the dot product's first term is still
// commuted (this build multiplies forward.i by up.i where retail multiplies up.i by forward.i) and the first
// cross product's operand order still differs; both follow from the shared helper's argument order, which
// cannot be swapped for the score.
// @retail 0xf44a0
bool __stdcall function_f44a0(long vehicle_index, matrix3x3 *facing, s_vehicle_physics_state *state,
	matrix3x3 *result)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	real fraction = function_f4250(vehicle_index);
	unsigned char flipping = fraction > 0.0f;

	facing->forward = vehicle->local_velocity;
	facing->up = *g_4687b0;

	vector3f projection;

	vector3f_scale(&facing->forward, dot3f(&facing->up, &facing->forward), &projection);

	vector3f *up = &facing->up;

	up->i -= projection.i;
	up->j -= projection.j;
	up->k -= projection.k;
	if (function_30bf0(up) == 0.0f)
	{
		*up = *g_4687a8;
	}
	cross3f_staged(up, &facing->forward, &facing->left);
	if (flipping)
	{
		double angle;
		real cosine;
		real sine;
		vector3f axis;

		*result = *facing;
		switch (vehicle->unknown350)
		{
		case 1:
			angle = 0.19634955f - fraction * 6.0868359f;
			break;
		case 2:
			angle = fraction * 6.0868359f - 0.19634955f;
			break;
		default:
			angle = fraction * 6.2831855f;
			axis.i = 0.0f - result->forward.i;
			axis.j = 0.0f - result->forward.j;
			axis.k = 0.0f - result->forward.k;
			sine = (real)sin(angle);
			cosine = (real)cos(angle);
			vector3f_assign(&result->forward,
				cosine * result->forward.i + sine * result->up.i,
				sine * result->up.j + cosine * result->forward.j,
				sine * result->up.k + cosine * result->forward.k);
			goto rotate_up;
		}
		cosine = (real)cos(angle);
		cross3f_staged(&result->forward, &result->up, &axis);
		sine = (real)sin(angle);
rotate_up:
		vector3f_assign(&result->up,
			axis.i * sine + result->up.i * cosine,
			result->up.j * cosine + axis.j * sine,
			result->up.k * cosine + axis.k * sine);
		cross3f_staged(&result->up, &result->forward, &result->left);
	}
	else
	{
		*result = state->matrix.rotation;
	}
	return flipping;
}

void __stdcall function_f6060(long vehicle_index);
void __stdcall function_f62f0(long vehicle_index);

/* counts the ticks the brake is held: a wheel-steered vehicle brakes with
   the brake control, or with the throttle against its motion */
// @retail 0xf5bf0
void function_f5bf0(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	long count;

	if (*(short *)(definition + 0x1f0) == 1)
	{
		if (vehicle->control_flags & 2)
		{
			goto braking;
		}
		if (vehicle->throttle != 0.0f)
		{
			vector3f velocity;

			function_ba1d0(vehicle_index, &velocity, NULL);
			if (function_30bf0(&velocity) > 1.0f)
			{
				if (0.0f > vehicle->throttle)
				{
					if (dot3f(&velocity, &vehicle->forward) > 0.0f)
					{
						goto braking;
					}
				}
				else if (vehicle->throttle > 0.0f && 0.0f > dot3f(&velocity, &vehicle->forward))
				{
					goto braking;
				}
			}
		}
	}
	count = 0;
	goto done;

braking:
	count = MIN(vehicle->unknown384 + 1, 0xfe);
done:
	vehicle->unknown384 = (byte)count;
}

/* sets or clears one of the vehicle's 32 flags at +0x3b4 */
// @retail 0xf5d10
bool __stdcall function_f5d10(long vehicle_index, long bit, bool set)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	bool result = false;

	if (bit >= 0 && bit < 32)
	{
		if (set)
		{
			vehicle->unknown3b4 |= 1 << bit;
		}
		else
		{
			vehicle->unknown3b4 &= ~(1 << bit);
		}
		result = true;
	}
	return result;
}

/* whether the vehicle is braking: briefly, or while the brake is held */
// @retail 0xf5d70
bool function_f5d70(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);

	return vehicle->unknown384 && (0.5f > vehicle->unknown384 * g_510c54->rate || (vehicle->control_flags & 2));
}

/* how firmly the vehicle's parking brake holds, from the ticks it has sat
   idle */
// @retail 0xf5e00
real function_f5e00(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	long delay = vehicle_round(g_510c54->field_2_3 * 0.45f);
	real result = 0.0f;

	if ((char)vehicle->unknown34a > delay &&
		(vehicle->unknown248 == NONE || 3.0625f > vehicle->linear_velocity.i * vehicle->linear_velocity.i +
			vehicle->linear_velocity.j * vehicle->linear_velocity.j +
			vehicle->linear_velocity.k * vehicle->linear_velocity.k))
	{
		real brake = ((char)vehicle->unknown34a * g_510c54->rate - 0.45f) * 1.25f;

		result = PIN(brake, 0.0f, 1.0f);
	}
	return result;
}

/* counts the ticks the vehicle sits idle: no control pushed (or braking),
   not every seat taken, and slow or undriven */
// @retail 0xf5ee0
void function_f5ee0(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool pushed;

	if (*(short *)(definition + 0x1f0) == 1)
	{
		pushed = (vehicle->throttle >= 0.0f ? vehicle->throttle : 0.0f - vehicle->throttle) > 0.001f;
	}
	else
	{
		vector3f const *input = (vector3f const *)&vehicle->throttle;

		pushed = input->i * input->i + input->j * input->j + input->k * input->k > 0.001f * 0.001f;
	}
	if ((!pushed || function_f5d70(vehicle_index)) &&
		vehicle->unknown38e != (1 << *(long *)(definition + 0x2f0)) - 1 &&
		(vehicle->unknown248 == NONE || !(vehicle->linear_velocity.i * vehicle->linear_velocity.i +
			vehicle->linear_velocity.j * vehicle->linear_velocity.j +
			vehicle->linear_velocity.k * vehicle->linear_velocity.k > 3.0625f)))
	{
		vehicle->unknown34a = (byte)MIN((char)vehicle->unknown34a + 1, 0x7e);
	}
	else
	{
		vehicle->unknown34a = 0;
	}
}

/* the vehicle type's own update after its physics: types 3 and 4 */
// @retail 0xf6010
void function_f6010(long vehicle_index)
{
	switch (*(short *)(VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index)) + 0x1f0))
	{
	case 3:
		function_f6060(vehicle_index);
		break;
	case 4:
		function_f62f0(vehicle_index);
		break;
	}
}

/* the collision result as 0xf6060 reads it */
struct s_vehicle_wash_collision
{
	long type;
	real t;
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[2];
	vector3f normal;
	byte unknown34[0x5c - 0x34];
};

void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle,
	real max_angle);
real function_1201a0(vector3f *v, vector3f const *fallback);
void function_1763a0(point3f const *point, vector3f const *direction, s_effect_marker *markers,
	vector3f const *normal);
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);

/* a hovering vehicle's wash: from each of its engine markers, a ray down a
   cone around the marker's forward, and the definition's effect where it
   hits, fading with the distance */
// MiniMax B (11-13): no new form, no lower score (48): the marker count sum still lands in EAX here and in EDI in retail
// (a short, a long and a split `count +=` form all compile to the same `movsx eax, ax; add eax, edi`), the definition
// pointer is still read into EDX early where retail reads it into EAX after the memset, and the facing copy still stores its
// three components in the order i, k, j where retail stores j, k, i (the member-wise copy scores 67); the cone call's set-up
// pair still waits on 0x146320's file.
// MiniMax G (11-12): no new form, no lower score (48): the cone call's arguments still ride in the other registers and the
// count's zero store still follows the sum.
// MiniMax B (11-11): no lower score (48): the count written as two statements and the count kept in a named local
// both compile to the same code as the single expression, so the sum still lands in EAX and the zero store still
// follows it.
// MiniMax D (11-11): 47 (was 48), NOT kept (coordinator: under the 8-point bar, the body stays at 48): the two zero fields of the effect parameters now share one chained assignment (unknown3c and unknown30); what is left is that the two marker counts sum into EAX where retail sums them into EDI, the colour constant takes EAX where retail takes ECX, and the definition pointer is read into EDX early where retail reads it into EAX at the end.
// MiniMax S (11-10): no new form, no lower score (48): the marker count sum still lands in EAX here and in EDI in retail, the scratch zero store still follows the sum here and precedes it in retail, and the 0x146320 set-up pair still waits on that file.
// MiniMax E (11-08): no new form, no lower score (48); the marker count sum still lands in EAX here and in EDI in retail, the scratch zero store still follows the sum here and precedes it in retail, the effect block's tag and colour still ride in the other order, and the 0x146320 set-up pair still waits on that file.
// MiniMax E (11-07): no new form, no lower score; the marker count sum still lands in EAX here and in
// EDI in retail, and the 0x146320 set-up pair still waits on that file.
// MiniMax R (11-01): no new form, no lower score; a long local for the second count and the sum's
// operands swapped both leave the second count's sign extension in EAX.
// MiniMax R (11-06): forms tried, none lower (48; a named short count compiles the same); the marker count sum lands in EAX here and in EDI in retail, and the scratch zero store follows the sum here and precedes it in retail.
// score 48 (was 48): from the marker count sum on, retail's scratch registers run one step
// ahead of ours (retail `movsx ecx, ax; add edi, ecx`, ours `movsx eax, ax; add eax, edi`; in the
// loop retail dx/eax/ecx where ours use cx/edx/eax; ours loads the definition first in the effect
// block), plus the 0x146320 set-up pair (its declared order, BLOCKED-BY 0x146320: seed declared
// first fixes the pair only, 51). Tried: the sum's operands swapped 53; `count += second` 53; a short
// local for the second count 53; `i` declared first 53; declarations without initialisers 53; `long
// i` 117; the loop bound written as the sum of the two counts 52; a short count 128; the speed choice
// as an if/else 53, into a named local 53. Round 85: the tag index stored after the owner fields
// (retail's store order) 48 (kept); then colors before the source 49; scale after the memset 48;
// `i` declared in the for 48; `count +=` 48; the sum's operands swapped 48. Round 90: no marker
// pointer 129; the ray through vector3f_scale 48; facing declared with the loop locals 48; `long count`
// declared before the first count 48; count from the second call, then `+= primary_count` 48 (the
// 48s are the same code: the second count's sign extension stays preferenced to eax).
// Round 420: native facing producer 67; native ray producer 48; native facing producer + native ray
// producer 67; collision miss continue 48; native facing producer + collision miss continue 67; native
// ray producer + collision miss continue 48; native facing producer + native ray producer + collision
// miss continue 67; native facing producer + native ray producer + collision miss continue + readonly
// marker 67; total owns first marker result 141. Original body restored.
// Round 91: long primary count 169; primary-count address reference 48; direct definition macro at
// the tag store 138; const definition 48; const primary count 48; const total 48; facing copied in
// j/k/i order 67; four-byte-aligned short primary count 127 (prologue changed). Original body restored.
// MiniMax D (11-10): no new form, no lower score (48): the two marker counts still sum into EAX where retail
// sums them into EDI, the scale is still computed before the memset where retail computes it after, and the
// colour constant still takes EAX where retail takes ECX; the permuter's run found nothing below 48.
// Round 147 (m3 lane 7, fast, with 0xf62f0 and 0xf6010): the colours as one chained assignment 48; the scales chained
// 50; length declared at its assignment 48; length and the ray vector declared where computed 48; the ray products
// written `length * direction.i` 48 (all same code but the scales). No untried natural form is left here that
// does not depend on the cone call's register order (BLOCKED-BY 0x146320; its file is held).
// MiniMax I (11-14): no new form, no lower score (48): the marker sum still lands in EAX (movsx eax, ax;
// add eax, edi) where retail has movsx ecx, ax and add edi, ecx; the sum's operand order, a second count
// variable (short, int or long), the compound sum and int-typed counts all compile to the same EAX form,
// and the int counts change the loop (169).
// MiniMax A (11-13): no new form tried this round, no lower score (48): the marker count's sum still lands in
// EAX here (movsx eax, ax; add eax, edi) and in EDI in retail (movsx ecx, ax; add edi, ecx), the definition
// pointer is still read into EDX before the memset where retail reads it into EAX after it, and the facing copy
// still stores its three components in the order i, k, j where retail stores them in another order.
// @retail 0xf6060
void __stdcall function_f6060(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);

	if (*(long *)(definition + 0x2a0) != NONE)
	{
		s_object_marker markers[16];
		short primary_count = function_b8d30(vehicle_index, 0xf0000bb, markers, 15, false);
		long count = primary_count + function_b8d30(vehicle_index, 0xd0000bc, &markers[primary_count],
			16 - primary_count, false);
		short i;

		for (i = 0; i < count; i++)
		{
			s_object_marker *marker = &markers[i];
			s_vehicle_wash_collision collision;
			vector3f direction;
			vector3f vector;
			real length;

			collision.unknown24 = NONE;
			random_vector_in_cone(&marker->matrix.forward, &direction, &g_4e7408->seed, 0.0f, 0.2617994f);
			length = (i < primary_count ? vehicle->unknown370 : vehicle->unknown374) * 6.0f + 2.0f;
			vector.i = direction.i * length;
			vector.j = direction.j * length;
			vector.k = direction.k * length;
			if (function_1697c0(0x800007, &marker->matrix.position, &vector, vehicle_index, NONE,
				(s_collision_result_1697c0 *)&collision))
			{
				s_effect_marker effect_markers[6];
				s_effect_parameters parameters;
				vector3f facing = direction;
				real scale;

				function_1201a0(&facing, g_4687bc);
				function_1763a0(&collision.point, &facing, effect_markers, &collision.normal);
				scale = 1.0f - collision.t;
				memset(&parameters, 0, sizeof(parameters));
				parameters.unknown34 = 0;
				parameters.unknown38 = 0;
				parameters.unknown3c = 0;
				parameters.unknown30 = 0;
				parameters.source = NULL;
				parameters.color_a = 0xff808080;
				parameters.color_b = 0xff808080;
				parameters.unknown18 = NONE;
				parameters.object_index = NONE;
				parameters.owner.unknown4 = NONE;
				parameters.owner.unknown0 = NONE;
				parameters.owner.unknown8 = NONE;
				parameters.tag_index = *(long *)(definition + 0x2a0);
				parameters.marker_count = 6;
				parameters.markers = effect_markers;
				parameters.flags = 4;
				parameters.scale_a = scale;
				parameters.scale_b = scale;
				effect_new_from_parameters(&parameters);
			}
		}
	}
}

extern byte g_5107ee;

/* a skating vehicle's spray: where a ray from each of its spray markers
   meets the ground, the definition's effect, its strength from the
   marker's downward pitch, the distance and the vehicle's speed; the
   effect's last marker sits between the marker and the hit, aimed along
   the reflected ray */
// score 2 (was 2): left: the cone call's set-up pair only (ours adds the seed's offset before it takes the forward's
// address, retail the reverse). The same swap sits at the cone call in 0xf6060 and 0x17c0e0, so it follows the cone
// function's declaration and convention, not this body: BLOCKED-BY 0x146320 (its file is held).
// MiniMax B (11-11): no new form, no lower score (41): retail's block holds the middle point's z and y words until
// after the first products and keeps the colour constant in ECX across them; no source form I tried moves either.
// MiniMax D (11-11): no lower score (2): the cone call's forward address and its seed address each in a
// named local compile to the same code, so the seed's add still precedes the forward's lea where retail has
// them the other way round; that pair belongs to 0x146320's parameter order, whose file is not this lane's.
// The seed's address as byte-pointer arithmetic and as an index into the globals compile the same (2), and
// the forward taken from the marker array rather than from the marker pointer scores 4. This file's
// declaration of the cone call is what fixes the register mapping: swapping its first two parameters moves
// the seed into EAX and the result into EDX, which is not what 0x146320 reads, so the order cannot change.
// MiniMax D (11-11): 2 (was 6): the name store now comes before the three forward values and before the
// colour stores, and the compiler then emits the name and the three forward values in retail's order;
// what is left is the cone call's set-up pair, where ours adds the seed's offset before it takes the
// forward's address and retail does the reverse.
// MiniMax D (11-11): no lower score (6): the name store moved ahead of the three forward values scores 7,
// and the same order with the three forward values in named locals scores 7 as well, because the compiler
// sinks the three products and differences to their stores; retail computes them before the colour stores
// and stores the name ahead of the three forward values, which our compiler does not do in one order.
// MiniMax D (11-10): 6 (was 9): the three forward values are now computed before the colour stores, as
// retail does, and the name store comes after them; what is left is the cone call's EDI-before-ECX pair
// (it waits on 0x146320's file) and the four stores' order, where retail keeps the name ahead of the
// three forward values.
// MiniMax D (11-10): the flag test through the field-bit macro, the tag test and the strength test as
// nested ifs, the marker, direction, collision and vector locals in retail's declaration order, and the
// colour store ahead of the source store.
// MiniMax S (11-10): no new form, no lower score (41): the cone call's EDI-before-ECX pair still waits on 0x146320's file, and the middle-point copy is still scheduled ahead of the reflection's loads.
// MiniMax E (11-08): no new form, no lower score (41); the middle-point copy is still scheduled ahead of the reflection's loads where retail stores its z and y words after the first products, retail keeps the colour constant in ECX across those products while ours materialises it in EAX, and the cone call's EDI-before-ECX pair still waits on 0x146320's file.
// MiniMax E (11-07): no new form, no lower score (41); the cone call's EDI-before-ECX pair still
// waits on 0x146320's file, and the middle-point copy is still scheduled ahead of the reflection's loads.
// MiniMax R (11-06): no new form, no lower score (41); a named seed pointer for the cone call compiles
// the same, and what is left is the cone call's EDI-before-ECX pair and the middle-point, dot and memset order.
// Earlier 41 (was 41): earlier 160; left: the cone call sets up EDI before ECX (retail ECX first; the same in
// 0xf6060, BLOCKED-BY 0x146320, whose file is held) and the middle-point copy is scheduled ahead of
// the reflection's loads (retail stores its z and y words after the first products).
// Frame 0x8cc agrees; count/marker carriers, strength residence and reflection schedule remain.
// Round 384: sixteen-marker capacity 186; with shared vector setter 170; indexed loop 186, with
// setter 170; complete reflection input captures 190/205/190/205.
// Late facing producer and scalar/point home names 170 each (four forms); plain admission-owned
// long/short/word loops 170/163/160, readonly marker 170.
// Retained sixteen native marker records, shared vector assignment and plain word-indexed marker
// loop; requested count and all vector/call values unchanged.
// DL180: facing setter 173; midpoint setter 160; facing/midpoint setters 173; reflection setter
// 160; facing/reflection setters 173; midpoint/reflection setters 160; all setters 173; in-place
// direction 183; long countdown 174; short countdown 174; count countdown 158; countdown/all
// setters 153; countdown/facing setter 153; countdown/midpoint setter 158;
// countdown/facing+midpoint 153; countdown/reflection setter 158; countdown/facing+reflection
// 153; countdown/midpoint+reflection 158; positive word countdown 159; positive dword countdown
// 165. Original executable body restored.
// J197: existing complete initializer, ordinary/inline/forced 330/276/159;
// request construction before vector helpers, forced/inline/ordinary 181/316/333;
// forced initializer with bounded strength 177, with native countdown 166.
// Count/marker/strength homes and reflection schedule remain.
// DL196: endpoint/request-owned strength/projection-product subsets
// 161/181/169/165/162/184/163; native full-width count 160.
// Original executable body and all supporting contracts restored.
// Round 43 (lane 16): long index 170 (strength-reduced, pointer at +0x54); short index 163
// (retail's test/movzx count); with the strength as PIN(..., 0.0f, 1.0f) and `if (strength > 0.0f)`
// 84 (kept: pointer at +0x38, spilled strength, frame); dot grouped 86; dot terms written i, j, k
// 80 (kept); position copy before the memset 53 (kept); colours, name, forward, NONE fields, tag
// order 43 (kept); copy before the dot / copy and memset before the dot 43/43; facing copied after
// the middle point 41 (kept); copy after the memset 39 (retail stores two words before it, not kept);
// dot declared where computed 41; dot3f(normal, facing) / dot3f(facing, normal) 45/45; a pointer to
// effect_markers[6] 41; the reflection through vector3f_assign 41.
// Round 90: incoming/normal/bounce pointers 45; incoming+normal 45; bounce alone 41;
// sequential dot with component midpoint stores 165 (frame 0x8bc), with the whole copy 47;
// reflection-order pointers 45, also before the vector providers 45; point setter 154
// (frame 0x8bc); normal pointer 43; incoming pointer 45. Original executable body restored.
// B392: Plain midpoint/copy/reflection loops: 43/41/43/41/41/41/41/41; native midpoint aggregate with CRT
// publication/projection-part follow-ups: 41/41/149/149. Original body restored; cone setup and
// reflection schedule remain.
// Round 147 (m3 lane 7, fast, with 0xf6060 and 0xf6010): the colours as one chained assignment 43; the scales
// chained 45; dot declared before middle 41 (same code); the dot written `2.0f * (...)` 41 (same code); the colours
// stored right after the memset 80 (891 bytes). Retail loads the colour constant into ECX before the zero stores and
// the middle copy's loads after the dot's; the cone pair stays BLOCKED-BY 0x146320.
// Round 86 (m3 lane 14, fast): the collision's NONE store after the cone call 19; a const marker pointer 2; the marker
// pointer declared ahead of the loop 2; the marker through a reference 2; the direction declared ahead of the marker
// pointer 2 (all four the same code).
// Round 80 (m3 lane 15, fast with 0xf6060 and 0xf6010; the cone pair is still the only difference): the random globals
// in a loop-body local for the seed's address 2 (same code); the seed through a reference 2 (same code); the seed's
// address taken once ahead of the marker loop 148; the random globals read once at the function's top 179; an inline
// forwarding wrapper declared seed first (diagnostic) 2 (same
// code: an inlined wrapper's arguments are propagated, so the callee's own order still sets the pair). Retail's set-up
// (the two pushes, the forward's `lea ecx`, the seed's `add edi, 4`, the result's `lea eax` last) is the right-to-left
// evaluation of a cone function declared with the seed ahead of the forward, so no caller-side form can give it:
// still BLOCKED-BY 0x146320, whose file this round's rules hold.
// @retail 0xf62f0
void __stdcall function_f62f0(long vehicle_index)
{
	if (TEST_FIELD_BIT(g_5107ee))
	{
		s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
		byte *definition = VEHICLE_DEFINITION_GET(vehicle);

		if (*(long *)(definition + 0x2a0) != NONE)
		{
			if (vehicle->unknown25c > 0.0f)
			{
				s_object_marker markers[16];
				short count = function_b8d30(vehicle_index, 0xf0000bb, markers, 15, false);

				for (short marker_index = 0; marker_index < count; ++marker_index)
				{
					s_object_marker *marker = &markers[marker_index];
					vector3f direction;
					s_vehicle_wash_collision collision;
					vector3f vector;

					collision.unknown24 = NONE;
					random_vector_in_cone(&marker->matrix.forward, &direction, &g_4e7408->seed, 0.0f, 15.0f);
					vector3f_assign(&vector, direction.i, direction.j, direction.k);
					if (function_1697c0(0x800007, &marker->matrix.position, &vector, vehicle_index, NONE,
						(s_collision_result_1697c0 *)&collision))
					{
						real strength = PIN((1.0f - collision.t) * (0.0f - marker->matrix.forward.k) * vehicle->unknown25c,
							0.0f, 1.0f);

						if (strength > 0.0f)
						{
							s_effect_marker effect_markers[7];
							s_effect_parameters parameters;
							point3f middle;
							real dot;

							middle.x = (marker->matrix.position.x + collision.point.x) * 0.5f;
							middle.y = (marker->matrix.position.y + collision.point.y) * 0.5f;
							middle.z = (marker->matrix.position.z + collision.point.z) * 0.5f;
							vector3f facing = direction;
							function_1201a0(&facing, g_4687bc);
							function_1763a0(&collision.point, &facing, effect_markers, &collision.normal);
							dot = (collision.normal.i * facing.i + facing.j * collision.normal.j +
								facing.k * collision.normal.k) * 2.0f;
							effect_markers[6].position = middle;
							memset(&parameters, 0, sizeof(parameters));
							parameters.unknown34 = 0;
							parameters.unknown38 = 0;
							parameters.unknown3c = 0;
							parameters.unknown30 = 0;
							effect_markers[6].name = 0x80000ba;
							effect_markers[6].forward.i = facing.i - collision.normal.i * dot;
							effect_markers[6].forward.j = facing.j - collision.normal.j * dot;
							effect_markers[6].forward.k = facing.k - collision.normal.k * (real)dot;
							parameters.color_a = 0xff808080;
							parameters.source = NULL;
							parameters.color_b = 0xff808080;
							parameters.unknown18 = NONE;
							parameters.object_index = NONE;
							parameters.owner.unknown4 = NONE;
							parameters.owner.unknown0 = NONE;
							parameters.owner.unknown8 = NONE;
							parameters.tag_index = *(long *)(definition + 0x2a0);
							parameters.marker_count = 7;
							parameters.markers = effect_markers;
							parameters.flags = 4;
							parameters.scale_a = strength;
							parameters.scale_b = strength;
							effect_new_from_parameters(&parameters);
						}
					}
				}

			}
		}
	}
}

/* the bounded world a havok component queries rays against */
class c_bounded_query_world
{
public:
	bool function_182800(s_vehicle_ray *ray, long ignore_index);
};
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore_object_index, long ignore_unit_index);
byte function_11f470(real lo, real hi, real value);
long function_189060(long object_index, short value, real scale, point3f const *position,
	vector3f const *direction, long tag_index);

/* the vehicle's suspension: for each of its graph's function animations
   whose marker it has, a ray down from the marker finds how far the ground
   pushes it (every fourth tick, a spread of the vehicles' work, else the
   last one), which sets the function's value; a hard landing plays the
   definition's sound */
// @retail 0xf6670
bool __stdcall function_f6670(long vehicle_index, bool update)
{
	bool sounded = false;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	s_animation_state *state = vehicle_animation_state_get(vehicle);

	if (!(definition[0x2ac] & 1))
	{
		long permutation_count;
		long permutations;
		s_graph_iterator3c iterator;
		real largest_change = 0.0f;

		function_ba690(vehicle_index, &permutation_count, NULL, NULL, &permutations);
		bool changed = false;

		while (function_1dceb0(&iterator, graph_tag_get(state->graph_tag_index)))
		{
			long index = iterator.index;
			byte current_value = ((byte *)vehicle->unknown398)[index];
			real current = current_value == 0xff ? 1.0f : current_value * (1.0f / 255.0f);
			s_object_marker marker;

			if (function_b8d30(vehicle_index, iterator.unknown00, &marker, 1, false) == 1 && iterator.unknown00)
			{
				real offset;
				real length;
				real rest;
				real value;
				long override_count = *(long *)(definition + 0x2f0);
				byte *override = NULL;
				long i;

				for (i = 0; i < override_count; i++)
				{
					byte *entry = *(byte **)(definition + 0x2f4) + i * 0x4c;

					if (*(long *)(entry + 0x44) == iterator.unknown04 && *(long *)(entry + 0x48) != NONE)
					{
						override = entry;
						break;
					}
				}

				if (override && (char)((byte const *)permutations)[*(long *)(override + 0x48) * 8 + 1] >=
					*(short *)(override + 0x42))
				{
					offset = iterator.unknown14;
					rest = iterator.unknown1c;
					length = iterator.unknown18;
				}
				else
				{
					offset = iterator.unknown08;
					rest = iterator.unknown10;
					length = iterator.unknown0c;
				}

				s_vehicle_ray ray;

				ray.point = marker.matrix.position;
				ray.vector = vehicle->up;
				vector3f_add_scaled((vector3f const *)&ray.point, &ray.vector, offset,
					(vector3f *)&ray.point);
				real reach = 0.0f - length;

				ray.vector.i = ray.vector.i * reach;
				ray.vector.j = ray.vector.j * reach;
				ray.vector.k = ray.vector.k * reach;
				if (!marker.node_index)
				{
					if (update && (g_510c54->game_time + index) % 4)
					{
						byte last = ((byte *)vehicle->unknown390)[index];

						value = last == 0xff ? 1.0f : last * (1.0f / 255.0f);
					}
					else
					{
						c_bounded_query_world *world = NULL;
						s_havok_component *component;

						if (vehicle->havok_component_index != NONE)
						{
							component = havok_component_get(vehicle->havok_component_index);

							if (TEST_FIELD_BIT(component->flag5))
							{
								world = (c_bounded_query_world *)component->unknown9c;
							}
						}

						if (world && world->function_182800(&ray, NONE))
						{
							real compression = ray.t * length - rest;
							real travel = length - rest;

							value = 1.0f - compression / travel;

							if (value < 0.0f)
							{
								value = 0.0f;
							}
							else if (value > 1.0f)
							{
								value = 1.0f;
							}
						}
						else
						{
							value = function_168f40(0x8c2d, &ray, vehicle_index, NONE) ? 1.0f : 0.0f;
						}
						((byte *)vehicle->unknown390)[index] = function_11f470(0.0f, 1.0f, value);
					}
				}
				else
				{
					value = 0.0f;
				}
				if (value - current > largest_change)
				{
					largest_change = value - current;
				}
				long quantized = function_11f470(0.0f, 1.0f, update ? (value + current) * 0.5f : value);

				if (((byte *)vehicle->unknown398)[index] != quantized)
				{
					changed = true;
					((byte *)vehicle->unknown398)[index] = quantized;
				}
			}
		}
		if (*(long *)(definition + 0x288) != NONE && largest_change > 0.3f && update)
		{
			function_189060(vehicle_index, NONE, PIN((largest_change - 0.3f) * 1.6666667f, 0.0f, 1.0f), g_468788,
				g_4687a8, *(long *)(definition + 0x288));
			sounded = true;
		}
		if (changed)
		{
			function_bba20(vehicle_index);
		}
	}
	word_flags_set((word *)&vehicle->flags348, 8, update);
	return sounded;
}

real magnitude3d(vector3f const *v);
void function_11d580(vector3f const *a, vector3f const *b, vector3f *projection,
	vector3f *rejection);
bool unit_action_active(long unit_index, long action_type);

/* a value over a maximum, 0 when the maximum is too small */
// @retail 0xf78b0
real __stdcall function_f78b0(real value, real maximum)
{
	real const *values = &value;
	real result = 0.0f;

	if ((real)fabs(maximum) > 0.001f)
	{
		result = *values / maximum;
	}
	return result;
}

/* the larger of two magnitudes */
PRIVATE __forceinline real vehicle_larger(real a, real b)
{
	real magnitude_a = (real)fabs(a);
	real magnitude_b = (real)fabs(b);

	return magnitude_a > magnitude_b ? magnitude_a : magnitude_b;
}

/* one of the vehicle definition's limits: its forward and reverse speeds
   (0, 1, the larger 2), its left and right turn speeds (3, 4, 5) and its
   steering angles in degrees (6, 7, 8) */
// @retail 0xf78e0
real __stdcall function_f78e0(long vehicle_index, long limit)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real result;

	switch (limit)
	{
	case 0:
		result = (real)fabs(*(real *)(definition + 0x1f4));
		break;
	case 1:
		result = (real)fabs(*(real *)(definition + 0x1f8));
		break;
	case 2:
		result = MAX((real)fabs(*(real *)(definition + 0x1f4)), (real)fabs(*(real *)(definition + 0x1f8)));
		break;
	case 3:
		result = (real)fabs(*(real *)(definition + 0x22c));
		break;
	case 4:
		result = (real)fabs(*(real *)(definition + 0x230));
		break;
	case 5:
		result = MAX((real)fabs(*(real *)(definition + 0x22c)), (real)fabs(*(real *)(definition + 0x230)));
		break;
	case 6:
		result = (real)fabs(*(real *)(definition + 0x204) * 0.017453292f);
		break;
	case 7:
		result = (real)fabs(*(real *)(definition + 0x208) * 0.017453292f);
		break;
	case 8:
		result = MAX((real)fabs(*(real *)(definition + 0x204) * 0.017453292f),
			(real)fabs(*(real *)(definition + 0x208) * 0.017453292f));
		break;
	default:
		__assume(0);
	}
	return result;
}

bool __stdcall function_f8070(long bit, word const *flags);

/* the vehicle object type's function values (+0x4c): its boost, brakes,
   ground contact, speeds, steering, engine and wheels; false for a name it
   does not know */
// MiniMax B (11-13): no new form, no lower score (749): the frame and the length are retail's, but the entry's zero and its
// 1.0 still take XMM4 and XMM5 where retail takes XMM2, and the definition receiver and the per-site 1.0 lifetime still differ.
// MiniMax G (11-12): no new form, no lower score (749): nothing new this round; the frame and the length are retail's, and the
// entry definition ownership, the zero/result SSE allocation and the per-site versus hoisted 1.0 constants
// are still not retail's.
// MiniMax G (11-12): no new form, no lower score (749): frame 0x18 and retail's length agree, but the entry
// definition ownership, the zero/result SSE allocation and the per-site versus hoisted 1.0 constants are still not
// retail's.
// MiniMax H (11-11): no new form, no lower score (749); the definition pointer is still re-derived at each use instead of
// retail's one copy in EBP, the zero and the result still take different SSE registers and the 1.0 is still hoisted here.
// score 749 (was 749): Frame 0x18 and retail length agree; entry definition ownership, zero/result SSE allocation and per-site versus hoisted 1.0 remain (was 765 before round 180).
// MiniMax S (11-10): no new form, no lower score (749); the definition pointer is re-derived from the tag table at each use while retail keeps one copy in a callee-saved register (used at seven of its sites) and re-derives it at nine others; the zero and the result take different SSE registers and the 1.0 is loaded once per site here.
// MiniMax R (11-09): no new form, no lower score (749); the definition pointer kept in a local unifies the branches and regresses to 1027, and retail's four-push prologue and per-site 1.0 load are not reachable from the source.
// MiniMax R (11-06): no new form, no lower score (749); the frame and the length agree, and the SSE zero register is xmm4 here and xmm2 in retail.
// Earlier at 817 (was 864): Frame 0x18 agrees; definition EBP and scalar/SSE allocation differ.
// v1C statement walk (retail 0xf6d10-0xf78ab, 2026-11-06): every case label, field offset, condition direction, constant,
// callee and argument order, clamp/forced/active publication and the default return agree with retail; no behaviour
// difference. x87 product operand order is canonicalised by the compiler whatever the source order (0xb000587's third
// term swapped or summed first 749); a named limit local in 0xa00001a 754; function_f78e0(vehicle_index, 6) at 0x900000e
// stays a call 772; entry definition for the 0x264/0x1f0/0x20c arms with VEHICLE_DEFINITION_GET or tags 999/1027 (2730/2707 bytes).
// Complete definition snapshot, result-first, direct active predicate in seven nonempty
// combinations 1006/864/1006/869/1002/869/1002; snapshot with numeric movement-case
// source order 1006. Original observations, switch and body restored.
// Round 304: entry-owned kind/angular-ceiling/engine subsets 1010/1010/1010/1010/1010/1010/1010;
// complete families with staged cross 1010. Original body restored; definition and float-return boundaries remain.
// Round 182: 0xf78e0 now matches (its limits return in xmm0), 864 -> 817; the inline larger-of sites as
// `MAX((real)fabs(a), (real)fabs(b))` as 0xf78e0 now spells them 830 (fast).
// Round 45 (lane 1): ours keeps 1.0 in XMM5 from entry (retail loads it per site; every callee writes only
// XMM0/XMM1); the final clamp as if/else-if 817 (identical); the 0xc0005a5 exit without `forced = false` 822
// (retail clears BL there); both 822; result declared before forced 817; result declared first 817 (identical).
// J221: Shared lateral cross and native byte forced state 817/817; full-width state 820; common recognized-name exit 817, with cross/full-width 818/820; ascending/descending complete cases 817/817; both Boolean-valued channels produced as floating indicators 813 (four points, below eight; restored). Original observations, result/control graph and complete body restored.
// Round 496: One tag-table receiver for every definition lookup 765, plain kept; complete arm-owned
// results 839, with the table 768; result graph with long/byte forced state 842/839; one
// recognized-name exit 833; table/result with long forced/one exit 765/766. Only the table receiver retained.
// P252: Channel result/forced state, blend/reverse scalar and native vector-output role-name subsets
// 1/2/4/3/5/6/7 765 each; all with the pure forced-state constant declared first 765. Original complete body
// restored; frame 0x18, entry definition and zero/result SSE allocation remain.
// Round 473: Existing magnitude selector with initialized-B/early/complete/
// initialized-A result graphs, original/early tag-table receiver: 833/765/765/807/835/768/768/810.
// Helper and full body restored; entry definition, zero/result roles and hoisted 1.0 remain.
// B358: Complete shared definition ordinal / paired velocity-selection observation /
// existing projections, subsets 1/2/4/3/5/6/7: 919/792/765/941/919/792/941;
// all with a native tag-entry receiver 1012. Projection forms are match-only
// for grouping; no candidate qualifies. Original complete body restored.
// The 0x18 frame agrees; entry definition and zero/result/constant residence remain.
// P268: Actual limit-record native field families (kind/spin/engine, speeds, steering/turn),
// seven subsets 765 each; complete typed const-record reference 1019 (2676 bytes).
// All types/observations/body restored; entry definition and zero/result/1.0 homes remain.
// AC481: active truth from the published output / read-only tag-table receiver /
// read-only vehicle receiver, subsets 1/2/4/3/5/6/7: 765 each; all with table
// acquisition before the pure object lookup 768. Original body restored.
// M319: Common normalization with bool/byte/short/long forced state, original/direct
// truth publication: 765/765/749/749/749/749/749/749. Minimal direct Boolean
// publication alone 749 fast/plain, 2974/2974 bytes; retained with original
// bypasses and bool state. Initial SIMD/definition allocation remains.
// AC494: Entry kind/spin/engine definition families on the retained table/direct-active
// graph, masks 1/2/4/3/5/6/7, then table-first: 1027/1027/1027/1027/1027/1027/1027/1026 (2707/2974 bytes). The compiler
// merges the later definition reads too; original observations and complete body restored.
// NOT kept: entry ownership merges later reads that remain separate in the native code.
// P283: Shared ratio-provider inputs for angular/engine/scalar-limit families,
// subsets 1/2/4/3/5/6/7, then entry declaration: 749/805/688/849/771/857/914/914.
// NOT kept: shared one-use inputs and join labels 688. Original direct calls with
// calculated limits named per case (all/steering/turn): 756/755/789. All restored;
// frame 0x18 agrees, zero/result SIMD, entry definition and per-site one homes remain.
// M327: Complete Boolean / guarded boost / admitted magnitude result producers on the retained
// tag-table and direct-active graph, masks 1/2/4/3/5/6/7: 777/749/749/777/777/749/777;
// all with direct Boolean-to-real results 751. Original full body restored; zero/result/1.0 and entry definition homes remain.
// AC499: Float magnitude boundaries for fields/computed scalars/projections, seven subsets
// then the existing magnitude selector too: 772/760/739/769/751/738/768/768. NOT kept: 738 grows
// to 3010/2974 bytes and changes rounding boundaries without a match. Original full body/helper restored.
// N338: Complete blend accumulation, native blend clamp and averaged-ratio
// accumulator subsets, then both blend contributions named, 749 each at 2974/2974.
// Original complete body restored; entry definition, zero/result SIMD and unit-constant homes remain.
// P286: Terminal active truth: forced-first logical/eager normalized 749/749; positive/reject
// branch returns 765/765; positive/forced complete arms 769/755; forced/positive default
// publications 784/771. All restored; entry definition and zero/result/one residency remain.
// F352: Contact-admission truth in bool/byte/short/long 749/749/749/749;
// ground-admission truth in those domains 749/749/749/749. NOT kept:
// one-use predicates do not match. Original gates/body restored; frame 0x18 agrees,
// but entry definition and zero/result/one residency remain.
// P291: Reverse-case zero/complete-arm/existing-ratio-helper/magnitude-owned results,
// conditional then branched reverse selection: 753/751/797/751/753/751/797/751.
// All restored; frame 0x18, entry definition and zero/result/one residence remain.
// Y566: Existing up/selected-forward/lateral/forward dot producers on the direct-active
// graph, individual masks 1/2/4/8, paired 10/13, all, all with staged lateral cross:
// 749 each at 2974/2974. Original complete body restored; zero/result/one and entry definition homes remain.
// AC511: Actual lateral/projection/rejection vector output entry lifetimes, seven
// subsets and complete graph with existing staged lateral cross: 749/749/749/749/749/749/749/749.
// NOT kept: frame 0x18 and 2974-byte extent agree but entry definition and zero/result/one
// homes remain; original output scopes, cross and complete body restored.
// J245: Magnitude selector on direct-active graph: parameter-owned ternary/early/joined/
// first-default/second-default, named joined result, MAX expression, named second-default: 749/749/749/803/829/749/749/829.
// NOT kept: no gain; default-result forms shorten away from 2974 bytes. Frame 0x18 agrees; entry definition and zero/result/one homes remain.
// P300: Existing steering/turning/speed limit-query families, graphs 1/2/4/3/5/6/7:
// 883/963/1192/1076/1290/1395/1504; all plus reverse maximum 1514. No gain; every
// form shortens away from 2974 bytes. Original provider boundaries/body restored;
// entry definition and zero/result/one residence remain.
// J253: Coordinated magnitude/projection inline/forced contracts
// 759/756/759/825/845/967/846/842; magnitude inline removes the unity hoist
// but grows the extent. Existing tag-getter entry-only/all-definition receivers
// 999/1015 (2730/2680 bytes); all provider contracts and complete body restored.
// P326: Selected-velocity blend/reverse speed and limit/combined speed-turn scalar
// declarations before dispatch, subsets 1..7, then all before object lookup: 749 each.
// Initializers stayed at case producers. NOT kept: no gain; original body and SSE homes remain.
// F381: Case-owned twice-used kind/gear/reverse-speed observations, subsets
// 1/2/4/3/5/6/7 then const all: 749/749/773/749/773/773/773/773.
// NOT kept: ties/regressions; reverse residence shortens to 2952/2974.
// Original observations/body restored; native entry definition and SSE homes remain.
// F387: Complemented forced-active state in bool/byte/short/long with logical
// terminal truth 749/749/749/749; complete terminal arms 763/763/748/748.
// NOT kept: one-point leads below gate; entry definition and SSE homes remain. Original body restored.
// B445: Actual recognized-name return on direct-active graph, failure/success defaults in
// bool/byte/short/long: 790/757/752/752/749/774/764/761. NOT kept: ties or regressions; whole body restored.
// Frame 0x18 agrees; native entry definition and zero/result/one residence remain.
// B447: Complete dispatch/normalization boundary, caller/provider entry receiver,
// pointer/reference scalar outputs: forced inline 1034/1034/1034/1034; ordinary
// inline 3616/3616/3597/3597. NOT kept: entry/later lookups still merge; forced
// forms shorten to 2700 bytes and ordinary providers stay called. Original full
// body restored; native zero/result/one and definition ownership remain.
// P334: Native component-range up/paired-forward/lateral projection contracts,
// seven family subsets then complete reversed input ranges: 873/958/904/1078/950/1100/1219/1243.
// NOT kept: no native entry-definition or zero/result/unit-constant residence; original complete body restored.
// P336: Final/blend nested bound producers, forward MAX, reverse MIN, both bounds,
// independent final/blend comparisons, staged final bounds: 748/751/749/749/749/749/749/749.
// NOT kept: best gain one; original body restored. Entry definition and zero/result/one homes remain.
// P341: Getter-produced vehicle-record lvalues, mutable/read-only, native/immutable table records and
// original/early pure table acquisition: 749/764/749/764/749/764/749/764. NOT kept: no native entry-
// definition or zero/result/one residence; original entire body and pointer receivers restored.
// DL265: Magnitude selector widened return, seven real-magnitude result graphs, then native
// combined speed/turn first-limit MAX: 749/946/749/946/846/888/749/764. NOT kept: no eligible native result;
// MAX regressed and shortened to 2958/2974; original helper/body restored.
// Entry definition and zero/result/one homes remain.
// B447: Complete dispatch/normalization boundary, caller/provider entry receiver,
// pointer/reference scalar outputs: forced inline 1034/1034/1034/1034; ordinary
// inline 3616/3616/3597/3597. NOT kept: entry/later lookups still merge; forced
// forms shorten to 2700 bytes and ordinary providers stay called. Original full
// body restored; native zero/result/one and definition ownership remain.
// M395: Complete result/blend/reverse-turning double receiver families 1/2/4/3/5/6/7, then immutable case receivers:
// 1489/852/771/1538/1510/832/1537/1537. NOT kept: no exact native-shape candidate; native entry definition and zero/result/unit-constant residence remain.
// Original complete body restored.
// Permuter, 30 minutes (11-06): 609 from 749, only with a volatile `forced`, function_f78b0's (value, limit) arguments
// swapped at every call (a behaviour change) and vehicle_larger's operands swapped. Not kept.
// P386: Gear-admission ready/excluded results, each bool/byte/short/long:
// 749/749/749/749/749/749/749/749. NOT kept: no exact one-use decision result; original
// guard, all observations and complete body restored. Entry definition and SSE homes remain.
// P396: Explicit normalization admission, bool/byte/short/long positive and complementary:
// 749/749/749/749/749/749/749/749. NOT kept: no exact tested gate or native entry-definition/
// SIMD recovery; original bypasses, switch, observations and complete body restored.
// T76: Complete cases on the table/direct-active graph: numeric ascending/
// descending, Boolean/scalar/vector/ratio first, then stable simple/bypass first:
// eight at 749, 2974/2974 bytes. NOT kept: no entry-definition or SIMD recovery;
// original complete dispatch and body restored.
// T78: Case-owned preliminary square/chain results, mutable/immutable, averaged turning,
// paired square+chain, all three and immutable all: 749/749/749/749/749/749/749/749.
// NOT kept: no gain or native entry-definition/SIMD recovery. Original complete body
// restored; frame 0x18 agrees, zero/result/unity residence remains.
// Round 180 (m3 lane 4, fast): LEAD (match-only, not kept). Retail's limit reads at 0x900000e/0xa00000f/0xa00001a/
// 0xb00001b/0xb000713/0xc000582/0xd00057f/0xe00057a/0xe00057c/0xe00057d/0xe000583/0xf000584/0x10000585/0x1300057e are
// function_f78e0's own case code expanded in place (its result staged through a parameter home: `fstp [esp+0x2c];
// mov edx, [esp+0x2c]; ...; push edx; fabs; push ecx`), each re-deriving the definition from the tag table, while
// the entry definition in EBP serves only the 0x264/0x1f0/0x20c fields. Entry definition from `vehicle` with the
// limit sites re-looking the vehicle up in place 768 (3014 bytes, the pool goes to EBP); entry definition through
// a second in-place lookup with the sites through `vehicle` 835 (2897 bytes, aligned 266 -> 143: retail's entry and
// EBP uses); on that, named limit locals at the four single-limit sites 818 (no staging through memory). With
// function_f78e0 marked `inline` and called at every limit site 797 (3029 bytes; our inliner expands the first 15
// sites in source order and keeps the limit-0 sites called, retail expands 15 too, a different set); moving cases
// 0x10000585/0x1300057e to the top (diagnostic) 723 at 2974 bytes, aligned 374; moving 0xb000587, 0xb000597/0xc0005a5
// and 0xd000586 after 0x1300057e gives retail's exact nine f78e0 calls: 736 at 3011 bytes, aligned 105 (a case
// order picked by score: match-only). Left on it: the zero/result SSE numbering (ours xmm3/xmm2, retail xmm2/xmm1),
// retail's `xorps xmm2, xmm2` after six f78b0 calls, the cross-jumped `fstp [esp]; call` tails (0x900000e into
// 0xa00000f's tail) and the 0xe000583 flag test materialized as a bool (`setne cl; test cl, cl`).
// Round 184 (m3 lane 4, fast): retail's nine out-of-line 0xf78e0 calls are exactly the nine our source writes as calls;
// its other 15 limit sites are 0xf78e0's case code expanded, re-deriving the definition from the tag table in EAX and
// staging the limit through [esp+0x2c] before the push. Re-measured: 0xf78e0 `inline` with every limit site calling
// it 797 (3029 bytes); with the four cases moved after 0x1300057e 736 (3011 bytes, aligned 105, masked 52; retail's
// layout keeps 0xb000587 in place, so that order is not retail's source order). New: that lead with 0xf78e0
// `__forceinline` 1183 (3338 bytes: every site expands); the lead with 0x6000595 after 0x800061e (retail's block
// order there) 797, the same swap alone 749 (same code); 0xf78e0's switch moved into an inline
// `vehicle_definition_limit(definition, limit)` that 0xf78e0 and the 15 expanded sites call 846 (3069 bytes, aligned
// 212) with 0xf78e0 itself left out of line (53 bytes); the 0xe000583 flag test as `== 0` and as a named bool 749 each
// (same code; retail materializes `setne cl; test cl, cl`). Left on the lead: the cross-jumped `fstp [esp]; call`
// tails, the zero re-materialized after six f78b0 calls, and the dot products' x87 operand orders.
// Round 112 (m3 lane 9, fast): retail re-zeroes xmm2 after exactly the five 0x24590 (magnitude3d) call paths and loads
// 1.0 at each use, so its caller treats 0x24590 as clobbering xmm; 0x24590 uses no xmm, and retail does not re-zero
// after f78b0 or f78e0. Ours knows 0x24590 and keeps zero in xmm4 and 1.0 in xmm5. The lever is magnitude3d's own
// declaration (a redeclaration `inline` in this file alone 749, no effect). Kept: every `goto done` (7) as `break`, the
// label dropped, 0x5000581 as if/else: 749, byte-identical (the constant results are threaded past the clamp). Tried:
// `0.0f >= vehicle->unknown25c` for `!(0.0f < ...)` 771 at 2958 bytes (retail's `comiss zero, [x]; jb`, NaN-correct;
// only that test changes, masked aligned 214 -> 199; a lead once the length agrees); f78e0 `inline` at the 15 expanded
// limit sites with an entry definition for 0x264/0x1f0/0x20c 797 at 3029 (re-measure of round 184's form; without the
// entry definition 937 at 3128); 0xe000583 as `!((flags & 4) || (flags & 8))` 749 (same code); existing cross3f(&up,
// &forward, &left) for the lateral cross 749 (first row then retail's, the others flip); `inline` on 0xf78b0 887 at
// 3097 (it stays out of line; zero moves to xmm2 and 1.0 per site as retail, but every xmm value then spills across
// f78b0 and the frame grows to 0x1c); 0xe000583/0xf000584 as positive guards around the result 749 (same code).
// MiniMax A (11-13): no new form tried this round, no lower score (749): the zero register at the top is still
// xmm4 here and xmm2 in retail, and the constant load and the pushes around it are in the other order, so the
// body's register plan differs from the first difference on.
// MiniMax C (11-14): no new form, no lower score (749); the frame agrees (0x18) and the first difference
// is still the zero/result SSE allocation.
// @retail 0xf6d10
bool __stdcall function_f6d10(long vehicle_index, long name, real *value, bool *active)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	s_tag_instance *tags = g_4e3b44;
	bool forced = false;
	real result = 0.0f;

	switch (name)
	{
	case 0x5000581:
		if (vehicle->control_flags & 0x1000)
		{
			result = 1.0f;
		}
		else
		{
			result = function_d1210(vehicle_index);
		}
		break;
	case 0x4000580:
		if (function_f5d70(vehicle_index) || (vehicle->control_flags & 2) ||
			TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit5))
		{
			result = 1.0f;
		}
		break;
	case 0x5000596:
		result = vehicle->unknown370;
		break;
	case 0x6000595:
		result = vehicle->unknown374;
		break;
	case 0x800061e:
		if (unit_action_active(vehicle_index, 0x32))
		{
			result = 1.0f;
		}
		break;
	case 0x900000e:
		result = function_f78b0((real)fabs(vehicle->steering_angle),
			(real)fabs(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x204) * 0.017453292f));
		break;
	case 0xa00000f:
		result = function_f78b0((real)fabs(vehicle->steering_angle),
			(real)fabs(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x208) * 0.017453292f));
		break;
	case 0xa00001a:
		result = function_f78b0((real)fabs(vehicle->turn), (real)fabs(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x22c)));
		break;
	case 0xa000598:
		result = function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(tags[vehicle->definition_index & 0xffff].bytes));
		break;
	case 0xb00001b:
		result = function_f78b0((real)fabs(vehicle->turn), (real)fabs(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x230)));
		break;
	case 0xb000587:
		result = function_f78b0((real)fabs(vehicle->linear_velocity.k * vehicle->up.k +
			vehicle->linear_velocity.j * vehicle->up.j + vehicle->up.i * vehicle->linear_velocity.i),
			function_f78e0(vehicle_index, 2));
		break;
	case 0xb000713:
		if (vehicle->gear == (char)0xfe || vehicle->gear == NONE)
		{
			break;
		}
		result = function_f78b0((real)fabs(vehicle->turn),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x22c), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x230)));
		result = (result + function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(tags[vehicle->definition_index & 0xffff].bytes))) * 0.5f;
		break;
	case 0xc000582:
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0xb000597:
	case 0xc0005a5:
		if (*(short *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f0) == 1 || *(short *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f0) == 0)
		{
			result = function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(tags[vehicle->definition_index & 0xffff].bytes));
		}
		else
		{
			vector3f const *velocity = function_f8070(7, (word *)&vehicle->flags348) ?
				&vehicle->unknown3f0 : &vehicle->linear_velocity;
			real speed = function_f8070(7, (word *)&vehicle->flags348) ? vehicle->unknown3dc : vehicle->speed;
			real moving = function_f78b0((real)fabs(vehicle->forward.k * velocity->k + vehicle->forward.j * velocity->j +
				vehicle->forward.i * velocity->i), function_f78e0(vehicle_index, 2));
			real driven = function_f78b0((real)fabs(speed), function_f78e0(vehicle_index, 0));
			real blend = PIN((vehicle->unknown34c * 0.2f + 1.0f) * 0.5f, 0.0f, 1.0f);

			result = (1.0f - blend) * moving + blend * driven;
		}
		if (name == 0xc0005a5 && !(0.0f < vehicle->unknown25c))
		{
			result = 0.0f;
			forced = false;
			break;
		}
		forced = true;
		break;
	case 0xd00057b:
		result = function_f78b0(0.0f > vehicle->speed ? 0.0f : vehicle->speed, function_f78e0(vehicle_index, 0));
		break;
	case 0xd00057f:
		result = function_f78b0((real)fabs(vehicle->steering_angle),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x204) * 0.017453292f, *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x208) * 0.017453292f));
		break;
	case 0xd000586:
	{
		vector3f left;

		left.i = vehicle->forward.k * vehicle->up.j - vehicle->forward.j * vehicle->up.k;
		left.j = vehicle->forward.i * vehicle->up.k - vehicle->up.i * vehicle->forward.k;
		left.k = vehicle->up.i * vehicle->forward.j - vehicle->forward.i * vehicle->up.j;
		result = function_f78b0((real)fabs(left.k * vehicle->linear_velocity.k + left.j * vehicle->linear_velocity.j +
			left.i * vehicle->linear_velocity.i), function_f78e0(vehicle_index, 2));
		break;
	}
	case 0xd00059a:
		result = vehicle->unknown3fc;
		break;
	case 0xe00057a:
		result = function_f78b0((real)fabs(vehicle->speed),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0xe00057c:
	{
		real reverse = vehicle->speed > 0.0f ? 0.0f : vehicle->speed;
		real maximum = (real)fabs(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8));

		if ((real)fabs(maximum) > 0.001f)
		{
			result = (real)fabs(reverse) / maximum;
		}
		break;
	}
	case 0xe00057d:
		result = function_f78b0((real)fabs(vehicle->turn),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x22c), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x230)));
		break;
	case 0xe000583:
		if (!(vehicle->object_flags & 0xc))
		{
			break;
		}
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0xf000584:
		if (!(vehicle->unknown34f > 0))
		{
			break;
		}
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0x10000585:
		result = function_f78b0((real)fabs(vehicle->forward.k * vehicle->linear_velocity.k +
			vehicle->forward.j * vehicle->linear_velocity.j + vehicle->linear_velocity.i * vehicle->forward.i),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0x10000594:
	{
		vector3f projection;
		vector3f rejection;

		function_11d580(&vehicle->linear_velocity, &vehicle->forward, &projection, &rejection);
		result = function_f78b0(magnitude3d(&rejection), 0.3f);
		result *= result;
		break;
	}
	case 0x1300057e:
	{
		real driven = function_f78b0((real)fabs(vehicle->speed),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		real turning = function_f78b0((real)fabs(vehicle->turn),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x22c), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x230)));

		result = driven > turning ? driven : turning;
		break;
	}
	case 0x13000588:
		result = function_f78b0(vehicle->unknown368, *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x20c));
		break;
	case 0x1300058a:
		result = function_f78b0((real)fabs(vehicle->speed - vehicle->steering_angle), function_f78e0(vehicle_index, 2));
		break;
	case 0x14000589:
		result = function_f78b0(vehicle->unknown36c, *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x20c));
		break;
	case 0x1400058b:
		result = function_f78b0((real)fabs(vehicle->steering_angle + vehicle->speed), function_f78e0(vehicle_index, 2));
		break;
	case 0x1700058e:
	case 0x1800058c:
	case 0x1800058f:
	case 0x1900058d:
		result = function_f78b0(vehicle->unknown360, *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x20c));
		break;
	case 0x14000599:
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity), function_f78e0(vehicle_index, 0));
		result = function_f78b0(result * vehicle->unknown374 - 0.15f, 0.15f);
		break;
	case 0x17000592:
	case 0x18000590:
	case 0x18000593:
	case 0x19000591:
		result = function_f78b0((real)fabs(vehicle->speed), function_f78e0(vehicle_index, 2));
		break;
	default:
		return false;
	}
	result = PIN(result, 0.0f, 1.0f);

	*value = result;
	*active = result > 0.0f || forced;
	return true;
}

/* whether a flag of a word of flags is set */
// @retail 0xf8070
bool __stdcall function_f8070(long bit, word const *flags)
{
	return TEST_FIELD_BIT(*flags & (1 << bit));
}

/* the units of up to 16 players on foot and their centers */
PRIVATE __forceinline long vehicle_players_on_foot(long *unit_indices, point3f *centers)
{
	s_record_pool_iterator iterator;
	byte *player;
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = data_iterator_next_inlined(&iterator)) != NULL && count < 16)
	{
		if (*(long *)(player + 0x2c) != NONE && VEHICLE_GET(*(long *)(player + 0x2c))->parent_object_index == NONE)
		{
			unit_indices[count] = *(long *)(player + 0x2c);
			centers[count] = VEHICLE_GET(*(long *)(player + 0x2c))->center;
			count++;
		}
	}
	return count;
}

/* an iteration over the vehicles: the current vehicle, then the object
   iterator */
struct s_vehicle_iterator
{
	s_vehicle *vehicle;
	s_type_f1af8e iterator;
};

/* starts an iteration over the vehicles */
PRIVATE inline void vehicle_iterator_new(s_type_f1af8e *iterator)
{
	iterator->signature = 0x86868686;
	iterator->type_mask = 2;
	iterator->flags = 0;
	iterator->index = 0;
	iterator->object_index = NONE;
}

/* whether a moving vehicle is within ten world units of a player on foot,
   and which */
// @retail 0xf7a60
bool __stdcall function_f7a60(long *vehicle_index)
{
	long unit_indices[16];
	point3f centers[16];
	long count = vehicle_players_on_foot(unit_indices, centers);

	if (count > 0)
	{
		s_vehicle_iterator iterator;

		vehicle_iterator_new(&iterator.iterator);
		while ((iterator.vehicle = (s_vehicle *)function_baeb0(&iterator.iterator)) != NULL)
		{
			short i;

			for (i = 0; i < count; i++)
			{
				if (VEHICLE_GET(unit_indices[i])->parent_object_index != iterator.iterator.object_index)
				{
					point3f const *center = &centers[i];
					real dx = iterator.vehicle->center.x - center->x;
					real dy = iterator.vehicle->center.y - center->y;
					real dz = iterator.vehicle->center.z - center->z;

					if (100.0f > dz * dz + dy * dy + dx * dx &&
						iterator.vehicle->linear_velocity.i * iterator.vehicle->linear_velocity.i +
						iterator.vehicle->linear_velocity.j * iterator.vehicle->linear_velocity.j +
						iterator.vehicle->linear_velocity.k * iterator.vehicle->linear_velocity.k >= 1.0f)
					{
						*vehicle_index = iterator.iterator.object_index;
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* an eight-byte vehicle seat-state entry; the occupancy word is at +4 */
struct s_vehicle_seat_state
{
    byte unknown00[4];
    word flags;
    word unknown06;
};
typedef char assert_vehicle_seat_state_size[(sizeof(s_vehicle_seat_state) == 8) ? 1 : -1];

/* whether an occupied vehicle (one with its flag 2 at +0x10a set) is within
   ten world units of a player on foot, and which */
// @retail 0xf7ca0
bool __stdcall function_f7ca0(long *vehicle_index)
{
	long unit_indices[16];
	point3f centers[16];
	s_record_pool_iterator iterator;
	byte *player;
	long count = 0;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (*(long *)(player + 0x2c) != NONE)
		{
			unit_indices[count] = *(long *)(player + 0x2c);
			centers[count] = VEHICLE_GET(*(long *)(player + 0x2c))->center;
			count++;
		}
	}

	if (count > 0)
	{
		s_vehicle_iterator iterator;

		vehicle_iterator_new(&iterator.iterator);
		while ((iterator.vehicle = (s_vehicle *)function_baeb0(&iterator.iterator)) != NULL)
		{
			short i;

			for (i = 0; i < count; i++)
			{
				if (VEHICLE_GET(unit_indices[i])->parent_object_index != iterator.iterator.object_index)
				{
					point3f const *center = &centers[i];
					real dx = iterator.vehicle->center.x - center->x;
					real dy = iterator.vehicle->center.y - center->y;
					real dz = iterator.vehicle->center.z - center->z;

					if (100.0f > dz * dz + dy * dy + dx * dx && TEST_FIELD_BIT(VEHICLE_WORD_BITS(iterator.vehicle->flags_10a)->bit2))
					{
						long seat_count = (unsigned long)iterator.vehicle->unknown120 >> 3;
						s_vehicle_seat_state const *seat = (s_vehicle_seat_state const *)((byte *)iterator.vehicle + iterator.vehicle->unknown122);
						long j;

						for (j = 0; j < seat_count; j++)
						{
							if ((seat[j].flags & 0xfff8u) > 0)
							{
								*vehicle_index = iterator.iterator.object_index;
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

/* scales an impulse on the vehicle by its definition: a wheeled vehicle with
   most seats empty takes less of the part that pushes it into the ground,
   and a boosting type 5 vehicle less by its throttle */
// @retail 0xf7ed0
void function_f7ed0(long vehicle_index, vector3f *impulse)
{
	s_vehicle *vehicle = (s_vehicle *)function_badc0(vehicle_index, 2);

	if (vehicle->unknown25c != 0.0f)
	{
		byte *definition = VEHICLE_DEFINITION_GET(vehicle);
		real scale = *(real *)(definition + 0x2e0);

		impulse->i *= scale;
		impulse->j *= scale;
		impulse->k *= scale;
		switch (*(short *)(definition + 0x1f0))
		{
		case 0:
		case 1:
		{
			long seat_count = *(long *)(definition + 0x2f0);

			if (seat_count > 0)
			{
				long empty = 0;
				long i;

				for (i = 0; i < seat_count; i++)
				{
					if (!(vehicle->unknown38e & (1 << i)))
					{
						empty++;
					}
				}
				if (empty > seat_count >> 1)
				{
					vector3f const *up = &vehicle->up;
					real dot = up->i * impulse->i + up->j * impulse->j + up->k * impulse->k;

					if (0.0f > dot)
					{
						vector3f into;

						into.i = up->i * dot;
						into.j = up->j * dot;
						into.k = up->k * dot;
						impulse->i = (impulse->i - into.i) * 0.45f + into.i;
						impulse->j = (impulse->j - into.j) * 0.45f + into.j;
						impulse->k = (impulse->k - into.k) * 0.45f + into.k;
					}
				}
			}
			break;
		}
		case 5:
			if (function_d1210(vehicle_index) > 0.0f)
			{
				real damping = 1.0f - (vehicle->throttle >= 0.0f ? vehicle->throttle : 0.0f - vehicle->throttle);

				impulse->i *= damping;
				impulse->j *= damping;
				impulse->k *= damping;
			}
			break;
		}
	}
}

void __stdcall function_a9120(long vehicle_index, long flip);
matrix3x3 *function_142eb0(matrix3x3 const *right, matrix3x3 const *left, matrix3x3 *out);
extern real g_5476c8;

/* transposes a rotation in place */
PRIVATE inline void vehicle_transpose(matrix3x3 *matrix)
{
	real temp;

	temp = matrix->forward.j;
	matrix->forward.j = matrix->left.i;
	matrix->left.i = temp;
	temp = matrix->forward.k;
	matrix->forward.k = matrix->up.i;
	matrix->up.i = temp;
	temp = matrix->left.k;
	matrix->left.k = matrix->up.j;
	matrix->up.j = temp;
}

/* the physics of type 5 vehicles (the flying ones): a flip the controls
   start, thrust toward the speed along the facing (faster diving, with
   boost), strafing, lift against gravity, and a torque that turns the
   vehicle toward its facing, pitched and banked into its turns */
// score 978 (was 990): earlier 1052, 1087, 1095; frame 0xfc versus 0xd8; index/offset and vector/rotation homes remain.
// V1A: statement-by-statement walk against all of retail's 1180 instructions found no logic difference (fields,
// conditions, constants, callees, argument orders, clamp and bank cases all agree). Two source shapes that retail
// shows and ours lacks, both measured by opcode alignment with offsets and registers ignored (0.757 -> 0.793):
// (1) the lift block (0xf5051-0xf5135) is repeated expansions: max(|speed|, max(|turn|, horizontal)) written as
// nested `a > b ? a : b` with |x| re-read from the vehicle at each use, then MIN(MAX(0.0f, fastest - g_4678c8),
// g_4678c4) (not PIN) - this reproduces retail's block instruction for instruction but costs 1185 / 5189 bytes
// (the baseline is 26 instructions short there and the rest evens out), 1189 / 5170 with (2) below as well; (2) the
// acceleration reversal (0xf4cb0-0xf4cf5) is a real 3-pass loop (acceleration's sign into a byte first, target's
// into a dword, counter 0..0xc step 4), ours unrolls it; a do/while recovers it: 1112 (5088 bytes), with a bool for
// the acceleration sign 1129, operands swapped 1134. NOT kept: score rises while the frame stays 0xf8. Tried:
// bool boosting (retail's flag is a byte at 0x63) 1141, frame 0xfc; sin/cos of the definition field read twice
// (retail reloads it) no change.
// Frame 0xf8 versus 0xd8; index/offset and vector/rotation homes remain.
// Pitch magnitude 1375; scalar control magnitudes with original steering fabs and
// desired-up add-scaled 1166; the flip-output matrix workspace 1181 (frame 0xdc).
// Repeated compound argument: controls/normalization/all magnitudes/target scales
// 1142/1142/1163/1162; force/brake/up/all vector forms 1142/1142/1108/1124 restored.
// Three-use steering ratio 1168; fraction with separate magnitude assignment 1163,
// a three-point incremental gain rejected. Keep original steering fabs and matrix homes.
// DL196: current-output/state-workspace/normalization subsets
// 1184/1165/1177/1198/1193/1177/1222. Original sum interactions
// 1135/1165/1135/1182/1167/1186/1166/1198 had a one-use input alias; restored.
// Desired output 1134, also 1134 with original names (kept); clean sum interactions
// 1135/1165/1135/1182/1166/1186/1166. Facing/flip scopes and normalizer/vector
// interactions 1166/1166/1166/1177/1177/1177/1135/1166, all restored.
// Desired with sum/normalizer/both/shared crosses 1129/1145/1135/1129; the five-point
// sum additions are rejected. Keep only the consumed desired-matrix output.
// J225: relative product in flip workspace 1179; current in facing workspace 1147; both early workspaces
// 1179; state rotation in facing workspace 1148; all rotation workspaces 1179; inertia in consumed matrix
// workspace 1197; all workspaces with flip output 1192; all workspaces with in-place output 1197; shared
// quaternion-axis setter 1134; shared torque setter 1135. Original body restored.
// P252: Full-width flip/boost/side-flip truth subsets 1/2/4/3/5/6/7: 1129/1126/1134/1175/1129/1125/1164; all
// native byte truths 1134. Only full-width boost truth kept at plain 1126, 5043/5094 bytes, frame 0xf8. The
// one-point side-flip addition and five-point flip-only form restored.
// P255: Angular-output/quaternion-conversion/late-force lifetime subsets 1-7 all 1126;
// consistent vector-role names 1126. Frame 0xf8 and index/vector/rotation homes remain; restored.
// F323: Frame 0xf8 versus 0xd8 and index/rotation homes remain. Sequential target reuse is plain-confirmed at 5056/5094 bytes.
// Target/drift, target/strafe, acceleration, target+drift+strafe, drift+acceleration,
// strafe+acceleration, all, all+rotation reuse 1130/1117/1127/1145/1130/1117/1145/1145.
// Naming only 1126; target/strafe reuse with original names 1117 (plain). Keep only that reuse.
// DH291: Library-copy current/desired/state rotations, seven subsets, and complete fallback
// transfers 1117 each. Complete predicted velocity in the existing target workspace with native
// component limiting 1107, plain-confirmed at 5077/5094 bytes; frame 0xf8, limiter unrolling and
// rotation/index homes remain.
// DH294: Quaternion axis before magnitude via memcpy/setter, original/shared scale and either
// outcome order: 1101/1095/1101/1095/1101/1095/1101/1095. Keep the existing setter and original
// scaling/test order, plain 1095 at 5072/5094 bytes; frame 0xf8, index/rotation homes remain.
// N322: Weighted bank-axis/driver-tilt/angular-target producer subsets 1/2/4/3/5/6/7:
// 1103/1087/1105/1114/1108/1087/1100; all with full torque scale 1100. Keep only
// the shared driver-tilt weighting, plain 1087 at retail length; frame 0xf8 versus 0xd8 remains.
// M316: On pre-magnitude axis graph: sequential facing/current, current/state and desired/tensor matrix ownership subsets 1/2/4/3/5/6/7, all with consumed flip output 1119/1095/1170/1119/1165/1170/1170/1169.
// Reuse reaches frame 0xd4 rather than retail 0xd8; entry index/offset and matrix/vector homes remain. Original executable body restored.
// AC491: Complete translation/orientation/spin phase scopes, masks 1/2/4/3/5/6/7, then all with
// coherent matrix identities: 1087/1087/1087/1087/1087/1087/1087/1087. Original complete body and
// producers restored; native frame and homes remain.
// AC494: Actual pool root/reused vehicle slot/flip-tick product, masks 1/2/4/3/5/6/7,
// then all with a named rounded threshold: 1128/1087/1087/1128/1128/1087/1128/1128. Frame 0xf8 persists;
// root ownership grows to 5104/5094 bytes. Original complete body and observations restored.
// NOT kept: root snapshot omits the later native reload; the threshold copy is one-use.
// P283: Existing complete quaternion converter, with desired normalization,
// torque rotation/scale and sequential current/state ownership subsets:
// 1203/1316/1203/1295/1203/1316/1203/1295. Converter stays called rather than
// expanding as retail does; all regress and shorten below retail. Original full body,
// matrices, axis/angle production and math providers restored; frame/index homes remain.
// AC499: Existing current-axis/desired-input/paired-target setters, seven subsets then
// complete force setter: 1211/1154/1109/1225/1157/1094/1220/1225. No native frame recovery; all
// original producers, matrix/vector ownership, observations and full body restored.
// B385: Existing current/state transpose and staged-cross boundaries, subsets 1/2/4/3/5/6/7,
// 1112/1112/1087/1112/1112/1112/1112; both transpose returns consumed 1112.
// Original complete body restored; frame, handle/offset and vector/matrix homes remain.
// M333: Actual up-length/inverse, bank-axis and quaternion-axis lexical identities,
// seven subsets, then independent component/point ordinals: 1087 each. Frame 0xf8
// remains; original full body restored, with native index and matrix/vector homes unresolved.
// F352: Existing axis inline/called normalizers and magnitude provider 1054/1143/1087;
// desired-up inline/called normalizers 1148/1251; paired inline/inline,
// inline/called, called/inline 1132/1236/1213. NOT kept: 1054 grows to
// 5120/5094 bytes and changes sum grouping; no provider form matches. Called desired-up
// alone or with inline axis gives frame 0xf4, others 0xf8, versus 0xd8. All restored.
// P291: Sequential facing/current, current/state and desired/tensor ownership on the
// weighted-driver graph, seven subsets then consumed flip output:
// 1090/1087/1140/1090/1140/1140/1140/1205. Sharing reaches frame 0xd4 rather
// than native 0xd8; original complete body restored at frame 0xf8 and retail length.
// Y566: Complete lift maximum with native repeated magnitudes: nested macro/ternary,
// inner maximum retained, wide flip, repeated horizontal root, facing/current reuse,
// wide flip with separate point ordinal, both lifetime interactions:
// 1185/1185/1118/1271/1217/1132/1271/1202. All regress and grow beyond 5094 bytes;
// original complete body restored. Native frame and handle/matrix homes remain.
// J245: Existing bank/scaled-angular/force producers after driver weighting,
// subsets 1/2/4/3/5/6/7, then all three cached up inputs with original magnitude tree: 1114/1069/1109/1100/1114/1109/1100/1087.
// NOT kept: 1069 grows to 5104/5094 bytes; other producers regress and cached up inputs tie 1087. Every frame stays 0xf8 versus 0xd8; all original producers restored.
// P300: First/final tensor and quaternion return graphs 1/2/4/3/5/6/7/15:
// 1090/1124/1035/1124/1079/1117/1117/1117. NOT kept: 1035/1079 shorten to
// 5075/5062 versus 5094 and grow frame to 0x104 versus 0xd8. All original bodies
// and workspaces restored; frame, handle/offset and matrix/vector homes remain.
// M350: Plain wind/forward-target/lateral-velocity component loops, subsets
// 1..7: 2767/2759/2759/2726/2726/2742/2682, frame 0xfc and 6443-6518 bytes.
// Native final point cursor 1087. Original body restored; frame/index homes remain.
// J253: Coordinated quaternion/multiply/velocity/step inline/forced contracts
// 1094/1599/1127/2743/1127/1378/1087/3412. No native frame/extent recovery;
// forced multiply and velocity forms lose matched controls; all contracts/body restored.
// F376: Existing drift/side dot and spin-squared-length boundaries, subsets 1/2/4/3/5/6/7,
// then complete projection dot: 1086/1087/1087/1086/1087/1087/1087/1087. NOT kept: one-point
// lead grows to 5104/5094; frame stays 0xf8/0xd8. Original complete body and helpers restored.
// F381: Reverse limiter while/do, own int/short/word/byte ordinals, real cursor
// and named signs: 1087/1112/1087/1113/1113/1113/1107/1105. NOT kept:
// ties/regressions; do recovers the 0xc bound but frame stays 0xf8 versus 0xd8.
// Original complete limiter/body restored; index and matrix/vector homes remain.
// F387: Complemented flip/boost/side-flip predicates, masks 1/2/4/3/5/6/7,
// 1159/1089/1087/1161/1159/1089/1161; native boost byte on weighted-driver graph 1141.
// NOT kept: ties/regressions; frame/index/rotation homes remain. Original full body restored.
// B445: Discarded query-output workspace reused by drift/side-input/both-axis producer
// subsets, then late angular acceleration: 1087/1087/1096/1107/1085/1064/1066/1066. NOT kept: unsupported combined workspace
// ownership; 1064 is 5104 bytes, 1066 is 5107, with frame still 0xf8. Whole body restored.
// B447: Complete orientation/tensor/torque/spin boundary, state/torque pointer
// and reference contracts: forced inline 1087/1087/1087/1087; ordinary inline
// 3746/3746/3746/3746. NOT kept: forced forms retain frame 0xf8; ordinary
// providers stay called, frame 0x94. Original complete body restored; native
// frame 0xd8, handle/offset and matrix/vector ownership remain.
// P334: Native backward/short-angle/target component-transform contracts,
// seven subsets then complete mass-force producer: 1040/1172/1161/1143/1153/1253/1230/1285. NOT kept: backward
// alone reaches 1040 but shortens to 5088/5094, frame 0xf0/0xd8; other forms regress. Original body restored.
// P336: Float CRT boundaries at pitch atan/root, horizontal root, driver/bank trig,
// rotation atan/root and desired-up root: 1064/1064/1087/1097/1097/1087/1087/1087.
// NOT kept: pitch gains grow to 5104/5094; frame stays 0xf8. Original full body restored.
// P341: Bounded scalar branch-results at lift numerator, paired unit clamps and paired spin-step limits,
// seven subsets then vertical attenuation: 1097/1087/1087/1097/1097/1087/1097/1097. NOT kept: native
// frame 0xd8 and handle/matrix homes are not recovered; original full expression graph, matrices and
// body restored.
// M393: Complete normalization length/inverse, paired trig and angular rate/inverse-step double receivers, subsets 1/2/4/3/5/6/7 then immutable all:
// 1118/1166/1166/1196/1152/1114/1143/1143. NOT kept: nonexact precision receivers; native frame, handle/offset and matrix/vector homes remain.
// Original complete body restored.
// B447: Complete orientation/tensor/torque/spin boundary, state/torque pointer
// and reference contracts: forced inline 1087/1087/1087/1087; ordinary inline
// 3746/3746/3746/3746. NOT kept: forced forms retain frame 0xf8; ordinary
// providers stay called, frame 0x94. Original complete body restored; native
// frame 0xd8, handle/offset and matrix/vector ownership remain.
// Permuter, 30 minutes (11-06): 1036 from 1087, only with function_f1380's two vector arguments swapped (a behaviour
// change), a cast on function_f44a0's state argument and a temporary for g_51e9c4->unknown0. Not kept.
// Round 68 (lane 13), fast checks, at 1087. Frame map (push-balanced slot tracking): retail packs its matrices
// into three homes, desired+inertia at 0x2c, facing+current+the state rotation copy at 0x60, flipped+the first
// 0x142eb0 product at 0xb4 (retail passes that call a third matrix rather than desired), and lays every home out
// by reference density (references per byte, densest nearest ESP; names and declaration order move nothing).
// Tried: current in its own block 1087, declared at the top 1087, facing/current renamed 1087 (identical code);
// memset of current (diagnostic) 1153; the first product into a separate `difference` matrix 1124 (frame 0xf8),
// into `flipped` (reuse, diagnostic) 1124 at frame 0xd4; `difference` renamed / declared first 1124/1124;
// `difference` in a block around the 0x141f60(0x142eb0) call 1124 at frame 0xd4, 5059 bytes (retail's three
// matrix homes; the shorter length is only disp8 encodings); on it, acceleration/velocity renamed 1124, the
// reversal sum and the strafe target as their own block locals 1124 (identical). LEAD (not kept, positional
// rise): that block plus `bool boosting` (retail's byte at [esp+0x63]) 1125: frame 0xd8 exactly, the three
// matrices at retail's offsets, first difference +0x2 -> +0x19; with the quaternion axis as an aggregate copy of
// its first three reals (retail's integer copy) 1128 (velocity then gets retail's home and reference count).
// Left on the lead: the vector order (retail velocity, acceleration, target+drift, facing, angular velocity +
// strafe acceleration + rotation, strafe velocity; ours groups drift with the strafe velocity) and the vehicle
// index in EDI (retail ESI).
// P386: Manual-first / forward-limit / reverse-limit throttle results, seven subsets then full conditional:
// 1087/1087/1087/1087/1087/1087/1087/1087. NOT kept: all tie 1087 with frame 0xf8; no native-frame gain.
// Native frame 0xd8 and handle/matrix homes remain; original complete body restored.
// P396: Complete lateral-flip outcome producers, false-default/complete/nested/failure-first,
// bool then long: 1064/1087/1101/1086/1098/1097/1101/1070. NOT kept: tested flags do not match; 1064/1070 grow
// to 5104/5094 bytes and the one-point lead is below gate. Original whole body restored.
// T76: Complete bank result: strafe/turn seeds, nested conditional, sum default,
// nonnegative-strafe/turn first, same-sign admission and inner conditionals:
// 1097/1088/1097/1088/1087/1087/1117/1097. NOT kept: ties/regressions;
// every frame stays 0xf8 versus 0xd8. Original bank flow and full body restored.
// T78: Final spin prior-state/difference/selected-change ownership, seven subsets,
// then branch-owned differences: 1104/1076/1087/1104/1104/1076/1104/1087. NOT kept: the 1076
// forms shorten to 5088/5094 while frame stays 0xf8/0xd8; other forms tie or regress.
// Original complete feedback and body restored; handle/matrix/vector homes remain.
// Round 180 (m3 lane 4, fast, plain-confirmed 1052): kept the boost set in both arms of an if/else (retail stores
// 1.0 in the else arm) 1069, and the reverse throttle as `-(speed / limit)` (retail `subss` from the zero
// register; `0.0f - speed / limit` became a constant divide and multiply) 1052 at 5088 bytes. Same boost as a
// ternary 1069 (identical); `-speed / limit` 1069. Not kept: the later vehicle_index uses through a pointer to
// the parameter 1088 (still EDI; retail re-reads [esp+0xec] after f44a0); `(void)&vehicle_index` 1087 (same code);
// the reversal loop as a ternary store (retail stores every pass) 1133, still unrolled; `speed` computed after the
// velocity correction (retail reads +0x354 after 0xba1d0) 1086, with the if/else boost 1068 (a moved declaration);
// `dive *` first or `(g_4678d8 * 1.6976527f)` grouped 1069/1073; bool boosting with the if/else 1114; the round-68
// frame lead (`difference` block + bool boosting) on the if/else 1093. On 1052: the strafe fraction as
// `-(strafe / maximum)` 1063 (its arm is then retail's, masked aligned 744 -> 742, but 5072 bytes); with the lift
// block as retail's macro expansions, `MAX(|speed|, MAX(|turn|, horizontal))` and `MIN(MAX(0.0f, fastest -
// g_4678c8), g_4678c4)`, 1142 (5172 bytes, masked 730); that plus the frame lead 1120 (5155 bytes, aligned 834 ->
// 767, masked 764 -> 720: the best structure, LEAD); without the lift 1138 (5043). Left on the lead: the unrolled
// reversal loop (+12 instructions; retail keeps a 3-pass byte-offset loop), the vehicle index in EDI for the
// whole body, and the rotation/torque tail's homes.
// Round 184 (m3 lane 4, fast): kept the quaternion's axis copied as one aggregate, `axis = rotation.vector;` (a vector
// member added to quaternionf; retail copies i/j/k with three integer moves) 1052 -> 990 at 5094/5094 bytes (aligned 810
// -> 728, register-masked 464 -> 415). On 990: target as `vector3f_scale(&vehicle->forward, vehicle->speed * boost *
// climb, &target)` (before the copy 1079; the speed product then follows 0xba1d0 as in retail, but ours multiplies
// `movaps; mulss [forward]`, retail loads forward first) 1074 (aligned 706, masked 407); every hand-written magnitude
// as vehicle_magnitude() (before the copy) 1085; the lift as `MAX(vehicle_magnitude(speed), MAX(vehicle_magnitude(turn),
// horizontal))` and `MIN(MAX(0.0f, fastest - g_4678c8), g_4678c4)` 1124 (5155 bytes); the strafe target and strafe
// velocity through vector3f_scale 1061 (5075 bytes, masked 394), the force through it 1061 (5075, masked 395), both
// 1061 (masked 393); both brakes through vector3f_add_scaled 990 (same code). Re-measured on 990: the `difference`
// block 1030 (5046 bytes), bool boosting 1089, both 1134 (frame 0xd8). Left: the vehicle index (retail ESI until
// 0xf44a0, then stack reloads; ours EDI to 0xf4250), the unrolled reversal loop (only a short/byte counter or a
// do/while keeps it a loop in a compile-only screen), the frame and the SSE homes.
// Round 154 (m3 lane 8, fast): kept the spin tail's else-arm selection as a clamp, `change = step; if (spin - u >
// change) change = spin - u;` (the `fastest` idiom above; retail computes spin - u before the negated step and keeps
// the change in the step's register, so both arms cross-jump into one `movaps`) 990 -> 978 (aligned 728 -> 723, masked
// 283 -> 280; left there: the zero register, xmm3 against xmm4); with `step = MAX-style; change = 0.0f - step;` first
// 978 (same code). Not kept: the reversal test's operands swapped on the for loop 990 (the compares swap, still
// unrolled); the negation inside the selection, `spin - u > 0.0f - step ? spin - u : 0.0f - step`, 994 (the change then
// takes the difference's register); on 978: the then arm as a clamp too 1014 (5078 bytes); the bank's `else if (0.0f >
// turn_bank)` as `else if (turn_bank >= 0.0f)` with the max arm first 978 (masked 276: retail's `comiss turn, 0; jb`
// shape, which sends a NaN turn to the sum); the sine after the bank cross product 978 (same code); the climb chain as
// `PIN(...) * g_4678d8 * dive` 978 (same code: VC multiplies by the global first either way, retail by dive); the
// strafe combine through vector3f_add_scaled (by `0.0f - along`) and vector3f_add 1039; the torque rows unscaled then
// vector3f_scale(&torque, torque_scale, &torque) (0xf2360's idiom: retail sums the three rows before loading the scale)
// 1035 (aligned 675), with the scale passed straight to it 1019 at 5096 bytes (aligned 676, masked 274, 1179/1179
// instructions: a lead once the earlier homes line up); the angular target through vector3f_scale and
// scaled_difference3f 1042. Re-measured on 978: `-(strafe / strafe_maximum)` 1053 (5075 bytes, masked 270).
// Lead: selections not yet screened for the clamp idiom of the spin tail: the side brake and the lift maxima.
// Round 425: lateral brake limit-seeded/complete/upper-first/lower-first
// 1037/978/1036/979; complete magnitude 978; sign admission/signed result 978/1039;
// existing clamp 978. NOT kept: no eligible native-frame/extent gain;
// original whole body restored. Current frame 0xfc versus native 0xd8.
// P441: Signed-slip deadband complete arms / biased accumulator / result accumulator / bound default, each original/complete outer result: 1036/988/978/1061/978/1061/1036/1044.
// NOT kept: No gain; native 0xd8 frame and handle/matrix/vector homes remain. Original complete body restored.
// P445: Existing wind difference, drift-length result, lift admission, driver-control receiver,
// complete parallel angular-error parts, driver-bit result, full flip-time admission and readonly definition receiver:
// 1035/1039/978/978/978/978/978/978. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P447: Successor saturation clamp/complete arms/predecessor/direct publication; exhaustive pitch sign; shortest-rotation/banking/reversal admissions:
// 978/978/1143/1140/978/978/978/1039. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P461: Shared desired-up degeneracy outcome bool/char/signed-char/byte/short/word/long/dword:
// 1059 each, 5075/5094 bytes, frame 0xfc. NOT kept: no native frame or exact
// decision/publication repair; original normalization, fallbacks and full body restored.
// @retail 0xf4800
void __stdcall function_f4800(long vehicle_index, s_vehicle_physics_state *state)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition;
	matrix3x3 facing;
	matrix3x3 flipped;
	matrix3x3 desired;
	vector3f velocity;
	vector3f angular_velocity;
	vector3f acceleration;
	vector3f target;
	vector3f force;
	bool flipping;
	long boosting;
	long i;

	if (vehicle->unknown351 > vehicle_round(g_510c54->field_2_3 * g_4678b8))
	{
		long flip = function_f4360(vehicle_index);

		if (flip)
		{
			s_vehicle *current = VEHICLE_GET(vehicle_index);

			current->unknown350 = (byte)flip;
			current->unknown351 = 0;
			function_a9120(vehicle_index, flip);
		}
	}
	flipping = function_f44a0(vehicle_index, &facing, state, &flipped);
	vehicle->unknown351 = (byte)MIN(vehicle->unknown351 + 1, 0xfe);
	definition = VEHICLE_DEFINITION_GET(vehicle);

	real pitch = (real)atan2(vehicle->forward.k, sqrt(vehicle->forward.i * vehicle->forward.i +
		vehicle->forward.j * vehicle->forward.j));
	real dive = 0.0f > pitch ? 1.0f : -1.0f;
	real climb = PIN((pitch >= 0.0f ? pitch : 0.0f - pitch), 0.0f, 0.58904862f) * dive * g_4678d8 * 1.6976527f + 1.0f;
	real boost;

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit4))
	{
		boost = function_d1210(vehicle_index) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
	}
	else
	{
		boost = 1.0f;
	}

	real maximum_speed = *(real *)(definition + 0x1f4) * boost * climb;
	real speed = vehicle->speed * boost * climb;
	real throttle;

	function_ba1d0(vehicle_index, &velocity, &angular_velocity);
	velocity.i -= g_51e9c4->vector.i;
	velocity.j -= g_51e9c4->vector.j;
	velocity.k -= g_51e9c4->vector.k;
	target.i = vehicle->forward.i * speed;
	target.j = vehicle->forward.j * speed;
	target.k = vehicle->forward.k * speed;
	boosting = TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit4);
	if (boosting && !flipping && !(TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit3)))
	{
		throttle = 1.0f;
	}
	else if (vehicle->speed > 0.0f)
	{
		throttle = 0.001f > (maximum_speed >= 0.0f ? maximum_speed : 0.0f - maximum_speed) ? 0.0f : vehicle->speed / maximum_speed;
	}
	else
	{
		throttle = 0.001f > (*(real *)(definition + 0x1f8) >= 0.0f ? *(real *)(definition + 0x1f8) : 0.0f - *(real *)(definition + 0x1f8)) ? 0.0f :
			-(vehicle->speed / *(real *)(definition + 0x1f8));
	}
	function_f1380(&acceleration, &target, &velocity, *(real *)(definition + 0x1fc) * throttle,
		*(real *)(definition + 0x200) * throttle, g_510c54->rate);
	if (vehicle->speed == 0.0f)
	{
		vector3f drift;
		real dot = facing.left.i * velocity.i + facing.left.j * velocity.j + facing.left.k * velocity.k;

		drift.i = velocity.i - facing.left.i * dot;
		drift.j = velocity.j - facing.left.j * dot;
		drift.k = 0.0f;
		if (function_30bf0(&drift) != 0.0f)
		{
			real brake = 0.0f - (*(real *)(definition + 0x200) >= 0.0f ? *(real *)(definition + 0x200) : 0.0f - *(real *)(definition + 0x200)) * g_4678d0;

			acceleration.i += drift.i * brake;
			acceleration.j += drift.j * brake;
			acceleration.k += drift.k * brake;
		}
	}
	if (!(0.0f < vehicle->speed))
	{
		vector3f_add(&acceleration, &velocity, &target);
		for (i = 0; i < 3; i++)
		{
			if ((target.n[i] >= 0.0f) != (0.0f >= acceleration.n[i]))
			{
				acceleration.n[i] = 0.0f - velocity.n[i];
			}
		}
		acceleration.k = 0.0f;
	}

	bool side_flip = function_f42f0(vehicle_index) && (vehicle->unknown350 == 1 || vehicle->unknown350 == 2);

	if (!boosting || side_flip)
	{
		real flip_scale = side_flip ? (1.0f - function_f4250(vehicle_index)) * g_4678cc * boost : 1.0f;
		real strafe_maximum = *(real *)(definition + 0x1f4) * flip_scale;
		real strafe = vehicle->turn * flip_scale;
		real strafe_fraction;
		vector3f strafe_velocity;
		vector3f strafe_acceleration;

		if (strafe > 0.0f)
		{
			strafe_fraction = 0.001f > (strafe_maximum >= 0.0f ? strafe_maximum : 0.0f - strafe_maximum) ? 0.0f : strafe / strafe_maximum;
		}
		else
		{
			strafe_fraction = 0.001f > (strafe_maximum >= 0.0f ? strafe_maximum : 0.0f - strafe_maximum) ? 0.0f : 0.0f - strafe / strafe_maximum;
		}
		target.i = strafe * facing.left.i;
		target.j = facing.left.j * strafe;
		target.k = facing.left.k * strafe;

		real side_speed = facing.left.i * velocity.i + facing.left.j * velocity.j + facing.left.k * velocity.k;

		strafe_velocity.i = facing.left.i * side_speed;
		strafe_velocity.j = facing.left.j * side_speed;
		strafe_velocity.k = facing.left.k * side_speed;
		function_f1380(&strafe_acceleration, &target, &strafe_velocity,
			*(real *)(definition + 0x234) * strafe_fraction,
			strafe_fraction * flip_scale * *(real *)(definition + 0x238), g_510c54->rate);

		real along = (facing.left.k * acceleration.k + acceleration.i * facing.left.i +
			acceleration.j * facing.left.j) * strafe_fraction;

		acceleration.i = strafe_acceleration.i + (acceleration.i - along * facing.left.i);
		acceleration.j = strafe_acceleration.j + (acceleration.j - facing.left.j * along);
		acceleration.k = strafe_acceleration.k + (acceleration.k - facing.left.k * along);
		if (vehicle->turn == 0.0f)
		{
			real magnitude = (side_speed >= 0.0f ? side_speed : 0.0f - side_speed);
			real limit = g_4678d0 * *(real *)(definition + 0x238);
			real brake = 0.0f - magnitude > limit ? 0.0f - magnitude : (limit > magnitude ? magnitude : limit);

			brake = 0.0f > side_speed ? brake : 0.0f - brake;
			acceleration.i += facing.left.i * brake;
			acceleration.j += facing.left.j * brake;
			acceleration.k += facing.left.k * brake;
		}
	}

	real horizontal_speed = (real)sqrt(velocity.i * velocity.i + velocity.j * velocity.j);
	real fastest = (vehicle->speed >= 0.0f ? vehicle->speed : 0.0f - vehicle->speed);
	real other = (vehicle->turn >= 0.0f ? vehicle->turn : 0.0f - vehicle->turn) > horizontal_speed ? (vehicle->turn >= 0.0f ? vehicle->turn : 0.0f - vehicle->turn) : horizontal_speed;

	if (!(fastest > other))
	{
		fastest = other;
	}

	real lift = PIN(fastest - g_4678c8, 0.0f, g_4678c4) / (g_4678c4 - g_4678c8);

	lift = PIN(lift, 0.0f, 1.0f);
	if (vehicle->speed == 0.0f && !flipping)
	{
		lift = (1.0f - PIN(velocity.k, 0.0f, 1.0f) * 0.2f) * lift;
	}
	lift = PIN(lift, 0.0f, 1.0f);
	acceleration.k += *(real *)(definition + 0x2e0) * lift * g_51e9c4->unknown0;

	real mass_scale = state->unknown6c * vehicle->unknown25c;
	real maximum_turn_rate = *(real *)(definition + 0x210) * 0.017453292f;
	matrix3x3 current;

	force.i = mass_scale * acceleration.i;
	force.j = acceleration.j * mass_scale;
	force.k = mass_scale * acceleration.k;
	current.forward = vehicle->forward;
	current.up = vehicle->up;
	vehicle_cross(&vehicle->up, &vehicle->forward, &current.left);
	if (flipping)
	{
		desired = flipped;
	}
	else
	{
		real length;

		desired.forward = vehicle->local_velocity;
		vector3f_add_scaled(g_4687b0, &desired.forward, 0.0f - desired.forward.k, &desired.up);
		length = (real)sqrt(desired.up.i * desired.up.i + desired.up.k * desired.up.k + desired.up.j * desired.up.j);
		if (!(0.0001f > (real)fabs(length)))
		{
			real inverse = 1.0f / length;

			desired.up.i *= inverse;
			desired.up.j *= inverse;
			desired.up.k *= inverse;
		}
		if (0.0001f > (real)fabs(length) || length == 0.0f)
		{
			desired.up = *g_4687a8;
		}
	}
	if (TEST_FIELD_BIT(VEHICLE_DWORD_BITS(vehicle->flags_134)->bit1))
	{
		s_vehicle *driver = VEHICLE_GET(vehicle_index);

		if (driver->unknown248 != NONE)
		{
			driver = VEHICLE_GET(driver->unknown248);
		}
		if (*(long *)((byte *)driver + 0x12c) == NONE)
		{
			real angle = *(real *)(definition + 0x24c);
			real sine = (real)sin(angle);
			real cosine = (real)cos(angle);
			vector3f backward;

			backward.i = 0.0f - desired.forward.i;
			backward.j = 0.0f - desired.forward.j;
			backward.k = 0.0f - desired.forward.k;
			desired.forward.i = sine * desired.up.i + cosine * desired.forward.i;
			desired.forward.j = desired.forward.j * cosine + desired.up.j * sine;
			desired.forward.k = cosine * desired.forward.k + desired.up.k * sine;
			vector3f_scale(&backward, sine, &backward);
			vector3f_add_scaled(&backward, &desired.up, cosine, &desired.up);
		}
	}
	if (!flipping || vehicle->unknown350 != 3)
	{
		real maximum_steering = *(real *)(definition + 0x204) * 0.017453292f;
		real speed_fraction = 0.001f > *(real *)(definition + 0x1f4) ? 0.0f :
			(vehicle->speed >= 0.0f ? vehicle->speed : 0.0f - vehicle->speed) / *(real *)(definition + 0x1f4);
		real steering_fraction = (real)fabs(0.001f > maximum_steering ? 0.0f :
			vehicle->steering_angle / maximum_steering);
		real turn_fraction = 0.001f > *(real *)(definition + 0x22c) ? 0.0f :
			vehicle->turn / *(real *)(definition + 0x22c);
		real steering_bias = (steering_fraction >= 0.0f ? steering_fraction : 0.0f - steering_fraction) * 2.0f;
		real slip = velocity.j * desired.forward.i - desired.forward.j * velocity.i;
		real slip_bank = 0.0f;

		if (!(0.001f > (slip >= 0.0f ? slip : 0.0f - slip)))
		{
			if (0.0f > slip)
			{
				real biased = slip + steering_bias;

				slip_bank = (biased > 0.0f ? 0.0f : biased) - steering_bias;
			}
			else
			{
				real biased = slip - steering_bias;

				slip_bank = (0.0f > biased ? 0.0f : biased) + steering_bias;
			}
		}

		real turn_bank = (speed_fraction > steering_fraction ? speed_fraction : steering_fraction) / maximum_speed *
			slip_bank * maximum_steering;
		real strafe_bank = turn_fraction * maximum_steering * -0.7f;
		real bank;

		if (0.0f > strafe_bank)
		{
			if (0.0f > turn_bank)
			{
				bank = turn_bank > strafe_bank ? strafe_bank : turn_bank;
			}
			else
			{
				bank = strafe_bank + turn_bank;
			}
		}
		else if (0.0f > turn_bank)
		{
			bank = strafe_bank + turn_bank;
		}
		else
		{
			bank = turn_bank > strafe_bank ? turn_bank : strafe_bank;
		}

		real cosine = (real)cos(bank);
		real sine = (real)sin(bank);
		vector3f axis;

		vehicle_cross(&desired.forward, &desired.up, &axis);
		desired.up.i = axis.i * sine + desired.up.i * cosine;
		desired.up.j = axis.j * sine + desired.up.j * cosine;
		desired.up.k = axis.k * sine + desired.up.k * cosine;
	}
	vehicle_cross(&desired.up, &desired.forward, &desired.left);
	vehicle_transpose(&current);

	quaternionf rotation;
	vector3f axis;
	real length;
	real angle;

	function_141f60(function_142eb0(&desired, &current, &desired), &rotation);
	axis = rotation.vector;
	length = (real)sqrt(axis.k * axis.k + axis.j * axis.j + axis.i * axis.i);
	if (0.0001f > (real)fabs(length))
	{
		length = 0.0f;
	}
	else
	{
		real inverse = 1.0f / length;

		axis.i = inverse * axis.i;
		axis.j = axis.j * inverse;
		axis.k = axis.k * inverse;
	}
	angle = (real)atan2(length, rotation.w) * 2.0f;
	if (angle > g_5476c4)
	{
		axis.i = 0.0f - axis.i;
		axis.j = 0.0f - axis.j;
		axis.k = 0.0f - axis.k;
		angle = g_5476c8 - angle;
	}

	real rate = angle * maximum_turn_rate * 0.31830987f;
	real inverse_dt = 1.0f / g_510c54->rate;
	vector3f local_461a4b;
	matrix3x3 rotation_matrix = state->matrix.rotation;
	matrix3x3 inertia;
	vector3f torque;

	local_461a4b.i = inverse_dt * (axis.i * rate - vehicle->angular_velocity.i);
	local_461a4b.j = (axis.j * rate - vehicle->angular_velocity.j) * inverse_dt;
	local_461a4b.k = (axis.k * rate - vehicle->angular_velocity.k) * inverse_dt;
	function_142eb0(&rotation_matrix, (matrix3x3 const *)&state->unknown3c, &inertia);
	vehicle_transpose(&rotation_matrix);
	function_142eb0(&inertia, &rotation_matrix, &inertia);

	real torque_scale = *(real *)(definition + 0x270) * vehicle->unknown25c;

	torque.i = (inertia.up.i * local_461a4b.k + inertia.left.i * local_461a4b.j +
		inertia.forward.i * local_461a4b.i) * torque_scale;
	torque.j = (inertia.up.j * local_461a4b.k + inertia.left.j * local_461a4b.j +
		inertia.forward.j * local_461a4b.i) * torque_scale;
	torque.k = (inertia.up.k * local_461a4b.k + inertia.left.k * local_461a4b.j +
		inertia.forward.k * local_461a4b.i) * torque_scale;

	real spin = (real)sqrt(vehicle->angular_velocity.i * vehicle->angular_velocity.i +
		vehicle->angular_velocity.j * vehicle->angular_velocity.j +
		vehicle->angular_velocity.k * vehicle->angular_velocity.k) / maximum_turn_rate;
	real change;

	if (spin > vehicle->unknown374)
	{
		real step = (1.0f - vehicle->unknown374) * (1.0f - vehicle->unknown374) * 0.2f;

		step = 0.01f > step ? 0.01f : (step > 0.05f ? 0.05f : step);
		change = spin - vehicle->unknown374 > step ? step : spin - vehicle->unknown374;
	}
	else
	{
		real step = vehicle->unknown374 * vehicle->unknown374 * 0.05f;

		step = 0.0f - (step > 0.005f ? step : 0.005f);
		change = step;
		if (spin - vehicle->unknown374 > change)
			change = spin - vehicle->unknown374;
	}
	vehicle->unknown374 += change;
	for (i = 0; i < state->definition->point_count; i++)
	{
		state->points[i].unknown9c = vehicle->unknown25c;
	}
	function_2056e0(vehicle_index, state, 0.0f, &force, &torque);
}

/* counts the vehicle's ticks off the ground (+0x34c), its ticks in contact
   with something (+0x34f), and, for type 5, its ticks in contact while
   upside down (+0x34b) */
// @retail 0xf6bd0
void __stdcall function_f6bd0(long vehicle_index, s_vehicle_physics_state *state)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	real upside_down = *(short *)(definition + 0x1f0) == 5 ? -0.075f : 0.0f;
	bool touching = false;
	long i;

	if (vehicle->unknown34c < 0xff)
	{
		vehicle->unknown34c++;
	}
	for (i = 0; i < *(long *)(definition + 0x2e8); i++)
	{
		if (state->points[i].unknown80)
		{
			vehicle->unknown34c = 0;
			break;
		}
	}
	for (i = 0; i < *(long *)(definition + 0x2f0); i++)
	{
		if (state->contacts[i].unknown80)
		{
			touching = true;
			break;
		}
	}
	if ((vehicle->havok_component_index != NONE &&
		havok_component_get(vehicle->havok_component_index)->unknown88.size) || touching)
	{
		if (vehicle->unknown34f < 0xff)
		{
			vehicle->unknown34f++;
		}
		vehicle->unknown34c = 0;
		if (upside_down > vehicle->up.k)
		{
			if (vehicle->unknown34b < 0xff)
			{
				vehicle->unknown34b++;
			}
			return;
		}
	}
	else
	{
		vehicle->unknown34f = 0;
	}
	vehicle->unknown34b = 0;
}

/* the vehicle object type's definition as its callbacks see it: the table
   that takes their addresses (the type list after it ends with this type) */
struct s_vehicle_type_definition_view
{
	char const *name;
	long group_tag;
	short datum_size;
	short unknown0a;
	long unknown0c;
	void *functions[29];
	void *types[3];
	byte unknown90[0xc8 - 0x90];
};

extern s_vehicle_type_definition_view g_467b40;

s_vehicle_type_definition_view g_467b40 =
{
	"vehicle",
	'vehi',
	0x428,
	0x70,
	0x540078,
	{
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, (void *)function_eeb80,
		(void *)vehicle_place, NULL, NULL, NULL,
		(void *)function_efde0, NULL, NULL, (void *)function_f6d10,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, (void *)function_f0f50,
		NULL, NULL, (void *)function_ee9b0, NULL,
		NULL
	},
	{ NULL, NULL, &g_467b40 }
};
