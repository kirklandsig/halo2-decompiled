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

// score 2 (was 2): the group's flags still load through the combined address
// (`mov ax, [eax + edx*4 + 2]`) where retail computes the address first
// (`lea eax, [eax + edx*4]`, then `mov ax, [eax + 2]`), and ours still masks the byte
// with 3, an instruction retail does not have; between the two the code is retail's
// shifted by one. Plain masks plus the zero store leave only the group pointer's register.
// MiniMax E (11-11): no new form, no lower score (34); retail loads the group's flags as a word sharing the group
// pointer's EAX (mov ax, [eax+2]) and tests bit 1 then bit 2, ours loads a byte and masks it with 3, and dropping
// the mask gives retail's word load but moves the group pointer into a pushed ESI; case 0x1200055f's zero store is
// still missing.
// Earlier (34, was 34): group-word load/EAX sharing and velocity-zero join remain;
// the `group_flags & 3` local (lane S) loads the flags as `movzx eax, byte [eax+2];
// and eax, 3` where retail loads `mov ax, [eax+2]` and tests al twice (one extra instruction), and
// retail clears the result in case 0x1200055f (one instruction missing); between the two the code is
// retail's shifted by one. Plain masks plus the zero store leave only the group pointer's register.
// MiniMax D (11-11): no new form, no lower score (34); the group's flags have to reach retail's shape (a word
// load into AX with the element address still in EAX, tested twice on AL) but VC splits the two: a computed
// flags value frees the address into EAX and narrows the load to a byte with a mask (115), while a plain word
// copy keeps retail's word load and pushes the address into ESI (122). The address alone never gives the lea
// plus the word load together - as an index variable with the pool base in a local (53), as a word pointer to
// the element (120), as an integer address (120) or read through the macro twice (122). Reading the flags
// after the machine test gives retail's delayed load but the address still lands in ESI, and reading them
// before it gives the combined address (117); the zero store of case 0x1200055f on top of the committed body
// is 117, not a gain. A byte cast widened back to a dword to keep the whole word is 120, so it is no better
// than the byte load with its mask.
// MiniMax S (11-11): 2 (was 34) with the group's flags read through the device-group macro into a
// word before the machine test and masked into a byte after it, plus the zero store at the head of
// case 0x1200055f's velocity block; left are retail's `lea eax, [eax + edx*4]` / `mov ax, [eax + 2]`
// against our folded `mov ax, [eax + edx*4 + 2]` and the extra `and al, 3`. Tried on top of it, all
// worse and all reverted: the group as a named pointer with the flags read after the machine test 122
// (the shape becomes retail's - the address computed before the machine test, the word loaded at the
// join - but the pointer lands in ESI with a push and a pop where retail keeps the address in EAX),
// the same through a reference 122, the same with the word read into a named local 122, the element
// address in a dword local with the word read through it 122, the flags read at each use after the
// machine test 123 and the flags read in both arms of the machine test 123 (the group base is then
// loaded late and the index is folded into the load); the permuter was started over the body twice
// (with and without --fast) and stopped without a result.
// MiniMax E (11-10): no new form, no lower score (34); retail loads the group's flags as a word into AX with the
// group pointer still in EAX and tests bits 1 and 2, ours loads a byte and masks it with 3 - every word form moves
// the pointer into ESI (120) or folds the index into the load's address (117), and the nested and compared tests
// give 170 and 120.
// MiniMax A (11-06, run 52), not kept: 2 (from 34) with MiniMax D's form below, the group flags read
// through the device-group macro into a word before the machine test and masked into a byte after
// it, plus `function_value = 0.0f;` first in case 0x1200055f's velocity block (a dead store there,
// match-only). What is left at 2 is retail's address fold (a lea before the load) where ours loads
// the word through the combined address, plus the mask instruction retail does not have. A named group pointer gives
// retail's delayed word load but lands in ESI, not EAX; a dword local for the element address does
// put the address in EAX but the load then narrows to a byte (115) or the address falls back to
// ESI (122), and two separate reads of the flags do not merge either (122). Lead: case 0xf00055e
// also starts its velocity block with `function_value = 0.0f;`, so retail's result may not be
// zero-initialised at entry; dropping the initialiser needs a store on every other path.
// MiniMax D (11-05), not kept: 2 fast (from 34) with the group flags read into a word before the machine test, masked
// into a byte (`byte group_flag_bits = (byte)(group_flags & 3)`), and `function_value = 0.0f;` added at the top of case
// 0x1200055f's velocity block, where function_value is still its 0.0f initial value (a dead store kept only for the
// code shape; the 10-30 rule keeps one only with a match). The flags form without the store is 89 fast. Lead: retail's
// zero store in that case suggests the result is not zero-initialized at entry the way ours is.
// MiniMax A (11-04, run 41): no new form, no lower score (34); the device-group flags are still a byte load
// with a mask where retail loads a word, and retail's zero store in case 0x1200055f is still missing.
// MiniMax D (11-07): the flags read in both arms of the machine-flags test plus `function_value = 0.0f;` at the top
// of case 0x1200055f's velocity block 3, Round 62's form; not kept (a read written into both arms only to shape the
// load, and a store of the 0 the result already holds).
// Earlier (34, was 34): the `group_flags & 3` local (lane S) loads the flags as `movzx eax, byte [eax+2];
// and eax, 3` where retail loads `mov ax, [eax+2]` and tests al twice (one extra instruction), and
// retail clears the result in case 0x1200055f (one instruction missing); between the two the code is
// retail's shifted by one. Plain masks plus the zero store leave only the group pointer's register.
// MiniMax A (11-04, run 41): no new form, no lower score (34); the device-group flags are still a byte load
// with a mask where retail loads a word, and retail's zero store in case 0x1200055f is still missing.
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
// Round 114 (lane 8, fast; with the 0x1200055f zero store unless noted): the group flags read through the
// macro at each test, no group local 123 (655 bytes: the address sinks below the machine test into `mov ax,
// [esi + eax*4 + 2]`, the table base in a pushed ESI); `result` and `function_value` declared before the
// device and definition with word-local masks 122, the same order on the lane S form without the zero store
// 34 (same code); `__assume(group != NULL)` after the group pointer (as matched 0x187450) with word-local
// masks 122 (no colouring change); `(group_flags & 1) == 1 && (group_flags & 2) == 2` 122.
// Round 21 (lane 17; compile-only /FAs screen of the file reproduces the listing): on masks + the zero store
// (122), a two-byte bitfield struct copy tested with `&&` 122, bitfields through a struct-pointer cast of the
// field 122, a `word const *` to the group's flags 122, a `short` flags local 122, `register` on the group
// pointer 122 (all the pushed ESI); screens only: `register` on the flags or machine flags, `!= 0` masks, the
// flags word at its read, machine flags first (ESI); the bitfield copy tested with `&` frees EAX only by
// spilling the copy to the stack. 1-7 dead temporaries (diagnostic) leave both forms unchanged.
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
// Round 285: native real magnitude APIs at the three velocity-case consumers,
// seven subsets and all with the power-velocity zero default 34/34/34/34/34/34/34/117.
// Original body restored; native word-load sharing and zero-join scheduling remain.
// Round 21 (lane 17; compile-only /FAs screen of the file reproduces the listing): on masks + the zero store
// (122), a two-byte bitfield struct copy tested with `&&` 122, bitfields through a struct-pointer cast of the
// field 122, a `word const *` to the group's flags 122, a `short` flags local 122, `register` on the group
// pointer 122 (all the pushed ESI); screens only: `register` on the flags or machine flags, `!= 0` masks, the
// flags word at its read, machine flags first (ESI); the bitfield copy tested with `&` frees EAX only by
// spilling the copy to the stack. 1-7 dead temporaries (diagnostic) leave both forms unchanged.
// Round 38 (m3 lane 2; compile-only screen against retail, base aligned 3 = 34): 612 combinations of
// pointer/const pointer/pointer-to-const, pointer before/after the machine flags, word/short/dword/long/
// unsigned short/int/unsigned long/unsigned int flags locals declared at the top, after the machine test or
// read directly, tested as masks, `!= 0` masks, `1U`/`2U` masks, `!!`, `(f & 1) && ((f >> 1) & 1)` and
// `((f >> 0) & 1) && ((f >> 1) & 1)`, all with the zero store: every one is the pushed ESI (aligned 4) or the
// machine flags read first (13). A bool for the second bit (`bool second = (group_flags & 2) != 0;` and 14
// other spellings) gives retail's `lea eax` / `mov ax, [eax+2]` / `test al, 1` exactly, aligned 2: only its
// `shr al, 1; test al, 1` for retail's `test al, 2`; a word/dword/int/long second-bit local folds back to
// `test al, 2` and the pushed ESI. Real (fast): two bools + zero store 114, a ternary bool 114 (656 bytes
// each), `bool both = (f & 1) ? (f & 2) != 0 : false` 117 (658), an int flags local with masks 122, a word
// second-bit local 122. Screen: that bool tested nested, after a `first` bool, with `!= 0` on the first bit,
// as `second == true`, const, through a `both` bool, through a ternary value, or braced: all the shift
// (aligned 2); `&` of the bits 13.
// Round 310: Complete device/tag receiver observations at each use, bool/byte/word/long
// success carrier without/with power-velocity zero default: 494/512/518/515 and
// 497/515/521/518. Original cached receivers and body restored; native word-load
// sharing and velocity-zero join remain.
// Round 284: complete case-local numeric result production, conditional delay/position/power
// selected-value subsets 151/150/123/143/134/145/120; all with native group masks 155.
// Original body restored; word-load/EAX sharing and power-velocity zero join remain.
// Round 78 (m3 lane 12, fast; word flags + the 0x1200055f zero store, aligned 4): a forceinline word getter taking the group pointer 122, a const pointer copy of
// the group at the read 122, nested word tests 122, the pointer scoped to a block that ends before the tests 122 (all the pushed ESI, same code).
// AC484: Native byte/signed-byte second-bit masks after the machine test 120/120;
// before it 132/132; with velocity zero 122/122 and 64/64. Original word read,
// masked bits, zero lifetime and body restored; word-load/EAX sharing remains.
// Round 42 (m3 lane 2, fast; each with the 0x1200055f zero store): a two-value dispatch on `group->flags & 3`
// 120 (diagnostic); the group pointer formed after the machine test with direct field masks 127 (657 bytes); word
// masks with a dead `else group_flags = 0;` 122 (diagnostic); `function_value = (f & 1) && (f & 2) ? 1.0f :
// function_value;` 122; the word flags read at the declaration before the machine flags, nested tests 115 (the
// load folds into `mov ax, [eax + edx*4 + 2]` ahead of the machine test, no pushed ESI).
// N336: Failure-owned power-velocity zero at cruise/disabled/power/paired/all leaves
// 103/49/49/104/103/41/104; including stopped default 108. Original callback restored;
// native group-word sharing and velocity-zero join remain.
// N345: Immediate velocity-magnitude nodes at three guarded divisions, seven
// subsets, then all with immutable bounded publication: all eight 34. NOT kept:
// single-use library results do not match. Original callback and observations restored.
// Round 81 (m3 lane 9, fast; on the 0x1200055f zero store; /FAs screen of 18 spellings: a read-only word value
// always puts the group pointer in a pushed ESI, only an in-place `shr al, 1` keeps it in EAX): `bool second =
// (group_flags >> 1) & 1` 114 (656 bytes); a `both` bool 122; `(bool)(f & 1) && (bool)(f & 2)` 114; first/second
// bools tested second-first 152 (655); `f & 1 ? (f & 2) != 0 : false` 116 (657).
// N345: Immediate velocity-magnitude nodes at three guarded divisions, seven
// subsets, then all with immutable bounded publication: all eight 34. NOT kept:
// single-use library results do not match. Original callback and observations restored.
// Round 85 (m3 lane 9, fast; each with the 0x1200055f zero store): a byte cast on the first flag test 122, on both 122; a byte low local with a word second
// test 122; a long flags local with a byte cast 122 (movzx); char casts on both 122. Screen: `(bool)` on the first test, the bits tested second first, a word
// flags pointer local, a const group and value: the pushed ESI. Reading the group through the macro as well as the local (`(group->flags & 1) &&
// (DEVICE_GROUP_GET(i)->flags & 2)`, or a value local for one test and the macro for the other) does put the pointer in EAX, retail's `lea eax`, but both
// tests then read memory (`test byte [eax+2], bl`) with no word load.
// F362: Existing callback as static device member, ordinary/inline, masked/word
// flags and original/active-velocity zero: 34/34/120/120/117/117/122/122.
// NOT kept: word-load/EAX sharing and zero join remain; original callback/table restored.
// B407: Complete native short-unit 16-bit flag fields, unsigned/signed, masked/full
// consumption and native velocity zero: 34/117/120/122 twice. No word-load/EAX recovery;
// every other file score preserved. Original scalar field and complete callback restored.
// P307: Actual machine bitfields with logical/bitwise admission, original/native group masks,
// and absent/present velocity zero: 34/142/120/131/117/146/122/137. NOT kept: no exact native
// observation/type graph; original view, word/masked result and entire callback restored.
// M375: Bounded missing-bit byte/word/long/dword without/with active-velocity zero:
// 95/93/93/93/17/17/17/17. NOT kept: 17 uses a byte load, complement and one combined
// mask test instead of the native word load and two tests; only its zero-default combination
// gains. Wider carriers grow to 655 bytes. Original whole callback, masks and result restored.
// Round 62 (m3 lane 14, fast; compile-only object screen exact at 34): the pointer wins EAX when the flags load
// is two load tuples at register allocation that merge later. With the zero store: a second if/else after the
// machine test whose arms both read the flags 0 (MATCH, byte for byte; identical arms, so reverted); the flags read in
// both arms of the machine test 3 (654 bytes; the load hoists above the `je`, the rest is retail's); both arms
// also holding the two tests 3; a store in the machine arm stops the hoist and the load tail-merges after the
// join (diagnostic). `!(~f & 1) && !(~f & 2)` 110; `!(~f & 3)` 121 (655); do/while(false) breaks 122 (656). Screens:
// a NULL-first or reassigned pointer, `group += 0`, a dead read of flags/identifier/value, a dead pointer copy, the
// flags read twice, a no-op test in an arm, a pointer to the flags field, 30 missing-bit carriers: the pushed ESI or 35+.
// Round 92 (m3 lane 16, fast; each with the zero store unless noted; "al" = aligned count, this body al 2): the group as a
// reference 122; the machine test as a `?:` on function_value 122; the group test as `else if` with direct field reads
// 111 (654 B); the pointer declared at function scope 122, at case scope 122; nested direct-read tests 122; the type and
// index tests nested 122; the word flags read before the machine test with plain ifs 124 (655 B, al 7), with the group
// test as `else if` 23 (654 B, but al 10: the load folds ahead of the machine test with the pushed ESI, and `else if`
// skips the group test retail always runs, so not kept); the lane S mask computed before the machine test, no zero
// store, 124 (al 8). A compile-only screen (no /GL) shows the base table or pointer in ESI for every word or dword
// load of the flags, even with the machine test removed; only a byte load (lane S) or two loads merged at a join
// leave EAX.
// MiniMax E (11-08): no new form, no lower score: retail loads the group's flags as a word and tests bit 1 then bit 2, ours loads a byte and masks it with 3, and testing the word without the mask scores 120.
// MiniMax E (11-09): no new form, no lower score: retail loads the group's flags as a word and tests bits 1 and 2, ours loads a byte and masks it with 3; dropping the mask gives retail's word load but moves the group pointer into esi (120).
// T80: Partial position-zero closed/displaced admission in bool/byte/short/long:
// 34/34/34/34/34/34/34/34. Group-word load/EAX sharing and power-velocity zero join remain.
// NOT kept: no byte equality or qualifying gain; original complete body restored.
// Round 81 (m3 lane 10, fast with all of this file): the two 0x1200055f arms as ternaries with a 0.0f false value on
// the lane S form 104 (each false arm gets its own `movaps xmm0, xmm1; jmp` block, no shared zero); the lane S bits read
// through the macro with no group pointer 124 (655 bytes); a zero-initialised local for the velocity fraction, copied
// to the result at the end of the 0x1200055f block, 117 (the local takes a stack slot); the group pointer formed after
// the machine test with the lane S bits 124 (655 bytes); a `long` machine-flags local 34 (same code).
// Round 161 (m3 lane 5, fast): retail zero-initialises the result at entry (`movaps xmm0, xmm1` before the dispatch) and
// again in both velocity blocks, so the 0x1200055f zero store is a second copy as in case 0xf00055e, not a missing entry
// initialiser. Re-measured: the plain word masks 120 (pointer `lea esi`, pushed and popped around the first test). Tried: no
// group pointer, the word read through the macro after the machine test 53 (index in EAX, table base in a pushed ESI, `mov ax,
// [esi + eax*4 + 2]`); the word local zero-initialised at its declaration 120; the power group index in a case-scoped local
// with word masks 121; the mask form plus the 0x1200055f zero store 117 (657 bytes); the word local at function scope 120.
// Round 107 (m3 lane 9, fast; compile-only screen exact at 34): the permuter (600 s, 53 variants) best 34. A
// file-local inline locked-group test taking the group pointer, `if (!(f & 1)) return false; return (f & 2) != 0;`,
// 115 (657 B); the same taking the group index (lookup inside, no group local) 167 (630 B: the device moves to ESI
// from entry); the pointer form as `if ((f & 1) && (f & 2)) return true; else return false;` 120; the index form with
// the if/return body 167; the pointer form plus the 0x1200055f zero store 119 (660 B).
// Round 76 (m3 lane 13, fast): the power/0x10 test without its inner `break` 34 (same code); the group pointer as a
// NULL-default ternary on the index, tested `if (group)` inside the type test, word masks + zero store 127 (662 bytes; the
// screened NULL-first pointer, measured): the pointer takes EAX and the load is retail's `lea eax, [eax + edx*4]` / `mov ax,
// [eax+2]` / `test al, 1` / `test al, 2`, only the extra `test eax, eax; je` differs, as round 13's third-reference finding
// predicts; the matched 0x1071e0's `!(!(f & 1) || !(f & 2))` spelling (screened above) 120, with the zero store 122 (the
// direct-read code). Re-measures: an inline both-bits helper, also used by 0x1071e0, 120 / with the zero store 122; a
// named typed table base 122. Every spelling of the group test gives one of two codes (the byte load with a mask, 34, or the
// word load with the pointer in a pushed ESI); only an extra pointer reference at allocation moves the pointer to EAX.
// M3 lane 14 r80 (fast): register-blind only three rows differ: the lane S mask (`movzx eax, byte [eax+2]; and eax, 3` for
// `mov ax, [eax+2]`) and the missing 0x1200055f zero store, so a match needs both the store and a word load sharing the pointer's
// EAX. New: no initialiser, `function_value = 0.0f;` at the top of each case that can leave it unset (and none inside 0xf00055e's
// velocity test) 146 (660 bytes, masked 11); case 0xf00055e's zero written as the false value of a ternary 84 (655 bytes: the
// zero store vanishes, so retail's own zero there is an explicit redundant store, as main writes it). No untried natural form is
// left for the two rows: every word-load spelling listed above keeps the pushed ESI, and the zero store is a dead store under the
// 10-30 ruling.
// Round 107 (m3 lane 9, fast; compile-only screen exact at 34): the permuter (600 s, 53 variants) best 34. A
// file-local inline locked-group test taking the group pointer, `if (!(f & 1)) return false; return (f & 2) != 0;`,
// 115 (657 B); the same taking the group index (lookup inside, no group local) 167 (630 B: the device moves to ESI
// from entry); the pointer form as `if ((f & 1) && (f & 2)) return true; else return false;` 120; the index form with
// the if/return body 167; the pointer form plus the 0x1200055f zero store 119 (660 B).
// m3 lane 14 r82 (fast): the register-masked aligned diff is still three rows: the lane S mask for retail's word load sharing the
// pointer's EAX, and case 0x1200055f's zero store (retail `movaps xmm0, xmm1` after the flags load, before the flag-8 branch; dead
// like main's case-0xf00055e store, since every later path re-sets the value or threads past the pin). No untried natural form for
// either row; the source is unchanged since round 107's permuter run, so it was not rerun.
// Also r82, on the lane S body without the zero store (fast): nested type/index tests, a group-index local inside the type test, a
// const group pointer, the machine flags read at each use, the group pointer at function scope, a dword flags local and the bits
// test in a named bool all compile to the base code (34); a byte bits local 45 (masked 3: `mov al, [eax+2]` shares EAX, then the
// mask); bit 2 tested first 34 (masked 6); `group_flag_bits == 3` 52 (as the listed `== 3`); the group test joined to the machine
// test by `||`, or as its `else if`, 54 (same code, masked 10); the power test's operands swapped 34 (masked 8); a byte
// machine-flags local 35 (masked 4); the machine flags read before the group pointer 127 (657 bytes, as listed).
// m3 lane 15 r74: a compile-only screen (/O2 /Gr without /GL) of this body against retail leaves exactly the two
// listed rows (the byte load with the mask for retail's `mov ax, [eax+2]`, and case 0x1200055f's zero store), so the
// screen is exact here. No natural spelling of the group test outside the listed families was found; every listed one
// keeps the 16-bit value ahead of the two-reference group pointer for EAX.
// Fast: the power/0x10 test as two clearing ifs 123 (662 B); the machine flags read before the group lookup 127 (657 B);
// the case opening as `function_value = position == 0.0f ? 1.0f : 0.0f` 133 (657 B); case 0x1200055f's velocity block
// through a file-local `inline` fraction helper whose result starts at 0 266 (547 B: the helper stays out of line),
// with the two ifs 258 (555 B); the helper as `PRIVATE __forceinline` 117 at 654 B (inlined, but its zero-initialised
// result takes a stack slot, as round 81's zero-initialised local did).
// Round 175 (m3 lane 5, fast): tools/permute.py on this 2 body (the first run since the body changed): 86 tries in 900 s,
// 36 at 2, none below. On the group pointer with a word read after the machine test (the pushed ESI, 122): the word local
// declared ahead of the pointer and assigned after the machine test 122; a file-local inline bit test taking the flags
// word and a bit number 122; the word assigned inside the first test 122; `if (!(f & 1)) {} else if (f & 2)` 122; a
// `short` local with `!= 0` masks 122.
// Round 176 (m3 lane 6, fast; each on the group-pointer form, re-measured 122 with the pointer in a pushed ESI): the group
// test as a ternary on function_value 122; the machine test as a ternary 122; the bit tests nested 122; a `locked` bool from
// the machine bits set by the group test and tested once 88 (654 bytes: with no branch between the lea and the word load
// the pointer takes EAX and the load is retail's `mov ax, [eax + 2]`, but the bool takes BL through setne); a group-locked
// bool computed ahead of the machine test 129 (664 bytes). On the lane S form: the type/index test as an early `break`
// with the rest unnested 2 (same code); the byte as `(byte)group_flags & 3` 2 (same code); a word mask carrier 14 (655
// bytes). Lane S form restored.
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
			word group_flags = DEVICE_GROUP_GET(device->power_group_index)->flags;
			dword machine_flags = ((s_device_machine_view *)device)->flags;

			if (machine_flags & 0xd)
				function_value = 1.0f;
			byte group_flag_bits = (byte)(group_flags & 3);
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
			function_value = 0.0f;
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

bool __stdcall function_106790(long device_index);

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
// MiniMax A (11-14): no new form, no lower score (121): retail takes the initial velocity in xmm6 with five stack
// floats and returns with ret 0x14, while this build passes it on the stack and returns with ret 0x18, and the
// six-float signature may not change; the clamp's register homes follow from that difference.
// MiniMax H (11-14): no new form, no lower score (121): this round found no new spelling; the clamp's three values and the initial velocity keep the SSE registers the LTCG convention gives them, and the earlier rounds' ninety-six clamp variants all measured above 121.
// MiniMax B (11-13): no new form, no lower score (121): the three clamped values are still taken in xmm5, xmm7 and xmm2 where retail takes them in xmm7, xmm1 and xmm2, and the first argument arrives in xmm6 under the custom LTCG convention, so the clamp's register choice is not ours to pick.
// MiniMax H (11-12): no new form, no lower score (121): the initial velocity read into a local in the callee, passed through a local at the call site, both together and the formula reading the stored velocity back all measure 121, so the body stands.
// MiniMax G (11-12): no new form, no lower score (121): nothing new this round; the initial velocity remains stack-passed
// instead of xmm6, and the ramp-down residency and the return size differ.
// MiniMax G (11-12): no new form, no lower score (121): the initial velocity remains stack-passed instead of xmm6,
// and the ramp-down residency and the return size differ.
// MiniMax B (11-12): no new form, no lower score (121): the initial velocity is still stack-passed where retail passes
// it in xmm6, a whole-program convention the callee sets, so the whole XMM allocation after the entry differs - retail's
// first float compare is in xmm5 where this build's is in xmm7; the caller's argument registers come from the callee's
// definition, which this lane may not edit.
// score 121 (was 121): Initial velocity remains stack-passed instead of XMM6; ramp-down residency and return size differ.
// MiniMax C (11-09): no new form, no lower score (121); the initial velocity is still passed on the stack where
// retail keeps it in xmm6 and takes five stack floats, so the whole register assignment after the entry differs.
// MiniMax A (11-06, run 52): no new form, no lower score (121) - retail keeps the initial velocity in
// xmm6 and leaves the ramp-down time in memory, ours caches the ramp-down time and pushes the initial
// velocity; both ramp-down clamps written as if-statements (retail's conditional-store form) score 122,
// and the permuter was stopped after half an hour without producing a try.
// MiniMax D (11-05): no new form, no lower score (121); the permuter run was killed at its time limit
// before it produced any result.
// MiniMax E (11-10): no new form, no lower score (121); the initial velocity is stack-passed here and in XMM6 in
// retail, and the parameter list is not mine to change, so the velocity's register home follows from it.
// Earlier (121, was 121): Initial velocity remains stack-passed instead of XMM6; ramp-down residency and return size differ.
// MiniMax A (11-04, run 41): no new form, no lower score (121); the initial velocity is still stack-passed
// where retail keeps it in xmm6, and the ramp-down time still takes a register through the clamps.
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
// DH round 262: immediate phase-distance selections 125/121/125; conditional phase derivatives
// 121/121/121; both producer families 125, with field-owned cruise speed 141 (529 bytes). All restored.
// Round 216: Bounded up/down-time producers and local/field cruise-speed ownership; then standard minimum/maximum duration/ramp floors and upper bounds:
// 128/128/142/142/137/137/138/138/143/148/163/151/166/167/181/217. Original executable source restored.
// Round 229 (lane P): both phase-distance outputs at common final stores, initialized at reset,
// entry or before pins 140/163/151; reset outputs with field-owned cruise velocity 166; with
// stored initial-speed reads 140; both phase results selected from their nonzero tests 152,
// at entry 172; before pins with field-owned cruise velocity 163. Original body restored.
// Round 276: complete native motion result published at the tail, declared at reset/entry,
// with local/field cruise speed and up/down phase order 173/174/173/174 each.
// Original field stores restored; frameless initial-speed/ramp-down ownership remains.
// Round 285: native real absolute API, absolute-duration and independent absolute-distance
// producers, all seven subsets 121 each; all with field-owned cruise speed 134. Original
// body restored; initial-velocity register and ramp-down residency remain.
// DH round 262: immediate phase-distance selections 125/121/125; conditional phase derivatives
// 121/121/121; both producer families 125, with field-owned cruise speed 141 (529 bytes). All restored.
// Round 305: complete immutable bounded-time producers with mutable/cruise-const/
// half-const/all-const phase nodes give 142/157 in each local/field cruise pair.
// All eight are 500/523 bytes and change caller allocation; original argument, clamp,
// phase lifetimes and field publication restored. Initial velocity residency remains.
// Round 38 (m3 lane 2, fast, both callers 31/13 unchanged): retail reads the ramp-down time from its slot at
// every use (as if address-taken); ours keeps it in xmm1 from the zero clamp to the half time. The pinned
// target in its own local (target parameter dead after the pin) 121 (same code); that with the per-branch
// cruise store 134 (524 bytes); the ramp-down minimum as `if (ramp_down_time > remaining)` 121; the half times
// named before the cruise time 121; the distance as `real distance = (real)fabs(delta);` inside the duration
// test (no copy at the top) 121.
// Round 310: Initial reset as contiguous zero span/native initialized reset/native zero reset/whole clear,
// with local cruise speed 142/170/181/140; field-owned speed 138/178/189/147. All reset
// forms restored; frameless ramp-down and initial-velocity residency remain.
// M304: Paired unchanged endpoint-speed scalar captures at entry/time bounds/pins/active work,
// local/field cruise-speed ownership: 121/134/121/134/121/134/121/134. Both callers stay 31/13;
// original parameter lifetimes, velocity ownership and complete body restored.
// Round 312: Paired phase-energy numerators before doubled derivative denominators,
// with endpoint/cruise square-part subsets 0-3: 121 each; field-owned cruise speed 134 each.
// Original phase expressions, publications and complete body restored; initial velocity
// XMM6 and ramp-down stack residency remain.
// Round 333: One-observation position pin with distinct target and complete initial/duration/cruise
// field domains: 142/142/142/142/106/106/106/106. Original-target and ordered-pin/complete-field/early-down-half
// refinements 106/106/106/140. The 106 forms introduce a frame and raise the second caller 13 -> 28;
// original input observations, publications and complete body restored.
// F344: Actual initial-speed publication at entry/after floors/after bounds/before
// reset, with original parameter arithmetic 128/123/123/126; consistent stored-speed
// arithmetic 128/123/123/126. NOT kept: early publication/disabled-path clears add
// stores absent in retail and grow length; original complete body and callers restored.
// Round 63 (m3 lane 10, fast, both callers 31/13): retail stores the zero straight into the ramp-down slot at its
// clamp and compares the minimum against memory, never touching xmm6 before the inner block. The three zero clamps
// written `0.0f < x ? x : 0.0f` 121 (same code); both ramp-down clamps as one PIN(ramp_down_time, 0.0f, remaining) 127;
// the zero-denominator velocity as `(target - position) / duration` 149 (533 bytes); the position pinned at the top
// before the time clamps 139 (525 bytes, the fast image changes both callers' conventions); the ramp-down zero clamp
// after `remaining`, right before its minimum 123.
// Round 34 (lane 17, fast, with both callers): the initial velocity moved to the second parameter
// (definition and both calls; diagnostic, no retail order evidence) 123 (callers 25/23, still six
// stack floats); the ramp-down minimum written `ramp_down_time < remaining ? ...` 121; the target
// pinned before the position 141 (507 bytes, a `push ecx` frame and ret 0x14: one float leaves the
// stack, callers 26/28); the initial-velocity store first among the inner stores 122 (a repeat); the
// denominator summed cruise, half ramp-up, half ramp-down 121 (same code). Original body restored.
// N345: Direct scalar truth at divisor/derivative tests, seven nonempty subsets,
// 121 each; nonzero admission on bounded nonnegative ramp-up 132. NOT kept: no
// gain. Both callers stay 31/13; original predicates and complete body restored.
// Round 123 (m3 lane 7, fast with both callers, which stay 31/13): both ramp-down clamps as `<=`/`>=` ifs 122;
// the ramp-up zero clamp as an if 122; the initial-velocity store first in the inner block 122; an early
// `return` on the tiny duration 121 (same code); one nested ternary for both ramp-down clamps 127.
// Round 81 (m3 lane 9, fast with both callers 31/13; /Gd /FAs screen of 48 combinations of inner-local order,
// pin order, delta/distance naming and duration-first reset: the initial velocity always loads into an xmm
// register written earlier, never a free XMM6): the duration stored first among the reset fields 121; the
// distance named first with delta copied from it 123; duration first with the half ramp-down time named first
// 123; distance first with the cruise time after the half times 123; that with the duration first 123.
// N345: Direct scalar truth at divisor/derivative tests, seven nonempty subsets,
// 121 each; nonzero admission on bounded nonnegative ramp-up 132. NOT kept: no
// gain. Both callers stay 31/13; original predicates and complete body restored.
// J248: Paired destructive derivative/phase-distance producers, local/field
// ownership, separate assignments and actual output-field receivers, then callee-pop
// and caller-clean declarations: 121/121/121/121/121/121/132/121/123. Original phase producers, convention
// and body restored; initial-speed XMM6 and ramp-down stack residency remain.
// F362: Existing initializer as motion instance/static member, ordinary/inline,
// local/field cruise speed: 121/121/121/121/134/134/134/134. NOT kept:
// initial-speed/ramp-down residency unchanged; original routine and both callers restored.
// P306: Mutable acceleration/deceleration/cruise-speed producers, seven subsets,
// then separately initialized derivatives: 121 each. NOT kept: no gain or match.
// Original phase producers restored; XMM6 initial-speed/ramp-down stack residency remains.
// Y580: Paired endpoint squares at entry/floors/bounds/pins, local/field cruise ownership:
// 169/185/168/184/168/184/175/191. Post-pin pair recovers ret 0x14 and both callers in
// preview, but grows a frame and 558/571-byte bodies. NOT kept: early one-use nodes lack
// equality and regress; original complete body restored at 121, native 523 bytes.
// Round 62 (m3 lane 14, fast, callers 31/13 unless noted): the velocity and cruise stores in both arms of the
// denominator test 121 (same code); the velocity store alone in both 138; all four reset stores in both 132; retail's
// statement order (cruise time, target/position/start stores, half times) with a velocity local 170 (486 bytes, the
// ramp-down held in xmm6 for life, callers 19/34), through the motion field 131 (510, callers 26/28). Diagnostics
// (reverted at once): 1/2/4/8 extra real uses of the initial velocity at the top or in the inner block never hold it in
// one register, so it does not follow reference weight; a volatile ramp-down on these three bodies 165/152/166.
// Round 92 (m3 lane 16, fast, callers 31/13 unless noted): the ramp-down minimum as `if (remaining <= ramp_down_time)` 121;
// `remaining` declared then assigned 121 (same code); the position read into its local and then pinned 142 (507 B, ret
// 0x14, but the target is the promoted parameter, in xmm5 from entry, with a `push ecx` frame; callers 26/28); the
// distance computed first with delta copied from it 123; no cruise-time local 121. A compile-only screen of this body
// and four clamp-order variants uses all eight xmm registers before the first fld; retail uses seven there.
// MiniMax E (11-08): no new form, no lower score: the clamp chain keeps its value in xmm7 here and in xmm5 in retail, and the second clamp uses xmm2 here and xmm7 in retail.
// MiniMax E (11-09): no new form, no lower score: retail keeps the initial velocity in xmm6 and returns 0x14 stack bytes, ours passes it on the stack and returns 0x18; the parameter list is not mine to change.
// Round 161 (m3 lane 5, fast, both callers 31/13 unless noted; every form ret 0x18 with the initial velocity on the stack unless
// noted): the ramp times renamed acceleration/deceleration time 121 (same code, so allocation ties do not follow names); no
// `remaining` local, written out at the clamp and the cruise time 121 (same code); half times as `/ 2.0f` 121 (same code); the
// eight reset stores through an inline reset helper 121 (same code); the inner block as a plain inline plan helper 359 (kept out
// of line, 291 bytes, ret 0x14; callers 26/28). Diagnostic, reverted at once: a volatile ramp-down parameter on this body 165
// (546 bytes, a `push ecx` frame, the target passed in a register, the initial velocity still on the stack; callers 26/28).
// After the five: the same plan helper as forceinline 121 (same code); the cruise velocity as one ternary 121 (same code).
// T79: Both duration/distance admissions formed at their original post-reset/post-fabs tests,
// tiny/active bool/byte/short/long outcomes: eight 121s; both callers stay 31/13. NOT kept:
// no exact result form; original body restored, initial-speed/ramp-down residency remains.
// Round 76 (m3 lane 13, fast with both callers): fresh locals for subsets of the clamped times (all four 110 and the down time
// alone 144 are above): duration 129 (511 bytes; duration then passed in XMM7 with a `push ecx` frame, still five stack floats;
// callers 26/28), ramp-up 133 (26/28), duration + ramp-up 133 (19/34), duration + ramp-up + ramp-down 133 (19/34), duration +
// ramp-down 144 (523; 26/28), ramp-up + ramp-down 141 (508; 19/34). Each puts an unmodified time parameter in a register
// from entry; none gives the initial velocity XMM6.
// M3 lane 14 r80 (fast, callers 31/13; masked = aligned count with registers masked, 22 at 121): the cruise velocity through the
// motion field, no velocity local (listed, 134), re-measured masked 16 against 22: its tail blocks are retail's and register-blind
// only the ramp-down clamps' register copies (`movaps xmm2, xmm0` before each store), the inner block's initial-velocity load from
// [esp+0x18] and the return size differ. Retail keeps the ramp-down time in memory at every use (each clamp a conditional store,
// every read a load), as an address-taken variable would be. Diagnostic, reverted at once: a volatile ramp-down parameter 165
// (546 bytes, `push ecx` frame, the initial velocity still on the stack). I found no untried natural form: the pointer, reference,
// helper, pin, if-clamp, fresh-local and order forms listed above all keep the ramp-down time in a register. New: the
// deceleration distance reduced in place (`motion->deceleration_distance -= ...`) 121 (same code), on the field form 134 (same code).
// Round 107 (m3 lane 9, fast, callers 31/13 unless noted): retail's callers push the five floats in the declared
// order (the literal 0 final velocity first) and load the initial velocity into XMM6 ahead of the pushes, so the
// declared order already agrees. The three zero clamps as `x < 0.0f ? 0.0f : x` 125; the ramp-up limit as `duration <
// ramp_up_time ? duration : ramp_up_time` 121 (same code); the ramp-down clamps as `if (!(x > 0.0f)) x = 0.0f; else
// if (!(remaining > x)) x = remaining;` after `remaining` 127; `remaining` renamed cruise_time and reduced in place
// (`cruise_time -= ramp_down_time;`) in the inner block 121 (same code), at top level right after the clamp 140
// (callers 26/28). Diagnostic, reverted at once: a volatile ramp-down parameter 165 (546 B, callers 26/28).
// m3 lane 14 r82: a /Gd compile-only first pass of this body fills all eight XMM registers in the clamp/pin phase (duration xmm7,
// ramp-up xmm2, ramp-down xmm1, remaining xmm3, one xmm4, position xmm6, target xmm5, zero xmm0) and loads the initial velocity once,
// late, into a reused xmm5, so it stays a stack argument; retail's clamp phase compares the ramp-down from a scratch xmm1 and stores
// the zero and the minimum only to its slot, which leaves a register free from entry. A volatile ramp-down local (diagnostic,
// reverted at once) 169 (549 bytes: it adds a frame); callers 25/34.
// Also r82, the same /Gd screen: the pins before the clamps, the target pin first, the ramp-down zero clamp after the duration
// clamp or after the pins, if-statement clamps, `remaining` written out at both uses, a pointer to the ramp-down time and an early
// named velocity copy all keep the one late load into a reused xmm5 (even the pointer folds away), so none was fast-checked.
// Also r82 (fast, callers 0x108600/0x108670 in brackets, base 31/13): both pins before the clamps 132 [19/34]; the ramp-down clamps
// after the pins 133 [19/34]; the initial-velocity store after the cruise-velocity zero in the inner block 120 [31/13] (one point,
// not retail's store order); the denominator test's arms swapped 121; the deceleration-distance store before the acceleration zero
// 121; the velocity store moved with the pins first 132, with the clamps after the pins 133, with start stored before position 120.
// Round 76 (m3 lane 13, fast with both callers): fresh locals for subsets of the clamped times (all four 110 and the down time
// alone 144 are above): duration 129 (511 bytes; duration then passed in XMM7 with a `push ecx` frame, still five stack floats;
// callers 26/28), ramp-up 133 (26/28), duration + ramp-up 133 (19/34), duration + ramp-up + ramp-down 133 (19/34), duration +
// ramp-down 144 (523; 26/28), ramp-up + ramp-down 141 (508; 19/34). Each puts an unmodified time parameter in a register
// from entry; none gives the initial velocity XMM6.
// Round 188 (m3 lane 4, fast with both callers; ret 0x18 and callers 31/13 unless noted): the call removed from either
// caller leaves this body at 121 (not caller-side). A /Gd compile-only screen of the first pass reproduces the LTCG
// results (a volatile ramp-down or the target pinned first make the target the register parameter); 96 clamp-phase
// variants all write eight XMM registers before the first fld (retail seven). Zero-clamp orders u,d,up,r 121; d,r,u,up
// 123; u,r,d,up 123; u,d,r,up 119 (two points, a statement order); r,u,d,up 123; the duration and ramp-up zero clamps as
// if-statements 121 (same code); `remaining` before the ramp-down zero clamp 123. On u,d,r,up: u,d,up, `remaining`, then
// the ramp-down zero clamp 121; all four clamps as ifs, the ramp-up zero clamp as an if, the ramp-down minimum as an if
// 119 each (its code); the target pinned first 141 (507 B, ret 0x14 with the target as the register parameter, callers
// 26/28); the position read into its local and then pinned 142 (507 B, the same convention move). Re-measured: the
// duration stored first among the reset fields 121.
// m3 lane 10 r99 (fast, callers 31/13 throughout, every form ret 0x18): the three zero clamps as `0.0f > x ? 0.0f : x` 125;
// the ramp-up limit through the remaining time's sign (`if (remaining < 0.0f) { ramp_up_time = duration; remaining = 0.0f; }`)
// 133; the cruise numerator's products initial/final velocity first 121; the duration zero clamp after both ramp zero clamps
// 123; both minimum clamps with the other comparison sense together (each alone listed at 121) 121. Retail's tail (the
// cruise velocity reloaded from the motion for the deceleration, 2.0 hoisted into XMM3) follows from XMM6 being taken: the
// accelerating block then has all eight XMM registers live and overwrites the velocity's register.
// MiniMax A (11-13): no new form, no lower score (121): the frame and the length agree, but the SSE register allocation differs
// from the first instruction (xmm7 here where retail uses xmm5) and every later pair follows that choice.
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
// score 13: retail's body is this one plus `xorps xmm6, xmm6` before the
// 0x107ab0 call, which takes the initial velocity in xmm6 where ours takes it
// on the stack, so this call pushes one zero more. Blocked by 0x107ab0's
// convention (see the notes there and at 0x108600).
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
// Round 134 (fast): a pointer local for channels[0] in both animation blocks 2; the same pointer in the first
// block only 2; a const time 2; a const animation 2; the frame position named before the scale 2. Every form
// keeps the fstp of the time between the 0.0f push and the animation push.
// Round 31 (lane 18, fast): sampled at function scope beside start 39; the 1/30 factor written first 2; one
// function-scope animation variable for both blocks 2; had_animation declared just before the flag test 10;
// had_animation and speed declared after the matrices 2. All restored.
// Round 70 (lane 10, fast): the time declared first in the block 2; the channel read as `channels->` 2; the sampled
// matrix declared after the time 2; the 1/30 factor in a const local 2; `time *= 1.0f / 30.0f` 2. Compile-only (which
// reproduces the order): 0-16 probe locals, `(void)&time`, a `(real)` cast and a named frame position move nothing.
// Round 94 (fast): the animation default-constructed then assigned 2; `time - step` named in a nested test 2; speed
// declared before had_animation 4; the state declared after both flags 2; the time renamed 2. With the time read only
// by the second call (diagnostic) the fstp still precedes the animation push.
// Round 58 (m3 lane 2, fast): the factor as the literal `0.033333335f` (as 0x0a76b0's file writes it) 2, written first 2;
// the first call's time as integer `0` 2; the frame position named before the product 2; `(real)(frame / 30.0)` 5.
// Diagnostic: 1 to 12 dead locals at the top (symbol count) 2 each, so the fstp slot is not a numbering tie.
// Round 174 (m3 lane 4, fast): the first call reading the channel's animation id directly with the local named after it
// 337 (1634 bytes); the time computed after the first sample call 20; the render model held as a pointer and cast at
// each call 2; the time as `frame_position / 30.0f` 2; a const render model 2. LTCG harness (this file with the files of
// the sampling, transform and object callees; not exact, but it shows the same swap): only the first call's render model
// replaced by a constant (diagnostic, wrong value) puts the fstp after `push edi`, so the render model load's timing
// sets it; a volatile render model (diagnostic), the time given to the first call, a label between the samples move nothing.
// Round 75 (m3 lane 14, fast): the animation id through a file-local inline getter in both blocks 2; the time through an
// inline seconds getter 2; both 2; `adjusted` copy-initialised at its declaration 2; the node index declared before the
// time 2.
// Round 172 (m3 lane 6, fast): a 600 s annealing permuter run (its full-image baseline alone took 924 s): 12 tries, none
// below 2; `time - step` named before the step test 9; the animation id read from the channel at each of the three
// sample calls (no local) 203. Same code: `time /= 30.0f` on the frame position; a `c_animation_channel const &`
// for the animation id and the frame position. On round 174's render-model lead: the render model read through the tag
// table at each of the five calls (no local) 532 (1744 bytes); computed inside the 0xbf5a0 block 424 (1681); the model's
// render model index in a local used at its test and the lookup 317.
// Round 94 (m3 lane 11, fast): the time assigned inside the first call's object expression (diagnostic, so its store
// follows the animation push in source order) 2: the store's source position does not set the order; a block-local copy
// of the render model for the block's three calls 2; a block-local copy of the state pointer 2; one function-scope
// seconds variable shared with the later duration 315; the flag test in a named bool ahead of the matrix copy 352
// (1633 bytes). Compile-only (same relative order as ours): the time declared first; the animation id read from the
// channel at the first call. Reading across four builds: in ours, the compile-only copy and round 174's constant
// render model the fstp is the eighth instruction after the branch, and in all of them except ours three instructions
// separate it from the call (ours four, since the render model's reload opens the block); retail has both the reload
// and three, as if that reload were not in the block when the store was placed.
// M3 lane 12 r121 (fast, and a one-file LTCG harness with the callee files that reproduces the swap): the render model
// declared at function scope 2; the animation and time read through a channel pointer local 2; the time computed first in
// the flag block with the sampled matrix after it 2 (harness); the render model as `(long)(void *)` of the tag data 2; the
// animation state fetched after the matrix copy 341 (harness positional; the prologue reorders). Diagnostics (harness,
// reverted at once): the render model or the time address-exposed, or the render model volatile, same code.
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

// score 250 (was 250): frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI where retail has
// ESI/EBX, the result in DL where retail keeps a stack byte, and the definition's re-reads (match-only leads at 58 and 69 below).
// MiniMax H (11-12): no new form, no lower score (250): the acceleration and the speed each written as one product added into a running value measure 359, and the same form without the volatile measures 382, so the body stands.
// MiniMax G (11-12): no new form, no lower score (250): nothing new this round; frame 0x20 and the state test are retail's; the
// index and index * 12 in edi/esi and the remaining homes differ.
// MiniMax G (11-12): no new form, no lower score (250): frame 0x20 and the state test are retail's; the index and
// index * 12 in edi/esi and the remaining homes differ.
// MiniMax B (11-12) lead 61, NOT kept (coordinator: the one-use rate local is match-only; without it 382): the 11-11 lane's shape measures 61 here at retail's 1495 bytes (plain check
// too), with 0x108600 and 0x108670 unchanged at 31 and 13 - the definition pointer's volatile dropped (retail reads the
// definition once), the acceleration and the speed each one product added into a running value, and the tick rate read
// into a local just before the power 0x11eed0 call; the rate local used by both reads gives 289 and the volatile kept
// gives 354, so the gain rests on that one-use local (coordinator 11-11 wanted this form match-only).
// MiniMax b (11-11): 58 (was 250), NOT kept (coordinator 11-11: needs the one-use `rate` local, match-only; without it 380; body at 250): the acceleration and the speed are each one product added into the
// running value (the 0xcc term opens the acceleration, the 0xc4 term opens the speed, and the 0xd4 and 0xd0
// terms are added into them), so both sums keep retail's operand order and its registers; the definition
// pointer's `volatile` is dropped and the rate is read into a local just before the power 0x11eed0 call;
// 1495 bytes, as retail. Left: the position arm's channel pointer (ours in ECX with a copy into ESI, retail's
// in ESI), the state slot read into EAX where retail reads it into ECX, and the order in which the position
// 0x11eed0 and 0x1c6920 calls set their arguments up.
// MiniMax G (11-11): no new form, no lower score (250); the frame 0x20 and the state test agree, but the index and index * 12
// sit in EDI/ESI where retail has ESI/EBX, the result stays in DL where retail keeps a stack byte, and the definition is re-read
// where retail reads it once; the rate local, the clamp as an if/else, the channel pointer and the position arm's earlier
// previous-value read all give the same code or a higher score.
// MiniMax C (11-11): no new form, no lower score (250): the full check counts the same as the reduced
// one (250 at 1479 bytes against retail's 1495), so the lead in the note below (dropping the definition
// pointer's volatile, a rate local just before the power call, `bool result = false;` after the
// definition and the 0xb3 byte as `<= 0`) is three points and stays under the eight-point gate; the
// index and index * 12 still sit in EDI/ESI where retail has ESI/EBX, and the result stays in DL.
// Earlier (250, was 250): the same body measured 248 at lane 14 round 62 and 250 in round 79's baseline. Earlier (248, was 285):
// frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// where retail has ESI/EBX, the result in DL where retail keeps a stack byte, and the definition's re-reads.
// MiniMax R (11-06): no new form, no lower score (248); the frame agrees and the registers are pushed in the order EBP, ESI, EDI here and EBX, EBP, ESI in retail.
// Walk 11-06 (b5P, statement by statement against retail 0x106790..0x106d64, no behaviour difference found): entry
// (datum, definition = table[def & 0xffff] + 8, state from the +0x12a offset, flags &= ~0x100); the manual power arm
// (0x106810-0x10689f: 0x107cc0 with previous in xmm0 and &position, velocity = (position - previous) / rate, 0x20 set
// or cleared, 0x107520, result); the power-group arm (0x1068a4-0x106b70: idle test value/power/velocity/0x80 -> delay 0;
// channel guard + 0x1c6620; acceleration = 0xcc*pos + 0xd4*(1-pos), speed = 0xc8*pos + 0xd0*(1-pos); 0x100 from
// acceleration > 0 && speed > 0; delay test with delay_ticks (movsx) * rate >= +0x10c, power == 0, power > value; fabs clamp;
// 0x11eed0 with wrap = def[0xbc] & 1; event ids 0xf0/0xf8 reached, 0xe0/0xe8 reversal; refresh + advance on change or
// 0x80); the manual position arm (0x106b7f-0x106bc9); the position-group arm (0x106bd3-0x106cd9, speed +0xc0, acceleration
// +0xc4, wrap false); the 0x80 tail (type bit 7 -> 0xbd020 + 0x115b10, byte +0xb3 == 0 or index NONE clears 0x80, returns true).
// Every field, condition direction, constant, callee and argument agrees; all float products and sums are in retail's
// order (the 0xcc/0xd4 pair and the 0xc8/0xd0 pair are added in retail's order, the 0.0f - speed negation is retail's
// xorps + subss) and there is no fst/fstp dword rounding point in retail to keep (fabs is the one x87 use, on the
// velocity copy in memory). What is left (248) is the callee-saved permutation (retail pushes EBX, EBP, ESI, with index * 12
// in EBX), the result as a stack byte at [esp+0xf] (ours DL), and one dead store retail makes before the power 0x11eed0
// call (rate into the speed's slot, 0x106aa3). Tried: the rate read once into a local `time_step` for the delay product
// and the call 334 (frame 0x24).
// Earlier 248 (was 248): earlier 285; frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// Earlier (248, was 248): earlier 285; frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// 285 before lane 14 round 62. Frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// 285 before lane 14 round 62. Frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// Earlier (248, was 248): 285 before lane 14 round 62. Frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// earlier 285; frame 0x20 and the state test as retail; left are the index and index * 12 in EDI/ESI
// where retail has ESI/EBX, the result in DL where retail keeps a stack byte, and the definition's re-reads.
// Earlier (285, was 285): our frame is 0x1c and retail's 0x20, and the index, header, state and result
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
// DH round 262: byte/short/long/dword active-channel counts 320/409/411/411; entry initialization
// 320/409/411/411. Frame remains 0x1c versus 0x20; original activity result and complete body restored.
// Round 229 (lane P): native header type-byte field unmeasured; original record and body
// restored after the check stalled. Frame/index/state/result homes remain as above.
// Round 276: actual header/device byte members 294/294; with ordinary index,
// header/device/both 395/395/395; with ordinary saved pointer 386/386; both
// with ordinary index and saved pointer 412. All declarations and storage restored.
// Round 285 (N): settled-power conjunction orders and complementary active-first
// admission graphs 300/419/419/353/353/419/353/353. Original field tests, body and inherited
// qualifiers restored; frame/index/header/state/result homes remain.
// DH round 262: byte/short/long/dword active-channel counts 320/409/411/411; entry initialization
// 320/409/411/411. Frame remains 0x1c versus 0x20; original activity result and complete body restored.
// Round 305: finite first-motion/integration/second-motion completion domains,
// subsets 1..7, 389/327/407/344/395/341/423; all with finite activity 433. Original
// bool carriers, activity, inherited qualifiers and body restored; frame/homes remain.
// Round 38 (m3 lane 2, fast; masked-aligned counts in brackets, this body 294 [156]): plain index with both
// event ids as selects between two field reads 367 [123] (1488 bytes, frame 0x20; slots F+0xc previous, F+0x10
// index * 12, F+0x14 position where retail has index * 12, position, previous; index*12 in ESI and the index in
// EDI where retail has EBX/ESI, EBX pushed late, the result in DL); that with a plain definition 405 [130];
// `previous` renamed (sorting before `position`) on that form 367 and on this body 294 (same code each); the
// position local renamed twice on that form 367/367 (same code): the slot order follows neither name.
// Round 310: Shared real group and repeated channel receivers across both group phases,
// with ordinary index/saved pointer/result-first storage subsets 0..7:
// 360/378/372/435/360/378/372/436. Original receivers, scalar qualifiers and
// body restored; frame/index/header/state/result homes remain.
// M304: Word header ordinal with entry false/null and ordinary saved-pointer subsets 0-7:
// 375/375/375/375/415/415/416/415. Inherited-pointer forms recover frame 0x20 but regress;
// original full handle/header observations, pointer home and entire body restored.
// Round 312: Native four-real coefficient-block receiver with inherited/ordinary index and
// saved-pointer storage: 370/389/378/420; result-first variants 370/389/378/421.
// Original scalar domains reach retail length or frame separately but regress; temporary
// rate record, receiver, declarations and whole body restored. Frame/index/header/state
// and activity-result homes remain.
// Round 333: Complete typed tag receiver and direct selected event fields, with ordinary
// index/saved-pointer/result-first domains: 342/367/400/405/342/367/400/406. All regress; original
// tag type, both declarations, qualifiers, field observations and body restored.
// F344: Native external power/position/activity flag-bit views, seven subsets,
// 294/294/369/294/369/369/369; all explicit-bool tests 458. Other 29 file scores
// unchanged; frame stays 0x1c versus 0x20. NOT kept: activity stores and bool
// extraction grow nonretail code; original word layout and complete body restored.
// Round 42 (m3 lane 2): retail's channel-0 tests are 0x0b6760's shape inlined (`mov ecx, [ebp + 0x18c]; cmp ecx, -1;
// lea reg, [ebp + 0x18c]; cmp word [reg + 6], -1`, the pointer then `this` in ESI), so a `power_channel` pointer to
// channels[0] serves both tests and both calls: 294 -> 285 (1496 bytes; kept). Tried: the two channel-0 tests as calls
// to function_0b6760 383 (plain index 376; all four sites stay out of line); the pointer on the plain index with
// both event-id selects 302 (1476 bytes); the pointer with a direct `if` for the first test 292; a second pointer
// for channels[1] 287; the definition read after the state block 286; event ids as selects between two field
// reads, both/either 331/317/317 (1506/1501 bytes); a plain (non-volatile) definition pointer 386.
// Round 144 (m3 lane 6, fast; [LCS-aligned / register-masked aligned] this body [172 / 122], 398 instructions
// against 394): plain index with both event-id selects re-measured 302 [137 / 72] (1476 bytes); that with
// block-scoped `previous` locals (one per phase, as retail's separate +0x14 / +0x1c homes) 298 [121 / 53]; that with
// a plain definition pointer 431 [154 / 90] (1449 bytes); plain index, selects, scoped `previous` and the 0x8 path
// as `if (finished) flags |= 0x20; else flags &= ~0x20; function_107520(device_index);` 343 [110 / 48] (1470 bytes;
// VC then hoists the flags load and duplicates the call into both arms, as retail); on this body the 0x8 path as
// that single call 364 [167 / 122], scoped `previous` alone 290 [179 / 130]. The 48-row form leaves the callee-saved
// roles (ours index*12 ESI, device_index EDI, group EBX, velocity pointer ESI; retail EBX, ESI, ESI, EBX), the result
// in DL (retail a stack byte), the state offset loaded once (retail compares memory and re-reads it), the volatile
// definition re-read four times in the acceleration block (retail once into EDX) and the group pointer's spill
// around the 0x1c6620 call.
// F354: Complete channel-array receiver across manual and group phases, ordinary
// index/saved-pointer/result-first domains 376/408/354/437/376/408/354/437.
// NOT kept: no byte match; original body and qualifiers restored; frame/index/state homes remain.
// Round 81 (m3 lane 9, fast): the header type byte read through s_device_header (DEVICE_GET's own shape) 285
// (same code); with a plain index 411 (1466 bytes: retail's 0x20 frame and the index*12 CSE spilled at
// [esp+0x1c], but in ESI/EDI where retail has EBX/ESI); the state test through the existing
// device_has_animation_state on this body 393 (1512); on the plain index 384 (1469 bytes: retail's entry from
// the state test on, `cmp word [ebp+0x12a], -1` then a second movsx read, state at [esp+0x14]; left: the result
// in DL where retail keeps a stack byte at [esp+0xf], index*12 in ESI not EBX); that without the volatile
// definition 426 (1437); with the result declared first 384.
// F354: Complete channel-array receiver across manual and group phases, ordinary
// index/saved-pointer/result-first domains 376/408/354/437/376/408/354/437.
// NOT kept: no byte match; original body and qualifiers restored; frame/index/state homes remain.
// F362: Existing state getter as actual device member, ordinary/forced expansion,
// with inherited/ordinary index and saved-pointer storage: 285/285/411/411/382/382/442/442.
// NOT kept: no frame/state/index repair or byte match; original getter, callers and body restored.
// Round 404: Repeated position/power velocity cursors 294/372/379; complete position/power/both output
// pairs 373/403/398; both with ordinary index/saved pointer 406/371. Original receivers and qualifiers
// restored; frame/index/state/result homes remain.
// P308: Flattened pending-manual power/position decisions and closing refresh-bit admission,
// subsets 1/2/3/4/5/6/7: 377/286/377/292/381/291/381; both direct conjunctions 408.
// NOT kept: no match or gain; original nested decisions, qualifiers and full body restored.
// P310: Reached/reversal shared event dispatch as value+pending/offset/field/admission:
// 285/331/327/291; ordinary-index forms 399/392/340/400. NOT kept: no match;
// original event observations, both inherited qualifications and full body restored.
// DH338: First-channel receiver across manual/group phases, pointer/reference,
// inherited/ordinary index and saved-pointer domains: 376/376/393/393/392/392/441/441.
// All restored; frame/index/header/state/result homes remain.
// B429: Existing shared power/position/activity flag writers, subsets 1/2/4/3/5/6/7,
// then all plus the refresh clear 367/285/387/367/371/387/371/371. NOT kept: no gain;
// original frame, scalar homes, flag publications and complete callback restored.
// M375: Complete closing refresh admission, bool/byte/short/long positive then complementary
// result: 285/285/285/285/285/285/285/285. NOT kept: one-use decisions did not match; original frame,
// observations, both inherited scalar qualifications and complete body restored.
// Round 62 (m3 lane 14, fast): device_channel_valid on channels[0] or on the pointer, old body 288/285 (1500/1496
// bytes); plain index, event-id selects, scoped `previous`, one 0x107520 call on the 0x8 path and
// device_channel_valid 360 (1478); that with device_has_animation_state 248 (1479 bytes, kept: retail's
// `cmp word [ebp+0x12a], -1`, then a second movsx read). On it: a plain definition pointer 386 (1464, frame 0x24);
// the closing block setting the result 248 (same code); the b3 byte `<= 0` 248; the result declared first 248; the
// state before the definition 249; the result after the state 248; a local tag table before the definition or
// before the device, or read after the device header 248/248/248; one bool for the result and the direction 248
// (same code); byte/char result 243/243 (1482). A volatile result (diagnostic, reverted at once) reaches retail's
// 1495 bytes, 231: the stack byte is most of the length gap; it crosses no call on any path.
// J259: Activity from manual/group power and position tests, separate/paired/all with logical/bitwise joins: 396/417/404/329/359/380/470/424.
// NOT kept: no eligible native repair; opening executable source restored. Native index/header and result residency remain; group-result forms grow the frame to 0x24.
// Round 107 (m3 lane 9, fast; the compile-only screen does not reproduce this body, 396 at 1512 B): retail calls
// 0xb6760 at both channel-1 sites and inlines only the channel-0 tests, as this body does. The 0x20 completion flag
// as a ternary like the 0x100 update 345 (1480 B), with the 0x40 flag too 345; the one-use `reached` result dropped
// (the call in the `if`) 248 (same code); the 0x100 update as an if/else 366 (1474 B); `result` renamed 248 (same
// code); `local_c01284_2` renamed previous_velocity 248 (same code, a re-measure of the previous-velocity name).
// Retail's result byte shares its stack slot with the `increasing` byte ([esp+0x13] after the late push), so retail
// keeps the result in memory and its false value is dead through the acceleration block.
// m3 lane 11 r83 (fast; aligned/masked against this body's 113/46): `result = true;` before the power manual arm's
// 0x107520 call 386 (the result goes to a stack byte as in retail, but device_index loses its callee-saved register;
// aligned 138); the same in the position manual arm too 386; the rate read into a local just before the power-group
// 0x11eed0 call 254 (1478 B, aligned 105/40: retail's dead store `mov [esp+0x38], ecx` of the rate into the speed's slot
// and g_510c54 kept in EDX through the arm; a one-use local, so a lead for a match only); that with the 0xb3 byte
// `<= 0` 254 (104/39; `<= 0` alone re-measured 248); `increasing` computed before the acceleration products 248
// (136/55); the delay product through a file-local game_ticks_to_seconds (the idiom of src/unknown_16bd10.cpp) 248.
// Round 76 (m3 lane 13, fast): retail's result byte shares one stack slot with `increasing` (entry esp - 0x1d), and retail
// spills the power group pointer around the `this`-in-ESI channel call (that slot then holds the speed). The definition
// declared before the result (retail's entry store order) 248 (same code); `increasing` first in the else block 248; the
// `reached` call inside its `if` 248; `result = true;` first in the manual power block 352 (1498 bytes), in both manual
// blocks 384 (1517). On the current body: a byte result with the definition first 243 (1482; the byte result is 243 above;
// the result still in DL), the acceleration as one expression 359 (1476; plain 303 on the old body).
// Round 107 (m3 lane 9, fast; the compile-only screen does not reproduce this body, 396 at 1512 B): retail calls
// 0xb6760 at both channel-1 sites and inlines only the channel-0 tests, as this body does. The 0x20 completion flag
// as a ternary like the 0x100 update 345 (1480 B), with the 0x40 flag too 345; the one-use `reached` result dropped
// (the call in the `if`) 248 (same code); the 0x100 update as an if/else 366 (1474 B); `result` renamed 248 (same
// code); `local_c01284_2` renamed previous_velocity 248 (same code, a re-measure of the previous-velocity name).
// Retail's result byte shares its stack slot with the `increasing` byte ([esp+0x13] after the late push), so retail
// keeps the result in memory and its false value is dead through the acceleration block.
// m3 lane 13 r79 (fast, base 250): `result = true;` at the top of the power-group else block (before the channel call)
// 352 (1466 bytes); the post-call velocity read once into a local used by the reversal tests 307 (1473); the four motion
// coefficients read through a block-local copy of the definition pointer 376 (1464: one load into a register as retail,
// but the frame and homes move); both group pointers const 250 (same code); the previous power read before the velocity
// copy 250 (same code). Our result stays in DL because its value never crosses a call; retail's byte shares the
// `increasing` slot. Channel 1's test calls 0xb6760 (src/unknown_0b66c0.cpp, built /Ob1, so LTCG cannot inline it here as
// retail does at the channel-0 sites).
// m3 lane 15 r74 (fast, base 250, aligned 115 / register-masked 44): the tick rate read once at entry into a local used
// by all four rate reads 316 (1477 B, masked 47). Same code as the base (spellings): the power group index in a named
// local inside a nested else, the clamp test as `speed < fabs(v)`, `if (++device->delay_ticks == 1)`. Diagnostic only
// (a volatile result, reverted at once, lane 14 r62's listed form): 234 at 1495 B, device_index then stays in its
// argument slot and the callee-saved roles stay permuted. Roles by region: ours ESI = index*12 (entry, tail) and the
// power-velocity pointer, retail EBX; retail gives the power-group pointer ESI (spilled around the `this`-in-ESI
// channel call) where ours gives it EBX; the position-group arm already agrees (group EBX, position EDI, channel ESI).
// Lead (fast, match-only): drop the definition pointer's `volatile` and read the rate into a local just before the
// power 0x11eed0 call: 249 at 1495 B (first difference +0x3 -> +0x44, masked 44 -> 26) with retail's prologue (EBX, EBP,
// ESI, EDI late), index*12 EBX, device_index ESI and the result byte at [esp+0xf]; plus `bool result = false;` after the
// definition 247 (+0x85, masked 24); plus the 0xb3 byte `<= 0` 247 (masked 23, 'p2'); plus the 0x100 update as an
// if/else 262 at 1487 B but masked 11 ('q2'). Left on q2: the acceleration block's float colouring (retail complement
// xmm1, 0xcc product xmm0, zero xmm1; ours complement xmm0, zero xmm0, one `movaps xmm0, xmm1` missing) and the
// position-group refresh (ours state ECX across the 0xb6760 call and device_index EBX; retail state EBX, device_index
// reloaded per call). On p2/q2 the acceleration spellings (one expression, both pairs named, shared complement, operands
// swapped, complement term first, speed first) give 247/262 (same code or masked 22/10), a rate local before the
// position call too 247 (same code); a two-use time_step for the delay product and the call 280/276, at the inner else
// top for the call 143 (frame 0x24, masked 28), at entry for all four reads 383, for both calls 376; `increasing`
// before the products 143 (1478 B, masked 35). Also tried here (old base): the rate local at the inner else top 335,
// the clamp as an if/else 361; the rate local alone re-measured 256 (masked 38).
// On q2: a channel pointer for the position arm's two tests and calls 247 (1495 B, masked 25); the position arm's
// `previous` read before its channel test 253 (masked 15); the power `previous` read after the clamp 273 (masked 15);
// the position idle test as `!(value == position && velocity == 0.0f)` 262 (same code).
// m3 lane 10 r99 (fast unless noted; brackets = register-masked aligned count, 46 for this body): lane 15 r74's q2 rebuilt
// 262 [11] (1487 B). On q2: acceleration as one expression 262 [11]; a shared complement local in both rows 262 [11]; speed
// before acceleration 263 [12]; all four products named, then both sums 262 [11]; the position products named in both rows
// (`position_acceleration`, `position_speed`, complement terms written out; MiniMax b's running-value shape) 69 at 1495 B
// [6], first difference +0x3f3: on it the position arm's first channel test already keeps the channel in ESI as retail;
// left are the state temporary (EAX, retail ECX) in the manual position arm and the refresh after the position 0x11eed0
// call (ours device_index in EBX and the state in ECX across the 0xb6760 call, retail the state in EBX and device_index
// reloaded for each push). Ablations with those named products: alone on this body 359 [45] (1476 B); without volatile and
// with the if/else 266 [15]; + `<= 0` 266 [14]; + the rate local 71 [9]; + the result after the definition 264 [13]; without
// volatile + rate + result + if/else 69 [7]; without volatile + rate + result + `<= 0` with the ternary kept 58 [10]
// (MiniMax b's 58; plain 58, callers 31/13), without `<= 0` 59 [11]. A 600 s permuter run on the 58 form (anneal, one job,
// about 20 variants) found none below 58. On the 69 form: `state` initialised by one conditional expression 404 (1476 B); a
// channel pointer for the position-group arm's tests and calls 58 [12] (the first channel test back in ECX); the state
// declared before the definition and result 72 [8]; a `const` definition pointer 69, a rate local for the position call too
// 69, `state != NULL` in the refresh 69, the manual position arm's call tested directly 69 (all four the same code); the
// position-group `previous` read before its channel test and passed to it 96 [10]. Renaming the manual arm's `finished`
// gives the same code. Retail does keep ECX live across calls to small leaves elsewhere (0x22eb8c after 0x22e37f), so
// the refresh is an allocation choice, not a missing callee summary.
// MiniMax A (11-13): no new form, no lower score (250): this build pushes ebp and esi where retail pushes ebx and ebp, and the
// body is 16 bytes shorter than retail's. This round: dropping the `volatile` on the saved definition pointer, an earlier
// round's lead, gives 386 at a 0x24 frame, so the qualifier stays. This round: the result local as a `byte` instead of a
// `bool` scores 245 (the prologue and the result in DL do not change), a gain of five that is under the eight-point gate, so it
// is reverted; the permuter (300 s, five tries) kept the baseline.
// body is 16 bytes shorter than retail's.
// P449: Power-change/position-activity, power preparation/advance, position preparation/advance,
// position-change and lazy reversal decisions: eight 250s. NOT kept: no byte equality or native result-home repair.

// MiniMax G (11-14), NOT kept (coordinator: the 250 -> 58 rests on the one-use `real rate = g_510c54->rate;` local
// passed to function_11eed0, a form kept only with a byte match; measured on G's body without that local: 380, and
// main without its volatile alone: 386; `== 0` for the `<= 0` byte test: 59; `bool increasing` for the reused
// result: 58). G's three notes on that form follow.
// G's form, not kept: score 58 (was 58): unchanged this round: the length is retail's 1495 bytes; the position-group arm keeps the
// channel pointer in ecx where retail re-leads it in esi, and the flags-and-0x10 arm puts the state pointer in eax
// where retail keeps it in ecx; the permuter (600 s) and the naming trials found no lower form.
// G's form, not kept: score 58 (was 250): the acceleration products are named with the position term first, the 0xb3 byte test is `<= 0`
// and the result is a bool, so the length is retail's 1495 bytes; left: the position-group arm keeps the channel
// pointer in ecx (retail esi) and the state in eax/ecx across the 0xb6760 call (retail ebx); the permuter found none.
// G's form, not kept: score 65 (was 250): the acceleration products are named with the position term first and the 0xb3 byte test is `<= 0`,
// so the frame and the register roles now agree with retail up to +0x3f3; the position-group arm still keeps the
// channel pointer in ecx (retail esi) and the state in eax/ecx across the 0xb6760 call (retail ebx), 3 bytes longer.
// P453: outer manual output 250; outer interpolation results 250;
// outer acceleration products 250; outer integration snapshots 250;
// separate power/position group declarations at function scope 250/250;
// outer first/second manual completions 250/250. NOT kept: no gain;
// original observations, pointer qualifier and body retained.
// P474: Standard typed products/sum/differences/quotient/ordered tests/
// equality tests, complete arithmetic and complete scalar contracts.
// Frame 0x20 agrees; saved-register and activity-byte homes remain.
// Scores 386/360/462/392/448/432/532/659. NOT kept: ties/regressions;
// original executable body and helper contracts restored.
// @retail 0x106790
bool __stdcall function_106790(long device_index)
{
	s_device *device = DEVICE_GET(device_index);
	bool result = false;
	byte const *volatile saved_definition = g_4e3b44[device->definition_index & 0xffff].bytes;
	s_animation_state *state = NULL;
	if (device_has_animation_state(device_index))
		state = device_get_animation_state(device);
	device->flags &= ~0x100;
	if (device->flags & 8)
	{
		if (!(device->flags & 0x20))
		{
			real previous = device->power;
			real position;
			bool finished = function_107cc0(device_index, &device->motion_14c,
				&device->channels[0], state, previous, &position);
			device->power = position;
			device->power_velocity = (position - previous) / g_510c54->rate;
			if (finished)
				device->flags |= 0x20;
			else
				device->flags &= ~0x20;
			function_107520(device_index);
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
			if (state && device_channel_valid(&device->channels[0]))
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
				real previous = device->power;
				if (fabs(local_c01284_2) > speed)
					device->power_velocity = increasing ? speed : 0.0f - speed;
				bool reached = function_11eed0(&device->power, &device->power_velocity,
					group->value, g_510c54->rate, speed, acceleration,
					(*(byte const *)(saved_definition + 0xbc) & 1) != 0, 0.0f, 1.0f);
				if (reached)
					function_107980(device_index, increasing ? *(long const *)(saved_definition + 0xf0) : *(long const *)(saved_definition + 0xf8));
				else if (device->power_velocity != 0.0f && device->power_velocity * local_c01284_2 <= 0.0f)
					function_107980(device_index, device->power_velocity > local_c01284_2 ? *(long const *)(saved_definition + 0xe0) :
						*(long const *)(saved_definition + 0xe8));
				if (previous != device->power || (device->flags & 0x80))
				{
					function_107520(device_index);
					if (state && device_channel_valid(&device->channels[0]))
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
			real previous = device->position;
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
