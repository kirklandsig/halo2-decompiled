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
#include "object_markers.h"
#include <xmmintrin.h>
#include <math.h>
#include "unknown_11cc90.h"

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
	byte unknown10c[0x116 - 0x10c];
	short node_matrices_offset;
	byte unknown118[0x12a - 0x118];
	short animation_state_offset;
	byte unknown12c[0x134 - 0x12c];
	byte unknown134[3];
	byte flags137;
	byte unknown138[4];
	long field_13c;
	byte unknown140[8];
	dword flags148;
	byte unknown14c[0x1f6 - 0x14c];
	char index1f6;
	char index1f7;
	byte unknown1f8[0x33e - 0x1f8];
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
	byte unknown3c[0xbc - 0x3c];
	dword flags;
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
bool g_4686a4 = true;

bool function_0c7070(long object_index);

// @retail 0x111010
bool function_111010(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = true;
	if (function_0c7070(unit_index))
		result = false;
	else
	{
		switch (UNIT_ANIMATION_STATE(unit)->unknown7c)
		{
		case 0x400004a: case 0x400076c: case 0x400076d:
		case 0x500000a: case 0x5000024: case 0x50000c3:
		case 0x5000768: case 0x5000769: case 0x500076a: case 0x500076b:
		case 0x600005f: case 0x600008c: case 0x60000cc: case 0x60000cd: case 0x60006ac:
		case 0x700000d: case 0x700002c: case 0x7000543: case 0x700076e:
		case 0x80000c4: case 0x800061e: case 0x800076f:
		case 0x900000e: case 0x9000011: case 0x9000012: case 0x900001f: case 0x9000020:
		case 0xa00000f: case 0xa000010: case 0xa000013: case 0xa00002d: case 0xa00003e:
		case 0xa000040: case 0xa00022d: case 0xa0005b9: case 0xa000767:
		case 0xb00002e: case 0xb00022e: case 0xb0005b2:
		case 0xc000073: case 0xc000075: case 0xc000077: case 0xc0006b3: case 0xc0006cd:
		case 0xd000021: case 0xd00002b: case 0xd00022c:
		case 0xe00002a: case 0xe000038: case 0xe00003b: case 0xe0000c2: case 0xe0000c3: case 0xe00067d:
		case 0xf00003a: case 0x1000006c: case 0x11000074: case 0x11000076: case 0x140005b3:
			result = false;
		}
	}
	return result;
}

real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference);

PRIVATE __forceinline short duration_ticks_1145f0(real seconds)
{
	long ticks;
	seconds *= g_510c54->field_2_3;
	__asm
	{
		fld seconds
		fistp ticks
	}
	return (short)ticks;
}

// @retail 0x1145f0
short function_1145f0(long definition_index, s_sound_permutation_reference const *reference, short type)
{
	if (definition_index != NONE)
		return duration_ticks_1145f0(sound_permutation_reference_duration(definition_index, reference));
	if (type == 12)
		return duration_ticks_1145f0(0.5f);
	return duration_ticks_1145f0(1.5f);
}

// @retail 0x113260
bool function_113260(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	bool result = false;
	if (!(bool)((definition->flags >> 11) & 1) && state->graph_tag_index != NONE &&
		state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE &&
		g_4686a4 && ((unit->flags148 & 0x8000) || (unit->flags137 & 1)) &&
		(unit->index1f6 != NONE || unit->index1f7 != NONE))
		result = true;
	return result;
}

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

// @retail 0x112dc0
void __stdcall function_112dc0(long unit_index, transform4x3f *matrix)
{
	s_unit_animation_object_header *headers = (s_unit_animation_object_header *)g_4e0300->data;
	s_unit_animation_object *unit = headers[unit_index & 0xffff].object;
	long parent_index = UNIT_ANIMATION_CONTROL(unit)->unknown28;
	if (parent_index != NONE && function_badc0(parent_index, (dword)NONE))
	{
		s_unit_animation_object *parent = headers[parent_index & 0xffff].object;
		transform4x3f *nodes = (transform4x3f *)((byte *)parent + parent->node_matrices_offset);
		function_142a60(nodes, matrix, matrix);
	}
}

/* Partial view of the callback table slot at 0x467a20. */
struct s_unit_matrix_callbacks
{
	void (__stdcall *update)(long, transform4x3f *);
};
s_unit_matrix_callbacks g_467a20 = { function_112dc0 };

void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_141ce0(real yaw, real pitch, real roll, transform4x3f *out);

PRIVATE __forceinline real angle_radians_112e20(real degrees)
{
	return degrees * 0.017453292f;
}

PRIVATE __forceinline real function_112e21(real arg_0)
{
    real local_0;
    __asm
    {
        rsqrtss xmm0, arg_0
        movss local_0, xmm0
    }
    return local_0;
}

// @retail 0x112e20
void function_112e20(transform4x3f *matrix, transform4x3f const *orientation, point3f const *target)
{
	static real yaw_limit = angle_radians_112e20(30.0f);
	static real pitch_limit = angle_radians_112e20(15.0f);
	real scale = matrix->scale;
	transform4x3f frame;
	function_1420f0(&frame, &matrix->position, &orientation->forward, &orientation->up);
	vector3f direction;
	if (frame.scale != 0.0f)
	{
		vector3f relative;
		vector3d_from_points3d(&frame.position, target, &relative);
		if (frame.scale != 1.0f)
		{
			real inverse = 1.0f / frame.scale;
			relative.i *= inverse;
			relative.j *= inverse;
			relative.k *= inverse;
		}
		direction.i = dot3f(&frame.forward, &relative);
		direction.j = dot3f(&frame.left, &relative);
		direction.k = dot3f(&frame.up, &relative);
	}
	else
		direction.i = direction.j = direction.k = 0.0f;
	real squared = direction.k * direction.k + direction.j * direction.j + direction.i * direction.i;
	if (squared != 0.0f)
	{
		real inverse;
		inverse = function_112e21(squared);
		direction.i *= inverse;
		direction.j *= inverse;
		direction.k *= inverse;
	}
	real yaw = (real)atan2(direction.j, direction.i);
	real pitch = (real)atan2(direction.k, sqrt(direction.i * direction.i + direction.j * direction.j));
	if (0.0f - yaw_limit > yaw)
		yaw = 0.0f - yaw_limit;
	else if (yaw > yaw_limit)
		yaw = yaw_limit;
	if (0.0f - pitch_limit > pitch)
		pitch = 0.0f - pitch_limit;
	else if (pitch > pitch_limit)
		pitch = pitch_limit;
	transform4x3f rotation;
	function_141ce0(yaw, pitch, 0.0f, &rotation);
	function_142a60(&frame, &rotation, matrix);
	matrix->scale = scale;
}


PRIVATE __forceinline void channel_reset_114240(c_animation_channel *channel)
{
	channel->graph_tag_index = NONE;
	channel->animation_id.graph_index = NONE;
	channel->animation_id.index = NONE;
	channel->frame_position = 0.0f;
	channel->unknown10 = 0;
	channel->unknown11 = 0;
	channel->flags = 0;
	channel->rate = 1.0f;
	channel->unknown14 = 0;
	channel->unknown16 = 0;
	channel->unknown08 = NONE;
	channel->unknown0c = NONE;
	channel->unknown0d = NONE;
	channel->unknown0e = NONE;
}

// @retail 0x114240
void function_114240(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	channel_reset_114240(&control->channel_9c);
	channel_reset_114240(&control->channel_bc);
}

// @retail 0x110fc0
bool function_110fc0(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = false;
	switch (UNIT_ANIMATION_CONTROL(unit)->channel_9c.unknown08)
	{
	case 0x9000008:
	case 0x9000009:
	case 0xa000066:
	case 0xf00067f:
		result = true;
	}
	return result;
}

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

// @retail 0x111590
bool function_111590(long unit_index, long bit)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = true;
	if (state->channels[0].animation_id.index != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(state->channels[0].graph_tag_index);
		s_animation *animation = function_1daea0(graph, state->channels[0].animation_id);
		if (animation && ((1 << bit) & *(word *)((byte *)animation + 0x18)))
			result = false;
	}
	if (control->channel_9c.animation_id.index != NONE)
	{
		s_graph_tag *graph = GRAPH_GET(control->channel_9c.graph_tag_index);
		s_animation *animation = function_1daea0(graph, control->channel_9c.animation_id);
		if (animation && ((1 << bit) & *(word *)((byte *)animation + 0x18)))
			return false;
	}
	return result;
}
void function_ba350(long object_index, real seconds);

struct s_1d9240;
void function_1d9240(s_1d9240 *slot, char flag, real seconds);
bool function_1d9320(s_1d9240 *slot);

void function_1d90e0(long render_model_index, transform4x3f *nodes, long node_index, transform4x3f const *marker_matrix,
	transform4x3f const *target_matrix, real weight, long node_count);

// @retail 0x113980
void __stdcall function_113980(long object_index, long target_index, bool alternate, real weight,
	s_animation_state *state, s_graph_pair_iterator *iterator, long node_count, transform4x3f *nodes)
{
	// The iterator pointer remains a stack argument in retail.
	s_graph_pair_iterator *const *iterator_reference = &iterator;
	iterator = *iterator_reference;
	if (object_index != NONE && target_index != NONE && weight > 0.0f)
	{
		s_graph_tag *graph = GRAPH_GET(state->graph_tag_index);
		if (graph && (short)graph->node_count != 0)
		{
			s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(object_index);
			s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
			byte *model = TAG_BYTES(definition->model_tag_index);
			dword visited[8];
			memset(visited, 0, ((state->node_count() + 31) >> 5) * sizeof(dword));
			for (;;)
			{
				bool found;
				if (alternate)
				{
					graph = GRAPH_GET(state->graph_tag_index);
					iterator->mode = state->unknown70;
					iterator->weapon_class = state->unknown74;
					found = function_1dcfa0(graph, iterator);
				}
				else
				{
					graph = GRAPH_GET(state->graph_tag_index);
					iterator->mode = state->unknown70;
					iterator->weapon_class = state->unknown74;
					found = function_1dcf20(graph, iterator);
				}
				if (!found)
					break;
				if (iterator->b != NONE && iterator->b && iterator->a != NONE && iterator->a)
				{
					s_object_marker marker;
					s_object_marker target;
					if (function_b8d30(object_index, iterator->a, &marker, 1, false) &&
						!(visited[marker.node_index >> 5] & (1 << (marker.node_index & 31))) &&
						function_b8d30(target_index, iterator->b, &target, 1, false))
					{
						function_1d90e0(*(long *)(model + 4), nodes, marker.node_index, &marker.node_matrix,
							&target.matrix, weight, node_count);
						visited[marker.node_index >> 5] |= 1 << (marker.node_index & 31);
					}
				}
			}
		}
	}
}
bool function_10ee20(s_animation_state *state);
bool function_10f630(long object_index, long *first, long *second);
long function_10f720(long object_index, bool first);

// @retail 0x113e40
bool function_113e40(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	return state && state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && !(state->flags & 1) && !function_10ee20(state);
}

// @retail 0x114040
bool function_114040(long unit_index, long name)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = false;
	if (control->channel_9c.unknown08 == name)
		return true;
	if (control->channel_bc.unknown08 == name)
		return true;
	long second, first;
	if (function_10f630(unit_index, &first, &second))
		result = second == name;
	return result;
}

// @retail 0x1143d0
bool function_1143d0(long unit_index)
{
	bool result = true;
	if (unit_index != NONE)
	{
		s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
		s_animation_state *state = UNIT_ANIMATION_STATE(unit);
		long name = state->unknown7c;
		if (state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE &&
			!(state->channels[0].unknown11 & 0xa))
		{
			if (name == 0x700005c)
			{
				if (function_10f720(unit_index, true) != 2)
					result = false;
			}
			else if (name == 0x700005d)
			{
				if (function_10f720(unit_index, true) != 1)
					result = false;
			}
		}
	}
	return result;
}

// @retail 0x113910
bool function_113910(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = function_1d9320((s_1d9240 *)&control->slots[0]);
	result |= function_1d9320((s_1d9240 *)&control->slots[1]);
	result |= function_1d9320((s_1d9240 *)&control->slots[2]);
	result |= function_1d9320((s_1d9240 *)&control->slots[3]);
	result |= function_1d9320((s_1d9240 *)&control->slots[4]);
	return result;
}

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

// @retail 0x1140b0
bool function_1140b0(long unit_index, long name)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	bool result = false;
	if (control->channel_9c.unknown08 == name)
	{
		channel_reset_114240(&control->channel_9c);
		function_ba350(unit_index, 0.267f);
		return true;
	}
	if (control->channel_bc.unknown08 == name)
	{
		channel_reset_114240(&control->channel_bc);
		function_ba350(unit_index, 0.267f);
		return true;
	}
	long first, second;
	if (function_10f630(unit_index, &first, &second) && second == name)
	{
		function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, 0x7000001, 0.0f, false, 0);
		return true;
	}
	return result;
}

// @retail 0x113e90
bool __stdcall function_113e90(long unit_index, long name, real blend, c_animation_channel **output, long mode)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	c_animation_channel *channel = mode == 3 ? &control->channel_bc : &control->channel_9c;
	long weapon_class = 0x7000101;
	long weapon_type = 0x7000101;
	if (mode == 1 || mode == 3)
	{
		weapon_class = 0x400054b;
		weapon_type = control->weapon_type;
	}
	else if (mode == 2 && state->unknown74 == 0x400054b)
		weapon_class = control->weapon_class;
	bool can_blend = true;
	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		s_animation *animation = function_1daea0(GRAPH_GET(channel->graph_tag_index), channel->animation_id);
		can_blend = !TEST_FIELD_BIT(animation->flag1);
	}
	if (state->overlay_play(channel, 0x803f, name, weapon_class, weapon_type))
	{
		if (blend > 0.0f && can_blend && channel->is_unflagged0())
			function_ba350(unit_index, blend);
		if (output)
			*output = channel;
		return true;
	}
	long first, second;
	if ((function_10f630(unit_index, &first, &second) && second == name) ||
		function_10f430(unit_index, 0x7000101, 0x7000101, 0x7000101, name, blend, false, 0))
	{
		if (output)
			*output = &UNIT_ANIMATION_STATE(unit)->channels[0];
		return true;
	}
	return false;
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

real function_1d9370(s_1d9240 const *slot);

// @retail 0x113ba0
void function_113ba0(long unit_index, long target_index, long node_count, transform4x3f *nodes)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && target_index != NONE && control->slots[0].unknown1)
	{
		real weight = function_1d9370((s_1d9240 *)&control->slots[0]);
		if (!(fabs(weight) < 0.0001f))
		{
			s_graph_pair_iterator iterator;
			iterator.a = NONE;
			iterator.b = NONE;
			iterator.index = NONE;
			iterator.step = 0;
			iterator.mode = NONE;
			iterator.weapon_class = NONE;
			function_113980(unit_index, target_index, true, weight, state, &iterator, node_count, nodes);
		}
	}
}

// @retail 0x113c60
void function_113c60(long unit_index, long target_index, long node_count, transform4x3f *nodes)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (state->graph_tag_index != NONE && state->channels[0].graph_tag_index != NONE &&
		state->channels[0].animation_id.index != NONE && target_index != NONE && control->slots[1].unknown1)
	{
		real weight = function_1d9370((s_1d9240 *)&control->slots[1]);
		if (!(fabs(weight) < 0.0001f))
		{
			s_graph_pair_iterator iterator;
			iterator.a = NONE;
			iterator.b = NONE;
			iterator.index = NONE;
			iterator.step = 0;
			iterator.mode = NONE;
			iterator.weapon_class = NONE;
			function_113980(unit_index, target_index, false, weight, state, &iterator, node_count, nodes);
		}
	}
}

// @retail 0x111490
bool function_111490(long unit_index)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	bool result = false;
	if (unit->parent_index != NONE && *(short *)((byte *)unit + 0x1fc) != NONE)
	{
		result = true;
		if (function_0c7070(unit_index))
			result = false;
		else
		{
			switch (UNIT_ANIMATION_STATE(unit)->unknown7c)
			{
			case 0x400004a: case 0x400076c: case 0x400076d:
			case 0x50000c3: case 0x5000768: case 0x5000769: case 0x500076a: case 0x500076b:
			case 0x600008c: case 0x700076e: case 0x80000c4: case 0x800076f:
			case 0xa000767: case 0xc000043: case 0xd00002b: case 0xd000042:
			case 0xe00002a: case 0xe00003b: case 0xe0000c3: case 0xf00003a:
				result = false;
			}
		}
		if (!function_111590(unit_index, 2))
			return false;
	}
	return result;
}


// @retail 0x1112f0
bool function_1112f0(long unit_index)
{
	bool result = true;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	switch (control->channel_9c.unknown08)
	{
	case NONE: case 0x500000a: case 0x6000006: case 0x6000007:
	case 0x9000004: case 0x9000005: case 0x9000008: case 0x9000009:
	case 0xa000040: case 0xa0005b9: case 0xa0005bb: case 0xb0005b2:
	case 0xc000073: case 0xc000075: case 0xc000077: case 0xc0006b3: case 0xc0006cd:
	case 0xe000038: case 0xe000607: case 0xe000608: case 0xe000609: case 0xe00060a: case 0xe00067d:
	case 0x11000074: case 0x11000076: case 0x130005bc: case 0x140005b3:
		break;
	default:
		result = false;
		break;
	}
	if (function_0c7070(unit_index))
		result = false;
	else
	{
		switch (state->unknown7c)
		{
		case 0x400004a: case 0x600008c: case 0xa00003e: case 0xb000059: case 0xb00005a:
		case 0xc000043: case 0xc000058: case 0xc00005b: case 0xd00003f: case 0xd000042:
		case 0xe00003b: case 0xe0000c2: case 0xf00003a: case 0x1000003c:
			result = false;
		}
	}
	if (!function_111590(unit_index, 3))
		return false;
	return result;
}


PRIVATE __forceinline long held_object_110ab0(long unit_index, short slot)
{
	long result = NONE;
	if (slot != NONE)
		result = *(long *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x218 + slot * 4);
	return result;
}

bool function_c70b0(long unit_index);

// @retail 0x110ab0
bool __stdcall function_110ab0(long unit_index)
{
	// Retail keeps this argument on the stack.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long overlay = control->channel_9c.unknown08;
	long name = state->unknown7c;
	long controlling = unit->field_13c;
	bool result = false;
	bool depleted = false;
	bool interruptible = false;
	if (controlling != NONE)
	{
		long first = held_object_110ab0(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
		if (first != NONE)
			depleted = *(short *)((byte *)UNIT_ANIMATION_OBJECT(first) + 0x17c) <= 0;
		long second = held_object_110ab0(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x213));
		if (second != NONE && *(short *)((byte *)UNIT_ANIMATION_OBJECT(second) + 0x17c) > 0)
			depleted = false;
		if (*(byte *)((byte *)state + 0x14) & 0x10)
			interruptible = true;
	}
	if (function_c70b0(unit_index))
		result = true;
	else
	{
		switch (name)
		{
		case 0x400004a:
		case 0x400076c:
		case 0x400076d:
		case 0x50000c3:
		case 0x5000768:
		case 0x5000769:
		case 0x500076a:
		case 0x500076b:
		case 0x600005f:
		case 0x600008c:
		case 0x60000cc:
		case 0x60000cd:
		case 0x60006ac:
		case 0x700002c:
		case 0x7000543:
		case 0x700076e:
		case 0x80000c4:
		case 0x800076f:
		case 0x9000011:
		case 0x9000012:
		case 0xa000010:
		case 0xa000013:
		case 0xa00002d:
		case 0xa00003e:
		case 0xa00022d:
		case 0xa000767:
		case 0xb00002e:
		case 0xb00022e:
		case 0xb0005b8:
		case 0xc000043:
		case 0xc0005b7:
		case 0xd00002b:
		case 0xd000042:
		case 0xd00022c:
		case 0xe00002a:
		case 0xe00003b:
		case 0xe0000c3:
		case 0xf00003a:
		case 0x1000003c:
			result = !interruptible;
			break;
		case 0x500000a:
		case 0x5000024:
		case 0xa000040:
		case 0xa0005b9:
		case 0xb0005b2:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xc0006b3:
		case 0xc0006cd:
		case 0xd000021:
		case 0xe000038:
		case 0xe00067d:
		case 0x1000006c:
		case 0x11000074:
		case 0x11000076:
		case 0x140005b3:
			result = !depleted;
			break;
		}
	}
	switch (overlay)
	{
	case 0xa00022d:
	case 0xb00022e:
	case 0xd00022c:
		result = !interruptible;
		break;
	case 0x500000a:
	case 0xa000040:
	case 0xa0005b9:
	case 0xb0005b2:
	case 0xc000073:
	case 0xc000075:
	case 0xc000077:
	case 0xc0006b3:
	case 0xc0006cd:
	case 0xd000021:
	case 0xe000038:
	case 0xe00067d:
	case 0x1000006c:
	case 0x11000074:
	case 0x11000076:
	case 0x140005b3:
		result = !depleted;
		break;
	}
	return result;
}


// @retail 0x1103a0
bool __stdcall function_1103a0(long unit_index, long requested, bool allow)
{
	// Retail keeps this argument on the stack.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	long controlling = unit->field_13c;
	bool result = true;
	bool depleted = false;
	bool interruptible = false;
	if (controlling != NONE)
	{
		long first = held_object_110ab0(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
		if (first != NONE)
			depleted = *(short *)((byte *)UNIT_ANIMATION_OBJECT(first) + 0x17c) <= 0;
		long second = held_object_110ab0(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x213));
		if (second != NONE && *(short *)((byte *)UNIT_ANIMATION_OBJECT(second) + 0x17c) > 0)
			depleted = false;
		if (*(byte *)((byte *)state + 0x14) & 0x10)
			interruptible = true;
	}
	if (function_0c7070(unit_index))
		result = interruptible;
	else
	{
		switch (state->unknown7c)
		{
		case 0x400004a:
		case 0xe0000c2:
			result = interruptible;
			break;
		case 0x400076c:
		case 0x400076d:
		case 0x5000768:
		case 0x5000769:
		case 0x500076a:
		case 0x500076b:
		case 0x600005f:
		case 0x700002c:
		case 0x7000543:
		case 0x700076e:
		case 0x800076f:
		case 0xa00003e:
		case 0xa00022d:
		case 0xa000767:
		case 0xb00022e:
		case 0xd000021:
		case 0xd00002b:
		case 0xd00022c:
		case 0xe00002a:
		case 0xe0000c3:
		case 0x1000006c:
			if (requested != 0x600008c)
				result = interruptible;
			break;
		case 0x500000a:
		case 0x5000024:
		case 0xa000040:
		case 0xa0005b9:
		case 0xb0005b2:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xc0006b3:
		case 0xc0006cd:
		case 0xe000038:
		case 0xe00067d:
		case 0x11000074:
		case 0x11000076:
		case 0x140005b3:
			result = depleted;
			break;
		case 0x50000c3:
		case 0x80000c4:
			result = interruptible;
			break;
		case 0x600008c:
			switch (requested)
			{
			case 0x600008d: case 0x600008e: case 0x900001f: case 0x9000020: case 0x90006b2:
			case 0xc000043: case 0xd000042:
				result = true;
				break;
			default:
				result = interruptible;
				break;
			}
			break;
		case 0x600008d:
		case 0x600008e:
		case 0xc000043:
		case 0xd000042:
			result = requested == 0xc000043 || requested == 0xd000042 || interruptible;
			break;
		case 0x60000cc:
		case 0x60000cd:
		case 0x60006ac:
		case 0x9000011:
		case 0x9000012:
		case 0x90006b2:
		case 0xa000010:
		case 0xa000013:
		case 0xa00002d:
		case 0xb00002e:
			result = interruptible;
			break;
		case 0x700005c:
		case 0x700005d:
		case 0x800061e:
		case 0x900000e:
		case 0xa00000f:
			if (requested == 0x400000c)
				result = allow;
			break;
		case 0x800001c:
		case 0xa00001d:
		case 0xb0005b8:
		case 0xc0005b7:
		case 0xe00003b:
		case 0xf00003a:
			result = false;
			break;
		}
	}
	bool overlay_interruptible = false;
	if (controlling != NONE && (*(byte *)((byte *)control + 0xb0) & 0x10))
		overlay_interruptible = true;
	switch (control->channel_9c.unknown08)
	{
	case 0xa00022d:
	case 0xb00022e:
	case 0xd00022c:
		switch (requested)
		{
		case 0x400000c:
		case 0x600008c:
		case 0x900000e:
		case 0x9000015:
		case 0x9000016:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xa000014:
		case 0xa000017:
			result = true;
			break;
		default:
			result = overlay_interruptible;
			break;
		}
		break;
	case 0x500000a:
	case 0xa000040:
	case 0xa0005b9:
	case 0xb0005b2:
	case 0xc000073:
	case 0xc000075:
	case 0xc000077:
	case 0xc0006b3:
	case 0xc0006cd:
	case 0xd000021:
	case 0xe000038:
	case 0xe00067d:
	case 0x1000006c:
	case 0x11000074:
	case 0x11000076:
	case 0x140005b3:
		switch (requested)
		{
		case 0x400000c:
		case 0x600008c:
		case 0x900000e:
		case 0x9000015:
		case 0x9000016:
		case 0x900001f:
		case 0x9000020:
		case 0x90006b2:
		case 0xa00000f:
		case 0xa000014:
		case 0xa000017:
			result = true;
			break;
		default:
			result = depleted;
			break;
		}
		break;
	}
	return result;
}

extern point2f *g_4687c4;
struct s_unit_seat_definition;
s_unit_seat_definition *function_c8fc0(long unit_index);
bool function_10f890(long object_index);
void function_b9fc0(long object_index, vector3f *forward, vector3f *up);
vector2f *function_11df30(vector2f *angles, vector3f const *v);

PRIVATE __forceinline void bounds_112070(s_aiming_screen const *screen, real *bounds)
{
	bounds[0] = 0.0f - screen->right_frame_count * screen->right_yaw_per_frame;
	bounds[1] = screen->left_frame_count * screen->left_yaw_per_frame;
	bounds[2] = 0.0f - screen->down_frame_count * screen->down_pitch_per_frame;
	bounds[3] = screen->up_frame_count * screen->up_pitch_per_frame;
}

PRIVATE __forceinline void project_112070(vector3f const *v, vector3f const *forward,
	vector3f const *side, vector3f const *up, vector3f *result)
{
	result->i = forward->k * v->k + forward->j * v->j + forward->i * v->i;
	result->j = side->k * v->k + side->j * v->j + side->i * v->i;
	result->k = up->k * v->k + up->j * v->j + up->i * v->i;
}

// @retail 0x112070
void __stdcall function_112070(long unit_index, dword const *node_mask, long node_count,
	real_quaternion_transform *transforms)
{
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	s_graph_tag *graph = GRAPH_GET(state->graph_tag_index);
	c_animation_channel channel;
	if (control->channel_9c.graph_tag_index != NONE && control->channel_9c.animation_id.index != NONE)
		control->channel_9c.sample(1.0f, node_mask, node_count, transforms);
	if (control->channel_bc.graph_tag_index != NONE && control->channel_bc.animation_id.index != NONE)
		control->channel_bc.sample(1.0f, node_mask, node_count, transforms);
	if (control->channel_dc.graph_tag_index != NONE && control->channel_dc.animation_id.index != NONE)
	{
		s_unit_seat_definition *seat = function_c8fc0(unit_index);
		real weight = 1.0f;
		if (function_10f890(unit_index) && seat && *(real *)((byte *)seat + 0x1c) > 0.0f)
			weight = *(real *)((byte *)seat + 0x1c);
		control->channel_dc.sample(weight, node_mask, node_count, transforms);
	}
	if (control->animation_104.index != NONE && state->graph_tag_index != NONE &&
		channel.set(state->graph_tag_index, 0x400, control->animation_104, NONE, NONE, NONE, NONE))
		channel.sample(1.0f, node_mask, node_count, transforms);
	if (control->animation_108.index != NONE && state->graph_tag_index != NONE &&
		channel.set(state->graph_tag_index, 0x800, control->animation_108, NONE, NONE, NONE, NONE))
		channel.sample(1.0f, node_mask, node_count, transforms);
	control->flags &= ~0x60;
	if (!(definition->flags & 0x800) && state->graph_tag_index != NONE)
	{
		if (*(real *)((byte *)unit + 0x208) > 0.0f &&
			*(short *)((byte *)UNIT_ANIMATION_CONTROL(UNIT_ANIMATION_OBJECT(unit_index)) + 0x36) == NONE)
		{
			c_type_709360 id;
			state->current_animation_get(&id);
			if (id.index != NONE && state->channel_play(&channel, id, 0))
				channel.sample(*(real *)((byte *)unit + 0x208), node_mask, node_count, transforms);
		}
		if (state->unknown7c != 0xe0000c2)
		{
			if (TEST_FIELD_BIT(control->overlay))
			{
				long i = 0;
				do
				{
					c_type_709360 id = control->overlays[i];
					if (id.index != NONE)
					{
						real ratio = *(real *)((byte *)unit + 0x288 + i * 4) * 0.5f + 0.5f;
						if (ratio < 0.0f) ratio = 0.0f;
						else if (ratio > 1.0f) ratio = 1.0f;
						if (!(fabs(ratio - 0.5f) < 0.0001f) && state->channel_play(&channel, id, 0))
							channel.sample_ratio(ratio, 1.0f, node_count, transforms, node_mask);
					}
					i++;
				} while (i < 3);
			}
			if (!(definition->flags & 0x400))
			{
				vector2f angles;
				angles.i = g_4687c4->x;
				angles.j = g_4687c4->y;
				vector3f forward, up, side, projected;
				function_b9fc0(unit_index, &forward, &up);
				side.i = forward.k * up.j - up.k * forward.j;
				side.j = up.k * forward.i - forward.k * up.i;
				side.k = forward.j * up.i - up.j * forward.i;
				c_type_709360 first = control->animation_fc;
				c_type_709360 second = control->animation_100;
				bool active = false;
				if (first.index != NONE || second.index != NONE)
				{
					s_aiming_screen const *screen_a = NULL;
					s_aiming_screen const *screen_b = NULL;
					if (first.index != NONE)
						screen_a = (s_aiming_screen const *)function_1daf30(graph, first);
					if (second.index != NONE)
						screen_b = (s_aiming_screen const *)function_1daf30(graph, second);
					if (screen_a || screen_b)
					{
						project_112070((vector3f *)((byte *)unit + 0x168), &forward, &side, &up, &projected);
						angles.i = (real)atan2(projected.j, projected.i);
						angles.j = (real)atan2(projected.k, sqrt(projected.i * projected.i + projected.j * projected.j));
						control->flags |= 0x20;
						if (screen_a) bounds_112070(screen_a, (real *)control->unknown04);
						else if (screen_b) bounds_112070(screen_b, (real *)control->unknown04);
						active = function_111590(unit_index, 4);
						if ((state->unknown7c == 0x700005c || state->unknown7c == 0x700005d) && !function_10ee20(state))
							active = false;
						bool centered = fabs(angles.j) < 0.0001f && fabs(angles.i) < 0.0001f;
						if (control->slots[4].unknown1)
						{
							real weight = function_1d9370((s_1d9240 *)&control->slots[4]);
							real blend = function_1d9370((s_1d9240 *)&control->slots[2]);
							c_animation_channel a, b;
							state->channel_play(&a, first, 0);
							state->channel_play(&b, second, 0);
							if (weight > 0.0001f)
							{
								if (screen_a && screen_b)
								{
									real wa = (1.0f - blend) * weight;
									real wb = blend * weight;
									if (0.0001f > wa) wb = 1.0f;
									else
									{
										if (0.0001f > wb) { wa = 1.0f; wb = 0.0f; }
										if (wa > 0.0001f && (wa < 0.9999f || !centered))
											a.sample_aiming(angles.i, angles.j, wa, node_mask, node_count, transforms);
									}
									if (wb > 0.0001f)
										b.sample_aiming(angles.i, angles.j, wb, node_mask, node_count, transforms);
								}
								else if (screen_a)
								{
									if (weight < 0.9999f || !centered)
										a.sample_aiming(angles.i, angles.j, weight, node_mask, node_count, transforms);
								}
								else if (screen_b)
									b.sample_aiming(angles.i, angles.j, weight, node_mask, node_count, transforms);
							}
						}
					}
				}
				function_1d9240((s_1d9240 *)&control->slots[4], active, active ? 0.267f : 0.1335f);
				active = false;
				if ((*(char *)((byte *)unit + 0x212) != NONE || unit->field_13c != NONE) && control->animation_114.index != NONE)
				{
					c_type_709360 id = control->animation_114;
					state->channel_play(&channel, id, 0);
					if (channel.graph_tag_index != NONE && channel.animation_id.index != NONE)
					{
						s_aiming_screen const *screen = (s_aiming_screen const *)function_1daf30(graph, id);
						if (screen)
						{
							project_112070((vector3f *)((byte *)unit + 0x18c), &forward, &side, &up, &projected);
							vector2f relative;
							function_11df30(&relative, &projected);
							control->flags |= 0x40;
							relative.i -= angles.i;
							relative.j -= angles.j;
							bounds_112070(screen, (real *)control->unknown14);
							active = function_111590(unit_index, 5);
							if (control->slots[3].unknown1)
							{
								real weight = function_1d9370((s_1d9240 *)&control->slots[3]);
								if (weight > 0.0001f && (weight < 0.9999f ||
									!(fabs(relative.i) < 0.0001f && fabs(relative.j) < 0.0001f)))
									channel.sample_aiming(relative.i, relative.j, weight, node_mask, node_count, transforms);
							}
						}
					}
				}
				function_1d9240((s_1d9240 *)&control->slots[3], active, active ? 0.267f : 0.1335f);
			}
		}
	}
	unit = UNIT_ANIMATION_OBJECT(unit_index);
	if (*(char *)((byte *)unit + 0x212) == NONE || *(char *)((byte *)unit + 0x213) == NONE)
	{
		if (control->animation_10c.index != NONE)
		{
			word flags = 0;
			if (control->slots[0].unknown1 && function_1d9370((s_1d9240 *)&control->slots[0]) < 0.0001f)
				flags = 0x400;
			if (state->graph_tag_index != NONE && state->channel_start(&channel, control->animation_10c, NONE, NONE, NONE, NONE, flags))
				channel.sample(1.0f, node_mask, node_count, transforms);
		}
	}
	else
	{
		if (control->animation_10c.index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0x400, control->animation_10c, NONE, NONE, NONE, NONE))
			channel.sample(1.0f, node_mask, node_count, transforms);
		if (control->animation_110.index != NONE && state->graph_tag_index != NONE &&
			channel.set(state->graph_tag_index, 0x800, control->animation_110, NONE, NONE, NONE, NONE))
			channel.sample(1.0f, node_mask, node_count, transforms);
	}
}

long function_cbd50(long unit_index, short weapon_index);
bool function_cd660(long unit_index);

// @retail 0x1130f0
void __stdcall function_1130f0(long unit_index, long node_count, transform4x3f *nodes)
{
	// Retail keeps the object argument on the stack.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_BYTES(unit->definition_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	if (!(bool)((definition->flags >> 11) & 1) && state->graph_tag_index != NONE &&
		state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE)
	{
		bool parent = function_111490(unit_index);
		bool weapon = function_1112f0(unit_index);
		if (function_cd660(unit_index))
			weapon = false;
		function_1d9240((s_1d9240 *)&control->slots[1], parent, parent ? 0.267f : 0.06675f);
		function_1d9240((s_1d9240 *)&control->slots[0], weapon, weapon ? 0.267f : 0.06675f);
		function_113c60(unit_index, unit->parent_index, node_count, nodes);
		if (*(char *)((byte *)unit + 0x212) != NONE && state->unknown74 != NONE && state->unknown78 != NONE)
		{
			long held = function_cbd50(unit_index, *(char *)((byte *)UNIT_ANIMATION_OBJECT(unit_index) + 0x212));
			function_113ba0(unit_index, held, node_count, nodes);
			control->flag0 = false;
		}
	}
}

short function_20ba00(short index);
bool function_114520(long unit_index, short entry_index, short priority, long *previous, long *tag_index);
void function_114680(long unit_index, void const *request);

struct s_request_extra_114b60
{
	short marker;
	short unknown02;
	long object_index;
	long flags;
	byte unknown0c[0x1c - 0xc];
};

struct s_request_114b60
{
	short priority;
	short entry_index;
	long definition_index;
	short unknown08;
	short delay;
	s_sound_permutation_reference permutation;
	short unknown0e;
	long sound_index;
	s_request_extra_114b60 extra;
};

// @retail 0x114b60
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra)
{
	// Retail keeps the object on the stack and reuses the priority slot for the tag.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	short request_priority = (short)priority;
	short index = entry_index;
	if (index == NONE)
	{
		index = function_20ba00(fallback_index);
		if (index == NONE)
			return false;
	}
	priority = NONE;
	if (function_114520(unit_index, index, request_priority, NULL, &priority))
	{
		s_request_114b60 request;
		memset(&request, 0, sizeof(request));
		request.priority = request_priority;
		request.entry_index = index;
		request.definition_index = priority;
		request.delay = duration_ticks_1145f0(0.25f);
		request.sound_index = NONE;
		if (extra)
		{
			memcpy(&request.extra, extra, sizeof(request.extra));
			function_114680(unit_index, &request);
			return true;
		}
		request.extra.marker = NONE;
		request.extra.object_index = NONE;
		request.extra.flags = 0;
		function_114680(unit_index, &request);
		return true;
	}
	return false;
}

real function_259a0(dword *seed);
void function_c96b0(long unit_index, long *result, real a, real b);

// @retail 0x1147e0
bool function_1147e0(long unit_index, bool a, real b, real c, long definition_index, bool hard)
{
	// Retail preserves the object argument on the stack throughout the request.
	long const *unit_reference = &unit_index;
	unit_index = *unit_reference;
	s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
	byte *state = (byte *)unit + *(short *)((byte *)unit + 0x342);
	short type = 0;
	bool special = false;
	bool result = false;
	bool outside = false;
	if (definition_index != NONE)
	{
		byte *definition = TAG_BYTES(definition_index);
		type = *(short *)(definition + 0x12);
		special = (*(dword *)(definition + 0x14) >> 7) & 1;
	}
	short selection;
	if (hard)
	{
		long actor_index = *(long *)((byte *)unit + 0x12c);
		bool strong = false;
		if (actor_index != NONE)
			strong = *(short *)((byte *)g_4f55f0->data + (actor_index & 0xffff) * 0x888 + 0x86) >= 5;
		else if (*(real *)((byte *)unit + 0x100) > b + 0.2f)
			strong = true;
		if (type == 7 || special)
			selection = 3;
		else if (type == 1)
			selection = g_4e6948->mode != 4;
		else if (type == 3 || type == 4 || type == 9)
			selection = 2;
		else if (!strong && type == 6)
			selection = 5;
		else
			selection = a ? 4 : 0;
	}
	else
	{
		if (*(short *)(state + 0x42))
			return false;
		a = *(real *)((byte *)unit + 0x100) > 0.0f;
		long range;
		function_c96b0(unit_index, &range, c, b);
		outside = range == 2;
		if (type == 1)
			selection = 11;
		else if (outside)
		{
			if (a)
				selection = function_259a0(&g_4e7408->unknown0) < 0.6667f ? 8 : 9;
			else
				selection = 10;
		}
		else
		{
			if (*(short *)(state + 0x40) || *(short *)(state + 0x3e) >= 3)
				return false;
			if (*(short *)(state + 0xc) && !(function_259a0(&g_4e7408->unknown0) < 0.4f))
				return false;
			selection = a ? 7 : 10;
		}
	}
	short priority = hard ? 15 : 12;
	short entry = function_20ba00(selection);
	long tag_index = NONE;
	if (entry != NONE && function_114520(unit_index, entry, priority, NULL, &tag_index))
	{
		s_request_114b60 request;
		memset(&request, 0, sizeof(request));
		request.priority = priority;
		request.entry_index = entry;
		request.definition_index = tag_index;
		request.delay = duration_ticks_1145f0(0.25f);
		request.sound_index = NONE;
		request.extra.marker = NONE;
		request.extra.object_index = NONE;
		request.extra.flags = 0;
		function_114680(unit_index, &request);
		result = true;
		if (outside)
			*(short *)(state + 0x42) = duration_ticks_1145f0(2.0f);
		else
		{
			*(short *)(state + 0x40) = duration_ticks_1145f0(0.5f);
			*(short *)(state + 0x3c) = duration_ticks_1145f0(1.5f);
			if (++*(short *)(state + 0x3e) == 3)
				*(short *)(state + 0x40) = duration_ticks_1145f0(2.0f);
		}
	}
	return result;
}


void function_20d220(long record_index, long object_index, short type, long const *target);

// @retail 0x114710
void __stdcall function_114710(long arg_0, long arg_1, long arg_2)
{
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    byte *local_1 = (byte *)local_0 + *(short *)((byte *)local_0 + 0x342);
    s_request_114b60 local_2;
    memset(&local_2, 0, sizeof(local_2));
    local_2.priority = 14;
    local_2.entry_index = NONE;
    local_2.definition_index = arg_1;
    local_2.delay = duration_ticks_1145f0(0.8f);
    local_2.sound_index = NONE;
    local_2.extra.marker = NONE;
    local_2.extra.object_index = NONE;
    local_2.extra.flags = 0;
    function_114680(arg_0, &local_2);
    if (*(long *)(local_1 + 0x10) == arg_1)
    {
        *(long *)(local_1 + 0x54) = arg_2;
        local_1[0x48] = 1;
        function_20d220(NONE, arg_0, NONE, (long const *)(local_1 + 0x20));
    }
}

void (__stdcall *g_467a2c)(long, long, long) = function_114710;


long players_first_active_local_player();
__declspec(noinline) s_player_state *function_16f3a0(long arg_0);
bool function_0bfe60(dword const *arg_0, long arg_1);

// @retail 0x1132f0
void function_1132f0(long arg_0, dword const *arg_1, long arg_2, transform4x3f *arg_3)
{
    (void)&arg_1;
    (void)&arg_2;
    (void)&arg_3;
    if (function_113260(arg_0))
    {
        s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
        point3f const *local_1;
        if ((short)local_0->flags148 < 0)
            local_1 = (point3f *)((byte *)local_0 + 0x1a4);
        else
        {
            if (!(local_0->flags137 & 1))
                return;
            long local_2 = players_first_active_local_player();
            if (local_2 == NONE)
                return;
            local_1 = (point3f const *)function_16f3a0(local_2);
        }
        if (local_1)
        {
            s_object_marker local_3;
            if (function_b8d30(arg_0, 0x4000095, &local_3, 1, false) > 0)
            {
                char local_4 = local_0->index1f6;
                if (local_4 >= 0 && local_4 < arg_2 && (!arg_1 || function_0bfe60(arg_1, local_4)))
                    function_112e20(&arg_3[local_4], &local_3.matrix, local_1);
                char local_5 = local_0->index1f7;
                if (local_5 >= 0 && local_5 < arg_2 && (!arg_1 || function_0bfe60(arg_1, local_5)))
                    function_112e20(&arg_3[local_5], &local_3.matrix, local_1);
            }
        }
    }
}

struct s_sound_label_play;
struct s_114c60
{
    long field_0;
    long field_4;
    real field_8;
    void const *field_c;
};

void sound_choose_permutation(long arg_0, s_sound_permutation_reference *arg_1, bool *arg_2);
long function_1891d0(long arg_0, long arg_1, s_sound_label_play const *arg_2);
void __stdcall function_20d570(long arg_0, short arg_1, long arg_2, void *arg_3);
void function_20fd50(long arg_0, long arg_1);

// @retail 0x114c60
void function_114c60(long arg_0)
{
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    byte *local_1 = (byte *)local_0 + *(short *)((byte *)local_0 + 0x342);
    short local_2 = *(short *)(local_1 + 0x3c);
    if ((short)local_2 > 0)
    {
        local_2--;
        *(short *)(local_1 + 0x3c) = (short)local_2;
        if (!(short)local_2)
        {
            short local_3 = *(short *)(local_1 + 0x3e);
            if ((short)local_3 > 0)
            {
                *(short *)(local_1 + 0x3e) = (short)(local_3 - 1);
                *(short *)(local_1 + 0x3c) = duration_ticks_1145f0(1.5f);
            }
        }
    }
    short local_4 = *(short *)(local_1 + 0x42);
    if ((short)local_4 > 0)
        *(short *)(local_1 + 0x42) = (short)(local_4 - 1);
    short local_5 = *(short *)(local_1 + 0x40);
    if ((short)local_5 > 0)
        *(short *)(local_1 + 0x40) = (short)(local_5 - 1);
    if (*(short *)(local_1 + 0xc) > 0)
    {
        if (!local_1[0x48])
        {
            long local_6 = *(long *)(local_1 + 0x10);
            if (local_6 != NONE)
            {
                long local_7 = *(long *)(local_1 + 8);
                if (*(long *)(local_1 + 0x1c) == NONE)
                    sound_choose_permutation(local_6, (s_sound_permutation_reference *)(local_1 + 0x18), NULL);
                s_114c60 local_8;
                local_8.field_0 = local_7;
                local_8.field_4 = *(long *)(local_1 + 0x10);
                local_8.field_8 = 1.0f;
                local_8.field_c = local_1 + 0x18;
                *(long *)(local_1 + 0x54) = function_1891d0(arg_0, 0x4000095, (s_sound_label_play const *)&local_8);
            }
            function_20d220(*(long *)(local_1 + 0x1c), arg_0, *(word *)(local_1 + 0xe), (long const *)(local_1 + 0x20));
            local_1[0x48] = true;
        }
        short local_9 = *(short *)(local_1 + 0x4e);
        if ((short)local_9 > 0)
            *(short *)(local_1 + 0x4e) = (short)(local_9 - 1);
        short local_10 = *(short *)(local_1 + 0x4c);
        if ((short)local_10 > 0)
        {
            local_10--;
            *(short *)(local_1 + 0x4c) = (short)local_10;
            if (!(short)local_10)
                *(long *)(local_1 + 0x54) = NONE;
        }
        else
        {
            if (!local_1[0x4a])
                local_1[0x4a] = true;
            short local_11 = *(short *)(local_1 + 0x50);
            if ((short)local_11 > 0)
                *(short *)(local_1 + 0x50) = (short)(local_11 - 1);
        }
        if (*(short *)(local_1 + 0xc) > 0 && !*(short *)(local_1 + 0x4e) && !local_1[0x49])
        {
            function_20d570(*(long *)(local_1 + 0x1c), *(short *)(local_1 + 0xe), arg_0, local_1 + 0x20);
            local_1[0x49] = true;
        }
        if (*(short *)(local_1 + 0xc) > 0 && !*(short *)(local_1 + 0x4c) && !*(short *)(local_1 + 0x50))
        {
            if (!local_1[0x49])
            {
                function_20d570(*(long *)(local_1 + 0x1c), *(short *)(local_1 + 0xe), arg_0, local_1 + 0x20);
                local_1[0x49] = true;
            }
            if (*(long *)(local_1 + 0x1c) != NONE)
                function_20fd50(arg_0, *(long *)(local_1 + 0x1c));
            *(short *)(local_1 + 0xc) = 0;
        }
    }
    real local_12 = 0.0f - *(real *)((byte *)local_0 + 0x208);
    if (-0.1f > local_12)
        local_12 = -0.1f;
    else if (local_12 > 0.1f)
        local_12 = 0.1f;
    *(real *)((byte *)local_0 + 0x208) += local_12;
}

struct s_bit_owner;
long function_10eef0(long arg_0, bool arg_1, bool arg_2);
void __stdcall function_bf600(long arg_0, real arg_1, s_animation_frame_event const *arg_2);
void function_e6a20(long arg_0, long arg_1, bool arg_2, bool arg_3);
void function_e6b00(long arg_0, long arg_1, long *arg_2, long *arg_3);
bool function_bfc90(long arg_0);
void c_animation_channel_data_get(c_animation_channel const *arg_0, s_animation_data *arg_1);
bool function_10eea0(s_bit_owner *arg_0, short arg_1);
byte *function_10ee70(s_bit_owner *arg_0);
long bit_vector_count_bits(dword const *arg_0, long arg_1);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
void c_animation_channel_node_position_get(c_animation_channel const *arg_0, point3f *arg_1, real arg_2, short arg_3);
point3f *transform4x3f_apply_point(transform4x3f const *arg_0, point3f const *arg_1, point3f *arg_2);
transform4x3f *function_b8bd0(long arg_0, short arg_1);
void function_e4300(long arg_0);
void function_e5840(long arg_0, vector3f const *arg_1, bool arg_2);
long function_10f5f0(long arg_0);
bool function_a7670(long arg_0);
void __stdcall function_d6bc0(long arg_0);
void function_1060a0(long arg_0, long arg_1);
void function_ba350(long arg_0, real arg_1);
bool function_10dc70(long arg_0);
bool function_10eaf0(long arg_0);
long g_55e70c;

PRIVATE __forceinline bool function_111651(c_animation_channel const *arg_0)
{
    return arg_0->graph_tag_index != NONE && arg_0->animation_id.index != NONE;
}

// @retail 0x111650
bool __stdcall function_111650(long arg_0, long *arg_1)
{
    // Retail keeps the second argument on the stack (ret 8).
    long *const *local_38 = &arg_1;
    arg_1 = *local_38;
    s_unit_animation_object *local_0 = UNIT_ANIMATION_OBJECT(arg_0);
    s_unit_animation_control *local_1 = UNIT_ANIMATION_CONTROL(local_0);
    s_animation_state *local_2 = UNIT_ANIMATION_STATE(local_0);
    s_unit_animation_definition *local_3 = (s_unit_animation_definition *)TAG_BYTES(local_0->definition_index);
    long local_4 = 0x7000101;
    long local_5 = 0x7000101;
    bool local_6 = false;
    bool local_7 = false;
    bool local_8 = false;
    long local_9 = 0;
    bool local_10 = function_113910(arg_0);
    if (arg_1[0] != NONE && arg_1[0])
        local_4 = arg_1[0];
    if (arg_1[1] != NONE && arg_1[1])
    {
        local_5 = arg_1[1];
        local_8 = ((byte *)arg_1)[8] != 0;
    }
    if (local_0->parent_index == NONE)
    {
        if (local_2->unknown70 == 0x20001b3)
            local_4 = 0x20001b3;
        else if (TEST_FIELD_BIT(local_0->animation_frozen))
            local_4 = 0x6000086;
        else
        {
            byte *local_11 = (byte *)UNIT_ANIMATION_OBJECT(arg_0);
            if (*(short *)(local_11 + *(short *)(local_11 + 0x346) + 0x36))
                local_5 = 0x7000101;
            else
            {
                if (local_4 == 0x7000101)
                    local_4 = function_10eef0(arg_0, ((byte *)arg_1)[9] != 0, false);
                if (*((byte *)local_0 + 0x1f4) > 0)
                    local_4 = 0x7000039;
                if (*(long *)((byte *)local_1 + 0x2c) != NONE)
                    local_4 = *(long *)((byte *)local_1 + 0x2c);
            }
        }
    }
    if (function_111651(&local_2->channels[0]))
    {
        byte *local_12 = (byte *)UNIT_ANIMATION_OBJECT(arg_0);
        local_2->update(function_bf600, arg_0,
            (unsigned long)(long)*(short *)(local_12 + 0x110) >> 5,
            (s_blend_orientation *)(local_12 + *(short *)(local_12 + 0x10e)),
            (s_blend_orientation *)(local_12 + *(short *)(local_12 + 0x112)));
        long local_13 = local_2->unknown70;
        long local_14 = local_2->unknown7c;
        if (local_2->channels[0].unknown14)
            function_e6a20(arg_0, local_14, (local_2->channels[0].unknown14 & 1) != 0, (local_2->channels[0].unknown16 & 1) != 0);
        if (!(local_2->flags & 1) && (local_2->channels[0].flags & 1) &&
            !(local_2->channels[0].unknown11 & 9) && local_2->channels[0].function_1c6440()->frame_count > 1)
        {
            local_10 = true;
            if (local_2->channels[0].unknown11 & 0xa)
            {
                if ((local_2->channels[0].flags & 0x4000) && (local_2->channels[0].flags & 2))
                    local_6 = false;
                else if ((local_2->channels[0].flags & 0x4000) && !(local_1->flags & 0x100))
                {
                    local_2->flags |= 1;
                    local_6 = false;
                }
                else
                {
                    long local_15 = 0x7000101;
                    long local_16 = 0x7000101;
                    function_e6b00(arg_0, local_14, &local_15, &local_16);
                    if (local_15 != 0x7000101 || local_16 != 0x7000101)
                    {
                        local_4 = local_15;
                        local_5 = local_16;
                        local_7 = true;
                    }
                    switch (local_14)
                    {
                    case 0x800001c:
                    case 0xa00001d:
                        if (!*((byte *)local_0 + 0xaa) && !function_bfc90(arg_0))
                        {
                            long local_17 = *(short *)(TAG_BYTES(UNIT_ANIMATION_OBJECT(arg_0)->definition_index) + 0x254);
                            if (local_17 != NONE)
                            {
                                s_animation_data local_18;
                                c_animation_channel_data_get(&local_2->channels[0], &local_18);
                                vector3f local_19;
                                if (function_10eea0((s_bit_owner *)&local_18, (short)local_17))
                                {
                                    long local_20 = bit_vector_count_bits((dword *)function_10ee70((s_bit_owner *)&local_18), local_17 + 1) - 1;
                                    transform4x3f local_21;
                                    function_ba160(arg_0, &local_21);
                                    point3f local_22;
                                    c_animation_channel_node_position_get(&local_2->channels[0], &local_22, 0.0f, (short)local_20);
                                    transform4x3f_apply_point(&local_21, &local_22, &local_22);
                                    transform4x3f *local_23 = function_b8bd0(arg_0, (short)local_17);
                                    local_19.i = local_23->position.x - local_22.x;
                                    local_19.j = local_23->position.y - local_22.y;
                                    local_19.k = local_23->position.z - local_22.z;
                                }
                                else
                                    local_19 = *g_4687a4;
                                function_e5840(arg_0, &local_19, local_5 != local_14 || local_4 != local_13);
                                local_6 = local_5 != local_14;
                            }
                        }
                        break;
                    case 0x700005c:
                    case 0x700005d:
                        if (function_111651(&local_2->channels[2]) && (local_2->channels[2].flags & 1))
                            local_2->channels[2].unknown11 |= 1;
                        if (function_111651(&local_2->channels[0]) && (local_2->channels[0].flags & 1))
                            local_2->channels[0].unknown11 |= 1;
                        if (function_111651(&local_2->channels[1]) && (local_2->channels[1].flags & 1))
                            local_2->channels[1].unknown11 |= 1;
                        break;
                    case 0x900001f:
                    case 0x9000020:
                    case 0x90006b2:
                        if (!*((byte *)local_0 + 0xaa))
                        {
                            function_e4300(arg_0);
                            local_6 = true;
                        }
                        break;
                    case 0xe0000c2:
                        function_11b710(arg_0, local_4);
                        local_7 = true;
                        break;
                    case 0xd000042:
                        local_6 = false;
                        break;
                    case 0xc000043:
                        {
                            bool local_24 = false;
                            if ((bool)((local_3->flags >> 1) & 1))
                            {
                                if (function_10f5f0(arg_0) != 0xd000042 || (*((byte *)local_0 + 0xc1) & 1))
                                    local_24 = true;
                            }
                            if (!function_a7670(arg_0) && local_24)
                                function_d6bc0(arg_0);
                            else
                            {
                                local_1->flags |= 4;
                                local_2->flags |= 1;
                            }
                        }
                        break;
                    }
                    if (!function_111010(arg_0))
                    {
                        local_6 = true;
                        if (local_5 == 0x7000101)
                            local_5 = 0x400000c;
                    }
                    long local_25 = local_0->parent_index;
                    if (local_25 != NONE && function_0c7070(arg_0))
                    {
                        byte *local_26 = TAG_BYTES(UNIT_ANIMATION_OBJECT(local_25)->definition_index);
                        byte *local_27 = *(byte **)(local_26 + 0x1cc) + *(short *)((byte *)local_0 + 0x1fc) * 0xb0;
                        if ((bool)((*(dword *)local_27 >> 11) & 1) && *(short *)(local_27 + 0x3e) != NONE)
                        {
                            local_9 = 4;
                            local_4 = 0x7000101;
                            local_5 = 0x400000c;
                        }
                    }
                }
            }
        }
    }
    bool local_28 = function_111651(&local_2->channels[0]);
    bool local_29 = false;
    if (local_4 != local_2->unknown70 && local_4 != 0x7000101)
        local_29 = true;
    if (local_5 != local_2->unknown7c && local_5 != 0x7000101)
        local_29 = true;
    bool local_30 = local_6 || !local_28;
    if ((local_29 && (local_7 || function_1103a0(arg_0, local_5, local_8))) || local_30)
    {
        if (!function_10f430(arg_0, local_4, 0x7000101, 0x7000101, local_5, 0.0f, false, local_9 | 0x401))
        {
            if ((local_5 == 0x7000101 ? local_2->unknown7c == 0x400000c : local_5 == 0x400000c) && !(local_2->unknown6e & 1))
                local_2->channels_clear();
        }
        if (local_2->graph_tag_index != NONE && function_111651(&local_2->channels[0]) && !function_10ee20(local_2))
            local_10 = true;
    }
    else if (!((++g_55e70c ^ arg_0) & 0xf))
    {
        if (local_2->graph_tag_index != NONE && !(local_2->flags & 1))
            local_2->resources_request(local_2->unknown70, local_2->unknown74, local_2->unknown78, true, true);
        long *local_31 = (long *)((byte *)local_0 + 0x218);
        long local_32 = 4;
        do
        {
            if (*local_31 != NONE)
                function_1060a0(*local_31, arg_0);
            ++local_31;
        } while (--local_32);
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_33 = &local_1->channel_dc;
        if (function_111651(local_33))
        {
            if (local_2->graph_tag_index != NONE && !(local_2->flags & 1))
                local_33->update(local_2, function_bf600, arg_0);
            if ((local_33->unknown11 & 0xa) && TEST_FIELD_BIT(local_1->flag7))
            {
                local_33->clear();
                local_1->flag7 = false;
                function_114330(arg_0);
            }
            if (function_111651(local_33) && (local_33->flags & 1) && !(local_33->unknown11 & 9))
                local_10 = true;
        }
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_34 = &local_1->channel_9c;
        if (function_111651(local_34))
        {
            if (local_2->graph_tag_index != NONE && !(local_2->flags & 1))
                local_34->update(local_2, function_bf600, arg_0);
            long local_35 = local_34->unknown08;
            if (local_34->unknown14)
                function_e6a20(arg_0, local_35, (local_34->unknown14 & 1) != 0, (local_34->unknown16 & 1) != 0);
            if ((local_34->unknown11 & 0xe) && !((local_34->flags & 0x4000) && (local_34->flags & 2)))
            {
                if ((bool)((local_34->flags >> 14) & 1))
                    function_11b710(arg_0, local_2->unknown70);
                function_e6b00(arg_0, local_35, NULL, NULL);
                if (local_34->unknown08 == local_35 && !function_110fc0(arg_0))
                {
                    local_34->clear();
                    if (*((byte *)local_0 + 0xaa) != 1)
                        function_ba350(arg_0, 0.267f);
                }
            }
        }
        if (function_111651(local_34) && (local_34->flags & 1) && !(local_34->unknown11 & 9))
            local_10 = true;
    }
    if (!(local_2->flags & 1))
    {
        c_animation_channel *local_36 = &local_1->channel_bc;
        if (function_111651(local_36))
        {
            if (local_2->graph_tag_index != NONE && !(local_2->flags & 1))
                local_36->update(local_2, function_bf600, arg_0);
            long local_37 = local_36->unknown08;
            if (local_36->unknown14)
                function_e6a20(arg_0, local_37, (local_36->unknown14 & 1) != 0, (local_36->unknown16 & 1) != 0);
            if (local_36->unknown11 & 0xe)
            {
                function_e6b00(arg_0, local_37, NULL, NULL);
                if (local_36->unknown08 == local_37 && !function_110fc0(arg_0))
                {
                    local_36->clear();
                    if (*((byte *)local_0 + 0xaa) != 1)
                        function_ba350(arg_0, 0.1335f);
                }
            }
            if (function_111651(local_36) && (local_36->flags & 1) && !(local_36->unknown11 & 9))
                local_10 = true;
        }
    }
    local_10 |= function_10dc70(arg_0);
    local_10 |= function_10eaf0(arg_0);
    return local_10;
}

void function_e70b0(long unit_index, short *value);
bool function_e4050(long object_index);
const long g_4408f8[8] = { NONE, 0, 1, 2, 2, NONE, NONE, 3 };
const long g_440918[8] = { NONE, NONE, 0x600008c, 0xc000043, 0xc000043, 0xd000042, 0xc000043, 0xc000043 };
const long g_440938[8] = { NONE, NONE, NONE, 0x6000086, 0x6000086, 0x6000086, 0x6000086, 0x6000086 };
const long g_46fc30[4] = { 0x600008b, 0x600008c, 0x600008d, 0x600008e };

// @retail 0x10ff40
bool function_10ff40(long unit_index, long type, short side, short value,
    bool *flag, short *side_out, short *value_out)
{
    (void)&unit_index;
    (void)&side;
    (void)&value;
    (void)&flag;
    (void)&side_out;
    (void)&value_out;
    s_unit_animation_object *unit = UNIT_ANIMATION_OBJECT(unit_index);
    byte *definition = (byte *)TAG_BYTES(unit->definition_index);
    s_animation_state *state = UNIT_ANIMATION_STATE(unit);
    s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
    *flag = false;
    long set = g_440918[type];
    long mode = g_440938[type];
    long lookup = g_4408f8[type];
    long animation = lookup == NONE ? NONE : g_46fc30[lookup];
    bool result = false;
    bool force_overlay = false;
    c_animation_channel *overlay = 0;
    if (set != NONE)
    {
        bool test_base = type < 3;
        if (test_base && state->unknown7c == 0x600008c)
        {
            real elapsed = 0.0f;
            if (state->channels[0].graph_tag_index != NONE && state->channels[0].animation_id.index != NONE)
                elapsed = state->channels[0].frame_position * 0.03333333507180214f;
            test_base = !(elapsed > *(real *)(definition + 0x110));
        }
        if (test_base)
        {
            if (!function_1103a0(unit_index, set, true) || function_c8fc0(unit_index))
                force_overlay = true;
            else
            {
                short action;
                function_e70b0(unit_index, &action);
                if (action != 0 ||
                    (*((byte *)g_4e0300->data + (unit_index & 0xffff) * 12 + 3) == 0 && function_e4050(unit_index)))
                    force_overlay = true;
            }
        }
        bool available = true;
        if (animation != NONE)
        {
            byte channel_storage[sizeof(c_animation_channel)];
            c_animation_channel *channel = (c_animation_channel *)channel_storage;
            channel->copy_from(&state->channels[0]);
            available = state->channel_play_indexed(channel, animation, side, value, 0, 0);
        }
        if (!available || force_overlay)
        {
            if (animation == 0x600008c) animation = 0x600008b;
            else if (animation == 0x600008e) animation = 0x600008d;
            else return result;
            set = NONE;
        }
    }
    if (set == NONE)
    {
        overlay = &control->channel_dc;
        if ((bool)((control->flags >> 7) & 1) && overlay->graph_tag_index != NONE &&
            overlay->animation_id.index != NONE &&
            overlay->frame_position * 0.03333333507180214f <= *(real *)(definition + 0x108))
            return result;
    }
    if (set != NONE)
    {
        long selected_mode = 0x7000101;
        if ((unit->parent_index == NONE || *(short *)((byte *)unit + 0x1fc) == NONE) && mode != NONE)
            selected_mode = mode;
        result = function_10f430(unit_index, selected_mode, 0x7000101, 0x7000101, set, 0.0f, false,
            animation == NONE ? 2 : 14);
    }
    if (animation != NONE)
    {
        bool animated;
        if (overlay)
        {
            animated = state->channel_play_indexed(overlay, animation, side, value, side_out, value_out);
            if (animated) control->flags |= 0x80;
        }
        else
            animated = state->play_indexed(animation, side, value, side_out, value_out);
        if (animated)
        {
            *flag = true;
            return true;
        }
    }
    return result;
}
