#ifndef CONSTRAINT_BUILDERS_314220_H
#define CONSTRAINT_BUILDERS_314220_H
#include "constraint_helpers_314090.h"
#include "solver_rows_31e7b0.h"
#include "unknown_1cec30.h"

struct s_ops_cursors_31e740 { __m128 *term; unsigned char *ops; };
struct s_ops_input_31e770 { long body_a, body_b; };
struct s_ops_output_31f5e0 { void *term; unsigned long *ops; };
struct s_ops_pair_31f5e0 { unsigned long a, b; };
typedef void (__cdecl *solver_callback_320290)();
void __cdecl function_31e770(s_ops_input_31e770 const *, s_ops_cursors_31e740 *, long, long);
void __cdecl function_31e740(float, __m128 const *, s_ops_cursors_31e740 const *, long);
void __cdecl function_31f5e0(s_ops_pair_31f5e0 const *, s_ops_output_31f5e0 *);
void *__cdecl function_320290(solver_callback_320290, long, void const *, s_ops_output_31f5e0 *);

struct c_query_transform { __m128 axes[3], position; };
struct c_transformed_point
{
	__m128 value;
	void transform(void const *matrix, __m128 const *point);
};
struct vector_2da6e0
{
	__m128 value;
	void rotate(c_query_transform const *matrix, __m128 const *direction);
};
struct s_2da8c0
{
	hkRotation field_0;
	void function_2da8c0(hkRotation const *, hkRotation const *);
};

// Entity +0x3c points to its motion; the world transform is at motion +0x80.
struct constraint_body_314220
{
	unsigned char unknown00[0x3c];
	unsigned char *motion;
	__forceinline c_query_transform const *transform() const
	{
		return (c_query_transform const *)(motion + 0x80);
	}
};
static __forceinline c_query_transform const *constraint_transform_314220(void const *body)
{
	return ((constraint_body_314220 const *)body)->transform();
}
#endif
