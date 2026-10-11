/* LANE_R.CPP: stubs for the functions lane R's code (0x170000 to 0x17ffff)
   calls that are not decompiled yet. Those marked "in region" are lane R's
   own, still to be written. */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_particle_system_datum;
struct s_particle_location_datum;
struct s_effect_datum;
struct s_effect_object_marker;
struct s_effect_color_query;
struct s_location;
struct s_effect_beam;
struct s_effect_object_placement;
struct s_effect_owner;
struct s_effect_source;



// @stub 0xc0350
void function_c0350(long tag_index, long object_index, long node_index, vector3f const *up, vector3f const *forward, point3f const *position, real scale) { }

/* in region */
/* in region */
// @stub 0x17e670
void function_17e670(s_effect_source *source, point3f const *point, long tag_index, vector3f const *vector, real radius, long unknown0, long unknown1, long unknown2) { }

struct s_decal_placement;
struct s_decal_preparation_17ef10;
struct s_decal_chain_state_17ee20;

// @stub 0x17ef10
bool function_17ef10(transform4x3f const *transform, long tag_index,
    s_decal_placement const *placement, vector3f const *direction, real radius,
    bool unknown0, long unknown1, long unknown2,
    s_decal_preparation_17ef10 *preparation, s_decal_chain_state_17ee20 *state)
{
    return false;
}

// @stub 0x211060
void function_211060(long unknown0, void *physics, s_location *location, long unknown3, point3f *position, long unknown5, long unknown6, long unknown7, real radius, real dt, vector3f *velocity) { }

// @stub 0x43890
void function_43890(void) { }

