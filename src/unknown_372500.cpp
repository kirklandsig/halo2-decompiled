// @flags /O2 /GL-
#include "derived_311920.h"

// Volatile output accesses preserve the retail order of scalar matrix and
// padding stores; ordinary stores let the compiler clear padding early.
// @retail 0x372500
void rotation_372500::function_372500(__m128 const *quaternion)
{
	volatile __m128 *output = column;
	float const *q = quaternion->m128_f32;
	float x2 = q[0] + q[0];
	float y2 = q[1] + q[1];
	float z2 = q[2] + q[2];
	float xx = x2 * q[0];
	float xy = y2 * q[0];
	float xz = z2 * q[0];
	float yy = y2 * q[1];
	float yz = z2 * q[1];
	float zz = z2 * q[2];
	float wx = x2 * q[3];
	float wy = y2 * q[3];
	float wz = z2 * q[3];
	output[0].m128_f32[0] = 1.0f - (zz + yy);
	output[0].m128_f32[1] = xy + wz;
	output[0].m128_f32[2] = xz - wy;
	output[0].m128_f32[3] = 0.0f;
	output[1].m128_f32[0] = xy - wz;
	output[1].m128_f32[1] = 1.0f - (zz + xx);
	output[1].m128_f32[2] = wx + yz;
	output[1].m128_f32[3] = 0.0f;
	output[2].m128_f32[0] = wy + xz;
	output[2].m128_f32[1] = yz - wx;
	output[2].m128_f32[2] = 1.0f - (yy + xx);
	output[2].m128_f32[3] = 0.0f;
}
