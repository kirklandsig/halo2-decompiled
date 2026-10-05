// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_025600.CPP: showing an object's name with a caption (an outside
   function lane A's script evaluator 0x2a2f00 needs) */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include "object_markers.h"

/* the object name caption (g_4b9970, defined in unknown_29f5b0.cpp):
   the object, its name, a string id and how long to show it */
struct s_object_name_caption
{
	long object_index;
	char name[0x20];
	long string_handle;
	real seconds;
	bool missing_marker;
	bool multiple_markers;
	byte unknown2e[2];
};

extern long g_4b9970[12];
extern byte g_5093fc;
extern transform4x3f *g_4687d0;
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
transform4x3f *function_b8c00(long object_index, long *node_count);

/* the scenario's object names (g_4e0350 +0x4c), 0x24 bytes each */
struct s_scenario_object_name
{
	char name[0x20];
	byte unknown20[4];
};

struct s_scenario_object_names_view
{
	byte unknown00[0x4c];
	s_scenario_object_name *object_names;
};

struct s_25600_object
{
	byte unknown00[0xac];
	short name_index;
};

struct s_25600_object_header
{
	byte unknown00[8];
	s_25600_object *object;
};

inline void object_name_caption_reset(void)
{
	memset(g_4b9970, 0, sizeof(g_4b9970));
	g_4b9970[0] = NONE;
}

// @retail 0x255e0
void function_255e0(void)
{
	object_name_caption_reset();
	g_5093fc = false;
}

// @retail 0x25600
void function_25600(long object_index, long string_handle, real seconds)
{
	s_scenario_object_names_view *scenario = (s_scenario_object_names_view *)g_4e0350;

	object_name_caption_reset();
	g_5093fc = false;

	if (scenario)
	{
		if (object_index != NONE)
		{
			s_object_name_caption *caption = (s_object_name_caption *)g_4b9970;
			s_25600_object *object = ((s_25600_object_header *)g_4e0300->data)[object_index & 0xffff].object;

			strncpy(caption->name, scenario->object_names[object->name_index].name, sizeof(caption->name));
			caption->name[sizeof(caption->name) - 1] = 0;
			caption->string_handle = string_handle;
			caption->object_index = object_index;
			real value = seconds;
			real limit = 0.0f;
			if (value != 0.0f)
			{
				limit = 10.0f;
				if (!(value < limit))
				{
					limit = 160.0f;
					if (value > limit)
					{
						caption->seconds = limit;
						return;
					}
					caption->seconds = value;
					return;
				}
			}
			caption->seconds = limit;
		}
		else
		{
			object_name_caption_reset();
		}
	}
}

// @retail 0x256f0
bool function_256f0(transform4x3f *out)
{
	transform4x3f matrix = *g_4687d0;
	bool result = false;
	s_object_name_caption *caption = (s_object_name_caption *)g_4b9970;
	if (g_4e0350 && caption->object_index != NONE)
	{
		long object_index = caption->object_index;
		s_25600_object *object = (s_25600_object *)function_badc0(object_index, (dword)NONE);
		if (object && object->name_index != NONE)
		{
			long marker_name = caption->string_handle;
			if (marker_name)
			{
				s_object_marker markers[2];
				long count = function_b8d30(object_index, marker_name, markers, 2, false);
				if (count > 0)
				{
					matrix = markers[0].matrix;
					result = true;
					if (count > 1 && !caption->multiple_markers)
						caption->multiple_markers = true;
					goto done;
				}
				marker_name = caption->string_handle;
			}
			long node_count = 0;
			transform4x3f *nodes = function_b8c00(object_index, &node_count);
			if (!caption->missing_marker && marker_name)
				caption->missing_marker = true;
			if (node_count > 0)
			{
				matrix = nodes[0];
				result = true;
			}
			else
				function_255e0();
		}
	}
done:
	if (out)
		*out = matrix;
	return result;
}
