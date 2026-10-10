// @flags /O2 /Ob1 /arch:SSE /Gr
/* VEHICLES.CPP: vehicles

The vehicle object type's callbacks (its definition at 0x467b40) and the
helpers only they call. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_1cec30.h"
#include "effects.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "unknown_1eb550.h"
#include <math.h>
#include <string.h>

#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
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
	vehicle->flags349 |= 1;
	function_b9ef0(vehicle_index, &vehicle->unknown3d0);
	vehicle->unknown3b8 = NONE;
	vehicle->unknown3bc = NONE;
	vehicle->unknown3c8 = NONE;
	vehicle->unknown3c0 = NONE;
	vehicle->unknown3cc = NONE;
	vehicle->unknown400 = NONE;
	for (i = 0; i < 12; i++)
		vehicle->unknown404[i] = NONE;
	for (i = 0; i < 3; i++)
	{
		vehicle->unknown41c[i][0] = NONE;
		vehicle->unknown41c[i][1] = NONE;
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

/* a vehicle's steering: the angle to its velocity, eased by the
   definition's steering curve, and its throttle and turn rates */
// @retail 0xeec90
void function_eec90(long vehicle_index, real *steering)
{
	real dt = g_510c54->rate;
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	vector3f const *forward = &vehicle->forward;
	vector3f const *up = &vehicle->up;
	vector3f const *velocity = &vehicle->local_velocity;
	real left_i = forward->k * up->j - forward->j * up->k;
	real left_j = forward->i * up->k - forward->k * up->i;
	real left_k = forward->j * up->i - forward->i * up->j;
	real angle = (real)atan2(left_j * velocity->j + left_k * velocity->k + left_i * velocity->i,
		velocity->k * forward->k + velocity->j * forward->j + forward->i * velocity->i);

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit3) &&
		!function_f42f0(vehicle_index) && !(function_d1210(vehicle_index) > 0.0f))
	{
		function_2054b0(&vehicle->speed, (real const *)(definition + 0x1f4), 1.0f, dt, 0.0f);
		function_2054b0(&vehicle->turn, (real const *)(definition + 0x22c), 1.0f, dt, 0.0f);
	}
	else
	{
		real bound;

		bound = (0.0f > vehicle->throttle ? *(real *)(definition + 0x1f8) : *(real *)(definition + 0x1f4)) * vehicle->throttle;
		if (vehicle->speed > bound)
		{
			function_2053c0(&vehicle->speed, (real const *)(definition + 0x1f4), -1.0f, dt);
			if (bound >= vehicle->speed)
				vehicle->speed = bound;
		}
		else if (bound > vehicle->speed)
		{
			function_2053c0(&vehicle->speed, (real const *)(definition + 0x1f4), 1.0f, dt);
			if (vehicle->speed >= bound)
				vehicle->speed = bound;
		}
		bound = (0.0f > vehicle->steering ? *(real *)(definition + 0x230) : *(real *)(definition + 0x22c)) * vehicle->steering;
		if (vehicle->turn > bound)
		{
			function_2053c0(&vehicle->turn, (real const *)(definition + 0x22c), -1.0f, dt);
			if (bound >= vehicle->turn)
				vehicle->turn = bound;
		}
		else if (bound > vehicle->turn)
		{
			function_2053c0(&vehicle->turn, (real const *)(definition + 0x22c), 1.0f, dt);
			if (vehicle->turn >= bound)
				vehicle->turn = bound;
		}
	}

	real maximum_angle = (real)((*(real *)(definition + 0x250) == 0.0f ? 1.0 : *(real *)(definition + 0x250)) *
		0.017453292384743690);
	real exponent = *(real *)(definition + 0x254) == 0.0f ? 1.0f : *(real *)(definition + 0x254);

	angle = PIN(angle, -3.1415927f, 3.1415927f);

	real sign = angle >= 0.0f ? 1.0f : -1.0f;
	real eased = (real)pow(fabs(angle / maximum_angle), exponent) * sign * maximum_angle;

	eased = PIN(eased, -3.1415927f, 3.1415927f);

	real target = vehicle->gear ? eased : 0.0f - eased;
	real rate = *(real *)(definition + 0x210) * 0.017453292f;
	real minimum = *(real *)(definition + 0x208) * 0.017453292f;
	real maximum = *(real *)(definition + 0x204) * 0.017453292f;
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
PRIVATE inline void vehicle_normalize2d(point2f *v)
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
	*v = *g_468778;
}

/* direct turn control: the input turned into the frame of the velocity,
   the angle it asks the vehicle to turn, and whether it's reversing */
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

	velocity.x = vehicle->local_velocity.i;
	velocity.y = vehicle->local_velocity.j;

	real speed = (real)sqrt(velocity.y * velocity.y + velocity.x * velocity.x);
	real input_magnitude_squared = vehicle->throttle * vehicle->throttle + vehicle->steering * vehicle->steering;

	if (!(0.0001f > (real)fabs(speed)))
	{
		real inverse = 1.0f / speed;

		direction.x = inverse * velocity.x;
		direction.y = inverse * velocity.y;
	}
	else
	{
		direction = velocity;
	}
	forward.x = vehicle->forward.i;
	forward.y = vehicle->forward.j;
	input.x = vehicle->throttle * direction.x - vehicle->steering * direction.y;
	input.y = vehicle->steering * direction.x + vehicle->throttle * direction.y;
	vehicle_normalize2d(&forward);

	real perpendicular = 0.0f - forward.y;

	heading.x = vehicle->local_velocity.i;
	heading.y = vehicle->local_velocity.j;
	vehicle_normalize2d(&heading);

	real k = 1.0f - (heading.y * forward.y + heading.x * forward.x);
	point2f blend;

	blend.y = forward.y * k + heading.y;
	blend.x = forward.x * k + heading.x;

	real blend_length = (real)sqrt(blend.y * blend.y + blend.x * blend.x);

	if (!(0.0001f > (real)fabs(blend_length)) && blend_length != 0.0f)
	{
		real inverse = 1.0f / blend_length;

		blend.x = inverse * blend.x;
		blend.y = blend.y * inverse;
	}
	else
	{
		blend = forward;
	}

	bool reversing = !((real)fabs(atan2(input.y * blend.x + (0.0f - blend.y) * input.x,
		blend.y * input.y + blend.x * input.x)) < 1.9198622f);
	real angle = (real)atan2(input.y * forward.x + perpendicular * input.x, forward.y * input.y + forward.x * input.x);
	real offset;

	if (reversing)
		offset = angle > 0.0f ? -3.1415927f : 3.1415927f;
	else
		offset = 0.0f;
	angle = offset + angle;
	input_magnitude_squared = vehicle->throttle * vehicle->throttle + vehicle->steering * vehicle->steering +
		vehicle->unknown1b8 * vehicle->unknown1b8;
	vehicle->gear = input_magnitude_squared != 0.0f ? !reversing : vehicle->gear;

	real steer = PIN(*(real *)(definition + 0x220) * angle, -0.78539819f, 0.78539819f) * 1.2732395f;
	real bound = (0.0f > steer ? *(real *)(definition + 0x230) : *(real *)(definition + 0x22c)) * steer;
	real dt = g_510c54->rate;

	if (vehicle->turn > bound)
	{
		function_2053c0(&vehicle->turn, (real const *)(definition + 0x22c), -1.0f, dt);
		if (bound >= vehicle->turn)
			vehicle->turn = bound;
	}
	else if (bound > vehicle->turn)
	{
		function_2053c0(&vehicle->turn, (real const *)(definition + 0x22c), 1.0f, dt);
		if (vehicle->turn >= bound)
			vehicle->turn = bound;
	}
	if (reversing && dt > 0.09f)
		vehicle->flags349 |= 2;
	else
		vehicle->flags349 &= ~2;
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
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	bool special = function_f42f0(vehicle_index);
	bool braking = special || (vehicle->control_flags & 0x800);

	if (*(short *)(definition + 0x1f0) == 5)
	{
		real throttle = PIN(vehicle->throttle, 0.0f, g_4678d4);

		if (throttle != vehicle->throttle)
			vehicle->throttle = g_4678d4;
	}
	*steering = 0.0f;
	SET_FLAG(*(word *)&vehicle->flags348, 3, (vehicle->control_flags & 2) ||
		TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit4) &&
		(vehicle->throttle > 0.0f && 0.0f > vehicle->speed || 0.0f > vehicle->throttle && vehicle->speed > 0.0f));
	if (TEST_FIELD_BIT(VEHICLE_DEFINITION_FLAGS(definition)->bit17))
		SET_FLAG(*(word *)&vehicle->flags348, 5,
			TEST_FIELD_BIT(((s_vehicle_flag_bits const *)&vehicle->control_flags)->bit11));
	*(word *)&vehicle->flags348 &= ~FLAG(4);
	if (TEST_FIELD_BIT(((s_vehicle_flag_bits const *)(definition + 0xbc))->bit28) &&
		(braking || vehicle->unknown334 > 0.0f))
	{
		SET_FLAG(*(word *)&vehicle->flags348, 4, vehicle->unknown334 > vehicle->unknown338);
		if (braking)
		{
			vector3f *input = (vector3f *)&vehicle->throttle;

			*input = *g_4687a8;
			if (special)
			{
				if (vehicle->unknown350 == 1)
				{
					*input = *g_4687ac;
					if (*(real *)(definition + 0x22c) >= 0.0f)
						vehicle->turn = *(real *)(definition + 0x22c);
					else
						vehicle->turn = 0.0f - *(real *)(definition + 0x22c);
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

	if ((vehicle->flags348 >> 7) & 1)
	{
		s_vehicle *current = VEHICLE_GET(vehicle_index);
		real dt = g_510c54->rate;
		point3f const *root = (point3f const *)((byte *)current + *(short *)((byte *)current + 0x116) + 0x28);
		real bound = (0.0f > vehicle->unknown3e0 ? *(real *)(definition + 0x1f8) : *(real *)(definition + 0x1f4)) *
			vehicle->unknown3e0;

		vehicle_approach(&vehicle->unknown3dc, (real const *)(definition + 0x1f4), bound, dt);
		vehicle->unknown3f0.i = root->x - vehicle->unknown3e4.x;
		vehicle->unknown3f0.j = root->y - vehicle->unknown3e4.y;
		vehicle->unknown3f0.k = root->z - vehicle->unknown3e4.z;
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
// @retail 0xef870
bool function_ef870(long vehicle_index)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit6) && vehicle->unknown34d &&
		(long)vehicle->unknown34e < vehicle_round((real)g_510c54->field_2_3 * 2.0f) &&
		0.9f >= vehicle->up.k)
	{
		real strength = vehicle->unknown34d == 2 || vehicle->unknown34d == 4 ? 0.3f : -0.3f;
		vector3f linear_velocity;
		vector3f angular_velocity;
		vector3f axis;

		function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
		if (vehicle->unknown34d == 4 || vehicle->unknown34d == 3)
		{
			axis.i = vehicle->up.k * vehicle->forward.j - vehicle->up.j * vehicle->forward.k;
			axis.j = vehicle->up.i * vehicle->forward.k - vehicle->up.k * vehicle->forward.i;
			axis.k = vehicle->forward.i * vehicle->up.j - vehicle->up.i * vehicle->forward.j;
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
		rate *= strength;
		if (vehicle->unknown34d == 2 || vehicle->unknown34d == 1)
		{
			vector3f side;
			real down = 0.0f - vehicle->forward.k;

			side.i = vehicle->up.k * vehicle->forward.j - vehicle->forward.k * vehicle->up.j;
			side.j = vehicle->forward.k * vehicle->up.i - vehicle->up.k * vehicle->forward.i;
			side.k = vehicle->forward.i * vehicle->up.j - vehicle->forward.j * vehicle->up.i;
			axis.i = down * side.i + axis.i;
			axis.j = side.j * down + axis.j;
			axis.k = side.k * down + axis.k;
		}
		angular_velocity.i = axis.i * rate;
		angular_velocity.j = axis.j * rate;
		angular_velocity.k = axis.k * rate;
		if (*(short *)(definition + 0x1f0) == 0)
		{
			real dot = vehicle->forward.j * linear_velocity.j + vehicle->forward.k * linear_velocity.k +
				vehicle->forward.i * linear_velocity.i;

			linear_velocity.i = vehicle->forward.i * dot;
			linear_velocity.j = vehicle->forward.j * dot;
			linear_velocity.k = vehicle->forward.k * dot;
		}
		function_b77d0(vehicle_index, &linear_velocity, &angular_velocity);
		vehicle->unknown34e++;
		return true;
	}
	vehicle->unknown34e = 0;
	vehicle->unknown34d = 0;
	vehicle->flags348 &= ~0x40;
	return false;
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

	((s_vehicle_word_flag_bits *)&vehicle->flags348)->bit5 = false;
	vehicle->gear = NONE;
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
bool function_205510(void *buffer, void const *definition_physics, s_havok_component *component);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_1cfb90(s_havok_component *component, transform4x3f const *matrix, void const *buffer);
bool __stdcall function_f6670(long vehicle_index, bool update);
void function_ba3d0(long object_index);
void function_f06c0(s_vehicle_physics_state *state);
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

/* the vehicle object type's update: its Havok bodies, controls, its
   vehicle type's physics, its kill volume, boost and lights, and the unit
   update */
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

					if (function_205510(buffer, definition + 0x2ac, component))
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
		s_vehicle *current = VEHICLE_GET(vehicle_index);
		s_animation_state *state_view = vehicle_animation_state_get(current);

		if (*(long *)((byte *)state_view + 0x68) != NONE && state_view->channels[0].graph_tag_index != NONE &&
			state_view->channels[0].animation_id.index != NONE && state_view->unknown7c == 0x400000c)
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

/* clears the sixteen entries of the physics state's block at +0xbc0 */
// @retail 0xf06c0
void function_f06c0(s_vehicle_physics_state *state)
{
	s_vehicle_contact *contact = state->contacts;
	short i;

	for (i = 16; i; i--)
	{
		contact->unknownb0 = NONE;
		contact++;
	}
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

#define VEHICLE_OVERLAY(vehicle, index) (*(c_type_709360 *)&(vehicle)->unknown404[(index) * 2])
#define VEHICLE_OVERLAY41C(vehicle, index) (*(c_type_709360 *)(vehicle)->unknown41c[index])

/* a speed as a fraction of the definition's maximum forward or reverse
   speed, 0.5 at rest */
PRIVATE inline real vehicle_speed_fraction(real speed, byte const *definition)
{
	real result = 0.5f;

	if (speed > 0.0f)
	{
		if (*(real *)(definition + 0x1f4) > 0.0f)
		{
			result = PIN(speed / *(real *)(definition + 0x1f4) * 0.5f + 0.5f, 0.0f, 1.0f);
		}
	}
	else if (*(real *)(definition + 0x1f8) > 0.0f)
	{
		result = PIN(speed / *(real *)(definition + 0x1f8) * 0.5f + 0.5f, 0.0f, 1.0f);
	}
	return result;
}

/* hands each of the vehicle's value-driven animations (its steering,
   sideways drift, speed, forward speed and two engine overlays, the
   graph's function animations and the three aiming overlays) to the
   callback with its value */
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
			VEHICLE_OVERLAY(vehicle, 0) = state->overlay_find(0x8000078, 0x7000101, 0x7000101);
			VEHICLE_OVERLAY(vehicle, 1) = state->overlay_find(0x4000079, 0x7000101, 0x7000101);
			VEHICLE_OVERLAY(vehicle, 2) = state->overlay_find(0x800007a, 0x7000101, 0x7000101);
			VEHICLE_OVERLAY(vehicle, 3) = state->overlay_find(0x800007b, 0x7000101, 0x7000101);
			VEHICLE_OVERLAY(vehicle, 4) = state->overlay_find(0xc00007d, 0x7000101, 0x7000101);
			VEHICLE_OVERLAY(vehicle, 5) = state->overlay_find(0x110005c9, 0x7000101, 0x7000101);
			for (i = 0; i < 3; i++)
			{
				VEHICLE_OVERLAY41C(vehicle, i) = state->overlay_kind_get(i);
			}
		}
		if (VEHICLE_OVERLAY(vehicle, 0).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 0), NONE, NONE, NONE, NONE))
		{
			callback(vehicle_index, graph, &channel, vehicle->steering_angle, 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 1).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 1), NONE, NONE, NONE, NONE))
		{
			vector3f left;

			left.i = vehicle->up.j * vehicle->forward.k - vehicle->up.k * vehicle->forward.j;
			left.j = vehicle->forward.i * vehicle->up.k - vehicle->up.i * vehicle->forward.k;
			left.k = vehicle->up.i * vehicle->forward.j - vehicle->forward.i * vehicle->up.j;
			callback(vehicle_index, graph, &channel,
				((linear_velocity.i * left.i + left.j * linear_velocity.j + linear_velocity.k * left.k) /
					*(real *)(definition + 0x1f4) + 1.0f) * 0.5f,
				0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 2).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 2), NONE, NONE, NONE, NONE))
		{
			callback(vehicle_index, graph, &channel, vehicle_speed_fraction(vehicle->speed, definition), 0.0f, 1.0f,
				user);
		}
		if (VEHICLE_OVERLAY(vehicle, 3).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 3), NONE, NONE, NONE, NONE))
		{
			real speed = vehicle->forward.k * linear_velocity.k + vehicle->forward.j * linear_velocity.j +
				linear_velocity.i * vehicle->forward.i;

			callback(vehicle_index, graph, &channel, vehicle_speed_fraction(speed, definition), 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 4).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 4), NONE, NONE, NONE, NONE))
		{
			real value = 0.0f;

			if (*(real *)(definition + 0x20c) > 0.0f)
			{
				value = vehicle->unknown360 / *(real *)(definition + 0x20c);
			}
			callback(vehicle_index, graph, &channel, value, 0.0f, 1.0f, user);
		}
		if (VEHICLE_OVERLAY(vehicle, 5).index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY(vehicle, 5), NONE, NONE, NONE, NONE))
		{
			real value = 0.0f;

			if (*(real *)(definition + 0x20c) > 0.0f)
			{
				value = vehicle->unknown364 / *(real *)(definition + 0x20c);
			}
			callback(vehicle_index, graph, &channel, value, 0.0f, 1.0f, user);
		}

		s_graph_iterator3c iterator;
		c_animation_channel function_channel;

		iterator.unknown00 = NONE;
		*(real *)&iterator.unknown08 = 0.0f;
		*(real *)&iterator.unknown0c = 0.0f;
		*(real *)&iterator.unknown10 = 0.0f;
		iterator.index = NONE;
		iterator.next_index = NONE;
		*(short *)iterator.unknown28 = NONE;
		while (function_1dceb0(&iterator, graph_tag_get(state->graph_tag_index)))
		{
			if (state->graph_tag_index != NONE &&
				function_channel.set(state->graph_tag_index, 0, iterator.animation_id, NONE, NONE, NONE, NONE))
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
				real value = PIN(vehicle->unknown288[i] * 0.5f + 0.5f, 0.0f, 1.0f);

				if (state->graph_tag_index != NONE &&
					channel.set(state->graph_tag_index, 0, VEHICLE_OVERLAY41C(vehicle, i), NONE, NONE, NONE, NONE))
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
	return (*(dword *)(VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index)) + 0x1ec) >> 7) & 1;
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
			vehicle->flags348 |= 4;
		}
		else
		{
			vehicle->flags348 &= ~4;
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
	real limit, real dt)
{
	real scale;
	real dot;
	real maximum;
	real length_squared;

	velocity->i = a->i - b->i;
	velocity->j = a->j - b->j;
	velocity->k = a->k - b->k;
	scale = 1.0f / dt;
	velocity->i *= scale;
	velocity->j *= scale;
	velocity->k *= scale;
	dot = a->j * velocity->j + a->k * velocity->k + a->i * velocity->i;
	if (dot > 0.0001f)
	{
		maximum = dot * dot / (velocity->k * velocity->k + velocity->j * velocity->j + velocity->i * velocity->i) /
			(a->j * a->j + a->k * a->k + a->i * a->i) * (aligned_limit - limit) + limit;
	}
	else
	{
		maximum = limit;
	}
	length_squared = velocity->k * velocity->k + velocity->j * velocity->j + velocity->i * velocity->i;
	if (length_squared > maximum * maximum)
	{
		real factor = maximum / (real)sqrt(length_squared);

		velocity->i *= factor;
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
	real maximum;

	if (peak > angular_velocity)
	{
		if (peak > 0.001f)
		{
			return (curve->max_torque - curve->min_torque) / peak * angular_velocity + curve->min_torque;
		}
		return 0.0f;
	}
	maximum = VEHICLE_MAX_ANGULAR_VELOCITY(definition);
	if (angular_velocity >= maximum)
	{
		real slope = (curve->torque_at_2x_max_angular_velocity - curve->torque_at_max_angular_velocity) / maximum;

		return curve->torque_at_max_angular_velocity - maximum * slope + slope * angular_velocity;
	}
	else
	{
		real range = maximum - peak;

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
				return fraction * torque;
			}
		}
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
	char current = vehicle->gear;
	short gear = current;

	if (vehicle->unknown384 && (0.5f > vehicle->unknown384 * time->rate || (vehicle->control_flags & 2)))
	{
		braking = true;
	}
	else
	{
		braking = false;
	}
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
				return NONE;
			}
		}
		else if (*(short *)(definition + 0x1f0) == 1)
		{
			if (0.0f > throttle)
			{
				if (!braking)
				{
					return 0;
				}
			}
			else if (current < 1)
			{
				if (!braking)
				{
					return 1;
				}
			}
			else
			{
				s_vehicle_gear *definition_gear = &VEHICLE_GEARS(definition)[current];

				if (vehicle->unknown387 * time->rate > definition_gear->min_time_to_upshift && throttle >= ratio &&
					ratio > definition_gear->engine_upshift_scale)
				{
					gear = current + 1;
					if (gear < 1)
					{
						gear = 1;
					}
					else if (gear > VEHICLE_GEAR_COUNT(definition) - 1)
					{
						gear = (short)(VEHICLE_GEAR_COUNT(definition) - 1);
					}
				}
				if (vehicle->unknown388 * time->rate > definition_gear->min_time_to_downshift &&
					definition_gear->engine_downshift_scale > ratio)
				{
					long lower = current - 1;

					if (gear <= 0 || lower < 1)
					{
						return 1;
					}
					if (lower <= VEHICLE_GEAR_COUNT(definition) - 1)
					{
						gear = (short)lower;
					}
				}
			}
		}
	}
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
		real throttle = 0.0f;

		if (!vehicle->unknown385 && !function_f5d70(vehicle_index))
		{
			throttle = throttle_input;
		}

		char current = vehicle->gear;
		real gear_ratio = function_f14f0(current, vehicle_index);
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

		bool reversed = new_gear != -2 && (current >= 1) != (new_gear >= 1);

		vehicle->unknown380 += (throttle - engine) * (g_510c54->rate * 45.454544f);
		vehicle->unknown380 = PIN(vehicle->unknown380, 0.0f, 1.0f);
		if (reversed || (current == NONE && new_gear != current))
		{
			vehicle->gear = (char)new_gear;
			vehicle->unknown38c = (char)0xfe;
			vehicle->unknown386 = 0;
			vehicle->unknown387 = 0;
			vehicle->unknown385 = 0;
			vehicle->unknown388 = 0;
		}
		else if (vehicle->unknown38c == (char)0xfe && new_gear == current)
		{
			vehicle->unknown385 = 0;
			vehicle->unknown386 = (byte)MIN(vehicle->unknown386 + 1, 0xff);
			if (current >= 0)
			{
				s_vehicle_gear *definition_gear = &VEHICLE_GEARS(definition)[current];

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
			long delay = (long)(g_510c54->field_2_3 * 0.05f);

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
			if (VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 0.05f > angular_velocity)
			{
				angular_velocity = VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 0.05f;
			}
			else if (angular_velocity > VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 1.3f)
			{
				angular_velocity = VEHICLE_MAX_ANGULAR_VELOCITY(definition) * 1.3f;
			}
			vehicle->unknown378 = angular_velocity;
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
PRIVATE inline real vehicle_wheel_spin(real spin, real period)
{
	if (period > 0.001f)
	{
		if (!(spin > 0.0f))
		{
			spin += period;
		}
		return (real)fmod(spin, period);
	}
	return 0.0f;
}

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
	if (vehicle->gear == NONE)
	{
		torque = 0.0f;
		torque_magnitude = torque;
	}
	else
	{
		torque = function_f14f0(vehicle->gear, vehicle_index) * VEHICLE_GET(vehicle_index)->unknown378;
		torque_magnitude = torque >= 0.0f ? torque : 0.0f - torque;
	}

	real turn_torque = torque_magnitude * turn;

	left = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque - turn_torque;
	right = (1.0f - (turn >= 0.0f ? turn : 0.0f - turn)) * torque + turn_torque;
	for (i = 0; i < state->definition->contact_count; i++)
	{
		state->contacts[i].torque = (i & 1) ? right : left;
	}
	vehicle->unknown368 += left * dt;
	vehicle->unknown368 = vehicle_wheel_spin(vehicle->unknown368, *(real *)(definition + 0x20c));
	vehicle->unknown36c += right * dt;
	vehicle->unknown36c = vehicle_wheel_spin(vehicle->unknown36c, *(real *)(definition + 0x20c));
	function_2056e0(vehicle_index, state, function_f5e00(vehicle_index), NULL, NULL);
	function_f1c80(vehicle_index, state);
	function_f5ee0(vehicle_index);
}

void function_f5bf0(long vehicle_index);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

/* a wheel's spin advanced by its angular velocity, wrapped into the
   definition's period */
PRIVATE inline real vehicle_wheel_spin_wrap(real spin, real period)
{
	spin = period > 0.001f ? (real)fmod(spin, period) : 0.0f;
	if (0.0f > spin)
	{
		spin += period;
	}
	return spin;
}

/* the contact turned about its up axis by half the steering angle, in the
   state's frame */
PRIVATE inline void vehicle_contact_steer(s_vehicle_physics_state *state, s_vehicle_contact *contact, real sine,
	real cosine)
{
	quaternionf rotation;

	rotation.i = 0.0f;
	rotation.j = 0.0f;
	rotation.k = sine;
	rotation.w = cosine;
	function_141e10(&contact->matrix.rotation, &rotation);
	contact->matrix.position.x = 0.0f;
	contact->matrix.position.y = 0.0f;
	contact->matrix.position.z = 0.0f;
	contact->matrix.scale = 1.0f;
	function_142a60(&state->matrix, &contact->matrix, &contact->matrix);
}

/* the physics of type 1 vehicles (the wheel-steered ones): the engine's
   torque to every contact, the steered contacts turned, and the wheels'
   spin */
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
	else if (vehicle->gear == NONE)
	{
		torque = 0.0f;
	}
	else
	{
		torque = function_f14f0(vehicle->gear, vehicle_index) * VEHICLE_GET(vehicle_index)->unknown378;
	}
	for (i = 0; i < state->definition->contact_count; i++)
	{
		dword flags = state->definition->contacts[i].flags;
		s_vehicle_contact *contact = &state->contacts[i];

		contact->torque = torque;
		if (braking)
		{
			contact->unknown83 = true;
		}
		if (TEST_FIELD_BIT((flags >> 2) & 1))
		{
			vehicle_contact_steer(state, contact, sine, cosine);
		}
		else if (TEST_FIELD_BIT((flags >> 3) & 1))
		{
			vehicle_contact_steer(state, contact, 0.0f - sine, cosine);
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

	vehicle->unknown360 = vehicle_wheel_spin_wrap(vehicle->unknown360 + spin, *(real *)(definition + 0x20c));
	if (!(TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit5)))
	{
		vehicle->unknown364 = vehicle_wheel_spin_wrap(vehicle->unknown364 + spin, *(real *)(definition + 0x20c));
	}
}

void function_141590(transform4x3f const *in, transform4x3f *out);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void function_11d790(quaternionf const *q, vector3f *axis, real *angle);

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
	vector3f const *facing = &vehicle->local_velocity;

	if (TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags348)->bit4))
	{
		boost = function_d1210(vehicle_index) * (*(real *)(definition + 0x1d0) - 1.0f) + 1.0f;
	}
	else
	{
		boost = 1.0f;
	}
	maximum_speed = *(real *)(definition + 0x1f4) * boost;
	function_ba1d0(vehicle_index, &linear_velocity, &angular_velocity);
	if (TEST_FIELD_BIT(VEHICLE_DWORD_BITS(vehicle->flags_134)->bit23))
	{
		vehicle->unknown1b8 = -1.0f;
	}
	else if (vehicle->control_flags & 2)
	{
		vehicle->unknown1b8 = 1.0f;
	}

	real elevation = vehicle->unknown1b8 *
		(0.0f > vehicle->unknown1b8 ? *(real *)(definition + 0x1f8) : *(real *)(definition + 0x1f4));

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

	real forward_mass = vehicle->unknown25c * (forward_fraction * forward_fraction);
	real up_mass = vehicle->unknown25c * (up_fraction * up_fraction);
	real left_mass = vehicle->unknown25c * (left_fraction * left_fraction);
	real length = (real)sqrt(up.k * up.k + up.j * up.j + up.i * up.i);

	if (!(0.0001f > (real)fabs(length)))
	{
		real inverse = 1.0f / length;

		up.i = inverse * up.i;
		up.j = inverse * up.j;
		up.k = inverse * up.k;
	}
	if (0.0001f > (real)fabs(length) || length == 0.0f)
	{
		up.i = 1.0f;
		up.j = 0.0f;
		up.k = 0.0f;
	}

	vector3f left;

	left.i = facing->k * up.j - facing->j * up.k;
	left.j = up.k * facing->i - facing->k * up.i;
	left.k = facing->j * up.i - up.j * facing->i;

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

		force.i += facing->i * friction;
		force.j += facing->j * friction;
		force.k += facing->k * friction;
	}
	if (vehicle->turn == 0.0f)
	{
		real friction = vehicle_friction(left_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c)) *
			state->unknown6c;

		force.i += left.i * friction;
		force.j += left.j * friction;
		force.k += left.k * friction;
	}
	if (vehicle->unknown370 == 0.0f)
	{
		real friction = vehicle_friction(up_speed, *(real *)(definition + 0x238), *(real *)(definition + 0x27c)) *
			state->unknown6c;

		force.i += up.i * friction;
		force.j += up.j * friction;
		force.k += up.k * friction;
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
	vector3f banked;

	axis.i = facing->j * up.k - facing->k * up.j;
	axis.j = facing->k * up.i - up.k * facing->i;
	axis.k = up.j * facing->i - facing->j * up.i;
	banked.i = cosine * up.i + axis.i * sine;
	banked.j = cosine * up.j + axis.j * sine;
	banked.k = cosine * up.k + axis.k * sine;

	transform4x3f current;
	transform4x3f desired;
	transform4x3f difference;
	quaternionf rotation;
	vector3f rotation_axis;
	real rotation_angle;

	current.scale = 1.0f;
	current.forward = vehicle->forward;
	current.left.i = vehicle->forward.k * vehicle->up.j - vehicle->up.k * vehicle->forward.j;
	current.left.j = vehicle->up.k * vehicle->forward.i - vehicle->forward.k * vehicle->up.i;
	current.left.k = vehicle->up.i * vehicle->forward.j - vehicle->forward.i * vehicle->up.j;
	current.up = vehicle->up;
	current.position.x = 0.0f;
	current.position.y = 0.0f;
	current.position.z = 0.0f;
	desired.scale = 1.0f;
	desired.forward = *facing;
	desired.left.i = facing->k * banked.j - facing->j * banked.k;
	desired.left.j = banked.k * facing->i - facing->k * banked.i;
	desired.left.k = facing->j * banked.i - banked.j * facing->i;
	desired.up = banked;
	desired.position.x = 0.0f;
	desired.position.y = 0.0f;
	desired.position.z = 0.0f;
	function_141590(&current, &current);
	function_142a60(&desired, &current, &difference);
	function_141f60(&difference.rotation, &rotation);
	function_11d790(&rotation, &rotation_axis, &rotation_angle);

	real rate = PIN(rotation_angle * maximum_turn_rate * 0.95492965f, 0.0f - maximum_turn_rate, maximum_turn_rate);
	vector3f torque;
	real torque_scale = angular_scale * state->unknown6c;

	torque.i = (rotation_axis.i * rate - vehicle->angular_velocity.i) * torque_scale;
	torque.j = (rotation_axis.j * rate - vehicle->angular_velocity.j) * torque_scale;
	torque.k = (rotation_axis.k * rate - vehicle->angular_velocity.k) * torque_scale;
	force.i *= vehicle->unknown25c;
	force.j *= vehicle->unknown25c;
	force.k *= vehicle->unknown25c;
	torque.i *= vehicle->unknown25c;
	torque.j *= vehicle->unknown25c;
	torque.k *= vehicle->unknown25c;
	function_2056e0(vehicle_index, state, 0.0f, &force, &torque);
}

/* a pointer to a default 2d vector (0x468774) */
point2f *g_468774;

real normalize2d(point2f *v);
real function_30bf0(vector3f *v);
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
			if (state->matrix.scale != 1.0f)
			{
				acceleration.i = state->matrix.scale * acceleration.i;
				acceleration.j = state->matrix.scale * acceleration.j;
				acceleration.k = state->matrix.scale * acceleration.k;
			}

			real scale = state->unknown6c * vehicle->unknown370;

			force.i += scale * (state->matrix.up.i * acceleration.k + state->matrix.left.i * acceleration.j +
				state->matrix.forward.i * acceleration.i);
			force.j += scale * (state->matrix.up.j * acceleration.k + state->matrix.left.j * acceleration.j +
				state->matrix.forward.j * acceleration.i);
			force.k += scale * (state->matrix.up.k * acceleration.k + state->matrix.left.k * acceleration.j +
				state->matrix.forward.k * acceleration.i);
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

	s_vehicle *current = VEHICLE_GET(vehicle_index);
	long permutation_count = current->unknown118 / 10;
	byte const *permutations = (byte *)current + current->unknown11a + permutation_count * 2;

	for (i = 0; i < *(long *)(definition + 0x2e8); i++)
	{
		byte *point_definition = *(byte **)(definition + 0x2ec) + i * 0x4c;
		short permutation;

		state->points[i].unknown9c = mass;
		if ((point_definition[4] & 1) && (permutation = *(short *)(point_definition + 0x32)) >= 0 &&
			permutation < permutation_count)
		{
			state->points[i].unknowna0 =
				*(real *)(point_definition + 0x38 + (char)permutations[permutation * 8 + 1] * 4);
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
PRIVATE inline real vehicle_flip_duration(s_vehicle const *vehicle)
{
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
	real duration = vehicle_flip_duration(vehicle);

	if (duration > elapsed)
	{
		if (vehicle->unknown350 == 1 || vehicle->unknown350 == 2)
		{
			real remaining = 1.0f - elapsed / duration;

			return 1.0f - remaining * remaining;
		}
		else
		{
			real fraction = elapsed / duration;

			fraction *= fraction;
			return fraction;
		}
	}
	return 0.0f;
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

		if (vehicle_flip_duration(vehicle) > elapsed)
		{
			result = true;
		}
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
			real direction = (real)fmod((double)(angle + 12.566371f), 6.283185307179586);

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
// @retail 0xf44a0
bool __stdcall function_f44a0(long vehicle_index, matrix3x3 *facing, s_vehicle_physics_state *state,
	matrix3x3 *result)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	real fraction = function_f4250(vehicle_index);
	bool flipping = fraction > 0.0f;
	real dot;

	facing->forward = vehicle->local_velocity;
	facing->up = *g_4687b0;
	dot = facing->up.j * facing->forward.j + facing->up.k * facing->forward.k + facing->up.i * facing->forward.i;
	facing->up.i -= facing->forward.i * dot;
	facing->up.j -= facing->forward.j * dot;
	facing->up.k -= facing->forward.k * dot;
	if (function_30bf0(&facing->up) == 0.0f)
	{
		facing->up = *g_4687a8;
	}
	vehicle_cross(&facing->up, &facing->forward, &facing->left);
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
			result->forward.i = cosine * result->forward.i + sine * result->up.i;
			result->forward.j = sine * result->up.j + cosine * result->forward.j;
			result->forward.k = sine * result->up.k + cosine * result->forward.k;
			goto rotate_up;
		}
		cosine = (real)cos(angle);
		vehicle_cross(&result->forward, &result->up, &axis);
		sine = (real)sin(angle);
rotate_up:
		result->up.i = axis.i * sine + result->up.i * cosine;
		result->up.j = result->up.j * cosine + axis.j * sine;
		result->up.k = result->up.k * cosine + axis.k * sine;
		vehicle_cross(&result->up, &result->forward, &result->left);
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
				vector3f const *forward = &vehicle->forward;

				if (0.0f > vehicle->throttle)
				{
					if (forward->i * velocity.i + forward->k * velocity.k + forward->j * velocity.j > 0.0f)
					{
						goto braking;
					}
				}
				else if (vehicle->throttle > 0.0f &&
					0.0f > velocity.i * forward->i + forward->k * velocity.k + forward->j * velocity.j)
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
	s_game_time_globals *time = g_510c54;
	long delay = vehicle_round(time->field_2_3 * 0.45f);

	if ((char)vehicle->unknown34a > delay &&
		(vehicle->unknown248 == NONE || 3.0625f > vehicle->linear_velocity.i * vehicle->linear_velocity.i +
			vehicle->linear_velocity.j * vehicle->linear_velocity.j +
			vehicle->linear_velocity.k * vehicle->linear_velocity.k))
	{
		real brake = ((char)vehicle->unknown34a * time->rate - 0.45f) * 1.25f;

		return PIN(brake, 0.0f, 1.0f);
	}
	return 0.0f;
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
				parameters.tag_index = *(long *)(definition + 0x2a0);
				parameters.unknown18 = NONE;
				parameters.object_index = NONE;
				parameters.owner.unknown4 = NONE;
				parameters.owner.unknown0 = NONE;
				parameters.owner.unknown8 = NONE;
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
// @retail 0xf62f0
void __stdcall function_f62f0(long vehicle_index)
{
	if (g_5107ee)
	{
		s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
		byte *definition = VEHICLE_DEFINITION_GET(vehicle);

		if (*(long *)(definition + 0x2a0) != NONE && vehicle->unknown25c > 0.0f)
		{
			s_object_marker markers[15];
			short count = function_b8d30(vehicle_index, 0xf0000bb, markers, 15, false);

			if (count > 0)
			{
				s_object_marker *marker = markers;
				long remaining;

				for (remaining = (word)count; remaining; remaining--, marker++)
				{
					s_vehicle_wash_collision collision;
					vector3f direction;
					vector3f vector;

					collision.unknown24 = NONE;
					random_vector_in_cone(&marker->matrix.forward, &direction, &g_4e7408->seed, 0.0f, 15.0f);
					vector = direction;
					if (function_1697c0(0x800007, &marker->matrix.position, &vector, vehicle_index, NONE,
						(s_collision_result_1697c0 *)&collision))
					{
						real strength = (1.0f - collision.t) * (0.0f - marker->matrix.forward.k) * vehicle->unknown25c;

						if (!(0.0f > strength))
						{
							if (strength > 1.0f)
							{
								strength = 1.0f;
							}
							else if (!(strength > 0.0f))
							{
								continue;
							}

							s_effect_marker effect_markers[7];
							s_effect_parameters parameters;
							point3f middle;
							vector3f facing = direction;
							real dot;

							middle.x = (marker->matrix.position.x + collision.point.x) * 0.5f;
							middle.y = (marker->matrix.position.y + collision.point.y) * 0.5f;
							middle.z = (marker->matrix.position.z + collision.point.z) * 0.5f;
							function_1201a0(&facing, g_4687bc);
							function_1763a0(&collision.point, &facing, effect_markers, &collision.normal);
							dot = (collision.normal.k * facing.k + collision.normal.j * facing.j +
								facing.i * collision.normal.i) * 2.0f;
							memset(&parameters, 0, sizeof(parameters));
							effect_markers[6].position = middle;
							parameters.unknown34 = 0;
							parameters.unknown38 = 0;
							parameters.unknown3c = 0;
							parameters.unknown30 = 0;
							parameters.source = NULL;
							parameters.tag_index = *(long *)(definition + 0x2a0);
							effect_markers[6].forward.i = facing.i - collision.normal.i * dot;
							effect_markers[6].forward.j = facing.j - collision.normal.j * dot;
							effect_markers[6].forward.k = facing.k - collision.normal.k * dot;
							parameters.color_a = 0xff808080;
							parameters.color_b = 0xff808080;
							effect_markers[6].name = 0x80000ba;
							parameters.unknown18 = NONE;
							parameters.object_index = NONE;
							parameters.owner.unknown4 = NONE;
							parameters.owner.unknown0 = NONE;
							parameters.owner.unknown8 = NONE;
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

/* a ray: a point, a vector, and the fraction along it something was hit */
struct s_vehicle_ray
{
	point3f point;
	vector3f vector;
	real t;
};

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
// @retail 0xf6670
bool __stdcall function_f6670(long vehicle_index, bool update)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
	byte *definition = VEHICLE_DEFINITION_GET(vehicle);
	s_animation_state *state = vehicle_animation_state_get(vehicle);
	bool sounded = false;

	if (!(definition[0x2ac] & 1))
	{
		long permutation_count = vehicle->unknown118 / 10;
		byte const *permutations = (byte *)vehicle + vehicle->unknown11a + permutation_count * 2;
		s_graph_iterator3c iterator;
		real largest_change = 0.0f;
		bool changed = false;

		iterator.unknown00 = NONE;
		*(real *)&iterator.unknown08 = 0.0f;
		*(real *)&iterator.unknown0c = 0.0f;
		*(real *)&iterator.unknown10 = 0.0f;
		iterator.index = NONE;
		*(short *)iterator.unknown28 = NONE;
		iterator.next_index = NONE;
		while (function_1dceb0(&iterator, graph_tag_get(state->graph_tag_index)))
		{
			long index = iterator.index;
			byte current_value = ((byte *)vehicle->unknown398)[index];
			real current = current_value == 0xff ? 1.0f : current_value * (1.0f / 255.0f);
			s_object_marker marker;

			if (function_b8d30(vehicle_index, iterator.unknown00, &marker, 1, false) == 1 && iterator.unknown00)
			{
				real offset = *(real *)&iterator.unknown08;
				real length = *(real *)&iterator.unknown0c;
				real rest = *(real *)&iterator.unknown10;
				real value;
				long i;

				for (i = 0; i < *(long *)(definition + 0x2f0); i++)
				{
					byte *override = *(byte **)(definition + 0x2f4) + i * 0x4c;

					if (*(long *)(override + 0x44) == iterator.unknown04 && *(long *)(override + 0x48) != NONE)
					{
						if ((char)permutations[*(long *)(override + 0x48) * 8 + 1] >= *(short *)(override + 0x42))
						{
							offset = *(real *)&iterator.unknown14;
							length = *(real *)&iterator.unknown18;
							rest = *(real *)&iterator.unknown1c;
						}
						break;
					}
				}

				s_vehicle_ray ray;

				ray.point.x = marker.matrix.position.x + vehicle->up.i * offset;
				ray.point.y = marker.matrix.position.y + vehicle->up.j * offset;
				ray.point.z = marker.matrix.position.z + vehicle->up.k * offset;
				ray.vector.i = vehicle->up.i * (0.0f - length);
				ray.vector.j = vehicle->up.j * (0.0f - length);
				ray.vector.k = vehicle->up.k * (0.0f - length);
				if (marker.node_index)
				{
					value = 0.0f;
				}
				else if (update && (g_510c54->game_time + index) % 4)
				{
					byte last = ((byte *)vehicle->unknown390)[index];

					value = last == 0xff ? 1.0f : last * (1.0f / 255.0f);
				}
				else
				{
					s_havok_component *component = NULL;

					if (vehicle->havok_component_index != NONE)
					{
						component = havok_component_get(vehicle->havok_component_index);
					}
					if (component && TEST_FIELD_BIT(component->flag5) && component->unknown9c &&
						function_182800(&ray, NONE, (void *)component->unknown9c))
					{
						value = PIN(1.0f - (ray.t * length - rest) / (length - rest), 0.0f, 1.0f);
					}
					else
					{
						value = function_168f40(0x8c2d, &ray, vehicle_index, NONE) ? 1.0f : 0.0f;
					}
					((byte *)vehicle->unknown390)[index] = function_11f470(0.0f, 1.0f, value);
				}
				if (value - current > largest_change)
				{
					largest_change = value - current;
				}
				if (update)
				{
					value = (value + current) * 0.5f;
				}

				byte quantized = function_11f470(0.0f, 1.0f, value);

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
	long const *reference = &limit;
	byte *definition = VEHICLE_DEFINITION_GET(VEHICLE_GET(vehicle_index));

	switch (limit)
	{
	case 0:
		return (real)fabs(*(real *)(definition + 0x1f4));
	case 1:
		return (real)fabs(*(real *)(definition + 0x1f8));
	case 2:
		return vehicle_larger(*(real *)(definition + 0x1f4), *(real *)(definition + 0x1f8));
	case 3:
		return (real)fabs(*(real *)(definition + 0x22c));
	case 4:
		return (real)fabs(*(real *)(definition + 0x230));
	case 5:
		return vehicle_larger(*(real *)(definition + 0x22c), *(real *)(definition + 0x230));
	case 6:
		return (real)fabs(*(real *)(definition + 0x204) * 0.017453292f);
	case 7:
		return (real)fabs(*(real *)(definition + 0x208) * 0.017453292f);
	case 8:
		return vehicle_larger(*(real *)(definition + 0x204) * 0.017453292f,
			*(real *)(definition + 0x208) * 0.017453292f);
	default:
		__assume(0);
	}
}

bool __stdcall function_f8070(long bit, word const *flags);

/* the vehicle object type's function values (+0x4c): its boost, brakes,
   ground contact, speeds, steering, engine and wheels; false for a name it
   does not know */
// @retail 0xf6d10
bool __stdcall function_f6d10(long vehicle_index, long name, real *value, bool *active)
{
	s_vehicle *vehicle = VEHICLE_GET(vehicle_index);
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
			(real)fabs(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x204) * 0.017453292f));
		break;
	case 0xa00000f:
		result = function_f78b0((real)fabs(vehicle->steering_angle),
			(real)fabs(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x208) * 0.017453292f));
		break;
	case 0xa00001a:
		result = function_f78b0((real)fabs(vehicle->turn), (real)fabs(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x22c)));
		break;
	case 0xa000598:
		result = function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(VEHICLE_DEFINITION_GET(vehicle)));
		break;
	case 0xb00001b:
		result = function_f78b0((real)fabs(vehicle->turn), (real)fabs(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x230)));
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
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x22c), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x230)));
		result = (result + function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(VEHICLE_DEFINITION_GET(vehicle)))) * 0.5f;
		break;
	case 0xc000582:
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
		break;
	case 0xb000597:
	case 0xc0005a5:
		if (*(short *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f0) == 1 || *(short *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f0) == 0)
		{
			result = function_f78b0(vehicle->unknown378, VEHICLE_MAX_ANGULAR_VELOCITY(VEHICLE_DEFINITION_GET(vehicle)));
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
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x204) * 0.017453292f, *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x208) * 0.017453292f));
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
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
		break;
	case 0xe00057c:
	{
		real reverse = vehicle->speed > 0.0f ? 0.0f : vehicle->speed;
		real maximum = (real)fabs(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8));

		if ((real)fabs(maximum) > 0.001f)
		{
			result = (real)fabs(reverse) / maximum;
		}
		break;
	}
	case 0xe00057d:
		result = function_f78b0((real)fabs(vehicle->turn),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x22c), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x230)));
		break;
	case 0xe000583:
		if (!(vehicle->object_flags & 0xc))
		{
			goto done;
		}
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
		break;
	case 0xf000584:
		if (!(vehicle->unknown34f > 0))
		{
			goto done;
		}
		result = function_f78b0(magnitude3d(&vehicle->linear_velocity),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
		break;
	case 0x10000585:
		result = function_f78b0((real)fabs(vehicle->forward.k * vehicle->linear_velocity.k +
			vehicle->forward.j * vehicle->linear_velocity.j + vehicle->linear_velocity.i * vehicle->forward.i),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
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
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f4), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x1f8)));
		real turning = function_f78b0((real)fabs(vehicle->turn),
			vehicle_larger(*(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x22c), *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x230)));

		result = driven > turning ? driven : turning;
		break;
	}
	case 0x13000588:
		result = function_f78b0(vehicle->unknown368, *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x20c));
		break;
	case 0x1300058a:
		result = function_f78b0((real)fabs(vehicle->speed - vehicle->steering_angle), function_f78e0(vehicle_index, 2));
		break;
	case 0x14000589:
		result = function_f78b0(vehicle->unknown36c, *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x20c));
		break;
	case 0x1400058b:
		result = function_f78b0((real)fabs(vehicle->steering_angle + vehicle->speed), function_f78e0(vehicle_index, 2));
		break;
	case 0x1700058e:
	case 0x1800058c:
	case 0x1800058f:
	case 0x1900058d:
		result = function_f78b0(vehicle->unknown360, *(real *)(VEHICLE_DEFINITION_GET(vehicle) + 0x20c));
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
		long unit_index = *(long *)(player + 0x2c);

		if (unit_index != NONE && VEHICLE_GET(unit_index)->parent_object_index == NONE)
		{
			unit_indices[count] = unit_index;
			centers[count] = VEHICLE_GET(unit_index)->center;
			count++;
		}
	}
	return count;
}

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
		s_type_f1af8e iterator;
		s_vehicle *vehicle;

		vehicle_iterator_new(&iterator);
		while ((vehicle = (s_vehicle *)function_baeb0(&iterator)) != NULL)
		{
			short i;

			for (i = 0; i < count; i++)
			{
				if (VEHICLE_GET(unit_indices[i])->parent_object_index != iterator.object_index)
				{
					real dx = vehicle->center.x - centers[i].x;
					real dy = vehicle->center.y - centers[i].y;
					real dz = vehicle->center.z - centers[i].z;

					if (100.0f > dz * dz + dy * dy + dx * dx &&
						vehicle->linear_velocity.i * vehicle->linear_velocity.i +
						vehicle->linear_velocity.j * vehicle->linear_velocity.j +
						vehicle->linear_velocity.k * vehicle->linear_velocity.k >= 1.0f)
					{
						*vehicle_index = iterator.object_index;
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* whether an occupied vehicle (one with its flag 2 at +0x10a set) is within
   ten world units of a player on foot, and which */
// @retail 0xf7ca0
bool __stdcall function_f7ca0(long *vehicle_index)
{
	long unit_indices[16];
	point3f centers[16];
	long count = vehicle_players_on_foot(unit_indices, centers);

	if (count > 0)
	{
		s_type_f1af8e iterator;
		s_vehicle *vehicle;

		vehicle_iterator_new(&iterator);
		while ((vehicle = (s_vehicle *)function_baeb0(&iterator)) != NULL)
		{
			short i;

			for (i = 0; i < count; i++)
			{
				if (VEHICLE_GET(unit_indices[i])->parent_object_index != iterator.object_index)
				{
					real dx = vehicle->center.x - centers[i].x;
					real dy = vehicle->center.y - centers[i].y;
					real dz = vehicle->center.z - centers[i].z;

					if (100.0f > dz * dz + dy * dy + dx * dx && TEST_FIELD_BIT(VEHICLE_WORD_BITS(vehicle->flags_10a)->bit2))
					{
						long seat_count = (unsigned long)vehicle->unknown120 >> 3;
						word const *seat = (word const *)((byte *)vehicle + vehicle->unknown122 + 4);
						long j;

						for (j = 0; j < seat_count; j++, seat += 4)
						{
							if (*seat & 0xfff8)
							{
								*vehicle_index = iterator.object_index;
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
matrix3x3 *function_142eb0(matrix3x3 const *a, matrix3x3 const *b, matrix3x3 *out);
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
	real climb = PIN((real)fabs(pitch), 0.0f, 0.58904862f) * dive * g_4678d8 * 1.6976527f + 1.0f;
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
		throttle = 0.001f > (real)fabs(maximum_speed) ? 0.0f : vehicle->speed / maximum_speed;
	}
	else
	{
		throttle = 0.001f > (real)fabs(*(real *)(definition + 0x1f8)) ? 0.0f :
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
			real brake = 0.0f - (real)fabs(*(real *)(definition + 0x200)) * g_4678d0;

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
			strafe_fraction = 0.001f > (real)fabs(strafe_maximum) ? 0.0f : strafe / strafe_maximum;
		}
		else
		{
			strafe_fraction = 0.001f > (real)fabs(strafe_maximum) ? 0.0f : 0.0f - strafe / strafe_maximum;
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
			real magnitude = (real)fabs(side_speed);
			real limit = g_4678d0 * *(real *)(definition + 0x238);
			real brake = 0.0f - magnitude > limit ? 0.0f - magnitude : (limit > magnitude ? magnitude : limit);

			brake = 0.0f > side_speed ? brake : 0.0f - brake;
			acceleration.i += facing.left.i * brake;
			acceleration.j += facing.left.j * brake;
			acceleration.k += facing.left.k * brake;
		}
	}

	real horizontal_speed = (real)sqrt(velocity.i * velocity.i + velocity.j * velocity.j);
	real fastest = (real)fabs(vehicle->speed);
	real other = (real)fabs(vehicle->turn) > horizontal_speed ? (real)fabs(vehicle->turn) : horizontal_speed;

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
		desired.up.i = desired.forward.i * (0.0f - desired.forward.k) + g_4687b0->i;
		desired.up.j = desired.forward.j * (0.0f - desired.forward.k) + g_4687b0->j;
		desired.up.k = (0.0f - desired.forward.k) * desired.forward.k + g_4687b0->k;
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
			(real)fabs(vehicle->speed) / *(real *)(definition + 0x1f4);
		real steering_fraction = (real)fabs(0.001f > maximum_steering ? 0.0f :
			vehicle->steering_angle / maximum_steering);
		real turn_fraction = 0.001f > *(real *)(definition + 0x22c) ? 0.0f :
			vehicle->turn / *(real *)(definition + 0x22c);
		real steering_bias = (real)fabs(steering_fraction) * 2.0f;
		real slip = velocity.j * desired.forward.i - desired.forward.j * velocity.i;
		real slip_bank = 0.0f;

		if (!(0.001f > (real)fabs(slip)))
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

	matrix3x3 difference;
	quaternionf rotation;
	vector3f axis;
	real length;
	real angle;

	function_141f60(function_142eb0(&current, &desired, &difference), &rotation);
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
	function_142eb0((matrix3x3 const *)&state->unknown3c, &rotation_matrix, &inertia);
	vehicle_transpose(&rotation_matrix);
	function_142eb0(&rotation_matrix, &inertia, &inertia);

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
