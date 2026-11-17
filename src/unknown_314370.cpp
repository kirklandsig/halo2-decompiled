// @flags /O2 /Ob1 /GL- /GX
#include "constraint_helpers_314090.h"
#include <float.h>

// Mutable registration records in this group are 0x28 bytes apart. This
// accessor returns the record without inspecting its opaque fields.
struct motion_registration_314370 { unsigned char opaque[0x28]; };
motion_registration_314370 g_4806f8;

// @retail 0x314370
void *c_314710::v3() { return &g_4806f8; }

// Destruction uses the shared reference teardown before freeing tag 0x26.
// @retail 0x3146e0 deleting c_314710

// Start both displacement histories outside the normal range and both
// orientation histories at identity, then derive the scale from the shape.
// @retail 0x314710
c_314710::c_314710(hkEntity *entity)
{
	distance_a = 0.01f;
	angle_a = 0.005f;
	distance_b = 0.1f;
	angle_b = 0.2f;
	__m128 outside = _mm_set_ss(FLT_MAX);
	outside = _mm_shuffle_ps(outside, outside, 0);
	_mm_store_ps(position_a.m128_f32, outside);
	_mm_store_ps(position_b.m128_f32, outside);
	_mm_store_ps(rotation_a.m128_f32, _mm_setzero_ps());
	rotation_a.m128_f32[3] = 1.0f;
	_mm_store_ps(rotation_b.m128_f32, _mm_setzero_ps());
	rotation_b.m128_f32[3] = 1.0f;
	update_radius_squared(*(shape_bounds_314650 const * const *)((unsigned char const *)entity + 0xc));
}

// The serialized form supplies the limits and the cached squared radius.
// @retail 0x3147c0
c_314710::c_314710(s_314320 const *description)
{
	distance_a = description->field_4;
	angle_a = description->field_8;
	distance_b = description->field_c;
	angle_b = description->field_10;
	__m128 outside = _mm_set_ss(FLT_MAX);
	outside = _mm_shuffle_ps(outside, outside, 0);
	_mm_store_ps(position_a.m128_f32, outside);
	_mm_store_ps(position_b.m128_f32, outside);
	_mm_store_ps(rotation_a.m128_f32, _mm_setzero_ps());
	rotation_a.m128_f32[3] = 1.0f;
	_mm_store_ps(rotation_b.m128_f32, _mm_setzero_ps());
	rotation_b.m128_f32[3] = 1.0f;
	radius_squared = *(float const *)&description->field_0;
}
