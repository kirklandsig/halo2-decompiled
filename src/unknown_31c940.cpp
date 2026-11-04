// @flags /O2 /GL-
// Iterative constraint solving, residual updates, and result writeback.
#include "solver_dispatch_31c940.h"
#include <math.h>

// Keep the SSE reduction in a scalar result object before x87 arithmetic.
struct solver_scalar_31c940 { float value; };

static __forceinline solver_scalar_31c940 sum_three_31c940(__m128 const &v)
{
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	solver_scalar_31c940 result;
	_mm_store_ss(&result.value, xyz);
	return result;
}

static __forceinline float angular_velocity(solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	return sum_three_31c940(_mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(b->angular, row->b))).value;
}

// Construct the scalar in its consumer's storage. This avoids the integer
// copies introduced by returning the plain scalar aggregate above.
struct solver_projection_31c940
{
	float value;
	__forceinline explicit solver_projection_31c940(__m128 const &v) { _mm_store_ss(&value, v); }
};
static __forceinline solver_projection_31c940 sum_three_projection(__m128 const &v)
{
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	return solver_projection_31c940(xyz);
}

// Paired projections retain retail's separate loads of A's velocity fields.
static __forceinline solver_projection_31c940 project_first_linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b, __m128 const &linear_axis)
{
	__m128 relative = _mm_sub_ps(_mm_load_ps((float const *)&a->linear), b->linear);
	__m128 angular = _mm_add_ps(_mm_mul_ps(b->angular, row->b), _mm_mul_ps(row->a, a->angular));
	return sum_three_projection(_mm_add_ps(angular, _mm_mul_ps(relative, linear_axis)));
}

static __forceinline solver_projection_31c940 project_second_linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	__m128 relative = _mm_sub_ps(a->linear, b->linear);
	__m128 angular = _mm_add_ps(_mm_mul_ps(_mm_load_ps((float const *)&a->angular), row->a), _mm_mul_ps(b->angular, row->b));
	return sum_three_projection(_mm_add_ps(angular, _mm_mul_ps(relative, row->linear)));
}

static __forceinline solver_scalar_31c940 linear_velocity(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	__m128 relative = _mm_sub_ps(a->linear, b->linear);
	__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(b->angular, row->b));
	return sum_three_31c940(_mm_add_ps(angular, _mm_mul_ps(relative, row->linear)));
}

static __forceinline void apply_angular(float const &impulse, solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b, float *result)
{
	// Keep the x87 impulse for accumulation; only the SIMD copy rounds here.
	float rounded_impulse;
	float volatile &rounded_store = rounded_impulse;
	rounded_store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
	// Retail captures both axes before either body update. A row can overlap
	// body storage, so updating A must not change the axis subsequently used by B.
	__m128 axis_a = row->a;
	__m128 axis_b = row->b;
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
	b->angular = _mm_add_ps(b->angular, _mm_mul_ps(change_b, axis_b));
	*result += impulse;
}

static __forceinline void apply_linear(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b, float *result)
{
	// SSE consumes a rounded float while result accumulation retains the x87
	// value. Passing the impulse by reference would round both paths early.
	float rounded_impulse;
	float volatile &store = rounded_impulse;
	store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	b->linear = _mm_sub_ps(b->linear, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	b->angular = _mm_add_ps(b->angular, angular_b);
	*result += impulse;
}

static __forceinline void apply_linear_spilled(float const volatile &source, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b, float *result)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = source;
	union { unsigned long bits; float value; } rounded;
	rounded.bits = *(unsigned long const volatile *)&source;
	__m128 scalar = _mm_set_ss(rounded.value);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 inverse_b = b->inverse_inertia;
	solver_velocity_31c940 *linear_body_b = b;
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_body_b->linear = _mm_sub_ps(linear_body_b->linear, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	solver_velocity_31c940 *angular_body_b = b;
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	angular_body_b->angular = _mm_add_ps(angular_body_b->angular, angular_b);
	*result += impulse;
}

// The first pair reloads B for inertia, then captures each update receiver.
static __forceinline void apply_first_normal_fallback(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b, float *result)
{
	// SSE consumes a rounded float while result accumulation retains the x87
	// value. Passing the impulse by reference would round both paths early.
	float rounded_impulse;
	float volatile &store = rounded_impulse;
	store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 inverse_b = b->inverse_inertia;
	solver_velocity_31c940 *linear_body_b = b;
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_body_b->linear = _mm_sub_ps(linear_body_b->linear, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	solver_velocity_31c940 *angular_body_b = b;
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	angular_body_b->angular = _mm_add_ps(angular_body_b->angular, angular_b);
	*result += impulse;
}


static __forceinline float &operation_float(unsigned char *op, long offset)
{
	return *(float *)(op + offset);
}

// Retail reloads the step pointer at these scalar reads. Return references so
// the x87 consumers retain their existing precision and evaluation boundaries.
static __forceinline float const &load_error_scale(solver_step_31c940 * const volatile &step)
{
	return step->error_scale;
}

static __forceinline float const &load_velocity_scale(solver_step_31c940 * const volatile &step)
{
	return step->velocity_scale;
}

static __forceinline float const &load_friction_scale(solver_step_31c940 * const volatile &step)
{
	return step->friction_scale;
}

// The spin block retains B's field addresses across its projections and
// tangential updates. Keep the two projections' separate A-field loads.
static __forceinline __m128 project_first_spin_vector(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b)
{
	__m128 relative = _mm_sub_ps(_mm_load_ps((float const *)&a->linear), b->linear);
	__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(b->angular, row->b));
	return _mm_add_ps(angular, _mm_mul_ps(relative, row->linear));
}

static __forceinline __m128 project_second_spin_vector(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b, __m128 const &angular_b)
{
	__m128 relative = _mm_sub_ps(a->linear, linear_b);
	__m128 angular = _mm_add_ps(_mm_mul_ps(_mm_load_ps((float const *)&a->angular), row->a), _mm_mul_ps(angular_b, row->b));
	return _mm_add_ps(angular, _mm_mul_ps(relative, row->linear));
}

// Keep the angular projection's A load distinct from the tangential rows.
static __forceinline float project_spin_angular_velocity(solver_angular_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &angular_b)
{
	return sum_three_31c940(_mm_add_ps(_mm_mul_ps(_mm_load_ps((float const *)&(a->angular)), row->a), _mm_mul_ps(angular_b, row->b))).value;
}

static __forceinline void apply_spin_linear(float impulse, solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 &linear_velocity_b, __m128 &angular_velocity_b,
	__m128 const *const volatile &inverse_b_pointer, float *result)
{
	// SSE consumes a rounded float while result accumulation retains the x87
	// value. Passing the impulse by reference would round both paths early.
	float rounded_impulse = impulse;
	__m128 input_a = _mm_set_ss(rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(input_a, input_a, 0), a->inverse_inertia);
	__m128 input_b = _mm_set_ss(rounded_impulse);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(input_b, input_b, 0), *inverse_b_pointer);
	// Capture both linear contributions before either body write.
	__m128 linear_contribution[2] =
	{
		_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear),
		_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear)
	};
	a->linear = _mm_add_ps(a->linear, linear_contribution[0]);
	linear_velocity_b = _mm_sub_ps(linear_velocity_b, linear_contribution[1]);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	// Capture both angular contributions before either body write.
	__m128 angular_contribution[2] =
	{
		_mm_mul_ps(change_a, row->a),
		_mm_mul_ps(change_b, row->b)
	};
	a->angular = _mm_add_ps(a->angular, angular_contribution[0]);
	angular_velocity_b = _mm_add_ps(angular_velocity_b, angular_contribution[1]);
	*result += impulse;
}

// Retain the first row's unshifted cursor alongside the second row's B axis.
// Their separate uses keep retail's cursor bias over the same row storage.
static __forceinline void solve_contact_pair(solver_step_31c940 *const &step, unsigned char *op,
	__m128 *axis, __m128 *first_axis, solver_velocity_31c940 *a,
	solver_velocity_31c940 *const volatile &b, float *result)
{
	solver_step_31c940 *parameters = step;
	float target_a = axis[-5].m128_f32[3];
	solver_projection_31c940 projected_a = project_first_linear_velocity(((solver_linear_row_31e7b0 *)(axis - 5)), a, b, *first_axis);
	// Give the first error its float home before projecting the second row.
	volatile float error_a = target_a * parameters->error_scale
		- projected_a.value * parameters->velocity_scale;
	solver_projection_31c940 projected_b = project_second_linear_velocity(((solver_linear_row_31e7b0 *)(axis - 2)), a, b);
	float error_b = axis[-2].m128_f32[3] * parameters->error_scale
		- projected_b.value * parameters->velocity_scale;
	float const volatile &mass_a_source = axis[-3].m128_f32[3];
	volatile float impulse_a = mass_a_source * error_a + error_b * operation_float(op, 4);
	// Capture the second mass before its coupled product.
	float const volatile &mass_b_source = axis[0].m128_f32[3];
	float mass_b = mass_b_source;
	volatile float impulse_b = mass_b * error_b + error_a * operation_float(op, 4);
	// A rejected second-row candidate joins the same first-row fallback as a failed
	// positive pair. Keep the positive pair before the second-row fallback.
	do
	{
		if (impulse_a > 0.0f)
		{
			if (impulse_b > 0.0f)
			{
				apply_linear_spilled(impulse_a, ((solver_linear_row_31e7b0 *)first_axis), a, b, result);
				apply_linear_spilled(impulse_b, ((solver_linear_row_31e7b0 *)(axis - 2)), a, b, result + 1);
				return;
			}
			break;
		}
		float candidate_b = error_b * axis[-1].m128_f32[3];
		// Unordered candidates enter the second-row update, as in retail.
		if (candidate_b <= 0.0f) break;
		apply_first_normal_fallback(candidate_b, ((solver_linear_row_31e7b0 *)(axis - 2)), a, b, result + 1);
		return;
	} while (false);
	float candidate_a = error_a * axis[-4].m128_f32[3];
	if (candidate_a > 0.0f) apply_first_normal_fallback(candidate_a, ((solver_linear_row_31e7b0 *)first_axis), a, b, result);
}

// The second unrolled pair keeps B field addresses and its first linear axis.
// Its two updates snapshot the angular-B address at different points.
static __forceinline void apply_second_pair_fallback(float impulse,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a, __m128 &linear_velocity_b,
	__m128 &angular_velocity_b, __m128 const &inverse_b, __m128 const &axis_linear, float *result)
{
	// SSE consumes a rounded float while result accumulation retains the x87
	// value. Passing the impulse by reference would round both paths early.
	float rounded_impulse;
	float volatile &store = rounded_impulse;
	store = impulse;
	__m128 scalar = _mm_load_ss(&rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_velocity_b = _mm_sub_ps(linear_velocity_b, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	angular_velocity_b = _mm_add_ps(angular_velocity_b, angular_b);
	*result += impulse;
}

// Read the stored float for x87 consumers; SIMD transfers can copy its bits.
static __forceinline float load_stored_impulse(float const &value)
{
	float const volatile &stored=value;
	return stored;
}

static __forceinline void apply_second_pair_first_impulse(float const &source,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a, __m128 &linear_velocity_b,
	__m128 *const volatile &angular_b_pointer, __m128 const &inverse_b, __m128 const &axis_linear,
	float *result)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = load_stored_impulse(source);
	union
	{
		unsigned long bits;
		float value;
	} rounded;
	rounded.bits = *(unsigned long const *)&source;
	__m128 scalar = _mm_set_ss(rounded.value);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_velocity_b = _mm_sub_ps(linear_velocity_b, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_a);
	__m128 *angular_b_address = angular_b_pointer;
	*angular_b_address = _mm_add_ps(*angular_b_address, angular_b);
	*result += impulse;
}

static __forceinline void apply_second_pair_second_impulse(float const &source,
	solver_linear_row_31e7b0 const *row, solver_velocity_31c940 *a, __m128 &linear_velocity_b,
	__m128 *const volatile &angular_b_pointer, __m128 const &inverse_b, float *result)
{
	// These normal-pair impulses have already been spilled to float. Load that
	// value for accumulation and copy its bits into the SIMD scalar.
	float impulse = load_stored_impulse(source);
	union
	{
		unsigned long bits;
		float value;
	} rounded;
	rounded.bits = *(unsigned long const *)&source;
	__m128 scalar = _mm_set_ss(rounded.value);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 linear_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_velocity_b = _mm_sub_ps(linear_velocity_b, linear_b);
	// Reload the axes after both linear writes, then form both contributions
	// before either angular write. Row storage may overlap the body fields.
	__m128 angular_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_b = _mm_mul_ps(change_b, row->b);
	__m128 *angular_b_address = angular_b_pointer;
	a->angular = _mm_add_ps(a->angular, angular_a);
	*angular_b_address = _mm_add_ps(*angular_b_address, angular_b);
	*result += impulse;
}

static __forceinline solver_projection_31c940 project_linear_axis(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, solver_velocity_31c940 *b, __m128 const &axis_linear)
{
	__m128 relative = _mm_sub_ps(_mm_load_ps((float const *)&a->linear), b->linear);
	// Retain the linear and angular contributions before reduction.
	__m128 projection_contribution[3] =
	{
		_mm_mul_ps(relative, axis_linear),
		_mm_mul_ps(a->angular, row->a),
		_mm_mul_ps(_mm_load_ps((float const *)&b->angular), row->b)
	};
	__m128 angular = _mm_add_ps(projection_contribution[1], projection_contribution[2]);
	return sum_three_projection(_mm_add_ps(angular, projection_contribution[0]));
}

static __forceinline solver_projection_31c940 project_second_pair_second_row(solver_linear_row_31e7b0 const *row,
	solver_velocity_31c940 *a, __m128 const &linear_b, __m128 const &angular_b, __m128 const &axis_linear)
{
	__m128 relative = _mm_sub_ps(a->linear, linear_b);
	__m128 projection_contribution[2] =
	{
		_mm_mul_ps(a->angular, row->a),
		_mm_mul_ps(angular_b, row->b)
	};
	__m128 angular = _mm_add_ps(projection_contribution[0], projection_contribution[1]);
	return sum_three_projection(_mm_add_ps(angular, _mm_mul_ps(relative, axis_linear)));
}

static __forceinline void solve_second_contact_pair(solver_step_31c940 *const &step,
	unsigned char *op, __m128 *terms, solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b,
	float *result)
{
	solver_velocity_31c940 *body_b = b;
	__m128 *linear_b_pointer = &body_b->linear;
	__m128 *volatile angular_b_pointer = &body_b->angular;
	__m128 first_linear_axis = terms[0];
	float target_a = terms[0].m128_f32[3];
	// Complete both SIMD projections before the coupled x87 errors.
	solver_projection_31c940 projected_a = project_linear_axis(((solver_linear_row_31e7b0 *)terms),
		a, body_b, first_linear_axis);
	solver_projection_31c940 projected_b = project_second_pair_second_row(((solver_linear_row_31e7b0 *)(terms + 3)),
		a, *linear_b_pointer, *angular_b_pointer, terms[3]);
	solver_step_31c940 *parameters = step;
	// Retail spills the first error and both coupled impulses to 32-bit storage.
	volatile float error_a = target_a * parameters->error_scale
		- projected_a.value * parameters->velocity_scale;
	float error_b = terms[3].m128_f32[3] * parameters->error_scale
		- projected_b.value * parameters->velocity_scale;
	float const volatile &mass_a_source = terms[2].m128_f32[3];
	float mass_a = mass_a_source;
	mass_a *= error_a;
	float impulse_a = mass_a + error_b * operation_float(op, 4);
	// Capture the second mass before its coupled product.
	float const volatile &mass_b_source = terms[5].m128_f32[3];
	float mass_b = mass_b_source;
	mass_b *= error_b;
	float impulse_b = mass_b + error_a * operation_float(op, 4);
	// Keep the first-row fallback in both source paths. The compiler merges
	// that tail while retaining retail's unordered second-row branch direction.
	if (load_stored_impulse(impulse_a) > 0.0f)
	{
		if (load_stored_impulse(impulse_b) > 0.0f)
		{
			__m128 const *inverse_b_pointer = &b->inverse_inertia;
			apply_second_pair_first_impulse(impulse_a,
				((solver_linear_row_31e7b0 *)terms), a, *linear_b_pointer, angular_b_pointer, *inverse_b_pointer,
				first_linear_axis, result);
			apply_second_pair_second_impulse(impulse_b,
				((solver_linear_row_31e7b0 *)(terms + 3)), a, *linear_b_pointer, angular_b_pointer,
				*inverse_b_pointer, result + 1);
			return;
		}
		{
			float candidate_a = error_a * terms[1].m128_f32[3];
			if (candidate_a > 0.0f) apply_second_pair_fallback(candidate_a,
				((solver_linear_row_31e7b0 *)terms), a, *linear_b_pointer, *angular_b_pointer, b->inverse_inertia,
				first_linear_axis, result);
			return;
		}
	}
	else
	{
		float candidate_b = error_b * terms[4].m128_f32[3];
		if (candidate_b <= 0.0f)
		{
			float candidate_a = error_a * terms[1].m128_f32[3];
			if (candidate_a > 0.0f) apply_second_pair_fallback(candidate_a,
				((solver_linear_row_31e7b0 *)terms), a, *linear_b_pointer, *angular_b_pointer, b->inverse_inertia,
				first_linear_axis, result);
			return;
		}
		else
		{
			apply_second_pair_fallback(candidate_b,
				((solver_linear_row_31e7b0 *)(terms + 3)), a, *linear_b_pointer, *angular_b_pointer,
				b->inverse_inertia, terms[3], result + 1);
			return;
		}
	}
}


// Retain the coefficient and B inertia addresses across consecutive rows.
// Native scalar locals keep the projection between SSE and x87 without a copy.
static __forceinline void solve_linear_friction(float target, float const volatile &limit,
	float const *const volatile &friction_scale_pointer,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a,
	__m128 &angular_b, __m128 &linear_b, __m128 const *const volatile &inverse_b_pointer,
	solver_step_31c940 *const volatile &step, float *result)
{
	__m128 relative = _mm_sub_ps(a->linear, linear_b);
	__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(angular_b, row->b));
	__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, row->linear));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	float impulse = (target * (*friction_scale_pointer)
		- projected * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor; row->linear.m128_f32[3] *= factor;
	}
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), *inverse_b_pointer);
	// Capture both linear contributions before either body write.
	__m128 linear_contribution[2] =
	{
		_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear),
		_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear)
	};
	a->linear = _mm_add_ps(a->linear, linear_contribution[0]);
	linear_b = _mm_sub_ps(linear_b, linear_contribution[1]);
	// Capture both angular contributions before either body write.
	__m128 angular_contribution[2] =
	{
		_mm_mul_ps(change_a, row->a),
		_mm_mul_ps(change_b, row->b)
	};
	a->angular = _mm_add_ps(a->angular, angular_contribution[0]);
	angular_b = _mm_add_ps(angular_b, angular_contribution[1]);
	*result += impulse;
}

// Select the active angular limit before the shared body-update tail. The
// lower error is stored to float before multiplication; the upper error keeps
// the extended x87 value. The scalar result is rounded only for SIMD use.
static __forceinline float select_angular_limit(double velocity, float const &target,
	float const &lower, float const &upper, float const &gain, float const &mass,
	bool &active, __m128 &scalar)
{
	float error = (target - lower) * gain - velocity;
	if (error > 0.0f)
	{
		float const volatile &positive_error = error;
		active = true;
		float impulse = positive_error * mass;
		scalar = _mm_set_ss(impulse);
		return impulse;
	}
	error = (target - upper) * gain - velocity;
	if (error < 0.0f)
	{
		active = true;
		float impulse = error * mass;
		scalar = _mm_set_ss(impulse);
		return impulse;
	}
	active = false;
	return 0.0f;
}

// Convert only the SIMD update to float; retain the x87 impulse for results.
// Capture B's inverse inertia before either body update, including aliases.
static __forceinline void update_angular_values(float impulse,
	__m128 const &axis_a, __m128 const &axis_b,
	__m128 &velocity_a, __m128 &velocity_b,
	__m128 const &inverse_a, __m128 inverse_b, float *result)
{
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_a);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	velocity_a = _mm_add_ps(velocity_a, _mm_mul_ps(change_a, axis_a));
	velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
	*result += impulse;
}

// Target and mass are captured before projection. Axes remain references so
// the angular contributions observe the preceding linear writes, including
// overlapping row/body storage. Capture B inertia before the body writes, and
// form both angular contributions before updating A.
static __forceinline void solve_linear_error_kernel(float target, float mass,
	__m128 const &axis_a, __m128 const &axis_b, __m128 const &axis_linear,
	__m128 &angular_a, __m128 &angular_b, __m128 &linear_a, float *linear_b,
	__m128 const &inverse_a, __m128 const &inverse_b,
	solver_step_31c940 *const volatile &step, float *result)
{
	__m128 relative = _mm_sub_ps(linear_a, _mm_load_ps(linear_b));
	__m128 angular = _mm_add_ps(_mm_mul_ps(angular_a, axis_a), _mm_mul_ps(angular_b, axis_b));
	__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, axis_linear));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	float impulse = mass * (target * load_error_scale(step) - projected * load_velocity_scale(step));
	// Give the rounded SIMD input a scalar object; keep the extended impulse
	// live for result accumulation. Retail reads this copy directly with MOVSS.
	solver_scalar_31c940 rounded;
	float volatile &store_rounded = rounded.value;
	store_rounded = impulse;
	__m128 scalar = _mm_load_ss(&rounded.value);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_a);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 delta_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 delta_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	linear_a = _mm_add_ps(linear_a, delta_a);
	_mm_store_ps(linear_b, _mm_sub_ps(_mm_load_ps(linear_b), delta_b));
	__m128 angular_change_a = _mm_mul_ps(change_a, axis_a);
	__m128 angular_change_b = _mm_mul_ps(change_b, axis_b);
	angular_a = _mm_add_ps(angular_a, angular_change_a);
	angular_b = _mm_add_ps(angular_b, angular_change_b);
	*result += impulse;
}

// Capture target and limit before projection, retaining the coefficient address
// across consecutive rows. Keep B's inertia pointer through the clamp and read
// its value when forming the update. Only the SIMD copy rounds to float.
static __forceinline void solve_angular_friction_kernel(float target, float const &limit,
	float const *const volatile &friction_scale_pointer,
	solver_angular_row_31e7b0 *row, solver_step_31c940 *const volatile &step,
	solver_velocity_31c940 *a, __m128 &velocity_b, __m128 const *const volatile &inverse_b_pointer, float *result)
{
	__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(velocity_b, row->b));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);

	float impulse = (target * (*friction_scale_pointer)
		- projected * load_velocity_scale(step)) * row->a.m128_f32[3];
	float magnitude = (float)fabs(impulse);
	if (magnitude > limit)
	{
		float factor = limit / magnitude;
		impulse *= factor; row->b.m128_f32[3] *= factor;
	}
	// Round the SIMD input once, then construct each body's broadcast separately.
	float rounded_impulse = impulse;
	__m128 input_a = _mm_set_ss(rounded_impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(input_a, input_a, 0), a->inverse_inertia);
	__m128 input_b = _mm_set_ss(rounded_impulse);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(input_b, input_b, 0), (*inverse_b_pointer));
	__m128 axis_a = row->a, axis_b = row->b;
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
	velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
	*result += impulse;
}

// Keep B's field addresses across consecutive bounded angular rows. Capture
// both axes before either angular write; only the SIMD impulse rounds here.
static __forceinline void solve_bounded_angular(float mass, float target,
	solver_angular_row_31e7b0 *row, solver_velocity_31c940 *a,
	float *velocity_b, __m128 const *const volatile &inverse_b_pointer,
	float const &upper, float const &lower, solver_step_31c940 *const volatile &step, float *result)
{
	__m128 axis_a = row->a, axis_b = row->b;
	__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, axis_a), _mm_mul_ps(_mm_load_ps(velocity_b), axis_b));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	// Keep the mass multiplication before the step scale, without an early
	// float spill. Both clamps retain their divide-then-multiply sequence.
	double mass_error = (target - projected) * mass;
	float impulse = mass_error * load_error_scale(step);
	if (impulse > upper)
	{
		float ratio = upper / impulse;
		impulse = ratio * impulse;
	}
	else if (impulse < lower)
	{
		float ratio = lower / impulse;
		impulse = ratio * impulse;
	}
	__m128 scalar = _mm_set_ss(impulse);
	// Retain both body weights before either angular velocity write.
	__m128 weights[2];
	weights[0] = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	weights[1] = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), *inverse_b_pointer);
	// Reload A after projection for the bounded angular update.
	a->angular = _mm_add_ps(_mm_load_ps((float const *)&a->angular), _mm_mul_ps(weights[0], axis_a));
	_mm_store_ps(velocity_b, _mm_add_ps(_mm_load_ps(velocity_b),
		_mm_mul_ps(weights[1], axis_b)));
	*result += impulse;
}

// Preserve the captured linear axis through projection and update. The angular
// axes reload after the linear stores, as required when rows overlap bodies.
static __forceinline void solve_unilateral_linear(float target,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b,
	__m128 &linear_b, __m128 &angular_b, solver_step_31c940 *const volatile &step, float *result)
{
	__m128 axis_linear = row->linear;
	__m128 relative = _mm_sub_ps(a->linear, linear_b);
	__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, row->a), _mm_mul_ps(angular_b, row->b));
	__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, axis_linear));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	solver_step_31c940 *parameters = step;
	float error = target * parameters->error_scale - projected * parameters->velocity_scale;
	if (error > 0.0f)
	{
		float impulse = error * row->a.m128_f32[3];
		__m128 scalar = _mm_set_ss(impulse);
		__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
		__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
		// Form both linear contributions before reloading and writing either body.
		__m128 linear_delta[2] =
		{
			_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear),
			_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear)
		};
		// Reload A for the update instead of retaining its projection load.
		a->linear = _mm_add_ps(_mm_load_ps((float const *)&a->linear), linear_delta[0]);
		linear_b = _mm_sub_ps(linear_b, linear_delta[1]);
		__m128 angular_a = _mm_mul_ps(change_a, row->a);
		__m128 angular_second = _mm_mul_ps(change_b, row->b);
		a->angular = _mm_add_ps(a->angular, angular_a);
		angular_b = _mm_add_ps(angular_b, angular_second);
		*result += impulse;
	}
}

static __forceinline void update_linear_limit_values(float impulse, solver_linear_row_31e7b0 const *row,
 solver_velocity_31c940 *a, solver_velocity_31c940 *const volatile &b,
 float *linear_b, __m128 &angular_b, float *result)
{
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), b->inverse_inertia);
	// Capture both linear contributions before either body write.
	__m128 linear_contribution[2] =
	{
		_mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear),
		_mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear)
	};
	a->linear = _mm_add_ps(a->linear, linear_contribution[0]);
	_mm_store_ps(linear_b, _mm_sub_ps(_mm_load_ps(linear_b), linear_contribution[1]));
	// Capture both angular contributions before either body write.
	__m128 angular_contribution[2] =
	{
		_mm_mul_ps(change_a, row->a),
		_mm_mul_ps(change_b, row->b)
	};
	a->angular = _mm_add_ps(a->angular, angular_contribution[0]);
	angular_b = _mm_add_ps(angular_b, angular_contribution[1]);
	*result += impulse;
}

static __forceinline void update_residual_pair(__m128 *&cursor,float *&result,solver_velocity_31c940 *a,solver_velocity_31c940 *b)
{
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)cursor;
	// Form the two angular sums before completing their linear projections.
	__m128 second = a->angular;
	__m128 work = row[0].a;
	__m128 other = b->linear;
	__m128 axis = row[0].b;
	__m128 first = second;
	first = _mm_mul_ps(first, work);
	work = row[1].a;
	second = _mm_mul_ps(second, work);
	work = a->linear;
	work = _mm_sub_ps(work, other);
	other = b->angular;
	__m128 product = other;
	__m128 relative_for_first = work;
	product = _mm_mul_ps(product, axis);
	axis = row[1].b;
	other = _mm_mul_ps(other, axis);
	axis = row[0].linear;
	second = _mm_add_ps(second, other);
	first = _mm_add_ps(first, product);
	product = row[1].linear;
	work = _mm_mul_ps(work, product);
	second = _mm_add_ps(second, work);
	// Reduce the second row first; its scalar stays live through the first store.
	work = _mm_shuffle_ps(second, second, 0x55);
	work = _mm_add_ss(work, second);
	other = _mm_shuffle_ps(second, second, 0xaa);
	second = _mm_add_ss(other, work);
	relative_for_first = _mm_mul_ps(relative_for_first, axis);
	first = _mm_add_ps(first, relative_for_first);
	work = _mm_shuffle_ps(first, first, 0x55);
	work = _mm_add_ss(work, first);
	other = _mm_shuffle_ps(first, first, 0xaa);
	other = _mm_add_ss(other, work);
	__m128 projected_a = other;
	// Round both projections before publishing either remaining error.
	float scalar_b;
	_mm_store_ss(&scalar_b, second);
	float scalar_a;
	_mm_store_ss(&scalar_a, projected_a);
	cursor += 6;
	result += 2;
	cursor[-6].m128_f32[3] -= scalar_a;
	cursor[-3].m128_f32[3] -= scalar_b;
}

static __forceinline void subtract_angular_residual(solver_angular_row_31e7b0 *row,solver_velocity_31c940 *a,solver_velocity_31c940 *b)
{
	__m128 axis_a = row->a, axis_b = row->b;
	__m128 va = a->angular, vb = b->angular;
	__m128 v = _mm_add_ps(_mm_mul_ps(va, axis_a), _mm_mul_ps(vb, axis_b));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	row->b.m128_f32[3] -= projected;
}

// Solver state stays together through the iterations. Result publication then
// reuses its storage for the result stride and current operation kind.
union solver_phase_state_31c940
{
	struct
	{
		solver_velocity_31c940 *volatile a_home;
		float saved_velocity;
		long iteration;
		float saved_error;
		float spin_limit;
		volatile float linear_limit;
		float angular_limit;
	} solving;
	struct
	{
		long stride;
		volatile int kind;
	} writing;
};

static __forceinline float const &capture_writeback_time(solver_step_31c940 *const volatile &step)
{
	return step->inverse_time;
}
// Main solver reconstruction; the ordinary checker determines its current score.
// SSE scheduling, scalar homes, repeated broadcasts and control flow still differ.
// @retail 0x31c940
long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses)
{
	solver_phase_state_31c940 state;

	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60) return 0;
	}
	for (state.solving.iteration = 0; state.solving.iteration < step->iterations; ++state.solving.iteration)
	{
		solver_step_31c940 *initial_parameters = step;
		state.solving.saved_error = initial_parameters->error_scale;
		state.solving.saved_velocity = initial_parameters->velocity_scale;
		unsigned char *op = operations;
		__m128 *terms;
		solver_velocity_31c940 *a;

		solver_velocity_31c940 *volatile b;
		float *result = impulses;
		goto read_header;
		for (;;)
		{
			__m128 *normal_axes = terms + 5;
					primary_axes_ready:
			switch (*(signed char *)op)
			{
			case 0: goto iteration_solved;
			case 1:
			read_header:
			{
				solver_header_31c940 *header = (solver_header_31c940 *)op;
				a = header->a; state.solving.a_home = a; b = header->b;
				// Keep one term cursor across the row kinds and restore A after calls.
				terms = header->terms;
				op += sizeof(*header);
				break;
			}
			// Retail groups up to two coupled normal pairs, then checks directly
			// for the three-axis friction record. Longer runs redispatch here.
			case 3:
			{
				{
					solve_contact_pair(step, op, normal_axes, terms, a, b, result);
					terms += 6; normal_axes += 6; result += 2; op += 8;

				}
				if (*op != 3) goto primary_axes_ready;
				{
					solve_second_contact_pair(step, op, terms, a, b, result);
					terms += 6; normal_axes += 6; result += 2; op += 8;

				}
				if (*op == 8) goto contact_friction;
				break;
			}
			// A single unilateral linear row.
			case 2:
			{
				solver_velocity_31c940 *body_b = b;
				__m128 *linear_b_pointer = &body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				do
				{
					solve_unilateral_linear(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), a, b,
						*linear_b_pointer, *angular_b_pointer, step, result);
					terms += 3; ++result; op += 4;
				} while (*op == 2);

				break;
			}
			// Two tangential rows and a spinning row share one friction limit.
			case 8:
			contact_friction:
			{
				solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(((solver_linear_row_31e7b0 *)terms) + 2);
				solver_velocity_31c940 *body_b = b;
				__m128 *volatile linear_b_pointer = &body_b->linear;
				__m128 *angular_b_pointer = &body_b->angular;
				float spin_mass = spin->a.m128_f32[3], spin_target = spin->b.m128_f32[3];
				solver_projection_31c940 projected_a = sum_three_projection(project_first_spin_vector(
					((solver_linear_row_31e7b0 *)terms), a, body_b));
				solver_projection_31c940 projected_b = sum_three_projection(project_second_spin_vector(
					((solver_linear_row_31e7b0 *)terms) + 1, a, *linear_b_pointer, *angular_b_pointer));
				// Round the initial spin before the coupled tangential calculation.
				volatile float initial_spin = (spin_target * load_friction_scale(step) - project_spin_angular_velocity(spin, a, *angular_b_pointer) * load_velocity_scale(step)) * spin_mass;
				float error_a = ((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3] * load_friction_scale(step) - projected_a.value * load_velocity_scale(step);
				float error_b = ((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] * load_friction_scale(step) - projected_b.value * load_velocity_scale(step);
				float impulse_a = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 8);
				float impulse_b = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 8);
				// Reload after the tangential impulses to retain spin-squared first
				// in the x87 norm accumulation.
				float impulse_spin = initial_spin;
				float norm = impulse_spin * impulse_spin + impulse_b * impulse_b + impulse_a * impulse_a;
				state.solving.spin_limit = operation_float(op, 12);
				if (norm > state.solving.spin_limit * state.solving.spin_limit)
				{
					float factor = state.solving.spin_limit / sqrtf(norm);
					impulse_a *= factor; impulse_b *= factor; impulse_spin *= factor;
					((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3] *= factor;
					((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] *= factor;
					spin->b.m128_f32[3] *= factor;
				}
				// Scale and round spin before either linear update can change ((solver_linear_row_31e7b0 *)terms) storage.
				volatile float rounded_spin = impulse_spin * operation_float(op, 20);
				__m128 const *volatile inverse_b_pointer = &b->inverse_inertia;
				apply_spin_linear(impulse_a, ((solver_linear_row_31e7b0 *)terms), a,
					*linear_b_pointer, *angular_b_pointer, inverse_b_pointer, result);
				apply_spin_linear(impulse_b, ((solver_linear_row_31e7b0 *)terms) + 1, a,
					*linear_b_pointer, *angular_b_pointer, inverse_b_pointer, result + 1);
				apply_angular((float)rounded_spin, spin, a, b, result + 2);
				terms += 8; result += 3; op += 24;
				break;
			}
			// Two tangential rows share a circular friction limit.
			case 7:
			{
				float target_a = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
				solver_scalar_31c940 projected_a = linear_velocity(((solver_linear_row_31e7b0 *)terms), a, b);
				solver_scalar_31c940 projected_b = linear_velocity(((solver_linear_row_31e7b0 *)terms) + 1, a, b);
				float error_a = target_a * load_friction_scale(step) - projected_a.value * load_velocity_scale(step);
				float error_b = ((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] * load_friction_scale(step) - projected_b.value * load_velocity_scale(step);
				float maximum = operation_float(op, 12);
				// Round the circular limit to its float home before comparison and
				// division; the reads can then use that home directly.
				float maximum_squared;
				float volatile &maximum_store = maximum_squared;
				maximum_store = maximum * maximum;
				// Capture each mass before its coupled product. These product locals
				// retain the extended x87 value until addition to the diagonal term.
				float const volatile &mass_a_source = ((solver_linear_row_31e7b0 *)terms)[0].b.m128_f32[3];
				float mass_a = mass_a_source;
				float coupled_a = error_b * operation_float(op, 8);
				float impulse_a = mass_a * error_a + coupled_a;
				float const volatile &mass_b_source = ((solver_linear_row_31e7b0 *)terms)[1].b.m128_f32[3];
				float mass_b = mass_b_source;
				float coupled_b = error_a * operation_float(op, 8);
				float impulse_b = mass_b * error_b + coupled_b;
				// Retain the first tangential square before adding the second.
				float norm = impulse_a * impulse_a;
				norm += impulse_b * impulse_b;
				if (norm > maximum_squared)
				{
					float factor = sqrtf(maximum_squared / norm);
					impulse_a *= factor; impulse_b *= factor;
					((solver_linear_row_31e7b0 *)terms)[0].linear.m128_f32[3] *= factor;
					((solver_linear_row_31e7b0 *)terms)[1].linear.m128_f32[3] *= factor;
				}
				apply_linear(impulse_a, ((solver_linear_row_31e7b0 *)terms), a, b, result);
				apply_linear(impulse_b, ((solver_linear_row_31e7b0 *)terms) + 1, a, b, result + 1);
				terms += 6; result += 2; op += 20;
				break;
			}
			// Angular friction with a scalar limit.
			case 9:
			{
				solver_velocity_31c940 *body_b = b;
				float const *volatile friction_scale_pointer = &step->friction_scale;
				__m128 *velocity_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				do
				{
					state.solving.angular_limit = operation_float(op, 4);
					solve_angular_friction_kernel(((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3], state.solving.angular_limit, friction_scale_pointer,
						((solver_angular_row_31e7b0 *)terms), step, a, *velocity_b_pointer, inverse_b_pointer, result);
					terms += 2; ++result; op += 8;
				} while (*op == 9);

				break;
			}
			// Angular limits.
			case 10:
			{
				__m128 &velocity_b = b->angular;
				do
				{
					__m128 axis_a = ((solver_angular_row_31e7b0 *)terms)->a, axis_b = ((solver_angular_row_31e7b0 *)terms)->b;
					__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, axis_a), _mm_mul_ps(velocity_b, axis_b));
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
					__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
					float projected;
					_mm_store_ss(&projected, xyz);
					bool active;
					__m128 scalar;
					float impulse = select_angular_limit(projected * load_velocity_scale(step),
						((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3], operation_float(op, 4), operation_float(op, 8),
						operation_float(op, 12), ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3], active, scalar);
					if (active)
					{
						__m128 inertia_a = a->inverse_inertia;
						__m128 inertia_second = b->inverse_inertia;
						__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_a);
						a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
						__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inertia_second);
						velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
						*result += impulse;
					}
					terms += 2; ++result; op += 16;
				} while (*op == 10);

				break;
			}
			// A bounded angular impulse.
			case 4:
			{
				solver_velocity_31c940 *body_b = b;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				float *velocity_b_pointer = (float *)&body_b->angular;
				do
				{
					float const volatile &mass_source = ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3];
					float mass = mass_source;
					float target = ((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3];
					solve_bounded_angular(mass, target, ((solver_angular_row_31e7b0 *)terms), a, velocity_b_pointer, inverse_b_pointer,
						operation_float(op, 4), operation_float(op, 8), step, result);
					terms += 2; ++result; op += 16;
				} while (*op == 4);

				break;
			}
			// Angular error correction.
			case 12:
			{
				solver_velocity_31c940 *body_b = b;
				__m128 *volatile velocity_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *velocity_a_pointer = &a->angular;
				do
				{
					float const volatile &mass_source = ((solver_angular_row_31e7b0 *)terms)->a.m128_f32[3];
					float mass = mass_source;
					float target = ((solver_angular_row_31e7b0 *)terms)->b.m128_f32[3];
					__m128 axis_a = ((solver_angular_row_31e7b0 *)terms)->a;
					__m128 axis_b = ((solver_angular_row_31e7b0 *)terms)->b;
					__m128 weighted_a = _mm_mul_ps(*velocity_a_pointer, axis_a);
					__m128 weighted_b = _mm_mul_ps(*velocity_b_pointer, axis_b);
					__m128 weighted = _mm_add_ps(weighted_a, weighted_b);
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(weighted, weighted, 0x55), weighted);
					__m128 xyz = _mm_add_ss(_mm_shuffle_ps(weighted, weighted, 0xaa), xy);
					float projected;
					_mm_store_ss(&projected, xyz);
					solver_step_31c940 *current_step = step;
					float impulse = mass * (target * current_step->error_scale - projected * current_step->velocity_scale);
					update_angular_values(impulse, axis_a, axis_b, *velocity_a_pointer, *velocity_b_pointer,
						a->inverse_inertia, *inverse_b_pointer, result);
					terms += 2; ++result; op += 4;
				} while (*op == 12);

				break;
			}
			// Linear error correction.
			case 13:
			{
				solver_velocity_31c940 *body_b = b;
				float *volatile linear_b_pointer = (float *)&body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *linear_a_pointer = &a->linear;
				do
				{
					solve_linear_error_kernel(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3], ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms)->a, ((solver_linear_row_31e7b0 *)terms)->b, ((solver_linear_row_31e7b0 *)terms)->linear,
						a->angular, *angular_b_pointer, *linear_a_pointer, linear_b_pointer,
						a->inverse_inertia, *inverse_b_pointer, step, result);
					terms += 3; ++result; op += 4;
				} while (*op == 13);

				break;
			}
			// Linear friction with a scalar limit.
			case 6:
			{
				float const *volatile friction_scale_pointer = &step->friction_scale;
				solver_velocity_31c940 *body_b = b;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *volatile linear_b_pointer = &body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				do
				{
					state.solving.linear_limit = operation_float(op, 4);
					solve_linear_friction(((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3], state.solving.linear_limit, friction_scale_pointer,
						((solver_linear_row_31e7b0 *)terms), a, *angular_b_pointer, *linear_b_pointer, inverse_b_pointer, step, result);
					terms += 3; ++result; op += 8;
				} while (*op == 6);

				break;
			}
			// Independent lower and upper linear limits.
			case 11:
			{
				solver_velocity_31c940 *body_b = b;
				// Retain the linear field address across both bound updates.
				float *linear_b_pointer = (float *)&body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				do
				{
					__m128 relative = _mm_sub_ps(_mm_load_ps((float const *)&a->linear),
						_mm_load_ps(linear_b_pointer));
					__m128 angular = _mm_add_ps(_mm_mul_ps(a->angular, ((solver_linear_row_31e7b0 *)terms)->a), _mm_mul_ps(*angular_b_pointer, ((solver_linear_row_31e7b0 *)terms)->b));
					__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, ((solver_linear_row_31e7b0 *)terms)->linear));
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
					__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
					float projected;
					_mm_store_ss(&projected, xyz);
					float velocity = projected * load_velocity_scale(step);
					float low = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 4)) * load_error_scale(step) - velocity;
					if (low > 0.0f)
						update_linear_limit_values(low * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), a, b, linear_b_pointer, *angular_b_pointer, result);
					float high = (((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3] - operation_float(op, 8)) * load_error_scale(step) - velocity;
					if (high < 0.0f)
						update_linear_limit_values(high * ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3], ((solver_linear_row_31e7b0 *)terms), a, b, linear_b_pointer, *angular_b_pointer, result);
					terms += 3; ++result; op += 12;
				} while (*op == 11);

				break;
			}
			// A bounded linear impulse.
			case 5:
			{
				do
				{
					solver_velocity_31c940 *body_b = b;
					// Capture the mass before projection, retaining its x87 value.
					float const volatile &mass_source = ((solver_linear_row_31e7b0 *)terms)->a.m128_f32[3];
					float mass = mass_source;
					__m128 *linear_b_pointer = &body_b->linear;
					__m128 *angular_b_pointer = &body_b->angular;
					float target = ((solver_linear_row_31e7b0 *)terms)->linear.m128_f32[3];
					// Match the same mass-before-step-scale order as the angular path.
					__m128 relative = _mm_sub_ps(a->linear, *linear_b_pointer);
					__m128 angular = _mm_add_ps(
						_mm_mul_ps(a->angular, ((solver_linear_row_31e7b0 *)terms)->a),
						_mm_mul_ps(*angular_b_pointer, ((solver_linear_row_31e7b0 *)terms)->b));
					__m128 axis = ((solver_linear_row_31e7b0 *)terms)->linear;
					__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, axis));
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
					__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
					float projected;
					_mm_store_ss(&projected, xyz);
					double mass_error = (target - projected) * mass;
					float impulse = mass_error * load_error_scale(step);
					// Keep the divide followed by multiply on both clamp paths;
					// reassociating them changes rounding at 24-bit x87 precision.
					if (impulse > operation_float(op, 4))
					{
						float ratio = operation_float(op, 4) / impulse;
						impulse = ratio * impulse;
					}
					else if (impulse < operation_float(op, 8))
					{
						float ratio = operation_float(op, 8) / impulse;
						impulse = ratio * impulse;
					}
					apply_linear(impulse, ((solver_linear_row_31e7b0 *)terms), a, b, result);
					terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
					a = state.solving.a_home; ++result; op += 16;
				} while (*op == 5);

				break;
			}
			case 15:
			{
				solver_step_31c940 *parameters = ((solver_step_31c940 *const volatile &)step);
				state.solving.saved_velocity = parameters->velocity_scale;
				state.solving.saved_error = parameters->error_scale;
				parameters->velocity_scale = operation_float(op, 8);
				parameters->error_scale = operation_float(op, 4);
				op += 12;
				break;
			}
			case 16:
				step->error_scale = state.solving.saved_error; step->velocity_scale = state.solving.saved_velocity;
				op += 4;
				break;
			case 14:
				(*(void (__cdecl **)(__m128 *, void *))(op + 4))(terms, op + 8);
				op += op[1];
			a = state.solving.a_home;
				break;
			}
		}

	iteration_solved:
		// Save the first iteration's velocities, then accumulate later ones.
		// Retail consumes consecutive kind-2 bodies before checking the sentinel.
		solver_velocity_31c940 *body = bodies;
		if (state.solving.iteration == 0)
		{
			for (;;)
			{
				switch ((signed char)body->kind)
				{
				case 3: goto bodies_done_0;
				case 2:
					do
					{
						body->accumulated_angular = body->angular;
						body->accumulated_linear = body->linear;
						++body;
					} while (body->kind == 2);
					break;
				}
			}
			bodies_done_0:;
		}
		else
		{
			for (;;)
			{
				switch ((signed char)body->kind)
				{
				case 3: goto bodies_done_1;
				case 2:
					do
					{
						body->accumulated_angular = _mm_add_ps(body->accumulated_angular, body->angular);
						body->accumulated_linear = _mm_add_ps(body->accumulated_linear, body->linear);
						++body;
					} while (body->kind == 2);
					break;
				}
			}
			bodies_done_1:;
		}
		// The third operation byte classifies the residual row layout.
		{
		// This pass keeps B in a register across consecutive residual rows.
		solver_velocity_31c940 *b;
		op = operations;
		// Retail advances the impulse cursor with each residual row, while
		// this pass writes the remaining errors back into the row stream.
		result = impulses;
		signed char residual_kind;
		goto residual_header;
		for (;;)
		{
			residual_kind = (signed char)op[2];
		residual_dispatch:
			switch (residual_kind)
			{
			case 0: goto iteration_done;
			case 1:
			residual_header:
			{
				solver_header_31c940 *header = (solver_header_31c940 *)op;
				a = header->a; b = header->b; terms = header->terms; op += 24;
				break;
			}
			case 5:
			{
				// Consume up to two paired records, then join the friction path.
				update_residual_pair(terms, result, a, b); op += op[1];
				// Redispatch the classification already read for this pair.
				residual_kind = (signed char)op[2];
				if (residual_kind != 5) goto residual_dispatch;
				update_residual_pair(terms, result, a, b); op += op[1];
				if (op[2] != 6) break;
			}
			case 6:
			{
				{
					update_residual_pair(terms, result, a, b);
					solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)terms;
					subtract_angular_residual(spin, a, b);
					terms += 2;
					++result;
					op += op[1];
				}
				// The header opcode takes precedence over its classification byte.
				if (*op == 1) goto residual_header;
				break;
			}
			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					row->linear.m128_f32[3] -= project_linear_axis(row, a, b, row->linear).value;
					++row; ++result; op += op[1];
				} while (op[2] == 3);
				terms = (__m128 *)row;
				break;
			}
			case 4:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					subtract_angular_residual(row,a,b);
					++row; ++result; op += op[1];
				} while (op[2] == 4);
				terms = (__m128 *)row;
				break;
			}
			case 2: op += op[1]; break;
			}
		}
		}
	iteration_done:;
	}

	// Publish impulses and remaining error using each header's destination and
	// result stride. Contact friction records carry a separate result destination.
	float *result = impulses;
	unsigned char *op = operations;
	__m128 *terms;
	unsigned char *destination;
	goto read_results_header;
	for (;;)
	{
		// Retain the dispatch value while publishing its home for repeated rows.
		int current_kind = *(signed char *)op;
		state.writing.kind = current_kind;
		switch (current_kind)
		{
		case 1:
		read_results_header:
		{
			solver_header_31c940 *header = (solver_header_31c940 *)op;
			// Capture all header fields before updating the phase state.
			long captured_stride = header->result_stride;
			terms = header->terms;
			destination = header->results;
			state.writing.stride = captured_stride;
			op += 24; break;
		}
		case 3:
		{
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;

			// Keep each stride read in retail order and retain the coefficient
			// address through both outputs, which may overlap the input buffers.
			float first_impulse = result[0];
			long const volatile &stride_source = state.writing.stride;
			long first_stride = stride_source;
			*(float *)destination = first_impulse;
			float const &inverse_time = capture_writeback_time(step);
			*(float *)(destination + 4) = row[0].linear.m128_f32[3] * inverse_time;
			destination += first_stride;
			float *next_result = result + 1;
			*(float *)destination = *next_result;
			long const volatile &stride_again = state.writing.stride;
			long second_stride = stride_again;
			*(float *)(destination + 4) = row[1].linear.m128_f32[3] * inverse_time;
			destination += second_stride;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result = next_result + 1; op += 8; break;
		}
		case 2: case 5: case 6: case 11: case 13:
		{
			// Keep the coefficient address across the cursor calls.
			float const *volatile inverse_time_pointer = &step->inverse_time;
			do
			{
				*(float *)destination = *result;
				*(float *)(destination + 4) = terms[0].m128_f32[3] * *inverse_time_pointer;
				destination += state.writing.stride;
				terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
				++result; op += op[1];
			} while (*(signed char *)op == state.writing.kind);
			break;
		}
		case 4: case 9: case 10: case 12:
		{
			float const *volatile inverse_time_pointer = &step->inverse_time;
			do
			{
				*(float *)destination = *result;
				*(float *)(destination + 4) = terms[1].m128_f32[3] * *inverse_time_pointer;
				destination += state.writing.stride;
				terms = (__m128 *)((s_angular_cursor_31c520 *)terms)->offset(1);
				++result; op += op[1];
			} while (*(signed char *)op == state.writing.kind);
			break;
		}
		case 7:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			// Earlier output may replace this word; read it after each store.
			long const volatile &friction_stride = *(long const volatile *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			float const *volatile inverse_time_pointer = &step->inverse_time;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * *inverse_time_pointer;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result += 2; op += 20; break;
		}
		case 8:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			// Earlier output may replace this word; read it after each store.
			long const volatile &friction_stride = *(long const volatile *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			float const *volatile inverse_time_pointer = &step->inverse_time;
			destination = 0;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * *inverse_time_pointer;

			friction_result += friction_stride;

			*(float *)friction_result = result[2];
			*(float *)(friction_result + 4) = terms[7].m128_f32[3] * *inverse_time_pointer;
			terms = (__m128 *)((s_angular_cursor_31c520 *)(terms + 6))->offset(1);

			result += 3; op += 24;
			break;
		}
		case 0: return 1;
		case 14: case 15: case 16: op += op[1]; break;
		}
	}
}
