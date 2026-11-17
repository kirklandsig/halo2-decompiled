// @flags /O2 /Ob1 /GL- /GX
#include "constraint_helpers_314090.h"

// Result-array elements initialize themselves before the description fields.
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
// These concrete joints add no teardown beyond their common constraint base.
// @retail 0x314b40
lane_b_rigid_constraint::~lane_b_rigid_constraint() {}
// @retail 0x3142c0 deleting lane_a_ball_socket
// @retail 0x314f90 adjustor lane_a_prismatic 8
