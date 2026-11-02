// @flags /O2 /Ob1 /GL- /GX
/* UNKNOWN_314270.CPP: the constructors of three constraints the game builds
   from a joint description (0x1d77d0): ball and socket, spring and prismatic.
   Each runs the common constructor (0x31bbd0), whose results arrays start at
   zero, then takes its own fields from the description. */

#include "constraint_helpers_314090.h"
#include <float.h>

// @retail 0x314270
lane_a_ball_socket::lane_a_ball_socket(c_component_joint_snapshot_4 const *description)
	: lane_b_rigid_constraint(description)
{
	pivot_a = description->pivot_a;
	pivot_b = description->pivot_b;
}

// @retail 0x314b00
lane_a_spring::lane_a_spring(c_component_joint_snapshot_3 const *description)
	: lane_b_rigid_constraint(description)
{
	pivot_a = description->pivot_a;
	pivot_b = description->pivot_b;
	length = description->length;
}

// @retail 0x314ef0
lane_a_prismatic::lane_a_prismatic(c_component_joint_snapshot_5 const *description)
	: lane_b_rigid_constraint(description)
{
	import_fields(description);
}

/* the motion tracker of an entity: no position or rotation seen yet (the
   largest float, the identity), its tolerances, and the squared radius of the
   entity's shape */
// @retail 0x314710
c_314710::c_314710(hkEntity *entity)
{
	unknown06 = 1;
	distance_a = 0.01f;
	angle_a = 0.005f;
	distance_b = 0.1f;
	angle_b = 0.2f;
	position_a = _mm_set_ps1(FLT_MAX);
	position_b = position_a;
	rotation_a = _mm_setzero_ps();
	rotation_a.m128_f32[3] = 1.0f;
	rotation_b = _mm_setzero_ps();
	rotation_b.m128_f32[3] = 1.0f;
	update_radius_squared(*(shape_bounds_314650 const **)((char *)entity + 0xc));
}
