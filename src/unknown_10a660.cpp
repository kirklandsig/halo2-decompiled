// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10A660.CPP: starts a scripted animation on an object's own
   animation state, optionally relative to another object */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_1dacb0.h"
#include "unknown_1cafc0.h"
#include "object_default_placement.h"

/* the object (a local view) */
struct s_object_10a660
{
	long tag_index;
	byte unknown004[0x12a - 4];
	short animation_state_offset;
	dword flags_12c;
	byte unknown130[0x138 - 0x130];
	long attached_object_index;
};

struct s_object_definition_10a660
{
	byte unknown00[0x38];
	long model_tag_index;
};

struct s_object_header_10a660
{
	byte unknown00[8];
	s_object_10a660 *object;
};

void function_1ccb80(c_animation_channel *channel, real seconds);

/* plays an animation (by graph and name) on an object from a frame */
// @retail 0x10a660
bool function_10a660(long animation_graph_index, long object_index, long animation_name, short frame,
	long attached_object_index, bool interpolate, bool loop)
{
	bool result = false;

	if (object_index != NONE && animation_graph_index != NONE && animation_name)
	{
		s_object_10a660 *object = ((s_object_header_10a660 *)g_4e0300->data)[object_index & 0xffff].object;
		if (object->animation_state_offset != NONE)
		{
			s_object_definition_10a660 *definition = (s_object_definition_10a660 *)g_4e3b44[object->tag_index & 0xffff].bytes;
			s_animation_state *state = (s_animation_state *)((byte *)object + object->animation_state_offset);
			if (state->graph_tag_index == animation_graph_index ||
				state->initialize(animation_graph_index, definition->model_tag_index, true))
			{
				c_type_709360 animation_id = state->animation_find(animation_name);
				if (animation_id.index != NONE)
				{
					word channel_flags = interpolate ? 0x4001 : 0x3f;
					if (loop)
						channel_flags |= 2;
					else
						channel_flags &= ~2;

					if (state->play(animation_id, channel_flags))
					{
						function_1ccb80(&state->channels[0], (real)frame * (1.0f / 30.0f));
						object->flags_12c &= ~1;
						function_b7290(object_index);
						object->attached_object_index = attached_object_index;
						if (attached_object_index != NONE)
							object_reset_default_placement(object_index, (s_object_default_placement_view *)object, false);
						function_b9b90(object_index, true);
						function_bd020(object_index);
						result = true;
					}
				}
			}
		}
	}

	return result;
}
