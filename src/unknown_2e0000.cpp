// @flags /O2 /GL-
// Havok (third-party physics library): a size from a count of 24-byte
// entries after a 0x24-byte header.
#include "axis_sweep_2e1090.h"

class class_2e0000
{
public:
	char unknown0[0x44];
	long field_44;
	char unknown48[0x28];
	long field_70;

	long entry_total() const;
};

// @retail 0x2e0000
long class_2e0000::entry_total() const
{
	return (field_44 - field_70) * 24 + 0x24;
}

// @retail 0x2e0020
void class_2e1090::function_2e0020(element_2e1090 const *element, bounds_2e0020 *bounds)
{
	__m128 corner;
	float inverse_x = 1.0f / scale.m128_f32[0];
	corner.m128_f32[0] = axes[0].data[element->slots[4]].value * inverse_x;
	float inverse_y = 1.0f / scale.m128_f32[1];
	corner.m128_f32[1] = axes[1].data[element->slots[0]].value * inverse_y;
	float inverse_z = 1.0f / scale.m128_f32[2];
	corner.m128_f32[2] = axes[2].data[element->slots[1]].value * inverse_z;
	corner.m128_f32[3] = 0.0f;
	__m128 lower = _mm_sub_ps(corner, lower_offset);
	corner.m128_f32[0] = axes[0].data[element->slots[5]].value * inverse_x;
	corner.m128_f32[1] = axes[1].data[element->slots[2]].value * inverse_y;
	corner.m128_f32[2] = axes[2].data[element->slots[3]].value * inverse_z;
	corner.m128_f32[3] = 0.0f;
	__m128 upper = _mm_sub_ps(corner, lower_offset);
	bounds->lower = lower;
	bounds->upper = upper;
}
