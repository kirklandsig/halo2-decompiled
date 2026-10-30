#ifndef CONTACT_MODIFIERS_315340_H
#define CONTACT_MODIFIERS_315340_H
#include "constraint_builders_314220.h"

extern void *g_480990;
extern void *g_480770;
extern void *g_480880;

// These modifiers replace the embedded contact object's table after charging
// its owner for the extra solver operations. The object starts at owner +0x10.
struct contact_modifier_view_315340
{
	void *table;
	unsigned char unknown04[8];
	hkEntity *body_a, *body_b;
	lane_b_constraint_owner *owner;
	unsigned char unknown18[0x28];
	union { float scale_a; void const *velocity; } value;
	float scale_b;
};
struct contact_term_315440 { float linear[4], angular_a[4], angular_b[4]; };
struct velocity_record_315440 { float velocity[4]; contact_term_315440 *terms; };
void __cdecl add_velocity_bias_315440(void *, velocity_record_315440 *);

struct lane_a_accumulator_prefix { float state[12]; float inverse_mass[4]; };
struct lane_a_mass_restore
{
	float saved[8];
	float scale_a, scale_b;
	lane_a_accumulator_prefix *a, *b;
};
void __cdecl lane_a_scale_masses(void *, lane_a_mass_restore *);
void __cdecl lane_a_restore_masses(void *, lane_a_mass_restore *);
#endif
