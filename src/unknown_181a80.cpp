// @flags /O2 /Gr
/* UNKNOWN_181A80.CPP: which sections of an object's render model are drawn
   with the object's current region permutations */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

struct s_model_object
{
	long definition_index;
	byte unknown04[0xb4 - 4];
	long havok_component_index;
	byte unknownb8[0x118 - 0xb8];
	short region_permutations_size;
	short region_permutations_offset;
};

struct s_model_object_header
{
	byte unknown00[8];
	s_model_object *object;
};

struct s_model_object_definition
{
	byte unknown00[0x38];
	long model_index;
};

struct s_model_tag
{
	byte unknown00[0x24];
	long render_model_index;
};

struct s_model_permutation
{
	byte unknown00[6];
	char name;
	byte unknown07;
};

struct s_model_region
{
	byte unknown00[5];
	char name;
	byte unknown06[6];
	s_model_permutation *permutations;
};

struct s_model_regions
{
	byte unknown00[0x70];
	long region_count;
	s_model_region *regions;
};

struct s_render_model_section
{
	short list_index;
	short region;
	short permutation;
	byte unknown06[0x90 - 6];
};

struct s_render_model_view
{
	byte unknown00[0x38];
	long section_count;
	s_render_model_section *sections;
	byte unknown40[0xc8 - 0x40];
	long list_count;
};

/* the sections of each list (0x44 bytes) */
struct s_section_list
{
	byte sections[0x40];
	long count;
};

struct s_section_lists
{
	long count;
	s_section_list lists[256];
	s_section_list unlisted;
};

static inline s_model_object *model_object_get(long object_index)
{
	return ((s_model_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

// @retail 0x181cf0
void function_181cf0(long model_index, long object_index, long *region_index, long region_name, long permutation_name, long *permutation_index)
{
	s_model_regions *model = (s_model_regions *)g_4e3b44[model_index & 0xffff].bytes;
	s_model_object *object = model_object_get(object_index);
	long region = NONE;
	byte *permutations = (byte *)object + object->region_permutations_offset;
	long permutation_count = object->region_permutations_size / 10;

	*region_index = region;
	*permutation_index = NONE;
	for (region = 0; region < model->region_count; region++)
	{
		s_model_region *region_entry = &model->regions[region];

		if (region_entry->name == region_name)
		{
			long valid_index;

			*region_index = region;
			if (region < 0)
			{
				valid_index = 0;
			}
			else
			{
				valid_index = permutation_count - 1;
				if (region <= valid_index)
				{
					valid_index = region;
				}
			}
			if (valid_index == region && permutations[region] != 0xff)
			{
				long permutation = (char)permutations[region];
				if (region_entry->permutations[permutation].name == permutation_name)
				{
					*permutation_index = permutation;
				}
			}
			return;
		}
	}
}

// @retail 0x181a80
s_section_lists *function_181a80(s_section_lists *lists, long object_index, bool all_sections)
{
	s_model_object *object = model_object_get(object_index);
	long model_index = ((s_model_object_definition *)g_4e3b44[object->definition_index & 0xffff].bytes)->model_index;

	lists->count = 0;
	if (model_index != NONE)
	{
		long render_model_index = ((s_model_tag *)g_4e3b44[model_index & 0xffff].bytes)->render_model_index;
		if (render_model_index != NONE)
		{
			s_render_model_view *render_model = (s_render_model_view *)g_4e3b44[render_model_index & 0xffff].bytes;
			long i;

			lists->count = render_model->list_count;
			for (i = 0; i < lists->count; i++)
			{
				lists->lists[i].count = 0;
			}
			lists->unlisted.count = 0;

			for (i = 0; i < render_model->section_count; i++)
			{
				s_render_model_section *section = &render_model->sections[i];
				long list_index = section->list_index;
				s_section_list *list = list_index == NONE ? &lists->unlisted : &lists->lists[list_index];
				long region_index;
				long permutation_index;

				if (!all_sections)
				{
					function_181cf0(model_index, object_index, &region_index, section->region, section->permutation, &permutation_index);
					if (region_index == NONE || permutation_index == NONE)
					{
						continue;
					}
				}
				list->sections[list->count++] = (byte)i;
			}
		}
	}
	return lists;
}

/* a rigid body of a havok component (0x60 bytes) and the sections it moves */
struct s_model_rigid_body
{
	byte unknown00[0x48];
	char *sections;
	long section_count;
	byte unknown50[0x10];
};

struct s_model_havok_component
{
	byte unknown00[0x70];
	s_model_rigid_body *rigid_bodies;
	long rigid_body_count;
	byte unknown78[0xa0 - 0x78];
};

/* the rigid body that moves each list of sections */
struct s_list_rigid_bodies
{
	long count;
	char lists[256];
	char unlisted;
};

// @retail 0x181bd0
s_list_rigid_bodies *function_181bd0(long object_index, s_list_rigid_bodies *rigid_bodies)
{
	s_model_object *object = model_object_get(object_index);
	long model_index = ((s_model_object_definition *)g_4e3b44[object->definition_index & 0xffff].bytes)->model_index;

	rigid_bodies->count = 0;
	rigid_bodies->unlisted = NONE;
	if (model_index != NONE)
	{
		long render_model_index = ((s_model_tag *)g_4e3b44[model_index & 0xffff].bytes)->render_model_index;
		if (render_model_index != NONE)
		{
			s_render_model_view *render_model = (s_render_model_view *)g_4e3b44[render_model_index & 0xffff].bytes;
			s_model_havok_component *component = &((s_model_havok_component *)g_51e9b8->data)[object->havok_component_index & 0xffff];
			long i;

			rigid_bodies->count = render_model->list_count;
			memset(rigid_bodies->lists, NONE, sizeof(rigid_bodies->lists));
			for (i = 0; i < component->rigid_body_count; i++)
			{
				long j;

				for (j = 0; j < component->rigid_bodies[i].section_count; j++)
				{
					long section_index = component->rigid_bodies[i].sections[j];
					if (section_index != NONE)
					{
						long list_index = render_model->sections[section_index].list_index;
						if (list_index != NONE)
						{
							rigid_bodies->lists[list_index] = (char)i;
						}
						else
						{
							rigid_bodies->unlisted = (char)i;
						}
					}
				}
			}
		}
	}
	return rigid_bodies;
}