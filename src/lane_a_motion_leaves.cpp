// @flags /O2 /Ob1 /GL-
#include "lane_a_motion_leaves.h"
#include "unknown_1cec30.h"
#include "motion_step_319f70.h"
#include <float.h>
#include <string.h>

struct c_transformed_point
{
	__m128 value;
	void transform(const void *matrix, const __m128 *point);
};
struct c_component_rotation
{
	hkVector4 value;
	void set(hkRotation const &);
};
inline __m128 lane_a_broadcast(float value)
{
	__m128 single = _mm_set_ss(value);
	return _mm_shuffle_ps(single,single,0);
}
inline __m128 lane_a_world_centre(class_3193c0 const &m)
{
	c_transformed_point result;
	result.transform(&m.transform_rotation, &m.field_20);
	return result.value;
}

// @retail 0x319740
void class_3193c0::function_319740(float value)
{
	field_20.m128_f32[3] = value;
}
// @retail 0x319890
void class_3193c0::store_linear_velocity(__m128 const *value)
{
	field_40 = *value;
}
// @retail 0x3198a0
void class_3193c0::store_angular_velocity(__m128 const *value)
{
	field_50 = *value;
}
// @retail 0x319700
void class_3193c0::set_mass(float value)
{
	float inverse;
	if (value == 0.0f)
		inverse = 0.0f;
	else
		inverse = 1.0f / value;
	function_319740(inverse);
}
// @retail 0x319430
char *class_3193c0::export_state(long, long, lane_a_motion_output *out) const
{
	__m128 zero = _mm_setzero_ps();
	out->kind = 2;
	out->rows[2] = zero;
	out->rows[3] = field_70;
	out->rows[1] = field_50;
	out->rows[0] = field_40;
	out->rows[4] = zero;
	out->rows[5] = zero;
	out->rows[6] = zero;
	out->component(4, 0) = 1.0f;
	out->component(5, 1) = 1.0f;
	out->component(6, 2) = 1.0f;
	out->enabled = 1;
	return (char *)out + 0x80;
}
// @retail 0x319750
void class_3193c0::set_local_centre(__m128 const *value)
{
	float &previous_time = field_60.m128_f32[3];
	previous_time += 1000000.0f;
	float inverse_mass = field_20.m128_f32[3];
	field_20 = *value;
	field_20.m128_f32[3] = inverse_mass;
	float last = field_70.m128_f32[3];
	((c_transformed_point *)&field_70)->transform(&transform_rotation, &field_20);
	field_70.m128_f32[3] = last;
}
// @retail 0x3197a0
void class_3193c0::store_position(__m128 const *value)
{
	transform_translation = *value;
	((c_transformed_point *)&field_70)->transform(&transform_rotation, &field_20);
	field_60 = field_70;
	float &previous_time = field_60.m128_f32[3];
	previous_time += 1000000.0f;
	field_70.m128_f32[3] = 0.0f;
}
// @retail 0x3197f0
void class_3193c0::set_rotation(__m128 const *value)
{
	field_60.m128_f32[3] += 1000000.0f;
	rotation = *value;
	transform_312630 *transform = (transform_312630 *)&transform_rotation;
	((rotation_372500 *)&transform->rotation)->function_372500(&rotation);
	_mm_store_ps(transform->translation.m128_f32, transform_translation);
	// The rotation operation only changes three columns; retain translation.
	((c_transformed_point *)&field_70)->transform(transform, &field_20);
	field_70.m128_f32[3] = 0.0f;
}
// @retail 0x319840
void class_3193c0::set_position_rotation(__m128 const *position, __m128 const *orientation)
{
	transform_312630 *transform = (transform_312630 *)&transform_rotation;
	field_60.m128_f32[3] += 1000000.0f;
	rotation = *orientation;
	((rotation_372500 *)&transform->rotation)->function_372500(&rotation);
	transform->translation = *position;
	((c_transformed_point *)&field_70)->transform(transform, &field_20);
	field_70.m128_f32[3] = 0.0f;
}
void class_3193c0::store_transform(transform_312630 const *value)
{
	field_60.m128_f32[3] += 1000000.0f;
	transform_rotation.column[0]=value->rotation.column[0];
	transform_rotation.column[1]=value->rotation.column[1];
	transform_rotation.column[2]=value->rotation.column[2];
	transform_translation=value->translation;
	c_component_rotation q;
	hkRotation matrix;
	memcpy(&matrix, &value->rotation, sizeof(matrix));
	q.set(matrix);
	memcpy(&rotation, &q.value, sizeof(rotation));
	field_70=lane_a_world_centre(*this);
	field_70.m128_f32[3]=0.0f;
}
void class_3193c0::velocity_at(__m128 const *point,__m128 *out) const
{
	__m128 offset=_mm_sub_ps(*point,field_70);
	__m128 w=field_50;
	*out=_mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(w,w,0xc9),_mm_shuffle_ps(offset,offset,0xd2)),
		_mm_mul_ps(_mm_shuffle_ps(w,w,0xd2),_mm_shuffle_ps(offset,offset,0xc9)));
	*out=_mm_add_ps(*out,field_40);
}
void class_3193c0::snapshot_into(class_3193c0 *out) const
{
	out->rotation=rotation;
	out->field_40=field_40;
	out->field_50=field_50;
	out->field_60=field_60;
	out->field_70=field_70;
	out->transform_rotation.column[0]=transform_rotation.column[0];
	out->transform_rotation.column[1]=transform_rotation.column[1];
	out->transform_rotation.column[2]=transform_rotation.column[2];
	out->transform_translation=transform_translation;
}
void class_3193c0::set_source(class_3193c0 *value)
{
	source=value;
	if(value)
	{
		field_c=value->field_c;
		field_20=value->field_20;
		field_20.m128_f32[3]=0.0f;
		field_70=value->field_70;
		field_60=value->field_60;
	}
}
// Both actual retail entry points integrate the same motion; only one also
// returns the solver-output cursor. Expand the computations, not a wrapper.
#define LANE_A_INTEGRATE_COMPUTATIONS \
	field_60 = field_70; \
	as_vector_319f70(field_70).add_mul(broadcast_319f70(step->delta_time), field_40); \
	as_vector_319f70(field_60)(3) = field_c * step->delta_time * as_vector_319f70(field_50).xyz_length(); \
	{ \
		quaternion_319300 omega, product; \
		as_vector_319f70(omega.vector).set_mul(broadcast_319f70(0.5f), field_50); \
		omega.vector = _mm_and_ps(omega.vector, *(__m128 const *)g_415120); \
		product.function_319300(omega, *(quaternion_319300 const *)&rotation); \
		rotation = _mm_add_ps(rotation, _mm_mul_ps(broadcast_319f70(step->delta_time), product.vector)); \
		((s_1d8d60 *)&rotation)->function_1d8d60(); \
	} \
	transform_rotation.function_372500(&rotation); \
	{ \
		vector_2db7a0 rotated; \
		rotated.function_2db7a0(*(rotation_2db630 const *)&rotation, *(vector_2db7a0 const *)&field_20); \
		transform_translation = _mm_sub_ps(field_70, rotated.v); \
	} \
	as_vector_319f70(field_70)(3) = 1.0f;
// @retail 0x3195d0
void class_3193c0::integrate(lane_a_motion_step const *step)
{
	LANE_A_INTEGRATE_COMPUTATIONS
}
// @retail 0x319490
char *class_3193c0::integrate_output(lane_a_motion_step const *step,long,lane_a_motion_output *out)
{
	LANE_A_INTEGRATE_COMPUTATIONS
	return (char *)out+0x80;
}
#undef LANE_A_INTEGRATE_COMPUTATIONS
