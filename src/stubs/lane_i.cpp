// stubs for the game functions that lane I's code (0x250000..0x25ffff) calls
// and that are not decompiled yet

#include "unknown_11c920.h"
#include "unknown_25fc30.h"
#include "slot_owner.h"
#include "unknown_1e1f20.h"
#include "unknown_1fa590.h"
#include "slot_handler.h"

// @stub 0x29e050
bool function_29e050(byte *unknown, long target_index, s_type_d4fbfa *definition, s_reference reference, long *unknown6a0)
{
	return false;
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


// @stub 0x267a80
short function_267a80(real *distance, point3f const *point, vector3f const *direction, point3f const *position, long unknown)
{
	return 0;
}

// @stub 0x29d7b0
bool function_29d7b0(s_pathfinding_data *pathfinding, s_type_d4fbfa *definition, vector3f const *direction,
	point3f *point, vector3f *normal, char *side)
{
	return false;
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


/* command script procs of 0x258b60 (unknown_257d00.cpp) */
struct s_cs_state;

// @stub 0x258cf0
short __stdcall function_258cf0(long actor_index, long object_index, s_cs_state *state, long cs_index)
{
	return 0;
}

// @stub 0x259430
short __stdcall function_259430(long actor_index, long object_index, s_cs_state *state, long cs_index)
{
	return 0;
}
