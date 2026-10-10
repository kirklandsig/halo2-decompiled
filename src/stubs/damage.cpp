// stubs for game functions not decompiled yet, called by damage.cpp
#include "unknown_11c920.h"

struct s_object_child_iterator;
struct s_damage_owner;
struct s_damage_info;
struct s_damage_region_accumulator;
struct s_damage_object;

// @stub 0xc1720
long __stdcall function_c1720(long object_index, long remove, long add) { return 0; }
/* sets a region's permutation */
// @stub 0xe6460
void __stdcall function_e6460(long object_index) { }
struct s_type_1e6529;
#include "unknown_0259d0.h"
/* the objects in a sphere */
/* damage.cpp's own, not written yet (temporary) */
/* the closest point of an object to an origin, and the surface normal there */
// @stub 0x183910
bool function_183910(long component_index, point3f const *origin, point3f *point, vector3f *normal) { return false; }
struct s_damage_report;
// @stub 0x119280
void function_119280(long object_index, unsigned long flags) { }
// @stub 0x1e9fa0
void __stdcall function_1e9fa0(void *engine_globals, long object_index, long player_index, unsigned short team, unsigned char kind) { }
/* called by function_d7b80 (0xd7b80) */
/* called by function_d5de0 (0xd5de0) */
/* the physics model constraint iterator and the model node search (for
   0xdb810, 0xdbb40) */
struct s_physics_constraint_iterator;
struct s_physics_constraint_block;
