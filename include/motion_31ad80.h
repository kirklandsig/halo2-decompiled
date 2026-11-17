#ifndef MOTION_31AD80_H
#define MOTION_31AD80_H
// Shared construction layout. The remaining retail virtual-method interface
// is still represented by the repository's separate method views.
#include "derived_311920.h"
#include <stddef.h>

struct motion_factors_31ad80
{
	float values[4];
	motion_factors_31ad80()
	{
		values[0] = 1.0f;
		values[1] = 1.0f;
		values[2] = 1.0f;
		values[3] = 1.0f;
	}
};

class class_31ad80 : public class_3114d0
{
public:
	class_31ad80(void const *position, void const *orientation);
	OBJECT_NEW_3114B0
	__m128 saved_linear;
	__m128 saved_angular;
	motion_factors_31ad80 factors;
	__m128 field_f0;
	__m128 field_100;
};

class class_31a870 : public class_31ad80
{
public:
	class_31a870(void const *position, void const *orientation);
};

typedef char assert_motion_31ad80_size[(sizeof(class_31ad80) == 0x110) ? 1 : -1];
typedef char assert_motion_31ad80_saved[(offsetof(class_31ad80,saved_linear) == 0xc0) ? 1 : -1];
typedef char assert_motion_31ad80_factors[(offsetof(class_31ad80,factors) == 0xe0) ? 1 : -1];
typedef char assert_motion_31a870_size[(sizeof(class_31a870) == 0x110) ? 1 : -1];
#endif
