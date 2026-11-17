#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "object_markers.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "unknown_2551c0.h"
#include "swarm.h"
#include "normalize3d.h"
#include <math.h>
#include <stddef.h>

// @flags /O2 /Gr /arch:SSE

/* the placement a spawned creature starts from (filled by its constructor, 0x200e60) */
struct s_actor_position_state
{
	s_actor_position_state();
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	real facing;
	real field18;
	long field1c;
	byte unknown20[0xe];
	short field2e;
	byte unknown30[0x64 - 0x30];
};
typedef char assert_256bd0_placement_size[(sizeof(s_actor_position_state) == 0x64) ? 1 : -1];
typedef char assert_256bd0_placement_facing[(offsetof(s_actor_position_state, facing) == 0x14) ? 1 : -1];
typedef char assert_256bd0_placement_field2e[(offsetof(s_actor_position_state, field2e) == 0x2e) ? 1 : -1];
struct s_28dab2;
struct s_203780;
extern short g_470c40, g_470c44;
extern point2f *g_468778;
real function_30bf0(vector3f *vector);
real function_11cc90(vector2f const *a, vector2f const *b);
long function_1e4990(long index);
void swarm_iterator_new(s_290c50_iterator *iterator);
long function_28dbd0(long arg_0, long tag_index, long object_definition_index, s_28dab2 const *placement, bool flag);
long __stdcall function_1e0850(long character_index, long squad_index, s_203780 const *placement, bool substitute);
bool function_1e11b0(long actor_index, bool active);
void __stdcall function_b77d0(long object_index, vector3f const *linear, vector3f const *angular);
void swarm_creatures_set_object_flag(long perception_index, bool flag);
void function_b7360(long object_index);
real distance_sq3f(point3f const *a, point3f const *b);

static inline byte *perception_iterator_next_256bd0(s_290c50_iterator *iterator)
{
	byte *entry = NULL;
	if (g_4f55d0->active)
	{
		/* one plain read of the parameter through its address: retail reads the activity byte again on every pass */
		s_290c50_iterator *const *reference = &iterator;
		s_data_datum_iterator *records = (s_data_datum_iterator *)*reference;
		if (data_datum_iterator_next(records))
			entry = records->datum;
		iterator->object_index = records->datum_index;
	}
	return entry;
}

/* the flags word at +0x10a of an object */
struct s_object_flags_256bd0
{
	word unknown0 : 6;
	word bit6 : 1;
};
// @retail 0x256bd0
short __stdcall function_256bd0(long actor_index, s_slot *slot, bool active)
{
	byte *actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
	short result = g_470c44;
	short *timer = (short *)slot->unknownc;
	--*timer;
	if (*timer <= 0)
	{
		s_object_marker marker;
		point3f position;
		long marker_object_index = *(long *)(actor + 0x18);
		if (function_b8d30(marker_object_index, 0xf0005b4, &marker, 1, false) > 0)
			position = marker.matrix.position;
		else
			position = *(point3f *)(actor + 0x22c);
		vector3f direction = *(vector3f *)(actor + 0x290);
		if (direction.k != 0.0f)
		{
			direction.k = 0.0f;
			if (function_30bf0(&direction) == g_45dbd8)
				direction = *g_4687a8;
		}
		s_actor_position_state placement;
		placement.position = position;
		placement.field10 = NONE;
		vector2f const *forward = (vector2f const *)g_468778;
		placement.facing = function_11cc90(forward, (vector2f const *)&direction);
		placement.field18 = 0.0f;
		placement.field1c = 4;
		placement.field2e = 1;
		byte *unit = *(byte **)(g_4e0300->data + (*(long *)(actor + 0x18) & 0xffff) * 12 + 8);
		byte *definition = g_4e3b44[*(long *)unit & 0xffff].bytes;
		long definition_index = *(long *)(definition + 0x30c);
		if (definition_index != NONE)
		{
			long variant = *(long *)(g_4e3b44[definition_index & 0xffff].bytes + 0x18);
			if (variant != NONE)
			{
				long object_index = NONE;
				long selected = NONE;
				byte *flags = (byte *)function_1e4990(definition_index);
				if (flags && (flags[0] & 1))
				{
					real minimum = 25.0f;
					s_290c50_iterator iterator;
					swarm_iterator_new(&iterator);
					byte *entry;
					while ((entry = perception_iterator_next_256bd0(&iterator)) != NULL)
					{
						vector3f delta;
						vector3d_from_points3d((point3f const *)(entry + 8), &position, &delta);
						real distance = length_sq3f(&delta);
						if (minimum > distance)
						{
							minimum = distance;
							selected = iterator.object_index;
						}
					}
					if (selected != NONE)
						object_index = function_28dbd0(selected, definition_index, variant, (s_28dab2 const *)&placement, false);
				}
				if (selected == NONE)
				{
					long spawned = function_1e0850(definition_index, *(long *)(actor + 0x30), (s_203780 const *)&placement, false);
					if (spawned != NONE)
					{
						byte *other = g_4f55f0->data + (spawned & 0xffff) * 0x888;
						if (*(long *)(actor + 0x30) == NONE)
							function_1e11b0(spawned, true);
						long perception = *(long *)(other + 0x1c);
						if (perception != NONE)
							object_index = *(long *)(g_5044c8->data + (perception & 0xffff) * 0x34 + 0x18);
					}
				}
				if (object_index != NONE)
				{
					vector3f velocity;
					vector3f_scale(&direction, 0.03f, &velocity);
					function_b77d0(object_index, &velocity, 0);
				}
			}
		}
		actor = g_4f55f0->data + (actor_index & 0xffff) * 0x888;
		if (actor[7])
		{
			long perception = *(long *)(actor + 0x1c);
			if (perception != NONE)
				swarm_creatures_set_object_flag(perception, true);
		}
		else
		{
			long unit_index = *(long *)(actor + 0x18);
			s_object_flags_256bd0 *flags = (s_object_flags_256bd0 *)(*(byte **)(g_4e0300->data + (unit_index & 0xffff) * 12 + 8) + 0x10a);
			flags->bit6 = true;
			function_b7360(*(long *)(actor + 0x18));
		}
		return g_470c40;
	}
	return result;
}
