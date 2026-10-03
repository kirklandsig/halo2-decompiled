// stubs for game functions not decompiled yet, called by damage.cpp
#include "cseries.h"

/* the difficulty multiplier of a team (kind 1 body, 2 shield); retail passes
   both arguments in registers and returns in xmm0 */
// @stub 0x1e9720
real function_1e9720(long kind, short team) { return 1.0f; }
struct s_object_child_iterator;
struct s_damage_owner;
struct s_damage_info;
struct s_damage_region_accumulator;
struct s_damage_object;

// @stub 0xb8b70
void function_b8b70(long object_index) { }
struct s_unit_child_iterator;
/* the units among an object's children (another file's) */
// @stub 0xd0590
void function_d0590(s_unit_child_iterator *iterator, long object_index) { }
// @stub 0xd05c0
s_damage_object *function_d05c0(s_unit_child_iterator *iterator) { return 0; }
// @stub 0xb9c60
void function_b9c60(long object_index, bool flag) { }
// @stub 0xb9d20
bool function_b9d20(long object_index) { return false; }
// @stub 0xbef30
void __stdcall function_bef30(long object_index, long a, long b, long c, long d) { }
// @stub 0x10d4e0
void function_10d4e0(long object_index) { }
/* sets a region's permutation */
// @stub 0xa8360
void function_a8360(long object_index, long region_index, long permutation_index, bool a) { }
// @stub 0xe6460
void __stdcall function_e6460(long object_index) { }
// @stub 0xba7f0
void __stdcall function_ba7f0(long object_index, long a, long b, long c) { }
struct damage_data;
#include "real_math.h"
/* the objects in a sphere */
// @stub 0xbb050
short __stdcall function_bb050(long a, unsigned long type_mask, void const *location, real_point3d const *position, float radius, long *objects, short maximum_count) { return 0; }
/* damage.cpp's own, not written yet (temporary) */
/* the closest point of an object to an origin, and the surface normal there */
// @stub 0xbaff0
void function_baff0(long object_index, real_point3d const *origin, real_point3d *closest_point, union real_vector3d *normal) { }
// @stub 0x153d10
void __stdcall function_153d10(short team, long definition_index, void *a, void *b, long c, float d, float e, long f) { }
// @stub 0x184250
void __stdcall function_184250(damage_data const *data) { }
/* an object's model states */
// @stub 0xba690
void function_ba690(long object_index, unsigned char **states, long *state_count, long *a, long *b) { }
struct s_damage_report;
// @stub 0xc9e70
void function_c9e70(long unit_index, unsigned long flags, damage_data const *data, s_damage_report const *report) { }
// @stub 0x119280
void function_119280(long object_index, unsigned long flags) { }
/* called by object_damage_aftermath (0xd9640) */
// @stub 0x101c80
void __stdcall function_101c80(long object_index) { }
// @stub 0xb7880
void __stdcall function_b7880(long object_index, long node_index, real_point3d const *point, union real_vector3d const *impulse, bool flag) { }
// @stub 0x10cf80
void function_10cf80(union real_vector3d const *impulse, long item_index, bool flag) { }
// @stub 0xde620
void __stdcall function_de620(long biped_index, union real_vector3d const *impulse) { }
// @stub 0x119020
void function_119020(long creature_index, union real_vector3d const *impulse) { }
// @stub 0x1e9fa0
void __stdcall function_1e9fa0(void *engine_globals, long object_index, long player_index, unsigned short team, unsigned char kind) { }
// @stub 0x1e8fa0
void function_1e8fa0(long player_index, long object_index, unsigned char kind) { }
// @stub 0xca0b0
void __stdcall function_ca0b0(long unit_index, s_damage_report const *report) { }
// @stub 0xa80f0
void function_a80f0(long object_index, s_damage_report const *report) { }
/* called by object_cause_damage (0xd7b80) */
// @stub 0xcc010
bool function_cc010(long object_index, union real_vector3d const *direction) { return false; }
/* called by 0xdc0a0 */
// @stub 0xcc410
bool function_cc410(long unit_index) { return false; }
// @stub 0x155b60
void function_155b60(long unit_index) { }
/* called by object_damage_update (0xd5de0) */
// @stub 0xa7a30
void function_a7a30(long object_index, unsigned long mask) { }
/* the physics model constraint iterator and the model node search (for
   0xdb810, 0xdbb40) */
struct s_physics_constraint_iterator;
struct s_physics_constraint_block;
// @stub 0x1eb110
void function_1eb110(s_physics_constraint_iterator *iterator) { }
// @stub 0x1eb160
void function_1eb160(s_physics_constraint_iterator *iterator) { }
