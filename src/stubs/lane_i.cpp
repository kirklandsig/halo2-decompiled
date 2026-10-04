// stubs for the game functions that lane I's code (0x250000..0x25ffff) calls
// and that are not decompiled yet

#include "cseries.h"
#include "unknown_25fc30.h"
#include "slot_owner.h"
#include "unknown_1e1f20.h"
#include "unknown_1fa590.h"
#include "slot_handler.h"

// @stub 0x29e050
bool function_29e050(byte *unknown, long target_index, firing_position_definition *definition, s_reference reference, long *unknown6a0)
{
	return false;
}

// @stub 0xcfec0
void function_cfec0(long unit_index)
{
}

// @stub 0x256bd0
short __stdcall function_256bd0(long actor_index, s_slot *slot, bool active)
{
	return 0;
}


// @stub 0x25d020
void __stdcall function_25d020(long actor_index, long prop_ref_index, long a, long b, long c, long d)
{
}

// @stub 0x262890
bool function_262890(long actor_index, s_reference reference)
{
	return false;
}

// @stub 0xcbd80
long __stdcall function_cbd80(long object_index, long unknown)
{
	return 0;
}

// @stub 0x267a80
short function_267a80(real *distance, real_point3d const *point, real_vector3d const *direction, real_point3d const *position, long unknown)
{
	return 0;
}

// @stub 0x29d7b0
bool function_29d7b0(s_pathfinding_data *pathfinding, firing_position_definition *definition, real_vector3d const *direction,
	real_point3d *point, real_vector3d *normal, char *side)
{
	return false;
}


// @stub 0xdfdb0
void function_dfdb0(long object_index, long unknown, long *location_index, real_point3d *point, long *a, long *b)
{
}

// @stub 0xf1070
void function_f1070(long object_index, long unknown, long *location_index, real_point3d *point, long *a, long *b)
{
}

// @stub 0x210420
void function_210420(s_location_view *location, long a, long b, real_point3d const *point)
{
}

// @stub 0x26ace0
long function_26ace0(long object_index, long actor_index, short type)
{
	return 0;
}

// @stub 0x25c230
void __stdcall function_25c230(long actor_index, long prop_ref_index, short unknown)
{
}

// @stub 0x25c570
long __stdcall function_25c570(long prop_ref_index, short unknown)
{
	return 0;
}

/* the unit request callbacks of g_4677c8 (unknown_0e68c0.cpp) */

