// @flags /O2 /arch:SSE /Gr
/* UNIT_ANIMATION_ACCESS.CPP: queries and resets of a unit's animation. The base
   channel's time and events and the lookups of the unit's graph act on the
   animation state (unknown_1cafc0.cpp); the layer resets, the stance mode and
   the channel checks act on the animation control (unit_animation_access.h). The
   rest of the control's code is in unknown_10dc70.cpp and unknown_10f1e0.cpp. */

#include "unknown_11c920.h"
#include <string.h>
#include "globals.h"
#include "unit_animation_access.h"
#include "unknown_1cafc0.h"

real function_1ccb40(c_animation_channel const *channel);

/* whether a channel has an animation of a graph */
PRIVATE __forceinline bool channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

/* whether the periodic layer has its animation and is fading it in or about
   to play it */
// @retail 0x10db60
bool unit_animation_periodic_layer_active(long unit_index)
{
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(ANIMATED_UNIT_GET(unit_index));
	bool fading = control->periodic_layer.weight >= 0.0001f;
	bool due = control->countdown > 0 && control->countdown < 7;

	return control->periodic_layer.periodic_animation.index != NONE && (fading || due);
}

/* the base channel's time in seconds, and its animation's length */
// @retail 0x10f690
real function_10f690(long unit_index, real *duration)
{
	c_animation_channel *channel = &UNIT_ANIMATION_STATE(ANIMATED_UNIT_GET(unit_index))->channels[0];

	if (duration)
	{
		real length = 0.0f;
		if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		{
			s_graph_tag *graph = ANIMATION_GRAPH_OF(channel->graph_tag_index);
			length = (real)function_1daea0(graph, channel->animation_id)->frame_count * (1.0f / 30.0f);
		}
		*duration = length;
	}

	real time = 0.0f;
	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		time = channel->frame_position * (1.0f / 30.0f);
	return time;
}

/* the time of the base channel's event */
// @retail 0x10f7f0
real function_10f7f0(long unit_index)
{
	return UNIT_ANIMATION_STATE(ANIMATED_UNIT_GET(unit_index))->channels[0].get_event_time();
}

/* whether the control's third channel is playing while flag 7 is set */
// @retail 0x10f890
bool unit_animation_control_channel_playing(long unit_index)
{
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(ANIMATED_UNIT_GET(unit_index));
	bool result = false;

	if (control->channels[2].graph_tag_index != NONE && control->channels[2].animation_id.index != NONE &&
		(control->channels[2].flags & 1) && !(control->channels[2].unknown11 & 9) && TEST_FIELD_BIT(control->flag7))
		result = true;
	return result;
}

/* the name the control's first channel was started with */
__declspec(noinline) long function_10f8f0(long unit_index);

// @retail 0x10f8f0
long function_10f8f0(long unit_index)
{
	c_animation_channel *channel = &UNIT_ANIMATION_CONTROL(ANIMATED_UNIT_GET(unit_index))->channels[0];
	long name = NONE;

	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
		name = channel->unknown08;
	return name;
}

/* moves the base channel to a time, counted from its start or its end */
// @retail 0x10f930
bool function_10f930(long unit_index, real time, bool from_end)
{
	s_animation_state *state = UNIT_ANIMATION_STATE(ANIMATED_UNIT_GET(unit_index));
	c_animation_channel *channel = &state->channels[0];
	bool result = false;

	if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		real seconds = from_end ? function_1ccb40(channel) - time : time;

		if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
			channel->set_frame_position(seconds * 30.0f);
		result = true;
	}
	return result;
}

/* 0 without a base animation or event, 2 while the base animation's first (or
   last) event is still ahead, 1 once it has passed */
// @retail 0x10f720
long function_10f720(long unit_index, bool first)
{
	s_animation_state *state = UNIT_ANIMATION_STATE(ANIMATED_UNIT_GET(unit_index));
	c_animation_channel *channel = &state->channels[0];
	long result = 0;

	if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		s_graph_tag *graph = ANIMATION_GRAPH_OF(channel->graph_tag_index);
		s_animation *animation = function_1daea0(graph, channel->animation_id);
		long event_frame = first ? function_1dae20(animation) : function_1dae50(animation);

		if (event_frame == NONE)
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
		result = (event_frame > current_frame) + 1;
	}
	return result;
}

static __forceinline void named_animation_copy(c_type_709360 *out, s_graph_tag *graph, long name)
{
	c_type_709360 value = graph->animation_by_name(name);
	memcpy(out, &value, sizeof(value));
}

/* clears the periodic layer and looks up its two animations by name */
// @retail 0x10dbc0
void unit_animation_periodic_layer_reset(long unit_index)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);

	memset(&control->periodic_layer, 0, sizeof(control->periodic_layer));
	named_animation_copy(&control->periodic_layer.periodic_animation, ANIMATION_GRAPH_OF(state->graph_tag_index), 0x8000145);
	named_animation_copy(&control->periodic_layer.triggered_animation, ANIMATION_GRAPH_OF(state->graph_tag_index), 0xc00024d);
	control->periodic_layer.weight = 0.0f;
	control->periodic_layer.trigger = NONE;
}

/* clears the channel blend and looks up its animation by name */
// @retail 0x10e920
void unit_animation_channel_blend_reset(long unit_index)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);

	memset(&control->channel_blend, 0, sizeof(control->channel_blend));
	control->channel_blend.channel = NONE;
	c_type_709360 animation = ANIMATION_GRAPH_OF(state->graph_tag_index)->animation_by_name(0x800004d);
	control->channel_blend.animation = animation;
}

bool function_10f630(long object_index, long *first, long *second);

/* the mode for the unit's stance name (0x7000101 when it has none). Unless
   told not to, a mode that differs from the one playing has its resources
   requested and the playing one is kept, every fifth call */
// @retail 0x10eef0
long function_10eef0(long unit_index, bool alternate, bool no_request)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_unit_animation_control *control = UNIT_ANIMATION_CONTROL(unit);
	long mode = 0x7000101;
	long stance_name = unit->stance_name;

	switch (stance_name)
	{
	case 0x50000cb:
		mode = stance_name;
		break;
	case 0x4000089:
		mode = 0x4000089;
		break;
	case 0x6000084:
		mode = 0x6000084;
		break;
	case 0x6000085:
		mode = alternate ? 0x6000087 : 0x6000085;
		break;
	case 0x6000086:
		mode = 0x6000086 + (alternate ? 1 : 0);
		break;
	case 0x700002c:
		mode = 0x700002c;
		break;
	case 0x7000039:
		mode = 0x7000039;
		break;
	case 0x70000c9:
		mode = 0x70000c9;
		break;
	}

	if (mode != 0x7000101 && mode != 0x50000cb && !no_request)
	{
		long current;
		long next;

		if (function_10f630(unit_index, &current, &next) && current != mode)
		{
			if (--control->stance_request_delay < 0)
			{
				control->stance_request_delay = 4;
			}
			else
			{
				UNIT_ANIMATION_STATE(unit)->resources_request(mode, 0x7000101, 0x7000101, true, false);
				mode = current;
			}
		}
	}
	return mode;
}

/* whether the unit's graph has an animation for a mode and set */
// @retail 0x10f340
bool function_10f340(long unit_index, long mode, long set)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, 0x7000101, 0x7000101, set, 0, &animation_id);
	if (animation_id.index != NONE)
		return true;
	return false;
}

/* retail keeps this out of line in its only caller (0xe7020) */
__declspec(noinline) byte function_10f3b0(long unit_index, long mode, long set);

/* whether the unit's graph has an overlay for a mode and set (0x7000101 is
   the playing mode) */
// @retail 0x10f3b0
byte function_10f3b0(long unit_index, long mode, long set)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);

	if (mode == 0x7000101)
	{
		mode = state->unknown70;
		if (mode == NONE)
			mode = 0x6000086;
	}
	else if (mode == 0x7000001)
	{
		mode = 0x6000086;
	}
	c_type_709360 animation_id = ANIMATION_GRAPH_OF(state->graph_tag_index)->overlay_get(mode, state->unknown74, state->unknown78,
		set, NULL, NULL, NULL);
	return animation_id.index != NONE;
}

/* whether the unit's graph has an animation for a mode, weapon and set */
// @retail 0x10fcd0
byte function_10fcd0(long unit_index, long mode, long weapon_class, long weapon_type)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, weapon_class, weapon_type, 0x400000c, 2, &animation_id);
	return animation_id.index != NONE;
}

/* a tag's definition: the unit's (its model at +0x38) and the model's (its
   render model at +4) */
struct s_animated_unit_definition
{
	byte unknown00[0x38];
	long model_tag_index;
};

struct s_animated_unit_model_definition
{
	byte unknown00[4];
	long render_model_tag_index;
};

#define TAG_GET(index) (g_4e3b44[(index) & 0xffff].bytes)

/* the start matrix of the animation for a mode and set, if the graph has one */
// @retail 0x10f9b0
bool function_10f9b0(long unit_index, long mode, long set, long lookup_flags, transform4x3f *matrix, bool any_weapon)
{
	s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
	s_animation_state *state = UNIT_ANIMATION_STATE(unit);
	bool result = false;
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, any_weapon ? 0x7000001 : 0x7000101, 0x7000001, set, lookup_flags,
		&animation_id);
	if (animation_id.index != NONE)
	{
		s_animated_unit_definition *definition = (s_animated_unit_definition *)TAG_GET(unit->definition_index);
		s_animated_unit_model_definition *model = (s_animated_unit_model_definition *)TAG_GET(definition->model_tag_index);

		state->animation_matrix_get(animation_id, 0.0f, (long)TAG_GET(model->render_model_tag_index), matrix);
		result = true;
	}
	return result;
}

bool function_0e6800(c_animation_channel const *channel);

/* whether a channel has stopped playing (0xe6800, inlined) */
PRIVATE inline long channel_stopped(c_animation_channel const *channel)
{
	long playing = (channel->flags & 1) && !(channel->unknown11 & 9);

	return !(byte)playing;
}

/* whether an animation state is done: flagged so, or its third channel (else
   its base channel) has stopped */
// @retail 0x10ee20
bool unit_animation_state_finished(s_animation_state *state)
{
	bool result = (state->flags & 1) != 0;

	if (!result)
	{
		if (channel_valid(&state->channels[2]))
			result = channel_stopped(&state->channels[2]);
		else if (channel_valid(&state->channels[0]))
			result = function_0e6800(&state->channels[0]);
	}
	return result;
}

void function_1dacb0(s_graph_tag *graph, c_type_709360 animation_id, real *event_distance, real *distance);

bool unit_animation_get_timing(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance);

/* the ticks to the event and to the end, and the distances, of the animation
   for a mode and set (or of the overlay for the set) */
// @retail 0x10fa80
bool unit_animation_get_timing(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance)
{
	bool result = false;
	if (unit_index == NONE || ANIMATED_UNIT_GET(unit_index)->value_b3 <= 0)
	{
		s_animated_unit *unit = ANIMATED_UNIT_GET(unit_index);
		s_animation_state *state = UNIT_ANIMATION_STATE(unit);
		c_type_709360 found_id;
		s_animation_names names;
		s_animation_names found;
		s_animation *animation;
		state->animation_lookup(&names, &found, mode, 0x7000101, 0x7000101, set, 1, &found_id);
		c_type_709360 animation_id = found_id;
		if (animation_id.index != NONE)
			animation = function_1daea0(ANIMATION_GRAPH_OF(state->graph_tag_index), animation_id);
		else
		{
			byte channel_storage[sizeof(c_animation_channel)];
			c_animation_channel &channel = *(c_animation_channel *)channel_storage;
			channel.copy_from(&UNIT_ANIMATION_CONTROL(unit)->channels[0]);
			animation = NULL;
			if (state->overlay_play_for_class(&channel, 0x3f, 0x7000101, 0x7000101, set) && channel_valid(&channel))
			{
				animation_id = channel.animation_id;
				animation = channel.function_1c6440();
			}
		}
		if (animation)
		{
			long event_frame = function_1dae20(animation);
			long frames = animation->frame_count;
			function_1dacb0(ANIMATION_GRAPH_OF(state->graph_tag_index), animation_id, distance, event_distance);
			if (event_frame != NONE)
			{
				real count = (real)(event_frame * (1.0f / 30.0f)) * g_510c54->field_2_3;
				long ticks;
				__asm
				{
					fld count
					fistp ticks
				}
				*event_ticks = (short)ticks;
			}
			else
				*event_ticks = NONE;
			real count = (real)(frames * (1.0f / 30.0f)) * g_510c54->field_2_3;
			long ticks;
			__asm
			{
				fld count
				fistp ticks
			}
			*duration_ticks = (short)ticks;
			result = true;
		}
	}
	return result;
}
