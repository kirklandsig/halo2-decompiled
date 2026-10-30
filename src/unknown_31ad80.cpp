// @flags /O2 /GL-
// Havok: initialize a motion's factors and saved state after its base pose.
#include "motion_31ad80.h"

// @retail 0x31ad80
class_31ad80::class_31ad80(void const *position, void const *orientation)
	: class_3114d0(position,orientation)
{
	field_f0 = _mm_setzero_ps();
	field_100 = _mm_setzero_ps();
	saved_linear = _mm_setzero_ps();
	saved_angular = _mm_setzero_ps();
	// The shared base currently models this floating-point field as a word.
	field_10 = (long)0xbf800000;
}
