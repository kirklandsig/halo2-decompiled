// @flags /O2 /GL-
// Havok (third-party physics library): keeping the closest hit (its
// distance in the first vector's w).
#include "closest_hit_301a00.h"

// @retail 0x301a00
void class_301a00::add_hit(const s_301a00_hit *candidate)
{
	if (!has_hit || candidate->position.m128_f32[3] < hit.position.m128_f32[3])
	{
		has_hit = true;
		hit.position = candidate->position;
		hit.normal = candidate->normal;
		early_out = candidate->position.m128_f32[3];
	}
}
