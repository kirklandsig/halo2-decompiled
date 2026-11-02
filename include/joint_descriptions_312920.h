#ifndef JOINT_DESCRIPTIONS_312920_H
#define JOINT_DESCRIPTIONS_312920_H
#include "lane_a_constraints.h"

// The game's joint snapshots have the same description layouts as the
// corresponding constraints; their constructors establish the defaults.
struct c_component_joint_snapshot_0 : lane_a_hinge_description
{
	c_component_joint_snapshot_0();
};
struct c_component_joint_snapshot_1 : lane_a_limited_hinge_description
{
	c_component_joint_snapshot_1();
};
struct c_component_joint_snapshot_2 : lane_a_ragdoll_description
{
	c_component_joint_snapshot_2();
};
#endif
