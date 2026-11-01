// @flags /O2 /GL- /GX
// Ragdoll and hinge description defaults, field export and serialization.
#include "joint_descriptions_312920.h"
#include <new>
#include <stddef.h>

typedef char hinge_description_size[sizeof(c_component_joint_snapshot_0) == 0x60 ? 1 : -1];
typedef char limited_hinge_description_size[sizeof(c_component_joint_snapshot_1) == 0x90 ? 1 : -1];
typedef char ragdoll_description_size[sizeof(c_component_joint_snapshot_2) == 0x90 ? 1 : -1];
typedef char ragdoll_limit_offset[offsetof(c_component_joint_snapshot_2, cone_limit) == 0x70 ? 1 : -1];

// @retail 0x312920
c_component_joint_snapshot_2::c_component_joint_snapshot_2()
{
	pivot_a = _mm_setzero_ps();
	pivot_b = _mm_setzero_ps();
	twist_a = _mm_setzero_ps();
	plane_a = _mm_setzero_ps();
	twist_b = _mm_setzero_ps();
	plane_b = _mm_setzero_ps();
	cone_limit = 1.0f;
	plane_low = -0.025f;
	plane_high = 0.025f;
	twist_low = -0.5236f;
	twist_high = 0.5236f;
	friction = 0.0f;
}

// @retail 0x313320
c_component_joint_snapshot_1::c_component_joint_snapshot_1()
{
	pivot_a = _mm_setzero_ps();
	pivot_b = _mm_setzero_ps();
	axle_a = _mm_setzero_ps();
	axle_b = _mm_setzero_ps();
	perp_a = _mm_setzero_ps();
	perp_b = _mm_setzero_ps();
	reference_b = _mm_setzero_ps();
	angle_min = -3.1415927410125732f;
	angle_max = 3.1415927410125732f;
	friction = 0.0f;
}

// @retail 0x313b30
c_component_joint_snapshot_0::c_component_joint_snapshot_0()
{
	pivot_a = _mm_setzero_ps();
	pivot_b = _mm_setzero_ps();
	axle_b = _mm_setzero_ps();
	axle_a = _mm_setzero_ps();
	perpendicular_a = _mm_setzero_ps();
}

// @retail 0x3129e0
void __cdecl construct_description_3129e0(void *storage)
{
	new (storage) c_component_joint_snapshot_2;
}

// @retail 0x3133d0
void __cdecl construct_description_3133d0(void *storage)
{
	new (storage) c_component_joint_snapshot_1;
}

// @retail 0x313bc0
void __cdecl construct_description_313bc0(void *storage)
{
	new (storage) c_component_joint_snapshot_0;
}

// @retail 0x312980
lane_a_ragdoll *__cdecl construct_ragdoll_312980(lane_a_ragdoll_description const *description)
{
	return new lane_a_ragdoll(description);
}

// @retail 0x313370
lane_a_limited_hinge *__cdecl construct_limited_hinge_313370(lane_a_limited_hinge_description const *description)
{
	return new lane_a_limited_hinge(description);
}

// @retail 0x313b60
lane_a_hinge *__cdecl construct_hinge_313b60(lane_a_hinge_description const *description)
{
	return new lane_a_hinge(description);
}
