// stubs for game functions not decompiled yet, called by damage.cpp
#include "unknown_11c920.h"

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
// @stub 0xb9c60
void function_b9c60(long object_index, bool flag) { }
// @stub 0xbef30
void __stdcall function_bef30(long object_index, long a, long b, long c, long d) { }
/* sets a region's permutation */
// @stub 0xa8360
void function_a8360(long object_index, long region_index, long permutation_index, bool a) { }
// @stub 0xe6460
void __stdcall function_e6460(long object_index) { }
// @stub 0xba7f0
void __stdcall function_ba7f0(long object_index, long a, long b, long c) { }
struct s_type_1e6529;
#include "unknown_0259d0.h"
/* the objects in a sphere */
// @stub 0xbb050
short __stdcall function_bb050(long a, unsigned long type_mask, void const *location, point3f const *position, float radius, long *objects, short maximum_count) { return 0; }
/* damage.cpp's own, not written yet (temporary) */
/* the closest point of an object to an origin, and the surface normal there */
// @stub 0xbaff0
void function_baff0(long object_index, point3f const *origin, point3f *arg_149545, union vector3f *normal) { }
// @stub 0x153d10
void __stdcall function_153d10(short team, long definition_index, void *a, void *b, long c, float d, float e, long f) { }
// @stub 0x184250
void __stdcall function_184250(s_type_1e6529 const *data) { }
/* an object's model states */
// @stub 0xba690
void function_ba690(long object_index, unsigned char **states, long *state_count, long *a, long *b) { }
struct s_damage_report;
// @stub 0x119280
void function_119280(long object_index, unsigned long flags) { }
/* called by function_d9640 (0xd9640) */
// @stub 0xb7880
void __stdcall function_b7880(long object_index, long node_index, point3f const *point, union vector3f const *impulse, union vector3f const *angular_impulse) { }
// @stub 0x119020
void function_119020(long creature_index, union vector3f const *impulse) { }
// @stub 0x1e9fa0
void __stdcall function_1e9fa0(void *engine_globals, long object_index, long player_index, unsigned short team, unsigned char kind) { }
// @stub 0x1e8fa0
void function_1e8fa0(long player_index, long object_index, unsigned char kind) { }
// @stub 0xa80f0
void function_a80f0(long object_index, s_damage_report const *report) { }
/* called by function_d7b80 (0xd7b80) */
// @stub 0x155b60
void function_155b60(long unit_index) { }
/* called by function_d5de0 (0xd5de0) */
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
