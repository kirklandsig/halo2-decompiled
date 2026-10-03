// stubs for game functions not decompiled yet, called by projectiles.cpp
#include "cseries.h"
#include "real_math.h"

// @stub 0xb9a90
void function_b9a90(long object_index) { }
/* the delta a projectile's guidance adds to its velocity */
// @stub 0x109a00
bool function_109a00(long projectile_index, union real_vector3d *delta, bool unknown, union real_vector3d *velocity) { return false; }
// @stub 0xb75a0
struct s_location;
void function_b75a0(long object_index, real_point3d const *point, union real_vector3d const *forward, union real_vector3d const *up, s_location const *location, bool unknown) { }
// @stub 0xb77d0
void __stdcall function_b77d0(long object_index, union real_vector3d const *velocity) { }
/* attaches an object to a parent's node */
// @stub 0xb93b0
void __stdcall function_b93b0(long parent_index, long object_index, long node_index) { }
// @stub 0x1e2930
void __stdcall function_1e2930(long object_index, long actor_index) { }
// @stub 0xa83e0
void function_a83e0(long object_index, long parent_index, real_point3d const *point, long node_index, union real_vector3d const *forward) { }
/* an object's forward and up vectors */
// @stub 0xb9fc0
void function_b9fc0(long object_index, union real_vector3d *forward, union real_vector3d *up) { }
// @stub 0xbc1d0
void __stdcall function_bc1d0(long object_index, real_point3d *point) { }
struct s_damage_owner;
/* an object's damage owner */
// @stub 0xbc190
void function_bc190(long object_index, s_damage_owner *owner) { }
// @stub 0xa84e0
void __stdcall function_a84e0(long projectile_index, short *material_index, union real_vector3d const *vector, unsigned long flags) { }
// @stub 0x1ca690
void __stdcall function_1ca690(long object_index, void const *data, long a, long b, long c) { }
// @stub 0x1ca9f0
void __stdcall function_1ca9f0(long object_index, long unknown) { }
// @stub 0xb7740
void function_b7740(long object_index, union real_vector3d const *linear_velocity, union real_vector3d const *angular_velocity, bool unknown) { }
// @stub 0xbba20
void function_bba20(long object_index) { }
struct s_collision_result_1697c0;
struct damage_data;
// @stub 0x184060
void function_184060(long unknown3c, unsigned char unknown59, damage_data *data, long unknown50) { }
// @stub 0xa85c0
void function_a85c0(long projectile_index, s_collision_result_1697c0 const *collision, union real_vector3d const *direction, bool unknown, float scale_a, float scale_b) { }
