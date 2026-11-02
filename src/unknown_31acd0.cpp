// @flags /O2 /GL-
#include "force_motion_31acd0.h"
#include <stddef.h>

typedef char force_motion_size[sizeof(force_motion_31acd0) == 0x110 ? 1 : -1];
typedef char force_motion_force[offsetof(force_motion_31acd0, force) == 0xf0 ? 1 : -1];
typedef char force_motion_torque[offsetof(force_motion_31acd0, torque) == 0x100 ? 1 : -1];

// @retail 0x31acd0
void force_motion_31acd0::add_force_at(__m128 const &value, __m128 const &point)
{
	force = _mm_add_ps(force, value);
	__m128 const arm = _mm_sub_ps(point, field_70);
	__m128 const applied = value;
	__m128 const moment = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(arm, arm, 0xc9), _mm_shuffle_ps(applied, applied, 0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(arm, arm, 0xd2), _mm_shuffle_ps(applied, applied, 0xc9)));
	torque = _mm_add_ps(torque, moment);
	as_vector_319f70(torque)(3) = 1.0f;
}

// Rewind the stored sweep fraction, then advance to the requested fraction.
// The double step keeps the scalar product in x87 while the vector updates
// use its float broadcast. The angle multiplies length by field_c first.
// Separate quaternion and centre scopes let both phases reuse vector slots.
// @retail 0x31b700
void force_motion_31acd0::interpolate_sweep(float fraction, float delta_time)
{
	{
		double step = -delta_time * as_vector_319f70(field_70)(3);
		field_60 = field_70;
		as_vector_319f70(field_70).add_mul(broadcast_319f70(step), saved_linear);
		as_vector_319f70(field_60)(3) = as_vector_319f70(saved_angular).xyz_length() * field_c * step;
		{
			quaternion_319300 spin;
			as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), saved_angular);
			spin.vector = _mm_and_ps(spin.vector, *(__m128 const *)g_415120);
			quaternion_319300 change;
			change.function_319300(spin, *(quaternion_319300 const *)&rotation);
			rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(step), change.vector));
			((s_1d8d60 *)&rotation)->function_1d8d60();
		}
		transform_rotation.function_372500(&rotation);
		{
			vector_2db7a0 centre;
			centre.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20);
			transform_translation = _mm_sub_ps(field_70, centre.v);
		}
	}
	as_vector_319f70(field_70)(3) = 1.0f;
	{
		double step = fraction * delta_time;
		field_60 = field_70;
		as_vector_319f70(field_70).add_mul(broadcast_319f70(step), saved_linear);
		as_vector_319f70(field_60)(3) = as_vector_319f70(saved_angular).xyz_length() * field_c * step;
		{
			quaternion_319300 spin;
			as_vector_319f70(spin.vector).set_mul(broadcast_319f70(0.5f), saved_angular);
			spin.vector = _mm_and_ps(spin.vector, *(__m128 const *)g_415120);
			quaternion_319300 change;
			change.function_319300(spin, *(quaternion_319300 const *)&rotation);
			rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(step), change.vector));
			((s_1d8d60 *)&rotation)->function_1d8d60();
		}
		transform_rotation.function_372500(&rotation);
		{
			vector_2db7a0 centre;
			centre.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20);
			transform_translation = _mm_sub_ps(field_70, centre.v);
		}
	}
	as_vector_319f70(field_70)(3) = fraction;
}
