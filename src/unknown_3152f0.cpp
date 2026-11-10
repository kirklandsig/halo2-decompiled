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
// score 6 (was 6): Three scalar load/product operand choices remain; frame and length agree.
// Earlier compound/product forms 6; both loops 14; retained pointers 44; both/group scales 38/34;
// scalar scale/input/result locals 14; native scalar accessors 6; coefficient pointer/reference forms 6;
// native vector/component views 6; separate first/second component loops 10/6.
// Scalar field/receiver names in eight combinations 6 each; shared value product both orders 6/6;
// shared output reference 51/49, coefficient reference 52/50, output pointer 51/49. All restored.
// Three x87 load/product operand pairs remain; frame and length agree.
// Earlier uniform compound/scale-first/mass-first 6; both index loops 14; saved
// accumulator pointers 44; scales together/by group 38/34; pointer ranges 71;
// named scalar scale/input/product 14. Field scale pointers/references at entry
// or by group 6 each; native vector/component fields 6/6; first/second group loop 10/6.
// three x87 products still load their operands in reverse order.
// Uniform compound, scale-first and mass-first forms 6; two index loops 14;
// retained pointers 44; retained scales 38; group scales 34; pointer ranges 71;
// per-product scale, input and result locals 14.
// Scalar member/index accessors, their compound and forced forms 6 each;
// read-only record and record/member form 6 each.
// Native coefficient/pointer arrays separately/together/compound 6/6/6/6; plain nested
// indexed/do/outer-only/unsigned-group traversals 64/86/6/64. Original fields and bodies restored.
// Shared four-component loop, value/reference scale, both argument orders and
// pointer/array inputs 56/48/56/48/56/48/56/48; original body restored.
// @retail 0x315280
void __cdecl lane_a_scale_masses(void *, lane_a_mass_restore *record)
{
	{
		float const &factor = record->scale_a;
		record->a->inverse_mass[0] = factor * record->a->inverse_mass[0];
	}
	{
		float const &factor = record->scale_a;
		record->a->inverse_mass[1] = factor * record->a->inverse_mass[1];
	}
	record->a->inverse_mass[2] *= record->scale_a;
	{
		float const &factor = record->scale_a;
		record->a->inverse_mass[3] = factor * record->a->inverse_mass[3];
	}
	record->b->inverse_mass[0] *= record->scale_b;
	{
		float const &factor = record->scale_b;
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

