// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0B9FC0.CPP: machines

The machine object type's callbacks (its definition
at 0x468310) and the helpers only they call. Halo CE's machine source file
has the same place, new and update; Halo 2 adds the portals a door opens
and closes, and keyframes the machine's Havok bodies to its nodes. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_1c62f0.h"
#include "unknown_1cec30.h"
#include <math.h>

#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))
#endif

/* the machine (a device; the device fields as devices.cpp has them, then
   the machine's own from +0x1cc) */
struct s_machine
{
	long definition_index;
	struct
	{
		dword unknown0 : 11;
		dword dynamic_lighting : 1;
		dword static_lighting : 1;
		dword static_lighting_sideways : 1;
		dword unknown14 : 8;
		dword unknown22 : 1;
		dword unknown23 : 1;
		dword : 8;
	} object_flags;
	byte unknown008[0xc];
	long parent_object_index;
	byte unknown018[0x28 - 0x18];
	s_location location;
	point3f center;
	real radius;
	byte unknown040[0xb4 - 0x40];
	long havok_component_index;
	byte unknown0b8[0xc0 - 0xb8];
	word flags_c0;
	byte unknown0c2[0xd4 - 0xc2];
	long value_d4;
	byte unknown0d8[0x10a - 0xd8];
	byte flags_10a;
	byte unknown10b[0x12a - 0x10b];
	short animation_state_offset;
	dword device_flags;
	long position_group_index;
	real position;
	real position_velocity;
	long power_group_index;
	real power;
	real power_velocity;
	byte unknown148[0x18c - 0x148];
	c_animation_channel channels[2];
	dword flags;
	long door_open_ticks;
	short placement_index;
	byte unknown1d6[0x1da - 0x1d6];
	short first_portal_reference;
	short portal_reference_count;
};

struct s_machine_header
{
	byte unknown00[2];
	byte flags;
	byte type;
	byte unknown04[4];
	s_machine *machine;
};

/* the machine definition (the tag data) */
struct s_machine_definition
{
	byte unknown000[0xbc];
	dword object_flags;
	byte unknown0c0[0xc8 - 0xc0];
	real powered_velocity;
	byte unknown0cc[4];
	real depowered_velocity;
	byte unknown0d4[0x118 - 0xd4];
	real automatic_activation_radius;
	short type;
	word flags;
	real door_open_seconds;
	real obstruction_lower_bound;
	real obstruction_upper_bound;
	byte unknown12c[4];
	short pathfinding_policy;
};

/* a unit's definition, as the door check reads it */
struct s_machine_unit_definition_view
{
	byte unknown000[0xbc];
	dword object_flags;
	byte unknown0c0[0x1ec - 0xc0];
	dword unit_flags;
};

/* the scenario machine (the placement) */
struct s_type_181dea
{
	byte unknown00[0x3c];
	dword flags;
	long portal_count;
	struct
	{
		short unknown0;
		short portal_index;
	} *portals;
};

/* the structure bsp's portals, and the portals' references to them, as a
   machine reads them (g_4e0348) */
struct s_machine_portal
{
	short front_cluster_index;
	short back_cluster_index;
	byte unknown04[0x24 - 4];
};

struct s_machine_portal_references
{
	byte unknown00[0xc];
	short *portal_indices;
};

struct s_machine_structure_bsp_view
{
	byte unknown000[0x60];
	s_machine_portal *portals;
	byte unknown064[0x220 - 0x64];
	s_machine_portal_references *portal_references;
};

/* the object nodes a machine's Havok bodies follow */
struct s_type_1a7926
{
	byte unknown00[8];
	transform4x3f root_matrix;
	byte unknown3c[0x48 - 0x3c];
	short *node_indices;
	byte unknown4c[4];
	transform4x3f *field_50;
};

#define MACHINE_GET(index) (((s_machine_header *)g_4e0300->data)[(index) & 0xffff].machine)
#define MACHINE_HEADER_GET(index) (&((s_machine_header *)g_4e0300->data)[(index) & 0xffff])
#define MACHINE_DEFINITION_GET(index) ((s_machine_definition *)g_4e3b44[(index) & 0xffff].bytes)
#define MACHINE_STRUCTURE_BSP ((s_machine_structure_bsp_view *)g_4e0348)
#define MACHINE_PORTAL_SCALES (g_4ed288->scales)
#define MACHINE_PORTAL_BITS ((dword *)g_4f93a4)

enum
{
	_machine_door = 0,
	_machine_platform,
	_machine_gear,

	MAXIMUM_OBJECTS_OPENING_MACHINE = 16,
	MACHINE_DOOR_OPEN_TICKS_DELAY = -3
};

struct s_device_group_view
{
	short identifier;
	word flags;
	real value;
	real desired_value;
};

#define MACHINE_DEVICE_GROUP_GET(index) (&((s_device_group_view *)g_4e0328.groups->data)[(index) & 0xffff])

void function_b8b70(long object_index);
long function_1fa7f0(void);
struct s_pair_table;
bool function_2101d0(s_pair_table *table, long object_index);
void __stdcall function_1d24a0(s_havok_component *component, real position);
void function_b58c0(long index, dword mask);
void __stdcall function_107520(long object_index);
bool __stdcall function_1071e0(long group_index, real value);
short __stdcall function_bb050(long a, dword type_mask, void const *location, point3f const *position, real radius,
	long *objects, short maximum_count);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
void __stdcall function_bd020(long object_index);
void function_bba20(long object_index);
bool function_20a9a0(s_type_1a7926 *matrices, long object_index);
void havok_component_rigid_body_linear_velocity_set(long rigid_body_index, s_havok_component *component,
	vector3f const *velocity);
void havok_component_rigid_body_angular_velocity_set(long rigid_body_index, s_havok_component *component,
	vector3f const *velocity);
void havok_component_rigid_body_matrix_set(long rigid_body_index, s_havok_component *component,
	transform4x3f const *matrix);
void function_1d0ee0(long rigid_body_index, s_havok_component *component, transform4x3f const *matrix);
void __stdcall function_bf600(long user, real frame, s_animation_frame_event const *event);
extern vector3f *g_4687a4;

/* a float to a short, rounded (fld, fistp) */
__forceinline short machine_round(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return (short)result;
}

PRIVATE void machine_keyframe_rigid_bodies(long machine_index, bool keyframed);
PRIVATE void machine_release_rigid_bodies(long machine_index);

// @retail 0x115020
void __stdcall machine_place(long machine_index, s_type_181dea *scenario_machine)
{
	s_machine *machine = MACHINE_GET(machine_index);

	if (scenario_machine->flags & 1)
		machine->flags |= 1;
	if (scenario_machine->flags & 0x20)
		machine->flags |= 2;
	if (scenario_machine->flags & 2)
		machine->flags |= 4;
	if (scenario_machine->flags & 0x10)
		machine->flags |= 8;
	if (scenario_machine->flags & 4)
		machine->flags |= 0x10;
	if (scenario_machine->flags & 8)
		machine->flags |= 0x20;
	machine->placement_index = NONE;
	machine->object_flags.unknown22 = false;
	machine->object_flags.unknown23 = false;

	short policy;

	if (machine->definition_index == NONE)
	{
		policy = 3;
	}
	else
	{
		switch (MACHINE_DEFINITION_GET(machine->definition_index)->pathfinding_policy)
		{
		case 0:
			policy = 2;
			break;
		case 1:
			policy = 1;
			break;
		case 2:
			policy = 0;
			break;
		default:
			policy = 3;
			break;
		}
	}

	switch (policy)
	{
	case 1:
		if (g_4686c4 >= 0 && g_4686c4 < scenario_machine->portal_count)
		{
			short const *portal_index = &scenario_machine->portals[g_4686c4].portal_index;

			machine->placement_index = *portal_index;
			if (*portal_index != NONE)
			{
				machine->object_flags.unknown22 = true;

				long table = function_1fa7f0();

				if (table)
				{
					short index = *portal_index;

					if (index >= 0 && index < *(long *)(table + 0x30) &&
						(*(byte *)(*(long *)(table + 0x34) + index * 0x1c) & 1))
					{
						function_2101d0((s_pair_table *)table, machine_index);
					}
				}
			}
		}
		// fall through
	case 0:
	case 3:
		machine->object_flags.unknown23 = true;
		function_b8b70(machine_index);
		break;
	default:
		function_b8b70(machine_index);
		break;
	}
}

// @retail 0x115190
bool __stdcall machine_new(long machine_index, void const *data, long unused)
{
	s_machine *machine = MACHINE_GET(machine_index);
	s_machine_definition *definition = MACHINE_DEFINITION_GET(machine->definition_index);

	*(dword *)&machine->object_flags |= 0x800;
	if ((*(byte *)&definition->flags >> 2) & 1)
		machine->object_flags.static_lighting = true;
	else
		machine->object_flags.static_lighting = false;
	if ((*(byte *)&definition->flags >> 2) & 1)
		machine->object_flags.static_lighting_sideways = true;
	else
		machine->object_flags.static_lighting_sideways = false;
	machine->first_portal_reference = NONE;
	machine->portal_reference_count = 0;
	return true;
}

/* opens the portals a machine blocks */
// @retail 0x115220
void __stdcall function_115220(long machine_index)
{
	s_machine *machine = MACHINE_GET(machine_index);

	if (machine->portal_reference_count > 0)
	{
		long i = machine->first_portal_reference;

		if (i < machine->first_portal_reference + machine->portal_reference_count)
		{
			real scale = 0.0f;

			do
			{
				short portal_index = MACHINE_STRUCTURE_BSP->portal_references->portal_indices[(word)i];

				MACHINE_PORTAL_BITS[portal_index >> 5] |= 1 << (portal_index & 0x1f);
				MACHINE_PORTAL_SCALES[i] = machine_round(scale);
			} while (++i < machine->first_portal_reference + machine->portal_reference_count);
		}
	}
}

// @retail 0x1152e0
bool __stdcall function_1152e0(long machine_index)
{
	s_machine *machine = MACHINE_GET(machine_index);
	s_machine_definition *definition = MACHINE_DEFINITION_GET(machine->definition_index);
	s_animation_state *state = NULL;
	bool animated = false;
	bool changed = false;
	word object_flags = machine->flags_c0;

	if (((object_flags >> 12) & 1) && (!(0.0001f > (real)fabs(machine->position)) || ((object_flags >> 10) & 1)) &&
		machine->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(machine->havok_component_index);
		real position = ((object_flags >> 10) & 1) ? 1.0f : 0.0f;

		if (machine->position > position)
			position = machine->position;
		function_1d24a0(component, position);
		changed = true;
	}

	if (MACHINE_GET(machine_index)->animation_state_offset != NONE && machine->channels[0].graph_tag_index != NONE &&
		machine->channels[0].animation_id.index != NONE)
	{
		state = (s_animation_state *)((byte *)machine + machine->animation_state_offset);
		animated = true;
	}

	if (definition->type == _machine_gear)
	{
		if (animated)
			machine->channels[0].set_frame_ratio(machine->power);
		machine->power_velocity = 0.0f;

		real velocity = (1.0f - machine->power) * definition->depowered_velocity +
			definition->powered_velocity * machine->power;

		if (fabs(velocity) > 0.0001f)
		{
			machine->power += g_510c54->rate * velocity;
			if (machine->power >= 1.0f)
			{
				machine->power -= 1.0f;
				if (animated)
				{
					machine->channels[0].set_frame_ratio_and_advance(1.0f, state, function_bf600, machine_index);
					machine->channels[0].set_frame_ratio(0.0f);
				}
			}

			s_machine *current = MACHINE_GET(machine_index);

			if (current->value_d4 != NONE)
				function_b58c0(current->value_d4, 0x400);
			function_107520(machine_index);
			if (animated)
				machine->channels[0].set_frame_ratio_and_advance(machine->power, state, function_bf600, machine_index);
			if (machine->power_group_index != NONE)
				MACHINE_DEVICE_GROUP_GET(machine->power_group_index)->value = machine->power;
			changed = true;
		}
	}

	if (!(machine->flags & 1) && definition->type == _machine_door)
	{
		changed = true;
		if (!((g_510c54->game_time + machine_index) & 3) && g_4e6948->mode != 4)
		{
			bool open = false;
			real radius = 0.0001f > definition->automatic_activation_radius ? machine->radius :
				definition->automatic_activation_radius;
			long object_indices[MAXIMUM_OBJECTS_OPENING_MACHINE];
			long object_count = function_bb050(1, 3, &machine->location, &machine->center, radius, object_indices,
				MAXIMUM_OBJECTS_OPENING_MACHINE);

			for (long i = 0; i < object_count; i++)
			{
				s_machine_header *header = MACHINE_HEADER_GET(object_indices[i]);
				s_machine *unit = header->machine;
				s_machine_unit_definition_view *local_98b918 =
					(s_machine_unit_definition_view *)g_4e3b44[unit->definition_index & 0xffff].bytes;
				bool can_open = true;

				if (((1 << header->type) & 2) && !((local_98b918->unit_flags >> 20) & 1))
					can_open = false;
				else if ((unit->flags_10a >> 2) & 1)
					continue;
				if (((local_98b918->object_flags >> 14) & 1) || !can_open)
					continue;
				if ((machine->flags & 4) || (machine->flags & 8) && *(long *)((byte *)unit + 0x13c) != NONE)
				{
					if (machine->power == 0.0f)
					{
						s_object_marker marker;
						vector3f direction;

						if (function_b8d30(machine_index, 0x7000686, &marker, 1, false) <= 0)
						{
							marker.matrix.position = machine->center;
							function_b9fc0(machine_index, &marker.matrix.forward, NULL);
						}
						direction.i = unit->center.x - marker.matrix.position.x;
						direction.j = unit->center.y - marker.matrix.position.y;
						direction.k = unit->center.z - marker.matrix.position.z;
						if (marker.matrix.forward.i * direction.i + marker.matrix.forward.k * direction.k +
							marker.matrix.forward.j * direction.j > 0.0f)
						{
							continue;
						}
					}
				}
				open = true;
			}

			if (open)
			{
				if (machine->power_group_index != NONE)
					function_1071e0(machine->power_group_index, 1.0f);
				machine->door_open_ticks = MACHINE_DOOR_OPEN_TICKS_DELAY;
			}
		}
	}

	if (definition->type == _machine_door)
	{
		bool closed = false;

		if (machine->power_group_index != NONE &&
			((machine->device_flags & 0x100) && MACHINE_DEVICE_GROUP_GET(machine->power_group_index)->value != 0.0f ||
			machine->power != 0.0f))
		{
			closed = true;
		}

		for (long i = machine->first_portal_reference; i < machine->first_portal_reference + machine->portal_reference_count; i++)
		{
			short portal_index = MACHINE_STRUCTURE_BSP->portal_references->portal_indices[(word)i];
			real obstruction = (definition->obstruction_upper_bound - definition->obstruction_lower_bound) * machine->power +
				definition->obstruction_lower_bound;

			if (closed)
				MACHINE_PORTAL_BITS[portal_index >> 5] |= 1 << (portal_index & 0x1f);
			else
				MACHINE_PORTAL_BITS[portal_index >> 5] &= ~(1 << (portal_index & 0x1f));
			MACHINE_PORTAL_SCALES[i] = machine_round(PIN(obstruction, 0.0f, 1.0f) * 65535.0f);
		}

		if (machine->power == 1.0f && !(machine->flags & 2) && g_4e6948->mode != 4)
		{
			if ((real)++machine->door_open_ticks * g_510c54->rate > definition->door_open_seconds &&
				machine->power_group_index != NONE)
			{
				function_1071e0(machine->power_group_index, 0.0f);
			}
			changed = true;
		}
		else
		{
			machine->door_open_ticks = 0;
		}
	}

	bool moved = (machine->device_flags >> 2) & 1;

	if (moved)
	{
		changed = true;
		function_bd020(machine_index);
		machine->device_flags &= ~4;
	}
	if (!moved)
	{
		for (long parent_index = machine->parent_object_index; parent_index != NONE; )
		{
			s_machine_header *header = MACHINE_HEADER_GET(parent_index);

			if (header->flags & 4)
				moved = true;
			parent_index = header->machine->parent_object_index;
			if (moved)
				break;
		}
	}
	if (moved)
	{
		machine_keyframe_rigid_bodies(machine_index, false);
		function_bba20(machine_index);
		return true;
	}
	machine_release_rigid_bodies(machine_index);
	return changed;
}

/* whether any cluster on either side of a machine's portals is set */
// @retail 0x115a40
bool __stdcall function_115a40(long machine_index, dword const *cluster_bits, bool *found)
{
	s_machine *machine = MACHINE_GET(machine_index);
	bool result = false;

	if (machine->portal_reference_count > 0)
	{
		long end = machine->first_portal_reference + machine->portal_reference_count;

		for (long i = machine->first_portal_reference; i < end; i++)
		{
			s_machine_portal *portal =
				&MACHINE_STRUCTURE_BSP->portals[MACHINE_STRUCTURE_BSP->portal_references->portal_indices[(word)i]];

			if (cluster_bits[portal->front_cluster_index >> 5] & (1 << (portal->front_cluster_index & 0x1f)) ||
				cluster_bits[portal->back_cluster_index >> 5] & (1 << (portal->back_cluster_index & 0x1f)))
			{
				*found = true;
				break;
			}
		}
		result = true;
	}
	return result;
}

/* moves a machine's keyframed Havok bodies to its nodes, stopping them
   first when they're being keyframed */
// @retail 0x115b10
PRIVATE void machine_keyframe_rigid_bodies(long machine_index, bool keyframed)
{
	s_machine *machine = MACHINE_GET(machine_index);
	s_type_1a7926 matrices;

	if (machine->havok_component_index == NONE || !function_20a9a0(&matrices, machine_index))
		return;

	s_havok_component *component = havok_component_get(machine->havok_component_index);

	for (long i = 0; i < component->rigid_bodies.size; i++)
	{
		if (havok_component_rigid_body_get(i, component)->m_motion->getType() == 6 &&
			component->rigid_bodies[i].node_count > 0)
		{
			short const *node_index = &matrices.node_indices[component->rigid_bodies[i].nodes[0] * 0x48];

			if (keyframed)
			{
				havok_component_rigid_body_linear_velocity_set(i, component, g_4687a4);
				havok_component_rigid_body_angular_velocity_set(i, component, g_4687a4);
				havok_component_rigid_body_matrix_set(i, component,
					*node_index == NONE ? &matrices.root_matrix : &matrices.field_50[*node_index]);
			}
			else
			{
				function_1d0ee0(i, component,
					*node_index == NONE ? &matrices.root_matrix : &matrices.field_50[*node_index]);
			}
		}
	}
	if (keyframed)
		machine->flags &= ~0x40;
	else
		machine->flags |= 0x40;
}

/* stops a machine's keyframed Havok bodies once it stops moving */
// @retail 0x115c70
PRIVATE void machine_release_rigid_bodies(long machine_index)
{
	s_machine *machine = MACHINE_GET(machine_index);

	if ((machine->flags & 0x40) && machine->havok_component_index != NONE)
	{
		s_havok_component *component = havok_component_get(machine->havok_component_index);

		for (long i = 0; i < component->rigid_bodies.size; i++)
		{
			if (havok_component_rigid_body_get(i, component)->m_motion->getType() == 6)
			{
				havok_component_rigid_body_linear_velocity_set(i, component, g_4687a4);
				havok_component_rigid_body_angular_velocity_set(i, component, g_4687a4);
			}
		}
		machine->flags &= ~0x40;
	}
}

struct s_component_property_view
{
	byte unknown00[4];
	dword flags;
	byte unknown08[0x1a - 8];
	byte value1a;
	byte value1b;
	bool function_1d3550(long key, bool *positive, real *value) const;
};

// @retail 0x11bd60
bool __stdcall function_11bd60(long object_index, long key, long positive, long value)
{
	long index = MACHINE_GET(object_index)->havok_component_index;
	bool result = false;
	if (index != NONE)
	{
		s_component_property_view *component = (s_component_property_view *)((byte *)g_51e9b8->data + (index & 0xffff) * 0xa0);
		if (component->function_1d3550(key, (bool *)positive, (real *)value))
			result = true;
	}
	return result;
}

/* the machine object type definition (through its last callback) */
struct s_machine_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	long unknown0c;
	void *unknown10[7];
	bool (__stdcall *name_3793c6)(long, void const *, long);
	void (__stdcall *place)(long, s_type_181dea *);
	void *unknown34[2];
	void (__stdcall *handler3c)(long);
	bool (__stdcall *update)(long);
	void *unknown44;
	bool (__stdcall *handler48)(long, dword const *, bool *);
	bool (__stdcall *handler4c)(long, long, long, long);
};

s_machine_type_definition g_468310 =
{
	"machine",
	'mach',
	0x1e0,
	0xa8,
	0x4800b0,
	{ 0, 0, 0, 0, 0, 0, 0 },
	machine_new,
	machine_place,
	{ 0, 0 },
	function_115220,
	function_1152e0,
	0,
	function_115a40,
	function_11bd60
};


#include "effects.h"
struct s_object;
s_object *function_badc0(long object_index, dword mask);
long function_189060(long object_index, short node, real scale, point3f const *position,
    vector3f const *direction, long tag_index);
void __stdcall function_e5040(long object_index, s_animation_frame_event const *event);

// @retail 0xbf600
void __stdcall function_bf600(long user, real frame, s_animation_frame_event const *event)
{
    if (event->category == 1 && event->tag_index != NONE)
    {
        byte *header = g_4e0300->data + (user & 0xffff) * 12;
        byte *object = *(byte **)(header + 8);
        dword type_mask = 1u << object[0xaa];
        long player = NONE;
        if (type_mask & 3)
        {
            byte *unit = (byte *)function_badc0(user, 3);
            if (unit) player = *(long *)(unit + 0x13c);
        }
        if ((event->flags & 1) || player == NONE)
        {
            s_object_marker marker;
            if (event->marker_name == NONE || event->marker_name == 0x0600008a
                || !function_b8d30(user, event->marker_name, &marker, 1, false))
            {
                marker.node_index = 0;
                marker.node_matrix.position = *g_468788;
                marker.node_matrix.forward = *g_4687a8;
            }
            function_189060(user, marker.node_index, 1.0f, &marker.node_matrix.position,
                &marker.node_matrix.forward, event->tag_index);
        }
    }
    else if (event->category == 2 && event->tag_index != NONE)
        function_176780(user, 0, 0.0f, event->tag_index, 0.0f, 0, 0);
    else
    {
        byte *header = g_4e0300->data + (user & 0xffff) * 12;
        if (!header[3]) function_e5040(user, event);
    }
}

