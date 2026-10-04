// stubs for lane C (0x1c0000..0x1cffff): callees outside the region that are
// not decompiled yet, and the library (Havok) functions the region calls
#include "cseries.h"
#include "unknown_1cec30.h"
#include "slot_owner.h"
#include "unknown_1c62f0.h"
#include "lane_c_callees.h"
#include "unknown_1765e0.h"

// @stub 0x3123a0
hkPropertyValue hkEntity::removeProperty(dword key) { return hkPropertyValue(0); }

// @stub 0x312530
void hkEntity::addProperty(dword key, hkPropertyValue value) { }

/* slot handler callbacks of the region not decompiled yet (their handler
   structs hold their addresses) */

/* game functions outside the region called by the ai lifecycle callbacks */

// @stub 0x1dfae0
void function_1dfae0(void) { }

// @stub 0x28d930
void function_28d930(void) { }

// @stub 0x200930
void function_200930(void) { }

// @stub 0x20b930
void function_20b930(void) { }

// @stub 0x292130
void function_292130(void) { }

// @stub 0x1a6d80
void function_1a6d80(void) { }

// @stub 0x28d9d0
void function_28d9d0(void) { }

// @stub 0x292e00
void function_292e00(void) { }

// @stub 0x292f60
void function_292f60(void) { }
/* the animation graph lookups (0x1d9000..0x1de000) */


/* the physics callees of the havok components */
struct s_havok_component;

// @stub 0x1d1260
void function_1d1260(s_havok_component *component) { }

// @stub 0x1d01c0
void __stdcall function_1d01c0(s_havok_component *component) { }
// @stub 0x3126f0
void hkRigidBody::setTransform(hkTransform const &transform) { }
/* callees of the slot handler callbacks (lane_c_callees.h) */


// @stub 0x1697c0
bool __stdcall function_1697c0(long flags, real_point3d const *point, real_vector3d const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result) { return false; }

// @stub 0x1f90f0
void function_1f90f0(long actor_index, s_path_source *source) { }

// @stub 0x2715a0
bool function_2715a0(byte *buffer) { return false; }

// @stub 0x270750
void function_270750(byte *buffer, long unknown, s_actor_point_target const *target, real *distance, long a, long b) { }

// @stub 0x265d30
real __stdcall function_265d30(long actor_index, long prop_index) { return 0.0f; }

// @stub 0x26c590
bool function_26c590(long node_index, real_point3d const *origin, s_path_trace_result *result,
	s_pathfinding_data *pathfinding, real_point3d const *position, long a, real_vector3d const *direction,
	real distance, long b) { return false; }

// @stub 0x26d100
long function_26d100(real_vector3d const *up, s_collision_result_1697c0 *collision, long *unknown, real_point3d const *point) { return 0; }

/* callees of the physics code (unknown_1c25a0.cpp, unknown_1cec30.cpp) */

// @stub 0x30bd50
void hkEntityApi::removeEntityListener(hkEntityListener *listener) { }

// @stub 0x30f800
hkBool hkWorld::removeEntity(hkEntity *entity) { return hkBool(); }

// @stub 0x278f00
void function_278f00(void) { }

// @stub 0x1d1540
void function_1d1540(s_havok_component *component) { }

// @stub 0x1d56a0
void function_1d56a0(s_havok_component *component) { }

// @stub 0x1d56f0
void function_1d56f0(s_havok_component *component) { }

// @stub 0x1d5940
bool __stdcall function_1d5940(s_havok_component *component, long a, long b, long c) { return false; }

// @stub 0x1d6b80
void function_1d6b80(s_havok_component *component) { }

// @stub 0x1d6ca0
void function_1d6ca0(s_havok_component *component) { }

/* in the region, not decompiled yet */

// @stub 0x1c4b00
void function_1c4b00(long object_index, void *a, void *b, long c) { }

// @stub 0x30f2d0
void hkWorld::addEntity(hkEntity *entity) { }

// @stub 0x30cc60
void hkWorld::removeSimulationIsland(hkSimulationIsland *island) { }

// @stub 0x30bc90
void hkEntityApi::activate(void) { }

// @stub 0xa7670
bool function_a7670(long object_index) { return false; }


/* the rigid body accessors of the havok components (unknown_1cec30.cpp) */

// @stub 0x30bc40
hkBool hkRigidBody::isActive(void) const { return hkBool(); }

// @stub 0x30bc60
void hkRigidBody::activate(void) { }

// @stub 0x2da5d0
void hkTransform::setMulEq(hkTransform const &b) { }

/* callees of ai.cpp's 0x1caa40 */

// @stub 0x2da3f0
void hkTransform::setInverse(hkTransform const &t) { }


struct real_quaternion_transform;




// @stub 0x2624d0
bool function_2624d0(s_261d20_entry *entry, s_reference reference) { return false; }

// @stub 0x260160
bool function_260160(long actor_index, s_261d20_entry *entry, s_prop_search *search) { return false; }

/* the animation channels (unknown_1c62f0.cpp: real code there, under #if 0) */

// @stub 0x1c66a0
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user) { }

/* the samplers the dispatchers of unknown_279d80.cpp tail-call; 0x27b020 and
   0x27cb90 are entered by falling through from 0x27ace0 and 0x27c750 */
// @stub 0x27a6e0
void function_27a6e0(void) { }

// @stub 0x27aac0
void function_27aac0(void) { }

// @stub 0x27ace0
void function_27ace0(void) { }

// @stub 0x27b020
void function_27b020(void) { }

// @stub 0x27b1d0
void function_27b1d0(void) { }

// @stub 0x27b660
void function_27b660(void) { }

// @stub 0x27b920
void function_27b920(void) { }

// @stub 0x27bd60
void function_27bd60(void) { }

// @stub 0x27bfe0
void function_27bfe0(void) { }

// @stub 0x27c490
void function_27c490(void) { }

// @stub 0x27c750
void function_27c750(void) { }

// @stub 0x27cb90
void function_27cb90(void) { }

// @stub 0x27ce20
void function_27ce20(void) { }

// @stub 0x27d420
void function_27d420(void) { }

// @stub 0x27d770
void function_27d770(void) { }

// @stub 0x27dd10
void function_27dd10(void) { }

// @stub 0x27dff0
void function_27dff0(void) { }

// @stub 0x27e6b0
void function_27e6b0(void) { }

// @stub 0x27eae0
void function_27eae0(void) { }

// @stub 0x27f160
void function_27f160(void) { }

// @stub 0x27f530
void function_27f530(void) { }

// @stub 0x27fc10
void function_27fc10(void) { }

// @stub 0x280050
void function_280050(void) { }

// @stub 0x2806e0
void function_2806e0(void) { }

// @stub 0x280ac0
void function_280ac0(void) { }

// @stub 0x281090
void function_281090(void) { }

// @stub 0x281370
void function_281370(void) { }

// @stub 0x2818e0
void function_2818e0(void) { }

// @stub 0x281b60
void function_281b60(void) { }

// @stub 0x2821f0
void function_2821f0(void) { }

// @stub 0x2825b0
void function_2825b0(void) { }

// @stub 0x282be0
void function_282be0(void) { }

// @stub 0x282f60
void function_282f60(void) { }

// @stub 0x283600
void function_283600(void) { }

// @stub 0x2839d0
void function_2839d0(void) { }

// @stub 0x284010
void function_284010(void) { }

// @stub 0x2843a0
void function_2843a0(void) { }

// @stub 0x284a20
void function_284a20(void) { }

// @stub 0x284de0
void function_284de0(void) { }

// @stub 0x285400
void function_285400(void) { }

// @stub 0x285750
void function_285750(void) { }

// @stub 0x285e90
void function_285e90(void) { }

// @stub 0x286320
void function_286320(void) { }

// @stub 0x286a00
void function_286a00(void) { }

// @stub 0x286e40
void function_286e40(void) { }

// @stub 0x287590
void function_287590(void) { }

// @stub 0x287a30
void function_287a30(void) { }

// @stub 0x288120
void function_288120(void) { }

// @stub 0x288570
void function_288570(void) { }

// @stub 0x288bd0
void function_288bd0(void) { }

// @stub 0x288f80
void function_288f80(void) { }

// @stub 0x289580
void function_289580(void) { }

// @stub 0x2898b0
void function_2898b0(void) { }

// @stub 0x289fa0
void function_289fa0(void) { }

// @stub 0x28a3e0
void function_28a3e0(void) { }

// @stub 0x28aaa0
void function_28aaa0(void) { }

// @stub 0x28ae70
void function_28ae70(void) { }

// @stub 0x28b570
void function_28b570(void) { }

// @stub 0x28b9c0
void function_28b9c0(void) { }

// @stub 0x28c090
void function_28c090(void) { }
