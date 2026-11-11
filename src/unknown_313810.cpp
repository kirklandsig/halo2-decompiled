// @flags /O2 /GL-
#include "joint_builders_312e00.h"

// @retail 0x313810
void lane_a_limited_hinge::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	joint_body_312e00 const *a = (joint_body_312e00 const *)body_a;
	// Retail reads the body pointer again after rotating its local vectors.
	void * const volatile &second_body = body_b;
	__m128 world_a[4], world_b[3];
	rotate_joint_vectors(world_a, &pivot_a, a->transform(), 4);
	world_a[0] = _mm_add_ps(world_a[0], a->transform()->translation);
	rotate_joint_vectors(world_b, &pivot_b, ((joint_body_312e00 const *)second_body)->transform(), 3);
	world_b[0] = _mm_add_ps(world_b[0], ((joint_body_312e00 const *)second_body)->transform()->translation);
	{
		solver_angular_error_31e7b0 angular;
		angular.axis = world_a[2];
		angular.error_a = world_a[3];
		angular.error_b = world_b[1];
		build_angular_error_31e7b0(&angular, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		angular.axis = world_a[3];
		angular.error_a = world_a[2];
		angular.error_b = _mm_xor_ps(world_b[1], *(__m128 const *)joint_sign_bits);
		build_angular_error_31e7b0(&angular, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	{
		solver_angular_range_31f000 range;
		range.axis = world_a[1];
		range.minimum = angle_min;
		range.maximum = angle_max;
		float sine = joint_dot(world_b[2], world_a[2]);
		float cosine = joint_dot(joint_cross(world_b[2], world_b[1]), world_a[2]);
		range.error = approximate_angle_3725e0(sine, cosine);
		// Keep the angle continuous when crossing the +/-pi boundary.
		if (previous_angle - range.error > 3.1415927410125732f)
			range.error += 6.2831854820251465f;
		else if (range.error - previous_angle > 3.1415927410125732f)
			range.error -= 6.2831854820251465f;
		previous_angle = range.error;
		build_angular_range_31f000(&range, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	build_point_axes_31f3e0(&world_a[0], &world_b[0], (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	if (friction != 0.0f)
	{
		solver_angular_array_31e900 friction_rows;
		friction_rows.axes = &world_a[1];
		friction_rows.velocity = friction;
		friction_rows.count = 1;
		friction_rows.values = (solver_axis_values_31e900 const *)&results[6];
		build_angular_array_31e900(&friction_rows, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
}
