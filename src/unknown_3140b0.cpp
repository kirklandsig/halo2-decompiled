// @flags /O2 /GL- /GX
#include "constraint_helpers_314090.h"

// Opaque registration records have a 0x28-byte stride in this type group.
struct joint_registration_3140b0 { unsigned char opaque[0x28]; };
joint_registration_3140b0 g_4806d0, g_480720, g_480748;

// @retail 0x3140b0
void *lane_a_ball_socket::v3() { return &g_4806d0; }
// @retail 0x314850
void *lane_a_spring::v3() { return &g_480720; }
// @retail 0x314ba0
void *lane_a_prismatic::v3() { return &g_480748; }

// @retail 0x3140e0
lane_a_ball_socket *__cdecl allocate_ball_socket_3140e0(c_component_joint_snapshot_4 const *description)
{
	return new lane_a_ball_socket(description);
}
// @retail 0x314880
lane_a_spring *__cdecl allocate_spring_314880(c_component_joint_snapshot_3 const *description)
{
	return new lane_a_spring(description);
}
// @retail 0x314c20
lane_a_prismatic *__cdecl allocate_prismatic_314c20(c_component_joint_snapshot_5 const *description)
{
	return new lane_a_prismatic(description);
}
