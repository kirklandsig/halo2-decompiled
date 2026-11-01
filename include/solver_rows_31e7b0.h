#ifndef SOLVER_ROWS_31E7B0_H
#define SOLVER_ROWS_31E7B0_H
#include <xmmintrin.h>
#include <stddef.h>

// The body prefix read by the solver builders. The fourth inverse-inertia
// lane holds inverse mass; the transform starts with its position vector.
struct solver_body_31e7b0
{
	unsigned char unknown00[12];
	unsigned char axes_already_local;
	unsigned char unknown0d[35];
	__m128 inverse_inertia;
	__m128 center;
	__m128 rotation[3];
};
struct solver_pair_31e7b0
{
	solver_body_31e7b0 const *a;
	solver_body_31e7b0 const *b;
	float velocity_scale;
	float error_scale;
};
struct solver_output_31e7b0
{
	__m128 *terms;
	unsigned long *operations;
};
struct solver_angular_row_31e7b0 { __m128 a, b; };
struct solver_linear_row_31e7b0 { __m128 linear, a, b; };
struct solver_scalar_op_31e7b0 { unsigned long opcode; float value; };
struct solver_range_op_31e7b0 { unsigned long opcode; float minimum, maximum, softness; };

struct solver_angular_error_31e7b0 { __m128 error_a, error_b, axis; };
struct solver_angular_range_31f000 { __m128 axis; float minimum, maximum, error; };
struct solver_axis_values_31e900 { float unused, error; };
struct solver_angular_array_31e900
{
	__m128 const *axes;
	solver_axis_values_31e900 const *values;
	float velocity;
	long count;
};
struct solver_linear_velocity_31ea90
{
	__m128 point, axis;
	float velocity;
	float const *result;
};
struct solver_linear_axis_31ec30 { __m128 point_a, point_b, axis; };
struct solver_linear_range_31ee10 : solver_linear_axis_31ec30 { float minimum, maximum; };
struct solver_transform_31f160 { __m128 rotation[3], translation; };

typedef char solver_body_inverse_offset[offsetof(solver_body_31e7b0, inverse_inertia) == 0x30 ? 1 : -1];
typedef char solver_body_rotation_offset[offsetof(solver_body_31e7b0, rotation) == 0x50 ? 1 : -1];
typedef char solver_body_prefix_size[sizeof(solver_body_31e7b0) == 0x80 ? 1 : -1];
typedef char solver_angular_row_size[sizeof(solver_angular_row_31e7b0) == 0x20 ? 1 : -1];
typedef char solver_linear_row_size[sizeof(solver_linear_row_31e7b0) == 0x30 ? 1 : -1];

void __cdecl build_angular_error_31e7b0(solver_angular_error_31e7b0 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_angular_array_31e900(solver_angular_array_31e900 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_linear_velocity_31ea90(solver_linear_velocity_31ea90 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_linear_axis_31ec30(solver_linear_axis_31ec30 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_linear_range_31ee10(solver_linear_range_31ee10 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_angular_range_31f000(solver_angular_range_31f000 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_transformed_points_31f160(solver_transform_31f160 const *, solver_transform_31f160 const *, __m128 const *, __m128 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
void __cdecl build_point_axes_31f3e0(__m128 const *, __m128 const *, solver_pair_31e7b0 const *, solver_output_31e7b0 *);
#endif
