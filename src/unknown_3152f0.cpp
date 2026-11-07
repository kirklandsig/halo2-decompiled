// @flags /O2 /GL-
// Havok solver callbacks. Accumulator prefix through the four inverse-mass
// terms at +0x30; no claim about the complete accumulator extent.
#include <xmmintrin.h>
struct lane_a_accumulator_prefix
{
	float state[12];
	float inverse_mass[4];
};
struct lane_a_mass_restore
{
	float saved[8];
	float scale_a;
	float scale_b;
	lane_a_accumulator_prefix *a;
	lane_a_accumulator_prefix *b;
};

// Preserve the scalar factor loads before the selected x87 products.
// @retail 0x315280
void __cdecl lane_a_scale_masses(void *, lane_a_mass_restore *record)
{
	{
		float const volatile &factor = record->scale_a;
		record->a->inverse_mass[0] = factor * record->a->inverse_mass[0];
	}
	{
		float const volatile &factor = record->scale_a;
		record->a->inverse_mass[1] = factor * record->a->inverse_mass[1];
	}
	record->a->inverse_mass[2] *= record->scale_a;
	{
		float const volatile &factor = record->scale_a;
		record->a->inverse_mass[3] = factor * record->a->inverse_mass[3];
	}
	record->b->inverse_mass[0] *= record->scale_b;
	{
		float const volatile &factor = record->scale_b;
		record->b->inverse_mass[1] = factor * record->b->inverse_mass[1];
	}
	record->b->inverse_mass[2] *= record->scale_b;
	record->b->inverse_mass[3] *= record->scale_b;
}

// @retail 0x3152f0
void __cdecl lane_a_restore_masses(void *, lane_a_mass_restore *record)
{
	record->a->inverse_mass[0] = record->saved[0];
	record->a->inverse_mass[1] = record->saved[1];
	record->a->inverse_mass[2] = record->saved[2];
	record->a->inverse_mass[3] = record->saved[3];
	record->b->inverse_mass[0] = record->saved[4];
	record->b->inverse_mass[1] = record->saved[5];
	record->b->inverse_mass[2] = record->saved[6];
	record->b->inverse_mass[3] = record->saved[7];
}

