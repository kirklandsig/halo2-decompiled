// @flags /O2 /Ob1 /GL- /GX
#include "joint_descriptions_312920.h"
#include <stddef.h>

typedef char limited_hinge_runtime_size[sizeof(lane_a_limited_hinge) == 0xe0 ? 1 : -1];
typedef char hinge_runtime_size[sizeof(lane_a_hinge) == 0xa0 ? 1 : -1];
typedef char limited_hinge_result_offset[offsetof(lane_a_limited_hinge, results) == 0x90 ? 1 : -1];
typedef char hinge_result_offset[offsetof(lane_a_hinge, results) == 0x70 ? 1 : -1];


// @retail 0x313550
lane_a_limited_hinge::lane_a_limited_hinge(lane_a_limited_hinge_description const *description)
	: lane_b_rigid_constraint(description)
{
	import_fields(description);
}

// Importing fields is also valid on an existing, potentially derived object;
// its validation call therefore dispatches through the virtual table.
inline void lane_a_hinge::import_fields(lane_a_hinge_description const *description)
{
	pivot_a = description->pivot_a;
	axle_a = description->axle_a;
	perpendicular_a = description->perpendicular_a;
	pivot_b = description->pivot_b;
	axle_b = description->axle_b;
	valid();
}

// @retail 0x313de0
lane_a_hinge::lane_a_hinge(lane_a_hinge_description const *description)
	: lane_b_rigid_constraint(description)
{
	import_fields(description);
}
