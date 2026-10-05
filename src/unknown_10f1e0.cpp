// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10F1E0.CPP: a unit's animation control: resetting the state at the
   object's offset +0x33e, and starting a unit's base animation, its weapon
   animations and its overlays on the animation state at offset +0x12a
   (unknown_1cafc0.cpp). The neighbouring queries are in unknown_10db60.cpp,
   unknown_10dc70.cpp and unknown_10ee20.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"
#include <string.h>

/* the unit (a view of the object data) */
struct s_unit_animation_object
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x10a - 0x18];
	word flag0 : 1;
	word flag1 : 1;
	word animation_frozen : 1;
	word : 13;
	byte unknown10c[0x12a - 0x10c];
	short animation_state_offset;
	byte unknown12c[0x13c - 0x12c];
	long field_13c;
	byte unknown140[0x33e - 0x140];
	short control_offset;
};

struct s_unit_animation_object_header
{
	byte unknown00[8];
	s_unit_animation_object *object;
};

/* a 4 byte state of the control, five of them at +0x80 */
struct s_unit_animation_slot
{
	byte unknown0;
	byte unknown1;
	byte unknown2;
	byte unknown3;
};

/* the state at the unit's offset +0x33e */
struct s_unit_animation_control
{
	union
	{
		word flags;
		struct
		{
			word flag0 : 1;
			word overlay : 1;
			word : 5;
			word flag7 : 1;
			word flag8 : 1;
			word : 7;
		};
	};
	byte unknown02;
	byte unknown03;
	long unknown04[4];
	long unknown14[4];
	byte unknown24[0x28 - 0x24];
	long unknown28;
	byte unknown2c[0x7c - 0x2c];
	short marker_7c;
	short marker_7e;
	s_unit_animation_slot slots[5];
	long weapon_class;
	long weapon_type;
	c_animation_channel channel_9c;
	c_animation_channel channel_bc;
	c_animation_channel channel_dc;
	c_type_709360 animation_fc;
	c_type_709360 animation_100;
	c_type_709360 animation_104;
	c_type_709360 animation_108;
	c_type_709360 animation_10c;
	c_type_709360 animation_110;
	c_type_709360 animation_114;
	c_type_709360 overlays[3];
};

/* the unit's definition (its model at +0x38) and the model's (its animation
   graph at +0x14) */
struct s_unit_animation_definition
{
	byte unknown00[0x38];
	long model_tag_index;
};

struct s_unit_animation_model
{
	byte unknown00[0x14];
	long graph_tag_index;
};

#define UNIT_ANIMATION_OBJECT(index) (((s_unit_animation_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define UNIT_ANIMATION_STATE(unit) ((s_animation_state *)((byte *)(unit) + (unit)->animation_state_offset))
#define UNIT_ANIMATION_CONTROL(unit) ((s_unit_animation_control *)((byte *)(unit) + (unit)->control_offset))
#define TAG_BYTES(index) (g_4e3b44[(index) & 0xffff].bytes)
#define GRAPH_GET(index) ((s_graph_tag *)TAG_BYTES(index))

bool g_5107f8;

/* the names whose animations mirror each other */
const long g_468684[4][2] =
{
	{ 0xa000014, 0x9000015 },
	{ 0x9000015, 0xa000014 },
	{ 0x9000016, 0xa000017 },
	{ 0xa000017, 0x9000016 },
};

void function_10f040(long object_index);
void function_10e920(long object_index);
void function_10dbc0(long object_index);
long render_model_find_named_entry(long render_model_index, long name);
void function_11b710(long unit_index, long field_7c);
c_type_709360 function_1dd0b0(s_graph_tag *graph, long name);
s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id);
void function_ba350(long object_index, real seconds);

struct s_1d9240;
void function_1d9240(s_1d9240 *slot, char flag, real seconds);

// @retail 0x113d20
void function_113d20(long unit_index, real seconds)
{
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		function_1d9240((s_1d9240 *)&UNIT_ANIMATION_CONTROL(unit)->slots[2], false, seconds);
	}
}

// @retail 0x113d60
void function_113d60(long unit_index, real seconds)
{
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		function_1d9240((s_1d9240 *)&UNIT_ANIMATION_CONTROL(unit)->slots[2], true, seconds);
	}
}

PRIVATE __forceinline long animation_slot_finished(s_unit_animation_control const *control)
{
	if (control->slots[2].unknown1 && !(control->slots[2].unknown3 & 1) && (control->slots[2].unknown3 & 2))
		return 1;
	return 0;
}

// @retail 0x113da0
long function_113da0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	if (!control->slots[2].unknown1)
		return 1;
	return animation_slot_finished(control);
}

// @retail 0x113df0
long function_113df0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	if (control->slots[2].unknown1 && (control->slots[2].unknown3 & 1) && (control->slots[2].unknown3 & 2))
		return 1;
	return 0;
}

static __forceinline long mirrored_name_get(long name)
{
	for (long i = 0; i < 4; i++)
	{
		if (name == g_468684[i][0])
			return g_468684[i][1];
	}
	return NONE;
}

static inline bool name_is_mirrored(long name)
{
	return name == 0xa000014 || name == 0x9000015 || name == 0x9000016 || name == 0xa000017;
}

static inline void quad_clear(long *quad)
{
	quad[0] = 0;
	quad[1] = 0;
	quad[2] = 0;
	quad[3] = 0;
}

static inline bool channel_refresh_if_valid(s_animation_state *state, c_animation_channel *channel, long weapon_class,
	long weapon_type)
{
	bool result = false;

	if (state->graph_tag_index != NONE)
		result = state->channel_refresh(channel, weapon_class, weapon_type);
	return result;
}

static inline void slot_reset(s_unit_animation_slot *slot, byte value)
{
	slot->unknown0 = 0;
	slot->unknown1 = 0;
	slot->unknown2 = value;
	slot->unknown3 = 0;
}

// @retail 0x113870
void function_113870(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);

	slot_reset(&control->slots[0], 1);
	slot_reset(&control->slots[1], 1);
	slot_reset(&control->slots[2], 2);
	slot_reset(&control->slots[3], 1);
	slot_reset(&control->slots[4], 1);
}

// @retail 0x114330
void function_114330(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	long weapon_class = state->unknown74;
	long weapon_type = state->unknown78;

	if (state->graph_tag_index != NONE)
	{
		c_type_709360 animation_id = GRAPH_GET(state->graph_tag_index)->overlay_get(state->unknown70, weapon_class,
			weapon_type, 0x10000551, NULL, NULL, NULL);

		if (animation_id.index != NONE &&
			state->channel_start(&control->channel_dc, animation_id, 0x10000551, NONE, NONE, 1, 0x803f))
		{
			control->flag7 = false;
		}
	}
}

// @retail 0x113410
void function_113410(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);

	if (state)
	{
		long weapon_class = state->unknown74;
		long weapon_type = state->unknown78;
		long other_weapon_type = control->weapon_type;
		long first_set = NONE;
		long second_set = NONE;

		switch (state->unknown7c)
		{
		case 0x400004a:
		case 0x5000049:
		case 0xc000058:
			if (unit->parent_index != NONE)
				break;
		case 0x400000c:
		case 0x700005c:
		case 0x700005d:
		case 0x800001e:
		case 0x900000e:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xb000059:
		case 0xb00005a:
		case 0xc00005b:
			first_set = 0xc000026;
			second_set = 0xe000028;
			break;
		case 0x9000015:
		case 0x9000016:
		case 0xa000014:
		case 0xa000017:
		case 0xa000019:
		case 0xa00001a:
		case 0xa00002d:
		case 0xb000018:
		case 0xb00001b:
		case 0xb00002e:
		case 0xc000033:
		case 0xc000034:
		case 0xd000032:
		case 0xd000035:
			first_set = 0xb000027;
			second_set = 0xd000029;
			break;
		}

		control->animation_fc = animation_state_overlay_or_animation_get(state, weapon_class, first_set, weapon_type);
		control->animation_100 = animation_state_overlay_or_animation_get(state, weapon_class, second_set, weapon_type);
		control->animation_114 = animation_state_overlay_or_animation_get(state, weapon_class, 0x400004b, weapon_type);
		if (weapon_class == 0x400054b)
		{
			control->animation_104 = animation_state_overlay_or_animation_get(state, weapon_class, 0xb00054a, weapon_type);
			control->animation_108 = animation_state_overlay_or_animation_get(state, weapon_class, 0xb00054a,
				other_weapon_type);
			control->animation_10c = animation_state_overlay_or_animation_get(state, weapon_class, 0x400060b, weapon_type);
			control->animation_110 = animation_state_overlay_or_animation_get(state, weapon_class, 0x400060b,
				other_weapon_type);
		}
		else
		{
			control->animation_104 = c_type_709360();
			control->animation_108 = c_type_709360();
			control->animation_10c = animation_state_overlay_or_animation_get(state, weapon_class, 0x400060b, weapon_type);
			control->animation_110 = animation_state_overlay_or_animation_get(state, weapon_class, 0x400060b, weapon_type);
		}
		function_114330(unit_index);

		bool any_overlay = false;
		for (long i = 0; i < 3; i++)
		{
			control->overlays[i] = state->overlay_kind_get(i);
			if (control->overlays[i].index != NONE)
				any_overlay = true;
		}
		if (any_overlay)
			control->overlay = true;
		else
			control->overlay = false;
	}
}

// @retail 0x113740
void function_113740(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);

	state->channel_refresh(&control->channel_9c, 0x7000101, 0x7000101);
	state->channel_refresh(&control->channel_dc, 0x7000101, 0x7000101);
	state->channel_refresh(&control->channel_bc, 0x400054b, control->weapon_type);
}

// @retail 0x1137b0
real function_1137b0(long set, bool other, real minimum, bool changed, long current_set)
{
	real result = 0.267f;

	if (set == 0x9000020 || set == 0x900001f || set == 0x90006b2)
		result = 0.1335f;
	if (changed || other)
		result = 0.267f;
	if (current_set == 0x400000c)
	{
		if (name_is_mirrored(set))
			result = 0.267f;
	}
	else if (name_is_mirrored(current_set) && (name_is_mirrored(set) || set == 0x400000c))
	{
		result = 0.267f;
	}
	return result > minimum ? result : minimum;
}

// @retail 0x10f260
void __stdcall function_10f260(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);

	function_10f040(unit_index);
	control->unknown28 = NONE;
	control->flags = 0;
	memset(control->unknown04, 0, sizeof(control->unknown04));
	memset(control->unknown14, 0, sizeof(control->unknown14));
	control->unknown02 = 0;
	control->unknown03 = 0;

	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	control->marker_7c = (short)render_model_find_named_entry(definition->model_tag_index, 0x80001a2);
	control->marker_7e = (short)render_model_find_named_entry(definition->model_tag_index, 0x90001a3);

	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state)
	{
		state->unknown74 = NONE;
		state->unknown78 = NONE;
	}
	function_10e920(unit_index);
	function_10dbc0(unit_index);
	function_113870(unit_index);
	control->weapon_class = NONE;
	control->weapon_type = NONE;
}

// @retail 0x10f1e0
void function_10f1e0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	long graph_tag_index = ((s_unit_animation_model *)TAG_BYTES(definition->model_tag_index))->graph_tag_index;

	UNIT_ANIMATION_CONTROL(unit)->unknown03 = 0;
	if (state->graph_tag_index != graph_tag_index)
	{
		state->initialize(graph_tag_index, definition->model_tag_index, true);
		state->unknown74 = NONE;
		state->unknown78 = NONE;
		function_10f260(unit_index);
	}
}

// @retail 0x10f430
bool __stdcall function_10f430(long unit_index, long mode, long weapon_class, long weapon_type, long set, real blend,
	bool force, long flags)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	long current_set = state->unknown7c;
	long old_mode = state->unknown70;
	bool result;

	if (current_set != NONE)
	{
		if (current_set == set || set == 0x7000101)
			flags |= 0x20;
		else if (current_set == mirrored_name_get(set))
			flags |= 0x40;
	}
	if (unit->field_13c != NONE && !g_5107f8)
		flags |= 0x200;
	if (mode == 0x7000101 && set == 0x7000101)
		result = true;
	else
		result = state->animation_set(mode, weapon_class, weapon_type, set, flags, 0x3f);
	if (result)
	{
		bool mode_changed = old_mode != mode && mode != 0x7000101 || force;
		bool set_changed = current_set != set && set != 0x7000101 || force;

		if (mode_changed)
		{
			function_113740(unit_index);
			if (state->overlay_exists())
				control->overlay = true;
			else
				control->overlay = false;
		}
		if (mode_changed || set_changed)
		{
			real seconds = function_1137b0(set, false, blend, mode_changed, current_set);

			if (seconds > 0.0f && !(bool)(((dword)(short)state->flags >> 4) & 1))
				function_ba350(unit_index, seconds);
			function_113410(unit_index);
		}
	}
	return result;
}

// @retail 0x10f5c0
bool function_10f5c0(long unit_index, real blend, long flags, long mode, long set)
{
	return function_10f430(unit_index, mode, 0x7000101, 0x7000101, set, blend, false, flags);
}

// @retail 0x10fd40
bool function_10fd40(long unit_index, long weapon_type, long weapon_class, bool flag)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long old_weapon_class = state->unknown74;
	long old_weapon_type = state->unknown78;
	bool take_other = !flag && weapon_type != 0x7000001;
	bool drop_other = !flag && weapon_type == 0x7000001;

	if (TEST_FIELD_BIT(unit->animation_frozen))
		return true;

	long type = weapon_type;
	if (take_other)
	{
		weapon_class = 0x400054b;
		type = 0x7000101;
	}
	else if (drop_other)
	{
		weapon_class = control->weapon_class;
		type = 0x7000101;
	}

	bool result = state->animation_set(0x7000101, weapon_class, type, 0x7000101, 0x26, 0x3f);
	if (result)
	{
		bool class_changed = old_weapon_class != weapon_class && weapon_class != 0x7000101;
		bool type_changed = old_weapon_type != type && type != 0x7000101;

		if (class_changed || type_changed)
		{
			real seconds = function_1137b0(state->unknown7c, true, 0.0f, false, state->unknown7c);

			channel_refresh_if_valid(state, &control->channel_9c, 0x7000101, 0x7000101);
			channel_refresh_if_valid(state, &control->channel_dc, 0x7000101, 0x7000101);
			if (seconds > 0.0f && !(bool)(((dword)(short)state->flags >> 4) & 1))
				function_ba350(unit_index, seconds);
			if (take_other)
			{
				control->weapon_class = old_weapon_class;
				control->weapon_type = weapon_type;
				control->channel_bc.clear();
				result = channel_refresh_if_valid(state, &control->channel_bc, 0x400054b, control->weapon_type);
			}
			else if (drop_other)
			{
				control->channel_bc.clear();
				control->weapon_type = NONE;
				control->weapon_class = NONE;
			}
			function_113410(unit_index);
		}
	}
	return result;
}

// @retail 0x1101e0
bool function_1101e0(long animation_graph_index, long unit_index, long animation_name, bool flag, bool global_flag)
{
	bool result = false;

	if (UNIT_ANIMATION_OBJECT(unit_index)->animation_state_offset != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(animation_graph_index);

		if (graph)
		{
			if (function_1dd0b0(graph, animation_name).index == NONE)
				return false;

			s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
			s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
			s_animation_state *state = UNIT_ANIMATION_STATE(unit);
			s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);

			function_11b710(unit_index, 0x7000101);
			if (state->graph_tag_index != animation_graph_index &&
				!state->initialize(animation_graph_index, definition->model_tag_index, true))
			{
				return false;
			}

			c_type_709360 animation_id = function_1dd0b0(GRAPH_GET(state->graph_tag_index), animation_name);
			function_10f260(unit_index);

			word channel_flags = flag ? 0x4003 : 0x4001;
			if (function_1daea0(GRAPH_GET(state->graph_tag_index), animation_id)->type)
			{
				control->channel_9c.clear();
				result = state->channel_play(&control->channel_9c, animation_id, channel_flags);
			}
			else
			{
				result = state->play(animation_id, channel_flags);
				if (result)
				{
					if (!global_flag)
						control->flag8 = true;
					else
						control->flag8 = false;
				}
			}
			function_114330(unit_index);
		}
	}
	return result;
}
