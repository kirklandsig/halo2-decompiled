// stubs for lane C (0x1c0000..0x1cffff): callees outside the region that are
// not decompiled yet, and the library (Havok) functions the region calls
#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "slot_owner.h"
#include "unknown_1c62f0.h"
#include "unknown_0259a0.h"
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
/* callees of the slot handler callbacks (unknown_0259a0.h) */


// @stub 0x1697c0
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result) { return false; }

// @stub 0x265d30
real __stdcall function_265d30(long actor_index, long prop_index) { return 0.0f; }

// @stub 0x26d100
long function_26d100(vector3f const *up, s_collision_result_1697c0 *collision, long *unknown, point3f const *point) { return 0; }

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





// @stub 0x260160
bool function_260160(long actor_index, s_261d20_entry *entry, s_prop_search *search) { return false; }

/* the animation channels (unknown_1c62f0.cpp: real code there, under #if 0) */

// @stub 0x1c66a0
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user) { }

// @stub 0x290250
void __stdcall function_290250(long tag_index, long ticks, long object_index, long node_index, real lower, real upper,
	transform4x3f const *matrix)
{
}
