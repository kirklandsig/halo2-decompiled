/* UNKNOWN_0259A0.H: game functions outside 0x1c0000..0x1cffff that lane C's
   sources call and nobody has decompiled yet. src/stubs/lane_c.cpp defines
   them (the ones lane B calls too are in slot_handler.h); whoever decompiles one moves its prototype to the callee's own header
   and deletes the stub. Functions retail calls with stack arguments only
   (ret N, nothing in registers) are declared __stdcall so the call sites
   match; the rest take LTCG register conventions retail chose from their
   bodies, which a stub can't reproduce. */

#ifndef UNKNOWN_0259A0_H
#define UNKNOWN_0259A0_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include "slot_handler.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "unknown_1fa590.h"
#include "unknown_26c380.h"
#include "path.h"
#include "unknown_1f9240.h"

/* the actor's tag entry function_1e4f90 returns (0x40 bytes) */
struct s_actor_tag_entry_1e4f90
{
	long unknown00;
	real unknown04;
	real unknown08;
	real unknown0c;
	real unknown10;
	long unknown14;
	real unknown18;
	byte unknown1c[0x40 - 0x1c];
};

/* a collision result (0x4c bytes) */
struct s_collision_result_1697c0
{
	byte unknown00[8];
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x4c - 0x26];
};

/* the scenario's firing positions: g_4e0350 holds a block (+0x1d8) whose
   first element holds the zones; a position reference is the zone index in
   the high word and the position index in the low word */
struct s_firing_position
{
	byte unknown00[0x20];
	s_actor_point_target target;
	long unknown30;
	real facing;
	byte unknown38[4];
};

struct s_firing_zone
{
	byte unknown00[0x20];
	long position_count;
	s_firing_position *positions;
	short bsp_index;
	byte unknown2a[2];
	byte flags;
	byte unknown2d[3];
};

struct s_firing_zone_set
{
	long zone_count;
	s_firing_zone *zones;
};

struct s_scenario_firing_view
{
	byte unknown000[0x1d8];
	long zone_set_count;
	s_firing_zone_set *zone_sets;
};

inline s_firing_position *firing_position_get(long reference)
{
	s_scenario_firing_view *scenario = (s_scenario_firing_view *)g_4e0350;

	return &scenario->zone_sets->zones[(reference >> 16) & 0xffff].positions[reference & 0xffff];
}

/* the prop search (0x758 bytes) of function_261280 and function_2605d0 */
struct s_prop_search_point
{
	real weight;
	point3f position;
};

struct s_prop_search
{
	short type;
	byte unknown02[0x14 - 0x2];
	bool unknown14;
	byte unknown15[0x19 - 0x15];
	bool unknown19;
	byte unknown1a[0x59 - 0x1a];
	bool unknown59;
	byte unknown5a[0x70 - 0x5a];
	long point_count;
	s_prop_search_point points[32];
	byte unknown274[0x758 - 0x274];
};

real function_259a0(dword *seed);
void *function_1e4f90(long actor_index);
long function_1469f0(real seconds);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
bool __stdcall function_1f8a70(long actor_index, long unknown);
void function_1f90f0(long actor_index, s_path_source *source);
bool function_2715a0(byte *buffer);
bool function_270750(byte *buffer, long unknown, s_actor_point_target const *target, real *distance, long a, long b);
s_reference function_261280(s_prop_search *search, long actor_index, s_261d20_entry *entry, long *b, byte *buffer, bool *c);
void function_265cb0(long actor_index);

long function_26d100(vector3f const *up, s_collision_result_1697c0 *collision, long *unknown, point3f const *point);
short function_272700(s_type_f17a25 *state, long node_index);

#endif
