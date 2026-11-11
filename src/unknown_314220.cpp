// @flags /O2 /GL-
#include "constraint_builders_314220.h"

// @retail 0x314220
void lane_a_ball_socket::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	build_transformed_points_31f160(
		(solver_transform_31f160 const *)constraint_transform_314220(body_a),
		(solver_transform_31f160 const *)constraint_transform_314220(body_b),
		&pivot_a, &pivot_b, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
}

// @retail 0x3149e0
void lane_a_spring::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	void *second = body_b;
	solver_linear_axis_31ec30 row;
	((c_transformed_point *)&row.point_a)->transform(constraint_transform_314220(body_a), &pivot_a);
	((c_transformed_point *)&row.point_b)->transform(constraint_transform_314220(second), &pivot_b);
	__m128 delta = _mm_sub_ps(row.point_a, row.point_b);
	__m128 squared = _mm_mul_ps(delta, delta);
	__m128 y = _mm_shuffle_ps(squared, squared, 0x55);
	y = _mm_add_ss(y, squared);
	__m128 z = _mm_shuffle_ps(squared, squared, 0xaa);
	__m128 norm = _mm_add_ss(z, y);
	__m128 inverse = _mm_rsqrt_ss(norm);
	__m128 correction = _mm_mul_ss(_mm_mul_ss(norm, inverse), inverse);
	correction = _mm_sub_ss(_mm_set_ss(3.0f), correction);
	inverse = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), inverse), correction);
	inverse = _mm_shuffle_ps(inverse, inverse, 0);
	float distance;
	_mm_store_ss(&distance, _mm_mul_ss(norm, inverse));
	row.axis = _mm_mul_ps(inverse, delta);
	build_linear_axis_31ec30(&row, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	function_31e740(length - distance, (__m128 const *)input, (s_ops_cursors_31e740 const *)output, 1);
}
