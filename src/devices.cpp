// @flags /O2 /arch:SSE /Gr
/* DEVICES.CPP: devices (object types 7..9: machines, controls and light
   fixtures) and the device groups that drive their position and power.
   device_groups_initialize and device_groups_dispose (0x106460, 0x106490)
   are in unknown_1061c0.cpp, the script flag setters (0x107590, 0x1075e0)
   in unknown_107590.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "unknown_1c62f0.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"
#include <math.h>

#define DEVICE_TYPE_MASK 0x380

/* a device group (12 bytes, in g_4e0328.groups) */
struct s_device_group
{
	short identifier;
	word flags;
	real value;
	real desired_value;
};

/* the scenario's device groups (a view of g_4e0350) */
struct s_scenario_device_group
{
	byte unknown00[0x20];
	real initial_value;
	byte flags;
	byte unknown25[3];
};

struct s_scenario_device_groups_view
{
	byte unknown00[0xa0];
	long device_group_count;
	s_scenario_device_group *device_groups;
};

/* the device definition (the tag data) */
struct s_device_definition
{
	byte unknown000[0x38];
	long model_tag_index;
	byte unknown03c[0xc0 - 0x3c];
	real position_speed;
	byte unknown0c4[4];
	real power_speed;
	byte unknown0cc[0x100 - 0xcc];
	long tag_index_100;
	byte unknown104[4];
	long tag_index_108;
	real delay_time;
};

/* a device's motion toward a target value: it accelerates from its velocity,
   cruises, then decelerates (8 floats, two of them in the device) */
struct s_device_motion
{
	real position;
	real velocity;
	real start;
	real target;
	real acceleration_distance;
	real deceleration_distance;
	real cruise_velocity;
	real duration;
};

/* the device (the object data) */
struct s_device
{
	long definition_index;
	byte unknown004[8];
	long next_object_index;
	long first_child_index;
	long parent_index;
	byte unknown018[0x64 - 0x18];
	point3f field_xcc658c;
	vector3f forward;
	vector3f up;
	byte unknown088[0xaa - 0x88];
	byte type;
	byte unknown0ab[0xc2 - 0xab];
	short location_c2;
	long location_c4;
	long location_c8;
	byte unknown0cc[0xd4 - 0xcc];
	long value_d4;
	byte unknown0d8[0x10e - 0xd8];
	short orientation_a_offset;
	byte unknown110[2];
	short orientation_b_offset;
	byte unknown114[0x12a - 0x114];
	short animation_state_offset;
	dword flags;
	long position_group_index;
	real position;
	real position_velocity;
	long power_group_index;
	real power;
	real power_velocity;
	short delay_ticks;
	byte unknown14a[2];
	s_device_motion motion_14c;
	s_device_motion motion_16c;
	c_animation_channel channels[2];
};

struct s_device_header
{
	byte unknown00[8];
	s_device *device;
};

/* the location the effects and sounds of a device start from */

struct s_tag_group_view
{
	dword group_tag;
};

/* an iteration over the devices: the current device, then the object
   iterator */
struct s_device_iterator
{
	s_device *device;
	s_type_f1af8e iterator;
};

static inline void device_iterator_new(s_device_iterator *iterator)
{
	function_bae80(&iterator->iterator, DEVICE_TYPE_MASK, 0);
}

static inline bool device_iterator_next(s_device_iterator *iterator)
{
	iterator->device = (s_device *)function_baeb0(&iterator->iterator);
	return iterator->device != NULL;
}

#define DEVICE_GET(index) (((s_device_header *)g_4e0300->data)[(index) & 0xffff].device)
#define DEVICE_GROUP_GET(index) (&((s_device_group *)g_4e0328.groups->data)[(index) & 0xffff])

void function_b7360(long object_index);
void function_b58c0(long index, dword mask);
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index);
void device_groups_initialize();
void device_groups_dispose();

static inline void data_make_invalid_inlined(s_record_pool *data)
{
	data->valid = false;
}

void __stdcall function_107520(long object_index);
void function_107a30(void);

// @retail 0x1064e0
void function_1064e0(void)
{
	data_make_valid_inlined(g_4e0328.groups);
	function_107a30();
}

// @retail 0x106500
void function_106500(void)
{
	data_make_invalid_inlined(g_4e0328.groups);
}

// @retail 0x106680
void __stdcall function_106680(long device_index)
{
	s_device *device = DEVICE_GET(device_index);

	if (device->position_group_index != NONE && (DEVICE_GROUP_GET(device->position_group_index)->flags & 4))
		record_pool_release(g_4e0328.groups, device->position_group_index);
	device->position_group_index = NONE;
	if (device->power_group_index != NONE && (DEVICE_GROUP_GET(device->power_group_index)->flags & 4))
		record_pool_release(g_4e0328.groups, device->power_group_index);
	device->power_group_index = NONE;
	device->motion_14c.position = 0.0f;
	device->motion_14c.velocity = 0.0f;
	device->motion_14c.start = 0.0f;
	device->motion_14c.target = 0.0f;
	device->motion_14c.cruise_velocity = 0.0f;
	device->motion_14c.acceleration_distance = 0.0f;
	device->motion_14c.deceleration_distance = 0.0f;
	device->motion_16c.position = 0.0f;
	device->motion_16c.velocity = 0.0f;
	device->motion_16c.start = 0.0f;
	device->motion_16c.target = 0.0f;
	device->motion_16c.cruise_velocity = 0.0f;
	device->motion_16c.acceleration_distance = 0.0f;
	device->motion_16c.deceleration_distance = 0.0f;
}

// @retail 0x106510
bool __stdcall function_106510(long device_index, long a, long b)
{
	s_device *device = DEVICE_GET(device_index);

	device->power_group_index = NONE;
	device->position_group_index = NONE;
	device->channels[0].reset();
	device->channels[1].reset();
	device->motion_14c.position = 0.0f;
	device->motion_14c.velocity = 0.0f;
	device->motion_14c.start = 0.0f;
	device->motion_14c.target = 0.0f;
	device->motion_14c.cruise_velocity = 0.0f;
	device->motion_14c.acceleration_distance = 0.0f;
	device->motion_14c.deceleration_distance = 0.0f;
	device->motion_16c.position = 0.0f;
	device->motion_16c.velocity = 0.0f;
	device->motion_16c.start = 0.0f;
	device->motion_16c.target = 0.0f;
	device->motion_16c.cruise_velocity = 0.0f;
	device->motion_16c.acceleration_distance = 0.0f;
	device->motion_16c.deceleration_distance = 0.0f;
	return true;
}

// @retail 0x106780
void __stdcall function_106780(long device_index)
{
	function_106680(device_index);
}

// @retail 0x1070d0
long function_1070d0(short group_index)
{
	long result = NONE;
	long pinned = group_index < 0 ? 0 : (group_index > ((s_scenario_device_groups_view *)g_4e0350)->device_group_count - 1 ? ((s_scenario_device_groups_view *)g_4e0350)->device_group_count - 1 : group_index);

	if (pinned == group_index)
	{
		result = data_datum_index(g_4e0328.groups, group_index);
		if (DEVICE_GROUP_GET(result)->flags & 4)
			result = NONE;
	}
	return result;
}

// @retail 0x107430
void function_107430(long group_index, real value)
{
	if (group_index != NONE)
	{
		if (0.0f > value)
			value = 0.0f;
		else if (value > 1.0f)
			value = 1.0f;
		DEVICE_GROUP_GET(group_index)->value = value;

		s_device_iterator iterator;
		device_iterator_new(&iterator);
		while (device_iterator_next(&iterator))
		{
			s_device *device = iterator.device;

			if (device->position_group_index == group_index)
			{
				function_107520(iterator.iterator.object_index);
				device->position = value;
				device->position_velocity = 0.0f;
				function_b7360(iterator.iterator.object_index);
			}
			if (device->power_group_index == group_index)
			{
				function_107520(iterator.iterator.object_index);
				device->power = value;
				device->power_velocity = 0.0f;
				function_b7360(iterator.iterator.object_index);
			}
		}
	}
}

// @retail 0x107980
void function_107980(long object_index, long tag_index)
{
	if (tag_index != NONE)
	{
		s_device *device = DEVICE_GET(object_index);
		s_effect_owner owner;

		owner.unknown4 = device->location_c8;
		owner.unknown0 = device->location_c4;
		owner.unknown8 = device->location_c2;
		switch (((s_tag_group_view *)&g_4e3b44[(short)tag_index])->group_tag)
		{
		case 'effe':
			function_176780(object_index, &owner, device->power, tag_index, device->position, NULL, NULL);
			break;
		case 'snd!':
			function_189060(object_index, NONE, 1.0f, g_468788, g_4687a8, tag_index);
			break;
		}
	}
}

// @retail 0x1071e0
bool __stdcall function_1071e0(long group_index, real value)
{
	bool result = false;

	if (0.0f > value)
		value = 0.0f;
	else if (value > 1.0f)
		value = 1.0f;
	if (group_index != NONE)
	{
		s_device_group *group = DEVICE_GROUP_GET(group_index);

		if (group->value != value && (!(group->flags & 1) || !(group->flags & 2)))
		{
			group->flags |= 2;
			group->value = value;
			result = true;

			s_device_iterator iterator;
			device_iterator_new(&iterator);
			while (device_iterator_next(&iterator))
			{
				s_device *device = iterator.device;
				s_device_definition *definition = (s_device_definition *)g_4e3b44[device->definition_index & 0xffff].bytes;

				if (device->position_group_index == group_index)
				{
					function_107980(iterator.iterator.object_index, value > 0.0f ? definition->tag_index_108 : definition->tag_index_100);
					function_b7360(iterator.iterator.object_index);
				}
				if (device->power_group_index == group_index)
				{
					s_device *object = DEVICE_GET(iterator.iterator.object_index);
					if (object->value_d4 != NONE)
						function_b58c0(object->value_d4, 0x800);
					function_b7360(iterator.iterator.object_index);
				}
			}
		}
	}
	return result;
}

// @retail 0x107140
bool function_107140(long device_index, real value)
{
	bool result = false;

	if (device_index != NONE)
	{
		s_device *device = DEVICE_GET(device_index);
		if (device->power_group_index != NONE)
			result = function_1071e0(device->power_group_index, value);
		function_b7360(device_index);
	}
	return result;
}

// @retail 0x107190
void function_107190(long device_index, real value)
{
	if (device_index != NONE)
	{
		s_device *device = DEVICE_GET(device_index);
		function_107520(device_index);
		device->position = value;
		function_b7360(device_index);
		function_1071e0(device->position_group_index, value);
	}
}

/* 0x107370 (the power setter) is in unknown_107370.cpp: retail calls it out
   of line, which needs an /Ob1 file */

// @retail 0x1073c0
void function_1073c0(void)
{
	s_record_pool_iterator iterator;
	s_device_group *group;

	iterator.data = g_4e0328.groups;
	iterator.index = NONE;
	while ((group = (s_device_group *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		function_107430(iterator.datum_index, group->desired_value);
		group->flags &= ~2;
	}
}

// @retail 0x107520
void __stdcall function_107520(long object_index)
{
	s_device *device = DEVICE_GET(object_index);

	if ((1 << device->type) & DEVICE_TYPE_MASK)
	{
		device->flags |= 4;
		function_b7360(object_index);
	}
	for (long child_index = device->first_child_index; child_index != NONE; child_index = DEVICE_GET(child_index)->next_object_index)
		function_107520(child_index);
}

// @retail 0x107630
void function_107630(long group_index, bool flag)
{
	if (group_index != NONE)
	{
		s_device_group *group = DEVICE_GROUP_GET(group_index);

		if (flag)
			group->flags |= 1;
		else
			group->flags &= ~1;
		group->flags &= ~2;

		s_device_iterator iterator;
		device_iterator_new(&iterator);
		while (device_iterator_next(&iterator))
		{
			s_device *device = iterator.device;

			if (device->position_group_index == group_index)
				function_b7360(iterator.iterator.object_index);
			if (device->power_group_index == group_index)
				function_b7360(iterator.iterator.object_index);
		}
	}
}

// @retail 0x107870
bool function_107870(long device_index)
{
	s_device *device = DEVICE_GET(device_index);
	bool result = false;

	if (device->power_group_index != NONE)
	{
		s_device_group *power_group = DEVICE_GROUP_GET(device->power_group_index);
		s_device_group *position_group = DEVICE_GROUP_GET(device->position_group_index);
		word flags = power_group->flags;
		bool active = true;

		if ((flags & 1) && !(flags & 2))
			active = false;
		if (device->flags & 2)
			active = false;
		result = position_group->value == 1.0f && active;
	}
	return result;
}

// @retail 0x107a30
void function_107a30(void)
{
	s_scenario_device_groups_view *scenario = (s_scenario_device_groups_view *)g_4e0350;

	for (long i = 0; i < scenario->device_group_count; i++)
	{
		s_scenario_device_group *scenario_group = &scenario->device_groups[i];
		word flags = 0;
		real value;

		if (scenario_group->flags & 1)
			flags = 1;
		value = scenario_group->initial_value;

		long group_index = record_pool_allocate(g_4e0328.groups);
		if (group_index != NONE)
		{
			s_device_group *group = DEVICE_GROUP_GET(group_index);
			group->value = value;
			group->desired_value = value;
			group->flags = flags;
		}
	}
}

/* a control (object type 8) */
struct s_control
{
	byte unknown000[0x1cc];
	byte control_flags;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);

// @retail 0x1078f0
bool function_1078f0(long control_index, vector3f const *direction)
{
	s_control *control = (s_control *)function_badc0(control_index, 0x100);
	s_object_marker marker;
	bool result = true;

	if (control && !(control->control_flags & 1))
	{
		if (function_b8d30(control_index, 0x500008f, &marker, 1, false) == 1 &&
			dot3f(direction, &marker.matrix.forward) > 0.0f)
		{
			result = false;
		}
	}
	return result;
}

/* a model definition: its render model */
struct s_device_model_definition
{
	byte unknown00[4];
	long render_model_tag_index;
};

PRIVATE inline bool device_channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

/* samples a device's two channels; the render model goes unused */
PRIVATE __forceinline void device_channels_sample(void const *render_model, s_device *device, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms)
{
	if (device->animation_state_offset != NONE)
	{
		if (device_channel_valid(&device->channels[0]))
			device->channels[0].sample(1.0f, node_mask, node_count, transforms);
		if (device_channel_valid(&device->channels[1]))
			device->channels[1].sample(1.0f, node_mask, node_count, transforms);
	}
}

// @retail 0x107000
void __stdcall function_107000(long device_index, dword const *node_mask, long node_count,
	real_quaternion_transform *transforms)
{
	s_device *device = DEVICE_GET(device_index);
	long model_tag_index = ((s_device_definition *)g_4e3b44[device->definition_index & 0xffff].bytes)->model_tag_index;

	if (model_tag_index != NONE)
	{
		long render_model_tag_index = ((s_device_model_definition *)g_4e3b44[model_tag_index & 0xffff].bytes)->render_model_tag_index;

		if (render_model_tag_index != NONE)
			device_channels_sample(g_4e3b44[render_model_tag_index & 0xffff].bytes, device, node_mask, node_count, transforms);
	}
}

/* the scenario placement of a device */
struct s_device_placement
{
	byte unknown00[0x34];
	short position_group_index;
	short power_group_index;
	dword flags;
};

bool function_1086e0(long name, long device_index);
bool function_1087c0(long name, long device_index);

// @retail 0x1076e0
void __stdcall function_1076e0(long device_index, s_device_placement const *placement)
{
	s_device *device = DEVICE_GET(device_index);

	if (placement->position_group_index == NONE)
	{
		real value = (placement->flags & 2) ? 0.0f : 1.0f;
		long group_index = record_pool_allocate(g_4e0328.groups);

		if (group_index != NONE)
		{
			s_device_group *group = DEVICE_GROUP_GET(group_index);

			group->value = value;
			group->desired_value = value;
			group->flags = 4;
		}
		device->position_group_index = group_index;
	}
	else
	{
		device->position_group_index = function_1070d0(placement->position_group_index);
	}
	if (placement->power_group_index == NONE)
	{
		real value = (placement->flags & 1) ? 1.0f : 0.0f;
		word flags = (word)(((placement->flags & 4) | 0x10) >> 2);
		long group_index = record_pool_allocate(g_4e0328.groups);

		if (group_index != NONE)
		{
			s_device_group *group = DEVICE_GROUP_GET(group_index);

			group->value = value;
			group->desired_value = value;
			group->flags = flags;
		}
		device->power_group_index = group_index;
	}
	else
	{
		device->power_group_index = function_1070d0(placement->power_group_index);
	}
	device->position = DEVICE_GROUP_GET(device->position_group_index)->value;
	device->power = DEVICE_GROUP_GET(device->power_group_index)->value;
	if (placement->flags & 8)
		device->flags |= 1;
	if (placement->flags & 0x10)
		device->flags |= 2;
	function_1086e0(0x8000080, device_index);
	function_1087c0(0x5000081, device_index);
	device->flags |= 0x80;
}

/* a machine's flags (beyond the device data) */
struct s_device_machine_view
{
	byte unknown000[0x1cc];
	dword flags;
};

#define DEVICE_PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x106d70
bool __stdcall device_export_function(long device_index, long name, real *value, bool *active)
{
	s_device *device = DEVICE_GET(device_index);
	s_device_definition *definition = (s_device_definition *)g_4e3b44[device->definition_index & 0xffff].bytes;
	bool result = true;
	real function_value = 0.0f;

	switch (name)
	{
	case 0x5000561:
		if (definition->delay_time > 0.0f)
			function_value = g_510c54->rate / definition->delay_time * (real)device->delay_ticks;
		break;
	case 0x5000081:
		function_value = device->position;
		break;
	case 0x40005ab:
		function_value = (real)(device->power <= 0.9f);
		break;
	case 0x6000560:
		if (device->position == 0.0f)
			function_value = 1.0f;
		if (device->type == 7 && device->power_group_index != NONE)
		{
			s_device_group *group = DEVICE_GROUP_GET(device->power_group_index);
			dword machine_flags = ((s_device_machine_view *)device)->flags;
			word group_flags;

			if (machine_flags & 0xd)
				function_value = 1.0f;
			group_flags = group->flags;
			if ((group_flags & 1) && (group_flags & 2))
				function_value = 1.0f;
			if (device->power == 1.0f || (machine_flags & 0x10))
			{
				function_value = 0.0f;
				break;
			}
		}
		break;
	case 0x8000080:
		function_value = device->power;
		break;
	case 0xf00055e:
		if (device->position_velocity != 0.0f && definition->position_speed != 0.0f)
			function_value = (real)fabs(device->position_velocity) / definition->position_speed;
		break;
	case 0x1200055f:
		if (device->power_velocity != 0.0f)
		{
			if (device->flags & 8)
			{
				if (!(device->flags & 0x20) && !(0.0001f > fabs(device->motion_14c.cruise_velocity)))
					function_value = (real)fabs(device->power_velocity) / (real)fabs(device->motion_14c.cruise_velocity);
			}
			else if (!(0.0001f > fabs(definition->power_speed)))
			{
				function_value = (real)fabs(device->power_velocity) / definition->power_speed;
			}
		}
		break;
	default:
		result = false;
		break;
	}
	if (result)
	{
		function_value = DEVICE_PIN(function_value, 0.0f, 1.0f);
		*value = function_value;
		*active = function_value > 0.0f;
	}
	return result;
}

/* the device object type definition */
struct s_device_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	long unknown0c;
	void (*initialize)(void);
	void (*dispose)(void);
	void (*field_c_5)(void);
	void (*field_10_2)(void);
	void *unknown20[3];
	bool (__stdcall *handler2c)(long, long, long);
	void (__stdcall *handler30)(long, s_device_placement const *);
	void (__stdcall *handler34)(long);
	void *handler38;
	void (__stdcall *handler3c)(long);
	void *handler40;
	void *unknown44[2];
	bool (__stdcall *handler4c)(long, long, real *, bool *);
	void *unknown50[7];
	void (__stdcall *handler6c)(long, dword const *, long, real_quaternion_transform *);
};

s_device_type_definition g_468248 =
{
	"device",
	'devi',
	0x1cc,
	NONE,
	NONE,
	device_groups_initialize,
	device_groups_dispose,
	function_1064e0,
	function_106500,
	{ 0, 0, 0 },
	function_106510,
	function_1076e0,
	function_106680,
	0,
	function_106780,
	0,
	{ 0, 0 },
	device_export_function,
	{ 0, 0, 0, 0, 0, 0, 0 },
	function_107000
};

void function_11b9d0(long control_index, long unit_index);

// @retail 0x107840
void function_107840(long device_index, long unit_index)
{
	if (DEVICE_GET(device_index)->type == 8)
		function_11b9d0(device_index, unit_index);
}

/* whether an object has an animation state (0x1cda50), and the state */
PRIVATE inline bool device_has_animation_state(long device_index)
{
	return DEVICE_GET(device_index)->animation_state_offset != NONE;
}

PRIVATE inline s_animation_state *device_get_animation_state(s_device *device)
{
	return (s_animation_state *)((byte *)device + device->animation_state_offset);
}

/* plays an animation on one of a device's two channels: the named animation,
   else the overlay of that name */
PRIVATE __forceinline bool device_channel_play(long name, long device_index, short channel_index)
{
	bool result = false;

	if (device_index != NONE && device_has_animation_state(device_index))
	{
		s_device *device = DEVICE_GET(device_index);
		s_animation_state *state = device_get_animation_state(device);
		c_animation_channel *channel = &device->channels[channel_index];

		result = state->channel_play_named(channel, name, 0x40);
		if (!result)
			result = state->overlay_play(channel, 0x40, name, 0x7000101, 0x7000101);
		function_b7360(device_index);
	}
	return result;
}

// @retail 0x1086e0
bool function_1086e0(long name, long device_index)
{
	return device_channel_play(name, device_index, 0);
}

// @retail 0x1087c0
bool function_1087c0(long name, long device_index)
{
	return device_channel_play(name, device_index, 1);
}

// @retail 0x108530
bool function_108530(long device_index, long name)
{
	bool result = false;

	if (device_index != NONE && function_badc0(device_index, DEVICE_TYPE_MASK))
	{
		s_device *device = DEVICE_GET(device_index);

		if (device->animation_state_offset != NONE)
		{
			device->motion_16c.position = 0.0f;
			device->motion_16c.velocity = 0.0f;
			device->motion_16c.start = 0.0f;
			device->motion_16c.target = 0.0f;
			device->motion_16c.cruise_velocity = 0.0f;
			device->motion_16c.acceleration_distance = 0.0f;
			device->motion_16c.deceleration_distance = 0.0f;
			result = function_1087c0(name, device_index);
			if (!result)
				function_1087c0(0x5000081, device_index);
			function_b7360(device_index);
			if (result)
				device->flags |= 0x10;
			else
				device->flags &= ~0x10;
		}
	}
	return result;
}

#define PIN(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

/* starts a motion from the current position toward a target (both pinned to
   0..1) over a duration: accelerating from a velocity for the acceleration
   time, cruising, and decelerating to a final velocity */
// @retail 0x107ab0
void device_motion_start(s_device_motion *motion, real target, real duration, real ramp_up_time,
	real ramp_down_time, real final_velocity, real initial_velocity)
{
	duration = duration > 0.0f ? duration : 0.0f;
	ramp_up_time = ramp_up_time > 0.0f ? ramp_up_time : 0.0f;
	ramp_down_time = ramp_down_time > 0.0f ? ramp_down_time : 0.0f;
	ramp_up_time = ramp_up_time > duration ? duration : ramp_up_time;

	real remaining = duration - ramp_up_time;
	ramp_down_time = remaining > ramp_down_time ? ramp_down_time : remaining;

	real position = PIN(motion->position, 0.0f, 1.0f);
	target = PIN(target, 0.0f, 1.0f);

	real delta = target - position;
	real distance = delta;

	motion->position = target;
	motion->start = target;
	motion->target = target;
	motion->acceleration_distance = 0.0f;
	motion->deceleration_distance = 0.0f;
	motion->velocity = 0.0f;
	motion->cruise_velocity = 0.0f;
	motion->duration = duration;
	if (!(0.0001f > (real)fabs(duration)))
	{
		distance = (real)fabs(distance);
		if (!(0.000001f > distance))
		{
			real cruise_time = remaining - ramp_down_time;
			real half_ramp_up_time = ramp_up_time * 0.5f;
			real half_ramp_down_time = ramp_down_time * 0.5f;
			real denominator = cruise_time + half_ramp_down_time + half_ramp_up_time;
			real velocity;

			motion->target = target;
			motion->position = position;
			motion->start = position;
			motion->acceleration_distance = 0.0f;
			motion->deceleration_distance = distance;
			motion->velocity = initial_velocity;
			motion->cruise_velocity = 0.0f;
			if (denominator != 0.0f)
			{
				velocity = (delta - half_ramp_up_time * initial_velocity - half_ramp_down_time * final_velocity) /
					denominator;
			}
			else
			{
				velocity = delta / duration;
			}
			motion->cruise_velocity = velocity;
			if (ramp_up_time > 0.0f)
			{
				real acceleration = (velocity - initial_velocity) / ramp_up_time;

				if (acceleration != 0.0f)
				{
					motion->acceleration_distance = (velocity * velocity - initial_velocity * initial_velocity) /
						(acceleration * 2.0f);
				}
			}
			if (ramp_down_time > 0.0f)
			{
				real deceleration = (final_velocity - motion->cruise_velocity) / ramp_down_time;

				if (deceleration != 0.0f)
				{
					motion->deceleration_distance = distance - (final_velocity * final_velocity -
						motion->cruise_velocity * motion->cruise_velocity) / (deceleration * 2.0f);
				}
			}
		}
	}
}

/* starts a device's power motion (if the device takes power changes) */
// @retail 0x108600
void function_108600(long device_index, real target, real duration, real ramp_up_time, real ramp_down_time,
	bool keep_velocity)
{
	if (device_index != NONE)
	{
		s_device *device = (s_device *)function_badc0(device_index, DEVICE_TYPE_MASK);

		if (device && (device->flags & 8))
		{
			device->flags &= ~0x20;
			device_motion_start(&device->motion_14c, target, duration, ramp_up_time, ramp_down_time, 0.0f,
				keep_velocity ? device->motion_14c.velocity : 0.0f);
			function_b7360(device_index);
		}
	}
}

/* starts a device's position motion (if the device takes position changes) */
// @retail 0x108670
void function_108670(long device_index, real target, real duration, real ramp_up_time, real ramp_down_time)
{
	if (device_index != NONE)
	{
		s_device *device = (s_device *)function_badc0(device_index, DEVICE_TYPE_MASK);

		if (device && (device->flags & 0x10))
		{
			device->flags &= ~0x40;
			device_motion_start(&DEVICE_GET(device_index)->motion_16c, target, duration, ramp_up_time,
				ramp_down_time, 0.0f, 0.0f);
			function_b7360(device_index);
		}
	}
}

#include "unknown_11cb00.h"

bool function_bf5a0(long object_index);
bool function_bf5d0(long object_index);
void function_ba350(long object_index, real seconds);
transform4x3f *function_b8bd0(long object_index, short node_index);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void orientation_from_matrix4x3(transform4x3f const *matrix, rigid_transform_scaled *out);
void function_11dbb0(real_quaternion_transform *out, real_quaternion_transform const *a,
	real_quaternion_transform const *b);
real function_30bf0(vector3f *vector);
struct s_location;
void function_b75a0(long object_index, point3f const *point, vector3f const *forward,
	vector3f const *up, s_location const *location, bool unknown);
void __stdcall function_bd020(long object_index);

struct s_device_model
{
	byte unknown00[4];
	long render_model_index;
};

PRIVATE inline real device_translation_distance_squared(point3f const *a, point3f const *b)
{
	vector3f difference;
	difference.i = a->x - b->x;
	difference.j = a->y - b->y;
	difference.k = a->z - b->z;
	return difference.k * difference.k + difference.j * difference.j + difference.i * difference.i;
}

// @retail 0x107ed0
bool __stdcall function_107ed0(long device_index, long name, real seconds)
{
	bool result = false;
	if (device_index != NONE)
	{
		s_device *device = (s_device *)function_badc0(device_index, DEVICE_TYPE_MASK);
		if (device)
		{
			s_device_definition *definition = (s_device_definition *)g_4e3b44[device->definition_index & 0xffff].bytes;
			if (definition->model_tag_index != NONE)
			{
				s_device_model *model = (s_device_model *)g_4e3b44[definition->model_tag_index & 0xffff].bytes;
				if (model->render_model_index != NONE)
				{
					long render_model = (long)g_4e3b44[model->render_model_index & 0xffff].bytes;
					if (function_bf5a0(device_index))
					{
						s_animation_state *state = device_get_animation_state(device);
						bool had_animation = false;
						real speed = 0.0f;
						transform4x3f original;
						transform4x3f adjusted;
						transform4x3f sampled;
						transform4x3f start;
						transform4x3f inverse;
						transform4x3f relative;
						function_1420f0(&original, &device->field_xcc658c, &device->forward, &device->up);
						adjusted = original;
						if (device->flags & 8)
						{
							c_type_709360 animation = device->channels[0].animation_id;
							real time = device->channels[0].frame_position * (1.0f / 30.0f);
							state->animation_matrix_get(animation, 0.0f, render_model, &start);
							state->animation_matrix_get(animation, time, render_model, &sampled);
							long node = state->node_find(0xd000533);
							if (node != NONE)
							{
								adjusted = *function_b8bd0(device_index, (short)node);
							}
							else
							{
								function_141590(&start, &inverse);
								function_142a60(&sampled, &inverse, &relative);
								function_142a60(&adjusted, &relative, &adjusted);
							}
							had_animation = true;
							real step = g_510c54->rate;
							if (step > 0.0f && time - step > 0.0f)
							{
								state->animation_matrix_get(animation, time - step, render_model, &start);
								real distance_squared = device_translation_distance_squared(&sampled.position, &start.position);
								if (distance_squared > 0.0f)
									speed = (real)sqrt(distance_squared) / step;
							}
						}
						device->motion_14c.position = 0.0f;
						device->motion_14c.velocity = 0.0f;
						device->motion_14c.start = 0.0f;
						device->motion_14c.target = 0.0f;
						device->motion_14c.cruise_velocity = 0.0f;
						device->motion_14c.acceleration_distance = 0.0f;
						device->motion_14c.deceleration_distance = 0.0f;
						device->power = 0.0f;
						device->power_velocity = 0.0f;
						if (had_animation && device->parent_index == NONE)
						{
							vector3f forward = adjusted.forward;
							vector3f up = adjusted.up;
							function_30bf0(&forward);
							function_30bf0(&up);
							if (!(fabs(original.position.x - adjusted.position.x) < 0.0001f &&
								fabs(original.position.y - adjusted.position.y) < 0.0001f &&
								fabs(original.position.z - adjusted.position.z) < 0.0001f &&
								fabs(original.forward.i - forward.i) < 0.0001f &&
								fabs(original.forward.j - forward.j) < 0.0001f &&
								fabs(original.forward.k - forward.k) < 0.0001f &&
								fabs(original.up.i - up.i) < 0.0001f &&
								fabs(original.up.j - up.j) < 0.0001f &&
								fabs(original.up.k - up.k) < 0.0001f))
							{
								function_b75a0(device_index, &adjusted.position, &forward, &up, NULL, false);
								if (function_bf5d0(device_index))
								{
									s_device *object = DEVICE_GET(device_index);
									real_quaternion_transform *a = (real_quaternion_transform *)((byte *)object + object->orientation_a_offset);
									real_quaternion_transform *b = (real_quaternion_transform *)((byte *)object + object->orientation_b_offset);
									real_quaternion_transform orientation;
									orientation_from_matrix4x3(&original, (rigid_transform_scaled *)&orientation);
									function_11dbb0(a, &orientation, a);
									function_11dbb0(b, &orientation, b);
									function_1420f0(&relative, &device->field_xcc658c, &device->forward, &device->up);
									function_141590(&relative, &inverse);
									orientation_from_matrix4x3(&inverse, (rigid_transform_scaled *)&orientation);
									function_11dbb0(a, &orientation, a);
									function_11dbb0(b, &orientation, b);
								}
							}
						}
						if (seconds > 0.0f && function_bf5d0(device_index))
							function_ba350(device_index, seconds);
						result = function_1086e0(name, device_index);
						if (!result)
						{
							function_1086e0(0x8000080, device_index);
						}
						else if (speed > 0.0f)
						{
							c_type_709360 animation = device->channels[0].animation_id;
							real duration = device->channels[0].get_duration();
							if (duration > 0.0f)
							{
								state->animation_matrix_get(animation, 0.0f, render_model, &original);
								state->animation_matrix_get(animation, duration, render_model, &start);
								real distance_squared = device_translation_distance_squared(&start.position, &original.position);
								if (distance_squared > 0.0f)
									device->motion_14c.velocity = (real)sqrt(distance_squared) * speed;
							}
						}
						function_b7360(device_index);
						function_bd020(device_index);
						device->flags |= 4;
						if (result)
							device->flags |= 8;
						else
							device->flags &= ~8;
					}
				}
			}
		}
	}
	return result;
}
