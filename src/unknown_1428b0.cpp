// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_1428B0.CPP: matrix math */

#include "unknown_11c920.h"
#include "unknown_1428b0.h"
#include <math.h>

// @retail 0x142b80
matrix3x3 *function_142b80(
	matrix3x3 const *in,
	matrix3x3 *out)
{
	if (in == out)
	{
		real t;
		t = in->forward.j; out->forward.j = in->left.i; out->left.i = t;
		t = in->forward.k; out->forward.k = in->up.i; out->up.i = t;
		t = in->left.k; out->left.k = in->up.j; out->up.j = t;
	}
	else
	{
		out->forward.i = in->forward.i;
		out->forward.j = in->left.i;
		out->forward.k = in->up.i;
		out->left.i = in->forward.j;
		out->left.j = in->left.j;
		out->left.k = in->up.j;
		out->up.i = in->forward.k;
		out->up.j = in->left.k;
		out->up.k = in->up.k;
	}
	return out;
}
__inline vector3f *cross3f(
	vector3f const *a,
	vector3f const *b,
	vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
	return result;
}

// @retail 0x142d10
matrix3x3 *function_142d10(
	matrix3x3 *out,
	vector3f const *forward,
	vector3f const *up)
{
	out->forward = *forward;
	vector3f left;
	left.k = up->i * forward->j;
	left.k -= up->j * forward->i;
	left.j = up->k * forward->i - up->i * forward->k;
	left.i = up->j * forward->k;
	left.i -= up->k * forward->j;
	out->left.j = left.j;
	out->left.k = left.k;
	out->left.i = left.i;
	out->up = *up;
	return out;
}

/* out = left * right; the right matrix comes first, which gives retail's
   operand order in the products */
// @retail 0x142eb0
matrix3x3 *function_142eb0(
	matrix3x3 const *right,
	matrix3x3 const *left,
	matrix3x3 *out)
{
	matrix3x3 temp;
	if (right == out)
	{
		temp = *right;
		right = &temp;
	}
	if (left == out)
	{
		temp = *left;
		left = &temp;
	}
	out->forward.i = right->forward.i * left->forward.i + right->left.i * left->forward.j + right->up.i * left->forward.k;
	out->forward.j = right->forward.j * left->forward.i + right->left.j * left->forward.j + right->up.j * left->forward.k;
	out->forward.k = right->forward.k * left->forward.i + right->left.k * left->forward.j + right->up.k * left->forward.k;
	out->left.i = right->forward.i * left->left.i + right->left.i * left->left.j + right->up.i * left->left.k;
	out->left.j = right->forward.j * left->left.i + right->left.j * left->left.j + right->up.j * left->left.k;
	out->left.k = right->forward.k * left->left.i + right->left.k * left->left.j + right->up.k * left->left.k;
	out->up.i = right->forward.i * left->up.i + right->left.i * left->up.j + right->up.i * left->up.k;
	out->up.j = right->forward.j * left->up.i + right->left.j * left->up.j + right->up.j * left->up.k;
	out->up.k = right->forward.k * left->up.i + right->left.k * left->up.j + right->up.k * left->up.k;
	return out;
}

__declspec(noinline) vector3f *function_143070(matrix3x3 const *m, vector3f const *v, vector3f *out);

// @retail 0x143070
vector3f *function_143070(
	matrix3x3 const *m,
	vector3f const *v,
	vector3f *out)
{
	vector3f temp;
	if (v == out)
	{
		temp = *v;
		v = &temp;
	}
	real i = m->up.i * v->k;
	i += m->left.i * v->j;
	i += m->forward.i * v->i;
	out->i = i;
	real j = m->up.j * v->k;
	j += m->forward.j * v->i;
	j += m->left.j * v->j;
	out->j = j;
	real k = m->up.k * v->k;
	k += m->forward.k * v->i;
	k += m->left.k * v->j;
	out->k = k;
	return out;
}

// @retail 0x142da0
matrix3x3 *function_142da0(
	real yaw,
	real pitch,
	real roll,
	matrix3x3 *out)
{
	real cos_roll = (real)cos(roll);
	real sin_roll = (real)sin(roll);
	real cos_pitch = (real)cos(pitch);
	real sin_pitch = (real)sin(pitch);
	real cos_yaw = (real)cos(yaw);
	real sin_yaw = (real)sin(yaw);
	real cc = cos_yaw * cos_roll;
	real sc = sin_yaw * cos_roll;
	real cs = cos_yaw * sin_roll;
	real ss = sin_yaw * sin_roll;
	out->forward.i = cos_yaw * cos_pitch;
	out->forward.j = sin_yaw * cos_pitch;
	out->forward.k = sin_pitch;
	out->left.i = 0.f - cs * sin_pitch - sc;
	out->left.j = cc - ss * sin_pitch;
	out->left.k = cos_pitch * sin_roll;
	out->up.i = ss - cc * sin_pitch;
	out->up.j = 0.f - sc * sin_pitch - cs;
	out->up.k = cos_pitch * cos_roll;
	return out;
}
// @retail 0x142bf0
matrix3x3 *function_142bf0(
	matrix3x3 const *in,
	real scale,
	matrix3x3 *out)
{
	matrix3x3 temp;
	real one_over_scale;
	real const *m;
	real *r = (real *)out;
	short i, j;
	if (in == out)
	{
		temp = *in;
		in = &temp;
	}
	m = (real const *)in;
	one_over_scale = 1.f / scale;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			short i1 = i < 2 ? i + 1 : 0;
			short i2 = i > 0 ? i - 1 : 2;
			short j1 = j < 2 ? j + 1 : 0;
			short j2 = j > 0 ? j - 1 : 2;
			r[j * 3 + i] = (m[i1 * 3 + j1] * m[i2 * 3 + j2] - m[i1 * 3 + j2] * m[i2 * 3 + j1]) * one_over_scale;
		}
	}
	return out;
}

// @retail 0x1429d0
int __stdcall function_1429d0(
	transform4x3f const *matrix,
	long count,
	point3f const *source,
	point3f *destination)
{
	__asm
	{
		mov ecx, matrix
		movss xmm0, dword ptr [ecx + 4]
		movss xmm1, dword ptr [ecx + 0x10]
		movss xmm2, dword ptr [ecx + 0x1c]
		movss xmm3, dword ptr [ecx + 0x28]
		movhps xmm0, qword ptr [ecx + 8]
		movhps xmm1, qword ptr [ecx + 0x14]
		movhps xmm2, qword ptr [ecx + 0x20]
		movhps xmm3, qword ptr [ecx + 0x2c]
		movss xmm4, dword ptr [ecx]
		shufps xmm4, xmm4, 0
		mulps xmm0, xmm4
		mulps xmm1, xmm4
		mulps xmm2, xmm4
		mov ecx, count
		cmp ecx, 0
		jle done
		mov edx, source
		mov eax, destination
		sub eax, edx
	again:
		movss xmm4, dword ptr [edx]
		movss xmm5, dword ptr [edx + 4]
		movss xmm6, dword ptr [edx + 8]
		shufps xmm4, xmm4, 0
		shufps xmm5, xmm5, 0
		shufps xmm6, xmm6, 0
		mulps xmm4, xmm0
		mulps xmm5, xmm1
		mulps xmm6, xmm2
		addps xmm4, xmm5
		addps xmm6, xmm3
		addps xmm4, xmm6
		movss dword ptr [edx + eax], xmm4
		movhps qword ptr [edx + eax + 4], xmm4
		add edx, 0xc
		dec ecx
		jne again
	done:
	}
}
/* naked, all asm: link-time code generation can't see what the body reads or keeps, so
   callers keep the code that fills its inputs and treat the matrices they pass as escaped
   (0xb98e0's stores keep their order, 0xeb960's frame). tools/build.py reads the
   declaration under the marker, as naked goes on a definition only. */
// @retail 0x142a60
int __fastcall function_142a60(
	transform4x3f const *a,
	transform4x3f const *b,
	transform4x3f *result);
__declspec(naked) int __fastcall function_142a60(
	transform4x3f const *a,
	transform4x3f const *b,
	transform4x3f *result)
{
	__asm
	{
		mov eax, [esp + 4]
		movss xmm7, dword ptr [ecx]
		shufps xmm7, xmm7, 0
		movss xmm3, dword ptr [edx]
		movss xmm0, dword ptr [edx + 0x28]
		movss xmm1, dword ptr [edx + 0x2c]
		movss xmm2, dword ptr [edx + 0x30]
		shufps xmm0, xmm0, 0
		shufps xmm1, xmm1, 0
		shufps xmm2, xmm2, 0
		mulss xmm3, xmm7
		movss xmm4, dword ptr [ecx + 4]
		movhps xmm4, qword ptr [ecx + 8]
		movss xmm5, dword ptr [ecx + 0x10]
		movhps xmm5, qword ptr [ecx + 0x14]
		movss xmm6, dword ptr [ecx + 0x1c]
		movhps xmm6, qword ptr [ecx + 0x20]
		mulps xmm0, xmm4
		mulps xmm1, xmm5
		mulps xmm2, xmm6
		addps xmm0, xmm1
		addps xmm0, xmm2
		mulps xmm0, xmm7
		movss xmm1, dword ptr [ecx + 0x28]
		movhps xmm1, qword ptr [ecx + 0x2c]
		addps xmm0, xmm1
		movss xmm2, dword ptr [edx + 0x24]
		movss dword ptr [eax + 0x28], xmm0
		movhps qword ptr [eax + 0x2c], xmm0
		movss xmm1, dword ptr [edx + 0x20]
		movss xmm0, dword ptr [edx + 0x1c]
		shufps xmm2, xmm2, 0
		shufps xmm1, xmm1, 0
		shufps xmm0, xmm0, 0
		mulps xmm2, xmm6
		mulps xmm1, xmm5
		mulps xmm0, xmm4
		addps xmm1, xmm2
		addps xmm0, xmm1
		movss xmm2, dword ptr [edx + 0x18]
		movss dword ptr [eax + 0x1c], xmm0
		movhps qword ptr [eax + 0x20], xmm0
		movss xmm1, dword ptr [edx + 0x14]
		movss xmm0, dword ptr [edx + 0x10]
		shufps xmm2, xmm2, 0
		shufps xmm1, xmm1, 0
		shufps xmm0, xmm0, 0
		mulps xmm2, xmm6
		mulps xmm1, xmm5
		mulps xmm0, xmm4
		addps xmm1, xmm2
		addps xmm0, xmm1
		movss xmm2, dword ptr [edx + 0xc]
		movss dword ptr [eax + 0x10], xmm0
		movhps qword ptr [eax + 0x14], xmm0
		movss xmm1, dword ptr [edx + 8]
		movss xmm0, dword ptr [edx + 4]
		shufps xmm2, xmm2, 0
		shufps xmm1, xmm1, 0
		shufps xmm0, xmm0, 0
		mulps xmm2, xmm6
		mulps xmm1, xmm5
		mulps xmm0, xmm4
		addps xmm1, xmm2
		addps xmm0, xmm1
		movss dword ptr [eax + 4], xmm0
		movhps qword ptr [eax + 8], xmm0
		movss dword ptr [eax], xmm3
		ret 4
	}
}
// @retail 0x1428b0
plane3f *function_1428b0(
	transform4x3f const *matrix,
	plane3f const *plane,
	plane3f *out)
{
	real vi, vj, vk;
	if (matrix->scale == 0.f)
	{
		out->d = 0.f;
	}
	else
	{
		out->d = plane->d - (matrix->position.z * plane->n.k + matrix->position.y * plane->n.j + matrix->position.x * plane->n.i);
		if (matrix->scale != 1.f)
		{
			out->d = out->d / matrix->scale;
		}
	}
	vi = plane->n.i;
	vj = plane->n.j;
	vk = plane->n.k;
	if (matrix->scale != 1.f)
	{
		real inv = 1.f / matrix->scale;
		vi = inv * vi;
		vj = inv * vj;
		vk = inv * vk;
	}
	out->n.i = matrix->rotation.forward.k * vk + matrix->rotation.forward.j * vj + matrix->rotation.forward.i * vi;
	out->n.j = matrix->rotation.left.k * vk + matrix->rotation.left.j * vj + matrix->rotation.left.i * vi;
	out->n.k = matrix->rotation.up.k * vk + matrix->rotation.up.j * vj + matrix->rotation.up.i * vi;
	return out;
}
