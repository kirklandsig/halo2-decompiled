// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10DB60.CPP: queries of an object's animation state (the state at
   the object's offset +0x12a) and of the state at its offset +0x33e
   (unknown_10dc70.cpp's vibration state) */

#include "unknown_11c920.h"
#include <string.h>
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_1c62f0.h"

/* the animation state of an object (a view of unknown_1cafc0.cpp's) */
struct s_object_animation_state
{
	c_animation_channel channels[3];
	byte unknown60[8];
	long graph_tag_index;
};

/* the state at +0x33e */
struct s_object_state_33e
{
	word : 7;
	word flag7 : 1;
	word : 8;
	byte countdown;
	char request_delay;
	byte unknown04[0x30 - 4];
	struct
	{
		c_type_709360 animation_30;
		c_type_709360 animation_34;
		long value_38;
		byte unknown3c[4];
		real value_40;
		byte unknown44[0x5c - 0x44];
	} transition;
	struct
	{
		c_type_709360 animation_5c;
		byte unknown60[0x6c - 0x60];
		short value_6c;
		byte unknown6e[0x7c - 0x6e];
	} block_5c;
	byte unknown7c[0x9c - 0x7c];
	c_animation_channel channel_9c;
	byte unknownbc[0xdc - 0xbc];
	long value_dc;
	byte unknowne0[2];
	short value_e2;
	byte unknowne4[0xed - 0xe4];
	byte flags_ed;
	byte flags_ee;
};

struct s_object_10db60
{
	long definition_index;
	byte unknown004[0x12a - 4];
	short animation_state_offset;
	byte unknown12c[0x14c - 0x12c];
	long stance_name;
	byte unknown150[0x33e - 0x150];
	short state_offset;
};

struct s_object_header_10db60
{
	byte unknown00[8];
	s_object_10db60 *object;
};

struct s_animation_view
{
	byte unknown00[0x14];
	short frame_count;
};

#define OBJECT_GET_10db60(index) (((s_object_header_10db60 *)g_4e0300->data)[(index) & 0xffff].object)
#define OBJECT_ANIMATION_STATE(object) ((s_object_animation_state *)((byte *)(object) + (object)->animation_state_offset))
#define OBJECT_STATE_33E(object) ((s_object_state_33e *)((byte *)(object) + (object)->state_offset))

s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id);
c_type_709360 function_1dd0b0(s_graph_tag *graph, long name);
real function_1ccb40(c_animation_channel const *channel);

// @retail 0x10db60
bool function_10db60(long object_index)
{
	s_object_state_33e *state = OBJECT_STATE_33E(OBJECT_GET_10db60(object_index));
	bool a = state->transition.value_40 >= 0.0001f;
	bool b = state->countdown > 0 && state->countdown < 7;

	return state->transition.animation_34.index != NONE && (a || b);
}

// @retail 0x10f690
real function_10f690(long object_index, real *duration)
{
	c_animation_channel *channel = &OBJECT_ANIMATION_STATE(OBJECT_GET_10db60(object_index))->channels[0];

	if (duration)
	{
		real value = 0.0f;
		if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		{
			s_graph_tag *graph = (s_graph_tag *)g_4e3b44[channel->graph_tag_index & 0xffff].bytes;
			value = (real)((s_animation_view *)function_1daea0(graph, channel->animation_id))->frame_count * (1.0f / 30.0f);
		}
		*duration = value;
	}

	real result = 0.0f;
	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		result = channel->frame_position * (1.0f / 30.0f);
	return result;
}

// @retail 0x10f7f0
real function_10f7f0(long object_index)
{
	return OBJECT_ANIMATION_STATE(OBJECT_GET_10db60(object_index))->channels[0].get_event_time();
}

// @retail 0x10f890
bool function_10f890(long object_index)
{
	s_object_state_33e *state = OBJECT_STATE_33E(OBJECT_GET_10db60(object_index));
	bool result = false;

	if (state->value_dc != NONE && state->value_e2 != NONE && (state->flags_ee & 1) && !(state->flags_ed & 9) && TEST_FIELD_BIT(state->flag7))
		result = true;
	return result;
}

// @retail 0x10f8f0
long function_10f8f0(long object_index)
{
	c_animation_channel *channel = &OBJECT_STATE_33E(OBJECT_GET_10db60(object_index))->channel_9c;
	long result = NONE;

	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		result = channel->unknown08;
	return result;
}

// @retail 0x10f930
bool function_10f930(long object_index, real time, bool from_end)
{
	s_object_animation_state *state = OBJECT_ANIMATION_STATE(OBJECT_GET_10db60(object_index));
	c_animation_channel *channel = &state->channels[0];
	bool result = false;

	if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		real value = from_end ? function_1ccb40(channel) - time : time;

		if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
			channel->set_frame_position(value * 30.0f);
		result = true;
	}
	return result;
}

short function_1dae20(s_animation const *animation);
short function_1dae50(s_animation const *animation);

// @retail 0x10f720
long function_10f720(long object_index, bool first)
{
	s_object_animation_state *state = OBJECT_ANIMATION_STATE(OBJECT_GET_10db60(object_index));
	c_animation_channel *channel = &state->channels[0];
	long result = 0;

	if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		s_graph_tag *graph = (s_graph_tag *)g_4e3b44[channel->graph_tag_index & 0xffff].bytes;
		s_animation *animation = function_1daea0(graph, channel->animation_id);
		long frame = first ? function_1dae20(animation) : function_1dae50(animation);

		if (frame == NONE)
			return 0;

		real time = 0.0f;
		if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
			time = channel->frame_position * (1.0f / 30.0f);

		real frame_real = time * 30.0f;
		long current_frame;
		__asm
		{
			fld frame_real
			fistp current_frame
		}
		result = (frame > current_frame) + 1;
	}
	return result;
}
#define GRAPH_GET(index) ((s_graph_tag *)g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x10dbc0
void function_10dbc0(long object_index)
{
	s_object_10db60 *object = OBJECT_GET_10db60(object_index);
	s_object_animation_state *animation_state = OBJECT_ANIMATION_STATE(object);
	s_object_state_33e *state = OBJECT_STATE_33E(object);

	memset(&state->transition, 0, sizeof(state->transition));
	state->transition.animation_34 = function_1dd0b0(GRAPH_GET(animation_state->graph_tag_index), 0x8000145);
	state->transition.animation_30 = function_1dd0b0(GRAPH_GET(animation_state->graph_tag_index), 0xc00024d);
	state->transition.value_40 = 0.0f;
	state->transition.value_38 = NONE;
}

// @retail 0x10e920
void function_10e920(long object_index)
{
	s_object_10db60 *object = OBJECT_GET_10db60(object_index);
	s_object_animation_state *animation_state = OBJECT_ANIMATION_STATE(object);
	s_object_state_33e *state = OBJECT_STATE_33E(object);

	memset(&state->block_5c, 0, sizeof(state->block_5c));
	state->block_5c.value_6c = NONE;
	state->block_5c.animation_5c = function_1dd0b0(GRAPH_GET(animation_state->graph_tag_index), 0x800004d);
}

/* unknown_1cafc0.cpp's animation state */
struct s_animation_state
{
	void resources_request(long mode, long weapon_class, long weapon_type, bool urgent, bool other);
};

bool function_10f630(long object_index, long *first, long *second);

// @retail 0x10eef0
long function_10eef0(long object_index, bool alternate, bool no_request)
{
	s_object_10db60 *object = OBJECT_GET_10db60(object_index);
	s_object_state_33e *state = OBJECT_STATE_33E(object);
	long result = 0x7000101;

	switch (object->stance_name)
	{
	case 0x50000cb:
		result = object->stance_name;
		return result;
	case 0x4000089:
		result = 0x4000089;
		break;
	case 0x6000084:
		result = 0x6000084;
		break;
	case 0x6000085:
		result = alternate ? 0x6000087 : 0x6000085;
		break;
	case 0x6000086:
		result = 0x6000086 + (alternate ? 1 : 0);
		break;
	case 0x700002c:
		result = 0x700002c;
		break;
	case 0x7000039:
		result = 0x7000039;
		break;
	case 0x70000c9:
		result = 0x70000c9;
		break;
	}

	if (result != 0x7000101 && result != 0x50000cb && !no_request)
	{
		long current;
		long next;

		if (function_10f630(object_index, &current, &next) && current != result)
		{
			if (--state->request_delay < 0)
			{
				state->request_delay = 4;
			}
			else
			{
				((s_animation_state *)OBJECT_ANIMATION_STATE(object))->resources_request(result, 0x7000101, 0x7000101, true, false);
				result = current;
			}
		}
	}
	return result;
}
