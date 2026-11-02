// @flags /O2 /GL-
/* UNKNOWN_31AD80.CPP: the motion with forces (force_motion_31acd0's layout),
   which 0x311940 builds for its part as class_31ad80: the common motion
   (0x3114d0), unit inverse inertia, no force, torque or saved velocities yet,
   and -1 at +0x10. */
#include "derived_311920.h"

class class_31ad80 : public class_3114d0
{
public:
	class_31ad80(void const *position, void const *orientation);

	__m128 saved_linear;
	__m128 saved_angular;
	__m128 inverse_inertia;
	__m128 force;
	__m128 torque;
};

typedef char class_31ad80_size[sizeof(class_31ad80) == 0x110 ? 1 : -1];

// @retail 0x31ad80
class_31ad80::class_31ad80(void const *position, void const *orientation)
	: class_3114d0(position, orientation)
{
	inverse_inertia.m128_f32[0] = 1.0f;
	inverse_inertia.m128_f32[1] = 1.0f;
	inverse_inertia.m128_f32[2] = 1.0f;
	inverse_inertia.m128_f32[3] = 1.0f;
	force = _mm_setzero_ps();
	torque = _mm_setzero_ps();
	saved_linear = _mm_setzero_ps();
	saved_angular = _mm_setzero_ps();
	*(float *)&field_10 = -1.0f;
}
