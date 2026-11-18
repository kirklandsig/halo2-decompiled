// @flags /O2 /GL- /Op
#include "contact_modifiers_315340.h"

long __cdecl havok_entity_property_2003_get(hkEntity const *);

// Append the velocity-bias callback only when contact building emitted rows
// beyond the per-contact linear rows, and the body has the velocity property.
// Reload the saved scalar for each component and finish the velocity stores
// before publishing the term pointer, matching the retail access order.
// Matched (was 57): the file is compiled with /Op (0x315740 is in its own file,
// without it), the body is named before the property lookup, and the four
// scaled components are stored by a loop.
// @retail 0x315620
void contact_sizes_3155f0::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	s_ops_output_31f5e0 *cursors = (s_ops_output_31f5e0 *)output;
	unsigned char *start = (unsigned char *)cursors->term;
	contact_default_31c000::build(input, output);
	hkEntity const *body = (hkEntity const *)body_a;
	float const *velocity = (float const *)havok_entity_property_2003_get(body);
	if (velocity)
	{
		long contact_bytes = points.size * 0x30;
		if ((unsigned char *)cursors->term - start != contact_bytes)
		{
			velocity_record_315440 *record = (velocity_record_315440 *)function_320290(
				(solver_callback_320290)add_velocity_bias_315440, 0x20, input, cursors);
			float scale = scale_b;
			for (int i = 0; i < 4; ++i) record->velocity[i] = scale * velocity[i];
			record->terms = (contact_term_315440 *)(start + contact_bytes);
		}
	}
}
