// @flags /O2 /Ob1 /Gr /arch:SSE
#include "orientations_storage.h"
/* UNKNOWN_141590.CPP: transform4x3f and quaternion math */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "normalize3d.h"
#include <math.h>
#include <string.h>



vector3f *function_11d000(vector3f const *v, vector3f *out);

// @retail 0x141590
void function_141590(
	transform4x3f const *in,
	transform4x3f *out)
{
	if (in->scale != 0.f)
	{
		real z, y, x;
		real t;
		x = 0.f - in->position.x;
		y = 0.f - in->position.y;
		z = 0.f - in->position.z;
		if (in->scale != 1.f)
		{
			real const &inverse = 1.f / in->scale;
			out->scale = inverse;
			x = (real)(x * inverse);
			y = (real)(y * inverse);
			z = (real)(z * inverse);
		}
		else
		{
			out->scale = 1.f;
		}
		out->forward.i = in->forward.i;
		out->left.j = in->left.j;
		out->up.k = in->up.k;
		t = in->left.i; out->left.i = in->forward.j; out->forward.j = t;
		t = in->up.i; out->up.i = in->forward.k; out->forward.k = t;
		t = in->up.j; out->up.j = in->left.k; out->left.k = t;
		out->position.x = out->up.i * z + out->forward.i * x + out->left.i * y;
		out->position.y = out->forward.j * x + out->up.j * z + out->left.j * y;
		out->position.z = out->forward.k * x + out->up.k * z + out->left.k * y;
	}
	else
	{
		memset(out, 0, sizeof(transform4x3f));
	}
}

// @retail 0x1416c0
void function_1416c0(
	point3f const *position,
	transform4x3f *out)
{
	out->scale = 1.f;
	out->forward.i = 1.f;
	out->forward.j = 0.f;
	out->forward.k = 0.f;
	out->left.i = 0.f;
	out->left.j = 1.f;
	out->left.k = 0.f;
	out->up.i = 0.f;
	out->up.j = 0.f;
	out->up.k = 1.f;
	out->position = *position;
}

// @retail 0x141710
void matrix4x3_from_forward_and_up(
	transform4x3f *out,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
}

real function_30bf0(vector3f *v);
bool function_143120(vector3f const *forward, vector3f const *left, vector3f const *up);

quaternionf *g_4687cc;
extern transform4x3f *g_4687d0;

/* rotates a vector by a unit quaternion: the vector form of quaternion_rotate_point
   (src/unknown_11cb00.cpp), with the same terms; here w squared is named and the doubled
   w comes after the dot, the order retail computes them in (w2 from a fresh 2.0) */
static inline void quaternion_transform_vector(
	quaternionf const *q,
	vector3f const *v,
	vector3f *out)
{
	real square = q->w * q->w;
	real m = square * 2.f - 1.f;
	real d = (q->j * v->j + v->i * q->i + v->k * q->k) * 2.f;
	real w2 = q->w * 2.f;
	real cx = q->j * v->k - v->j * q->k;
	real cy = v->i * q->k - q->i * v->k;
	real cz = q->i * v->j - q->j * v->i;

	out->i = cx * w2 + v->i * m + q->i * d;
	out->j = q->j * d + cy * w2 + m * v->j;
	out->k = v->k * m + cz * w2 + d * q->k;
}

/* the rotation that takes one unit vector to another */
// @retail 0x1417b0
void matrix4x3_rotation_between_vectors(
	transform4x3f *matrix,
	vector3f const *arg_5f338b,
	vector3f const *arg_bc44c6)
{
	quaternionf rotation;
	real dot = dot3f(arg_5f338b, arg_bc44c6);
	real cosine = dot;

	if (dot < -1.f)
	{
		cosine = -1.f;
	}
	else if (dot > 1.f)
	{
		cosine = 1.f;
	}

	real cosine_half = (real)sqrt((cosine + 1.f) * 0.5f);
	real sine_half = (real)sqrt((1.f - cosine) * 0.5f);
	real sine = sine_half * cosine_half * 2.f;

	if (sine != 0.f)
	{
		cross3f_staged(arg_5f338b, arg_bc44c6, (vector3f *)&rotation);
		real scale = sine_half / sine;
		vector3f_scale((vector3f *)&rotation, scale, (vector3f *)&rotation);
		rotation.w = cosine_half;
	}
	else if (cosine < 0.f)
	{
		function_11d000(arg_5f338b, (vector3f *)&rotation);
		rotation.w = 0.f;
	}
	else
	{
		rotation = *g_4687cc;
	}

	*matrix = *g_4687d0;
	quaternion_transform_vector(&rotation, &matrix->forward, &matrix->forward);
	quaternion_transform_vector(&rotation, &matrix->up, &matrix->up);
	quaternion_transform_vector(&rotation, &matrix->left, &matrix->left);
	if (!function_143120(&matrix->forward, &matrix->left, &matrix->up))
	{
		function_30bf0(&matrix->up);
		cross3f_staged(&matrix->up, &matrix->forward, &matrix->left);
		function_30bf0(&matrix->left);
		cross3f_staged(&matrix->left, &matrix->up, &matrix->forward);
	}
}

// @retail 0x141ce0
void function_141ce0(
	real a,
	real b,
	real c,
	transform4x3f *out)
{
	point2f angle_c;
	real cos_b, sin_b, cos_a, sin_a, sin_b_sin_c, sin_b_cos_c;
	angle_c.x = (real)cos(c);
	angle_c.y = (real)sin(c);
	cos_b = (real)cos(b);
	sin_b = (real)sin(b);
	cos_a = (real)cos(a);
	sin_a = (real)sin(a);
	sin_b_cos_c = sin_b * angle_c.x;
	sin_b_sin_c = sin_b * angle_c.y;
	out->scale = 1.f;
	out->forward.j = sin_a * angle_c.x - sin_b_sin_c * cos_a;
	out->forward.i = cos_a * cos_b;
	out->forward.k = sin_b_cos_c * cos_a + sin_a * angle_c.y;
	out->left.i = 0.f - sin_a * cos_b;
	out->left.j = sin_b_sin_c * sin_a + cos_a * angle_c.x;
	out->left.k = cos_a * angle_c.y - sin_b_cos_c * sin_a;
	out->up.i = 0.f - sin_b;
	out->up.j = 0.f - cos_b * angle_c.y;
	out->up.k = cos_b * angle_c.x;
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
}

// @retail 0x141e10
matrix3x3 *function_141e10(
	matrix3x3 *out,
	quaternionf const *q)
{
	real norm = q->i * q->i + q->j * q->j + q->k * q->k + q->w * q->w;
	real s = norm > 0.0001f ? 2.f / norm : 0.f;
	real xs = q->i * s;
	real ys = q->j * s;
	real zs = q->k * s;
	real wx = q->w * xs;
	real wy = q->w * ys;
	real wz = q->w * zs;
	real xx = q->i * xs;
	real xy = q->i * ys;
	real xz = q->i * zs;
	real yy = q->j * ys;
	real yz = q->j * zs;
	real zz = q->k * zs;
	out->forward.i = 1.f - (yy + zz);
	out->left.i = xy - wz;
	out->up.i = xz + wy;
	out->forward.j = xy + wz;
	out->left.j = 1.f - (xx + zz);
	out->up.j = yz - wx;
	out->forward.k = xz - wy;
	out->left.k = yz + wx;
	out->up.k = 1.f - (xx + yy);
	return out;
}

// @retail 0x141f60
quaternionf *function_141f60(
	matrix3x3 const *matrix,
	quaternionf *out)
{
	real const *m = (real const *)matrix;
	real trace = matrix->left.j + matrix->forward.i + matrix->up.k;
	if (trace > 0.f)
	{
		real root = (real)(sqrt(trace + 1.f) * 0.5f);
		real inverse = 0.25f / root;
		out->w = root;
		out->i = (m[5] - m[7]) * inverse;
		out->j = (m[6] - m[2]) * inverse;
		out->k = (m[1] - m[3]) * inverse;
	}
	else
	{
		long i = 0, j, k;
		real root, inverse;
		if (m[4] > m[0])
			i = 1;
		if (m[8] > m[i * 4])
			i = 2;
		j = (i + 1) % 3;
		k = (i + 2) % 3;
		root = (real)(sqrt(m[i * 4] - m[j * 4] - m[k * 4] + 1.f) * 0.5f);
		inverse = 0.25f / root;
		out->n[i] = root;
		out->w = (m[j * 3 + k] - m[k * 3 + j]) * inverse;
		out->n[j] = (m[i * 3 + j] + m[j * 3 + i]) * inverse;
		out->n[k] = (m[i * 3 + k] + m[k * 3 + i]) * inverse;
	}
	if (out->w < 0.f)
	{
		out->w = 0.f - out->w;
		out->i = 0.f - out->i;
		out->j = 0.f - out->j;
		out->k = 0.f - out->k;
	}
	return out;
}

void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);

// @retail 0x1420f0
void function_1420f0(
	transform4x3f *out,
	point3f const *position,
	vector3f const *forward,
	vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	vector3f left;
	left.k = forward->j * up->i;
	left.k -= forward->i * up->j;
	left.j = up->k * forward->i - forward->k * up->i;
	left.i = forward->k * up->j;
	left.i -= up->k * forward->j;
	out->left.k = left.k;
	out->left.j = left.j;
	out->left.i = left.i;
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
	out->position = *position;
}

// @retail 0x1421b0
void function_1421b0(
	transform4x3f *out,
	point3f const *position,
	quaternionf const *rotation)
{
	function_141e10(&out->rotation, rotation);
	out->position.x = 0.f;
	out->position.y = 0.f;
	out->position.z = 0.f;
	out->scale = 1.f;
	out->position = *position;
}

real g_45dbdc = 0.0001f;
static const real g_45dbc0 = 1.f;
__declspec(align(16)) static const unsigned long g_453750[4] = {0x80000000, 0, 0, 0x80000000};

// @retail 0x1421f0
void __stdcall function_1421f0(
	transform4x3f *out,
	rigid_transform_scaled const *orientation)
{
	static real epsilon = g_45dbdc;
	__asm
	{
		mov eax, orientation
		mov ecx, out
		movlps xmm0, qword ptr [eax]
		movhps xmm0, qword ptr [eax + 8]
		movaps xmm7, xmm0
		mulps xmm0, xmm7
		movhlps xmm1, xmm0
		addps xmm0, xmm1
		movaps xmm1, xmm0
		shufps xmm1, xmm1, 0x55
		addss xmm0, xmm1
		comiss xmm0, epsilon
		ja big
		movss xmm0, g_45dbc0
		movlps qword ptr [ecx + 4], xmm0
		movhps qword ptr [ecx + 0xc], xmm0
		movlps qword ptr [ecx + 0x14], xmm0
		movhps qword ptr [ecx + 0x1c], xmm0
		movss dword ptr [ecx + 0x24], xmm0
		jmp done
	big:
		rcpss xmm1, xmm0
		mulss xmm0, xmm1
		mulss xmm0, xmm1
		addss xmm1, xmm1
		subss xmm1, xmm0
		addss xmm1, xmm1
		shufps xmm1, xmm1, 0
		mulps xmm7, xmm1
		movss xmm4, dword ptr [eax]
		shufps xmm4, xmm4, 0
		mulps xmm4, xmm7
		movss xmm5, dword ptr [eax + 0xc]
		shufps xmm5, xmm5, 0
		mulps xmm5, xmm7
		movss xmm6, dword ptr [eax + 4]
		shufps xmm6, xmm6, 0
		mulps xmm6, xmm7
		movhlps xmm7, xmm7
		mulss xmm7, dword ptr [eax + 8]
		movaps xmm0, xmm4
		shufps xmm0, xmm0, 0x99
		movaps xmm1, xmm5
		shufps xmm1, xmm1, 0x66
		xorps xmm1, xmmword ptr g_453750
		addps xmm0, xmm1
		movhps qword ptr [ecx + 8], xmm0
		movss dword ptr [ecx + 0x10], xmm0
		movlps qword ptr [ecx + 0x18], xmm0
		movhlps xmm2, xmm6
		shufps xmm6, xmm6, 0x55
		movss xmm0, g_45dbc0
		subss xmm0, xmm7
		movss xmm1, xmm0
		subss xmm0, xmm6
		movss dword ptr [ecx + 4], xmm0
		subss xmm1, xmm4
		movss dword ptr [ecx + 0x14], xmm1
		movss xmm3, g_45dbc0
		subss xmm3, xmm4
		subss xmm3, xmm6
		movss dword ptr [ecx + 0x24], xmm3
		movss xmm7, xmm2
		subss xmm7, xmm5
		movss dword ptr [ecx + 0x20], xmm7
		addss xmm2, xmm5
		movss dword ptr [ecx + 0x18], xmm2
	done:
		movss xmm0, dword ptr [eax + 0x1c]
		movss dword ptr [ecx], xmm0
		movlps xmm0, qword ptr [eax + 0x10]
		movlps qword ptr [ecx + 0x28], xmm0
		movss xmm0, dword ptr [eax + 0x18]
		movss dword ptr [ecx + 0x30], xmm0
	}
}

// @retail 0x142360
void orientation_from_matrix4x3(
	transform4x3f const *matrix,
	rigid_transform_scaled *out)
{
	function_141f60(&matrix->rotation, &out->rotation);
	out->position = matrix->position;
	out->scale = matrix->scale;
}

void function_142390(
	plane3f const *plane,
	transform4x3f *out);

static __forceinline void matrix4x3_basis_store(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up)
{
	out->scale = 1.f;
	out->forward = *forward;
	cross3f_staged(up, forward, &out->left);
	out->up = *up;
	real_point3d_set(&out->position, 0.f, 0.f, 0.f);
	out->position = *position;
}

static __forceinline void function_x7a3668(plane3f const *plane, transform4x3f *out)
{
	vector3f w;
	point3f position;
	function_11d000(&plane->n, &w);
	normalize3d(&w);
	position.x = plane->d * plane->n.i;
	position.y = plane->n.j * plane->d;
	position.z = plane->n.k * plane->d;
	matrix4x3_basis_store(out, &position, &w, &plane->n);
}

// score 51 (was 51): frame 0x1c and sum grouping agree; the extra early first-component
// load changes normalization-join registers and subsequent cross-product allocation.
// The first difference is an extra early load of the first component (retail
// loads only the second before the magnitude test). Second component scaled first
// 52; first scaled as component * inverse 51; third stored first 53; first and
// third read into locals 51; third scaled as k * inverse 51 (fast).
// Round 406: direct nonnegative magnitude guard, length return and shared setter
// 76/76/76/76; original magnitude expression at the caller boundary with the same
// return/setter choices 89/89/89/89. All restored.
// Round 22 (one-file harness, base 52 for the build's 51): the scale operand order of
// each component, all 8 orders, makes no difference (with the third read through out-> 89
// for every order and for sums i-j-k and j-i-k, 105 for j + (i + k)); component locals in
// every subset and declaration order 52 when k is declared before j, 57 otherwise, 89 for
// i alone; magnitude returned (early or else-zero) 52; `>=` guard 80; i/j scaled with *=
// 52; the normalize written on w directly 91; cross3f_staged/cross3f (up, forward) into
// out->left 77/82, through a local 105/105 (371 bytes).
// Round 27 (fast): the file's own unused normalize3d 88; the shared normalize3d.h normalize (dot3f and
// vector3f_scale) in place of it and the local normalize 68 at 353 bytes: the first component is then
// scaled as retail (a copy of the inverse times the component in memory, only the second loaded before
// the test), but the join keeps the first in xmm2 and moves the third to xmm4 (retail xmm4/xmm0) and the
// cross products run in the other order; that with the position products before the normalize 83; the
// local normalize without the third-component local 88; with dot3f for the magnitude 88.
// Round 18 (lane 14): normalize helper plain `inline` 51 (same code); basis store plain `inline` 51;
// magnitude through the existing length_sq3f 98 (332 bytes); normalize applied to the returned pointer
// of 0x11d000 94 (354 bytes); position declared before the vector 51. Compile-only screens (which
// reproduce the early first-component load): every subset of component locals x operand orders x
// scale-statement orders (384 forms) at best the second-component local; the shared normalize3d.h
// scale and the cross3f/cross3f_staged forms on it leave the position temporaries in xmm0 (retail
// xmm2), the third component in xmm4 (retail xmm0) and the first in xmm2 (retail xmm4).
// @retail 0x142390
void function_142390(
	plane3f const *plane,
	transform4x3f *out)
{
	function_x7a3668(plane, out);
}

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_11d790(quaternionf const *q, real *angle, vector3f *axis);

// @retail 0x1424f0
vector3f *matrix4x3_rotation_between(
	transform4x3f const *a,
	transform4x3f const *b,
	vector3f *out)
{
	transform4x3f inverse;
	transform4x3f relative;
	quaternionf rotation;
	real angle;

	function_141590(a, &inverse);
	function_142a60(b, &inverse, &relative);
	function_141f60(&relative.rotation, &rotation);
	function_11d790(&rotation, &angle, out);
	out->i *= angle;
	out->j *= angle;
	out->k *= angle;
	return out;
}

// @retail 0x142570
point3f *transform4x3f_apply_point(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	/* retail passes the matrix on the stack (ret 4) while 0x142640, with the
	   same body, takes it in ecx: the parameter's address is taken here, and
	   the optimizer removes the indirection only after LTCG has chosen the
	   convention. Its callers' conventions (0x2104b0 ...) follow from it. */
	transform4x3f const *const *matrix_reference = &matrix;
	real x = point->x;
	real y = point->y;
	real z = point->z;
	if ((*matrix_reference)->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

// @retail 0x142640
vector3f *function_142640(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

// @retail 0x142700
point3f *function_142700(
	transform4x3f const *matrix,
	point3f const *point,
	point3f *out)
{
	if (matrix->scale != 0.f)
	{
		real x = point->x - matrix->position.x;
		real y = point->y - matrix->position.y;
		real z = point->z - matrix->position.z;
		if (matrix->scale != 1.f)
		{
			real inverse = 1.f / matrix->scale;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
		}
		out->x = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
		out->y = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
		out->z = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
		out->z = 0.f;
	}
	return out;
}

// @retail 0x1427f0
vector3f *function_1427f0(
	transform4x3f const *matrix,
	vector3f const *vector,
	vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	if (matrix->scale != 1.f)
	{
		real inverse = 1.f / matrix->scale;
		x = inverse * x;
		y = inverse * y;
		z = inverse * z;
	}
	out->i = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
	out->j = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
	out->k = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
	return out;
}

inline bool function_a0190(vector3f const *vector);
__declspec(noinline) bool function_a0200(real a, real b);

static __forceinline real basis_dot_xzy_vertical(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	real vertical = b->k;
	vertical *= a->k;
	result += vertical;
	real side = a->j;
	side *= b->j;
	result += side;
	return result;
}

static __forceinline real basis_dot_xyz(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	real side = a->j;
	side *= b->j;
	result += side;
	result += a->k * b->k;
	return result;
}

// @retail 0x143120
bool function_143120(
	vector3f const *forward,
	vector3f const *left,
	vector3f const *up)
{
	return function_a0190(forward) &&
		function_a0190(left) &&
		function_a0190(up) &&
		function_a0200(basis_dot_xzy(forward, left), 0.f) &&
		function_a0200(basis_dot_xyz(left, up), 0.f) &&
		function_a0200(basis_dot_xzy_vertical(forward, up), 0.f);
}

inline void function_143250(real *out, unsigned long n, real const *a, real const *b);

// @retail 0x143250
void function_143250(
	real *out,
	unsigned long n,
	real const *a,
	real const *b)
{
	unsigned long i, j;
	for (i = 0; i < n; i++)
	{
		out[i] = b[0] * a[i * n];
		for (j = 1; j < n; j++)
			out[i] = b[j] * a[i * n + j] + out[i];
	}
}

// score 939 (was 939): Frame 0x2fc versus retail 0x300. The scalar homes and coefficient 25/28 schedules still differ.
// M3 lane 2 r107 (fast = plain 939): the build already has retail's convention (rotation in EAX, ret 4). With registers and
// numbers masked 352 lines still differ, most in third-band entries 21-28 (retail evaluates entry 23's first product
// first and keeps 0.071428575 in a register for entries 23 and 25; ours evaluates the subtracted product first and
// multiplies from memory); retail's extra 4-byte cell sits among the scalar spill homes (both use their top cell for
// the same value). Entry 23's first product in parentheses 939 (same code); the order tests as nested `if (order > 2)`
// / `if (order > 3)` blocks with one return 939 (same code); the second-band block unscoped 939, the third-band
// block unscoped 939 (same code); the nine rotation reads `real const` 939 (same code). Diagnostic only: one to seven
// dead initialised locals 1231/1231/1095/1226/1226/1415/1226 (frame stays 0x2fc, ret 4 kept).
// Earlier status line (939, was 1333): Frame 0x2fc versus retail 0x300. The scalar homes and coefficient 25/28 schedules still differ.
// Previous 939 (was 1333): frame 0x2fc versus retail 0x300, displaced scalar homes and differing schedules at entries 25 and 28 remain.
// frame 0x2fc versus retail 0x300 and 10752 of 10773 bytes; `ret 4` with rotation in EAX as retail. 80 of the 83
// matrix entries have retail's value tree; band three's entries 8, 25 and 28 and the stack homes (140 of 216 stores) differ.
// g1A walk (symbolic): retail 0x143600-0x146012 executed as an expression DAG (SSE/x87, f32/f64 and (real) points, association and
// constants from the XBE kept) against this body: all 83 values handed to the three function_143250 calls (3x3, 5x5, 7x7) are identical,
// the order <= 2 / <= 3 returns, call arguments (result+1/+4/+9, n 3/5/7) and result[0] agree; no behaviour difference. Retail's op order
// equals this source's order (0 inversions of 891); ours is reordered (43k). Fast-check trials, no change at 1766: term_64 from term_60; term_41 operands swapped.
// Convention (g2A, 11-05): retail takes rotation in EAX, coefficients in ECX and result in ESI with only `order` on the stack
// (ret 4; callers' setup `push 3; lea ecx; lea eax; lea esi`, 0x23a39); this body takes rotation from the stack (ret 8).
// Not kept: writing the 388 earliest single-use terms into their next statement gives retail's convention with the body
// otherwise identical (0x22850 237 -> 235, 0x23690 481 -> 467, both setups then equal retail's) but 1775 here; 387 terms or
// the last 388 keep ret 8, as does any other 388th term but #420, so it is a count of locals outranking the parameter, not a
// source shape. All 1002 single-use terms inlined: ret 4 at 1789, frame 0x330; the 370 pure copies only: ret 8, 1927.
// Parameters as (result, order, rotation, coefficients) with every call updated: no change on that form, 236/464 on this one
// (rotation still on the stack); retail's ECX/EAX/ESI setup fits both orders, so no evidence for it. The x87 multiply at
// retail 0x1438ba is register allocation (rotation[7] lives in xmm6 in retail, a memory operand here), not the double's spelling.
// Earlier: frame 0x314 versus retail 0x300; rotation admission and basis/matrix homes differ.
// Earlier: full frame 0x300 agrees; rotation input and basis/matrix homes remain.
// Frame pointer 3182; separate adjacent basis pairs [0,1] 2278, [2,3] 2320,
// [4,5] 2286, [6,7] 2329.
// Round 310: second-band product trees 2265; second-band full trees 2265; third-band
// products 2272; third-band full trees 2273; both full trees 2273 (plain). Input and matrix homes remain.
// Frame 0x300 agrees; rotation input and basis/matrix spill homes differ.
// Independent snapshots [1,5,6] 2302; [0,2,7] 2309; [3,4] 2286; [0,4,5] 2286; [3,5,7] 2276 (plain).
// frame 0x300 agrees; rotation admission and matrix homes differ.
// Independent basis pairs [0,5] 2275; [1,6] 2320; [2,4] 2325; [3,6] 2317; subset [0,2,4,6] 2367
// (plain).
// Frame 0x300 agrees; rotation still arrives on the stack and
// basis/matrix homes differ. independent basis 0 6 2311; independent basis 1 7 2293;
// independent basis 2 5 2320; independent basis 4 7 2298; independent basis 0 3 5 7 2299.
// Round 349: Independent basis pairs [1,2] 2323; [2,6] 2355; [0,7] 2287; [3,7] 2290; [5,6] 2317 (fast; control 2266).
// Round 357: Separate first/second/third band tables 2268/2268/2266; independent
// basis pairs [0,2] 2314, [1,4] 2288 (fast; control 2266). Opening body restored.
// Band-exclusive scalar workspaces scoped across each actual 3/5/7 matrix producer,
// all seven subsets and earlier shared declarations: 2266 each (fast control 2266; full 2265).
// Round 224: complete input matrix copy with retained workspace, library/value/aggregate
// 2275/2275/2289; direct matrix library/const/aggregate 2371/2367/2364; direct input table
// library/const 2371/2366 (fast control 2266). Original workspace and expressions restored.
// All four initial negatives at stores/early 2266/2266; shared matrix receiver
// at entry/publication 2163/2163; receiver with inlined negatives 2169/2169;
// receiver with early negatives 2149/2149 (fast control 2266). Last form 2148
// plain, 10390/10773 bytes; receiver used throughout the genuine shared matrix.
// Round 385: Current shared-matrix receiver graph: independent native rows, all seven subsets
// 2240/2237/2348/2191/2294/2286/2397; all with component-order names 2397 (fast control 2149; full
// 2148). Original workspace and complete body restored; input and basis/matrix homes remain.
// Round 259: complete matrix publication after its scalar producer, band masks
// 2/4/6/1/3/5/7 in row order and all in original order: fast
// 2067/2156/2236/2145/2051/2156/2263/2263 (control 2149). First and second
// matrices in row order kept: 2050 plain, 10488/10773 bytes; every scalar
// computation, input, precision boundary, order guard and helper tuple unchanged.
// Round 263: complete column publication of band masks 1/2/3/4/5/6/7 and all reversed columns
// 2051/2040/2040/2199/2199/2210/2210/2291 (fast control 2051). Five-by-five columns kept: 2039 plain,
// 10500/10773 bytes; every scalar producer and helper tuple unchanged.
// Round 268: native two-dimensional band rows, masks 1/2/3/4/5/6/7,
// 2055/2088/2107/2123/2146/2179/2279; all arrays at entry 2279
// (fast control 2040, full 2039). Original shared matrix and publication orders restored;
// full frame 0x300, rotation admission and basis/matrix homes remain.
// Round 271: exact first/second/both native retail matrix store orders after complete
// producers, 2040/2051/2051; separate shared 49-element array alone/first/second/both
// 2279/2279/2294/2294; complete graph with basis identity 2294 (fast control 2040).
// Original shared workspace, publications and complete polynomial body restored.
// Round 187: On retained publications, helper inline/noinline/both 2088/2039/2088; with independent basis 2347/2298/2347; first-matrix negative lifetimes with inline 2092, with both plus independent basis 2347. All restored; rotation admission and matrix homes remain.
// Third-band row publications at the complete scalar producer, rows 0/1/2/3/4/5/6,
// 2076/1997/1972/2026/1808/2105/2040; all rows in producer order 2272
// (fast control 2040). Rows [2,4] 1879; rows [1,2,3,4] 1921. Single row 4
// kept 1808 plain at 10731/10773 bytes; all scalar statements, precision boundaries,
// matrix element values and application calls preserved. Rotation admission and basis/matrix homes remain.
// Round 320: third-band row publication combinations [0,4]/[1,4]/[3,4]/[4,5]/[4,6]/
// [0,1,4]/[3,4,5,6]/[0,1,3,4,5,6] 1843/1806/1837/1836/1799/1781/1915/2075.
// Rows [0,1,4] kept: 1781 plain, 10789/10773 bytes, frame 0x314; all other measurements unchanged.
// Only fourteen additional matrix stores move; every scalar, precision boundary, value and application tuple remains.
// Round 447: Third-band retained late rows [0,1,4] by columns in six row orders, then descending columns across rows and within each row
// 1791/1795/1795/1790/1795/1791/1812/1803. Rotation admission and basis/matrix spill homes remain.
// Original executable source and all trial helper/interface changes restored.
// Lane 10 round 34: retail passes rotation in EAX, coefficients in ECX, result in ESI and the order on the stack
// (ret 4); ours stacks rotation and the order (ret 8). --fast pins conventions from the last full build, so only
// plain checks show them. Plain: the parameters in retail's call-setup order (result, order, rotation,
// coefficients; definition, declaration and four calls) 1781, rotation still stacked; 0x22850's coefficient loop
// written as three per-channel calls as retail has them (and as its second half does) 1781 here, 0x22850 237->254;
// the eight leading terms and the matrix as plain locals 2116 (frame 0x300 as retail, rotation still stacked).
// Fast: rotation-first orders (rotation, order, result, coefficients) and (rotation, result, order, coefficients)
// 1781 each (convention pinned); round 19's body 2266 (rotation stacked there too).
// Round 215: producer-ready retained column 0 1768; producer-ready retained column 1 1782;
// producer-ready retained column 2 1784; producer-ready retained column 3 1913; producer-ready
// retained column 4 1772; producer-ready retained column 5 1766; producer-ready retained column 6
// 1782; all producer-ready retained columns 1833; producer-ready retained columns 0 and 5 (one-
// point increment rejected) 1765. Keep t06 producer-ready column 5, plain-confirmed 1781 to 1766
// at exact 10773-byte retail length; all 11489 baseline matches preserved. Combined columns 0/5 at
// 1765 add only one point and are rejected.
// Round 255: producer-ready retained columns 1/5, 2/5, 3/5, 4/5, 5/6, 1/4/5, 2/4/5,
// 4/5/6 score 1772/1768/1768/1772/1771/1802/1801/1783. Original publication graph restored;
// frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// Round 263: Immutable first-assignment intermediates in all seven band subsets tie 1766 at 10773 bytes;
// complete immutable graph without the mixed workspace 2229 at 10304 bytes. Original scalar declarations
// and inherited workspace restored; frame, input admission and basis/matrix homes remain.
// Round 145: the stacked rotation comes from the number of locals: LTCG passes it in EAX (retail's
// `ret 4`, order alone on the stack) only while the function declares fewer than about 950 of them
// (a one-file LTCG probe flips between 950 and 980 unused declarations on a cut-down body; block or
// struct scope still counts). Plain checks: every term copy propagated (971 terms, rotation still stacked)
// 1927 (10933 bytes); copies propagated and every single-use term written into its consumer (299 terms,
// retail's convention) 1886 (10894); the first 400 single-use terms written in, copies kept (941 terms,
// retail's convention) 1778 (10758); all single-use terms written in, copies kept (356) 1813 (10723); the
// 299-term form with the workspace as plain locals 2222 (10310, frame 0x304). Callers 0x22850 237->235 and
// 0x23690 508->512 with retail's convention.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 46 (lane 14, plain): only terms whose single use is the next statement written into it: the first 420
// (921 declarations) 1775 at 10763 bytes with retail's convention (rotation in EAX, ret 4; 0x22850 235, 0x23690
// 512); all 678 such terms the same 1775 code (only the declaration count changes); on those, the eight leading
// terms and the matrix as plain locals 2236 (frame 0x300 as retail, 10297 bytes), with the matrix pointer kept
// over a plain array 2235. Retail loads the eight leading terms in declaration order, then rotation[7].
// Round 439: single-consumer same-type sum/product/copy elision, all seven subsets,
// 1881/1796/1903/1766/1867/1796/1905; complete graph with matrix receiver renamed 1905.
// Product elision leaves 943 scalars and recovers rotation in EAX, but frame 0x314 remains;
// all nine initial loads and explicit precision conversions were retained. Entire body restored.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 278: Native last-use sum/product/mixed accumulators, left/right priority and band subsets:
// 1835/1776/1805/1774/1776/1781/1774/1810. Product form confirms 1776 plain, EAX rotation and ret 4 but
// frame 0x314. On that product graph, native row subsets and late matrix declaration:
// 1807/1855/1989/1992/2093/2090/2238/2238. Original scalar graph, workspace, publications and body
// restored; frame 0x314/0x300 and basis/matrix homes remain.
// Round 289: native final matrix-entry chains in bands 5/7/both, left-priority 1774/1791/1786;
// right-priority 1766/1804/1804; complete chains plus same-type local alias cleanup 1817/1983.
// The last two recover EAX/ret 4 but frames 0x324/0x314 and caller 508 -> 512 regress; all restored.
// Round 285: Direct matrix destinations in bands 1/2/12/3/13/23/123 score
// 1791/1840/1880/2057/2065/2174/2201; all with the inherited array receiver 2276. Frame, rotation
// admission and basis/matrix homes remain. Original polynomial body and publication graph restored.
// Round 246: Independent complete input columns on the retained publication graph, masks
// 1/2/3/4/5/6/7, 1901/1766/1901/1914/2230/1774/2113; all with a native-table receiver
// 2228. Full extraction recovers frame 0x300 but regresses; original snapshots,
// workspace and polynomial graph restored. Rotation admission and spill homes remain.
// Round 500: literal-only scalar cleanup with complete basis/single-consumer/reused
// same-type copy families, masks 0..7, 1767/1948/1766/1936/1927/2000/1917/1986.
// Original scalar and publication graph restored; frame, rotation admission and basis homes remain.
// The full identity/literal graph has 917 numeric declarations, EAX rotation and frame 0x304
// in the reduced image, but regresses to 1986 at 10988 bytes.
// Round 222: numeric scalar identities, mutable first-assignment declarations,
// first-producer/final-consumer declaration order, double-first/real-first declarations,
// numeric first-assignment and reverse numeric declarations: 1766 each. Every expression,
// precision boundary and publication retained; complete original scalar graph/body restored.
// Round 297: same-real one-use negative/literal-scaling/unequal-factor product families,
// masks 1..7, 1767/1767/1767/1766/1766/1771/1771; complete graph with independent
// original basis/matrix storage 2237. Squares and reused values retained; all executable
// source restored. Frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// Round 72 (lane 16, fast): single-use terms written into the next statement, first 420 and all 648 eligible 1775/1775
// (10763 bytes, rotation EAX, ret 4, frame 0x30c); on the 648 form the leading terms as plain locals, with a matrix
// pointer or a plain array 2238/2238 (10297 bytes, frame 0x2fc); plain leading locals keeping every term 2114 (10412).
// Restored.
// Round 519: Shared identical third-band real sum with complete basis/single-consumer/
// reused copy-family masks 0..7: 1793/1928/1793/1916/1898/1975/1898/1964.
// Native typed replay preserves all 83 matrix values and precision boundaries;
// Full cleanup has frame 0x30c and EAX/ret 4 in preview but regresses and raises a caller score.
// Original scalar, publication and input graphs restored; frame and homes remain.
// P264: single-consumer real arithmetic owned by second-band product/sum/all,
// third-band product/sum/all, crossed product/sum and sum/product: 1766/1771/1816/1790/1869/1854/1869/1784.
// Original scalar graph, workspace, publication phases and complete body restored.
// N325: Native same-type scalar workspace reuse across complete band boundaries,
// six distinct boundary graphs, double-only reuse and first-producer declarations:
// 1767/1767/1788/1766/1788/1780/1767/1788. All 83 typed matrix outputs audited unchanged; original scalar
// lifetimes and complete body restored. Frame, rotation admission and basis/matrix homes remain.
// Round 482: Complete seven-row third-band producer-ready columns 0/1/2/3/4/5/6/all
// give 1952/1805/1778/1767/1779/1786/1859/2210. Exact scalar sequence and all 49
// matrix values retained in every trial; original publication graph and body restored.
// Frame 0x314/0x300, rotation admission and basis/matrix homes remain.
// N333: Single-consumer double roots in bands two/three and double product ownership,
// families 1..7: 1767/1767/1767/1767/1767/1767/1766; complete immutable reused root 1766.
// All 84 typed outputs/call phases agree; original graph restored; frame/admission/homes remain.
// NOT kept: precision-sensitive compaction remained unmatched and tied or regressed.
// N335: Disjoint second-band real/third-band real/both-band double temporary lifetimes, subsets
// 1/2/4/3/5/6/7: 2186/2262/1767/2722/2186/2262/2722; complete last-available reuse 3063. Third-
// band sharing recovers ret 4 but frame 0x324 and length grow; double-only frame 0x30c regresses
// by one. NOT kept: unmatched shared-storage forms; all 84 typed outputs agree, original body
// restored.
// N337: Last-use same-type second/third-band subtraction and zero-minus accumulators,
// family subsets 1..7: 1766/1788/1788/1766/1766/1788/1788; all with right-first ownership 1774.
// All 84 typed outputs agree. NOT kept: unmatched storage reuse ties or regresses and changes
// length away from retail; original scalar graph and complete body restored.
// Round 47 (lane 15, plain): nested single-exit order tests 1766; matrix read directly on the workspace, no pointer, 2027 (10501 bytes,
// frame 0x304); term_10 dropped 1766; term_9 in the workspace 1777 (10791, frame 0x318); first two workspace terms swapped 1766.
// N339: Explicit real narrowing owned by consumers, second-band product/third-band
// roots/third-band product family masks 1..7, then all with immutable reused double root:
// 1767/1781/1780/1771/1771/1775/1774/1774. All 84 typed outputs agree.
// NOT kept: unmatched repeated-value/precision-storage forms regress and move length;
// original polynomial, conversion producers and complete body restored.
// Round 85 (m3 lane 12, plain): retail passes rotation in EAX, coefficients in ECX and result in ESI with order on the
// stack (ret 4); ours pushes rotation (ret 8) although the /Gd first pass loads it once into EAX at entry. Parameters as
// (result, rotation, coefficients, order), retail's call-setup order, 1766 (same code); rotation reads in index order
// 1784 (10754 B); `result[0] = coefficients[0];` first 1766; term_9 read before the matrix pointer 1766; the workspace
// struct as plain locals and a 49-real array 2228 (10304 B, frame 0x300). Body and parameter order restored.
// Round 47 (lane 15, plain): nested single-exit order tests 1766; matrix read directly on the workspace, no pointer, 2027 (10501 bytes,
// frame 0x304); term_10 dropped 1766; term_9 in the workspace 1777 (10791, frame 0x318); first two workspace terms swapped 1766.
// N343: Real storage for both second-band roots, each third-band root, all third-band roots
// and all roots: 1762/1766/1766/1766/1769/1766/1769/1769. Original double roots and full body restored.
// NOT kept: unmatched precision/storage changes; native frame/input/basis homes remain.
// M3 lane 7 r126 (fast): the /Gd first pass loads each parameter once (rotation EAX at entry, result ESI, coefficients
// EDI, order EBX after the first call), so that rule predicts retail's EAX rotation. Diagnostics: a body cut after band
// one still pushes rotation, under five parameter orders and with a void return; the cut body without the 0x143250
// call (out EAX, n EBX) takes rotation in ECX, so that call keeps rotation on the stack in our build. Trials: all nine
// doubles as real 1813 (10825 bytes, frame 0x308, ret 8); the terms as plain locals and a `real matrix[49]` 2114
// (10412, frame 0x2fc, ret 8); `result[0] = coefficients[0]` just before band one's call 1766 (same code); band one in
// retail's store order (each matrix entry after its term) 1794 (10740); the matrix pointer declared first 1766 (same
// code). Retail keeps the nine rotation terms as scattered separate locals and the matrix at the frame top. Restored.
// N346: Isolated matrix/workspace/rotation/result/coefficients/term_9/term_8/term_11 names:
// 1766/1766/1766/1766/1766/1766/1766/1766. Original identifiers and complete scalar graph restored;
// NOT kept: unmatched lexical allocation. Frame, rotation admission and basis/matrix homes remain.
// N361: Actual band-table aggregate initialization, band masks 1..7 and all immutable,
// 1976/1930/1880/2123/2126/2166/2187/2187. Original assignment-based tables and full scalar
// graph restored. NOT kept: unmatched table lifetime/publication forms; frame, admission and homes remain.
// N363: Complete arithmetic-role scalar identities, product/sum-difference/copy-literal
// masks 1/2/4/3/5/6/7 then all scalar roles: 1766 each. Original names and full body restored;
// NOT kept: unmatched home allocation. Native frame, rotation admission and basis homes remain.
// N365: Complete actual-result copy/compound multiply/add/subtract producer families,
// seven subsets and all with producer declarations, 1766 each. Original scalar graph
// restored. NOT kept: unmatched producer ownership; native frame/admission/homes remain.
// N368: Final-consumer lexical homes for real/double/both families, forward/reverse,
// then both within producer bands, forward/reverse: 1766 each. Original names restored.
// NOT kept: unmatched home allocation; native frame, rotation admission and basis/matrix homes remain.
// N371: Complete scalar lexical homes by fanout/depth/live-span, forward/reverse,
// then band-fanout-depth/band-depth-fanout: 1766/1766/1766/1766/1766/1766/1766/1766. Original names restored.
// NOT kept: unmatched spill-priority forms; native frame, input admission and basis/matrix homes remain.
// N374: Actual band-local matrix receiver lifetimes from first store through
// application, band masks 1..7 and all immutable: 1766/1766/1766/1766/1766/1766/1766/1766.
// Original shared receiver and full scalar/publication graph restored;
// NOT kept: no native frame/admission recovery or byte match.
// N376: Same-type copy contraction by actual input/product/root-or-literal/sum
// producers within bands, eight forms 1947/1766/1766/1771/1761/1766/1925/1948. All typed outputs/call
// phases agree. NOT kept: no exact native storage recovery; full scalar graph restored.
// N399: Native-located disjoint scalar reuse, second-band spills/third-band spills/SIMD
// families 1..7 then all at first declarations: fast 1766/2001/2001/2612/2528/2341/2278/2278. All 84 ordered typed outputs
// and application tuples agree. SIMD forms recover EAX/ret 4 and improve both callers, but grow
// the frame/extent (all-SIMD 0x334, 11540 bytes). NOT kept: unmatched physical-storage forms; original body restored.
// Round 90 (m3 lane 16): the workspace declared after the terms 1766 (fast, same code); every same-type plain copy
// propagated (404 copies, 937 declarations) 1986 plain (10988 bytes, rotation EAX and ret 4, frame 0x30c; 0x22850 235,
// 0x23690 467); the workspace matrix ahead of the rotation terms 1766 (fast); that with the 648 single-use terms in
// their next statement 1775 plain (10763 bytes, ret 4, frame 0x314; 0x22850 235, 0x23690 467); only the 37 rotation
// snapshot copies propagated 1948 (fast, 10949 bytes: the band-start copies keep the snapshots out of the workspace).
// M3 lane 12 r108 (plain; callers 0x22850 223 and 0x23690 401 unchanged): band one's four negations and term_10 written into
// the matrix stores 1791 (10740 B); band two's 40 pure copies propagated 1947 (10952 B, likely N376's 1947 form); band two's
// four negations alone 1767; band one's term_10 copy alone 1766 (same code); band three's 363 copies 1802 (10813 B);
// band two's copies and negations (44) 1947; bands one and two together (49) 1906 (10916 B). All keep rotation on the stack (ret
// 8, frame 0x30c). Retail stores each product to two stack slots at once (the term and its copy, e.g. [esp+0x98] and [esp+0x30]),
// so its copies are real variables too, kept in memory in source order. Restored.
// M3 lane 3 r225 (plain; base 1766, frame 0x314, ret 8 against retail's ret 4 with rotation in EAX; fast builds it differently,
// frame 0x30c): `result[0] = coefficients[0]` as the first statement 1766 (same score, aligned 3009 against 3005); the matrix pointer
// declared ahead of the rotation reads 1766; rotation[7] read straight after the other eight 1766; the matrix pointer dropped for
// `workspace.matrix[...]` at every use 2027 (10501 bytes, frame 0x304, ret 8); the first copy first with rotation[7] in line 1766.
// None moves rotation into a register: the entry statement order is not the convention's cause.
// Lane 5 r166 (fast, 0x22850/0x23690/0x143250 listed, control 1766/223/401/MATCH): the nine leading rotation copies
// dropped and rotation[] read at each use (matrix a plain array) 2516 (10021 bytes; rotation then arrives in ESI, 0x23690 350);
// parameters as (result, rotation, coefficients, order), definition, declaration and all seven calls updated, 1766 (callers
// unchanged); the matrix as its own array beside the eight grouped terms 2089 (10438); `result[0] = coefficients[0];` first
// 1766; the matrix pointer as `real *const` 1766; the matrix pointer defined right after the grouped terms (top of the
// body) 1766. Both builds compute the seven sqrt(N.0) terms at run time (seven fsqrt each). All restored.
// m3 lane 14 r77: fast (convention pinned by the last full build, as lane 10 round 34 found), 0x143250 matched each time:
// the workspace with matrix[49] as its first member 1766 (frame 0x314, ret 8); band one's five single-use negations and
// copies written into their matrix stores 1791 (10740 B; a subset of round 145's first-400 form). Re-measured listed
// forms: the matrix pointer declared ahead of the rotation reads 1766, term_10 dropped 1766 (same code). Plain checks of
// round 145's declaration-count cause: every single-use real term written into its consumer within band three only
// (945 written in, 396 declarations left) 2081 (10458 B, rotation EAX and ret 4, frame 0x308; callers 0x22850 223->226,
// 0x23690 401->366), bands two and three (284 left) 2104 (10433 B, frame 0x304, ret 4); disjoint lifetimes sharing one
// term name, all (152 declarations) 3684 (12498 B, frame 0x328, ret 4), the first 400 merges only (941 left) 3145
// (12049 B, frame 0x32c, ret 4, masked difflib count 699 against 1063): any way below about 950 declarations gives
// retail's convention, but each rewrites the body's allocation. Compile-only (no /GL): rotation is loaded once at entry
// under both /Gd (EAX) and /Gr (ECX). The call deletions tried under --fast said nothing about the convention (pinned).
// M3 lane 12 r111 (plain, callers 0x22850 223 and 0x23690 401 unchanged, 0x143250 matched): workspace fields in rotation-index
// order (r0 term_5, r1 term_1, r2 term_6, r3 term_7, r4 term_0, r5 term_2, r6 term_3, r8 term_4) 1766 (67 listing lines move);
// fields in band two's first-use order (term_7, term_5, term_0, term_1, term_3, term_2, term_6, term_4) 1766 (57 lines move);
// `term_10 = workspace.term_4;` right after term_8 (retail evaluates term_8, term_10, term_11, term_12, term_13) 1766 (same
// code); the workspace as plain locals with `real matrix[49]` declared first 2113 (10412 bytes, frame 0x300 as retail, ret 8); the plain
// terms declared in rotation-index order 2113. Body restored.
// Round 87 (m3 lane 11, plain with 0x22850/0x23690/0x143250): band one's nine matrix stores read from rotation[] (its five
// negation/copy terms dropped) 1787 (10745 B, ret 8, rotation still on the stack); with the nine leading copies after
// band one's early return 1830 (10706 B); the 37 copies of copies in bands two and three (`term_N = workspace.term_K`,
// `term_N = term_9`) read from rotation[] 1780 (10788 B): rotation then arrives in ESI and result in EDI, but coefficients
// moves to the stack (still ret 8; 0x23690 401 -> 350); both 1776 (10792 B); band two's 22 only 2050 (11035 B, rotation
// ESI). Use count alone gives rotation a register but never a third register parameter (retail EAX/ECX/ESI).
// m3 lane 11 r90: no trial run. The lever this note records (fewer than about 950 declared locals for retail's EAX rotation and
// ret 4) has a dozen measured families above; each reaches the convention only by rewriting the allocation (1775-3684), and a new
// attempt needs the 1341 term declarations re-expressed per band rather than another one-step form.
// M3 lane 2 r72 (plain checks with 0x22850/0x23690/0x143250; base 1766, aligned 2934, callers 223/401 unchanged in each,
// 0x143250 matched): retail ends `ret 4` (rotation EAX) and ours `ret 8` as the prompt says (both listings' last lines).
// Bands two and three nested under `if (order > 2)` / `if (order > 3)` with one `return result;` 1766 (same code); rotation
// read through one `real const *` local copy at the nine reads 1766 (same code); every term declared at its first
// assignment instead of at the top 1766 (same code); the workspace declared after the 1341 terms (plain; earlier rounds
// measured it fast only, where the convention is pinned) 1766 (same code); the two root products term_41/term_221 stored as
// real 1766 (aligned 2952). Diagnostic (not kept): `#pragma optimize("p", on)` around the body 3960 (12712 bytes), so
// retail's source-order schedule is not the float-consistency setting.
// M3 lane 12 r120 (plain checks with 0x22850/0x23690/0x143250; the terms named term_N above are the old one-operation-per-statement
// body, replaced this round). Retail walk: its 0x300 frame is 49 matrix slots and 143 scalar homes with 215 stores for 952 values,
// so most values never touch memory; each band-two product that band three needs is stored to two homes at once (band two's own
// and a temporary read once in band three), and every xmm value is reloaded after each 0x143250 call. Causes found, in order:
// band three's copies of band-two products written as the product again 2614 (11558 B; gives the paired stores); 0x143250
// defined after this function 1766 (same code); 0x143250 declared `inline` 1819 (10714 B; callers of an out-of-line `inline`
// callee assume every xmm register clobbered, so the rotation values are reloaded as in retail; 0x143250 still matches); the
// body rewritten as expressions (nine rotation locals in retail's load order, band two's twelve repeated products named, each
// entry one expression, band three recomputing its products) with that declaration 1681 (frame 0x2f8, ret 4); the nine rotation
// locals declared r0..r8 (a product's later-declared operand is its first operand; retail's 267 operand pairs fit one
// declaration order), band two's two roots as float locals ahead of the products (`fxch st(1); fxch st(1); fmulp`, the x87
// code delayed to the first use as in retail), a product chain's unsorted head kept together as a parenthesized right operand
// (a plain chain is flattened and sorted by declaration order, constants last), entries 28, 29, 34 and 43 as plain sums of
// products (the compiler then factors them as retail has them) 1403 (10742 B, frame 0x2fc); r5_r0 declared after r4_r2, and
// the three products retail computes on their own between two entries (every use reads the home) declared as locals ahead
// of their first entry, 1333 (kept). Harness forms (build/r120/h, standalone LTCG link, positional preview in brackets against 1279 kept): both r5 pair
// locals declared between r4_r2 and r3_r2, where retail's entry 8 order puts them [1470; entry 8 then factors into three
// sums]; entry 8 with the pairs inline [1496, 1664]; every entry as a plain sum [1631; 61 entries equal]; the fourth
// between-entries product (r2_r2_s105 * s21_s2) as a local ahead of entry 19 as retail computes it [1536: the allocation at
// band three's start shifts]; the three r5 products inline everywhere [1368; operand order of S * (r5 * r1) lost]; root
// locals declared at their conversion instead of at the constant load [1574]. Left: entry 8's term order (retail r3_r2,
// r5_r1, r5_r0, r4_r2, r4_r0, r3_r1 unfactored), entry 25's four-term order, entry 28's subtracted pair, and the homes, which
// retail orders by memory-reference count (r8 with 24 reads at +0, r5 with 20 at +4, r7 with 3 at +0x11c).
// M3 lane 9 r120 (fast unless given): retail's scalar homes are ordered by reference count, and its extra home comes from two
// band-three products (stored at retail 0x144008 and 0x1444eb) that keep their own two-reference homes where ours packs them into slots
// shared with other values, so the frame follows the band-three schedule. Entry 8's first two terms swapped 1333 (same code:
// the sum is re-sorted, later-declared operand first); r3_r2 declared after r5_r1 (entry 8 then has retail's term order) 939 at
// 10777 bytes, plain 939 with 0x22850 226, 0x23690 330 and 0x143250 matched (a declaration-order change, kept only with a
// match: restored, diff in that round's held patch); on it entry 25's four operand products in the 23 other declaration
// orders 943-1474, entry 28 as T1 - (T2 + T3) 940; that grouping on this body 1330. Left on the 939 form: band three's
// preamble (the copies of band-two products into band-three homes, through xmm7 where retail uses xmm6, in another order),
// entries 25-28, and band two's entries 18, 19 and 22. A hill climb of dependency-safe adjacent declaration swaps on
// the 939 form (fast, 290 swaps) then took s105_r0_r0_r6 after r1_r0_r7 866, r7_r7_r0 after r0_r0_r0 865, r3_r3_r0
// after r1_r1_r0 854 and r2_r1 after r5_r4 853 (plain 853 at 10777 bytes with frame 0x300 as retail, first difference
// +0x3b; 0x22850 226, 0x23690 330, 0x143250 matched); every other single swap 858-1500. Restored like the 939 form.
// P476: Explicit-polynomial matrix accumulators in bands 2/3/both; coefficient accumulators in bands 2/3/both; immutable scalar captures; frequency lexical homes. Scores 1337/1535/1539/1439/1535/1539/1333/1333.
// NOT kept: unmatched diagnostic source forms; original executable body restored.
// P478: First-consumer repeated scaled mixed/cubic/triple/pair/cubic products, paired cubic family,
// complete reusable family and prepublication family: 1552/1509/1333/1759/1597/1623/1628/1619.
// NOT kept: four products recover frame 0x300, but spill homes/schedules regress; original body restored.
// P478 m3: Cross-band square/mixed/all/axial-square/transverse-square retained products,
// then all/square/mixed shared receivers with original second producers: 1918/1843/2010/1811/1827/1538/1529/1501.
// NOT kept: lifetimes regress and shorten away from retail; original polynomial restored. Frame and homes remain.
// P480 m3: Complete third-band row-producer lifetimes 0/1/2/3/4, second-band square/all
// product first-consumer declarations and all third-band products: 1810/1643/1742/1648/1627/1672/1707/1789.
// NOT kept: native frame/homes unrecovered; original polynomial and complete body restored.
// M3 lane 11 r100 (fast with 0x143250 matched in each; rm = register-masked aligned count, 169 at 1333): the band-three
// preamble is retail's instruction for instruction; what differs there is spill-slot packing. Retail has five 2-reference
// scalar homes at the top of the scalar area (0x234-0x244) where ours has three: the product r6_r0 (r6 * r0, then times r7)
// takes a home of its own in retail (0x234) and r6_r3 shares slot 0x90, where ours packs r6_r0 into 0x90. Tried: r6_r0
// renamed (band three) 1333 (same code: names do not place these homes); lane 9 r120's r3_r2 after r5_r1 re-measured 939
// (10777 B, rm 151, frame still 0x2fc in fast; a declaration move, kept only with a match); the matrix declared after
// the nine rotation locals 1333 (same code); `result[0] = coefficients[0];` first 1361; the r6_r3 and r6_r0 pairs'
// declarations exchanged 1334; band three's sqrt(2) declared at its first use 1336; band two's sqrt(5) before sqrt(15)
// 1333. Body unchanged.
// M3 lane 13 r93 (fast with 0x143250 matched, 0x22850 226 and 0x23690 330 throughout; control 1333 at 10752 B, frame 0x2fc,
// `ret 4` on both sides now): dead-local phase scan at the top (n = 0..7, removed) 1333/1425/1425/1337/1435/1435/1447/1435;
// the two early returns as nested `order > 2` / `order > 3` blocks (one exit) 1333, only the first nested 1333;
// `result[0]` stored after the nine band-one matrix stores 1333; the nine rotation locals in one declaration 1333 (all the
// same code). Body unchanged.
// P500: Current-polynomial actual 3x3/5x5/7x7 matrix lifetimes, seven subsets, then all entry declarations:
// 1420/1420/1333/1420/1420/1435/1420/1420. NOT kept: No native spill-home/frame recovery: all tie or regress and
// separate workspaces shorten away from retail. Original complete executable body restored.
// m3 lane 14 r96 (fast with 0x143250, which stays matched; control 1333 at 10752 B): band two's entry 6 written
// `r8 * r4 + r7 * r5` 1333 (same code: the compiler re-sorts each sum, so textual term order never matters); band three's
// declarations ordered so that every all-plus sum's terms run from the latest-declared local down (lane 9 r120's rule for
// entry 8, applied to all 25 pure-sum chains by a stable topological sort, seven lines moved) 1577 (10738 B), so that rule
// does not hold for every sum. On lane 9 r120's r3_r2-after-r5_r1 form (re-measured 939, 10777 B; a declaration move,
// kept only with a match): s7_r7_r6_r6 after s7_r7_r7_r7 1536 (10789 B), s7_r6_r7_r7 after s7_r6_r6_r6 1357, r2_r2
// and r2_r2_s105 after s35_r2_r3_r3 1597 (10739 B). Body unchanged.
// P511: Standard real-return roots, band-two 15/5 and band-three 2/35/105/21/7, then all seven:
// 1307/1307/1324/1322/1322/1324/1324/1266. NOT kept: nonexact root-return/precision contracts only change spill/scheduling;
// qword fsqrt producers, frame 0x2fc versus 0x300 and displaced scalar homes remain. Original polynomial restored.
// M3 lane 12 r127 (fast with 0x143250 matched, 0x22850 226, 0x23690 330; control 1333, register-masked aligned count 171):
// retail gives band three's r6_r0 its own home (0x234, stored at retail 0x144008); its two uses are r6_r0_r7 and r6_r0_r8.
// r6_r0_r8 declared right after r6_r0_r7 1567 (10734 B); r6_r0_r8 declared just ahead of band three's matrix[25] 1531
// (10708 B); r6_r3_r8 declared right after r6_r3_r7 1560 (10739 B); both moves together 1563 (10723 B); r6_r3_r8 declared
// just ahead of band three's matrix[23] 1532 (10712 B). Body unchanged.
// P521: Current complete matrix initializers, band masks 1/2/4/3/5/6/7 and all immutable:
// Third-band initializer alone recovers native frame 0x300 and first +0x4c, but regresses to 1498.
// 1420/1410/1498/1420/1556/1535/1556/1556. NOT kept: no exact native matrix/scalar-storage recovery; original complete body restored.
// P536: Separate double sqrt producers with unchanged real narrowing,
// individual 15/5/2/35/105/21/7 roots then all: 1401/1401/1410/1410/1410/1410/1410/1377.
// Native frame 0x300 and first +0x3b recovered, but extent shrinks to 10716/10773.
// NOT kept: unmatched one-use result carriers regress and worsen the length gap;
// original roots, precision boundaries, polynomial and complete body restored.
// M3 lane 11 r106 (fast with 0x143250 matched; control 1333): retail recomputes r6 * r3 and r6 * r0 in band three (its
// 0x90 and 0x234 homes are stored from fresh `mulss` at 0x143fe9/0x144008), so band two's products are not reused there.
// Band two's `s15 * s5` 1333 and band three's `s2 * s35` 1333 (same code: products re-sorted); band one's four negatives
// as unary minus 1333 (same code: compiled as 0 - x); band three's sqrt(7) declared after s21_s35 1535 (10748 B); band
// two's root product as one `(real)sqrt(5.0) * (real)sqrt(15.0)` 1729 (10712 B).
// P566: Actual positive-root input/result tables, second/third/both bands through
// long for/while, then third/both short for: 1671/1671/1592/1592/1751/1751/1748/1771.
// NOT kept: grouped root-pipeline storage is unmatched and regresses; native paired
// scalar homes and missing frame word remain. Original polynomial and full body restored.
// P580: Current-polynomial reused rotation scalar memcpy, seven complete-row subsets,
// then all scalar memmove: 1559/1754/1148/1770/1768/1582/1790/1865. NOT kept: no exact paired-home/frame recovery;
// the 1148 transfer moves the frame farther from native 0x300 to 0x2f8; unmatched self-address storage restored.
// original nine captures, polynomial, publications, helper calls and full body restored.
// M3 lane 8 r175 (fast with 0x143250 matched; control 1333 at 10752 B, `ret 4` on both sides, frame 0x2fc against 0x300; the
// listing is retail's instruction stream almost line for line, the count is stack-home placement): band three's r6_r0 written into
// its two products 1656 (10698 B); band three's r6_r3 the same 1663 (10700 B); band two's entry 18 stored last, just ahead of its
// 0x143250 call (retail stores it after the call's pushes) 1544 (10755 B); r5_r2_s105_s2_s21 and r5_r5_s21_s35 written at their uses
// 1333 each (same code). Body unchanged.
// M3 lane 1 r114 (fast with 0x22850/0x23690, both at their baseline): the convention is already retail's (rotation EAX,
// coefficients ECX, result ESI, order on the stack, ret 4); the 49-entry matrix tops the frame in both, so retail's
// extra 4 bytes are one more scalar spill home below it (0x23c bytes against 0x238). s5 folded into `s5_s15 =
// (real)sqrt(5.0) * s15` 1728 (10712 B); the nine rotation reads as `real const` 1333 (same code); band one's negatives as
// unary minus 1333; the matrix declared after the rotation reads 1333; band three's s2 declared beside s35 1333.
// Round 96 (m3 lane 2, fast with 0x22850/0x23690; control 1333, register/stack/global-masked aligned 171): every scalar
// local renamed so the names sort in the reverse order 1333 (same score: names do not order these homes); a table per
// band declared in its band's block 1420 (10716 B); band three's table alone in its block (bands one and two sharing a
// 25-entry table) 1333; the ten products bands two and three share declared once ahead of band two 2031 (10475 B: retail's
// paired stores go); `result[0] = coefficients[0]` after band one's table fill 1333 (re-measured: the matrix declared
// after the rotation reads 1333). Diagnostic: `#pragma optimize("s", on)` around the body 3394 (9134 B, an EBP frame), so
// retail's source-order schedule is not the size preference either. Retail's band-three entry 8 is the source's
// left-to-right sum, with its partial sum spilled once to a scalar home.
// Named scalar prefixes for third-band entries 8 1376.
// Named scalar prefixes for third-band entries 25 1379.
// Named scalar prefixes for third-band entries 28 1333.
// Named scalar prefixes for third-band entries 8, 25 1411.
// Named scalar prefixes for third-band entries 8, 28 1376.
// Named scalar prefixes for third-band entries 25, 28 1379.
// Named scalar prefixes for third-band entries 8, 25, 28 1411.
// Compound-assignment prefixes for third-band entries 8, 25, 28 1303.
// Compound-assignment prefixes for third-band entries 8 1315.
// Compound-assignment prefixes for third-band entries 25 1337.
// Compound-assignment prefixes for third-band entries 28 1333.
// Compound-assignment accumulation through the entire third-band entry 8 939.
// Kept the complete six-term accumulation for entry 8, which restores the separate multiplies and the four-term spill. NOT kept: the prefix-only forms tie, regress or still fold the final pair.
// Weighted squares produced together 1556; both cubic consumers at each
// weighted square 1612; paired sqrt(35) components 1498, with derived squares
// 1566; named entry-27 weighted product 939; const weighted products 939;
// two-step weighted-component consumers and weighted cubic consumers 939/939.
// The dedicated weighted product homes and the extra frame word still differ.
// M3 lane 3 r263 (fast; base 939; with stack offsets masked 219 of 1768 lines differ): running sums like entry 8's for entry 12's
// six products 939, entry 36's six triples 939, entry 40's eight terms 939, entry 22's seven terms 939 (all the same code: sums
// with constant factors already run left to right); entry 29's seven terms 1253 (10821 B).
// Third-band shared weighted mixed/first cube/axial component/axial pair/axial square/second cube products
// 1517/1477/1357/939/1314/1554; both cubes 1589; all independent factors 1629.
// NOT kept: the mixed product and the two individual cube forms recover frame 0x300 with displaced
// spill homes and higher scores.
// M3 lane 14 r115 (fast with 0x143250 matched, 0x22850 226 and 0x23690 330 unchanged; control 939 at 10777 bytes, `ret 4`
// with rotation in EAX on both sides; aligned 650, 122 with the stack offsets masked, so the count is almost all home
// placement): entry 25 as a running sum like entry 8's 1095 (aligned 638), entry 28 the same 1496 (10804 bytes), both
// 1494; the nine rotation reads declared in retail's load order (4, 1, 5, 6, 8, 0, 2, 3, 7) 1749 (10827 bytes), in reverse
// order 1716 (10791 bytes).
// Diagnostic: a one-object LTCG link of this file (scratch copy, an entry calling the function, no other objects) gives
// this build's 1768 instructions exactly and names every home. The homes are 4-byte slots shared by locals with disjoint
// lifetimes, stacked from the bottom roughly by memory-reference count. Retail's slots follow ours one cell lower up to
// the r6_r6 slot; retail then has the s21/s21_r2 slot (ours ranks it far higher, at -656), which is the extra frame word;
// above r2_r2 the offsets agree again until the band-three terms, which retail interleaves in a different order.
// M3 lane 16 r144 (fast, control 939): the nine rotation reads declared in retail's load order (r4, r1, r5, r6, r8, r0,
// r2, r3, r7) 1749 (10827 B); `matrix` declared after the reads 939 and band two's roots s5 first 939 (both the same
// code); one exit with bands two and three nested under `order > 2` / `order > 3` 1902 (10607 B); unused initialised
// locals after `matrix` (diagnostic only) 1-7: 1231 (10744 B)/1231/1095 (10777 B)/1226/1226/1415 (10756 B)/1226.
// Band two's braces dropped (its names at function scope, band three's block shadowing them) 939 (same code).
// M3 lane 15 r109 (fast with 0x143250 matched; control 939): retail reads s21's home eight times (0x4c: s105_s2_s21 multiplies
// from memory, then band two's eight copies, then s21_s2 loads it again), ours seven (one load in XMM6 serves s105_s2_s21 and
// s21_s2, and the copies move behind it). s21_s2 declared after s7_s2_s21 1104 (10780 B); right after s105_s2_s21 1021
// (10775 B); beside s21_s35 1299 (10774 B); band three's r6_r3/r6_r0 as `r3 * r6`/`r0 * r6` 939 (same code); band two's r6_r3
// and r6_r0 written into their four uses 1718 (10752 B). Body unchanged.
// M3 lane 15 r111 (fast with 0x143250 matched in every form; control 939, `ret 4` on both sides, 214 rows differ once the
// stack offsets are masked). A symbolic trace of both listings (m3-lane15 build/r111/trace.py and order.py: every xmm
// and home value as a product of r0..r8 and the roots) gives the row at which each band-three local is first defined:
// 139 of the 154 are defined in the same order on both sides. The 15 that are not: around band three's copies of
// band-two products retail defines r4_r2, r6_r4, r3_r2, s105_s2, r6_r5, s7_s105, s105_s2_s21, r6_r1, r6_r2 ... r8_r8, s21_s2
// (its copies r6_r4 and r6_r5 go through XMM7 ahead of s7_s105, the later ones through XMM6 once s105 is spent), where
// ours loads s7 into XMM7 twelve rows early, defines s7_s105 first and s21_s2 thirteen rows early; r6_r3_r8 comes after
// s21_r5 in ours (before it in retail); and retail's order r7_r7_r0, r0_r0_r0, r3_r3_r0, r4_r4_r0 is r0_r0_r0, r7_r7_r0,
// r4_r4_r0, r3_r3_r0 in ours. Declaration moves taken from that (each kept only with a match, so all restored):
// r6_r5 declared right after s105_s2 880 (10776 B; masked rows 216); on it s105_r0_r0_r6 after r1_r0_r7 807 (frame
// 0x300, first difference +0x3b, masked 217), r7_r7_r0 after r0_r0_r0 892, r3_r3_r0 after r1_r1_r0 868, r2_r1 after
// r5_r4 881, those four together 797 (masked 223), r3_r3_r0 ahead of r4_r4_r0 900, r6_r1 ahead of s105_s2_s21 1484,
// r6_r4 ahead of s7_s105 1424. On the control: r3_r3_r0 ahead of r4_r4_r0 955; r6_r4 and r6_r5 both ahead of s105_s2
// 1422; r6_r4 after r4_r2 with r6_r5 after r3_r2 1422; r6_r4 after r4_r2 with r6_r5 after s105_s2 1424; r6_r4 after
// r4_r2 alone 1423; r6_r4 ahead of r3_r2, r6_r5 after s105_s2 and r6_r1 ahead of s105_s2_s21 1556. The positional
// count falls with these moves while the masked count does not (214 at best on the control), so they move the homes,
// not the schedule. 0x143250's parameters in the orders its three call setups could imply, definition, declaration
// and calls together, the callee matching in each: (n, out, a, b) 939, (n, a, out, b) 936, (a, b, n, out) 935,
// (a, out, n, b) 936, (out, a, n, b) 939, (a, n, out, b) 936. Body unchanged.
// First-band matrix carries r0/r1/r4 into the higher-band computations:
// singles 1736/1739/1657, pairs 1770/1732/1750, all 1782 before or after
// the order guard. NOT kept: the reloads regress and retail uses independent scalar homes.
// M3 lane 1 r133 (fast with 0x143250, matched unless said; control 939, 82 rows differ with registers and stack offsets
// masked). Lead with retail evidence, not kept (4 points, under the bar; build/r133/lead-0x143600-helper-params.patch in the
// m3-lane1 worktree): 0x143250's parameters declared (n, matrix, coefficients, out) instead of (out, n, matrix,
// coefficients), definition, declaration and the three calls: 935 with 0x143250 still matching. Retail's three call sites
// then come out exactly: the out argument's `lea eax` ahead of the matrix push, and the scratch registers ECX/EDX at the
// 5x5 call and ECX at the 7x7 call (EAX is already taken by out when the two stack arguments are formed; with out
// declared first ours takes EAX/ECX and EDX there). (n, matrix, out, coefficients) 936 (the lea order only, registers as
// before); (matrix, n, coefficients, out) and (matrix, coefficients, n, out) 935 (same code); (coefficients, matrix, n,
// out) 957 with 0x143250 at 4 (the pushes swap). The order of the stores into the matrix is retail's in bands one and
// three; in band two retail computes entry 18 in place but stores it last, with the call's pushes (after entry 23), which
// shifts some 45 rows by one: entry 18's statement moved after entry 23 on the lead 1517 (its products move with it); its
// value named where retail computes it and stored after entry 23 1597 (10747 B: the one-use local is written back into
// the store); that value held through a reference and stored after entry 23 1221 (10744 B, diagnostic).
// The home of sqrt(21) is read once more in retail (a memory multiply for s105_s2_s21 and a second
// load for s21_s2, after the eight copies): ours loads it once into XMM6 for both, so the eight band-two copies go
// through XMM7 where retail has XMM6; that follows from s7_s105 being scheduled earlier here, a
// schedule difference like the others (the block is scheduled as a whole, so every remaining difference moves the rest).
// Body unchanged.
// The sqrt(21) home and inter-band spill schedule still differ.
// first mixed-root accumulation 939, later mixed-root accumulation 939, both mixed-root accumulations 939,
// four root-product accumulations 939, weighted component and square accumulations 939, root-product names 939,
// separate root assignments 939, axial squares and cubic accumulations 1848.
// NOT kept: The axial accumulations shrink the frame to 0x2f8 and raise the score. The other forms retain 0x2fc.
// Second-band axial weight named at first use, with the other products, or by compound multiplication
// 1231/1296/1231. With named transverse products, transverse accumulations, or named up products
// 1095/1095/1095. With all four products or the diagonal half-square terms 1226/1226. NOT kept: All
// eight raise the score and leave the frame four bytes short.
// Input/output/both coefficient slices advanced after admission 1652/1706/1723.
// Input/output/both slices advanced after the calls 1625/1725/1701.
// Shared coefficient offset advanced after admission or the calls 1231/1231.
// NOT kept: The advancing forms retain the 0x2fc frame and do not recover the displaced scalar homes.
// Complete running sums for the multi-term third-band coefficients, rows 1 through 7,
// 939/939/939/1095/1520/939/1196. Paired running sums at entries 23 and 25 1095.
// NOT kept: All tie or regress, and the scalar spill frame stays four bytes short.
// Third-band products declared at their first coefficient: initial/later/both
// mixed roots 1548/1497/1556; third mixed root 1515; component-weight families
// at entries 23/25/both 1669/1689/1655; all product groups 1671. NOT kept:
// the first three recover frame 0x300 but displace the scalar homes and
// regress. The other forms retain the short frame.
// The third-band scalar homes still leave the frame one word short. Running sums for paired mixed axial,
// transverse diagonal, axial-transverse, four axial diagonal, weighted-square interior, weighted-component
// outer/interior, and transverse conversion coefficients 939/939/1253/939/939/939/1095/939.
// NOT kept: the accumulations leave the frame at 0x2fc and either tie or raise the score.
// Third-band coefficient-local cubic products of r6 times r0 or r3, or both pairs 1560/1559/1537.
// Coefficient-local square products of weighted r2 or r5, or both families 1533/1589/1611.
// Coefficient-local consumers of both dedicated-home products 1399; all four families 1579.
// NOT kept: the forms raise the score and do not recover the 0x300 frame.
// capture with matrix store for r1 1740; capture with matrix store for r4 1721;
// capture with matrix store for r1, r4 1748; capture with matrix store for r0 1717;
// capture with matrix store for r0, r1 1766; capture with matrix store for r0, r4 1742;
// capture with matrix store for r0, r1, r4 1788; capture with matrix store for r0, r1, r3, r4, r8 1832.
// NOT kept: The early matrix stores displace the capture and spill schedule, miss the retail frame and raise the score.
// M3 lane 8 r207 (fast with 0x22850 209 and 0x23690 354 unchanged each run; control 939 at 10777 B, `ret 4`, rotation in
// EAX): of 1766 listing lines 600 differ only in a stack offset and 352 in the instruction; the matrix and everything
// above it sit 4 bytes lower than retail's, the scalar homes from +0x48 to +0x240 are permuted. The matrix declared after
// the nine rotation reads 939; r0..r8 renamed (diagnostic: slot order does not follow these names) 939; `s5_s15` as
// `s15 * s5` 939; every band temporary but the accumulator `real const` 939 (all the same code); the third band's
// accumulated diagonal sum as one expression stored to matrix[8] 1333 (10752 B).
// Third-band coefficient-local cubic products of r6 times r0 or r3, or both pairs 1560/1559/1537.
// Coefficient-local square products of weighted r2 or r5, or both families 1533/1589/1611.
// Coefficient-local consumers of both dedicated-home products 1399; all four families 1579.
// NOT kept: the forms raise the score and do not recover the 0x300 frame.
// Stage the mixed/axial/square weights at coefficients 23 and 25, singly 1213/939/1213,
// paired 1213/939/1213, all weights 939, and complete mixed terms through the final
// factor 1154. NOT kept: no form recovers frame 0x300 and the shorter forms move
// away from the retail extent.
// One recalculated product local across the two higher bands, for r6*r0/r6*r3/both 1216/1231/1238,
// transverse mixed pair 1282, axial squares 1281, diagonal squares 1296, off-diagonal squares 1205,
// and the r0 mixed pair 1280. NOT kept: the off-diagonal squares reach frame 0x300 and first +0x39,
// but all forms shorten the body past retail and raise the score.
// Direct accumulation into the matrix, third-band rows 1 through 7 independently, then all seven
// rows.
// Scores 1472/1380/1608/1324/1632/1220/1203/1611.
// NOT kept: Row 1 reaches retail length and row 2 restores frame 0x300. Both raise the score. The
// previous scalar spills remain.
// Separate third-band copies of the second-band axial/horizontal mixed products
// 1697/1721 and axial/horizontal squares 1751/1791. Both mixed sets 1792, both
// square sets 1866, all products 1959, all immutable copies 1959.
// NOT kept: the explicit copies shorten past retail and raise the score.
// Separate third-band declarations for the up/forward pair, root-35/forward
// component, both or both symmetric families 939/939/939/939.
// Band-specific mixed names, root-35 axis names, both root axis families,
// and all those role names 939/939/939/939. The scalar spill frame stays 0x2fc.
// M3 lane 8 r214 (fast with 0x143250 matched and 0x22850 209 / 0x23690 354 each run; control 939, frame-blind aligned
// count 154): this function's parameters in retail's call-setup order (result, rotation, coefficients, order; the callers
// set up ECX, EAX, ESI after pushing the order), definition, declaration and the seven calls 939 (same code; 0x23690 357);
// 0x143250 as (n, matrix, coefficients, out) re-measured 935 (its three call setups then retail's; 4 points); both
// reorders 935 (0x23690 357); on that 935 form one array per band declared in its band's block (9, 25 and 49 reals)
// 1195 (10744 B, frame 0x320) and the first coefficient copy after band one's stores 935 (same code).
// M3 lane 14 r135 (fast, control 939; `ret 4` in both builds): entry 23's first product with `r6_r3 * r8` for r6_r3_r8 1601
// (10732 B); entry 23 as `-(subtracted product) + first product` 1221 (10762 B); entry 23's first product named just above
// it 939 (same code: the order is set after the homes); its subtracted product with `s21 * r5` for s21_r5 1147 (10783 B);
// entry 37's r6_r3_r8 written `r6_r3 * r8` 1571 (10765 B). Retail keeps 143 scalar cells under the workspace, this build
// 142; 428 of the 935 differing lines differ only in stack offsets.
// Second-band named products for sums, ordinary differences, zero-minus differences, or all paired coefficients
// 1226/1415/1226/1226. Running accumulators for the same groups 1229/1095/1226/1229. NOT kept: all retain the
// 0x2fc frame and raise the score.
// M3 lane 2 r128 (H2M_FAST with 0x143250, MATCH; control 939, aligned 777, register/offset-masked 154; the convention is
// already retail's: rotation in EAX, ret 4): third-band entries 23-25 in another statement order (value trees kept),
// 25 first 1113 (10773 B), 24 first 942, 23 last 1114 (10773 B); entry 23 accumulated in place over four statements 939
// (same code); entries 23 and 25 both accumulated 1095. Reading: the masked rows are scheduling and spill placement
// spread over the body (retail keeps 0.071428575 in XMM5 for entries 23 and 25), and retail's one extra spill cell
// sits below the 49-entry matrix (frame 0x300 against 0x2fc).
// Higher bands read back first-band entries for r0/r1/r4: 1754/1741/1658;
// pairs r0-r1, r0-r4, r1-r4 1770/1767/1743; all three 1785; all five positive entries 1816.
// NOT kept: the forms regress and move the scalar frame away from 0x300.
// M3 lane 2 r131 (H2M_FAST with 0x143250 MATCH; control 939, aligned 777, masked 154; aimed at the s21/s21_r2 cell): s21_r2
// written into its three uses 1507 (10747 B); s21_r5_r6_r6 as `r6_r6 * s21_r5` 939, s7_s2_s21 as `s21 * s7_s2` 939 and entry
// 25's last product with s7_s2 first 939 (each the same code); s21_s35 from its own root with s21 declared after it 1491
// (10773 B, frame still 0x2fc). Body unchanged.
// M3 lane 1 r155 (H2M_FAST; control 939): entry 23's first product as `r6_r3 * r8 * ...` with the named product kept for
// its other use 1601 (10732 B); entry 25's as `r6_r0 * r8 * ...` 1517 (10757 B); both 1592 (10743 B); the matrix declared
// after the nine rotation scalars 939, the band-two constant product as `s15 * s5` 939 (same code each).
// M3 lane 16 r175 (H2M_FAST; control 939): outside the matrix our cells hold 59/63/16/4 values with 1/2/3/4 stores, retail's
// 60/64/15/4: one cell of ours is shared by three values (a band-two square, a band-three product and a later value) that
// retail keeps in two cells, so the short frame is a cell our packer reuses, not a missing value. Band two's three unnamed
// squares named like its other six (entries 12 and 14) 1095; r8's square alone 1231 (10744 B); r2's and r5's alone 1231
// (10744 B); its three unnamed mixed products named (entries 10, 11, 13) 1710 (10816 B); all six 1648 (10792 B).
// M3 lane 11 r156 (H2M fast with 0x143250 MATCH, 0x22850 209 and 0x23690 354 unchanged; control 939, aligned 777): `s5`
// declared ahead of `s15` 939; band one's negations as unary minus 939 (same code each); the order tests as `order < 3` /
// `order < 4` 894 (10776 B, aligned 781: positional only, the first test becomes `cmp ebp, ebx; jb` on the call's 3 where
// retail has `cmp ebp, 2; jbe`, so not kept); 0x143250 as (n, matrix, coefficients, out) re-measured 935, with `s5`
// declared first 935. Body unchanged.
// M3 lane 12 r177 (H2M_FAST with 0x143250, MATCH; control 939): our cell +0x8c holds band two's r5 square, band three's
// r2-square-times-root-105 product and a later value; retail keeps the second in its own cell under the matrix (+0x234).
// Band three's r2_r2_s105 declared ahead of r5_r5_s105 939, r5_r5 ahead of r2_r2 939 (both the same code); r2_r2 and
// r5_r5 at the top of band three 1569 (10780 B); r8_r8 after r5_r5 944 (10781 B); band two's s5 and s5_s15 declared
// just ahead of entry 0 1134 (10763 B, frame 0x2f8). Body unchanged.
// M3 lane 6 r235 (H2M_FAST with 0x143250 MATCH; control 939): retail's extra cell (+0x234 under the matrix) holds band three's
// root-105 times r1-square product alone (stored once, read once for its r6 product); ours shares that product's cell
// with two other values. That product's r6 product declared right after its r7 product 1567 (10728 B); the product
// written into its two uses 1226 (10744 B); its operands as `r1_r1 * s105` 939; entry 0 of band two as
// `r3 * r1 + r4 * r0` 939; the three band calls with `&result[n]` / `&coefficients[n]` 939 (same code each); band three
// nested in band two's block reusing band two's ten shared products (no redeclarations) 1994 (10500 B: retail keeps the
// second home of each shared product, so band three's redeclarations are retail's). Body unchanged.
// M3 lane 3 r303 (h2m fast with 0x143250 MATCH; control 939; "masked" = aligned count with stack offsets and constants
// masked, control 214, from a one-file link-step screen that reproduces the fast build's 1768 instructions): the s35 * r2
// product written out at its three uses with no named local 1282 (10793 B: the products regroup as (s35 * r4_r4) * r2);
// only band three's entry 27 using the written-out product 1155 (10784 B, frame 0x2f8); band two's entry 1 as
// `0.0f - r6 * r4 - r7 * r3` 1061 (10775 B, masked 218), as `0.0f - (r7 * r3 + r6 * r4)` masked 218, with the factors of
// either product swapped or the leading zero as a unary minus masked 214 (same code); 0x143250's parameters in all 24
// orders, definition, declaration and the three calls: masked 204 to 222 with the frame 0x2fc in every one ((n, matrix,
// coefficients, out), the listed 935 form, is the 204: only the three call setups move). Diagnostic dead-local scans at
// the function top, band two's top and band three's top, K = 0..40: the same 32-period at each anchor (masked 214, 253,
// 253, 205, 228, 228, 275, then 228 for K = 7..31), frame 0x2fc at every count. Reading from a value-by-value map of the
// stack cells (each store with its loads, paired through the masked alignment): both builds hold 298 stored values with
// the same peak of 142 live at once; retail has one cell more because two of its values sit alone. One is the
// root-105 times r1-square product (stored once, read once), which ours packs with a band-two product and a later
// value; the other is band three's s35 * r2: retail copies it to XMM7 right after storing it (`movaps xmm7, xmm3`,
// with the pending r1_r1_r8 stored first), forms the r3_r3 product in that register and reads the home once, for entry
// 27, where ours reloads it for the r3_r3 product (two loads) and packs it with the first band-three spill. Seven
// band-two homes are then re-used by different later values in the two builds. The first code difference with identical
// bytes ahead of it is band two's entry 1, where retail forms the shared r6 * r4 product ahead of the zero and ours
// behind it; retail also holds band two's entry 18 in XMM5 until the call where ours stores it at once. Body unchanged.
// M3 lane 16 r184 (H2M_FAST with 0x143250 MATCH; control 939, aligned 777, masked 154). Reading of the frame packer on
// this build: 142 four-byte cells; the cells that end up highest, just under the matrix, are the ones holding a single
// value with two references. Retail has four of those where this build has three, and the extra one is band three's
// s35 * r2 (stored once at +0x23c, read once for entry 27): retail stores the pending r1_r1_r8, copies the product to
// XMM7 and multiplies that copy by r3_r3 five statements later, where this build reloads the home for the r3_r3 product,
// so here the value has three references and shares band two's r3-square cell. Every integer register web but the two
// 0x143250 argument set-ups is already retail's; 173 of 192 stack cells sit elsewhere. Tried on the s35 * r2 value: its
// r4_r4 product with the operands exchanged 939, its r3_r3 product exchanged 939, the value declared const 939, written
// `r2 * s35` 939, entry 27's first product as `s7_s2 * s35_r2` 939 (the same code each); its r4_r4 product declared
// beside the r3_r3 product 1368 (10783 B). Body unchanged.
// The extra dedicated scalar cell is still missing. Root-35 component-2 product before the component-1 axial
// cube 1410, cube between weighted product and first use 1411, adjacent square consumers 939, adjacent
// weighted component pairs 990. Root-105 component-1 square alongside component-3 1475, its pair after the
// component-0 pair 945, its component-7 product alone beside component-0 1470, and four weighted squares
// before their component-7 consumers 1556.
// M3 lane 6 r240 (H2M_FAST with 0x143250 MATCH, 0x22850 209 and 0x23690 354 each run; control 939, offset-masked aligned
// rows 214 in 109 runs). Reading of the first masked rows: band two's entry 0 store and the zero for entry 1 trail the
// r6 * r4 product in retail and lead it here, and in entry 10 retail multiplies the bracket by s5_s15 ahead of the r4_r1
// chain; both are equal-priority picks of the instruction scheduler (the earlier-generated instruction goes first), so
// they follow the order the values are generated in, not the expression shapes. Band two's r6 * r4 named after its
// squares (band three shares it) 1231 (10744 B, masked 253); entry 0's two products named 1231 (same code); all three
// named 1095 (10777 B, masked 205): the one-, two- and three-symbol steps of lane 3 r303's dead-local scan, so these
// names act only through the symbol count. On the s35 * r2 value: its r3_r3 product alone written `s35 * r2 * r3_r3`
// 1417 (10795 B, frame 0x300 as retail and first difference +0x3b, masked 226); its r4_r4 product alone written out 1476
// (10793 B, frame 0x2fc, masked 184); both written out with the named value left for entry 27 1322 (10810 B, masked
// 278); the r4_r4 product declared just ahead of the r3_r3 product 1368 (re-measured); both products with the named
// value as the second operand 939 (same code). Body unchanged.
// The extra dedicated scalar cell is still missing. Root-35 component-2 product before the component-1 axial
// cube 1410, cube between weighted product and first use 1411, adjacent square consumers 939, adjacent
// weighted component pairs 990. Root-105 component-1 square alongside component-3 1475, its pair after the
// component-0 pair 945, its component-7 product alone beside component-0 1470, and four weighted squares
// before their component-7 consumers 1556.
// Round 467: in-place multiplies at the original weighted root-35 and root-105 product
// producers 939/939, both 939, each pair of consumers 939/939, each producer with
// its consumers 939/939, and both complete families 939. No change to the 0x2fc frame,
// early basis spills or higher-band product homes.
// GB r1 (fast with 0x143250 MATCH; control 939, masked 152): inlining band-3 product locals into matrix[0..48] (all 2454,
// band-2 copies only 2103, 3+ factor triples 1678, root products 2034, the 5 preamble-mismatched locals with 0x143250 retail
// param order 1648 at frame 0x300) and inlining band-2 product locals 1869 all confirm retail declared those intermediates;
// inlining r1_r1_r8 1233, r2_r0_r6+r5_r4_r7 1493, r1_r1_r8+s105_r1_r1+param order 1453; 0x143250 retail param order
// (n, matrix, coefficients, out) + matrix[28] T1-(T2+T3) grouping 936 (10781 B, below keep bar), + m23/m25 factoring 1150;
// band-2 s5_s15 factoring + param order 1712. Body unchanged.
// Third-band mixed terms named at 21/27 and 35/41 939/945. Square terms at those pairs 1183/939.
// Grouped sums before or after scaling 939/939. All terms staged at 21/27 or 35/41 939/939.
// NOT kept: the shared spill home and 0x2fc frame persist.
// M3 lane 8 r237 (h2m fast with 0x143250 MATCH; control 939): band three's declarations around the s35 * r2 value, which
// retail keeps in XMM7 from its store to the r3_r3 product: the r2_r0 and r5_r4 products ahead of r1_r1_r8 945; s35_r2 and
// its r4_r4 product right after r1_r1_r8 945; its r3_r3 product right after r3_r0_r5 939 and right after its r4_r4
// product 939 (same code each); r4_r1_r5 ahead of the two s35 cube products 1376 (10779 B). Body unchanged.
// Native rotation basis captures forward/left/up 1209/1232/1272, pairs
// forward-left/forward-up/left-up 1450/1446/1476, all rows/full matrix 1321/1328.
// NOT kept: typed captures do not resolve the independent scalar homes
// at the opening extent.
// The extra scalar word and first-band spills still differ. Moving third-band products
// confined to a column to their first consumer gives 1600/1552/1576/1602/1574/1585/1590;
// moving all seven product groups gives 1698. NOT kept: the second column recovers
// frame 0x300 at 10765 bytes and first +0x3b, with a higher score.
// Late axial and transverse mixed-product consumers, alone and together, 1540/1460/1538;
// intermediate axial and transverse pairs 1534/1493; late root-35 squares, root-105
// mixed consumers and both weighted pairs 1368/1511/1515. NOT kept: the last two
// recover frame 0x300 and first +0x39, but shorten the body and raise the score.
// Third-band weighted product declarations at band entry for the root-35
// transverse group, root-105 axial group, both, and the three complete
// family selections all 939. Component-first names for either family 939.
// The 0x2fc frame and separate weighted spill homes still differ.
// The first three basis spills and the missing scalar word still differ.
// Component names for forward j, left j, forward i, their pairs and all three,
// or the complete nine-component set, all 939. The original names are restored.
// M3 lane 6 r245 (h2m fast with 0x143250 MATCH, 0x22850 209 and 0x23690 354 each run; control 939). LEAD 918 at retail's
// 10773 B (not kept: a declaration move, match-only): `s35_r2_r3_r3` declared right after `s35_r2`, ahead of its r4_r4
// product (held as a patch). It gives retail's handling of s35 * r2: stored once
// at retail's +0x23c, copied to XMM7 (`movaps xmm7, xmm3`) and multiplied by r3_r3 there, but the frame stays 0x2fc, so the
// other lone cell (the root-105 r1-square product) is still packed with other values. Other placements of the s35 * r2
// group: `s35_r2` alone right after `s35` 1327 (10743 B); the group with both products after `s35` 1555 (10773 B); the
// group just ahead of `r1_r1_r8` 1000 (10773 B); the group after `r3_r0_r5` 941. On the 918 lead: each root-105 square's
// r6 product declared ahead of its r7 product, r1/r0/r3/r4 squares 1583/1582/1583/1564 (about 10722 B), all four 1632
// (10681 B); `s35_r5_r0_r0` right after `s35_r5` or after `s35_r5_r1_r1` 930/930; `s21_r5_r7_r7` right after `s21_r5` or
// after `s21_r5_r6_r6` 1508/1508 (10757 B). A forced-ranking search (12 of about 5000 decisions) moves nothing.
// The root-105 component-1 square and root-35 component-2 product still
// lack separate spill homes. Copy then multiply for either weighted pair, both, the
// full axial or transverse family, either family plus the other pair, and
// both full families all 939. The frame stays 0x2fc against retail's 0x300.
// GB r17 (fast with 0x143250 MATCH, 0x22850 209, 0x23690 354; control 939 at 10777/10773 B): matrix3x3 const *rotation
// param (updating 0x22850/0x23690) 939 (same code), + 0x143250 param order (n, a, b, out) 935 (-4, under bar); band-2
// (real)(sqrt(15.0)*sqrt(5.0)) / reverse 1692 (10740 B); band-3 double s21_d = sqrt(21.0) with (real)(s21_d * s35) 1559
// (frame 0x300, +0x39, 10739 B); inlining mid-matrix locals r4_r0_s105_s35_s2 / r5_r5_s21_s35 / r5_r2_s105_s2_s21 / all
// 939/939/1563/1563; hoisting them to preamble 1528/1036/982 (frame 0x300, +0x3b)/1487 (frame 0x300, +0x3b); naming
// r2_r2_s105_s21_s2 before matrix[19] / in preamble 1314 (10773 B) / 1563; unparenthesizing (r2_r2_s105*s21_s2) 1260;
// dropping band-3 r5_r0/r5_r1/r5_r5 locals 1466; matrix[8] direct += accumulation 939 (same code); on 0x143250 param
// order + matrix[28] T1-(T2+T3): matrix[25] sub-pair 1152, grouped 1225, matrix[28] factored 936. Body unchanged.
// M3 lane 8 r241 (H2M_FAST with 0x143250 MATCH; control 939). On lane 6 r245's 918 lead (`s35_r2_r3_r3` right after
// `s35_r2`, re-measured 918 at 10773 B), aimed at the root-105 r1-square cell: its r6 product right after its r7 product
// 1583 (10722 B), the r0 and r3 squares' the same 1582/1583, all four 1619 (10684 B); the r1 square's r6 product moved up
// into the r6 group 981, moved after the r0 square's r6 product 919; the r1 square and its r7 product declared just ahead of
// its r6 product 1554 (10750 B); the r7 product alone moved there 1504 (10741 B). Body unchanged.
// @retail 0x143600
real *function_143600(real const *rotation, unsigned long order, real const *coefficients, real *result)
{
	real matrix[49];
	real r0 = rotation[0];
	real r1 = rotation[1];
	real r2 = rotation[2];
	real r3 = rotation[3];
	real r4 = rotation[4];
	real r5 = rotation[5];
	real r6 = rotation[6];
	real r7 = rotation[7];
	real r8 = rotation[8];

	result[0] = coefficients[0];
	matrix[0] = r4;
	matrix[1] = 0.0f - r7;
	matrix[2] = r1;
	matrix[3] = 0.0f - r5;
	matrix[4] = r8;
	matrix[5] = 0.0f - r2;
	matrix[6] = r3;
	matrix[7] = 0.0f - r6;
	matrix[8] = r0;
	function_143250(result + 1, 3, matrix, coefficients + 1);
	if (order <= 2)
		return result;
	{
		real s15 = (real)sqrt(15.0);
		real s5 = (real)sqrt(5.0);
		real s5_s15 = s5 * s15;
		real r3_r0 = r3 * r0;
		real r4_r1 = r4 * r1;
		real r6_r3 = r6 * r3;
		real r7_r4 = r7 * r4;
		real r6_r6 = r6 * r6;
		real r7_r7 = r7 * r7;
		real r6_r0 = r6 * r0;
		real r7_r1 = r7 * r1;
		real r0_r0 = r0 * r0;
		real r1_r1 = r1 * r1;
		real r3_r3 = r3 * r3;
		real r4_r4 = r4 * r4;
		matrix[0] = r4 * r0 + r3 * r1;
		matrix[1] = 0.0f - r7 * r3 - r6 * r4;
		matrix[2] = s5_s15 * r7 * r6 * 0.2f;
		matrix[3] = 0.0f - r7 * r0 - r6 * r1;
		matrix[4] = r1 * r0 - r4 * r3;
		matrix[5] = 0.0f - r4 * r2 - r5 * r1;
		matrix[6] = r7 * r5 + r8 * r4;
		matrix[7] = s5_s15 * r8 * r7 * -0.2f;
		matrix[8] = r7 * r2 + r8 * r1;
		matrix[9] = r5 * r4 - r2 * r1;
		matrix[10] = (r5 * r2 * 0.13333334f - r3_r0 * 0.06666667f) * s5_s15 - r4_r1 * s5_s15 * 0.06666667f;
		matrix[11] = r6_r3 * s5_s15 * 0.06666667f - s5_s15 * r8 * r5 * 0.13333334f
			+ r7_r4 * s5_s15 * 0.06666667f;
		matrix[12] = r8 * r8 - r6_r6 * 0.5f - r7_r7 * 0.5f;
		matrix[13] = r6_r0 * s5_s15 * 0.06666667f - s5_s15 * r8 * r2 * 0.13333334f
			+ r7_r1 * s5_s15 * 0.06666667f;
		matrix[14] = s5_s15 * (r2 * r2) * 0.06666667f - (r5 * r5 * 0.06666667f + r0_r0 * 0.033333335f) * s5_s15
			- r1_r1 * s5_s15 * 0.033333335f + (r4_r4 + r3_r3) * s5_s15 * 0.033333335f;
		matrix[15] = 0.0f - r5 * r0 - r3 * r2;
		matrix[16] = r8 * r3 + r6 * r5;
		matrix[17] = s5_s15 * r8 * r6 * -0.2f;
		matrix[18] = r8 * r0 + r6 * r2;
		matrix[19] = r5 * r3 - r2 * r0;
		matrix[20] = r3_r0 - r4_r1;
		matrix[21] = r7_r4 - r6_r3;
		matrix[22] = r6_r6 * s5_s15 * 0.1f - r7_r7 * s5_s15 * 0.1f;
		matrix[23] = r7_r1 - r6_r0;
		matrix[24] = (r4_r4 + r0_r0) * 0.5f - r1_r1 * 0.5f - r3_r3 * 0.5f;
		function_143250(result + 4, 5, matrix, coefficients + 4);
	}
	if (order <= 3)
		return result;
	{
		real s2 = (real)sqrt(2.0);
		real r3_r3 = r3 * r3;
		real r3_r0 = r3 * r0;
		real r3_r3_r4 = r3_r3 * r4;
		real r4_r1 = r4 * r1;
		real r6_r6 = r6 * r6;
		real r3_r0_r1 = r3_r0 * r1;
		real r0_r0 = r0 * r0;
		real r0_r0_r4 = r0_r0 * r4;
		real r1_r1 = r1 * r1;
		real r1_r1_r4 = r1_r1 * r4;
		real r4_r4 = r4 * r4;
		real r4_r4_r4 = r4_r4 * r4;
		real s35 = (real)sqrt(35.0);
		real s35_s2 = s35 * s2;
		real s105 = (real)sqrt(105.0);
		real s21 = (real)sqrt(21.0);
		real s21_s35 = s21 * s35;
		real s105_s35_s2 = s105 * s35_s2;
		real r3_r1 = r3 * r1;
		real r3_r1_r6 = r3_r1 * r6;
		real r4_r1_r7 = r4_r1 * r7;
		real r4_r0 = r4 * r0;
		real r4_r0_r6 = r4_r0 * r6;
		real r3_r0_r7 = r3_r0 * r7;
		real r6_r6_r4 = r6_r6 * r4;
		real r7_r7 = r7 * r7;
		real r7_r7_r4 = r7_r7 * r4;
		real r6_r3 = r6 * r3;
		real r6_r3_r7 = r6_r3 * r7;
		real s7 = (real)sqrt(7.0);
		real s7_r7 = s7 * r7;
		real s7_r7_r6_r6 = s7_r7 * r6_r6;
		real s7_r7_r7_r7 = s7 * r7_r7 * r7;
		real r6_r0 = r6 * r0;
		real r6_r0_r7 = r6_r0 * r7;
		real r6_r6_r1 = r6_r6 * r1;
		real r7_r7_r1 = r7_r7 * r1;
		real r4_r3 = r4 * r3;
		real r4_r3_r6 = r4_r3 * r6;
		real s105_r3_r3 = s105 * r3_r3;
		real s105_r3_r3_r7 = s105_r3_r3 * r7;
		real s105_r1_r1 = s105 * r1_r1;
		real s105_r1_r1_r7 = s105_r1_r1 * r7;
		real r1_r0 = r1 * r0;
		real r1_r0_r6 = r1_r0 * r6;
		real s105_r0_r0 = s105 * r0_r0;
		real s105_r0_r0_r7 = s105_r0_r0 * r7;
		real s105_r4_r4 = s105 * r4_r4;
		real s105_r4_r4_r7 = s105_r4_r4 * r7;
		real r1_r1_r1 = r1_r1 * r1;
		real r3_r0_r4 = r3_r0 * r4;
		real r4_r4_r1 = r4_r4 * r1;
		real r0_r0_r1 = r0_r0 * r1;
		real r3_r3_r1 = r3_r3 * r1;
		real r4_r2 = r4 * r2;
		real r5_r0 = r5 * r0;
		real r3_r2 = r3 * r2;
		real s105_s2 = s105 * s2;
		real s7_s105 = s7 * s105;
		real r5_r1 = r5 * r1;
		real s105_s2_s21 = s105_s2 * s21;
		real r6_r4 = r6 * r4;
		real r6_r1 = r6 * r1;
		real r6_r5 = r6 * r5;
		real r6_r2 = r6 * r2;
		real r2_r0 = r2 * r0;
		real r2_r1 = r2 * r1;
		real r5_r4 = r5 * r4;
		real r5_r3 = r5 * r3;
		real r2_r2 = r2 * r2;
		real r5_r2 = r5 * r2;
		real r8_r8 = r8 * r8;
		real s21_s2 = s21 * s2;
		real r5_r5 = r5 * r5;
		real r5_r5_s105 = r5_r5 * s105;
		real r2_r2_s105 = r2_r2 * s105;
		real s7_s2 = s7 * s2;
		real s35_r5 = s35 * r5;
		real s35_r5_r1_r1 = s35_r5 * r1_r1;
		real s7_s2_s35 = s7_s2 * s35;
		real r3_r0_r2 = r3_r0 * r2;
		real r4_r1_r2 = r4_r1 * r2;
		real s35_r4_r4_r5 = s35 * r4_r4 * r5;
		real s35_r3_r3_r5 = s35 * r3_r3 * r5;
		real s35_r5_r0_r0 = s35_r5 * r0_r0;
		real r3_r0_r8 = r3_r0 * r8;
		real r3_r2_r6 = r3_r2 * r6;
		real r5_r0_r6 = r5_r0 * r6;
		real r4_r1_r8 = r4_r1 * r8;
		real r5_r1_r7 = r5_r1 * r7;
		real r4_r2_r7 = r4_r2 * r7;
		real s7_s2_s21 = s7_s2 * s21;
		real r6_r3_r8 = r6_r3 * r8;
		real s21_r5 = s21 * r5;
		real s21_r5_r6_r6 = s21_r5 * r6_r6;
		real r8_r7 = r8 * r7;
		real r8_r7_r4 = r8_r7 * r4;
		real s21_r5_r7_r7 = s21_r5 * r7_r7;
		real r7_r7_r8 = r7_r7 * r8;
		real r6_r6_r8 = r6_r6 * r8;
		real r8_r7_r1 = r8_r7 * r1;
		real r6_r0_r8 = r6_r0 * r8;
		real s21_r2 = s21 * r2;
		real s21_r2_r6_r6 = s21_r2 * r6_r6;
		real s21_r2_r7_r7 = s21_r2 * r7_r7;
		real r2_r1_r7 = r2_r1 * r7;
		real r3_r3_r8 = r3_r3 * r8;
		real r5_r3_r6 = r5_r3 * r6;
		real r0_r0_r8 = r0_r0 * r8;
		real r4_r4_r8 = r4_r4 * r8;
		real r1_r1_r8 = r1_r1 * r8;
		real r2_r0_r6 = r2_r0 * r6;
		real r5_r4_r7 = r5_r4 * r7;
		real s35_r2 = s35 * r2;
		real s35_r2_r4_r4 = s35_r2 * r4_r4;
		real r3_r0_r5 = r3_r0 * r5;
		real s35_r0_r0_r2 = s35 * r0_r0 * r2;
		real s35_r1_r1_r2 = s35 * r1_r1 * r2;
		real r4_r1_r5 = r4_r1 * r5;
		real s35_r2_r3_r3 = s35_r2 * r3_r3;
		real r4_r0_r1 = r4_r0 * r1;
		real r4_r4_r3 = r4_r4 * r3;
		real r0_r0_r3 = r0_r0 * r3;
		real r3_r3_r3 = r3_r3 * r3;
		real r1_r1_r3 = r1_r1 * r3;
		real r3_r0_r6 = r3_r0 * r6;
		real r4_r1_r6 = r4_r1 * r6;
		real r3_r1_r7 = r3_r1 * r7;
		real r4_r0_r7 = r4_r0 * r7;
		real r6_r6_r3 = r6_r6 * r3;
		real r6_r4_r7 = r6_r4 * r7;
		real r7_r7_r3 = r7_r7 * r3;
		real s7_r6 = s7 * r6;
		real s7_r6_r7_r7 = s7_r6 * r7_r7;
		real s7_r6_r6_r6 = s7 * r6_r6 * r6;
		real r6_r1_r7 = r6_r1 * r7;
		real r6_r6_r0 = r6_r6 * r0;
		real s105_r4_r4_r6 = s105_r4_r4 * r6;
		real s105_r3_r3_r6 = s105_r3_r3 * r6;
		real r4_r3_r7 = r4_r3 * r7;
		real s105_r1_r1_r6 = s105_r1_r1 * r6;
		real s105_r0_r0_r6 = s105_r0_r0 * r6;
		real r1_r0_r7 = r1_r0 * r7;
		real r3_r1_r4 = r3_r1 * r4;
		real r7_r7_r0 = r7_r7 * r0;
		real r0_r0_r0 = r0_r0 * r0;
		real r4_r4_r0 = r4_r4 * r0;
		real r3_r3_r0 = r3_r3 * r0;
		real r1_r1_r0 = r1_r1 * r0;
		matrix[0] = r0_r0_r4 * 0.75f + r3_r0_r1 * 1.5f - r3_r3_r4 * 0.75f - r1_r1_r4 * 0.75f + r4_r4_r4 * 0.25f;
		matrix[1] = (r4_r1_r7 * 0.014285714f - r3_r1_r6 * 0.014285714f) * s105_s35_s2
			- r4_r0_r6 * s105_s35_s2 * 0.014285714f - r3_r0_r7 * s105_s35_s2 * 0.014285714f;
		matrix[2] = r6_r6_r4 * s21_s35 * 0.035714287f - r7_r7_r4 * s21_s35 * 0.035714287f
			+ r6_r3_r7 * s21_s35 * 0.071428575f;
		matrix[3] = (s7_r7_r7_r7 * 0.035714287f - s7_r7_r6_r6 * 0.10714286f) * s35_s2;
		matrix[4] = (r6_r6_r1 * 0.035714287f + r6_r0_r7 * 0.071428575f) * s21_s35
			- r7_r7_r1 * s21_s35 * 0.035714287f;
		matrix[5] = (s105_r1_r1_r7 + s105_r3_r3_r7) * s35_s2 * 0.007142857f
			+ r4_r3_r6 * s105_s35_s2 * 0.014285714f - r1_r0_r6 * s105_s35_s2 * 0.014285714f
			- s105_r0_r0_r7 * s35_s2 * 0.007142857f - s105_r4_r4_r7 * s35_s2 * 0.007142857f;
		matrix[6] = r0_r0_r1 * 0.75f - (r3_r0_r4 * 1.5f + r1_r1_r1 * 0.25f) + r4_r4_r1 * 0.75f
			- r3_r3_r1 * 0.75f;
		real r4_r0_s105_s35_s2 = r4_r0 * s105_s35_s2;
		matrix[7] = r4_r3 * s105_s35_s2 * r5 * 0.014285714f - r3_r1 * s105_s35_s2 * r2 * 0.014285714f
			- r5_r0 * s105_s35_s2 * r1 * 0.014285714f - r4_r0_s105_s35_s2 * r2 * 0.014285714f;
		real second_diagonal_sum = r3_r2 * r7;
		second_diagonal_sum += r5_r1 * r6;
		second_diagonal_sum += r5_r0 * r7;
		second_diagonal_sum += r4_r2 * r6;
		second_diagonal_sum += r4_r0 * r8;
		second_diagonal_sum += r3_r1 * r8;
		matrix[8] = second_diagonal_sum;
		matrix[9] = r6_r4 * s105_s2_s21 * r8 * -0.023809524f - s105_s2_s21 * r8 * r7 * r3 * 0.023809524f
			- r6_r5 * s105_s2_s21 * r7 * 0.023809524f;
		matrix[10] = s7_s105 * r8 * r7 * r6 * 0.14285715f;
		matrix[11] = r6_r1 * s105_s2_s21 * r8 * -0.023809524f - s105_s2_s21 * r8 * r7 * r0 * 0.023809524f
			- r6_r2 * s105_s2_s21 * r7 * 0.023809524f;
		matrix[12] = r2_r0 * r7 + r2_r1 * r6 + r1_r0 * r8 - r5_r4 * r6 - r5_r3 * r7 - r4_r3 * r8;
		matrix[13] = (r3_r2 * r4 + r3_r1 * r5) * s105_s35_s2 * 0.014285714f
			- r1_r0 * s105_s35_s2 * r2 * 0.014285714f + r4_r0_s105_s35_s2 * r5 * 0.014285714f;
		real r5_r5_s21_s35 = r5_r5 * s21_s35;
		matrix[14] = (r2_r2 * r4 * 0.028571429f - r0_r0_r4 * 0.007142857f) * s21_s35
			- s21_s35 * r1_r1_r4 * 0.021428572f
			+ (r5_r1 * r2 * 0.057142857f + r3_r3_r4 * 0.007142857f) * s21_s35
			- r5_r5_s21_s35 * r4 * 0.028571429f + s21_s35 * r4_r4_r4 * 0.007142857f
			- s21_s35 * r3_r0_r1 * 0.014285714f;
		real r5_r2_s105_s2_s21 = r5_r2 * s105_s2_s21;
		matrix[15] = s105_s2_s21 * r4_r0_r6 * 0.004761905f
			- (s105_s2_s21 * r5_r1 * r8 + r5_r2_s105_s2_s21 * r7) * 0.01904762f
			- s105_s2_s21 * r4_r2 * r8 * 0.01904762f
			+ ((r3_r0_r7 + r3_r1_r6) * 0.004761905f + r4_r1_r7 * 0.014285714f) * s105_s2_s21;
		matrix[16] = r8_r8 * r4 - r7_r7_r4 * 0.75f + r8_r7 * r5 * 2.0f - r6_r3_r7 * 0.5f - r6_r6_r4 * 0.25f;
		matrix[17] = (s7_r7_r6_r6 * 0.035714287f + s7_r7_r7_r7 * 0.035714287f) * s21_s2
			- s21_s2 * r8_r8 * s7_r7 * 0.14285715f;
		matrix[18] = r8_r7 * r2 * 2.0f - r6_r6_r1 * 0.25f - r7_r7_r1 * 0.75f + r8_r8 * r1 - r6_r0_r7 * 0.5f;
		matrix[19] = r5_r4 * s105_s2_s21 * r8 * 0.01904762f + s21_s2 * s105_r1_r1_r7 * 0.007142857f
			- s21_s2 * s105_r4_r4_r7 * 0.007142857f + r5_r5_s105 * s21_s2 * r7 * 0.00952381f
			- s21_s2 * s105_r3_r3_r7 * 0.0023809525f - r2_r1 * s105_s2_s21 * r8 * 0.01904762f
			+ s105_s2_s21 * r1_r0_r6 * 0.004761905f - r7 * (r2_r2_s105 * s21_s2) * 0.00952381f
			- s105_s2_s21 * r4_r3_r6 * 0.004761905f + s21_s2 * s105_r0_r0_r7 * 0.0023809525f;
		matrix[20] = (r3_r3_r1 * 0.007142857f - r4_r2 * r5 * 0.057142857f + r3_r0_r4 * 0.014285714f) * s21_s35
			- r0_r0_r1 * s21_s35 * 0.007142857f
			+ (r2_r2 * r1 * 0.028571429f + r4_r4_r1 * 0.021428572f) * s21_s35
			- r5_r5_s21_s35 * r1 * 0.028571429f - r1_r1_r1 * s21_s35 * 0.007142857f;
		matrix[21] = r3_r0_r2 * s7_s2_s35 * 0.042857144f + s35_r5_r1_r1 * s7_s2 * 0.021428572f
			- s35_r5 * s7_s2 * r2_r2 * 0.042857144f + r4_r1_r2 * s7_s2_s35 * 0.042857144f
			- s35_r4_r4_r5 * s7_s2 * 0.021428572f - s35_r3_r3_r5 * s7_s2 * 0.021428572f
			+ (r5_r5 * s35 * r5 * 0.014285714f + s35_r5_r0_r0 * 0.021428572f) * s7_s2;
		matrix[22] = r5_r2 * s7_s105 * r8 * 0.057142857f - r3_r0_r8 * s7_s105 * 0.028571429f
			- r3_r2_r6 * s7_s105 * 0.028571429f - r5_r0_r6 * s7_s105 * 0.028571429f
			- r4_r1_r8 * s7_s105 * 0.028571429f - r5_r1_r7 * s7_s105 * 0.028571429f
			- r4_r2_r7 * s7_s105 * 0.028571429f;
		matrix[23] = r6_r3_r8 * s7_s2_s21 * 0.071428575f - s21_r5 * s7_s2 * r8_r8 * 0.071428575f
			+ (s21_r5_r7_r7 + s21_r5_r6_r6) * s7_s2 * 0.035714287f + r8_r7_r4 * s7_s2_s21 * 0.071428575f;
		matrix[24] = r8_r8 * r8 - (r6_r6_r8 + r7_r7_r8) * 1.5f;
		matrix[25] = r6_r0_r8 * s7_s2_s21 * 0.071428575f + s21_r2_r7_r7 * s7_s2 * 0.035714287f
			+ s21_r2_r6_r6 * s7_s2 * 0.035714287f + r8_r7_r1 * s7_s2_s21 * 0.071428575f
			- s21_r2 * s7_s2 * r8_r8 * 0.071428575f;
		matrix[26] = (r5_r3_r6 * 0.028571429f + r3_r3_r8 * 0.014285714f - r2_r1_r7 * 0.028571429f) * s7_s105
			- r0_r0_r8 * s7_s105 * 0.014285714f + r4_r4_r8 * s7_s105 * 0.014285714f
			- r1_r1_r8 * s7_s105 * 0.014285714f - r2_r0_r6 * s7_s105 * 0.028571429f
			+ (r2_r2 * r8 + r5_r4_r7) * s7_s105 * 0.028571429f - r5_r5 * s7_s105 * r8 * 0.028571429f;
		matrix[27] = s35_r2 * s7_s2 * r5_r5 * 0.042857144f
			- (r2_r2 * s35 * r2 * 0.014285714f + s35_r2_r3_r3 * 0.021428572f) * s7_s2
			- s35_r2_r4_r4 * s7_s2 * 0.021428572f - r3_r0_r5 * s7_s2_s35 * 0.042857144f
			+ (s35_r1_r1_r2 + s35_r0_r0_r2) * s7_s2 * 0.021428572f - r4_r1_r5 * s7_s2_s35 * 0.042857144f;
		matrix[28] = r4_r4_r3 * s21_s35 * 0.007142857f - r5_r5_s21_s35 * r3 * 0.028571429f
			- r4_r0_r1 * s21_s35 * 0.014285714f - r0_r0_r3 * s21_s35 * 0.021428572f
			+ r2_r2 * r3 * s21_s35 * 0.028571429f + r5_r0 * r2 * s21_s35 * 0.057142857f
			+ r3_r3_r3 * s21_s35 * 0.007142857f - r1_r1_r3 * s21_s35 * 0.007142857f;
		matrix[29] = r3_r0_r6 * s105_s2_s21 * 0.014285714f - r5_r2_s105_s2_s21 * r6 * 0.01904762f
			- s105_s2_s21 * r5_r0 * r8 * 0.01904762f + r3_r1_r7 * s105_s2_s21 * 0.004761905f
			+ r4_r1_r6 * s105_s2_s21 * 0.004761905f - s105_s2_s21 * r3_r2 * r8 * 0.01904762f
			+ r4_r0_r7 * s105_s2_s21 * 0.004761905f;
		matrix[30] = r6_r5 * r8 * 2.0f - r6_r6_r3 * 0.75f - r6_r4_r7 * 0.5f - r7_r7_r3 * 0.25f + r8_r8 * r3;
		matrix[31] = (s7_r6_r7_r7 * 0.035714287f + s7_r6_r6_r6 * 0.035714287f) * s21_s2
			- s7_r6 * s21_s2 * r8_r8 * 0.14285715f;
		matrix[32] = r8_r8 * r0 - r6_r1_r7 * 0.5f - r6_r6_r0 * 0.75f - r7_r7_r0 * 0.25f + r6_r2 * r8 * 2.0f;
		matrix[33] = r5_r3 * s105_s2_s21 * r8 * 0.01904762f
			- (s105_r4_r4_r6 * s21_s2 * 0.0023809525f + r6 * (r2_r2_s105 * s21_s2) * 0.00952381f)
			- s105_r3_r3_r6 * s21_s2 * 0.007142857f - r4_r3_r7 * s105_s2_s21 * 0.004761905f
			+ s105_r1_r1_r6 * s21_s2 * 0.0023809525f - r2_r0 * s105_s2_s21 * r8 * 0.01904762f
			+ (r5_r5_s105 * r6 * 0.00952381f + s105_r0_r0_r6 * 0.007142857f) * s21_s2
			+ r1_r0_r7 * s105_s2_s21 * 0.004761905f;
		matrix[34] = r3_r1_r4 * s21_s35 * 0.014285714f - r5_r5_s21_s35 * r0 * 0.028571429f
			- r0_r0_r0 * s21_s35 * 0.007142857f + r2_r2 * r0 * s21_s35 * 0.028571429f
			+ r3_r3_r0 * s21_s35 * 0.021428572f + r4_r4_r0 * s21_s35 * 0.007142857f
			- r1_r1_r0 * s21_s35 * 0.007142857f - r3_r2 * s21_s35 * r5 * 0.057142857f;
		matrix[35] = s35_r5_r1_r1 * s105_s2 * 0.007142857f - s35_r5_r0_r0 * s105_s2 * 0.007142857f
			+ s35_r3_r3_r5 * s105_s2 * 0.007142857f + r4_r1_r2 * s105_s35_s2 * 0.014285714f
			- r3_r0_r2 * s105_s35_s2 * 0.014285714f - s35_r4_r4_r5 * s105_s2 * 0.007142857f;
		matrix[36] = r5_r0_r6 + r3_r2_r6 + r3_r0_r8 - r4_r1_r8 - r4_r2_r7 - r5_r1_r7;
		matrix[37] = s21_r5_r7_r7 * s105_s2 * 0.011904762f - s21_r5_r6_r6 * s105_s2 * 0.011904762f
			- r6_r3_r8 * s105_s2_s21 * 0.023809524f + r8_r7_r4 * s105_s2_s21 * 0.023809524f;
		matrix[38] = r6_r6_r8 * s7_s105 * 0.071428575f - r7_r7_r8 * s7_s105 * 0.071428575f;
		matrix[39] = (r8_r7_r1 * 0.023809524f - r6_r0_r8 * 0.023809524f) * s105_s2_s21
			- s21_r2_r6_r6 * s105_s2 * 0.011904762f + s21_r2_r7_r7 * s105_s2 * 0.011904762f;
		matrix[40] = r4_r4_r8 * 0.5f - r1_r1_r8 * 0.5f - r3_r3_r8 * 0.5f + r0_r0_r8 * 0.5f - r5_r3_r6 - r2_r1_r7
			+ r5_r4_r7 + r2_r0_r6;
		matrix[41] = (s35_r1_r1_r2 * 0.007142857f - s35_r0_r0_r2 * 0.007142857f + s35_r2_r3_r3 * 0.007142857f) * s105_s2
			+ r3_r0_r5 * s105_s35_s2 * 0.014285714f - s35_r2_r4_r4 * s105_s2 * 0.007142857f
			- r4_r1_r5 * s105_s35_s2 * 0.014285714f;
		matrix[42] = r0_r0_r3 * 0.75f - r1_r1_r3 * 0.75f - r4_r0_r1 * 1.5f + r4_r4_r3 * 0.75f
			- r3_r3_r3 * 0.25f;
		matrix[43] = r3_r1_r7 * s105_s35_s2 * 0.014285714f + r4_r1_r6 * s105_s35_s2 * 0.014285714f
			- r3_r0_r6 * s105_s35_s2 * 0.014285714f + r4_r0_r7 * s105_s35_s2 * 0.014285714f;
		matrix[44] = (r6_r6_r3 * 0.035714287f - r6_r4_r7 * 0.071428575f) * s21_s35
			- r7_r7_r3 * s21_s35 * 0.035714287f;
		matrix[45] = (s7_r6_r7_r7 * 0.10714286f - s7_r6_r6_r6 * 0.035714287f) * s35_s2;
		matrix[46] = r6_r6_r0 * s21_s35 * 0.035714287f
			- (r7_r7_r0 * 0.035714287f + r6_r1_r7 * 0.071428575f) * s21_s35;
		matrix[47] = r1_r0_r7 * s105_s35_s2 * 0.014285714f + s105_r3_r3_r6 * s35_s2 * 0.007142857f
			- s105_r0_r0_r6 * s35_s2 * 0.007142857f - s105_r4_r4_r6 * s35_s2 * 0.007142857f
			+ s105_r1_r1_r6 * s35_s2 * 0.007142857f - r4_r3_r7 * s105_s35_s2 * 0.014285714f;
		matrix[48] = r4_r4_r0 * 0.75f - r1_r1_r0 * 0.75f - r3_r3_r0 * 0.75f + r0_r0_r0 * 0.25f
			+ r3_r1_r4 * 1.5f;
		function_143250(result + 9, 7, matrix, coefficients + 9);
	}
	return result;
}
