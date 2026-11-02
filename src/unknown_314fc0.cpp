// @flags /O2 /GL-
#include "constraint_builders_314220.h"

// Three angular rows hold the frame together, two linear rows keep the
// pivots on the sliding axis, followed by travel limits and optional friction.
// @retail 0x314fc0
void lane_a_prismatic::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	void *second = body_b;
	void *first = body_a;
	c_query_transform const *second_frame = constraint_transform_314220(second);
	{
		s_2da8c0 frame;
		frame.function_2da8c0((hkRotation const *)constraint_transform_314220(first), (hkRotation const *)frame_b);
		solver_angular_error_31e7b0 angular;
		angular.error_a = frame.field_0.m_col0.m_quad;
		angular.error_b = second_frame->axes[1];
		angular.axis = frame.field_0.m_col2.m_quad;
		build_angular_error_31e7b0(&angular, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		solver_angular_error_31e7b0 other;
		other.error_a = frame.field_0.m_col1.m_quad;
		other.error_b = second_frame->axes[2];
		other.axis = angular.error_a;
		build_angular_error_31e7b0(&other, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		other.axis = other.error_a;
		other.error_a = angular.axis;
		other.error_b = second_frame->axes[0];
		build_angular_error_31e7b0(&other, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	c_transformed_point point_a, point_b;
	point_a.transform(constraint_transform_314220(first), &pivot_a);
	point_b.transform(constraint_transform_314220(second), &pivot_b);
	vector_2da6e0 axis;
	axis.rotate(constraint_transform_314220(second), &axis_b);
	__m128 product = _mm_mul_ps(_mm_sub_ps(point_a.value, point_b.value), axis.value);
	__m128 y = _mm_shuffle_ps(product, product, 0x55);
	__m128 z = _mm_shuffle_ps(product, product, 0xaa);
	__m128 projection = _mm_add_ss(z, _mm_add_ss(y, product));
	{
		solver_linear_axis_31ec30 linear;
		linear.point_a = point_a.value;
		linear.point_b = _mm_add_ps(point_b.value, _mm_mul_ps(_mm_shuffle_ps(projection, projection, 0), axis.value));
		((vector_2da6e0 *)&linear.axis)->rotate(constraint_transform_314220(second), &perpendicular_b);
		build_linear_axis_31ec30(&linear, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		__m128 a = linear.axis, b = axis.value;
		__m128 ac = _mm_shuffle_ps(a, a, 0xc9);
		__m128 bd = _mm_shuffle_ps(b, b, 0xd2);
		__m128 first_product = _mm_mul_ps(bd, ac);
		__m128 ad = _mm_shuffle_ps(a, a, 0xd2);
		__m128 bc = _mm_shuffle_ps(b, b, 0xc9);
		linear.axis = _mm_sub_ps(_mm_mul_ps(bc, ad), first_product);
		build_linear_axis_31ec30(&linear, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	{
		solver_linear_range_31ee10 range;
		range.point_a = point_a.value;
		range.point_b = point_b.value;
		range.minimum = minimum;
		range.maximum = maximum;
		range.axis = axis.value;
		build_linear_range_31ee10(&range, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	if (friction > 0.0f)
	{
		solver_linear_velocity_31ea90 velocity;
		velocity.point = point_a.value;
		velocity.axis = axis.value;
		velocity.velocity = friction;
		velocity.result = &results[6].impulse;
		build_linear_velocity_31ea90(&velocity, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
}
