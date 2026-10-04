// stubs for game functions not decompiled yet, called by device_machines.cpp
#include "cseries.h"
#include "real_math.h"

struct s_havok_component;
struct s_machine_node_matrices;
struct s_animation_frame_event;


/* moves a Havok component's bodies to a device position */
// @stub 0x1d24a0
void __stdcall function_1d24a0(s_havok_component *component, float position) { }
/* an object's node matrices, for its Havok bodies */
// @stub 0x20a9a0
bool function_20a9a0(long object_index, s_machine_node_matrices *matrices) { return false; }
/* keyframes a Havok body to a matrix */
// @stub 0x1d0ee0
void function_1d0ee0(long rigid_body_index, s_havok_component *component, real_matrix4x3 const *matrix) { }
/* a device's animation event callback */
// @stub 0xbf600
void __stdcall function_bf600(long user, float frame, s_animation_frame_event const *event) { }
/* an object's forward and up vectors */
// @stub 0xb9fc0
void function_b9fc0(long object_index, union real_vector3d *forward, union real_vector3d *up) { }
// @stub 0xbba20
void function_bba20(long object_index) { }
/* the machine's and the crate's callback at +0x4c: asks the object's Havok
   component (another file's) */
// @stub 0x11bd60
bool __stdcall function_11bd60(long object_index, long a, long b, long c) { return false; }
