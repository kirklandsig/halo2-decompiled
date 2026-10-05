// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10EE20.CPP: queries of a unit's animation state (the state at the
   unit's offset +0x12a, unknown_1cafc0.cpp's s_animation_state). The rest of
   this file's functions are in unknown_10db60.cpp and unknown_10dc70.cpp. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"

/* the unit (a view of the object data) */
struct s_unit_animation_view
{
	long definition_index;
	byte unknown004[0x12a - 4];
	short animation_state_offset;
};

struct s_unit_animation_header
{
	byte unknown00[8];
	s_unit_animation_view *unit;
};

#define GRAPH_GET(index) ((s_graph_tag *)g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x10f340
bool function_10f340(long unit_index, long mode, long set)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, 0x7000101, 0x7000101, set, 0, &animation_id);
	return animation_id.index != NONE;
}

// @retail 0x10f3b0
bool function_10f3b0(long unit_index, long mode, long set)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);

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
	c_type_709360 animation_id = GRAPH_GET(state->graph_tag_index)->overlay_get(mode, state->unknown74, state->unknown78,
		set, NULL, NULL, NULL);
	return animation_id.index != NONE;
}

// @retail 0x10fcd0
bool function_10fcd0(long unit_index, long mode, long weapon_class, long weapon_type)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, weapon_class, weapon_type, 0x400000c, 2, &animation_id);
	return animation_id.index != NONE;
}

/* a tag's definition: the unit's (its model at +0x38) and the model's (its
   render model at +4) */
struct s_unit_animation_definition
{
	byte unknown00[0x38];
	long model_tag_index;
};

struct s_unit_animation_model_definition
{
	byte unknown00[4];
	long render_model_tag_index;
};

#define TAG_GET(index) (g_4e3b44[(index) & 0xffff].bytes)

// @retail 0x10f9b0
bool function_10f9b0(long unit_index, long mode, long set, long lookup_flags, transform4x3f *matrix, bool any_weapon)
{
	s_unit_animation_view *unit = ((s_unit_animation_header *)g_4e0300->data)[unit_index & 0xffff].unit;
	s_animation_state *state = (s_animation_state *)((byte *)unit + unit->animation_state_offset);
	bool result = false;
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	state->animation_lookup(&names, &found, mode, any_weapon ? 0x7000001 : 0x7000101, 0x7000001, set, lookup_flags,
		&animation_id);
	if (animation_id.index != NONE)
	{
		s_unit_animation_definition *definition = (s_unit_animation_definition *)TAG_GET(unit->definition_index);
		s_unit_animation_model_definition *model = (s_unit_animation_model_definition *)TAG_GET(definition->model_tag_index);

		state->animation_matrix_get(animation_id, 0.0f, (long)TAG_GET(model->render_model_tag_index), matrix);
		result = true;
	}
	return result;
}

bool function_0e6800(c_animation_channel const *channel);

PRIVATE inline bool channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

/* whether a channel has stopped playing (0xe6800, inlined) */
PRIVATE inline bool channel_stopped(c_animation_channel const *channel)
{
	bool playing = (channel->flags & 1) && !(channel->unknown11 & 9);

	return !playing;
}

// @retail 0x10ee20
bool function_10ee20(s_animation_state *state)
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
