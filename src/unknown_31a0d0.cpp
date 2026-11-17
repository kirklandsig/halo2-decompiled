// @flags /O2 /GL-
// Havok (third-party physics library): rigid motions' methods built on the
// motion step (include/motion_step_319f70.h): a step from velocities the
// solver hands back, scaled, and a step back to a moment within the last
// step from the velocities kept.
#include "motion_step_319f70.h"

// one body's velocities as the solver hands them back, 0x80 bytes each
struct solver_result_31a0d0
{
	char unknown0[0x10];
	__m128 field_10;
	__m128 field_20;
	char unknown30[0x10];
	__m128 linear_velocity;
	__m128 angular_velocity;
	char unknown60[0x20];
};

class motion_31a0d0 : public motion_319f70
{
public:
	solver_result_31a0d0 const *function_31a0d0(float const *step, float scale, solver_result_31a0d0 const *result);
};

class motion_31a2b0 : public motion_319f70
{
public:
	void function_31a2b0(float time, float step_length);

	// a move by a length of time from the velocities kept, with the
	// transform's position from the quaternion
	__forceinline void move_31a2b0(float duration, float fraction)
	{
		float length = duration * fraction;
		field_60 = field_70;
		as_vector_319f70(field_70).add_mul(broadcast_319f70(length), saved_40);
		field_60.m128_f32[3] = as_vector_319f70(saved_50).xyz_length() * field_c * length;
		quaternion_319300 change;
		{
			quaternion_319300 spin;
			as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), saved_50);
			spin.vector = _mm_and_ps(spin.vector, *(__m128 const *)g_415120);
			change.function_319300(spin, *(quaternion_319300 const *)&rotation);
		}
		rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(length), change.vector));
		((s_1d8d60 *)&rotation)->function_1d8d60();
		transform_rotation.function_372500(&rotation);
		{
			vector_2db7a0 centre;
			centre.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20);
			transform_translation = _mm_sub_ps(field_70, centre.v);
		}
		as_vector_319f70(field_70)(3) = 1.0f;
	}
};

class motion_31a500 : public motion_319f70
{
public:
	solver_result_31a0d0 const *function_31a500(float const *step, float scale, solver_result_31a0d0 const *result);
};

// the velocities taken from the solver's result, scaled, then a step, and
// the velocities set to the result's next two vectors, their last lanes
// cleared; the next result is returned
// @retail 0x31a0d0
solver_result_31a0d0 const *motion_31a0d0::function_31a0d0(float const *step, float scale, solver_result_31a0d0 const *result)
{
	as_vector_319f70(field_40).set_mul(broadcast_319f70(scale), result->linear_velocity);
	as_vector_319f70(field_50).set_mul(broadcast_319f70(scale), result->angular_velocity);
	step_319f70(step);
	field_40 = result->field_10;
	field_50 = result->field_20;
	as_vector_319f70(field_40).set_and(*(__m128 const *)g_415120);
	as_vector_319f70(field_50).set_and(*(__m128 const *)g_415120);
	return result + 1;
}

// the motion moved to a moment within the last step: back by the part of the
// step its position's last lane records, then on by the part given, which
// is recorded in its place
// @retail 0x31a2b0
void motion_31a2b0::function_31a2b0(float time, float step_length)
{
	move_31a2b0(-step_length, as_vector_319f70(field_70)(3));
	move_31a2b0(time, step_length);
	as_vector_319f70(field_70)(3) = time;
}

// the velocities taken from the solver's result, scaled, their last lanes
// cleared, then a step with the transform's position from the quaternion;
// the next result is returned
// @retail 0x31a500
solver_result_31a0d0 const *motion_31a500::function_31a500(float const *step, float scale, solver_result_31a0d0 const *result)
{
	as_vector_319f70(field_40).set_mul(broadcast_319f70(scale), result->linear_velocity);
	as_vector_319f70(field_50).set_mul(broadcast_319f70(scale), result->angular_velocity);
	as_vector_319f70(field_40).set_and(*(__m128 const *)g_415120);
	as_vector_319f70(field_50).set_and(*(__m128 const *)g_415120);
	step_2db7a0(step);
	return result + 1;
}
