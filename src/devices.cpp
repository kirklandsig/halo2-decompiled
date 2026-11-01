// @flags /O2 /arch:SSE /Gr
#include "orientations_storage.h"
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
		bool active = true;

		if ((power_group->flags & 1) && (power_group->flags & 2))
			active = false;
		if (device->flags & 2)
			active = false;
		result = position_group->value != 1.0f ? false : active;
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

/* samples a device's two channels */
PRIVATE __forceinline void device_channels_sample(s_device *device, dword const *node_mask,
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
	s_device_definition *definition = (s_device_definition *)g_4e3b44[device->definition_index & 0xffff].bytes;

	if (definition->model_tag_index != NONE)
	{
		s_device_model_definition *model = (s_device_model_definition *)g_4e3b44[definition->model_tag_index & 0xffff].bytes;

		if (model->render_model_tag_index != NONE)
		{
			void const *volatile render_model = g_4e3b44[model->render_model_tag_index & 0xffff].bytes;
			device_channels_sample(device, node_mask, node_count, transforms);
		}
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
		dword flags = ((placement->flags & 4) | 0x10) >> 2;
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

// score 34 (was 34): the `group_flags & 3` local (lane S) loads the flags as `movzx eax, byte [eax+2];
// and eax, 3` where retail loads `mov ax, [eax+2]` and tests al twice (one extra instruction), and
// retail clears the result in case 0x1200055f (one instruction missing); between the two the code is
// retail's shifted by one. Plain masks plus the zero store leave only the group pointer's register.
// Lane S (34, was 42): masking the group flags into a dword local gives retail's `test al, 1` and
// `test al, 2` with the group pointer in EAX, but the flags are loaded as a byte and masked instead of
// retail's word load (the extra bytes shift every later block), and case 0x1200055f still lacks
// retail's zero store.
// Group-flag load width and velocity zero-store schedule remain.
// Earlier: the group-flag load is the only content difference left in the block: ours is a
// byte load with a mask (`movzx eax, byte ptr [eax+2]; and eax,3`), retail's is a word load
// (`mov ax, word ptr [eax+2]`), and the three extra bytes shift every later block.
// Earlier: masking the group flags into a dword local gives retail's `test al, 1` and
// `test al, 2` with the group pointer in EAX, but the flags are loaded as a byte and masked
// instead of retail's word load, and case 0x1200055f still lacks retail's zero store.
// Earlier: the group-flag test is two bytes longer than retail's: ours shifts the flags byte
// down by one before the second test (`shr al, 1; test al, 1`), retail tests bit 2 directly; retail
// also clears the result in case 0x1200055f where ours does not (`function_value = 0.0f;` first in
// the velocity block is retail's store, but alone it shifts every later instruction: 656 bytes, 114,
// so it stays out until the bit test is solved). Every form whose second test is a condition on the
// flags (masks, shifts in the condition, nested ifs, direct `group->flags` reads as in the matched
// 0x107870) gives retail's `mov ax, [ptr+2]; test al, 1; test al, 2` but keeps the group pointer in
// a shrink-wrapped pushed ESI (live past the load); every form that makes a bool of the second bit
// keeps the pointer in EAX as retail but shifts.
// Tried (with the zero store unless noted): masks 122; a word getter by group index 123; an inline
// group-pointer getter 122; the machine test as a named bool 124; actual signed group flags with
// Boolean/masks/early load/default 43/120/54/122; actual group bitfields with direct/second/both/
// default tests 50/50/50/122; the group flags read at each use 120, with the machine flags first
// 116; the zero store alone 114; a word parameter of an inline both-bits test 120 (diagnostic); a
// bool of both tests 120; nested ifs 120; two bools 42 (no zero store); `(group_flags & 3) == 3` 52;
// the group pointer formed in the flags read 53/53, with the flags read first 117; `== 2` /
// `== 1 && == 2` / a bool from `== 2` 120 each; a bool from the bare mask or its cast 42/42 (no zero
// store); masks on the word local 120; a bool for bit 0 and a mask for bit 1 120; a const group
// pointer with masks 120; no group pointer (the address folded into the load) 115; the pointer
// formed in the read of the word local with masks 53; machine and group flags each read at each use
// 122. The zero store mirrors case 0xf00055e, where MSVC keeps the same redundant copy. Screened
// (one-file LTCG): word/long `flags & 2` locals, `!!`, a ternary, bit-test helpers taking the mask, a
// named group condition before/after the machine test, `||` with the machine test, `!(!a || !b)`,
// the pointer declared after the machine flags, a named `both` or `first` bool, the group read at
// its test, the flags read at the declaration: all the shift or the pushed ESI.
// Round 100: `(group_flags >> 1) & 1` in the condition 122 (test al, 2, pushed ESI); a bool of
// `(group_flags >> 1) & 1` 114 and its (bool) cast in the condition 114 (the shift); a const word
// local with masks 122; the machine flags read before the group pointer, with masks 127.
// down by one before the second test, retail tests bit 2 directly; retail also clears the result in
// case 0x1200055f where ours does not (`function_value = 0.0f;` first in the velocity block gives it,
// 656 bytes 114, kept out until the bit test is solved; it mirrors case 0xf00055e, where MSVC keeps the
// same redundant copy). Every mask form gives retail's `mov ax, [ptr+2]; test al, 1; test al, 2` but
// keeps the group pointer in a pushed, shrink-wrapped esi; every bool form keeps the pointer in eax but
// shifts. Tried with the zero store: masks 122; a word getter by group index 123; an inline
// group-pointer getter 122; the machine test as a named bool 124; machine and group flags each read at
// each use 122. Actual signed group flags with Boolean/masks/early load/default; actual group bitfields
// with direct/second/both/default tests 43/120/54/122/50/50/50/122. The group flags read at each use as
// in the matched 0x107870, 120; the same with the machine flags first 116; a word parameter of an
// inline both-bits test 120 (diagnostic); a bool of both tests 120; nested ifs 120; two bools 42;
// `(group_flags & 3) == 3` 52; the group pointer formed in the flags read 53/53, with the flags read
// first 117; `== 2` / `== 1 && == 2` / a bool from `== 2` 120 each; a bool from the bare mask or its
// cast 42/42; masks on the word local 120; a bool for bit 0 and a mask for bit 1 120; the pointer
// formed in the read of the word local with masks 53; a const group pointer with masks 120; no group
// pointer (the address folded into the load) 115. Screened (one-file LTCG): word/long `flags & 2`
// locals, `!!`, a ternary bool, bit-test helpers taking the mask, a named group condition before/after
// the machine test, `||` with the machine test, `!(!a || !b)`, the pointer declared after the machine
// flags, a named `both` or `first` bool, the group read at its test, the flags read at the
// declaration: all the shift or the pushed esi. Round 120: `!= 0` masks 120; the bool computed inside
// `if (group_flags & 1)` 42 (same code); the group pointer formed after the machine test with direct
// masks 116 (pointer in eax, but the table base then takes a pushed esi: retail's early pointer is
// what leaves edx free for the index); nested direct masks 120; the second test as a ternary inside
// the first 120. Retail's shared `mov ax, [eax + n]; test al, 1; test al, 2` appears in no matched
// function (only in unmatched 0x2c0d60; matched 0x1bdd70 loads into dx and keeps the pointer).
// Round 3 (lane 17): a file-local inline `(group->flags & 1) && (group->flags & 2)` test on the group
// pointer 120, with a result variable 120, with a word local inside 120; the result set by a ternary
// on the bool form 42; a const group pointer with the bool form 42; `second` declared beside the
// flags and assigned later 42. In the mask forms the pointer is dead after the flags load (esi is
// popped after the first test), so it is a coalescing choice, not a longer live range.
// Round 101 (lane 8; exact LTCG harness through a function-pointer caller, harness 32 = 34): plain
// masks on the word local plus `function_value = 0.0f;` first in the 0x1200055f velocity block leave
// only the group pointer's register (`push esi; lea esi` / `mov ax, [esi+2]` / `pop esi` against
// retail's `lea eax` and `mov ax, [eax+2]`), 656 bytes, 106 positional: the 16-bit flags value never
// shares EAX with the dying pointer. On that base: no pointer local 113, direct field reads 106, the
// word local declared at its read 106, the flags read before the machine test 114, the machine flags
// read at each use 106, nested tests 106, the power test's operands swapped 106 (the pushed ESI in
// each). On the lane S form: a byte/word bits local 32/32 (harness); dword/long/int/short flags locals
// with masks 105 each (pushed ESI); the zero store added 105.
// Round 456: Success constructed in recognized cases, at tail/entry, crossed with native group reference and explicit velocity zero:
// 128/124/128/124/184/185/184/185. Original executable body and supporting declarations restored.
// Round 119 (with the zero store unless noted): word-local masks 122; the group pointer formed after the machine
// test 123 (the pointer folds into the flags load, the table base takes a pushed ESI); a group-index local 122;
// nested word tests 122; the 0x1200055f arms as ternaries with a 0.0f false value, no zero store 104.
// Compile-only screen (non-LTCG, reproduces the listing; jump targets masked, base 3, word masks with the zero
// store 4): pointer before/after the machine flags or const, word/short/long/word-at-read/direct/nested flag
// reads, machine flags local or read at each use, with or without a group-index local: all 72 keep the pushed
// ESI (4). Without the machine test the table base takes ESI instead; ternary or explicit `else 0.0f` arms
// 8/10. Matched 0x2101d0 shares EAX between a live-in pointer and a 16-bit value only needing a 16-bit
// register; here `test al` needs a byte register.
// Round 33 (lane 13; one-file LTCG harness, base 32 = 34): on the masks + zero-store form (106, the
// pointer in a pushed ESI): type and group-index tests nested 106; a long machine flags local 106; the
// value declared before the result 108; the machine test as a ternary 106; the group index in a long
// local inside the type test 106; a `word const &` flags reference 106; the group pointer or the word
// flags declared at function scope 106/106; on the lane S form with the zero store, word/byte bits
// locals 105/105 (a byte bits local loads `mov al, [eax+2]` into the pointer's EAX: only the 16-bit
// load refuses it). Diagnostics: no machine test 108 (the table base then takes ESI), the machine
// test after the group test 113. Matched 0x1bb1d0 and 0x1c7450 do load 16 bits into a pointer's own
// register across a branch, so the conflict is not the load width alone.
// Round 13 (lane 18; compile-only /FAs screen reproduces both 34 and the pushed ESI): with the masks and the
// zero store the 16-bit flags value takes EAX before the group pointer, and the pointer, whose last use is
// that load, then avoids EAX. Any third reference to the pointer (a compare of it or of a field, before
// or after the machine test, even inside its arm) makes the pointer win and gives retail's `lea eax` /
// `mov ax, [eax+2]` with no pushed ESI; a pointer loaded from memory or `data + constant` also shares
// EAX, so the two-reference `lea [base+index*4]` web is what loses. Names (20 pairs), a named table base
// with `group += index`, a field pointer, a `const &` group, a byte offset, an index local, an inline
// both-bits helper and a 16-bit bitfield view all keep ESI. Real (masks + zero store): `const &` group,
// index local with typed pointer arithmetic, inline both-bits helper, bitfield view, byte-offset pointer
// 122 each (656 bytes). A second test through a re-derived group pointer gives EAX but two memory tests.
// Round 41 (lane 11, fast; on the word-local masks + 0x1200055f zero store, 122): the group pointer named
// for the power group 122; the flags local named `flags` 122 (names never move the pushed ESI); a named
// delay-time local in case 0x5000561 131; a named position-speed local in case 0xf00055e 136; a named
// cruise-speed local in case 0x1200055f 89 (657 bytes); `(machine_flags & 0xd) | (both bits)` 135;
// `else if` for the group test 23 (654 bytes; identical arms, not kept); `(machine_flags & 0xd) ||
// (both bits)` 23 (654 bytes), the same with direct field reads 111. The two 23 forms are not retail's
// control flow (retail's machine-set movaps falls into the group test; they skip it) and only fit the
// length because the block shrinks by the zero store's size: there the table base and index are
// computed after the machine flags load and the word load folds the address (`mov ax, [esi + eax*4 +
// 2]`), so the 16-bit load shares EAX only once the address computation sinks below the machine test.
// Round 46 (m3 lane 10, fast): the machine test as `(flags & 1) || (flags & 4) || (flags & 8)` on the lane S
// form 34 (same code), on word-local masks + the 0x1200055f zero store 122; `function_value = 0.0f;` at the
// top of both 0x1200055f arms (hoisted above the `je` exactly like the single zero store) on the lane S form
// 117 (657 bytes), on word-local masks 122; a byte machine-flags local with masks + zero store 122 (the
// pointer still takes the pushed ESI, popped right after `test al, 1`).
// Round 14 (lane 17, fast; on the masks + 0x1200055f zero-store form, 122): the case labels in numeric
// order, in retail's block layout order and with the 0x6000560 case first 122/122/122; the machine block
// behind an early `break` 122; a const group reference with direct flag masks 122 (all the pushed ESI).
// Matched retail sites that load 16 bits into the pointer's own register (0x3f500, 0x1c7450, 0x1023d0)
// use the value in 16-bit compares or a phi, never in byte tests.
// Round 276: complete group value before/after the machine test, masked bits/word
// masks without/with velocity zero 199/203/192/199 and 204/207/200/204.
// Original pointer and values restored; word-load/EAX sharing and zero move remain.
// Round 127 (lane 5, fast): word-local masks + the 0x1200055f zero store re-measured 122 (pushed ESI);
// `__assume(group != NULL)` after the lookup on it 122 and on the lane S form 34 (same code: it is no
// third reference); the lane S bits masked straight from the field 34 (same code). Matched 0x107870
// loads the same two bits with `mov ax, [ecx+2]` into another register than its pointer (no lead).
// Round 48 (lane 10, fast): on the lane S form, the bits local as `word` 34 (same code), with the
// 0x1200055f zero store 117 (657 bytes; the listing is then retail's except the 3-byte longer
// `movzx eax, byte [eax+2]; and eax, 3` for retail's `mov ax, [eax+2]`, so the score is a shift);
// the zero store alone re-measured 117; the bits local `dword` without the `& 3` plus the zero store 122
// (a full `movzx eax, word [esi+2]` and still the pushed ESI, so the conflict is not the 16-bit width);
// `group->flags & 3` read directly without the word local 34.
// Round 21 (lane 17; compile-only /FAs screen of the file reproduces the listing): on masks + the zero store
// (122), a two-byte bitfield struct copy tested with `&&` 122, bitfields through a struct-pointer cast of the
// field 122, a `word const *` to the group's flags 122, a `short` flags local 122, `register` on the group
// pointer 122 (all the pushed ESI); screens only: `register` on the flags or machine flags, `!= 0` masks, the
// flags word at its read, machine flags first (ESI); the bitfield copy tested with `&` frees EAX only by
// spilling the copy to the stack. 1-7 dead temporaries (diagnostic) leave both forms unchanged.
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
			dword group_flag_bits = group_flags & 3;
			if ((group_flag_bits & 1) && (group_flag_bits & 2))
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
		if (device->position_velocity != 0.0f)
		{
			function_value = 0.0f;
			if (definition->position_speed != 0.0f)
				function_value = (real)fabs(device->position_velocity) / definition->position_speed;
		}
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

bool __stdcall function_106790(long volatile device_index);

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
	bool (__stdcall *handler40)(long);
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
	function_106790,
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
			result = state->overlay_play_for_class(channel, 0x40, 0x7000101, 0x7000101, name);
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
// score 121 (was 121): Initial velocity remains stack-passed instead of XMM6; ramp-down residency and return size differ.
// Earlier: retail keeps the initial velocity in xmm6 for the whole function and leaves
// the ramp-down time in its stack slot; ours keeps the ramp-down time in xmm1 through the clamps
// and loads the initial velocity late, so the argument list and the register use still differ.
// Earlier: retail passes the initial velocity in xmm6 (one register for its whole
// life, so ret 0x14) and keeps the ramp-down time in its stack slot throughout; ours keeps the
// ramp-down time in xmm1 through the clamps and loads the initial velocity late (ret 0x18).
// The cruise velocity stored into the motion in each branch (no velocity local) gives retail's
// acceleration and deceleration blocks exactly, but alone 134. Tried (with both callers): that
// tail with the position pinned after the target 102 (target then passed in a register; 0x108670
// 13 -> 28); cruise time first, then the stores, then the halves 116 (ramp-down then passed in a
// register); stores before the cruise and half times 144; if-statement clamps, no remaining local,
// fresh distance, every order of the denominator terms and the initial velocity at every parameter
// position never give the initial velocity a register (about 2000 forms screened).
// incoming velocity remains on the stack rather than XMM6;
// duration/ramp-down allocation and the 0x18/0x14 return gap remain. Earlier half-time/store
// sweeps best 121; clamp helpers 123; clamped locals 122; coordinated parameter placements 121-124;
// acceleration-field pointer store 131; early initial-speed square 142. External inline/noinline/both
// 121/121/121; file-local plain/inline/noinline 121/121/121; shared signed phase acceleration 121;
// shared acceleration with file-local inline/noinline 121. Original routine and both calls restored.
// initial velocity remains on the stack while ramp-down time occupies a register.
// Earlier: retail passes the initial velocity in xmm6 with five stack floats and
// returns 0x14; ours passes it on the stack (ret 0x18), and the clamped times land in other xmm
// registers.
// Isolated ramp-down zero/min/both if-clamps 121/121/121; const initial velocity 121;
// all-read/all-read-write ramp-down references 121/130; shared velocity with stored initial 125;
// entry velocity copy plus reused parameter 152. Original body restored.
// Round 3 (lane 17): LTCG passes a float in XMM only when the whole parameter stays in one register;
// a volatile ramp-down (diagnostic, removed) gives ret 0x14 with the initial velocity in a register, so
// retail spills the ramp-down entirely at the clamp/pin peak where ours spills the initial velocity.
// The min clamp into a named local, motion-field tail 109 (511 bytes, the ramp-down then the register
// parameter; 0x108670 13 -> 28); that local with the velocity tail 143; both clamps into it 134/144;
// parameter renames 121; early returns 121/134; `fabs(duration) >= 0.0001f` and `>` 132/142;
// pointer-taking inline clamps 122/125; MIN-macro upper clamps 121/134; the initial velocity read back
// from its motion field 121/134; the inner stores first 144/134 (the ramp-down in xmm7, ret 0x14);
// the initial-velocity store first 131/140.
// Round 106: retail keeps the ramp-down time only in its stack slot and gives XMM6 to the initial
// velocity (XMM register count is the trade); the ramp-down minimum with the time on the left 121;
// the cruise velocity through the motion field (no velocity local) 134 (aligned 53 against 72: retail's
// acceleration/deceleration blocks); the deceleration through the velocity local 121 (same code);
// constant-first half times 121 (same code); cruise time as duration - ramp-up - ramp-down 123.
// Round 2 of lane 18, on the per-branch cruise-velocity store (134, exact tail blocks): no remaining
// local 134; remaining clamp as an if 134; up-time clamps before the down-time zero clamp 134; no delta
// local 163; fresh pinned-target local 134; fresh down-time local 144 (callers 26/28); initial before
// final velocity 134 (caller 33); fresh locals for all four clamped values 110, but it passes the
// duration and ramp times in xmm1-3 (ret 0xc) and 0x108670 13 -> 42. That last form shows LTCG passes
// an unmodified float in a register only when the callee holds it from entry: retail spends no
// register on the ramp-down time in the clamp phase, which frees xmm6 for the initial velocity.
// Round 22 (lane 13): both doubling factors constant-first (`2.0f * acceleration`) 121; the ramp-down
// zero clamp first 121; on the per-branch cruise-velocity form (134, which also hoists retail's 2.0
// into xmm3 and gives its tail, with the initial velocity loaded late into xmm5 and the distance in
// xmm6) the initial-velocity store first in the branch 134; the delta and distance locals after the
// reset stores 121; the target pinned before the position 141 (507 bytes, target then passed in a
// register; 0x108670 13 -> 28).
// Round 456: Parallel half-ramp distance contributions, phase velocity changes and squared-speed differences named at their producers, seven subsets and all with field-owned cruise velocity:
// 121/121/121/121/121/121/121/134. Original executable body and supporting declarations restored.
// Round 33 (lane 13; LTCG harness with both callers, listing equal to the full build's): ours keeps the
// ramp-down time in xmm1 from its zero clamp to the half-time product, retail only in its stack slot.
// A `real const *` to the ramp-down time read once at the zero clamp, the min clamp, the cruise time,
// the half time or the deceleration test, and `(void)&` of it: 121 each (identical code, the address
// folds away); the inner block's locals, or position/delta/distance, or both, declared uninitialised
// at the top: 121 each (identical code).
// Round 13 (lane 18, fast with both callers, which stay 31/13): the up-time limit as an if 121 (same
// code); the per-branch cruise-velocity store re-measured 134 (aligned 82 against 135 rows: retail's
// acceleration/deceleration blocks, the ramp-down still enregistered and the xmm colours rotated), with the
// up-time if 134, with the ramp-down limit as `if (!(remaining > ramp_down_time))` 134; both ramp-down
// clamps as negated ifs on this body 121 (same code); the per-branch store with the cruise velocity read
// back once into the velocity local for the acceleration block 137 (527 bytes).
// Round 41 (lane 11, fast, with both callers): retail's clamp/pin phase keeps the ramp-down time only in its
// stack slot and uses seven xmm registers (duration 5, ramp-up 7, remaining 1, one 4, position 3, target 2,
// zero 0), leaving xmm6 free for the initial velocity; ours holds the ramp-down in xmm1 and fills all eight.
// The ramp-up clamped to the duration before the ramp-down zero clamp 123; the initial velocity stored first
// in the inner block 122; the half ramp-up time named before the half ramp-down 123; the cruise time named
// right after the remaining-time clamp 140 (the remaining time dies early, a `push ecx` frame, 0x108600 31 ->
// 26, 0x108670 13 -> 28; still ret 0x18); the motion's duration stored first among the reset fields 121.
// Round 46 (m3 lane 10, fast, with both callers): `distance = target - position` with delta copied from it
// 123; the denominator staged with two `+=` 121; the velocity staged (difference, then `/= denominator`) 121;
// both delta swap and staged velocity 123; the reset stores as two chained assignments 121 (identical code).
// Round 14 (lane 17, fast, with both callers): the deceleration through the velocity local and the
// acceleration through the motion field 121 (same code); the three target stores chained 121; the half
// ramp-down time computed before the position pin 129; the denominator written out in its test and
// division (no local) 136 (525 bytes); the velocity computed before the inner block's stores 136 (the
// target, duration and ramp-up then passed in registers, ret 0x10; 0x108600 31 -> 19 at ret 0xc,
// 0x108670 13 -> 34).
// Round 276: complete native motion result published at the tail, declared at reset/entry,
// with local/field cruise speed and up/down phase order 173/174/173/174 each.
// Original field stores restored; frameless initial-speed/ramp-down ownership remains.
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
			real half_ramp_down_time = ramp_down_time * 0.5f;
			real half_ramp_up_time = ramp_up_time * 0.5f;
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
// score 31 (was 31): retail passes the initial velocity to 0x107ab0 in xmm6 with five
// stack floats; our 0x107ab0 takes it on the stack, so its call pushes one argument more.
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

// score 2 (was 2): the float argument is stored to the stack before edi is pushed; retail pushes edi first and
// stores the float one slot higher. Round 23: had_animation set after the time / first in the block 43/44;
// the animation as a const reference 203; sampled declared after the time 2; time before the animation 2.
// Round 30: sampled at function scope 39; the animation direct-initialised 2; time declared beside speed and
// assigned in the block 2; time computed after the first sample call 20; had_animation set before the node
// lookup 31. Round 99: existing vector subtraction/length helpers 2; seconds divided by 30 2;
// sampled renamed 2; start renamed 2; an independent duration matrix 13; step declared beside speed 2;
// a forceinline distance helper 2; one shared distance local 33. All restored.
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
						transform4x3f start;
						function_1420f0(&original, &device->field_xcc658c, &device->forward, &device->up);
						adjusted = original;
						if (device->flags & 8)
						{
							transform4x3f sampled;
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
								transform4x3f inverse;
								transform4x3f relative;
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
									real_quaternion_transform *b = (real_quaternion_transform *)((byte *)object + object->orientation_b_offset);
									real_quaternion_transform *a = (real_quaternion_transform *)((byte *)object + object->orientation_a_offset);
									real_quaternion_transform orientation;
									orientation_from_matrix4x3(&original, (rigid_transform_scaled *)&orientation);
									function_11dbb0(b, &orientation, b);
									function_11dbb0(a, &orientation, a);
									transform4x3f current;
									transform4x3f current_inverse;
									function_1420f0(&current, &device->field_xcc658c, &device->forward, &device->up);
									function_141590(&current, &current_inverse);
									orientation_from_matrix4x3(&current_inverse, (rigid_transform_scaled *)&orientation);
									function_11dbb0(b, &orientation, b);
									function_11dbb0(a, &orientation, a);
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

void __stdcall function_bf600(long user, real frame, s_animation_frame_event const *event);

PRIVATE inline long device_motion_sign(real value)
{
	return value != 0.0f ? (value < 0.0f ? -1 : 1) : 0;
}

// @retail 0x107cc0
bool function_107cc0(long device_index, s_device_motion *motion, c_animation_channel *channel,
	s_animation_state *state, real position, real *out_position)
{
	s_device *device = DEVICE_GET(device_index);
	bool result = false;
	real current = position;
	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		real remaining = motion->target - motion->position;
		real distance = (real)fabs(motion->position - motion->start);
		real absolute_remaining = (real)fabs(remaining);
		if (!(absolute_remaining < 0.000001f))
		{
			real step = g_510c54->rate;
			if (distance < motion->acceleration_distance)
			{
				distance = motion->acceleration_distance - distance;
				if (!(fabs(distance) < 0.000001f))
					motion->velocity += (motion->cruise_velocity * motion->cruise_velocity - motion->velocity * motion->velocity) / (2.0f * distance) * step;
			}
			else if (distance > motion->deceleration_distance)
			{
				*(volatile dword *)&distance = *(dword *)&absolute_remaining;
				if (!(fabs(distance) < 0.000001f))
					motion->velocity += (0.0f - motion->velocity * motion->velocity / (2.0f * distance)) * step;
			}
			else
				motion->velocity = motion->cruise_velocity;
			motion->position += motion->velocity * step;
			if (device_motion_sign(motion->target - motion->position) != device_motion_sign(remaining))
			{
				motion->position = motion->target;
				result = true;
			}
		}
		else
		{
			motion->position = motion->target;
			result = true;
		}
		motion->position = DEVICE_PIN(motion->position, 0.0f, 1.0f);
		current = motion->position;
		channel->set_frame_ratio_and_advance(current, state, function_bf600, device_index);
		device->flags |= 4;
	}
	if (out_position)
		*out_position = current;
	return result;
}

bool function_107cc0(long device_index, s_device_motion *motion, c_animation_channel *channel,
	s_animation_state *state, real position, real *out_position);
bool function_11eed0(real *position, real *velocity, real target, real time_step, real speed,
	real acceleration, bool wrap, real lower, real upper);
void __stdcall function_bd020(long object_index);
void machine_keyframe_rigid_bodies(long object_index, bool value);
struct s_index_pair
{
	long index;
	byte field_4[2];
	short index2;
};
bool function_0b6760(s_index_pair const *pair);

// score 294 (was 294): our frame is 0x1c and retail's 0x20, and the index, header, state and result
// homes still differ; the group path reloads the definition pointer from three base registers.
// Earlier (294, was 294): our frame is 0x1c and retail's 0x20; the index, header, state and result
// homes still differ, and the group path reloads the definition pointer from three base registers.
// Earlier (294, was 294): Frame 0x1c versus 0x20 and index/header/state/result homes remain; the
// group path still reloads the definition pointer from three base registers and adds the two
// products in the opposite order.
// Earlier: Frame 0x1c versus 0x20 and index/header/state/result homes remain; native
// power/position/both/activity flag values 376/302/379/370; power-activity/position-
// activity/all/shared 376/373/376/376. One-point gain rejected.
// Earlier: Frame 0x1c versus 0x20; index/header and state/result homes remain.
// Frame 0x1c versus 0x20; index/header and state/result homes remain.
// Frame 0x1c versus 0x20; index/header/state/result homes remain. Ordinary
// index 389; ordinary saved pointer 378; both 421; ordinary index with header ordinal 389; both
// with ordinal 421; both with byte result 413; ordinary saved pointer/index with byte result
// 386/385. Acceleration producers preserved.
// Frame 0x1c versus 0x20; index/header and state/result homes remain.
// Acceleration products 303; speed products 316; both pairs 341; both plus complement 341;
// delay product 310; acceleration plus delay 303; both plus delay 341; ordinary receiver plus both 378;
// position/remaining product names 303, plain 303 kept. Sum grouping, fields, calls and existing qualifiers
// unchanged.
// frame 0x1c versus 0x20; index/header and state/result homes differ.
// Long direction flag 367; shared completion 365; shared previous value 361;
// channel guard helper 382; ordinary result 355; shared previous/ordinary result 351 kept.
// Round 290: ordinary index with shared previous 408; shared previous and header index 408; header index
// and ordinary tag pointer 384; group path first 392; byte direction predicate 351.
// Frame 0x1c versus 0x20; index/header and state/result homes differ. In-place
// power output 377; isolated rate position 371; speed first 352; integration-only direction 383;
// conditional activity-bit publication 347 kept (plain).
// Round 303: entry position output 351; previous-value name 351; early previous velocity 373;
// changing-power arm first 351; early activity result 418. Frame/index/state homes still differ.
// Ordinary saved pointer with shared previous 381 at retail length; original qualifiers kept.
// frame 0x1c versus 0x20 and index/state homes remain; byte result 351;
// word result 358; shared position complement 347; common group receiver 347; clamp arms 352.
// Native device/power-group/position-group record references and their pairs/all 303
// (seven forms); real tag fields 351. All receivers and the original tag type restored.
// Motion-helper inline/noinline 303/303; named derivative numerator 303; direct second completion test 303; both 303; named wait predicate 303; outer direction storage 303; previous-velocity name 303. Frame 0x1c versus 0x20 and index/header/state/result homes remain; all helper declarations restored.
// Round 148: Explicit absent-state else 309; false result after flag clear 302; both joins 308; zero-first
// direction/activity 303/303; state else plus direction 309; later false result plus activity 302; all
// joins/zero tests 308. One-point gains dropped; frame/index/header/state/result homes remain.
// Motion-helper inline/noinline 303/303; named derivative numerator 303; direct second completion test 303;
// both 303; named wait predicate 303; outer direction storage 303; previous-velocity name 303. Frame 0x1c
// versus 0x20 and index/header/state/result homes remain; all helper declarations restored.
// Long activity result/entry-long/ordinary index/ordinary saved pointer/both/header ordinal/both with
// ordinal/shared notification 307,305,380,378,407,316,421,375. Original frame/index/state/result storage and
// qualifiers restored.
// Separate power/position activity bool/long 443/451; phase-local position 373/448;
// with shared motion completion 443/451, phase-local position 373/448.
// Original single activity result and completion lifetimes restored.
// Round 350: selected reached/reversal/both event values 336/336/351; native power-channel
// admission 385; with reached/reversal/both value selections 379/378/340; all with native
// repeated channel receiver 347. Frame 0x1c/0x20 and index/header/state/result homes remain.
// Round 96: retail computes (index & 0xffff) * 12 once at entry, keeps it in EBX and its own frame slot
// (the extra dword) and reuses it for the final header and datum reads, so its index is not volatile;
// it also re-reads the animation-state offset after the result/state stores. Plain index 389; plain
// index and definition 421; plain index with the result declared first 394; plain index with a
// ternary state 416; result declared first 301 (two-point gain dropped).
// Round 230: existing state-admission helper with independent tag-pointer name/result-first
// homes 400/400/400/400; the same four with ordinary index 369/369/370/370. Original
// admission, index, declarations and scalar homes restored.
// Round 352: next-tick value, elapsed product and both 303; one shared previous power velocity
// alone and with next-tick/elapsed/both 368; all with long next-tick 365. Frame remains 0x1c;
// original delay, velocity observations, acceleration producers and inherited qualifiers restored.
// Round 23: distinct phase/block/channel previous owners 307/307/303; short first/second/both
// completion 387/335/408; phase plus first short 388; channel plus both shorts 408.
// Frame stays 0x1c versus 0x20; original scalar lifetimes and bool completions restored.
// Round 239: actual power/position/both field references 386/391/373;
// velocity reference 396, with power/position/both 401/396/414; all references
// with result initialized before lookup 414. Frame and index/state/result homes
// remain; original field observations, producers, qualifiers and body restored.
// Round 84 (lane 5): event ids as a select between two field reads (retail's separate loads)
// 351; with a plain index 361 (frame 0x20, the index * 12 kept in a register as retail, 394
// instructions as retail, but index/offset in EDI/ESI where retail has ESI/EBX and the result in
// a register where retail keeps its byte); also a plain definition 410; with the result declared
// first 366, the state before the result 361, the state second 360.
// Round 390: Complete first-motion/integration/second-motion byte-or-long completion
// carrier domains, all eight combinations: 294/383/386/414/324/402/392/413. Original bool carriers restored.
// Long integration gives frame 0x20 but worsens length and register allocation.
// Round 190: shared motion completion in bool/byte/short/long, before/after activity by name,
// 294/294/406/402 each. Refresh/event ordinary-inline-noinline pairs 294 each;
// the refresh-inline forms also lose two matched callers. Original body and declarations restored.
// Round 285: actual repeated power-target/position-target/both/delay/power-delay/position-delay/
// all field lvalues, then all with result-first storage: 296/295/297/296/298/297/299/299 fast.
// Original complete body, observations and inherited qualifiers restored; frame/index/state homes remain.
// Round 424: actual power/position/both group target snapshots 351/389/368;
// each with ordinary index 312/349/392; both with ordinary saved pointer,
// without/with ordinary index 388/425. All restored; frame/index/header/state homes remain.
// Round 229: complete tag-table observations with device/entry-key reads, inherited/ordinary
// index and ordinary/result-first lifetime, all eight 442/370/374/412/442/370/374/412.
// Original saved pointer, qualifiers and complete body restored.
// Round 33 (lane 13; closure LTCG harness, base listing equal to the full build's, positional 260 =
// 294): plain index 372 (1478 bytes, frame 0x20); plain index with both event ids as selects between
// two field reads 353 (1488 bytes, retail's 394 instructions; the callee-saved registers permuted:
// group pointer EBX, velocity pointer ESI, index EDI, where retail has ESI, EBX, ESI, and the result in
// DL where retail keeps a stack byte); on that, the result declared first 353, the state test before
// the result and definition 352, the definition before the state and result 348. Retail compares the
// state offset in memory and re-reads it with movsx (no CSE); ours loads it once.
// Round 41 (lane 11, fast): plain index 395 (1478 bytes); plain index with both event ids as selects between
// two field reads 367 (1488 bytes; aligned with registers masked 69 rows against this body's 120: what is
// left is the callee-saved permutation, the result in DL where retail keeps a stack byte, the state offset
// loaded once, the index * 12 spill); on that, the closing block setting `result = true` before its calls
// and falling through to the one `return result;` 379 (1495 bytes, retail's length, result still in DL);
// on this body the same 303 (1510 bytes); the b3 byte tested `<= 0` 294 (retail's `ja`, same count); the
// state as one conditional expression 300 (1512 bytes). Retail's result byte never crosses a call on any
// path, so its stack home is not register pressure from a live range.
// Round 276: actual header/device byte members 294/294; with ordinary index,
// header/device/both 395/395/395; with ordinary saved pointer 386/386; both
// with ordinary index and saved pointer 412. All declarations and storage restored.
// @retail 0x106790
bool __stdcall function_106790(long volatile device_index)
{
	s_device *device = DEVICE_GET(device_index);
	bool result = false;
	byte const *volatile saved_definition = g_4e3b44[device->definition_index & 0xffff].bytes;
	real previous;
	s_animation_state *state = NULL;
	if (device->animation_state_offset != NONE)
		state = device_get_animation_state(device);
	device->flags &= ~0x100;
	if (device->flags & 8)
	{
		if (!(device->flags & 0x20))
		{
			previous = device->power;
			real position;
			bool finished = function_107cc0(device_index, &device->motion_14c,
				&device->channels[0], state, previous, &position);
			device->power = position;
			device->power_velocity = (position - previous) / g_510c54->rate;
			if (finished)
			{
				device->flags |= 0x20;
				function_107520(device_index);
			}
			else
			{
				device->flags &= ~0x20;
				function_107520(device_index);
			}
			result = true;
		}
	}
	else if (device->power_group_index != NONE)
	{
		s_device_group *group = DEVICE_GROUP_GET(device->power_group_index);
		if (group->value == device->power && device->power_velocity == 0.0f && !(device->flags & 0x80))
		{
			device->delay_ticks = 0;
		}
		else
		{
			bool power_channel_animates = false;
			if (state)
				power_channel_animates = device->channels[0].graph_tag_index != NONE &&
					device->channels[0].animation_id.index != NONE;
			if (power_channel_animates)
				device->channels[0].set_frame_ratio(device->power);
			real position_acceleration = *(real const *)(saved_definition + 0xcc) * device->position;
			real remaining_acceleration = *(real const *)(saved_definition + 0xd4) * (1.0f - device->position);
			real acceleration = position_acceleration + remaining_acceleration;
			real speed = *(real const *)(saved_definition + 0xc8) * device->position +
				*(real const *)(saved_definition + 0xd0) * (1.0f - device->position);
			bool increasing = device->power_velocity > 0.0f;
			device->flags = acceleration > 0.0f && speed > 0.0f ?
				device->flags | 0x100 : device->flags & ~0x100;
			if (!((real)device->delay_ticks * g_510c54->rate >= *(real const *)(saved_definition + 0x10c)) &&
				device->power == 0.0f && !(device->power > group->value))
			{
				++device->delay_ticks;
				if (device->delay_ticks == 1)
					function_107980(device_index, *(long const *)(saved_definition + 0x114));
			}
			else
			{
				real local_c01284_2 = device->power_velocity;
				previous = device->power;
				if (fabs(local_c01284_2) > speed)
					device->power_velocity = increasing ? speed : 0.0f - speed;
				bool reached = function_11eed0(&device->power, &device->power_velocity,
					group->value, g_510c54->rate, speed, acceleration,
					(*(byte const *)(saved_definition + 0xbc) & 1) != 0, 0.0f, 1.0f);
				if (reached)
					function_107980(device_index, *(long const *)(saved_definition + (increasing ? 0xf0 : 0xf8)));
				else if (device->power_velocity != 0.0f && device->power_velocity * local_c01284_2 <= 0.0f)
					function_107980(device_index, *(long const *)(saved_definition +
						(device->power_velocity > local_c01284_2 ? 0xe0 : 0xe8)));
				if (previous != device->power || (device->flags & 0x80))
				{
					function_107520(device_index);
					if (state && device->channels[0].graph_tag_index != NONE && device->channels[0].animation_id.index != NONE)
						device->channels[0].set_frame_ratio_and_advance(device->power, state,
							(animation_event_callback)function_bf600, device_index);
				}
			}
			result = true;
		}
	}
	if (device->flags & 0x10)
	{
		if (!(device->flags & 0x40))
		{
			bool finished = function_107cc0(device_index, &device->motion_16c,
				&device->channels[1], state, 0.0f, NULL);
			if (finished)
				device->flags |= 0x40;
			else
				device->flags &= ~0x40;
			function_107520(device_index);
			result = true;
		}
	}
	else if (!(device->flags & 8) && device->position_group_index != NONE)
	{
		s_device_group *group = DEVICE_GROUP_GET(device->position_group_index);
		if (group->value != device->position || device->position_velocity != 0.0f)
		{
			if (state && function_0b6760((s_index_pair const *)&device->channels[1]))
				device->channels[1].set_frame_ratio(device->position);
			previous = device->position;
			function_11eed0(&device->position, &device->position_velocity, group->value, g_510c54->rate,
				*(real const *)(saved_definition + 0xc0), *(real const *)(saved_definition + 0xc4), false, 0.0f, 1.0f);
			if (previous != device->position)
			{
				function_107520(device_index);
				if (state && function_0b6760((s_index_pair const *)&device->channels[1]))
					device->channels[1].set_frame_ratio_and_advance(device->position, state,
						(animation_event_callback)function_bf600, device_index);
			}
			result = true;
		}
	}
	if (((device->flags & 8) || device->power_group_index != NONE) && (device->flags & 0x80))
	{
		if ((1 << ((byte *)g_4e0300->data)[(device_index & 0xffff) * 12 + 3]) & 0x80)
		{
			function_bd020(device_index);
			machine_keyframe_rigid_bodies(device_index, true);
		}
		if (device_index == NONE || *((byte *)DEVICE_GET(device_index) + 0xb3) == 0)
			device->flags &= ~0x80;
		return true;
	}
	return result;
}
