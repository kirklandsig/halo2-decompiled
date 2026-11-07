// @flags /O2 /GL-
#include "contact_modifiers_315340.h"

long __cdecl havok_entity_property_2003_get(hkEntity const *);

// Append the velocity-bias callback only when contact building emitted rows
// beyond the per-contact linear rows, and the body has the velocity property.
// Reload the saved scalar for each component and finish the velocity stores
// before publishing the term pointer, matching the retail access order.
// @retail 0x315620
void contact_sizes_3155f0::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	s_ops_output_31f5e0 *cursors = (s_ops_output_31f5e0 *)output;
	unsigned char *start = (unsigned char *)cursors->term;
	((lane_b_contact_constraint_prefix *)this)->build(input, output);
	float const *velocity = (float const *)havok_entity_property_2003_get((hkEntity const *)body_a);
	if (velocity)
	{
		long contact_bytes = points.size * 0x30;
		if ((unsigned char *)cursors->term - start != contact_bytes)
		{
			velocity_record_315440 volatile *record = (velocity_record_315440 *)function_320290(
				(solver_callback_320290)add_velocity_bias_315440, 0x20, input, cursors);
			volatile float scale = scale_b;
			record->velocity[0] = scale * velocity[0];
			record->velocity[1] = scale * velocity[1];
			record->velocity[2] = scale * velocity[2];
			record->velocity[3] = scale * velocity[3];
			record->terms = (contact_term_315440 *)(start + contact_bytes);
		}
	}
}

// Save both bodies' inverse masses, build using the requested scales, and
// restore immediately as well as recording the corresponding solver callbacks.
// @retail 0x315740
void contact_sizes_315720::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	lane_a_accumulator_prefix *a = *(lane_a_accumulator_prefix * const *)input;
	lane_a_accumulator_prefix *b = ((lane_a_accumulator_prefix * const *)input)[1];
	s_ops_output_31f5e0 *cursors = (s_ops_output_31f5e0 *)output;
	function_31f5e0((s_ops_pair_31f5e0 const *)input, cursors);
	float first_scale = modifier.scale_a, second_scale = scale_b;
	lane_a_mass_restore *record = (lane_a_mass_restore *)function_320290(
		(solver_callback_320290)lane_a_scale_masses, 0x30, input, cursors);
	record->scale_a = first_scale;
	record->scale_b = second_scale;
	record->a = a;
	record->b = b;
	__declspec(align(16)) float saved[8];
	for (int i = 0; i < 4; ++i) saved[i] = a->inverse_mass[i];
	for (int i = 0; i < 4; ++i) saved[i + 4] = b->inverse_mass[i];
	for (int i = 0; i < 4; ++i) a->inverse_mass[i] = first_scale * a->inverse_mass[i];
	for (int i = 0; i < 4; ++i) b->inverse_mass[i] = second_scale * b->inverse_mass[i];
	((lane_b_contact_constraint_prefix *)this)->build(input, output);
	record = (lane_a_mass_restore *)function_320290((solver_callback_320290)lane_a_restore_masses, 0x30, input, cursors);
	for (int i = 0; i < 8; ++i) record->saved[i] = saved[i];
	for (int i = 0; i < 4; ++i) a->inverse_mass[i] = saved[i];
	for (int i = 0; i < 4; ++i) b->inverse_mass[i] = saved[i + 4];
	record->b = b;
	record->a = a;
}
