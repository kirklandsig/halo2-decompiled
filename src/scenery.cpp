// @flags /O2 /arch:SSE /Gr
/* SCENERY.CPP: the scenery object type (its definition is at 0x467ff0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "unknown_1dacb0.h"
#include "unknown_1c62f0.h"

/* the scenery definition (the tag data) */
struct s_scenery_definition
{
	byte unknown000[0x38];
	long model_index;
	byte unknown03c[0xbc - 0x3c];
	short pathfinding_policy;
	byte unknown0be[2];
	short value_c0;
};

struct s_model_definition_view
{
	byte unknown00[4];
	long collision_model_index;
};

struct s_collision_model_view
{
	byte unknown00[8];
	long value_08;
};

/* where a scenery is placed */
struct s_scenery_location
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
};

struct s_scenery_placement
{
	byte unknown00[0x4c];
	short pathfinding_policy;
};

/* the scenery (the object data) */
struct s_scenery
{
	long definition_index;
	byte unknown004[0xa4 - 4];
	s_scenery_location location;
	byte unknown0ac[0x116 - 0xac];
	short node_matrices_offset;
	byte unknown118[0x12a - 0x118];
	short animation_state_offset;
	dword flags;
	byte unknown130[0x134 - 0x130];
	long value_134;
	long attached_object_index;
};

struct s_scenery_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_scenery *scenery;
};

/* the animation state of an object (a view of unknown_1cafc0.cpp's) */
struct s_animation_view
{
	byte unknown00[0x14];
	short frame_count;
};

struct s_scenery_animation_state
{
	c_animation_channel channels[3];
	byte unknown60[8];
	long graph_tag_index;
};

static inline long real_to_long(real value)
{
	long result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

/* g_4e0344: the structure bsp's entries (the first of the array at +0x84
   has a count at +0x58 and 12 byte entries at +0x5c) */
struct s_bsp_location_entry
{
	long value_00;
	short value_04;
	byte value_06;
	byte value_07;
	long value_08;
};

struct s_bsp_locations
{
	byte unknown00[0x58];
	long count;
	s_bsp_location_entry *entries;
};

s_structure_bsp_globals *g_4e0344;

#define SCENERY_GET(index) (((s_scenery_header *)g_4e0300->data)[(index) & 0xffff].scenery)
#define TAG_DATA(type, index) ((type *)g_4e3b44[(index) & 0xffff].bytes)

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);

/* 0x10a8b0 is in unknown_10a8b0.cpp: retail calls it out of line, which
   needs an /Ob1 file */
long function_10a8b0(s_scenery_location *location, long value);

// @retail 0x10a820
void function_10a820(long scenery_index)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);
	long model_index = TAG_DATA(s_scenery_definition, scenery->definition_index)->model_index;
	long result = NONE;

	if (model_index != NONE && TAG_DATA(s_scenery_definition, scenery->definition_index)->value_c0 != 2)
	{
		long collision_model_index = TAG_DATA(s_model_definition_view, model_index)->collision_model_index;
		if (collision_model_index != NONE)
			result = function_10a8b0(&scenery->location, TAG_DATA(s_collision_model_view, collision_model_index)->value_08);
	}
	scenery->value_134 = result;
}

// @retail 0x10a030
void __stdcall function_10a030(long scenery_index)
{
	function_10a820(scenery_index);
}

// @retail 0x10a040
short function_10a040(long definition_index, s_scenery_placement *placement)
{
	short result;

	if (definition_index == NONE)
	{
		result = 3;
	}
	else
	{
		switch (placement->pathfinding_policy)
		{
		case 0:
			result = TAG_DATA(s_scenery_definition, definition_index)->pathfinding_policy;
			break;
		case 1:
			result = 2;
			break;
		case 2:
			result = 0;
			break;
		case 3:
			result = 1;
			break;
		default:
			result = 3;
			break;
		}
	}
	return result;
}

// @retail 0x10a0b0
short function_10a0b0(long scenery_index)
{
	return TAG_DATA(s_scenery_definition, SCENERY_GET(scenery_index)->definition_index)->value_c0;
}

// @retail 0x10a390
void __stdcall function_10a390(long scenery_index, transform4x3f *matrix)
{
	long attached_object_index = SCENERY_GET(scenery_index)->attached_object_index;

	if (attached_object_index != NONE && function_badc0(attached_object_index, NONE))
	{
		s_scenery *object = SCENERY_GET(attached_object_index);
		function_142a60((transform4x3f *)((byte *)object + object->node_matrices_offset), matrix, matrix);
	}
}

// @retail 0x10a460
long function_10a460(long object_index)
{
	long result = 0;

	if (object_index != NONE)
	{
		if (((s_scenery_header *)g_4e0300->data)[object_index & 0xffff].type == 6)
		{
			s_scenery *scenery = SCENERY_GET(object_index);

			if (scenery->animation_state_offset != NONE)
			{
				s_scenery_animation_state *state = (s_scenery_animation_state *)((byte *)scenery + scenery->animation_state_offset);
				c_animation_channel *channel = &state->channels[0];

				if (state->graph_tag_index != NONE && channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
				{
					s_animation_view *animation = (s_animation_view *)channel->function_1c6440();
					real time = 0.0f;
					real remaining;
					long frames;

					if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
						time = channel->frame_position * (1.0f / 30.0f);
					remaining = ((real)animation->frame_count * (1.0f / 30.0f) - time) * 30.0f;
					__asm
					{
						fld remaining
						fistp frames
					}
					result = (short)frames - 2;
					result = result > 0 ? result : 0;
				}
			}
		}
	}
	return result;
}

/* the scenery object type definition */
struct s_scenery_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[4];
	void (__stdcall *handler20)(long);
	void *unknown24[0x70 / 4 - 9];
	void (__stdcall *handler70)(long, transform4x3f *);
};

s_scenery_type_definition g_467ff0 =
{
	"scenery",
	'scen',
	0x13c,
	0x50,
	0x58,
	0x5c,
	{ 0, 0, 0, 0 },
	function_10a030,
	{ 0 },
	function_10a390
};

bool function_10a660(long animation_graph_index, long object_index, long animation_name, short frame,
	long attached_object_index, bool interpolate, bool loop);

/* starts a scenery's default looping animation; flag 0 says it plays */
// @retail 0x10a3f0
void function_10a3f0(long scenery_index)
{
	s_scenery *scenery = SCENERY_GET(scenery_index);

	scenery->flags &= ~1;
	if (SCENERY_GET(scenery_index)->animation_state_offset != NONE)
	{
		long graph_tag_index = ((s_scenery_animation_state *)((byte *)scenery + scenery->animation_state_offset))->graph_tag_index;

		if (graph_tag_index != NONE &&
			function_10a660(graph_tag_index, scenery_index, 0x400000c, 0, NONE, false, true))
		{
			scenery->flags |= 1;
		}
	}
}