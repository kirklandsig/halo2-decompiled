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

// Keep paired projections as scalar objects until both rows are ready.
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
	volatile float rounded_impulse = impulse;
	__m128 scalar = _mm_set_ss(rounded_impulse);
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

// Borrow the dispatcher's pointer homes so the two projections do not
// require an extra vector spill between their body loads.
static __forceinline void solve_contact_pair(solver_step_31c940 *const &step, unsigned char *op,
	solver_linear_row_31e7b0 *row, solver_velocity_31c940 *a,
	solver_velocity_31c940 *const volatile &b, float *result)
{
	float target_a = row[0].linear.m128_f32[3];
	// Complete both SIMD projections before the coupled x87 errors.
	solver_scalar_31c940 projected_a = linear_velocity(row, a, b);
	solver_scalar_31c940 projected_b = linear_velocity(row + 1, a, b);
	// Retail spills the first error and both coupled impulses to 32-bit storage.
	volatile float error_a = target_a * load_error_scale(step)
		- projected_a.value * load_velocity_scale(step);
	float error_b = row[1].linear.m128_f32[3] * load_error_scale(step)
		- projected_b.value * load_velocity_scale(step);
	volatile float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 4);
	volatile float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 4);
	if (impulse_a > 0.0f)
	{
		if (impulse_b > 0.0f)
		{
			apply_linear(impulse_a, row, a, b, result);
			apply_linear(impulse_b, row + 1, a, b, result + 1);
			return;
		}
	}
	else
	{
		float candidate = error_b * row[1].a.m128_f32[3];
		// The second-row fallback accepts unordered results in retail.
		if (!(candidate <= 0.0f))
		{
			apply_linear(candidate, row + 1, a, b, result + 1);
			return;
		}
	}
	float candidate = error_a * row[0].a.m128_f32[3];
	if (candidate > 0.0f) apply_linear(candidate, row, a, b, result);
}

// Retain the coefficient and B inertia addresses across consecutive rows.
// Native scalar locals keep the projection between SSE and x87 without a copy.
static __forceinline void solve_linear_friction(float target, float limit,
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
	__m128 linear_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), row->linear);
	__m128 delta_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), row->linear);
	a->linear = _mm_add_ps(a->linear, linear_a);
	linear_b = _mm_sub_ps(linear_b, delta_b);
	__m128 angular_change_a = _mm_mul_ps(change_a, row->a);
	__m128 angular_change_b = _mm_mul_ps(change_b, row->b);
	a->angular = _mm_add_ps(a->angular, angular_change_a);
	angular_b = _mm_add_ps(angular_b, angular_change_b);
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
static __forceinline void update_angular_values(float impulse,
	__m128 const &axis_a, __m128 const &axis_b,
	__m128 &velocity_a, __m128 &velocity_b,
	__m128 const &inverse_a, __m128 const &inverse_b, float *result)
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
// overlapping row/body storage. Form both contributions before updating A.
static __forceinline void solve_linear_error_kernel(float target, float mass,
	__m128 const &axis_a, __m128 const &axis_b, __m128 const &axis_linear,
	__m128 &angular_a, __m128 &angular_b, __m128 &linear_a, __m128 &linear_b,
	__m128 const &inverse_a, __m128 const &inverse_b,
	solver_step_31c940 *const volatile &step, float *result)
{
	__m128 relative = _mm_sub_ps(linear_a, linear_b);
	__m128 angular = _mm_add_ps(_mm_mul_ps(angular_a, axis_a), _mm_mul_ps(angular_b, axis_b));
	__m128 v = _mm_add_ps(angular, _mm_mul_ps(relative, axis_linear));
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
	float projected;
	_mm_store_ss(&projected, xyz);
	float impulse = mass * (target * load_error_scale(step) - projected * load_velocity_scale(step));
	volatile float rounded = impulse;
	__m128 scalar = _mm_set_ss(rounded);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_a);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 delta_a = _mm_mul_ps(_mm_shuffle_ps(change_a, change_a, 0xff), axis_linear);
	__m128 delta_b = _mm_mul_ps(_mm_shuffle_ps(change_b, change_b, 0xff), axis_linear);
	linear_a = _mm_add_ps(linear_a, delta_a);
	linear_b = _mm_sub_ps(linear_b, delta_b);
	__m128 angular_change_a = _mm_mul_ps(change_a, axis_a);
	__m128 angular_change_b = _mm_mul_ps(change_b, axis_b);
	angular_a = _mm_add_ps(angular_a, angular_change_a);
	angular_b = _mm_add_ps(angular_b, angular_change_b);
	*result += impulse;
}

// Capture target and limit before projection, retaining the coefficient address
// across consecutive rows. Only the SIMD copy of the impulse rounds to float.
static __forceinline void solve_angular_friction_kernel(float target, float limit,
	float const *const volatile &friction_scale_pointer,
	solver_angular_row_31e7b0 *row, solver_step_31c940 *const volatile &step,
	solver_velocity_31c940 *a, __m128 &velocity_b, __m128 const &inverse_b, float *result)
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
	__m128 scalar = _mm_set_ss(impulse);
	__m128 change_a = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), a->inverse_inertia);
	__m128 change_b = _mm_mul_ps(_mm_shuffle_ps(scalar, scalar, 0), inverse_b);
	__m128 axis_a = row->a, axis_b = row->b;
	a->angular = _mm_add_ps(a->angular, _mm_mul_ps(change_a, axis_a));
	velocity_b = _mm_add_ps(velocity_b, _mm_mul_ps(change_b, axis_b));
	*result += impulse;
}

// score 2114 (was 2219): native friction projections and retained field homes.
// Register allocation, repeated broadcasts and stack layout still differ.
// @retail 0x31c940
long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses)
{
	if (!g_480c60)
	{
		function_31c7d0();
		if (!g_480c60) return 0;
	}
	for (long iteration = 0; iteration < step->iterations; ++iteration)
	{
		float saved_error = load_error_scale(step);
		float saved_velocity = load_velocity_scale(step);
		unsigned char *op = operations;
		__m128 *terms;
		solver_velocity_31c940 *a;
		solver_velocity_31c940 *volatile b;
		float *result = impulses;
		goto read_header;
		for (;;)
		{
			switch (*(signed char *)op)
			{
			case 0: goto iteration_solved;
			case 1:
			read_header:
			{
				solver_header_31c940 *header = (solver_header_31c940 *)op;
				a = header->a; b = header->b;
				// Retain the term cursor in its local home when reading a header.
				__m128 *volatile &terms_slot = terms;
				terms_slot = header->terms;
				op += sizeof(*header);
				break;
			}
			// Retail groups up to two coupled normal pairs, then checks directly
			// for the three-axis friction record. Longer runs redispatch here.
			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				{
					solve_contact_pair(step, op, row, a, b, result);
					row += 2; result += 2; op += 8;
					terms = (__m128 *)row;
				}
				if (*op != 3) break;
				{
					solve_contact_pair(step, op, row, a, b, result);
					row += 2; result += 2; op += 8;
					terms = (__m128 *)row;
				}
				if (*op == 8) goto contact_friction;
				break;
			}
			// A single unilateral linear row.
			case 2:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float error = row->linear.m128_f32[3] * load_error_scale(step)
						- linear_velocity(row, a, b).value * load_velocity_scale(step);
					if (error > 0.0f) apply_linear(error * row->a.m128_f32[3], row, a, b, result);
					++row; ++result; op += 4;
				} while (*op == 2);
				terms = (__m128 *)row;
				break;
			}
			// Two tangential rows and a spinning row share one friction limit.
			case 8:
			contact_friction:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(row + 2);
				float spin_mass = spin->a.m128_f32[3], spin_target = spin->b.m128_f32[3];
				solver_scalar_31c940 projected_a = linear_velocity(row, a, b);
				solver_scalar_31c940 projected_b = linear_velocity(row + 1, a, b);
				// Round the initial spin before the coupled tangential calculation.
				volatile float initial_spin = (spin_target * load_friction_scale(step) - angular_velocity(spin, a, b) * load_velocity_scale(step)) * spin_mass;
				float error_a = row[0].linear.m128_f32[3] * load_friction_scale(step) - projected_a.value * load_velocity_scale(step);
				float error_b = row[1].linear.m128_f32[3] * load_friction_scale(step) - projected_b.value * load_velocity_scale(step);
				float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 8);
				float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 8);
				// Reload after the tangential impulses to retain spin-squared first
				// in the x87 norm accumulation.
				float impulse_spin = initial_spin;
				float norm = impulse_spin * impulse_spin + impulse_b * impulse_b + impulse_a * impulse_a;
				float maximum = operation_float(op, 12);
				if (norm > maximum * maximum)
				{
					float factor = maximum / sqrtf(norm);
					impulse_a *= factor; impulse_b *= factor; impulse_spin *= factor;
					row[0].linear.m128_f32[3] *= factor;
					row[1].linear.m128_f32[3] *= factor;
					spin->b.m128_f32[3] *= factor;
				}
				// Scale and round spin before either linear update can change row storage.
				volatile float rounded_spin = impulse_spin * operation_float(op, 20);
				apply_linear(impulse_a, row, a, b, result);
				apply_linear(impulse_b, row + 1, a, b, result + 1);
				apply_angular((float)rounded_spin, spin, a, b, result + 2);
				terms += 8; result += 3; op += 24;
				break;
			}
			// Two tangential rows share a circular friction limit.
			case 7:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				solver_scalar_31c940 projected_a = linear_velocity(row, a, b);
				solver_scalar_31c940 projected_b = linear_velocity(row + 1, a, b);
				float error_a = row[0].linear.m128_f32[3] * load_friction_scale(step) - projected_a.value * load_velocity_scale(step);
				float error_b = row[1].linear.m128_f32[3] * load_friction_scale(step) - projected_b.value * load_velocity_scale(step);
				float maximum = operation_float(op, 12);
				// The circular limit is rounded before comparison and division.
				volatile float maximum_squared = maximum * maximum;
				float impulse_a = row[0].b.m128_f32[3] * error_a + error_b * operation_float(op, 8);
				float impulse_b = row[1].b.m128_f32[3] * error_b + error_a * operation_float(op, 8);
				// Retain the first tangential square before adding the second.
				float norm = impulse_a * impulse_a;
				norm += impulse_b * impulse_b;
				if (norm > maximum_squared)
				{
					float factor = sqrtf(maximum_squared / norm);
					impulse_a *= factor; impulse_b *= factor;
					row[0].linear.m128_f32[3] *= factor;
					row[1].linear.m128_f32[3] *= factor;
				}
				apply_linear(impulse_a, row, a, b, result);
				apply_linear(impulse_b, row + 1, a, b, result + 1);
				terms += 6; result += 2; op += 20;
				break;
			}
			// Angular friction with a scalar limit.
			case 9:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				solver_velocity_31c940 *body_b = b;
				float const *volatile friction_scale_pointer = &step->friction_scale;
				__m128 *velocity_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				do
				{
					solve_angular_friction_kernel(row->b.m128_f32[3], operation_float(op, 4), friction_scale_pointer,
						row, step, a, *velocity_b_pointer, *inverse_b_pointer, result);
					++row; ++result; op += 8;
				} while (*op == 9);
				terms = (__m128 *)row;
				break;
			}
			// Angular limits.
			case 10:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				__m128 &velocity_b = b->angular;
				do
				{
					__m128 axis_a = row->a, axis_b = row->b;
					__m128 v = _mm_add_ps(_mm_mul_ps(a->angular, axis_a), _mm_mul_ps(velocity_b, axis_b));
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(v, v, 0x55), v);
					__m128 xyz = _mm_add_ss(_mm_shuffle_ps(v, v, 0xaa), xy);
					float projected;
					_mm_store_ss(&projected, xyz);
					bool active;
					__m128 scalar;
					float impulse = select_angular_limit(projected * load_velocity_scale(step),
						row->b.m128_f32[3], operation_float(op, 4), operation_float(op, 8),
						operation_float(op, 12), row->a.m128_f32[3], active, scalar);
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
					++row; ++result; op += 16;
				} while (*op == 10);
				terms = (__m128 *)row;
				break;
			}
			// A bounded angular impulse.
			case 4:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					float mass = row->a.m128_f32[3];
					float target = row->b.m128_f32[3];
					// The wider intermediate prevents reassociation of mass and step scale
					// while retaining the extended x87 value across both multiplications.
					double mass_error = (target - angular_velocity(row, a, b)) * mass;
					float impulse = mass_error * load_error_scale(step);
					// Preserve the ratio before multiplying by the original impulse.
					// Compound assignment can cancel the lower clamp's division.
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
					apply_angular(impulse, row, a, b, result);
					++row; ++result; op += 16;
				} while (*op == 4);
				terms = (__m128 *)row;
				break;
			}
			// Angular error correction.
			case 12:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				solver_velocity_31c940 *body_b = b;
				__m128 *volatile velocity_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *velocity_a_pointer = &a->angular;
				do
				{
					float const volatile &mass_source = row->a.m128_f32[3];
					float mass = mass_source;
					float target = row->b.m128_f32[3];
					__m128 axis_a = row->a;
					__m128 axis_b = row->b;
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
					++row; ++result; op += 4;
				} while (*op == 12);
				terms = (__m128 *)row;
				break;
			}
			// Linear error correction.
			case 13:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				solver_velocity_31c940 *body_b = b;
				__m128 *volatile linear_b_pointer = &body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *linear_a_pointer = &a->linear;
				do
				{
					solve_linear_error_kernel(row->linear.m128_f32[3], row->a.m128_f32[3], row->a, row->b, row->linear,
						a->angular, *angular_b_pointer, *linear_a_pointer, *linear_b_pointer,
						a->inverse_inertia, *inverse_b_pointer, step, result);
					++row; ++result; op += 4;
				} while (*op == 13);
				terms = (__m128 *)row;
				break;
			}
			// Linear friction with a scalar limit.
			case 6:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				float const *volatile friction_scale_pointer = &step->friction_scale;
				solver_velocity_31c940 *body_b = b;
				__m128 const *volatile inverse_b_pointer = &body_b->inverse_inertia;
				__m128 *volatile linear_b_pointer = &body_b->linear;
				__m128 *volatile angular_b_pointer = &body_b->angular;
				do
				{
					solve_linear_friction(row->linear.m128_f32[3], operation_float(op, 4), friction_scale_pointer,
						row, a, *angular_b_pointer, *linear_b_pointer, inverse_b_pointer, step, result);
					++row; ++result; op += 8;
				} while (*op == 6);
				terms = (__m128 *)row;
				break;
			}
			// Independent lower and upper linear limits.
			case 11:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float velocity = linear_velocity(row, a, b).value * load_velocity_scale(step);
					float low = (row->linear.m128_f32[3] - operation_float(op, 4)) * load_error_scale(step) - velocity;
					if (low > 0.0f) apply_linear(low * row->a.m128_f32[3], row, a, b, result);
					float high = (row->linear.m128_f32[3] - operation_float(op, 8)) * load_error_scale(step) - velocity;
					if (high < 0.0f) apply_linear(high * row->a.m128_f32[3], row, a, b, result);
					++row; ++result; op += 12;
				} while (*op == 11);
				terms = (__m128 *)row;
				break;
			}
			// A bounded linear impulse.
			case 5:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					float mass = row->a.m128_f32[3];
					float target = row->linear.m128_f32[3];
					// Match the same mass-before-step-scale order as the angular path.
					double mass_error = (target - linear_velocity(row, a, b).value) * mass;
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
					apply_linear(impulse, row, a, b, result);
					row = (solver_linear_row_31e7b0 *)((s_linear_term_31c530 *)row)->offset(1); ++result; op += 16;
				} while (*op == 5);
				terms = (__m128 *)row;
				break;
			}
			case 15:
			{
				solver_step_31c940 *parameters = ((solver_step_31c940 *const volatile &)step);
				saved_velocity = parameters->velocity_scale;
				saved_error = parameters->error_scale;
				parameters->velocity_scale = operation_float(op, 8);
				parameters->error_scale = operation_float(op, 4);
				op += 12;
				break;
			}
			case 16:
				step->error_scale = saved_error; step->velocity_scale = saved_velocity;
				op += 4;
				break;
			case 14:
				(*(void (__cdecl **)(__m128 *, void *))(op + 4))(terms, op + 8);
				op += op[1];
				break;
			}
		}

	iteration_solved:
		// Save the first iteration's velocities, then accumulate later ones.
		// Retail consumes consecutive kind-2 bodies before checking the sentinel.
		solver_velocity_31c940 *body = bodies;
		if (iteration == 0)
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
		goto residual_header;
		for (;;)
		{
			switch ((signed char)op[2])
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
				do
				{
					solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
					float velocity_a = linear_velocity(row, a, b).value;
					float velocity_b = linear_velocity(row + 1, a, b).value;
					row[0].linear.m128_f32[3] -= velocity_a;
					row[1].linear.m128_f32[3] -= velocity_b;
					terms += 6;
					op += op[1];
				} while (op[2] == 5);
				break;
			}
			case 6:
			{
				do
				{
					solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
					float velocity_a = linear_velocity(row, a, b).value;
					float velocity_b = linear_velocity(row + 1, a, b).value;
					row[0].linear.m128_f32[3] -= velocity_a;
					row[1].linear.m128_f32[3] -= velocity_b;
					terms += 6;
					solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)terms;
					spin->b.m128_f32[3] -= angular_velocity(spin, a, b);
					terms += 2;
					op += op[1];
				} while (op[2] == 6);
				break;
			}
			case 3:
			{
				solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
				do
				{
					row->linear.m128_f32[3] -= linear_velocity(row, a, b).value;
					++row; op += op[1];
				} while (op[2] == 3);
				terms = (__m128 *)row;
				break;
			}
			case 4:
			{
				solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)terms;
				do
				{
					row->b.m128_f32[3] -= angular_velocity(row, a, b);
					++row; op += op[1];
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
	// stride. Contact friction records carry a separate result destination.
	unsigned char *op = operations;
	__m128 *terms;
	unsigned char *destination;
	long stride;
	float *result = impulses;
	goto read_results_header;
	for (;;)
	{
		switch (*(signed char *)op)
		{
		case 1:
		read_results_header:
		{
			solver_header_31c940 *header = (solver_header_31c940 *)op;
			terms = header->terms; destination = header->results; stride = header->result_stride;
			op += 24; break;
		}
		case 2: case 5: case 6: case 11: case 13:
		{
			*(float *)destination = *result;
			*(float *)(destination + 4) = terms[0].m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(1);
			++result; op += op[1]; break;
		}
		case 4: case 9: case 10: case 12:
		{
			*(float *)destination = *result;
			*(float *)(destination + 4) = terms[1].m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_angular_cursor_31c520 *)terms)->offset(1);
			++result; op += op[1]; break;
		}
		case 3:
		{
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)destination = result[0];
			*(float *)(destination + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			destination += stride;
			*(float *)destination = result[1];
			*(float *)(destination + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			destination += stride;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result += 2; op += 8; break;
		}
		case 7:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			long friction_stride = *(long *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			terms = (__m128 *)((s_linear_term_31c530 *)terms)->offset(2);
			result += 2; op += 20; break;
		}
		case 8:
		{
			unsigned char *friction_result = *(unsigned char **)(op + 4);
			long friction_stride = *(long *)(op + 16);
			solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)terms;
			*(float *)friction_result = result[0];
			*(float *)(friction_result + 4) = row[0].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			*(float *)friction_result = result[1];
			*(float *)(friction_result + 4) = row[1].linear.m128_f32[3] * step->inverse_time;
			friction_result += friction_stride;
			solver_angular_row_31e7b0 *spin = (solver_angular_row_31e7b0 *)(row + 2);
			*(float *)friction_result = result[2];
			*(float *)(friction_result + 4) = spin->b.m128_f32[3] * step->inverse_time;
			terms = (__m128 *)((s_angular_cursor_31c520 *)spin)->offset(1);
			destination = 0;
			result += 3; op += 24;
			break;
		}
		case 0: return 1;
		default: op += op[1]; break;
		}
	}
}
