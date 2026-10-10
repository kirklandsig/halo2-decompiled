// @flags /O2 /Ob1 /Gr /arch:SSE
/* UNKNOWN_141590.CPP: transform4x3f and quaternion math */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

struct rigid_transform_scaled
{
	quaternionf rotation;
	point3f position;
	real scale;
};

vector3f *function_11d000(vector3f const *v, vector3f *out);

__inline void real_point3d_set(point3f *point, real x, real y, real z)
{
	point->x = x;
	point->y = y;
	point->z = z;
}

// @retail 0x141590
void function_141590(
	transform4x3f const *in,
	transform4x3f *out)
{
	if (in->scale != 0.f)
	{
		real z, y, x;
		real t;
		z = 0.f - in->position.z;
		y = 0.f - in->position.y;
		x = 0.f - in->position.x;
		if (in->scale != 1.f)
		{
			real inverse = 1.f / in->scale;
			out->scale = inverse;
			x = inverse * x;
			y = inverse * y;
			z = inverse * z;
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

/* rotates a vector by a unit quaternion */
static inline void quaternion_transform_vector(
	quaternionf const *q,
	vector3f const *v,
	vector3f *out)
{
	real dot = (q->i * v->i + q->j * v->j + q->k * v->k) * 2.f;
	real w2 = q->w * 2.f;
	real s = q->w * q->w * 2.f - 1.f;
	vector3f cross;

	cross.i = q->j * v->k - q->k * v->j;
	cross.j = q->k * v->i - q->i * v->k;
	cross.k = q->i * v->j - q->j * v->i;
	out->i = v->i * s + q->i * dot + cross.i * w2;
	out->j = v->j * s + q->j * dot + cross.j * w2;
	out->k = v->k * s + q->k * dot + cross.k * w2;
}

/* the rotation that takes one unit vector to another */
// @retail 0x1417b0
void matrix4x3_rotation_between_vectors(
	transform4x3f *matrix,
	vector3f const *arg_5f338b,
	vector3f const *arg_bc44c6)
{
	quaternionf rotation;
	real cosine = arg_5f338b->i * arg_bc44c6->i + arg_5f338b->j * arg_bc44c6->j + arg_5f338b->k * arg_bc44c6->k;

	if (cosine < -1.f)
	{
		cosine = -1.f;
	}
	else if (cosine > 1.f)
	{
		cosine = 1.f;
	}

	real cosine_half = (real)sqrt((cosine + 1.f) * 0.5f);
	real sine_half = (real)sqrt((1.f - cosine) * 0.5f);
	real sine = sine_half * cosine_half * 2.f;

	if (sine != 0.f)
	{
		real scale = sine_half / sine;

		rotation.i = (arg_5f338b->j * arg_bc44c6->k - arg_5f338b->k * arg_bc44c6->j) * scale;
		rotation.j = (arg_5f338b->k * arg_bc44c6->i - arg_5f338b->i * arg_bc44c6->k) * scale;
		rotation.k = (arg_5f338b->i * arg_bc44c6->j - arg_5f338b->j * arg_bc44c6->i) * scale;
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
		matrix->left.i = matrix->up.j * matrix->forward.k - matrix->up.k * matrix->forward.j;
		matrix->left.j = matrix->up.k * matrix->forward.i - matrix->up.i * matrix->forward.k;
		matrix->left.k = matrix->up.i * matrix->forward.j - matrix->up.j * matrix->forward.i;
		function_30bf0(&matrix->left);
		matrix->forward.i = matrix->left.j * matrix->up.k - matrix->left.k * matrix->up.j;
		matrix->forward.j = matrix->left.k * matrix->up.i - matrix->left.i * matrix->up.k;
		matrix->forward.k = matrix->left.i * matrix->up.j - matrix->left.j * matrix->up.i;
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
	sin_b_sin_c = sin_b * angle_c.y;
	sin_b_cos_c = sin_b * angle_c.x;
	sin_a = (real)sin(a);
	out->scale = 1.f;
	out->forward.j = sin_a * angle_c.x - sin_b_sin_c * cos_a;
	out->forward.i = cos_a * cos_b;
	out->forward.k = sin_b_cos_c * cos_a + sin_a * angle_c.y;
	out->left.i = 0.f - sin_a * cos_b;
	out->left.k = cos_a * angle_c.y - sin_b_cos_c * sin_a;
	out->left.j = sin_b_sin_c * sin_a + cos_a * angle_c.x;
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

__declspec(noinline) void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);

// @retail 0x1420f0
inline void function_1420f0(
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

__inline real normalize3d(vector3f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < 0.0001f))
	{
		real inv = 1.f / m;
		v->i = inv * v->i;
		v->j = v->j * inv;
		v->k = inv * v->k;
		return m;
	}
	return 0.f;
}

void function_142390(
	plane3f const *plane,
	transform4x3f *out);

static __forceinline void matrix4x3_basis_store(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up)
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

static __forceinline void plane_vector_normalize(vector3f *out, real j)
{
    real magnitude = (real)sqrt(out->i * out->i + j * j + out->k * out->k);
    if (!(fabs(magnitude) < 0.0001f))
    {
        real inverse = 1.f / magnitude;
        out->i = inverse * out->i;
        out->j = j * inverse;
        out->k = inverse * out->k;
    }
}

static __forceinline void function_x7a3668(plane3f const *plane, transform4x3f *out)
{
	vector3f w;
	point3f position;
	function_11d000(&plane->n, &w);
	plane_vector_normalize(&w, w.j);
	position.x = plane->d * plane->n.i;
	position.y = plane->n.j * plane->d;
	position.z = plane->n.k * plane->d;
	matrix4x3_basis_store(out, &position, &w, &plane->n);
}

// @retail 0x142390
void function_142390(
	plane3f const *plane,
	transform4x3f *out)
{
	function_x7a3668(plane, out);
}

int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_11d790(quaternionf const *q, vector3f *axis, real *angle);

// @retail 0x1424f0
vector3f *matrix4x3_rotation_between(
	transform4x3f const *a,
	transform4x3f const *b,
	vector3f *out)
{
	transform4x3f const * const *a_reference = &a;
	transform4x3f inverse;
	transform4x3f relative;
	quaternionf rotation;
	real angle;

	function_141590((*a_reference), &inverse);
	function_142a60(b, &inverse, &relative);
	function_141f60(&relative.rotation, &rotation);
	function_11d790(&rotation, out, &angle);
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

static __forceinline real basis_dot_xzy(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	result += b->k * a->k;
	result += a->j * b->j;
	return result;
}

static __forceinline real basis_dot_xyz(vector3f const *a, vector3f const *b)
{
	real result = a->i * b->i;
	result += b->j * a->j;
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
		function_a0200(basis_dot_xzy(forward, up), 0.f);
}

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

// @retail 0x143600
real *function_143600(real const *rotation, unsigned long order, real const *coefficients, real *result)
{
	real matrix[49];
	real term_0;
	real term_1;
	real term_2;
	real term_3;
	real term_4;
	real term_5;
	real term_6;
	real term_7;
	real term_8;
	real term_9;
	real term_10;
	real term_11;
	real term_12;
	real term_13;
	real term_14;
	double term_15;
	real term_16;
	real term_17;
	real term_18;
	real term_19;
	real term_20;
	real term_21;
	real term_22;
	real term_23;
	real term_24;
	real term_25;
	real term_26;
	real term_27;
	real term_28;
	real term_29;
	real term_30;
	real term_31;
	real term_32;
	real term_33;
	real term_34;
	real term_35;
	real term_36;
	real term_37;
	real term_38;
	real term_39;
	double term_40;
	double term_41;
	real term_42;
	real term_43;
	real term_44;
	real term_45;
	real term_46;
	real term_47;
	real term_48;
	real term_49;
	real term_50;
	real term_51;
	real term_52;
	real term_53;
	real term_54;
	real term_55;
	real term_56;
	real term_57;
	real term_58;
	real term_59;
	real term_60;
	real term_61;
	real term_62;
	real term_63;
	real term_64;
	real term_65;
	real term_66;
	real term_67;
	real term_68;
	real term_69;
	real term_70;
	real term_71;
	real term_72;
	real term_73;
	real term_74;
	real term_75;
	real term_76;
	real term_77;
	real term_78;
	real term_79;
	real term_80;
	real term_81;
	real term_82;
	real term_83;
	real term_84;
	real term_85;
	real term_86;
	real term_87;
	real term_88;
	real term_89;
	real term_90;
	real term_91;
	real term_92;
	real term_93;
	real term_94;
	real term_95;
	real term_96;
	real term_97;
	real term_98;
	real term_99;
	real term_100;
	real term_101;
	real term_102;
	real term_103;
	real term_104;
	real term_105;
	real term_106;
	real term_107;
	real term_108;
	real term_109;
	real term_110;
	real term_111;
	real term_112;
	real term_113;
	real term_114;
	real term_115;
	real term_116;
	real term_117;
	real term_118;
	real term_119;
	real term_120;
	real term_121;
	real term_122;
	real term_123;
	real term_124;
	real term_125;
	real term_126;
	real term_127;
	real term_128;
	real term_129;
	real term_130;
	real term_131;
	real term_132;
	real term_133;
	real term_134;
	real term_135;
	real term_136;
	real term_137;
	real term_138;
	real term_139;
	real term_140;
	real term_141;
	real term_142;
	real term_143;
	real term_144;
	real term_145;
	real term_146;
	real term_147;
	real term_148;
	real term_149;
	real term_150;
	real term_151;
	real term_152;
	real term_153;
	real term_154;
	real term_155;
	real term_156;
	real term_157;
	real term_158;
	real term_159;
	real term_160;
	real term_161;
	real term_162;
	real term_163;
	real term_164;
	real term_165;
	real term_166;
	real term_167;
	real term_168;
	real term_169;
	real term_170;
	real term_171;
	real term_172;
	real term_173;
	real term_174;
	real term_175;
	real term_176;
	real term_177;
	real term_178;
	real term_179;
	real term_180;
	real term_181;
	real term_182;
	double term_183;
	real term_184;
	real term_185;
	real term_186;
	real term_187;
	real term_188;
	real term_189;
	real term_190;
	real term_191;
	real term_192;
	real term_193;
	real term_194;
	real term_195;
	real term_196;
	real term_197;
	double term_198;
	real term_199;
	real term_200;
	double term_201;
	real term_202;
	real term_203;
	real term_204;
	double term_205;
	real term_206;
	real term_207;
	real term_208;
	real term_209;
	real term_210;
	real term_211;
	real term_212;
	real term_213;
	real term_214;
	real term_215;
	real term_216;
	real term_217;
	real term_218;
	real term_219;
	real term_220;
	double term_221;
	real term_222;
	double term_223;
	real term_224;
	real term_225;
	real term_226;
	real term_227;
	real term_228;
	real term_229;
	real term_230;
	real term_231;
	real term_232;
	real term_233;
	real term_234;
	real term_235;
	real term_236;
	real term_237;
	real term_238;
	real term_239;
	real term_240;
	real term_241;
	real term_242;
	real term_243;
	real term_244;
	real term_245;
	real term_246;
	real term_247;
	real term_248;
	real term_249;
	real term_250;
	real term_251;
	real term_252;
	real term_253;
	real term_254;
	real term_255;
	real term_256;
	real term_257;
	real term_258;
	real term_259;
	real term_260;
	real term_261;
	real term_262;
	real term_263;
	real term_264;
	real term_265;
	real term_266;
	real term_267;
	real term_268;
	real term_269;
	real term_270;
	real term_271;
	real term_272;
	real term_273;
	real term_274;
	real term_275;
	real term_276;
	real term_277;
	real term_278;
	real term_279;
	real term_280;
	real term_281;
	real term_282;
	real term_283;
	real term_284;
	real term_285;
	real term_286;
	real term_287;
	real term_288;
	real term_289;
	real term_290;
	real term_291;
	real term_292;
	real term_293;
	real term_294;
	real term_295;
	real term_296;
	real term_297;
	real term_298;
	real term_299;
	real term_300;
	real term_301;
	real term_302;
	real term_303;
	real term_304;
	real term_305;
	real term_306;
	real term_307;
	real term_308;
	real term_309;
	real term_310;
	real term_311;
	real term_312;
	real term_313;
	real term_314;
	real term_315;
	real term_316;
	real term_317;
	real term_318;
	real term_319;
	real term_320;
	real term_321;
	real term_322;
	real term_323;
	real term_324;
	real term_325;
	real term_326;
	real term_327;
	real term_328;
	real term_329;
	real term_330;
	real term_331;
	real term_332;
	real term_333;
	real term_334;
	real term_335;
	real term_336;
	real term_337;
	real term_338;
	real term_339;
	real term_340;
	real term_341;
	real term_342;
	real term_343;
	real term_344;
	real term_345;
	real term_346;
	real term_347;
	real term_348;
	real term_349;
	real term_350;
	real term_351;
	real term_352;
	real term_353;
	real term_354;
	real term_355;
	real term_356;
	real term_357;
	real term_358;
	real term_359;
	real term_360;
	real term_361;
	real term_362;
	real term_363;
	real term_364;
	real term_365;
	real term_366;
	real term_367;
	real term_368;
	real term_369;
	real term_370;
	real term_371;
	real term_372;
	real term_373;
	real term_374;
	real term_375;
	real term_376;
	real term_377;
	real term_378;
	real term_379;
	real term_380;
	real term_381;
	real term_382;
	real term_383;
	real term_384;
	real term_385;
	real term_386;
	real term_387;
	real term_388;
	real term_389;
	real term_390;
	real term_391;
	real term_392;
	real term_393;
	real term_394;
	real term_395;
	real term_396;
	real term_397;
	real term_398;
	real term_399;
	real term_400;
	real term_401;
	real term_402;
	real term_403;
	real term_404;
	real term_405;
	real term_406;
	real term_407;
	real term_408;
	real term_409;
	real term_410;
	real term_411;
	real term_412;
	real term_413;
	real term_414;
	real term_415;
	real term_416;
	real term_417;
	real term_418;
	real term_419;
	real term_420;
	real term_421;
	real term_422;
	real term_423;
	real term_424;
	real term_425;
	real term_426;
	real term_427;
	real term_428;
	real term_429;
	real term_430;
	real term_431;
	real term_432;
	real term_433;
	real term_434;
	real term_435;
	real term_436;
	real term_437;
	real term_438;
	real term_439;
	real term_440;
	real term_441;
	real term_442;
	real term_443;
	real term_444;
	real term_445;
	real term_446;
	real term_447;
	real term_448;
	real term_449;
	real term_450;
	real term_451;
	real term_452;
	real term_453;
	real term_454;
	real term_455;
	real term_456;
	real term_457;
	real term_458;
	real term_459;
	real term_460;
	real term_461;
	real term_462;
	real term_463;
	real term_464;
	real term_465;
	real term_466;
	real term_467;
	real term_468;
	real term_469;
	real term_470;
	real term_471;
	real term_472;
	real term_473;
	real term_474;
	real term_475;
	real term_476;
	real term_477;
	real term_478;
	real term_479;
	real term_480;
	real term_481;
	real term_482;
	real term_483;
	real term_484;
	real term_485;
	real term_486;
	real term_487;
	real term_488;
	real term_489;
	real term_490;
	real term_491;
	real term_492;
	real term_493;
	real term_494;
	real term_495;
	real term_496;
	real term_497;
	real term_498;
	real term_499;
	real term_500;
	real term_501;
	real term_502;
	real term_503;
	real term_504;
	real term_505;
	real term_506;
	real term_507;
	real term_508;
	real term_509;
	real term_510;
	real term_511;
	real term_512;
	real term_513;
	real term_514;
	real term_515;
	real term_516;
	real term_517;
	real term_518;
	real term_519;
	real term_520;
	real term_521;
	real term_522;
	real term_523;
	real term_524;
	real term_525;
	real term_526;
	real term_527;
	real term_528;
	real term_529;
	real term_530;
	real term_531;
	real term_532;
	real term_533;
	real term_534;
	real term_535;
	real term_536;
	real term_537;
	real term_538;
	real term_539;
	real term_540;
	real term_541;
	real term_542;
	real term_543;
	real term_544;
	real term_545;
	real term_546;
	real term_547;
	real term_548;
	real term_549;
	real term_550;
	real term_551;
	real term_552;
	real term_553;
	real term_554;
	real term_555;
	real term_556;
	real term_557;
	real term_558;
	real term_559;
	real term_560;
	real term_561;
	real term_562;
	real term_563;
	real term_564;
	real term_565;
	real term_566;
	real term_567;
	real term_568;
	real term_569;
	real term_570;
	real term_571;
	real term_572;
	real term_573;
	real term_574;
	real term_575;
	real term_576;
	real term_577;
	real term_578;
	real term_579;
	real term_580;
	real term_581;
	real term_582;
	real term_583;
	real term_584;
	real term_585;
	real term_586;
	real term_587;
	real term_588;
	real term_589;
	real term_590;
	real term_591;
	real term_592;
	real term_593;
	real term_594;
	real term_595;
	real term_596;
	real term_597;
	real term_598;
	real term_599;
	real term_600;
	real term_601;
	real term_602;
	real term_603;
	real term_604;
	real term_605;
	real term_606;
	real term_607;
	real term_608;
	real term_609;
	real term_610;
	real term_611;
	real term_612;
	real term_613;
	real term_614;
	real term_615;
	real term_616;
	real term_617;
	real term_618;
	real term_619;
	real term_620;
	real term_621;
	real term_622;
	real term_623;
	real term_624;
	real term_625;
	real term_626;
	real term_627;
	real term_628;
	real term_629;
	real term_630;
	real term_631;
	real term_632;
	real term_633;
	real term_634;
	real term_635;
	real term_636;
	real term_637;
	real term_638;
	real term_639;
	real term_640;
	real term_641;
	real term_642;
	real term_643;
	real term_644;
	real term_645;
	real term_646;
	real term_647;
	real term_648;
	real term_649;
	real term_650;
	real term_651;
	real term_652;
	real term_653;
	real term_654;
	real term_655;
	real term_656;
	real term_657;
	real term_658;
	real term_659;
	real term_660;
	real term_661;
	real term_662;
	real term_663;
	real term_664;
	real term_665;
	real term_666;
	real term_667;
	real term_668;
	real term_669;
	real term_670;
	real term_671;
	real term_672;
	real term_673;
	real term_674;
	real term_675;
	real term_676;
	real term_677;
	real term_678;
	real term_679;
	real term_680;
	real term_681;
	real term_682;
	real term_683;
	real term_684;
	real term_685;
	real term_686;
	real term_687;
	real term_688;
	real term_689;
	real term_690;
	real term_691;
	real term_692;
	real term_693;
	real term_694;
	real term_695;
	real term_696;
	real term_697;
	real term_698;
	real term_699;
	real term_700;
	real term_701;
	real term_702;
	real term_703;
	real term_704;
	real term_705;
	real term_706;
	real term_707;
	real term_708;
	real term_709;
	real term_710;
	real term_711;
	real term_712;
	real term_713;
	real term_714;
	real term_715;
	real term_716;
	real term_717;
	real term_718;
	real term_719;
	real term_720;
	real term_721;
	real term_722;
	real term_723;
	real term_724;
	real term_725;
	real term_726;
	real term_727;
	real term_728;
	real term_729;
	real term_730;
	real term_731;
	real term_732;
	real term_733;
	real term_734;
	real term_735;
	real term_736;
	real term_737;
	real term_738;
	real term_739;
	real term_740;
	real term_741;
	real term_742;
	real term_743;
	real term_744;
	real term_745;
	real term_746;
	real term_747;
	real term_748;
	real term_749;
	real term_750;
	real term_751;
	real term_752;
	real term_753;
	real term_754;
	real term_755;
	real term_756;
	real term_757;
	real term_758;
	real term_759;
	real term_760;
	real term_761;
	real term_762;
	real term_763;
	real term_764;
	real term_765;
	real term_766;
	real term_767;
	real term_768;
	real term_769;
	real term_770;
	real term_771;
	real term_772;
	real term_773;
	real term_774;
	real term_775;
	real term_776;
	real term_777;
	real term_778;
	real term_779;
	real term_780;
	real term_781;
	real term_782;
	real term_783;
	real term_784;
	real term_785;
	real term_786;
	real term_787;
	real term_788;
	real term_789;
	real term_790;
	real term_791;
	real term_792;
	real term_793;
	real term_794;
	real term_795;
	real term_796;
	real term_797;
	real term_798;
	real term_799;
	real term_800;
	real term_801;
	real term_802;
	real term_803;
	real term_804;
	real term_805;
	real term_806;
	real term_807;
	real term_808;
	real term_809;
	real term_810;
	real term_811;
	real term_812;
	real term_813;
	real term_814;
	real term_815;
	real term_816;
	real term_817;
	real term_818;
	real term_819;
	real term_820;
	real term_821;
	real term_822;
	real term_823;
	real term_824;
	real term_825;
	real term_826;
	real term_827;
	real term_828;
	real term_829;
	real term_830;
	real term_831;
	real term_832;
	real term_833;
	real term_834;
	real term_835;
	real term_836;
	real term_837;
	real term_838;
	real term_839;
	real term_840;
	real term_841;
	real term_842;
	real term_843;
	real term_844;
	real term_845;
	real term_846;
	real term_847;
	real term_848;
	real term_849;
	real term_850;
	real term_851;
	real term_852;
	real term_853;
	real term_854;
	real term_855;
	real term_856;
	real term_857;
	real term_858;
	real term_859;
	real term_860;
	real term_861;
	real term_862;
	real term_863;
	real term_864;
	real term_865;
	real term_866;
	real term_867;
	real term_868;
	real term_869;
	real term_870;
	real term_871;
	real term_872;
	real term_873;
	real term_874;
	real term_875;
	real term_876;
	real term_877;
	real term_878;
	real term_879;
	real term_880;
	real term_881;
	real term_882;
	real term_883;
	real term_884;
	real term_885;
	real term_886;
	real term_887;
	real term_888;
	real term_889;
	real term_890;
	real term_891;
	real term_892;
	real term_893;
	real term_894;
	real term_895;
	real term_896;
	real term_897;
	real term_898;
	real term_899;
	real term_900;
	real term_901;
	real term_902;
	real term_903;
	real term_904;
	real term_905;
	real term_906;
	real term_907;
	real term_908;
	real term_909;
	real term_910;
	real term_911;
	real term_912;
	real term_913;
	real term_914;
	real term_915;
	real term_916;
	real term_917;
	real term_918;
	real term_919;
	real term_920;
	real term_921;
	real term_922;
	real term_923;
	real term_924;
	real term_925;
	real term_926;
	real term_927;
	real term_928;
	real term_929;
	real term_930;
	real term_931;
	real term_932;
	real term_933;
	real term_934;
	real term_935;
	real term_936;
	real term_937;
	real term_938;
	real term_939;
	real term_940;
	real term_941;
	real term_942;
	real term_943;
	real term_944;
	real term_945;
	real term_946;
	real term_947;
	real term_948;
	real term_949;
	real term_950;
	real term_951;
	real term_952;
	real term_953;
	real term_954;
	real term_955;
	real term_956;
	real term_957;
	real term_958;
	real term_959;
	real term_960;
	real term_961;
	real term_962;
	real term_963;
	real term_964;
	real term_965;
	real term_966;
	real term_967;
	real term_968;
	real term_969;
	real term_970;
	real term_971;
	real term_972;
	real term_973;
	real term_974;
	real term_975;
	real term_976;
	real term_977;
	real term_978;
	real term_979;
	real term_980;
	real term_981;
	real term_982;
	real term_983;
	real term_984;
	real term_985;
	real term_986;
	real term_987;
	real term_988;
	real term_989;
	real term_990;
	real term_991;
	real term_992;
	real term_993;
	real term_994;
	real term_995;
	real term_996;
	real term_997;
	real term_998;
	real term_999;
	real term_1000;
	real term_1001;
	real term_1002;
	real term_1003;
	real term_1004;
	real term_1005;
	real term_1006;
	real term_1007;
	real term_1008;
	real term_1009;
	real term_1010;
	real term_1011;
	real term_1012;
	real term_1013;
	real term_1014;
	real term_1015;
	real term_1016;
	real term_1017;
	real term_1018;
	real term_1019;
	real term_1020;
	real term_1021;
	real term_1022;
	real term_1023;
	real term_1024;
	real term_1025;
	real term_1026;
	real term_1027;
	real term_1028;
	real term_1029;
	real term_1030;
	real term_1031;
	real term_1032;
	real term_1033;
	real term_1034;
	real term_1035;
	real term_1036;
	real term_1037;
	real term_1038;
	real term_1039;
	real term_1040;
	real term_1041;
	real term_1042;
	real term_1043;
	real term_1044;
	real term_1045;
	real term_1046;
	real term_1047;
	real term_1048;
	real term_1049;
	real term_1050;
	real term_1051;
	real term_1052;
	real term_1053;
	real term_1054;
	real term_1055;
	real term_1056;
	real term_1057;
	real term_1058;
	real term_1059;
	real term_1060;
	real term_1061;
	real term_1062;
	real term_1063;
	real term_1064;
	real term_1065;
	real term_1066;
	real term_1067;
	real term_1068;
	real term_1069;
	real term_1070;
	real term_1071;
	real term_1072;
	real term_1073;
	real term_1074;
	real term_1075;
	real term_1076;
	real term_1077;
	real term_1078;
	real term_1079;
	real term_1080;
	real term_1081;
	real term_1082;
	real term_1083;
	real term_1084;
	real term_1085;
	real term_1086;
	real term_1087;
	real term_1088;
	real term_1089;
	real term_1090;
	real term_1091;
	real term_1092;
	real term_1093;
	real term_1094;
	real term_1095;
	real term_1096;
	real term_1097;
	real term_1098;
	real term_1099;
	real term_1100;
	real term_1101;
	real term_1102;
	real term_1103;
	real term_1104;
	real term_1105;
	real term_1106;
	real term_1107;
	real term_1108;
	real term_1109;
	real term_1110;
	real term_1111;
	real term_1112;
	real term_1113;
	real term_1114;
	real term_1115;
	real term_1116;
	real term_1117;
	real term_1118;
	real term_1119;
	real term_1120;
	real term_1121;
	real term_1122;
	real term_1123;
	real term_1124;
	real term_1125;
	real term_1126;
	real term_1127;
	real term_1128;
	real term_1129;
	real term_1130;
	real term_1131;
	real term_1132;
	real term_1133;
	real term_1134;
	real term_1135;
	real term_1136;
	real term_1137;
	real term_1138;
	real term_1139;
	real term_1140;
	real term_1141;
	real term_1142;
	real term_1143;
	real term_1144;
	real term_1145;
	real term_1146;
	real term_1147;
	real term_1148;
	real term_1149;
	real term_1150;
	real term_1151;
	real term_1152;
	real term_1153;
	real term_1154;
	real term_1155;
	real term_1156;
	real term_1157;
	real term_1158;
	real term_1159;
	real term_1160;
	real term_1161;
	real term_1162;
	real term_1163;
	real term_1164;
	real term_1165;
	real term_1166;
	real term_1167;
	real term_1168;
	real term_1169;
	real term_1170;
	real term_1171;
	real term_1172;
	real term_1173;
	real term_1174;
	real term_1175;
	real term_1176;
	real term_1177;
	real term_1178;
	real term_1179;
	real term_1180;
	real term_1181;
	real term_1182;
	real term_1183;
	real term_1184;
	real term_1185;
	real term_1186;
	real term_1187;
	real term_1188;
	real term_1189;
	real term_1190;
	real term_1191;
	real term_1192;
	real term_1193;
	real term_1194;
	real term_1195;
	real term_1196;
	real term_1197;
	real term_1198;
	real term_1199;
	real term_1200;
	real term_1201;
	real term_1202;
	real term_1203;
	real term_1204;
	real term_1205;
	real term_1206;
	real term_1207;
	real term_1208;
	real term_1209;
	real term_1210;
	real term_1211;
	real term_1212;
	real term_1213;
	real term_1214;
	real term_1215;
	real term_1216;
	real term_1217;
	real term_1218;
	real term_1219;
	real term_1220;
	real term_1221;
	real term_1222;
	real term_1223;
	real term_1224;
	real term_1225;
	real term_1226;
	real term_1227;
	real term_1228;
	real term_1229;
	real term_1230;
	real term_1231;
	real term_1232;
	real term_1233;
	real term_1234;
	real term_1235;
	real term_1236;
	real term_1237;
	real term_1238;
	real term_1239;
	real term_1240;
	real term_1241;
	real term_1242;
	real term_1243;
	real term_1244;
	real term_1245;
	real term_1246;
	real term_1247;
	real term_1248;
	real term_1249;
	real term_1250;
	real term_1251;
	real term_1252;
	real term_1253;
	real term_1254;
	real term_1255;
	real term_1256;
	real term_1257;
	real term_1258;
	real term_1259;
	real term_1260;
	real term_1261;
	real term_1262;
	real term_1263;
	real term_1264;
	real term_1265;
	real term_1266;
	real term_1267;
	real term_1268;
	real term_1269;
	real term_1270;
	real term_1271;
	real term_1272;
	real term_1273;
	real term_1274;
	real term_1275;
	real term_1276;
	real term_1277;
	real term_1278;
	real term_1279;
	real term_1280;
	real term_1281;
	real term_1282;
	real term_1283;
	real term_1284;
	real term_1285;
	real term_1286;
	real term_1287;
	real term_1288;
	real term_1289;
	real term_1290;
	real term_1291;
	real term_1292;
	real term_1293;
	real term_1294;
	real term_1295;
	real term_1296;
	real term_1297;
	real term_1298;
	real term_1299;
	real term_1300;
	real term_1301;
	real term_1302;
	real term_1303;
	real term_1304;
	real term_1305;
	real term_1306;
	real term_1307;
	real term_1308;
	real term_1309;
	real term_1310;
	real term_1311;
	real term_1312;
	real term_1313;
	real term_1314;
	real term_1315;
	real term_1316;
	real term_1317;
	real term_1318;
	real term_1319;
	real term_1320;
	real term_1321;
	real term_1322;
	real term_1323;
	real term_1324;
	real term_1325;
	real term_1326;
	real term_1327;
	real term_1328;
	real term_1329;
	real term_1330;
	real term_1331;
	real term_1332;
	real term_1333;
	real term_1334;
	real term_1335;
	real term_1336;
	real term_1337;
	real term_1338;
	real term_1339;
	real term_1340;
	real term_1341;
	real term_1342;
	real term_1343;
	real term_1344;
	real term_1345;
	real term_1346;
	real term_1347;
	real term_1348;

	term_0 = rotation[4];
	term_1 = rotation[1];
	term_2 = rotation[5];
	term_3 = rotation[6];
	term_4 = rotation[8];
	term_5 = rotation[0];
	term_6 = rotation[2];
	term_7 = rotation[3];
	matrix[2] = term_1;
	matrix[0] = term_0;
	term_8 = 0.0f - term_2;
	term_9 = rotation[7];
	matrix[3] = term_8;
	term_10 = term_4;
	matrix[4] = term_10;
	term_11 = 0.0f - term_3;
	result[0] = coefficients[0];
	term_12 = 0.0f - term_9;
	term_13 = 0.0f - term_6;
	matrix[1] = term_12;
	matrix[5] = term_13;
	matrix[6] = term_7;
	matrix[7] = term_11;
	matrix[8] = term_5;
	function_143250(result + 1, 3, &matrix[0], coefficients + 1);
	if (order <= 2) return result;
	term_14 = term_7;
	term_15 = sqrt(5.0);
	term_16 = term_5;
	term_17 = term_0;
	term_18 = term_1;
	term_19 = term_3;
	term_20 = term_9;
	term_21 = term_14 * term_16;
	term_22 = term_17 * term_18;
	term_23 = term_19 * term_14;
	term_24 = term_20 * term_17;
	term_25 = term_19 * term_19;
	term_26 = term_20 * term_20;
	term_27 = term_19 * term_16;
	term_28 = term_20 * term_18;
	term_29 = term_16 * term_16;
	term_30 = term_18 * term_18;
	term_31 = term_14 * term_14;
	term_32 = term_17 * term_17;
	term_33 = term_17 * term_16;
	term_34 = term_14 * term_18;
	term_35 = term_33 + term_34;
	term_36 = term_19 * term_17;
	matrix[0] = term_35;
	term_37 = term_20 * term_14;
	term_38 = 0.0f - term_37;
	term_39 = term_38 - term_36;
	matrix[1] = term_39;
	term_40 = sqrt(15.0);
	term_41 = term_15 * term_40;
	term_42 = (real)term_41;
	term_43 = term_42;
	term_44 = term_43 * term_20;
	term_45 = term_44 * term_19;
	term_46 = term_45 * 0.200000003f;
	matrix[2] = term_46;
	term_47 = term_19 * term_18;
	term_48 = term_20 * term_16;
	term_49 = 0.0f - term_48;
	term_50 = term_49 - term_47;
	matrix[3] = term_50;
	term_51 = term_17 * term_14;
	term_52 = term_18 * term_16;
	term_53 = term_52 - term_51;
	term_54 = term_2;
	term_55 = term_54 * term_18;
	matrix[4] = term_53;
	term_56 = term_6;
	term_57 = term_17 * term_56;
	term_58 = 0.0f - term_57;
	term_59 = term_58 - term_55;
	matrix[5] = term_59;
	term_60 = term_4;
	term_61 = term_60 * term_17;
	term_62 = term_20 * term_2;
	term_63 = term_62 + term_61;
	matrix[6] = term_63;
	term_64 = term_43 * term_4;
	term_65 = term_64 * term_20;
	term_66 = term_65 * -0.200000003f;
	term_67 = term_20 * term_56;
	matrix[7] = term_66;
	term_68 = term_4;
	term_69 = term_68 * term_18;
	term_70 = term_67 + term_69;
	term_71 = term_2;
	matrix[8] = term_70;
	term_72 = term_71 * term_17;
	term_73 = term_56 * term_18;
	term_74 = term_72 - term_73;
	matrix[9] = term_74;
	term_75 = term_21;
	term_76 = term_71 * term_56;
	term_77 = term_76 * 0.13333334f;
	term_78 = 0.0666666701f;
	term_79 = term_75 * term_78;
	term_80 = term_77 - term_79;
	term_81 = term_22;
	term_82 = term_80 * term_43;
	term_83 = term_81 * term_43;
	term_84 = term_83 * term_78;
	term_85 = term_82 - term_84;
	matrix[10] = term_85;
	term_86 = term_23;
	term_87 = term_86 * term_43;
	term_88 = term_64 * term_71;
	term_89 = term_88 * 0.13333334f;
	term_90 = term_87 * term_78;
	term_91 = term_90 - term_89;
	term_92 = term_24;
	term_93 = term_25;
	term_94 = term_92 * term_43;
	term_95 = term_94 * term_78;
	term_96 = term_91 + term_95;
	term_97 = 0.5f;
	term_98 = term_93 * term_97;
	matrix[11] = term_96;
	term_99 = term_4;
	term_100 = term_99 * term_99;
	term_101 = term_100 - term_98;
	term_102 = term_26;
	term_103 = term_102 * term_97;
	term_104 = term_101 - term_103;
	matrix[12] = term_104;
	term_105 = term_27;
	term_106 = term_29;
	term_107 = term_105 * term_43;
	term_108 = term_107 * term_78;
	term_109 = term_64 * term_56;
	term_110 = term_109 * 0.13333334f;
	term_111 = term_108 - term_110;
	term_112 = term_28;
	term_113 = term_112 * term_43;
	term_114 = term_113 * term_78;
	term_115 = term_111 + term_114;
	term_116 = term_2;
	matrix[13] = term_115;
	term_117 = term_116 * term_116;
	term_118 = term_117 * term_78;
	term_119 = term_56 * term_56;
	term_120 = term_119 * term_43;
	term_121 = term_120 * term_78;
	term_122 = 0.0333333351f;
	term_123 = term_106 * term_122;
	term_124 = term_118 + term_123;
	term_125 = term_7;
	term_126 = term_124 * term_43;
	term_127 = term_121 - term_126;
	term_128 = term_30;
	term_129 = term_128 * term_43;
	term_130 = term_129 * term_122;
	term_131 = term_32;
	term_132 = term_127 - term_130;
	term_133 = term_131 + term_31;
	term_134 = term_133 * term_43;
	term_135 = term_134 * 0.0333333351f;
	term_136 = term_132 + term_135;
	term_137 = term_2;
	matrix[14] = term_136;
	term_138 = term_125 * term_56;
	term_139 = term_137 * term_16;
	term_140 = 0.0f - term_139;
	term_141 = term_140 - term_138;
	matrix[15] = term_141;
	term_142 = term_3;
	term_143 = term_142 * term_2;
	term_144 = term_4;
	term_145 = term_7;
	term_146 = term_144 * term_145;
	term_147 = term_146 + term_143;
	term_148 = term_3;
	term_149 = term_64 * term_148;
	term_150 = term_149 * -0.200000003f;
	matrix[17] = term_150;
	term_151 = term_4;
	term_152 = term_148 * term_56;
	matrix[16] = term_147;
	term_153 = term_151 * term_16;
	term_154 = term_131 + term_29;
	term_155 = term_153 + term_152;
	term_156 = term_2;
	term_157 = term_156 * term_145;
	term_158 = term_56 * term_16;
	term_159 = term_26;
	term_160 = term_157 - term_158;
	matrix[19] = term_160;
	term_161 = term_21;
	term_162 = term_161 - term_22;
	matrix[20] = term_162;
	term_163 = term_24;
	term_164 = term_163 - term_23;
	matrix[21] = term_164;
	term_165 = term_25;
	term_166 = term_165 * term_43;
	term_167 = term_159 * term_43;
	term_168 = term_28;
	term_169 = term_168 - term_27;
	term_170 = 0.100000001f;
	term_171 = term_166 * term_170;
	term_172 = term_167 * term_170;
	term_173 = term_171 - term_172;
	matrix[22] = term_173;
	term_174 = term_30;
	matrix[23] = term_169;
	term_175 = 0.5f;
	term_176 = term_174 * term_175;
	term_177 = term_154 * term_175;
	term_178 = term_177 - term_176;
	term_179 = term_31;
	term_180 = term_179 * term_175;
	term_181 = term_178 - term_180;
	matrix[18] = term_155;
	matrix[24] = term_181;
	function_143250(result + 4, 5, &matrix[0], coefficients + 4);
	if (order <= 3) return result;
	term_182 = term_0;
	term_183 = sqrt(2.0);
	term_184 = term_31;
	term_185 = term_21;
	term_186 = term_1;
	term_187 = term_184 * term_182;
	term_188 = term_22;
	term_189 = term_25;
	term_190 = term_185 * term_186;
	term_191 = term_29;
	term_192 = term_191 * term_182;
	term_193 = term_30;
	term_194 = term_193 * term_182;
	term_195 = term_32;
	term_196 = term_195 * term_182;
	term_197 = (real)term_183;
	term_198 = sqrt(35.0);
	term_199 = (real)term_198;
	term_200 = term_199;
	term_201 = sqrt(105.0);
	term_202 = term_200 * term_197;
	term_203 = (real)term_201;
	term_204 = term_203;
	term_205 = sqrt(21.0);
	term_206 = term_204 * term_202;
	term_207 = term_34;
	term_208 = term_3;
	term_209 = term_207 * term_208;
	term_210 = term_9;
	term_211 = term_188 * term_210;
	term_212 = term_33;
	term_213 = term_212 * term_208;
	term_214 = term_185 * term_210;
	term_215 = term_189 * term_182;
	term_216 = term_26;
	term_217 = term_216 * term_182;
	term_218 = term_23;
	term_219 = term_218 * term_210;
	term_220 = (real)term_205;
	term_221 = term_205 * term_199;
	term_222 = (real)term_221;
	term_223 = sqrt(7.0);
	term_224 = (real)term_223;
	term_225 = term_224;
	term_226 = term_225 * term_210;
	term_227 = term_226 * term_189;
	term_228 = term_216;
	term_229 = term_224;
	term_230 = term_229 * term_228;
	term_231 = term_230 * term_210;
	term_232 = term_27;
	term_233 = term_232 * term_210;
	term_234 = term_189;
	term_235 = term_234 * term_186;
	term_236 = term_228 * term_186;
	term_237 = term_51;
	term_238 = term_237 * term_208;
	term_239 = term_195;
	term_240 = term_204 * term_184;
	term_241 = term_240 * term_210;
	term_242 = term_204 * term_193;
	term_243 = term_242 * term_210;
	term_244 = term_52;
	term_245 = term_244 * term_208;
	term_246 = term_204 * term_191;
	term_247 = term_246 * term_210;
	term_248 = term_204 * term_239;
	term_249 = term_248 * term_210;
	term_250 = term_193;
	term_251 = term_250 * term_186;
	term_252 = term_185 * term_182;
	term_253 = term_36;
	term_254 = term_239 * term_186;
	term_255 = term_191;
	term_256 = term_255 * term_186;
	term_257 = term_184;
	term_258 = term_257 * term_186;
	term_259 = term_57;
	term_260 = term_138;
	term_261 = term_143;
	term_262 = term_204 * term_197;
	term_263 = term_224;
	term_264 = term_263 * term_204;
	term_265 = term_47;
	term_266 = term_262 * term_220;
	term_267 = term_152;
	term_268 = term_158;
	term_269 = term_73;
	term_270 = term_72;
	term_271 = term_157;
	term_272 = term_119;
	term_273 = term_76;
	term_274 = term_100;
	term_275 = term_220;
	term_276 = term_275 * term_197;
	term_277 = term_203;
	term_278 = term_117;
	term_279 = term_278 * term_277;
	term_280 = term_272;
	term_281 = term_280 * term_277;
	term_282 = term_224;
	term_283 = term_282 * term_197;
	term_284 = term_199;
	term_285 = term_284 * term_2;
	term_286 = term_6;
	term_287 = term_285 * term_193;
	term_288 = term_283;
	term_289 = term_288 * term_199;
	term_290 = term_185 * term_286;
	term_291 = term_188;
	term_292 = term_291 * term_286;
	term_293 = term_199;
	term_294 = term_293 * term_195;
	term_295 = term_294 * term_2;
	term_296 = term_199;
	term_297 = term_296 * term_184;
	term_298 = term_297 * term_2;
	term_299 = term_285;
	term_300 = term_299 * term_191;
	term_301 = term_283;
	term_302 = term_4;
	term_303 = term_185 * term_302;
	term_304 = term_260;
	term_305 = term_304 * term_208;
	term_306 = term_139;
	term_307 = term_306 * term_208;
	term_308 = term_188;
	term_309 = term_308 * term_302;
	term_310 = term_55;
	term_311 = term_310 * term_210;
	term_312 = term_259;
	term_313 = term_312 * term_210;
	term_314 = term_220;
	term_315 = term_301 * term_314;
	term_316 = term_218;
	term_317 = term_316 * term_302;
	term_318 = term_314 * term_2;
	term_319 = term_318 * term_189;
	term_320 = term_302 * term_210;
	term_321 = term_318;
	term_322 = term_320 * term_182;
	term_323 = term_216;
	term_324 = term_321 * term_323;
	term_325 = term_323 * term_302;
	term_326 = term_189;
	term_327 = term_326 * term_302;
	term_328 = term_320;
	term_329 = term_328 * term_186;
	term_330 = term_232;
	term_331 = term_330 * term_302;
	term_332 = term_220;
	term_333 = term_332 * term_6;
	term_334 = term_333 * term_189;
	term_335 = term_333;
	term_336 = term_335 * term_323;
	term_337 = term_269;
	term_338 = term_337 * term_210;
	term_339 = term_184;
	term_340 = term_339 * term_302;
	term_341 = term_271;
	term_342 = term_341 * term_208;
	term_343 = term_191;
	term_344 = term_343 * term_302;
	term_345 = term_195;
	term_346 = term_345 * term_302;
	term_347 = term_193;
	term_348 = term_347 * term_302;
	term_349 = term_268;
	term_350 = term_349 * term_208;
	term_351 = term_270;
	term_352 = term_351 * term_210;
	term_353 = term_199;
	term_354 = term_353 * term_6;
	term_355 = term_354 * term_195;
	term_356 = term_185;
	term_357 = term_356 * term_2;
	term_358 = term_199;
	term_359 = term_358 * term_191;
	term_360 = term_359 * term_6;
	term_361 = term_199;
	term_362 = term_361 * term_193;
	term_363 = term_362 * term_6;
	term_364 = term_188;
	term_365 = term_364 * term_2;
	term_366 = term_354 * term_339;
	term_367 = term_212;
	term_368 = term_367 * term_186;
	term_369 = term_195;
	term_370 = term_7;
	term_371 = term_369 * term_370;
	term_372 = term_191;
	term_373 = term_372 * term_370;
	term_374 = term_339 * term_370;
	term_375 = term_193;
	term_376 = term_375 * term_370;
	term_377 = term_185;
	term_378 = term_377 * term_208;
	term_379 = term_188;
	term_380 = term_379 * term_208;
	term_381 = term_207;
	term_382 = term_381 * term_210;
	term_383 = term_367 * term_210;
	term_384 = term_189;
	term_385 = term_384 * term_370;
	term_386 = term_253;
	term_387 = term_386 * term_210;
	term_388 = term_216;
	term_389 = term_388 * term_370;
	term_390 = term_224;
	term_391 = term_390 * term_208;
	term_392 = term_216;
	term_393 = term_391 * term_392;
	term_394 = term_224;
	term_395 = term_394 * term_384;
	term_396 = term_395 * term_208;
	term_397 = term_265;
	term_398 = term_397 * term_210;
	term_399 = term_5;
	term_400 = term_384 * term_399;
	term_401 = term_248;
	term_402 = term_401 * term_208;
	term_403 = term_240;
	term_404 = term_403 * term_208;
	term_405 = term_237;
	term_406 = term_405 * term_210;
	term_407 = term_242;
	term_408 = term_407 * term_208;
	term_409 = term_246;
	term_410 = term_409 * term_208;
	term_411 = term_244;
	term_412 = term_411 * term_210;
	term_413 = term_207;
	term_414 = term_413 * term_182;
	term_415 = term_191;
	term_416 = term_392 * term_399;
	term_417 = term_415 * term_399;
	term_418 = term_195;
	term_419 = term_192;
	term_420 = term_339 * term_399;
	term_421 = term_418 * term_399;
	term_422 = term_206;
	term_423 = term_193;
	term_424 = term_423 * term_399;
	term_425 = term_190;
	term_426 = term_425 * 1.5f;
	term_427 = term_215;
	term_428 = term_219;
	term_429 = 0.75f;
	term_430 = term_419 * term_429;
	term_431 = term_430 + term_426;
	term_432 = term_187;
	term_433 = term_432 * term_429;
	term_434 = term_431 - term_433;
	term_435 = term_194;
	term_436 = term_435 * term_429;
	term_437 = term_196;
	term_438 = term_437 * 0.25f;
	term_439 = term_434 - term_436;
	term_440 = term_209;
	term_441 = term_439 + term_438;
	term_442 = 0.0142857144f;
	matrix[0] = term_441;
	term_443 = term_211;
	term_444 = term_443 * term_442;
	term_445 = term_440 * term_442;
	term_446 = term_444 - term_445;
	term_447 = term_213;
	term_448 = term_447 * term_422;
	term_449 = term_217;
	term_450 = term_448 * term_442;
	term_451 = term_446 * term_422;
	term_452 = term_451 - term_450;
	term_453 = term_214;
	term_454 = term_453 * term_422;
	term_455 = term_454 * term_442;
	term_456 = term_452 - term_455;
	term_457 = term_222;
	term_458 = term_427 * term_457;
	term_459 = term_449 * term_457;
	matrix[1] = term_456;
	term_460 = 0.0357142873f;
	term_461 = term_458 * term_460;
	term_462 = term_459 * term_460;
	term_463 = term_461 - term_462;
	term_464 = 0.0714285746f;
	term_465 = term_428 * term_457;
	term_466 = term_465 * term_464;
	term_467 = term_463 + term_466;
	term_468 = term_227;
	term_469 = term_468 * 0.107142858f;
	matrix[2] = term_467;
	term_470 = term_231;
	term_471 = term_470 * term_460;
	term_472 = term_471 - term_469;
	term_473 = term_472 * term_202;
	term_474 = term_233;
	matrix[3] = term_473;
	term_475 = term_235;
	term_476 = term_474 * term_464;
	term_477 = term_236;
	term_478 = term_475 * term_460;
	term_479 = term_477 * term_457;
	term_480 = term_479 * term_460;
	term_481 = 0.00714285718f;
	term_482 = term_478 + term_476;
	term_483 = term_482 * term_457;
	term_484 = term_483 - term_480;
	term_485 = term_243;
	term_486 = term_485 + term_241;
	matrix[4] = term_484;
	term_487 = term_202;
	term_488 = term_486 * term_487;
	term_489 = term_238;
	term_490 = term_489 * term_422;
	term_491 = term_490 * term_442;
	term_492 = term_488 * term_481;
	term_493 = term_492 + term_491;
	term_494 = term_245;
	term_495 = term_494 * term_422;
	term_496 = term_247;
	term_497 = term_496 * term_487;
	term_498 = term_497 * term_481;
	term_499 = term_495 * term_442;
	term_500 = term_493 - term_499;
	term_501 = term_500 - term_498;
	term_502 = term_249;
	term_503 = term_502 * term_487;
	term_504 = term_256;
	term_505 = term_504 * 0.75f;
	term_506 = term_503 * term_481;
	term_507 = term_501 - term_506;
	term_508 = term_252;
	term_509 = term_508 * 1.5f;
	term_510 = term_237;
	matrix[5] = term_507;
	term_511 = term_251;
	term_512 = term_511 * 0.25f;
	term_513 = term_509 + term_512;
	term_514 = term_254;
	term_515 = term_505 - term_513;
	term_516 = 0.75f;
	term_517 = term_514 * term_516;
	term_518 = term_515 + term_517;
	term_519 = term_258;
	term_520 = term_519 * term_516;
	term_521 = term_206;
	term_522 = term_510 * term_521;
	term_523 = term_522 * term_2;
	term_524 = term_518 - term_520;
	term_525 = term_6;
	matrix[6] = term_524;
	term_526 = term_212;
	term_527 = term_526 * term_521;
	term_528 = term_207;
	term_529 = term_528 * term_521;
	term_530 = term_529 * term_525;
	term_531 = term_523 * term_442;
	term_532 = term_530 * term_442;
	term_533 = term_531 - term_532;
	term_534 = term_139;
	term_535 = term_534 * term_521;
	term_536 = term_535 * term_1;
	term_537 = term_527;
	term_538 = term_537 * term_525;
	term_539 = term_3;
	term_540 = term_538 * term_442;
	term_541 = term_536 * term_442;
	term_542 = term_533 - term_541;
	term_543 = term_9;
	term_544 = term_542 - term_540;
	term_545 = term_260;
	term_546 = term_545 * term_543;
	term_547 = term_55;
	term_548 = term_547 * term_539;
	matrix[7] = term_544;
	term_549 = term_546;
	term_550 = term_549 + term_548;
	term_551 = term_139;
	term_552 = term_551 * term_543;
	term_553 = term_550 + term_552;
	term_554 = term_259;
	term_555 = term_554 * term_539;
	term_556 = term_553 + term_555;
	term_557 = term_212;
	term_558 = term_4;
	term_559 = term_556;
	term_560 = term_557 * term_558;
	term_561 = term_559 + term_560;
	term_562 = term_207;
	term_563 = term_562 * term_558;
	term_564 = term_561 + term_563;
	term_565 = term_253;
	term_566 = term_565 * term_266;
	term_567 = term_566 * term_558;
	term_568 = term_567 * -0.0238095243f;
	matrix[8] = term_564;
	term_569 = term_266 * term_558;
	term_570 = 0.0238095243f;
	term_571 = term_569 * term_543;
	term_572 = term_571 * term_7;
	term_573 = term_572 * term_570;
	term_574 = term_568 - term_573;
	term_575 = term_261;
	term_576 = term_575 * term_266;
	term_577 = term_576 * term_543;
	term_578 = term_577 * term_570;
	term_579 = term_574 - term_578;
	term_580 = term_4;
	matrix[9] = term_579;
	term_581 = term_264;
	term_582 = term_581 * term_580;
	term_583 = term_582 * term_543;
	term_584 = term_583 * term_3;
	term_585 = term_584 * 0.142857149f;
	matrix[10] = term_585;
	term_586 = term_265;
	term_587 = term_586 * term_266;
	term_588 = term_587 * term_580;
	term_589 = term_588 * -0.0238095243f;
	term_590 = term_571;
	term_591 = term_590 * term_5;
	term_592 = term_591 * term_570;
	term_593 = term_589 - term_592;
	term_594 = term_267;
	term_595 = term_594 * term_266;
	term_596 = term_595 * term_543;
	term_597 = term_596 * term_570;
	term_598 = term_593 - term_597;
	term_599 = term_268;
	matrix[11] = term_598;
	term_600 = term_269;
	term_601 = term_600 * term_3;
	term_602 = term_260;
	term_603 = term_602 * term_0;
	term_604 = term_599 * term_543;
	term_605 = term_604 + term_601;
	term_606 = term_244;
	term_607 = term_606 * term_4;
	term_608 = term_605 + term_607;
	term_609 = term_270;
	term_610 = term_609 * term_3;
	term_611 = term_608 - term_610;
	term_612 = term_271;
	term_613 = term_612 * term_543;
	term_614 = term_611 - term_613;
	term_615 = term_237;
	term_616 = term_615 * term_4;
	term_617 = term_614 - term_616;
	term_618 = term_207;
	term_619 = term_618 * term_2;
	term_620 = term_603 + term_619;
	term_621 = term_244;
	matrix[12] = term_617;
	term_622 = term_206;
	term_623 = term_621 * term_622;
	term_624 = term_623 * term_6;
	term_625 = term_624 * term_442;
	term_626 = term_620 * term_622;
	term_627 = term_272;
	term_628 = term_626 * term_442;
	term_629 = term_628 - term_625;
	term_630 = term_527;
	term_631 = term_630 * term_2;
	term_632 = term_631 * term_442;
	term_633 = term_629 + term_632;
	term_634 = term_117;
	term_635 = term_634 * term_457;
	matrix[13] = term_633;
	term_636 = term_627 * term_0;
	term_637 = term_636 * 0.0285714287f;
	term_638 = term_192;
	term_639 = term_638 * term_481;
	term_640 = term_637 - term_639;
	term_641 = term_640 * term_457;
	term_642 = term_457 * term_194;
	term_643 = term_642 * 0.021428572f;
	term_644 = term_641 - term_643;
	term_645 = term_55;
	term_646 = term_645 * term_6;
	term_647 = term_646 * 0.0571428575f;
	term_648 = term_187;
	term_649 = term_648 * term_481;
	term_650 = term_647 + term_649;
	term_651 = term_635;
	term_652 = term_651 * term_0;
	term_653 = term_652 * 0.0285714287f;
	term_654 = term_650 * term_457;
	term_655 = term_644 + term_654;
	term_656 = term_655 - term_653;
	term_657 = term_457 * term_196;
	term_658 = term_657 * term_481;
	term_659 = term_656 + term_658;
	term_660 = term_457 * term_190;
	term_661 = term_660 * term_442;
	term_662 = term_659 - term_661;
	term_663 = term_273;
	term_664 = term_663 * term_266;
	term_665 = term_55;
	matrix[14] = term_662;
	term_666 = term_266 * term_665;
	term_667 = term_666 * term_4;
	term_668 = term_664;
	term_669 = term_668 * term_543;
	term_670 = term_667 + term_669;
	term_671 = term_670 * 0.0190476198f;
	term_672 = term_266 * term_213;
	term_673 = term_672 * 0.00476190494f;
	term_674 = term_673 - term_671;
	term_675 = 0.25f;
	term_676 = term_266 * term_259;
	term_677 = term_676 * term_4;
	term_678 = term_677 * 0.0190476198f;
	term_679 = term_674 - term_678;
	term_680 = term_214;
	term_681 = term_680 + term_209;
	term_682 = term_681 * 0.00476190494f;
	term_683 = term_682 + term_444;
	term_684 = term_683 * term_266;
	term_685 = term_679 + term_684;
	term_686 = term_217;
	term_687 = term_686 * 0.75f;
	matrix[15] = term_685;
	term_688 = term_274;
	term_689 = term_688 * term_0;
	term_690 = term_689 - term_687;
	term_691 = term_320;
	term_692 = term_691 * term_2;
	term_693 = term_692 * 2.0f;
	term_694 = term_690 + term_693;
	term_695 = term_219;
	term_696 = term_695 * 0.5f;
	term_697 = term_694 - term_696;
	term_698 = term_215;
	term_699 = term_698 * term_675;
	term_700 = term_697 - term_699;
	term_701 = term_227;
	term_702 = term_701 * 0.0357142873f;
	term_703 = term_702 + term_471;
	matrix[16] = term_700;
	term_704 = term_276;
	term_705 = term_703 * term_704;
	term_706 = term_704 * term_274;
	term_707 = term_706 * term_226;
	term_708 = term_707 * 0.142857149f;
	term_709 = term_705 - term_708;
	term_710 = term_235;
	term_711 = term_710 * term_675;
	matrix[17] = term_709;
	term_712 = term_320;
	term_713 = term_712 * term_6;
	term_714 = term_713 * 2.0f;
	term_715 = term_714 - term_711;
	term_716 = term_236;
	term_717 = term_716 * 0.75f;
	term_718 = term_715 - term_717;
	term_719 = term_274;
	term_720 = term_719 * term_1;
	term_721 = term_718 + term_720;
	term_722 = term_233;
	term_723 = term_722 * 0.5f;
	term_724 = term_721 - term_723;
	term_725 = term_276;
	matrix[18] = term_724;
	term_726 = term_281;
	term_727 = term_726 * term_725;
	term_728 = term_270;
	term_729 = term_728 * term_266;
	term_730 = term_729 * term_4;
	term_731 = term_730 * 0.0190476198f;
	term_732 = term_725 * term_243;
	term_733 = term_732 * term_481;
	term_734 = term_731 + term_733;
	term_735 = term_725 * term_249;
	term_736 = term_735 * term_481;
	term_737 = term_734 - term_736;
	term_738 = term_279;
	term_739 = term_738 * term_725;
	term_740 = term_725 * term_241;
	term_741 = term_740 * 0.00238095247f;
	term_742 = term_739 * term_543;
	term_743 = term_742 * 0.00952380989f;
	term_744 = term_737 + term_743;
	term_745 = term_744 - term_741;
	term_746 = term_269;
	term_747 = term_746 * term_266;
	term_748 = term_747 * term_4;
	term_749 = term_748 * 0.0190476198f;
	term_750 = term_745 - term_749;
	term_751 = 0.00476190494f;
	term_752 = term_266 * term_245;
	term_753 = term_752 * term_751;
	term_754 = term_750 + term_753;
	term_755 = term_727;
	term_756 = term_755 * term_543;
	term_757 = term_756 * 0.00952380989f;
	term_758 = term_754 - term_757;
	term_759 = term_266 * term_238;
	term_760 = term_759 * term_751;
	term_761 = term_758 - term_760;
	term_762 = term_276;
	term_763 = term_762 * term_247;
	term_764 = term_763 * 0.00238095247f;
	term_765 = term_761 + term_764;
	term_766 = term_259;
	term_767 = term_766 * term_2;
	term_768 = term_767 * 0.0571428575f;
	matrix[19] = term_765;
	term_769 = term_258;
	term_770 = term_769 * term_481;
	term_771 = term_770 - term_768;
	term_772 = term_252;
	term_773 = term_772 * term_442;
	term_774 = term_771 + term_773;
	term_775 = term_256;
	term_776 = term_775 * term_457;
	term_777 = term_774 * term_457;
	term_778 = term_776 * term_481;
	term_779 = term_272;
	term_780 = term_779 * term_1;
	term_781 = term_780 * 0.0285714287f;
	term_782 = term_254;
	term_783 = term_777 - term_778;
	term_784 = 0.021428572f;
	term_785 = term_782 * term_784;
	term_786 = term_781 + term_785;
	term_787 = term_635;
	term_788 = term_787 * term_1;
	term_789 = term_788 * 0.0285714287f;
	term_790 = term_786 * term_457;
	term_791 = term_783 + term_790;
	term_792 = term_791 - term_789;
	term_793 = term_251;
	term_794 = term_290;
	term_795 = term_794 * term_289;
	term_796 = term_795 * 0.042857144f;
	term_797 = term_793 * term_457;
	term_798 = term_797 * term_481;
	term_799 = term_792 - term_798;
	term_800 = term_283;
	matrix[20] = term_799;
	term_801 = term_287;
	term_802 = term_801 * term_800;
	term_803 = term_802 * term_784;
	term_804 = term_796 + term_803;
	term_805 = term_285;
	term_806 = term_805 * term_800;
	term_807 = term_806 * term_272;
	term_808 = term_807 * 0.042857144f;
	term_809 = term_804 - term_808;
	term_810 = term_292;
	term_811 = term_810 * term_289;
	term_812 = term_811 * 0.042857144f;
	term_813 = term_295;
	term_814 = term_809 + term_812;
	term_815 = term_283;
	term_816 = term_813 * term_815;
	term_817 = term_816 * term_784;
	term_818 = term_814 - term_817;
	term_819 = term_298;
	term_820 = term_819 * term_815;
	term_821 = term_117;
	term_822 = term_821 * term_199;
	term_823 = term_822 * term_2;
	term_824 = term_820 * term_784;
	term_825 = term_818 - term_824;
	term_826 = term_300;
	term_827 = term_826 * term_784;
	term_828 = term_273;
	term_829 = term_823 * term_442;
	term_830 = term_829 + term_827;
	term_831 = term_830 * term_283;
	term_832 = 0.0285714287f;
	term_833 = term_825 + term_831;
	term_834 = term_264;
	matrix[21] = term_833;
	term_835 = term_303;
	term_836 = term_835 * term_834;
	term_837 = term_836 * term_832;
	term_838 = term_828 * term_834;
	term_839 = term_838 * term_4;
	term_840 = term_839 * 0.0571428575f;
	term_841 = term_840 - term_837;
	term_842 = term_305;
	term_843 = term_842 * term_834;
	term_844 = term_843 * term_832;
	term_845 = term_841 - term_844;
	term_846 = term_307;
	term_847 = term_846 * term_834;
	term_848 = term_847 * term_832;
	term_849 = term_845 - term_848;
	term_850 = term_309;
	term_851 = term_850 * term_834;
	term_852 = term_851 * term_832;
	term_853 = term_849 - term_852;
	term_854 = term_311;
	term_855 = term_854 * term_834;
	term_856 = term_855 * term_832;
	term_857 = term_853 - term_856;
	term_858 = term_313;
	term_859 = term_858 * term_834;
	term_860 = term_859 * term_832;
	term_861 = term_857 - term_860;
	term_862 = term_317;
	term_863 = term_862 * term_315;
	term_864 = term_863 * 0.0714285746f;
	matrix[22] = term_861;
	term_865 = term_318;
	term_866 = term_865 * term_283;
	term_867 = term_866 * term_274;
	term_868 = term_867 * 0.0714285746f;
	term_869 = term_864 - term_868;
	term_870 = term_324;
	term_871 = term_870 + term_319;
	term_872 = term_871 * term_283;
	term_873 = term_872 * 0.0357142873f;
	term_874 = term_869 + term_873;
	term_875 = 0.0714285746f;
	term_876 = term_322;
	term_877 = term_876 * term_315;
	term_878 = term_877 * term_875;
	term_879 = term_874 + term_878;
	term_880 = term_327;
	term_881 = term_880 + term_325;
	term_882 = term_881 * 1.5f;
	matrix[23] = term_879;
	term_883 = term_274;
	term_884 = term_883 * term_4;
	term_885 = term_884 - term_882;
	matrix[24] = term_885;
	term_886 = term_331;
	term_887 = term_886 * term_315;
	term_888 = term_887 * term_875;
	term_889 = term_336;
	term_890 = term_889 * term_283;
	term_891 = term_890 * 0.0357142873f;
	term_892 = term_888 + term_891;
	term_893 = term_334;
	term_894 = term_893 * term_283;
	term_895 = term_894 * 0.0357142873f;
	term_896 = term_892 + term_895;
	term_897 = term_329;
	term_898 = term_897 * term_315;
	term_899 = term_898 * 0.0714285746f;
	term_900 = term_340;
	term_901 = term_896 + term_899;
	term_902 = term_333;
	term_903 = term_902 * term_283;
	term_904 = term_903 * term_274;
	term_905 = term_904 * 0.0714285746f;
	term_906 = term_901 - term_905;
	term_907 = 0.0285714287f;
	term_908 = term_900 * term_442;
	matrix[25] = term_906;
	term_909 = term_342;
	term_910 = term_909 * term_907;
	term_911 = term_910 + term_908;
	term_912 = term_338;
	term_913 = term_912 * term_907;
	term_914 = term_911 - term_913;
	term_915 = term_344;
	term_916 = term_915 * term_834;
	term_917 = term_916 * term_442;
	term_918 = term_914 * term_834;
	term_919 = term_918 - term_917;
	term_920 = term_346;
	term_921 = term_920 * term_834;
	term_922 = term_921 * term_442;
	term_923 = term_919 + term_922;
	term_924 = term_348;
	term_925 = term_924 * term_834;
	term_926 = term_925 * term_442;
	term_927 = term_923 - term_926;
	term_928 = term_350;
	term_929 = term_928 * term_834;
	term_930 = term_929 * term_907;
	term_931 = term_272;
	term_932 = term_931 * term_4;
	term_933 = term_932 + term_352;
	term_934 = term_927 - term_930;
	term_935 = term_933 * term_834;
	term_936 = term_935 * 0.0285714287f;
	term_937 = term_934 + term_936;
	term_938 = term_117;
	term_939 = term_938 * term_834;
	term_940 = term_939 * term_4;
	term_941 = term_940 * 0.0285714287f;
	term_942 = term_937 - term_941;
	term_943 = term_354;
	term_944 = term_943 * term_283;
	term_945 = term_272;
	term_946 = term_945 * term_199;
	term_947 = term_946 * term_6;
	term_948 = term_944 * term_938;
	term_949 = term_948 * 0.042857144f;
	term_950 = 0.021428572f;
	matrix[26] = term_942;
	term_951 = term_366;
	term_952 = term_951 * term_950;
	term_953 = term_947 * term_442;
	term_954 = term_953 + term_952;
	term_955 = term_283;
	term_956 = term_954 * term_955;
	term_957 = term_949 - term_956;
	term_958 = term_355;
	term_959 = term_958 * term_955;
	term_960 = term_357;
	term_961 = term_960 * term_289;
	term_962 = term_961 * 0.042857144f;
	term_963 = term_959 * term_950;
	term_964 = term_957 - term_963;
	term_965 = term_964 - term_962;
	term_966 = term_363;
	term_967 = term_966 + term_360;
	term_968 = term_967 * term_283;
	term_969 = term_368;
	term_970 = term_968 * term_950;
	term_971 = term_965 + term_970;
	term_972 = term_365;
	term_973 = term_972 * term_289;
	term_974 = term_973 * 0.042857144f;
	term_975 = term_971 - term_974;
	term_976 = term_371;
	matrix[27] = term_975;
	term_977 = term_635;
	term_978 = term_977 * term_7;
	term_979 = term_978 * 0.0285714287f;
	term_980 = term_976 * term_457;
	term_981 = term_980 * term_481;
	term_982 = term_969 * term_457;
	term_983 = term_982 * term_442;
	term_984 = term_979 + term_983;
	term_985 = term_981 - term_984;
	term_986 = term_373;
	term_987 = term_986 * term_457;
	term_988 = term_987 * term_950;
	term_989 = term_272;
	term_990 = term_989 * term_7;
	term_991 = term_990 * 0.0285714287f;
	term_992 = term_985 - term_988;
	term_993 = term_139;
	term_994 = term_993 * term_6;
	term_995 = term_994 * 0.0571428575f;
	term_996 = term_991 + term_995;
	term_997 = term_374;
	term_998 = term_997 * term_481;
	term_999 = term_996 + term_998;
	term_1000 = term_376;
	term_1001 = term_1000 * term_457;
	term_1002 = term_999 * term_457;
	term_1003 = term_992 + term_1002;
	term_1004 = 0.0190476198f;
	term_1005 = term_1001 * term_481;
	term_1006 = term_1003 - term_1005;
	term_1007 = term_664;
	term_1008 = term_1007 * term_3;
	matrix[28] = term_1006;
	term_1009 = term_378;
	term_1010 = term_1009 * term_266;
	term_1011 = term_1010 * term_442;
	term_1012 = term_1008 * term_1004;
	term_1013 = term_1011 - term_1012;
	term_1014 = term_139;
	term_1015 = term_266 * term_1014;
	term_1016 = term_1015 * term_4;
	term_1017 = term_1016 * term_1004;
	term_1018 = term_382;
	term_1019 = term_1018 + term_380;
	term_1020 = term_1013 - term_1017;
	term_1021 = term_1019 * term_266;
	term_1022 = term_1021 * 0.00476190494f;
	term_1023 = term_1020 + term_1022;
	term_1024 = term_4;
	term_1025 = term_266 * term_260;
	term_1026 = term_1025 * term_1024;
	term_1027 = term_1026 * term_1004;
	term_1028 = term_383;
	term_1029 = term_1028 * term_266;
	term_1030 = term_1029 * 0.00476190494f;
	term_1031 = term_1023 - term_1027;
	term_1032 = term_1031 + term_1030;
	term_1033 = term_261;
	term_1034 = term_1033 * term_1024;
	term_1035 = term_1034 * 2.0f;
	term_1036 = term_385;
	term_1037 = term_1036 * 0.75f;
	term_1038 = term_1035 - term_1037;
	term_1039 = term_387;
	term_1040 = term_1039 * 0.5f;
	term_1041 = term_1038 - term_1040;
	term_1042 = term_389;
	term_1043 = term_1042 * 0.25f;
	term_1044 = term_1041 - term_1043;
	term_1045 = term_274;
	term_1046 = term_393;
	matrix[29] = term_1032;
	term_1047 = term_1045 * term_7;
	term_1048 = term_1044 + term_1047;
	term_1049 = 0.0357142873f;
	matrix[30] = term_1048;
	term_1050 = term_396;
	term_1051 = term_1050 * term_1049;
	term_1052 = term_1046 * term_1049;
	term_1053 = term_276;
	term_1054 = term_1052 + term_1051;
	term_1055 = term_391;
	term_1056 = term_1055 * term_1053;
	term_1057 = term_1056 * term_1045;
	term_1058 = term_1057 * 0.142857149f;
	term_1059 = term_1045 * term_5;
	term_1060 = term_1054 * term_1053;
	term_1061 = term_1060 - term_1058;
	term_1062 = term_398;
	term_1063 = term_1062 * 0.5f;
	term_1064 = term_1059 - term_1063;
	term_1065 = term_400;
	term_1066 = term_1065 * 0.75f;
	term_1067 = term_1064 - term_1066;
	term_1068 = term_416;
	term_1069 = term_1068 * 0.25f;
	term_1070 = term_1067 - term_1069;
	term_1071 = term_267;
	matrix[31] = term_1061;
	term_1072 = term_4;
	term_1073 = term_1071 * term_1072;
	term_1074 = term_1073 * 2.0f;
	term_1075 = term_1070 + term_1074;
	term_1076 = term_727;
	term_1077 = term_1076 * term_3;
	matrix[32] = term_1075;
	term_1078 = term_271;
	term_1079 = term_1078 * term_266;
	term_1080 = term_1079 * term_1072;
	term_1081 = term_402;
	term_1082 = term_1080 * 0.0190476198f;
	term_1083 = term_1081 * term_1053;
	term_1084 = term_1083 * 0.00238095247f;
	term_1085 = term_1077 * 0.00952380989f;
	term_1086 = term_1084 + term_1085;
	term_1087 = term_1082 - term_1086;
	term_1088 = term_404;
	term_1089 = term_1088 * term_1053;
	term_1090 = term_1089 * term_481;
	term_1091 = term_1087 - term_1090;
	term_1092 = term_406;
	term_1093 = term_1092 * term_266;
	term_1094 = term_1093 * 0.00476190494f;
	term_1095 = term_1091 - term_1094;
	term_1096 = term_408;
	term_1097 = term_410;
	term_1098 = term_1096 * term_1053;
	term_1099 = term_1098 * 0.00238095247f;
	term_1100 = term_1095 + term_1099;
	term_1101 = term_268;
	term_1102 = term_1101 * term_266;
	term_1103 = term_1102 * term_4;
	term_1104 = term_1103 * 0.0190476198f;
	term_1105 = term_1100 - term_1104;
	term_1106 = term_279;
	term_1107 = term_1106 * term_3;
	term_1108 = term_1107 * 0.00952380989f;
	term_1109 = term_1097 * term_481;
	term_1110 = term_1108 + term_1109;
	term_1111 = term_420;
	term_1112 = term_1111 * 0.021428572f;
	term_1113 = term_1110 * term_1053;
	term_1114 = term_1105 + term_1113;
	term_1115 = term_412;
	term_1116 = term_1115 * term_266;
	term_1117 = term_1116 * 0.00476190494f;
	term_1118 = term_1114 + term_1117;
	term_1119 = term_414;
	matrix[33] = term_1118;
	term_1120 = term_635;
	term_1121 = term_1120 * term_5;
	term_1122 = term_1121 * 0.0285714287f;
	term_1123 = term_1119 * term_457;
	term_1124 = term_1123 * term_442;
	term_1125 = term_1124 - term_1122;
	term_1126 = term_417;
	term_1127 = term_1126 * term_457;
	term_1128 = term_1127 * term_481;
	term_1129 = term_1125 - term_1128;
	term_1130 = term_272;
	term_1131 = term_1130 * term_5;
	term_1132 = term_1131 * 0.0285714287f;
	term_1133 = term_1132 + term_1112;
	term_1134 = term_421;
	term_1135 = term_1134 * term_481;
	term_1136 = term_1133 + term_1135;
	term_1137 = term_300;
	term_1138 = term_1136 * term_457;
	term_1139 = term_1129 + term_1138;
	term_1140 = term_424;
	term_1141 = term_1140 * term_457;
	term_1142 = term_1141 * term_481;
	term_1143 = term_1139 - term_1142;
	term_1144 = term_260;
	term_1145 = term_1144 * term_457;
	term_1146 = term_1145 * term_2;
	term_1147 = term_1146 * 0.0571428575f;
	term_1148 = term_1143 - term_1147;
	term_1149 = term_287;
	matrix[34] = term_1148;
	term_1150 = term_262;
	term_1151 = term_1149 * term_1150;
	term_1152 = term_1137 * term_1150;
	term_1153 = term_1151 * term_481;
	term_1154 = term_1152 * term_481;
	term_1155 = term_1153 - term_1154;
	term_1156 = term_298;
	term_1157 = term_1156 * term_1150;
	term_1158 = term_1157 * term_481;
	term_1159 = term_1155 + term_1158;
	term_1160 = term_292;
	term_1161 = term_1160 * term_206;
	term_1162 = term_1161 * term_442;
	term_1163 = term_1159;
	term_1164 = term_1163 + term_1162;
	term_1165 = term_290;
	term_1166 = term_1165 * term_206;
	term_1167 = term_1166 * term_442;
	term_1168 = term_1164 - term_1167;
	term_1169 = term_295;
	term_1170 = term_1169 * term_1150;
	term_1171 = term_1170 * term_481;
	term_1172 = term_1168 - term_1171;
	term_1173 = term_307;
	term_1174 = term_1173 + term_305;
	term_1175 = term_1174 + term_303;
	term_1176 = term_1175 - term_309;
	term_1177 = term_1176 - term_313;
	term_1178 = term_1177 - term_311;
	matrix[36] = term_1178;
	term_1179 = term_324;
	term_1180 = term_1179 * term_1150;
	matrix[35] = term_1172;
	term_1181 = 0.0119047621f;
	term_1182 = term_1180 * term_1181;
	term_1183 = term_319;
	term_1184 = term_1183 * term_1150;
	term_1185 = term_1184 * term_1181;
	term_1186 = term_1182;
	term_1187 = term_1186 - term_1185;
	term_1188 = term_317;
	term_1189 = term_1188 * term_266;
	term_1190 = term_1189 * 0.0238095243f;
	term_1191 = term_1187 - term_1190;
	term_1192 = term_322;
	term_1193 = term_1192 * term_266;
	term_1194 = term_1193 * 0.0238095243f;
	term_1195 = term_1191 + term_1194;
	term_1196 = term_327;
	term_1197 = term_1196 * term_264;
	term_1198 = term_1197 * 0.0714285746f;
	term_1199 = term_325;
	term_1200 = term_1199 * term_264;
	term_1201 = term_1200 * 0.0714285746f;
	matrix[37] = term_1195;
	term_1202 = term_1198;
	term_1203 = term_1202 - term_1201;
	term_1204 = 0.0238095243f;
	matrix[38] = term_1203;
	term_1205 = term_329;
	term_1206 = term_1205 * term_1204;
	term_1207 = term_331;
	term_1208 = term_1207 * term_1204;
	term_1209 = term_1206;
	term_1210 = term_1209 - term_1208;
	term_1211 = 0.0119047621f;
	term_1212 = term_1210 * term_266;
	term_1213 = term_334;
	term_1214 = term_1213 * term_1150;
	term_1215 = term_1214 * term_1211;
	term_1216 = term_1212 - term_1215;
	term_1217 = term_336;
	term_1218 = term_1217 * term_1150;
	term_1219 = term_1218 * term_1211;
	term_1220 = term_348;
	term_1221 = term_1216 + term_1219;
	term_1222 = term_346;
	matrix[39] = term_1221;
	term_1223 = 0.5f;
	term_1224 = term_1220 * term_1223;
	term_1225 = term_1222 * term_1223;
	term_1226 = term_1225 - term_1224;
	term_1227 = term_340;
	term_1228 = term_1227 * term_1223;
	term_1229 = term_1226 - term_1228;
	term_1230 = term_344;
	term_1231 = term_1230 * term_1223;
	term_1232 = term_1229 + term_1231;
	term_1233 = term_1232 - term_342;
	term_1234 = term_1233 - term_338;
	term_1235 = term_1234 + term_352;
	term_1236 = term_1235 + term_350;
	matrix[40] = term_1236;
	term_1237 = term_363;
	term_1238 = term_360;
	term_1239 = term_1237 * term_481;
	term_1240 = term_357;
	term_1241 = term_1238 * term_481;
	term_1242 = term_1239 - term_1241;
	term_1243 = term_366;
	term_1244 = term_1243 * term_481;
	term_1245 = term_1242 + term_1244;
	term_1246 = term_1245 * term_1150;
	term_1247 = term_206;
	term_1248 = term_1240 * term_1247;
	term_1249 = term_1248 * term_442;
	term_1250 = term_1246 + term_1249;
	term_1251 = term_355;
	term_1252 = term_1251 * term_1150;
	term_1253 = term_365;
	term_1254 = term_1253 * term_1247;
	term_1255 = term_1254 * term_442;
	term_1256 = term_1252 * term_481;
	term_1257 = term_1250 - term_1256;
	term_1258 = term_1257 - term_1255;
	term_1259 = 0.75f;
	term_1260 = term_376;
	matrix[41] = term_1258;
	term_1261 = term_373;
	term_1262 = term_1261 * term_1259;
	term_1263 = term_1260 * term_1259;
	term_1264 = term_1262 - term_1263;
	term_1265 = term_368;
	term_1266 = term_1265 * 1.5f;
	term_1267 = term_1264 - term_1266;
	term_1268 = term_371;
	term_1269 = term_1268 * term_1259;
	term_1270 = term_1267 + term_1269;
	term_1271 = term_374;
	term_1272 = term_1271 * 0.25f;
	term_1273 = term_1270 - term_1272;
	term_1274 = term_378;
	term_1275 = 0.0357142873f;
	matrix[42] = term_1273;
	term_1276 = term_382;
	term_1277 = term_1276 + term_380;
	term_1278 = term_1277 * term_1247;
	term_1279 = term_1278 * term_442;
	term_1280 = term_1274 * term_1247;
	term_1281 = term_1280 * term_442;
	term_1282 = term_1279 - term_1281;
	term_1283 = term_383;
	term_1284 = term_1283 * term_1247;
	term_1285 = term_1284 * term_442;
	term_1286 = term_1282 + term_1285;
	term_1287 = term_387;
	term_1288 = term_1287 * 0.0714285746f;
	matrix[43] = term_1286;
	term_1289 = term_385;
	term_1290 = term_1289 * 0.0357142873f;
	term_1291 = term_1290 - term_1288;
	term_1292 = term_389;
	term_1293 = term_1291 * term_457;
	term_1294 = term_1292 * term_457;
	term_1295 = term_1294 * term_1275;
	term_1296 = term_1293 - term_1295;
	term_1297 = term_416;
	matrix[44] = term_1296;
	term_1298 = term_393;
	term_1299 = term_1298 * 0.107142858f;
	term_1300 = term_1299 - term_1051;
	term_1301 = term_1300 * term_202;
	matrix[45] = term_1301;
	term_1302 = term_400;
	term_1303 = term_1302 * term_457;
	term_1304 = term_1303 * term_1275;
	term_1305 = term_1297 * term_1275;
	term_1306 = term_398;
	term_1307 = term_1306 * 0.0714285746f;
	term_1308 = term_1305 + term_1307;
	term_1309 = term_1308 * term_457;
	term_1310 = term_202;
	term_1311 = term_1304 - term_1309;
	matrix[46] = term_1311;
	term_1312 = term_404;
	term_1313 = term_1312 * term_1310;
	term_1314 = term_1313 * term_481;
	term_1315 = term_1115 * term_1247;
	term_1316 = term_1315 * term_442;
	term_1317 = term_1316 + term_1314;
	term_1318 = term_410;
	term_1319 = term_1318 * term_1310;
	term_1320 = term_1319 * term_481;
	term_1321 = term_1317 - term_1320;
	term_1322 = term_402;
	term_1323 = term_1322 * term_1310;
	term_1324 = term_1323 * term_481;
	term_1325 = term_1321 - term_1324;
	term_1326 = term_408;
	term_1327 = term_1326 * term_1310;
	term_1328 = term_1327 * term_481;
	term_1329 = term_406;
	term_1330 = term_1329 * term_1247;
	term_1331 = term_1330 * term_442;
	term_1332 = term_421;
	term_1333 = term_1325 + term_1328;
	term_1334 = term_424;
	term_1335 = term_1333 - term_1331;
	term_1336 = 0.75f;
	term_1337 = term_1332 * term_1336;
	term_1338 = term_1334 * term_1336;
	term_1339 = term_1337 - term_1338;
	term_1340 = term_420;
	term_1341 = term_1340 * term_1336;
	term_1342 = term_417;
	term_1343 = term_1342 * 0.25f;
	term_1344 = term_1339 - term_1341;
	term_1345 = term_1344 + term_1343;
	term_1346 = term_414;
	term_1347 = term_1346 * 1.5f;
	term_1348 = term_1345 + term_1347;
	matrix[47] = term_1335;
	matrix[48] = term_1348;
	function_143250(result + 9, 7, &matrix[0], coefficients + 9);
	return result;
}
