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
// score 126 (was 126): frame, length and layout now retail's; left: SSE register colouring (the 1.0 constant in
// xmm2 against retail's xmm3, the normalized forward/heading in other registers), the velocity copy's eax/ecx, two
// x87 operand orders in the atan2 sums and the reversing compare's operand order (`fxch`, jb).
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
// score 8 (was 8): retail's length and tail now; four products load the other operand (ours loads up.j
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

			cross3f(&vehicle->forward, &vehicle->up, &side);
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
// score 597 (was 597): Frame 0x19ac versus retail 0x1984, native flag-result byte stores, sum-loop unrolling
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
// @retail 0xefde0
bool __stdcall function_efde0(long vehicle_index)
{
	s_vehicle_physics_state state;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool stopped = function_f1320(vehicle_index);
	volatile bool changed = false;
	word flags_c0 = vehicle->flags_c0;
	vector3f linear_velocity;

	if (((flags_c0 >> 12) & 1) && vehicle->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(vehicle->havok_component_index);
		real powered = (vehicle->control_flags & 0x2000) ? 1.0f : 0.0f;
		real open = ((flags_c0 >> 10) & 1) ? 1.0f : 0.0f;
		real position = powered > open ? powered : open;

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
					byte buffer[0x28];

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

					for (i = 0; i < count; i++)
						total += state.points[i].value;
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

					function_d6660(&damage, *(long *)(globals + 0x28));
					damage.material_index = NONE;
					function_d7b80(&damage, child_index, NONE, NONE, NONE, NULL);
				}
				child_index = *(long *)((byte *)child + 0xc);
			}
		}
	}

	if (vehicle->control_flags & 1)
		vehicle->flags_134 |= 0x800000;
	else
		vehicle->flags_134 &= ~0x800000;
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
// score 40 (was 40): Frame 0x9c and extent agree; two header SIB operands, join reloads, cross/dot operand
// ranks and constructor/graph-load scheduling remain.
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
// @retail 0xf0760
void __stdcall function_f0760(long vehicle_index, vehicle_animation_value_callback callback, void *user)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	byte *model = g_4e3b44[*(long *)(definition + 0x38) & 0xffff].bytes;
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
			real value = *(real *)(definition + 0x20c) > 0.0f ? vehicle->unknown360 / *(real *)(definition + 0x20c) : 0.0f;
			callback(vehicle_index, graph, &channel, value, 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 5).index != NONE && state->channel_play(&channel, VEHICLE_OVERLAY(vehicle, 5), 0))
		{
			real value = *(real *)(definition + 0x20c) > 0.0f ? vehicle->unknown364 / *(real *)(definition + 0x20c) : 0.0f;
			callback(vehicle_index, graph, &channel, value, 0.0f, 1.0f, user);
		}

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

/* a wheel's spin advanced by its angular velocity, wrapped to the
   definition's period */
PRIVATE inline real vehicle_wheel_spin(real spin, real const *period)
{
	if ((*period) > 0.001f)
	{
		if (!(spin > 0.0f))
		{
			spin += (*period);
		}
		return (real)fmod(spin, (*period));
	}
	return 0.0f;
}

/* the physics of type 0 vehicles (the differential-steered ones): the
   engine drives each side's contacts, the turn trading torque between
   the sides */
// score 101 (was 101): 115 before round 114; the frame now agrees (0x10; round 114 removed 0x2056e0's named double, whose
// alignment need LTCG had pushed onto this caller); before that, frame 0x14 against 0x10 (one dead
// slot below gear_throttle; the four used slots agree but sit 4 higher); zero/scalar residency and
// spin schedule remain. Kept earlier: live period receiver, native twice-used gear and result-first
// torque.
// Live period 136; native gear 190; both 113; scalar roles 192/136/190/113; result-first torque 101 kept plain.
// Spin exit / period-first / both 101; named absolute steering with each 127; early torque zero 140.
// Y405: Frame 0x14 versus 0x10 and zero/scalar homes remain;
// gear-none/two-defaults/active-gear/sign-arm torque results 126/140/138/126; each with shared
// straight-drive product 132/141/141/132.
// Round 21: `if (gear == NONE) torque = 0; else torque = ...` gives retail's arm order and threaded
// zero magnitude but torque in xmm3, 697 bytes, 138 (ternary 138, `!=` with else 145, magnitude set in
// each arm 126; all 24 declaration orders of torque/magnitude/left/right 138); the turn product inline
// in both sides 113; removing the gear call or merging the two throttle minimums shrinks the frame by
// two slots (diagnostic).
// r97: the turn product written at both uses (retail's xmm6 zero, xmm5 product) 113;
// torque if/else with NONE first 138, with that 138; spin helper as one fmod of
// `spin > 0 ? spin : spin + period` 112, with the product at both uses 149; spin
// result local 134. The extra frame word is not 0x2056e0's (it aligns itself).
// Lane 13 round 25: the gear read at each use (no short local) 127; the torque as a ternary 145; as
// `if (gear == NONE) torque = 0; else ...` 138; the f1740 result in a named local 115; the two throttle minimums
// declared engine first 113; both minimums written inside the call 115 (same code). Diagnostics: the wheel spin
// helper replaced by plain fmod keeps the 0x14 frame; one shared minimum (or no f1740 call) drops it to 0xc, so the
// dead word comes with the second minimum; the opening magnitude already matches retail.
// Lane 13 round 28: the gear throttle's minimum written in the f1740 call 113 (frame still 0x14); the engine
// throttle's minimum in the f1950 call 115; engine_throttle renamed (diagnostic) 115, slots unchanged;
// `if (gear == NONE) torque = 0; else ...` 138 (retail's arm order and per-path zero, but torque in xmm3 against
// retail's xmm0, 697 bytes); call arm first 145.
// Round 117 (fast; aligned 52 for the unchanged body): `if (gear == NONE) torque = 0; else ...` now 127 (zero
// XMM6, product XMM5 and the negation's fresh zero as retail; torque XMM3), with `-torque`, the field-first
// product, torque-first sides or `turn * torque_magnitude` 127 each (same code); the turn product written at both
// uses 98 (aligned 32: every float register as retail; left the NONE arm laid out after `jne` with the zero set in
// it, the right value copied to XMM0 before its store, and the spin temp stored before the period test), with the
// NONE-first if/else 127, the call-first ternary 134, the NONE-first ternary 127, call arm first with an else
// zero 134, magnitude declared first 127, a single-exit spin helper 121, an if/else result spin helper 98, the
// zero-period exit first 103. Compile-only: every spin shape (local copy, ternary argument, fmodf, float fmod
// overload, pointer argument, no helper, double local) stores the temp at each definition.
// Round 45 (lane 1): `-torque` 101 (identical); `torque < 0.0f ? 0.0f - torque : torque` 100; torque declared at
// the gear read 101; NONE-first if/else with `-torque` 127 at 697 bytes (torque XMM3, magnitude XMM0; retail XMM0,
// XMM1), with the flipped test 129.
// Round 44 (lane 11): the contact torque store as if/else 171 (692 bytes); right computed before left 101; right
// declared before left 101; `!(i & 1) ? left : right` 101 (identical code); diagnostic: the turn product at both
// uses 98 again (left: the NONE gear arm, right copied to XMM0 before its store, the spin temp at each definition).
// J225: shared period receiver 101; torque call then multiply 101; period and torque assignment 101; shared
// drive fraction 132; period and drive fraction 132; shared control length 101; period, drive fraction and
// length 132; native byte gear and torque assignment 114. Original body restored.
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
	torque = 0.0f;
	if (gear != NONE)
	{
		torque = function_f14f0(gear, vehicle_index) * VEHICLE_GET(vehicle_index)->unknown378;
	}
	torque_magnitude = torque >= 0.0f ? torque : 0.0f - torque;

	real turn_torque = torque_magnitude * turn;

	left = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque - turn_torque;
	right = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque + turn_torque;
	for (i = 0; i < state->definition->contact_count; i++)
	{
		state->contacts[i].torque = (i & 1) ? right : left;
	}
	vehicle->unknown368 += left * dt;
	vehicle->unknown368 = vehicle_wheel_spin(vehicle->unknown368, (real const *)(definition + 0x20c));
	vehicle->unknown36c += right * dt;
	vehicle->unknown36c = vehicle_wheel_spin(vehicle->unknown36c, (real const *)(definition + 0x20c));
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

/* a damping force against a speed, at most what stops it within a tick */
PRIVATE __forceinline real vehicle_friction(real speed, real friction, real scale)
{
	static real s_ticks = g_510c54->field_2_3 * 0.1f;
	real direction = speed > 0.0f ? 1.0f : -1.0f;
	real magnitude = speed >= 0.0f ? speed : 0.0f - speed;
	real force = scale * (friction >= 0.0f ? friction : 0.0f - friction) * direction;
	real limit = magnitude * s_ticks;

	return PIN(0.0f - force, 0.0f - limit, limit);
}

/* the physics of type 3 vehicles (the hovering ones): forces toward the
   desired speeds along the facing, its left and its up, friction where a
   control is idle, and a torque that turns the vehicle to face its
   facing, banked into its turns */
// score 706 (was 719): Frame 0xd0 versus retail 0xfc; scalar/vector/transform homes and remaining force/trig scheduling differ.
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
	real inverse_maximum = 1.0f / maximum_speed;
	real forward_fraction = PIN(speed, 0.0f - maximum_speed, maximum_speed) * inverse_maximum;
	real left_fraction = PIN(vehicle->turn, 0.0f - maximum_speed, maximum_speed) * inverse_maximum;
	real up_fraction = PIN(vehicle->unknown370, 0.0f - maximum_speed, maximum_speed) * inverse_maximum;
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

	real forward_speed = linear_velocity.j * facing->j + linear_velocity.i * facing->i + facing->k * linear_velocity.k;
	real up_speed = linear_velocity.k * up.k + linear_velocity.j * up.j + linear_velocity.i * up.i;
	real left_speed = linear_velocity.k * left.k + linear_velocity.j * left.j + linear_velocity.i * left.i;
	real left_force = (vehicle->turn - left_speed) * left_mass * acceleration_scale * state->unknown6c;
	real up_force = (vehicle->unknown370 - up_speed) * up_mass * acceleration_scale * state->unknown6c;
	real forward_force = (speed - forward_speed) * forward_mass * acceleration_scale * state->unknown6c;
	vector3f force;

	force.i = facing->i * forward_force + up.i * up_force + left.i * left_force;
	force.j = facing->j * forward_force + up.j * up_force + left.j * left_force;
	force.k = facing->k * forward_force + up.k * up_force + left.k * left_force;
	if (vehicle->speed == 0.0f)
	{
		real friction = vehicle_friction(forward_speed, *(real *)(definition + 0x200),
			*(real *)(definition + 0x27c) * acceleration_scale) * state->unknown6c;

		vector3f_add_scaled(&force, facing, friction, &force);
	}
	if (vehicle->turn == 0.0f)
	{
		real friction = vehicle_friction(left_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c)) *
			state->unknown6c;

		vector3f_add_scaled(&force, &left, friction, &force);
	}
	if (vehicle->unknown370 == 0.0f)
	{
		real friction = vehicle_friction(up_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c)) *
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
			maximum_turn_rate = 2.5f * maximum_turn_rate;
		}
		maximum_bank = 0.78539819f;
	}

	real turn = 0.001f > *(real *)(definition + 0x22c) ? 0.0f :
		vehicle->turn / (real)fabs(*(real *)(definition + 0x22c));
	real speed_fraction = 0.001f > maximum_speed ? 0.0f : inverse_maximum * speed;
	real speed_magnitude = speed_fraction >= 0.0f ? speed_fraction : 0.0f - speed_fraction;
	real turn_magnitude = turn >= 0.0f ? turn : 0.0f - turn;
	real slip_scale = 0.0f > speed_magnitude - turn_magnitude ? 0.0f : speed_magnitude - turn_magnitude;
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
	desired.up.i = cosine * up.i + axis.i * sine;
	desired.up.j = cosine * up.j + axis.j * sine;
	desired.up.k = cosine * up.k + axis.k * sine;

	transform4x3f transform;
	quaternionf rotation;
	vector3f rotation_axis;
	real rotation_angle;

	transform.scale = 1.0f;
	transform.forward = vehicle->forward;
	cross3f(&vehicle->up, &vehicle->forward, &transform.left);
	transform.up = vehicle->up;
	transform.position.x = 0.0f;
	transform.position.y = 0.0f;
	transform.position.z = 0.0f;
	desired.scale = 1.0f;
	desired.forward = *facing;
	cross3f(&desired.up, facing, &desired.left);
	desired.position.x = 0.0f;
	desired.position.y = 0.0f;
	desired.position.z = 0.0f;
	function_141590(&transform, &transform);
	// Reuse the inverse transform workspace for the relative transform.
	function_142a60(&desired, &transform, &transform);
	function_141f60(&transform.rotation, &rotation);
	function_11d790(&rotation, &rotation_angle, &rotation_axis);

	real rate = PIN(rotation_angle * maximum_turn_rate * 0.95492965f, 0.0f - maximum_turn_rate, maximum_turn_rate);
	vector3f torque;
	real torque_scale = angular_scale * state->unknown6c;

	torque.i = (rotation_axis.i * rate - vehicle->angular_velocity.i) * torque_scale;
	torque.j = (rotation_axis.j * rate - vehicle->angular_velocity.j) * torque_scale;
	torque.k = (rotation_axis.k * rate - vehicle->angular_velocity.k) * torque_scale;
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
// score 1052 (was 1053): Frame 0x80 versus 0x74; velocity and force homes still differ.
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
				forward_speed_maximum -= (forwards == (pitch > 0.0f) ? 1.0f : -1.0f) * (real)fabs(pitch_slope) *
					forward_speed_maximum * 0.33f;
				side_speed_maximum -= (leftwards == (roll > 0.0f) ? 1.0f : -1.0f) * (real)fabs(roll_slope) *
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
				acceleration.i *= 0.5f;
				acceleration.j *= 0.5f;
				acceleration.k *= 0.5f;
			}
			else if (vehicle->unknown352 & 0x10)
			{
				acceleration.i *= 0.9f;
				acceleration.j *= 0.9f;
				acceleration.k *= 0.9f;
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
					real magnitude = PIN((real)fabs(arg_3097c5), 0.78539819f, 3.1415927f);

					maximum_yaw_acceleration = (magnitude - 0.78539819f) * 14.0f + maximum_yaw_acceleration;
				}
			}

			real target = (real)sqrt((real)fabs(steering) * maximum_yaw_acceleration * 2.0f) * vehicle_sign(steering);

			if (vehicle_sign(steering) == vehicle_sign(target) &&
				(real)fabs(target) * dt * 2.0f > (real)fabs(steering))
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
	real grounded = 0.0f;

	if (count > 0)
	{
		long touching = 0;

		for (i = 0; i < count; i++)
		{
			if (state->points[i].unknown80)
			{
				touching++;
			}
		}
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
// score 145 (was 154): Staged cross, native up receiver and complete rotation setters kept; projection/trig schedules remain.
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
// @retail 0xf44a0
bool __stdcall function_f44a0(long vehicle_index, matrix3x3 *facing, s_vehicle_physics_state *state,
	matrix3x3 *result)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	real fraction = function_f4250(vehicle_index);
	bool flipping = fraction > 0.0f;

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
		real angle;
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
			sine = (real)sin(angle);
			cosine = (real)cos(angle);
			axis.i = 0.0f - result->forward.i;
			axis.j = 0.0f - result->forward.j;
			axis.k = 0.0f - result->forward.k;
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
// score 41 (was 160): left: the cone call sets up EDI before ECX (retail ECX first; the same in
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
// @retail 0xf62f0
void __stdcall function_f62f0(long vehicle_index)
{
	if (g_5107ee)
	{
		s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
		byte *definition = VEHICLE_DEFINITION_GET(vehicle);

		if (*(long *)(definition + 0x2a0) != NONE && vehicle->unknown25c > 0.0f)
		{
			s_object_marker markers[16];
			short count = function_b8d30(vehicle_index, 0xf0000bb, markers, 15, false);

			for (short marker_index = 0; marker_index < count; ++marker_index)
			{
				s_object_marker *marker = &markers[marker_index];
				s_vehicle_wash_collision collision;
				vector3f direction;
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
						parameters.source = NULL;
						parameters.color_a = 0xff808080;
						parameters.color_b = 0xff808080;
						effect_markers[6].name = 0x80000ba;
						effect_markers[6].forward.i = facing.i - collision.normal.i * dot;
						effect_markers[6].forward.j = facing.j - collision.normal.j * dot;
						effect_markers[6].forward.k = facing.k - collision.normal.k * dot;
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

bool function_182800(s_vehicle_ray *ray, long ignore_index, void *world);
bool __stdcall function_168f40(long flags, s_vehicle_ray const *ray, long ignore_object_index, long ignore_unit_index);
byte function_11f470(real lo, real hi, real value);
long function_189060(long object_index, short value, real scale, point3f const *position,
	vector3f const *direction, long tag_index);

/* the vehicle's suspension: for each of its graph's function animations
   whose marker it has, a ray down from the marker finds how far the ground
   pushes it (every fourth tick, a spread of the vehicles' work, else the
   last one), which sets the function's value; a hard landing plays the
   definition's sound */
// score 197 (was 206): Frame 0xec agrees; permutation/flag store order is recovered. Found-index retest,
// scalar/SSE operand homes, ray/world staging and fallback-result lifetime remain; first differing branch is
// still +0x5a.
// Earlier at 206 (was 206): Frame 0xec agrees; changed/permutation stores, found-index retest, ray scale/world/zero and fallback-result ownership remain.
// Earlier at 206 (was 251): every frame slot now retail's; left: the override search's exit (retail tests the found
// every frame slot now retail's; left: the override search's exit (retail tests the found
// element out of line, threaded from the loop; ours re-tests i < count), the permutation pointer's load order,
// integer registers in the ray copies, the scale products (ours copy the scale, retail scales in place), the
// clamp/fallback store placement and the final flag writer. Before round 45 (286, was 303): the state/permutation
// and byte flag slots, the late zero register and the component pointer's NULL test also differed.
// Earlier: frame 0xe0 versus 0xec, iterator/scalar homes and final flag stores remain.
// Native point copy 350; up copy 341; both 333; both with long quantization 328;
// both with post-search scalar selection 321; all three 316 (kept); scalar selection alone 362;
// native ray setters 333; ray/marker/both at entry and complete workspace names 316 each.
// Round 425: point before up 317; shared word flag writer 316, with point first 317; long sound
// result 319, with point first 319, with writer/everything 341/341; both long decisions with all 344.
// All original source restored; missing twelve frame bytes and iterator/scalar homes remain.
// Lane 13 round 25: the twelve bytes are the ray's hit normal (0x182800 fills a 0x28-byte ray: point, vector, t,
// normal; the struct gained the field) 303 (kept). On it: the override search as a NULL element pointer set on the
// match (retail's loop and threaded exit exactly) 276 (a flag set in the loop: kept only on a match); the element
// pointer kept from the loop with `i < count` 292; with a count local 286; the count local alone 314; the point
// before up 303; the three offsets loaded rest first 303; `if (!marker.node_index) {...} else value = 0` 300; all
// three 300; with the component read at each use behind the index test (no NULL test) 298; byte_flags_set 300;
// word_flags_set bit 8 (retail's or/and RMW and split epilogue) 310 at 1378 bytes; that with the NULL pointer
// search 288; the iterator initialized before the largest change, permutations and changed flag 298 (stores
// then in retail's order).
// Lane 13 round 28: the element pointer kept from the loop with `i < count` 306 (frame moves); with a count local
// 286 (kept: the loop is retail's; its exit still re-tests i < count). On 286: the ray point before up 288; the
// offsets loaded rest first 289; `iterator.animation_id = c_type_709360();` after the index (on 303) 322; explicit
// graph_index/index stores in member order 286 (init then in retail's order); the init before the largest change,
// permutations and changed 286; `if (!marker.node_index) {...} else value = 0` 286 (zero block out of line as
// retail); havok_component_get at each use (no NULL test) 284; both 285; `sounded` declared first 284; the init
// first too 284; the NULL found pointer 276 (a flag set in the loop).
// Round 182 (retail packs sounded/changed as bytes +0xe/+0xf of one dword, ours puts them in two dwords at
// +0xf/+0x17, which moves largest_change, current and the state/permutations slots): changed declared at
// function scope beside sounded 289 (still two dwords); changed renamed (slot order by name) 286 (same slots);
// the update bit through word_flags_set bit 8 296 (1383 bytes); the permutation count inside the pointer
// expression 286 (identical); the ray declared at the top of the block 286 (identical).
// Round 45 (lane 1): the averaged value written in the quantize call instead of assigned back to value 277 (kept:
// retail's sounded/changed packed dword, call temp slot and permutation/state order); sounded declared first 273
// (kept); the iterator stores in member order before largest_change/changed 276 on 277 (with or without the
// redundant id stores); the point copied before up 275 on 273; `if (!marker.node_index) {...} else value = 0`
// 274 on 273 (retail's layout); vector3f_scale for the ray vector 274 (identical); offset/rest/length assignment
// order 270 on 273, with the component read at each use behind the index test (no NULL test, 0xf6bd0's form) 268
// with the moved init / 269 without (kept); the component change alone 271; offset/length/rest orders
// rol/lor/rlo 264/265/272 against orl 261 on the NULL found pointer form (that form 264; a flag set in the loop).
// Round 45, second pass on 269: the point copied first 271; SET_FLAG for the update bit 269; `node_index != 0`
// 269; the fallback as `value = 0.0f; if (function_168f40(...)) value = 1.0f;` 265 (the XMM7 zero goes); with the
// point first 265, with the node test's zero arm last 263, with both 251 (kept); on 251 `real reach = 0.0f -
// length;` before the ray / at the products / after the copies / between the copies 249/251/249/249 (in-place
// products but a hoisted zero); the Havok clamp as an explicit store then if/else-if 257.
// Round 44 (lane 11): the permutation count/pointer through the existing 0xba690 (count, NULL, NULL, &pointer;
// LTCG inlines it with retail's loads) 250 -> 214 (kept); the iterator's constructor and real-typed floats
// (0x1dceb0 still matches; iterator stores then exactly retail's, the casts at the reads gone) 251 -> 250 and
// 214 -> 206 (kept); 0xba690 before the iterator 215; the havok component pointer read before the && chain 204
// (computed ahead of the NONE test); as `index != NONE ? get : NULL` 206; the fallback ternary 236, as if/else
// 242 (LTCG keeps its zero in an XMM register across 0x168f40); word_flags_set bit 8 216; the update bit through
// the word-bits view in if/else 216 (retail's `or/and byte [edi+0x349]` and split epilogue exactly, a score rise
// at 1376 bytes); `ray.vector.i *= 0.0f - length` 272; the ray vector copied by components 318; the override
// search as a NULL found pointer 206 (retail's loop exit exactly; a flag set in the loop); the Havok clamp as a
// store then if/else-if 212 (retail's clamp block exactly); `changed` declared after the 0xba690 call 199 with
// reach / 204 without (retail's store order; under the bar); `real reach = 0.0f - length;` used in all three
// products, before/after/between the ray copies 199/199/199 (retail's in-place products, but the zero is then
// hoisted to XMM5; 7 points, under the bar); reach declared with offset or first, or at the products, 206
// (identical); vector3f_scale 206; an inline helper returning the active component's world (NULL otherwise),
// diagnostic: with the found pointer, the bit writer and the clamp store 211 at 1375 bytes, the component block
// then retail's (`[eax+ebx+4]` flag read, then `add eax, ebx`), a bool ray-cast helper the same; all of those
// with reach 181. Left on that set: the world loaded into EAX then moved to ESI (retail loads ESI), the zero
// hoisting with reach, and the fallback select (retail `movss 1.0 / jne / xorps`).
// Round 469: Integer-displacement grouping with indexed mismatch-continue, body-stepped
// cursor, predicate-owned cursor and indexed while searches 251/257/257/251;
// the four searches with the original displacement 251/257/257/251. Original body restored;
// frame 0xec agrees, metadata load order and found-index retest remain.
// J222: Translated-point setter, reused change and both 206/206/206; promoted current sample alone and with
// point/change/both 245 each; point/change/scale plus named compression and travel 205 (one point, restored).
// Original complete executable body and all helper declarations restored; no calibrated loss.
// J222: Fresh compression/travel parts with the recorded three-use negative factor 199; adding late change-flag
// initialization 197; the same pair on the translated-point/reused-change graph 199/197. The expression parts
// add no further gain on these graphs. Selected the simpler joint 197 source (nine points from round baseline
// 206), already ordinary whole-program confirmed without a loss; no isolated seven/two/one-point form kept.
// Failed all-root comparisons t10/t11 excluded; exact-source fallbacks t13/t14 and final t15 are archived.
// J224: Native override records 197; with live iterator index/live count/scalar role names 197/265/197.
// True-first fallback 207; with records 207; with completed clamp-result assignment 203;
// all three 203. Original executable source restored; frame and first +0x5a unchanged.
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
				byte *override;
				long i;

				for (i = 0; i < override_count; i++)
				{
					override = *(byte **)(definition + 0x2f4) + i * 0x4c;
					if (*(long *)(override + 0x44) == iterator.unknown04 && *(long *)(override + 0x48) != NONE)
					{
						break;
					}
				}

				if (i < override_count && (char)((byte const *)permutations)[*(long *)(override + 0x48) * 8 + 1] >=
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
				real reach = 0.0f - length;

				ray.point = marker.matrix.position;
				ray.vector = vehicle->up;
				ray.point.x += ray.vector.i * offset;
				ray.point.y += ray.vector.j * offset;
				ray.point.z += ray.vector.k * offset;
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
						if (vehicle->havok_component_index != NONE &&
							TEST_FIELD_BIT(havok_component_get(vehicle->havok_component_index)->flag5) &&
							havok_component_get(vehicle->havok_component_index)->unknown9c &&
							function_182800(&ray, NONE,
								(void *)havok_component_get(vehicle->havok_component_index)->unknown9c))
						{
							real compression = ray.t * length - rest;
							real travel = length - rest;

							value = PIN(1.0f - compression / travel, 0.0f, 1.0f);
						}
						else
						{
							value = 0.0f;
							if (function_168f40(0x8c2d, &ray, vehicle_index, NONE))
							{
								value = 1.0f;
							}
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
	if (update)
	{
		vehicle->flags349 |= 1;
	}
	else
	{
		vehicle->flags349 &= ~1;
	}
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
// score 765 (was 817): Frame 0x18 agrees, 16 bytes short; entry definition ownership, zero/result SSE allocation and per-site versus hoisted 1.0 remain.
// Earlier at 817 (was 864): Frame 0x18 agrees; definition EBP and scalar/SSE allocation differ.
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
			goto done;
		}
		result = function_d1210(vehicle_index);
		break;
	case 0x4000580:
		if (function_f5d70(vehicle_index) || (vehicle->control_flags & 2) ||
			TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit5))
		{
			result = 1.0f;
		}
		goto done;
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
		goto done;
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
			goto done;
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
			goto done;
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
			goto done;
		}
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f4), *(real *)(tags[vehicle->definition_index & 0xffff].bytes + 0x1f8)));
		break;
	case 0xf000584:
		if (!(vehicle->unknown34f > 0))
		{
			goto done;
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

done:
	*value = result;
	if (result > 0.0f || forced)
	{
		*active = true;
	}
	else
	{
		*active = false;
	}
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
// score 1134 (was 1166): Frame 0xfc versus 0xd8; index/offset and vector/rotation homes remain.
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
	bool boosting;
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
	real boost = 1.0f;

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit4))
	{
		boost = function_d1210(vehicle_index) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
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
			0.0f - vehicle->speed / *(real *)(definition + 0x1f8);
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
		real const *velocity_components = &velocity.i;
		real *acceleration_components = &acceleration.i;

		for (i = 0; i < 3; i++)
		{
			real next = acceleration_components[i] + velocity_components[i];

			if ((next >= 0.0f) != (0.0f >= acceleration_components[i]))
			{
				acceleration_components[i] = 0.0f - velocity_components[i];
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
		vector3f strafe_target;
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
		strafe_target.i = strafe * facing.left.i;
		strafe_target.j = facing.left.j * strafe;
		strafe_target.k = facing.left.k * strafe;

		real side_speed = facing.left.i * velocity.i + facing.left.j * velocity.j + facing.left.k * velocity.k;

		strafe_velocity.i = facing.left.i * side_speed;
		strafe_velocity.j = facing.left.j * side_speed;
		strafe_velocity.k = facing.left.k * side_speed;
		function_f1380(&strafe_acceleration, &strafe_target, &strafe_velocity,
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
			desired.up.i = backward.i * sine + desired.up.i * cosine;
			desired.up.j = backward.j * sine + desired.up.j * cosine;
			desired.up.k = backward.k * sine + desired.up.k * cosine;
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
	length = (real)sqrt(rotation.k * rotation.k + rotation.j * rotation.j + rotation.i * rotation.i);
	if (0.0001f > (real)fabs(length))
	{
		axis.i = rotation.i;
		axis.j = rotation.j;
		axis.k = rotation.k;
		length = 0.0f;
	}
	else
	{
		real inverse = 1.0f / length;

		axis.i = inverse * rotation.i;
		axis.j = rotation.j * inverse;
		axis.k = rotation.k * inverse;
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
		change = spin - vehicle->unknown374 > step ? spin - vehicle->unknown374 : step;
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
