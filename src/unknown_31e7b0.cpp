// @flags /O2 /GL-
// Havok solver row construction: transform constraint axes into each body's
// space, compute inverse effective mass, and append Jacobians and operations.
#include "solver_rows_31e7b0.h"
#include "lane_b_constraint_dependencies.h"
#include <math.h>

__declspec(align(16)) static unsigned long const solver_sign_bits[4] =
	{0x80000000, 0x80000000, 0x80000000, 0x80000000};

static inline __m128 rotated_axis(__m128 const &axis, solver_body_31e7b0 const *body)
{
	__m128 z = _mm_mul_ps(body->rotation[2], _mm_shuffle_ps(axis, axis, 0xaa));
	__m128 y = _mm_mul_ps(body->rotation[1], _mm_shuffle_ps(axis, axis, 0x55));
	__m128 x = _mm_mul_ps(body->rotation[0], _mm_shuffle_ps(axis, axis, 0));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

static inline __m128 body_axis(__m128 const &axis, solver_body_31e7b0 const *body)
{
	__m128 result = axis;
	if (!body->axes_already_local)
		result = rotated_axis(result, body);
	return result;
}

static inline __m128 cross_axis(__m128 const &a, __m128 const &b)
{
	__m128 reverse = _mm_mul_ps(_mm_shuffle_ps(a, a, 0xd2), _mm_shuffle_ps(b, b, 0xc9));
	__m128 forward = _mm_mul_ps(_mm_shuffle_ps(a, a, 0xc9), _mm_shuffle_ps(b, b, 0xd2));
	return _mm_sub_ps(forward, reverse);
}

static inline float sum_three(__m128 const &value)
{
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(value, value, 0x55), value);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(value, value, 0xaa), xy);
	float result;
	_mm_store_ss(&result, xyz);
	return result;
}

static inline __m128 angular_diagonal_terms(__m128 const &a, __m128 const &b,
	solver_body_31e7b0 const *body_a, solver_body_31e7b0 const *body_b)
{
	__m128 b_squared = _mm_mul_ps(b, b);
	__m128 b_weighted = _mm_mul_ps(b_squared, body_b->inverse_inertia);
	__m128 a_squared = _mm_mul_ps(a, a);
	__m128 a_weighted = _mm_mul_ps(a_squared, body_a->inverse_inertia);
	return _mm_add_ps(a_weighted, b_weighted);
}

// Retail keeps this helper inline in the conditional row-construction path.
static __forceinline void store_body_axis(__m128 &result, __m128 const &axis, solver_body_31e7b0 const *body)
{
	if (body->axes_already_local)
		result = axis;
	else
		result = rotated_axis(axis, body);
}

// A single angular error row and its four-byte operation.
// score 0 (was 70): write the first axis through the output-reference helper
// and let the diagonal helper reload both stored axes. An explicit first-axis
// reload changes SIMD temporaries and pointer allocation in this builder.
// @retail 0x31e7b0
void __cdecl build_angular_error_31e7b0(solver_angular_error_31e7b0 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)output->terms;
	store_body_axis(row->a, input->axis, body_a);
	if (body_b->axes_already_local)
	{
		row->b = _mm_xor_ps(input->axis, *(__m128 const *)solver_sign_bits);
	}
	else
	{
		__m128 negative_axis = _mm_xor_ps(input->axis, *(__m128 const *)solver_sign_bits);
		__m128 result_b = rotated_axis(negative_axis, body_b);
		row->b = result_b;
	}
	row->a.m128_f32[3] = 1.0f / (sum_three(angular_diagonal_terms(row->a, row->b, body_a, body_b)) + 1.1920928955078125e-7f);
	float error = -(input->error_b.m128_f32[2] * input->error_a.m128_f32[2]
		+ input->error_b.m128_f32[1] * input->error_a.m128_f32[1]
		+ input->error_b.m128_f32[0] * input->error_a.m128_f32[0]) * pair->error_scale;
	unsigned long *op = output->operations;
	*op = 0x4040c;
	output->terms = (__m128 *)(row + 1);
	output->operations = op + 1;
	row->b.m128_f32[3] = error;
}

// One row per supplied axis, with a scalar operation value for each row.
// score 0 (was 15): retain the input pointer on the stack through a reference,
// and initialize the output cursors before the values cursor. This reproduces
// the initial cursor spill and the scalar multiply/store scheduling.
// @retail 0x31e900
void __cdecl build_angular_array_31e900(solver_angular_array_31e900 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_angular_array_31e900 const * const &input_ref = input;
	__m128 const *axis = input_ref->axes;
	solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)output->terms;
	solver_scalar_op_31e7b0 *op = (solver_scalar_op_31e7b0 *)output->operations;
	solver_axis_values_31e900 const *value = input_ref->values;
	for (long count = input_ref->count; --count >= 0; ++op, ++axis, ++row, ++value)
	{
		solver_body_31e7b0 const *body_a = pair->a;
		solver_body_31e7b0 const *body_b = pair->b;
		store_body_axis(row->a, *axis, body_a);
		if (body_b->axes_already_local)
		{
			row->b = _mm_xor_ps(*axis, *(__m128 const *)solver_sign_bits);
		}
		else
		{
			__m128 negative_axis = _mm_xor_ps(*axis, *(__m128 const *)solver_sign_bits);
			row->b = rotated_axis(negative_axis, body_b);
		}
		row->a.m128_f32[3] = 1.0f / (sum_three(angular_diagonal_terms(row->a, row->b, body_a, body_b)) + 1.1920928955078125e-7f);
		float velocity = pair->velocity_scale * input_ref->velocity;
		op->opcode = 0x40809;
		op->value = velocity;
		row->b.m128_f32[3] = value->error * pair->error_scale;
	}
	output->terms = (__m128 *)row;
	output->operations = (unsigned long *)op;
}

// An angular row with lower/upper impulse bounds and half inverse mass.
// score 0 (was 108): use the output-reference first-axis transform, initialize
// body B after the output cursors, and compute both bounds before storing them.
// This retains retail's pointer allocation and x87 calculation/store order.
// @retail 0x31f000
void __cdecl build_angular_range_31f000(solver_angular_range_31f000 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_angular_row_31e7b0 *row = (solver_angular_row_31e7b0 *)output->terms;
	solver_range_op_31e7b0 *op = (solver_range_op_31e7b0 *)output->operations;
	solver_body_31e7b0 const *body_b = pair->b;
	store_body_axis(row->a, input->axis, body_a);
	if (body_b->axes_already_local)
	{
		row->b = _mm_xor_ps(input->axis, *(__m128 const *)solver_sign_bits);
	}
	else
	{
		__m128 negative_axis = _mm_xor_ps(input->axis, *(__m128 const *)solver_sign_bits);
		__m128 result_b = rotated_axis(negative_axis, body_b);
		row->b = result_b;
	}
	row->a.m128_f32[3] = 1.0f / (sum_three(angular_diagonal_terms(row->a, row->b, body_a, body_b)) + 1.1920928955078125e-7f);
	float scale = -pair->error_scale;
	float minimum = scale * input->minimum;
	float maximum = scale * input->maximum;
	op->opcode = 0x4100a;
	op->minimum = minimum;
	op->maximum = maximum;
	op->softness = 0.5f;
	output->operations = (unsigned long *)(op + 1);
	output->terms = (__m128 *)(row + 1);
	row->b.m128_f32[3] = scale * input->error;
	row->a.m128_f32[3] *= 0.5f;
}

// A linear velocity row at a common point, retaining its diagonal as well
// as its reciprocal for the solver's velocity operation.
// score 0 (was 8): keep the first-axis reload in each body-space branch and
// inline the transform helper. Hoisting the reload changes pointer allocation;
// allowing an out-of-line helper introduces a call absent from retail.
// @retail 0x31ea90
void __cdecl build_linear_velocity_31ea90(solver_linear_velocity_31ea90 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	row->linear = input->axis;
	__m128 relative_a = _mm_sub_ps(input->point, body_a->center);
	__m128 relative_b = _mm_sub_ps(input->point, body_b->center);
	__m128 angular_a = cross_axis(relative_a, input->axis);
	store_body_axis(row->a, angular_a, body_a);
	__m128 angular_b = cross_axis(input->axis, relative_b);
	__m128 result_a;
	if (body_b->axes_already_local)
	{
		result_a = _mm_load_ps((float const *)&row->a);
		row->b = angular_b;
	}
	else
	{
		__m128 result_b;
		store_body_axis(result_b, angular_b, body_b);
		result_a = _mm_load_ps((float const *)&row->a);
		row->b = result_b;
	}
	float diagonal = body_b->inverse_inertia.m128_f32[3] + body_a->inverse_inertia.m128_f32[3]
		+ 1.1920928955078125e-7f;
	float angular = sum_three(angular_diagonal_terms(result_a, row->b, body_a, body_b));
	diagonal += angular;
	row->b.m128_f32[3] = diagonal;
	row->a.m128_f32[3] = 1.0f / diagonal;
	float bias = input->result[1] * pair->error_scale;
	solver_scalar_op_31e7b0 *op = (solver_scalar_op_31e7b0 *)output->operations;
	op->value = input->velocity * pair->velocity_scale;
	op->opcode = 0x30806;
	output->terms = (__m128 *)(row + 1);
	output->operations = (unsigned long *)(op + 1);
	row->linear.m128_f32[3] = bias;
}

// Constrain the projected separation between two world-space points.
// score 0 (was 7): explicitly reload the first axis before storing the second.
// An ordinary vector copy retains the old value across the second transform;
// the aligned load reproduces retail's reload and diagonal scheduling.
// @retail 0x31ec30
void __cdecl build_linear_axis_31ec30(solver_linear_axis_31ec30 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	unsigned long *op = output->operations;
	row->linear = input->axis;
	__m128 relative_a = _mm_sub_ps(input->point_a, body_a->center);
	__m128 relative_b = _mm_sub_ps(input->point_b, body_b->center);
	__m128 angular_a = cross_axis(relative_a, input->axis);
	row->a = body_axis(angular_a, body_a);
	__m128 angular_b = cross_axis(input->axis, relative_b);
	__m128 result_b = body_axis(angular_b, body_b);
	__m128 result_a = _mm_load_ps((float const *)&row->a);
	row->b = result_b;
	float diagonal = body_b->inverse_inertia.m128_f32[3] + body_a->inverse_inertia.m128_f32[3]
		+ 1.1920928955078125e-7f;
	float angular = sum_three(angular_diagonal_terms(result_a, row->b, body_a, body_b));
	diagonal += angular;
	row->b.m128_f32[3] = diagonal;
	row->a.m128_f32[3] = 1.0f / diagonal;
	__m128 difference = _mm_sub_ps(input->point_b, input->point_a);
	row->linear.m128_f32[3] = sum_three(_mm_mul_ps(difference, input->axis)) * pair->error_scale;
	*op = 0x3040d;
	output->terms = (__m128 *)(row + 1);
	output->operations = op + 1;
}

// A projected separation row with lower and upper operation bounds.
// score 0 (was 7): explicitly reload the first axis before storing the second,
// retaining the output-reference transforms and scaled bound temporaries.
// @retail 0x31ee10
void __cdecl build_linear_range_31ee10(solver_linear_range_31ee10 const *input,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	row->linear = input->axis;
	__m128 relative_a = _mm_sub_ps(input->point_a, body_a->center);
	__m128 relative_b = _mm_sub_ps(input->point_b, body_b->center);
	__m128 angular_a = cross_axis(relative_a, input->axis);
	store_body_axis(row->a, angular_a, body_a);
	__m128 angular_b = cross_axis(input->axis, relative_b);
	__m128 result_b;
	store_body_axis(result_b, angular_b, body_b);
	__m128 result_a = _mm_load_ps((float const *)&row->a);
	row->b = result_b;
	float diagonal = body_b->inverse_inertia.m128_f32[3] + body_a->inverse_inertia.m128_f32[3]
		+ 1.1920928955078125e-7f;
	float angular = sum_three(angular_diagonal_terms(result_a, row->b, body_a, body_b));
	diagonal += angular;
	row->b.m128_f32[3] = diagonal;
	row->a.m128_f32[3] = 1.0f / diagonal;
	__m128 difference = _mm_sub_ps(input->point_b, input->point_a);
	row->linear.m128_f32[3] = sum_three(_mm_mul_ps(difference, input->axis)) * pair->error_scale;
	float negative_scale = -pair->error_scale;
	float minimum = negative_scale * input->minimum;
	float maximum = negative_scale * input->maximum;
	struct range_operation { unsigned long opcode; float minimum, maximum; };
	range_operation *op = (range_operation *)output->operations;
	op->opcode = 0x30c0b;
	op->minimum = minimum;
	op->maximum = maximum;
	output->terms = (__m128 *)(row + 1);
	output->operations = (unsigned long *)(op + 1);
}

__declspec(align(16)) static float const solver_identity_axes[4][4] =
{
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, 1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, 0.0f, 1.0f}
};

// Three Cartesian rows between world-space points.
// score 0 (was 7): explicitly reload the first axis before storing the second,
// retaining the output-reference transforms and operation-first increments.
// @retail 0x31f3e0
void __cdecl build_point_axes_31f3e0(__m128 const *point_a, __m128 const *point_b,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	unsigned long *op = output->operations;
	__m128 const *axis = (__m128 const *)solver_identity_axes;
	for (long count = 3; count > 0; --count, ++op, ++row, ++axis)
	{
		row->linear = *axis;
		__m128 relative_a = _mm_sub_ps(*point_a, body_a->center);
		__m128 relative_b = _mm_sub_ps(*point_b, body_b->center);
		__m128 angular_a = cross_axis(relative_a, *axis);
		store_body_axis(row->a, angular_a, body_a);
		__m128 angular_b = cross_axis(*axis, relative_b);
		__m128 result_b;
		store_body_axis(result_b, angular_b, body_b);
		__m128 result_a = _mm_load_ps((float const *)&row->a);
		row->b = result_b;
		float diagonal = body_b->inverse_inertia.m128_f32[3] + body_a->inverse_inertia.m128_f32[3]
			+ 1.1920928955078125e-7f;
		float angular = sum_three(angular_diagonal_terms(result_a, row->b, body_a, body_b));
		diagonal += angular;
		row->b.m128_f32[3] = diagonal;
		row->a.m128_f32[3] = 1.0f / diagonal;
		__m128 difference = _mm_sub_ps(*point_b, *point_a);
		row->linear.m128_f32[3] = sum_three(_mm_mul_ps(difference, *axis)) * pair->error_scale;
		*op = 0x3040d;
	}
	output->operations = op;
	output->terms = (__m128 *)row;
}

static inline __m128 transform_point(solver_transform_31f160 const *transform, __m128 const &point)
{
	__m128 z = _mm_add_ps(_mm_mul_ps(transform->rotation[2], _mm_shuffle_ps(point, point, 0xaa)), transform->translation);
	__m128 y = _mm_mul_ps(transform->rotation[1], _mm_shuffle_ps(point, point, 0x55));
	__m128 x = _mm_mul_ps(transform->rotation[0], _mm_shuffle_ps(point, point, 0));
	return _mm_add_ps(_mm_add_ps(x, y), z);
}

// Transform local pivots and constrain their three world-space components.
// score 0 (was 7): keep the first-axis reload in each body-space branch and
// use the transform helper in the rotated path. This preserves the pointer
// registers and delays the reload until after the second axis is transformed.
// @retail 0x31f160
void __cdecl build_transformed_points_31f160(solver_transform_31f160 const *transform_a,
	solver_transform_31f160 const *transform_b, __m128 const *pivot_a, __m128 const *pivot_b,
	solver_pair_31e7b0 const *pair, solver_output_31e7b0 *output)
{
	__m128 point_a = transform_point(transform_a, *pivot_a);
	__m128 point_b = transform_point(transform_b, *pivot_b);
	solver_body_31e7b0 const *body_a = pair->a;
	solver_body_31e7b0 const *body_b = pair->b;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	unsigned long *op = output->operations;
	__m128 const *axis = (__m128 const *)solver_identity_axes;
	__m128 difference = _mm_sub_ps(point_b, point_a);
	for (long count = 3; count > 0; --count, ++op, ++row, ++axis)
	{
		row->linear = *axis;
		__m128 relative_a = _mm_sub_ps(point_a, body_a->center);
		__m128 relative_b = _mm_sub_ps(point_b, body_b->center);
		__m128 angular_a = cross_axis(relative_a, *axis);
		store_body_axis(row->a, angular_a, body_a);
		__m128 angular_b = cross_axis(*axis, relative_b);
		__m128 result_a;
		if (body_b->axes_already_local)
		{
			result_a = _mm_load_ps((float const *)&row->a);
			row->b = angular_b;
		}
		else
		{
			__m128 result_b;
			store_body_axis(result_b, angular_b, body_b);
			result_a = _mm_load_ps((float const *)&row->a);
			row->b = result_b;
		}
		float diagonal = body_b->inverse_inertia.m128_f32[3] + body_a->inverse_inertia.m128_f32[3]
			+ 1.1920928955078125e-7f;
		float angular = sum_three(angular_diagonal_terms(result_a, row->b, body_a, body_b));
		diagonal += angular;
		row->b.m128_f32[3] = diagonal;
		row->a.m128_f32[3] = 1.0f / diagonal;
		row->linear.m128_f32[3] = sum_three(_mm_mul_ps(difference, *axis)) * pair->error_scale;
		*op = 0x3040d;
	}
	output->operations = op;
	output->terms = (__m128 *)row;
}

struct contact_property_31f620
{
	float impulse, bias, friction, unknown0c, unknown10, distance;
	unsigned short flags, unknown1a;
};
struct contact_state_31f620
{
	unsigned long flags;
	float unknown04, tangent_bias_a, unknown0c, tangent_bias_b, unknown14;
	float angular_bias, radius;
	__m128 tangent_a, tangent_b;
};
struct contact_input_31f620 : solver_pair_31e7b0
{
	float maximum_shift, unknown14, coupling_scale, friction_scale;
};
struct contact_header_31f620
{
	unsigned long opcode;
	__m128 *terms;
	contact_property_31f620 *properties;
	solver_body_31e7b0 const *a, *b;
	long property_stride;
};
struct contact_friction_op_31f620
{
	unsigned long opcode;
	void *results;
	float coupling, maximum;
	long result_stride;
	float radius;
};
typedef char contact_property_size_31f620[sizeof(contact_property_31f620) == 28 ? 1 : -1];
typedef char contact_state_size_31f620[sizeof(contact_state_31f620) == 64 ? 1 : -1];
typedef char contact_property_flags_offset[offsetof(contact_property_31f620, flags) == 24 ? 1 : -1];
typedef char contact_state_tangent_offset[offsetof(contact_state_31f620, tangent_a) == 32 ? 1 : -1];

// Explicit aligned stores retain the closest contact-row register allocation.
static inline void store_contact_axis_31f620(__m128 &result, __m128 const &axis, solver_body_31e7b0 const *body)
{
	if (body->axes_already_local)
		_mm_store_ps((float *)&result, axis);
	else
		_mm_store_ps((float *)&result, rotated_axis(axis, body));
}

// Retail inlines both normalization sites in the contact builder.
static __forceinline __m128 normalized_axis(__m128 const &axis)
{
	__m128 square = _mm_mul_ps(axis, axis);
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(square, square, 0x55), square);
	__m128 length = _mm_add_ss(_mm_shuffle_ps(square, square, 0xaa), xy);
	__m128 reciprocal = _mm_rsqrt_ss(length);
	__m128 correction = _mm_sub_ss(_mm_set_ss(3.0f), _mm_mul_ss(_mm_mul_ss(length, reciprocal), reciprocal));
	__m128 factor = _mm_mul_ss(_mm_mul_ss(_mm_set_ss(0.5f), reciprocal), correction);
	return _mm_mul_ps(_mm_shuffle_ps(factor, factor, 0), axis);
}

// Retail also inlines the normal-row and friction-row coupling calculations.
static __forceinline __m128 linear_coupling_terms(solver_linear_row_31e7b0 const *a,
	solver_linear_row_31e7b0 const *b, solver_body_31e7b0 const *body_a,
	solver_body_31e7b0 const *body_b)
{
	__m128 linear = _mm_mul_ps(a->linear, b->linear);
	__m128 inverse_a = _mm_set_ss(body_a->inverse_inertia.m128_f32[3]);
	__m128 inverse_b = _mm_set_ss(body_b->inverse_inertia.m128_f32[3]);
	__m128 mass_b = _mm_mul_ps(_mm_shuffle_ps(inverse_b, inverse_b, 0), linear);
	__m128 mass_a = _mm_mul_ps(_mm_shuffle_ps(inverse_a, inverse_a, 0), linear);
	__m128 linear_mass = _mm_add_ps(mass_a, mass_b);
	__m128 angular_b = _mm_mul_ps(_mm_mul_ps(a->b, b->b), body_b->inverse_inertia);
	__m128 angular_a = _mm_mul_ps(_mm_mul_ps(a->a, b->a), body_a->inverse_inertia);
	return _mm_add_ps(_mm_add_ps(angular_a, angular_b), linear_mass);
}

static __forceinline float linear_coupling(solver_linear_row_31e7b0 const *a,
	solver_linear_row_31e7b0 const *b, solver_body_31e7b0 const *body_a,
	solver_body_31e7b0 const *body_b)
{
	return sum_three(linear_coupling_terms(a, b, body_a, body_b));
}

// The output reference keeps friction coupling and its inverse determinant
// in the separate scalar stack slots used by retail.
static __forceinline void store_linear_coupling_31f620(float &result, solver_linear_row_31e7b0 const *a,
	solver_linear_row_31e7b0 const *b, solver_body_31e7b0 const *body_a,
	solver_body_31e7b0 const *body_b)
{
	__m128 value = linear_coupling_terms(a, b, body_a, body_b);
	__m128 xy = _mm_add_ss(_mm_shuffle_ps(value, value, 0x55), value);
	__m128 xyz = _mm_add_ss(_mm_shuffle_ps(value, value, 0xaa), xy);
	_mm_store_ss(&result, xyz);
}

// The wide argument retains retail's x87 float copy and the preceding SIMD
// reduction's address scheduling; a plain float assignment uses integer moves.
static __forceinline void store_contact_radius_31f620(float &result, double value)
{
	result = value;
}

// By-value operands retain retail's proposed-shift spill and selection order.
static __forceinline float contact_minimum(float a, float b)
{
	return a < b ? a : b;
}

// By-value negation retains retail's normal-accumulation and bias-store order.
static __forceinline float contact_negate_31f620(float value)
{
	return -value;
}

// Keep the local cursor in its stack slot and preserve the loop-exit reload order.
static __forceinline void advance_contact_cursor_31f620(unsigned long *volatile &cursor, unsigned long *position)
{
	cursor = position + 1;
}

// Contact normal rows, optional coupled pairs, a two-axis friction block,
// and a radius-scaled angular friction row for a multi-point manifold.
// score 0 (was 776): retain row-relative coupling, explicit axis reloads,
// scalar-lane broadcasts, a single tangent retry loop, and separate friction
// cursors. The final 12 -> 0 uses by-value negation, an inlined cursor advance,
// and a radius-validity result that keeps the degenerate return after the
// angular row. All 3182 bytes match retail after address normalization.
// @retail 0x31f620
void __cdecl lane_b_contact_build(void const *points_, long count,
	void *properties_, void *solver_data_, void const *input_, void *output_)
{
	lane_b_contact_point const *points = (lane_b_contact_point const *)points_;
	contact_property_31f620 *properties = (contact_property_31f620 *)properties_;
	contact_state_31f620 *state = (contact_state_31f620 *)solver_data_;
	contact_input_31f620 const *input = (contact_input_31f620 const *)input_;
	solver_output_31e7b0 *output = (solver_output_31e7b0 *)output_;
	solver_body_31e7b0 const *body_a = input->a;
	solver_body_31e7b0 const *body_b = input->b;
	float total_impulse = 0.0f;
	float inverse_count = 1.0f / count;
	__m128 *first_terms = output->terms;
	contact_header_31f620 *header = (contact_header_31f620 *)output->operations;
	header->properties = properties;
	header->opcode = 0x11801;
	header->a = body_a;
	header->b = body_b;
	header->property_stride = sizeof(*properties);
	header->terms = first_terms;
	solver_linear_row_31e7b0 *row = (solver_linear_row_31e7b0 *)output->terms;
	float total_friction = 0.0f;
	__m128 position_sum = _mm_setzero_ps();
	__m128 normal_sum = _mm_setzero_ps();
	// The volatile accesses below keep this local cursor on the stack across
	// paired/unpaired paths, leaving retail's point-pointer register available.
	unsigned long *op = (unsigned long *)(header + 1);
	lane_b_contact_point const *point = points;
	contact_property_31f620 *property = properties;
	for (long remaining = count; --remaining >= 0; ++point, ++property, ++row)
	{
		solver_linear_row_31e7b0 *previous_row = row - 1;
		row->linear = point->normal;
		__m128 relative_a = _mm_sub_ps(point->position, body_a->center);
		__m128 relative_b = _mm_sub_ps(point->position, body_b->center);
		__m128 angular_a = cross_axis(relative_a, point->normal);
		store_contact_axis_31f620(row->a, angular_a, body_a);
		__m128 angular_b = cross_axis(point->normal, relative_b);
		__m128 normal_axis_a;
		if (body_b->axes_already_local)
		{
			normal_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = angular_b;
		}
		else
		{
			__m128 result_b;
			store_body_axis(result_b, angular_b, body_b);
			normal_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = result_b;
		}
		// Keep A's inverse mass load before B's; a plain sum reverses the
		// x87 operands here. Only this existing field load is volatile.
		float diagonal = *(float const volatile *)&body_a->inverse_inertia.m128_f32[3]
			+ body_b->inverse_inertia.m128_f32[3] + 1.1920928955078125e-7f;
		diagonal += sum_three(angular_diagonal_terms(normal_axis_a, row->b, body_a, body_b));
		row->b.m128_f32[3] = diagonal;
		row->a.m128_f32[3] = 1.0f / diagonal;
		total_friction += property->friction;
		total_impulse += property->impulse;
		float distance = property->distance;
		float gap = point->position.m128_f32[3] - distance;
		float margin = -property->bias - gap;
		float shift = contact_minimum(distance * -0.050000011920928955f, input->maximum_shift);
		float adjusted_distance = distance + shift;
		float separation = gap - shift;
		if (shift + shift + 0.001f < margin)
		{
			adjusted_distance -= margin;
			separation += margin;
		}
		if (fabs(separation) < 1.0e-10f)
			separation = 0.0f;
		property->distance = adjusted_distance < 0.0f ? adjusted_distance : 0.0f;
		position_sum = _mm_add_ps(position_sum, point->position);
		normal_sum = _mm_add_ps(normal_sum, point->normal);
		row->linear.m128_f32[3] = contact_negate_31f620(separation) * input->error_scale;
		if (!property->flags)
		{
			unsigned long *operation = *(unsigned long * const volatile *)&op;
			*operation = 0x30402;
			advance_contact_cursor_31f620(op, operation);
		}
		else
		{
			float coupling = linear_coupling(row, previous_row, body_a, body_b) * input->coupling_scale;
			unsigned long *operation = *(unsigned long * const volatile *)&op;
			operation[-1] = 0x50803;
			float previous_diagonal = previous_row[0].b.m128_f32[3];
			// The wide temporary and float denominator retain retail's x87
			// operand duplication and its single-precision reciprocal constant.
			double current_diagonal = row->b.m128_f32[3];
			float inverse_determinant = 1.0f / (float)(previous_diagonal * current_diagonal - coupling * coupling);
			previous_row[0].b.m128_f32[3] = inverse_determinant * current_diagonal;
			row->b.m128_f32[3] = inverse_determinant * previous_diagonal;
			*(float *)operation = -(inverse_determinant * coupling);
			advance_contact_cursor_31f620(op, operation);
		}
	}
	float friction_limit = (total_friction * inverse_count) * total_impulse;
	if (friction_limit > 0.0f)
	{
		float normal_length_squared = sum_three(_mm_mul_ps(normal_sum, normal_sum));
		__m128 normal;
		if ((inverse_count * inverse_count) * normal_length_squared > 0.9999f)
			normal = points->normal;
		else if (normal_length_squared > 0.1f)
			normal = normalized_axis(normal_sum);
		else
		{
			normal = points->normal;
			if (sum_three(_mm_mul_ps(normal, normal)) < 0.9f)
			{
				normal = _mm_setzero_ps();
				normal.m128_f32[1] = 1.0f;
				friction_limit = 0.0f;
			}
		}
		float tangent_length;
		bool retry;
		do
		{
			state->tangent_b = cross_axis(state->tangent_a, normal);
			tangent_length = sum_three(_mm_mul_ps(state->tangent_b, state->tangent_b));
			retry = !(tangent_length > 0.001f);
			if (retry)
			{
				long smallest = 0, first = 1, second = 2;
				float smallest_value = (float)fabs(normal.m128_f32[0]);
				float y = (float)fabs(normal.m128_f32[1]);
				float z = (float)fabs(normal.m128_f32[2]);
				if (y < smallest_value) { first = smallest; smallest = 1; smallest_value = y; }
				if (z < smallest_value) { second = smallest; smallest = 2; }
				state->tangent_a.m128_f32[smallest] = 0.0f;
				state->tangent_a.m128_f32[first] = normal.m128_f32[second];
				state->tangent_a.m128_f32[second] = -normal.m128_f32[first];
				friction_limit = 0.0f;
			}
		} while (retry);
		state->tangent_b = normalized_axis(state->tangent_b);
		state->tangent_a = cross_axis(normal, state->tangent_b);
		__m128 inverse_count_lane = _mm_set_ss(inverse_count);
		__m128 mean_point = _mm_mul_ps(_mm_shuffle_ps(inverse_count_lane, inverse_count_lane, 0), position_sum);
		row->linear = _mm_load_ps((float const *)&state->tangent_a);
		__m128 relative_a = _mm_sub_ps(mean_point, body_a->center);
		__m128 relative_b = _mm_sub_ps(mean_point, body_b->center);
		__m128 angular_a = cross_axis(relative_a, state->tangent_a);
		store_contact_axis_31f620(row->a, angular_a, body_a);
		__m128 angular_b = cross_axis(state->tangent_a, relative_b);
		__m128 first_tangent_axis_a;
		if (body_b->axes_already_local)
		{
			first_tangent_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = angular_b;
		}
		else
		{
			__m128 result_b;
			store_body_axis(result_b, angular_b, body_b);
			first_tangent_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = result_b;
		}
		float diagonal = body_a->inverse_inertia.m128_f32[3] + body_b->inverse_inertia.m128_f32[3] + 1.1920928955078125e-7f;
		diagonal += sum_three(angular_diagonal_terms(first_tangent_axis_a, row->b, body_a, body_b));
		row->b.m128_f32[3] = diagonal;
		row->a.m128_f32[3] = 1.0f / diagonal;
		row->linear.m128_f32[3] = state->tangent_bias_a * input->error_scale;
		++row;
		row->linear = state->tangent_b;
		relative_a = _mm_sub_ps(mean_point, body_a->center);
		relative_b = _mm_sub_ps(mean_point, body_b->center);
		angular_a = cross_axis(relative_a, state->tangent_b);
		store_contact_axis_31f620(row->a, angular_a, body_a);
		angular_b = cross_axis(state->tangent_b, relative_b);
		__m128 second_tangent_axis_a;
		if (body_b->axes_already_local)
		{
			second_tangent_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = angular_b;
		}
		else
		{
			__m128 result_b;
			store_body_axis(result_b, angular_b, body_b);
			second_tangent_axis_a = _mm_load_ps((float const *)&row->a);
			row->b = result_b;
		}
		diagonal = body_a->inverse_inertia.m128_f32[3] + body_b->inverse_inertia.m128_f32[3] + 1.1920928955078125e-7f;
		diagonal += sum_three(angular_diagonal_terms(second_tangent_axis_a, row->b, body_a, body_b));
		row->b.m128_f32[3] = diagonal;
		row->a.m128_f32[3] = 1.0f / diagonal;
		row->linear.m128_f32[3] = state->tangent_bias_b * input->error_scale;
		++row;
		float coupling;
		store_linear_coupling_31f620(coupling, row - 1, row - 2, body_a, body_b);
		float maximum_friction = friction_limit * input->friction_scale;
		contact_friction_op_31f620 *friction_op = (contact_friction_op_31f620 *)op;
		friction_op->opcode = 0x51407;
		float first_diagonal = row[-2].b.m128_f32[3];
		float second_diagonal = row[-1].b.m128_f32[3];
		float inverse_determinant = 1.0f / (first_diagonal * second_diagonal - coupling * coupling);
		row[-2].b.m128_f32[3] = inverse_determinant * second_diagonal;
		row[-1].b.m128_f32[3] = inverse_determinant * first_diagonal;
		friction_op->results = (char *)state + 4;
		friction_op->coupling = -(inverse_determinant * coupling);
		friction_op->maximum = maximum_friction;
		friction_op->result_stride = 8;
		unsigned long *next_op = op + 5;
		if (count > 1)
		{
			bool radius_valid = true;
			while (state->flags & 4)
			{
				state->radius = 0.0f;
				lane_b_contact_point const *point = points;
				for (long remaining = count; --remaining >= 0; ++point)
				{
					__m128 distance = _mm_sub_ps(point->position, mean_point);
					__m128 square = _mm_mul_ps(distance, distance);
					__m128 xy = _mm_add_ss(_mm_shuffle_ps(square, square, 0x55), square);
					__m128 squared = _mm_add_ss(_mm_shuffle_ps(square, square, 0xaa), xy);
					__m128 root = _mm_sqrt_ss(squared);
					float length;
					_mm_store_ss(&length, root);
					state->radius += length;
				}
				state->radius *= inverse_count;
				if (state->radius < 1.0e-6f)
				{
					radius_valid = false;
					break;
				}
				state->flags &= ~4;
			}
			if (radius_valid)
			{
				solver_angular_row_31e7b0 *angular_row = (solver_angular_row_31e7b0 *)row;
				store_contact_axis_31f620(angular_row->a, normal, body_a);
				__m128 negative_normal = _mm_xor_ps(normal, *(__m128 const *)solver_sign_bits);
				store_body_axis(angular_row->b, negative_normal, body_b);
				angular_row->a.m128_f32[3] = 1.0f / (sum_three(angular_diagonal_terms(angular_row->a, angular_row->b, body_a, body_b)) + 1.1920928955078125e-7f);
				float inverse_radius = 1.0f / state->radius;
				store_contact_radius_31f620(friction_op->radius, state->radius);
				friction_op->opcode = 0x61808;
				angular_row->a.m128_f32[3] *= inverse_radius;
				angular_row->b.m128_f32[3] = state->angular_bias * input->error_scale;
				row = (solver_linear_row_31e7b0 *)(angular_row + 1);
				next_op = (unsigned long *)friction_op + 6;
			}
			else
			{
				output->terms = (__m128 *)row;
				output->operations = next_op;
				return;
			}
		}
		output->operations = next_op;
		output->terms = (__m128 *)row;
		return;
	}
	output->operations = op;
	output->terms = (__m128 *)row;
}
