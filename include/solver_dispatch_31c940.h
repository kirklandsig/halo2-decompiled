#ifndef SOLVER_DISPATCH_31C940_H
#define SOLVER_DISPATCH_31C940_H
#include "solver_rows_31e7b0.h"
#include "solver_validation_31c540.h"

struct solver_step_31c940
{
	float unknown00, error_scale, velocity_scale, friction_scale, inverse_time;
	float unknown14, unknown18;
	long iterations;
};
struct solver_velocity_31c940
{
	signed char kind;
	unsigned char unknown01[15];
	__m128 linear, angular, inverse_inertia;
	__m128 accumulated_linear, accumulated_angular;
	unsigned char unknown60[32];
};
struct solver_header_31c940
{
	unsigned long opcode;
	__m128 *terms;
	unsigned char *results;
	solver_velocity_31c940 *a, *b;
	long result_stride;
};

// Existing out-of-line row cursor helpers called by retail's result pass.
struct s_linear_term_31c530
{
	__m128 linear, angular_a, angular_b;
	s_linear_term_31c530 *offset(long index);
};
struct s_angular_term_31c520 { __m128 a, b; };
struct s_angular_cursor_31c520 : s_angular_term_31c520
{
	s_angular_cursor_31c520 *offset(long index);
};

long __cdecl solve_operations_31c940(solver_step_31c940 *step, unsigned char *operations,
	solver_velocity_31c940 *bodies, float *impulses);

typedef char solver_velocity_size_31c940[sizeof(solver_velocity_31c940) == 0x80 ? 1 : -1];
typedef char solver_step_size_31c940[sizeof(solver_step_31c940) == 0x20 ? 1 : -1];
typedef char solver_header_size_31c940[sizeof(solver_header_31c940) == 0x18 ? 1 : -1];
#endif
