// @flags /O2 /Ob1 /GL-
#include "joint_descriptions_312920.h"
#include <stddef.h>

typedef char ragdoll_runtime_size[sizeof(lane_a_ragdoll) == 0xe0 ? 1 : -1];
typedef char ragdoll_result_offset[offsetof(lane_a_ragdoll, results) == 0x98 ? 1 : -1];

// @retail 0x312d50
lane_a_ragdoll::lane_a_ragdoll(lane_a_ragdoll_description const *description)
	: lane_b_rigid_constraint(description)
{
	import_fields(description);
}
