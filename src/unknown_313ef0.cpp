// @flags /O2 /GL-
#include "joint_builders_312e00.h"

// @retail 0x313ef0
void lane_a_hinge::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	joint_body_312e00 const *a = (joint_body_312e00 const *)body_a;
	joint_body_312e00 const *b = (joint_body_312e00 const *)body_b;
	__m128 world_a[3], world_b[2];
	rotate_joint_vectors(world_a, &pivot_a, a->transform(), 3);
	world_a[0] = _mm_add_ps(world_a[0], a->transform()->translation);
	rotate_joint_vectors(world_b, &pivot_b, b->transform(), 2);
	world_b[0] = _mm_add_ps(world_b[0], b->transform()->translation);
	{
		solver_angular_error_31e7b0 angular;
		angular.axis = world_a[1];
		angular.error_a = world_a[2];
		angular.error_b = world_b[1];
		build_angular_error_31e7b0(&angular, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		angular.axis = world_a[2];
		angular.error_a = world_a[1];
		angular.error_b = _mm_xor_ps(world_b[1], *(__m128 const *)joint_sign_bits);
		build_angular_error_31e7b0(&angular, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	build_point_axes_31f3e0(&world_a[0], &world_b[0], (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
}
