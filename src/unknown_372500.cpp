// @flags /O2 /GL-
#include "derived_311920.h"

// score 32 (first implementation): arithmetic and size match; padding stores
// are scheduled early. Row pointers, integer padding stores and a row setter
// also score 32. Direct field stores retained.
// @retail 0x372500
void rotation_372500::function_372500(__m128 const *quaternion)
{
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
	column[0].m128_f32[0] = 1.0f - (zz + yy);
	column[0].m128_f32[1] = xy + wz;
	column[0].m128_f32[2] = xz - wy;
	column[0].m128_f32[3] = 0.0f;
	column[1].m128_f32[0] = xy - wz;
	column[1].m128_f32[1] = 1.0f - (zz + xx);
	column[1].m128_f32[2] = wx + yz;
	column[1].m128_f32[3] = 0.0f;
	column[2].m128_f32[0] = wy + xz;
	column[2].m128_f32[1] = yz - wx;
	column[2].m128_f32[2] = 1.0f - (yy + xx);
	column[2].m128_f32[3] = 0.0f;
}
