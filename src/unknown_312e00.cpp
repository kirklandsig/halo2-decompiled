// @flags /O2 /GL-
#include "joint_builders_312e00.h"
#include <math.h>

static __forceinline __m128 scale_joint_axis(float factor, __m128 const &axis)
{
	__m128 scalar = _mm_set_ss(factor);
	return _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), axis);
}

// The unordered case follows retail into normalization, as does equality.
// Matched: each error is computed before the axis is scaled, which places the
// reciprocal temporaries in retail's stack cells.
// @retail 0x312e00
void lane_a_ragdoll::build_swing_limits(__m128 const &twist, __m128 const &cone, __m128 const &plane,
	lane_a_solver_input const *input, lane_a_solver_output *output) const
{
	{
		__m128 axis = joint_cross(twist, cone);
		float norm;
		_mm_store_ss(&norm, joint_sum_three(_mm_mul_ps(axis, axis)));
		if (!(norm < 1.1920928955078125e-7f))
		{
			solver_angular_range_31f000 range;
			float error;
			_mm_store_ss(&error, joint_sum_three(_mm_mul_ps(twist, cone)));
			range.axis = scale_joint_axis(1.0f / sqrtf(norm), axis);
			range.error = error;
			range.minimum = cone_limit;
			range.maximum = 100.0f;
			build_angular_range_31f000(&range, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		}
	}
	{
		__m128 axis = joint_cross(twist, plane);
		float norm;
		_mm_store_ss(&norm, joint_sum_three(_mm_mul_ps(axis, axis)));
		if (!(norm < 1.1920928955078125e-7f))
		{
			solver_angular_range_31f000 range;
			float error;
			_mm_store_ss(&error, joint_sum_three(_mm_mul_ps(twist, plane)));
			range.axis = scale_joint_axis(1.0f / sqrtf(norm), axis);
			range.error = error;
			range.minimum = plane_low;
			range.maximum = plane_high;
			build_angular_range_31f000(&range, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
		}
	}
}

// Matched: in the twist range the cross product is held through a reference and the
// sine is computed before the cosine.
// @retail 0x312fe0
void lane_a_ragdoll::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	function_31e770((s_ops_input_31e770 const *)input, (s_ops_cursors_31e740 *)output, (long)results, 8);
	joint_body_312e00 const *a = (joint_body_312e00 const *)body_a;
	__m128 world_a[3], world_b[3];
	rotate_joint_vectors(world_a, &pivot_a, a->transform(), 3);
	world_a[0] = _mm_add_ps(world_a[0], a->transform()->translation);
	rotate_joint_vectors(world_b, &pivot_b, ((joint_body_312e00 const *)body_b)->transform(), 3);
	world_b[0] = _mm_add_ps(world_b[0], ((joint_body_312e00 const *)body_b)->transform()->translation);
	build_swing_limits(world_a[2], world_b[2], world_b[1], input, output);
	{
		solver_angular_range_31f000 range;
		range.axis = world_a[2];
		__m128 axis = joint_normalize(_mm_add_ps(world_b[2], world_a[2]));
		__m128 perpendicular = joint_cross(axis, world_b[1]);
		__m128 reference = joint_cross(perpendicular, axis);
		__m128 &along = reference;
		range.minimum = twist_low;
		range.maximum = twist_high;
		float sine = joint_dot(perpendicular, world_a[1]);
		float cosine = joint_dot(along, world_a[1]);
		range.error = approximate_angle_3725e0(sine, cosine);
		range.axis = axis;
		build_angular_range_31f000(&range, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
	build_point_axes_31f3e0(&world_a[0], &world_b[0], (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	if (friction > 0.0f)
	{
		solver_angular_array_31e900 friction_rows;
		friction_rows.velocity = friction;
		friction_rows.axes = a->transform()->rotation;
		friction_rows.count = 3;
		friction_rows.values = (solver_axis_values_31e900 const *)&results[6];
		build_angular_array_31e900(&friction_rows, (solver_pair_31e7b0 const *)input, (solver_output_31e7b0 *)output);
	}
}
